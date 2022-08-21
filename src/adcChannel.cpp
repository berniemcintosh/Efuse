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

#define		TIME	millis()


void adcChannel::init(const uint8_t channel, const uint8_t pin, void (*cbckON)(int), void (*cbckOFF)(int), const bool repeat, const bool logic, const uint32_t filter)
{
	#if defined(DBG_adcChannel)
		Serial.begin(115200);
	#endif

	pinMode(pin, INPUT_PULLUP);

	Pin = pin;
	Logic = logic;
	Repeat = repeat;
	timFilter = filter;
    channelIndex = channel;
	onPush = cbckON;
	onRelease = cbckOFF;

	holdDone = false;
	pusDone = false;
	relDone = true;

	butState = LOW;
	holdTime = 0;
	memTime = TIME;
}

void adcChannel::init(const uint8_t channel, const uint8_t pin, void (*cbckON)(int), void (*cbckOFF)(int))
{
	init(channel, pin, cbckON, cbckOFF, false, LOW, 50);
}

bool adcChannel::handler(void)
{
	
	#if defined(DBG_adcChannel)
		static bool dbg = false;
	#endif

	if (digitalRead(Pin) == Logic)
	{
		if ((TIME - memTime >= timFilter) || (holdDone))
		{
			relDone = false;
			butState = HIGH;

			if (!pusDone)
			{
				if (onPush)	// only if callback is defined
				{
					#if defined(DBG_adcChannel)
						Serial.print("Push\n");
						dbg = true;
					#endif
					onPush(channelIndex);
				}
			}

			if (!Repeat)	{ pusDone = true; }

			if (!holdDone)
			{
				memTime = TIME;		// Store current time for holdTime calculations
				holdDone = true;
			}

			holdTime = TIME - memTime;
		}
	}
	else
	{
		holdTime = TIME - memTime;	// Compute holdTime once more before memTime is re-written
		memTime = TIME;
		pusDone = false;
		holdDone = false;
		butState = LOW;

		if (!relDone)
		{
			relDone = true;
			if (onRelease)	{ onRelease(channelIndex); }	// only if callback is defined
			#if defined(DBG_adcChannel)
				Serial.print("Release\n");
				dbg = false;
			#endif
		}
	}

	return butState;
}
