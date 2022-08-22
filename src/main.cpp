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
char buffer[100];



void setup()
{


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
  channelObj[0].init(0, 10, NULL, &handleButton);
  channelObj[1].init(1, 11, NULL, &handleButton);
  delay(100);
  
  
}

void loop()
{
for (i=0;i<CHANNELS;i++) {
 channelObj[i].handler();
}
 


}

void handleButton(int p) {

 
}