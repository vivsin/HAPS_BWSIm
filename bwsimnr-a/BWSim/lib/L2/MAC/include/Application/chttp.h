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
#ifndef CHTTP_H
#define CHTTP_H

#include "cTrafficClass.h"
#include "../Others/cPacket.h"
#include "../Others/L2SimConfig.h"

#define TC_TIME_LAMBDA 20	// mean of t_c is 50 ms, so rate lambda = 20





typedef enum _eObjectType {
	MAIN = 1,
	EMBEDDED = 2,
} eObjectType;

typedef struct _sHTTPObject {
	eObjectType sType;
	int sSize;
	double sGenTime;
} sHTTPObject;

typedef struct _sHTTPPage {
	sHTTPObject sMainObject;
	int sParsingTime;
	vector <sHTTPObject> sEmbeddedObjects;
} sHTTPPage;


class cHTTP : public cTrafficClass
{

	/* Size of the main object is distributed as truncated lognormal distribution */
	double mMainObjMean;	// Mean;
	double mMainObjSD;	// Standard Deviation
	unsigned long int mMainObjMax;
	unsigned long int mMainObjMin;

	/* Size of the Embedded objects are truncated lognormal distributed */
	double mEmbeddedObjMean;		// Mean;
	double mEmbeddedObjSD;		// Standard Deviation
	unsigned long int mEmbeddedObjMax;
	unsigned long int mEmbeddedObjMin;

	/* Number of Embedded objects per page is distributed as truncated pareto */
	double mNumEmbeddedObjThreshold;
	double mNumEmbeddedObjIndex;
	unsigned int mNumEmbeddedObjMax;

	/* Reading time is distributed as Exponential distribution */
	double mReadingTimeLambda;

	/* Parsing time is distributed as Exponential distribution */
	double mParsingTimeLambda;

	/* TCTime is ditributed as Exponential distribution */
        double mTCTimeLambda;

	int mUserId;
	vector <sHTTPPage> mHTTPPages;
// 	vector <sPacketSubFrameNoPair> mPacketSubFrameNoPair;

	int mReadingTime;
	unsigned long int mCurSubFrameNum;
	unsigned int mPayLoad;
	unsigned int mTotalHeaderSize;
	int mNumPktsInQ;
// 	int mPktToSendIndx;


public:	
	unsigned long int mTotalTrafGen;

	cHTTP();
//         cHTTP(eTrafficType trafficType, int tSrc, int tDest, int tAppId, sL2SimConfig* sSimConfig);
	cHTTP(eTrafficType trafficType,string l2SupportFileName ,int subframeCnt );
	~cHTTP();

        void fInitHTTPTrafGen(sHTTPTrafficGeneratorConfig& httpConfig);
	void fInitHTTPTrafGen(long int tEndSubFrameNo,string l2SupportFileName );
	
	unsigned int fGenMTUSize();
	unsigned long int fGenReadingTime();
	unsigned long int fGenParsingTime();
	unsigned long int fGenTCTime();
	unsigned long int fGenMainObject();
	unsigned long int fGenEmbeddedObject();
	unsigned int fGenNumEmbeddedObject();
	void fGenHTTPTraffic(int tStartSubFrameNo, long int tEndSubFrameNo);
	void fGenHTTPPage();
	void fPacketize(unsigned long int tObjSize);
// 	cPacketQueue fCallTraffic(int tCurSubFrameNum);
// 	void fEnqueuePktToQueue(cPacketQueue *tPktQueue,int pktIndx);
	void fDisplay();
    int getNumOfPages(){return mHTTPPages.size();}
};

#endif
