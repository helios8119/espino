#include "espino.h"

RingBuff_t uart0_rx_buffer;

void LOCAL ICACHE_FLASH_ATTR uart0_rx_intr_handler(void *para){
  uint32_t int_status = U0IS;

  if(int_status & (1 << UIFR)) U0IC = (1 << UIFR);//clear any frame error

  if(int_status & (1 << UIFF) || int_status & (1 << UITO)){
    ETS_UART_INTR_DISABLE();
    while(((U0S >> USRXC) & 0xF) != 0){
      WDT_RESET();
      RingBuffer_Insert(&uart0_rx_buffer, U0F);
    }
    int_status = U0IS;
    if(int_status & (1 << UIFF)) U0IC = (1 << UIFF);//clear any FIFO FULL error
    if(int_status & (1 << UITO)) U0IC = (1 << UITO);//clear any TimeOut error
    ETS_UART_INTR_ENABLE();
  }
}

int ICACHE_FLASH_ATTR uart0_read(){
  return RingBuffer_Remove(&uart0_rx_buffer);
}

int ICACHE_FLASH_ATTR uart0_available(){
  return RingBuffer_GetCount(&uart0_rx_buffer);
}

void ICACHE_FLASH_ATTR uart0_flush(){
  RingBuffer_InitBuffer(&uart0_rx_buffer);
}

void ICACHE_FLASH_ATTR uart0_begin(uint32_t uart_br){
  uart0_flush();
  ETS_UART_INTR_ATTACH(uart0_rx_intr_handler,  NULL);
  pinMode(1, SPECIAL);//TX
  pinMode(3, SPECIAL);//RX
  U0D = (80000000 / uart_br);
  U0C0 = (3 << UCBN) | (1 << UCTXRST) | (1 << UCRXRST);
  U0C0 |= ((1 << UCTXRST) | (1 << UCRXRST));
  U0C0 &= ~((1 << UCTXRST) | (1 << UCRXRST));
  U0C1 = (127 << UCFFT) | (0x02 << UCTOT) | (1 <<UCTOE );
  U0IC = 0xffff;
  U0IE = (1 << UIFF) | (1 << UIFR) | (1 << UITO);
  ETS_UART_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR uart0_end() {
  pinMode(1, INPUT);
  pinMode(3, INPUT);
}

void ICACHE_FLASH_ATTR uart0_write(uint8_t data){
  while(((U0S >> USTXC) & 0xF) != 0);//wait TX FIFO is if full
  U0F = data;
}

void ICACHE_FLASH_ATTR uart0_print(const char *data){
  while(*data) uart0_write(*data++);
}

void ICACHE_FLASH_ATTR uart0_println(const char *data){
  while(*data) uart0_write(*data++);
  uart0_write('\n');
}

void ICACHE_FLASH_ATTR uart0_printI(int data){
  char str[16];
  os_sprintf(str, "%d", data);
  uart0_print(str);
}

void ICACHE_FLASH_ATTR uart0_printB(uint32_t data, uint8_t len){
  while(len--) uart0_write(0x30 + ((data & (1 << len)) != 0));
}

void ICACHE_FLASH_ATTR uart0_printH(uint32_t data){
  char str[16];
  os_sprintf(str, "%X", data);
  uart0_print(str);
}

void ICACHE_FLASH_ATTR uart0_printF(double data){
  int32_t t1 = (int32_t)data;
  int32_t t2 = (int32_t)((data * 1000.0) - (t1 * 1000));
  if(t2 < 0) t2 = -t2;
  char str[33];
  os_sprintf(str, "%d.%u", t1, t2);
  uart0_print(str);
}
