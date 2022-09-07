//  The Main Program
#include <Arduino.h>
#include <string.h>
#include "config.h"
#include "fuseio.h"
#include "adcChannel.h"
#include "SeqButton.h"

/*
There are two button handler classes. One is the class SeqButton. This is unmodified from
 a library and is used for button presses not relating to a particular channel.
 The other is adcChannel which was modified to include all of the Efuse behaviour inside the class while
 still  handling the on-off buttons.
 */

int tripTable[TRIPLEVELS] = {TRIP1, TRIP2, TRIP3, TRIP4, TRIP5, TRIP6, TRIP7, TRIP8, TRIP9, TRIP10, TRIP11};
void handleButton(adcChannel *button);  // May not get used, but this is a callback from adcChannel
void handleButton12(SeqButton *button); // Callback from SeqButton, for the instance buttonport12, i.e. the fifth switch
void handleButton13(SeqButton *button); // Callback from SeqButton, for the instance buttonport13, i.e. the sixth switch
void pModeHandler(SeqButton *button);
void programMode(uint8_t subjChannel, uint8_t tripCurrent); // This function handles comletely prograMode and you do not leave this function until all programming is complete.
// Define all the button handlers
adcChannel channelObj[CHANNELS]; // One channel for each controlled channel
SeqButton buttonPort8;
SeqButton buttonPort9;
SeqButton buttonPort10;
SeqButton buttonPort11;
SeqButton buttonPort12;
SeqButton buttonPort13;
SeqButton progButton8;
SeqButton progButton9;
SeqButton progButton10;
SeqButton progButton11;
bool confirmed; // used in the program mode and its handler.

// some variables - global but all intended for the programming mode
uint8_t subjChannel = 255;

// Define some timers
Chrono metro500;
Chrono metro200;

buzzer myBuzzer;

// Define the IO Expander class.
PCA9555 ioport(PCA9555_ADDRESS);

// Some general purpose variables.
int i = 0;
char buffer[120];

void setup() // Runs Once
{

  ioport.begin();
  ioport.setClock(PCA9555_CLOCK);
  pinMode(9, OUTPUT); // LED on board, probably not use. Left in for historical reasons
  // initialise and flash for 2 seconds all LED
  for (uint8_t b = 0; b < 7; b++)
  {
    ioport.pinMode(b, OUTPUT);
    ioport.digitalWrite(b, HIGH);
  }

  // BUZZ_ON;
  delay(STARTUP_FLASH_TIME);
  // BUZZ_OFF;

  for (uint8_t b = 0; b < 7; b++)
    ioport.digitalWrite(b, LOW);

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
  for (int c = 0; c < CHANNELS; c++) // Initialise all the channel objects
  {
    channelObj[c].init(c, c, NULL, &handleButton);
  }
  buttonPort8.init(8, &pModeHandler);
  buttonPort9.init(9, &pModeHandler);
  buttonPort10.init(10, &pModeHandler);
  buttonPort11.init(11, &pModeHandler);
  buttonPort12.init(12, &handleButton12);
  buttonPort13.init(13, &handleButton13);
  progButton8.init(8, &pModeHandler);
  progButton9.init(9, &pModeHandler);
  progButton10.init(10, &pModeHandler);
  progButton11.init(11, &pModeHandler);
}

void loop() // repeats
{

  SCOPE_3_TOGGLE;
  for (i = 0; i < CHANNELS; i++) // need to constantly call the handlers, one after another
  {
    channelObj[i].handler();
  }
  buttonPort12.handler(); // also hanlde the extra two buttons
  buttonPort13.handler(); // also hanlde the extra two buttons
  myBuzzer.process();     // handle the buzzer
}

void handleButton(adcChannel *button) // Callback from our channel handler
{
}

void handleButton12(SeqButton *button) // Callback from 5th button hander. Turns all channels on
{
  channelObj[0].allChannels(true);
  myBuzzer.sound(100);
}

void handleButton13(SeqButton *button) // Callback from 6th button handler. Turns all channels off.
{
  channelObj[0].allChannels(false);
  // myBuzzer.sound(100);
}

void pModeHandler(SeqButton *button)
{
  // subjChannel = (button->getPin() - 8);
  confirmed = true;
}

void programMode(uint8_t subjChannel, uint8_t tripCurrent)
{
  myBuzzer.sound(20);

  subjChannel -= 8;
  Serial.print("We got a channel to program ");
  Serial.println(subjChannel);
  // Show current state in binary.
  Serial.println(tripCurrent);

  ioport.digitalWrite(3, tripCurrent & 0x01);
  ioport.digitalWrite(2, tripCurrent & 0x02);
  ioport.digitalWrite(1, tripCurrent & 0x04);
  ioport.digitalWrite(0, tripCurrent & 0x08);
  // If there is not the same  keypress for 5 seconds then quit programming mode
  confirmed = false;
  uint32_t startTime = millis();
  while (millis() < (startTime + 5000) && !confirmed)
  {
    myBuzzer.process();
    // need to change all of this to an indexed array t some point befor going live
    switch (subjChannel)
    {
    case 0:
      progButton8.handler();
      break;

    case 1:
      progButton9.handler();
      break;
    case 2:
      progButton10.handler();
      break;

    case 3:
      progButton8.handler();
      break;
    }
  }
  if (confirmed)
  { // lets go ahead and make a change
  for (int h=0; h< 5 ; h++ ){
    for (uint8_t i = 0; i < TRIPLEVELS; i++)
    {
      ioport.digitalWrite(3, i & 0x01);
      ioport.digitalWrite(2, i & 0x02);
      ioport.digitalWrite(1, i & 0x04);
      ioport.digitalWrite(0, i & 0x08);
      delay(2000);
    }
  }
  }
  else // no change - sound some error beeps
  {
    for (int g = 0; g < 3; g++)
    {
      myBuzzer.sound(20);
      delay(200);
      myBuzzer.process();
      delay(200);
    }
  }

  channelObj[0].allChannels(false);
  for (int x = 0; x < 4000; x++)
  {
    delay(1);
    myBuzzer.process();
  }
}