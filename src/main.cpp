#include <Arduino.h>
#include <string.h>
#include "config.h"
#include "fuseio.h"
#include "adcChannel.h"


#define IDENTIFYCHIP
void handleButton(int);

const int SHORT_PRESS_TIME = 1000; // 1000 milliseconds
const int LONG_PRESS_TIME = 2000;  // 1000 milliseconds
adcChannel channelObj[CHANNELS];
int i = 0;
char buffer[120];


void setup()
{

  ioport.begin();
  ioport.setClock(PCA9555_CLOCK);
  // flash for 2 seconds all LED
  for (uint8_t b = 0; b < 7; b++)
  {
    ioport.pinMode(b, OUTPUT);

    ioport.digitalWrite(b, HIGH);
  }
  delay(2000);
  for (uint8_t b = 0; b < 7; b++)
  {
    ioport.pinMode(b, OUTPUT);
    ioport.digitalWrite(b, LOW);
  }
  ioport.pinMode(10, INPUT_PULLUP);



  pinMode(LED_BUILTIN, OUTPUT);
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
  channelObj[0].init(0, 0, NULL, &handleButton);
  channelObj[1].init(1, 1, NULL, &handleButton);
  delay(100);
}

void loop()
{
  SCOPE_3_TOGGLE;
  for (i = 0; i < CHANNELS; i++)
  {
    channelObj[i].handler();
  }
}

void handleButton(int p)
{
}