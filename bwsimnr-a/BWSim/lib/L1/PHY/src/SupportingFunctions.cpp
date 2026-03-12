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

#include "../include/SupportingFunctions.h"
#include "../../../Frozen/sequence/include/goldSequence.h"
#include <map>
std::map <int,ivec> goldSequence;

#ifdef ENABLE_COUT
#define ASSOCIATION_DEBUG
#endif



// ivec powerHeadroomNodeIDs; 
// vec powerHeadroomInDBPerNode;
// ivec powerHeadroomInDBPerNode_subframeCount;

void getSubbandsAndBWPartsNR(Array<ivec> &subbandSizes,int &numOfBWParts,int DLRBs)
{
    if(DLRBs<24)
    {
        numOfBWParts=0;
        subbandSizes.set_length(0);
    }
    else if(DLRBs<=275)
    {
        numOfBWParts=1;
        subbandSizes.set_length(numOfBWParts);
        subbandSizes(0) = getSubbandSizeAperiodicNR(DLRBs);
    }
    else
    {
        cout<<"Invalid nRBs.."<<endl;abort();
    }
}

void getSubbandsAndBWParts(Array<ivec> &subbandSizes,int &numOfBWParts,int DLRBs){
    
  return getSubbandsAndBWPartsNR(subbandSizes,numOfBWParts,DLRBs);  
  
  if(6<=DLRBs&&DLRBs<=7)
  {
    numOfBWParts=0;
    subbandSizes.set_length(0);
  }
  else if(7<DLRBs&&DLRBs<=10)
  {
    numOfBWParts=1;
    int bandSize=4;
    subbandSizes.set_length(1);
    int N=ceil_i((double)DLRBs/bandSize);
    subbandSizes(0).set_length(N);
    if(DLRBs%bandSize==0)
    {
      subbandSizes(0)=bandSize*ones_i(N);
    }
    else
    {
      subbandSizes(0).set_subvector(0,bandSize*ones_i(N-1));
      subbandSizes(0)(N-1)=DLRBs-(bandSize*(N-1));
    }
  }
  else if(10<DLRBs&&DLRBs<=26)
  {
    numOfBWParts=2;
    int bandSize=4;
    subbandSizes.set_length(2);
    ivec tempAllSubbandSizes;
    ivec subbandsInBWParts;
    subbandsInBWParts.set_length(numOfBWParts);
    int N=ceil_i((double)DLRBs/bandSize);
    tempAllSubbandSizes.set_length(N);
    if(DLRBs%bandSize==0)
    {
      tempAllSubbandSizes=bandSize*ones_i(N);
    }
    else
    {
      tempAllSubbandSizes.set_subvector(0,bandSize*ones_i(N-1));
      tempAllSubbandSizes(N-1)=DLRBs-(bandSize*(N-1));
    }
    int tempSize=ceil_i((double)N/numOfBWParts);
    for(int Cnt=0;Cnt<numOfBWParts-((tempSize*numOfBWParts)-N);Cnt++){
      subbandSizes(Cnt).set_length(tempSize);
      subbandSizes(Cnt)=tempAllSubbandSizes.get(Cnt*tempSize,(Cnt+1)*tempSize-1);
    }
    int offset=(numOfBWParts-((tempSize*numOfBWParts)-N))*tempSize;
    int initCnt=0;
    for(int Cnt=numOfBWParts-((tempSize*numOfBWParts)-N);Cnt<numOfBWParts;Cnt++,initCnt++){
      subbandSizes(Cnt).set_length(tempSize-1);
      subbandSizes(Cnt)=tempAllSubbandSizes.get(initCnt*(tempSize-1)+offset,(initCnt+1)*(tempSize-1)-1+offset);
    }
    
  }
  else if(26<DLRBs&&DLRBs<=63)
  {
    numOfBWParts=3;
    int bandSize=6;
    subbandSizes.set_length(3);
    ivec tempAllSubbandSizes;
    ivec subbandsInBWParts;
    subbandsInBWParts.set_length(numOfBWParts);
    int N=ceil_i((double)DLRBs/bandSize);
    tempAllSubbandSizes.set_length(N);
    if(DLRBs%bandSize==0)
    {
      tempAllSubbandSizes=bandSize*ones_i(N);
    }
    else
    {
      tempAllSubbandSizes.set_subvector(0,bandSize*ones_i(N-1));
      tempAllSubbandSizes(N-1)=DLRBs-(bandSize*(N-1));
    }
    int tempSize=ceil_i((double)N/numOfBWParts);
    for(int Cnt=0;Cnt<numOfBWParts-((tempSize*numOfBWParts)-N);Cnt++){
      subbandSizes(Cnt).set_length(tempSize);
      subbandSizes(Cnt)=tempAllSubbandSizes.get(Cnt*tempSize,(Cnt+1)*tempSize-1);
    }
    int offset=(numOfBWParts-((tempSize*numOfBWParts)-N))*tempSize;
    int initCnt=0;
    for(int Cnt=numOfBWParts-((tempSize*numOfBWParts)-N);Cnt<numOfBWParts;Cnt++,initCnt++){
      subbandSizes(Cnt).set_length(tempSize-1);
      subbandSizes(Cnt)=tempAllSubbandSizes.get(initCnt*(tempSize-1)+offset,(initCnt+1)*(tempSize-1)-1+offset);
    }
  }
  else if(63<DLRBs&&DLRBs<=110)
  {
    numOfBWParts=4;
    int bandSize=8;
    subbandSizes.set_length(4);
    ivec tempAllSubbandSizes;
    ivec subbandsInBWParts;
    subbandsInBWParts.set_length(numOfBWParts);
    int N=ceil_i((double)DLRBs/bandSize);
    tempAllSubbandSizes.set_length(N);
    if(DLRBs%bandSize==0)
    {
      tempAllSubbandSizes=bandSize*ones_i(N);
    }
    else
    {
      tempAllSubbandSizes.set_subvector(0,bandSize*ones_i(N-1));
      tempAllSubbandSizes(N-1)=DLRBs-(bandSize*(N-1));
    }
    int tempSize=ceil_i((double)N/numOfBWParts);
    for(int Cnt=0;Cnt<numOfBWParts-((tempSize*numOfBWParts)-N);Cnt++){
      subbandSizes(Cnt).set_length(tempSize);
      subbandSizes(Cnt)=tempAllSubbandSizes.get(Cnt*tempSize,(Cnt+1)*tempSize-1);
    }
    int offset=(numOfBWParts-((tempSize*numOfBWParts)-N))*tempSize;
    int initCnt=0;
    for(int Cnt=numOfBWParts-((tempSize*numOfBWParts)-N);Cnt<numOfBWParts;Cnt++,initCnt++){
      subbandSizes(Cnt).set_length(tempSize-1);
      subbandSizes(Cnt)=tempAllSubbandSizes.get(initCnt*(tempSize-1)+offset,(initCnt+1)*(tempSize-1)-1+offset);
    }
  }
  else
  {
    cout<<"[both:]error in getSubbandSizeAperiodicUESelected"<<endl;abort();
  }
  
  
};

void getSubbandsAndBWPartsUESelected(Array<ivec> &subbandSizes,int &numOfBWParts,int DLRBs){
  
  if(6<=DLRBs&&DLRBs<=7)
  {
    numOfBWParts=0;
    subbandSizes.set_length(0);
  }
  else if(7<DLRBs&&DLRBs<=10)
  {
    numOfBWParts=1;
    int bandSize=2;
    subbandSizes.set_length(1);
    int N=ceil_i((double)DLRBs/bandSize);
    subbandSizes(0).set_length(N);
    if(DLRBs%bandSize==0)
    {
      subbandSizes(0)=bandSize*ones_i(N);
    }
    else
    {
      subbandSizes(0).set_subvector(0,bandSize*ones_i(N-1));
      subbandSizes(0)(N-1)=DLRBs-(bandSize*(N-1));
    }
  }
  else if(10<DLRBs&&DLRBs<=26)
  {
    numOfBWParts=2;
    int bandSize=2;
    subbandSizes.set_length(1);
    int N=ceil_i((double)DLRBs/bandSize);
    subbandSizes(0).set_length(N);
    if(DLRBs%bandSize==0)
    {
      subbandSizes(0)=bandSize*ones_i(N);
    }
    else
    {
      subbandSizes(0).set_subvector(0,bandSize*ones_i(N-1));
      subbandSizes(0)(N-1)=DLRBs-(bandSize*(N-1));
    }
    
  }
  else if(26<DLRBs&&DLRBs<=63)
  {
    numOfBWParts=1;
    int bandSize=3;
    subbandSizes.set_length(1);
    int N=ceil_i((double)DLRBs/bandSize);
    subbandSizes(0).set_length(N);
    if(DLRBs%bandSize==0)
    {
      subbandSizes(0)=bandSize*ones_i(N);
    }
    else
    {
      subbandSizes(0).set_subvector(0,bandSize*ones_i(N-1));
      subbandSizes(0)(N-1)=DLRBs-(bandSize*(N-1));
    }
  }
  else if(63<DLRBs&&DLRBs<=110)
  {
    numOfBWParts=4;
    int bandSize=4;
    subbandSizes.set_length(1);
    int N=ceil_i((double)DLRBs/bandSize);
    subbandSizes(0).set_length(N);
    if(DLRBs%bandSize==0)
    {
      subbandSizes(0)=bandSize*ones_i(N);
    }
    else
    {
      subbandSizes(0).set_subvector(0,bandSize*ones_i(N-1));
      subbandSizes(0)(N-1)=DLRBs-(bandSize*(N-1));
    }
  }
  else
  {
    cout<<"[both:]error in getSubbandSizeAperiodicUESelected"<<endl;abort();
  }
}

ivec getSubbandSizeAperiodicNR(int DLRBs)
{
    //refer Table 5.2.1.4-2 in TS 38.214 
    ivec subbandSize;
    if(DLRBs<24)
    {
        subbandSize.set_length(0);
    }
    else if(DLRBs<=275)  //DLRBs must be the size of the bandwidth part
    {
        int bandSize=(DLRBs<=72) ? 4 : (DLRBs<=140) ? 8 : (DLRBs<=207) ? 16 : 32;
        int N=ceil_i((double)DLRBs/bandSize);
        subbandSize.set_length(N);
        if(DLRBs%bandSize==0)
        {
            subbandSize=bandSize*ones_i(N);
        }
        else
        {
            subbandSize.set_subvector(0,bandSize*ones_i(N-1));
            subbandSize(N-1)=DLRBs-(bandSize*(N-1));
        }
    }
    else
    {
        cout<<"Invalid RBs "<<DLRBs<<" for selecting subband sizes..."<<endl;abort();
    }
    return subbandSize;
}

ivec getSubbandSizeAperiodic(int DLRBs){
    
  return getSubbandSizeAperiodicNR(DLRBs);  
  
  ivec subbandSize;
  if(6<=DLRBs&&DLRBs<=7)
  {
    subbandSize.set_length(0);
  }
  else if(7<DLRBs&&DLRBs<=10)
  {
    int bandSize=4;
    int N=ceil_i((double)DLRBs/bandSize);
    subbandSize.set_length(N);
    if(DLRBs%bandSize==0)
    {
      subbandSize=bandSize*ones_i(N);
    }
    else
    {
      subbandSize.set_subvector(0,bandSize*ones_i(N-1));
      subbandSize(N-1)=DLRBs-(bandSize*(N-1));
    }
  }
  else if(10<DLRBs&&DLRBs<=26)
  {
    int bandSize=4;
    int N=ceil_i((double)DLRBs/bandSize);
    subbandSize.set_length(N);
    if(DLRBs%bandSize==0)
    {
      subbandSize=bandSize*ones_i(N);
    }
    else
    {
      subbandSize.set_subvector(0,bandSize*ones_i(N-1));
      subbandSize(N-1)=DLRBs-(bandSize*(N-1));
    }
  }
  else if(26<DLRBs&&DLRBs<=63)
  {
    //     int bandSize=6;
    int bandSize=4; // Band size changed from 6 for divisibility  - Dhiv
    
    int N=ceil_i((double)DLRBs/bandSize);
    subbandSize.set_length(N);
    if(DLRBs%bandSize==0)
    {
      subbandSize=bandSize*ones_i(N);
    }
    else
    {
      subbandSize.set_subvector(0,bandSize*ones_i(N-1));
      subbandSize(N-1)=DLRBs-(bandSize*(N-1));
    }
  }
  else if(63<DLRBs&&DLRBs<=110)
  {
    int bandSize=8;
    int N=ceil_i((double)DLRBs/bandSize);
    subbandSize.set_length(N);
    if(DLRBs%bandSize==0)
    {
      subbandSize=bandSize*ones_i(N);
    }
    else
    {
      subbandSize.set_subvector(0,bandSize*ones_i(N-1));
      subbandSize(N-1)=DLRBs-(bandSize*(N-1));
    }
  }
  else
  {
    cout<<"[both:]error in getsubbandSizeAperiodic"<<endl;abort();
  }
  return(subbandSize);
}

ivec getSubbandSizeAperiodicUESelected(int DLRBs){
    
    ivec subbandSize;
    if(6<=DLRBs&&DLRBs<=7)
    {
        subbandSize.set_length(0);
    }
    else if(7<DLRBs&&DLRBs<=10)
    {
        int bandSize=2;
        int N=ceil_i((double)DLRBs/bandSize);
        subbandSize.set_length(N);
        if(DLRBs%bandSize==0)
        {
            subbandSize=bandSize*ones_i(N);
        }
        else
        {
            subbandSize.set_subvector(0,bandSize*ones_i(N-1));
            subbandSize(N-1)=DLRBs-(bandSize*(N-1));
        }
    }
    else if(10<DLRBs&&DLRBs<=26)
    {
        int bandSize=2;
        int N=ceil_i((double)DLRBs/bandSize);
        subbandSize.set_length(N);
        if(DLRBs%bandSize==0)
        {
            subbandSize=bandSize*ones_i(N);
        }
        else
        {
            subbandSize.set_subvector(0,bandSize*ones_i(N-1));
            subbandSize(N-1)=DLRBs-(bandSize*(N-1));
        }
    }
    else if(26<DLRBs&&DLRBs<=63)
    {
        int bandSize=3;
        int N=ceil_i((double)DLRBs/bandSize);
        subbandSize.set_length(N);
        if(DLRBs%bandSize==0)
        {
            subbandSize=bandSize*ones_i(N);
        }
        else
        {
            subbandSize.set_subvector(0,bandSize*ones_i(N-1));
            subbandSize(N-1)=DLRBs-(bandSize*(N-1));
        }
    }
    else if(63<DLRBs&&DLRBs<=110)
    {
        int bandSize=4;
        int N=ceil_i((double)DLRBs/bandSize);
        subbandSize.set_length(N);
        if(DLRBs%bandSize==0)
        {
            subbandSize=bandSize*ones_i(N);
        }
        else
        {
            subbandSize.set_subvector(0,bandSize*ones_i(N-1));
            subbandSize(N-1)=DLRBs-(bandSize*(N-1));
        }
    }
    else
    {
        cout<<"[both:]error in getSubbandSizeAperiodicUESelected"<<endl;abort();
    }
    
    return(subbandSize);
    
}

int getNumberOfUESelectedSubbands(int DLRBs){
    
    int numSubbands;
    if(6<=DLRBs&&DLRBs<=7)
    {
        numSubbands=0;
    }
    else if(7<DLRBs&&DLRBs<=10)
    {
        numSubbands=1;
    }
    else if(10<DLRBs&&DLRBs<=26)
    {
        numSubbands=3;
    }
    else if(26<DLRBs&&DLRBs<=63)
    {
        numSubbands=5;
    }
    else if(63<DLRBs&&DLRBs<=110)
    {
        numSubbands=6;
    }
    else
    {
        cout<<"[both:]error in getSubbandSizeAperiodicUESelected"<<endl;abort();
    }
    
    return(numSubbands);
}

TransmissionMode_E getTransmissionMode(string transmissionMode){
  
  TransmissionMode_E TransmissionMode;
  if(transmissionMode=="_TRANSMISSION_MODE_1_")
    TransmissionMode=_TRANSMISSION_MODE_1_;
  else if(transmissionMode=="_TRANSMISSION_MODE_2_")
    TransmissionMode=_TRANSMISSION_MODE_2_;
  else if(transmissionMode=="_TRANSMISSION_MODE_3_")
    TransmissionMode=_TRANSMISSION_MODE_3_;
  else if(transmissionMode=="_TRANSMISSION_MODE_4_")
    TransmissionMode=_TRANSMISSION_MODE_4_;
  else if(transmissionMode=="_TRANSMISSION_MODE_5_")
    TransmissionMode=_TRANSMISSION_MODE_5_;
  else if(transmissionMode=="_TRANSMISSION_MODE_6_")
    TransmissionMode=_TRANSMISSION_MODE_6_;
  else if(transmissionMode=="_TRANSMISSION_MODE_7_")
    TransmissionMode=_TRANSMISSION_MODE_7_;
  else if(transmissionMode=="_TRANSMISSION_MODE_8_")
    TransmissionMode=_TRANSMISSION_MODE_8_;
  else if(transmissionMode=="_TRANSMISSION_MODE_9_")
    TransmissionMode=_TRANSMISSION_MODE_9_;
  else if(transmissionMode=="_TRANSMISSION_MODE_NR_")
    TransmissionMode=_TRANSMISSION_MODE_NR_;
  else if(transmissionMode=="_TRANSMISSION_MODE_10_")
    TransmissionMode=_TRANSMISSION_MODE_10_;
  else if(transmissionMode=="_TRANSMISSION_MODE_11_")
    TransmissionMode=_TRANSMISSION_MODE_11_;
  else
  {
    cout<<"[both:]Undefined Transmission Mode to getTransmissionMode()"<<endl;
    cout<<transmissionMode<<endl;
    abort();
  }
  return TransmissionMode;
}

// void setRsInfo(RSInfo_S &rsInfo,PortInfo_S &portInfo,FrameStructure &frameStructure, int cellID ,TransmissionScheme_E txScheme, int nLayers, int nAntenna, double rsBoostInDB){
//   
//   rsInfo.cellid=cellID;
//   rsInfo.CQIports=portInfo.getCQIPorts(txScheme,nAntenna);
//   
//   if(txScheme<6) //CRS Modes
//     rsInfo.crsPorts=rsInfo.CQIports;
//   else
//   {
//     if(nAntenna<=2)
//       rsInfo.crsPorts=getIntegers(0,nAntenna-1);
//     else if(nAntenna==3)
//       rsInfo.crsPorts=getIntegers(0,1);
//     else //(nAntenna>=4)
//       rsInfo.crsPorts=getIntegers(0,3);
//   }
//    
//   rsInfo.DeModPorts=portInfo.getDemodPorts(txScheme,nLayers,nAntenna);
//   rsInfo.rsPorts=findUniqueNumbers(concat(rsInfo.crsPorts,concat(rsInfo.CQIports,rsInfo.DeModPorts)));
//   sort(rsInfo.rsPorts);
//   getRStypes(rsInfo.CQIports, rsInfo.DeModPorts, rsInfo.CQIrsType, rsInfo.DeModrsType);
//   rsInfo.rsBoostInDB=rsBoostInDB;
//   rsInfo.portReferenceNumbers=getPortReferenceNumbers(frameStructure,rsInfo.rsPorts);
//   rsInfo.fillPRS=false;
// }


PortInfo_S::PortInfo_S(){
  
  
  configuredTxMode=_TRANSMISSION_MODE_UNINIT_;
  TM7DemodPort=5;
  nCRSPorts=-1;
  enableFallBackMode=false;
  multipleCSIProcessFlag = false;
  enableZPcsirs=false;
  ZPcsirsPattern=-1;
  enablePTRS = true;
  
}

void PortInfo_S::load(string DLConfigFile,string ULConfigFile){
  
  configuredTxMode = parseTransmissionMode(DLConfigFile,1);
  configuredULTxMode = parseTransmissionMode(ULConfigFile,0);

  if(!parse(ULConfigFile,"SRSCount",SRSCount))
  {
      cout <<"variable SRSCount is not present in ulTxRxConfigFileName.txt.. Process aborted..";
      abort();
  }
  if(!parse(ULConfigFile,"enablePTRS",enablePTRS))
  {
    cout <<"variable enablePTRS is not present in ulTxRxConfigFileName.txt.. Process aborted..";
    abort();
  }
  
  parse(DLConfigFile,"TM7_DeModPorts",TM7DemodPort);
  if(!parse(DLConfigFile,"DL_nPorts",nPorts))
  {
    cout<<"[both:]Unable to parse DL_nPorts from myDLTxRxConfig.txt"<<endl;
    abort();
  }
  if(!parse(DLConfigFile,"DL_nCRSPorts",nCRSPorts))
  {
    cout<<"[both:]Unable to parse DL_nCRSPorts from myDLTxRxConfig.txt"<<endl;
    abort();
  }
 
  parse(DLConfigFile,"enableFallbackMode",enableFallBackMode);
  if(!parse(DLConfigFile,"NZPCSIConfigurations",NZPCSIConfigurations))
  {
    cout <<"variable NZPCSIConfigurations is not present in dlTxRxConfigFileName.txt.. Process aborted..";
    abort();
  }
  if(!parse(DLConfigFile,"multipleCSIProcessFlag",multipleCSIProcessFlag))
  {
    cout <<"variable multipleCSIProcessFlag is not present in dlTxRxConfigFileName.txt.. Process aborted..";
    abort();
  }
   if(!parse(DLConfigFile,"enableZPcsirs",enableZPcsirs))
  {
    cout <<"variable enableZPcsirs is not present in dlTxRxConfigFileName.txt.. Process aborted..";
    abort();
  }
     if(!parse(DLConfigFile,"ZPcsirsPattern",ZPcsirsPattern))
  {
    cout <<"variable ZPcsirsPattern is not present in dlTxRxConfigFileName.txt.. Process aborted..";
    abort();
  }
  if(!parse(DLConfigFile,"enablePTRS",enablePTRS))
  {
    cout <<"variable enablePTRS is not present in dlTxRxConfigFileName.txt.. Process aborted..";
    abort();
  }
  
      if(!parse(DLConfigFile,"ZPcsirsSymPositions",ZPcsirsSymPositions))
  {
    cout <<"variable ZPcsirsSymPositions is not present in dlTxRxConfigFileName.txt.. Process aborted..";
    abort();
  }
       if(!parse(DLConfigFile,"NZPcsirsSymPositions",NZPcsirsSymPositions))
  {
    cout <<"variable NZPcsirsSymPositions is not present in dlTxRxConfigFileName.txt.. Process aborted..";
    abort();
  }
 
}

void setRsInfo(RSInfo_S& rsInfo, PortInfo_S& portInfo, FrameStructure& frameStructure, int cellID, TransmissionScheme_E txScheme, LayerInfo_S layerInfo, int nAntennaPorts, double rsBoostInDB, bool isDownlink, bool isToTransmitCQIPorts, int dataStartSymbol, bool isToTransmitTRSPorts,TRSConfig_S mTRSConf, Waveform_E waveform){
  
  rsInfo.cellid=cellID;
 
   // if CQI Ports are  CSIRS ports then for multiple congifuration support Port Name 15 to 22 is changes to
  // 1500 to 2200 where last 2 digits specifies selected configurations
  int dmrstype=layerInfo.dmrstype;
  
  rsInfo.dmrsType=layerInfo.dmrstype;
  rsInfo.nCDM=layerInfo.nCDM;
  rsInfo.CDM=layerInfo.CDM;
  
  if(isDownlink)
  { 
    rsInfo.CQIports=portInfo.getCQIPorts();
    rsInfo.DeModPorts=portInfo.getDLDemodPorts(txScheme,layerInfo,nAntennaPorts);
    if(txScheme == _TRANSMISSION_SCHEME_NR_CL_)
    rsInfo.otherDemodPorts=portInfo.getOtherDemodPorts(txScheme,layerInfo);
  
    if(portInfo.enableZPcsirs == true)
    rsInfo.ZPcsiports=portInfo.getZPCsiPorts(txScheme,portInfo);
//     // Keeping only non CDM ports
//     ivec demodRefNumbers=getPortReferenceNumbers(frameStructure,rsInfo.DeModPorts);
//     ivec otherPortRefNumbers=getPortReferenceNumbers(frameStructure,rsInfo.otherDemodPorts);
//     ivec nonCDMOtherRefIndices=findNot(otherPortRefNumbers,demodRefNumbers);
//     if(nonCDMOtherRefIndices.length())
//       rsInfo.otherDemodPorts=rsInfo.otherDemodPorts(nonCDMOtherRefIndices);
//     else
//       rsInfo.otherDemodPorts.set_size(0);
    
    if(txScheme != _TRANSMISSION_SCHEME_NR_CL_)
    {
      rsInfo.crsPorts=getIntegers(0,portInfo.nCRSPorts-1);
    }
    rsInfo.rsPorts=findUniqueNumbers(concat(rsInfo.ZPcsiports,concat(rsInfo.crsPorts,concat(rsInfo.CQIports,concat(rsInfo.otherDemodPorts,rsInfo.DeModPorts)))));
    
    if(isToTransmitTRSPorts)
      append(rsInfo.rsPorts,portInfo.getTRSPort());
    
  }
  else	
  {
    rsInfo.CQIports=portInfo.getULCQIPorts(nAntennaPorts);
    rsInfo.DeModPorts=portInfo.getULDemodPorts(txScheme,layerInfo,nAntennaPorts);
    rsInfo.otherDemodPorts=portInfo.getOtherDemodPorts(txScheme,layerInfo);
    
//     // Keeping only non CDM ports
//     ivec demodRefNumbers=getPortReferenceNumbers(frameStructure,rsInfo.DeModPorts);
//     ivec otherPortRefNumbers=getPortReferenceNumbers(frameStructure,rsInfo.otherDemodPorts);
//     ivec nonCDMOtherRefIndices=findNot(otherPortRefNumbers,demodRefNumbers);
//     if(nonCDMOtherRefIndices.length())
//       rsInfo.otherDemodPorts=rsInfo.otherDemodPorts(nonCDMOtherRefIndices);
//     else
//       rsInfo.otherDemodPorts.set_size(0);
//     
//     
    rsInfo.rsPorts=findUniqueNumbers(concat(rsInfo.CQIports,concat(rsInfo.otherDemodPorts,rsInfo.DeModPorts)));
    if(txScheme == _TRANSMISSION_SCHEME_NR_CL_ && waveform==_Waveform_SCFDMA_ && rsInfo.DeModPorts.length()<=2)
    {
        if(dmrstype==1)  
         append(rsInfo.rsPorts,-2);
        else
        append(rsInfo.rsPorts,-3);
    }
    
  }
  rsInfo.multipleCSIProcessFlag=portInfo.multipleCSIProcessFlag;
  sort(rsInfo.rsPorts);
  
  if (rsInfo.CQIrsType == _RSTYPE_CSIRS_ && isDownlink)
  {
    ivec tempCQIports = findUniqueNumbers(ivec(rsInfo.CQIports/100));
    if(tempCQIports(0) == 0)
      tempCQIports = findUniqueNumbers(ivec(rsInfo.CQIports));
    rsInfo.nCQIports = tempCQIports.length();
  }
  else
  {
    rsInfo.nCQIports = rsInfo.CQIports.length();
  }
  
  rsInfo.rsSymPosition.set_length(rsInfo.rsPorts.length());
  rsInfo.rsScPosition=zeros_i(rsInfo.rsPorts.length());
  
  ivec allDMRSPorts=concat(rsInfo.DeModPorts,rsInfo.otherDemodPorts);
  sort(allDMRSPorts);
  if(isDownlink)
  {
    if(txScheme == _TRANSMISSION_SCHEME_NR_CL_)
    { 
      int portindex;
      for(int port_cnt=0;port_cnt<rsInfo.rsPorts.length();port_cnt++)
      { 
	int dmrsIndx=find(allDMRSPorts,rsInfo.rsPorts(port_cnt));
	if(dmrsIndx!=-1)
	{
	  if(dataStartSymbol==0)
	    rsInfo.rsSymPosition(port_cnt)="2 4";
	  else
	    rsInfo.rsSymPosition(port_cnt)=to_ivec(dataStartSymbol);
	 
      if(rsInfo.CDM==4)
          append(rsInfo.rsSymPosition(port_cnt),rsInfo.rsSymPosition(port_cnt)(0)+1);
	  
//       if(dmrstype==1)
// 	  {   
// 	    int symOffset=dmrsIndx/4; // Assuming Type 1 upto 8 ports
// 	    rsInfo.rsSymPosition(port_cnt)+=symOffset;
// 	  }
// 	  else if(dmrstype==2)
// 	  {   
// 	    int symOffset=dmrsIndx/6; // Assuming Type 2 upto 12 ports
// 	    rsInfo.rsSymPosition(port_cnt)+=symOffset;
// 	  
// 	  }
	  // 	  else if(rsInfo.DeModPorts.length()==7)
// 	  { 
// 	    if(dmrsIndx<5)
// 	    { 
// 	      if(dataStartSymbol==0)
// 		rsInfo.rsSymPosition(port_cnt)="2 3";
// 	      else
// 		rsInfo.rsSymPosition(port_cnt)=to_ivec(dataStartSymbol);
// 	    }
// 	    else
// 	      rsInfo.rsSymPosition(port_cnt)="2"; 
// 	  }
	}
	else if(find(rsInfo.ZPcsiports,rsInfo.rsPorts(port_cnt))!=-1)
	{ 
	  rsInfo.rsSymPosition(port_cnt)=to_ivec(portInfo.ZPcsirsSymPositions(0));
	  if(portInfo.ZPcsirsPattern==1)
	  rsInfo.rsScPosition(port_cnt)=4*(cellID%3);
	  else if(portInfo.ZPcsirsPattern==2)
	   rsInfo.rsScPosition(port_cnt)=2*(cellID%6);
	}
	else if( find(rsInfo.CQIports,rsInfo.rsPorts(port_cnt))!=-1)
	{
	  if(rsInfo.nCQIports>1)
	  { 
	    switch(rsInfo.nCQIports)
	    {
	      case 1: 
		rsInfo.csirsPattern=1; // take 1 or 2
		rsInfo.rsSymPosition(port_cnt)=to_ivec(portInfo.NZPcsirsSymPositions(0));
		break;
	      case 2:
		rsInfo.csirsPattern=3;
		rsInfo.rsSymPosition(port_cnt)=to_ivec(portInfo.NZPcsirsSymPositions(0));
		break;
	      case 4:
		rsInfo.csirsPattern=3;
		portindex = find(rsInfo.CQIports,rsInfo.rsPorts(port_cnt)); //assuming single CSI-process
		rsInfo.rsScPosition(port_cnt) = 2*(portindex/2);
		rsInfo.rsSymPosition(port_cnt)=to_ivec(portInfo.NZPcsirsSymPositions(0));
		break;
	      case 8:
	     
// 		  rsInfo.csirsPattern=3; // 3=>CDM2 
// 		  portindex = find(rsInfo.CQIports,rsInfo.rsPorts(port_cnt)); //assuming single CSI-process
// 		  rsInfo.rsScPosition(port_cnt) = 2*(portindex/2);
// 		  rsInfo.rsSymPosition(port_cnt)=portInfo.NZPcsirsSymPositions(0);
// 		  break;
		
		
// 		{
// 		  rsInfo.csirsPattern=3; // 3=>CDM2 
// 		  int portindex = find(rsInfo.CQIports,rsInfo.rsPorts(port_cnt)); //assuming single CSI-process
// 		  
// 		  if(portindex>3)
// 		  {
// 		    rsInfo.rsScPosition(port_cnt) = 2*((portindex-3)/2);
// 		    rsInfo.rsSymPosition(port_cnt)="4";// make sure to consider both symbol  positions
// 		  }
// 		  else
// 		  {
// 		    rsInfo.rsScPosition(port_cnt) = 2*(portindex/2);
// 		    rsInfo.rsSymPosition(port_cnt)=portInfo.NZPcsirsSymPositions(0); // make sure to consider both symbol  positions
// 		  }
// 		  break;
// 		}
// 		
// 		{
	      case 12:
		rsInfo.csirsPattern=4; // 4=>CDM4 
		  portindex = find(rsInfo.CQIports,rsInfo.rsPorts(port_cnt)); //assuming single CSI-process
		  rsInfo.rsScPosition(port_cnt) = 2*(portindex/4);
		  rsInfo.rsSymPosition(port_cnt)=to_ivec(portInfo.NZPcsirsSymPositions(0));
		  break;
	      case 16:
// 	      case 24:
		 rsInfo.csirsPattern=4; // 4=>CDM4 
		  portindex = find(rsInfo.CQIports,rsInfo.rsPorts(port_cnt)); //assuming single CSI-process
		  rsInfo.rsScPosition(port_cnt) = 2*(portindex/4);
		  if(portindex < 12)
		  rsInfo.rsSymPosition(port_cnt)=to_ivec(portInfo.NZPcsirsSymPositions(0));
		  else
		  rsInfo.rsSymPosition(port_cnt)=to_ivec(portInfo.NZPcsirsSymPositions(1));
		  break;
	      case 24:
		  rsInfo.csirsPattern=4; // 4=>CDM4 
		  portindex = find(rsInfo.CQIports,rsInfo.rsPorts(port_cnt)); //assuming single CSI-process
		  rsInfo.rsScPosition(port_cnt) = (2*(portindex/4))%6;
		//  cout<<"rsInfo.rsScPosition(port_cnt) : "<<rsInfo.rsScPosition(port_cnt)<<endl;
		  if(portindex < 12)
		  rsInfo.rsSymPosition(port_cnt)=to_ivec(portInfo.NZPcsirsSymPositions(0));
		  else
		  rsInfo.rsSymPosition(port_cnt)=to_ivec(portInfo.NZPcsirsSymPositions(1));
		  break;
	      
	      case 32:
		rsInfo.csirsPattern=4; // 4=>CDM4 
		portindex = find(rsInfo.CQIports,rsInfo.rsPorts(port_cnt)); //assuming single CSI-process
		rsInfo.rsScPosition(port_cnt) = (2*(portindex/4))%8;
		if(portindex < 16)
		  rsInfo.rsSymPosition(port_cnt)=to_ivec(portInfo.NZPcsirsSymPositions(0));
		else
		  rsInfo.rsSymPosition(port_cnt)=to_ivec(portInfo.NZPcsirsSymPositions(1));
		break;
	      
// 	      case 32:
// 		rsInfo.csirsPattern=4; // 4=>CDM4 
// 		portindex = find(rsInfo.CQIports,rsInfo.rsPorts(port_cnt)); //assuming single CSI-process
// 		cout<<"rsInfo.CQIports : "<<rsInfo.CQIports<<endl;
// 		cout<<"rsInfo.rsPorts(port_cnt) : "<<rsInfo.rsPorts(port_cnt)<<endl;
// 		cout<<"portindex : "<<portindex<<endl;
// 		rsInfo.rsScPosition(port_cnt) = 2*(portindex/4);
// 		if(portindex < 16)
// 		  rsInfo.rsSymPosition(port_cnt)="8";
// 		else
// 		  rsInfo.rsSymPosition(port_cnt)="11";
// 		break;
// // 		}
	    }
	    // set rsScPosition for all csirs ports
	  }
	  else
	  {  
		rsInfo.csirsPattern=1; // take 1 or 2
	    rsInfo.rsSymPosition(port_cnt)=to_ivec(portInfo.NZPcsirsSymPositions(0));
	 	    
	  /*  rsInfo.csirsPattern=1; // take 1 or 2 for single port
	    rsInfo.rsSymPosition(port_cnt)="8";
	  */} 
	}
	else if(rsInfo.rsPorts(port_cnt)==3200)
	{
	  rsInfo.rsSymPosition(port_cnt)=ivec("0 4")+mTRSConf.trssymbolno;
	}
	else
	  rsInfo.rsSymPosition(port_cnt).set_length(0);
	
	
      }
      ivec cqiPortIndices=find(rsInfo.rsPorts,rsInfo.CQIports);
      
      if(portInfo.multipleCSIProcessFlag)
	rsInfo.CQIports+=10000*rsInfo.csirsPattern;
      else
	rsInfo.CQIports+=100*rsInfo.csirsPattern;	
      setInVec(rsInfo.rsPorts,cqiPortIndices,rsInfo.CQIports);
	
    }  
  }  
  else
  {
    if(txScheme == _TRANSMISSION_SCHEME_NR_CL_)
    {
      for(int port_cnt=0;port_cnt<rsInfo.rsPorts.length();port_cnt++)
	if(find(allDMRSPorts,rsInfo.rsPorts(port_cnt))!=-1)
	{
	  
	  if(dataStartSymbol==0)
	    rsInfo.rsSymPosition(port_cnt)="2";
	  else
	    rsInfo.rsSymPosition(port_cnt)=to_ivec(dataStartSymbol);
	  
	  if(dmrstype==1)
	  {
	    int symOffset=(rsInfo.rsPorts(port_cnt)%1000)/4;
	  rsInfo.rsSymPosition(port_cnt)+=symOffset;
	  }
	  else if(dmrstype==2)
	  {
	    int symOffset=(rsInfo.rsPorts(port_cnt)%1100)/6;
	  rsInfo.rsSymPosition(port_cnt)+=symOffset;
	  }
	    
	    
	}
	else if( find(rsInfo.CQIports,rsInfo.rsPorts(port_cnt))!=-1)
    {
        rsInfo.srsPattern=1;
        rsInfo.rsSymPosition(port_cnt)=getIntegers(13-portInfo.SRSCount+1,13);
    }
	else
	  rsInfo.rsSymPosition(port_cnt).set_length(0);
    }  
    
    int resIndx=find(rsInfo.rsPorts,-2);
    if(resIndx!=-1)
        rsInfo.rsSymPosition(resIndx)= rsInfo.rsSymPosition(find(rsInfo.rsPorts,allDMRSPorts(0)));
    else
    {
        resIndx=find(rsInfo.rsPorts,-3);
        if(resIndx!=-1)
            rsInfo.rsSymPosition(resIndx)= rsInfo.rsSymPosition(find(rsInfo.rsPorts,allDMRSPorts(0)));
    } 
  }

  //abort();

  getRStypes(rsInfo.CQIports, rsInfo.DeModPorts, rsInfo.CQIrsType, rsInfo.DeModrsType);
  rsInfo.rsBoostInDB=rsBoostInDB;
  
  if(isDownlink)
    formCSIRSPortStructures(rsInfo, frameStructure);
  else
    formSRSPortStructures(rsInfo, frameStructure);
  
  if(!isToTransmitCQIPorts && txScheme>7)
  {
      
      if(isDownlink && portInfo.enableZPcsirs == true)
      {
          ivec zpCsiportIndices=find(rsInfo.rsPorts,rsInfo.ZPcsiports);
          deleteInArray(rsInfo.rsSymPosition,zpCsiportIndices);
          deleteInVec(rsInfo.rsScPosition,zpCsiportIndices);
          rsInfo.rsPorts=remove(rsInfo.rsPorts,rsInfo.ZPcsiports);
      }
    
      ivec cqiPortIndices=find(rsInfo.rsPorts,rsInfo.CQIports);
      deleteInArray(rsInfo.rsSymPosition,cqiPortIndices);
      deleteInVec(rsInfo.rsScPosition,cqiPortIndices);
      rsInfo.rsPorts=remove(rsInfo.rsPorts,rsInfo.CQIports);
      rsInfo.CQIports.set_length(0);
  }
  
  
  
  if(isToTransmitTRSPorts)
   formTRSPortStructures(rsInfo, frameStructure);
   rsInfo.portReferenceNumbers=getPortReferenceNumbers(frameStructure,rsInfo.rsPorts);
  //if CQI ports are CSIRS ports, then port numbers will be more than 8 based on configurations configures
  // Assuming port number will be XXYY : were XX will be actual port number and YY will be configuration,
  // total distinguish ports are calculated

  
}

void addptrs(RSInfo_S &rsInfo,PortInfo_S &portInfo,FrameStructure &frameStructure, int cellID ,TransmissionScheme_E txScheme, int rnti, ivec Scheduledrbs, double rsBoostInDB, bool isDownlink)
{  int noofptrsports=1; //argument (1 or 2) depends on higher layer parameters
  if(isDownlink)
  { 
  rsInfo.PtrsPorts=portInfo.getPTRSPorts(txScheme,noofptrsports-1); 
  rsInfo.rsPorts=concat(rsInfo.rsPorts,rsInfo.PtrsPorts);
  }
  else
  { rsInfo.PtrsPorts=portInfo.getPTRSPorts(txScheme,noofptrsports-1); 
    rsInfo.rsPorts=concat(rsInfo.rsPorts,rsInfo.PtrsPorts);
  }
  int ptrsfrequencydensity=2;
  int ptrsreoffset=2;
  if(noofptrsports==1)
    if(rsInfo.dmrsType==1)
      rsInfo.PTRSassocDMRSPorts="1000";
    else
      rsInfo.PTRSassocDMRSPorts="1100";
  else
  rsInfo.PTRSassocDMRSPorts="1000 1001";  
  ivec tempnum(0);
  ivec dmrssympos(0);
  ivec csisympos(0);
  int lref;
  int flag;
  int ptrstimedensity=4;
  Array<ivec> ptrssympos;
 
 ptrssympos.set_length(rsInfo.PtrsPorts.length());
 //ptrsscpos.set_length(rsInfo.PtrsPorts);
 //if(isDownlink)
 //{
   if(txScheme == _TRANSMISSION_SCHEME_NR_CL_)
   { int portindex;
     for(int port_cnt=0;port_cnt<rsInfo.rsPorts.length();port_cnt++)
     { int dmrsIndx=find(rsInfo.DeModPorts,rsInfo.rsPorts(port_cnt));
       if(dmrsIndx!=-1)
       {
	 dmrssympos=findUniqueNumbers(concat(dmrssympos,rsInfo.rsSymPosition(port_cnt)));
       }
	 else if( find(rsInfo.CQIports,rsInfo.rsPorts(port_cnt))!=-1)
	 {
	   csisympos=findUniqueNumbers(concat(csisympos,rsInfo.rsSymPosition(port_cnt)));
	 }
     }
    if(dmrssympos.length()!=0)
     { for(lref=dmrssympos(0);lref<=13 && lref+ptrstimedensity<=13;)
       { tempnum=getIntegers(lref+1,lref+ptrstimedensity);
	  flag=0;
	   for(int loop=0;loop<tempnum.length();loop++)
	   { if(find(dmrssympos,tempnum(loop))!=-1)
	     { lref=tempnum(loop);
	       flag=1;
	     }
	   }
	   
// 	 if(flag==0)
// 	 { if(find(csisympos,lref+ptrstimedensity)!=-1)  //PTRS is punctured
// 	   lref=lref+ptrstimedensity;
// 	  else
//	  {
	   for(int loop2=0;loop2<ptrssympos.length();loop2++)
	    append(ptrssympos(loop2),lref+ptrstimedensity);
	   // append(ptrssympos(1),lref+ptrstimedensity-1); 
	    lref=lref+ptrstimedensity;
       //}
	 }
	}
      }
 //  }


for(int loop2=0;loop2<ptrssympos.length();loop2++)
 append(rsInfo.rsSymPosition,ptrssympos(loop2));

 ptrsfrequencyassignment(rsInfo,portInfo,frameStructure,rnti, ptrsfrequencydensity, Scheduledrbs, ptrsreoffset);
// append(rsInfo.rsSymPosition,ptrssympos(1));
 //append(rsInfo.rsScPosition,ptrsscpos); 
append(rsInfo.portReferenceNumbers,getPortReferenceNumbers(frameStructure,rsInfo.PtrsPorts));
  
}

void ptrsfrequencyassignment(RSInfo_S &rsInfo,PortInfo_S &portInfo,FrameStructure &frameStructure ,int rnti,int ptrsfrequencydensity,ivec Scheduledrbs,int ptrsreoffset)
{int kRBMax, RBOffset, REOffset;
  int numofSchRBs=Scheduledrbs.length();
  ivec ptrsscpos(0);
  ptrsscpos.set_length(rsInfo.PtrsPorts.length());
  kRBMax=numofSchRBs%ptrsfrequencydensity;
  if(!kRBMax)
    kRBMax=ptrsfrequencydensity;
  RBOffset=rnti%kRBMax;
  for(int loop=0;loop<rsInfo.PtrsPorts.length();loop++)
  {
    switch(rsInfo.PTRSassocDMRSPorts(loop))
    {
      case 1000 :
	switch (ptrsreoffset)
	{	
	  case 1:ptrsscpos(loop)=0;
	    break;
	  case 2:ptrsscpos(loop)=2;
	    break;
	  case 3:ptrsscpos(loop)=6;
	    break;
	  case 4:ptrsscpos(loop)=8;
	    break;
	  default:
	    cout<<"Invalid ptrsreoffset";
	    abort();
	}
	break;
      case 1001:
	switch (ptrsreoffset)
	{
	  case 1:ptrsscpos(loop)=2;
	    break;
	  case 2:ptrsscpos(loop)=4;
	    break;
	  case 3:ptrsscpos(loop)=8;
	    break;
	  case 4:ptrsscpos(loop)=10;
	    break;
	  default:
	    cout<<"Invalid ptrsreoffset";
	    abort();
	}
	break;
      case 1002:
	switch (ptrsreoffset)
	{
	  case 1:ptrsscpos(loop)=1;
	    break;
	  case 2:ptrsscpos(loop)=3;
	    break;
	  case 3:ptrsscpos(loop)=7;
	    break;
	  case 4:ptrsscpos(loop)=9;
	    break;
	  default:
	    cout<<"Invalid ptrsreoffset";
	    abort();
	}
	break;
      case 1003:
       switch (ptrsreoffset)
	{
	  case 1:ptrsscpos(loop)=3;
	    break;
	  case 2:ptrsscpos(loop)=5;
	    break;
	  case 3:ptrsscpos(loop)=9;
	    break;
	  case 4:ptrsscpos(loop)=11;
	    break;
	  default:
	    cout<<"Invalid ptrsreoffset";
	    abort();
	}
	break;
      case 1100 :
       switch (ptrsreoffset)
        {
	  case 1:ptrsscpos(loop)=0;
	    break;
	  case 2:ptrsscpos(loop)=1;
	    break;
	  case 3:ptrsscpos(loop)=6;
	    break;
	  case 4:ptrsscpos(loop)=7;
	    break;
	  default:
	    cout<<"Invalid ptrsreoffset";
	    abort();
	}
	break;
      case 1101 :
	switch (ptrsreoffset)
	{
	  case 1:ptrsscpos(loop)=1;
	    break;
	  case 2:ptrsscpos(loop)=6;
	    break;
	  case 3:ptrsscpos(loop)=7;
	    break;
	  case 4:ptrsscpos(loop)=0;
	    break;
	  default:
	    cout<<"Invalid ptrsreoffset";
	    abort();
	}
	break;
      case 1102 :
	switch (ptrsreoffset)
	  {
	    case 1:ptrsscpos(loop)=2;
	      break;
	    case 2:ptrsscpos(loop)=3;
	      break;
	    case 3:ptrsscpos(loop)=8;
	      break;
	    case 4:ptrsscpos(loop)=9;
	      break;
	    default:
	      cout<<"Invalid ptrsreoffset";
	      abort();
	  }
	  break;
      case 1103 :
	switch (ptrsreoffset)
	  {
	    case 1:ptrsscpos(loop)=3;
	      break;
	    case 2:ptrsscpos(loop)=8;
	      break;
	    case 3:ptrsscpos(loop)=9;
	      break;
	    case 4:ptrsscpos(loop)=2;
	      break;
	    default:
	      cout<<"Invalid ptrsreoffset";
	      abort();
	  }
	  break;
	case 1104 :
	  switch (ptrsreoffset)
	    {
	      case 1:ptrsscpos(loop)=4;
		break;
	      case 2:ptrsscpos(loop)=5;
		break;
	      case 3:ptrsscpos(loop)=10;
		break;
	      case 4:ptrsscpos(loop)=11;
		break;
	      default:
		cout<<"Invalid ptrsreoffset";
		abort();
	    }
	    break;
	 default:
	  cout<<"Invalid associated dmrs ports";
	  abort();
    }
  }
  for(int loop2=0;loop2<ptrsscpos.length();loop2++)
    append(rsInfo.rsScPosition,ptrsscpos(loop2));
   rsInfo.rsrbpos.set_length(rsInfo.rsPorts.length());
  for(int port_cnt=0;port_cnt<rsInfo.rsPorts.length();port_cnt++)
  {for(int loop3=0;(loop3*ptrsfrequencydensity)+RBOffset<Scheduledrbs.length();loop3++)
    {
      if(find(rsInfo.PtrsPorts,rsInfo.rsPorts(port_cnt))!=-1)
       append(rsInfo.rsrbpos(port_cnt),Scheduledrbs((loop3*ptrsfrequencydensity)+RBOffset));
    }
  }
  
}

ivec getOrthoCoverCodeForPTRSTPenabled(int nRNTI, int nSamplesPerGroup)
{
  ivec orthoCoverCode;
  int tempIndex;
  tempIndex=mod(nRNTI, nSamplesPerGroup);
  // generates orthogonal Cover Code for UE-specific reference signals from port-7 to port-15
  
  orthoCoverCode.set_length(nSamplesPerGroup);
  
  if(nSamplesPerGroup==4)
  {
    switch(tempIndex)
    {
      case 0:
	orthoCoverCode = "1 1 1 1";
	break;
      case 1:
	orthoCoverCode = "1 -1 1 -1";
	break;
      case 2:
	orthoCoverCode = "1 1 -1 -1";
	break;
      case 3:
	orthoCoverCode = "1 -1 -1 1";
	break;
      default:
	cout<<"[both:] Undefined nRNTI, exiting in ....getOrthoCoverCode()"<<endl;abort();
    }
  }
  else if(nSamplesPerGroup==2)
  {
    switch(tempIndex)
    {
      case 0:
	orthoCoverCode = "1 1";
	break;
      case 1:
	orthoCoverCode = "1 -1";
	break;
      default:
	cout<<"[both:] Undefined nRNTI, exiting in ....getOrthoCoverCode()"<<endl;abort();
    }
  }
  
  return orthoCoverCode;
}

void getNoOfSamplesAndPTRSGroupsTPenabled(int scheduledBW, int &nSamplesPerGroup, int &nPTRSGroups)
{
  //considered config 0 for scheduled MCS<10
  int NRB0=0, NRB1=8, NRB2 = 24, NRB3=24, NRB4=96;
  switch(scheduledBW)
  {
    case 0 ... 7:
      nSamplesPerGroup=2;
      nPTRSGroups=2;
      break;
    case 8 ... 23:
      nSamplesPerGroup=2;
      nPTRSGroups=4;
      break;
      //     case NRB2 ... NRB3: //Disabled as per specifications, since both thresholds are equal 
      //       nSamplesPerGroup=4;
      //       nPTRSGroups=2;
      break;
    case 24 ... 95:
      nSamplesPerGroup=4;
      nPTRSGroups=4;
      break;
    case 96 ... 110:
      nSamplesPerGroup=8;
      nPTRSGroups=4;
      break;
    default:
      cout<<"Invalid number of RBs"<<endl;
      
  };
}

ivec getPTRSindexmTPenabled(int nSamplesPerGroup, int nPTRSGroups, int MpuschSC)
{
  int s1, k1, n1, m1, PTRSindexm;
  vec s, n;
  ivec PTRSindxm(0);
  //   PTRSindxm.set_size(nSamplesPerGroup*nPTRSGroups);
  s.set_size(nPTRSGroups);
  imat k(nPTRSGroups,nSamplesPerGroup);
  if(nSamplesPerGroup==2 && nPTRSGroups==2)
  {
    s= "1 3";
    k= "0 1;0 1";
    m1= (MpuschSC/4);
    n1=-1;
  }
  else if(nPTRSGroups == 2 && nSamplesPerGroup ==4)
  {
    s = "0 1";
    k= "0 1 2 3;-4 -3 -2 -1";
    m1= (MpuschSC);
    n1=0;
  }
  else if(nPTRSGroups==4 && nSamplesPerGroup==2)
  {
    s= "1 3 5 7";
    k= "0 1;0 1;0 1; 0 1";
    m1= (MpuschSC/8);
    n1=-1;
  }
  else if(nPTRSGroups==4 && nSamplesPerGroup==4)
  {
    s= "0 1 2 4";
    k= "0 1 2 3; -2 -1 0 1; -2 -1 0 1; -4 -3 -2 -1";
    n= "0 MpuschSC/8 MpuschSC/8 0";
    m1= (MpuschSC/4);
  }
  else if(nPTRSGroups==8 && nSamplesPerGroup==4)
  {
    s= "0 1 2 3 4 5 6 8";
    k= "0 1 2 3; -2 -1 0 1; -2 -1 0 1; -2 -1 0 1; -2 -1 0 1; -2 -1 0 1; -2 -1 0 1; -4 -3 -2 -1";
    n= "0 MpuschSC/16 MpuschSC/16 MpuschSC/16 MpuschSC/16 MpuschSC/16 MpuschSC/16 0";
    m1= (MpuschSC/8);
  }
  else
  {
    cout<<"invalid index"<<endl;
  }
  for(int i=0; i< nPTRSGroups; i++)
    for(int j=0; j<nSamplesPerGroup; j++)
    {
      s1=s(i);
      k1=k(i,j);
      
      if(nSamplesPerGroup==4||nSamplesPerGroup==8 && nPTRSGroups==4)
	n1=n(i);
      
      //       PTRSindxm.push_back(PTRSindexm);
      PTRSindexm= s1*m1+k1+n1;
      append(PTRSindxm, PTRSindexm);
      //       cout<<"deep "<<PTRSindxm <<endl;
    }
    return PTRSindxm;
}


void formCSIRSPortStructures(RSInfo_S &rsInfo, FrameStructure &frameStructure)
{	
    ivec cqiPorts;
    cqiPorts=rsInfo.CQIports;
    int csipattern=rsInfo.csirsPattern;
    string toneTypeSuffix;
    ResourceUnitInfo_S newRUInfo,baseRUInfo;
    int toneNumberOffset;
    string baseRuType;
    baseRuType="csipattern"+toString(csipattern);
    if(csipattern==1 || csipattern==2)
    {
      for(int port_cnt=0;port_cnt<cqiPorts.length();port_cnt++)
      {
	string rbName="port"+toString(cqiPorts(port_cnt));
	toneTypeSuffix=toString(cqiPorts(port_cnt));
	toneNumberOffset=cqiPorts(port_cnt)-30; //30 is used in the RB structure file as refernce number;
	if(!frameStructure.isResourceUnitAvailable(rbName))
	  frameStructure.modifyResourceUnit(baseRuType,rbName,"data",toneNumberOffset,toneTypeSuffix,0,0);
      }         
    }
    else if(csipattern>=3)
    {
      
      for(int port_cnt=0;port_cnt<cqiPorts.length();port_cnt++)
      {
	string rbName="port"+toString(cqiPorts(port_cnt));
	toneTypeSuffix=toString(cqiPorts(port_cnt));
	
	if(csipattern==3) //CDM2
	  toneNumberOffset=cqiPorts((port_cnt/2)*2)-30; //30 is used in the RB structure file as refernce number;
	else //CDM4
	  toneNumberOffset=cqiPorts((port_cnt/4)*4)-30; //30 is used in the RB structure file as refernce number;
	

	if(!frameStructure.isResourceUnitAvailable(rbName))
	  frameStructure.modifyResourceUnit(baseRuType,rbName,"data",toneNumberOffset,toneTypeSuffix,0,0);

	
      }   
    }
}

void formSRSPortStructures(RSInfo_S &rsInfo, FrameStructure &frameStructure)
{	
    ivec cqiPorts;
    cqiPorts=rsInfo.CQIports;
    int srspattern=rsInfo.srsPattern;
    string toneTypeSuffix;
    ResourceUnitInfo_S newRUInfo,baseRUInfo;
    int toneNumberOffset;
    string baseRuType;
    baseRuType="srspattern"+toString(srspattern);
    if(srspattern==1 || srspattern==2)
    {
      for(int port_cnt=0;port_cnt<cqiPorts.length();port_cnt++)
      {
        string rbName="port"+toString(cqiPorts(port_cnt));
        toneTypeSuffix=toString(cqiPorts(port_cnt));
        toneNumberOffset=cqiPorts(0)-10; //10 is used in the RB structure file as reference number;
        
        if(!frameStructure.isResourceUnitAvailable(rbName))
        {
            frameStructure.modifyResourceUnit(baseRuType,rbName,"res",toneNumberOffset,toneTypeSuffix,0,0);
        }
        }         
    }
}

void formTRSPortStructures(RSInfo_S &rsInfo, FrameStructure &frameStructure)
{	
    int csipattern=2;
    string toneTypeSuffix;
    ResourceUnitInfo_S newRUInfo,baseRUInfo;
    int toneNumberOffset;
    string baseRuType;
    baseRuType="csipattern"+toString(csipattern);
    
     
	string rbName="port"+toString(3200);
	toneTypeSuffix=toString(3200);
	toneNumberOffset=3200-30; //30 is used in the RB structure file as refernce number;
	if(!frameStructure.isResourceUnitAvailable(rbName))
	  frameStructure.modifyResourceUnit(baseRuType,rbName,"data",toneNumberOffset,toneTypeSuffix,0,0);
   
}


ivec PortInfo_S::getCQIPorts()
{
  TransmissionScheme_E transmissionScheme = TransmissionScheme_E(configuredTxMode);
  
   imat listCSIRSConfig = (reshape(NZPCSIConfigurations,1,NZPCSIConfigurations.rows()*NZPCSIConfigurations.cols()));

  ivec CQIports;
  ivec toReturn(0);
  if(transmissionScheme!=_TRANSMISSION_SCHEME_UNINIT_)
  {
    switch (transmissionScheme){
      case _TRANSMISSION_SCHEME_SINGLE_PORT_CRS_:
	if(nCRSPorts==1 && nPorts==1) return(to_ivec(0));//CRS Ports
	else
	{
	  cout<<"[both:]Unsupported nPorts or nCRSPorts for TM1 in getCQIPorts()."<<endl;
	  abort();
	}
	break;
      case _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_:
      case _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_:
      case _TRANSMISSION_SCHEME_CL_SM_:
      case _TRANSMISSION_SCHEME_CL_MU_MIMO_:
      case _TRANSMISSION_SCHEME_CL_BF_:
      case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
	if(nCRSPorts <= nPorts) return(getIntegers(0,nCRSPorts-1));//CRS Ports
	else
	{
	  cout<<"[both:]Unsupported nPorts or nCRSPorts for TM"<<(int)transmissionScheme+1<<" in getCQIPorts()."<<endl;
	  abort();
	}
	break;
      case _TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_:
	return(to_ivec(0));
	break;
      case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
      case _TRANSMISSION_SCHEME_COMP_:
      case _TRANSMISSION_SCHEME_OLMUMIMO_:
     	if(multipleCSIProcessFlag && listCSIRSConfig.cols()!=1)
	{
	  
	  for(int nCSIRSCnt = 0;nCSIRSCnt<listCSIRSConfig.cols();nCSIRSCnt++)
	  {
	    if(nPorts>=2 && nPorts<4) 
	    {
	      ivec csirsPorts = getIntegers(15,16);
	      append(CQIports,(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt));
	     cout<<"listCSIRSConfig(nCSIRSCnt) = "<<listCSIRSConfig(nCSIRSCnt)<<endl;
	    }
	    else if(nPorts>=4 && nPorts<8)
	    {
	      ivec csirsPorts = getIntegers(15,18);
	      append(CQIports,(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt));
	    }
	    else if(nPorts==8) 
	    {
	      ivec csirsPorts = getIntegers(15,22);
	      append(CQIports,(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt));
	      
	    }
	    else
	    {
	      cout<<"[both:]Unsupported antenna count for TM9 in getCQIPorts()."<<endl;abort();
	    }
	    
	  }
	}
	else{/// Backward compatibility of code for single configuration
	  
	  if(nPorts>=2 && nPorts<4) 
	  {
	    CQIports = getIntegers(15,16);
	  }
	  else if(nPorts>=4 && nPorts<8)
	  {
	    CQIports = getIntegers(15,18);
	  }
	  else if(nPorts==8) 
	  {
	    CQIports = getIntegers(15,22);
	  }
	  else
	  {
	    cout<<"[both:]Unsupported antenna count for TM9 in getCQIPorts()."<<endl;abort();
	  }
	  
	}  
	return(CQIports);
	break;
	case _TRANSMISSION_SCHEME_NR_CL_:
	 if(multipleCSIProcessFlag && listCSIRSConfig.cols()!=1)
	  {
	    
	    for(int nCSIRSCnt = 0;nCSIRSCnt<listCSIRSConfig.cols();nCSIRSCnt++)
	    { 
	      if(nPorts==1)
	      {ivec csirsPorts;
		append(csirsPorts,3000);
		append(CQIports,(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt));
		cout<<"listCSIRSConfig(nCSIRSCnt) = "<<(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt)<<endl;
		}
	      else if(nPorts>=2 && nPorts<4) 
	      {
		ivec csirsPorts = getIntegers(3000,3001);
		append(CQIports,(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt));
		cout<<"listCSIRSConfig(nCSIRSCnt) = "<<listCSIRSConfig(nCSIRSCnt)<<endl;
	      }
	      else if(nPorts>=4 && nPorts<8)
	      {
		ivec csirsPorts = getIntegers(3000,3003);
		append(CQIports,(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt));
	      }
	      else if(nPorts==8) 
	      {
		ivec csirsPorts = getIntegers(3000,3007);
		append(CQIports,(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt));
		
	      }
	      else if(nPorts>8 && nPorts <=12) 
	      {
		ivec csirsPorts = getIntegers(3000,3011);
		append(CQIports,(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt));
		
	      }
	      else if(nPorts>12 && nPorts <=16) 
	      {
		ivec csirsPorts = getIntegers(3000,3015);
		append(CQIports,(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt));
		
	      }
	      else if(nPorts>16 && nPorts <=24) 
	      {
		ivec csirsPorts = getIntegers(3000,3023);
		append(CQIports,(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt));
		
	      }
	       else if(nPorts>24 && nPorts <=32) 
	      {
		ivec csirsPorts = getIntegers(3000,3031);
		append(CQIports,(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt));
		
	      }
	      else
	      {
		cout<<"[both:]Unsupported antenna count for TM9 in getCQIPorts()."<<endl;abort();
	      }
	      
	    }
	  }
	  else{/// Backward compatibility of code for single configuration
	    if(nPorts==1) 
	    {
	      append(CQIports,3000); // or 3200
	     }
	     else if(nPorts>=2 && nPorts<4) 
	    {
	      CQIports = getIntegers(3000,3001);
	    }
	    else if(nPorts>=4 && nPorts<8)
	    {
	      CQIports = getIntegers(3000,3003);
	    }
	    else if(nPorts==8) 
	    {
	      CQIports = getIntegers(3000,3007);
	    }
	    else if(nPorts>8 && nPorts<=12) 
	    {
	      CQIports = getIntegers(3000,3011);
	    }
	    else if(nPorts>12 && nPorts<=16) 
	    {
	      CQIports = getIntegers(3000,3015);
	    }
	    else if(nPorts>16 && nPorts<=24) 
	    {
	      CQIports = getIntegers(3000,3023);
	    }
	    else if(nPorts>24 && nPorts<=32) 
	    {
	      CQIports = getIntegers(3000,3031);
	    }
	    else
	    {
	      cout<<"[both:]Unsupported antenna count for TM11 in getCQIPorts()."<<endl;abort();
	    }
	    
	  }  
	  return(CQIports); 
      default:
      {cout<<"[both:]Unsupported Transmission Mode in getCQIPorts()."<<endl;abort();
    }
      }
  }
  else
  {
    cout<<"[both:]transmissionScheme is uninitialized.."<<endl;
    cout<<"transmissionScheme= "<<transmissionScheme<<endl;
    abort();
  }
  return ivec("");
}

int PortInfo_S::getTRSPort()
{
   TransmissionScheme_E transmissionScheme = TransmissionScheme_E(configuredTxMode);
  
  if(transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_)
    return 3200;
  else
  {
    cout<<"[both:]Unsupported Transmission Mode in getTRSPort()."<<endl;
    abort();
  }
    
}

ivec PortInfo_S::getCQIPorts(int nAntennaPorts)
{
  imat NZPCSIConfigurations;
  bool multipleCSIProcessFlag=false;
  TransmissionScheme_E transmissionScheme = TransmissionScheme_E(configuredTxMode);

  imat listCSIRSConfig = (reshape(NZPCSIConfigurations,1,NZPCSIConfigurations.rows()*NZPCSIConfigurations.cols()));
  
  ivec CQIports;
  ivec toReturn(0);
  if(transmissionScheme!=_TRANSMISSION_SCHEME_UNINIT_)
  {
    switch ((int)transmissionScheme){
      case 0:
	if(nCRSPorts==1 && nAntennaPorts==1) return(to_ivec(0));//CRS Ports
	else
	{
	  cout<<"[both:]Unsupported nPorts or nCRSPorts for TM1 in getCQIPorts()."<<endl;
	  abort();
	}
	break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 7:
	if(nCRSPorts <= nAntennaPorts) return(getIntegers(0,nCRSPorts-1));//CRS Ports
	else
	{
	  cout<<"[both:]Unsupported nPorts or nCRSPorts for TM"<<(int)transmissionScheme+1<<" in getCQIPorts()."<<endl;
	  abort();
	}
	break;
      case 6:
	return(to_ivec(0));
	break;
      case 8:
      case 9:
      case 10:
      case 11:
	if(multipleCSIProcessFlag && listCSIRSConfig.cols()!=1)
	{
	  
	  for(int nCSIRSCnt = 0;nCSIRSCnt<listCSIRSConfig.cols();nCSIRSCnt++)
	  {
	    if(nAntennaPorts>=2 && nAntennaPorts<4) 
	    {
	      ivec csirsPorts = getIntegers(15,16);
	      append(CQIports,(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt));
	    }
	    else if(nAntennaPorts>=4 && nAntennaPorts<8)
	    {
	      ivec csirsPorts = getIntegers(15,18);
	      append(CQIports,(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt));
	    }
	    else if(nAntennaPorts==8) 
	    {
	      ivec csirsPorts = getIntegers(15,22);
	      append(CQIports,(csirsPorts*100)+listCSIRSConfig(nCSIRSCnt));
	      
	    }
	    else
	    {
	      cout<<"[both:]Unsupported antenna count for TM9 in getCQIPorts()."<<endl;abort();
	    }
	    
	  }
	}
	else{/// Backward compatibility of code for single configuration
	  
	  if(nAntennaPorts>=2 && nAntennaPorts<4) 
	  {
	    CQIports = getIntegers(15,16);
	  }
	  else if(nAntennaPorts>=4 && nAntennaPorts<8)
	  {
	    CQIports = getIntegers(15,18);
	  }
	  else if(nAntennaPorts==8) 
	  {
	    CQIports = getIntegers(15,22);
	  }
	  else
	  {
	    cout<<"[both:]Unsupported antenna count for TM9 in getCQIPorts()."<<endl;abort();
	  }
	  
	}  
	return(CQIports);  
      default:
      {cout<<"[both:]Unsupported Transmission Mode in getCQIPorts()."<<endl;
	cout<<"transmissionScheme 2 = "<<transmissionScheme<<endl;
	abort();}
    }
  }
  else
  {
    cout<<"[both:]transmissionScheme is uninitialized.."<<endl;abort();
  }
  return ivec("");
}

ivec getAllPorts(int nFrontLoadedSymbols,int nCDMgroupsWithoutData,int dmrsType,Array<ivec>& layersPerCDM)
{
    if(dmrsType==1)
    {
        if(nFrontLoadedSymbols==1)
            layersPerCDM = "{[0 1][2 3]}";
        else
            layersPerCDM = "{[0 1 4 5][2 3 6 7]}";
    }
    else
    {
        if(nFrontLoadedSymbols==1)
            layersPerCDM = "{[0 1][2 3][4 5]}";
        else
            layersPerCDM = "{[0 1 6 7][2 3 8 9][4 5 10 11]}";
    }
    ivec allPorts(0);
    for(int i=0;i<nCDMgroupsWithoutData;i++)
        append(allPorts,layersPerCDM(i));
    return allPorts;
}

// It considers one CDM per UE - Goraknath.
ivec getDMRSports(LayerInfo_S layerInfo,bool returnMyPorts)
{
    Array <ivec> layersPerCDM;
    ivec otherPorts(0),allPorts= getAllPorts(layerInfo.CDM/2,layerInfo.nCDM,layerInfo.dmrstype,layersPerCDM);
    if(layerInfo.CDMMap.length()==1) // SU-MIMO
    {
        return (returnMyPorts) ? allPorts(0,layerInfo.nLayers-1) : otherPorts;
    }
    ivec myCDMs=find(layerInfo.CDMMap==1);
    ivec myPorts = layersPerCDM(myCDMs(0))(0,layerInfo.nLayers-1);
    otherPorts = allPorts(findNot(allPorts,myPorts));    
    return (returnMyPorts) ? myPorts : otherPorts;
}

ivec getLayers(ivec CDMMap, int CDM, int nLayers)
{
    ivec myCDMs=find(CDMMap==1);
    Array< Array <ivec> > layersPerCDM="{ {[0 1] [2 3] [4 5]} {[0 1 6 7] [2 3 8 9] [4 5 10 11]} }";
    ivec myLayers(0);
    for(int cnt=0;cnt<myCDMs.length();cnt++)
        append(myLayers,layersPerCDM(CDM/2-1)(myCDMs(cnt)));
    return(myLayers(0,nLayers-1));
}

ivec getOthersLayers(ivec CDMMap, int CDM)
{
    ivec otherCDMs=find(CDMMap==0);
    Array< Array <ivec> > layersPerCDM="{ {[0 1] [2 3] [4 5]} {[0 1 6 7] [2 3 8 9] [4 5 10 11]} }";
    ivec otherLayers(0);
    for(int cnt=0;cnt<otherCDMs.length();cnt++)
        append(otherLayers,layersPerCDM(CDM/2-1)(otherCDMs(cnt)));
    return(otherLayers);
}

ivec PortInfo_S:: getDLDemodPorts(TransmissionScheme_E transmissionScheme,LayerInfo_S layerInfo, int nAntennaPorts)
{
  int dmrstype=layerInfo.dmrstype;
  ivec toReturn(0);
  if(transmissionScheme!=_TRANSMISSION_SCHEME_UNINIT_)
  {
    switch ((int)transmissionScheme){
      case 0:
	if(nCRSPorts==1 && nAntennaPorts==1) return(to_ivec(0));//CRS Ports
	else
	{
	  cout<<"[both:]Unsupported nPorts or nCRSPorts for TM1 in getDemodPorts()."<<endl;
	  abort();
	}
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
	if(nCRSPorts <= nAntennaPorts) return(getIntegers(0,nCRSPorts-1));//CRS Ports
	else
	{
	  cout<<"[both:]Unsupported nPorts or nCRSPorts for TM"<<(int)transmissionScheme+1<<" in getDemodPorts()."<<endl;
	  abort();
	}
      case 6:
	return(to_ivec(TM7DemodPort));
      case 7:
      case 8:
      case 9:
      case 10:
	return(getIntegers(7,7+layerInfo.nLayers-1));
      case 11:
        //FIXME how CDMMap length and number of users in MU-MIMO is related?
        //temporarily fixed
// 	if(layerInfo.CDMMap.length()==1)
// 	{  if(dmrstype==1)
// 	    return(getIntegers(1000,1000+layerInfo.nLayers-1));
// 	  else
// 	   return(getIntegers(1100,1100+layerInfo.nLayers-1));
// 	}   
// 	else
	{
	  if(dmrstype==1)
	    //return(1000+getLayers(to_ivec(layerInfo.CDMMap),layerInfo.CDM,layerInfo.nLayers));
          return (1000+getDMRSports(layerInfo,1));
	 else
         //return(1100+getLayers(to_ivec(layerInfo.CDMMap),layerInfo.CDM,layerInfo.nLayers));
         return (1100+getDMRSports(layerInfo,1));
	}
      default:
      {cout<<"[both:]Unsupported Transmission Mode in getDemodPorts()."<<endl;abort();}
    };
  }
  else
  {
    cout<<"[both:]transmissionScheme is uninitialized.."<<endl;abort();
  }
  return ivec("");
}

ivec PortInfo_S:: getPTRSPorts(TransmissionScheme_E transmissionScheme,int higherlayerparam1)
{
  ivec toReturn(0);
  if(transmissionScheme!=_TRANSMISSION_SCHEME_UNINIT_)
  {
    switch ((int)transmissionScheme)
    {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
	return ivec("");
      case 11:
	return(getIntegers(1200,1200+higherlayerparam1));	//return(getIntegers(1200,1201));
      default:
      {cout<<"[both:]Unsupported Transmission Mode in getDemodPorts()."<<endl;abort();}
    };
  }
  else
  {
    cout<<"[both:]transmissionScheme is uninitialized.."<<endl;abort();
  }
  return ivec("");
  
  
}

ivec PortInfo_S::getZPCsiPorts(TransmissionScheme_E transmissionScheme, PortInfo_S& portInfo)
{
  ivec toReturn(0);
  if(transmissionScheme!=_TRANSMISSION_SCHEME_UNINIT_)
  {
    switch ((int)transmissionScheme){
      case 11:
      {
	toReturn.set_length(1);
	toReturn[0] = 3500+(portInfo.ZPcsirsPattern-1);
	  return toReturn;
      }
      default:
      {cout<<"[both:]Unsupported Transmission Mode in getZPCsiPorts()."<<endl;abort();}
    };
    
   }
  else
  {
    cout<<"[both:]transmissionScheme is uninitialized.."<<endl;abort();
  }
  
}

ivec PortInfo_S:: getOtherDemodPorts(TransmissionScheme_E transmissionScheme,LayerInfo_S layerInfo)
{
  int dmrstype=layerInfo.dmrstype;
  
  ivec toReturn(0);
  if(transmissionScheme!=_TRANSMISSION_SCHEME_UNINIT_)
  {
    switch ((int)transmissionScheme){
      case 11:
	if(layerInfo.CDMMap.length()==0)
	  return ivec("");
	else
	{ 
	  if(dmrstype==1)
          //return(1000+getOthersLayers(to_ivec(layerInfo.CDMMap),layerInfo.CDM));
          return (1000+getDMRSports(layerInfo,0));
	  else
          //return(1100+getOthersLayers(to_ivec(layerInfo.CDMMap),layerInfo.CDM));
          return (1100+getDMRSports(layerInfo,0));
	}
      default:
      {cout<<"[both:]Unsupported Transmission Mode in getOtherDemodPorts()."<<endl;abort();}
    };
  }
  else
  {
    cout<<"[both:]transmissionScheme is uninitialized.."<<endl;abort();
  }
  return ivec("");
}

ivec PortInfo_S::getULCQIPorts(int nAntennaPorts)
{

  TransmissionScheme_E transmissionScheme = TransmissionScheme_E(configuredULTxMode);

  ivec CQIports;
  ivec toReturn(0);
  if(transmissionScheme!=_TRANSMISSION_SCHEME_UNINIT_)
  {
    switch ((int)transmissionScheme){
//       case 0:
// 	if(nAntennaPorts==1) return(to_ivec(0));
// 	else
// 	{
// 	  cout<<"[both:]Unsupported nAntennaPorts for TM1 in getULCQIPorts()."<<endl;
// 	  abort();
// 	}
// 	break;
//       case 3:
//       if(nAntennaPorts==2 || nAntennaPorts==4) return(getIntegers(0,nAntennaPorts-1));
// 	else
// 	{ cout<<"[both:]Unsupported nAntennaPorts for TM"<<(int)transmissionScheme+1<<" in getULCQIPorts()."<<endl;
// 	  abort();
// 	}
// 	break;
      case 11:
	return(getIntegers(0,nAntennaPorts-1)+4000); // using 4000 for SRS ports
	
	break;
      default:
      {cout<<"[both:]Unsupported Transmission Mode in getULCQIPorts()."<<endl;abort();}
    }
  }
  else
  {
    cout<<"[both:]transmissionScheme is uninitialized.."<<endl;abort();
  }
  return ivec("");
}


ivec PortInfo_S:: getULDemodPorts(TransmissionScheme_E transmissionScheme, LayerInfo_S layerInfo, int nAntenna)
{
  int dmrstype=layerInfo.dmrstype;
  
  ivec toReturn(0);
  if(transmissionScheme!=_TRANSMISSION_SCHEME_UNINIT_)
  {
    switch ((int)transmissionScheme){
      case 0:
	if(nAntenna==1) return(to_ivec(0));
	else
	{cout<<"[both:]Unsupported antenna count for TM1 in getDemodPorts()."<<endl;abort();}
	break;
      case 3:
	if(nAntenna==2 || nAntenna==4) return(getIntegers(0,nAntenna-1));
	else
	{
	  cout<<"[both:]Unsupported nAntennaPorts for TM"<<(int)transmissionScheme+1<<" in getULDemodPorts()."<<endl;
	  abort();
	}
	break;
      case 11:
// 	if(layerInfo.CDMMap.length()==0)
// 	{  if(dmrstype==1)
// 	    return(getIntegers(1000,1000+layerInfo.nLayers-1));
// 	  else
// 	   return(getIntegers(1100,1100+layerInfo.nLayers-1));
// 	}   
// 	else
	{
	  if(dmrstype==1)
          //return(1000+getLayers(to_ivec(layerInfo.CDMMap),layerInfo.CDM,layerInfo.nLayers));
         return(1000+getDMRSports(layerInfo,1));
	 else
         //return(1100+getLayers(to_ivec(layerInfo.CDMMap),layerInfo.CDM,layerInfo.nLayers));
         return(1100+getDMRSports(layerInfo,1));
	}
	break;
      default:
      {cout<<"[both:]Unsupported Transmission Mode in getULDemodPorts()."<<endl;abort();}
    };
  }
  else
  {
    cout<<"[both:]transmissionScheme is uninitialized.."<<endl;abort();
  }
  return ivec("");
}

// Array<imat> getMergedRBStructures(FrameStructure *frameStructure, ivec rsPorts,ivec vrbs,bool isDownlink,bool isLocalised,FrameReservationInfo_S *cchReservation)
// {
//   Array<imat> mergedRBs(vrbs.length());
//   
//   string mergedRbName="";
//   Array<string> rbName(rsPorts.length());
//   for(int port_cnt=0;port_cnt<rsPorts.length();port_cnt++)
//     mergedRbName+=toString(rsPorts(port_cnt));
//   if(isDownlink)
//   {
//     if(cchReservation->cchInfo.pdcchCount>0)
//       mergedRbName+="pdcch"+toString(cchReservation->cchInfo.pdcchCount);
//   }
//   
//   for(int rb_cnt=0;rb_cnt<vrbs.length();rb_cnt++)
//   {
//     string localString=mergedRbName;
//     int pbch=0,pss=0;
//     bool srsPresent=false;
//     
//     if(isDownlink)
//     {
//       ivec prbs=cchReservation->dlPrbMapping(vrbs(rb_cnt));
//       if(cchReservation->cchInfo.enablePBCH)
//       {
// 	int prb;
// 	if(isLocalised)
// 	  prb=vrbs(rb_cnt);
// 	else
// 	  prb=prbs(1); // considering only the second slot for prbs
// 	if(find(cchReservation->cchtonesPerRB(prb),54)!=-1)
// 	{localString+="pbch";pbch=54;}
// 	else if(find(cchReservation->cchtonesPerRB(prb),55)!=-1)
// 	{localString+="pbchUp";pbch=55;}
// 	else if(find(cchReservation->cchtonesPerRB(prb),56)!=-1)
// 	{localString+="pbchDown";pbch=56;}
//       }
//       if(cchReservation->cchInfo.enablePssSss)
//       {
// 	int prb;
// 	if(isLocalised)
// 	  prb=vrbs(rb_cnt);
// 	else
// 	  prb=prbs(0); // considering only the first slot for prbs
// 	if(find(cchReservation->cchtonesPerRB(prb),57)!=-1)
// 	{localString+="pssSss";pss=57;}
// 	else if(find(cchReservation->cchtonesPerRB(prb),58)!=-1)
// 	{localString+="pssSssUp";pss=58;}
// 	else if(find(cchReservation->cchtonesPerRB(prb),59)!=-1)
// 	{localString+="pssSssDown";pss=59;}
//       }
//     }
//     else //!Uplink
//     {
//       #ifdef USING_SRS          
//       if(cchReservation!=NULL)
// 	if(cchReservation->cchtonesPerRB.length())
// 	{
// 	  int prb;
// 	  prb=vrbs(rb_cnt); 
// 	  if(find(cchReservation->cchtonesPerRB(prb),60)!=-1)
// 	  {localString+="srs";srsPresent=true;}
// 	}
// 	#endif
//     }
//     if(!frameStructure->isResourceUnitAvailable(localString))
//     {
//       rbName.set_length(rsPorts.length());
//       for(int port_cnt=0;port_cnt<rsPorts.length();port_cnt++)
// 	rbName(port_cnt)="port"+toString(rsPorts(port_cnt));
//       
//       if(isDownlink)
//       {
// 	if(cchReservation->cchInfo.pdcchCount>0)
// 	  append(rbName,"pdcch"+toString(cchReservation->cchInfo.pdcchCount));
// 	
// 	if(pbch!=0)
// 	{
// 	  if(pbch==54)	  	{string temp="pbch";append(rbName,temp);}
// 	  else if(pbch==55)	{string temp="pbchUp";append(rbName,temp);}
// 	  else if(pbch==56)	{string temp="pbchDown";append(rbName,temp);}
// 	}
// 	if(pss!=0)
// 	{
// 	  if(pss==57)	  	{string temp="pssSss";append(rbName,temp);}
// 	  else if(pss==58)	{string temp="pssSssUp";append(rbName,temp);}
// 	  else if(pss==59)	{string temp="pssSssDown";append(rbName,temp);}
// 	}
//       }
//       else
//       {
// 	if(srsPresent)
// 	{
// 	  string temp="srs";
// 	  append(rbName,temp);
// 	}
//       }
//       if(rbName.length()>1)
// 	mergedRBs(rb_cnt)=frameStructure->getMergedResourceUnit(rbName(0),rbName(1,rbName.length()-1),"data");
//          else if(rbName.length()==1)
//       {
// 	Array<string> dummy(0);
// 	mergedRBs(rb_cnt)=frameStructure->getMergedResourceUnit(rbName(0),dummy,"data");
//       }
//     }
//     else
//       mergedRBs(rb_cnt)=frameStructure->getResourceUnitMatrix(localString);
//   }
//   return(mergedRBs);
// }

Array<imat> getMergedRBStructures(FrameStructure *frameStructure, RSInfo_S &rsInfo,ivec vrbs ,bool isDownlink,bool isLocalised,FrameReservationInfo_S *cchReservation)
{  
  
  Array<imat> mergedRBs(vrbs.length());
      string mergedRbName="";
      
  ivec rsPorts=rsInfo.rsPorts;    
  
  Array<ivec> rsSymPositions=rsInfo.rsSymPosition;
  
  Array<string> rbName(rsPorts.length());
  for(int port_cnt=0;port_cnt<rsPorts.length();port_cnt++)
  {
    mergedRbName+=toString(rsPorts(port_cnt));
    for(int sym_cnt=0;sym_cnt<rsSymPositions(port_cnt).length();sym_cnt++)
      mergedRbName+=("sp"+to_string(rsSymPositions(port_cnt)(sym_cnt)));
  }
  
  if(isDownlink)
  {
    if(cchReservation->cchInfo.pdcchCount>0)
      mergedRbName+="pdcch"+toString(cchReservation->cchInfo.pdcchCount);
  }
 
 std::ofstream fout1("RBStructureInfoFileName.txt", std::ios::out);
 
  for(int rb_cnt=0;rb_cnt<vrbs.length();rb_cnt++)
  {
    string localString=mergedRbName;
    int pbch=0,pss=0;
   
    bool srsPresent=false;
    ivec prbs;prbs.set_length(0);
    if(isDownlink)
    {
      prbs=cchReservation->dlPrbMapping(vrbs(rb_cnt));
//       if(cchReservation->cchInfo.enablePBCH)
//       {
// 	int prb;
// 	if(isLocalised)
// 	  prb=vrbs(rb_cnt);
// 	else
// 	  prb=prbs(1); // considering only the second slot for prbs
// 	if(find(cchReservation->cchtonesPerRB(prb),54)!=-1)
// 	{localString+="pbch";pbch=54;}
// 	else if(find(cchReservation->cchtonesPerRB(prb),55)!=-1)
// 	{localString+="pbchUp";pbch=55;}
// 	else if(find(cchReservation->cchtonesPerRB(prb),56)!=-1)
// 	{localString+="pbchDown";pbch=56;}
//       }
//       if(cchReservation->cchInfo.enablePssSss)
//       {
// 	int prb;
// 	if(isLocalised)
// 	  prb=vrbs(rb_cnt);
// 	else
// 	  prb=prbs(0); // considering only the first slot for prbs
// 	if(find(cchReservation->cchtonesPerRB(prb),57)!=-1)
// 	{localString+="pssSss";pss=57;}
// 	else if(find(cchReservation->cchtonesPerRB(prb),58)!=-1)
// 	{localString+="pssSssUp";pss=58;}
// 	else if(find(cchReservation->cchtonesPerRB(prb),59)!=-1)
// 	{localString+="pssSssDown";pss=59;}
//       }
     }
    else //!Uplink
    {
      #ifdef USING_SRS          
      if(cchReservation!=NULL)
	if(
        .length())
	{
	  int prb;
	  prb=vrbs(rb_cnt); 
	  if(find(cchReservation->cchtonesPerRB(prb),60)!=-1)
	  {localString+="srs";srsPresent=true;}
	}
	#endif
    }
    
    for(int sym_cnt=0;sym_cnt<cchReservation->reservedSymbols.length();sym_cnt++)
      localString+="b"+toString(cchReservation->reservedSymbols(sym_cnt));
    
    ivec reservedSymbols=cchReservation->reservedSymbols;
    int len1=reservedSymbols.length();
    Array<ivec> reservedSCPerSym(reservedSymbols.length());
    for(int cnt=0;cnt<reservedSymbols.length();cnt++)
      reservedSCPerSym(cnt).set_length(0);
    
    if(isDownlink && cchReservation->cchInfo.enableSSB && cchReservation->cchInfo.isSSBPresent)
    {
      int prb;
      if(isLocalised)
	prb=vrbs(rb_cnt);
      else
	prb=prbs(0); // considering only the first slot for prbs
	
	int prbStart=prb*12;
      int prbEnd=prb*12+11;
      int ssbStart=floor_i(cchReservation->cchInfo.SSBOffsetFromPRB0/12.0)*12;//cchReservation->cchInfo.SSBOffsetFromPRB0;
      int ssbEnd=ceil_i((cchReservation->cchInfo.SSBOffsetFromPRB0+cchReservation->cchInfo.SSBSubcarrierCount)/12.0)*12-1;//cchReservation->cchInfo.SSBOffsetFromPRB0+cchReservation->cchInfo.SSBSubcarrierCount-1;

      ivec blockedSymbols=cchReservation->cchInfo.SSBSymbols(findNot(cchReservation->cchInfo.SSBSymbols,cchReservation->reservedSymbols));  
     
      if((ssbStart<=prbStart && prbStart<=ssbEnd) || (ssbStart<=prbEnd && prbEnd<=ssbEnd)) // PRB is within SSB region
      {
	for(int sym_cnt=0;sym_cnt<blockedSymbols.length();sym_cnt++)
	  localString+="b"+toString(blockedSymbols(sym_cnt));
	
	append(reservedSymbols,blockedSymbols);
	if(ssbStart<=prbStart && prbStart<=ssbEnd && ssbStart<=prbEnd && prbEnd<=ssbEnd) // Full PRB is within region
	{
	  for(int cnt=0;cnt<blockedSymbols.length();cnt++)
	    append(reservedSCPerSym,ivec(""));
	}
	else if(ssbStart<=prbStart && prbStart<=ssbEnd) // End PRB
	{ 
	  for(int cnt=0;cnt<blockedSymbols.length();cnt++)
	  {
	    append(reservedSCPerSym,zeros_i(2));
	    reservedSCPerSym(cnt+len1)(0)=0;
	    reservedSCPerSym(cnt+len1)(1)=ssbEnd-floor(ssbEnd/12)*12;
	    
	    localString+="bsc0_"+toString(reservedSCPerSym(cnt+len1)(1));
	    
	  }
	}
	else // Start PRB
	{
	  for(int cnt=0;cnt<blockedSymbols.length();cnt++)
	  {
	    append(reservedSCPerSym,zeros_i(2));
	    reservedSCPerSym(cnt+len1)(0)=ssbStart-floor(ssbStart/12)*12;
	    reservedSCPerSym(cnt+len1)(1)=11;
	    
	    localString+="bsc"+toString(reservedSCPerSym(cnt+len1)(0))+"_11";
	  }
	}
      }
    }
    
    
    if(localString=="")
      localString="nullRB";
    
    
    if(!frameStructure->isResourceUnitAvailable(localString))
    {
      rbName.set_length(rsPorts.length());
      for(int port_cnt=0;port_cnt<rsPorts.length();port_cnt++)
   	rbName(port_cnt)="port"+toString(rsPorts(port_cnt));

      if(isDownlink)
      {
	if(cchReservation->cchInfo.pdcchCount>0)
	  append(rbName,"pdcch"+toString(cchReservation->cchInfo.pdcchCount));
	
	if(pbch!=0)
	{
	  if(pbch==54)	  	{string temp="pbch";append(rbName,temp);}
	  else if(pbch==55)	{string temp="pbchUp";append(rbName,temp);}
	  else if(pbch==56)	{string temp="pbchDown";append(rbName,temp);}
	}
	if(pss!=0)
	{
	  if(pss==57)	  	{string temp="pssSss";append(rbName,temp);}
	  else if(pss==58)	{string temp="pssSssUp";append(rbName,temp);}
	  else if(pss==59)	{string temp="pssSssDown";append(rbName,temp);}
	}
      }
      else
      {
	if(srsPresent)
	{
	  string temp="srs";
	  append(rbName,temp);
	}
      }
   
      Array<ivec> colPos=rsSymPositions;
      ivec rowPositions=rsInfo.rsScPosition;
      if(rbName.length()>rsSymPositions.length())
      {
	Array<ivec> emptySet(rbName.length()-rsSymPositions.length());
	append(colPos,emptySet);
	append(rowPositions,zeros_i(rbName.length()-rsSymPositions.length()));
      }
//       cout<<"colPos : "<<colPos<<endl;
      mergedRBs(rb_cnt)=frameStructure->getMergedResourceUnit("nullRB",rbName,"data", colPos,rowPositions,reservedSymbols,reservedSCPerSym);
// cout<<"mergedRBs from rb count  "<<mergedRBs(rb_cnt)<<endl;
      //       cout<<"mergedRBs(rb_cnt) : "<<endl;
//       cout<<mergedRBs(rb_cnt)<<endl;

     
    }
    else
      mergedRBs(rb_cnt)=frameStructure->getResourceUnitMatrix(localString);
    
    for (int i=0; i < mergedRBs(rb_cnt).rows(); i++)
    {
      for (int j=0; j < mergedRBs(rb_cnt).cols(); j++)
      {                      
	fout1 << mergedRBs(rb_cnt)(i,j) << "\t";
      }
      fout1<<std::endl;
    }
    
    fout1<<"\n";
  }
  fout1.close();
   
    
    return(mergedRBs);
}
// void getRStypes(ivec CQIports, ivec DeModPorts, RSType_E &CQIrsType, RSType_E &DeModrsType)
// {
//   
//   if(CQIports(0)>=0 && CQIports(0)<=4)
//     CQIrsType = _RSTYPE_CRS_;/// For TMs 1 to 6
//     else if(CQIports(0)>=15 && CQIports(0)<=22)
//       CQIrsType = _RSTYPE_CSIRS_;/// For TMs 7 to 9
//       else
//       {cout<<"[both:]Invalid CQIports exiting in getRStypes()..."<<endl;abort();}
//       
//       if(DeModPorts(0)>=0 && DeModPorts(0)<=4)
// 	DeModrsType = _RSTYPE_CRS_;/// For TMs 1 to 6
// 	else if(DeModPorts(0)==5 || (DeModPorts(0)>=7 && DeModPorts(0)<=14))
// 	  DeModrsType = _RSTYPE_DMRS_;/// For TMs 7 to 9
// 	  else
// 	  {cout<<"[both:]Invalid DeModPorts exiting in getRStypes()..."<<endl;abort();}
// }

//AbhiChanges:
void getRStypes(ivec CQIports, ivec DeModPorts, RSType_E &CQIrsType, RSType_E &DeModrsType)
{
  if(CQIports(0)>=0 && CQIports(0)<=4)
    CQIrsType = _RSTYPE_CRS_;/// For TMs 1 to 6
  else if(CQIports(0)>=15 && CQIports(0)<=22)
    CQIrsType = _RSTYPE_CSIRS_;/// For TMs 7 to 10
    else if((CQIports(0)>=3100 && CQIports(0)<=3432))
      CQIrsType = _RSTYPE_CSIRS_;/// For TMs 11
  else if(CQIports(0)>=1000 && CQIports(0)<=1007)
    CQIrsType = _RSTYPE_CSIRS_;/// For Uplink, using DMRS port numbers for CSIRS - Need to fix
      //CSIRS port nubmers are modified for ultople CSIrs
  else if(int(CQIports(0))/100>=15 && int(CQIports(0))/100<=22)
    CQIrsType = _RSTYPE_CSIRS_;/// For TMs 7 to 10
    else if((int(CQIports(0))/100>=3100 && int(CQIports(0))/100<=3432))
      CQIrsType = _RSTYPE_CSIRS_;/// For TMs 11 
      else if(CQIports(0)>=4000 && CQIports(0)<=4003)  //SRS... Need to change this - Dhiv
          CQIrsType = _RSTYPE_CSIRS_;/// For TMs 11 
          
  else
  {cout<<"[both:]Invalid CQIports..."<<CQIports<<" \nexiting in getRStypes()..."<<endl;abort();}
  
  if(DeModPorts(0)>=0 && DeModPorts(0)<=3)
    DeModrsType = _RSTYPE_CRS_;/// For TMs 1 to 6
  else if(DeModPorts(0)==5 || (DeModPorts(0)>=7 && DeModPorts(0)<=14))
    DeModrsType = _RSTYPE_DMRS_;/// For TMs 7 to 9
  else if(DeModPorts(0)>=1000 && DeModPorts(0)<=1007)
    DeModrsType = _RSTYPE_DMRS_;/// For TMs 1000 to 1011
    else if(DeModPorts(0)>=1100 && DeModPorts(0)<=1111)
    DeModrsType = _RSTYPE_DMRS_;/// For TMs 1000 to 1011
    else if(DeModPorts(0)>=2000 && DeModPorts(0)<=2007)
      DeModrsType = _RSTYPE_DMRS_;/// For TMs 2000 to 2007
  else
  {cout<<"[both:]Invalid DeModPorts exiting in getRStypes()..."<<endl;abort();}
}

ivec getPortReferenceNumbers(FrameStructure &frameStructure, ivec rsPorts){
     ivec portReferenceNumbers=zeros_i(rsPorts.length())-1;
    
  for(int port_cnt=0;port_cnt<rsPorts.length();port_cnt++)
  {     ResourceUnitInfo_S ruInfo=frameStructure.getResourceUnitInfo("port"+toString(rsPorts(port_cnt)));
    ivec temp=ruInfo.getToneNumbers("port"+toString(rsPorts(port_cnt)));
           if(temp.length()!=1)
    {
        cout<<"port"+toString(rsPorts(port_cnt))<<" : "<<temp<<endl;
      cout<<"[both:]Error ... PortReferenceNumber must be a single integer for a given port..."<<endl;abort();
    }
    portReferenceNumbers(port_cnt)=temp(0);
  }
    return(portReferenceNumbers);
}

// double getPowerHeadroomInDB(int nodeID, int& subframeNumber)
// {
//   int nodeIndx=find(powerHeadroomNodeIDs,nodeID);
//   subframeNumber = powerHeadroomInDBPerNode_subframeCount(nodeIndx);
//   return powerHeadroomInDBPerNode(nodeIndx);
// }
// 
// vec getPowerHeadroomInDB(ivec nodeIDs, ivec& subframeNumbers)
// {
//   ivec nodeIndices=find(powerHeadroomNodeIDs,nodeIDs);
//   subframeNumbers = powerHeadroomInDBPerNode_subframeCount(nodeIndices);
//   return powerHeadroomInDBPerNode(nodeIndices);
// }

ivec getCodeBlockSizesForTB(int tbSize, ivec& interleaverParameters_K){
  
  int maxSize,nSegmentSize,nSegmentSize_,iBlock;
  double nCodes_,nCodes,deltaSegmentSize,tbSizeEffective,crcLength;
  
  maxSize = 6144;
  
  if (tbSize <= maxSize)
  {
    crcLength = 0;
    nCodes = 1;
    tbSizeEffective = tbSize;
  }
  else
  {
    crcLength = 24;
    nCodes = ceil((double) tbSize / (double)(maxSize - crcLength));
    tbSizeEffective = tbSize + nCodes * crcLength;
  }
  
  for (iBlock = 0;iBlock < interleaverParameters_K.length();iBlock ++)
    if (nCodes * interleaverParameters_K(iBlock) >= tbSizeEffective)
      break;
    
    nSegmentSize = interleaverParameters_K(iBlock);
  
  if (nCodes == 1)
  {
    nCodes_ = 0;
    nSegmentSize_ = 0;
  }
  else
  {
    for (iBlock = interleaverParameters_K.length() - 1;iBlock >= 0;iBlock --)
      if (nSegmentSize > interleaverParameters_K(iBlock))
	break;
      
      nSegmentSize_ = interleaverParameters_K(iBlock);
    deltaSegmentSize = nSegmentSize - nSegmentSize_;
    nCodes_ = floor((nCodes * nSegmentSize - tbSizeEffective) / deltaSegmentSize);
    
    nCodes = nCodes - nCodes_;
  }
  
  //! Create output vector,
  ivec sizePerCodeBlock = concat(nSegmentSize*ones_i(int(nCodes)),nSegmentSize_*ones_i(int(nCodes_)));
  
  return sizePerCodeBlock;
}

ivec getCodeBlockSizesForTBNR(int tbSize, double tCodeRate){
  
  int BG=0,L=24,C=0,Kcb;
  
  if(tbSize<=292 || (tbSize<=3824 && tCodeRate<=0.67) || tCodeRate<= 0.25)
  {
    BG=2;
    Kcb=3840;
  }
  else
  {
    BG=1;
    Kcb=8448;
  }
  
  C = (tbSize<Kcb) ? 1 : ceil_i(tbSize/(double)(Kcb-L));
  
  int B_ = tbSize+C*L; // FIXME (C+1)*L ?
  auto r = (B_)%C;
  ivec K_(C);
  int startPos=0;
  for (int i = 0; i<C;i++)
  {
    if(i<r)   // This is assuming B_ may not be multiple of C.
    {
      K_(i) = B_/C+1;  // //FIXME ceiling for LTE compatability
    }
    else
    {
      K_(i) = B_/C;
    }
  }
  return K_;
}

TransmissionMode_E parseTransmissionMode(string fileName, bool isDownlink){
  
  string prefix;
  if(isDownlink) prefix="DL_";
  else	prefix="UL_";
  string transmissionMode;
  TransmissionMode_E TransmissionMode;
   if(parse(fileName,prefix+"TransmissionMode",transmissionMode))
   {
    TransmissionMode=getTransmissionMode(transmissionMode);
   }
   else
  {
    cout<<"[both:]Unable to load TransmissionMode from the Configuration File - myDLTxRxConfig / myULTxRxConfig."<<endl;abort();
  }
  
  return TransmissionMode;
}

int getSymbolRepetitionFactor(TransmissionScheme_E txnScheme, int nLayers)
{
  int SymbolRepititionFactor;
  
  switch(txnScheme)
  {
    case _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_:
      if(nLayers == 2)
	SymbolRepititionFactor = 2;
      else if(nLayers == 4)
	SymbolRepititionFactor = 4;/// Symbol repetition of 2 and 2 nuulls for FSTD.
	else
	{cout<<"[both:] Invalid nLayers ... exiting in getSymbolRepititionFactor()"<<endl;abort();}
	break;
    default:
      SymbolRepititionFactor = 1;
  }
  return SymbolRepititionFactor;
}


vec getSINRwithWhiteMRCperLayer(cmat precodedChannel,cmat Intrcov, int nLayers, double sigpowInWatts)
{
  vec sinr(nLayers);
  complex< double > noiseVariance= (trace(Intrcov)/complex< double >(Intrcov.rows(),0));// Taking avg interference power across antennas
  cmat B = sqrt((double)sigpowInWatts)*hermitian_transpose(precodedChannel)/((hermitian_transpose(precodedChannel)*precodedChannel)(0,0)+noiseVariance);
  
  cmat sigpwr = B*precodedChannel*precodedChannel.hermitian_transpose()*B.hermitian_transpose();
  cmat IntrPwr = B*(Intrcov)*B.hermitian_transpose();
  for(int iLayer=0; iLayer<nLayers; iLayer++)
  {
    sinr(iLayer) = abs(sigpwr(iLayer, iLayer))/abs(sum(sigpwr.get_row(iLayer))-sigpwr(iLayer, iLayer)+ IntrPwr(iLayer, iLayer));
    if (sigpwr(iLayer, iLayer)==0.0)
      sinr(iLayer)=0;
  }
  return(sinr);
}

vec getSINRwithMRCperLayer(cmat precodedChannel,cmat Intrcov, int nLayers, double sigpowInWatts)
{
  vec sinr(nLayers);
  cmat B = precodedChannel.hermitian_transpose()/sqrt(sigpowInWatts);
  cmat sigpwr = B*precodedChannel*precodedChannel.hermitian_transpose()*B.hermitian_transpose();
  cmat IntrPwr = B*(Intrcov)*B.hermitian_transpose();
  for(int iLayer=0; iLayer<nLayers; iLayer++)
  {
    sinr(iLayer) = abs(sigpwr(iLayer, iLayer))/abs(sum(sigpwr.get_row(iLayer))-sigpwr(iLayer, iLayer)+ IntrPwr(iLayer, iLayer));
    if (sigpwr(iLayer, iLayer)==0.0)
      sinr(iLayer)=0;
  }
  return(sinr);
}

vec getSINRwithMMSEperLayer(cmat precodedChannel, cmat Intrcov, int nLayers, double sigpowInWatts)
{
  vec sinr(nLayers);
  
  cmat MMSEfilter  =  sqrt(sigpowInWatts)*precodedChannel.H()*inv( precodedChannel*precodedChannel.H() + Intrcov);
  cmat EqChn   =  MMSEfilter*precodedChannel;
  cmat EquNoise = MMSEfilter*Intrcov*MMSEfilter.H();
  
  // Included non-diagonal terms in the noise variance 
  cvec temp=diag(EqChn);
  cmat desiredEquChn=diag(temp);
  cmat residualISI=EqChn-desiredEquChn;
  EquNoise += residualISI*residualISI.H();
  EqChn=desiredEquChn;
  
 // cout<<"temp : "<<temp<<endl;
 // cout<<"residualISI*residualISI.H() : "<<residualISI*residualISI.H()<<endl;
  
  for (int iLayer =0;iLayer <nLayers;iLayer++) // Assuming independent detection
  {
    EqChn(iLayer,iLayer) = EqChn(iLayer,iLayer)/sqrt(abs(EquNoise(iLayer,iLayer)));
    sinr(iLayer)=sqr(abs(EqChn(iLayer,iLayer)));
  }

// cout<<"SINR with MMSE filter: "<<sinr<<endl;
//   
//   
//   cmat tmpMat = precodedChannel;
//   
//   cmat tmpMat2 =inv(tmpMat.H()*inv(Intrcov)*tmpMat+eye_c(tmpMat.cols()));
//   
//   cvec tmpSinr = diag(tmpMat2);
//   
//   cout<<"tmpSinr: "<<tmpSinr<<endl;
//   
//   vec sinrDiff(nLayers);
//   
//   for (int iLayer =0;iLayer <nLayers;iLayer++) // Assuming independent detection
//   {
//       sinrDiff(iLayer) = 1/abs(tmpSinr(iLayer))-1;
//   }
//   
//   cout <<"SINR new: "<< sinrDiff<<endl;abort();
  
  
  
  return(sinr);
}

ivec getNumLayersPerCodeWord(int nCodeWords, int mimoRate)
{
  
  if(mimoRate < nCodeWords)
  {
    cout<<"nCodeWords :"<<nCodeWords<<" mimoRate :"<<mimoRate<<endl;
    cout<<"Unsupported nCodeWords and mimoRate"<<endl;
    abort();
  }
  
  ivec numLayersPerCodeWord;
  numLayersPerCodeWord.set_size(nCodeWords);
  
  switch(nCodeWords) {
    case 1:
      numLayersPerCodeWord(0) = mimoRate;
      break;
    case 2:
      numLayersPerCodeWord(0) = floor_i(mimoRate/2.0);
      numLayersPerCodeWord(1) = ceil_i(mimoRate/2.0);
      break;
    default:
      cout<<"[both:]Error: Number of LayersPerCodeWord is not defined for nCodeWords ="<<nCodeWords<<", not in the set {1,2}...\n";
      abort();
  }
  
  return(numLayersPerCodeWord);
}
cmat MatrixJkl(cmat reportedPrecoder, int layerNum, Array<cmat>& precodersOfComb, vec oldPPSinrOfComb, int UEIndxInComb)
{
    // Ref:  IEEE paper: MU-MIMO and CSI Feedback Performance of NR/LTE: Eqn 7
    cmat Jkl,sigmaMatrix;
    int nUEsinComb = precodersOfComb.length();
    //initialization
    Jkl = oldPPSinrOfComb(UEIndxInComb)*reportedPrecoder.get_col(layerNum);
    Jkl = Jkl.H();
//     cout<< "Jkl cols: "<<Jkl.cols()<<endl;
    for(int tmpCnt=0;tmpCnt<nUEsinComb;tmpCnt++)
    {
//         cout<<" precodersOfComb(tmpCnt): "<<precodersOfComb(tmpCnt)<<endl; 
        if(tmpCnt!=UEIndxInComb)
        {
            int rankOfReportedPrecoder = precodersOfComb(tmpCnt).cols(); // # of Columns of precoder
            sigmaMatrix = oldPPSinrOfComb(tmpCnt)*eye_c(rankOfReportedPrecoder);
            cmat tmp = sigmaMatrix.H()*precodersOfComb(tmpCnt).H();
//             cout<< "tmp cols: "<<tmp.cols()<<endl;
            Jkl = concat_vertical(Jkl,tmp);
        }
    }
//     cout<< "Jkl: "<<Jkl<<endl;
    
    return Jkl;
}

Array<vec> getModifiedSINRInDBperCodeword_SMMSE(cmat reportedPrecoder,Array<cmat>& precodersOfComb, Array<vec> oldPPSinrOfComb, int UEIndxInComb, cmat &modifiedPrecoder)
{
    // Ref:  IEEE paper: MU-MIMO and CSI Feedback Performance of NR/LTE
    int nCodeWords=oldPPSinrOfComb(UEIndxInComb).length(); 
    Array<vec> modifiedSINRsPerCodeword(nCodeWords); //<codewords<layers>>
    // Dimensions swaped to extract ppsinr of 1st codewors of all UEs in the combination
    // <User<codeword>> to <codeword<user>>
    Array<vec> oldPPSinrOfCombSwapped= swapDimensions(oldPPSinrOfComb); 
    if(precodersOfComb.length() != oldPPSinrOfCombSwapped(0).length())
    {
        // checking dimension swapDimensions
        cout<<"dimension mismatch (check swap dimensions)... aborting \n"<<endl;abort();
        
    }
    for (int iCodeword=0;iCodeword<nCodeWords;iCodeword++)
    {
        int nLayers = reportedPrecoder.cols();
        modifiedSINRsPerCodeword(iCodeword).set_length(nLayers);
        modifiedPrecoder=zeros_c(reportedPrecoder.rows(),nLayers);
        cmat Jkl, Pkl, interMatrix;
        for(int lyrCnt=0;lyrCnt<nLayers;lyrCnt++)
        {
            // eqn 7
            Jkl = MatrixJkl(reportedPrecoder, lyrCnt, precodersOfComb, oldPPSinrOfCombSwapped(iCodeword), UEIndxInComb);
//             cout<<"\n Jkl of UE Indx  "<<UEIndxInComb<<" is : \n"<<Jkl<<endl;
            interMatrix = inv(Jkl.H()*Jkl+eye_c(Jkl.cols())); // Eqn 6
            Pkl = interMatrix*reportedPrecoder.get_col(lyrCnt); // Eqn 6
//             cout<<"\n Pkl: "<<Pkl<<endl;
//             cout<<"interMatrix: "<<interMatrix<<endl;
            modifiedPrecoder.set_col(lyrCnt,Pkl.get_col(0));
            modifiedSINRsPerCodeword(iCodeword)(lyrCnt) = abs(dComplex(1,0)/(interMatrix(0,0))-dComplex(1,0)); // Eqn 8
        }
        
        for(int lyrCnt=0;lyrCnt<modifiedPrecoder.cols();lyrCnt++)
        {
            double normFactor = norm(modifiedPrecoder.get_col(lyrCnt));
//             cout<< " normFactor: "<<normFactor<<endl;
            modifiedPrecoder.set_col(lyrCnt, (1/normFactor)*modifiedPrecoder.get_col(lyrCnt));
        }
        modifiedPrecoder = (1/sqrt(modifiedPrecoder.cols()))*modifiedPrecoder;
        
//         cout<< "SMMSE: "<<modifiedSINRsPerCodeword(iCodeword)<<endl;
        
    }
    return modifiedSINRsPerCodeword;
}


vec getSINROffsetInDBwithMMSEperCodeword(vec oldPPSinr, cmat precodedChannelAcrossLayers, cmat additionalIntrcov, dComplex additionalScaling)
{
  int nLayers=precodedChannelAcrossLayers.cols();
  int nCodewords=oldPPSinr.length();
  int nR=additionalIntrcov.cols();
  vec sinrOffset(nCodewords);
  ivec nLayersPerCW=getNumLayersPerCodeWord(nCodewords,nLayers);
  for (int iCodeword=0;iCodeword<nCodewords;iCodeword++)
  {
    double newPPSinr=0;
    //averaging over layers
    for (int iLayer=0;iLayer<nLayersPerCW(iCodeword);iLayer++)
    {
      cmat hi=precodedChannelAcrossLayers.get_col(iLayer); // assuming hi has original scaling
      double oldPPNo=real((hi.H()*hi)(0))/oldPPSinr(iCodeword); // only noiseVariance estimation possible
      cmat newhi=hi*additionalScaling;
      newPPSinr+=real((newhi.H()*inv(additionalIntrcov+complex< double >(oldPPNo,0.0)*eye_c(nR))*newhi)(0)); 
    }
    newPPSinr/=(double)nLayersPerCW(iCodeword);
    sinrOffset(iCodeword)=dB(newPPSinr)-dB(oldPPSinr(iCodeword));
  }
  return(sinrOffset);
}

void computeCDMsizeAndcount(ivec layersPerUE,int& CDMSize,int& CDMCount,int dmrstype,int maxLength)
{
    int maxLayers = maxLength * ((dmrstype==1) ? 4 : 6);
    if(sum(layersPerUE)>maxLayers){cout<<"LayersPerUE: "<<layersPerUE<<"\tDMRS type "<<dmrstype<<" with MaxLength "<<maxLength<<" supports upto "<<maxLayers<<" only..."<<endl;abort();}
       
    if(dmrstype==1)
    {
        CDMSize = (maxLength==1) ? 2 : (max(layersPerUE)>2 || sum(layersPerUE)>4) ? 4 : 2;
        if(CDMSize==2)
            CDMCount = (sum(layersPerUE)<=2) ? 1 : 2;
        else
            CDMCount = (sum(layersPerUE)<=4) ? 1 : 2;
    }
    else
    {
        CDMSize = (maxLength==1) ? 2 : (max(layersPerUE)>2 || sum(layersPerUE)>6) ? 4 : 2;
        if(layersPerUE.length()==1) //SU-MIMO
        {
            CDMCount = ceil_i((double)layersPerUE(0)/(double)CDMSize); return;
        }
        //MU-MIMO
        if(CDMSize==2)
            CDMCount = (sum(layersPerUE)<=2) ? 1 : (sum(layersPerUE)<=4 && max(layersPerUE)<=2) ? 2 : 3;
        else
            CDMCount = (sum(layersPerUE)<=4) ? 1 : (sum(layersPerUE)<=8 && !sum(layersPerUE!=2)) ? 2 : 3;
    }
}




/*! Check if gold-Sequence already exists in repository(Already stored sequences), if not then generate the sequence & store it to repository if repository memory doesn't exceed limit.
 * \ref 36.211v10.2.0 section:7.2	(page:100)*/
ivec generateGoldSequence(int Cinit, int seqLength)
{
  //! Check If Cinit already there with (length >= seqLength), then pick the sequence & return
  if(length(goldSequence[Cinit]) >= seqLength)
  {
    return goldSequence[Cinit](0,seqLength-1);
  }
  //! else generate the sequence.....
  else
  {
    
    ivec tempGoldSequence = goldIvec::generateGoldSequence(Cinit, seqLength);

    // bvec secSequence = reverse(dec2bin(nSeq,(int)Cinit));
    // 
    // secSequence.set_length(Nc+seqLength+nSeq,1);
    // firstSequence.set_length(Nc+seqLength+nSeq,1);
    // 
    // for(int n=0;n<seqLength+Nc;n++)
    // {
    //     firstSequence(n+31) = firstSequence(n + 3) + firstSequence(n);
    //     secSequence(n+31) = secSequence(n + 3) + secSequence(n + 2) + secSequence(n + 1) + secSequence(n);
    // }
    // 
//  //    for (int iBit = 0;iBit < (Nc+seqLength-nSeq);iBit ++)
//  //    {
//  //      append(firstSequence, firstSequence(iBit + 3) + firstSequence(iBit));
//  //      append(secSequence, secSequence(iBit + 3) + secSequence(iBit + 2) + secSequence(iBit + 1) + secSequence(iBit));
//  //    }
    // 
    // ivec tempGoldSequence = to_ivec(firstSequence(Nc,Nc+seqLength-1) + secSequence(Nc,Nc+seqLength-1));
    
    //! If goldSequence size is within the limit(ie, if it doesn't consume much memory), then store the sequence..
    //! \todo total memory taken has to be considered instead of number of ivecs...
    if(goldSequence.size() < 1000)
//       goldSequence[Cinit] = tempGoldSequence;
        goldSequence.insert(std::pair<int,ivec>(Cinit,tempGoldSequence));
    
    return tempGoldSequence;
  }
}

/*!\brief Perform DFT/IDFT precoding for LTE-uplink. This module inserted after Layer-mapper & before Precoder.
 * 
 * \param[in] dataCarriers  ModulatedSymbols after Layer-Mapper
 * 
 * \param[in] isDFT         if true, then take \b DFT. Otherwise do \b IDFT.
 * 
 * \param[in] transformSize Inputed dataCarriers is divided into blocks each with length transformSize. Then FFT is applied to each block & concatenated to ouput \
 * 
 * By Default, set to dataCarriers.length()
 * 
 * \return finalData        DFT/IDFT precoded Data.
 * 
 * \ref 3GPP TS 36.211 version 10.2.0 Release 10 -Section 5.3.3: Transform Precoding. (page:17,18) */

Array<cvec> performTransformPrecodingForUplink(const  Array<cvec> &dataCarriersPerLayer,bool isDFT, int transformSize) // 
{
  
  cvec tempData,modifiedData;
  Array<cvec> finalData(dataCarriersPerLayer.length());
  int startIndex,blockIndex,totalBlocks;
  
  for(int layer_cnt=0;layer_cnt<dataCarriersPerLayer.length();layer_cnt++)
  {
    startIndex = 0;
    totalBlocks = ceil(dataCarriersPerLayer(layer_cnt).length() / double(transformSize));
    finalData(layer_cnt).set_size(0);
    if (isDFT)
    {
      for (blockIndex = 0;blockIndex < totalBlocks;blockIndex ++)
      {
	tempData = dataCarriersPerLayer(layer_cnt).mid(startIndex,transformSize);
	modifiedData = fft(tempData) / sqrt((double)transformSize);
	startIndex += transformSize;
	append(finalData(layer_cnt),modifiedData);
      }
      
    }
    else
    {
      for (blockIndex = 0;blockIndex < totalBlocks;blockIndex ++)
      {
	tempData = dataCarriersPerLayer(layer_cnt).mid(startIndex,transformSize);
	modifiedData = ifft(tempData) * sqrt((double)transformSize);
	startIndex += transformSize;
	append(finalData(layer_cnt),modifiedData);
      }
    }
  }
  return finalData;
}
  
Array<cvec> getFreqDomainFrame(Array<cmat> &subframe)
{
  Array<cvec> txFrameFD(subframe.length());
  for(int ant_cnt=0;ant_cnt<subframe.length();ant_cnt++)
  {
    txFrameFD(ant_cnt).set_length(0);
    for(int sym_cnt=0; sym_cnt<subframe(ant_cnt).cols(); sym_cnt++)
      append(txFrameFD(ant_cnt), subframe(ant_cnt).get_col(sym_cnt));
  }
  return txFrameFD;
}
  
  
  Array<cmat> getFreqDomainFrame(Array<cvec> &timeDomainFrame, int fftSize, int startSym, int nSymbols)
  {
      Array<cmat> freqFrame(timeDomainFrame.length());
      int currentSymbolCP;
      cvec cyclicPrefix(0);
      cvec temp;
      for(int ant_cnt=0;ant_cnt<timeDomainFrame.length();ant_cnt++)
      {
          int startSample=0,endSample;
          freqFrame(ant_cnt).set_size(fftSize,nSymbols);
          for(int sym_cnt=0; sym_cnt<nSymbols; sym_cnt++)
          {
              currentSymbolCP=getCPLength(fftSize,sym_cnt+startSym);
              int symbolLength = fftSize+currentSymbolCP;
              endSample=startSample+symbolLength-1;
              temp=timeDomainFrame(ant_cnt)(startSample,endSample);
              startSample+=symbolLength;
              temp=temp.right(fftSize);
              temp = fft(temp)/(double)fftSize;
              temp=fftShift(temp);
              freqFrame(ant_cnt).set_col(sym_cnt,temp);
          }
      }
      return freqFrame;
  }
  
  Array<cmat> getFreqDomainFrame(Array<cmat> &timeDomainFrame, int fftSize)
  {
      Array<cmat> freqFrame(timeDomainFrame.length());
      cvec temp;
      int nSymbols = timeDomainFrame(0).cols();
      for(int ant_cnt=0;ant_cnt<timeDomainFrame.length();ant_cnt++)
      {
          freqFrame(ant_cnt)=zeros_c(fftSize,nSymbols);
          for(int sym_cnt=0; sym_cnt<nSymbols; sym_cnt++)
          {
              temp=timeDomainFrame(ant_cnt).get_col(sym_cnt).right(fftSize);
              temp = fft(temp)/(double)fftSize;
              temp=fftShift(temp);
              freqFrame(ant_cnt).set_col(sym_cnt,temp);
          }
      }
      return freqFrame;
  }
  
int getCPLength(int fftSize,int symb_cnt,int scsInkHz)
{
      int ExtraCPSamplesForFirstSymbol=16, CPlength=144, TDsamples = 2048;
      int scaling=scsInkHz/15;
      int currentSymbolCP;
      
      if(symb_cnt==0 || symb_cnt==7*scaling)
          currentSymbolCP=(int)((CPlength+ExtraCPSamplesForFirstSymbol*scaling)*((double)fftSize/(double)TDsamples));
      else
          currentSymbolCP=(int)(CPlength*((double)fftSize/(double)TDsamples));
      
      return currentSymbolCP;
}
  
Array<cvec> getTimeDomainFrame(Array<cmat> &frequencyDomainFrame, int fftSize, int startSym)
{
  Array<cvec> txFrameTD(frequencyDomainFrame.length());
  int currentSymbolCP;
  cvec cyclicPrefix(0);
  cvec temp;
  for(int ant_cnt=0;ant_cnt<frequencyDomainFrame.length();ant_cnt++)
  {
    txFrameTD(ant_cnt).set_length(0);
    for(int sym_cnt=0; sym_cnt<frequencyDomainFrame(ant_cnt).cols(); sym_cnt++)
    {
      currentSymbolCP=getCPLength(fftSize,sym_cnt+startSym);
      temp=frequencyDomainFrame(ant_cnt).get_col(sym_cnt);
      temp=fftShift(temp);
      temp = ifft(temp)*(double)fftSize;
      cyclicPrefix = temp.right(currentSymbolCP);
      temp=concat(cyclicPrefix,temp);
      append(txFrameTD(ant_cnt), temp);
    }
  }
  return txFrameTD;
}
Array<cmat> getTimeDomainFrameMat(Array<cmat> &frequencyDomainFrame, int fftSize, int startSym)
{
    Array<cmat> txFrameTD(frequencyDomainFrame.length());
    int currentSymbolCP=getCPLength(fftSize,1); // fixed CP over all symbols
    cvec cyclicPrefix(0);
    cvec temp;
    for(int ant_cnt=0;ant_cnt<frequencyDomainFrame.length();ant_cnt++)
    {
        txFrameTD(ant_cnt)=zeros_c(fftSize+currentSymbolCP,frequencyDomainFrame(ant_cnt).cols());
        for(int sym_cnt=0; sym_cnt<frequencyDomainFrame(ant_cnt).cols(); sym_cnt++)
        {
//             currentSymbolCP=getCPLength(fftSize,sym_cnt+startSym);
            temp=frequencyDomainFrame(ant_cnt).get_col(sym_cnt);
            temp=fftShift(temp);
            temp = ifft(temp)*(double)fftSize;
            cyclicPrefix = temp.right(currentSymbolCP);
            temp=concat(cyclicPrefix,temp);
            txFrameTD(ant_cnt).set_col(sym_cnt,temp);
        }
    }
    return txFrameTD;
}

// void printReAssociation(AssociationInfo_S& associationInfo, string linkTableFileName, Array< Array< vec > >& RSRP,string rsrpDistributionFileName,string dlRSRPcouplingFileName)
// {
//   ivec serverNodes,serviceNodes,serverNodeCentralUnitID,serviceNodeCentralUnitID,serverIDsPerServiceNode,serverNodeNetworkIDs,serviceNodeNetworkIDs;
//   vec serverNodeTransmitPowerInDBm,serviceNodeTransmitPowerInDBm;
//   mat downlinkLinkGainInDB,uplinkLinkGainInDB;
//   mat downlinkPowerMatrixInDBm;
//   ivec serverNodeType;
//   vec serverNodeCarrierFreqInHz; 
//   cout<<"[both:] Printing re association ..."<<endl;
//   if(parse(linkTableFileName,"serverNodeTransmitPowerInDBm",serverNodeTransmitPowerInDBm)==false)  {	cout<<"[both:]Error... Unable to load serverNodeTransmitPowerInDBm from the file in associateNodes()..."<<endl;	abort();  }
//   if(parse(linkTableFileName,"serviceNodeTransmitPowerInDBm",serviceNodeTransmitPowerInDBm)==false)  {	cout<<"[both:]Error... Unable to load serviceNodeTransmitPowerInDBm from the file in associateNodes()..."<<endl;	abort();  }
//   if(parse(linkTableFileName,"serverToServiceNodeLinkGainInDB",downlinkLinkGainInDB)==false)  {	cout<<"[both:]Error... Unable to load serverToServiceNodeLinkGainInDB from the file in associateNodes()..."<<endl;	abort();  }
//   if(parse(linkTableFileName,"serviceToServerNodeLinkGainInDB",uplinkLinkGainInDB)==false)  {	cout<<"[both:]Error... Unable to load serviceToServerNodeLinkGainInDB from the file in associateNodes()..."<<endl;	abort();  }
//    
//   double BandwidthInHz = 1e7; //FIXME BandwidthInHz in hardcoded
//   vec serviceNodeReceiveNoiseFigureInDB;
//   if(parse(linkTableFileName,"serviceNodeReceiveNoiseFigureInDB",serviceNodeReceiveNoiseFigureInDB)==false)  {	cout<<"[both:] Error... Unable to load serviceNodeReceiveNoiseFigureInDB from the file in associateNodes()..."<<endl;	abort();  }
//   vec serviceNodeReceiveNoiseVariance=inv_dB(-203.8 + 10*log10(BandwidthInHz) + serviceNodeReceiveNoiseFigureInDB);
// 
//   downlinkPowerMatrixInDBm=downlinkLinkGainInDB;
//   for(int col_cnt=0;col_cnt<downlinkPowerMatrixInDBm.cols();col_cnt++)
//     downlinkPowerMatrixInDBm.set_col(col_cnt,downlinkPowerMatrixInDBm.get_col(col_cnt)+serverNodeTransmitPowerInDBm(col_cnt));
//   Array< Array <vec > >RSRPwithPower=RSRP;
//   for(int cnt1=0;cnt1<RSRP.length();cnt1++)
//     for(int cnt2=0;cnt2<RSRP(cnt1).length();cnt2++)
//     {
//       RSRPwithPower(cnt1)(cnt2) = RSRP(cnt1)(cnt2)+serverNodeTransmitPowerInDBm(cnt2)-30;
//     }
//   computeGeometrySINR(associationInfo,downlinkPowerMatrixInDBm,serviceNodeReceiveNoiseVariance);
//   
//   if(currentTaskid==0)
//   {
//     printCouplingGain(associationInfo,RSRP,dlRSRPcouplingFileName);
//     printSINRDistribution(associationInfo,RSRPwithPower,serviceNodeReceiveNoiseVariance,rsrpDistributionFileName);
// 
//   }
// 
// }
void getSpecialSubFrameConfig(SpecialSubFrameConfig_S *SSFConfig,string simFileName)  //returns SSFConfiguration,nDwPts,nUpPts using the input "SpecialSubframeConfig" given in mySimulationConfig.txt
{    
       int temp; 
       bool isnormalCP=true;
       if(!parse(simFileName,"SpecialSubframeConfig",temp)) 
       SSFConfig->SSFConfiguration=_SSF_CONF_UNINIT_;
       SSFConfig->SSFConfiguration=(SpecialSubFrameConfiguration_E) temp;
       
     //reference: 3GPP TS 36.211 version 12.7.0 Release 12 
     if(SSFConfig->SSFConfiguration!=_SSF_CONF_UNINIT_ && isnormalCP)// for normal CP
     {
      if     (SSFConfig->SSFConfiguration==_SSF_CONF_0_)	 {SSFConfig->nDwPts= 3;	SSFConfig->nUpPts=1;}
      else if(SSFConfig->SSFConfiguration==_SSF_CONF_1_)	 {SSFConfig->nDwPts= 9;	SSFConfig->nUpPts=1;}
      else if(SSFConfig->SSFConfiguration==_SSF_CONF_2_)	 {SSFConfig->nDwPts=10;	SSFConfig->nUpPts=1;}
      else if(SSFConfig->SSFConfiguration==_SSF_CONF_3_)	 {SSFConfig->nDwPts=11;	SSFConfig->nUpPts=1;}
      else if(SSFConfig->SSFConfiguration==_SSF_CONF_4_)	 {SSFConfig->nDwPts=12;	SSFConfig->nUpPts=1;}
      else if(SSFConfig->SSFConfiguration==_SSF_CONF_5_)	 {SSFConfig->nDwPts= 3;	SSFConfig->nUpPts=2;}
      else if(SSFConfig->SSFConfiguration==_SSF_CONF_6_)	 {SSFConfig->nDwPts= 9;	SSFConfig->nUpPts=2;}
      else if(SSFConfig->SSFConfiguration==_SSF_CONF_7_)	 {SSFConfig->nDwPts=10;	SSFConfig->nUpPts=2;}
      else if(SSFConfig->SSFConfiguration==_SSF_CONF_8_)	 {SSFConfig->nDwPts=11;	SSFConfig->nUpPts=2;}
      else         /*if(uldlConf==_SSF_CONF_9_)*/                {SSFConfig->nDwPts= 6;	SSFConfig->nUpPts=2;}
     }
     else if(SSFConfig->SSFConfiguration!=_SSF_CONF_UNINIT_ && !isnormalCP)// for extended CP
     {
      if     (SSFConfig->SSFConfiguration==_SSF_CONF_0_)	 {SSFConfig->nDwPts= 3;	SSFConfig->nUpPts=1;}
      else if(SSFConfig->SSFConfiguration==_SSF_CONF_1_)	 {SSFConfig->nDwPts= 8;	SSFConfig->nUpPts=1;}
      else if(SSFConfig->SSFConfiguration==_SSF_CONF_2_)	 {SSFConfig->nDwPts= 9;	SSFConfig->nUpPts=1;}
      else if(SSFConfig->SSFConfiguration==_SSF_CONF_3_)	 {SSFConfig->nDwPts=10;	SSFConfig->nUpPts=1;}
      else if(SSFConfig->SSFConfiguration==_SSF_CONF_4_)	 {SSFConfig->nDwPts= 3;	SSFConfig->nUpPts=2;}
      else if(SSFConfig->SSFConfiguration==_SSF_CONF_5_)	 {SSFConfig->nDwPts= 8;	SSFConfig->nUpPts=2;}
      else if(SSFConfig->SSFConfiguration==_SSF_CONF_6_)	 {SSFConfig->nDwPts= 9;	SSFConfig->nUpPts=2;}
      else         /*if(uldlConf==_SSF_CONF_7_)*/                {SSFConfig->nDwPts= 5;	SSFConfig->nUpPts=2;}
     }
    else
    {
      cout<<"Error in setting SpecialSubframeConfiguration for TDD Mode"<<endl;
      abort();
    }
}

FrameConfigInfo_S getFrameConfigurationInfo(UlDlMode_E uldlMode,TDDConfiguration_E tddConf,ivec CellConfig, int CellConfigPeriodicityInSlots){
  
  FrameConfigInfo_S frameConfigInfo;
  if(uldlMode==_ULDL_MODE_FDD_)
  {
    frameConfigInfo.dlFrameIndx="1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 ";
    frameConfigInfo.framePeriodicityInMilliSec=1;
  }
  else
  {
    if(uldlMode==_ULDL_MODE_UPLINK_)	 {  frameConfigInfo.dlFrameIndx=zeros_i(10);frameConfigInfo.framePeriodicityInMilliSec=1;}
    else 	 {  frameConfigInfo.dlFrameIndx=ones_i(10);frameConfigInfo.framePeriodicityInMilliSec=1;}
    
    if(uldlMode==_ULDL_MODE_TDD_ && tddConf!=_TDD_CONF_UNINIT_ && CellConfig.length()==0)  //Special Subframe ::  21-DwPts 20-UpPts 
    { 
      if     (tddConf==_TDD_CONF_0_)	{	frameConfigInfo.dlFrameIndx="1 0 1 0 1 0 1 0 1 0";	frameConfigInfo.framePeriodicityInMilliSec=5;}
      else if(tddConf==_TDD_CONF_1_)	{     	frameConfigInfo.dlFrameIndx="1 1 0 0 1 1 1 0 0 1";	frameConfigInfo.framePeriodicityInMilliSec=5;}
      else if(tddConf==_TDD_CONF_2_)	{	frameConfigInfo.dlFrameIndx="1 1 0 1 1 1 1 0 1 1";	frameConfigInfo.framePeriodicityInMilliSec=5;}
      else if(tddConf==_TDD_CONF_3_)	{	frameConfigInfo.dlFrameIndx="1 1 0 0 0 1 1 1 1 1";	frameConfigInfo.framePeriodicityInMilliSec=10;}
      else if(tddConf==_TDD_CONF_4_)	{	frameConfigInfo.dlFrameIndx="1 1 0 0 1 1 1 1 1 1";	frameConfigInfo.framePeriodicityInMilliSec=10;}
      else if(tddConf==_TDD_CONF_5_)	{	frameConfigInfo.dlFrameIndx="1 1 0 1 1 1 1 1 1 1";	frameConfigInfo.framePeriodicityInMilliSec=10;}
      else /*if(uldlConf==_TDD_CONF_6_)*/{	frameConfigInfo.dlFrameIndx="1 1 0 0 0 1 1 0 0 1";	frameConfigInfo.framePeriodicityInMilliSec=5;}
      
    }
    else if(CellConfig.length()!=0)//Cell Specific UL/DL Configuration
    {
        int periodicityInSlots=CellConfigPeriodicityInSlots;
        frameConfigInfo.dlFrameIndx=ones_i(CellConfig(0));//Full Dl slots
        int flexiSlots=periodicityInSlots-CellConfig(0)-CellConfig(2);
        ivec flexibleSlots="21 20";
      
        for(int i=0;i<flexiSlots;i++)
            append(frameConfigInfo.dlFrameIndx,flexibleSlots);
       
        append(frameConfigInfo.dlFrameIndx,zeros_i(CellConfig(2)));//Full UL slots
        
    }
  }
  return(frameConfigInfo);
}

  
  int getNumOfBitsForPrecoder(int nPorts,int nCodewords)
  {
    if(nPorts==2)
    {
      switch(nCodewords)
      {
	case 1:return(2);break;
	case 2:return(1);break;
	default: {cout<<"Invalid no. of codewords "<<nCodewords<<" in getNumOfBitsForPrecoder"<<endl;abort();}      
      }
    }
    else if(nPorts==4)
    {
      switch(nCodewords)
      {
	case 1:
	case 2: return(4);break;
	default: {cout<<"Invalid no. of codewords "<<nCodewords<<" in getNumOfBitsForPrecoder"<<endl;abort();}      
      }
    }
    else
    {
      cout<<"no of Ports "<<nPorts<<" not supported in getNumOfBitsForPrecoder"<<endl;abort();
    }
  }
  int getHigherLayerSubbandDiffCQIvalue(int WidebandCQIIndex, int SubbandCQIIndex)
  {
    int offsetLevel=SubbandCQIIndex-WidebandCQIIndex;
    if(offsetLevel==0 || offsetLevel==1)
      return(offsetLevel);
    else if(offsetLevel>=2)
      return(2);
    else if(offsetLevel<=-1)
      return(3);
    
    return(-1);
  }
  
  uint8_t getCQIIndexForHigherLayerSubband(unsigned int WidebandCQIIndex, unsigned int SubbandCQIIndexOffset)
  {
     int SubbandCqi;
   
    if (SubbandCQIIndexOffset==0 || SubbandCQIIndexOffset==1)
    SubbandCqi=min((int)(WidebandCQIIndex+SubbandCQIIndexOffset),15);
    else if(SubbandCQIIndexOffset==2)
    SubbandCqi=min((int)(WidebandCQIIndex+2),15);
    else if(SubbandCQIIndexOffset==3)
      SubbandCqi=max((int)(WidebandCQIIndex-1),1);
    else
    {
      cout<<"[detl:] Invalid SubbandCQIIndexOffset in getCQIIndexForHigherLayerSubband() "<<endl;abort();
    }
   
    return((uint8_t)SubbandCqi);
  }
  
  int getUESelectedSubbandDiffCQIvalue(int widebandCQIIndex,int SelectedSubbandsCQiIndex)
  {
    int offsetLevel=SelectedSubbandsCQiIndex-widebandCQIIndex;
    if(offsetLevel==2 || offsetLevel==3)
      return(offsetLevel-1);
    else if(offsetLevel<=1)
      return(0);
    else if(offsetLevel>=4)
      return(3);  
    
    return(-1);
  }
  
  int getUESelectedSubbandCQIvalue(int widebandCQIIndex,int SelectedSubbandsCQiIndexOffset)
  {
    if(SelectedSubbandsCQiIndexOffset==1 || SelectedSubbandsCQiIndexOffset==2)
    {
      return(min((int)(widebandCQIIndex+SelectedSubbandsCQiIndexOffset+1),15));
    }
    else if(SelectedSubbandsCQiIndexOffset==0)
    {
      return(min((int)(widebandCQIIndex+1),15));
    }
    else if(SelectedSubbandsCQiIndexOffset==3)
    {
      return(min((int)(widebandCQIIndex+4),15));
    }
    else
    {
      cout<<"[detl:] Invalid SelectedSubbandsCQiIndexOffset in getUESelectedSubbandCQIvalue()"<<endl;abort();
    }
  }
  
  
  Array<int> getBestMSubbands(int CombinatorialIndex,int nDLRBs)
  {    
    cout<<"Combinatorial index:"<<CombinatorialIndex<<endl;
    int M=getNumberOfUESelectedSubbands(nDLRBs);
    int N=getSubbandSizeAperiodicUESelected(nDLRBs).length();
    Array<int> Index;
    Index.set_length(M);
    int temp=M;
    int computedValue=0;
    int ind;
    for(int nItr=0;nItr<M;nItr++)
    {
      ind=temp-1;
      if(ind<0)
      {
	ind=0;
      }
      computedValue=NCr(ind,temp);
      while(computedValue<=CombinatorialIndex)
      {
	ind++;
	computedValue= NCr(ind,temp);
	
      }
    cout<<"ind:	"<<ind<<endl;
      if(ind>=1)
      {
      Index(nItr)=N-(ind-1);
      CombinatorialIndex=CombinatorialIndex-NCr(ind-1,temp);
      }
      else
      {
	Index(nItr)=N-0;
	CombinatorialIndex=CombinatorialIndex-NCr(0,temp);;
      }
      
      temp--;
    }
    
    return(Index);
  }
/*  
bool getLatestDLCQI(PerUserDLCQIFeedback_S perUserCQIReport,bool isAPeriodicCQIConfigured, AperiodicReportingMode_E aperiodicMode, PeriodicReportingMode_E periodicMode , ivec codewordIndicesToConsider,int DLRBs,ivec rbs,ivec &CQIPerCodeword,int &PMI, int requestID){
    
    int nCodewords=codewordIndicesToConsider.length();
    int measurementIndx=0;
    if(requestID!=-1)
      measurementIndx=perUserCQIReport.getMeasurementIndx(requestID);
    
    ///  this function assumes localised mapping
    if(perUserCQIReport.isValid)
    {
      if(isAPeriodicCQIConfigured&&perUserCQIReport.APeriodicReportedSubframe>=0)
      {
	if(aperiodicMode==_APERIODIC_MODE_1_2_)
	{
	CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
	  ivec subbandSize=getSubbandSizeAperiodic(DLRBs);
	  int sum=0;//temp
	  for(int Cnt=0;Cnt<subbandSize.length();Cnt++)
	  {
	    sum+=subbandSize(Cnt);
	    if(rbs(0)<sum)
	    {
	    PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPMI(Cnt);
	      break;
	    }
	  }
	}
	else if(aperiodicMode==_APERIODIC_MODE_3_0_||aperiodicMode==_APERIODIC_MODE_3_1_||aperiodicMode==_APERIODIC_MODE_3_2_)
	{
	  PMI=-1;
	  ivec subbandSize=getSubbandSizeAperiodic(DLRBs);
	  if(rbs.length()==1)
	  {
	    int sum=0;//temp
	    for(int Cnt=0;Cnt<subbandSize.length();Cnt++)
	    {
	      sum+=subbandSize(Cnt);
	      if(rbs(0)<sum)
	      {
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(Cnt);
		
		if(aperiodicMode==_APERIODIC_MODE_3_1_)
		  PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI;
		
		else if(aperiodicMode == _APERIODIC_MODE_3_2_)
		{
		  PMI = perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPMI(Cnt);
		}
		break;
	      }
	    }
	  }
	  else
	  {
	    int sum=0;//temp 
	    for(int Cnt=0;Cnt<subbandSize.length();Cnt++)
	    {
	      sum+=subbandSize(Cnt);
	      if(rbs(0)<sum&&rbs(rbs.length()-1)<sum)
	      {
		
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(Cnt);
		
		#ifdef USING_CALIBRATION 
		//to enable the wideband cqi in a particularband
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
		#endif
		if(aperiodicMode==_APERIODIC_MODE_3_1_)
		  PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI;
		
		
		else if(aperiodicMode == _APERIODIC_MODE_3_2_)
		{
		  PMI = perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPMI(Cnt);
		}
		
		break;
	      }
	      else if((Cnt+1)<subbandSize.length())// Doubt here why this case is necessary
		if(rbs(0)<sum&&rbs(rbs.length()-1)<sum+subbandSize(Cnt+1))
		{
		  CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(Cnt);
		  CQIPerCodeword+=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(Cnt+1);
		  CQIPerCodeword=CQIPerCodeword/2;
		  #ifdef USING_CALIBRATION 
		  //to enable the wideband cqi in a particularband
		  CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
		  #endif
		  
		  if(aperiodicMode==_APERIODIC_MODE_3_1_)
		    PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI;
		  
		
		  // Here As RBG is overlaping with two adjacent subbands assigning PMI to respective RBG is not clear as wideband PMI is not availabel
		  // Assigning subband PMI of first subband to RBG
		  else if(aperiodicMode == _APERIODIC_MODE_3_2_) 
		  {
		    PMI = perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPMI(Cnt);
		  }
		  break;
		}
	    }
	  }
	  
	}
	  PMI=-1;
	  ivec subbandSize=getSubbandSizeAperiodic(DLRBs);
	  if(rbs.length()==1)
	  {
	    int sum=0;//temp
	    for(int Cnt=0;Cnt<subbandSize.length();Cnt++)
	    {
	      sum+=subbandSize(Cnt);
	      if(rbs(0)<sum)
	      {
	      CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(Cnt);
		
		if(aperiodicMode==_APERIODIC_MODE_3_1_)
		PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI;
		
		break;
	      }
	    }
	  }
	  else
	  {
	    int sum=0;//temp 
	    for(int Cnt=0;Cnt<subbandSize.length();Cnt++)
	    {
	      sum+=subbandSize(Cnt);
	      if(rbs(0)<sum&&rbs(rbs.length()-1)<sum)
	      {
		
	      CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(Cnt);
		
		#ifdef USING_CALIBRATION 
		//to enable the wideband cqi in a particularband
	      CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
		#endif
		if(aperiodicMode==_APERIODIC_MODE_3_1_)
		PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI;
		
		break;
	      }
	      else if((Cnt+1)<subbandSize.length())
		if(rbs(0)<sum&&rbs(rbs.length()-1)<sum+subbandSize(Cnt+1))
		{
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(Cnt);
		CQIPerCodeword+=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(Cnt+1);
		  CQIPerCodeword=CQIPerCodeword/2;
		  #ifdef USING_CALIBRATION 
		  //to enable the wideband cqi in a particularband
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
		  #endif
		  
		  if(aperiodicMode==_APERIODIC_MODE_3_1_)
		  PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI;
		  
		  break;
		}
	    }
	  }
	  
	}
	else if(aperiodicMode==_APERIODIC_MODE_2_0_)
	{
	  ivec subbandSize=getSubbandSizeAperiodicUESelected(DLRBs);
	  int index,index1;
	  int sum=0;//temp
	  for(int Cnt=0;Cnt<subbandSize.length();Cnt++)
	  {
	    sum+=subbandSize(Cnt);
	    if(rbs(0)<sum&&rbs(rbs.length()-1)<sum) //!RBG is entirely inside the subband
	    {     
	    index=find(perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,Cnt);
	      if(index!=-1)
	      {
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord;
		#ifdef USING_CALIBRATION
		if(traceValuePerTraceName["PUSCH_2_0wideband"]=="true")
		  CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
		#endif
	      }
	      else
	      CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
	      break;
	    }
	    else if((Cnt+1)<subbandSize.length())
	      if(rbs(0)<sum&&rbs(rbs.length()-1)<sum+subbandSize(Cnt+1))
	      {
	      index=find(perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,Cnt);
	      index1=find(perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,Cnt+1);
		if(index==-1 && index1==-1)
		{
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
		}
		else
		{
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord;
		}
		break;
	      }
	  }
	  
	  
	}
	else if(aperiodicMode==_APERIODIC_MODE_2_2_)
	{
	  ivec subbandSize=getSubbandSizeAperiodicUESelected(DLRBs);
	  int index,index1;
	  int sum=0;//temp
	  for(int Cnt=0;Cnt<subbandSize.length();Cnt++)
	  {
	    sum+=subbandSize(Cnt);
	    if(rbs(0)<sum&&rbs(rbs.length()-1)<sum) //!RBG is entirely inside the subband
	    {     
	    index=find(perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,Cnt);
	      if(index!=-1)
	      {
	      CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord;
	      PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.PMI;
	      }
	      else
	      {
	      CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
	      PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI;
	      }
	      break;
	    }
	    else if((Cnt+1)<subbandSize.length())
	      if(rbs(0)<sum&&rbs(rbs.length()-1)<sum+subbandSize(Cnt+1))
	      {
	      index=find(perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,Cnt);
	      index1=find(perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,Cnt+1);
		if(index==-1 && index1==-1)
		{
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
		PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI;
		}
		else
		{
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord;
		PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.PMI;
		}
		break;
	      }
	  }
	  
	  
	}
	else{
	  cout<<"[both:]unsupported mode in getCQI "<<endl;abort();
	}
	
      }
      else
      {
      if(periodicMode==_PERIODIC_MODE_1_0_&&perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.isValid)
	{
	  PMI=-1;
	CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord;
	}
      else if(periodicMode==_PERIODIC_MODE_1_1_&&perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.isValid)
	{
	CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord;
	PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.pmiIndex;
	}
      else if((periodicMode==_PERIODIC_MODE_2_0_||periodicMode==_PERIODIC_MODE_2_1_)&&perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.isValid)
	{
	  PMI=-1;
	  int sum=0;bool isValidSubbandCQIavailable=false;
	  Array<ivec> SubbandSizePerBWPart;int numOfBWParts;
	  getSubbandsAndBWParts(SubbandSizePerBWPart,numOfBWParts,DLRBs);
	  
	  for(int BWPart=0;BWPart<numOfBWParts;BWPart++)
	  {
	    sum=0;
	    //! If rank changed in TM3 and TM4 in subband modes, Subband CQI cannot be used since wideband cqi and subband cqi will have different length 
	  if(perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.isValidPerBWPart(BWPart) && perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord.length()==perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(BWPart).length())
	    {
	      
	      for(int cnt=0;cnt<BWPart;cnt++)
	      {
		for(int Cnt=0;Cnt<SubbandSizePerBWPart(BWPart).length();Cnt++)
		{
		  sum+=SubbandSizePerBWPart(BWPart)(Cnt);
		}
	      for(int Cnt=0;Cnt<=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart(BWPart);Cnt++)
		{
		if(Cnt==perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart(BWPart))
		  {
		    if(rbs(0)>=sum&&rbs(rbs.length()-1)<sum+SubbandSizePerBWPart(BWPart)(Cnt))
		    {
		    CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(BWPart);
		      isValidSubbandCQIavailable=true;
		      break;
		    }
		    else if(rbs(0)>=sum&&rbs(0)<sum+SubbandSizePerBWPart(BWPart)(Cnt))
		    {
		    CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(BWPart);
		    CQIPerCodeword=(CQIPerCodeword+perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord)/2;
		      isValidSubbandCQIavailable=true;
		      break;
		    }
		    sum+=SubbandSizePerBWPart(BWPart)(Cnt);
		  }
		}
	      }
	    }
	  }
	if(perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.isValid){
	    if(periodicMode==_PERIODIC_MODE_2_1_)
	    PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.pmiIndex;
	    if(!isValidSubbandCQIavailable)
	    CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord;
	  }
	  
	}
      }
      /// default if CQI is not available
      if(CQIPerCodeword.length()==0)
      {
	CQIPerCodeword=ones_i(nCodewords);
	PMI=0;
      }
      else if(CQIPerCodeword.length()==1)
	CQIPerCodeword=CQIPerCodeword(0);
      else
	CQIPerCodeword=CQIPerCodeword(codewordIndicesToConsider);
      return(true);
    }
    else
      return(false);
  }
  */

bool getLatestDLCQI(PerUserDLCQIFeedback_S perUserCQIReport,bool isAPeriodicCQIConfigured, AperiodicReportingMode_E aperiodicMode, PeriodicReportingMode_E periodicMode , ivec codewordIndicesToConsider,int DLRBs,ivec rbs,ivec &CQIPerCodeword,int &PMI, int requestID){
  
  int nCodewords=codewordIndicesToConsider.length();
  int measurementIndx=0;
  if(requestID!=-1)
    measurementIndx=perUserCQIReport.getMeasurementIndx(requestID);
  
  ///  this function assumes localised mapping
  if(perUserCQIReport.isValid)
  {
    if(isAPeriodicCQIConfigured&&perUserCQIReport.APeriodicReportedSubframe>=0)
    {
      if(aperiodicMode==_APERIODIC_MODE_1_2_)
      {
	CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
	ivec subbandSize=getSubbandSizeAperiodic(DLRBs);
	int sum=0;//temp
	for(int Cnt=0;Cnt<subbandSize.length();Cnt++)
	{
	  sum+=subbandSize(Cnt);
	  if(rbs(0)<sum)
	  {
	    PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPMI(Cnt);
	    break;
	  }
	}
      }
      else if(aperiodicMode==_APERIODIC_MODE_3_0_||aperiodicMode==_APERIODIC_MODE_3_1_||aperiodicMode==_APERIODIC_MODE_3_2_)
      {
	PMI=-1;
	ivec subbandSize=getSubbandSizeAperiodic(DLRBs);
	if(rbs.length()==1)
	{
	  int sum=0;//temp
	  for(int Cnt=0;Cnt<subbandSize.length();Cnt++)
	  {
	    sum+=subbandSize(Cnt);
	    if(rbs(0)<sum)
	    {
	      CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(Cnt);
	      
	      if(aperiodicMode==_APERIODIC_MODE_3_1_)
		PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI;
	      
	      else if(aperiodicMode == _APERIODIC_MODE_3_2_)
	      {
		PMI = perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPMI(Cnt);
	      }
	      break;
	    }
	  }
	}
	else
	{
	  int sum=0;//temp 
	  for(int Cnt=0;Cnt<subbandSize.length();Cnt++)
	  {
	    sum+=subbandSize(Cnt);
	    if(rbs(0)<sum&&rbs(rbs.length()-1)<sum)
	    {
	      
	      CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(Cnt);
	      
	      #ifdef USING_CALIBRATION 
	      //to enable the wideband cqi in a particularband
	      CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
	      #endif
	      if(aperiodicMode==_APERIODIC_MODE_3_1_)
		PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI;
	      
	      else if(aperiodicMode == _APERIODIC_MODE_3_2_)
	      {
		PMI = perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPMI(Cnt);
	      }
	      
	      break;
	    }
	    else if((Cnt+1)<subbandSize.length())// Doubt here why this case is necessary
	      if(rbs(0)<sum&&rbs(rbs.length()-1)<sum+subbandSize(Cnt+1))
	      {
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(Cnt);
		CQIPerCodeword+=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(Cnt+1);
		CQIPerCodeword=CQIPerCodeword/2;
		#ifdef USING_CALIBRATION 
		//to enable the wideband cqi in a particularband
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
		#endif
		
		if(aperiodicMode==_APERIODIC_MODE_3_1_)
		  PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI;
		
		// Here As RBG is overlaping with two adjacent subbands assigning PMI to respective RBG is not clear as wideband PMI is not availabel
		// Assigning subband PMI of first subband to RBG
		else if(aperiodicMode == _APERIODIC_MODE_3_2_) 
		{
		  PMI = perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPMI(Cnt);
		}
		break;
	      }
	  }
	}
	
      }
      else if(aperiodicMode==_APERIODIC_MODE_2_0_)
      {
	ivec subbandSize=getSubbandSizeAperiodicUESelected(DLRBs);
	int index,index1;
	int sum=0;//temp
	for(int Cnt=0;Cnt<subbandSize.length();Cnt++)
	{
	  sum+=subbandSize(Cnt);
	  if(rbs(0)<sum&&rbs(rbs.length()-1)<sum) //!RBG is entirely inside the subband
	  {     
	    index=find(perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,Cnt);
	    if(index!=-1)
	    {
	      CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord;
	      #ifdef USING_CALIBRATION
	      if(traceValuePerTraceName["PUSCH_2_0wideband"]=="true")
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
	      #endif
	    }
	    else
	      CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
	    break;
	  }
	  else if((Cnt+1)<subbandSize.length())
	    if(rbs(0)<sum&&rbs(rbs.length()-1)<sum+subbandSize(Cnt+1))
	    {
	      index=find(perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,Cnt);
	      index1=find(perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,Cnt+1);
	      if(index==-1 && index1==-1)
	      {
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
	      }
	      else
	      {
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord;
	      }
	      break;
	    }
	}
	
	
      }
      else if(aperiodicMode==_APERIODIC_MODE_2_2_)
      {
	ivec subbandSize=getSubbandSizeAperiodicUESelected(DLRBs);
	int index,index1;
	int sum=0;//temp
	for(int Cnt=0;Cnt<subbandSize.length();Cnt++)
	{
	  sum+=subbandSize(Cnt);
	  if(rbs(0)<sum&&rbs(rbs.length()-1)<sum) //!RBG is entirely inside the subband
	  {     
	    index=find(perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,Cnt);
	    if(index!=-1)
	    {
	      CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord;
	      PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.PMI;
	    }
	    else
	    {
	      CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
	      PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI;
	    }
	    break;
	  }
	  else if((Cnt+1)<subbandSize.length())
	    if(rbs(0)<sum&&rbs(rbs.length()-1)<sum+subbandSize(Cnt+1))
	    {
	      index=find(perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,Cnt);
	      index1=find(perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,Cnt+1);
	      if(index==-1 && index1==-1)
	      {
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord;
		PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI;
	      }
	      else
	      {
		CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord;
		PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.PMI;
	      }
	      break;
	    }
	}
	
	
      }
      else{
	cout<<"[both:]unsupported mode in getCQI "<<endl;abort();
      }
      
    }
    else
    {
      if(periodicMode==_PERIODIC_MODE_1_0_&&perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.isValid)
      {
	PMI=-1;
	CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord;
      }
      else if(periodicMode==_PERIODIC_MODE_1_1_&&perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.isValid)
      {
	CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord;
	PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.pmiIndex;
      }
      else if((periodicMode==_PERIODIC_MODE_2_0_||periodicMode==_PERIODIC_MODE_2_1_)&&perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.isValid)
      {
	PMI=-1;
	int sum=0;bool isValidSubbandCQIavailable=false;
	Array<ivec> SubbandSizePerBWPart;int numOfBWParts;
	getSubbandsAndBWParts(SubbandSizePerBWPart,numOfBWParts,DLRBs);
	
	for(int BWPart=0;BWPart<numOfBWParts;BWPart++)
	{
	  sum=0;
	  //! If rank changed in TM3 and TM4 in subband modes, Subband CQI cannot be used since wideband cqi and subband cqi will have different length 
	  if(perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.isValidPerBWPart(BWPart) && perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord.length()==perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(BWPart).length())
	  {
	    
	    for(int cnt=0;cnt<BWPart;cnt++)
	    {
	      for(int Cnt=0;Cnt<SubbandSizePerBWPart(BWPart).length();Cnt++)
	      {
		sum+=SubbandSizePerBWPart(BWPart)(Cnt);
	      }
	      for(int Cnt=0;Cnt<=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart(BWPart);Cnt++)
	      {
		if(Cnt==perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart(BWPart))
		{
		  if(rbs(0)>=sum&&rbs(rbs.length()-1)<sum+SubbandSizePerBWPart(BWPart)(Cnt))
		  {
		    CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(BWPart);
		    isValidSubbandCQIavailable=true;
		    break;
		  }
		  else if(rbs(0)>=sum&&rbs(0)<sum+SubbandSizePerBWPart(BWPart)(Cnt))
		  {
		    CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(BWPart);
		    CQIPerCodeword=(CQIPerCodeword+perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord)/2;
		    isValidSubbandCQIavailable=true;
		    break;
		  }
		  sum+=SubbandSizePerBWPart(BWPart)(Cnt);
		}
	      }
	    }
	  }
	}
	if(perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.isValid){
	  if(periodicMode==_PERIODIC_MODE_2_1_)
	    PMI=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.pmiIndex;
	  if(!isValidSubbandCQIavailable)
	    CQIPerCodeword=perUserCQIReport.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord;
	}
	
      }
    }
    /// default if CQI is not available
    if(CQIPerCodeword.length()==0)
    {
      CQIPerCodeword=ones_i(nCodewords);
      PMI=0;
    }
    else if(CQIPerCodeword.length()==1)
      CQIPerCodeword=CQIPerCodeword(0);
    else
      CQIPerCodeword=CQIPerCodeword(codewordIndicesToConsider);
    return(true);
  }
  else
    return(false);
}


void getSpatialDifferentialCQIIndxForPeriodicReporting(int CQIFirstCodeword,int CQISecondCodeword, int &DifferentialCQIIndx){
  
  int Difference=CQIFirstCodeword-CQISecondCodeword;
  
  if(Difference==0)
    DifferentialCQIIndx=0;
  else if(Difference==1)
    DifferentialCQIIndx=1;
  else if(Difference==2)
    DifferentialCQIIndx=2;
  else if(Difference>=3)
    DifferentialCQIIndx=3;
  else if(Difference<=-4)
    DifferentialCQIIndx=4;
  else if(Difference==-3)
    DifferentialCQIIndx=5;
  else if(Difference==-2)
    DifferentialCQIIndx=6;
  else if(Difference==-1)
    DifferentialCQIIndx=7;
  else
    cout<<"[both:]error in getSpatialDifferentialCQIPeriodicReporting() "<<endl;
  
}
  
Array<cmat> extractFromRBArea(Array<cmat> &physicalFrame, FrameStructure& frameStructure, ResourceMapper *resourceMapper, int rbNumber, bool isDownlink){
  
  int symbolOffset=frameStructure.getRUOffset(rbNumber).symbolOffset;
  Size_S rbSize;
  if(isDownlink)
    rbSize=resourceMapper->dlResourceMapper.getRBSize();
  else
    rbSize=resourceMapper->ulResourceMapper.getRBSize();
 
  Array<cmat> dataFromRUArea(physicalFrame.length());
  for(int level1_cnt=0;level1_cnt<physicalFrame.length();level1_cnt++)
    dataFromRUArea(level1_cnt).set_size(rbSize.numberOfSubcarriers,rbSize.numberOfSymbols);
  
  Array<ivec> phySCLocationInEachSlot;
  
  if(isDownlink)
    phySCLocationInEachSlot=resourceMapper->dlResourceMapper.getPRBSubcarrierStartLocationinEachSlot(rbNumber% resourceMapper->dlResourceMapper.getResourceBlocksPerSubframe());
  else
    phySCLocationInEachSlot=resourceMapper->ulResourceMapper.getPRBSubcarrierStartLocationinEachSlot(rbNumber%resourceMapper->ulResourceMapper.getResourceBlocksPerSubframe());
  
  int slotIndx;
  for(int sym_cnt=0;sym_cnt<rbSize.numberOfSymbols;sym_cnt++)
  {
    slotIndx=!(sym_cnt<rbSize.numberOfSymbols/2);
    for(int sc_cnt=0;sc_cnt<rbSize.numberOfSubcarriers;sc_cnt++)
      for(int ant_cnt=0;ant_cnt<physicalFrame.size();ant_cnt++)
	dataFromRUArea(ant_cnt).set(sc_cnt,sym_cnt,physicalFrame(ant_cnt)(phySCLocationInEachSlot(slotIndx)(sc_cnt),symbolOffset+sym_cnt));
  }
  return(dataFromRUArea);
}
  
  Array< Array<cmat> > extractFromRBArea(Array< Array<cmat> > &physicalFrame, FrameStructure& frameStructure, ResourceMapper *resourceMapper, int rbNumber, bool isDownlink){
  
  int symbolOffset=frameStructure.getRUOffset(rbNumber).symbolOffset;
  Size_S rbSize;
  if(isDownlink)
    rbSize=resourceMapper->dlResourceMapper.getRBSize();
  else
    rbSize=resourceMapper->ulResourceMapper.getRBSize();
  
  Array< Array<cmat> > dataFromRUArea(physicalFrame.length());
  for(int level1_cnt=0;level1_cnt<physicalFrame.length();level1_cnt++)
  {
    dataFromRUArea(level1_cnt).set_size(physicalFrame(level1_cnt).length());
    for(int level2_cnt=0;level2_cnt<physicalFrame(level1_cnt).length();level2_cnt++)
      dataFromRUArea(level1_cnt)(level2_cnt).set_size(rbSize.numberOfSubcarriers,rbSize.numberOfSymbols);
  }
  Array<ivec> phySCLocationInEachSlot;
  if(isDownlink)
    phySCLocationInEachSlot=resourceMapper->dlResourceMapper.getPRBSubcarrierStartLocationinEachSlot(rbNumber % resourceMapper->dlResourceMapper.getResourceBlocksPerSubframe());
  else
    phySCLocationInEachSlot=resourceMapper->ulResourceMapper.getPRBSubcarrierStartLocationinEachSlot(rbNumber % resourceMapper->ulResourceMapper.getResourceBlocksPerSubframe());
  
  int slotIndx;
  
  for(int sym_cnt=0;sym_cnt<rbSize.numberOfSymbols;sym_cnt++)
  {
    slotIndx=!(sym_cnt<rbSize.numberOfSymbols/2);
    for(int sc_cnt=0;sc_cnt<rbSize.numberOfSubcarriers;sc_cnt++)
      for(int level1_cnt=0;level1_cnt<physicalFrame.length();level1_cnt++)
	for(int level2_cnt=0;level2_cnt<physicalFrame(level1_cnt).length();level2_cnt++)
	  dataFromRUArea(level1_cnt)(level2_cnt).set(sc_cnt,sym_cnt,physicalFrame(level1_cnt)(level2_cnt)(phySCLocationInEachSlot(slotIndx)(sc_cnt),symbolOffset+sym_cnt));
  }
  return(dataFromRUArea);
}
  
Array< Array< Array<cmat> > > extractFromRBArea(Array< Array< Array< cmat > > >& physicalFrame, FrameStructure& frameStructure, ResourceMapper* resourceMapper, int rbNumber, bool isDownlink, ivec indices){
  
  
  Size_S rbSize;
  if(isDownlink)
    rbSize=resourceMapper->dlResourceMapper.getRBSize();
  else
    rbSize=resourceMapper->ulResourceMapper.getRBSize();
  int symbolOffset=frameStructure.getRUOffset(rbNumber).symbolOffset;
  if(indices.length()==0)
    indices=getIntegers(0,physicalFrame.length()-1);
  Array< Array< Array<cmat> > > dataFromRUArea(indices.length());
  for(int level1_cnt=0;level1_cnt<indices.length();level1_cnt++)
  {
    dataFromRUArea(level1_cnt).set_size(physicalFrame(indices(level1_cnt)).length());
    for(int level2_cnt=0;level2_cnt<physicalFrame(indices(level1_cnt)).length();level2_cnt++)
    {
      dataFromRUArea(level1_cnt)(level2_cnt).set_size(physicalFrame(indices(level1_cnt))(level2_cnt).length());
      for(int level3_cnt=0;level3_cnt<physicalFrame(indices(level1_cnt))(level2_cnt).length();level3_cnt++)
	dataFromRUArea(level1_cnt)(level2_cnt)(level3_cnt).set_size(rbSize.numberOfSubcarriers,rbSize.numberOfSymbols);
    }
  }
  
  Array<ivec> phySCLocationInEachSlot;
  if(isDownlink)
    phySCLocationInEachSlot=resourceMapper->dlResourceMapper.getPRBSubcarrierStartLocationinEachSlot(rbNumber % resourceMapper->dlResourceMapper.getResourceBlocksPerSubframe());
  else
    phySCLocationInEachSlot=resourceMapper->ulResourceMapper.getPRBSubcarrierStartLocationinEachSlot(rbNumber % resourceMapper->ulResourceMapper.getResourceBlocksPerSubframe());
  int slotIndx;
  for(int sym_cnt=0;sym_cnt<rbSize.numberOfSymbols;sym_cnt++)
  {
    slotIndx=!(sym_cnt<rbSize.numberOfSymbols/2);
    for(int sc_cnt=0;sc_cnt<rbSize.numberOfSubcarriers;sc_cnt++)
      for(int level1_cnt=0;level1_cnt<indices.length();level1_cnt++)
	for(int level2_cnt=0;level2_cnt<physicalFrame(indices(level1_cnt)).length();level2_cnt++)
	  for(int level3_cnt=0;level3_cnt<physicalFrame(indices(level1_cnt))(level2_cnt).length();level3_cnt++)
	    dataFromRUArea(level1_cnt)(level2_cnt)(level3_cnt).set(sc_cnt,sym_cnt,physicalFrame(indices(level1_cnt))(level2_cnt)(level3_cnt)(phySCLocationInEachSlot(slotIndx)(sc_cnt),symbolOffset+sym_cnt));
  }
  return(dataFromRUArea);
}
  
void applyIDFT(EqualizerOutput_S &equalizerOutput, int dftSize){
  
  
  int nSubcarriers=dftSize;
  int nStreams=equalizerOutput.equalizedSymbols.length();
  int nSymbols=equalizerOutput.equalizedSymbols(0).length()/dftSize;
  cmat equSymbolsTemp;
  Array<cmat> equChannel(nStreams);
  Array<mat> equNoiseVariance(nStreams);
  Array<cmat> equSymbols(nStreams);
  
  for(int d1=0;d1<nStreams;d1++)
  {
    equSymbolsTemp=reshape(equalizerOutput.equalizedSymbols(d1),dftSize,nSymbols);
    equSymbols(d1)=ifftNorm(equSymbolsTemp);
    equChannel(d1)=reshape(equalizerOutput.equalizedChannel(d1),dftSize,nSymbols);
    equNoiseVariance(d1)=reshape(equalizerOutput.equalizedNoiseVariance(d1),dftSize,nSymbols);
  
  }
  
  for(int sym_cnt=0;sym_cnt<nSymbols;sym_cnt++)
  {
    complex< double > alphaPerSymbol;
    double noiseVariance;
    double ppSINR;
    // Computing alpha => signal co-eff matrix
    for(int d1=0;d1<nStreams;d1++)
    {
      alphaPerSymbol=sum(equChannel(d1).get_col(sym_cnt))/(double)nSubcarriers; 
      noiseVariance=sum(equNoiseVariance(d1).get_col(sym_cnt))/(double)nSubcarriers; 
      ppSINR=sqr(abs(alphaPerSymbol))/noiseVariance;
      alphaPerSymbol/=sqrt(noiseVariance); //Dividing by noiseVariance to avoid passing it to the decoder
      
      cvec temp = equSymbols(d1).get_col(sym_cnt)/sqrt(noiseVariance);
      equalizerOutput.equalizedChannel(d1).set_subvector(sym_cnt*nSubcarriers,repeatAsVector(alphaPerSymbol,nSubcarriers));
      equalizerOutput.equalizedSymbols(d1).set_subvector(sym_cnt*nSubcarriers,temp);
      equalizerOutput.postProcessingSinr(d1).set_subvector(sym_cnt*nSubcarriers,repeatAsVector(ppSINR,nSubcarriers));
    }  
  }
}

CQIComputationInfo_S createBasicCQIComputationInfo(CQIComputationConfig_S &cqiComputationConfig){

  Array<ivec> serverNodeGroups;
  Array<ivec> cqiPorts;
  ivec requestIDPerNodeGroup;

  CQIComputationInfo_S output;
  output.cqiMeasurementConfig=cqiComputationConfig.cqiMeasurementConfig;
  output.enableCQI0=cqiComputationConfig.enableCQI0;

  return(output);
}
    
RSInfo_S getRSInfoForUnScheduled(RSInfo_S defaultRSInfo,FrameStructure &frameStructure){
  
  RSInfo_S rsInfo;
  rsInfo=defaultRSInfo;
  rsInfo.DeModrsType=_RSTYPE_UNINIT_;
  
  ivec indicesToRemove=find(rsInfo.rsPorts,rsInfo.DeModPorts);
  
  rsInfo.DeModPorts.set_length(0);
  rsInfo.fillPRS=false;
  
  ivec originalPorts=rsInfo.rsPorts;
  
  deleteInVec(rsInfo.rsPorts,indicesToRemove);
  if(rsInfo.crsPorts.length())
    rsInfo.rsPorts=findUniqueNumbers(concat(rsInfo.rsPorts,rsInfo.crsPorts));
  
  rsInfo.portReferenceNumbers=getPortReferenceNumbers(frameStructure,rsInfo.rsPorts);
  
  ivec indicesToRetain=find(originalPorts,rsInfo.rsPorts);
  rsInfo.rsScPosition=rsInfo.rsScPosition(indicesToRetain);
  rsInfo.rsSymPosition=getFromArray(rsInfo.rsSymPosition,indicesToRetain);
    
  return(rsInfo);
}

int getModulationOrderAsLinearIndex(int mcs){
  
  if(mcs<10)
    return 0;
  else if(mcs>9 && mcs<17)
    return 1;
  else if(mcs>16 && mcs<29)
    return 2;
  else
  {
    cout<<"[both:]Error ...Unsupported MCS.. aborting"<<endl;abort();
  }
}
    
std::ostream& operator<<(std::ostream &o,UlDlMode_E n) {
  switch(n) {
    case _ULDL_MODE_UNINIT_: return o<<"_ULDL_MODE_UNINIT_";
    case _ULDL_MODE_DOWNLINK_: return o<<"_ULDL_MODE_DOWNLINK_";
    case _ULDL_MODE_UPLINK_: return o<<"_ULDL_MODE_UPLINK_";
    case _ULDL_MODE_TDD_: return o<<"_ULDL_MODE_TDD_";
    case _ULDL_MODE_FDD_: return o<<"_ULDL_MODE_FDD_";
    default: return o<<"(invalid value)";
  }
}
    
void PrintRI(int nodeID, int RI, double avgPPSINRindB ,vec SpecEffPerRI, int subframenumber, string fileName){
  
//   #ifdef USING_MPI
//   string fileName="./Results/RIlogs.txt.temp"+toString(currentTaskid);
//   #else
//   string fileName="./Results/RIlogs.txt";
//   #endif
    #ifdef USING_MPI
    fileName = fileName+".temp"+toString(currentTaskid);
    #endif
  ofstream RIlogs;
   static bool isNewPrint=1;
  if(isNewPrint)
  {
    RIlogs.open(fileName.c_str(),ios::out);

    if(currentTaskid==0)
      RIlogs<<"% SubframeNumber,\tUE_ID,\t\t  RI,\t\tavgPPSINRindB,\t\tExp_Spec_Eff_over_possibleRanks\n";
    isNewPrint=false;
  }
  else
    RIlogs.open(fileName.c_str(),ios::app);
  
  if(SpecEffPerRI.length()==2)
    RIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(4)<<RI<<",\t\t"<<setw(9)<<avgPPSINRindB<<",\t\t"<<setw(4)<<SpecEffPerRI(0)<<",\t"<<setw(4)<<SpecEffPerRI(1)<<endl;
  else if(SpecEffPerRI.length()==3)
    RIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(4)<<RI<<",\t\t"<<setw(9)<<avgPPSINRindB<<",\t\t"<<setw(4)<<SpecEffPerRI(0)<<",\t"<<setw(4)<<SpecEffPerRI(1)<<",\t"<<setw(4)<<SpecEffPerRI(2)<<endl;
  else if(SpecEffPerRI.length()==4)
    RIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(4)<<RI<<",\t\t"<<setw(9)<<avgPPSINRindB<<",\t\t"<<setw(4)<<SpecEffPerRI(0)<<",\t"<<setw(4)<<SpecEffPerRI(1)<<",\t"<<setw(4)<<SpecEffPerRI(2)<<",\t"<<setw(4)<<SpecEffPerRI(3)<<endl;
  else if(SpecEffPerRI.length()==8)
    RIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(4)<<RI<<",\t\t"<<setw(9)<<avgPPSINRindB<<",\t\t"<<setw(4)<<SpecEffPerRI(0)<<",\t"<<setw(4)<<SpecEffPerRI(1)<<",\t"<<setw(4)<<SpecEffPerRI(2)<<",\t"<<setw(4)<<SpecEffPerRI(3)<<",\t"<<setw(4)<<SpecEffPerRI(4)<<",\t"<<setw(4)<<SpecEffPerRI(5)<<",\t"<<setw(4)<<SpecEffPerRI(6)<<",\t"<<setw(4)<<SpecEffPerRI(7)<<endl;
  else
    RIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(4)<<RI<<",\t\t"<<setw(9)<<avgPPSINRindB<<endl;
}

void printRI(int nodeID,int eNBID, int RI,double prepSINRindB,double avgPPSINRindB ,vec specEffPerRI, int subframenumber,vec conditionNumber ,string fileName,string parameters){
  
   ofstream RIlogs;
   string dir = fileName;
    fileName = dir +"/eNB_ID_"+ toString(eNBID)+".txt";
    if(!IsFileExists(fileName))
    {
        static bool isSchDirExist=false;
        if(!isSchDirExist)
        {
            createDirectory(dir);isSchDirExist=true;
        }
        RIlogs.open(fileName.c_str(),ios::out);
        
        //             RIlogs<<Parameters<<endl;
        RIlogs<<"% SubframeNumber,\tUE_ID,\t  RI,\tprepSINRindB,\tavgPPSINRindB,\tExp_Spec_Eff_over_possibleRanks";
        for(int i=0;i<specEffPerRI.length();i++)
            RIlogs<<"\t";
        RIlogs<<"\tConditionNumberOverRanks"<<endl;
    }
    else
        RIlogs.open(fileName.c_str(),ios::app);
  
  RIlogs<<subframenumber<<",\t"<<nodeID<<",\t"<<RI<<",\t"<<prepSINRindB<<",\t"<<avgPPSINRindB<<",\t";
  for(uint32_t i=0;i<specEffPerRI.length();i++)
      RIlogs<<specEffPerRI(i)<<"\t";
  RIlogs<<"\t";
  for(uint32_t i=0;i<specEffPerRI.length();i++)
  {   
      RIlogs<<conditionNumber(i)<<"\t";
  }
  if(conditionNumber.length()>specEffPerRI.length())
  {
      RIlogs<<"\t"<<conditionNumber(specEffPerRI.length());
  }
  RIlogs<<endl;
}


void PrintCQIindices(int nodeID, ivec cqiIndexPerCodeWord,ivec modOrders, vec effSINRs, int subframenumber,string WideBandCQIlogsFileName, int PMI){
    
    #ifdef USING_MPI
    WideBandCQIlogsFileName = WideBandCQIlogsFileName+".temp"+toString(currentTaskid);
    #endif
    
  std::ofstream CQIlogs;
  static bool isNewPrint=1;
  if(isNewPrint)
  {
    CQIlogs.open(WideBandCQIlogsFileName.c_str(),ios::out);

    if(currentTaskid==0)
      CQIlogs<<"%SubframeNumber,\tUE_ID,\t\tCQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tPMI  \t\t (NOTE: -1 => CQI/PMI is not available)\n";
    isNewPrint=0;
  }
  else
    CQIlogs.open(WideBandCQIlogsFileName.c_str(),ios::app);
  
  if(cqiIndexPerCodeWord.length()==1)
    CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<",-1,"<<"\t\t"<<setw(6)<<modOrders(0)<<",-1," <<"\t\t"<<setw(8)<<effSINRs(0)<<",-100," <<"\t\t"<<setw(3)<<PMI<<endl;
  else if(cqiIndexPerCodeWord.length()==2)
    CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<","<<cqiIndexPerCodeWord(1)<<",\t\t"<<setw(6)<<modOrders(0)<<","<<modOrders(1) <<",\t\t"<<setw(8)<<effSINRs(0)<<","<<effSINRs(1)<<",\t\t"<<setw(3)<<PMI<<endl;
}

void PrintCQIindices(int nodeID, Array<ivec> cqiIndexPerCodeWord, int subframenumber,string SubbandCQIlogsFileName, int BWPartNumber){
  
    #ifdef USING_MPI
    SubbandCQIlogsFileName = SubbandCQIlogsFileName+".temp"+toString(currentTaskid);
    #endif
    
  std::ofstream CQIlogs;
  static bool isNewPrint=1;
  if(isNewPrint)
  {
    CQIlogs.open(SubbandCQIlogsFileName.c_str(),ios::out);

    if(currentTaskid==0)
      CQIlogs<<"%SubframeNumber,\tUE_ID,\t\tBWPartNumber,\t\tCQI_Indices \t\t (NOTE: -1 => CQI is not available)";
    isNewPrint=0;
  }
  else
    CQIlogs.open(SubbandCQIlogsFileName.c_str(),ios::app);
  
  CQIlogs<<endl<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t\t"<<setw(4)<<BWPartNumber<<",\t"<<setw(10);
  for(int indx=0;indx<cqiIndexPerCodeWord.length();indx++)
    if(cqiIndexPerCodeWord(indx).length()==1)
      CQIlogs<<cqiIndexPerCodeWord(indx)(0)<<",-1,\t";
    else
      CQIlogs<<cqiIndexPerCodeWord(indx)(0)<<","<<cqiIndexPerCodeWord(indx)(1)<<",\t"<<endl;
    
}
/*    
 void PrintCQIindices(AperiodicReportingMode_E AperiodicMode,int nodeID, ivec cqiIndexPerCodeWord,ivec modOrders, vec effSINRs, int subframenumber,string AperiodicCQIlogsFileName,int WB_PMI,Array<ivec> SBcqiIndexPerCodeword, ivec SB_PMI)
    {
      
      std::ofstream CQIlogs;
      static bool isNewPrint=1;
      if(isNewPrint)
      {
	CQIlogs.open(AperiodicCQIlogsFileName.c_str(),ios::out);
	
	if(currentTaskid==0)
	{
	  if(AperiodicMode==_APERIODIC_MODE_1_2_)
	  CQIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tSB_PMIs  \t\t (NOTE: -1 => CQI/PMI is not available)\n";
	  else if(AperiodicMode==_APERIODIC_MODE_3_0_ || AperiodicMode==_APERIODIC_MODE_3_1_)
	    CQIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tWB_PMI,\t\tSB_CQI_Indices  \t\t (NOTE: -1 => CQI/PMI is not available)\n";
	  else if(AperiodicMode==_APERIODIC_MODE_2_0_ || AperiodicMode==_APERIODIC_MODE_2_2_)
	    CQIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tSB_CQI_Indices,\t\tWB_PMI,\t\tSB_PMI \t\t (NOTE: -1 => CQI/PMI is not available)\n";
	  else
	  {
	    cout<<"[detl:] Invalid aperiodic mode in PrintCQIindices"<<endl;
	    abort();
	  }
	}
	isNewPrint=0;
      }
      else
	CQIlogs.open(AperiodicCQIlogsFileName.c_str(),ios::app);
      
      if(AperiodicMode==_APERIODIC_MODE_1_2_)
      {
      if(cqiIndexPerCodeWord.length()==1)
	CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<",-1,"<<"\t\t"<<setw(6)<<modOrders(0)<<",-1," <<"\t\t"<<setw(8)<<effSINRs(0)<<",-100," <<"\t\t";
      else if(cqiIndexPerCodeWord.length()==2)
	CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<","<<cqiIndexPerCodeWord(1)<<",\t\t"<<setw(6)<<modOrders(0)<<","<<modOrders(1) <<",\t\t"<<setw(8)<<effSINRs(0)<<","<<effSINRs(1)<<",\t\t";
      for(int count=0;count<SB_PMI.length();count++)
      {
	CQIlogs<<setw(3)<<SB_PMI(count)<<"\t\t";
      }
      CQIlogs<<endl;
      }
      else if(AperiodicMode==_APERIODIC_MODE_3_0_ || AperiodicMode==_APERIODIC_MODE_3_1_)
      {
	if(cqiIndexPerCodeWord.length()==1)
	  CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<",-1,"<<"\t\t"<<setw(6)<<modOrders(0)<<",-1," <<"\t\t"<<setw(8)<<effSINRs(0)<<",-100," <<"\t\t"<<setw(3)<<WB_PMI<<"\t\t";
	else if(cqiIndexPerCodeWord.length()==2)
	  CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<","<<cqiIndexPerCodeWord(1)<<",\t\t"<<setw(6)<<modOrders(0)<<","<<modOrders(1) <<",\t\t"<<setw(8)<<effSINRs(0)<<","<<effSINRs(1)<<",\t\t"<<setw(3)<<WB_PMI<<"\t\t";
	for(int count=0;count<SBcqiIndexPerCodeword.length();count++)
	{
	  if(SBcqiIndexPerCodeword(count).length()==1)
	    CQIlogs<<setw(8)<<SBcqiIndexPerCodeword(count)(0)<<",-1,"<<"\t\t\t";
	  else
	    CQIlogs<<setw(8)<<SBcqiIndexPerCodeword(count)(0)<<","<<SBcqiIndexPerCodeword(count)(1)<<"\t\t\t";
	}
	CQIlogs<<endl;
	
      }
      else if(AperiodicMode==_APERIODIC_MODE_2_0_ || AperiodicMode==_APERIODIC_MODE_2_2_)
      {
	if(cqiIndexPerCodeWord.length()==1)
	  CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<",-1,"<<"\t\t"<<setw(6)<<modOrders(0)<<",-1," <<"\t\t"<<setw(8)<<effSINRs(0)<<",-100," <<"\t\t"<<setw(8)<<SBcqiIndexPerCodeword(0)(0)<<",-1,"<<"\t\t"<<setw(3)<<WB_PMI<<endl;
	else if(cqiIndexPerCodeWord.length()==2)
	  CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<","<<cqiIndexPerCodeWord(1)<<",\t\t"<<setw(6)<<modOrders(0)<<","<<modOrders(1) <<",\t\t"<<setw(8)<<effSINRs(0)<<","<<effSINRs(1)<<",\t\t"<<setw(8)<<SBcqiIndexPerCodeword(0)(0)<<","<<SBcqiIndexPerCodeword(0)(1)<<"\t\t"<<setw(3)<<WB_PMI<<"\t\t"<<setw(3)<<SB_PMI(0)<<endl;
      }
      else
      {
	cout<<"[detl:] Invalid aperiodic mode in PrintCQIindices"<<endl;
	abort();
      }
      
      
    }
    
  */  

void printCQIindices(AperiodicReportingMode_E aperiodicMode,int nodeID,int eNBID, int nLayers,ivec cqiIndexPerCodeWord,ivec modOrders, vec effSINRs, int subframenumber,string aperiodicCQIlogsFileName,int WB_PMI,Array<ivec> SBcqiIndexPerCodeword, ivec SB_PMI,double computationTime,ivec pmiIndices)
{
    ofstream CQIlogs;
    string dir = aperiodicCQIlogsFileName;
    aperiodicCQIlogsFileName = dir +"/eNB_ID_"+ toString(eNBID)+".txt";
    if(!IsFileExists(aperiodicCQIlogsFileName))
    {
        static bool isDirExist=false;
        if(!isDirExist)
        {
            createDirectory(dir);isDirExist=true;
        }
        CQIlogs.open(aperiodicCQIlogsFileName.c_str(),ios::out);
        
        switch(aperiodicMode)
        {
            case _APERIODIC_MODE_1_2_:
                CQIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tSB_PMIs";
                break;
            case _APERIODIC_MODE_3_0_:
            case _APERIODIC_MODE_3_1_:
                CQIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tWB_PMI,\t\tSB_CQI_Indices";
                break;
            case _APERIODIC_MODE_3_2_:
                CQIlogs<<"%SubframeNumber,\tUE_ID,\t\tnLayers,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tWB_PMI,\t\tWB_PMI_Indices,\t\t[{SB_CQI_Indices}, SB_PMIs]";  
                break;
            case _APERIODIC_MODE_2_0_:
            case _APERIODIC_MODE_2_2_:
                CQIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tSB_CQI_Indices,\t\tWB_PMI,\t\tSB_PMI";
                break;
            default:
                cout<<"[detl:] Invalid aperiodic mode in PrintCQIindices"<<endl;abort();
                break;
        }
        
        for(int i=0;i<SB_PMI.length();i++)
            CQIlogs<<"\t";
        CQIlogs<<"\tCSIComputationTime\t\t (NOTE: -1 => CQI/PMI is not available)"<<endl;
    }
    else
        CQIlogs.open(aperiodicCQIlogsFileName.c_str(),ios::app);
    
    switch(aperiodicMode)
    {
        case _APERIODIC_MODE_1_2_:
            if(cqiIndexPerCodeWord.length()==1)
                CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<",-1,"<<"\t\t"<<setw(6)<<modOrders(0)<<",-1," <<"\t\t"<<setw(8)<<effSINRs(0)<<",-100," <<"\t\t";
            else if(cqiIndexPerCodeWord.length()==2)
                CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<","<<cqiIndexPerCodeWord(1)<<",\t\t"<<setw(6)<<modOrders(0)<<","<<modOrders(1) <<",\t\t"<<setw(8)<<effSINRs(0)<<","<<effSINRs(1)<<",\t\t";
            for(int count=0;count<SB_PMI.length();count++)
            {
                CQIlogs<<setw(3)<<SB_PMI(count)<<"\t\t";
            }
            break;
            
        case _APERIODIC_MODE_3_0_:
        case _APERIODIC_MODE_3_1_:
            if(cqiIndexPerCodeWord.length()==1)
                CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<",-1,"<<"\t\t"<<setw(6)<<modOrders(0)<<",-1," <<"\t\t"<<setw(8)<<effSINRs(0)<<",-100," <<"\t\t"<<setw(3)<<WB_PMI<<"\t\t";
            else if(cqiIndexPerCodeWord.length()==2)
                CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<","<<cqiIndexPerCodeWord(1)<<",\t\t"<<setw(6)<<modOrders(0)<<","<<modOrders(1) <<",\t\t"<<setw(8)<<effSINRs(0)<<","<<effSINRs(1)<<",\t\t"<<setw(3)<<WB_PMI<<"\t\t";
            for(int count=0;count<SBcqiIndexPerCodeword.length();count++)
            {
                if(SBcqiIndexPerCodeword(count).length()==1)
                    CQIlogs<<setw(8)<<SBcqiIndexPerCodeword(count)(0)<<",-1,"<<"\t\t\t";
                else if(SBcqiIndexPerCodeword(count).length()==2)
                    CQIlogs<<setw(8)<<SBcqiIndexPerCodeword(count)(0)<<","<<SBcqiIndexPerCodeword(count)(1)<<"\t\t\t";
            }
            break;
            
        case _APERIODIC_MODE_3_2_:
            if(cqiIndexPerCodeWord.length()==1)
                CQIlogs<<setw(6)<<subframenumber<<",\t"<<setw(5)<<nodeID<<",\t\t"<<nLayers<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<",-1,"<<"\t\t"<<setw(6)<<modOrders(0)<<",-1," <<"\t\t"<<setw(8)<<effSINRs(0)<<",-100," <<"\t\t";
            else if(cqiIndexPerCodeWord.length()==2)
                CQIlogs<<setw(6)<<subframenumber<<",\t"<<setw(5)<<nodeID<<",\t\t"<<nLayers<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<","<<cqiIndexPerCodeWord(1)<<",\t\t"<<setw(6)<<modOrders(0)<<","<<modOrders(1) <<",\t\t"<<setw(8)<<effSINRs(0)<<","<<effSINRs(1)<<",\t\t";
        
            CQIlogs<<setw(6)<<WB_PMI<<",\t\t"<<pmiIndices<<",\t\t";
        
            for(int count=0;count<SBcqiIndexPerCodeword.length();count++)
            {
                if(SBcqiIndexPerCodeword(count).length()==1)
                    CQIlogs<<setw(8)<<"[{"<<SBcqiIndexPerCodeword(count)(0)<<",-1},"<< SB_PMI(count)<<"],\t";
                else if(SBcqiIndexPerCodeword(count).length()==2)
                    CQIlogs<<setw(8)<<"[{"<<SBcqiIndexPerCodeword(count)(0)<<","<<SBcqiIndexPerCodeword(count)(1)<<"},"<< SB_PMI(count)<<"],\t";
            }
            break;
            
        case _APERIODIC_MODE_2_0_:
        case _APERIODIC_MODE_2_2_:
            if(cqiIndexPerCodeWord.length()==1)
                CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<",-1,"<<"\t\t"<<setw(6)<<modOrders(0)<<",-1," <<"\t\t"<<setw(8)<<effSINRs(0)<<",-100," <<"\t\t"<<setw(8)<<SBcqiIndexPerCodeword(0)(0)<<",-1,"<<"\t\t"<<setw(3)<<WB_PMI<<endl;
            else if(cqiIndexPerCodeWord.length()==2)
                CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<","<<cqiIndexPerCodeWord(1)<<",\t\t"<<setw(6)<<modOrders(0)<<","<<modOrders(1) <<",\t\t"<<setw(8)<<effSINRs(0)<<","<<effSINRs(1)<<",\t\t"<<setw(8)<<SBcqiIndexPerCodeword(0)(0)<<","<<SBcqiIndexPerCodeword(0)(1)<<"\t\t"<<setw(3)<<WB_PMI<<"\t\t"<<setw(3)<<SB_PMI(0)<<endl;
            break;
            
        default:
            cout<<"[detl:] Invalid aperiodic mode in PrintCQIindices"<<endl;abort();
            break;
    }
    
    CQIlogs<<endl;
//     CQIlogs<<"\t"<<computationTime<<endl;
}

void PrintCQIindices(AperiodicReportingMode_E AperiodicMode,int nodeID, ivec cqiIndexPerCodeWord,ivec modOrders, vec effSINRs, int subframenumber,string AperiodicCQIlogsFileName,int WB_PMI,Array<ivec> SBcqiIndexPerCodeword, ivec SB_PMI)
{
    #ifdef USING_MPI
    AperiodicCQIlogsFileName = AperiodicCQIlogsFileName+".temp"+toString(currentTaskid);
    #endif
  std::ofstream CQIlogs;
  static bool isNewPrint=1;
  if(isNewPrint)
  {
    CQIlogs.open(AperiodicCQIlogsFileName.c_str(),ios::out);
    
    if(currentTaskid==0)
    {
      if(AperiodicMode==_APERIODIC_MODE_1_2_)
        CQIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tSB_PMIs  \t\t (NOTE: -1 => CQI/PMI is not available)\n";
      else if(AperiodicMode==_APERIODIC_MODE_3_0_ || AperiodicMode==_APERIODIC_MODE_3_1_)
        CQIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tWB_PMI,\t\tSB_CQI_Indices  \t\t (NOTE: -1 => CQI/PMI is not available)\n";
      else if (AperiodicMode ==_APERIODIC_MODE_3_2_)
        CQIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\t[{SB_CQI_Indices}, SB_PMIs]\t\t (NOTE: -1 => CQI/PMI is not available)\n";  
      else if(AperiodicMode==_APERIODIC_MODE_2_0_ || AperiodicMode==_APERIODIC_MODE_2_2_)
        CQIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tSB_CQI_Indices,\t\tWB_PMI,\t\tSB_PMI \t\t (NOTE: -1 => CQI/PMI is not available)\n";
      else
      {
        cout<<"[detl:] Invalid aperiodic mode in PrintCQIindices"<<endl;
        abort();
      }
    }
    isNewPrint=0;
  }
  else
    CQIlogs.open(AperiodicCQIlogsFileName.c_str(),ios::app);
  
  if(AperiodicMode==_APERIODIC_MODE_1_2_)
  {
    if(cqiIndexPerCodeWord.length()==1)
      CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<",-1,"<<"\t\t"<<setw(6)<<modOrders(0)<<",-1," <<"\t\t"<<setw(8)<<effSINRs(0)<<",-100," <<"\t\t";
    else if(cqiIndexPerCodeWord.length()==2)
      CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<","<<cqiIndexPerCodeWord(1)<<",\t\t"<<setw(6)<<modOrders(0)<<","<<modOrders(1) <<",\t\t"<<setw(8)<<effSINRs(0)<<","<<effSINRs(1)<<",\t\t";
    for(int count=0;count<SB_PMI.length();count++)
    {
      CQIlogs<<setw(3)<<SB_PMI(count)<<"\t\t";
    }
    CQIlogs<<endl;
  }
  else if(AperiodicMode==_APERIODIC_MODE_3_0_ || AperiodicMode==_APERIODIC_MODE_3_1_)
  {
    if(cqiIndexPerCodeWord.length()==1)
      CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<",-1,"<<"\t\t"<<setw(6)<<modOrders(0)<<",-1," <<"\t\t"<<setw(8)<<effSINRs(0)<<",-100," <<"\t\t"<<setw(3)<<WB_PMI<<"\t\t";
    else if(cqiIndexPerCodeWord.length()==2)
      CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<","<<cqiIndexPerCodeWord(1)<<",\t\t"<<setw(6)<<modOrders(0)<<","<<modOrders(1) <<",\t\t"<<setw(8)<<effSINRs(0)<<","<<effSINRs(1)<<",\t\t"<<setw(3)<<WB_PMI<<"\t\t";
    for(int count=0;count<SBcqiIndexPerCodeword.length();count++)
    {
      if(SBcqiIndexPerCodeword(count).length()==1)
	CQIlogs<<setw(8)<<SBcqiIndexPerCodeword(count)(0)<<",-1,"<<"\t\t\t";
      else if(SBcqiIndexPerCodeword(count).length()==2)
	CQIlogs<<setw(8)<<SBcqiIndexPerCodeword(count)(0)<<","<<SBcqiIndexPerCodeword(count)(1)<<"\t\t\t";
    }
    CQIlogs<<endl;
    
  }
  else if(AperiodicMode ==_APERIODIC_MODE_3_2_)
  {
    if(cqiIndexPerCodeWord.length()==1)
      CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<",-1,"<<"\t\t"<<setw(6)<<modOrders(0)<<",-1," <<"\t\t"<<setw(8)<<effSINRs(0)<<",-100," <<"\t\t";
    else if(cqiIndexPerCodeWord.length()==2)
      CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<","<<cqiIndexPerCodeWord(1)<<",\t\t"<<setw(6)<<modOrders(0)<<","<<modOrders(1) <<",\t\t"<<setw(8)<<effSINRs(0)<<","<<effSINRs(1)<<",\t\t";
    
    CQIlogs<<setw(6)<<WB_PMI<<",\t\t\t";
    
    for(int count=0;count<SBcqiIndexPerCodeword.length();count++)
    {
      if(SBcqiIndexPerCodeword(count).length()==1)
	CQIlogs<<setw(8)<<SBcqiIndexPerCodeword(count)(0)<<",-1,"<<"\t\t\t "<< SB_PMI(count)<<"\t\t\t";
      else if(SBcqiIndexPerCodeword(count).length()==2)
	CQIlogs<<setw(8)<<SBcqiIndexPerCodeword(count)(0)<<","<<SBcqiIndexPerCodeword(count)(1)<<"\t\t\t "<< SB_PMI(count)<<"\t\t\t";
    }
    CQIlogs<<endl;
  }
  else if(AperiodicMode==_APERIODIC_MODE_2_0_ || AperiodicMode==_APERIODIC_MODE_2_2_)
  {
    if(cqiIndexPerCodeWord.length()==1)
      CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<",-1,"<<"\t\t"<<setw(6)<<modOrders(0)<<",-1," <<"\t\t"<<setw(8)<<effSINRs(0)<<",-100," <<"\t\t"<<setw(8)<<SBcqiIndexPerCodeword(0)(0)<<",-1,"<<"\t\t"<<setw(3)<<WB_PMI<<endl;
    else if(cqiIndexPerCodeWord.length()==2)
      CQIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8)<<cqiIndexPerCodeWord(0)<<","<<cqiIndexPerCodeWord(1)<<",\t\t"<<setw(6)<<modOrders(0)<<","<<modOrders(1) <<",\t\t"<<setw(8)<<effSINRs(0)<<","<<effSINRs(1)<<",\t\t"<<setw(8)<<SBcqiIndexPerCodeword(0)(0)<<","<<SBcqiIndexPerCodeword(0)(1)<<"\t\t"<<setw(3)<<WB_PMI<<"\t\t"<<setw(3)<<SB_PMI(0)<<endl;
  }
  else
  {
    cout<<"[detl:] Invalid aperiodic mode in PrintCQIindices"<<endl;
    abort();
  }
}

void PrintPMILog(AperiodicReportingMode_E AperiodicMode,int nodeID, int subframenumber,string AperiodicPMIlogsFileName,NRMultipanelCQIInfoAPeriodic_S nrMPCQIInfoAPeriodic)
{
    std::ofstream PMIlogs;
    static bool isNewPrint=1;
    if(isNewPrint)
    {
        PMIlogs.open(AperiodicPMIlogsFileName.c_str(),ios::out);
        
        if(currentTaskid==0)
        {
            if(AperiodicMode==_APERIODIC_MODE_1_2_)
                PMIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tSB_PMIs  \t\t (NOTE: -1 => CQI/PMI is not available)\n";
            else if(AperiodicMode==_APERIODIC_MODE_3_0_ || AperiodicMode==_APERIODIC_MODE_3_1_)
                PMIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tWB_PMI,\t\tSB_CQI_Indices  \t\t (NOTE: -1 => CQI/PMI is not available)\n";
            else if (AperiodicMode ==_APERIODIC_MODE_3_2_)
                PMIlogs<<"%_APERIODIC_MODE_3_2_"<<endl<<"%SubframeNumber,\tUE_ID,\t\t WB_PMI,\t\t WB_PMI i1_1,\t\t WB_PMI i1_2,\t\t WB_PMI i1_3,\tSB_CQI_Indices i2\t\t (NOTE: -1 => PMI is not available)\n";  
            else if(AperiodicMode==_APERIODIC_MODE_2_0_ || AperiodicMode==_APERIODIC_MODE_2_2_)
                PMIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tSB_CQI_Indices,\t\tWB_PMI,\t\tSB_PMI \t\t (NOTE: -1 => CQI/PMI is not available)\n";
            else
            {
                cout<<"[detl:] Invalid aperiodic mode in PrintCQIindices"<<endl;
                abort();
            }
        }
        isNewPrint=0;
    }
    else
        PMIlogs.open(AperiodicPMIlogsFileName.c_str(),ios::app);
    
    if(AperiodicMode==_APERIODIC_MODE_1_2_)
    {
        
    }
    else if(AperiodicMode==_APERIODIC_MODE_3_0_ || AperiodicMode==_APERIODIC_MODE_3_1_)
    {
        
    }
    else if(AperiodicMode ==_APERIODIC_MODE_3_2_)
    {
        
        PMIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8);
        
        PMIlogs<<setw(6)<<nrMPCQIInfoAPeriodic.widebandPMI<<",\t\t\t"<<setw(6)<<nrMPCQIInfoAPeriodic.wideBandPrecoderIndi1_1<<",\t\t\t"<<setw(6)<<nrMPCQIInfoAPeriodic.wideBandPrecoderIndi1_2<<",\t\t\t"<<setw(6)<<nrMPCQIInfoAPeriodic.wideBandPrecoderIndi1_3<<",\t\t\t";
        
        for(int count=0;count<nrMPCQIInfoAPeriodic.subbandPrecIndi2.length();count++)
        {
            PMIlogs<<setw(8)<<nrMPCQIInfoAPeriodic.subbandPrecIndi2(count)<<",";
        }
        PMIlogs<<endl;
    }
    else if(AperiodicMode==_APERIODIC_MODE_2_0_ || AperiodicMode==_APERIODIC_MODE_2_2_)
    {
    }
    else
    {
        cout<<"[detl:] Invalid aperiodic mode in PrintCQIindices"<<endl;
        abort();
    }
}

void PrintPMILog(AperiodicReportingMode_E AperiodicMode,int nodeID, int subframenumber,string AperiodicPMIlogsFileName,NRCQIInfoAPeriodic_S nrCQIInfoAPeriodic)
{
  
  std::ofstream PMIlogs;
  static bool isNewPrint=1;
  if(isNewPrint)
  {
    PMIlogs.open(AperiodicPMIlogsFileName.c_str(),ios::out);
    
    if(currentTaskid==0)
    {
      if(AperiodicMode==_APERIODIC_MODE_1_2_)
        PMIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tSB_PMIs  \t\t (NOTE: -1 => CQI/PMI is not available)\n";
      else if(AperiodicMode==_APERIODIC_MODE_3_0_ || AperiodicMode==_APERIODIC_MODE_3_1_)
        PMIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tWB_PMI,\t\tSB_CQI_Indices  \t\t (NOTE: -1 => CQI/PMI is not available)\n";
      else if (AperiodicMode ==_APERIODIC_MODE_3_2_)
        PMIlogs<<"%_APERIODIC_MODE_3_2_"<<endl<<"%SubframeNumber,\tUE_ID,\t\t WB_PMI,\t\t WB_PMI i1_1,\t\t WB_PMI i1_2,\t\t WB_PMI i1_3,\tSB_CQI_Indices i2\t\t (NOTE: -1 => PMI is not available)\n";  
      else if(AperiodicMode==_APERIODIC_MODE_2_0_ || AperiodicMode==_APERIODIC_MODE_2_2_)
        PMIlogs<<"%SubframeNumber,\tUE_ID,\t\tWB_CQI_Indices,\t\tModOrder,\t\teffSINRindB,\t\tSB_CQI_Indices,\t\tWB_PMI,\t\tSB_PMI \t\t (NOTE: -1 => CQI/PMI is not available)\n";
      else
      {
        cout<<"[detl:] Invalid aperiodic mode in PrintCQIindices"<<endl;
        abort();
      }
    }
    isNewPrint=0;
  }
  else
    PMIlogs.open(AperiodicPMIlogsFileName.c_str(),ios::app);
  
  if(AperiodicMode==_APERIODIC_MODE_1_2_)
  {
    
  }
  else if(AperiodicMode==_APERIODIC_MODE_3_0_ || AperiodicMode==_APERIODIC_MODE_3_1_)
  {
    
  }
  else if(AperiodicMode ==_APERIODIC_MODE_3_2_)
  {
    
    PMIlogs<<setw(6)<<subframenumber<<",\t\t\t"<<setw(5)<<nodeID<<",\t\t"<<setw(8);
    
    PMIlogs<<setw(6)<<nrCQIInfoAPeriodic.widebandPMI<<",\t\t\t"<<setw(6)<<nrCQIInfoAPeriodic.wideBandPrecoderIndi1_1<<",\t\t\t"<<setw(6)<<nrCQIInfoAPeriodic.wideBandPrecoderIndi1_2<<",\t\t\t"<<setw(6)<<nrCQIInfoAPeriodic.wideBandPrecoderIndi1_3<<",\t\t\t";
    
    for(int count=0;count<nrCQIInfoAPeriodic.subbandPrecIndi2.length();count++)
    {
      PMIlogs<<setw(8)<<nrCQIInfoAPeriodic.subbandPrecIndi2(count)<<",";
    }
    PMIlogs<<endl;
  }
  else if(AperiodicMode==_APERIODIC_MODE_2_0_ || AperiodicMode==_APERIODIC_MODE_2_2_)
  {
  }
  else
  {
    cout<<"[detl:] Invalid aperiodic mode in PrintCQIindices"<<endl;
    abort();
  }
}

_UE_CATEGORY_ findUEcategory(int ueCategory)
{
  _UE_CATEGORY_ output;
  
  switch(ueCategory)
  {
    case 1: output=CATEGORY_1;break;
    case 2: output=CATEGORY_2;break;
    case 3: output=CATEGORY_3;break;
    case 4: output=CATEGORY_4;break;
    case 5: output=CATEGORY_5;break;
    case 6: output=CATEGORY_6;break;
    case 7: output=CATEGORY_7;break;
    case 8: output=CATEGORY_8;break;
    case 9: output=CATEGORY_9;break;
    case 10: output=CATEGORY_10;break;
    default: {cout<<"[both:]Unkonown ueCategory.. Aborting"<<endl;abort();}
  }
  
  return output;
}

void PerUserULCQIFeedback_S::update(PerUserULCQIFeedback_S &cqiInfo){
  
  serviceNodeID=cqiInfo.serviceNodeID;

  if(subbandCqiInfoPerGroup.length()!=0)
  {
    for(int cqi_cnt=0;cqi_cnt<cqiInfo.subbandCqiInfoPerGroup.length();cqi_cnt++)
    {
      int measurementIndx=getMeasurementIndx(cqiInfo.subbandCqiInfoPerGroup(cqi_cnt).requestID);
      if(measurementIndx!=-1)
      {
	for(int rb_cnt=0;rb_cnt<cqiInfo.subbandCqiInfoPerGroup(cqi_cnt).subbandIndexPerRB.length();rb_cnt++)
	{
	  int sbIndx=cqiInfo.subbandCqiInfoPerGroup(cqi_cnt).subbandIndexPerRB(rb_cnt);
	  if(sbIndx!=-1) // subband index will be -1 if CQI is not computed for that rb
	  {
	    subbandCqiInfoPerGroup(measurementIndx).subbandIndexPerRB(rb_cnt)=sbIndx;
	    subbandCqiInfoPerGroup(measurementIndx).effSINRPerCodeWord(sbIndx)=cqiInfo.subbandCqiInfoPerGroup(cqi_cnt).effSINRPerCodeWord(sbIndx);
	    if(cqiInfo.subbandCqiInfoPerGroup(measurementIndx).pmiIndex.length())
	      subbandCqiInfoPerGroup(cqi_cnt).pmiIndex(sbIndx)=cqiInfo.subbandCqiInfoPerGroup(cqi_cnt).pmiIndex(sbIndx);
	  }
	}
      }
      else
      {
	append(subbandCqiInfoPerGroup,cqiInfo.subbandCqiInfoPerGroup(cqi_cnt));
	append(cqiMeasurementConfig,cqiInfo.cqiMeasurementConfig(cqi_cnt));
      }
    }
  }
  else
  {
    subbandCqiInfoPerGroup=cqiInfo.subbandCqiInfoPerGroup;
    cqiMeasurementConfig=cqiInfo.cqiMeasurementConfig;

  }
  isValid = cqiInfo.isValid;
}

int getNumOfREsPerRB(ivec CSIRSPorts,FrameStructure& frameStructure,Array<ivec>& rsSymPositions,ivec& rsScPositions)
{
  int nrCSIRSPorts = CSIRSPorts.length();
  Array<string> rbName(nrCSIRSPorts);
  
  for(int port_cnt=0;port_cnt<nrCSIRSPorts;port_cnt++)
    rbName(port_cnt)="port"+toString(CSIRSPorts(port_cnt));
  
  imat rsRBStructure=frameStructure.getMergedResourceUnit("nullRB",rbName,"data",rsSymPositions,rsScPositions);
  
  rsRBStructure = rsRBStructure.get_cols(3,rsRBStructure.cols()-1);
  ivec dataTones = find(rsRBStructure,0,true);
  
  return dataTones.length();
}

void checkNRTransmissionScheme(TransmissionScheme_E txScheme){
  
  if(txScheme != _TRANSMISSION_SCHEME_NR_CL_) // NR Tx Mode check
  {
    cout<<"[both:] Only _TRANSMISSION_SCHEME_NR_CL_ is supported"<<endl;
    cout<<txScheme<<endl;
    abort();
  }
}

cmat avgCovariance(Array<cmat>& channel)
{
    cmat avgChannelCovariance = channel(0).H()*channel(0);
    for(int i=1;i<channel.length();i++)
    {
        avgChannelCovariance += channel(i).H()*channel(i);
    }
    return avgChannelCovariance/channel.length();
}

ivec getNearestIndices(vec Phase,ivec N)
{
    if(Phase.length()!=N.length() && N.length()!=1){cout<<"Lengths of the vectors must be same.."<<endl;}
    if(N.length()==1){N = N(0)*ones_i(Phase.length());}
    ivec n(Phase.length());
    for(int i=0;i<Phase.length();i++)
        n(i) = mod((int)std::round(Phase(i)*2/pi),N(i));
    return n;
}
