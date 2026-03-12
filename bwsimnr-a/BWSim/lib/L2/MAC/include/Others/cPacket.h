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
#ifndef CPACKET_H
#define CPACKET_H

#include "cByteSet.h"


enum eTrafficType
{
    TRAFFIC_TYPE_UNINIT=-1,
    TRAFFIC_TYPE_VOIP,
    TRAFFIC_TYPE_FTP,
    TRAFFIC_TYPE_INFINITE_BUFFER,
    TRAFFIC_TYPE_VIDEO,
    TRAFFIC_TYPE_HTTP,
    TRAFFIC_TYPE_CBR,
    TRAFFIC_TYPE_XR
};

enum eFrameType
{
    I = 1,
    P = 2,
    B = 3
};

class cPacket
{
private:
    int mSize; /**< TODO */
    int mTimeStamp; /**< TODO */
    bool mIsFragmented;
    eTrafficType mTrafficType;
    int mReTxCount;
    double mArrivalTime;

    cByteSet mPacketBits;
//     cRLCHeader mRLCHeader; /**< RLC Header in the Packet */
//     cTCPHeader mTCPHeader;
//     cPDCPHeader mPDCPHeader; /**< PDCP Header in the Packet */
//     cMACHeader mMACHeader; /**< MAC Header in the Packet */
//      bool mIsPdcpSnAssigned;//We want to distingusih between PDCP SDUs and PDCP PDUs in mPDCPTxQueue. Dated:15/09/14 . harsha
//     bool mIsTcpSnAssigned;
#ifdef USING_RRC
    RRCConnectionSetupPacket mRRCConnectionSetupPkt;
#endif
public:
    /**
   * @brief
   * Constructor for Packet
   * @fn cPacket
  */
    cPacket();
    /**
   * @brief
   * Destructor for Packet
   * @fn ~cPacket
  */
    cPacket(cPacket const& tPkt);
    
    cPacket& operator=(cPacket const& tPkt);

    ~cPacket();

    /**
   * @brief
   * Gets Packet Size
   * @fn fGetPacketSize
   * @return int
   * Returns Packet Size
  */
    int fGetPacketSize();

    /**
   * @brief
   * Sets Packet Size
   * @fn fSetPacketSize
   * @param int
   * Takes packet size as the parameter
  */
    void fSetPacketSize(int tSize);

    /**
   * @brief
   *
   * @fn fGetPacketTimeStamp
   * @return int
  */
    int fGetPacketTimeStamp();

    /**
   * @brief
   *
   * @fn fSetPacketTimeStamp
   * @param int
  */
    void fSetPacketTimeStamp(int tTimeStamp);

    /*
     * Set the arrival time for packet, in ms
     */
    void fSetArrivalTime(double tArrivalTime);

    /*
     * Get the arrival time of packet, in ms
     */ 
    double fGetArrivalTime();

    
     /**
   * @brief
   *
   * @fn fSetTCDPHeader
   * @param header
  */
//     void fSetTCPHeader(cTCPHeader theader);
    /**
   * @brief
   *
   * @fn fGetTCPHeader
   * @return cHeader
  */
//     cTCPHeader& fGetTCPHeader();
    /**
   * @brief
   *
   * @fn fPrintTCPHeader
  */
//     void fPrintTCPHeader();
    
      
    /**
   * @brief
   *
   * @fn fSetPDCDPHeader
   * @param header
  */
//     void fSetPDCDPHeader(cPDCPHeader theader);
    /**
   * @brief
   *
   * @fn fGetPDCPHeader
   * @return cHeader
  */
//    cPDCPHeader& fGetPDCPHeader();
    /**
   * @brief
   *
   * @fn fPrintPDCPHeader
  */
//     void fPrintPDCPHeader();

    /**
   * @brief
   *
   * @fn fSetRLCHeader
   * @param header
  */
//     void fSetRLCHeader(cRLCHeader theader);
    /**
   * @brief
   *
   * @fn fGetRLCHeader
   * @return cHeader
  */
//     cRLCHeader& fGetRLCHeader();
    /**
   * @brief
   *
   * @fn fPrintRLCHeader
  */
//     void fPrintRLCHeader();

    /**
   * @brief
   *
   * @fn fSetMACHeader
   * @param header
  */
//     void fSetMACHeader(cMACHeader theader);
    /**
   * @brief
   *
   * @fn fGetMACHeader
   * @return cHeader
  */
//     cMACHeader& fGetMACHeader();
    /**
   * @brief
   *
   * @fn fPrintMACHeader
  */

    bool fGetIsFragmented();
//     void fPrintMACHeader();
    void fDestroy();
    void fSetIsFragmented(bool tIsFragmented);

    eTrafficType fGetPktTrafficType();
    void fSetPktTrafficType(eTrafficType tType);

    void fSetReTxCount(int tCount);
    int fGetReTxCount();

    cByteSet& fGetPacketBits();
    int fGetPacketSizeInBytes();
    void fSetPacketBits(cByteSet tByteSet);

    void fGenerateRandomPacketBits(int size);

//     int fGetRLCPDUSize();
//     bool fGetIsPdcpSnAssigned();
//     void fSetIsPdcpSnAssigned();
//      bool fGetIsTcpSnAssigned();
//     void fSetIsTcpSnAssigned(bool x);

#ifdef USING_RRC
    void fSetRRCConnectionSetupPkt(RRCConnectionSetupPacket tSetupPkt);
    RRCConnectionSetupPacket fGetRRCConnectionSetupPkt();
#endif
};
string getString(eTrafficType trafficType);

#endif // CPACKET_H
