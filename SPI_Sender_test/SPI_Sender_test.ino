#include <SPI.h>

void setup() {
  Serial.begin(115200);
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
}

void readSpiStatus(){
  uint8_t data[4];
  digitalWrite(SS, LOW);
  SPI.transfer(0x04);
  for(uint8_t i=0; i<4; i++){
    data[i] = SPI.transfer(0);
  }
  digitalWrite(SS, HIGH);
  uint32_t status = (data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
  Serial.print("Status: "); Serial.println(status, HEX);
}

void readSpiBuffer(){
  uint8_t data[32];
  digitalWrite(SS, LOW);
  SPI.transfer(0x03);
  SPI.transfer(0x00);
  for(uint8_t i=0; i<32; i++) data[i] = SPI.transfer(0);
  digitalWrite(SS, HIGH);
  Serial.print("Data: "); Serial.println((char *)data);
}

void writeSpiBuffer(char *str){
  uint8_t i=0;
  digitalWrite(SS, LOW);
  SPI.transfer(0x02);
  SPI.transfer(0x00);
  while(*str && i++ < 32) SPI.transfer(*str++);
  while(i++ < 32) SPI.transfer(0);
  digitalWrite(SS, HIGH);
}

void writeSpiStatus(uint32_t status){
  digitalWrite(SS, LOW);
  SPI.transfer(0x01);
  SPI.transfer(status & 0xFF);
  SPI.transfer((status >> 8) & 0xFF);
  SPI.transfer((status >> 16) & 0xFF);
  SPI.transfer((status >> 32) & 0xFF);
  digitalWrite(SS, HIGH);
}

void loop() {
  readSpiBuffer();
  delay(1000);
  readSpiStatus();
  delay(1000);
  writeSpiBuffer("Hello mate :)");
  delay(1000);
  writeSpiStatus(micros());
  delay(1000);
}
