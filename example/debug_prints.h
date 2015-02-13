#include "espino.h"

void ICACHE_FLASH_ATTR printBin(uint32_t data, uint8_t len){
  while(len--) uart1_write(0x30 + ((data & (1 << len)) != 0));
}

void ICACHE_FLASH_ATTR printPinControl(uint32_t data){
  os_printf(", I: %u, WE: %u, D: %u, S: %u",
    ((data >> GPCI) & 0x07) & 0x01,
    ((data & (1 << GPCWE)) != 0) & 0x01,
    ((data & (1 << GPCD)) != 0) & 0x01,
    ((data & (1 << GPCS)) != 0) & 0x01);
}

void ICACHE_FLASH_ATTR printPinFunction(uint32_t data){
  os_printf(", FS: %u, PU: %u, SOE: %u, SS: %u, SPU: %u",
    (((data & (1 << GPFFS2)) != 0) << 2) | (((data & (1 << GPFFS1)) != 0) << 1) | (((data & (1 << GPFFS0)) != 0) << 0) & 0x07,
    ((data & (1 << GPFPU)) != 0) & 0x01,
    ((data & (1 << GPFSOE)) != 0) & 0x01,
    ((data & (1 << GPFSS)) != 0) & 0x01,
    ((data & (1 << GPFSPU)) != 0) & 0x01);
}

void ICACHE_FLASH_ATTR printPinInfo(uint8_t pin){
  if(pin < 16){
    os_printf("GPIO%u, EN: %u, OUT: %u, IN: %u, IE: %u", pin, GPEP(pin) & 0x01, GPOP(pin) & 0x01, GPIP(pin) & 0x01, GPIEP(pin) & 0x01);
    printPinControl(GPC(pin));
    printPinFunction(GPF(pin));
    os_printf("\n");
  }
}

void ICACHE_FLASH_ATTR printGPIORegs(){
  uint8_t i;
  for(i=0; i<6; i++) printPinInfo(i);
  for(i=12; i<16; i++) printPinInfo(i);
}


void ICACHE_FLASH_ATTR printTimer1(){
  os_printf("TIMER1: L: "); os_printf("%u", T1L);
  os_printf(", V: "); os_printf("%u", T1V);
  os_printf(", C: "); printBin(T1C, 9);
  os_printf(", I: "); printBin(T1I, 1);
  os_printf("\n");
}

void ICACHE_FLASH_ATTR printTimer2(){
  os_printf("TIMER2: L: "); os_printf("%u", T2L);
  os_printf(", V: "); os_printf("%u", T2V);
  os_printf(", C: "); printBin(T2C, 9);
  os_printf(", I: "); printBin(T2I, 1);
  os_printf(", A: "); os_printf("%u", T2A);
  os_printf("\n");
}


void ICACHE_FLASH_ATTR printFramId(){
  pinMode(16, OUTPUT);
  digitalWrite(16, HIGH);
  digitalWrite(16, LOW);
  hspi_transfer(0x9F);
  os_printf("FRAM ID: 0x%02X%02X%02X%02X\n", hspi_transfer(0), hspi_transfer(0), hspi_transfer(0), hspi_transfer(0));
  digitalWrite(16, HIGH);
}

void ICACHE_FLASH_ATTR printRtc(){
  wire_beginTransmission(0x68);
  wire_write(0x00);
  uint8_t did = wire_endTransmission();
  uint8_t got = wire_requestFrom(0x68, 7);
  
  os_printf("RTC: %u, %u:", did, got);
  uint8_t i; for(i=0;i<got;i++) os_printf(" %02X", wire_read());
  os_printf("\n");
}


void ICACHE_FLASH_ATTR testPrint(){
  uart1_print("print Is Working\n");
  uart1_println("println Is Working");
  uart1_print("printI: "); uart1_printI(-365); uart1_print("\n");
  uart1_print("printH: "); uart1_printH(115200); uart1_print("\n");
  uart1_print("printB: "); uart1_printB(123456789, 32); uart1_print("\n");
  uart1_print("printF: "); uart1_printF(-127.345); uart1_print("\n");
  uart1_print("printF: "); uart1_printF(127.345); uart1_print("\n");
}


