#include "espino.h"

void (*hspi_slave_rx_data_cb)(uint8_t *, uint8_t);
void (*hspi_slave_tx_data_cb)(void);
void (*hspi_slave_rx_status_cb)(uint32_t);
void (*hspi_slave_tx_status_cb)(void);

void ICACHE_FLASH_ATTR hspi_slave_setCallbacks(void (*rxd_cb)(uint8_t *, uint8_t), void (*txd_cb)(void), void (*rxs_cb)(uint32_t), void (*txs_cb)(void)){
  hspi_slave_rx_data_cb = rxd_cb;
  hspi_slave_tx_data_cb = txd_cb;
  hspi_slave_rx_status_cb = rxs_cb;
  hspi_slave_tx_status_cb = txs_cb;
}

void ICACHE_FLASH_ATTR hspi_slave_setStatus(uint32_t status){
  SPI1WS = status;
}

void ICACHE_FLASH_ATTR hspi_slave_setData(uint8_t *data, uint8_t len){
  uint8_t i;
  uint32_t out = 0;
  uint8_t bi = 0;
  uint8_t wi = 8;

  for(i=0; i<32; i++){
    out |= (i<len)?(data[i] << (bi * 8)):0;
    bi++; bi &= 3; if(!bi){
      SPI1W(wi) = out;
      out = 0;
      wi++;
    }
  }
}

LOCAL void ICACHE_FLASH_ATTR hspi_slave_isr_handler(void *para){
  uint32_t status;
  uint32_t istatus;

  istatus = SPIIR;

  if(istatus & (1 << SPII1)){ //SPI1 ISR
    status = SPI1S;
    SPI1S &= ~(0x3E0);//disable interrupts
    SPI1S |= (1 << SPISSRES);//reset
    SPI1S &= ~(0x1F);//clear interrupts 
    SPI1S |= (0x3E0);//enable interrupts

    if((status & (1 << SPISWBIS)) != 0 && (hspi_slave_rx_data_cb)){
      uint8_t i;
      uint32_t data;
      uint8_t buffer[32];
      for(i=0;i<8;i++){
        data=SPI1W(i);
        buffer[i<<2] = data & 0xff;
        buffer[(i<<2)+1] = (data >> 8) & 0xff;
        buffer[(i<<2)+2] = (data >> 16) & 0xff;
        buffer[(i<<2)+3] = (data >> 24) & 0xff;
      }
      hspi_slave_rx_data_cb(buffer, 32);
    }
    if((status & (1 << SPISRBIS)) != 0 && (hspi_slave_tx_data_cb)) hspi_slave_tx_data_cb();
    if((status & (1 << SPISWSIS)) != 0 && (hspi_slave_rx_status_cb)) hspi_slave_rx_status_cb(SPI1WS);
    if((status & (1 << SPISRSIS)) != 0 && (hspi_slave_tx_status_cb)) hspi_slave_tx_status_cb();
  } else if(istatus & (1 << SPII0)){ //SPI0 ISR
    SPI0S &= ~(0x3ff);//clear SPI ISR
  } else if(istatus & (1 << SPII2)){} //I2S ISR
}

void ICACHE_FLASH_ATTR hspi_slave_begin(uint8_t status_len){
  status_len &= 7;
  if(status_len > 4) status_len == 4;//max 32 bits
  if(status_len == 0) status_len == 1;//min 8 bits

  pinMode(SS, SPECIAL);
  pinMode(SCK, SPECIAL);
  pinMode(MISO, SPECIAL);
  pinMode(MOSI, SPECIAL);

  SPI1S = (1 << SPISE) | (1 << SPISBE) | 0x3E0;
  SPI1U = SPIUMISOH | SPIUCOMMAND | SPIUSSE;
  SPI1CLK = 0;
  SPI1U2 = (7 << SPILCOMMAND);
  SPI1S1 = (((status_len * 8) - 1) << SPISLSTA) | (0xff << SPISLBUF) | (7 << SPISLWBA) | (7 << SPISLRBA) | (1 << SPISRSTA);
  SPI1P = (1 << 19);
  SPI1CMD = SPIBUSY;
  
  ETS_SPI_INTR_ATTACH(hspi_slave_isr_handler,NULL);
  ETS_SPI_INTR_ENABLE();
}


