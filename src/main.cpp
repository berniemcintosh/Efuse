#include <Arduino.h>
#include <string.h>
#include "fuseio.h"
#include "adcChannel.h"

#define IDENTIFYCHIP
void handleButton(int);
const int SHORT_PRESS_TIME = 1000; // 1000 milliseconds
const int LONG_PRESS_TIME = 2000;  // 1000 milliseconds
adcChannel chan0;
adcChannel chan1;
int i = 0;
char buffer[100];
uint16_t adcTripCurrent[4];
uint16_t adcTripTime[4];
int channelIndex;

void setup()
{

  // Place starting trip curernt and trip time valus here
  adcTripCurrent[0] = 1000; // mA
  adcTripCurrent[1] = 1000; // mA
  adcTripCurrent[2] = 1000; // mA
  adcTripCurrent[3] = 1000; // mA
  adcTripTime[0] = 100;     // ms
  adcTripTime[1] = 100;     // ms
  adcTripTime[2] = 100;     // ms
  adcTripTime[3] = 100;     // ms
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Program Startup....................................");
#ifdef IDENTIFYCHIP
  sprintf(buffer, "%02x", SIGNATURE_0);
  Serial.println(buffer);
  sprintf(buffer, "%02x", SIGNATURE_1);
  Serial.println(buffer);
  sprintf(buffer, "%02x", SIGNATURE_2);
  Serial.println(buffer);
#endif
  setupIO();
  setupADC();
  chan0.init(1, 11, NULL, &handleButton);
  chan1.init(0, 10, NULL, &handleButton);
}

void loop()
{

  chan0.handler();
  chan1.handler();
}

void handleButton(int p)
{

  sprintf(buffer, "Channel %i      ADC %i    Output %s ", p, adcRaw[p], "ON");
  Serial.println(buffer);
}