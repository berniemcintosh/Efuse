// Include file for most of the IO setp
#ifndef FUSEIO_H
#define FUSEIO_H


#include <avr/io.h>
#include <Arduino.h>
#include "config.h"
#include "clsPCA9555.h"


extern void setupIO();
extern void  setupADC();
extern char buffer[120];
extern PCA9555 ioport;
extern volatile byte admuxChannel;
extern volatile uint16_t adcRaw[CHANNELS];
extern volatile uint16_t adcRawMvg[CHANNELS][MVGAVERAGESAMPLES];
extern volatile uint16_t adcRawDiscarded[CHANNELS];

void setupIO(void);
void ledTest(void);
void setupADC(void);
void makeTone(uint16_t, uint16_t);


// Defines for ports to be used for Oscilloscope maontiring and debugging.

#define SCOPE_1_ON PORTD &= ~(1 << 2)
#define SCOPE_1_OFF PORTD |= (1 << 2)
#define SCOPE_1_TOGGLE PORTD ^= (1 << 2)
#define SCOPE_2_ON PORTD &= ~(1 << 3)
#define SCOPE_2_OFF PORTD |= (1 << 3)
#define SCOPE_2_TOGGLE PORTD ^= (1 << 3)
#define SCOPE_3_ON PORTD &= ~(1 << 4)
#define SCOPE_3_OFF PORTD |= (1 << 4)
#define SCOPE_3_TOGGLE PORTD ^= (1 << 4)
#define SCOPE_4_ON PORTD &= ~(1 << 5)
#define SCOPE_4_OFF PORTD |= (1 << 5)
#define SCOPE_4_TOGGLE PORTD ^= (1 << 5)
#define BUZZ_OFF PORTB &= ~(1 << 1)
#define BUZZ_ON PORTB |= (1 << 1)








#endif
