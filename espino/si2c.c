#include "espino.h"

uint8_t si2c_dcount = 18;
static uint8_t si2c_sda, si2c_scl;

#define SDA_LOW()   (GPES = (1 << si2c_sda)) //Enable SDA (becomes output and since GPO is 0 for the pin, it will pull the line low)
#define SDA_HIGH()  (GPEC = (1 << si2c_sda)) //Disable SDA (becomes input and since it has pullup it will go high) 
#define SDA_READ()  ((GPI & (1 << si2c_sda)) != 0)
#define SCL_LOW()   (GPES = (1 << si2c_scl)) 
#define SCL_HIGH()  (GPEC = (1 << si2c_scl)) 
#define SCL_READ()  ((GPI & (1 << si2c_scl)) != 0)

LOCAL void ICACHE_FLASH_ATTR si2c_setSpeed(uint32_t freq){
  if(freq <= 100000) si2c_dcount = 18;//about 100KHz
  else if(freq <= 200000) si2c_dcount = 8;//about 200KHz
  else if(freq <= 300000) si2c_dcount = 4;//about 300KHz
  else if(freq <= 400000) si2c_dcount = 2;//about 370KHz
  else si2c_dcount = 1;//about 450KHz
}

LOCAL void ICACHE_FLASH_ATTR si2c_begin(uint8_t sda, uint8_t scl){
  si2c_sda = sda;
  si2c_scl = scl;
  pinMode(si2c_sda, INPUT_PULLUP);
  pinMode(si2c_scl, INPUT_PULLUP);
  si2c_setSpeed(100000);
}

LOCAL void ICACHE_FLASH_ATTR si2c_stop(void){
  pinMode(si2c_sda, INPUT);
  pinMode(si2c_scl, INPUT);
}

LOCAL void ICACHE_FLASH_ATTR si2c_delay(uint8_t v){
  uint8_t i; uint32_t reg; for(i=0;i<v;i++) reg = GPI;
}

LOCAL bool ICACHE_FLASH_ATTR si2c_write_start(void) {
  SCL_HIGH();
  SDA_HIGH();
  if (SDA_READ() == 0) return false;
  si2c_delay(si2c_dcount);
  SDA_LOW();
  si2c_delay(si2c_dcount);
  return true;
}

LOCAL bool ICACHE_FLASH_ATTR si2c_write_stop(void){
  uint8_t i = 0;
  SCL_LOW();
  SDA_LOW();
  si2c_delay(si2c_dcount);
  SCL_HIGH();
  while (SCL_READ() == 0 && (i++) < 200);// Clock stretching (up to 100us)
  si2c_delay(si2c_dcount);
  SDA_HIGH();
  si2c_delay(si2c_dcount);
  
  return true;
}

LOCAL bool ICACHE_FLASH_ATTR si2c_write_bit(bool bit) {
  uint8_t i = 0;
  SCL_LOW();
  if (bit) SDA_HIGH();
  else SDA_LOW();
  si2c_delay(si2c_dcount+1);
  SCL_HIGH();
  while (SCL_READ() == 0 && (i++) < 200);// Clock stretching (up to 100us)
  si2c_delay(si2c_dcount+1);
  return true;
}

LOCAL bool ICACHE_FLASH_ATTR si2c_read_bit(void) {
  uint8_t i = 0;
  SCL_LOW();
  SDA_HIGH();
  si2c_delay(si2c_dcount+1);
  SCL_HIGH();
  while (SCL_READ() == 0 && (i++) < 200);// Clock stretching (up to 100us)
  bool bit = SDA_READ();
  si2c_delay(si2c_dcount);
  return bit;
}

LOCAL bool ICACHE_FLASH_ATTR si2c_write_byte(uint8_t byte) {
  uint8_t bit;
  for (bit = 0; bit < 8; bit++) {
    si2c_write_bit(byte & 0x80);
    byte <<= 1;
  }
  return si2c_read_bit();//NACK/ACK
}

LOCAL uint8_t ICACHE_FLASH_ATTR si2c_read_byte(bool nack) {
  uint8_t byte = 0;
  uint8_t bit;
  for (bit = 0; bit < 8; bit++) byte = (byte << 1) | si2c_read_bit();
  si2c_write_bit(nack);
  return byte;
}

uint8_t ICACHE_FLASH_ATTR si2c_write(uint8_t address, uint8_t * buf, uint32_t len, uint8_t sendStop){
  uint32_t i;
  if(!si2c_write_start()) return 1;//line busy
  si2c_write_byte(((address << 1) | 0) & 0xFF);
  for(i=0; i<len; i++) si2c_write_byte(buf[i]);
  if(sendStop) si2c_write_stop();
  return 0;
}

uint8_t ICACHE_FLASH_ATTR si2c_read(uint8_t address, uint8_t* buf, uint32_t len, uint8_t sendStop){
  uint32_t i;
  if(!si2c_write_start()) return 1;//line busy
  si2c_write_byte(((address << 1) | 1) & 0xFF);
  for(i=0; i<len; i++) buf[i] = si2c_read_byte(false);
  if(sendStop){
    si2c_write_stop();
    i = 0;
    while(SDA_READ() == 0 && (i++) < 10){
      SCL_LOW();
      si2c_delay(si2c_dcount);
      SCL_HIGH();
      si2c_delay(si2c_dcount);
    }
  } 
  return 0;
}

RingBuff_t si2c_rx_buffer;
RingBuff_t si2c_tx_buffer;
uint8_t si2c_tx_address;

void ICACHE_FLASH_ATTR wire_begin(uint8_t sda, uint8_t scl){
  si2c_begin(sda, scl);
  RingBuffer_InitBuffer(&si2c_rx_buffer);
  RingBuffer_InitBuffer(&si2c_tx_buffer);
}

void ICACHE_FLASH_ATTR wire_end(void){
  si2c_stop();
}

void ICACHE_FLASH_ATTR wire_setClock(uint32_t frequency){
  si2c_setSpeed(frequency);
}

void ICACHE_FLASH_ATTR wire_beginTransmission(uint8_t address){
  si2c_tx_address = address;
  RingBuffer_InitBuffer(&si2c_tx_buffer);
}

uint8_t ICACHE_FLASH_ATTR wire_endTransmission(){
  uint16_t txBufferLength = RingBuffer_GetCount(&si2c_tx_buffer);
  uint8_t txBuffer[txBufferLength];
  uint16_t i; for(i=0; i<txBufferLength; i++) txBuffer[i] = RingBuffer_Remove(&si2c_tx_buffer);
  uint8_t ret = si2c_write(si2c_tx_address, txBuffer, txBufferLength, true);
  RingBuffer_InitBuffer(&si2c_tx_buffer);
  return ret;
}

uint8_t ICACHE_FLASH_ATTR wire_requestFrom(uint8_t address, uint8_t quantity){
  if(quantity > BUFFER_SIZE) quantity = BUFFER_SIZE;
  uint8_t rxBuffer[quantity];
  RingBuffer_InitBuffer(&si2c_rx_buffer);
  uint8_t read = si2c_read(address, rxBuffer, quantity, true);
  uint8_t i; for(i=0; i<quantity; i++) RingBuffer_Insert(&si2c_rx_buffer, rxBuffer[i]);
  return (read == 0)?quantity:0;
}

uint8_t ICACHE_FLASH_ATTR wire_write(uint8_t data){
  RingBuffer_Insert(&si2c_tx_buffer, data);
  return 1;
}

int ICACHE_FLASH_ATTR wire_available(void){
  return RingBuffer_GetCount(&si2c_rx_buffer);
}

int ICACHE_FLASH_ATTR wire_read(void){
  return RingBuffer_Remove(&si2c_rx_buffer);
}

void ICACHE_FLASH_ATTR wire_flush(void){
  RingBuffer_InitBuffer(&si2c_rx_buffer);
  RingBuffer_InitBuffer(&si2c_tx_buffer);
}
