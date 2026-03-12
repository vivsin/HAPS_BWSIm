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
#ifndef CRADIOBEARER_H
#define CRADIOBEARER_H

// #include "../../Sim/SimInclude.h"

// #include "../lib/L2/MAC/include/Application/cTrafficClass.h"
#include "../Application/cTrafficClass.h"
// #include "../RLC/cRLC.h"
// #include "../RLC/cRLCtm.h"
// #include "../RLC/cRLCum.h"
// #include "../RLC/cRLCam.h"
// #include "../PDCP/cPDCP.h"

#include "../Application/cFTP.h"
#include "../Application/cVoip.h"
#include "../Application/chttp.h"
#include "../Application/cVideo.h"
#include "../Application/cCBR.h"
#include "../Application/cXR.h"
// #include "../lib/L2/MAC/include/Application/"


// #include "../../Sim/L2SimConfig.h"
#include "../Others/L2SimConfig.h"

#include <deque>
#include <numeric>

// #include "../../Interface/cMACRLCInterface.h"

using namespace std;

#define MAX_PRIORITY 16

class Application
{
    cVoip* voip;
    cFTP* ftp;
    cHTTP* http;
    cVideo* video;
    cCBR* cbr;
    cXR* xr;
public:
    eTrafficType trafficType;
    Application();
    Application(sL2SimConfig* l2SimConfig,eTrafficType traffic,int appID,int txNode,int rxNode,bool isDownlink);
    int getPktQueueSize(int subframeNumber);

    // TO get he packet for each minislot based on the packet arrival time
    int getPktQueueSize(double miniSlotStart, double miniSlotEnd, bool isSubframeEnd);
    void printStartTime(int rbID);
    void init(sL2SimConfig* l2SimConfig,eTrafficType traffic,int appID,int txNode,int rxNode,bool isDownlink);
};

class cRadioBearer
{
public:
    vector<cVoip > mVoipTraffic; /**< VoIP Applications associated with Radio Bearer. It can be one/more  */
    vector<cFTP > mFTPTraffic; /**< FTP Applications associated with Radio Bearer. It can be one/more  */
    vector<cHTTP > mHTTPTraffic; /**< HTTP Applications associated with Radio Bearer. It can be one/more  */
    vector<cVideo > mVideoTraffic; /**< Video Applications associated with Radio Bearer. It can be one/more  */
    vector<cCBR > mCBRTraffic; /**< Video Applications associated with Radio Bearer. It can be one/more  */
    vector<cXR > mXRTraffic; /**< XR Applications associated with Radio Bearer. It can be one/more  */
    Array<Application> mAppTraffic;
//     cRLC *mRLC; /**< RLC Entity Object */
//     cPDCP mPDCP; /**< PDCP Entity Object */
    int mSourceId; /**< Identificaiton Number of Source */
    int mDestinationId; /**< Identificaiton Number of Destination */
    int mNumberOfApp; /**< Number of Applications attached to the RB*/
    int mRBID; /**< Radio Bearer ID */

    /// direction - UL or DL
    int mDirection;

    cPacketQueue mRBPktQueue; /**< Radio Bearer Packet Queue */
    bool mIsInfiniteBuffer; /**< Flag - set when Inifinite Buffer is included in Traffic Class  */
    int mTransmittedBits; /**< Number of bits transmitted */
//     cRLCHeader fGenerateRLCHeader(int tBytes);


//     cLogicalChannel mLogicalChannel;

public:
    /**
   * @brief
   * Default Constructor for Radio Bearer
   * @fn cRadioBearer
  */
    cRadioBearer();

    /**
   * @brief
   * Constructor for Radio Bearer
   * @fn cRadioBearer
   * @param dest
   * int - Destination Identification Number
   * @param src
   * int - Source Identification Number
   * @param RBId
   * int - Radio Bearer Identification Number
  */
//     cRadioBearer(int dest, int src, int RBId, int direction, sLCConfigInfo tLCConfigInfo, sL2SimConfig* sSimConfig);
    cRadioBearer(cRadioBearer const& tRB);
    cRadioBearer& operator=(cRadioBearer const& tRB);

    /**
   * @brief
   * Destructor of RadioBearer Entity
   * @fn ~cRadioBearer
  */
    ~cRadioBearer();

    /**
   * @brief
   * Initate the RLC object
   * @fn fSetRLC
   * @param tRLC
   * Temporary RLC Object
  */
//     void fSetRLC(cRLC *tRLC);

    /**
   * @brief
   * Returns the RLC object
   * @fn fGetRLC
   * @return cRLC
   * Returns RLC Object associated with RadioBearer
  */
//     cRLC* fGetRLC() const;

    /**
   * @brief
   * Initate the PDCP object
   * @fn fSetPDCP
   * @param tPDCP
   * Temporary PDCP Object
  */
//     void fSetPDCP(cPDCP tPDCP);

    /**
   * @brief
   * Returns PDCP Object
   * @fn fGetPDCP
   * @return cPDCP
   * Returns PDCP Object associated with RadioBearer
  */
//     cPDCP & fGetPDCP();

    /**
   * @brief
   * To get vector of VoIP Traffic Class
   * @fn fGetVoipTrafficClass
   * @return vector<cVoip>
   * Returns vector of VoIP Traffic Class
  */
    vector<cVoip > fGetVoipTrafficClass();

    /**
   * @brief
   * To set VoIP Traffic Class
   * @fn fSetVoipTrafficClass
   * @param tTraffic
   * vector <cVoip> - vector of VoIP Traffic Class
  */
    void fSetVoipTrafficClass(vector< cVoip > tTraffic);

    /**
   * @brief
   * To get Vector of FTP Traffic Class
   * @fn fGetFTPTrafficClass
   * @return vector<cFTP>
   * Returns vector of FTP Traffic Class
  */
    vector<cFTP > fGetFTPTrafficClass();

    /**
   * @brief
   * To set FTP traffic Class
   * @fn fSetFTPTrafficClass
   * @param tTraffic
   * vector <cFTP> - vector of FTP Traffic Class
  */
    void fSetFTPTrafficClass(vector< cFTP > tTraffic);

    /**
   * @brief
   * To get Vector of HTTP Traffic Class
   * @fn fGetHTTPTrafficClass
   * @return vector<cHTTP>
   * Returns vector of HTTP Traffic Class
  */
    vector<cHTTP > fGetHTTPTrafficClass();

    /**
   * @brief
   * To set HTTP traffic Class
   * @fn fSetHTTPTrafficClass
   * @param tTraffic
   * vector <HTTP> - vector of HTTP Traffic Class
  */
    void fSetHTTPTrafficClass(vector< cHTTP > tTraffic);

    /**
   * @brief
   * To get Vector of HTTP Traffic Class
   * @fn fGetHTTPTrafficClass
   * @return vector<cHTTP>
   * Returns vector of HTTP Traffic Class
  */
    vector<cVideo > fGetVideoTrafficClass();

    /**
   * @brief
   * To set HTTP traffic Class
   * @fn fSetHTTPTrafficClass
   * @param tTraffic
   * vector <Video> - vector of HTTP Traffic Class
  */
    void fSetVideoTrafficClass(vector< cVideo > tTraffic);

    /**
   * @brief
   * To destroy Radio Bearer
   * @fn fDestroy
  */
    void fDestroy();

    /**
   * @brief
   * To set Radio Bearer Identification Number
   * @fn fSetRBId
   * @param tRBId
   * int - Radio Bearer Identification Number
  */
    void fSetRBId(int tRBId);

    /**
   * @brief
   * To get Radio Bearer Identification Number
   * @fn fGetRBId
   * @return int
   * Returns Radio Bearer Identification Number
  */
    int fGetRBId();

    /**
   * @brief
   * To set Packet Queue to Radio Bearer
   * @fn fSetRBPktQueue
   * @param tPktQueue
   * cPacketQueue - Packet Queue to be set to Radio Bearer
  */
    void fSetRBPktQueue(cPacketQueue tPktQueue);

    /**
   * @brief
   * To return RadioBearer Packet Queue
   * @fn fGetRBPktQueue
   * @return cPacketQueue
   * Returns RadioBearer Packet Queue
  */
    cPacketQueue fGetRBPktQueue();

    /**
   * @brief
   * Check if the RadioBearer has Infinite Buffer as Traffic Class
   * @fn fIsInfiniteBuffer
   * @return bool
   * True - If RadioBearer has Inifinite Buffer as Traffic Class
   * False - otherwise
  */
    bool fIsInfiniteBuffer();

    /**
   * @brief
   * To create Packet for Infinite Buffer of size 'tBytes' bytes
   * @fn fCreatePacket
   * @param tBytes
   * int Size of the packet to be created for Infinite Buffer
   * @param tFrame
   * int Current time
   * @return cPacket
   * Returns the Packet with size 'tBytes' (byte) and timestamp 'tFrame' (ms)
  */
    cPacket fCreatePacket(int tBytes, int tFrame);

    /**
   * @brief
   * Get Source Identification Number
   * @fn fGetSourceId
   * @return int
   * Returns Source Identification Number
  */
    int fGetSourceId();

    /**
   * @brief
   * Get Destination Identification Number
   * @fn fGetDestinationId
   * @return int
   * Returns Destination Identification Number
  */
    int fGetDestinationId();

    /**
   * @brief
   * To make RadioBearer Packet Queue Empty
   * @fn fSetRBPktQueueEmpty
  */
    void fSetRBPktQueueEmpty();

    /**
   * @brief
   * To update the Transmitted bytes to total transmitted bits
   * @fn fUpdateTransmittedBytes
   * @param tBytes
   * int - Bytes to be updated
  */
    void fUpdateTransmittedBytes(int tBytes);

    /**
   * @brief
   * Returns the Complete Transmitted Bits
   * @fn fGetTransmittedBits
   * @return unsigned int
   * Returns the total transmitted bits.
  */
    unsigned int fGetTransmittedBits();

    /**
   * @brief
   * Create the Packet from Radio Bearer Packet Queue.
   * The Segementation and Concatenation of Packets from Packet Queue is done here.
   * The RLC Header is also prepared for the output Packet
   * @fn fPacketToTransmit
   * @param tAvailableBytes
   * Maximum size of the packet to be created from RadioBearer PacketQueue for Transmission.
   * @return cPacket
   * Returns a packet for Transmission.
  */
//     cPacket fPacketToTransmit(int tAvailableBytes);

    vector< cCBR > fGetCBRTrafficClass();
    void fSetCBRTrafficClass(vector< cCBR > tTraffic);

    vector< cXR > fGetXRTrafficClass();
    void fSetXRTrafficClass(vector< cXR > tTraffic);

//     LCQueueLoad fGetLCQueueLoad();

//     void fSetLogicalChannel(cLogicalChannel); //fSetLogicalChannel() and fGetLogicalChannel() are the mutator functions to access the private variable :mLogicalChannel of cLogicalChannel class.
//     cLogicalChannel & fGetLogicalChannel()
//     {
//         return mLogicalChannel;
//     }


};

class DataRadioBearer : public cRadioBearer
{
  int bufferSizeInBytes;
  eTrafficType trafficType;

    // The following variables are used for the tracking packet latency.
    // Valid values for pktStartEnd
    // pktStartEnd = 0 := Packet start. New pkt started tx.
    // pktStartEnd = 1 := Packet start & end. One pkt finished tx and the next pkt started tx.
    // pktStartEnd = 2 := No change. Part of old pkt being tx'd.
    // pktStartEnd = 3 := Packet End. Old pkt finished tx and no new pkt to tx.
    std::deque<int> pktInBuffer;
    int pktStartEnd;
public : 
  void init(string L2SupportFileName ,int subframeCnt, int& arrivalTimeFTP);
  void init(int txNode,int rxNode,int rbID,sL2SimConfig* l2SimConfig,bool isDownlink);
  
  void reduceBufferSize(int sizeToReduceInBytes)
  {
    if(bufferSizeInBytes>=sizeToReduceInBytes)
      bufferSizeInBytes-=sizeToReduceInBytes;
    else
    {
      bufferSizeInBytes=0;
    }

    // Trakcking when the packet transmission starts and ends.
    // int isNewPkt = -1;
    if (pktInBuffer.size() > 0) {
      if (pktInBuffer[0] >= sizeToReduceInBytes) {
        pktInBuffer[0] -= sizeToReduceInBytes;
        pktStartEnd = 2;
      } else if (pktInBuffer.size() > 1) {
        pktInBuffer[1] -= sizeToReduceInBytes - pktInBuffer[0];
        pktInBuffer.pop_front();
        pktStartEnd = 1;
      } else {
        pktInBuffer.pop_front();
        pktStartEnd = 3;
      }
    } else
      pktStartEnd = 0;

    // std::cout << "\033[1;31m[DEBUG]: \033[0m" << __FILE__ << ": " << __LINE__ << ": BufferSize = " << bufferSizeInBytes
    //           << " size Reduced =  " << sizeToReduceInBytes
    //           << " pkt Queue size =  " << pktInBuffer.size()
    //           << " pkt Queue byte =  " << std::accumulate(pktInBuffer.begin(), pktInBuffer.end(), 0)
    //           << " pktStartEnd =  " << pktStartEnd
    //           << std::endl;
  }
  int getBufferSizeInBytes(int subframeNumber);
  int getBufferSizeInBytes(int subframeNumber, double miniSlotStart, double miniSlotEnd,
                           bool isSubframeEnd);
  int getPktStartEnd() {
    return this->pktStartEnd;
  };
};

class RadioBearer_S
{
    Array<DataRadioBearer> radioBearer;     //Radio bearers associated to one UE. 
    int nRadioBearers;
public:
    RadioBearer_S()
    {
        radioBearer.set_length(0);
        nRadioBearers = -1;
    }
    void init(int txNode,int rxNode,sL2SimConfig* l2SimConfig,bool isDownlink);
    int getBufferSize(int subframeNumber,int& rbID);
    int getBufferSize(int subframeNumber, double miniSlotStart, double miniSlotEnd,
                      bool isSubframeEnd, int & rbID);
    void reduceBufferSize(int sizeToReduceInBytes,int rbID=0)
    {
        radioBearer(rbID).reduceBufferSize(sizeToReduceInBytes);
    }
    bool isInfiniteBuffer(int rbID=0);
    int getPktStartEnd(int rbID);
};

#endif // CRADIOBEARER_H
