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

#include "../../include/Others/cPacketQueue.h"

cPacketQueue::cPacketQueue()
{
  mPackets.resize(0);
}

cPacketQueue::cPacketQueue(const cPacketQueue& tPktQueue)
{
  mPackets.resize(0);
  for(list<cPacket>::const_iterator it = tPktQueue.mPackets.begin();it!=tPktQueue.mPackets.end();it++)
  {
    mPackets.push_back(*it);
  }
}


cPacketQueue& cPacketQueue::operator=(const cPacketQueue& tPktQueue)
{
  this->mPackets.resize(0);
  for(list<cPacket>::const_iterator it = tPktQueue.mPackets.begin();it!=tPktQueue.mPackets.end();it++)
  {
    this->mPackets.push_back(*it);
  }
  return *this;
}


unsigned int cPacketQueue::fGetNumberOfPackets(void ) 
{
//   list<cPacket>::iterator it = mPackets.begin();
//   if((mPackets.size()==1)   && ((*it).fGetPacketSize()==0)){ return 0;}
//   else{ return mPackets.size();  }
    return mPackets.size();
}

unsigned int cPacketQueue::fGetSize(void )
{
  int size=0;
  for(list<cPacket>::iterator ipkt=mPackets.begin();ipkt!=mPackets.end();ipkt++)
  {
    size +=(*ipkt).fGetPacketSizeInBytes();
  }
  return size;
}

list < cPacket > cPacketQueue::fGetPackets(void ) const
{
    return mPackets;
}

void cPacketQueue::fSetPackets (list<cPacket > tPkts)
{
  mPackets.resize(0);
  mPackets = tPkts;
}
  
void cPacketQueue::fAddPacket(cPacket &tpacket)
{
  cPacket tPkt=tpacket;  
  mPackets.push_back(tPkt);;
}

cPacketQueue::~cPacketQueue(void )
{
  mPackets.clear();
}

void cPacketQueue::fAddPktQueue(cPacketQueue tPktQueue)
{
    if(mPackets.size()==0)
    {
      this->mPackets=tPktQueue.fGetPackets();  
    }
    else
    {
      list<cPacket> tPkt=tPktQueue.fGetPackets();
      for(list<cPacket>::iterator it=tPkt.begin();it!=tPkt.end();it++)
      {
	  mPackets.insert(mPackets.end(),(*it));
      }
    }
}

// void cPacketQueue::fSetbool(int i)
// {
//   int j=0;
//       for(list<cPacket>::iterator it=mPackets.begin();it!=mPackets.end();it++)
//       {
// 	if(j==i)
// 	{
// 	  (it)->fSetIsPdcpSnAssigned();
// 	  break; 
// 	}
// 	j++;
//       }
// }
// vector<bool> cPacketQueue::fGetbool()
// {
//   mPQ.resize(mPackets.size());
//   int i=0;
//        for(list<cPacket>::iterator it=mPackets.begin();it!=mPackets.end();it++)
//        {
// 	 
// 	  mPQ[i]=(*it).fGetIsPdcpSnAssigned();
// 	  i++;
//        }
//   return mPQ;     
// }

cPacket cPacketQueue::fGetPeekPacket()
{
  if(mPackets.size()>0){
      list<cPacket>::iterator tPkt=mPackets.begin();
      return (*tPkt);
  }
  else
  {
    cPacket tpkt;
    return tpkt;
  }
}

void cPacketQueue::fDequeuePkt()
{
  mPackets.pop_front();
}


void cPacketQueue::fDeleteAllPkts()
{
  for(int i=0;i<(int)mPackets.size();i++)
  {
    mPackets.pop_front();
  }
  mPackets.clear();
}

void cPacketQueue::fUpdatePeerPacket(cPacket tPkt)
{
  list<cPacket>::iterator itPkt= mPackets.begin();
  *itPkt = tPkt;
}

// int cPacketQueue::fGetLowestSN()
// {
//   int tLowSN = (*mPackets.begin()).fGetRLCHeader().fGetSequenceNumber();
//    for(list<cPacket>::iterator it = mPackets.begin();it!=mPackets.end();it++)
//   {
//     if(tLowSN > (*it).fGetRLCHeader().fGetSequenceNumber())
//     {
//       tLowSN = (*it).fGetRLCHeader().fGetSequenceNumber();
//     }
//   }
//   return tLowSN;
// }
void cPacketQueue::fResizeToZero(void)
{
  mPackets.resize(0);
}
// vector < bool > cPacketQueue::fGetbool(void ) const
// {
//     return mPQ;
// }
// void cPacketQueue::fSetbool (int tIndex)
// {
// //   mPQ.resize(0);
//   mPQ[tIndex] = true;
// }
