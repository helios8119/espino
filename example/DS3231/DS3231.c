#include "DS3231.h"

#define dec2bcd(n) ((n % 10) + ((uint8_t)(n / 10) << 4)) // Convert Decimal to Binary Coded Decimal (BCD)
#define bcd2dec(n) ((n & 0x0F) + (10 * ((n & 0xF0) >> 4)))// Convert Binary Coded Decimal (BCD) to Decimal

int ICACHE_FLASH_ATTR rtc_getReg(uint8_t reg){
    uint8_t data;
    if(si2c_write(0x68, &reg, 1, true)) return -1;
    if(si2c_read(0x68, &data, 1, true)) return -1;
	return data;
}

bool ICACHE_FLASH_ATTR rtc_setReg(uint8_t reg, uint8_t value){
	uint8_t data[2] = {reg, value};
  	return si2c_write(0x68, data, 2, true) == 0;
}

bool ICACHE_FLASH_ATTR rtc_readBytes(uint8_t reg, uint8_t *buf, uint16_t len){
    if(si2c_write(0x68, &reg, 1, true)) return false;
    return si2c_read(0x68, buf, len, true) == 0;
}

bool ICACHE_FLASH_ATTR rtc_writeBytes(uint8_t reg, uint8_t *buf, uint16_t len){
    uint8_t data[len+1];
    data[0] = reg;
    uint8_t i;
    for (i=0; i<len; i++) data[i+1] = buf[i];
    return si2c_write(0x68, data, len+1, true) == 0;
}

bool ICACHE_FLASH_ATTR rtc_updateReg(uint8_t reg, uint8_t value, uint8_t mask){
    int regval = rtc_getReg(reg);
    if(regval == -1) return false;
    return rtc_setReg(reg, (regval & ~mask) | value);
}

bool ICACHE_FLASH_ATTR rtc_read(rtc_t *rtm){
	uint8_t buf[7];
	if(rtc_readBytes(0x00, buf, 7)){
		//if ((buf[0] & 0x80) == 0){
			rtm->s = bcd2dec(buf[0] & 0x7F);
			rtm->m = bcd2dec(buf[1] & 0x7F);
			rtm->h = (buf[2] & 0x40)?(bcd2dec(buf[2] & 0x1F) + (12 * ((buf[2] & 0x20) >> 5))):bcd2dec(buf[2] & 0x3F);
			rtm->w = bcd2dec(buf[3] & 0x07);
			rtm->d = bcd2dec(buf[4] & 0x3F);
			rtm->mo = bcd2dec(buf[5] & 0x1F);
			rtm->y = bcd2dec(buf[6]);
			return true;
		//}
    } else {
        os_printf(" read ");
    }
    return false;
}

bool ICACHE_FLASH_ATTR rtc_write(rtc_t *rtm){
	uint8_t buf[7];
	buf[0] = dec2bcd(rtm->s);   
	buf[1] = dec2bcd(rtm->m);
	buf[2] = dec2bcd(rtm->h);      // sets 24 hour format
	buf[3] = dec2bcd(rtm->w);   
	buf[4] = dec2bcd(rtm->d);
	buf[5] = dec2bcd(rtm->mo);
	buf[6] = dec2bcd(rtm->y); 
	return rtc_writeBytes(0x00, buf, 7);
}

bool ICACHE_FLASH_ATTR rtc_set(){
	rtc_t rtm;
	/*time_t timev = time(0);
	struct tm * now = localtime(&timev);
	rtm.s = now->tm_sec;
	rtm.m = now->tm_min;
	rtm.h = now->tm_hour;
	rtm.w = now->tm_wday?now->tm_wday:7;
	rtm.d = now->tm_mday;
	rtm.mo = now->tm_mon + 1;
	rtm.y = now->tm_year - 100;*/
	return rtc_write(&rtm) && rtc_setReg(RTCCR, RTCINTCN) && rtc_setReg(RTCSR, 0);
}

bool ICACHE_FLASH_ATTR rtc_hasStopped(){
    return (rtc_getReg(RTCSR) & RTCOSF);
}

uint16_t ICACHE_FLASH_ATTR rtc_temperature(){
    uint16_t temp = 0;
    temp += rtc_getReg(RTCTH) * 100;
    temp += (rtc_getReg(RTCTL) >> 6) * 25;
    return temp;
}

bool ICACHE_FLASH_ATTR rtc_setAlarm1(int seconds, int minutes, int hour, int day, bool weekday){
    uint8_t data[4];
    if(day <= 0 || hour == -1 || minutes == -1 || seconds == -1){
        data[3] = 0x80;//A1M4 = 1; ignore day
    } else {
        if(weekday){
            day -= 1; //make it start from 0
            day %= 7; //make sure we are in range (0-6)
            day += 1; // return back to 1-7
            data[3] = 0x40 | day;
        } else {
            day -= 1; //go to 0-30
            day %= 31; // make sure it's in 0-30
            day += 1; //go back to 1-31
            data[3] = ((day/10) << 4) | (day % 10);
        }
    }
    
    if(hour == -1 || minutes == -1 || seconds == -1){
        data[2] = 0x80;//A1M3 = 1; ignore hour
    } else {
        hour %= 24;//ensure we are in 0-23 range
        data[2] = ((hour/10) << 4) | (hour % 10);
    }
    
    if(minutes == -1 || seconds == -1){
        data[1] = 0x80;//A1M2 = 1; ignore minutes
    } else {
        minutes %= 60;//ensure we are in 0-59 range
        data[1] = ((minutes/10) << 4) | (minutes % 10);
    }
    
    if(seconds == -1){
        data[0] = 0x80;//A1M1 = 1; ignore seconds
    } else {
        seconds %= 60;//ensure we are in 0-59 range
        data[0] = ((seconds/10) << 4) | (seconds % 10);
    }
    return rtc_writeBytes(RTCA1, data, 4) && rtc_updateReg(RTCCR, RTCA1IE, RTCA1IE) && rtc_updateReg(RTCSR, 0, RTCA1F);
}

bool ICACHE_FLASH_ATTR rtc_hasAlarm1(){
    uint8_t reg = rtc_getReg(RTCSR);
    if(reg & RTCA1F) rtc_setReg(RTCSR, reg & ~RTCA1F);
    return (reg & RTCA1F) != 0;
}

bool ICACHE_FLASH_ATTR rtc_enableAlarm1(){
    return rtc_setReg(RTCCR, rtc_getReg(RTCCR) | RTCA1IE);
}

bool ICACHE_FLASH_ATTR rtc_disableAlarm1(){
    return rtc_setReg(RTCCR, rtc_getReg(RTCCR) & ~RTCA1IE);
}

bool ICACHE_FLASH_ATTR rtc_isAlarm1Enabled(){
    return (rtc_getReg(RTCCR) & RTCA1IE) != 0;
}



bool ICACHE_FLASH_ATTR rtc_setAlarm2(int minutes, int hour, int day, bool weekday){
    uint8_t data[3];
    if(day <= 0 || hour == -1 || minutes == -1){
        data[2] = 0x80;//A2M4 = 1; ignore day
    } else {
        if(weekday){
            day -= 1; //make it start from 0
            day %= 7; //make sure we are in range (0-6)
            day += 1; // return back to 1-7
            data[2] = 0x40 | day;
        } else {
            day -= 1; //go to 0-30
            day %= 31; // make sure it's in 0-30
            day += 1; //go back to 1-31
            data[2] = ((day/10) << 4) | (day % 10);
        }
    }
    
    if(hour == -1 || minutes == -1){
        data[1] = 0x80;//A2M3 = 1; ignore hour
    } else {
        hour %= 24;//ensure we are in 0-23 range
        data[1] = ((hour/10) << 4) | (hour % 10);
    }
    
    if(minutes == -1){
        data[0] = 0x80;//A2M2 = 1; ignore minutes
    } else {
        minutes %= 60;//ensure we are in 0-59 range
        data[0] = ((minutes/10) << 4) | (minutes % 10);
    }
    return rtc_writeBytes(RTCA2, data, 3) && rtc_updateReg(RTCCR, RTCA2IE, RTCA2IE) && rtc_updateReg(RTCSR, 0, RTCA2F);
}

bool ICACHE_FLASH_ATTR rtc_enableAlarm2(){
    return rtc_setReg(RTCCR, rtc_getReg(RTCCR) | RTCA2IE);
}

bool ICACHE_FLASH_ATTR rtc_disableAlarm2(){
    return rtc_setReg(RTCCR, rtc_getReg(RTCCR) & ~RTCA2IE);
}

bool ICACHE_FLASH_ATTR rtc_isAlarm2Enabled(){
    return (rtc_getReg(RTCCR) & RTCA2IE) != 0;
}

bool ICACHE_FLASH_ATTR rtc_hasAlarm2(){
    uint8_t reg = rtc_getReg(RTCSR);
    if(reg & RTCA2F) rtc_setReg(RTCSR, reg & ~RTCA2F);
    return (reg & RTCA2F) != 0;
}



