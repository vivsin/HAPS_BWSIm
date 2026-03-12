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
#include "../../include/Application/cFTP.h"

cFTP::cFTP()
{
    fSetSubFrameIndx(0);
    fSetSourceID(0);
    fSetDestinationId(0);
}

// cFTP::cFTP(eTrafficType trafficType, int tSrc, int tDest, int tAppId, sL2SimConfig* sSimConfig, int UEId, int eNBId): cTrafficClass(trafficType,tSrc, tDest, tAppId, sSimConfig->mSimulationTime)
// {
//     fInitFtp(sSimConfig->ftpConfig,sSimConfig->frameDuration);
// 
//     double tExp;
//     int tInterArrTime;
//     tExp = exp_rand(mArrTimeLambda);
// 
//     tInterArrTime = int (tExp/mFrameDuration);
// 
//     if(sSimConfig->arrTimePereNB(eNBId) == -1)
//         sSimConfig->arrTimePereNB(eNBId) = tInterArrTime;
//     else
//         sSimConfig->arrTimePereNB(eNBId)  += tInterArrTime;
// 
//     switch(mFTPModelType)
//     {
//         case 0:
//             startTime = 0;
//             fGenFTPTraffic(startTime, sSimConfig->mSimulationTime);
//             break;
//         case 1:
//         case 2:
//         case 3:
//             startTime = sSimConfig->arrTimePereNB(eNBId);
//             fGenFTPTrafficFor3GPPModel(startTime, sSimConfig->mSimulationTime);
//             break;
//         default:
//             cout<<"Unknown mFTPModelType..."<<endl;abort();
//             break;  
//     }
// 
//     int indx=find(sSimConfig->UElist,UEId);
//     sSimConfig->startTimePerUE(indx)=sSimConfig->arrTimePereNB(eNBId);
// 
//     sSimConfig->interArrTimePerUE(indx) = tInterArrTime;
// }

cFTP::cFTP(cTrafficClass& tTrafficClass): cTrafficClass(tTrafficClass.fGetSourceId(), tTrafficClass.fGetDestinationId(), tTrafficClass.fGetAppId(), tTrafficClass.fGetSimTime())
{
    fSetSubFrameIndx(tTrafficClass.fGetSubFrameIndx());

    if(this->fGetSubFrameIndx()==0){
        fInitFtp();
        fGenFTPTraffic(0, tTrafficClass.fGetSimTime());
    }
    else{

    }

    fSetTrafficType(tTrafficClass.fGetTrafficType());
}

cFTP::cFTP(eTrafficType trafficType,string l2SupportFileName, int subframeCnt, int& arrivalTimeFTP)
{
    fSetTrafficType(trafficType);
    fSetSubFrameIndx(0);
    ivec tarTimePereNB;

    if(fGetSubFrameIndx() == 0){

        fInitFtp(l2SupportFileName);

        double tExp;
        double tArrTimeLambda = mArrTimeLambda;
        int tArrTime;

        if(mFTPModelType==0 || mFTPModelType==1)
        {
            tExp = exp_rand(tArrTimeLambda);
            int tArrTime = ceil(tExp); 

            //If I get the UE ID and the assocaited BS arrval rate.
            if(arrivalTimeFTP == -1)
                arrivalTimeFTP = tArrTime;
            else
                arrivalTimeFTP  += tArrTime;

            if(mFTPModelType == 0)
            {
                fGenFTPTraffic(0, subframeCnt);
            }
            else if(mFTPModelType == 1)
            {
                fGenFTPTrafficFor3GPPModel(arrivalTimeFTP, subframeCnt);
            }
            else
            {
                cout<<"[both:]Unsupported FTP model"<<endl;abort();
            }
        }
        else  // if mFTPModelType=2 or 3
        {
            tExp = exp_rand(tArrTimeLambda);
            tArrTime = ceil_i(tExp/mFrameDuration);
            tArrTime = 0;
            
            fGenFTPTrafficFor3GPPModel(tArrTime, subframeCnt);
            
        }

        string filename = "StartTimePerUE.txt";
        std::ofstream fout;

        static bool isNewPrint1=1;
        if(isNewPrint1)
        {
            fout.open(filename.c_str(), std::ios::out);
            fout<<"% UE ID,\t StartTime\n";
            isNewPrint1=false;
        }
        else
            fout.open(filename.c_str(), std::ios::app);

        //       fout<<UEId<<",\t" <<arrivalTimeFTP<<endl;
        //       int indx=find(sSimConfig->UElist,UEId);
        //       sSimConfig->startTimePerUE(indx)=arrivalTimeFTP;

    }
    else{

    }
}

cFTP& cFTP::operator=(const cFTP& tFTP)
{
    this->fSetSubFrameIndx(tFTP.fGetSubFrameIndx());
    this->fSetTrafficType(tFTP.fGetTrafficType());
    this->fSetSourceID(tFTP.fGetSourceId());
    this->fSetDestinationId(tFTP.fGetDestinationId());
    this->fSetAppId(tFTP.fGetAppId());


    return *this;
}

cFTP::~cFTP()
{

}
void cFTP::fregen(int subframeCnt, int& arrivalTimeFTP)
{
    // cout<<"Regenerating FTP packets for next "<<subframeCnt<<" slots, starting: "<<arrivalTimeFTP<<endl;
    // cout<<"current size of queue: "<<mPacketSubFrameNoPair.size()<<endl;
    double tExp;
    double tArrTimeLambda = mArrTimeLambda;
    int tArrTime;
    mPacketSubFrameNoPair.clear();
    if(mFTPModelType==0 || mFTPModelType==1)
    {
        tExp = exp_rand(tArrTimeLambda);
        int tArrTime = ceil(tExp); 

        //If I get the UE ID and the assocaited BS arrval rate.
        if(arrivalTimeFTP == -1)
            arrivalTimeFTP = tArrTime;
        else
            arrivalTimeFTP  += tArrTime;

        if(mFTPModelType == 0)
        {
            fGenFTPTraffic(0, arrivalTimeFTP+subframeCnt);
        }
        else if(mFTPModelType == 1)
        {
            fGenFTPTrafficFor3GPPModel(arrivalTimeFTP, subframeCnt);
        }
        else
        {
            cout<<"[both:]Unsupported FTP model"<<endl;abort();
        }
    }
    else  // if mFTPModelType=2 or 3
    {
        tExp = exp_rand(tArrTimeLambda);
        tArrTime = ceil_i(tExp/mFrameDuration);
        tArrTime = 0;        
        fGenFTPTrafficFor3GPPModel(arrivalTimeFTP+tArrTime, arrivalTimeFTP+subframeCnt);
    }
    // cout<<"final size of queue: "<<mPacketSubFrameNoPair.size()<<endl;

}

void cFTP::fInitFtp()
{
    /* constant -- start */

    mTcpHeaderLen = 0;
    mInitialCongestionWindow = 1;
    mLogNormalMean = 14.45;
    mLogNormalStandardDeviation = 0.35;
    mInterReadTimeLamba = 0.6;
    mTCTimeLambda = 20.0;
    mArrTimeLambda = 1.0;
    mChunkSize = 2;
    /* constant -- end */

    mUserState = FTP_ACTIVE;
    mFileSize = 0;
    mInterReadDuration = 0;
    mTcTime = 0;
    mWindowSize = 1;
    mMtuSize = 0;
    mNumPktPerFile = 0;
    mNumFtpSessions = 0;
    mTotalTrafGen = 0;

    //queue = NULL;
    mNumPktsInQ = 0;

    mFtpSessionStatus = FINISHED;
    mNextFtpStartFrame = randi(1,10);

    return;
}

void cFTP::fInitFtp(sFTPTrafficGeneratorConfig& ftpConfig,double frameDuration)
{
    /* constant -- start */
    mFTPModelType = ftpConfig.mFTPModelType;
    mFrameDuration = frameDuration;
    isTCPenabled = ftpConfig.isTCPEnabled;
    mWindowSize = ftpConfig.mWindowSize;

    if(mFTPModelType == 0 && isTCPenabled)
        mTcpHeaderLen = 40;
    else 
        mTcpHeaderLen = 0;

    mInitialCongestionWindow = ftpConfig.mInitialCongestionWindow;
    mLogNormalMean = ftpConfig.mLogNormalMean;
    mLogNormalStandardDeviation = ftpConfig.mLogNormalStandardDeviation;
    mMaxFileSize = ftpConfig.mMaxFileSize;
    mMinFileSize = ftpConfig.mMinFileSize;
    mInterReadTimeLamba = ftpConfig.mInterReadTimeLamba;
    mTCTimeLambda = ftpConfig.mTCTimeLambda;
    mArrTimeLambda = ftpConfig.mArrTimeLambda;
    mInterPktTimeLambda = ftpConfig.mInterPktTimeLambda;
    mChunkSize = 2;
    /* constant -- end */

    mUserState = FTP_ACTIVE;
    mFileSize = 0;
    mInterReadDuration = 0;
    mTcTime = 0;
    mWindowSize = 1;
    mMtuSize = 0;
    mNumPktPerFile = 0;
    mNumFtpSessions = 0;
    mTotalTrafGen = 0;

    mCurSubFrameNo =0;
    mNumPktsInQ = 0;

    mFtpSessionStatus = FINISHED;

    mNextFtpStartFrame = randi(1,10);

    mFileSizeType = eFtpFileSizeType(ftpConfig.mFTPFileSizeType);

    fGenFileSize((mFTPModelType==0) ? 0 : 1);

#ifdef FTP_DEBUG
    cout << "FTP file size: " << mFileSize << " " << sSimConfig->ftpConfig.mFTPFileSizeType << " " << mFileSizeType << endl;
#endif

    return;
}

void cFTP::fSessionInit()
{
    mNumPktsInQ = 0;

    mWindowSize = 1;
    mNumPktPerFile = 0;

    return;
}


void cFTP::fInitFtp(string l2SupportFileName) //firstcall
{
    /* constant -- start */
    unsigned int tInitialCongestionWindow;/**< Initial congestion window for TCP **/

    double tLogNormalMean;		/**< File is lognormal distributed. this gives the mean of this distribution **/
    double tLogNormalStandardDeviation;	/**< this gives standard deviation of the lognormal distribution **/

    double tInterReadTimeLamba;		/**< lambda value of inter read time **/
    double tTCTimeLambda;			/**< lambda value of TC time **/
    double tArrTimeLambda;		/**> lambda for User Arrival Rate **/

    /// file size can be either 'SMALL', 'MEDIUM' or 'LARGE'
    int tFTPFileSizeType;
    int tFTPModelType; ///Type can be 0, 1, 2, 3 - the last 3 correspond to 3GPP FTP models


    parse(l2SupportFileName,"mFTPModelType",tFTPModelType );
    parse(l2SupportFileName,"mFTPFileSizeType", tFTPFileSizeType);
    parse(l2SupportFileName,"mArrTimeLambda", tArrTimeLambda);
    parse(l2SupportFileName,"mInterPktTimeLambda",mInterPktTimeLambda);
    parse(l2SupportFileName, "mTCTimeLambda", tTCTimeLambda);

    parse(l2SupportFileName, "mInterReadTimeLamba",tInterReadTimeLamba);
    parse(l2SupportFileName, "mLogNormalMean",tLogNormalMean);
    parse(l2SupportFileName, "mLogNormalStandardDeviation",tLogNormalStandardDeviation);
    parse(l2SupportFileName, "mInitialCongestionWindow",tInitialCongestionWindow);

    //    sSimConfig->arrTimePereNB = -1*ones_i(sSimConfig->mNumberOfeNB);
    //    sSimConfig->UElist=tSysInitInfo.serviceNodes;
    //    sSimConfig->startTimePerUE = zeros_i(sSimConfig->UElist.size());

    mFTPModelType = tFTPModelType;
    mFrameDuration = (double) _frameDuration;

    if(mFTPModelType == 0)
        mTcpHeaderLen = 40;
    else 
        mTcpHeaderLen = 0;

    mInitialCongestionWindow = tInitialCongestionWindow;
    mLogNormalMean = tLogNormalMean;
    mLogNormalStandardDeviation = tLogNormalStandardDeviation;
    mInterReadTimeLamba = tInterReadTimeLamba;
    mTCTimeLambda = tTCTimeLambda;
    mArrTimeLambda = tArrTimeLambda;
    mChunkSize = 2;
    isTCPenabled = 0; //Hard coded as of now.
    /* constant -- end */

    mUserState = FTP_ACTIVE;
    mFileSize = 0;
    mInterReadDuration = 0;
    mTcTime = 0;
    mWindowSize = 1;
    mMtuSize = 0;
    mNumPktPerFile = 0;
    mNumFtpSessions = 0;
    mTotalTrafGen = 0;

    mCurSubFrameNo =0;
    mNumPktsInQ = 0;

    mFtpSessionStatus = FINISHED;

    mNextFtpStartFrame = randi(1,10);

    switch (tFTPFileSizeType)
    {
        case SMALL_FILE:
            mFileSizeType = SMALL_FILE;
            mFileSize = 0.5e6;     // 0.5 Mbytes 
            break;

        case MEDIUM_FILE:
            mFileSizeType = MEDIUM_FILE;
            mFileSize = 2e6;     // 2 Mbytes
            break;

        case LARGE_FILE:
            mFileSizeType = LARGE_FILE;
            mFileSize = 5e6;     // 5 Mbytes
            break;

        default:
            cout << "Value of mFTPFileSizeType is not in the set {1, 2, 3}" << endl;
            exit(-1);
    }

#ifdef FTP_DEBUG
    cout << "FTP file size: " << mFileSize << " " << sSimConfig->ftpConfig.mFTPFileSizeType << " " << mFileSizeType << endl;
#endif

    return;
}

void cFTP::fGenFileSize(bool useFixedFileSize)
{
    if(useFixedFileSize)
    {
        unsigned int mb = (unsigned int)pow(2,20);
        switch(mFileSizeType)
        {
            case SMALL_FILE:
                mFileSize = 0.5*mb;     // 0.5 Mbytes 
                break;

            case MEDIUM_FILE:
                mFileSize = 2*mb;     // 2 Mbytes
                break;

            case LARGE_FILE:
                mFileSize = 5*mb;     // 5 Mbytes
                break;

            default:
                cout << "Value of mFTPFileSizeType is not in the set {1, 2, 3}" << endl;
                abort();
        }
    }
    else
    {
        double tLN;
        do
        {
            tLN = lognormal_rand(mLogNormalMean, mLogNormalStandardDeviation);
        }
        while(tLN<mMinFileSize || tLN>mMaxFileSize);
        mFileSize = (unsigned long int) tLN;
    }
}

void cFTP::fGenInterReadDuration()
{
    double tExp;

    if(mFTPModelType != 3)
    {
        tExp = exp_rand(mInterReadTimeLamba);
    }
    else
    {
        tExp = exp_rand(mArrTimeLambda);
    }

    mInterReadDuration = ceil_i (tExp/mFrameDuration);  
    return;
}

int cFTP::fGetInterReadDuration()
{
    double tExp;
    tExp = exp_rand(mInterReadTimeLamba);
    int mInterReadDuration = ceil_i (tExp/mFrameDuration);  
    return mInterReadDuration;
}

void cFTP::fGenTCTime()
{
    double tExp;

    tExp = exp_rand(mTCTimeLambda);
    if(isTCPenabled)
    {
        mTcTime = 0/*int (tExp/mFrameDuration)*/;
    }
    else
    {
        mTcTime = ceil_i (tExp/mFrameDuration);
    }
    return;
}

void cFTP::fGenTCTimeFor3GPPModel()
{
    mTcTime = 0;

    return;
}

int cFTP::fGetInterPacketDurationFor3GPPModel()
{
    if(mFTPModelType==3)
    {
        double tExp = exp_rand(mInterPktTimeLambda);
        int mTcTime = ceil_i (tExp/mFrameDuration);
        return mTcTime;
    }
    return 0;
}

void cFTP::fGenMtuSize()
{
    mMtuSize = (randu()<0.24) ? 576 : 1500;
}

void cFTP::fGenMtuSizeFor3GPPModel()
{
    mMtuSize = 1500;
}

void cFTP::fGenPacketFromFile(int tEndSubFrameNo) {

    long int tFileSize = mFileSize;
    long int tCumTCTime = mCurSubFrameNo;

    fGenMtuSize();

    for (int iPkt = 0; iPkt < (int)mWindowSize; iPkt++) 
    {
        generatePacket(mCurSubFrameNo,mMtuSize);
        tFileSize -= (mMtuSize - mTcpHeaderLen);
        mNumPktPerFile++;
    }

    while (tFileSize > 0) 
    {

        if (tCumTCTime >= tEndSubFrameNo) 
            break;

        if ((mNumPktPerFile - 1) % mChunkSize == 0) 
        {
            fGenTCTime();
            tCumTCTime += (mCurSubFrameNo + mTcTime);  
        }

        if (tFileSize < (int)(mMtuSize - mTcpHeaderLen)) 
        {
            mMtuSize = tFileSize + mTcpHeaderLen;
            tFileSize = 0;
        } 
        else 
        {
            tFileSize -= (mMtuSize - mTcpHeaderLen);
        }

        generatePacket(tCumTCTime,mMtuSize);

        mNumPktPerFile++;
    }

    mCurSubFrameNo = tCumTCTime;
}

void cFTP::fGenPacketFromFileFor3GPPModel(int tEndSubFrameNo) 
{
    long int tFileSize = mFileSize;
    long int tCumTCTime = mCurSubFrameNo;

    int tPayLoad = fGetPacketSizeFor3GPPModel();// - mTcpHeaderLen;
    double tDataRate = mArrTimeLambda*mFileSize;
    int nPackets = ceil_i((double)tFileSize/(tPayLoad));
    for(int pkt_cnt=0;pkt_cnt<nPackets;pkt_cnt++)
    {
        int packetSize = (pkt_cnt == nPackets-1) ? tFileSize - (nPackets-1)*tPayLoad : tPayLoad;

        generatePacket(tCumTCTime,packetSize);
        mNumPktPerFile++;

        //NOTE: For Model 3, there will be a delay of inter-packet duration between adjacent packets of same file. 
        // Time gap between the start time of adjacent packets will be equal to sum of inter-packet delay and the packet transmission time. 

        int tPktTxnTime = ceil_i(packetSize/(tDataRate*mFrameDuration));
        if(mFTPModelType==3 && mNumPktPerFile!=0) // packet txn time is not added for the first packet of the session.
        {
            if(packetSize<tFileSize) // Inter packet time is not added for first and last packet. 
            {
                int tInterPktTime = fGetInterPacketDurationFor3GPPModel();
                tCumTCTime += tInterPktTime + tPktTxnTime; 
            }
            else
                tCumTCTime += tPktTxnTime; 
        }

        if(tCumTCTime>=tEndSubFrameNo)
            break;
    }

    // NOTE: For Model 2, there is no time gap between two adjacent packets, hence the next session starts at time D (inter-read time) 
    // after the current session is completed.
    // Time required to complete the current session is the file transmission time. 
    if(mFTPModelType == 2) 
    {
        int tFileTxnTime = ceil_i(mFileSize/(tDataRate*mFrameDuration));
        tCumTCTime += tFileTxnTime;
    }

    mCurSubFrameNo = tCumTCTime;
    return;
}

void cFTP::fGenFTPSession(int tEndSubFrameNo) {

    fSessionInit();

    if(mFTPModelType == 0)
        fGenPacketFromFile(tEndSubFrameNo);
    else
        fGenPacketFromFileFor3GPPModel(tEndSubFrameNo);

    return;
}

void cFTP::fGenFTPTraffic(int tStartSubFrameNo, int tEndSubFrameNo) 
{ //second call
    // cout<<"tStartSubFrameNo : "<<tStartSubFrameNo<<endl;
    mCurSubFrameNo = tStartSubFrameNo;
    // cout<<"mCurSubFrameNo = "<<mCurSubFrameNo<<" -- tEndSubFrameNo "<<tEndSubFrameNo<<endl;
    while(mCurSubFrameNo<tEndSubFrameNo)
    {
        fGenFileSize(0);
        fGenFTPSession(tEndSubFrameNo);
        if(mCurSubFrameNo<tEndSubFrameNo)
            mCurSubFrameNo += fGetInterReadDuration();
    }
}

void cFTP::fGenFTPTrafficFor3GPPModel(int tStartSubFrameNo, int tEndSubFrameNo) {

    mCurSubFrameNo = tStartSubFrameNo;

    if(mFTPModelType == 1)
    {
        fGenFTPSession(tEndSubFrameNo);
    }
    else if(mFTPModelType == 2 || mFTPModelType == 3)
    {
        while(mCurSubFrameNo < tEndSubFrameNo)
        {
            fGenFTPSession(tEndSubFrameNo);

            if (mCurSubFrameNo >= tEndSubFrameNo) 
            {
                mNumFtpSessions++;
                break;
            }

            int tInterReadDuration = fGetInterReadDuration();
            mCurSubFrameNo += tInterReadDuration;
            mNumFtpSessions++;
        }
    }
    return;
}


// cPacketQueue cFTP::fCallTraffic(int tCurSubFrameNum) {
// 
//     cPacketQueue tPktQueue;
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

// // enqueue
// void cFTP::fEnqueuePktToQueue(cPacketQueue *tPktQueue, int packetIndx)
// {
//     cPacket tPkt;
//     tPkt.fSetPacketSize(mPacketSubFrameNoPair[packetIndx].pktSize);
//     tPkt.fSetPacketTimeStamp(mPacketSubFrameNoPair[packetIndx].subFrameNo);
//     tPkt.fSetPktTrafficType(TRAFFIC_TYPE_FTP);
//     (*tPktQueue).fAddPacket(tPkt);
// 
//     mTotalTrafGen = mTotalTrafGen + mPacketSubFrameNoPair[packetIndx].pktSize;
// 
// #ifdef FTP_DEBUG
//     cout<< "Sub-Frame ID " << tCurSubFrameNum << " App Type : " << fGetTrafficType()<< " App ID : " << fGetAppId() << " Size : "<< tPkt.fGetPacketSize() << " num.packets: " << (*tPktQueue).fGetNumberOfPackets() << endl;
// #endif
// 
//     return;
// }
