/*************************************************************************
*
* CEWiT CONFIDENTIAL
* __________________
*
* All Rights Reserved © 2014 CEWiT, India
*
\ NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
* and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
* Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
* express, printed and signed license for use is strictly forbidden.
*/

#ifndef _cTimer_h
#define _cTimer_h

#include "simSupport.h"
#include "stdint.h"

enum	eTimerState	{NotYetStarted=0, Running, Expired, Stopped};




class	cTimer {
private:
	uint32_t		mStartTime;
	uint32_t		mTimerLength;
	uint32_t		mStepSize;
	eTimerState		mTimerState;
    uint32_t mStopTime;
public:
	cTimer();	//default constructor
	cTimer(uint32_t	tStartTime, uint32_t tTimerLength, uint32_t tSubFrameIndex, uint32_t tStepSize = 1);	//contains a default parameter
        void fStartTheTimer(uint32_t	tStartTime, uint32_t tTimerLength, uint32_t tSubFrameIndex, uint32_t tStepSize = 1);

        uint32_t	fTimeAfterExpiry(uint32_t tSubFrameIndex);

	//Mutator functions
			//Set functions
	void	fSetStartTime(uint32_t	tStartTime);
	void	fSetTimerLength(uint32_t tTimerLength);
	void	fSetStepSize(uint32_t tStepSize);
	void	fSetTimerState(eTimerState tTimerStatus);
        uint32_t fGetElapsedTime(uint32_t tSubFrameIndex);
			//Get functions
	uint32_t		fGetStartTime(void);
	uint32_t		fGetTimerLength(void);
	uint32_t		fGetStepSize(void);
	eTimerState		fGetTimerState(uint32_t tSubFrameIndex);

	//Additional functions
	void	fStopTheTimer(uint32_t tSubFrameIndex);

};

#endif   

/*

Timer is defined by the sub frame index "mStartTime" in which it is started, the length of the time
"mTimerLength" for which the timer will run, and the step size "mStepSize". When mStepSize = 1 then the
timer definition corresponds to the usual definition given to a timer. When mStepSize IS NOT EQUAL TO 1
then the timer will start at in the sub frame index mStartTime but will run for a duration equal to
mTimerLength*mStepSize. 

If timer starts in the sub frame index n then the timer will start just at the begining of the sub frame n.
If the timer ends in the sub frame index n then the timer will end just at the end of the nth sub frame.

A Timer is defined to be in one of the four states: 1. NotYetStarted 2. Running 3. Expired 4. Stopped
The reason to have the state "Stopped" is that a timer in the Running state may get Stopped. 
A Timer, when created, may only start either in "NotYetStarted" or "Running" states.
So we have to
make sure that Stopped state is reached only from the Running state.


*/

