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
#include "../../include/Others/cRadioBearer.h"
// #include "../../../include/Sim/cltel2sim.h"  
// #include "../../../include/ProtocolStack/L2/MAC/cLogicalChannel.h"

Application::Application()
{
    voip = NULL;ftp = NULL;http = NULL;video = NULL;cbr = NULL;trafficType = TRAFFIC_TYPE_UNINIT;
}

void Application::init(sL2SimConfig* l2SimConfig,eTrafficType traffic,int appID,int txNode,int rxNode,bool isDownlink)
{
    int bsID = (isDownlink) ? txNode : rxNode;
    int ueID = (isDownlink) ? rxNode : txNode;
    trafficType = traffic;
    int arrTime = -1;
    switch(trafficType)
    {
        case TRAFFIC_TYPE_VOIP:
            voip = new cVoip(trafficType,txNode,rxNode,appID,l2SimConfig->mSimulationTime,l2SimConfig->frameDuration);
            voip->fSetSubFrameIndx(-1);
            break;
        case TRAFFIC_TYPE_FTP: 
            
            ftp = new cFTP(TRAFFIC_TYPE_FTP,l2SimConfig->l2SupportConfigFile,l2SimConfig->mSimulationTime,arrTime);
            ftp->fSetSubFrameIndx(-1);
            break;
//         case TRAFFIC_TYPE_HTTP:
//             http = new cHTTP(trafficType,txNode,rxNode,appID,l2SimConfig);
//             http->fSetSubFrameIndx(-1);
//             break;
        case TRAFFIC_TYPE_VIDEO:
            video = new cVideo(TRAFFIC_TYPE_VIDEO,l2SimConfig->l2SupportConfigFile, l2SimConfig->mSimulationTime);
            video->fSetSubFrameIndx(-1);
            break;
//         case TRAFFIC_TYPE_CBR:
//             cbr = new cCBR(trafficType,txNode,rxNode,appID,l2SimConfig);
//             cbr->fSetSubFrameIndx(-1);
//             break;
	case TRAFFIC_TYPE_XR:
            xr = new cXR(trafficType,txNode,rxNode,appID,l2SimConfig);
            xr->fSetSubFrameIndx(-1);
            break;
        case TRAFFIC_TYPE_INFINITE_BUFFER:
            break;
        default:
            cout<<"Unknown Traffic Model..."<<endl;abort();
            break;
    }
}

int Application::getPktQueueSize(int subframeNumber)
{
    int prevSubframeIndx = 0,bufferSizeInBytes=0;
    switch(trafficType)
    {
        case TRAFFIC_TYPE_VOIP:
            prevSubframeIndx=(int)voip->fGetSubFrameIndx();
            for(int cnt=prevSubframeIndx+1;cnt<=subframeNumber;cnt++)
            {
                cPacketQueue pktQueue=voip->fCallTraffic(cnt);
                bufferSizeInBytes+=pktQueue.fGetSize();
                voip->fSetSubFrameIndx(cnt);
            }
        break;
        case TRAFFIC_TYPE_FTP:
            prevSubframeIndx=ftp->fGetSubFrameIndx();
            for(int cnt=prevSubframeIndx+1;cnt<=subframeNumber;cnt++)
            {
                cPacketQueue pktQueue=ftp->fCallTraffic(cnt);
                bufferSizeInBytes+=pktQueue.fGetSize();
                ftp->fSetSubFrameIndx(cnt);
            }
        break;
        case TRAFFIC_TYPE_HTTP:
            prevSubframeIndx=http->fGetSubFrameIndx();
            for(int cnt=prevSubframeIndx+1;cnt<=subframeNumber;cnt++)
            {
                cPacketQueue pktQueue=http->fCallTraffic(cnt);
                bufferSizeInBytes+=pktQueue.fGetSize();
                http->fSetSubFrameIndx(cnt);
            }
        break;
        case TRAFFIC_TYPE_VIDEO:
            prevSubframeIndx=video->fGetSubFrameIndx();
            for(int cnt=prevSubframeIndx+1;cnt<=subframeNumber;cnt++)
            {
                cPacketQueue pktQueue=video->fCallTraffic(cnt);
                bufferSizeInBytes+=pktQueue.fGetSize();
                video->fSetSubFrameIndx(cnt);
            }
        break;
        case TRAFFIC_TYPE_CBR:
            prevSubframeIndx=cbr->fGetSubFrameIndx();
            for(int cnt=prevSubframeIndx+1;cnt<=subframeNumber;cnt++)
            {
                cPacketQueue pktQueue=cbr->fCallTraffic(cnt);
                bufferSizeInBytes+=pktQueue.fGetSize();
                cbr->fSetSubFrameIndx(cnt);
            }
        break;
        case TRAFFIC_TYPE_XR:
            prevSubframeIndx=xr->fGetSubFrameIndx();
            for(int cnt=prevSubframeIndx+1;cnt<=subframeNumber;cnt++)
            {
                cPacketQueue pktQueue=xr->fCallTraffic(cnt);
                bufferSizeInBytes+=pktQueue.fGetSize();
                xr->fSetSubFrameIndx(cnt);
            }
        break;
        case TRAFFIC_TYPE_INFINITE_BUFFER:
            bufferSizeInBytes += 1000000;
        break;
        default:
            cout<<"Unknown Traffic Model.."<<endl;abort();
        break;
    }
    return bufferSizeInBytes;
}

int Application::getPktQueueSize(double miniSlotStart, double miniSlotEnd, bool isSubframeEnd) {
  int prevSubframeIndx = 0,bufferSizeInBytes=0;

  if (trafficType != TRAFFIC_TYPE_XR) {
    std::cout << "Mini-slot implemented only for XR Traffic." << endl;
    abort();
  }

  cPacketQueue pktQueue=xr->fCallTraffic(miniSlotStart, miniSlotEnd);
  bufferSizeInBytes+=pktQueue.fGetSize();

  prevSubframeIndx=xr->fGetSubFrameIndx();
  if (isSubframeEnd)
    xr->fSetSubFrameIndx(prevSubframeIndx+1);


  // std::cout << "\033[1;31m[DEBUG]: \033[0m" << __FILE__ << ": " << __LINE__
  //           << " start =  " << miniSlotStart << " end =  " << miniSlotEnd
  //           << " size = " << bufferSizeInBytes
  //           << std::endl;
  return bufferSizeInBytes;
}

void Application::printStartTime(int rbID)
{
    switch(trafficType)
    {
        case TRAFFIC_TYPE_VOIP:
            voip->print(voip->fGetMCount(),rbID);
            break;
        case TRAFFIC_TYPE_FTP: 
            ftp->print(ftp->getNumOfSessions(),rbID);
            break;
        case TRAFFIC_TYPE_HTTP:
            http->print(http->getNumOfPages(),rbID);
            break;
        case TRAFFIC_TYPE_VIDEO:
            video->print(1,rbID);
            break;
        case TRAFFIC_TYPE_CBR:
            cbr->print(1,rbID);
            break;
        case TRAFFIC_TYPE_XR:
            xr->print(1,rbID);
            break;
        case TRAFFIC_TYPE_INFINITE_BUFFER:
            break;
        default:
            cout<<"Unknown Traffic Model..."<<endl;abort();
            break;
    }
}
    
cRadioBearer::cRadioBearer()
{
    mDestinationId=0;
    mSourceId=0;
    mNumberOfApp=0;

    mDirection = 0;

    mRBPktQueue=cPacketQueue();
    mIsInfiniteBuffer=false;
    mVoipTraffic.resize(0);
    mFTPTraffic.resize(0);
    mHTTPTraffic.resize(0);
    mVideoTraffic.resize(0);
    mCBRTraffic.resize(0);
    mXRTraffic.resize(0);
}


// cRadioBearer::cRadioBearer(int dest, int src, int RBId, int direction, sLCConfigInfo tLCConfigInfo, sL2SimConfig* sSimConfig)
// {
//     mTransmittedBits=0;
//     mIsInfiniteBuffer=false;
//     mRBID=RBId;
//     mDestinationId=dest;
//     mSourceId=src;
//     mDirection = direction;
// 
// 
// //     if(sSimConfig->mRLCMode[RBId -1] ==0 )
// //     {
// //         cRLC *tRLC=new cRLCtm(mSourceId,mDestinationId,cRLC::TM_RLC_MODE); /** Ambiguous if MAC request for PDU > Packet size in RLC Queue for TM Mode.**/
// //         fSetRLC(tRLC);
// //     }
// //     else if (sSimConfig->mRLCMode[RBId -1] ==1 )
// //     {
// //         cRLC *tRLC=new cRLCum(mSourceId,mDestinationId,cRLC::UM_RLC_MODE, sSimConfig); /** Ambiguous if MAC request for PDU > Packet size in RLC Queue for TM Mode.**/
// //         fSetRLC(tRLC);
// //     }
// //     else if (sSimConfig->mRLCMode[RBId -1] ==2 )
// //     {
// //         cRLC *tRLC=new cRLCam(mSourceId,mDestinationId,cRLC::AM_RLC_MODE, sSimConfig); /** Ambiguous if MAC request for PDU > Packet size in RLC Queue for TM Mode.**/
// //         fSetRLC(tRLC);
// //     }
// //     else
// //     {
// //         cout<<"[both:] RadioBearer Constructor: Error in RLC Mode Type" <<endl ;
// //         exit(-1);
// //     }
// 
// //     fSetPDCP(cPDCP(mSourceId, mDestinationId));
// 
//     int tNumApp=sSimConfig->mNumberOfAppPerRB[RBId];
//     vector< vector<eTrafficType> > tAppType =sSimConfig->mTrafficType;
// 
//     for(int AppId=0;AppId<tNumApp;AppId++){
//         if(tAppType[RBId-1][AppId]==TRAFFIC_TYPE_VOIP){
//             mVoipTraffic.push_back(cVoip(tAppType[RBId-1][AppId]));
//         }
//         else if(tAppType[RBId-1][AppId]==TRAFFIC_TYPE_FTP){
//             mFTPTraffic.push_back(cFTP(tAppType[RBId-1][AppId],mSourceId,mDestinationId,AppId+1, sSimConfig));
//             cVideo(TRAFFIC_TYPE_VIDEO,"myL2Support.txt", TOTAL_ITERS);
//         }
//         else if(tAppType[RBId-1][AppId]==TRAFFIC_TYPE_HTTP){
//             mHTTPTraffic.push_back(cHTTP(tAppType[RBId-1][AppId],mSourceId,mDestinationId,AppId+1, sSimConfig));
//         }
//         else if(tAppType[RBId-1][AppId]==TRAFFIC_TYPE_VIDEO){
//             mVideoTraffic.push_back(cVideo(tAppType[RBId-1][AppId],mSourceId,mDestinationId,AppId+1, sSimConfig));
//         }
//         else if(tAppType[RBId-1][AppId]==TRAFFIC_TYPE_CBR){
//             mCBRTraffic.push_back(cCBR(tAppType[RBId-1][AppId],mSourceId,mDestinationId,AppId+1, sSimConfig));
//         }
//         else if(tAppType[RBId-1][AppId]==TRAFFIC_TYPE_INFINITE_BUFFER)
//         {
//             mIsInfiniteBuffer=true;
//         }
//         else{
//             cout<<"[both:] RadioBearer Constructor: Error in Traffic Type" <<endl ;
//             exit(-1);
//         }
//     }
// 
// //     fSetLogicalChannel(cLogicalChannel(tLCConfigInfo));
// }

cRadioBearer::cRadioBearer(const cRadioBearer& tRB)
{
    mDestinationId=tRB.mDestinationId;
    mSourceId=tRB.mSourceId;
    mIsInfiniteBuffer=tRB.mIsInfiniteBuffer;
    mRBID=tRB.mRBID;
    mNumberOfApp=tRB.mNumberOfApp;
    mTransmittedBits=tRB.mTransmittedBits;
//     mPDCP = tRB.mPDCP;
//     mRLC = tRB.fGetRLC();
    mRBPktQueue = tRB.mRBPktQueue;

    mDirection = tRB.mDirection;

    mFTPTraffic.resize(0);
    mVoipTraffic.resize(0);
    mHTTPTraffic.resize(0);
    mVideoTraffic.resize(0);
    mCBRTraffic.resize(0);
    mXRTraffic.resize(0);

    for(int i=0;i<(int)(tRB.mFTPTraffic.size());i++)
    {
        mFTPTraffic.push_back(tRB.mFTPTraffic[i]);
    }
    for(int i= 0;i<(int)(tRB.mVoipTraffic.size());i++)
    {
        mVoipTraffic.push_back(tRB.mVoipTraffic[i]);
    }
    for(int i= 0;i<(int)(tRB.mHTTPTraffic.size());i++)
    {
        mHTTPTraffic.push_back(tRB.mHTTPTraffic[i]);
    }
    for(int i= 0;i<(int)(tRB.mVideoTraffic.size());i++)
    {
        mVideoTraffic.push_back(tRB.mVideoTraffic[i]);
    }
    for(int i= 0;i<(int)(tRB.mCBRTraffic.size());i++)
    {
        mCBRTraffic.push_back(tRB.mCBRTraffic[i]);
    }
    for(int i= 0;i<(int)(tRB.mXRTraffic.size());i++)
    {
        mXRTraffic.push_back(tRB.mXRTraffic[i]);
    }
}

cRadioBearer& cRadioBearer::operator=(const cRadioBearer& tRB)
{
    this->mDestinationId=tRB.mDestinationId;
    this->mSourceId=tRB.mSourceId;
    this->mIsInfiniteBuffer=tRB.mIsInfiniteBuffer;
    this->mRBID=tRB.mRBID;
    this->mNumberOfApp=tRB.mNumberOfApp;
    this->mTransmittedBits=tRB.mTransmittedBits;
//     this->mPDCP = tRB.mPDCP;
//     this->mRLC = tRB.fGetRLC();
    this->mRBPktQueue = tRB.mRBPktQueue;

    this->mDirection = tRB.mDirection;

    this->mFTPTraffic.resize(0);
    this->mVoipTraffic.resize(0);
    this->mHTTPTraffic.resize(0);
    this->mVideoTraffic.resize(0);
    this->mCBRTraffic.resize(0);
    this->mXRTraffic.resize(0);
    for(int i=0;i<(int)(tRB.mFTPTraffic.size());i++)
    {
        this->mFTPTraffic.push_back(tRB.mFTPTraffic[i]);
    }
    for(int i= 0;i<(int)(tRB.mVoipTraffic.size());i++)
    {
        this->mVoipTraffic.push_back(tRB.mVoipTraffic[i]);
    }
    for(int i= 0;i<(int)(tRB.mVoipTraffic.size());i++)
    {
        this->mHTTPTraffic.push_back(tRB.mHTTPTraffic[i]);
    }
    for(int i= 0;i<(int)(tRB.mVoipTraffic.size());i++)
    {
        this->mVideoTraffic.push_back(tRB.mVideoTraffic[i]);
    }
    for(int i= 0;i<(int)(tRB.mCBRTraffic.size());i++)
    {
        this->mCBRTraffic.push_back(tRB.mCBRTraffic[i]);
    }
    for(int i= 0;i<(int)(tRB.mXRTraffic.size());i++)
    {
        this->mXRTraffic.push_back(tRB.mXRTraffic[i]);
    }
    return *this;
}

cRadioBearer::~cRadioBearer()
{
    mVoipTraffic.clear();
    mFTPTraffic.clear();
    mHTTPTraffic.clear();
    mVideoTraffic.clear();
    mCBRTraffic.clear();
    mXRTraffic.clear();
//     delete mRLC;
}		


/// Logical channel support
// void cRadioBearer::fSetLogicalChannel(cLogicalChannel tLogicalChannel)
// {
//     mLogicalChannel = tLogicalChannel;
// }
// 
// void cRadioBearer::fSetRLC(cRLC *tRLC)
// {
//     mRLC=tRLC;
// }
// 
// cRLC* cRadioBearer::fGetRLC() const
// {
//     return mRLC;
// }
// 
// void cRadioBearer::fSetPDCP(cPDCP tPDCP)
// {
//     mPDCP =tPDCP;
// }
// 
// cPDCP& cRadioBearer::fGetPDCP()
// {
//     return mPDCP;
// }

vector< cCBR > cRadioBearer::fGetCBRTrafficClass()
{
    return mCBRTraffic;
}

vector< cXR > cRadioBearer::fGetXRTrafficClass()
{
    return mXRTraffic;
}

void cRadioBearer::fSetCBRTrafficClass(vector< cCBR > tTraffic)
{
    mCBRTraffic=vector<cCBR>();
    mCBRTraffic=tTraffic;
}

void cRadioBearer::fSetXRTrafficClass(vector< cXR > tTraffic)
{
    mXRTraffic=vector<cXR>();
    mXRTraffic=tTraffic;
}

vector< cVoip > cRadioBearer::fGetVoipTrafficClass()
{
    return mVoipTraffic;
}

void cRadioBearer::fSetVoipTrafficClass(vector< cVoip > tTraffic)
{
    mVoipTraffic=vector<cVoip>();
    mVoipTraffic=tTraffic;
}


vector< cFTP > cRadioBearer::fGetFTPTrafficClass()
{
    return mFTPTraffic;
}

void cRadioBearer::fSetFTPTrafficClass(vector< cFTP > tTraffic)
{
    mFTPTraffic.resize(0);
    for(int i=0;i<(int)tTraffic.size();i++)
    {
        mFTPTraffic.push_back(tTraffic[i]);
    }
}

vector< cHTTP > cRadioBearer::fGetHTTPTrafficClass()
{
    return mHTTPTraffic;
}

void cRadioBearer::fSetHTTPTrafficClass(vector< cHTTP > tTraffic)
{
    mHTTPTraffic.resize(0);
    for(int i=0;i<(int)tTraffic.size();i++)
    {
        mHTTPTraffic.push_back(tTraffic[i]);
    }
}

vector< cVideo > cRadioBearer::fGetVideoTrafficClass()
{
    return mVideoTraffic;
}

void cRadioBearer::fSetVideoTrafficClass(vector< cVideo > tTraffic)
{
    mVideoTraffic.resize(0);
    for(int i=0;i<(int)tTraffic.size();i++)
    {
        mVideoTraffic.push_back(tTraffic[i]);
    }
}


int cRadioBearer::fGetSourceId()
{
    return mSourceId;
}

int cRadioBearer::fGetDestinationId()
{
    return mDestinationId;
}


int cRadioBearer::fGetRBId()
{
    return mRBID;
}

void cRadioBearer::fSetRBId(int tRBId)
{
    mRBID=tRBId;
}

cPacketQueue cRadioBearer::fGetRBPktQueue()
{
    return mRBPktQueue;
}

void cRadioBearer::fSetRBPktQueue(cPacketQueue tPktQueue)
{
    mRBPktQueue.fAddPktQueue(tPktQueue);
}

bool cRadioBearer::fIsInfiniteBuffer()
{
    return mIsInfiniteBuffer;
}

cPacket cRadioBearer::fCreatePacket(int tBytes, int tFrame)
{
    cPacket tpkt;
    tpkt.fSetPacketTimeStamp(tFrame);
//     cPDCPHeader tPDCPHeader;
//     tpkt.fSetPDCDPHeader(tPDCPHeader);
//     cRLCHeader tRLCHeader;
//     tpkt.fSetRLCHeader(tRLCHeader);
//     if(fGetRLC()->fGetRlcMode() == 0)
//         tpkt.fGetRLCHeader().fSetRLCMode(cRLCHeader::TM_RLC_MODE);
//     else if (fGetRLC()->fGetRlcMode() == 1)
//         tpkt.fGetRLCHeader().fSetRLCMode(cRLCHeader::UM_RLC_MODE, fGetRLC()->fGetSequenceNumSize());
//     else
//         tpkt.fGetRLCHeader().fSetRLCMode(cRLCHeader::AM_RLC_MODE);

    //Generate packet with out mac.
//     cMACHeader tMACHeader=cMACHeader();
//     tpkt.fSetMACHeader(tMACHeader);
    tpkt.fSetPktTrafficType(TRAFFIC_TYPE_INFINITE_BUFFER);
    tpkt.fGenerateRandomPacketBits(tBytes);

#ifdef RADIOBEARER_DEBUG
    cout<<" Packet Created for Source Id : "<<fGetSourceId()<<" Destination Id : "<<fGetDestinationId();
#endif
    return tpkt;
}

// cRLCHeader cRadioBearer::fGenerateRLCHeader(int tBytes)
// {
// 
//     cPacketQueue tPdcpPDUs = mPDCP.fGetPdcpTxQueue();
//     cPacket tPkt = tPdcpPDUs.fGetPeekPacket();
//     cRLCHeader tHeader = tPkt.fGetRLCHeader();
// 
//     if(fGetRLC()->fGetRlcMode() == 0)
//         tHeader.fSetRLCMode(cRLCHeader::TM_RLC_MODE);
//     else if (fGetRLC()->fGetRlcMode() == 1)
//         tHeader.fSetRLCMode(cRLCHeader::UM_RLC_MODE, fGetRLC()->fGetSequenceNumSize());
//     else
//         tHeader.fSetRLCMode(cRLCHeader::AM_RLC_MODE);
// 
//     tHeader.fSetSequenceNumber(0);
//     int tOverHead;
// 
//     if(tHeader.fGetSequenceNumberLength() == 5)
//     {
//         tOverHead = 1;
//     }
//     else
//     {
//         tOverHead = 2;
//     }
// 
//     int tOverHeadSize = tOverHead;
// 
//     if(tPkt.fGetIsFragmented())	/**< Check if the Packet is Fragmented or not */
//     {
//         tHeader.fSetFramingIndex(1);
//     }
//     else
//     {
//         tHeader.fSetFramingIndex(0);
//     }
// 
//     if(tPkt.fGetPacketSize() + tOverHead <= tBytes)
//     {
//         tBytes -= tPkt.fGetPacketSize();
// 
//         tPdcpPDUs.fDequeuePkt();
// 
//         tHeader.fSetExtensionBit(0);
//         if(tHeader.fGetFramingIndex()==1) { tHeader.fSetFramingIndex(2);}
// 
//         /// Checking if another PDCP Segment can be added
//         if(((tOverHead + 2) < tBytes - tOverHead) && (tPdcpPDUs.fGetNumberOfPackets() > 0))
//         {
//             tHeader.fSetExtensionBit(1);
//             tHeader.fAddVarExtensionBit(0);
//             tHeader.fAddVarLengthIndex(tPkt.fGetPacketSize());
//         }
//         else
//         {
//             tBytes = 0;
//         }
//     }
//     else
//     {
//         /**< Segmentation of Peer Packet in the Radio Bearer Packet Buffer */
// 
//         int tFragmentSize= tBytes - tOverHead;
//         tPkt.fSetPacketSize(tFragmentSize);
//         cPacket tempPkt = tPdcpPDUs.fGetPeekPacket();
//         tempPkt.fSetIsFragmented(true);  /**< Indicate it is a fragmented packet */
//         tempPkt.fSetPacketSize(tempPkt.fGetPacketSize() - tFragmentSize);
//         tPdcpPDUs.fUpdatePeerPacket(tempPkt);
//         tHeader.fSetExtensionBit(0);
//         if(tHeader.fGetFramingIndex() == 0){tHeader.fSetFramingIndex(1);}
//         else {tHeader.fSetFramingIndex(3);}
//         tBytes =0;
//     }
// 
//     tHeader.fSetRLCHeaderBytes();
//     tOverHead = tHeader.fGetRLCHeaderBytes().fGetBytes().size();
// 
//     while((tPdcpPDUs.fGetNumberOfPackets() > 0) && (tBytes>0))
//     {
//         cPacket temp= tPdcpPDUs.fGetPeekPacket();
// 
//         if((temp.fGetPacketSize() <= tBytes - tOverHead) && (0 < tBytes - tOverHead))
//         {
//             /**< Concatenation of Packets in the Radio Bearer Packet Buffer */
//             tPkt.fSetPacketSize(tPkt.fGetPacketSize() + temp.fGetPacketSize());
//             tBytes -= temp.fGetPacketSize();
//             tHeader.fSetExtensionBit(1);
// 
//             //no need E&LI for last data field
//             if(tHeader.fGetFramingIndex()==0){	tHeader.fSetFramingIndex(0);}
//             else{ tHeader.fSetFramingIndex(2); }
//             tPdcpPDUs.fDequeuePkt();
// 
//             if((tHeader.fGetVarLengthIndex().size() % 2)==0)
//             {
//                 tOverHeadSize = 2;
//             }
//             else
//             {
//                 tOverHeadSize = 1;
//             }
// 
//             if((tOverHeadSize < tBytes - tOverHead) && (tPdcpPDUs.fGetNumberOfPackets() >0))
//             {
//                 tHeader.fAddVarExtensionBit(0);
//                 tHeader.fAddVarLengthIndex(temp.fGetPacketSize());
//             }
//             else
//             {tBytes=0;}
// 
//         }
//         else
//         {
//             int tFragmentSize= 0;
//             if((tHeader.fGetVarLengthIndex().size() % 2)==0)
//             {
//                 tFragmentSize= tBytes - (tOverHead + 2);
//             }
//             else
//             {
//                 tFragmentSize= tBytes - (tOverHead + 1);
//             }
// 
//             /**< Segmentation of Peer Packet in the Radio Bearer Packet Buffer */
//             tPkt.fSetPacketSize(tPkt.fGetPacketSize() + tFragmentSize);
//             cPacket tempPkt = tPdcpPDUs.fGetPeekPacket();
//             tempPkt.fSetIsFragmented(true);  /**< Indicate its fragmented packet */
//             tempPkt.fSetPacketSize(tempPkt.fGetPacketSize() - tFragmentSize);
//             tPdcpPDUs.fUpdatePeerPacket(tempPkt);  /**< Update Peer Packet from RadioBearer Packet Queue  */
//             tHeader.fSetExtensionBit(1);
// 
//             if(tHeader.fGetFramingIndex()==0)
//             {
//                 tHeader.fSetFramingIndex(1);
//             }
//             else
//             {
//                 tHeader.fSetFramingIndex(3);
//             }
// 
//             tBytes =0;
//         }
//         tHeader.fSetRLCHeaderBytes();
//         tOverHead = tHeader.fGetRLCHeaderBytes().fGetBytes().size();
// #ifdef RLC_DEBUG 
//         tPkt.fGetRLCHeader().fPrintRLCHeader();
// #endif
//     }
// 
//     return tHeader;
// }

// cPacket cRadioBearer::fPacketToTransmit(int tAvailableBytes)
// {   
//     //correct the position pf packet generation.
//   
//     cPacketQueue tPdcpPDUs = mPDCP.fGetPdcpTxQueue();
// 
//     cPacket tRLC_PDU;
//     cByteSet tPktHeader;
//     cByteSet tByteSet;
// 
//     cPacket tRLC_SDU = tPdcpPDUs.fGetPeekPacket();
//     cRLCHeader tRLCHeader = fGenerateRLCHeader(tAvailableBytes);
//     int tOverHead = tRLCHeader.fGetRLCHeaderBytes().fGetBytes().size();
// 
// #ifdef RADIOBEARER_DEBUG
//     cout << tRLCHeader.fGetRLCHeaderBytes();
// #endif
// 
//     /**< Check if the Packet Queue is empty or Overhead size is greater than or equal to Available Bytes */
//     if((tPdcpPDUs.fGetNumberOfPackets() == 0) || (tOverHead >= tAvailableBytes))
//     {
//         cPacket t;
//         return t;
//     }
// 
// #ifdef RADIOBEARER_DEBUG          
//     cout << "Available Bytes " << tAvailableBytes << " peek packet size: " << tRLC_SDU.fGetPacketSize() << endl;
//     cout << "No of Packet in queue : " << tPdcpPDUs.fGetNumberOfPackets() << endl;
//     cout << "Size of the first packet: " << tRLC_SDU.fGetPacketSize() << "Frag? : " << tRLC_SDU.fGetIsFragmented() << endl;
// #endif
// 
//     int tTotalPktBytes = 0;
//     tAvailableBytes -= tRLCHeader.fGetRLCHeaderBytes().fGetBytes().size();
// 
//     if (tRLCHeader.fGetExtensionBit() == 1)
//     {
//       cout<<"entering extension"<<endl;
//         vector <unsigned int> tVarLenIdx = tRLCHeader.fGetVarLengthIndex();
//         int tSDUCnt = tVarLenIdx.size() + 1;
// 
//         for (int i = 0; i < (tSDUCnt - 1); i++)
//         {
// 	    if (tRLC_SDU.fGetPacketSize() != (int)tVarLenIdx[i]) {
//                 cout << "packet size and LI not matching: " << tRLC_SDU.fGetPacketSize() << " " << tVarLenIdx[i] << endl;
//                 exit(-1);
//             }
//             cout<<"entering radi barrrier"<<endl;
//             tByteSet.fAddBytesAtLast(tRLC_SDU.fGetPacketBits().fGetBytes());
//             tTotalPktBytes += tRLC_SDU.fGetPacketSize();
// 	    cout<<"trlc sdu size"<<tRLC_SDU.fGetPacketSize()<<endl;
//             tRLC_PDU.fSetPacketSize(tRLC_PDU.fGetPacketSize() + tRLC_SDU.fGetPacketSize());
// 	    cout<<"ffttttt"<<tRLC_PDU.fGetPacketSize() + tRLC_SDU.fGetPacketSize()<<endl;
// 
//             tPdcpPDUs.fDequeuePkt();
// 
//             tRLC_SDU = tPdcpPDUs.fGetPeekPacket();
//         }
//     }
// 
//     if (tAvailableBytes - tTotalPktBytes < tRLC_SDU.fGetPacketSize()) //3909 -3000 < 1002 we got it as 909 which is less than get packet size which doesnt form a packet.
//     {
//         if (tRLCHeader.fGetFramingIndex() != 3 && tRLCHeader.fGetFramingIndex() != 1)
//         {
//             cout << "FI should be 3 or 1" << endl;
//             abort();
//         }
//         tByteSet.fAddBytesAtLast(tRLC_SDU.fGetPacketBits().fGetAndRemoveFirstNBytes(tAvailableBytes - tTotalPktBytes));
//         tRLC_SDU.fSetPacketSize(tRLC_SDU.fGetPacketSize() - (tAvailableBytes - tTotalPktBytes));
//         tRLC_SDU.fSetIsFragmented(true);
//         tRLC_PDU.fSetPacketSize(tRLC_PDU.fGetPacketSize() + (tAvailableBytes - tTotalPktBytes));
// 
//         tPdcpPDUs.fUpdatePeerPacket(tRLC_SDU);
//     }
//     else if (tAvailableBytes - tTotalPktBytes >= tRLC_SDU.fGetPacketSize())
//     {
//         if (tRLCHeader.fGetFramingIndex() != 2 && tRLCHeader.fGetFramingIndex() != 0)
//         {
//             cout << "FI should be 2 or 0" << endl;
//             abort();
//         }
//         tByteSet.fAddBytesAtLast(tRLC_SDU.fGetPacketBits().fGetBytes());
//         tRLC_PDU.fSetPacketSize(tRLC_PDU.fGetPacketSize() + tRLC_SDU.fGetPacketSize());
// 
//         tPdcpPDUs.fDequeuePkt();
//     }
// 
//     tRLC_PDU.fSetRLCHeader(tRLCHeader);
//     tRLC_PDU.fSetPacketBits(tByteSet);
// 
//     mPDCP.fUpdateTxQueue(tPdcpPDUs);
//     fGetLogicalChannel().fUpdateQlength(mPDCP.fGetPdcpTxQueue().fGetSize());
// 
// #ifdef RADIOBEARER_DEBUG
//     cout << "PDCP details: " << tPdcpPDUs.fGetNumberOfPackets()  << " " << mPDCP.fGetPdcpTxQueue().fGetSize() << " " << mPDCP.fGetPdcpTxQueue().fGetNumberOfPackets() << endl;
//     cout << "packet to be sent SN: " << tRLC_PDU.fGetRLCHeader().fGetSequenceNumber() << " size: " << tRLC_PDU.fGetPacketSize() << " FI: " << tRLC_PDU.fGetRLCHeader().fGetFramingIndex() << endl;
// #endif
// 
//     return tRLC_PDU;
// }

void cRadioBearer::fSetRBPktQueueEmpty()
{
    mRBPktQueue.fDeleteAllPkts();
}

void cRadioBearer::fUpdateTransmittedBytes(int tBytes)
{
    mTransmittedBits = mTransmittedBits + (tBytes*8);
}

unsigned int cRadioBearer::fGetTransmittedBits()
{
    return mTransmittedBits;
}
/*
LCQueueLoad cRadioBearer::fGetLCQueueLoad()
{
    LCQueueLoad tLCQueueLoad;
    tLCQueueLoad.lcId = fGetRBId();
    tLCQueueLoad.lcQueueLoad = fGetRBPktQueue().fGetSize();
    return tLCQueueLoad; //WarningFix: added return statement
}*/

void DataRadioBearer::init(string L2SupportFileName, int subframeCnt,int& arrivalTimeFTP)
{
  Array<Array<string>> traffic;
  parse(L2SupportFileName,"mTrafficType",traffic);
  
  //Considering one traffic 
  
  if(traffic(0)(0)=="TRAFFIC_TYPE_VOIP"){
    trafficType = TRAFFIC_TYPE_VOIP;
    mVoipTraffic.push_back(cVoip(trafficType));
    mVoipTraffic[0].fSetSubFrameIndx(-1);
  }
  else if(traffic(0)(0)=="TRAFFIC_TYPE_CBR"){
    trafficType = TRAFFIC_TYPE_CBR;
    mCBRTraffic.push_back(cCBR(trafficType,L2SupportFileName));
    mCBRTraffic[0].fSetSubFrameIndx(-1);
  }
  else if(traffic(0)(0)=="TRAFFIC_TYPE_XR"){
    trafficType = TRAFFIC_TYPE_XR;
    cout<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% ---- XR subframeCnt : "<<subframeCnt<<endl;
    mXRTraffic.push_back(cXR(trafficType,L2SupportFileName,subframeCnt));
    mXRTraffic[0].fSetSubFrameIndx(-1);
  }
  else if(traffic(0)(0)=="TRAFFIC_TYPE_VIDEO"){ 
    trafficType = TRAFFIC_TYPE_VIDEO;
    mVideoTraffic.push_back(cVideo(trafficType,L2SupportFileName,subframeCnt));
    mVideoTraffic[0].fSetSubFrameIndx(-1);
  } 
  else if(traffic(0)(0)=="TRAFFIC_TYPE_HTTP"){
    trafficType = TRAFFIC_TYPE_HTTP;
    mHTTPTraffic.push_back(cHTTP(trafficType,L2SupportFileName,subframeCnt));
    mHTTPTraffic[0].fSetSubFrameIndx(-1);
  }
   else if(traffic(0)(0)=="TRAFFIC_TYPE_FTP"){
    trafficType = TRAFFIC_TYPE_FTP;
    mFTPTraffic.push_back(cFTP(trafficType,L2SupportFileName,subframeCnt,arrivalTimeFTP));
    mFTPTraffic[0].fSetSubFrameIndx(-1);
  }
  else if(traffic(0)(0)=="TRAFFIC_TYPE_INFINITE_BUFFER")
  {
    trafficType = TRAFFIC_TYPE_INFINITE_BUFFER;
    mIsInfiniteBuffer=true;
  }
  else{
    cout<<"[both:] RadioBearer Constructor: Error in Traffic Type" <<endl ;
    abort();
  }
  bufferSizeInBytes = 0;
}

void DataRadioBearer::init(int txNode,int rxNode,int rbID,sL2SimConfig* l2SimConfig,bool isDownlink)
{
    mNumberOfApp = l2SimConfig->mTrafficType[rbID].size();
    mSourceId = txNode;
    mDestinationId = rxNode;
    mDirection = int(isDownlink);
    mRBID = rbID;
    mAppTraffic.set_length(mNumberOfApp);
    for(int app_cnt=0;app_cnt<mNumberOfApp;app_cnt++)
    {
        eTrafficType TrafficType = l2SimConfig->mTrafficType[rbID][app_cnt];
        if(TrafficType == TRAFFIC_TYPE_INFINITE_BUFFER) mIsInfiniteBuffer = true;
        mAppTraffic(app_cnt).init(l2SimConfig,TrafficType,app_cnt,txNode,rxNode,isDownlink);
        if(currentTaskid==0)
            mAppTraffic(app_cnt).printStartTime(rbID);
    }
    bufferSizeInBytes = 0;
}

int DataRadioBearer::getBufferSizeInBytes(int subframeNumber)
{
    stringstream ss;
    int prevBufferSize = bufferSizeInBytes;
    for(int app_cnt=0;app_cnt<mNumberOfApp;app_cnt++)
    {
        int dataSize = mAppTraffic(app_cnt).getPktQueueSize(subframeNumber);
        bufferSizeInBytes += dataSize;
        if(dataSize>0)
            ss<<"("<<getString(mAppTraffic(app_cnt).trafficType)<<","<<dataSize<<"),";
    }
    if(bufferSizeInBytes!=0)
    {
        string fileName = "dataSizePerSubframe.txt";
        ofstream op;
        if(!IsFileExists(fileName))
        {
            op.open(fileName.c_str(),std::ios::out);
            op<<"%subframeNumber,\ttxNodeID,\trxNodeID,\trbID,\tcurrentBufferSizeInBytes,\tcurrentDataSizeInBytes [App,dataSize]"<<endl;
        }
        else
            op.open(fileName.c_str(),std::ios::app);
        op<<subframeNumber<<",\t"<<mSourceId<<",\t"<<mDestinationId<<",\t"<<mRBID<<",\t"<<bufferSizeInBytes<<",\t"<<bufferSizeInBytes-prevBufferSize<<"\t["<<ss.str()<<"]"<<endl;
        op.close();
    }
    return bufferSizeInBytes;
}

int DataRadioBearer::getBufferSizeInBytes(int subframeNumber, double miniSlotStart,
                                          double miniSlotEnd, bool isSubframeEnd) {
  stringstream ss;
  int prevBufferSize = bufferSizeInBytes;

  if (bufferSizeInBytes == 0)
    pktStartEnd = 0;

  for (int app_cnt = 0; app_cnt < mNumberOfApp; app_cnt++) {
    int dataSize = mAppTraffic(app_cnt).getPktQueueSize(miniSlotStart, miniSlotEnd, isSubframeEnd);
    bufferSizeInBytes += dataSize;
    if (dataSize > 0) {
      pktInBuffer.push_back(dataSize);
      ss << "(" << getString(mAppTraffic(app_cnt).trafficType) << "," << dataSize << "),";
    }
  }

  if (bufferSizeInBytes != 0) {
    string fileName = "dataSizePerSubframeMini.txt";
    ofstream op;
    if (!IsFileExists(fileName)) {
      op.open(fileName.c_str(), std::ios::out);
      op << "%subframeNumber,\ttxNodeID,\trxNodeID,\trbID,\tcurrentBufferSizeInBytes,"
            "\tcurrentDataSizeInBytes,"
            "\tminiSlotStart,\tminiSlotEnd,\tisSubframeEnd,"
            "\t[App,dataSize]"
         << endl;
    } else
      op.open(fileName.c_str(), std::ios::app);

    op << subframeNumber << ",\t" << mSourceId << ",\t" << mDestinationId << ",\t" << mRBID << ",\t"
       << bufferSizeInBytes << ",\t" << bufferSizeInBytes - prevBufferSize << ",\t"
       << miniSlotStart << ",\t" << miniSlotEnd << ",\t" << isSubframeEnd << ","
       << "\t[" << ss.str() << "]" << endl;
    op.close();
  }

  // std::cout << "\033[1;31m[DEBUG]: \033[0m" << __FILE__ << ": " << __LINE__
  //           << " start =  " << miniSlotStart << " end =  " << miniSlotEnd
  //           << " size = " << bufferSizeInBytes
  //           << std::endl;

  return bufferSizeInBytes;
}

// void RadioBearer::init(int txNode,int rxNode,int rbID,sL2SimConfig* l2SimConfig,bool isDownlink)
// {
//     int bsID = (isDownlink) ? txNode : rxNode;
//     int ueID = (isDownlink) ? rxNode : txNode;
//     mNumberOfApp = l2SimConfig->mNumberOfAppPerRB;
//     mSourceId = txNode;
//     mDestinationId = rxNode;
//     mDirection = int(isDownlink);
//     mRBID = rbID;
//     
//     trafficType = l2SimConfig->mTrafficType[rbID][0];
//     for(int app_cnt=0;app_cnt<mNumberOfApp;app_cnt++)
//     {
//         eTrafficType TrafficType = l2SimConfig->mTrafficType[rbID][app_cnt];
//         switch(TrafficType)
//         {
//             case TRAFFIC_TYPE_FTP:
//                 mFTPTraffic.push_back(cFTP(TrafficType,txNode,rxNode,app_cnt,l2SimConfig,ueID,bsID));
//                 mFTPTraffic[app_cnt].fSetSubFrameIndx(-1);
//                 break;
//             case TRAFFIC_TYPE_HTTP:
//                 mHTTPTraffic.push_back(cHTTP(TrafficType,txNode,rxNode,app_cnt,l2SimConfig));
//                 mHTTPTraffic[app_cnt].fSetSubFrameIndx(-1);
//                 break;
//             case TRAFFIC_TYPE_CBR:
//                 mCBRTraffic.push_back(cCBR(TrafficType,txNode,rxNode,app_cnt,l2SimConfig));
//                 mCBRTraffic[app_cnt].fSetSubFrameIndx(-1);
//                 break;
//             case TRAFFIC_TYPE_VOIP:
//                 mVoipTraffic.push_back(cVoip(TrafficType));
//                 mVoipTraffic[app_cnt].fSetSubFrameIndx(-1);
//                 break;
//             case TRAFFIC_TYPE_VIDEO:
//                 mVideoTraffic.push_back(cVideo(TrafficType,txNode,rxNode,app_cnt,l2SimConfig));
//                 mVideoTraffic[app_cnt].fSetSubFrameIndx(-1);
//                 break;
//             case TRAFFIC_TYPE_INFINITE_BUFFER:
//                 mIsInfiniteBuffer=true;
//                 break;
//             default:
//                 cout<<"[both:] RadioBearer Constructor: Error in Traffic Type" <<endl ;
//                 abort();
//                 break;
//         }
//     }
//     bufferSizeInBytes = 0;
// }
// 
// int RadioBearer::getBufferSizeInBytes(int subframeNumber)
// {
//   if(trafficType==TRAFFIC_TYPE_VOIP){
//     
//     int prevSubframeIndx=(int)mVoipTraffic[0].fGetSubFrameIndx();
//     for(int cnt=prevSubframeIndx+1;cnt<=subframeNumber;cnt++)
//     {
//       cPacketQueue pktQueue=mVoipTraffic[0].fCallTraffic();
//       bufferSizeInBytes+=pktQueue.fGetSize();
//       mVoipTraffic[0].fSetSubFrameIndx(cnt);
//     }
//     
//   }
//   else if(trafficType==TRAFFIC_TYPE_FTP){
//     int prevSubframeIndx=mFTPTraffic[0].fGetSubFrameIndx();
//     for(int cnt=prevSubframeIndx+1;cnt<=subframeNumber;cnt++)
//       {
// 	cPacketQueue pktQueue=mFTPTraffic[0].fCallTraffic(cnt);
// 	bufferSizeInBytes+=pktQueue.fGetSize();
// 	mFTPTraffic[0].fSetSubFrameIndx(cnt);
//       }
//     }
//     else if(trafficType==TRAFFIC_TYPE_HTTP){
//       int prevSubframeIndx=mHTTPTraffic[0].fGetSubFrameIndx();
//       for(int cnt=prevSubframeIndx+1;cnt<=subframeNumber;cnt++)
//       {
// 	cPacketQueue pktQueue=mHTTPTraffic[0].fCallTraffic(cnt);
// 	bufferSizeInBytes+=pktQueue.fGetSize();
// 	mHTTPTraffic[0].fSetSubFrameIndx(cnt);
//       }
//     }
//     else if(trafficType==TRAFFIC_TYPE_VIDEO){
//       int prevSubframeIndx=mVideoTraffic[0].fGetSubFrameIndx();
//       for(int cnt=prevSubframeIndx+1;cnt<=subframeNumber;cnt++)
//       {
// 	cPacketQueue pktQueue=mVideoTraffic[0].fCallTraffic(cnt);
// 	bufferSizeInBytes+=pktQueue.fGetSize();
// 	mVideoTraffic[0].fSetSubFrameIndx(cnt);
//       }
//     }
//     else if(trafficType==TRAFFIC_TYPE_CBR){
//       int prevSubframeIndx=mCBRTraffic[0].fGetSubFrameIndx();
//     for(int cnt=prevSubframeIndx+1;cnt<=subframeNumber;cnt++)
//     {
//       cPacketQueue pktQueue=mCBRTraffic[0].fCallTraffic(cnt);
//       bufferSizeInBytes+=pktQueue.fGetSize();
//       mCBRTraffic[0].fSetSubFrameIndx(cnt);
//     }
//   }
//   else if(trafficType==TRAFFIC_TYPE_INFINITE_BUFFER)
//   {
//     bufferSizeInBytes=1000000;
//   }
//   else{
//     cout<<"[both:] getBufferSizeInBytes(): Error in Traffic Type" <<endl ;
//     abort();
//   }
//   return bufferSizeInBytes;
//   
// }

void RadioBearer_S::init(int txNode,int rxNode,sL2SimConfig* l2SimConfig,bool isDownlink)
{
    nRadioBearers = l2SimConfig->nRadioBearers;
    radioBearer.set_length(l2SimConfig->nRadioBearers);
    for(int rb_cnt=0;rb_cnt<l2SimConfig->nRadioBearers;rb_cnt++)
        radioBearer(rb_cnt).init(txNode,rxNode,rb_cnt,l2SimConfig,isDownlink);
}

int RadioBearer_S::getBufferSize(int subframeNumber,int& rbID)
{
    int bufferSize = 0;
    for(int rb_cnt=0;rb_cnt<nRadioBearers;rb_cnt++)
    {
        bufferSize = radioBearer(rb_cnt).getBufferSizeInBytes(subframeNumber);
        if(bufferSize!=0)
        {
            rbID = rb_cnt;
            break;
        }
    }
    return bufferSize;
}

int RadioBearer_S::getBufferSize(int subframeNumber, double miniSlotStart, double miniSlotEnd,
                                 bool isSubframeEnd, int &rbID) {
  int bufferSize = 0;
  for (int rb_cnt = 0; rb_cnt < nRadioBearers; rb_cnt++) {
    bufferSize = radioBearer(rb_cnt).getBufferSizeInBytes(subframeNumber, miniSlotStart,
                                                          miniSlotEnd, isSubframeEnd);
    if (bufferSize != 0) {
      rbID = rb_cnt;
      break;
    }
  }

  // std::cout << "\033[1;31m[DEBUG]: \033[0m" << __FILE__ << ": " << __LINE__
  //           << " start =  " << miniSlotStart << " end =  " << miniSlotEnd
  //           << " size = " << bufferSize
  //           << std::endl;
  return bufferSize;
}

bool RadioBearer_S::isInfiniteBuffer(int rbID)
{
    return(radioBearer(rbID).mIsInfiniteBuffer);  // RB0 is the highest priority for scheduling.
}

int RadioBearer_S::getPktStartEnd(int rbID) {
  return(radioBearer(rbID).getPktStartEnd());
}
