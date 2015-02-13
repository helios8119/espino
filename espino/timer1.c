#include "espino.h"

void (*timer1_user_cb)(void);

LOCAL void ICACHE_FLASH_ATTR timer1_isr_handler(void *para){
    if((T1C & ((1 << TCAR) | (1 << TCIT))) == 0) TEIE &= ~TEIE1;//edge int disable
    T1I = 0;
    if(timer1_user_cb) timer1_user_cb();
}

void ICACHE_FLASH_ATTR timer1_attachInterrupt(void (*userFunc)(void)) {
    timer1_user_cb = userFunc;
    ETS_FRC1_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR timer1_detachInterrupt() {
    timer1_user_cb = 0;
    TEIE &= ~TEIE1;//edge int disable
    ETS_FRC1_INTR_DISABLE();
}

void ICACHE_FLASH_ATTR timer1_enable(uint8_t divider, uint8_t int_type, uint8_t reload){
    T1C = (1 << TCTE) | ((divider & 3) << TCPD) | ((int_type & 1) << TCIT) | ((reload & 1) << TCAR);
    T1I = 0;
}

void ICACHE_FLASH_ATTR timer1_write(uint32_t ticks){
    T1L = ((ticks) & 0x7FFFFF);
    if((T1C & (1 << TCIT)) == 0) TEIE |= TEIE1;//edge int enable
}

void ICACHE_FLASH_ATTR timer1_disable(){
    T1C = 0;
    T1I = 0;
}

void ICACHE_FLASH_ATTR timer1_isr_init(){
    ETS_FRC1_INTR_ATTACH(timer1_isr_handler, NULL);
}