#include "SSD1306.h"
#include "glcdfont.h"
int8_t ssd1306_rotation;

// the memory buffer for the LCD

static uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8] = { 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x80, 0x80, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0xFF,
#if (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH > 96*16)
0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
0x80, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x8C, 0x8E, 0x84, 0x00, 0x00, 0x80, 0xF8,
0xF8, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80,
0x00, 0xE0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xC7, 0x01, 0x01,
0x01, 0x01, 0x83, 0xFF, 0xFF, 0x00, 0x00, 0x7C, 0xFE, 0xC7, 0x01, 0x01, 0x01, 0x01, 0x83, 0xFF,
0xFF, 0xFF, 0x00, 0x38, 0xFE, 0xC7, 0x83, 0x01, 0x01, 0x01, 0x83, 0xC7, 0xFF, 0xFF, 0x00, 0x00,
0x01, 0xFF, 0xFF, 0x01, 0x01, 0x00, 0xFF, 0xFF, 0x07, 0x01, 0x01, 0x01, 0x00, 0x00, 0x7F, 0xFF,
0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0xFF,
0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0x0F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x8F,
0x8F, 0x9F, 0xBF, 0xFF, 0xFF, 0xC3, 0xC0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC,
0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x01, 0x03, 0x03, 0x03,
0x03, 0x03, 0x01, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01,
0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00,
0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00, 0x00, 0x03,
0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#if (SSD1306_LCDHEIGHT == 64)
0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x1F, 0x0F,
0x87, 0xC7, 0xF7, 0xFF, 0xFF, 0x1F, 0x1F, 0x3D, 0xFC, 0xF8, 0xF8, 0xF8, 0xF8, 0x7C, 0x7D, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x00, 0x30, 0x30, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xC0, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xC0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0x1F,
0x0F, 0x07, 0x1F, 0x7F, 0xFF, 0xFF, 0xF8, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xE0,
0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00,
0x00, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x0E, 0xFC, 0xF8, 0x00, 0x00, 0xF0, 0xF8, 0x1C, 0x0E,
0x06, 0x06, 0x06, 0x0C, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0xFC,
0xFE, 0xFC, 0x00, 0x18, 0x3C, 0x7E, 0x66, 0xE6, 0xCE, 0x84, 0x00, 0x00, 0x06, 0xFF, 0xFF, 0x06,
0x06, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x06, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0xC0, 0xF8,
0xFC, 0x4E, 0x46, 0x46, 0x46, 0x4E, 0x7C, 0x78, 0x40, 0x18, 0x3C, 0x76, 0xE6, 0xCE, 0xCC, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x0F, 0x03,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00,
0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x03, 0x07, 0x0E, 0x0C,
0x18, 0x18, 0x0C, 0x06, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x01, 0x0F, 0x0E, 0x0C, 0x18, 0x0C, 0x0F,
0x07, 0x01, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00,
0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x07,
0x07, 0x0C, 0x0C, 0x18, 0x1C, 0x0C, 0x06, 0x06, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif
#endif
};


void ICACHE_FLASH_ATTR ssd1306_command(uint8_t c) {
    uint8_t control = 0x00;
    wire_beginTransmission(SSD1306_I2C_ADDRESS);
    WIRE_WRITE(control);
    WIRE_WRITE(c);
    wire_endTransmission();
}

void ICACHE_FLASH_ATTR ssd1306_data(uint8_t c) {
  uint8_t control = 0x40;   // Co = 0, D/C = 1
  wire_beginTransmission(SSD1306_I2C_ADDRESS);
  WIRE_WRITE(control);
  WIRE_WRITE(c);
  wire_endTransmission();
}

void ICACHE_FLASH_ATTR ssd1306_draw(void) {
  uint16_t i;
  for (i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
    wire_beginTransmission(SSD1306_I2C_ADDRESS);
    WIRE_WRITE(0x40);
    uint8_t x;
    for (x=0; x<16; x++) {
      WIRE_WRITE(buffer[i]);
      i++;
    }
    i--;
    wire_endTransmission();
  }
}

void ICACHE_FLASH_ATTR ssd1306_begin() {

  // Init sequence for 128x64 OLED module
  ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
  ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
  ssd1306_command(0x80);                                  // the suggested ratio 0x80
  ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
  ssd1306_command(0x3F);
  ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
  ssd1306_command(0x0);                                   // no offset
  ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
  ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
  ssd1306_command(0x14);
  ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
  ssd1306_command(0x00);                                  // 0x0 act like ks0108
  ssd1306_command(SSD1306_SEGREMAP | 0x1);
  ssd1306_command(SSD1306_COMSCANDEC);
  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x12);
  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
  ssd1306_command(0xCF);
  ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
  ssd1306_command(0xF1);
  ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
  ssd1306_command(0x40);
  ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
  ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
    
  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
}


void ICACHE_FLASH_ATTR ssd1306_invertDisplay(uint8_t i) {
  if (i) {
    ssd1306_command(SSD1306_INVERTDISPLAY);
  } else {
    ssd1306_command(SSD1306_NORMALDISPLAY);
  }
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void ICACHE_FLASH_ATTR ssd1306_startscrollright(uint8_t start, uint8_t stop){
  ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X00);
  ssd1306_command(0XFF);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void ICACHE_FLASH_ATTR ssd1306_startscrollleft(uint8_t start, uint8_t stop){
  ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X00);
  ssd1306_command(0XFF);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void ICACHE_FLASH_ATTR ssd1306_startscrolldiagright(uint8_t start, uint8_t stop){
  ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);  
  ssd1306_command(0X00);
  ssd1306_command(SSD1306_LCDHEIGHT);
  ssd1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X01);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void ICACHE_FLASH_ATTR ssd1306_startscrolldiagleft(uint8_t start, uint8_t stop){
  ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);  
  ssd1306_command(0X00);
  ssd1306_command(SSD1306_LCDHEIGHT);
  ssd1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X01);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void ICACHE_FLASH_ATTR ssd1306_stopscroll(void){
  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
}

void ICACHE_FLASH_ATTR ssd1306_dim(uint8_t contrast) {
  ssd1306_command(SSD1306_SETCONTRAST);
  ssd1306_command(contrast);
}

void ICACHE_FLASH_ATTR ssd1306_display(void) {
  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(0);   // Column start address (0 = reset)
  ssd1306_command(SSD1306_LCDWIDTH-1); // Column end address (127 = reset)

  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(0); // Page start address (0 = reset)
  ssd1306_command(7); // Page end address

  ssd1306_draw();
}

// clear everything
void ICACHE_FLASH_ATTR ssd1306_clearDisplay(void) {
  memset(buffer, 0, (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8));
}

#define swap(a, b) { int16_t t = a; a = b; b = t; }
// the most basic function, set a single pixel
void ICACHE_FLASH_ATTR ssd1306_drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= SSD1306_LCDWIDTH) || (y < 0) || (y >= SSD1306_LCDHEIGHT))
    return;

  // check rotation, move pixel around if necessary
  switch (ssd1306_rotation) {
  case 1:
    swap(x, y);
    x = SSD1306_LCDWIDTH - x - 1;
    break;
  case 2:
    x = SSD1306_LCDWIDTH - x - 1;
    y = SSD1306_LCDHEIGHT - y - 1;
    break;
  case 3:
    swap(x, y);
    y = SSD1306_LCDHEIGHT - y - 1;
    break;
  }  

  // x is which column
    switch (color) 
    {
      case WHITE:   buffer[x+ (y/8)*SSD1306_LCDWIDTH] |=  (1 << (y&7)); break;
      case BLACK:   buffer[x+ (y/8)*SSD1306_LCDWIDTH] &= ~(1 << (y&7)); break; 
      case INVERSE: buffer[x+ (y/8)*SSD1306_LCDWIDTH] ^=  (1 << (y&7)); break; 
    }
    
}

// Bresenham's algorithm - thx wikpedia
void ICACHE_FLASH_ATTR ssd1306_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }
  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }
  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }
  for (; x0<=x1; x0++) {
    if (steep) {
      ssd1306_drawPixel(y0, x0, color);
    } else {
      ssd1306_drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void ICACHE_FLASH_ATTR ssd1306_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  int16_t i; for (i=x; i<x+w; i++) ssd1306_drawLine(i, y, x, y+h-1, color);
}

// Draw a character
void ICACHE_FLASH_ATTR ssd1306_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
  if((x >= SSD1306_LCDWIDTH)  || // Clip right
     (y >= SSD1306_LCDHEIGHT) || // Clip bottom
     ((x + 6 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;
  int8_t i;
  for (i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5) 
      line = 0x0;
    else 
      line = font[(c*5)+i];
    int8_t j;
    for (j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          ssd1306_drawPixel(x+i, y+j, color);
        else {  // big size
          ssd1306_fillRect(x+(i*size), y+(j*size), size, size, color);
        } 
      } else if (bg != color) {
        if (size == 1) // default size
          ssd1306_drawPixel(x+i, y+j, bg);
        else {  // big size
          ssd1306_fillRect(x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

static int16_t ssd1306_cursor_y = 0;
static int16_t ssd1306_cursor_x = 0;
static uint8_t ssd1306_textsize = 1;

void ICACHE_FLASH_ATTR ssd1306_setCursor(int16_t x, int16_t y) {
  ssd1306_cursor_x = x;
  ssd1306_cursor_y = y * (ssd1306_textsize * 8);
}

void ICACHE_FLASH_ATTR ssd1306_setTextSize(uint8_t s) {
  ssd1306_textsize = (s > 0) ? s : 1;
}

void ICACHE_FLASH_ATTR ssd1306_write(uint8_t c) {
  if (c == '\n') {
    ssd1306_cursor_y += ssd1306_textsize*8;
    ssd1306_cursor_x  = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    ssd1306_drawChar(ssd1306_cursor_x, ssd1306_cursor_y, c, WHITE, BLACK, ssd1306_textsize);
    ssd1306_cursor_x += ssd1306_textsize*6;
    if ((ssd1306_cursor_x > (SSD1306_LCDWIDTH - ssd1306_textsize*6))) {
      ssd1306_cursor_y += ssd1306_textsize*8;
      ssd1306_cursor_x = 0;
    }
  }
}

void ICACHE_FLASH_ATTR ssd1306_print(const char *data){
  while(*data) ssd1306_write(*data++);
}

void ICACHE_FLASH_ATTR ssd1306_println(const char *data){
  while(*data) ssd1306_write(*data++);
  ssd1306_write('\n');
}

void ICACHE_FLASH_ATTR ssd1306_printI(int data){
  char str[16];
  os_sprintf(str, "%d", data);
  ssd1306_print(str);
}

void ICACHE_FLASH_ATTR ssd1306_printB(uint32_t data, uint8_t len){
  while(len--) ssd1306_write(0x30 + ((data & (1 << len)) != 0));
}

void ICACHE_FLASH_ATTR ssd1306_printH(uint32_t data){
  char str[16];
  os_sprintf(str, "%X", data);
  ssd1306_print(str);
}

void ICACHE_FLASH_ATTR ssd1306_printF(double data){
  int32_t t1 = (int32_t)data;
  int32_t t2 = (int32_t)((data * 1000.0) - (t1 * 1000));
  if(t2 < 0) t2 = -t2;
  char str[33];
  os_sprintf(str, "%d.%03u", t1, t2);
  ssd1306_print(str);
}