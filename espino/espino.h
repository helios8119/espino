#ifndef espino_h
#define espino_h

#include "ets_sys.h"
#include "c_types.h"
#include "ip_addr.h"
#include "mem.h"
#include "osapi.h"
#include "os_type.h"
#include "espconn.h"
#include "user_interface.h"
//#include "eagle_soc.h"
#include "gpio.h"

#include "esp8266_peri.h"
#include "binary.h"
#include "pins_espino.h"
#include "lrb.h"

#define NOT_A_PIN -1

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

//Digital Levels
#define HIGH 0x1
#define LOW  0x0

//Interrupt Modes
#define DISABLED	0x00
#define RISING 		0x01
#define FALLING 	0x02
#define CHANGE 		0x03
#define ONLOW 		0x04
#define ONHIGH 		0x05
#define ONLOW_WE 	0x0C
#define ONHIGH_WE 	0x0D

//PWM RANGE & FREQ
#define PWMRANGE 1023
#define PWMFREQ  1000

//GPIO FUNCTIONS
#define INPUT 			0x00
#define OUTPUT 			0x01
#define INPUT_PULLUP 	0x02
#define INPUT_PULLDOWN 	0x04
#define SPECIAL 		0xF8 //defaults to the usable BUSes uart0rx/tx uart1tx and hspi
#define FUNCTION_0 		0x08
#define FUNCTION_1 		0x18
#define FUNCTION_2 		0x28
#define FUNCTION_3 		0x38
#define FUNCTION_4 		0x48

#define SPI_CLOCK_DIV80M	0x80000000 //80 MHz
#define SPI_CLOCK_DIV40M	0x00001001 //40 MHz
#define SPI_CLOCK_DIV20M	0x00041001 //20 MHz
#define SPI_CLOCK_DIV16M	0x000fffc0 //16 MHz
#define SPI_CLOCK_DIV10M	0x000c1001 //10 MHz
#define SPI_CLOCK_DIV2 		0x00101001 //8 MHz
#define SPI_CLOCK_DIV5M		0x001c1001 //5 MHz
#define SPI_CLOCK_DIV4 		0x00241001 //4 MHz
#define SPI_CLOCK_DIV8 		0x004c1001 //2 MHz
#define SPI_CLOCK_DIV16 	0x009c1001 //1 MHz
#define SPI_CLOCK_DIV32 	0x013c1001 //500 KHz
#define SPI_CLOCK_DIV64 	0x027c1001 //250 KHz
#define SPI_CLOCK_DIV128 	0x04fc1001 //125 KHz

#define LSBFIRST 0
#define MSBFIRST 1

//timer dividers
#define TIM_DIV1 	0 //80MHz (80 ticks/us - 104857.588 us max)
#define TIM_DIV16	1 //5MHz (5 ticks/us - 1677721.4 us max)
#define TIM_DIV265	3 //312.5Khz (1 tick = 3.2us - 26843542.4 us max)
//timer int_types
#define TIM_EDGE	0
#define TIM_LEVEL	1
//timer reload values
#define TIM_SINGLE	0 //on interrupt routine you need to write a new value to start the timer again
#define TIM_LOOP	1 //on interrupt the counter will start with the same value again

void delayMicroseconds(uint32_t m);
void delay(uint32_t m);
uint32_t micros(void);
uint32_t millis(void);

void pinMode(uint8_t, uint8_t);
void digitalWrite(uint8_t, uint8_t);//47.5ns direct register write takes 23ns
int digitalRead(uint8_t);//110ns direct register read takes 74ns

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);

void isr_init(void);
void attachInterrupt(uint8_t, void (*)(void), int mode);
void detachInterrupt(uint8_t);

void analogWrite(uint8_t pin, uint16_t value);//0-PWMRANGE

int analogRead(uint8_t);

#define timer1_read()           (T1V)
#define timer1_enabled()        ((T1C & (1 << TCTE)) != 0)
#define timer1_interrupted()    ((T1C & (1 << TCIS)) != 0)

void timer1_isr_init(void);
void timer1_enable(uint8_t divider, uint8_t int_type, uint8_t reload);
void timer1_disable(void);
void timer1_attachInterrupt(void (*userFunc)(void));
void timer1_detachInterrupt(void);
void timer1_write(uint32_t ticks); //maximum ticks 8388607

void uart0_begin(uint32_t baud);
void uart0_end(void);
void uart0_write(uint8_t data);
void uart0_print(const char *data);
void uart0_println(const char *data);
void uart0_printI(int data);
void uart0_printH(uint32_t data);
void uart0_printF(double data);
void uart0_printB(uint32_t data, uint8_t len);
void uart0_flush(void);
int uart0_available(void);
int uart0_read(void);

void uart1_begin(uint32_t baud);
void uart1_end(void);
void uart1_write(uint8_t data);
void uart1_print(const char *data);
void uart1_println(const char *data);
void uart1_printI(int data);
void uart1_printH(uint32_t data);
void uart1_printF(double data);
void uart1_printB(uint32_t data, uint8_t len);

void hspi_begin(void);
void hspi_end(void);
uint8_t hspi_transfer(uint8_t data);
void hspi_setClockDivider(uint32_t d);

void hspi_slave_begin(uint8_t status_len);
void hspi_slave_setStatus(uint32_t status);
void hspi_slave_setData(uint8_t *data, uint8_t len);
void hspi_slave_setCallbacks(
	void (*rxd_cb)(uint8_t *, uint8_t),//BUFFER DATA RECEIVED
	void (*txd_cb)(void),//BUFFER DATA SENT
	void (*rxs_cb)(uint32_t),//STATUS RECEIVED
	void (*txs_cb)(void));//STATUS SENT

uint8_t si2c_write(uint8_t address, uint8_t * buf, uint32_t len, uint8_t sendStop);
uint8_t si2c_read(uint8_t address, uint8_t * buf, uint32_t len, uint8_t sendStop);
void wire_begin(uint8_t sda, uint8_t scl);
void wire_end(void);
void wire_setClock(uint32_t frequency);
void wire_beginTransmission(uint8_t address);
uint8_t wire_endTransmission();
uint8_t wire_requestFrom(uint8_t address, uint8_t quantity);

void loop(void);
void setup(void);

#endif
