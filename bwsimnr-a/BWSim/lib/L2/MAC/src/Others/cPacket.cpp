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

// #include "../../../include/ProtocolStack/Others/cPacket.h"
// #include "../../../include/Sim/SimInclude.h"
// #include "../../../include/ProtocolStack/Header/cRLCHeader.h"
#include "../../include/Others/cPacket.h"
 
cPacket::cPacket()
{
  mSize=0;
  mTimeStamp=0;
//   mRLCHeader = cRLCHeader();
//   mTCPHeader=cTCPHeader();
//   mPDCPHeader =cPDCPHeader();
//   mMACHeader =cMACHeader();
  mIsFragmented = false;
  mReTxCount = -1;
  mPacketBits = cByteSet();
//   mIsPdcpSnAssigned = false;
//   mIsTcpSnAssigned = false;
  
}

cPacket::cPacket(const cPacket& tPkt)
{
  mSize = tPkt.mSize;
  mIsFragmented = tPkt.mIsFragmented;
  mTimeStamp = tPkt.mTimeStamp;
 /* mMACHeader = tPkt.mMACHeader;
  mRLCHeader = tPkt.mRLCHeader;
  mPDCPHeader = tPkt.mPDCPHeader;
  mTCPHeader = tPkt.mTCPHeader;
 */ mTrafficType=tPkt.mTrafficType;
  mReTxCount = tPkt.mReTxCount;
  mPacketBits = tPkt.mPacketBits;
//   mIsPdcpSnAssigned = tPkt.mIsPdcpSnAssigned;
//   mIsTcpSnAssigned = tPkt.mIsTcpSnAssigned;
}

cPacket& cPacket::operator=(cPacket const& tPkt)
{
      this->mIsFragmented = tPkt.mIsFragmented;
      this->mSize = tPkt.mSize;
      this->mTimeStamp = tPkt.mTimeStamp;
//       this->mMACHeader = tPkt.mMACHeader;
//       this->mRLCHeader = tPkt.mRLCHeader;
//       this->mPDCPHeader = tPkt.mPDCPHeader;
//       this->mTCPHeader = tPkt.mTCPHeader;
      this->mTrafficType =tPkt.mTrafficType;
      this->mReTxCount = tPkt.mReTxCount;
      this->mPacketBits = tPkt.mPacketBits;
//       this->mIsPdcpSnAssigned = tPkt.mIsPdcpSnAssigned;
//       this->mIsTcpSnAssigned = tPkt.mIsTcpSnAssigned;
      return *this;
}
/*
cMACHeader& cPacket::fGetMACHeader()
{
  return mMACHeader;
}*/

int cPacket::fGetPacketSize()
{
  return mSize;
}

int cPacket::fGetPacketTimeStamp()
{
  return mTimeStamp;
}
// cTCPHeader& cPacket::fGetTCPHeader()
// {
//   return mTCPHeader;
// }
// void cPacket::fPrintTCPHeader()
// {
//   cout<<"cPacket::fPrintTCPHeader() : Not Implemented";
// }
// void cPacket::fSetTCPHeader(cTCPHeader theader)
// {
//   mTCPHeader = theader;
// }

// cPDCPHeader& cPacket::fGetPDCPHeader()
// {
// //   cout<<"entering getpdcpheader"<<endl;
//   return mPDCPHeader;
// }
// cRLCHeader& cPacket::fGetRLCHeader()
// {
//   return mRLCHeader;
// }
// void cPacket::fPrintMACHeader()
// {
//  cout<<"cPacket::fPrintMACHeader() : Not Implemented";
// }
// void cPacket::fPrintPDCPHeader()
// {
//   cout<<"cPacket::fPrintPDCPHeader() : Not Implemented";
// }
// void cPacket::fPrintRLCHeader()
// {
//   mRLCHeader.fPrintRLCHeader();
// }

void cPacket::fSetPacketSize(int tSize)
{
  mSize=tSize;
}
void cPacket::fSetPacketTimeStamp(int tTimeStamp)
{
  mTimeStamp =tTimeStamp;
}

void cPacket::fSetArrivalTime(double tArrivalTime)
{
  mArrivalTime = tArrivalTime;
}

double cPacket::fGetArrivalTime() {
  return mArrivalTime;
}

// void cPacket::fSetPDCDPHeader(cPDCPHeader theader)
// {
//   mPDCPHeader = theader;
// }
// void cPacket::fSetRLCHeader(cRLCHeader theader)
// {	
//   mRLCHeader = theader;
// }
// void cPacket::fSetMACHeader(cMACHeader theader)
// {
//   mMACHeader = theader;
// }
cPacket::~cPacket()
{

}

bool cPacket::fGetIsFragmented()
{
  return mIsFragmented;
}

void cPacket::fSetIsFragmented(bool tIsFragmented)
{
  mIsFragmented = tIsFragmented;
}

eTrafficType cPacket::fGetPktTrafficType()
{
  return mTrafficType ;
}

void cPacket::fSetPktTrafficType(eTrafficType tType)
{
  mTrafficType = tType;
}

void cPacket::fDestroy()
{

}

int cPacket::fGetReTxCount()
{
  return mReTxCount;
}
void cPacket::fSetReTxCount(int tCount)
{
  mReTxCount = tCount;
}

cByteSet& cPacket::fGetPacketBits()
{
  return mPacketBits;
}

int cPacket::fGetPacketSizeInBytes()
{
//   return fGetPacketBits().fGetBytes().size();
  return mSize;
}


void cPacket::fSetPacketBits(cByteSet tByteSet)
{
    mPacketBits = tByteSet;
}

void cPacket::fGenerateRandomPacketBits(int size)
{ 
  mPacketBits.fGenerateRandomBytes(size);
  mPacketBits.fUpdateNBits((int)mTrafficType,0 ,7);
  mPacketBits.fUpdateNBits(mTimeStamp,8 ,23);
}

// int cPacket::fGetRLCPDUSize()
// {
//   if(mRLCHeader.fGetRLCHeaderBytes().fGetBytes().size() == 0)
//   {
//     cout<<"[both:] RLC Header is not created for RLC PDU"<<endl; exit(0);
//   }
//   return mPacketBits.fGetBytes().size();
// }
// void cPacket::fSetIsPdcpSnAssigned()
// {
//   mIsPdcpSnAssigned=true;
// }
// bool cPacket::fGetIsPdcpSnAssigned()
// {
// //   cout<<"entering into fgetPDCPSNassgned"<<endl;
//   return mIsPdcpSnAssigned;
// }
// void cPacket::fSetIsTcpSnAssigned(bool x)
// {
//   mIsTcpSnAssigned=x;
// }
// bool cPacket::fGetIsTcpSnAssigned()
// {
//   cout<<"entering into mIsTcpSnAssigned"<<endl;
//   return mIsTcpSnAssigned;
// }

#ifdef USING_RRC
RRCConnectionSetupPacket cPacket::fGetRRCConnectionSetupPkt()
{
  return mRRCConnectionSetupPkt;
}
void cPacket::fSetRRCConnectionSetupPkt(RRCConnectionSetupPacket tSetupPkt)
{
  mRRCConnectionSetupPkt = tSetupPkt;
}
#endif

string getString(eTrafficType trafficType)
{
    string toReturn = "";
    switch(trafficType)
    {
        case TRAFFIC_TYPE_XR:
            toReturn = "XR";
            break;
        case TRAFFIC_TYPE_CBR:
            toReturn = "CBR";
            break;
        case TRAFFIC_TYPE_FTP:
            toReturn = "FTP";
            break;
        case TRAFFIC_TYPE_HTTP:
            toReturn = "HTTP";
            break;
        case TRAFFIC_TYPE_INFINITE_BUFFER:
            toReturn = "INFINITE_BUFFER";
            break;
        case TRAFFIC_TYPE_VIDEO:
            toReturn = "VIDEO";
            break;
        case TRAFFIC_TYPE_VOIP:
            toReturn = "VOIP";
            break;
        default: 
            cout<<"UnKnown Traffic Model..."<<endl;abort();
            break;
    }
    return toReturn;
}
