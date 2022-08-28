/*!\file adcChannel.h
** \author SMFSW
** \date 2018/07/15
** \copyright BSD 3-Clause License (c) 2017-2018, SMFSW
** \brief Sequential Button Arduino Library
** \details Handling filtered button press with callbacks for push (with or without repeat) and release, logic and filtering time
**/

#ifndef adcChannel_h
#define adcChannel_h
#include "config.h"
#include <inttypes.h>
#include "clsPCA9555.h"
#include <Chrono.h>

//#define		DBG_adcChannel
extern char buffer[120];
extern PCA9555 ioport;
extern void programMode(void);



/*!	\class adcChannel adcChannel.h "adcChannel/adcChannel.h"
**	\brief Class containing the required methods for handling sequential button
**/


extern volatile uint16_t adcRaw[CHANNELS];
extern volatile uint16_t adcRawMvg[CHANNELS][MVGAVERAGESAMPLES];

class adcChannel
{
private:
bool nextFlash;
    Chrono metro500;
    bool outputEnabled;
	volatile bool tripped;
	
	bool Logic;			//!< Button logic: LOW for NO / HIGH for NC (internal pullup for input is enabled)
	bool Repeat;		//!< Push callback repeated calls
	bool butState;		  //!< Memorized button state
	uint32_t memTime;	  //!< Previously recorded timer
	uint32_t holdTime;	  //!< Time button held
	uint32_t timFilter; //!< Filtering time (in ms)	
	uint16_t timeToTrip;
	uint16_t tripCurrent;
	uint8_t holdDone : 1; //!< Time for button hold already memorized
	uint8_t pusDone : 1;  //!< ON function already called
	uint8_t Pin;		//!< Pin on which button is connected
	uint8_t relDone : 1;  //!< OFF function already called
	uint8_t channelIndex;
	void (*onPush)(adcChannel *);	//!< Push callback ON function pointer
	void (*onRelease)(adcChannel *); //!< Push callback OFF function pointer
	void displayStatus (void);
	uint16_t adcRawAverage (void);

public:

	/*!	\brief Initialization routine
	**	\note Input pin is configured with device internal pullup
	**	\param [in] pin - Pin on which button is connected
	**	\param [in] repeat - callback called repeatedly when set to true
	**	\param [in] logic - Button logic: LOW for NO / HIGH for NC (internal pullup for input is enabled)
	**	\param [in] filter - filtering time (in ms)
	**	\param [in] cbckON - callback function on press (NULL when no callback needed)
	**	\param [in] cbckOFF - callback function on release (NULL when no callback needed)
	**	\note Callbacks have to be declared using an adcChannel* as parameter (instance is passed to the callback function)
	**	\return nothing
	**/
	//void init(const uint8_t channel, const uint8_t pin, void (*cbckON)(int p), void (*cbckOFF)(int p), const bool repeat, const bool logic = HIGH, const uint32_t filter = 50);
      void init(const uint8_t channel, const uint8_t pin, void (*cbckON)(adcChannel*), void (*cbckOFF)(adcChannel*), const bool repeat, const bool logic=LOW, const uint32_t filter=50);
	/*!	\brief Initialization routine
	**	\note Input pin is configured with device internal pullup
	**	\param [in] pin - Pin on which button is connected
	**	\param [in] cbckON - callback function on press (NULL when no callback needed)
	**	\param [in] cbckOFF - callback function on release (NULL when no callback needed)
	**	\note Callbacks have to be declared using an adcChannel* as parameter (instance is passed to the callback function)
	**	\return nothing
	**/
	//void init(const uint8_t channel, uint8_t pin, void (*cbckON)(int p), void (*cbckOFF)(int p) = NULL);
	  void init(const uint8_t channel, uint8_t pin, void (*cbckON)(adcChannel*), void (*cbckOFF)(adcChannel*)=NULL);

	/*!	\brief Check button state and perform callbacks accordingly
	**	\note handler for button, this function has to be called in a pseudo main loop to work properly
	**	\return Current button state
	**/
    bool isEnabled(void);
	void allChannels(bool);
	void switchChannel(bool);
	bool areAllEnabled(void);
	bool handler(void);
	

	/*!	\brief Get button pin
	**	\return Current pin assigned to instance
	**/
	uint8_t getPin(void) __attribute__((always_inline))
	{
		return Pin;
	}

	/*!	\brief Get button state
	**	\return Current button state
	**/
	bool getState(void) __attribute__((always_inline))
	{
		return butState;
	}

	/*!	\brief Get button state
	**	\deprecated getButton was already used in adcChannel lib, alias is made to keep compatibility with earlier versions
	**	\return Current button state
	**/
	bool getButton(void) __attribute__((always_inline))
	{
		return getState();
	}

	/*!	\brief Get button hold duration
	**	\return Current button hold duration
	**/
	uint32_t getHoldTime(void) __attribute__((always_inline))
	{
		return holdTime;
	}
};


extern adcChannel channelObj[];


#endif /* adcChannel_h */