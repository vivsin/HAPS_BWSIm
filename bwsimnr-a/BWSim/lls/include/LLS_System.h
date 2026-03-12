/*************************************************************************
*
* CEWiT CONFIDENTIAL
* __________________
*
* All Rights Reserved © 2012 CEWiT, India
*
* NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
* and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
* Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
* express, printed and signed license for use is strictly forbidden.
*/

#ifndef _LLSSYSTEM_H_
#define _LLSSYSTEM_H_

#include "SupportingFunctions.h"

class LLS_System 
{
public:
    Array<LLSChannel> channel;
    Antenna txAntenna,rxAntenna;
    SimulationParameters simParam;
    mat ueVelocity;
public:
    LLS_System(){};
    void initSystem();
    void parseSimulationParameters(string simFile="",string AASFile = "");
    Array<Array<cvec>> generateChannelForAllAntennas(double currentTimeInSeconds,bool isTimeDomain=true,int channelIndx=0);
    Array<cmat> passThroughChannel(Array<cmat>& txGridWithCP,Array<Array<Array<cvec>>>& channelPerAntennaPerSymbol,double N0,double currentTimeInSeconds,int channelIndx=0);
    Array<cmat> multipleWithFDChannel(Array<cmat>& txGridInFrequencyDomain,Array<Array<Array<cvec>>>& channelPerAntennaPerSymbol,double N0,double currentTimeInSeconds,int channelIndx=0);
    double getDopplerSpreadInHz();
    
    
};

#endif


