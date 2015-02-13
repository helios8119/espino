#define ARDUINO_MAIN

#include "espino.h"

void ICACHE_FLASH_ATTR delayMicroseconds(uint32_t m){
  uint32_t start = micros();
  while((micros() - start) < m);
}

void ICACHE_FLASH_ATTR delay(uint32_t m){ delayMicroseconds(m * 1000); }
uint32_t ICACHE_FLASH_ATTR micros(){ return system_get_time(); }
uint32_t ICACHE_FLASH_ATTR millis(){ return (micros() / 1000); }

#define arduLoopTaskPrio        2
#define arduLoopTaskQueueLen    1

os_event_t arduLoopTaskQueue[arduLoopTaskQueueLen];

static void ICACHE_FLASH_ATTR arduLoopTask(os_event_t *events){
  loop();
  system_os_post(arduLoopTaskPrio, 0, 0);
}

void ICACHE_FLASH_ATTR onInit(void){
	isr_init();
	timer1_isr_init();
	setup();
	system_os_task(arduLoopTask, arduLoopTaskPrio, arduLoopTaskQueue, arduLoopTaskQueueLen);
	system_os_post(arduLoopTaskPrio, 0, 0);
}

void ICACHE_FLASH_ATTR user_init(void){
	system_init_done_cb((init_done_cb_t)onInit);
}
