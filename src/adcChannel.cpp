// Class for a channel and associated button handling

#include <Arduino.h>


#include "adcChannel.h"
#include "fuseio.h"
#define TIME millis()

void adcChannel::init(const uint8_t channel, const uint8_t pin, void (*cbckON)(adcChannel *), void (*cbckOFF)(adcChannel *), const bool repeat, const bool logic, const uint32_t filter)
{

	nextFlash = false;
	tripped = false;
	outputEnabled = false;
	ioport.pinMode(pin + 8, INPUT_PULLUP); 
	Serial.println(pin + 8);
	Pin = pin + 8;
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
}

void adcChannel::init(const uint8_t channel, const uint8_t pin, void (*cbckON)(adcChannel *), void (*cbckOFF)(adcChannel *))
{
	init(channel, pin, cbckON, cbckOFF, false, LOW, 50);
}

bool adcChannel::handler(void)
{
	if (metro500.hasPassed(500, true))
		if (tripped)
		{
			ioport.digitalWrite(channelIndex, nextFlash);
			nextFlash = !nextFlash;
		}

	if (!tripped && (adcRawAverage() > tripCurrent))
	{
		// turn off mosfet when we have got to that part of the dev
		tripped = true;
		// ioport.digitalWrite(5,HIGH);
		ioport.digitalWrite(channelIndex, LOW);
		outputEnabled = false;
	}

	if (ioport.digitalRead(Pin) == LOW)
	{  
		if ((TIME - memTime >= timFilter) || (holdDone)) // If we have debounced the press or we have already debounced
		{
			relDone = false; //   mark that we haven't released yet (beacause we just started officially the button press)
			butState = HIGH; // Button press has started so it must be high
			if (!pusDone)	 // if this is the first time through that we have registered the button push
			{
				if (onPush) // only if callback is defined
				{

					onPush(this); // mak the callback becasue this is the first time through for this particular button push
				}
			}
			if (!Repeat) // if we are only going to do this once for each button push
			{
				pusDone = true; // we need to set pusDone to true so next time through we don't make the callback
			}
			if (!holdDone) // if we have just kicked things off after agreeing that it is debounced
			{
				memTime = TIME;	 // Store current time for holdTime calculations
				holdDone = true; // next time hrough ew can see that we have already debounced if this is set to true
			}
			holdTime = TIME - memTime; //  regardless of if this is the first or the n'th time through, lets keep the hold time variable updated.adcRawAverage
			if (holdTime > 6000)
			{
				// turn off all ports
				allChannels(LOW);
				// and  set programming mode here
				programMode();
			}
			else if (holdTime > 3000)
			{
				if (areAllEnabled())
				{
					for (uint8_t d = 0; d < CHANNELS; d++)
						ioport.digitalWrite(d, HIGH);
				}
				else
				{
					for (uint8_t d = 0; d < CHANNELS; d++)
						ioport.digitalWrite(d, LOW);
				}
			}
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

			if (getHoldTime() > 3000)
			{ // actions for a longer button press
			  //   if all the outputs are on turn them off
				//  else
				//  turn on all outputs

				if (areAllEnabled())
					allChannels(HIGH);
				else
					allChannels(LOW);
			}
			if (getHoldTime() < 2000)
			{ // actions for a short button press
				outputEnabled = !outputEnabled;
				// ioport.digitalWrite(5,LOW);
				ioport.digitalWrite(channelIndex, outputEnabled);
				tripped = false;
			}

			if (onRelease)
			{
				onRelease(this);
			} // only if callback is defined

			displayStatus();
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

	// This may be all wrong - rentrant or not !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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

bool adcChannel::isEnabled(void)   // Returns true if the channel associated with (this) is true
{

	return outputEnabled;
}
void adcChannel::allChannels(bool switchAll)  //  Takes action on all channels
{

	for (int d = 0; d < CHANNELS; d++)
	{
		channelObj[d].switchChannel(switchAll);
	}
}
void adcChannel::switchChannel(bool switchDirection) // Control of a single channel output
{
	outputEnabled = switchDirection;
	ioport.digitalWrite(channelIndex, outputEnabled);
}

bool adcChannel::areAllEnabled()  // returns true if all outputs are enabled otherwise returns false
{
	bool allEnabled = true;
	for (int d = 0; d < CHANNELS; d++)
	{
		if (channelObj[d].isEnabled() == true)

			allEnabled = false;
	}
	return allEnabled;
}