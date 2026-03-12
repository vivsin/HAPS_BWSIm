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
#include "../../include/Application/cVoip.h"

cVoip::cVoip()
{
fSetSubFrameIndx(0);
fSetSourceID(0);
fSetDestinationId(0);
mCount=0;
mState=fInitState();
}

cVoip::cVoip(eTrafficType trafficType)
{
 fSetTrafficType(trafficType); 
 fSetSubFrameIndx(0);
 mState=fInitState();
}

cVoip::cVoip(eTrafficType trafficType, int tSrc, int tDest, int tAppId, double mSimulationTime, double frameDuration): cTrafficClass(trafficType,tSrc, tDest, tAppId, mSimulationTime) 
{
    mState=fInitState();
    startTime = 0;
    mFrameDuration = frameDuration;
    genVoipTraffic(startTime,mSimulationTime);
}

cVoip::cVoip(cTrafficClass &tTrafficClass): cTrafficClass(tTrafficClass.fGetSourceId(),tTrafficClass.fGetDestinationId(),tTrafficClass.fGetAppId(),tTrafficClass.fGetSimTime())
{
  fSetSubFrameIndx(tTrafficClass.fGetSubFrameIndx());
   if(this->fGetSubFrameIndx()==0){
    mCount=0; 
    mState=fInitState();
  }
  else{
    mCount=this->fGetMCount();
    mState=this->fGetState();
  }
  fSetTrafficType(tTrafficClass.fGetTrafficType());
}

cVoip& cVoip::operator=(const cVoip& tVoip)
{
  this->mCount=tVoip.mCount;
  this->mState=tVoip.mState;
  this->fSetSubFrameIndx(tVoip.fGetSubFrameIndx());
  this->fSetTrafficType(tVoip.fGetTrafficType());
  this->fSetSourceID(tVoip.fGetSourceId());
  this->fSetDestinationId(tVoip.fGetDestinationId());
  this->fSetAppId(tVoip.fGetAppId());
  return *this;
}


cVoip::~cVoip()
{

}

double cVoip::fInitState()
{	
/*
	double gens = unifRand(0,1); 
	if (gens == 0) {
		int upper_limit = 1;
		int lower_limit = 0;
		double r = rand() % (upper_limit - lower_limit + 1) + lower_limit;
		int state = r;
		return state;
	}
	if (gens == 1) {
		double ustate = unifRand(0,1);
		return ustate;
	}
*/
return 1;
}

void cVoip::genVoipTraffic(int tStartSubFrameNo, int tEndSubFrameNo)
{
	// cout<<"current size of Voip queue: "<<mPacketSubFrameNoPair.size()<<endl;
    int interPktDuration = 20;
    for(int start = tStartSubFrameNo;start<tEndSubFrameNo;start = start+interPktDuration)
    {
        int packetSize=fVoipSize();
        if(packetSize>0)
        {
            generatePacket(start,packetSize);
        }
    }
	// cout<<"final size of Voip queue: "<<mPacketSubFrameNoPair.size()<<endl;
}

// cPacketQueue cVoip::fCallTraffic()
// {
//   cPacketQueue tPktQueue;
//   cPacket tPkt ;
//   long int tFrameIdx=fGetSubFrameIndx();  
//  if(!tFrameIdx) 
//   {
//     mCount=0; 
//     mState=fInitState();
//   }
//   if((tFrameIdx-startTime) % 20 ==0)
//   {
//     int tSizePkt=fVoipSize();
//     if(tSizePkt>0){
// 	    tPkt.fSetPacketSize(tSizePkt);
// 	    tPkt.fSetPacketTimeStamp(tFrameIdx);
// 	    tPkt.fSetPktTrafficType(TRAFFIC_TYPE_VOIP);
// 	    tPktQueue.fAddPacket(tPkt);
// 	#ifdef VOIP_DEBUG
// 	    cout<<"Sub-Frame ID " <<fGetSubFrameIndx() << " App Type : " <<fGetTrafficType()<< " App ID : " <<fGetAppId()<<" State : "<<mState <<" mCount " << mCount<< " Size : "<< tPktQueue.fGetPackets().size()<<endl;
// 	#endif
//     }
//   }
//   tFrameIdx++;
//   fSetSubFrameIndx(tFrameIdx);
//   return tPktQueue;
// }


double cVoip::fVoipSize() {
	double file_size;
	mState = fStateTransition();
	if (mState == 1) {
		file_size = 37.0;   /**	 40 bytes = 37 Bytes + 3 Bytes (MAC Header) */
		mCount=0;
	}
	if (mState == 0) {
		mCount++;
		if (mCount == 8) {
			file_size = 12.0;   /**	 15 bytes = 12 Bytes + 3 Bytes (MAC Header) */
			mCount = 0;
		} else {
			file_size = 0.0;
		}
	}
	
	return file_size;
}


double cVoip::fStateTransition() {
	/*char trans;
	if (randi(1,100) == 1) {
		trans = 'y';
	} else
		trans = 'n';
	
	if (trans == 'y') {
		if (mState == 1)
			mState = 0;
		else
			mState = 1;
	  }*/ 
  return (randu()<0.01) ? !mState : mState;
}

int cVoip::fGetState()
{
  return mState;
}

void cVoip::fSetState(int tState)
{
  mState=tState;
}

int cVoip::fGetMCount(){
  return mCount;
}

