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

#include "../../include/Others/cByteSet.h"

cByteSet::cByteSet()
{
    mBytes.resize(0);
}

void cByteSet::fSetBytes(vector<cByte> tBytes)
{
    mBytes.resize(0);
    mBytes = tBytes;
}

vector<cByte> cByteSet::fGetBytes()
{
    return mBytes;
}

vector<cByte> cByteSet::fGetBytesConst() const
{
    return mBytes;
}

bool  cByteSet::fGetNthBitInByte(int tN, int tByte)
{
    return mBytes[tByte].fGetBitAtPostion(tN);
}

bool  cByteSet::fGetNthBit(int tN)
{
    int tByte = tN / 8;
    tN = tN % 8;
    return fGetNthBitInByte(tN, tByte);
}

void  cByteSet::fUpdateNthBitInByte(bool tBit,int tN, int tByte)
{
    mBytes[tByte].fSetBitAtPostion(tBit, tN);
}

void  cByteSet::fUpdateNthBit(bool tBit,int tN)
{
    int tByte = tN / 8;
    int tVal  = tN % 8;
    fUpdateNthBitInByte(tBit,tVal,tByte);

}

void cByteSet::fUpdateNBits(long unsigned int tVal, unsigned int tFrom, unsigned int tTo)
{
    int tByteStart =  tFrom/8;
    int tByteEnd = tTo / 8;

    int tByteFrom =  tFrom % 8;
    int tByteTo = tTo % 8;
    int tByteNo = tByteEnd - tByteStart; //to support 3-byte case
    if(tByteEnd == tByteStart)
    {
        fUpdateNBitsInByte(tVal,tByteFrom,tByteTo,tByteEnd);
    }
    else if(tByteNo == 1)
    {
        int temp1 = tFrom % 8;
        int temp2 = (tTo % 8)+1;
        int tempVal = tVal >> temp2;
        fUpdateNBitsInByte(tempVal,temp1,7,tByteEnd-1);
        int tempVal2 = tempVal << temp2;
        int tempVal3 = tempVal2 ^ tVal;
        fUpdateNBitsInByte(tempVal3,0,temp2-1,tByteEnd);
    }
    else if(tByteNo == 2)
    {
        int temp1 = tFrom % 8;
        int temp2 = (tTo % 8)+1+8;
        int tempVal = tVal >> temp2;
        fUpdateNBitsInByte(tempVal,temp1,7,tByteEnd-2);
        int tempVal2 = tempVal << temp2;
        int tempVal3 = tempVal2 ^ tVal;
        temp2 = (tTo % 8)+1;
        int tempVal4 = tempVal3 >> temp2;
        fUpdateNBitsInByte(tempVal4,0,7,tByteEnd-1);
        int tempVal5 = tempVal4 << temp2;
        int tempVal6 = tempVal5 ^ tVal;
        fUpdateNBitsInByte(tempVal6,0,temp2-1,tByteEnd);
    }
   else if(tByteNo == 3)   // Included by mohana for the header length of four bytes
    {
      int temp1=tFrom % 8;
      int temp2=(tTo % 8)+1+8+8;
      int tempVal = tVal >> temp2;
      fUpdateNBitsInByte(tempVal,temp1,7,tByteEnd-3);
      int tempVal2=tempVal<<temp2;
      int tempVal3=tempVal2 ^ tVal;
      temp2=(tTo % 8)+1+8;
      int tempVal4=tempVal3 >> temp2;
      fUpdateNBitsInByte(tempVal4,0,7,tByteEnd-2);
      int tempVal5=tempVal4<<temp2;
      int tempVal6=tempVal5 ^ tVal;
      temp2=(tTo % 8)+1;
      int tempVal7 = tempVal6 >> temp2;
      fUpdateNBitsInByte(tempVal7,0,7,tByteEnd-1);
      int tempVal8= tempVal7 << temp2;
      int tempVal9=tempVal8 ^ tVal;
      fUpdateNBitsInByte(tempVal9,0,temp2-1,tByteEnd);
    }
    else
    {
        cout  << "in fUpdateBits: this case is not supported" << endl;
        exit(-1);
    }
}

void cByteSet::fUpdateNBitsInByte(unsigned int tVal, unsigned int tFrom, unsigned int tTo, unsigned int tByte)
{
    mBytes[tByte].fUpdateBits((unsigned char)tVal, tFrom, tTo);
}

void  cByteSet::fAddByte(cByte tByte)
{
    mBytes.push_back(tByte);
}

void cByteSet::fAddNBytes(int tN)
{
  if(tN>0)
  {
    for(int i=0; i<tN;i++) 
    {
      cByte tByte;
      fAddByte(tByte);
    }           
  }  
  else
  {
    cout<<"WARN:::Byte to Add should not be less than 1"<<endl;
  }  
}

void cByteSet::fCheckAndAdd(int tN)
{
  if(tN/8 + 1 > (int) mBytes.size())
  {
    fAddNBytes((tN/8 + 1)-mBytes.size());
  }
  else
  {

  }
}

cByte cByteSet::fGetNthByte(int tN)
{
    return mBytes[tN];
}

ostream& operator<<( ostream &output, const cByteSet &D )
{
    assert(D.fGetBytesConst().size() != 0) ;

    for(int i=0;i<(int)(D.fGetBytesConst().size());i++)
    {
        cout<<D.fGetBytesConst()[i]<<endl;
    }
    return output;
}

void cByteSet::fGenerateRandomBytes(int tNBytes)
{
    for(int i=0;i<tNBytes;i++)
    {
        cByte tByte;
        tByte.fGenerateRamdomBits();
        mBytes.push_back(tByte);
    }
}

void  cByteSet::fAddBytesAtFirst(vector<cByte> tBytes)
{
    if(tBytes.size() != 0)
    {
        vector<cByte> tByteSet;
        tByteSet.resize(0);
        for(int i=0;i<(int)tBytes.size();i++)
        {
            tByteSet.push_back(tBytes[i]);
        }
        for(int i=0;i<(int)mBytes.size();i++)
        {
            tByteSet.push_back(mBytes[i]);
        }
        mBytes.resize(0);
        for(int i=0;i<(int)tByteSet.size();i++)
        {
            mBytes.push_back(tByteSet[i]);
        }
    }
}

void cByteSet::fAddBytesAtLast(vector<cByte> tBytes)
{
    if(tBytes.size() != 0)
    {
        for(int i=0;i<(int)tBytes.size();i++)
        {
            mBytes.push_back(tBytes[i]);
        }
    }
}

unsigned int cByteSet::fGetBits(int tFrom, int tTo)
{
    assert(tFrom <= tTo);

    unsigned int tempFinal;
    int toByte = tTo / 8;
    int fromByte = tFrom / 8;
    int tByteNo = toByte - fromByte;

    if(toByte == fromByte) //1-byte case
    {
        int tempTo, tempFrom;
        tempTo = tTo % 8;
        tempFrom = tFrom % 8;

        tempFinal = mBytes[fromByte].fGetBits(tempFrom,tempTo);
    }
    else if (tByteNo == 1) //2-byte case
    {
        assert( fromByte == (toByte - 1));
        int tempFrom = tFrom % 8;
        unsigned int temp = mBytes[fromByte].fGetBits(tempFrom,7);
        int tempTo = tTo % 8;
        temp <<= (tempTo +1);
        unsigned int temp2 = mBytes[toByte].fGetBits(0,tempTo);

        tempFinal = temp  | temp2;
    }
    else if (tByteNo == 2) //3-byte case
    {
        int tempFrom = tFrom % 8;
        int tempTo = tTo % 8;

        unsigned int temp = mBytes[fromByte].fGetBits(tempFrom,7);
        temp <<= (8+tempTo+1);

        unsigned int temp2 = mBytes[fromByte+1].fGetBits(0,7);
        temp2 <<= (tempTo+1);

        unsigned int temp3 = temp  | temp2;
        unsigned int temp4 = mBytes[toByte].fGetBits(0,tempTo);
        tempFinal = temp3  | temp4;
    }
    else if(tByteNo == 3)   // Included by mohana for the header length of four bytes
    {
      int tempFrom=tFrom % 8;
      int tempTo=tTo % 8;
      
      unsigned int temp = mBytes[fromByte].fGetBits(tempFrom,7);
      temp <<= (8+tempTo+1+8);
      
      
//       unsigned int temp2=mBytes[fromByte+1].fGetBits(0,7);
//       temp <<= (8+tempTo+1);
      unsigned int temp2=mBytes[fromByte+1].fGetBits(0,7);
      temp2 <<= (8+tempTo+1);
      
      unsigned int temp3 = temp  | temp2;
      
      unsigned int temp4=mBytes[fromByte+2].fGetBits(0,7);
      temp4 <<= (tempTo+1);
      
      
      unsigned int temp5 = temp3 | temp4;
       
      unsigned int temp6=mBytes[toByte].fGetBits(0,tempTo);
      
      tempFinal = temp5 | temp6;
      
    }
    else
    {
        cout << "In fGetBits: this case is not supported" << endl;
        exit(-1);
    }
    
    return tempFinal;
}

vector<cByte> cByteSet::fGetAndRemoveLastNBytes(int tBytes)
{
    assert((tBytes != 0) || (tBytes <= (int)mBytes.size()));
    
    vector<cByte> retBytes;
    retBytes.resize(0);
    
    vector<cByte> remBytes;
    remBytes.resize(0);
    
    int tRemBytes = mBytes.size() - tBytes;
    int i=0;
    for(;i<tRemBytes;i++)
    {
        remBytes.push_back(mBytes[i]);
    }
    
    for(;i<(int)mBytes.size();i++)
    {
        retBytes.push_back(mBytes[i]);
    }
    
    fReset();
    for(int i=0;i<(int)remBytes.size();i++)
    {
        mBytes.push_back(remBytes[i]);
    }
    
    return retBytes;
}


vector<cByte> cByteSet::fGetFirstNBytes(int tBytesSize)
{
    assert((tBytesSize != 0) || (tBytesSize <= (int)mBytes.size()));

    vector<cByte> retBytes; retBytes.resize(0);
    int i=0;
    for(;i<tBytesSize;i++)
    {
        retBytes.push_back(mBytes[i]);
    }
    return retBytes;
}


vector<cByte> cByteSet::fGetAndRemoveFirstNBytes(int tBytesSize)
{
  assert((tBytesSize != 0) || (tBytesSize <= (int)mBytes.size()));

    vector<cByte> retBytes; retBytes.resize(0);
    int i=0;
    for(;i<tBytesSize;i++)
    {
        retBytes.push_back(mBytes[i]);
    }
    mBytes.erase(mBytes.begin(),mBytes.begin()+tBytesSize);

    return retBytes;
}

void cByteSet::fReset()
{
    mBytes.resize(0);
}

//WarningFix: unused function

// char* cByteSet::fGetBytesInArray()
// {
//     int size  = mBytes.size();
//     unsigned char a[size];
//     for(int i=0;i<(int)mBytes.size();i++)
//     {
//         a[i] = mBytes[i].fGetChar();
//     }
// }

int cByteSet::fGetSize()
{
    return mBytes.size();
}
