#include "espino.h"

void ICACHE_FLASH_ATTR pinMode(uint8_t pin, uint8_t mode){
  if(pin < 16){
    if(mode == SPECIAL){
      GPC(pin) = (GPC(pin) & (0xF << GPCI)); //SOURCE(GPIO) | DRIVER(NORMAL) | INT_TYPE(UNCHANGED) | WAKEUP_ENABLE(DISABLED)
      GPEC = (1 << pin); //Disable
      GPF(pin) = GPFFS(GPFFS_BUS(pin));//Set mode to BUS (RX0, TX0, TX1, SPI, HSPI or CLK depending in the pin)
    } else if(mode & FUNCTION_0){
      GPC(pin) = (GPC(pin) & (0xF << GPCI)); //SOURCE(GPIO) | DRIVER(NORMAL) | INT_TYPE(UNCHANGED) | WAKEUP_ENABLE(DISABLED)
      GPEC = (1 << pin); //Disable
      GPF(pin) = GPFFS((mode >> 4) & 0x07);
    }  else if(mode == OUTPUT){
      GPF(pin) = GPFFS(GPFFS_GPIO(pin));//Set mode to GPIO
      GPC(pin) = (GPC(pin) & (0xF << GPCI)); //SOURCE(GPIO) | DRIVER(NORMAL) | INT_TYPE(UNCHANGED) | WAKEUP_ENABLE(DISABLED)
      GPES = (1 << pin); //Enable
    } else if(mode == INPUT || mode == INPUT_PULLUP){
      GPF(pin) = GPFFS(GPFFS_GPIO(pin));//Set mode to GPIO
      GPC(pin) = (GPC(pin) & (0xF << GPCI)) | (1 << GPCD); //SOURCE(GPIO) | DRIVER(OPEN_DRAIN) | INT_TYPE(UNCHANGED) | WAKEUP_ENABLE(DISABLED)
      GPEC = (1 << pin); //Disable
      if(mode == INPUT_PULLUP){
        GPF(pin) |= (1 << GPFPU);//Enable Pullup
      }
    }
  } else if(pin == 16){
    GPF16 = GP16FFS(GPFFS_GPIO(pin));//Set mode to GPIO
    GPC16 = 0;
    if(mode == INPUT){
      if(mode == INPUT_PULLDOWN){
        GPF16 |= (1 << GP16FPD);//Enable Pulldown
      }
      GP16E &= ~1;
    } else if(mode == OUTPUT){
      GP16E |= 1;
    }
  }
}

void ICACHE_FLASH_ATTR digitalWrite(uint8_t pin, uint8_t val){
  val &= 0x01;
  if(pin < 16){
    if(val) GPOS = (1 << pin);
    else GPOC = (1 << pin);
  } else if(pin == 16){
    if(val) GP16O |= 1;
    else GP16O &= ~1;
  }
}

int ICACHE_FLASH_ATTR digitalRead(uint8_t pin){
  if(pin < 16){
    return GPIP(pin);
  } else if(pin == 16){
    return GP16I & 0x01;
  }
}

uint8_t ICACHE_FLASH_ATTR shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
	uint8_t value = 0;
	uint8_t i;

	for (i = 0; i < 8; ++i) {
		digitalWrite(clockPin, HIGH);
		if (bitOrder == LSBFIRST)
			value |= digitalRead(dataPin) << i;
		else
			value |= digitalRead(dataPin) << (7 - i);
		digitalWrite(clockPin, LOW);
	}
	return value;
}

void ICACHE_FLASH_ATTR shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val){
	uint8_t i;
	for (i = 0; i < 8; i++)  {
		if (bitOrder == LSBFIRST)
			digitalWrite(dataPin, !!(val & (1 << i)));
		else	
			digitalWrite(dataPin, !!(val & (1 << (7 - i))));
			
		digitalWrite(clockPin, HIGH);
		digitalWrite(clockPin, LOW);		
	}
}

unsigned long ICACHE_FLASH_ATTR pulseIn(uint8_t pin, uint8_t state, unsigned long timeout){
  pinMode(pin, INPUT);
  uint32_t start = micros();
  while(digitalRead(pin) == state && (micros() - start) < timeout);
  while(digitalRead(pin) != state && (micros() - start) < timeout);
  start = micros();
  while(digitalRead(pin) == state && (micros() - start) < timeout);
  return micros() - start;
}

/*
  GPIO INTERRUPTS
*/


typedef struct {
  uint8_t mode;
  void (*fn)(void);
} isr_handler_t;

static isr_handler_t isr_handlers[16];
static uint32_t isr_reg = 0;

LOCAL void ICACHE_FLASH_ATTR isr_handler(void *para){
  uint32_t status = GPIE;
  GPIEC = status;//clear them interrupts
  if(status == 0 || isr_reg == 0) return;
  ETS_GPIO_INTR_DISABLE();
  int i = 0;
  uint32_t changedbits = status & isr_reg;
  while(changedbits){
    while(!(changedbits & (1 << i))) i++;
    changedbits &= ~(1 << i);
    isr_handler_t *handler = &isr_handlers[i];
    if(((handler->mode & 1) == digitalRead(i)) && handler->fn) handler->fn();
  }
  ETS_GPIO_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR isr_init(){
  ETS_GPIO_INTR_ATTACH(isr_handler, &isr_reg);
  ETS_GPIO_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR attachInterrupt(uint8_t pin, void (*userFunc)(void), int mode) {
  if(pin < 16) {
    isr_handler_t *handler = &isr_handlers[pin];
    handler->mode = mode;
    handler->fn = userFunc;
    isr_reg |= (1 << pin);
    GPC(pin) &= ~(0xF << GPCI);//INT mode disabled
    GPIEC = (1 << pin); //Clear Interrupt for this pin
    GPC(pin) |= ((mode & 0xF) << GPCI);//INT mode "mode"
  }
}

void ICACHE_FLASH_ATTR detachInterrupt(uint8_t pin) {
  if(pin < 16) {
    GPC(pin) &= ~(0xF << GPCI);//INT mode disabled
    GPIEC = (1 << pin); //Clear Interrupt for this pin
    isr_reg &= ~(1 << pin);
    isr_handler_t *handler = &isr_handlers[pin];
    handler->mode = 0;
    handler->fn = 0;
  }
}
