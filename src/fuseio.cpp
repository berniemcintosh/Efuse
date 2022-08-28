#include "fuseio.h"
#include "config.h"


volatile byte admuxChannel = 0;
volatile uint16_t adcRaw[CHANNELS];
volatile uint16_t adcRawMvg[CHANNELS][(byte)MVGAVERAGESAMPLES];
volatile uint16_t adcRawDiscarded[CHANNELS];


volatile bool newReadingAvailable = false;
volatile bool toBeDiscarded=true;
volatile uint16_t dummyResult; 
volatile byte mvgAvgCount[CHANNELS];


void setupIO()
{

  //  DDRA |= (1 << 7) | (1 << 6 ) | (1 << 4) ;
  //  DDRB |= (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0);
 //   DDRC |= (1 << 7) | (1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2);
    DDRD |= (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5); // Scope ports for monitoring timing on eval board
SCOPE_1_OFF;
SCOPE_2_OFF;
SCOPE_3_OFF;
SCOPE_4_OFF;

 
}


void setupADC()
{
    for (int i=0;i < CHANNELS; i++) {adcRaw[i] = 0; mvgAvgCount[i]=0;}  // set all the initial readings to zero
    for (int i=0;i < CHANNELS;i++) for (int j=0; j< MVGAVERAGESAMPLES;j++)   adcRawMvg[i][j]=0;

    //Set up ADC. To be run once at startup.


    // Above 200KHz 10-bit results are not reliable.
    // Prescaler set to 32
    // ADEN: ADCENABLE set to 0
    // ADSC: ADC START CONVERSION set to 0
    // ADATE: ADC Auyto Trigger Enable set to 0
    // ADIF: ADC Interrupt Flag set to 0  
    ADCSRA |= B00000000; 
   


    // Set REFS1..0 in ADMUX  to change reference voltage to AVcc
    // All ADC 0 selected
    // Result left adjusted
    ADMUX |= B01000000;
    ADMUX &= B01000000;
    

    // Set ADEN in ADCSRA (0x7A) to enable the ADC.
    // Note, this instruction takes 12 ADC clocks to execute
    ADCSRA |= B10000000;

    // Set ADIE in ADCSRA (0x7A) to enable the ADC interrupt.
    // Without this, the internal interrupt will not trigger.
    ADCSRA |= B00001000;
    // Timer 2 Setup

// Turn off global interrupts while we set up timer
    cli();
 // original atmega32   TCCR2A |= (1 << WGM21) | (0 << CS22) | (0 << CS21) | (1 << CS20);
 // original atmega32    TCCR2A &= ~((1 << 6) | (1 << 4) | (1 << 5));
   TCCR2A = 0;
   TCCR2B = 0;
   TCCR2A |= (1 << WGM21) ; // Set CTC mode
  //  TCCR2A &= ~((1 << WGM20) | (1 << COM2A1) | (1 << COM2A0)(1 << COM2B1) | (1 << COM2B0));  // Set CTC mode and turn OC2 pin off. It isn't used.
     TCCR2B |=  (1 << CS22) | (0 << CS21) | (0 << CS20); // Set CTC mode
 //     TCCR2B &= ~((1 << WGM22) | (1 << COM2A1) | (1 << COM2A0)(1 << COM2B1) | (1 << COM2B0));  // Set CTC mode and turn OC2 pin off. It isn't used.
    TCNT2 = 0; // initialize counter
    OCR2A = (byte)32;
    TIMSK2 |= (1 << OCIE2A); // enable Compare  interrupt
    sei();
}


ISR(TIMER2_COMPA_vect)
{
    // might need t use a ADCisStable flag to ignore first reading
SCOPE_1_ON ;
if (toBeDiscarded == false) {
if (++admuxChannel >= CHANNELS) admuxChannel = 0;
ADMUX = ( B01000000 | admuxChannel);  // set to the next channel, leaving the first nybble of the register unchanged
//ADMUX |= ( B01000000 );  // set to the next channel, leaving the first nybble of the register unchanged
}
ADCSRA |= B01000000; // trigger ADC
SCOPE_1_OFF;
}

ISR(ADC_vect)
{   SCOPE_2_ON;
    uint8_t low;
    if (toBeDiscarded) {
        toBeDiscarded = false;
        low = ADCL;
        adcRawDiscarded[admuxChannel] = ADCH << 8 | low;
    } else {
    low = ADCL;
    adcRaw[admuxChannel] = ADCH << 8 | low;
    adcRawMvg[admuxChannel][mvgAvgCount[admuxChannel]] = adcRaw[admuxChannel];
    mvgAvgCount[admuxChannel]++;
   if (mvgAvgCount[admuxChannel]>(byte)MVGAVERAGESAMPLES-(uint8_t)1) mvgAvgCount[admuxChannel] = (byte)0;
    toBeDiscarded = true;
    }
    SCOPE_2_OFF;
    
}