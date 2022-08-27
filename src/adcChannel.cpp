/*!\file adcChannel.cpp
** \author SMFSW
** \date 2018/07/15
** \copyright BSD 3-Clause License (c) 2017-2018, SMFSW
** \brief Sequential Button Arduino Library
** \details Handling filtered button press with callbacks for push (with or without repeat) and release, logic and filtering time
**/

#if ARDUINO > 22
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#if defined(DBG_adcChannel)
#include <HardwareSerial.h>
#endif

#include "adcChannel.h"
#include "fuseio.h"

#define TIME millis()

void adcChannel::init(const uint8_t channel, const uint8_t pin, void (*cbckON)(int), void (*cbckOFF)(int), const bool repeat, const bool logic, const uint32_t filter)
{
#if defined(DBG_adcChannel)
	Serial.begin(115200);
#endif
    nextFlash = false;
	tripped = false;
	outputEnabled = false;
	//pinMode(pin, INPUT_PULLUP);
	ioport.pinMode(pin+8, INPUT_PULLUP);  // change to PCA9555
	Serial.println(pin+8);
	Pin = pin+8;
	Logic = logic;
	Repeat = repeat;
	timFilter = filter;
	channelIndex = channel;
	onPush = cbckON;
	onRelease = cbckOFF;
	holdDone = false;
	pusDone = false;
	relDone = true;
	timeToTrip = 100;
	tripCurrent = 500;
	butState = HIGH;
	holdTime = 0;
	memTime = TIME;
	programMode = false;
}

void adcChannel::init(const uint8_t channel, const uint8_t pin, void (*cbckON)(int), void (*cbckOFF)(int))
{
	init(channel, pin, cbckON, cbckOFF, false, LOW, 50);
}

bool adcChannel::handler(void)
{
if (metro500.hasPassed(500,true))
    if (tripped) {
	ioport.digitalWrite(channelIndex,nextFlash);
	nextFlash = !nextFlash;	
	} 
	
if (!tripped && (adcRawAverage() > tripCurrent)) {
	// turn off mosfet when we have got to that part of the dev
tripped = true;
ioport.digitalWrite(channelIndex+4,HIGH); 
ioport.digitalWrite(channelIndex,LOW); 
outputEnabled = false;


}

//	if (digitalRead(Pin) == LOW)
   if (ioport.digitalRead(Pin) == LOW)
	{
		if ((TIME - memTime >= timFilter) || (holdDone))
		{
			relDone = false;
			butState = HIGH;
			if (!pusDone)
			{
				if (onPush) // only if callback is defined
				{

					onPush(channelIndex);
				}
			}
			if (!Repeat)
			{
				pusDone = true;
			}
			if (!holdDone)
			{
				memTime = TIME; // Store current time for holdTime calculations
				holdDone = true;
			}
			holdTime = TIME - memTime;
		}
	}
	else
	{
		holdTime = TIME - memTime; // Compute holdTime once more before memTime is re-written
		memTime = TIME;
		pusDone = false;
		holdDone = false;
		butState = LOW;
		if (!relDone)
		{
			relDone = true;
			if (programMode){displayStatus();}
			else{
				if (getHoldTime() >3000)
			{ // actions for a short button press
				programMode=true;
			}
			if (getHoldTime() < 2000)
			{ // actions for a short button press
				outputEnabled = !outputEnabled;
				ioport.digitalWrite(channelIndex+4,LOW); 
				 ioport.digitalWrite(channelIndex, outputEnabled);
				 tripped = false;
			}

			if (onRelease)
			{
				onRelease(channelIndex);
			} // only if callback is defined
            
			displayStatus();
			}
		}
	}

	return butState;
}

void adcChannel::displayStatus(void)
{

	sprintf(buffer, "Channel %i      ADC %i   MVG ADC %i     Output %s       Trip Time %i        Trip Current %i        %s ", channelIndex, adcRaw[channelIndex], adcRawAverage(), outputEnabled ? "ON" : "OFF", timeToTrip, tripCurrent, tripped ? "Tripped" : "Not Tripped");
	Serial.println(buffer);
}

uint16_t adcChannel::adcRawAverage(void)
{
	uint16_t total = 0;

	SCOPE_4_ON;
	for (int i = 0; i < MVGAVERAGESAMPLES; i++)
		total += adcRawMvg[channelIndex][i];
	// for (int i=0;i<MVGAVERAGESAMPLES;i++) {
	//	sprintf(buffer,"Value %i = %i   ",i,adcRawMvg[channelIndex][i]);
	// Serial.print(buffer);
	// }
	// Serial.println();
	SCOPE_4_OFF;
	return total / MVGAVERAGESAMPLES;
}