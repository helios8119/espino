#include "espino.h"

void ICACHE_FLASH_ATTR hspi_begin() {
  pinMode(SCK, SPECIAL);
  pinMode(MISO, SPECIAL);
  pinMode(MOSI, SPECIAL);
  
  GPMUX = 0x105;
  SPI1C = 0;
  SPI1CLK = SPI_CLOCK_DIV16;//1MHz
  SPI1U = 0;
  SPI1U1 = (7 << SPILMOSI) | (7 << SPILMISO);
  SPI1U2 = 0;
}

void ICACHE_FLASH_ATTR hspi_end() {
  pinMode(SCK, INPUT);
  pinMode(MISO, INPUT);
  pinMode(MOSI, INPUT);
}

void ICACHE_FLASH_ATTR hspi_setClockDivider(uint32_t d){
  SPI1CLK = d;
}

uint8_t ICACHE_FLASH_ATTR hspi_transfer(uint8_t data) {
  if(!data){
    //read
    while(SPI1CMD & SPIBUSY);
    SPI1U = SPIUMISO;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY);
    return (uint8_t)(SPI1W0 & 0xff);
  }
  //write
  while(SPI1CMD & SPIBUSY);
  SPI1U = SPIUMOSI;
  SPI1W0 = data;
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY);
  return (uint8_t)(SPI1W0 & 0xff);
}