#include "espino.h"

int ICACHE_FLASH_ATTR analogRead(uint8_t pin){
  if(pin == 17){
    uint8_t i;
    uint16_t data[8];

    rom_i2c_writeReg_Mask(0x6C,2,0,5,5,1);

    ESP8266_REG(0xD5C) |= (1 << 21);
    while ((ESP8266_REG(0xD50) & (7 << 24)) > 0);
    ESP8266_REG(0xD50) &= ~(1 << 1);
    ESP8266_REG(0xD50) |= (1 << 1);
    delayMicroseconds(2);
    while ((ESP8266_REG(0xD50) & (7 << 24)) > 0);

    read_sar_dout(data);
    rom_i2c_writeReg_Mask(0x6C,2,0,5,5,1);

    while ((ESP8266_REG(0xD50) & (7 << 24)) > 0);
    ESP8266_REG(0xD5C) &= ~(1 << 21);
    ESP8266_REG(0xD60) |= (1 << 0);
    ESP8266_REG(0xD60) &= ~(1 << 0);

    uint16_t tout = 0;
    for (i = 0; i < 8; i++) tout += data[i];
    return tout >> 4;//tout is 10 bits fraction
  }
  return digitalRead(pin) * 1023;
}
