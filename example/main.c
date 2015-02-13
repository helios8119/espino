#include "espino.h"

#include "debug_prints.h"
#include "DS3231.h"
#include "SSD1306.h"
#include "math.h"

rtc_t rtc_time;

void ICACHE_FLASH_ATTR printTime(){
	ssd1306_clearDisplay();
	ssd1306_setCursor(0,0);
	char str[128];

    if(!rtc_read(&rtc_time)){
	    ssd1306_println("ERROR reading the time!");
	} else {
		uint16_t temp = rtc_temperature();
		int8_t t1 = (temp / 100);
		uint8_t t2 = (temp % 100);
	    os_sprintf(str, " %02u/%02u/20%02u %02u:%02u:%02u\n Temperature: %d.%uC", rtc_time.d, rtc_time.mo, rtc_time.y, rtc_time.h, rtc_time.m, rtc_time.s, t1, t2);
	    ssd1306_println(str);
	}
	//ssd1306_print("      Analog: ");
	//ssd1306_printF(analogRead(A0)/1000.0);
	//ssd1306_println("mV");

	ssd1306_display();
}

//void ICACHE_FLASH_ATTR checkClock(){
	//if(rtc_hasAlarm1()) printTime();
    //if(rtc_hasAlarm2()){ os_printf("Alarm 2 "); printTime(); }
//}

void ICACHE_FLASH_ATTR onTimerInterrupt(){
	printTime();
}

void onSpiData(uint8_t *data, uint8_t len){
	os_printf("DATA[%u]: %s\n", len, (char *)data);
}

void onSpiDataSent(void){
	os_printf("DATA SENT\n");
	if(rtc_read(&rtc_time)){
		char str[32];
		uint16_t temp = rtc_temperature();
		int8_t t1 = (temp / 100);
		uint8_t t2 = (temp % 100);
	    os_sprintf(str, "%02u/%02u/20%02u %02u:%02u:%02u %d.%uC", rtc_time.d, rtc_time.mo, rtc_time.y, rtc_time.h, rtc_time.m, rtc_time.s, t1, t2);
	    hspi_slave_setData((uint8_t *)str, os_strlen(str));
	} else {
		hspi_slave_setData(NULL, 0);
	}
}

void onSpiStatus(uint32_t status){
	os_printf("STATUS: %lu\n", status);
    hspi_slave_setStatus(micros());
}

void onSpiStatusSent(void){
	os_printf("STATUS SENT\n");
}


void ICACHE_FLASH_ATTR setup(){
	uart0_begin(115200);
  	os_install_putc1((void *)uart0_write);

	//hspi_begin();
	//hspi_setClockDivider(SPI_CLOCK_DIV2);
	
	wire_begin(4,5);
	wire_setClock(400000);
	ssd1306_begin();
	ssd1306_display();

	rtc_disableAlarm1();
	rtc_disableAlarm2();
	rtc_hasAlarm1(); rtc_hasAlarm2();
	//pinMode(0, INPUT_PULLUP);
	//attachInterrupt(0, checkClock, FALLING);
	//rtc_setAlarm1(-1, -1, -1, -1, false);//every second
	//rtc_setAlarm1(30, -1, -1, -1, false);//every time seconds is 30
	//rtc_setAlarm2(-1, -1, -1, false);//every minute

	timer1_disable();
	timer1_attachInterrupt(onTimerInterrupt);
	timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
	timer1_write(5000000);//1 second

	printGPIORegs();
	printTimer1();
	printTimer2();

	//testPrint();
	//printRtc();
	//printFramId();
    printTime();

    //load initial buffer data
    char *str = "Hello World!";
    hspi_slave_setData((uint8_t *)str, os_strlen(str));
    hspi_slave_setStatus(micros());
    hspi_slave_setCallbacks(onSpiData, onSpiDataSent, onSpiStatus, onSpiStatusSent);
	hspi_slave_begin(4);//4 bytes status
}

void ICACHE_FLASH_ATTR loop(){
	
}
