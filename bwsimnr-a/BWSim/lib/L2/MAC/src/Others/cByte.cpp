/*************************************************************************
*
* CEWiT CONFIDENTIAL
* __________________
*
* All Rights Reserved © 2014 CEWiT, India
*
\ NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
* and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
* Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
* express, printed and signed license for use is strictly forbidden.
*/

#include "../../include/Others/cByte.h"

cByte::cByte()
{
  mByte = 0;
}

void cByte::fSetBitAtPostion(bool tBit, int tPos)
{
  assert( (tPos >= 0) && (tPos < 8) );
  unsigned char a=128;
  if(tBit == 1)
    mByte = mByte | (a >> tPos);
  else
    mByte = mByte & ~(a >> tPos);
}

bool cByte::fGetBitAtPostion(int tPos)
{
  assert( (tPos >= 0) && (tPos < 8) );
  unsigned char a=128;
  unsigned char b=128;
  if((mByte & (a >> tPos)) == (b>>tPos)) return 1;
  else	return 0;
}

unsigned char cByte::fGetByte() const
{
  return mByte;
}


ostream& operator<<( ostream &output, const cByte &D )
{ 
  int tCount = 0;
  unsigned char temp = D.fGetByte();
  while(tCount < 8)
  {
	  if ( (temp & 128) == 128 ) cout<<1;
	  else	cout<<0;
	  
	  temp <<= 1;
	  tCount++;
  }
    return output;            
}

void cByte::fGenerateRamdomBits()
{
  for(int i=0;i<8;i++)
  {
    if((randi(0,RAND_MAX)%2) == 0)	fSetBitAtPostion(0,i);
    else fSetBitAtPostion(1,i);
  }
}

unsigned int cByte::fGetBits(unsigned tFrom, unsigned int tTo)
{
  assert((tFrom < tTo) || (tTo < 8));
  //int tRet = 0;
  unsigned char temp = 0;
  
  for(int tPos = tFrom ; tPos<=(int)tTo; tPos++)
  {
  unsigned char a=128;
  temp = temp | (a >> tPos);
  }
  temp = mByte & temp;
  temp >>= (7-tTo);
  return (unsigned int)temp; 
}

void cByte::fUpdateBits(unsigned char tVal,unsigned int tFrom, unsigned int tTo)
{
  assert((tFrom < tTo) || (tTo < 8));
  //int nBits =0 ;
  int tBitsAvail = tTo - tFrom + 1;
  assert(tVal < (int)pow(2,tBitsAvail));   
  
  int cnt=0;
  for(int i=(int)tTo ; (i >= (int)tFrom) && (cnt<tBitsAvail) ; --i)
  {
//      cout<<"to : "<<tTo << " From : "<<tFrom << "Val : " << (tVal >> cnt ) << " (tVal >> cnt ) & 1) : "<< ((tVal >> cnt ) & 1) << " i = "<<i<<endl;
    if(((tVal >> cnt ) & 1)  == 1)
	fSetBitAtPostion(1, i);
    else
	fSetBitAtPostion(0, i);
    cnt++;
  }
}

void cByte::fReset()
{
  fUpdateBits(0,0, 7);
}

char cByte::fGetChar()
{
  return mByte;
}

void cByte::fSetChar(char tVal)
{
  mByte = (unsigned char)tVal;
}
