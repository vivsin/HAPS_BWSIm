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
#include "../../include/Application/cVideo.h"
// #include "../../../include/ProtocolStack/Application/random_gen.h"

cVideo::cVideo() {

	fSetSubFrameIndx(0);
	fSetSourceID(0);
	fSetDestinationId(0);
}

// cVideo::cVideo(eTrafficType trafficType, int tSrc, int tDest, int tAppId, sL2SimConfig* sSimConfig): cTrafficClass(trafficType,tSrc, tDest, tAppId, sSimConfig->mSimulationTime) 
// {
//     mFrameDuration = sSimConfig->frameDuration;
//     int ueIndx = find(sSimConfig->UElist,tSrc);
//     if(ueIndx==-1)
//         ueIndx = find(sSimConfig->UElist,tDest);
//     mUserId = sSimConfig->UElist(ueIndx);
//     
//     fInitVideoTrafGen(sSimConfig->mSimulationTime, sSimConfig->videoConfig);
//     startTime = randi(1,sSimConfig->mSimulationTime);
// 
//     fGenVideoStream(startTime, sSimConfig->mSimulationTime);
//     fPacketizeVideoStream(startTime, sSimConfig->mSimulationTime);
// }


cVideo::cVideo(eTrafficType trafficType, string l2SupportFileName, int subframeCnt) {

	fSetTrafficType(trafficType); 
	fSetSubFrameIndx(0);

	if (this->fGetSubFrameIndx() == 0) {
		fInitVideoTrafGen(subframeCnt, l2SupportFileName);
		int tStart = randi(0,20);
		fGenVideoStream(tStart,subframeCnt);
		fPacketizeVideoStream(tStart,subframeCnt);
	}
}
void cVideo::fregen(int startTime,int subframeCnt)
{
	// cout<<"current size of video queue: "<<mPacketSubFrameNoPair.size()<<endl;
	mVideoFramesArr.clear();
	int tStart = randi(startTime,startTime+20);
	fGenVideoStream(tStart,tStart+subframeCnt);
	fPacketizeVideoStream(tStart,tStart+subframeCnt);
	// cout<<"final size of video queue: "<<mPacketSubFrameNoPair.size()<<endl;

}

cVideo::cVideo(cTrafficClass& tTrafficClass): cTrafficClass(tTrafficClass.fGetSourceId(), tTrafficClass.fGetDestinationId(), tTrafficClass.fGetAppId(), tTrafficClass.fGetSimTime()) {

	fSetSubFrameIndx(tTrafficClass.fGetSubFrameIndx());

	if (this->fGetSubFrameIndx() == 0) {
		fInitVideoTrafGen(tTrafficClass.fGetSimTime());

		int tStart = randi(1,20);

		fGenVideoStream(tStart, tTrafficClass.fGetSimTime());
		fPacketizeVideoStream(tStart);
	}

	fSetTrafficType(tTrafficClass.fGetTrafficType());
}

cVideo& cVideo::operator=(const cVideo& tVideo)
{

	this->fSetSubFrameIndx(tVideo.fGetSubFrameIndx());
	this->fSetTrafficType(tVideo.fGetTrafficType());
	this->fSetSourceID(tVideo.fGetSourceId());
	this->fSetDestinationId(tVideo.fGetDestinationId());
	this->fSetAppId(tVideo.fGetAppId());
	this->fSetSimTime(tVideo.fGetSimTime());

	return *this;
}

cVideo::~cVideo() {
}

void cVideo::fInitVideoTrafGen(int tEndSubFrameNo) {

	mVideoFramesPerSecond = 25;

	/* To change the GOP, change two variables (mGOP2 and mGOPSize).
mGOP2: denotes the GOP pattern
mGOPSize: denote the size of the GOP */
	mGOPSize = 13;
	eFrameType tFrm[] = {I, P, B, P, B, P, B, P, B, P, B, P, B};
	vector<eFrameType> tGOP2(tFrm, tFrm + sizeof(tFrm)/sizeof(eFrameType));
	mGOP2 = tGOP2;

	/* To change the parameter of each frame, I, P and B, change the below parameters */
	double p1[] = {3.5447e5, 7.6209e9, 16.487, 21499};
	vector <double> tParam1(p1, p1 + sizeof(p1)/sizeof(double));
	mGammaParams_I = tParam1;

	double p2[] = {2.7134e5, 4.3131e9, 17.071, 15895};
	vector <double> tParam2(p2, p2 + sizeof(p2)/sizeof(double));
	mGammaParams_P = tParam2;

	double p3[] = {2.2765e5, 3.3255e9, 15.584, 14608};
	vector <double> tParam3(p3, p3 + sizeof(p3)/sizeof(double));
	mGammaParams_B = tParam3;

	//mUserId = tUserId;
	mInterVideoFrameDur = 1.0/mVideoFramesPerSecond;
	mTotalFrames = (tEndSubFrameNo * mFrameDuration) * mVideoFramesPerSecond;
	mTotalGOPs = ceil((double)mTotalFrames/mGOPSize);
	mMtuSize = 1500;

	mVideoPayLoad = mMtuSize - 57;	// 57 = 20 + 8 + 12 + 13 + 4 (IP + UDP + RTP + TS + PES) overhead

	mTotalTrafGen = 0;
    
// 	queue = NULL;
	mNumPktsInQ = 0;
}

void cVideo::fInitVideoTrafGen(int tEndSubFrameNo,string l2SupportFileName) {
  
  std::string tBitRate;
   parse(l2SupportFileName, "mBitRate",tBitRate);
	int nVideoPayLoad=1443;		// = 1500 - 57; 57 = 20 + 8 + 12 + 13 + 4 (IP + UDP + RTP + TS + PES) overhead

        int nGOPSize=13;                   // number of frames in a GOP
	mGOP2.resize(0);
	vector<double> nGammaParams_I;      // {3.5447e5, 7.6209e9, 16.487, 21499}
        vector<double> nGammaParams_P;      // {2.7134e5, 4.3131e9, 17.071, 15895}
        vector<double> nGammaParams_B;      // {2.2765e5, 3.3255e9, 15.584, 14608}
	int nVideoFramesPerSecond=25;	// Number of video frames per second - determined from the data rate of the stream (32kbps - 10fps)
	int nPktsPerVideoFrame=8;		// Number of packets in frame - its deterministic - 8 packet per frame.
	Array<char> tGOP2("{I, P, B, P, B, P, B, P, B, P, B, P, B}"); 
	for(int cnt =0 ;cnt<tGOP2.size();cnt++)
	  {
	    if(tGOP2(cnt)=='I') 
		mGOP2.push_back(I);
	    else if(tGOP2(cnt)=='P') 	
		mGOP2.push_back(P);
	    else if(tGOP2(cnt)=='B') 
		mGOP2.push_back(B);
	     else {cout<<"[both:] Unknown GOP2 in the config File for InitL2Support...";exit(0);}
    } 
    
   if(tBitRate.compare("5.5Mbps") == 0)
   {
      Array<double> tGammaParams_I("{0.0, 0.0, 16.487, 21499}");
      for(int cnt =0 ;cnt<tGammaParams_I.size();cnt++)
      {
	  nGammaParams_I.push_back(tGammaParams_I(cnt));
      } 
      Array<double> tGammaParams_B("{0.0, 0.0, 15.584, 14608}");
      for(int cnt =0 ;cnt<tGammaParams_B.size();cnt++)
      {
	  nGammaParams_B.push_back(tGammaParams_B(cnt));
      } 
      Array<double> tGammaParams_P = ("{0.0, 0.0, 17.071, 15895}");
      for(int cnt =0 ;cnt<tGammaParams_P.size();cnt++)
      {
	  nGammaParams_P.push_back(tGammaParams_P(cnt));
      } 
   }
   else if(tBitRate.compare("1.3Mbps") == 0)
   {
      Array<double> tGammaParams_I("{0.0, 0.0, 6.468, 22905}");
      for(int cnt =0 ;cnt<tGammaParams_I.size();cnt++)
      {
	 nGammaParams_I.push_back(tGammaParams_I(cnt));
      } 
      Array<double> tGammaParams_B("{0.0, 0.0, 1.682 , 25572}");
      for(int cnt =0 ;cnt<tGammaParams_B.size();cnt++)
      {
	  nGammaParams_B.push_back(tGammaParams_B(cnt));
      } 
      Array<double> tGammaParams_P = ("{0.0, 0.0, 2.549, 26287}");
      for(int cnt =0 ;cnt<tGammaParams_P.size();cnt++)
      {
	  nGammaParams_P.push_back(tGammaParams_P(cnt));
      } 
   }
   else if(tBitRate.compare("315Kbps") == 0)
   {
      Array<double> tGammaParams_I("{0.0, 0.0, 4.406, 12235 }");
      for(int cnt =0 ;cnt<tGammaParams_I.size();cnt++)
      {
	  nGammaParams_I.push_back(tGammaParams_I(cnt));
      } 
      Array<double> tGammaParams_B("{0.0, 0.0, 0.813  , 7857}");
      for(int cnt =0 ;cnt<tGammaParams_B.size();cnt++)
      {
	  nGammaParams_B.push_back(tGammaParams_B(cnt));
      } 
      Array<double> tGammaParams_P = ("{0.0, 0.0, 1.376, 11874}");
      for(int cnt =0 ;cnt<tGammaParams_P.size();cnt++)
      {
	  nGammaParams_P.push_back(tGammaParams_P(cnt));
      } 
   }
   else
   {
     cout<<"[both:] The video Bit rate value is given wrong... Only accepted values are 5.5Mbps...1.3Mbps..or...315Kbps"<<endl; exit(0);
   }

	
	mGammaParams_B.resize(0);
	mGammaParams_I.resize(0);
	mGammaParams_P.resize(0);
	
	 mVideoPayLoad = nVideoPayLoad;		
	  mGOPSize=nGOPSize;         
/*
       
	for(int cnt = 0;cnt <(int)(nGOP2.size());cnt++)
	{
	  if(nGOP2[cnt] == sVideoTrafficGeneratorConfig::I)
	    mGOP2.push_back(I);
	  else if(nGOP2[cnt] == sVideoTrafficGeneratorConfig::P)
	    mGOP2.push_back(P);
	  else if(nGOP2[cnt] == sVideoTrafficGeneratorConfig::B)
	    mGOP2.push_back(B);
	  else
	  { cout<< "Error in GOP2 values in Video Config..."<<endl; exit(0);}
	}*/

	mGammaParams_I=nGammaParams_I;     
        mGammaParams_P=nGammaParams_P;      
        mGammaParams_B=nGammaParams_B;     
	mVideoFramesPerSecond=nVideoFramesPerSecond;	
	mPktsPerVideoFrame=nPktsPerVideoFrame;	
	  
    mVideoFramesPerSecond = 25;
    mInterVideoFrameDur = (double)((1.0/mVideoFramesPerSecond));
    
	// printf("mFrameDuration = %d \n",mFrameDuration);
	mFrameDuration = 20;
	mTotalFrames = (tEndSubFrameNo * mFrameDuration) * mVideoFramesPerSecond;
// 	cout<<" values: "<< mTotalFrames<< " "<<tEndSubFrameNo<< " "<<mFrameDuration<<" "<<mVideoFramesPerSecond<<endl;
	if (mTotalFrames < mGOPSize) {
	  mTotalGOPs =1;
	} 
	else
	{
	mTotalGOPs = ceil((double)mTotalFrames/mGOPSize);
// 	cout<<" values2: "<< mTotalGOPs<<endl;
	}
	mMtuSize = 1500;

	mVideoPayLoad = mMtuSize - 57;	// 57 = 20 + 8 + 12 + 13 + 4 (IP + UDP + RTP + TS + PES) overhead

	mTotalTrafGen = 0;
    
// 	queue = NULL;
	mNumPktsInQ = 0;
}


void cVideo::fInitVideoTrafGen(int tEndSubFrameNo, sVideoTrafficGeneratorConfig& videoConfig) {
	mGOP2.resize(0);
	mGammaParams_B.resize(0);
	mGammaParams_I.resize(0);
	mGammaParams_P.resize(0);
	
    mVideoPayLoad = videoConfig.mVideoPayLoad;		
    mGOPSize=videoConfig.mGOPSize;         

    mGOP2 = videoConfig.mGOP2;

	mGammaParams_I=videoConfig.mGammaParams_I;     
    mGammaParams_P=videoConfig.mGammaParams_P;      
    mGammaParams_B=videoConfig.mGammaParams_B;     
	mVideoFramesPerSecond=videoConfig.mVideoFramesPerSecond;	
	mPktsPerVideoFrame=videoConfig.mPktsPerVideoFrame;	
	  

	mTotalFrames = (tEndSubFrameNo * mFrameDuration) * mVideoFramesPerSecond;

    if (mTotalFrames < mGOPSize) 
    {
        mTotalGOPs =1;
	} 
	else
	{
        mTotalGOPs = ceil((double)mTotalFrames/mGOPSize);
	}
	mMtuSize = 1500;

	mVideoPayLoad = mMtuSize - 57;	// 57 = 20 + 8 + 12 + 13 + 4 (IP + UDP + RTP + TS + PES) overhead

	mTotalTrafGen = 0;

// 	queue = NULL;
	mNumPktsInQ = 0;
}

void cVideo::fGenVideoStream(int tStartSubFrameNo, int tEndSubFrameNo) 
{
	(void) tStartSubFrameNo;
	(void) tEndSubFrameNo;
	double tShapeParam=0.0, tScaleParam=0.0, tFrmSize; //WarningFix:  [-Wmaybe-uninitialized]
	sVideoFrame tVideoFrame;
	
	for (int iGOP = 0; iGOP < mTotalGOPs; iGOP++) {
		for (int iFrm = 0; iFrm < mGOPSize; iFrm++) {

			mFrameType = mGOP2[iFrm];

			if (mFrameType == I) {

				tShapeParam = mGammaParams_I[SHAPE_PARAM];
				tScaleParam = mGammaParams_I[SCALE_PARAM];

			} else if (mFrameType == P) {

				tShapeParam = mGammaParams_P[SHAPE_PARAM];
				tScaleParam = mGammaParams_P[SCALE_PARAM];

			} else if (mFrameType == B) {

				tShapeParam = mGammaParams_B[SHAPE_PARAM];
				tScaleParam = mGammaParams_B[SCALE_PARAM];
			}

			tFrmSize = gamma_rand(tShapeParam, tScaleParam);

			tFrmSize = floor(tFrmSize/8);	//converting into bytes

			tVideoFrame.sFrameType = mFrameType;
			tVideoFrame.sFrameSize = tFrmSize;
			tVideoFrame.sFrameGenTime = mVideoFramesArr.size() * mInterVideoFrameDur;
			mVideoFramesArr.push_back(tVideoFrame);
		}
	}
}

void cVideo::fPacketizeVideoStream(int tStartSubFrameNo,int tEndSubFrameNo) {

	int tStart = tStartSubFrameNo;
	int tFrmSize;
	// cout<<"tStartSubFrameNo = "<<tStartSubFrameNo<< "-- tEndSubFrameNo "<<tEndSubFrameNo<<endl;
    for(int frame_cnt=0;frame_cnt<mVideoFramesArr.size();frame_cnt++)
    {
        tFrmSize = mVideoFramesArr[frame_cnt].sFrameSize;
        mVideoFramesArr[frame_cnt].startSubFrameno = tStart;
        int nPackets = ceil_i(tFrmSize/mVideoPayLoad);
        for(int pkt_cnt=0;pkt_cnt<nPackets;pkt_cnt++)
        {
            int packetSize = (pkt_cnt == nPackets-1) ? tFrmSize - (nPackets-1)*mVideoPayLoad : mVideoPayLoad;
            generatePacket(tStart,packetSize);
            int interPktDuration = ceil_i(exp_rand(VIDEO_TC_TIME_LAMBDA)/mFrameDuration);
			tStart = tStart + interPktDuration;
            if(tStart>=tEndSubFrameNo)
                return;
        }
		//cout<<"tStart = "<<tStart<<endl;
        mVideoFramesArr[frame_cnt].endSubFrameno = tStart;
        tStart = tStart + ceil_i(mInterVideoFrameDur/mFrameDuration);
    }
/*
	for (vector<sVideoFrame>::iterator t = mVideoFramesArr.begin(); t != mVideoFramesArr.end(); t++) {
		tFrmSize = t->sFrameSize;
		t->startSubFrameno = tStart;
// 		cout << "fPacketizeVideoStream() : "<<t->sFrameSize << " " << mVideoPayLoad << endl;

		while (tFrmSize > 0) {
			
			if (tFrmSize > mVideoPayLoad) {
				tFrmSize = tFrmSize - mVideoPayLoad;
				tPacketSize = mVideoPayLoad;
			} else {
				tPacketSize = tFrmSize;
				tFrmSize = 0;
			}
            sPacketSubFrameNoPair tPacketSubFrameNoPair;
			tPacketSubFrameNoPair.subFrameNo = tStart;
			tPacketSubFrameNoPair.pktSize = tPacketSize;
			mPacketSubFrameNoPair.push_back(tPacketSubFrameNoPair);
			tStart = tStart + exp_rand(VIDEO_TC_TIME_LAMBDA)/mFrameDuration;
		} 

		t->endSubFrameno = tStart;
		tStart = tStart + mInterVideoFrameDur/mFrameDuration;
	}*/
}

void cVideo::fDisplay() {
	cout << "Frames" << endl;

	for (vector<sVideoFrame>::iterator t = mVideoFramesArr.begin(); t != mVideoFramesArr.end(); t++) {
		cout << "Frames: " << t->sFrameType << " " << t->sFrameSize << " " << t->sFrameGenTime << endl;
	}

	cout << "Packets" << endl;
	  for (vector<sPacketSubFrameNoPair>::iterator t = mPacketSubFrameNoPair.begin(); t != mPacketSubFrameNoPair.end(); t++) {
	  cout << t->subFrameNo << " "  << t->pktSize << endl;
	  }

	return;
}

// cPacketQueue cVideo::fCallTraffic(int tCurSubFrameNum) {
// 
// 	cPacketQueue tPktQueue;
// 	mNumPktsInQ = 0;
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
// void cVideo::fEnqueuePktToQueue(cPacketQueue* tPktQueue, int pktIndx)
// {
// 	cPacket tPkt;
// // 	long int tFrameIdx = fGetSubFrameIndx();
// 
// 	tPkt.fSetPacketSize(mPacketSubFrameNoPair[pktIndx].pktSize);
// 	tPkt.fSetPacketTimeStamp(mPacketSubFrameNoPair[pktIndx].subFrameNo);
// 	tPkt.fSetPktTrafficType(TRAFFIC_TYPE_VIDEO);
// 	if(tPkt.fGetPacketSize() > mVideoPayLoad)
// 	{
// 	  cout<<" [both:] Error in Video Traffic Generation"<< endl;
// 	  cout<<" [both:] Packet Size : "<< tPkt.fGetPacketSize() << endl;
// 	  exit(0);
// 	}
// 	(*tPktQueue).fAddPacket(tPkt);
// 
// 	mTotalTrafGen = mTotalTrafGen + mPacketSubFrameNoPair[pktIndx].pktSize;
// #ifdef VIDEO_DEBUG
// 	cout<<"Sub-Frame ID " <<tSubFrame << " App Type : " <<fGetTrafficType()<< " App ID : " <<fGetAppId()<< " Size : "<<tPkt.fGetPacketSize() << " num.packets: " << (*tPktQueue).fGetNumberOfPackets() << endl;
// #endif
// 	return;
// }

#if 0
int cVideo::addPktToQueue(int tCurSubFrameNum)
{
	struct TxPktBuffer* lPkt_Node = new struct TxPktBuffer;

	lPkt_Node->pkt_size = mPacketSubFrameNoPair[mPktToSendIndx].pktSize;
	//lPkt_Node->arrival_time = ftpTraffic[gUserID].pktArrTimeArray[ftpTraffic[gUserID].pktToSendIndx-1];
	lPkt_Node->arrival_time = tCurSubFrameNum;
	//lPkt_Node->pkt_status = IN_QUEUE;
	lPkt_Node->dequeued_size = 0;
	lPkt_Node->trafficType = VIDEO;
	//lPkt_Node->sessionId = numFtpSessions;
	lPkt_Node->next = NULL;

	if (queue == NULL) {

		if (mNumPktsInQ > 0) {
			cout << "Fire in the mountain. run run run..." << endl;
		}

		queue = lPkt_Node;
		mNumPktsInQ++;
	} else {

		struct TxPktBuffer* lPktEnd_Node = queue;

		while (lPktEnd_Node->next != NULL)
			lPktEnd_Node = lPktEnd_Node->next;

		lPktEnd_Node->next = lPkt_Node;
		mNumPktsInQ++;
	}

	/*TxPktBuffer *pQueue = queue;
	  int chk_cnt = 0;

	  while(pQueue) {
	  if (chk_cnt > 100) { break; }
	  chk_cnt++;
	  pQueue = pQueue->next;
	  }*/

	cout << "mNumPktsInQ: " << mNumPktsInQ << " " << mPacketSubFrameNoPair[mPktToSendIndx].pktSize << " " << mPacketSubFrameNoPair[mPktToSendIndx].subFrameNo << endl;
}
#endif
