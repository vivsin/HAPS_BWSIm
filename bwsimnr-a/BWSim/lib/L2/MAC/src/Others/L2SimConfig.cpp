/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved Â© 2012 CEWiT, India
 *
 * NOTICE: All information contained herein is, and remains the property of Center of Excellence in Wireless Technology (CEWiT) and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT. Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior, express, printed and signed license for use is strictly forbidden.
 */
 
#include "../../include/Others/L2SimConfig.h"
 
uint16_t fComputeSFNSNFromTTICount(int subframeNumber)
{
    //assert(subframeNumber < 40950);
  
  uint16_t SFNVal = subframeNumber/10;
    SFNVal = SFNVal % 1024;
  int SFVal = subframeNumber % 10;
  vector<cByte> tRet;
  tRet.resize(2);
  tRet[1].fUpdateBits((unsigned char) SFVal, 4, 7);
  
  vector<cByte> temp = fGetVecByteFromUNIT(SFNVal);
  int val =  temp[1].fGetBits(4,7);
  tRet[1].fUpdateBits((unsigned char) val, 0, 3);
  SFNVal = SFNVal >> 4;
  tRet[0].fUpdateBits((unsigned char)SFNVal,0,7);
  
  uint16_t ret = fGetUNIT16FromVecByte(tRet);
  
    //cout<<"SFNSF "<<(int)ret<<endl;
  return ret;
}

int fGetTTICountFromSFNSF(uint16_t tSFNSF)
{
  uint16_t temp = tSFNSF;
  temp = temp >> 4;
  uint16_t tSFN =  temp;
  temp = temp << 4;
  uint16_t tSF = tSFNSF ^ temp;
    //cout<<"TTI "<<((tSFN * 10) + tSF)<<endl;
  return ((tSFN * 10) + tSF );
}

vector<cByte> fGetVecByteFromUNIT(uint8_t tVal)
{
  vector<cByte> tByte;
  tByte.resize(1);
  cByte temp;
  int cnt=0;
  for(int i=7 ; (i >= 0) && (cnt<8) ; --i)
  {
    if(((tVal >> cnt ) & 1)  == 1)
      temp.fSetBitAtPostion(1, i);
    else
      temp.fSetBitAtPostion(0, i);
    cnt++;
  }
  tByte[0] = temp;
  return tByte;
}

vector<cByte> fGetVecByteFromUNIT(uint16_t tVal)
{
  vector<cByte> tByte;
  tByte.resize(2);
  cByte temp;
  int cnt=0;
  for(int i=7 ; (i >= 0)  ; --i)
  {
    if(((tVal >> cnt ) & 1)  == 1)
      temp.fSetBitAtPostion(1, i);
    else
      temp.fSetBitAtPostion(0, i);
    cnt++;
  }
  tByte[1]=temp;
  temp.fReset();
  for(int i=7 ; (i >= 0)  ; --i)
  {
    if(((tVal >> cnt ) & 1)  == 1)
      temp.fSetBitAtPostion(1, i);
    else
      temp.fSetBitAtPostion(0, i);
    cnt++;
  }
  tByte[0] = temp;
  return tByte;
}

vector<cByte> fGetVecByteFromUNIT(uint32_t tVal)
{
  vector<cByte> tByte;
  tByte.resize(4);
  cByte temp;
  int cnt=0;
  for(int i=7 ; (i >= 0) ; --i)
  {
    if(((tVal >> cnt ) & 1)  == 1)
      temp.fSetBitAtPostion(1, i);
    else
      temp.fSetBitAtPostion(0, i);
    cnt++;
  }
  tByte[3]=temp;
  temp.fReset();
  for(int i=7 ; (i >= 0)  ; --i)
  {
    if(((tVal >> cnt ) & 1)  == 1)
      temp.fSetBitAtPostion(1, i);
    else
      temp.fSetBitAtPostion(0, i);
    cnt++;
  }
  tByte[2]=temp;
  temp.fReset();
  for(int i=7 ; (i >= 0)  ; --i)
  {
    if(((tVal >> cnt ) & 1)  == 1)
      temp.fSetBitAtPostion(1, i);
    else
      temp.fSetBitAtPostion(0, i);
    cnt++;
  }
  tByte[1]=temp;
  temp.fReset();
  for(int i=7 ; (i >= 0)  ; --i)
  {
    if(((tVal >> cnt ) & 1)  == 1)
      temp.fSetBitAtPostion(1, i);
    else
      temp.fSetBitAtPostion(0, i);
    cnt++;
  }
  tByte[0] = temp;
  return tByte;
}

uint16_t fGetUNIT16FromVecByte(vector<cByte> tByte)
{
  assert(tByte.size() == 2);
  uint16_t retVal=0; //WarningFix:  [-Wmaybe-uninitialized]
  uint16_t tVal = 65535;
  uint16_t cnt=0;
  
  for(int j=0;j<8;j++)
  {
    if(tByte[0].fGetBitAtPostion(j) == 1)
      retVal = retVal | (tVal >> cnt);
    else
      retVal = retVal & ~(tVal >> cnt);
    cnt++;
  }
  
  for(int j=0;j<8;j++)
  {
    if(tByte[1].fGetBitAtPostion(j) == 1)
      retVal = retVal | (tVal >> cnt);
    else
      retVal = retVal & ~(tVal >> cnt);
    cnt++;
  }
  
  return retVal;
}

uint32_t fGetUNIT32FromVecByte(vector<cByte> tByte)
{
  assert(tByte.size() == 4);
  
  uint32_t retVal=0; //WarningFix:  [-Wmaybe-uninitialized]
  uint32_t tVal = 4294967295u; //WarningFix: 'u' signifies unsigned
  uint32_t cnt=0;
  
  for(int j=0;j<8;j++)
  {
    if(tByte[0].fGetBitAtPostion(j) == 1)
      retVal = retVal | (tVal >> cnt);
    else
      retVal = retVal & ~(tVal >> cnt);
    cnt++;
  }
  
  for(int j=0;j<8;j++)
  {
    if(tByte[1].fGetBitAtPostion(j) == 1)
      retVal = retVal | (tVal >> cnt);
    else
      retVal = retVal & ~(tVal >> cnt);
    cnt++;
  }
  
  for(int j=0;j<8;j++)
  {
    if(tByte[2].fGetBitAtPostion(j) == 1)
      retVal = retVal | (tVal >> cnt);
    else
      retVal = retVal & ~(tVal >> cnt);
    cnt++;
  }
  
  for(int j=0;j<8;j++)
  {
    if(tByte[3].fGetBitAtPostion(j) == 1)
      retVal = retVal | (tVal >> cnt);
    else
      retVal = retVal & ~(tVal >> cnt);
    cnt++;
  }
  
  return retVal;
}

uint8_t fGetUNIT8FromVecByte(vector<cByte> tByte)
{
  assert(tByte.size() == 1);
  
  uint8_t retVal=0; //WarningFix:  [-Wmaybe-uninitialized]
  uint8_t tVal = 255;
  uint8_t cnt=0;
  
  for(int j=0;j<8;j++)
  {
    if(tByte[0].fGetBitAtPostion(j) == 1)
      retVal = retVal | (tVal >> cnt);
    else
      retVal = retVal & ~(tVal >> cnt);
    cnt++;
  }
  
  return retVal;
}
//! getCQIindexForCW1 returns CQI index for second CW using the CQI index for the 1st CW and the quantized offset level of the 2nd CW from the 1st CW
uint8_t getCQIindexForCW1(unsigned int CQIIndexForCW0,unsigned int DifferentialCQI)
{
  ivec offset;
  offset.set_size(8);
  offset="0 1 2 3 -4 -3 -2 -1";
  return((uint8_t)((int)CQIIndexForCW0-offset(DifferentialCQI)));
}

int fFindDataAllocated(vector< std::vector< int > > arg1, int arg2)
{
  int retVal = 0;
  for (int i = 0; i < (int)arg1.size(); i++)
  {
    if(arg2 == arg1[i][0])
    {
      retVal =  retVal +  arg1[i][1];
    }
  }
  return retVal;
}

eTrafficType getTrafficType(string name)
{
    if(name=="TRAFFIC_TYPE_VOIP")
        return TRAFFIC_TYPE_VOIP;
    else if(name=="TRAFFIC_TYPE_FTP")
        return TRAFFIC_TYPE_FTP;
    else if(name=="TRAFFIC_TYPE_INFINITE_BUFFER")
        return TRAFFIC_TYPE_INFINITE_BUFFER;
    else if(name=="TRAFFIC_TYPE_VIDEO")
        return TRAFFIC_TYPE_VIDEO;
    else if(name=="TRAFFIC_TYPE_HTTP")
        return TRAFFIC_TYPE_HTTP;
    else if(name=="TRAFFIC_TYPE_CBR")
        return TRAFFIC_TYPE_CBR;
    else if(name=="TRAFFIC_TYPE_XR")
        return TRAFFIC_TYPE_XR;
    cout<<"Unknown Traffic Type..."<<name<<endl;abort();
}

sL2SimConfig::sL2SimConfig(string l2ConfigFile,string tcpConfigFile,ivec ueList,int nBS,int simTime,double slotDuration)
{
    l2SupportConfigFile = l2ConfigFile;
    Array<Array<string>> trafficType;
    parse(l2ConfigFile,"mTrafficType",trafficType);
    
    for(int cnt1=0;cnt1<trafficType.length();cnt1++)
    {
        vector<eTrafficType> traffic;
        for(int cnt=0;cnt<trafficType(cnt1).length();cnt++)
        {
            eTrafficType tTrafficType = getTrafficType(trafficType(cnt1)(cnt));
            traffic.push_back(tTrafficType);
            switch(tTrafficType)
            {
                case TRAFFIC_TYPE_VIDEO:
                    if(!videoConfig.isInitialised) 
                    {
                        if(currentTaskid==0)    cout<<"Traffic Model: "<<trafficType(cnt1)(cnt)<<endl;
                        videoConfig.init(l2ConfigFile);
                    }
                    break;
                case TRAFFIC_TYPE_HTTP:
                    if(!httpConfig.isInitialised)
                    {
                        if(currentTaskid==0)    cout<<"Traffic Model: "<<trafficType(cnt1)(cnt)<<endl;
                        httpConfig.init(l2ConfigFile);
                    }
                    break;
                case TRAFFIC_TYPE_FTP:
                    if(!ftpConfig.isInitialised) 
                    {
                        if(currentTaskid==0)    cout<<"Traffic Model: "<<trafficType(cnt1)(cnt)<<endl;
                        ftpConfig.init(l2ConfigFile,tcpConfigFile);
                    }
                    break;
                case TRAFFIC_TYPE_CBR:
                    if(!cbrConfig.isInitialised) 
                    {
                        if(currentTaskid==0)    cout<<"Traffic Model: "<<trafficType(cnt1)(cnt)<<endl;
                        cbrConfig.init(l2ConfigFile);
                    }
                    break;
                case TRAFFIC_TYPE_XR:
                    if(!xrConfig.isInitialised) 
                    {
                        if(currentTaskid==0)    cout<<"Traffic Model: "<<trafficType(cnt1)(cnt)<<endl;
                        xrConfig.init(l2ConfigFile);
                    }
                    break;
                case TRAFFIC_TYPE_INFINITE_BUFFER:
                case TRAFFIC_TYPE_VOIP:
                    if(currentTaskid==0)    cout<<"Traffic Model: "<<trafficType(cnt1)(cnt)<<endl;
                    break;
                default:
                    cout<<"Unknown Traffic Model.."<<endl;abort();
                    break;
            }
        }
        mTrafficType.push_back(traffic);
        append(mNumberOfAppPerRB,trafficType(cnt1).length());
    }
    UElist = ueList;
    startTimePerUE = -1*ones_i(ueList.length());
    mSimulationTime = simTime;
    arrTimePereNB = -1*ones_i(nBS);
    interArrTimePerUE = -1*ones_i(ueList.length());
    frameDuration = slotDuration;
    nRadioBearers = mTrafficType.size();
}
