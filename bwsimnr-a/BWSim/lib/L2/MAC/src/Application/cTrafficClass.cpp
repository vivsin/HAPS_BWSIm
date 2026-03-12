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
#include "../../include/Application/cTrafficClass.h"

cTrafficClass::cTrafficClass()
{
    mSourceId =0;
    mDestinationId =0;
    mAppId=0;
    mSubFrameIndx=0;
    mDataToTransmit=0;
        mPktToSendIndx=0;

}
cTrafficClass::cTrafficClass(eTrafficType applicationType,int src, int dest, int tAppId, int tSimTime)
{
    mTrafficType=applicationType;
    mSourceId =src;
    mDestinationId =dest;
    mAppId=tAppId;
    mSimTime = tSimTime;
    mSubFrameIndx=0;
    mPktToSendIndx=0;
    mDataToTransmit=0;
}

cTrafficClass::cTrafficClass(int src, int dest, int tAppId, int tSimTime)
{
    mSourceId =src;
    mDestinationId =dest;
    mAppId=tAppId;
    mSimTime = tSimTime;
    mSubFrameIndx=0;
    mDataToTransmit=0;
    mPktToSendIndx=0;

}

void cTrafficClass::fSetTrafficType(eTrafficType applicationType)
{
    mTrafficType=applicationType;
}
eTrafficType cTrafficClass::fGetTrafficType(void ) const
{
    return mTrafficType;
}

void cTrafficClass::fInitialize(eTrafficType trafficType, int src, int dest)
{
    (void) trafficType;
    (void) src;
    (void) dest;
}

cTrafficClass::~cTrafficClass()
{

}

int cTrafficClass::fGetDestinationId(void ) const
{
    return mDestinationId;
}

int cTrafficClass::fGetSourceId(void ) const
{
    return mSourceId;
}

void cTrafficClass::fSetDestinationId(int dest)
{
    mDestinationId=dest;
}

void cTrafficClass::fSetSourceID(int src)
{
    mSourceId =src;
}

cPacketQueue cTrafficClass::fCallTraffic()
{
     cPacketQueue tObj; return tObj;   // Warning fix: Function body was empty
}

void cTrafficClass::fEnqueuePktToQueue(cPacketQueue* tPktQueue, int pktIndx)
{
	cPacket tPkt;

	tPkt.fSetPacketSize(mPacketSubFrameNoPair[pktIndx].pktSize);
	tPkt.fSetPacketTimeStamp(mPacketSubFrameNoPair[pktIndx].subFrameNo);
	tPkt.fSetPktTrafficType(mTrafficType);
	tPkt.fSetArrivalTime(mPacketSubFrameNoPair[pktIndx].arrivalTime);
	tPktQueue->fAddPacket(tPkt);
	return;
}

cPacketQueue cTrafficClass::fCallTraffic(int subframeNumber)
{
    cPacketQueue tPktQueue;
    for(int pktIndx = mPktToSendIndx;pktIndx<mPacketSubFrameNoPair.size();pktIndx++)
    {
        if((int)(mPacketSubFrameNoPair[pktIndx].subFrameNo) == subframeNumber)
        {
            fEnqueuePktToQueue(&tPktQueue, pktIndx);
            mPktToSendIndx++;
        }
    }
    return tPktQueue;
}

cPacketQueue cTrafficClass::fCallTraffic(double miniSlotStart, double miniSlotEnd) {
  /*
   * Since the scheduler assumes that the data is ready to be transmitted for
   * a particular mini-slot, the mini-slot boundaries passed here are assumed to
   * be of the previous mini-slot. Meaning for transmitting in mini-slot n, the
   * passed mini-slot boundaries are assumed to be of mini-slot (n-1).
   * Also the mini-slot boundaries are assumed to take of the UE PDSCh
   * processing delay as well.
   */
  cPacketQueue tPktQueue;
  double time = 0.0;
  for (int pktIndx = mPktToSendIndx; pktIndx < mPacketSubFrameNoPair.size(); pktIndx++) {
    if ((mPacketSubFrameNoPair[pktIndx].arrivalTime > miniSlotStart) and
        (mPacketSubFrameNoPair[pktIndx].arrivalTime <= miniSlotEnd)) {
      time = mPacketSubFrameNoPair[pktIndx].arrivalTime;
      fEnqueuePktToQueue(&tPktQueue, pktIndx);
      mPktToSendIndx++;
    }
  }

  // std::cout << "\033[1;31m[DEBUG]: \033[0m" << __FILE__ << ": " << __LINE__
  //           << " start =  " << miniSlotStart << " end =  " << miniSlotEnd << " arrival = " << time
  //           << " size = " << tPktQueue.fGetSize()
  //           << std::endl;
  return tPktQueue;
}

long int cTrafficClass::fGetSubFrameIndx(void ) const
{
    return mSubFrameIndx;
}
void cTrafficClass::fSetSubFrameIndx(long int tSubFrameIndx)
{
    mSubFrameIndx=tSubFrameIndx;
}

int cTrafficClass::fGetAppId() const
{
    return mAppId;
}

void cTrafficClass::fSetAppId(int tAppId)
{
    mAppId=tAppId;
}

int cTrafficClass::fGetSimTime() const
{
    return mSimTime;
}

void cTrafficClass::fSetSimTime(int tSimTime)
{
    mSimTime = tSimTime;
}

void cTrafficClass::generatePacket(int subframe,int pktSize)
{
    sPacketSubFrameNoPair tPacketSubFrameNoPair;
    tPacketSubFrameNoPair.subFrameNo = subframe;
    tPacketSubFrameNoPair.pktSize = pktSize;
    mPacketSubFrameNoPair.push_back(tPacketSubFrameNoPair);
    mDataToTransmit += pktSize;
}

void cTrafficClass::generatePacket(int subframe,int pktSize, double curTime)
{
    sPacketSubFrameNoPair tPacketSubFrameNoPair;
    tPacketSubFrameNoPair.subFrameNo = subframe;
    tPacketSubFrameNoPair.pktSize = pktSize;
    tPacketSubFrameNoPair.arrivalSubFrameNo = subframe;
    tPacketSubFrameNoPair.arrivalTime = curTime;
    mPacketSubFrameNoPair.push_back(tPacketSubFrameNoPair);
    mDataToTransmit += pktSize;
}

void cTrafficClass::print(int nSessions,int rbID)
{
    string filename = "./Results/StartTimePerUE.txt";
    std::ofstream fout;
      
    static bool isNewPrint=1;
    if(isNewPrint)
    {
        fout.open(filename.c_str(), std::ios::out);
        fout<<"%txNodeID,\t rxNodeID,\trbID,\tappID,\tappType,\t StartTime(s),\t EndTime(s),\t nPacketsToTransmit,\tnBytesToTransmit,\t nSessions\n";
        isNewPrint=false;
    }
    else
        fout.open(filename.c_str(), std::ios::app);
    
    fout<<mSourceId<<",\t"<<mDestinationId<<",\t"<<rbID<<",\t"<<mAppId<<",\t"<<getString(mTrafficType)<<",\t"<<startTime*mFrameDuration<<",\t"<<getEndTime()*mFrameDuration<<",\t"<<getNumOfPackets()<<",\t"<<mDataToTransmit<<",\t"<<nSessions<<endl;
    fout.close();
}
