#include "espino.h"

uint8_t ICACHE_FLASH_ATTR uart1_level(){
  return ((U1S >> USTXC) & 0xF);
}

void ICACHE_FLASH_ATTR uart1_write(uint8_t data){
  while(uart1_level() != 0);//wait TX FIFO is if full
  U1F = data;
}

void ICACHE_FLASH_ATTR uart1_begin(uint32_t uart_br){
  pinMode(2, SPECIAL);
  U1D = (80000000 / uart_br);
  U1C0 = (3 << UCBN) | (1 << UCTXRST) | (1 << UCRXRST);
  U1C0 |= ((1 << UCTXRST) | (1 << UCRXRST));
  U1C0 &= ~((1 << UCTXRST) | (1 << UCRXRST));
  U1C1 = (127 << UCFFT);//RX Full at 127 bits
  U1IC = 0xffff;
}

void ICACHE_FLASH_ATTR uart1_end() {
  pinMode(2, INPUT);
}

void ICACHE_FLASH_ATTR uart1_print(const char *data){
  while(*data) uart1_write(*data++);
}

void ICACHE_FLASH_ATTR uart1_println(const char *data){
  while(*data) uart1_write(*data++);
  uart1_write('\n');
}

void ICACHE_FLASH_ATTR uart1_printI(int data){
  char str[16];
  os_sprintf(str, "%d", data);
  uart1_print(str);
}

void ICACHE_FLASH_ATTR uart1_printB(uint32_t data, uint8_t len){
  while(len--) uart1_write(0x30 + ((data & (1 << len)) != 0));
}

void ICACHE_FLASH_ATTR uart1_printH(uint32_t data){
  char str[16];
  os_sprintf(str, "%X", data);
  uart1_print(str);
}

void ICACHE_FLASH_ATTR uart1_printF(double data){
  int32_t t1 = (int32_t)data;
  int32_t t2 = (int32_t)((data * 1000.0) - (t1 * 1000));
  if(t2 < 0) t2 = -t2;
  char str[33];
  os_sprintf(str, "%d.%u", t1, t2);
  uart1_print(str);
}
