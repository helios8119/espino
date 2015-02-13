#ifndef SSD1306_H
#define SSD1306_H

#include "espino.h"

#define WIRE_WRITE wire_write


typedef volatile uint8_t PortReg;
typedef uint8_t PortMask;

#define BLACK 0
#define WHITE 1
#define INVERSE 2

#define SSD1306_I2C_ADDRESS   0x3C	// 011110+SA0+RW - 0x3C or 0x3D
// Address for 128x32 is 0x3C
// Address for 128x64 is 0x3D (default) or 0x3C (if SA0 is grounded)

/*=========================================================================
    SSD1306 Displays
    ---------------------------------------------------------------------*/
#define SSD1306_128_64
#define SSD1306_LCDWIDTH                  128
#define SSD1306_LCDHEIGHT                 64

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A


void ssd1306_begin();
void ssd1306_command(uint8_t c);
void ssd1306_data(uint8_t c);
void ssd1306_draw(void);

void ssd1306_clearDisplay(void);
void ssd1306_invertDisplay(uint8_t i);
void ssd1306_display();

void ssd1306_startscrollright(uint8_t start, uint8_t stop);
void ssd1306_startscrollleft(uint8_t start, uint8_t stop);
void ssd1306_startscrolldiagright(uint8_t start, uint8_t stop);
void ssd1306_startscrolldiagleft(uint8_t start, uint8_t stop);
void ssd1306_stopscroll(void);

void ssd1306_dim(uint8_t contrast);


void ssd1306_setCursor(int16_t x, int16_t y);
void ssd1306_setTextSize(uint8_t s);
void ssd1306_write(uint8_t c);

void ssd1306_drawPixel(int16_t x, int16_t y, uint16_t color);
void ssd1306_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void ssd1306_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);


#endif
