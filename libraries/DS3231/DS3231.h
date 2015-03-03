#ifndef DS3231_h
#define DS3231_h
#include "espino.h"

#define DS3231_SQW_MASK  0x18
#define DS3231_SQW_1HZ   0x00
#define DS3231_SQW_1KHZ  0x08
#define DS3231_SQW_4KHZ  0x10
#define DS3231_SQW_8KHZ  0x18

//registers
#define RTCDR       0x00 //data register start
#define RTCA1       0x07 //Alarm1 register start
#define RTCA2       0x0B //Alarm 2 register start
#define RTCCR       0x0E //Control Register
#define RTCSR       0x0F //Status Register
#define RTCAO       0x10 //Aging Offset
#define RTCTH       0x11 //Temperature MSB
#define RTCTL       0x12 //Temperature LSB (value * 0.25 deg)

//Control register bits
#define RTCEOSC     (1 << 7) //Enable Oscillator (active low)
#define RTCBBSQW    (1 << 6) //Battery-Backed Square-Wave Enable
#define RTCCONV     (1 << 5) //Convert Temperature
#define RTCRS2      (1 << 4) //Rate Select
#define RTCRS1      (1 << 3) //Rate Select
#define RTCINTCN    (1 << 2) //Interrupt Control
#define RTCA2IE     (1 << 1) //Alarm 2 Interrupt Enable
#define RTCA1IE     (1 << 0) //Alarm 1 Interrupt Enable

//Status register bits
#define RTCOSF      (1 << 7) //Oscillator Stop Flag (clear needed)
#define RTCEN32kHz  (1 << 3) //Enable 32kHz Output
#define RTCBSY      (1 << 2) //Busy
#define RTCA2F      (1 << 1) //Alarm 2 Flag
#define RTCA1F      (1 << 0) //Alarm 1 Flag

typedef struct { 
  uint8_t s; 
  uint8_t m; 
  uint8_t h;
  uint8_t w;
  uint8_t d;
  uint8_t mo; 
  uint8_t y;   // offset from 2000;
} rtc_t;

int  rtc_getReg(uint8_t address);
bool rtc_setReg(uint8_t address, uint8_t value);
bool rtc_updateReg(uint8_t address, uint8_t value, uint8_t mask);
bool rtc_readBytes(uint8_t address, uint8_t *data, uint16_t len);
bool rtc_writeBytes(uint8_t address, uint8_t *data, uint16_t len);

bool rtc_read(rtc_t *rtm);
bool rtc_write(rtc_t *rtm);
bool rtc_set(void);
bool rtc_hasStopped(void);

uint16_t rtc_temperature(void);

bool rtc_setAlarm1(int seconds, int minutes, int hour, int day, bool weekday);
bool rtc_enableAlarm1(void);
bool rtc_disableAlarm1(void);
bool rtc_hasAlarm1(void);
bool rtc_isAlarm1Enabled(void);

bool rtc_setAlarm2(int minutes, int hour, int day, bool weekday);
bool rtc_enableAlarm2(void);
bool rtc_disableAlarm2(void);
bool rtc_hasAlarm2(void);
bool rtc_isAlarm2Enabled(void);

#endif
