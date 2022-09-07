#include <Arduino.h>
#include "SeqButton.h"

#define		TIME	millis()

buzzer::buzzer(){

}



	void buzzer::sound(uint32_t reqDuration){
		onTime = TIME;
		isOn = true;
		BUZZ_ON;
		duration = reqDuration;

	};
	void buzzer::process(void){
		if (isOn) {
			if (TIME > (onTime + duration))
			{
				BUZZ_OFF;
				isOn = false;

			}
		}

	};


void SeqButton::init(const uint8_t pin, void (*cbckON)(SeqButton*), void (*cbckOFF)(SeqButton*), const bool repeat, const bool logic, const uint32_t filter)
{
	#if defined(DBG_SEQBUTTON)
		Serial.begin(115200);
	#endif

	ioport.pinMode(pin, INPUT_PULLUP);

	Pin = pin;
	Logic = logic;
	Repeat = repeat;
	timFilter = filter;

	onPush = cbckON;
	onRelease = cbckOFF;

	holdDone = false;
	pusDone = false;
	relDone = true;

	butState = LOW;
	holdTime = 0;
	memTime = TIME;
}

void SeqButton::init(const uint8_t pin, void (*cbckON)(SeqButton*), void (*cbckOFF)(SeqButton*))
{
	init(pin, cbckON, cbckOFF, false, LOW, 50);
}

bool SeqButton::handler(void)
{
	#if defined(DBG_SEQBUTTON)
		static bool dbg = false;
	#endif

	if (ioport.digitalRead(Pin) == Logic)
	{
		if ((TIME - memTime >= timFilter) || (holdDone))
		{
			relDone = false;
			butState = HIGH;

			if (!pusDone)
			{
				if (onPush)	// only if callback is defined
				{
					#if defined(DBG_SEQBUTTON)
						Serial.print("Push\n");
						dbg = true;
					#endif
					onPush(this);
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
			if (onRelease)	{ onRelease(this); }	// only if callback is defined
			#if defined(DBG_SEQBUTTON)
				Serial.print("Release\n");
				dbg = false;
			#endif
		}
	}

	return butState;
}
