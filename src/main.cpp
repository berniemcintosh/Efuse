#include <Arduino.h>
#include <string.h>
#include "config.h"
#include "fuseio.h"
#include "adcChannel.h"
#include "SeqButton.h"

// There are two button handler classes. One is the class SeqButton. This is unmodified from
// a library and is used for button presses not relating to a particular channel.
// The other is adcChannel which was modified to include all of the Efuse behaviour inside the class while handling the on-off buttons.

void handleButton(adcChannel * button); // May not get used, but this is a callback from adcChannel
void handleButton12(SeqButton * button);  // Callback from SeqButton, for the instance buttonport12
void handleButton13(SeqButton * button);  // Callback from SeqButton, for the instance buttonport12
void programMode(void);
// Define all the button handlers
adcChannel channelObj[CHANNELS];
SeqButton buttonPort12;
SeqButton buttonPort13;

// Define the IO Expander classes. This is a simple way of using one piece of code for Channel Buttons and another piece for others
PCA9555 ioport(PCA9555_ADDRESS);

// Some general purpose variables.
int i = 0;
char buffer[120];


void setup()  // Runs Once
{

  ioport.begin();
  ioport.setClock(PCA9555_CLOCK);

  // initialise and flash for 2 seconds all LED
  for (uint8_t b = 0; b < 7; b++)
  {
    ioport.pinMode(b, OUTPUT);
    ioport.digitalWrite(b, HIGH);
  }
  delay(STARTUP_FLASH_TIME);
  for (uint8_t b = 0; b < 7; b++)
    ioport.digitalWrite(b, LOW);

  pinMode(LED_BUILTIN, OUTPUT);  // LED on board, probably not use. Left in for historical reasons
  Serial.begin(9600);
  Serial.println("Program Startup....................................");
#ifdef IDENTIFYCHIP
  sprintf(buffer, "0x%02x", SIGNATURE_0);
  Serial.print(buffer);
  sprintf(buffer, "%02x", SIGNATURE_1);
  Serial.print(buffer);
  sprintf(buffer, "%02x", SIGNATURE_2);
  Serial.println(buffer);
#endif

  setupIO();
  setupADC();
  for (int c = 0; c < CHANNELS; c++)  // Initialise all the channel objects
  {
    channelObj[c].init(c, c, NULL, &handleButton);
  }
  buttonPort12.init(12, &handleButton12);
  buttonPort13.init(13, &handleButton13);
  delay(100);
}

void loop()   // repeats
{

  SCOPE_3_TOGGLE;
  for (i = 0; i < CHANNELS; i++)  // need to constantly call the handlers, one after another
  {
    channelObj[i].handler();
  }
  buttonPort12.handler(); // also hanlde the extra two buttons
  buttonPort13.handler(); // also hanlde the extra two buttons
}

void handleButton(adcChannel * button)  // Callback from our channel handler
{
}

void handleButton12(SeqButton * button)  // Callback from 5th button hander. Turns all channels on
{
  channelObj[0].allChannels(true);
}

void handleButton13(SeqButton * button) // Callback from 6th button handler. Turns all channels off.
{
  channelObj[0].allChannels(false);
}

void programMode(void) {
  
}