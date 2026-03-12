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
#ifndef CVIDEO_H
#define CVIDEO_H

#include "cTrafficClass.h"
#include "../Others/L2SimConfig.h"

/* GOP - Group Of Picture - is collection of frames I, P and B in some predefined order.
 this number of frames and the order depends on the movie */
//eFrameType GOP2[] = {I, P, B, P, B, P, B, P, B, P, B, P, B};




enum eGammaParamsPos{
    MEAN = 0,
    VARIANCE = 1,
    SCALE_PARAM = 2,
    SHAPE_PARAM = 3,
    NUM_PARAMS = 4,
};

struct sVideoFrame 
{
    eFrameType sFrameType;	// I, B, or P frame
    int sFrameSize;
    double sFrameGenTime;	// seconds
    int startSubFrameno;
    int endSubFrameno;
};

#define VIDEO_TC_TIME_LAMBDA 666.66       // mean of t_c is 10 ms, so rate lambda = 100

class cVideo : public cTrafficClass
{
public:
    
private:
    int mUserId;			// the user id to which this instance of video class is associated.
    eFrameType mFrameType;			// I, P, or B

    int mTotalFrames;		// mVideoFramesPerSecond * simulation duration
    int mVideoPayLoad;		// = 1500 - 57; 57 = 20 + 8 + 12 + 13 + 4 (IP + UDP + RTP + TS + PES) overhead

    int mGOPSize;                   // number of frames in a GOP
    vector<eFrameType> mGOP2;               // {I, P, B, P, B, P, B, P, B, P, B, P, B};

    vector<double> mGammaParams_I;      // {3.5447e5, 7.6209e9, 16.487, 21499}
    vector<double> mGammaParams_P;      // {2.7134e5, 4.3131e9, 17.071, 15895}
    vector<double> mGammaParams_B;      // {2.2765e5, 3.3255e9, 15.584, 14608}

    int mTotalGOPs;			// ceil(mTotalFrames/mGOPSize)
    int mPktsPerVideoFrame;		// Number of packets in frame - its deterministic - 8 packet per frame.
    double mInterVideoFrameDur;	// Time between start two consequtive video frames
    int mVideoFramesPerSecond;	// Number of video frames per second - determined from the data rate of the stream (32kbps - 10fps)

    /* packet size follows pareto distribution [mean = 50 bytes, max = 125 bytes] */
    double mPktsizeDistThreshold;	// This variable holds the threshold parameter of pareto distribution.
    double mPktsizeDistIndex;		// This value holds the index parameter of pareto distribution
    double mMaxPktSize;		// If the generated value greater than this variable, the generated variable is made equal to this.

    int mCurVideoFrameIdx;
    int mCurPktIdxWithinVideoFrame;

//     int mPktToSendIndx;

//     vector <sPacketSubFrameNoPair> mPacketSubFrameNoPair;
    vector <sVideoFrame> mVideoFramesArr;

    int mTotalTrafGen;
    int mMtuSize;
    unsigned int mNumPktsInQ;
//     struct TxPktBuffer *queue;

public:

    /**
    * @brief
    *
    *
    */
    cVideo();

    /**
    * @brief
    *
    *
    */
//     cVideo(eTrafficType trafficType, int tSrc, int tDest, int tAppId, sL2SimConfig* sSimConfig);
    cVideo(eTrafficType trafficType, string l2SupportFileName, int subframeCnt);

    cVideo(cTrafficClass& tTrafficClass);

    cVideo& operator=(cVideo const& tVideo);

    ~cVideo();
    /**
    * @brief
    *
    *
    */
    double fGenParetoVar(double tThreshold, double tIndex, double max);

    /**
    * @brief
    *
    *
    */
    void fInitVideoTrafGen(int);
    void fInitVideoTrafGen(int tEndSubFrameNo,string l2SupportFileName);

    void fInitVideoTrafGen(int tEndSubFrameNo, sVideoTrafficGeneratorConfig& videoConfig) ;

    /**
    * @brief
    *
    *
    */
    void fGenVideoStream(int tStartSubFrameNo, int tEndSubFrameNo);

    void fBuildVideoFrame(int tCurSubFrameNo);

    double fGenGammaVar(double tShapeParam, double tScaleParam);

    void fPacketizeVideoStream(int tStartSubFrameNo, int tEndSubFrameNo=1e6);

    double fGenInterPacketDelay();

//     cPacketQueue fCallTraffic(int subframeNumber);
//     void fEnqueuePktToQueue(cPacketQueue *tPktQueue, int pktIndx);
    void fregen(int startTime,int subframeCnt);

    void fDisplay();
    int addPktToQueue(int tCurSubFrameNo);
};
#endif // CVIDEO_H
