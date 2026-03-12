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
#include "../../include/Application/chttp.h"

cHTTP::cHTTP() {
    fSetSubFrameIndx(0);
    fSetSourceID(0);
    fSetDestinationId(0);
}

// cHTTP::cHTTP(eTrafficType trafficType, int tSrc, int tDest, int tAppId, sL2SimConfig* sSimConfig): cTrafficClass(trafficType,tSrc, tDest, tAppId, sSimConfig->mSimulationTime) {
// 
//     mFrameDuration = sSimConfig->frameDuration;
// 
//     fInitHTTPTrafGen(sSimConfig->httpConfig);
//     
//     startTime = round_i((min(10.0,sSimConfig->mSimulationTime*mFrameDuration)*randu())/mFrameDuration);
//     fGenHTTPTraffic(startTime, sSimConfig->mSimulationTime);
// }

cHTTP::cHTTP(eTrafficType trafficType,string l2SupportFileName , int subframeCnt){
  
    fSetSimTime(subframeCnt);
    fSetTrafficType(trafficType);
    fSetSubFrameIndx(0);

    if (this->fGetSubFrameIndx() == 0) {
        fInitHTTPTrafGen(subframeCnt, l2SupportFileName);
        startTime = randi(0,20);
        fGenHTTPTraffic(startTime, subframeCnt);
    }
    mFrameDuration = (double) _frameDuration;
}

cHTTP::~cHTTP() {
}

void cHTTP::fInitHTTPTrafGen(sHTTPTrafficGeneratorConfig& httpConfig) 
{
    mMainObjMean = httpConfig.mMainObjMean;
    mMainObjSD = httpConfig.mMainObjSD;
    mMainObjMax = httpConfig.mMainObjMax;		// 2 MegaBytes
    mMainObjMin = httpConfig.mMainObjMin;		// 100 Bytes

    mEmbeddedObjMean = httpConfig.mEmbeddedObjMean;
    mEmbeddedObjSD = httpConfig.mEmbeddedObjSD;
    mEmbeddedObjMax = httpConfig.mEmbeddedObjMax;	// 2 MegaBytes
    mEmbeddedObjMin = httpConfig.mEmbeddedObjMin;		// 50 Bytes

    mReadingTimeLambda = httpConfig.mReadingTimeLambda;
    mParsingTimeLambda = httpConfig.mParsingTimeLambda;
    mTCTimeLambda = httpConfig.mTCTimeLambda;
    
    mNumEmbeddedObjThreshold = httpConfig.mNumEmbeddedObjThreshold;
    mNumEmbeddedObjIndex = httpConfig.mNumEmbeddedObjIndex;
    mNumEmbeddedObjMax = httpConfig.mNumEmbeddedObjMax	;

    mTotalHeaderSize = 40;		//12 + 8 + 20
    mCurSubFrameNum = 0;
}

void cHTTP::fInitHTTPTrafGen(long int /*tEndSubFrameNo*/,string l2SupportFileName )
{
  int tMainObjMean,tMainObjMax,tMainObjSD,tMainObjMin;
  int tEmbeddedObjMax,tEmbeddedObjMean,tEmbeddedObjMin,tEmbeddedObjSD;
  int tNumEmbeddedObjIndex,tNumEmbeddedObjMax,tNumEmbeddedObjThreshold;
  int tReadingTimeLambda,tParsingTimeLambda,tTCTimeLambda;
  int simTime;
  
   parse(l2SupportFileName, "mMainObjMean", tMainObjMean);
   parse(l2SupportFileName, "mMainObjSD",tMainObjSD);
   parse(l2SupportFileName, "mMainObjMax",tMainObjMax);
   parse(l2SupportFileName, "mMainObjMin",tMainObjMin);
   parse(l2SupportFileName, "mEmbeddedObjMean",tEmbeddedObjMean);
   parse(l2SupportFileName, "mEmbeddedObjSD",tEmbeddedObjSD);
   parse(l2SupportFileName, "mEmbeddedObjMax",tEmbeddedObjMax);
   parse(l2SupportFileName, "mEmbeddedObjMin",tEmbeddedObjMin);
   parse(l2SupportFileName, "mNumEmbeddedObjThreshold",tNumEmbeddedObjThreshold);
   parse(l2SupportFileName, "mNumEmbeddedObjIndex",tNumEmbeddedObjIndex);
   parse(l2SupportFileName, "mNumEmbeddedObjMax",tNumEmbeddedObjMax);
   parse(l2SupportFileName, "mReadingTimeLambda",tReadingTimeLambda);
   parse(l2SupportFileName, "mParsingTimeLambda",tParsingTimeLambda);
   parse(l2SupportFileName, "mTCTimeLambda", tTCTimeLambda);
   
    mMainObjMean = tMainObjMean;
    mMainObjSD = tMainObjSD;
    mMainObjMax = tMainObjMax;		// 2 MegaBytes
    mMainObjMin = tMainObjMin;		// 100 Bytes

    mEmbeddedObjMean = tEmbeddedObjMean;
    mEmbeddedObjSD = tEmbeddedObjSD;
    mEmbeddedObjMax = tEmbeddedObjMax;	// 2 MegaBytes
    mEmbeddedObjMin = tEmbeddedObjMin;		// 50 Bytes

    mReadingTimeLambda = tReadingTimeLambda; 
    mParsingTimeLambda = tParsingTimeLambda; 
    mTCTimeLambda = tTCTimeLambda;
    mNumEmbeddedObjThreshold = tNumEmbeddedObjThreshold;
    mNumEmbeddedObjIndex = tNumEmbeddedObjIndex;
    mNumEmbeddedObjMax = tNumEmbeddedObjMax;

    mTotalHeaderSize = 40;		//12 + 8 + 20
    mCurSubFrameNum = 0;

    return;
 
}

unsigned int cHTTP::fGenMTUSize() {

    unsigned int tMTU;

    double tRand = randi(0,10000);
    if (tRand < 10000/3) {
        tMTU = 576;
    } else {
        tMTU = 1500;
    }

    return tMTU;
}

unsigned long int cHTTP::fGenReadingTime() {
    unsigned long int tReadingTime;		// in terms subframe count
    tReadingTime = (unsigned long int) (exp_rand(mReadingTimeLambda)/mFrameDuration);

    return tReadingTime;
}

unsigned long int cHTTP::fGenParsingTime() {

    unsigned long int tParsingTime;
    tParsingTime = (unsigned long int) (exp_rand(mParsingTimeLambda)/mFrameDuration);

    return tParsingTime;
}

unsigned long int cHTTP::fGenTCTime() {

    unsigned long int tTCTime;
    tTCTime = (unsigned long int) (exp_rand(mTCTimeLambda)/mFrameDuration);
    return tTCTime; //WarningFix: added return statement
}

unsigned long int cHTTP::fGenMainObject() {

    unsigned long int tMainObjectSize;
    do
    {
        tMainObjectSize = (unsigned long int) lognormal_rand(mMainObjMean, mMainObjSD);
    }
    while(tMainObjectSize>mMainObjMax || tMainObjectSize<mMainObjMin);
    
    return tMainObjectSize;
}

unsigned long int cHTTP::fGenEmbeddedObject() {

    unsigned long int tEmbeddedObjectSize;
    do
    {
        tEmbeddedObjectSize = (unsigned long int) lognormal_rand(mEmbeddedObjMean, mEmbeddedObjSD);
    }
    while(tEmbeddedObjectSize<mEmbeddedObjMin || tEmbeddedObjectSize>mEmbeddedObjMax);

    return tEmbeddedObjectSize;
}

unsigned int cHTTP::fGenNumEmbeddedObject() {

    unsigned int tNumEmbeddedObject;
    do
    {
        tNumEmbeddedObject = (unsigned int)pareto_rand(mNumEmbeddedObjThreshold, mNumEmbeddedObjIndex);
    }
    while(tNumEmbeddedObject>mNumEmbeddedObjMax);
    
    return tNumEmbeddedObject - mNumEmbeddedObjThreshold;
}

void cHTTP::fGenHTTPTraffic(int tStartSubFrameNo, long int tEndSubFrameNo)
{
    mCurSubFrameNum = tStartSubFrameNo;
    while((int)mCurSubFrameNum < tEndSubFrameNo) 
    {
        fGenHTTPPage();
        mCurSubFrameNum += fGenReadingTime();
    }
}

void cHTTP::fGenHTTPPage() {

    sHTTPPage tHTTPPage;

    sHTTPObject tMainObject;
    tMainObject.sType = MAIN;
    tMainObject.sSize = fGenMainObject();
    tHTTPPage.sMainObject = tMainObject;

    fPacketize(tHTTPPage.sMainObject.sSize);

    tHTTPPage.sParsingTime = fGenParsingTime();
    if((int)mCurSubFrameNum < fGetSimTime())
    {
        mCurSubFrameNum += (tHTTPPage.sParsingTime + fGenTCTime());
    }
    else { return ; }

    unsigned int tNumEmbeddedObject = fGenNumEmbeddedObject();

    for (int iObj = 0; iObj < (int)tNumEmbeddedObject; iObj++) 
    {
        sHTTPObject tEmbeddedObject;

        tEmbeddedObject.sType = EMBEDDED;
        tEmbeddedObject.sSize = fGenEmbeddedObject();
        tEmbeddedObject.sGenTime = mCurSubFrameNum;

        tHTTPPage.sEmbeddedObjects.push_back(tEmbeddedObject);
        fPacketize(tHTTPPage.sEmbeddedObjects[iObj].sSize);
        
        if((int)mCurSubFrameNum < fGetSimTime())
        {
            mCurSubFrameNum += fGenTCTime();
        }
        else{ return; }
    }

    mHTTPPages.push_back(tHTTPPage);
}

void cHTTP::fPacketize(unsigned long int tObjSize) {

    mPayLoad = fGenMTUSize() - mTotalHeaderSize;
    
    int nPackets = ceil_i(tObjSize/mPayLoad);
    
    for(int pkt_cnt=0;pkt_cnt<nPackets;pkt_cnt++)
    {
        int packetSize = (pkt_cnt == nPackets-1) ? tObjSize - (nPackets-1)*mPayLoad : mPayLoad;
        
        generatePacket(mCurSubFrameNum,packetSize);
        
        if((int)mCurSubFrameNum < fGetSimTime())
        {
            mCurSubFrameNum += fGenTCTime();
        }
        else
        { return;}
    }

//     while (tObjSize > 0) {
//         
//         mPayLoad = fGenMTUSize() - mTotalHeaderSize;
// 
//         if (tObjSize > mPayLoad) {
//             tObjSize = tObjSize - mPayLoad;
//             tPacketSize = mPayLoad;
//         } else {
//             tPacketSize = tObjSize;
//             tObjSize = 0;
//         }
//         
//         sPacketSubFrameNoPair tPacketSubFrameNoPair;
//         tPacketSubFrameNoPair.subFrameNo = mCurSubFrameNum;
//         tPacketSubFrameNoPair.pktSize = tPacketSize;
//         mPacketSubFrameNoPair.push_back(tPacketSubFrameNoPair);
// 	if((int)mCurSubFrameNum < mHttpSimulationTime)
//         {
//             mCurSubFrameNum += fGenTCTime();
//         }
//         else{ return ;}
//     }
}

void cHTTP::fDisplay() {

    cout << "Pages" << endl;
    for (vector<sHTTPPage>::iterator t = mHTTPPages.begin(); t != mHTTPPages.end(); t++) {
        cout << t->sMainObject.sType << " " << t->sMainObject.sSize << " " << t->sParsingTime << " " << t->sEmbeddedObjects.size() << " ";

        cout << "[";
        for (vector <sHTTPObject>::iterator tObj = t->sEmbeddedObjects.begin(); tObj != t->sEmbeddedObjects.end(); tObj++) {
            cout << "(" << tObj->sType << " " << tObj->sSize << ") ";
        }
        cout << "]" << endl;
    }

    cout << "Packets" << endl;
    for (vector<sPacketSubFrameNoPair>::iterator t = mPacketSubFrameNoPair.begin(); t != mPacketSubFrameNoPair.end(); t++) {
        cout << t->subFrameNo << " "  << t->pktSize << endl;
    }

}

// cPacketQueue cHTTP::fCallTraffic(int tCurSubFrameNum) {
// 
//     cPacketQueue tPktQueue;
// 
//     mNumPktsInQ = 0;
//     
//     for(int pktIndx = mPktToSendIndx;pktIndx<mPacketSubFrameNoPair.size();pktIndx++)
//     {
//         if((int)(mPacketSubFrameNoPair[pktIndx].subFrameNo) == tCurSubFrameNum)
//         {
//             fEnqueuePktToQueue(&tPktQueue, pktIndx);
//             mPktToSendIndx++;
//         }
//     }
//     return tPktQueue;
// }
// 
// void cHTTP::fEnqueuePktToQueue(cPacketQueue *tPktQueue,int pktIndx)
// {
//     cPacket tPkt;
// 
//     tPkt.fSetPacketSize(mPacketSubFrameNoPair[pktIndx].pktSize);
//     tPkt.fSetPacketTimeStamp(mPacketSubFrameNoPair[pktIndx].subFrameNo);
//     tPkt.fSetPktTrafficType(TRAFFIC_TYPE_HTTP);
//     (*tPktQueue).fAddPacket(tPkt);
// 
//     mTotalTrafGen = mTotalTrafGen + mPacketSubFrameNoPair[pktIndx].pktSize;
// #ifdef HTTP_DEBUG
//     cout<<"Sub-Frame ID " <<fGetSubFrameIndx() << " App Type : " <<fGetTrafficType()<< " App ID : " <<fGetAppId()<< " Size : "<<tPkt.fGetPacketSize() << " num.packets: " << (*tPktQueue).fGetNumberOfPackets() << endl;
// #endif
//     return;
// }
