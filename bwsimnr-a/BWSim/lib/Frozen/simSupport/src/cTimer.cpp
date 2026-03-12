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

#include "../include/cTimer.h"


cTimer::cTimer()
{
 mStartTime=-1;
 mTimerLength=-1;
 mStepSize=-1;
 mTimerState=NotYetStarted;
 mStopTime=-1;
}

cTimer::cTimer(uint32_t	tStartTime, uint32_t tTimerLength, uint32_t tSubFrameIndex, uint32_t tStepSize) // Constructor intializing the StartTime,Timerlength,Stepsize.
{
	mStartTime = tStartTime;
	mTimerLength = tTimerLength;
	mStepSize = tStepSize;
	mStopTime=0;
	
	
	//to set the variable "mTimerState"
	if(tSubFrameIndex < mStartTime)
	{
		mTimerState = NotYetStarted;
	}
	else if((tSubFrameIndex >= mStartTime) && (tSubFrameIndex < mStartTime + (mTimerLength*mStepSize)))
	{
		mTimerState = Running;
	}
	else
	{
		mTimerState = Expired;
	}
}

void cTimer::fStartTheTimer(uint32_t	tStartTime, uint32_t tTimerLength, uint32_t tSubFrameIndex, uint32_t tStepSize)
{
        mStartTime = tStartTime;
	mTimerLength = tTimerLength;
	mStepSize = tStepSize;
	//to set the variable "mTimerState"
	if(tSubFrameIndex < mStartTime)
	{
		mTimerState = NotYetStarted;
	}
	else if((tSubFrameIndex >= mStartTime) && (tSubFrameIndex < mStartTime + (mTimerLength*mStepSize)))
	{
        //cout << "timer is running: " << endl;
		mTimerState = Running;
	}
	else
	{
		mTimerState = Expired;
	}
}

uint32_t cTimer::fTimeAfterExpiry(uint32_t tSubFrameIndex) // returns the elapsed time after expiry. This function is used to check whether the timer is expired,
{
    if(tSubFrameIndex < mStartTime)
    {
        //cout << "In file cTimer.cpp" << endl;
        //cout << "Invalid sub frame index: the timer is not yet started" << endl;
        return -1;
    }

    if((tSubFrameIndex >= mStartTime) && (tSubFrameIndex < (mStartTime + (mTimerLength*mStepSize)) - 1))
    {
        if(mTimerState == Running)
        {
            //cout << "Timer running" << endl;
        }
        return 0;
    }
         
    if(mTimerState == Stopped)
    {
        //cout << "In file cTimer.cpp" << endl;
        //cout << "The timer is already Stopped" << endl;
    }
    else
    {
        return(tSubFrameIndex - mStartTime - (mTimerLength*mStepSize) + 1);
    }
    return -1;
}



//Mutator functions
//Get functions
void cTimer::fSetStartTime(uint32_t tStartTime) // sets the startTime , this function is used as the timer restarter.
{
	mStartTime = tStartTime;
	
}

/*  Returns the time after the timer has been started. It returns a value only if the timer is running */
uint32_t cTimer::fGetElapsedTime(uint32_t tSubFrameIndex)
{
    if (fGetTimerState(tSubFrameIndex) == Running)
    {
        return (tSubFrameIndex - mStartTime);
    }
    else
    {
        //cout << "Invalid sub frame index: Elapsed time does not make sense for a Timer that is not running" << endl;
        return -1;
    }
}

void	cTimer::fSetTimerLength(uint32_t tTimerLength)
{
	mTimerLength = tTimerLength;
}


void	cTimer::fSetStepSize(uint32_t tStepSize)
{
	mStepSize = tStepSize;
}


void	cTimer::fSetTimerState(eTimerState tTimerState)
{
	mTimerState = tTimerState;
}


//Set functions
uint32_t cTimer::fGetStartTime(void)
{
	return(mStartTime);
}


uint32_t cTimer::fGetTimerLength(void)
{
	return(mTimerLength);
}

uint32_t cTimer::fGetStepSize(void)
{
	return(mStepSize);
}

/*LOGIC : If a running timer is STOPPED , mStopTime will always be greater then the mStartTime,
 *        state diagram is defined such a way that only a timer in running state can be stopped , and Stopped timer can be restarted.
 *         There is no transition between the Stopped state with  neither NotYetStarted state nor expired state .
 *         When we are restarting the timer , mStartTime  is updated with a new value ... So on timer will be shifted back to running state */

eTimerState	cTimer::fGetTimerState(uint32_t tSubFrameIndex)
{
      if((tSubFrameIndex >= mStartTime) &&  (mTimerState == Stopped ))  // Condition to evaluvate whether the timer is stopped.
      {                                                                //Variable mStopTime captures the frame at which the timer is stopped.
            if(mStopTime > mStartTime)
            {
                mTimerState = Stopped;
                return Stopped;
            }
            else
            {
                mTimerState= Running;
                return Running;
            }
       }

       if(tSubFrameIndex < mStartTime || mTimerState==NotYetStarted)
       {
            mTimerState = NotYetStarted;
            return NotYetStarted;
       }
       else if((tSubFrameIndex >= mStartTime) && (tSubFrameIndex <  mStartTime +(mTimerLength*mStepSize) ))
       {
           if(mTimerState != Stopped)
           {
               mTimerState = Running;
               return Running;
           }
       }
       else
       {
           mTimerState = Expired;
           return Expired;
       }
       return Expired;
}


//Additional functions
void cTimer::fStopTheTimer(uint32_t tSubFrameIndex)
{
  //cout << "timer status :" << mTimerState <<endl; 
  if((tSubFrameIndex >= mStartTime) && (tSubFrameIndex <  mStartTime +(mTimerLength*mStepSize) )) 
    {
        mStopTime = tSubFrameIndex;
        mTimerState = Stopped;
	cout << "timer status after stop : "<<mTimerState <<endl;
	
     }	
     else
	{
		//cout << "In file cTimer.cpp" << endl;
		//cout << "Invalid sub frame index: the timer is still running" << endl;
		
	}
	
}



/*int	main()
{
	eTimerState	x;
	uint32_t	y;
	

	for(uint32_t i = 0; i <= 100; i++)
	{
	  cout <<"********SUBFRAME"<<i<<"*******"<<endl;
	  cTimer	timeObj(10, 10 , i ,2);
	  
	  if (i == 15 )
	{
	  cout<<"TIMER STOPPED"<<endl;
	  timeObj.fStopTheTimer(15);
	 
	
	}
	
	if ( i ==  80 )
	{
	  cout<<"timer started"<<endl;
	  timeObj.fSetStartTime(80);

	}
	  
	  cout<< " Timer state :"<<timeObj.fGetTimerState(i)<<endl;
	  
	  y = timeObj.fTimeAfterExpiry(i); 
	cout << "time after expiry = " << y << endl;
	
	
	



	
	  
	}

} */
