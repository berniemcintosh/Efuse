#ifndef FUSEIO_H
#define FUSEIO_H


#include <avr/io.h>
#include <Arduino.h>
#include "config.h"


enum ledState {off, on, flash};

extern void setupIO();
extern void  setupADC();





 extern volatile bool adcSwitchEnabled[CHANNELS];
  extern volatile bool adcOutputEnabled[CHANNELS];
  extern volatile ledState chanLED[CHANNELS];


extern volatile byte admuxChannel;
extern volatile uint16_t adcRaw[CHANNELS];
extern volatile uint16_t adcRawDiscarded[CHANNELS];
extern volatile bool newReadingAvailable;








enum adcSubjectEnum { isPrimary, isSecondary, isCurrent};
enum outputStates { P, S, O};
enum sourceStatus {UVF, OVF, OK};
volatile extern outputStates outputSelection;
volatile extern sourceStatus priStatus;
volatile extern sourceStatus secStatus;
volatile extern bool currentFault;
volatile extern uint16_t tone1;
volatile extern bool secEnablePermissable;
volatile extern bool primEnablePermissable;
extern uint8_t switchA, switchB, lastStateSwitchA, lastStateSwitchB;
extern bool modeSwitchOverride;
volatile extern uint8_t pwm_Value;
volatile extern uint16_t iTripValue;
volatile extern uint16_t sTripValue;

void setupIO(void);
void ledTest(void);
void setupADC(void);
void makeTone(uint16_t, uint16_t);



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








#endif
