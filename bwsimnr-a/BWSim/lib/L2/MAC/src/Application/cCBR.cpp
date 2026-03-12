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
#include "../../include/Application/cCBR.h"

cCBR::cCBR()
{
    fSetSubFrameIndx(0);
    fSetSourceID(0);
    fSetDestinationId(0);
}

cCBR::cCBR(eTrafficType trafficType, string l2SupportFileName)
{
  fSetTrafficType(trafficType);
  fSetSubFrameIndx(0);
  int mPacketSize,mInterPacketDelay;
  parse(l2SupportFileName,"mPacketSize",mPacketSize);
  parse(l2SupportFileName,"mInterPacketDelay",mInterPacketDelay);
  mPktSize = mPacketSize;
  mInterPktDelay = mInterPacketDelay;
}

/*
cCBR::cCBR(eTrafficType trafficType, int tSrc, int tDest,int AppId, double frameDuration , int mPacketSize , double mInterPacketDelay, double mSimulationTime): cTrafficClass(trafficType,tSrc, tDest, AppId, mSimulationTime)
{
    mFrameDuration = sSimConfig->frameDuration;
    mPktSize = sSimConfig->cbrConfig.mPacketSize;
    mInterPktDelay = ceil_i(sSimConfig->cbrConfig.mInterPacketDelay*1e-3/mFrameDuration);
    startTime = 0;
    
    genCBRTraffic(startTime,sSimConfig->mSimulationTime);
}
*/
cCBR::cCBR(eTrafficType trafficType, int tSrc, int tDest,int AppId, double frameDuration , int mPacketSize , double mInterPacketDelay, double mSimulationTime): cTrafficClass(trafficType,tSrc, tDest, AppId, mSimulationTime)
{
    mFrameDuration = frameDuration;
    mPktSize = mPacketSize;
    mInterPktDelay = ceil_i(mInterPacketDelay*1e-3/mFrameDuration);
    startTime = 0;
    
    genCBRTraffic(startTime,mSimulationTime);
}

cCBR::cCBR(cTrafficClass &tTrafficClass): cTrafficClass(tTrafficClass.fGetSourceId(),tTrafficClass.fGetDestinationId(),tTrafficClass.fGetAppId(),tTrafficClass.fGetSimTime())
{
    fSetSubFrameIndx(tTrafficClass.fGetSubFrameIndx());

    fSetTrafficType(tTrafficClass.fGetTrafficType());
}

cCBR& cCBR::operator=(const cCBR& tCBR)
{
    this->mPktSize = tCBR.mPktSize;
    this->mInterPktDelay = tCBR.mInterPktDelay;
    this->fSetSubFrameIndx(tCBR.fGetSubFrameIndx());
    this->fSetTrafficType(tCBR.fGetTrafficType());
    this->fSetSourceID(tCBR.fGetSourceId());
    this->fSetDestinationId(tCBR.fGetDestinationId());
    this->fSetAppId(tCBR.fGetAppId());
    return *this;
}


cCBR::~cCBR()
{

}

void cCBR::genCBRTraffic(int tStartSubFrameNo, int tEndSubFrameNo)
{
    for(int start = tStartSubFrameNo;start<tEndSubFrameNo;start = start+mInterPktDelay)
    {
        generatePacket(start,mPktSize);
    }
}

// cPacketQueue cCBR::fCallTraffic(int tSubframe)
// {
//     cPacketQueue tPktQueue;
//     cPacket tPkt ;
// 
//     if((((tSubframe-startTime) % mInterPktDelay) == 0))
//     {
//         tPkt.fSetPacketSize(mPktSize);
//         tPkt.fSetPacketTimeStamp(tSubframe);
//         tPkt.fSetPktTrafficType(TRAFFIC_TYPE_CBR);
//         tPktQueue.fAddPacket(tPkt);
// #ifdef VOIP_DEBUG
//         // 	cout<<"Sub-Frame ID " <<fGetSubFrameIndx() << " App Type : " <<fGetTrafficType()<< " App ID : " <<fGetAppId()<< " Size : "<< tPktQueue.fGetSize()<<endl;
// #endif
//     }
// 
//     return tPktQueue;
// }

void cCBR::fSetPktSize(int tPktSize)
{
    mPktSize =  tPktSize;
}

int cCBR::fGetPktSize()
{
    return mPktSize;
}

void cCBR::fSetInterPktDelay(int tInterPktDelay)
{
    mInterPktDelay = tInterPktDelay;
}

int cCBR::fGetInterPktDelay()
{
    return mInterPktDelay;
}
