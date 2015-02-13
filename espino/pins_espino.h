#ifndef Pins_espino_h
#define Pins_espino_h

#include <stdint.h>

#define EXTERNAL_NUM_INTERRUPTS     16
#define NUM_DIGITAL_PINS            17
#define NUM_ANALOG_INPUTS           1

#define analogInputToDigitalPin(p)  (p+NUM_DIGITAL_PINS)
#define digitalPinToInterrupt(p)  	(((p) < 16)?p:-1)
#define digitalPinHasPWM(p)         (-1)

static const uint8_t SS    = 15;
static const uint8_t MOSI  = 13;
static const uint8_t MISO  = 12;
static const uint8_t SCK   = 14;

static const uint8_t RX    = 3;
static const uint8_t TX    = 1;

static const uint8_t TX1   = 2;
static const uint8_t A0    = 17;

#endif
