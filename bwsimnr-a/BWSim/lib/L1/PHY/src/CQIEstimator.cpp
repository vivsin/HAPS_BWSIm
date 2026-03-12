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

#include "../include/CQIEstimator.h"

CQIEstimator::CQIEstimator(){
  
  geometrySINRindB=-10;
  sinrOffsetForOLLA=0.0;
  ackCount=0;
  nackCount=0;
  //isWithAbstraction=false;
  sinrOverWideband.set_length(0);
  estimateInterference=true;
  
  #ifdef USING_SRS
  isSRSEnabled=false;
  srsRBs.set_size(0);
  isOddSCs=false;
  #endif
}

void CQIEstimator::getPhysicalChannel(Array< Array<cmat> >& outputChannel,ResourceMapper *resourceMapper,Array< Array<cmat> > &channel, int vrbNumber,bool isDownlink){
  
  Size_S rbSize;
  Array<ivec> phyLoc;
  if(isDownlink)
  {
    rbSize=resourceMapper->dlResourceMapper.getRBSize();
    phyLoc=resourceMapper->dlResourceMapper.getPRBSubcarrierStartLocationinEachSlot(vrbNumber);
  }
  else
  {
    rbSize=resourceMapper->ulResourceMapper.getRBSize();
    phyLoc=resourceMapper->ulResourceMapper.getPRBSubcarrierStartLocationinEachSlot(vrbNumber);
  }

  int txAntenna=channel.length();
  int rxAntenna=channel(0).length();
  
  outputChannel.set_size(txAntenna);//CQI1 <nTx<nRx<nSubcarrier,nSymbol>>>
  cmat temp=zeros_c(rbSize.numberOfSubcarriers,rbSize.numberOfSymbols);
  
  // obtaining the values of the channel for the PRB under consideration from the complete channel matrix
  int nSymbolsInOneSlot=rbSize.numberOfSymbols/2;
  int r1=0,r2=rbSize.numberOfSubcarriers-1;
  for(int tx_cnt=0;tx_cnt<txAntenna;tx_cnt++)
  {
    outputChannel(tx_cnt).set_length(rxAntenna);
    for(int rx_cnt=0;rx_cnt<rxAntenna;rx_cnt++)
    {
      cmat tempMat0,tempMat1;
      for(int slot_cnt=0;slot_cnt<2;slot_cnt++)
      {
	if(slot_cnt==0)
	  tempMat0=channel(tx_cnt)(rx_cnt)(phyLoc(slot_cnt)(r1),phyLoc(slot_cnt)(r2),slot_cnt*nSymbolsInOneSlot,slot_cnt*nSymbolsInOneSlot+nSymbolsInOneSlot-1);
	else
	  tempMat1=channel(tx_cnt)(rx_cnt)(phyLoc(slot_cnt)(r1),phyLoc(slot_cnt)(r2),slot_cnt*nSymbolsInOneSlot,slot_cnt*nSymbolsInOneSlot+nSymbolsInOneSlot-1);
      }
      outputChannel(tx_cnt)(rx_cnt)=concat_horizontal(tempMat0,tempMat1);
    }
  }
}


Array< cmat > CQIEstimator::getPrecodedPhysicalChannel(ResourceMapper *resourceMapper, PortToAntennaMapper_S *portToAntennaMapper ,Array< Array<cmat> > &channel, int vrbNumber,TransmissionScheme_E txScheme, int nLayers, int nPorts,bool isCloseLoop,int pmi_i1,bool isDownlink,ivec positionsToConsider, cmat precoder){
  
  Array< Array<cmat> > myChannel;
  getPhysicalChannel(myChannel, resourceMapper,channel,vrbNumber,isDownlink);
  return(precodeMyRBChannelForCQI(myChannel, portToAntennaMapper , vrbNumber, txScheme, nLayers, nPorts,isCloseLoop,pmi_i1,positionsToConsider, precoder));
}

Array< cmat > CQIEstimator::getRawPhysicalChannel(ResourceMapper *resourceMapper ,Array< Array<cmat> > &channel, int vrbNumber,bool isDownlink,ivec positionsToConsider){

  Array< Array<cmat> > myChannel;
  getPhysicalChannel(myChannel, resourceMapper,channel,vrbNumber,isDownlink);

  Array< cmat > rawChannel(positionsToConsider.length());
  for(int tone_cnt=0;tone_cnt<positionsToConsider.length();tone_cnt++)
    rawChannel(tone_cnt)=getAcrossArray(myChannel,positionsToConsider(tone_cnt));

  return rawChannel;
}

void CQIEstimator::getPrecodedPhysicalChannelAcrossRE(Array<cmat>& signalChannel, ResourceMapper *resourceMapper, PortToAntennaMapper_S *portToAntennaMapper ,Array< Array<cmat> > &channel, int vrbNumber,TransmissionScheme_E txScheme, int nLayers, int nPorts,bool isCloseLoop,int pmi_i1,bool isDownlink,int reCountPerRb,int numberOfRbs,ivec positionsToConsider,cmat precoder)
{
  int totalRe_cnt=0;

  int ReCountPerBand=numberOfRbs*reCountPerRb;
  signalChannel.set_length(ReCountPerBand);
  for(int rb_cnt=0;rb_cnt<numberOfRbs;rb_cnt++)
  {
    Array< cmat > ruPrecodedChannel=getPrecodedPhysicalChannel(resourceMapper,portToAntennaMapper,channel,vrbNumber+rb_cnt,txScheme,nLayers,nPorts,isCloseLoop,pmi_i1,isDownlink,positionsToConsider,precoder);
    for(int re_cnt=0;re_cnt <reCountPerRb;re_cnt++, totalRe_cnt++)
    {
      signalChannel(totalRe_cnt)=ruPrecodedChannel(re_cnt);
      signalChannel(totalRe_cnt)= sqrt(inv_dB(crsPowBoostInDB))*signalChannel(totalRe_cnt);  // DL Pow alloc - scaling done to take care of CRS boost done to achive DL power allocation    
    }
  }
  
}

void CQIEstimator::getRawPhysicalChannelAcrossRE(Array<cmat>& rawChannel, ResourceMapper *resourceMapper ,Array< Array<cmat> > &channel, int vrbNumber,bool isDownlink,int reCountPerRb,int numberOfRbs,ivec positionsToConsider)
{
  int totalRe_cnt=0;

  int ReCountPerBand=numberOfRbs*reCountPerRb;
  rawChannel.set_length(ReCountPerBand);
  for(int rb_cnt=0;rb_cnt<numberOfRbs;rb_cnt++)
  {
    Array< cmat > ruPrecodedChannel=getRawPhysicalChannel(resourceMapper,channel,vrbNumber+rb_cnt,isDownlink,positionsToConsider);
    for(int re_cnt=0;re_cnt <reCountPerRb;re_cnt++, totalRe_cnt++)
    {
      rawChannel(totalRe_cnt)=ruPrecodedChannel(re_cnt);
    }
  }
}

Array< cmat > CQIEstimator::getIntrPrecodedPhysicalChannelOverRS(ResourceMapper *resourceMapper, PortToAntennaMapper_S *portToAntennaMapper ,Array< Array<cmat> > &channel, int vrbNumber, DataBlkInfo_S &dataBlkInfo,bool isDownlink,ivec positionsToConsider){
  
  Array< Array<cmat> > myChannel;
  getPhysicalChannel(myChannel,resourceMapper,channel,vrbNumber,isDownlink);
  return(precodeIntrRBChannelOverRS(myChannel, portToAntennaMapper, vrbNumber ,dataBlkInfo,positionsToConsider));
}

// void CQIEstimator::getPrecodedIntrfrChannelAcrossRE(Array< Array<cmat> >& intrfrChannel,ResourceMapper *resourceMapper,Array<Framehandler*> interferingTxFrameHandlers,ivec interfererChannelIndices,PortToAntennaMapper_S *portToAntennaMapper ,Array< Array< Array<cmat> > > &allChannels, int vrbNumber,bool isDownlink,int reCountPerRb,int numOfRbs,int nRx,int nLayers,ivec positionsToConsider)
// {
//   intrfrChannel.set_length(interfererChannelIndices.length());
//   for(int intr_cnt=0;intr_cnt<interfererChannelIndices.length();intr_cnt++)
//   {
//     int totalRe_cnt=0;
//     intrfrChannel(intr_cnt).set_length(reCountPerRb*numOfRbs);
//     
//     for(int rb_cnt=0;rb_cnt<numOfRbs;rb_cnt++)
//     {
//       ivec dataBlkNumbers=interferingTxFrameHandlers(intr_cnt)->getDataBlkNumbers(vrbNumber+rb_cnt);
//       if(dataBlkNumbers.length()==1)
//       {
// 	DataBlkInfo_S dataBlkInfoInInterferingTx=interferingTxFrameHandlers(intr_cnt)->getDataBlkInfo(dataBlkNumbers(0));
// 	Array< cmat > ruPrecodedChannel;
// 	ruPrecodedChannel=getIntrPrecodedPhysicalChannelOverRS(resourceMapper,portToAntennaMapper,allChannels(interfererChannelIndices(intr_cnt)),vrbNumber+rb_cnt,dataBlkInfoInInterferingTx,isDownlink,positionsToConsider);
// 	
// 	//!------- DL power-allocation related
// 	if(isDownlink)
// 	  if(dataBlkInfoInInterferingTx.dataGenInfo.rbInfo.dlPowMask.length()!=0)
// 	  {
// 	    int rb_loc_ind = find(dataBlkInfoInInterferingTx.resourceBlocks,vrbNumber+rb_cnt);
// //	    int rxAntenna = allChannels(interfererChannelIndices(intr_cnt))(0).length();     
// 	    //int nLayers = dataBlkInfoInInterferingTx.dataGenInfo.nLayers;
// 	    cvec PowerVec; PowerVec.set_size(positionsToConsider.length());PowerVec.zeros();
// 	    for(int cqiToneInd = 0 ; cqiToneInd < positionsToConsider.length();cqiToneInd++)
// 	      PowerVec(cqiToneInd) = sqrt(dataBlkInfoInInterferingTx.dataGenInfo.rbInfo.dlPowMask(rb_loc_ind)(positionsToConsider(cqiToneInd)));
// 	    for(int re_cnt=0;re_cnt<ruPrecodedChannel.length();re_cnt++)              
// 	      ruPrecodedChannel(re_cnt) = ruPrecodedChannel(re_cnt)*PowerVec(re_cnt);              
// 	  }
// 	  //!------------------------------	
// 	  
// 	  for(int re_cnt=0;re_cnt <reCountPerRb;re_cnt++, totalRe_cnt++)
// 	    intrfrChannel(intr_cnt)(totalRe_cnt)=ruPrecodedChannel(re_cnt);
//       }
//       else if(dataBlkNumbers.length()> 1) //Intr is MUMIMO
//       {
// 	DataBlkInfo_S intrDataBlkInfo1=interferingTxFrameHandlers(intr_cnt)->getDataBlkInfo(dataBlkNumbers(0));
// 
// 	for(int cnt = 1; cnt < dataBlkNumbers.length(); cnt++)
// 	{
// 	  DataBlkInfo_S intrDataBlkInfo2=interferingTxFrameHandlers(intr_cnt)->getDataBlkInfo(dataBlkNumbers(cnt));
// 	  intrDataBlkInfo1.dataGenInfo.nLayers += intrDataBlkInfo2.dataGenInfo.nLayers;
// 	  int rbIndx1=find(intrDataBlkInfo1.resourceBlocks,vrbNumber+rb_cnt);
// 	  int rbIndx2=find(intrDataBlkInfo2.resourceBlocks,vrbNumber+rb_cnt);
// 	  
// 	  appendHorizontal(intrDataBlkInfo1.precoderPerRB(rbIndx1),intrDataBlkInfo2.precoderPerRB(rbIndx2));
// 	  
// 	}
// 	Array< cmat > ruPrecodedChannel;
// 	ruPrecodedChannel=getIntrPrecodedPhysicalChannelOverRS(resourceMapper,portToAntennaMapper,allChannels(interfererChannelIndices(intr_cnt)),vrbNumber+rb_cnt,intrDataBlkInfo1,isDownlink,positionsToConsider);
// 	//!------- DL power-allocation related
// 	if(isDownlink)
// 	  if(intrDataBlkInfo1.dataGenInfo.rbInfo.dlPowMask.length()!=0)
// 	  {
// 	    int rb_loc_ind = find(intrDataBlkInfo1.resourceBlocks,vrbNumber+rb_cnt);
// 	    cvec PowerVec; PowerVec.set_size(positionsToConsider.length());PowerVec.zeros();
// 	    for(int cqiToneInd = 0 ; cqiToneInd < positionsToConsider.length();cqiToneInd++)
// 	      PowerVec(cqiToneInd) = sqrt(intrDataBlkInfo1.dataGenInfo.rbInfo.dlPowMask(rb_loc_ind)(positionsToConsider(cqiToneInd)));
// 	    for(int re_cnt=0;re_cnt<ruPrecodedChannel.length();re_cnt++)              
// 	      ruPrecodedChannel(re_cnt) = ruPrecodedChannel(re_cnt)*PowerVec(re_cnt);              
// 	  }
// 	  //!------------------------------
// 	  for(int re_cnt=0;re_cnt <reCountPerRb;re_cnt++, totalRe_cnt++)
// 	    intrfrChannel(intr_cnt)(totalRe_cnt)=ruPrecodedChannel(re_cnt);
//       }
//       else
//       {
// 	for(int re_cnt=0;re_cnt <reCountPerRb;re_cnt++, totalRe_cnt++)
// 	  intrfrChannel(intr_cnt)(totalRe_cnt)=zeros_c(nRx,nLayers);
//       }
//     }
//   }
// }

Array<cmat > CQIEstimator::getPerToneInterferenceCovariance(Array< cmat >& rxFrame, FrameStructure& frameStructure, ResourceMapper* resourceMapper, bool isDownlink, Array< Array< cmat > >& myChannel, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, imat rbRSPattern, ivec portReferenceNumbers, TransmissionScheme_E myTransmissionScheme, int reCountPerRb, int rbNumber, int NumOfRbs, ivec cqiPorts, double sigpowInWatts, double& rsrp)
{
  Array<cmat > perToneInterferenceCovariance(reCountPerRb*NumOfRbs);
  int totRe_cntForIntrCov=0;
  rsrp = 0;

  for(int rb_cnt=0;rb_cnt<NumOfRbs;rb_cnt++)
  {
    Array<cmat> receivedRB=extractFromRBArea(rxFrame,frameStructure,resourceMapper,rbNumber+rb_cnt,isDownlink);
    Array< Array<cmat> > signalChannelPerRB=extractFromRBArea(myChannel,frameStructure,resourceMapper,rbNumber+rb_cnt,isDownlink);
    
    if(cqiPorts.length()!=signalChannelPerRB.length())
      signalChannelPerRB=applyPortToAntennaMapperOnMyRBChannel(signalChannelPerRB,portToAntennaMapper,cqiPorts.length(),myTransmissionScheme<6);
     cmat estimatedInterferenceCovariancePerRB;
    Array<cvec> txRSPerPort=getTxRSPerPort(refSignal,resourceMapper,rbNumber+rb_cnt,cqiPorts,rbRSPattern,isDownlink);
    for(int Cnt=0; Cnt<txRSPerPort.length(); Cnt++)
      txRSPerPort(Cnt)=sqrt(sigpowInWatts)*txRSPerPort(Cnt); 
    rsrp += channelEstimator->estimateInterferenceCovarianceInRB(estimatedInterferenceCovariancePerRB, receivedRB, signalChannelPerRB, txRSPerPort,myTransmissionScheme, rbRSPattern,cqiPorts, portReferenceNumbers);
    
    for(int re_cnt=0;re_cnt <reCountPerRb;re_cnt++, totRe_cntForIntrCov++)
    {
      perToneInterferenceCovariance(totRe_cntForIntrCov).clear();
      perToneInterferenceCovariance(totRe_cntForIntrCov)=estimatedInterferenceCovariancePerRB;
    }
  }
  rsrp /= NumOfRbs; 
  
  return(perToneInterferenceCovariance);
}

Array<vec> CQIEstimator::computeSinrPerCW(FrameStructure  &frameStructure,double myTxSCPowerInDB/*, Array<Framehandler*> interferingTxFrameHandlers,vec interfererTxSCPowerInDB,ivec interfererChannelIndices*/,Array< Array<cmat> > myChannel,Array<cmat> &rxFrame, Array< Array< Array< cmat > > >& allChannels, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper,ReferenceSignal_S *refSignal, ResourceMapper* resourceMapper, double noiseVariance, double restOfInterferenceInDB,int rbNumber,TransmissionScheme_E txScheme, int nLayers,ivec cqiPorts,int nCodewords,int NumOfRbs,bool isCloseLoop,int pmi_i1,bool isDownlink,cmat precoder)
{
    int nPorts=cqiPorts.length();
  ivec portReferenceNumbers = getPortReferenceNumbers(frameStructure,cqiPorts);
  ivec portLocations=getPortLocationsInRB(frameStructure, cqiPorts,0,true);
  imat rbRSPattern = frameStructure.getResourceUnitMatrix(0);
  int reCountPerRb=portLocations.length();
  int nRxAntenna=myChannel(0).length();

  Array<cmat > signalChannel;
  getPrecodedPhysicalChannelAcrossRE(signalChannel,resourceMapper,portToAntennaMapper,myChannel,rbNumber,txScheme,nLayers,nPorts,isCloseLoop,pmi_i1,isDownlink,reCountPerRb,NumOfRbs,portLocations,precoder);

  Array<cmat> perToneInterferenceCovariance;
  double rsrp;
  if(estimateInterference)
    perToneInterferenceCovariance=getPerToneInterferenceCovariance(rxFrame,frameStructure,resourceMapper,isDownlink,myChannel,portToAntennaMapper,refSignal,rbRSPattern, portReferenceNumbers,txScheme,reCountPerRb,rbNumber,NumOfRbs,cqiPorts,inv_dB(myTxSCPowerInDB),rsrp);
  
//   else
//     perToneInterferenceCovariance=calculateIdealInterferenceCoVariance(resourceMapper,interferingTxFrameHandlers,interfererChannelIndices,portToAntennaMapper,allChannels,rbNumber,isDownlink,reCountPerRb,NumOfRbs,nRxAntenna,nLayers,portLocations,txScheme,2*noiseVariance+inv_dB(restOfInterferenceInDB),inv_dB(interfererTxSCPowerInDB));
  return(calculateSinrWithIdealChannelperSubband(txScheme,nCodewords,nLayers,demodulationScheme,reCountPerRb*NumOfRbs,signalChannel, inv_dB(myTxSCPowerInDB),perToneInterferenceCovariance));
}

Array < Array<vec > > CQIEstimator::computeSinrPerCWforI1indices(FrameStructure& frameStructure,double myTxSCPowerInDB, /*Array<Framehandler*> interferingTxFrameHandlers,vec interfererTxSCPowerInDB,ivec interfererChannelIndices,*/Array< Array<cmat> > myChannel,Array<cmat> &rxFrame, Array< Array< Array< cmat > > >& allChannels, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper,ReferenceSignal_S *refSignal, ResourceMapper* resourceMapper, double noiseVariance, double restOfInterferenceInDB,int rbNumber,TransmissionScheme_E txScheme, int nLayers, ivec cqiPorts, int nCodewords,int NumOfRbs,bool isCloseLoop,bool isDownlink, ivec wb_I1indices)
{
  int nPorts=cqiPorts.length();
  ivec portReferenceNumbers = getPortReferenceNumbers(frameStructure,cqiPorts);
  ivec portLocations=getPortLocationsInRB(frameStructure, cqiPorts,0,true);
  imat rbRSPattern = frameStructure.getResourceUnitMatrix(0);
  int reCountPerRb=portLocations.length();
  int nRxAntenna=myChannel(0).length();
  
  //CQI extracting the channel over all the DLRBS on positions corresponding to CRS Ports
  Array<cmat > rawChannel,signalChannel;
  Array<cmat> perToneInterferenceCovariance;
  double rsrp;
  
  if(estimateInterference)
    perToneInterferenceCovariance=getPerToneInterferenceCovariance(rxFrame,frameStructure,resourceMapper,isDownlink,myChannel,portToAntennaMapper,refSignal,rbRSPattern, portReferenceNumbers,txScheme,reCountPerRb,rbNumber,NumOfRbs,cqiPorts,inv_dB(myTxSCPowerInDB),rsrp);
//   else
//     perToneInterferenceCovariance = calculateIdealInterferenceCoVariance(resourceMapper,interferingTxFrameHandlers,interfererChannelIndices,portToAntennaMapper,allChannels,rbNumber,isDownlink,reCountPerRb,NumOfRbs,nRxAntenna,nLayers,portLocations,txScheme,2*noiseVariance+inv_dB(restOfInterferenceInDB),inv_dB(interfererTxSCPowerInDB));
  
  Array<cmat> codebook = mCodebook->getCodebook(nPorts,nLayers,isDownlink);
  Array < Array<vec > > sinrPerCodeword;
  if(txScheme == _TRANSMISSION_SCHEME_OLMUMIMO_)
  {
    int I1Length = wb_I1indices.length();
    sinrPerCodeword.set_length(I1Length);
    
    getRawPhysicalChannelAcrossRE(rawChannel,resourceMapper,myChannel,rbNumber,isDownlink,reCountPerRb,NumOfRbs,portLocations);
    

    //CQI loop over all the codebooks to compute the post processing SINR
    for(int precIndex=0; precIndex<I1Length; precIndex++)
    {
      // I2 precoder cycling
      cmat precoder = codebook(wb_I1indices(precIndex)*16 + (rbNumber % 4));
      signalChannel = precodeChannelForCQI(rawChannel,portToAntennaMapper,txScheme,nPorts,precoder);
      
      sinrPerCodeword(precIndex)=calculateSinrWithIdealChannelperSubband(txScheme,nCodewords,nLayers,demodulationScheme,reCountPerRb*NumOfRbs,signalChannel, inv_dB(myTxSCPowerInDB),perToneInterferenceCovariance);
    }
  }
  else
  {
    cout<<"Unsupported transmissionScheme..  "<<endl;abort();
  }
  
  return(sinrPerCodeword);
}

Array < Array<vec > > CQIEstimator::computeSinrPerCWForCodeBook(FrameStructure &frameStructure,double myTxSCPowerInDB/*, Array<Framehandler*> interferingTxFrameHandlers,vec interfererTxSCPowerInDB,ivec interfererChannelIndices*/,Array< Array<cmat> > myChannel,Array<cmat> &rxFrame, Array< Array< Array< cmat > > >& allChannels, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper,ReferenceSignal_S *refSignal, ResourceMapper* resourceMapper, double noiseVariance, double restOfInterferenceInDB,int rbNumber,TransmissionScheme_E txScheme, int nLayers, ivec cqiPorts, int nCodewords,int NumOfRbs,bool isCloseLoop,bool isDownlink, Array<cmat>& tCodeBook)
{
  int nPorts=cqiPorts.length();
  ivec portReferenceNumbers = getPortReferenceNumbers(frameStructure,cqiPorts);
  ivec portLocations=getPortLocationsInRB(frameStructure, cqiPorts,0,true);
   imat rbRSPattern = frameStructure.getResourceUnitMatrix(0);
  int reCountPerRb=portLocations.length();
  int nRxAntenna=myChannel(0).length();
  
  //CQI extracting the channel over all the DLRBS on positions corresponding to CRS Ports 
  Array<cmat > rawChannel,signalChannel;
  Array<cmat> perToneInterferenceCovariance;
  double rsrp;
 
  if(estimateInterference)		    
    perToneInterferenceCovariance=getPerToneInterferenceCovariance(rxFrame,frameStructure,resourceMapper,isDownlink,myChannel,portToAntennaMapper,refSignal,rbRSPattern, portReferenceNumbers,txScheme,reCountPerRb,rbNumber,NumOfRbs,cqiPorts,inv_dB(myTxSCPowerInDB),rsrp);
//   else
//     perToneInterferenceCovariance = calculateIdealInterferenceCoVariance(resourceMapper,interferingTxFrameHandlers,interfererChannelIndices,portToAntennaMapper,allChannels,rbNumber,isDownlink,reCountPerRb,NumOfRbs,nRxAntenna,nLayers,portLocations,txScheme,2*noiseVariance+inv_dB(restOfInterferenceInDB),inv_dB(interfererTxSCPowerInDB));
  Array < Array<vec > > sinrPerCodeword(1);
   /// tx modes 4,5,6,8,9
  if(txScheme==_TRANSMISSION_SCHEME_CL_SM_ || txScheme == _TRANSMISSION_SCHEME_CL_BF_  || txScheme==_TRANSMISSION_SCHEME_CL_MU_MIMO_|| txScheme == _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ || txScheme == _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme == _TRANSMISSION_SCHEME_NR_CL_ || txScheme == _TRANSMISSION_SCHEME_COMP_ || (txScheme == _TRANSMISSION_SCHEME_OLMUMIMO_ && 1 /* isSemiOpenloop */))
  {
    int codebookLength=tCodeBook.length();
    sinrPerCodeword.set_length(codebookLength);
   
    getRawPhysicalChannelAcrossRE(rawChannel,resourceMapper,myChannel,rbNumber,isDownlink,reCountPerRb,NumOfRbs,portLocations);
    
   
    //CQI loop over all the codebooks to compute the post processing SINR
    for(int precIndex=0; precIndex<codebookLength; precIndex++)
    {
      //! identity matrix precoder is restricted from the selection..
      if(txScheme != _TRANSMISSION_SCHEME_NR_CL_)
	if(precIndex==0 && nPorts==2 && nLayers==2)
	  continue;
      
      signalChannel = precodeChannelForCQI(rawChannel,portToAntennaMapper,txScheme,nPorts,tCodeBook(precIndex));
      
      sinrPerCodeword(precIndex)=calculateSinrWithIdealChannelperSubband(txScheme,nCodewords,nLayers,demodulationScheme,reCountPerRb*NumOfRbs,signalChannel, inv_dB(myTxSCPowerInDB),perToneInterferenceCovariance);
    }
  }
  else
  {
    getPrecodedPhysicalChannelAcrossRE(signalChannel,resourceMapper,portToAntennaMapper,myChannel,rbNumber,txScheme,nLayers,nPorts,isCloseLoop,-1,isDownlink,reCountPerRb,NumOfRbs,portLocations);
     
    sinrPerCodeword(0)=calculateSinrWithIdealChannelperSubband(txScheme,nCodewords,nLayers,demodulationScheme,reCountPerRb*NumOfRbs,signalChannel,inv_dB(myTxSCPowerInDB),perToneInterferenceCovariance);
  }
  
  return(sinrPerCodeword);
}

Array < Array<vec > > CQIEstimator::computeSinrPerPrecodersPerCW(FrameStructure &frameStructure,double myTxSCPowerInDB, /*Array<Framehandler*> interferingTxFrameHandlers,vec interfererTxSCPowerInDB,ivec interfererChannelIndices,*/Array< Array<cmat> > myChannel,Array<cmat> &rxFrame, Array< Array< Array< cmat > > >& allChannels, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper,ReferenceSignal_S *refSignal, ResourceMapper* resourceMapper, double noiseVariance, double restOfInterferenceInDB,int rbNumber,TransmissionScheme_E txScheme, int nLayers, ivec cqiPorts, int nCodewords,int NumOfRbs,bool isCloseLoop,bool isDownlink)
{ 
  int nPorts=cqiPorts.length();
  ivec portReferenceNumbers = getPortReferenceNumbers(frameStructure,cqiPorts);
  ivec portLocations=getPortLocationsInRB(frameStructure, cqiPorts,0,true);
  imat rbRSPattern = frameStructure.getResourceUnitMatrix(0);
  int reCountPerRb=portLocations.length();
  int nRxAntenna=myChannel(0).length();

  //CQI extracting the channel over all the DLRBS on positions corresponding to CRS Ports 
  Array<cmat > rawChannel,signalChannel;
  Array<cmat> perToneInterferenceCovariance;
  double rsrp;

  if(estimateInterference)		    
    perToneInterferenceCovariance=getPerToneInterferenceCovariance(rxFrame,frameStructure,resourceMapper,isDownlink,myChannel,portToAntennaMapper,refSignal,rbRSPattern, portReferenceNumbers,txScheme,reCountPerRb,rbNumber,NumOfRbs,cqiPorts,inv_dB(myTxSCPowerInDB),rsrp);
//   else
//     perToneInterferenceCovariance = calculateIdealInterferenceCoVariance(resourceMapper,interferingTxFrameHandlers,interfererChannelIndices,portToAntennaMapper,allChannels,rbNumber,isDownlink,reCountPerRb,NumOfRbs,nRxAntenna,nLayers,portLocations,txScheme,2*noiseVariance+inv_dB(restOfInterferenceInDB),inv_dB(interfererTxSCPowerInDB));

  Array < Array<vec > > sinrPerCodeword(1);
  /// tx modes 4,5,6,8,9
  if(txScheme==_TRANSMISSION_SCHEME_CL_SM_ || txScheme == _TRANSMISSION_SCHEME_CL_BF_  || txScheme==_TRANSMISSION_SCHEME_CL_MU_MIMO_|| txScheme == _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ || txScheme == _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme == _TRANSMISSION_SCHEME_NR_CL_ || txScheme == _TRANSMISSION_SCHEME_COMP_ || (txScheme == _TRANSMISSION_SCHEME_OLMUMIMO_ && 1 /* isSemiOpenloop */))
  {
    Array<cmat> codeBook = mCodebook->getCodebook(nPorts,nLayers,1);
    int codebookLength=codeBook.length();
    sinrPerCodeword.set_length(codebookLength);

    getRawPhysicalChannelAcrossRE(rawChannel,resourceMapper,myChannel,rbNumber,isDownlink,reCountPerRb,NumOfRbs,portLocations);


    //CQI loop over all the codebooks to compute the post processing SINR
    for(int precIndex=0; precIndex<codebookLength; precIndex++)
    {
      //! identity matrix precoder is restricted from the selection..
      if(precIndex==0 && nPorts==2 && nLayers==2)
	continue;

      signalChannel = precodeChannelForCQI(rawChannel,portToAntennaMapper,txScheme,nPorts,codeBook(precIndex));

      sinrPerCodeword(precIndex)=calculateSinrWithIdealChannelperSubband(txScheme,nCodewords,nLayers,demodulationScheme,reCountPerRb*NumOfRbs,signalChannel, inv_dB(myTxSCPowerInDB),perToneInterferenceCovariance);
    }
  }
  else
  {
    getPrecodedPhysicalChannelAcrossRE(signalChannel,resourceMapper,portToAntennaMapper,myChannel,rbNumber,txScheme,nLayers,nPorts,isCloseLoop,-1,isDownlink,reCountPerRb,NumOfRbs,portLocations);
    sinrPerCodeword(0)=calculateSinrWithIdealChannelperSubband(txScheme,nCodewords,nLayers,demodulationScheme,reCountPerRb*NumOfRbs,signalChannel,inv_dB(myTxSCPowerInDB),perToneInterferenceCovariance);
  }

  return(sinrPerCodeword);
}

ivec CQIEstimator::getCQIPerCodewords(double& exp_spec_eff, Array<vec>& sinrPerCodeword, vec& MMIBperModSchemes,ivec& modOrdersPerCW, vec& effSINRsPerCW,int rbCount,int nREsPerRB,TransmissionScheme_E txScheme,int nLayers, bool enableCQI0){

  ivec nLayersPerCodeword = getNumLayersPerCodeWord(sinrPerCodeword.length(),nLayers/getSymbolRepetitionFactor(txScheme,nLayers));
  exp_spec_eff=0.0;
  double exp_spec_eff1;
  ivec CQIPerCodewords(sinrPerCodeword.length());
  modOrdersPerCW.set_length(sinrPerCodeword.length());
  effSINRsPerCW.set_length(sinrPerCodeword.length());
  MMIBperModSchemes = zeros(4);
  vec mmib_perModScheme = zeros(4);
  
  for(int icw=0; icw<CQIPerCodewords.length(); icw++)
  {
    ivec tbSizePerCQI = zeros_i(16);
    for(int cqi=1;cqi<tbSizePerCQI.length();cqi++)
      tbSizePerCQI(cqi) = tbTables->cqiToMaxMCSRate.modulationIndex(cqi-1) * tbTables->cqiToMaxMCSRate.codeRate(cqi-1) * nREsPerRB * rbCount * nLayersPerCodeword(icw);
    
    vec sinr = sinrPerCodeword(icw) * inv_dB(sinrOffsetForOLLA); // Linear scale
    sinrPerCodeword(icw) = sinr;
    //cout<<"sinr"<<sinrPerCodeword(icw)<<endl;
    // Using QPSK MI curve even for BPSK enabled case - Dhiv
    mmib_perModScheme(0) = tbTables->awgnTablesConvCoder.sinrToEffMMIB(sinr,_MODULATION_SCHEME_QPSK_);
    mmib_perModScheme(1) = tbTables->awgnTablesConvCoder.sinrToEffMMIB(sinr,_MODULATION_SCHEME_16_QAM_);
    mmib_perModScheme(2) = tbTables->awgnTablesConvCoder.sinrToEffMMIB(sinr,_MODULATION_SCHEME_64_QAM_);
    if(tbTables->awgnTablesConvCoder.is256QAMenabled)
      mmib_perModScheme(3) = tbTables->awgnTablesConvCoder.sinrToEffMMIB(sinr,_MODULATION_SCHEME_256_QAM_);
    
    CQIPerCodewords(icw) = tbTables->awgnTablesConvCoder.CQISelect(exp_spec_eff1, mmib_perModScheme(3), mmib_perModScheme(2), mmib_perModScheme(1), mmib_perModScheme(0),tbSizePerCQI,enableCQI0);
    //cout<<"cqi"<<CQIPerCodewords(icw)<<endl;
//     exp_spec_eff += (exp_spec_eff1 * nLayersPerCodeword(icw));
    MMIBperModSchemes += (mmib_perModScheme * nLayersPerCodeword(icw));
       
    int linearIndex;
    modOrdersPerCW(icw) = tbTables->cqiToMaxMCSRate.getCQIModulationAlphabet(CQIPerCodewords(icw));
//     modOrdersPerCW(icw) = getModulationOrderFromCQIIndex(tbTables->awgnTablesConvCoder.get256QAMFlag(),CQIPerCodewords(icw),linearIndex);
    if(modOrdersPerCW(icw)==1) // BPSK
        linearIndex=0;
    else
        linearIndex =  modOrdersPerCW(icw)/2-1;
    
    effSINRsPerCW(icw) = tbTables->awgnTablesConvCoder.mmibToSinr(ModulationScheme_E(modOrdersPerCW(icw)),mmib_perModScheme(linearIndex));
  
    exp_spec_eff += mean(log2(1.0 + sinrPerCodeword(icw))) * nLayersPerCodeword(icw);
    
}
  return CQIPerCodewords;
  
}

int PMIselect(ivec possiblePMIs, Array<vec>& MMIBvaluesperModScheme){
  
  int PMI=-1;
  double mmib_qpsk=0,mmib_sxtn=0,mmib_sxfr=0;
  double tempMMIBsxfr,tempMMIBsxtn,tempMMIBQpsk;
  
  for(int precIndex=0; precIndex<possiblePMIs.length(); precIndex++)
  {
    tempMMIBsxfr=MMIBvaluesperModScheme(precIndex)(2);
    tempMMIBsxtn=MMIBvaluesperModScheme(precIndex)(1);
    tempMMIBQpsk=MMIBvaluesperModScheme(precIndex)(0);
    
    
    if(tempMMIBsxfr>=mmib_sxfr)
    {
      mmib_qpsk=tempMMIBQpsk;
      mmib_sxtn=tempMMIBsxtn;
      mmib_sxfr=tempMMIBsxfr;
      PMI=possiblePMIs(precIndex);
    }
    else if(tempMMIBsxtn>=mmib_sxtn)
    {
      mmib_qpsk=tempMMIBQpsk;
      mmib_sxtn=tempMMIBsxtn;
      mmib_sxfr=tempMMIBsxfr;
      PMI=possiblePMIs(precIndex);
    }
    else if(tempMMIBQpsk>=mmib_qpsk)
    {
      mmib_qpsk=tempMMIBQpsk;
      mmib_sxtn=tempMMIBsxtn;
      mmib_sxfr=tempMMIBsxfr;
      PMI=possiblePMIs(precIndex);
    }
  }
  
  if(PMI==-1)
  {
    cout<<"MMIB Exception.."<<endl;
    cout<<"possiblePMIs : "<<possiblePMIs<<endl;
    cout<<"MMIBvaluesperModScheme : "<<MMIBvaluesperModScheme<<endl;
    abort();
  }
  
  return PMI;
}

int getBestPMIindex(vec SpectralEfficiencyOverPMIs,Array<vec> MMIBvaluesPermodscheme)
{
  ivec possiblePMIs = find(SpectralEfficiencyOverPMIs,max(SpectralEfficiencyOverPMIs),true);
  
  int tempPMI=-1;
  if(possiblePMIs.length()==1)
    tempPMI=possiblePMIs(0);
  else
  {
    Array<vec> MMIBvaluesPermodschemeTemp = getFromArray(MMIBvaluesPermodscheme,possiblePMIs);
    tempPMI=PMIselect(possiblePMIs,MMIBvaluesPermodschemeTemp);
  }
  return(tempPMI);
}

void CQIEstimator::computeCQIFromIdealChannelForDownlinkPeriodicCQIReport(DLCQIMeasurement_S &perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper,ReferenceSignal_S *refSignal, ResourceMapper* resourceMapper,Array<cmat> &rxFrame, Array< Array< Array< cmat > > >& allChannels, double myTxSCPowerInDB, int desiredTxNode, ivec txNodeIDSForChannel,CQIMeasurementConfig_S &cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int ServiceNodeID, int subframeNumber, string WideBandCQIlogsFileName , string SubbandCQIlogsFileName){

  Array<cmat> codeBook;
  cmat precoder;
  int nPorts;
  ivec portLocations;
  Array<vec> sinrPerCodewordForSubbandCQI; 
  Array<vec> sinrPerCodewordForWidebandCQI;
  double exp_spec_eff;
  int rbNumber;
  ivec modOrderPerCW;
  vec effSINRsPerCW;
  TransmissionScheme_E txScheme=cqiMeasurementConfig.transmissionScheme;
  int nLayers=cqiMeasurementConfig.numberOfLayers;
  ivec cqiPorts=cqiMeasurementConfig.cqiPorts;
  int nCodewords=cqiMeasurementConfig.numberOfCodeWords;
  
  
  int tempPMI=0;  
  int desiredTxNodeIndx=find(txNodeIDSForChannel,desiredTxNode);
//   Framehandler* frameStructure=TxFrameHandlers(desiredTxNodeIndx);
//   Array<Framehandler*> interferingTxFrameHandlers;
//   vec interfererTxSCPowerInDB;
//   interferingTxFrameHandlers.set_length(txNodeIDSForChannel.length()-1);
//   interfererTxSCPowerInDB.set_length(txNodeIDSForChannel.length()-1);
  ivec interfererIndices=findNot(txNodeIDSForChannel,desiredTxNode);
  ivec portReferenceNumbers = getPortReferenceNumbers(frameStructure,cqiPorts);
  nPorts=cqiPorts.length();
  portLocations=getPortLocationsInRB(frameStructure, cqiPorts,0,true);
  
  Array<string> rbName(cqiPorts.length());
  rbName.set_length(cqiPorts.length());
  for(int port_cnt=0;port_cnt<cqiPorts.length();port_cnt++)
    rbName(port_cnt)="port"+toString(cqiPorts(port_cnt));
  imat rsRBStructure;
  rsRBStructure=frameStructure.getMergedResourceUnit("nullRB",rbName,"data",cqiMeasurementConfig.rsSymPositions,cqiMeasurementConfig.rsScPositions);
  
  
  rsRBStructure = rsRBStructure.get_cols(3,rsRBStructure.cols()-1);
  ivec dataTones = find(rsRBStructure,0,true);
  int nREsPerRB = dataTones.length();
  
//   for(int intr_cnt=0;intr_cnt<txNodeIDSForChannel.length()-1;intr_cnt++)
//   {
//     interfererTxSCPowerInDB(intr_cnt)=txSubcarrierPowerInDB(interfererIndices(intr_cnt));
//     interferingTxFrameHandlers(intr_cnt)=TxFrameHandlers(interfererIndices(intr_cnt));
//   }
  Array< Array<cmat> > myChannel=allChannels(desiredTxNodeIndx);
  
  switch(downlinkCQIInfo->cqiComputationInfo.periodicMode)
  {
    case _PERIODIC_MODE_1_0_:
    {
      if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger)
      {
	/// Temp Variables
	int DLRBs=cqiMeasurementConfig.nRBs;
	rbNumber=cqiMeasurementConfig.startRB;
	cout<<"Measuring wideband CQI over RBs : "<<DLRBs<<" from "<<rbNumber<<endl;
	switch(txScheme)
	{
	  /// tx modes 1,2,3,7...8,9
	  case _TRANSMISSION_SCHEME_SINGLE_PORT_CRS_:
	  case _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_:
	  case _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_:
	  case _TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_:
	  case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
	  case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
	  case _TRANSMISSION_SCHEME_COMP_:
	  case _TRANSMISSION_SCHEME_OLMUMIMO_:
	  case _TRANSMISSION_SCHEME_NR_CL_:
	  {
	    sinrPerCodewordForWidebandCQI=computeSinrPerCW(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,0,-1,1);
	    break;
	  }
	  default:{
	    cout<<"[both:] mode1_0 not available for all tx schemes "<<endl;
	    abort();
	  }
	}
	
	vec MMIBvaluesPermodscheme;
	perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQI,MMIBvaluesPermodscheme,modOrderPerCW,effSINRsPerCW,DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	perUserFeedback.widebandCqiInfoPerGroupPeriodic.isValid=true;
	
	if(traceValuePerTraceName["EnableUElogs"]=="true")
	  PrintCQIindices(ServiceNodeID,perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord,modOrderPerCW,effSINRsPerCW, subframeNumber,WideBandCQIlogsFileName);
	
	if(traceValuePerTraceName["EnablePrints"]=="true")
	{
	  if(perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord.length()==1)
	    cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" , Wideband CQI : "<<setw(5)<<perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0)<< " , effSINRs : "<<setw(11)<<effSINRsPerCW(0)<<" in subframe "<<setw(4)<<subframeNumber<<endl;
	  else
	    cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" , Wideband CQI : "<<setw(2)<<perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0)<<" "<<setw(2)<< perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(1)<< " , effSINRs : "<<setw(5)<<effSINRsPerCW(0)<<" "<<setw(5)<<effSINRsPerCW(1)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
	}
      }
      break;
    }
    
    case _PERIODIC_MODE_1_1_: // Wideband PMI and CQI Reporting
    {
      int DLRBs=cqiMeasurementConfig.nRBs;
      rbNumber=cqiMeasurementConfig.startRB;
      
      if(txScheme==_TRANSMISSION_SCHEME_OLMUMIMO_ && nPorts==8 && downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger)
      {
	Array< Array<vec> > sinrPerCodewordForWidebandCQItemp;
	
	ivec wb_I1indices = getValidi1Indices(nLayers);
	int I1Length = wb_I1indices.size();

	sinrPerCodewordForWidebandCQItemp = computeSinrPerCWforI1indices(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,1,1,wb_I1indices);

	vec SpectralEfficiencyOverPMIs=zeros(I1Length);
	Array<ivec> tempCQIperCodewords(I1Length);
	Array<vec> MMIBvaluesPermodscheme(I1Length);
	Array<ivec> modOrderPerCWPerPMI(I1Length);
	Array<vec> effSINRsPerCWPerPMI(I1Length);
	
	for(int precIndex=0; precIndex<I1Length; precIndex++)
	{
	  tempCQIperCodewords(precIndex)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQItemp(precIndex),MMIBvaluesPermodscheme(precIndex),modOrderPerCWPerPMI(precIndex),effSINRsPerCWPerPMI(precIndex),DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	  SpectralEfficiencyOverPMIs(precIndex)=exp_spec_eff;
	}
	int tempI1 = getBestPMIindex(SpectralEfficiencyOverPMIs,MMIBvaluesPermodscheme);
	perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1=wb_I1indices(tempI1);

	perUserFeedback.widebandCqiInfoPerGroupPeriodic.pmiIndex=-1; // PMI is not computed here

	perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord=tempCQIperCodewords(tempI1);
	perUserFeedback.widebandCqiInfoPerGroupPeriodic.isValid=true;
	
	if(traceValuePerTraceName["EnableUElogs"]=="true")
	  PrintCQIindices(ServiceNodeID,perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord,modOrderPerCWPerPMI(tempI1),effSINRsPerCWPerPMI(tempI1),subframeNumber,WideBandCQIlogsFileName,perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1);
	
	if(traceValuePerTraceName["EnablePrints"]=="true")
	{
	  if(perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord.length()==1)
	  {
	    cout<<"entering into periodic mode wideband  "<<endl;
	    cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" , Wideband I1 : "<<setw(2)<<perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1<<" , Wideband CQI : "<<setw(5)<<perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0)<< " , effSINRs : "<<setw(11)<<effSINRsPerCWPerPMI(tempI1)(0)<<" in subframe "<<setw(4)<<subframeNumber<<endl;
	  }
	  else
	    cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" , Wideband I1 : "<<setw(2)<<perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1<<" , Wideband CQI : "<<setw(2)<<perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0)<<" "<<setw(2)<< perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(1)<< " , effSINRs : "<<setw(5)<<effSINRsPerCWPerPMI(tempI1)(0)<<" "<<setw(5)<<effSINRsPerCWPerPMI(tempI1)(1)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
	}
	break;
      }
      
      
      else if(downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger)
      {
	if((txScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_  || txScheme==_TRANSMISSION_SCHEME_COMP_) && nPorts==8 && downlinkCQIInfo->cqiComputationInfo.submode==_SUBMODE_1_)
	{
	  Array< Array<vec> > sinrPerCodewordForWidebandCQItemp;
	  codeBook=mCodebook->getSubSampledCodeBookForSubmode1(nLayers);

	  sinrPerCodewordForWidebandCQItemp = computeSinrPerCWForCodeBook(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,1,1,codeBook);

	  vec SpectralEfficiencyOverPMIs=zeros(codeBook.length());
	  Array<ivec> tempCQIperCodewords(codeBook.length());
	  Array<vec> MMIBvaluesPermodscheme(codeBook.length());
	  Array<ivec> modOrderPerCWPerPMI(codeBook.length());
	  Array<vec> effSINRsPerCWPerPMI(codeBook.length());
	  
	  for(int precIndex=0; precIndex<codeBook.length(); precIndex++)
	  {
	    tempCQIperCodewords(precIndex)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQItemp(precIndex),MMIBvaluesPermodscheme(precIndex),modOrderPerCWPerPMI(precIndex),effSINRsPerCWPerPMI(precIndex),DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	    SpectralEfficiencyOverPMIs(precIndex)=exp_spec_eff;
	  }
	  tempPMI=getBestPMIindex(SpectralEfficiencyOverPMIs,MMIBvaluesPermodscheme);
	  
	  ivec PrecIndForSubSampledCB=mCodebook->CodeBookIndicesToSelecti1ForSubmode1(nLayers-1);
	  perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1=getLongTermPMIInd(nLayers,PrecIndForSubSampledCB(tempPMI));
	}
      }

      if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger)
      {
	/// Temp Variables
	Array< Array<vec> > sinrPerCodewordForWidebandCQItemp;
	if(nPorts<8)
	{
	  switch(txScheme)
	  {
	    /// tx modes 4,6,8,9
	    case _TRANSMISSION_SCHEME_CL_SM_:
	    case _TRANSMISSION_SCHEME_CL_MU_MIMO_:
	    case _TRANSMISSION_SCHEME_CL_BF_:
	    case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
	    case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
	    case _TRANSMISSION_SCHEME_NR_CL_:
	    {
	      codeBook=mCodebook->getCodebook(nPorts,nLayers,1);
	      sinrPerCodewordForWidebandCQItemp=computeSinrPerCWForCodeBook(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,1,1,codeBook);
	      break;
	    }
	    
	    default:
	    {
	      cout<<"[both:] mode1_1 not available for all tx schemes "<<endl;
	      abort();
	    }
	  }
	}
	
	else if(nPorts==8)
	{
	  switch(txScheme)
	  {
	    case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
	    case _TRANSMISSION_SCHEME_COMP_:
	    case _TRANSMISSION_SCHEME_NR_CL_:
	    {
	      if(downlinkCQIInfo->cqiComputationInfo.submode==_SUBMODE_2_)
	      {
		codeBook=mCodebook->getSubSampledCodeBookForsubmode2(nLayers);
		sinrPerCodewordForWidebandCQItemp = computeSinrPerCWForCodeBook(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,1,1,codeBook);
	      }
	      else if(downlinkCQIInfo->cqiComputationInfo.submode==_SUBMODE_1_)
	      {
		if(perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1==-1)
		{
		  cout<<"[both:] WideBandPrecoderIndi1 should have been computed.. "<<endl;abort();
		}
		codeBook=getCodeBookForSpecifiedi1(nLayers,perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1);

		sinrPerCodewordForWidebandCQItemp = computeSinrPerCWForCodeBook(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,1,1,codeBook);

	      }
	      else
	      {
		cout<<"[both:] cqiComputationInfo.submode should be either _SUBMODE_1_ or _SUBMODE_2_ for _PERIODIC_MODE_1_1_"<<endl;abort();
	      }
	      break;
	    }
	    default:
	    {
	      cout<<"[both:] mode1_1 not available for all tx schemes "<<endl;
	      abort();
	    }
	  }
	}
	
	vec SpectralEfficiencyOverPMIs=zeros(codeBook.length())-1;
	Array<ivec> tempCQIperCodewords(codeBook.length());
	Array<vec> MMIBvaluesPermodscheme(codeBook.length());
	Array<ivec> modOrderPerCWPerPMI(codeBook.length());
	Array<vec> effSINRsPerCWPerPMI(codeBook.length());
	
	for(int precIndex=0; precIndex<codeBook.length(); precIndex++)
	{
	  //! identity matrix precoder is restricted from the selection..
	  if(precIndex==0 && nPorts==2 && nLayers==2)
	    continue;
	  tempCQIperCodewords(precIndex)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQItemp(precIndex),MMIBvaluesPermodscheme(precIndex),modOrderPerCWPerPMI(precIndex),effSINRsPerCWPerPMI(precIndex),DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	  SpectralEfficiencyOverPMIs(precIndex)=exp_spec_eff;
	}
	
	tempPMI=getBestPMIindex(SpectralEfficiencyOverPMIs,MMIBvaluesPermodscheme);
	
	if(nPorts<8)
	{
	  perUserFeedback.widebandCqiInfoPerGroupPeriodic.pmiIndex=tempPMI;
	  
	  
	}
	else if((txScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_  || txScheme==_TRANSMISSION_SCHEME_COMP_) && nPorts==8 )
	{
	  ivec PrecIndForSubSampledCB;
	  if( downlinkCQIInfo->cqiComputationInfo.submode==_SUBMODE_2_) 
	    PrecIndForSubSampledCB=mCodebook->CodeBookIndicesForSubMode2(nLayers-1);
	  else if( downlinkCQIInfo->cqiComputationInfo.submode==_SUBMODE_1_)
	    PrecIndForSubSampledCB=getCodeBookIndicesForEightPorts(nLayers,perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1);
	  else
	  {cout<<"[both:] submode not configured"<<endl;abort();}
	  
	  perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1=getLongTermPMIInd(nLayers,PrecIndForSubSampledCB(tempPMI));
	  perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi2=getShortTermPMIInd(nLayers,PrecIndForSubSampledCB(tempPMI));
	  perUserFeedback.widebandCqiInfoPerGroupPeriodic.pmiIndex=PrecIndForSubSampledCB(tempPMI);
	}
	//cout << "i2 :" << perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi2 << endl;
	perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord=tempCQIperCodewords(tempPMI);
	perUserFeedback.widebandCqiInfoPerGroupPeriodic.isValid=true;
	
	if(traceValuePerTraceName["EnableUElogs"]=="true")
	  PrintCQIindices(ServiceNodeID,perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord,modOrderPerCWPerPMI(tempPMI),effSINRsPerCWPerPMI(tempPMI),subframeNumber,WideBandCQIlogsFileName,perUserFeedback.widebandCqiInfoPerGroupPeriodic.pmiIndex);
	
	if(traceValuePerTraceName["EnablePrints"]=="true")
	{
	  if(perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord.length()==1)
	    cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" , Wideband PMI : "<<setw(2)<<perUserFeedback.widebandCqiInfoPerGroupPeriodic.pmiIndex<<" , Wideband CQI : "<<setw(5)<<perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0)<< " , effSINRs : "<<setw(11)<<effSINRsPerCWPerPMI(tempPMI)(0)<<" in subframe "<<setw(4)<<subframeNumber<<endl;
	  else
	    cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" , Wideband PMI : "<<setw(2)<<perUserFeedback.widebandCqiInfoPerGroupPeriodic.pmiIndex<<" , Wideband CQI : "<<setw(2)<<perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0)<<" "<<setw(2)<< perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(1)<< " , effSINRs : "<<setw(5)<<effSINRsPerCWPerPMI(tempPMI)(0)<<" "<<setw(5)<<effSINRsPerCWPerPMI(tempPMI)(1)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
	}
      }
      
      break;
    }
    // case mode1_1 loop ends
	  
    case _PERIODIC_MODE_2_0_:
    {
      if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger)
      {
	int DLRBs=cqiMeasurementConfig.nRBs;
	rbNumber=cqiMeasurementConfig.startRB;
	cout<<"Measuring wideband CQI over RBs : "<<DLRBs<<" from "<<rbNumber<<endl;
	switch(txScheme)
	{
	  /// tx modes 1,2,3,7...8,9
	  case _TRANSMISSION_SCHEME_SINGLE_PORT_CRS_:
	  case _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_:
	  case _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_:
	  case _TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_:
	  case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
	  case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
	  case _TRANSMISSION_SCHEME_NR_CL_:
	  {
	    sinrPerCodewordForWidebandCQI=computeSinrPerCW(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,0,-1,1);
	    break;
	  }
	  default:
	  {
	    cout<<"[both:] mode2_0 not available for all tx schemes "<<endl;
	    abort();
	  }
	}
	
	vec MMIBvaluesPermodscheme;
	perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQI,MMIBvaluesPermodscheme,modOrderPerCW,effSINRsPerCW,DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	perUserFeedback.widebandCqiInfoPerGroupPeriodic.isValid=true;
	
	if(traceValuePerTraceName["EnableUElogs"]=="true")
	  PrintCQIindices(ServiceNodeID,perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord,modOrderPerCW,effSINRsPerCW,subframeNumber,WideBandCQIlogsFileName);
	
	if(traceValuePerTraceName["EnablePrints"]=="true")
	{
	  if(perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord.length()==1)
	    cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" , Wideband CQI : "<<setw(5)<<perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0)<< " , effSINRs : "<<setw(11)<<effSINRsPerCW(0)<<" in subframe "<<setw(4)<<subframeNumber<<endl;
	  else
	    cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" , Wideband CQI : "<<setw(2)<<perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0)<<" "<<setw(2)<< perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(1)<< " , effSINRs : "<<setw(5)<<effSINRsPerCW(0)<<" "<<setw(5)<<effSINRsPerCW(1)<<" in subframe "<<setw(5)<<subframeNumber<<endl;
	}
      }
      else if(downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger&&perUserFeedback.widebandCqiInfoPerGroupPeriodic.isValid)
      {
	ivec subbandSize=downlinkCQIInfo->cqiComputationInfo.subbandSizePeriodic(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1); // subbband size for the present BW Part
	perUserFeedback.subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1).set_size(nCodewords);// <nBWParts<nCodeword>>
	Array< Array<vec> > sinrPerCodewordForSubbandCQItemp;
	sinrPerCodewordForSubbandCQItemp.set_length(subbandSize.length());
	int subbandOffset=0;int tempSubbandCnt=0;
	
	for(int Cnt=0;Cnt<downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1;Cnt++)
	  subbandOffset+=sum(downlinkCQIInfo->cqiComputationInfo.subbandSizePeriodic(Cnt));
	
	
	switch(txScheme)
      {
	/// tx modes 1,2,3,7...,8,9
	case _TRANSMISSION_SCHEME_SINGLE_PORT_CRS_:
	case _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_:
	case _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_:
	case _TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_:
	case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
	case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
	case _TRANSMISSION_SCHEME_NR_CL_:
	{
	  for(int subbandCnt=0;subbandCnt<subbandSize.length();subbandCnt++)
	  {
	    rbNumber=subbandOffset+subbandCnt*subbandSize(subbandCnt);
	    
	      sinrPerCodewordForSubbandCQItemp(subbandCnt)=computeSinrPerCW(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,subbandSize(subbandCnt),0,-1,1);
	  }
	  break;
	}

	default:{
	  cout<<"[both:] mode2_0 not available for all tx schemes "<<endl;
	  abort();
	}
      }
	
	vec SpectralEfficiencyOverSubbands=zeros(subbandSize.length());
	Array<ivec> CQIforAllSubbands(subbandSize.length());
	Array<vec> MMIBvaluesPermodscheme(subbandSize.length());
	Array<ivec> modOrderPerCWPerSubband(subbandSize.length());
	Array<vec> effSINRsPerCWPerSubband(subbandSize.length());
	
	for(int Cnt=0; Cnt<subbandSize.length();Cnt++)
	{
	  CQIforAllSubbands(Cnt)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForSubbandCQItemp(Cnt),MMIBvaluesPermodscheme(Cnt),modOrderPerCWPerSubband(Cnt),effSINRsPerCWPerSubband(Cnt),subbandSize(Cnt),nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	  SpectralEfficiencyOverSubbands(Cnt)=exp_spec_eff;
	}
	
	ivec possibleSubbandIndices = find(SpectralEfficiencyOverSubbands,max(SpectralEfficiencyOverSubbands),true);
	
	if(possibleSubbandIndices.length()==1)
	  tempSubbandCnt=possibleSubbandIndices(0);
	else
	{
	  Array<vec> MMIBvaluesPermodschemeTemp = getFromArray(MMIBvaluesPermodscheme,possibleSubbandIndices);
	  // For UE selected subband also the PMIselect function is used
	  tempSubbandCnt=PMIselect(possibleSubbandIndices,MMIBvaluesPermodschemeTemp);
	}
	
	perUserFeedback.subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)=CQIforAllSubbands(tempSubbandCnt);
	perUserFeedback.subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)=tempSubbandCnt;// best subband in each BWPart
	perUserFeedback.subbandCQIInfoPerGroupBWPart.isValidPerBWPart(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)=true;
	
	if(traceValuePerTraceName["EnableUElogs"]=="true")
	  PrintCQIindices(ServiceNodeID,CQIforAllSubbands,subframeNumber,SubbandCQIlogsFileName,downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger);
	
	if(traceValuePerTraceName["EnablePrints"]=="true")
	{
	  if(perUserFeedback.subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1).length()==1)
	    cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" , BWPart " <<setw(3)<<downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1<<" Subband CQI : "<<setw(5)<<perUserFeedback.subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)(0)<< " , effSINRs : "<<setw(11)<<effSINRsPerCWPerSubband(tempSubbandCnt)(0)<<" in subframe "<<setw(4)<<subframeNumber<<endl;
	  else
	    cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" , BWPart " <<setw(3)<<downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1<<" Subband CQI : "<<setw(2)<<perUserFeedback.subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)(0)<<" "<<setw(2)<<perUserFeedback.subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)(1)<< " , effSINRs : "<<setw(5)<<effSINRsPerCWPerSubband(tempSubbandCnt)(0)<<" "<<setw(5)<<effSINRsPerCWPerSubband(tempSubbandCnt)(1)<<" in subframe "<<setw(4)<<subframeNumber<<endl;
	}
      }
      break;
    }
    // mode_2_0 ends
    case _PERIODIC_MODE_2_1_:
    {
      if(downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger)
      {
	if((txScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_ || txScheme==_TRANSMISSION_SCHEME_COMP_) && nPorts==8)// removing PTI check
	{
	  Array< Array<vec> > sinrPerCodewordForWidebandCQItemp; //CQI1 <nPrecoder<nCodeword<nLayer>>>
	  int DLRBs=cqiMeasurementConfig.nRBs;
	  rbNumber=cqiMeasurementConfig.startRB;
	  cout<<"Measuring wideband CQI over RBs : "<<DLRBs<<" from "<<rbNumber<<endl;
	  
	  rbNumber=0;
	  codeBook=mCodebook->getCodeBookToSelecti1(nLayers);
	  
	  sinrPerCodewordForWidebandCQItemp = computeSinrPerCWForCodeBook(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,1,1,codeBook);
	  
	  vec SpectralEfficiencyOverPMIs=zeros(codeBook.length());
	  Array<ivec> tempCQIperCodewords(codeBook.length());
	  Array<vec> MMIBvaluesPermodscheme(codeBook.length());
	  Array<ivec> modOrderPerCWPerPMI(codeBook.length());
	  Array<vec> effSINRsPerCWPerPMI(codeBook.length());
	  
	  for(int precIndex=0; precIndex<codeBook.length(); precIndex++)
	  {
	    tempCQIperCodewords(precIndex)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQItemp(precIndex),MMIBvaluesPermodscheme(precIndex),modOrderPerCWPerPMI(precIndex),effSINRsPerCWPerPMI(precIndex),DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	    SpectralEfficiencyOverPMIs(precIndex)=exp_spec_eff;
	  }
	  tempPMI=getBestPMIindex(SpectralEfficiencyOverPMIs,MMIBvaluesPermodscheme);
	  ivec precInd=mCodebook->CodeBookIndicesToSelecti1(nLayers-1);
	  int tempi1;
	  tempi1=perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1;
	  perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1=getLongTermPMIInd(nLayers,precInd(tempPMI));
	  if(subframeNumber==0)
	    downlinkCQIInfo->cqiComputationInfo.i1diffcounter=0;
	  else if(tempi1!=perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1)
	    downlinkCQIInfo->cqiComputationInfo.i1diffcounter++;
	}
      }
      
      if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger)
      {
	Array< Array<vec> > sinrPerCodewordForWidebandCQItemp; // <nPrecoder<nCodeword<nLayer>>>
	int DLRBs=resourceMapper->dlResourceMapper.getResourceBlocksPerSubframe();
	
	rbNumber=0;
	if(nPorts<8)
	{
	  switch(txScheme)
	  {
	    /// tx modes 4,5,6,8,9
	    case _TRANSMISSION_SCHEME_CL_SM_:
	    case _TRANSMISSION_SCHEME_CL_MU_MIMO_:
	    case _TRANSMISSION_SCHEME_CL_BF_:
	    case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
	    case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
	    case _TRANSMISSION_SCHEME_COMP_:
	    case _TRANSMISSION_SCHEME_NR_CL_:
	    {
	      codeBook=mCodebook->getCodebook(nPorts,nLayers,1);
	      sinrPerCodewordForWidebandCQItemp=computeSinrPerCWForCodeBook(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,1,1,codeBook);
	      break;
	    }
	    default:
	    {
	      cout<<"[both:] mode2_1 not available for all tx schemes "<<endl;
	      abort();
	    }
	  }
	}
	else if((txScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_ || txScheme==_TRANSMISSION_SCHEME_COMP_) && nPorts==8)
	{
	  codeBook=getCodeBookForSpecifiedi1(nLayers,perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1);
	  sinrPerCodewordForWidebandCQItemp = computeSinrPerCWForCodeBook(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,1,1,codeBook);
	}
	else
	{
	  cout<<"[both:] Invalid number of ports"<<nPorts<<endl;abort();
	}
	
	int codebookLength = codeBook.length();
	vec SpectralEfficiencyOverPMIs=zeros(codebookLength)-1;
	Array<ivec> tempCQIperCodewords(codebookLength);
	Array<vec> MMIBvaluesPermodscheme(codebookLength);
	Array<ivec> modOrderPerCWPerPMI(codebookLength);
	Array<vec> effSINRsPerCWPerPMI(codebookLength);
	
	for(int precIndex=0; precIndex<codebookLength; precIndex++)
	{
	  //! identity matrix precoder is restricted from the selection..
	  if(precIndex==0 && nPorts==2 && nLayers==2)
	    continue;
	  tempCQIperCodewords(precIndex)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQItemp(precIndex),MMIBvaluesPermodscheme(precIndex),modOrderPerCWPerPMI(precIndex),effSINRsPerCWPerPMI(precIndex),DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	  SpectralEfficiencyOverPMIs(precIndex)=exp_spec_eff;
	}
	
	tempPMI = getBestPMIindex(SpectralEfficiencyOverPMIs,MMIBvaluesPermodscheme);
	
	perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord=tempCQIperCodewords(tempPMI);
	if(nPorts<8)
	  perUserFeedback.widebandCqiInfoPerGroupPeriodic.pmiIndex=tempPMI;
	else
	{
	  ivec precInd=getCodeBookIndicesForEightPorts(nLayers,perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1);
	  perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi2=getShortTermPMIInd(nLayers,precInd(tempPMI));
	  perUserFeedback.widebandCqiInfoPerGroupPeriodic.pmiIndex=precInd(tempPMI);	    
	}
	perUserFeedback.widebandCqiInfoPerGroupPeriodic.isValid=true;

	if(traceValuePerTraceName["EnableUElogs"]=="true")
	  PrintCQIindices(ServiceNodeID,perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord,modOrderPerCWPerPMI(tempPMI),effSINRsPerCWPerPMI(tempPMI),subframeNumber,WideBandCQIlogsFileName,perUserFeedback.widebandCqiInfoPerGroupPeriodic.pmiIndex);
	
	if(traceValuePerTraceName["EnablePrints"]=="true")
	  cout<<"[detl:]Wideband CQI for CQI Group "<<0<<" : " <<perUserFeedback.widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord<<" Wideband PMI : "<< perUserFeedback.widebandCqiInfoPerGroupPeriodic.pmiIndex<<endl;
	
	break;
      }
      
      else if(downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger&&perUserFeedback.widebandCqiInfoPerGroupPeriodic.isValid)
      {
	cout << "..........computing subband CQI............." << endl;
	/// temp
	ivec subbandSize=downlinkCQIInfo->cqiComputationInfo.subbandSizePeriodic(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1);
	perUserFeedback.subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1).set_size(nCodewords);// <BWPart<nCodeword>>
	int subbandOffset=0;int tempSubbandCnt=0;
	for(int Cnt=0;Cnt<downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1;Cnt++)
	  subbandOffset+=sum(downlinkCQIInfo->cqiComputationInfo.subbandSizePeriodic(Cnt));
	
	if(nPorts<8)
	{
	  Array< Array<vec> > sinrPerCodewordForSubbandCQItemp;
	  sinrPerCodewordForSubbandCQItemp.set_length(subbandSize.length());
	  codeBook=mCodebook->getCodebook(nPorts,nLayers,1);
	  if(perUserFeedback.widebandCqiInfoPerGroupPeriodic.isValid)
	  {
	    precoder=codeBook(perUserFeedback.widebandCqiInfoPerGroupPeriodic.pmiIndex);
	  }
	  else
	  {
	    precoder=codeBook(0);//FIXME Why?
	  }
	  
	  switch(txScheme)
	  {
	    /// tx modes 4,5,6,8,9
	    case _TRANSMISSION_SCHEME_CL_SM_:
	    case _TRANSMISSION_SCHEME_CL_MU_MIMO_:
	    case _TRANSMISSION_SCHEME_CL_BF_:
	    case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
	    case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
	    case _TRANSMISSION_SCHEME_COMP_:
	     case _TRANSMISSION_SCHEME_NR_CL_:
	    {
	      for(int subbandCnt=0;subbandCnt<subbandSize.length();subbandCnt++)
	      {
		rbNumber=subbandOffset+sum(subbandSize.left(subbandCnt));
		sinrPerCodewordForSubbandCQItemp(subbandCnt)=computeSinrPerCW(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,subbandSize(subbandCnt),1,-1,1,precoder);
	      }
	      break;
	    }
	    default:
	    {
	      cout<<"[both:] mode2_1 not available for all tx schemes "<<endl;
	      abort();
	    }
	  }
  
	  vec SpectralEfficiencyOverSubbands=zeros(subbandSize.length());
	  Array<ivec> CQIforAllSubbands(subbandSize.length());
	  Array<vec> MMIBvaluesPermodscheme(subbandSize.length());
	  Array<ivec> modOrderPerCWPerSubband(subbandSize.length());
	  Array<vec> effSINRsPerCWPerSubband(subbandSize.length());
	  
	  for(int Cnt=0; Cnt<subbandSize.length();Cnt++)
	  {
	    CQIforAllSubbands(Cnt)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForSubbandCQItemp(Cnt),MMIBvaluesPermodscheme(Cnt),modOrderPerCWPerSubband(Cnt),effSINRsPerCWPerSubband(Cnt),subbandSize(Cnt),nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	    SpectralEfficiencyOverSubbands(Cnt)=exp_spec_eff;
	  }
	  
	  tempSubbandCnt = getBestPMIindex(SpectralEfficiencyOverSubbands,MMIBvaluesPermodscheme);

	  perUserFeedback.subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)=CQIforAllSubbands(tempSubbandCnt);
	  perUserFeedback.subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)=tempSubbandCnt;
	  perUserFeedback.subbandCQIInfoPerGroupBWPart.isValidPerBWPart(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)=true;
	  
	  if(traceValuePerTraceName["EnableUElogs"]=="true")
	    PrintCQIindices(ServiceNodeID,CQIforAllSubbands,subframeNumber,SubbandCQIlogsFileName,downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger);
	  
	  if(traceValuePerTraceName["EnablePrints"]=="true")
	  {
	    if(perUserFeedback.subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1).length()==1)
	      cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" , BWPart " <<setw(3)<<downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1<<" Subband CQI : "<<setw(5)<<perUserFeedback.subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)(0)<< " , effSINRs : "<<setw(11)<<effSINRsPerCWPerSubband(tempSubbandCnt)(0)<<" in subframe "<<setw(4)<<subframeNumber<<endl;
	    else
	      cout<<"[detl:]Feedback for UE "<<setw(3)<<ServiceNodeID<<" , BWPart " <<setw(3)<<downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1<<" Subband CQI : "<<setw(2)<<perUserFeedback.subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)(0)<<" "<<setw(2)<<perUserFeedback.subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)(1)<< " , effSINRs : "<<setw(5)<<effSINRsPerCWPerSubband(tempSubbandCnt)(0)<<" "<<setw(5)<<effSINRsPerCWPerSubband(tempSubbandCnt)(1)<<" in subframe "<<setw(4)<<subframeNumber<<endl;
	  }
	}
	
	else if(nPorts==8 && downlinkCQIInfo->perUserFeedback.PTI==1 && (txScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_  || txScheme==_TRANSMISSION_SCHEME_COMP_))
	{
	  Array< Array< Array<vec> > > sinrPerCodewordForSubbandCQItemp;//CQI1 <precoder<Subband<nCodeword<nLayer>>>>    
	  ivec tempSubbandCnt;int tempPMIInd;
	  ivec CodeBookIndices;
	  if(perUserFeedback.widebandCqiInfoPerGroupPeriodic.isValid)
	  {
	    codeBook=getSubSampledCodeBookForMode2_1(nLayers,perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1);
	    CodeBookIndices=getCodeBookIndicesForMode2_1(nLayers,perUserFeedback.widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1);
	    sinrPerCodewordForSubbandCQItemp.set_length(codeBook.length());
	    tempSubbandCnt.set_length(codeBook.length());
	    for(int count=0;count<codeBook.length();count++)
	      sinrPerCodewordForSubbandCQItemp(count).set_length(subbandSize.length());	  	      
	  }	  
	  else
	  {
	    codeBook=getSubSampledCodeBookForMode2_1(nLayers,0);
	    CodeBookIndices=getCodeBookIndicesForMode2_1(nLayers,0);
	    sinrPerCodewordForSubbandCQItemp.set_length(codeBook.length());
	    tempSubbandCnt.set_length(codeBook.length());
	    for(int count=0;count<codeBook.length();count++)
	      sinrPerCodewordForSubbandCQItemp(count).set_length(subbandSize.length());	     	      
	  }
	  for(int precInd=0;precInd<codeBook.length();precInd++)
	  {
	    for(int subbandCnt=0;subbandCnt<subbandSize.length();subbandCnt++)
	    {		  
	      rbNumber=subbandOffset+subbandCnt*subbandSize(subbandCnt);	
	      sinrPerCodewordForSubbandCQItemp(precInd)(subbandCnt)=computeSinrPerCW(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,subbandSize(subbandCnt),1,-1,1,codeBook(precInd));
	    }
	  }
	  
	  Array<vec > SpectralEfficiencyOverSubbandsAndPrecoder(codeBook.length());//=zeros(subbandSize.length());
	  Array<Array<ivec> > CQIforAllSubbandsPerPrecoder(codeBook.length());//(subbandSize.length());
	  Array<Array<vec> > MMIBvaluesPermodscheme(codeBook.length());//(subbandSize.length());
	  Array<Array<ivec> > modOrderPerCWPerPrecPerSubband(codeBook.length());//(subbandSize.length());
	  Array<Array<vec> > effSINRsPerCWPerSubbandPerPrec(codeBook.length());//(subbandSize.length());
	  for(int precInd=0;precInd<codeBook.length();precInd++)
	  {
	    SpectralEfficiencyOverSubbandsAndPrecoder(precInd)=zeros(subbandSize.length());
	    CQIforAllSubbandsPerPrecoder(precInd).set_length(subbandSize.length());
	    MMIBvaluesPermodscheme(precInd).set_length(subbandSize.length());
	    modOrderPerCWPerPrecPerSubband(precInd).set_length(subbandSize.length());
	    effSINRsPerCWPerSubbandPerPrec(precInd).set_length(subbandSize.length());	      
	    for(int Cnt=0; Cnt<subbandSize.length();Cnt++)
	    {
	      CQIforAllSubbandsPerPrecoder(precInd)(Cnt)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForSubbandCQItemp(precInd)(Cnt),MMIBvaluesPermodscheme(precInd)(Cnt),modOrderPerCWPerPrecPerSubband(precInd)(Cnt),effSINRsPerCWPerSubbandPerPrec(precInd)(Cnt),subbandSize(Cnt),nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	      SpectralEfficiencyOverSubbandsAndPrecoder(precInd)(Cnt)=exp_spec_eff;
	    }
	    tempSubbandCnt(precInd)=getBestPMIindex(SpectralEfficiencyOverSubbandsAndPrecoder(precInd),MMIBvaluesPermodscheme(precInd));
	    
	  }
	  
	  vec SpectralEfficiencytemp(codeBook.length());
	  Array<ivec> CQItemp(codeBook.length());
	  Array<vec > MMIBperModSchemestemp(codeBook.length());
	  for(int cnt=0;cnt<codeBook.length();cnt++)
	  {
	    SpectralEfficiencytemp(cnt)=SpectralEfficiencyOverSubbandsAndPrecoder(cnt)(tempSubbandCnt(cnt));
	    CQItemp(cnt)=CQIforAllSubbandsPerPrecoder(cnt)(tempSubbandCnt(cnt));
	    MMIBperModSchemestemp(cnt)=MMIBvaluesPermodscheme(cnt)(tempSubbandCnt(cnt));
	  }
	  tempPMIInd=getBestPMIindex(SpectralEfficiencytemp, MMIBperModSchemestemp);
	  
	  
	  perUserFeedback.subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)=CQItemp(tempPMIInd);	     
	  perUserFeedback.subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)=tempSubbandCnt(tempPMIInd);
	  perUserFeedback.subbandCQIInfoPerGroupBWPart.subbandPrecoderIndi2(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)=getShortTermPMIInd(nLayers,CodeBookIndices(tempPMIInd));
	  perUserFeedback.subbandCQIInfoPerGroupBWPart.PMI(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)=tempPMIInd;	       
	  perUserFeedback.subbandCQIInfoPerGroupBWPart.isValidPerBWPart(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)=true;
	  
	}

      }
      
      break;
    }
    default :
    {
      cout<<" [both:] not ready for other periodic cases"<<endl;abort();
    }
  }

  perUserFeedback.isValid=true;
  
}

void CQIEstimator::computeCQIFromIdealChannelForDownlinkAperiodicCQIReport(DLCQIMeasurement_S &perUserFeedback, FrameStructure& frameStructure,DemodulationScheme_E demodulationScheme,PortToAntennaMapper_S *portToAntennaMapper,ReferenceSignal_S *refSignal,ResourceMapper *resourceMapper,Array<cmat> &rxFrame,Array< Array< Array<cmat> > > &allChannels,double myTxSCPowerInDB,int desiredTxNode,ivec txNodeIDSForChannel,CQIMeasurementConfig_S &cqiMeasurementConfig,double noiseVariance, double restOfInterferenceInDB,int nodeID,int subframeNumber, string AperiodicCQILogsFileName)
{
 
  TransmissionScheme_E txScheme=cqiMeasurementConfig.transmissionScheme;
  int nLayers=cqiMeasurementConfig.numberOfLayers;
  ivec cqiPorts=cqiMeasurementConfig.cqiPorts;
  int nCodewords=cqiMeasurementConfig.numberOfCodeWords;
  
  Array<vec> sinrPerCodewordForSubbandCQI(nCodewords);
  Array<vec> sinrPerCodewordForWidebandCQI(nCodewords);

  double mmib_sxfr, mmib_256=0,mmib_sxtn, mmib_qpsk, exp_spec_eff;
  int rbNumber;

  int DLRBs=resourceMapper->dlResourceMapper.getResourceBlocksPerSubframe();
  int nSubbands = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length();

  /// New for aperiodic CQI
  int desiredTxNodeIndx = find(txNodeIDSForChannel,desiredTxNode);

  if(desiredTxNodeIndx == -1)
  {
    cout<<"[detl:] desiredTxNode is not available in txNodeIDSForChannel, Aborting in computeCQIFromIdealChannelForDownlinkAperiodicCQIReport()"<<endl;
    cout<<"desiredTxNode : "<<desiredTxNode<<endl;
    cout<<"txNodeIDSForChannel : "<<txNodeIDSForChannel<<endl;
    abort();
  }

  
  ivec portLocations=getPortLocationsInRB(frameStructure, cqiPorts,0,true); // assuming ports are available in all RBs and taking locations from Oth RB
  int reCountPerRb=portLocations.length();
  int nPorts=cqiPorts.length();

  Array<cmat> codeBook;
  int codebookLength=-1;
  ivec precoderInd;
  
  if(downlinkCQIInfo->cqiComputationInfo.aperiodicMode == _APERIODIC_MODE_1_2_ || downlinkCQIInfo->cqiComputationInfo.aperiodicMode == _APERIODIC_MODE_3_1_ || downlinkCQIInfo->cqiComputationInfo.aperiodicMode == _APERIODIC_MODE_2_2_)
  {
    codeBook = mCodebook->getCodebook(nPorts,nLayers,1);
    codebookLength=codeBook.length();
    precoderInd=getIntegers(0,codebookLength-1);
    if(nPorts==8)
    {
      codebookLength=mCodebook->unique8portCodebookIndices(nLayers-1).length();
      precoderInd = mCodebook->unique8portCodebookIndices(nLayers-1);
    }
  }


  int tempPMI=-1;
  cmat precoder;

//   int nInterferers = interfererIndices.length();
//   vec interfererTxSCPowerInDB(nInterferers);
//   Array<Framehandler*> interferingTxFrameHandlers(nInterferers);
// 
//   for(int intr_cnt=0;intr_cnt<nInterferers;intr_cnt++)
//   {
//     interfererTxSCPowerInDB(intr_cnt)=txSubcarrierPowerInDB(interfererIndices(intr_cnt));
//     interferingTxFrameHandlers(intr_cnt)=TxFrameHandlers(interfererIndices(intr_cnt));
//   }

  // obtaining the channel matrix for a node under consideration
  Array< Array<cmat> > myChannel=allChannels(desiredTxNodeIndx);

  Array<string> rbName(cqiPorts.length());
  rbName.set_length(cqiPorts.length());
  for(int port_cnt=0;port_cnt<cqiPorts.length();port_cnt++)
    rbName(port_cnt)="port"+toString(cqiPorts(port_cnt));
  imat rsRBStructure;
  rsRBStructure=frameStructure.getMergedResourceUnit("nullRB",rbName,"data",cqiMeasurementConfig.rsSymPositions,cqiMeasurementConfig.rsScPositions);
  

  rsRBStructure = rsRBStructure.get_cols(3,rsRBStructure.cols()-1);
  ivec dataTones = find(rsRBStructure,0,true);
  int nREsPerRB = dataTones.length();
  switch(downlinkCQIInfo->cqiComputationInfo.aperiodicMode)
  {
    case _APERIODIC_MODE_1_2_:
    // In this mode subband level precoders are derermined and wideband CQI is computed.
    // The subbband level precoder is determined by computing the sinr in each band and mapping to MMIB for all the precoder and the one
    // which gives better CQI is selected for that band. This is done for all the subbands.
    {
      /// Temp Variables
      Array<vec> sinrPerCodewordForSubbandCQItemp; // <codeword<layer>>
      sinrPerCodewordForSubbandCQItemp.set_length(nCodewords);
      sinrPerCodewordForSubbandCQI.set_length(nCodewords);
      sinrPerCodewordForWidebandCQI.set_length(nCodewords);

      perUserFeedback.widebandCqiInfoPerGroupAperiodic.subbandPMI = zeros_i(nSubbands);

      for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
      {
	rbNumber=subbandCnt*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(0);
	switch(txScheme)
	{
	  /// tx modes 4,6,8,9
	  case _TRANSMISSION_SCHEME_CL_SM_:
	  case _TRANSMISSION_SCHEME_CL_BF_:
	  case _TRANSMISSION_SCHEME_CL_MU_MIMO_:
	  case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
	  case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
	  case _TRANSMISSION_SCHEME_NR_CL_:
	  {
	    sinrPerCodewordForSubbandCQItemp.set_size(nCodewords);// repeated
	    sinrPerCodewordForSubbandCQI.set_size(nCodewords);

	    vec SpectralEfficiencyOverPMIs=zeros(codebookLength)-1;
	    Array<ivec> tempCQIperCodewords(codebookLength);
	    Array<vec> MMIBvaluesPermodscheme(codebookLength);
	    Array<ivec> modOrderPerCWPerPMI(codebookLength);
	    Array<vec> effSINRsPerCWPerPMI(codebookLength);

	    for(int precIndex=0; precIndex<codebookLength; precIndex++)
	    {
	      if(precIndex==0 && nPorts==2 && nLayers==2)
		continue;
	      precoder = codeBook(precoderInd(precIndex));

	      sinrPerCodewordForSubbandCQItemp=computeSinrPerCW(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(subbandCnt),1,-1,1,precoder);
	      tempCQIperCodewords(precIndex)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForSubbandCQItemp,MMIBvaluesPermodscheme(precIndex),modOrderPerCWPerPMI(precIndex),effSINRsPerCWPerPMI(precIndex),downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(subbandCnt),nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	      SpectralEfficiencyOverPMIs(precIndex)=exp_spec_eff;

	    }

	    int tPMI = getBestPMIindex(SpectralEfficiencyOverPMIs,MMIBvaluesPermodscheme);

	    tempPMI=precoderInd(tPMI);

	    perUserFeedback.widebandCqiInfoPerGroupAperiodic.subbandPMI(subbandCnt)=tempPMI;
	    //index of  precoder,  which provides a better CQI is  stored for each subband
	    sinrPerCodewordForSubbandCQI=computeSinrPerCW(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(subbandCnt),1,-1,1,codeBook(tempPMI));
	    concatInArray(sinrPerCodewordForWidebandCQI,sinrPerCodewordForSubbandCQI);

	  }
	  break;
	  default:
	  {
	    cout<<"[both:] mode1_2 not available for all tx schemes "<<endl;
	    cout<<txScheme<<endl;
	    abort();
	  }
	}

      }
      // to determine the CQI index by mapping wideband sinr to MMIB domain
      vec MMIBvaluesPermodscheme;
      ivec modOrderPerCW;
      vec effSINRsPerCW;
      perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQI,MMIBvaluesPermodscheme,modOrderPerCW,effSINRsPerCW,DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
      perUserFeedback.widebandCqiInfoPerGroupAperiodic.isValid=true;
      Array<ivec> dummy;
      if(traceValuePerTraceName["EnableUElogs"]=="true")
	PrintCQIindices(downlinkCQIInfo->cqiComputationInfo.aperiodicMode,nodeID,perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord,modOrderPerCW,effSINRsPerCW, subframeNumber,AperiodicCQILogsFileName,-1,dummy,  perUserFeedback.widebandCqiInfoPerGroupAperiodic.subbandPMI);

    }
    break;
    case _APERIODIC_MODE_3_0_:
    // In this mode no PMI transmitted. Wideband and subband level CQI is computed.
    {
      /// Temp Variables
      rbNumber = 0;
      Array<vec> sinrPerCodewordTemp;	// <subband<pmi<codewords<REs>>>>

      if(txScheme==_TRANSMISSION_SCHEME_SINGLE_PORT_CRS_ || txScheme == _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_ || txScheme == _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_ || txScheme == _TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_ || txScheme == _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ || txScheme == _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme == _TRANSMISSION_SCHEME_NR_CL_)
      {
	if(sinrOverWideband.length() == 0)
	{
	  sinrOverWideband.set_length(1);
	  sinrOverWideband(0) = computeSinrPerCW(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,0,-1,1);   // <pmi<codewords<REs>>>>
	}
      }
      else
      {
	cout<<"[both:] mode 3_0 not available for all tx schemes "<<endl;
	abort();
      }

      vec MMIBvaluesPermodscheme;
      ivec modOrderPerCW;
      vec effSINRsPerCW;

      perUserFeedback.subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord.set_length(nSubbands);// <subband<nCodeword>>>
      perUserFeedback.subbandCqiInfoPerGroupHigherLayer.subbandIndex.set_length(nSubbands);// <nSubband>

      int repetitionFactor = 1;
      if(txScheme == _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_ || txScheme == _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_)
	repetitionFactor = nLayers;

      for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
      {
	sinrPerCodewordTemp.set_length(nCodewords);
	int start = subbandCnt*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(0)*reCountPerRb*repetitionFactor;
	int stop = (subbandCnt+1)*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(0)*reCountPerRb*repetitionFactor-1;

	for(int iCW=0;iCW<nCodewords;iCW++)
	  {
	  if(subbandCnt!=nSubbands-1)
	    sinrPerCodewordTemp(iCW)=sinrOverWideband(0)(iCW)(start,stop);
	  else
	    sinrPerCodewordTemp(iCW)=sinrOverWideband(0)(iCW)(start,sinrOverWideband(0)(iCW).length()-1);
	}

	perUserFeedback.subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(subbandCnt)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordTemp,MMIBvaluesPermodscheme,modOrderPerCW,effSINRsPerCW,downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(subbandCnt),nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	perUserFeedback.subbandCqiInfoPerGroupHigherLayer.subbandIndex(subbandCnt)=subbandCnt;
	    #ifdef USING_CALIBRATION
	    string HLSubbandCQIFilename="./Results/HLSubbandCQIlogs.txt";
	    PrintCQIindices(nodeID,perUserFeedback.subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(subbandCnt),modOrderPerCW,effSINRsPerCW,subframeNumber,HLSubbandCQIFilename,subbandCnt);
	    #endif
	  }

      perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord=getCQIPerCodewords(exp_spec_eff,sinrOverWideband(0),MMIBvaluesPermodscheme,modOrderPerCW,effSINRsPerCW,DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
      perUserFeedback.widebandCqiInfoPerGroupAperiodic.isValid=true;
      #ifdef USING_CALIBRATION
      string HLWidebandCQIFilename="./Results/HLWidebandCQIlogs.txt";

      PrintCQIindices(nodeID,perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord,modOrderPerCW,effSINRsPerCW,subframeNumber,HLWidebandCQIFilename);
      #endif
      if(traceValuePerTraceName["EnableUElogs"]=="true")
          PrintCQIindices(downlinkCQIInfo->cqiComputationInfo.aperiodicMode,nodeID,perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord,modOrderPerCW,effSINRsPerCW, subframeNumber,AperiodicCQILogsFileName,-1,perUserFeedback.subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord,ivec("-1"));
      
      sinrOverWideband.set_length(0);
      break;
    }
    
    case _APERIODIC_MODE_3_1_:
    // In this mode wideband PMI is determined. Wideband and subband level CQI is computed. The Wideband PMI is selected
    //such that the  total wideband CQI over the codewords is max
    {
      /// Temp Variables
      rbNumber = 0;
      Array<vec> sinrPerCodewordTemp;	// <subband<pmi<codewords<REs>>>>

      if(txScheme==_TRANSMISSION_SCHEME_CL_SM_ || txScheme == _TRANSMISSION_SCHEME_CL_BF_ || txScheme == _TRANSMISSION_SCHEME_CL_MU_MIMO_ || txScheme == _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ || txScheme == _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme == _TRANSMISSION_SCHEME_NR_CL_)
      {
          if(sinrOverWideband.length() != codebookLength)
          {
              sinrOverWideband = computeSinrPerPrecodersPerCW(frameStructure, myTxSCPowerInDB/*, interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices*/, myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,1,1);   // <pmi<codewords<REs>>>>
          }
      }
      else
      {
          cout<<"[both:] mode 3_1 not available for all tx schemes "<<endl;
          abort();
      }

      vec SpectralEfficiencyOverPMIs=zeros(codebookLength)-1;
      Array<ivec> tempCQIperCodewords(codebookLength);
      Array<vec> MMIBvaluesPermodscheme(codebookLength);
      Array<ivec> modOrderPerCWPerPMI(codebookLength);
      Array<vec> effSINRsPerCWPerPMI(codebookLength);

      // precoder which maximises the SE in wideband level over both the codewords is selected
      for(int precIndex=0; precIndex<codebookLength; precIndex++)
      {
	//! identity matrix precoder is restricted from the selection..
	if(precIndex==0 && nPorts==2 && nLayers==2)
	  continue;

	tempCQIperCodewords(precIndex)=getCQIPerCodewords(exp_spec_eff,sinrOverWideband(precIndex),MMIBvaluesPermodscheme(precIndex),modOrderPerCWPerPMI(precIndex),effSINRsPerCWPerPMI(precIndex),DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	SpectralEfficiencyOverPMIs(precIndex)=exp_spec_eff;
      }

      int tPMI = getBestPMIindex(SpectralEfficiencyOverPMIs,MMIBvaluesPermodscheme);

      perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord=tempCQIperCodewords(tPMI);
      perUserFeedback.widebandCqiInfoPerGroupAperiodic.widebandPMI=precoderInd(tPMI);
      perUserFeedback.widebandCqiInfoPerGroupAperiodic.isValid=true;

      // computation of CQI for subbands considering the determined precoder
      perUserFeedback.subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord.set_size(nSubbands); // <subband<nCodeword>>
      perUserFeedback.subbandCqiInfoPerGroupHigherLayer.subbandIndex.set_size(nSubbands);

      for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
      {
	sinrPerCodewordTemp.set_length(nCodewords);
	for(int iCW=0;iCW<nCodewords;iCW++)
	{
	  int start = subbandCnt*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(0)*reCountPerRb;
	  int stop = (subbandCnt+1)*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(0)*reCountPerRb-1;

	  if(subbandCnt!=nSubbands-1)
	    sinrPerCodewordTemp(iCW)=sinrOverWideband(tPMI)(iCW)(start,stop);
	  else
	    sinrPerCodewordTemp(iCW)=sinrOverWideband(tPMI)(iCW)(start,sinrOverWideband(tPMI)(iCW).length()-1);
	}

	vec MMIBvaluesPermodschemeTemp,effSINRsPerCWTemp;
	ivec modOrderPerCWTemp;
	perUserFeedback.subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(subbandCnt)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordTemp,MMIBvaluesPermodschemeTemp,modOrderPerCWTemp,effSINRsPerCWTemp,downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(subbandCnt),nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	perUserFeedback.subbandCqiInfoPerGroupHigherLayer.subbandIndex(subbandCnt)=subbandCnt;
      }

      if(traceValuePerTraceName["EnableUElogs"]=="true")
	PrintCQIindices(downlinkCQIInfo->cqiComputationInfo.aperiodicMode,nodeID,perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord,modOrderPerCWPerPMI(tPMI),effSINRsPerCWPerPMI(tPMI), subframeNumber,AperiodicCQILogsFileName,perUserFeedback.widebandCqiInfoPerGroupAperiodic.widebandPMI,perUserFeedback.subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord,ivec("-1"));

    }
    sinrOverWideband.set_length(0);
    break;
  case _APERIODIC_MODE_3_2_:
    {
      Array< Array < Array<vec> > > sinrPerCodewordForSubbandCQItemp;// <subband<pmi<codewords<REs>>>>
      Array<vec>  sinrPerCodewordForWidebandCQItemp;   	   // <codewords<REs>>
      sinrPerCodewordForWidebandCQItemp.set_length(nCodewords);
      
      int DLRBs=resourceMapper->dlResourceMapper.getResourceBlocksPerSubframe();
      rbNumber = 0;
      ivec precIndex_i2;
      if(txScheme==_TRANSMISSION_SCHEME_CL_SM_ || txScheme == _TRANSMISSION_SCHEME_CL_BF_ || txScheme == _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ || txScheme == _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme == _TRANSMISSION_SCHEME_NR_CL_)
      {
	if(sinrOverWideband.length() != codebookLength)
	{
	  sinrOverWideband = computeSinrPerPrecodersPerCW(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,1,1);   // <pmi<codewords<REs>>>>
	}
	if(nPorts==8)//or atlernativecodebookForTx4: need to add
	{
	  codeBook=mCodebook->getCodeBookToSelecti1(nLayers);
	  //cout<<"Test codebook length for complete set:"<<codebookLength<<endl;
	  codebookLength = codeBook.length();
	  //cout<<"Test codebook length for i1:"<<codebookLength<<endl;
	  
	  vec SpectralEfficiencyOverPMIs=zeros(codebookLength)-1;
	  Array<ivec> tempCQIperCodewords(codebookLength);
	  Array<vec> MMIBvaluesPermodscheme(codebookLength);
	  Array<ivec> modOrderPerCWPerPMI(codebookLength);
	  Array<vec> effSINRsPerCWPerPMI(codebookLength);
	  for(int precIndex=0; precIndex<codebookLength; precIndex++)
	  {
	      //! identity matrix precoder is restricted from the selection..
	      if(precIndex==0 && nPorts==2 && nLayers==2)
		  continue;
	      
	      tempCQIperCodewords(precIndex)=getCQIPerCodewords(exp_spec_eff,sinrOverWideband(precIndex),MMIBvaluesPermodscheme(precIndex),modOrderPerCWPerPMI(precIndex),effSINRsPerCWPerPMI(precIndex),DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	      SpectralEfficiencyOverPMIs(precIndex)=exp_spec_eff;
	  }
	  
	  tempPMI = getBestPMIindex(SpectralEfficiencyOverPMIs,MMIBvaluesPermodscheme);

	  ivec PrecIndices=mCodebook->CodeBookIndicesToSelecti1(nLayers-1);
	  perUserFeedback.widebandCqiInfoPerGroupAperiodic.WidebandPrecoderIndi1=getLongTermPMIInd(nLayers,PrecIndices(tempPMI));
	}
       
	
      }
      else
      {
	cout<<"[both:] mode 3_2 not available for all tx schemes "<<endl;
	abort();
      }
      
      if(nPorts==8)//or atlernativecodebookForTx4: need to add
      {
	codeBook=getCodeBookForSpecifiedi1(nLayers,perUserFeedback.widebandCqiInfoPerGroupAperiodic.WidebandPrecoderIndi1);
	codebookLength = codeBook.length();
	precIndex_i2 = getCodeBookIndicesForEightPorts(nLayers,perUserFeedback.widebandCqiInfoPerGroupAperiodic.WidebandPrecoderIndi1);
	
      }
      perUserFeedback.subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord.set_size(nSubbands); // <subband<nCodeword>>
      perUserFeedback.subbandCqiInfoPerGroupHigherLayer.subbandIndex.set_size(nSubbands);
      
      sinrPerCodewordForSubbandCQItemp.set_length(nSubbands);
      
    for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
    {
      sinrPerCodewordForSubbandCQItemp(subbandCnt).set_length(codebookLength);
      
      for(int precIndex=0; precIndex<codebookLength; precIndex++)
      {
	int actualPrecIndex;
	if(precIndex==0 && nPorts==2 && nLayers==2)
	  continue;
	
	if(nPorts ==8)
	{
	  actualPrecIndex = precIndex_i2(precIndex);
	}
	else
	{
	  actualPrecIndex = precIndex;
	}
	
	sinrPerCodewordForSubbandCQItemp(subbandCnt)(precIndex).set_length(nCodewords);
	for(int iCW=0;iCW<nCodewords;iCW++)
	{
	  int start = subbandCnt*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(0)*reCountPerRb;
	  int stop = (subbandCnt+1)*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(0)*reCountPerRb-1;
	  
	  if(subbandCnt!=nSubbands-1)
	    sinrPerCodewordForSubbandCQItemp(subbandCnt)(precIndex)(iCW)= sinrOverWideband(actualPrecIndex)(iCW)(start,stop);
	  else
	    sinrPerCodewordForSubbandCQItemp(subbandCnt)(precIndex)(iCW)= sinrOverWideband(actualPrecIndex)(iCW)(start,sinrOverWideband(actualPrecIndex)(iCW).length()-1);
	}  
      }
    }
      
    Array<vec> SpectralEfficiencyPerSubbandOverPMIs(nSubbands);
    Array< Array <ivec> > tempCQIperCodewords(nSubbands);
    Array< Array <vec> > MMIBvaluesPermodscheme(nSubbands);
    Array< Array <ivec> > modOrderPerCWPerPMI(nSubbands);
    Array< Array <vec> > effSINRsPerCWPerPMI(nSubbands);
    ivec subbandPMI(nSubbands);
    perUserFeedback.subbandCqiInfoPerGroupHigherLayer.subbandPrecoderIndi2.set_length(nSubbands);
    perUserFeedback.widebandCqiInfoPerGroupAperiodic.subbandPMI.set_length(nSubbands);
    // computed CQI per subband per precoder per codeword 
    for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
    {
      SpectralEfficiencyPerSubbandOverPMIs(subbandCnt)=zeros(codebookLength)-1;
      tempCQIperCodewords(subbandCnt).set_length(codebookLength);
      MMIBvaluesPermodscheme(subbandCnt).set_length(codebookLength);
      modOrderPerCWPerPMI(subbandCnt).set_length(codebookLength);
      effSINRsPerCWPerPMI(subbandCnt).set_length(codebookLength);
      
      for(int precIndex=0; precIndex<codebookLength; precIndex++)
      {
	//! identity matrix precoder is restricted from the selection..
	if(precIndex==0 && nPorts==2 && nLayers==2)
	  continue;
	
	tempCQIperCodewords(subbandCnt)(precIndex)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForSubbandCQItemp(subbandCnt)(precIndex),MMIBvaluesPermodscheme(subbandCnt)(precIndex),modOrderPerCWPerPMI(subbandCnt)(precIndex),effSINRsPerCWPerPMI(subbandCnt)(precIndex),downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(subbandCnt),nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	SpectralEfficiencyPerSubbandOverPMIs(subbandCnt)(precIndex)=exp_spec_eff;
      }
      
      int tPMI = getBestPMIindex(SpectralEfficiencyPerSubbandOverPMIs(subbandCnt),MMIBvaluesPermodscheme(subbandCnt));
      if(nPorts ==8)
      {
	subbandPMI(subbandCnt) = precIndex_i2(tPMI);
	perUserFeedback.subbandCqiInfoPerGroupHigherLayer.subbandPrecoderIndi2(subbandCnt) = tPMI;//subbandPMI(subbandCnt);
      }
      else
      {
	subbandPMI(subbandCnt) = tPMI;
	perUserFeedback.widebandCqiInfoPerGroupAperiodic.subbandPMI (subbandCnt)= subbandPMI(subbandCnt);
      }
      
      // concatinating subband sinr based on chosen PMI for respective subband
      concatInArray(sinrPerCodewordForWidebandCQItemp,sinrPerCodewordForSubbandCQItemp(subbandCnt)(tPMI));
      
      // Loading Computed CQI info to respective structures
      perUserFeedback.subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(subbandCnt) = tempCQIperCodewords(subbandCnt)(tPMI); 
      perUserFeedback.subbandCqiInfoPerGroupHigherLayer.subbandIndex(subbandCnt)=subbandCnt;
    }
      
    // saving actual PMI based on i1 and i2 value for each subband
    //This is for L1 purpose with absolute index from 0 to 255
    if(nPorts ==8)
    {
      perUserFeedback.widebandCqiInfoPerGroupAperiodic.subbandPMI = getCodeBookIndicesForEightPorts(nLayers,perUserFeedback.widebandCqiInfoPerGroupAperiodic.WidebandPrecoderIndi1,perUserFeedback.subbandCqiInfoPerGroupHigherLayer.subbandPrecoderIndi2);
    }

    // Calculation of wide band CQI : calculation is done by choosing best PMI per subband
    vec tempMMIBvaluesPermodscheme;
    ivec modOrderPerCW;
    vec effSINRsPerCW;
    ivec tempWidebandCQIperCodewords;
    tempWidebandCQIperCodewords = getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQItemp,tempMMIBvaluesPermodscheme,modOrderPerCW,effSINRsPerCW,DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
    
    // Loading Computed CQI info to respective structures
    
    perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord=tempWidebandCQIperCodewords;
    perUserFeedback.widebandCqiInfoPerGroupAperiodic.isValid=true;
    

    if(traceValuePerTraceName["EnableUElogs"]=="true") 	    
      PrintCQIindices(downlinkCQIInfo->cqiComputationInfo.aperiodicMode,nodeID,perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord,modOrderPerCW,effSINRsPerCW, subframeNumber,AperiodicCQILogsFileName,perUserFeedback.widebandCqiInfoPerGroupAperiodic.widebandPMI,perUserFeedback.subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord, perUserFeedback.widebandCqiInfoPerGroupAperiodic.subbandPMI);
    
    sinrOverWideband.set_length(0);
    break;
  }
// 	case _APERIODIC_MODE_3_2_:
// 	{
// 	  Array<Array<vec> > sinrOverWideband_Fori1;
// 	  Array< Array < Array<vec> > > sinrPerCodewordForSubbandCQItemp;// <subband<pmi<codewords<REs>>>>
// 	  Array<vec>  sinrPerCodewordForWidebandCQItemp;   	   // <codewords<REs>>
// 	  sinrPerCodewordForWidebandCQItemp.set_length(nCodewords);
// 	  
// 	  if(txScheme==_TRANSMISSION_SCHEME_CL_SM_ || txScheme == _TRANSMISSION_SCHEME_CL_BF_ || txScheme == _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ || txScheme == _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || || txScheme == _TRANSMISSION_SCHEME_NR_CL_)
// 	  {
// 	    if(nPorts==8)//or atlernativecodebookForTx4: need to add
// 	    {
// 	      codeBook=getCodeBookToSelecti1(nLayers);
// 	      codebookLength = codeBook.length();
// 	      sinrOverWideband_Fori1 = computeSinrPerPrecodersPerCW(frameStructure, myTxSCPowerInDB, interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices, myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,1,1,codeBook);   // <pmi<codewords<REs>>>>
// 	      
// 	    }
// 	    
// 	    if(!sinrOverWideband.length())
// 	    {
// 	      sinrOverWideband = computeSinrPerPrecodersPerCW(frameStructure, myTxSCPowerInDB, interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices, myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,DLRBs,1,1);   // <pmi<codewords<REs>>>>
// 	    }
// 	  
// 	  }
// 	  else
// 	  {
// 	    cout<<"[both:] mode 3_2 not available for all tx schemes "<<endl;
// 	    abort();
// 	  }
// 	  int DLRBs=resourceMapper->dlResourceMapper.getResourceBlocksPerSubframe();
// 	  vec SpectralEfficiencyOverPMIs=zeros(codebookLength)-1;
// 	  Array<ivec> tempCQIperCodewords(codebookLength);
// 	  Array<vec> MMIBvaluesPermodscheme(codebookLength);
// 	  Array<ivec> modOrderPerCWPerPMI(codebookLength);
// 	  Array<vec> effSINRsPerCWPerPMI(codebookLength);
// 	  for(int precIndex=0; precIndex<codebookLength; precIndex++)
// 	  {
// 	    //! identity matrix precoder is restricted from the selection..
// 	    tempCQIperCodewords(precIndex)=getCQIPerCodewords(exp_spec_eff,sinrOverWideband_Fori1(precIndex),MMIBvaluesPermodscheme(precIndex),modOrderPerCWPerPMI(precIndex),effSINRsPerCWPerPMI(precIndex),DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
// 	    SpectralEfficiencyOverPMIs(precIndex)=exp_spec_eff;
// 	  }
// 	  
// 	  ivec possiblePMIs = find(SpectralEfficiencyOverPMIs,max(SpectralEfficiencyOverPMIs),true);
// 	  if(possiblePMIs.length()==1)
// 	    tempPMI=possiblePMIs(0);
// 	  else
// 	  {
// 	    Array<vec> MMIBvaluesPermodschemeTemp = getFromArray(MMIBvaluesPermodscheme,possiblePMIs);
// 	    tempPMI=PMIselect(possiblePMIs,MMIBvaluesPermodschemeTemp);
// 	  }
// 	  ivec PrecIndices=getCodeBookIndicesToSelecti1(nLayers);
// 	  perUserFeedback.widebandCqiInfoPerGroupAperiodic.WidebandPrecoderIndi1=getLongTermPMIInd(nLayers,PrecIndices(tempPMI));
// 	  
// 	  //
// 	  perUserFeedback.subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord.set_size(nSubbands); // <subband<nCodeword>>
// 	  perUserFeedback.subbandCqiInfoPerGroupHigherLayer.subbandIndex.set_size(nSubbands);
// 	  
// 	  sinrPerCodewordForSubbandCQItemp.set_length(nSubbands);
// 	  
// 	  for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
// 	  {
// 	    sinrPerCodewordForSubbandCQItemp(subbandCnt).set_length(codebookLength);
// 	    
// 	    for(int precIndex=0; precIndex<codebookLength; precIndex++)
// 	    {
// 	      if(precIndex==0 && nPorts==2 && nLayers==2)
// 		continue;
// 	      
// 	      sinrPerCodewordForSubbandCQItemp(subbandCnt)(precIndex).set_length(nCodewords);
// 	      for(int iCW=0;iCW<nCodewords;iCW++)
// 	      {
// 		int start = subbandCnt*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(0)*reCountPerRb;
// 		int stop = (subbandCnt+1)*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(0)*reCountPerRb-1;
// 		
// 		if(subbandCnt!=nSubbands-1)
// 		  sinrPerCodewordForSubbandCQItemp(subbandCnt)(precIndex)(iCW)=sinrOverWideband(precIndex)(iCW)(start,stop);
// 		else
// 		  sinrPerCodewordForSubbandCQItemp(subbandCnt)(precIndex)(iCW)=sinrOverWideband(precIndex)(iCW)(start,sinrOverWideband(precIndex)(iCW).length()-1);
// 	      }  
// 	    }
// 	  }
// 	  
// 	  Array<vec> SpectralEfficiencyPerSubbandOverPMIs(nSubbands);
// 	  Array< Array <ivec> > tempCQIperCodewords(nSubbands);
// 	  Array< Array <vec> > MMIBvaluesPermodscheme(nSubbands);
// 	  Array< Array <ivec> > modOrderPerCWPerPMI(nSubbands);
// 	  Array< Array <vec> > effSINRsPerCWPerPMI(nSubbands);
// 	  ivec subbandPMI(nSubbands);
// 	  
// 	  // computed CQI per subband per precoder per codeword 
// 	  for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
// 	  {
// 	    SpectralEfficiencyPerSubbandOverPMIs(subbandCnt)=zeros(codebookLength)-1;
// 	    tempCQIperCodewords(subbandCnt).set_length(codebookLength);
// 	    MMIBvaluesPermodscheme(subbandCnt).set_length(codebookLength);
// 	    modOrderPerCWPerPMI(subbandCnt).set_length(codebookLength);
// 	    effSINRsPerCWPerPMI(subbandCnt).set_length(codebookLength);
// 	    
// 	    for(int precIndex=0; precIndex<codebookLength; precIndex++)
// 	    {
// 	      //! identity matrix precoder is restricted from the selection..
// 	      if(precIndex==0 && nPorts==2 && nLayers==2)
// 		continue;
// 	      
// 	      tempCQIperCodewords(subbandCnt)(precIndex)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForSubbandCQItemp(subbandCnt)(precIndex),MMIBvaluesPermodscheme(subbandCnt)(precIndex),modOrderPerCWPerPMI(subbandCnt)(precIndex),effSINRsPerCWPerPMI(subbandCnt)(precIndex),downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(subbandCnt),nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
// 	      SpectralEfficiencyPerSubbandOverPMIs(subbandCnt)(precIndex)=exp_spec_eff;
// 	    }
// 	    
// 	    ivec possiblePMIs = find(SpectralEfficiencyPerSubbandOverPMIs(subbandCnt),max(SpectralEfficiencyPerSubbandOverPMIs(subbandCnt)),true);
// 	    
// 	    int tPMI = -1;
// 	    if(possiblePMIs.length()==1)
// 	      tPMI = possiblePMIs(0);
// 	    else
// 	    {
// 	      Array<vec> MMIBvaluesPermodschemeTemp = getFromArray(MMIBvaluesPermodscheme(subbandCnt),possiblePMIs);
// 	      tPMI=PMIselect(possiblePMIs,MMIBvaluesPermodschemeTemp);
// 	    }
// 	    subbandPMI(subbandCnt) = precoderInd(tPMI);
// 	    // concatinating subband sinr based on chosen PMI for respective subband
// 	    concatInArray(sinrPerCodewordForWidebandCQItemp,sinrPerCodewordForSubbandCQItemp(subbandCnt)(tPMI));
// 	    
// 	    // Loading Computed CQI info to respective structures
// 	    perUserFeedback.subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(subbandCnt) = tempCQIperCodewords(subbandCnt)(tPMI); 
// 	    perUserFeedback.subbandCqiInfoPerGroupHigherLayer.subbandIndex(subbandCnt)=subbandCnt;
// 	  }
// 	  // Calculation of wide band CQI : calculation is done by choosing best PMI per subband
// 	  vec tempMMIBvaluesPermodscheme;
// 	  ivec modOrderPerCW;
// 	  vec effSINRsPerCW;
// 	  ivec tempWidebandCQIperCodewords;
// 	  tempWidebandCQIperCodewords = getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQItemp,tempMMIBvaluesPermodscheme,modOrderPerCW,effSINRsPerCW,DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
// 	  
// 	  // Loading Computed CQI info to respective structures
// 	  perUserFeedback.widebandCqiInfoPerGroupAperiodic.subbandPMI = subbandPMI;
// 	  perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord=tempWidebandCQIperCodewords;
// 	  perUserFeedback.widebandCqiInfoPerGroupAperiodic.isValid=true;
// 	  
// 	  if(traceValuePerTraceName["EnableUElogs"]=="true") 	    
// 	    PrintCQIindices(downlinkCQIInfo->cqiComputationInfo.aperiodicMode,nodeID,perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord,modOrderPerCW,effSINRsPerCW, subframeNumber,AperiodicCQILogsFileName,perUserFeedback.widebandCqiInfoPerGroupAperiodic.widebandPMI,perUserFeedback.subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord, perUserFeedback.widebandCqiInfoPerGroupAperiodic.subbandPMI);
// 	  
// 	  sinrOverWideband.set_length(0);
// 	break;
// 	}
    case _APERIODIC_MODE_2_0_:
    // In this mode the wideband CQI is computed and the avg CQI over the UE selected subbands
    //(best M subbands w.r.t. MMIB)
    {
      Array<Array<vec> > sinrPerCodewordForAllSubbandCQI;
      perUserFeedback.subbandCqiInfoPerGroupUESelected.subbandIndex.set_size(downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands);
      perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord.set_size(nCodewords); // <ncodeword>
      perUserFeedback.subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord.set_size(nCodewords); // <ncodeword>
      Array<ivec> tempCQIIndex; // <subband<nCodeword>>
      vec tempRate = zeros(downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected.length());
      ivec subbandIndices;
      tempCQIIndex.set_length(downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected.length());
      subbandIndices.set_length(downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected.length());
      sinrPerCodewordForAllSubbandCQI.set_size(downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected.length());
      for(int subbandCnt=0;subbandCnt<downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected.length();subbandCnt++)
      {
	rbNumber=subbandCnt*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected(subbandCnt);

	switch(txScheme)
	{
	  /// tx modes 1,2,3,7,...,8,9
	  case _TRANSMISSION_SCHEME_SINGLE_PORT_CRS_:
	  case _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_:
	  case _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_:
	  case _TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_:
	  case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
	  case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
	  case _TRANSMISSION_SCHEME_NR_CL_:
	  {
	    sinrPerCodewordForWidebandCQI.set_length(nCodewords);
	    sinrPerCodewordForAllSubbandCQI(subbandCnt).set_size(nCodewords); //<nCodeword<nLayer>>
	    mmib_qpsk=0;mmib_sxtn=0;mmib_sxfr=0;
	    sinrPerCodewordForAllSubbandCQI(subbandCnt)=computeSinrPerCW(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected(subbandCnt),0,-1,1);

	    concatInArray(sinrPerCodewordForWidebandCQI,sinrPerCodewordForAllSubbandCQI(subbandCnt));

	    vec MMIBvaluesPermodschemeTemp,effSINRsPerCWTemp;
	    ivec modOrderPerCWTemp;
	    tempCQIIndex(subbandCnt)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForAllSubbandCQI(subbandCnt),MMIBvaluesPermodschemeTemp,modOrderPerCWTemp,effSINRsPerCWTemp,downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected(subbandCnt),nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	    tempRate(subbandCnt) = exp_spec_eff;
	    subbandIndices(subbandCnt)=subbandCnt;
	  }
	  break;
	  // case loop
	  default:{
	    cout<<"[both:] mode2_0 not available for all tx schemes "<<endl;
	    abort();
	  }
	}
      }

      subbandIndices=reverse(sort_index(tempRate));// obtaining the indices of the subband corresponding to the sorted tempRate
      perUserFeedback.subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord.clear();

      ivec tbSizePerCQI = zeros_i(16);
      ivec nLayersPerCodeword = getNumLayersPerCodeWord(nCodewords,nLayers/getSymbolRepetitionFactor(txScheme,nLayers));
      for(int icw=0; icw<nCodewords; icw++)
      {
	mmib_qpsk=0;mmib_sxfr=0;mmib_sxtn=0;mmib_256=0;
	for(int Cnt=0;Cnt<downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands;Cnt++)
	{
	  for(int cqi=1;cqi<tbSizePerCQI.length();cqi++)
	    tbSizePerCQI(cqi) = tbTables->cqiToMaxMCSRate.modulationIndex(cqi-1) * tbTables->cqiToMaxMCSRate.codeRate(cqi-1) * nREsPerRB * downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected(Cnt) * nLayersPerCodeword(icw);

	  mmib_sxfr += tbTables->awgnTablesConvCoder.sinrToEffMMIB(sinrPerCodewordForAllSubbandCQI(subbandIndices(Cnt))(icw),_MODULATION_SCHEME_64_QAM_);
	  mmib_sxtn += tbTables->awgnTablesConvCoder.sinrToEffMMIB(sinrPerCodewordForAllSubbandCQI(subbandIndices(Cnt))(icw),_MODULATION_SCHEME_16_QAM_);
	  mmib_qpsk += tbTables->awgnTablesConvCoder.sinrToEffMMIB(sinrPerCodewordForAllSubbandCQI(subbandIndices(Cnt))(icw),_MODULATION_SCHEME_QPSK_);
	  perUserFeedback.subbandCqiInfoPerGroupUESelected.subbandIndex(Cnt)=subbandIndices(Cnt); // storing the indices of the best numberOfUESelectedSubband
	}
	mmib_qpsk=mmib_qpsk/downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands;
	mmib_sxtn=mmib_sxtn/downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands;
	mmib_sxfr=mmib_sxfr/downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands;
	perUserFeedback.subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord(icw)=tbTables->awgnTablesConvCoder.CQISelect(exp_spec_eff, mmib_256, mmib_sxfr, mmib_sxtn, mmib_qpsk,tbSizePerCQI,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
      }

      // the avg cqi value across the UE selected subbands for each codeword is calculated
      vec MMIBvaluesPermodscheme,effSINRsPerCWTemp;
      ivec modOrderPerCWTemp;
      perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQI,MMIBvaluesPermodscheme,modOrderPerCWTemp,effSINRsPerCWTemp,DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);

      perUserFeedback.widebandCqiInfoPerGroupAperiodic.isValid=true;
      Array<ivec> SBCQI;
      SBCQI.set_size(1);
      SBCQI(0)=perUserFeedback.subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord;

      if(traceValuePerTraceName["EnableUElogs"]=="true")
	PrintCQIindices(downlinkCQIInfo->cqiComputationInfo.aperiodicMode,nodeID,perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord,modOrderPerCWTemp,effSINRsPerCWTemp, subframeNumber,AperiodicCQILogsFileName,-1,SBCQI,ivec("-1"));

    }
    break;

    case _APERIODIC_MODE_2_2_:
    {
      Array<Array<Array<vec> > > sinrPerCodewordForAllSubbandCQI;// <precoder<subband<ncodeword<re>>>>
      perUserFeedback.subbandCqiInfoPerGroupUESelected.subbandIndex.set_size(downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands);
      perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord.set_size(nCodewords); // <ncodeword>
      perUserFeedback.subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord.set_size(nCodewords); // <ncodeword>
      Array<Array<ivec> > tempSubbandCQIIndex; // <Precoder<subband<nCodeword>>>
      Array< vec > tempRate;
      Array<ivec> tempWidebandCQIIndex; // <precoder<nCodeword>>
      Array<ivec> subbandIndices;// <precoder<subbandindex>>
      Array<Array<vec> > sinrPerCodewordForWidebandCQItemp;// <precoder<nCodewords<REs>>>

      tempSubbandCQIIndex.set_length(codebookLength);
      tempRate.set_length(codebookLength);
      tempWidebandCQIIndex.set_length(codebookLength);
      subbandIndices.set_length(codebookLength);
      sinrPerCodewordForAllSubbandCQI.set_size(codebookLength);
      for(int precCnt=0;precCnt<codebookLength;precCnt++)
      {
	sinrPerCodewordForAllSubbandCQI(precCnt).set_size(downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected.length());
      }
      for(int precCnt=0;precCnt<codebookLength;precCnt++)
      {
	for(int SubbandCnt=0;SubbandCnt<downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected.length();SubbandCnt++)
	{
	  sinrPerCodewordForAllSubbandCQI(precCnt)(SubbandCnt).set_size(nCodewords); //<nCodeword<nLayer>>
	}
      }
      sinrPerCodewordForWidebandCQItemp.set_length(codebookLength);

      for(int subbandCnt=0;subbandCnt<downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected.length();subbandCnt++)
      {
	rbNumber=subbandCnt*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected(subbandCnt);

	switch(txScheme)
	{
	  /// tx modes 1,2,3,7,...,8,9
	  case _TRANSMISSION_SCHEME_CL_SM_ :
	  case _TRANSMISSION_SCHEME_CL_BF_ :
	  case _TRANSMISSION_SCHEME_CL_MU_MIMO_:
	  case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ :
	  case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ :
	    case _TRANSMISSION_SCHEME_NR_CL_ :
	  {
	    sinrPerCodewordForWidebandCQI.set_length(nCodewords);

	    for(int precIndex=0;precIndex<codebookLength;precIndex++)
	    {
	      //! identity matrix precoder is restricted from the selection..
	      if(precIndex==0 && nPorts==2 && nLayers==2)
		continue;
	      precoder=codeBook(precoderInd(precIndex));
	      subbandIndices(precIndex).set_length(downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected.length());
	      sinrPerCodewordForWidebandCQItemp(precIndex).set_length(nCodewords);
	      tempSubbandCQIIndex(precIndex).set_length(downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected.length());
	      tempRate(precIndex).set_length(downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected.length());
	      tempWidebandCQIIndex(precIndex).set_length(nCodewords);
	      mmib_qpsk=0;mmib_sxtn=0;mmib_sxfr=0;
	      sinrPerCodewordForAllSubbandCQI(precIndex)(subbandCnt)=computeSinrPerCW(frameStructure, myTxSCPowerInDB, /*interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererIndices,*/ myChannel,rxFrame,allChannels,demodulationScheme,portToAntennaMapper,refSignal,resourceMapper,noiseVariance,restOfInterferenceInDB,rbNumber,txScheme,nLayers,cqiPorts,nCodewords,downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected(subbandCnt),1,-1,1,precoder);

	      concatInArray(sinrPerCodewordForWidebandCQItemp(precIndex),sinrPerCodewordForAllSubbandCQI(precIndex)(subbandCnt));
	      vec MMIBvaluesPermodschemeTemp,effSINRsPerCWTemp;
	      ivec modOrderPerCWTemp;
	      tempSubbandCQIIndex(precIndex)(subbandCnt)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForAllSubbandCQI(precIndex)(subbandCnt),MMIBvaluesPermodschemeTemp,modOrderPerCWTemp,effSINRsPerCWTemp,downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected(subbandCnt),nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	      tempRate(precIndex)(subbandCnt) = exp_spec_eff;
	      subbandIndices(precIndex)(subbandCnt)=subbandCnt;
	    }
	    break;
	  }
	  // case loop
	  default:{
	    cout<<"[both:] mode2_2 not available for all tx schemes "<<endl;
	    abort();
	  }
	}
	// tx scheme loop
      }
      // subband loop ends


      for(int precIndex=0;precIndex<codebookLength;precIndex++)
      {
	if(precIndex==0 && nPorts==2 && nLayers==2)
	  continue;
	subbandIndices(precIndex)=reverse(sort_index(tempRate(precIndex)));// obtaining the indices of the subband corresponding to the sorted tempRate
      }
      int selectedPrecIndex=-1,maxRate=0;
      double combinedRate;
      for(int precIndex=0;precIndex<codebookLength;precIndex++)
      {
	if(precIndex==0 && nPorts==2 && nLayers==2)
	  continue;
	combinedRate=0;
	for(int Cnt=0;Cnt<downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands;Cnt++)
	{
	  combinedRate+=tempRate(precIndex)(subbandIndices(precIndex)(Cnt));
	}
	if(maxRate<combinedRate)
	{
	  maxRate=combinedRate;
	  selectedPrecIndex=precIndex;
	}
      }
      perUserFeedback.subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord.clear();
      perUserFeedback.subbandCqiInfoPerGroupUESelected.subbandIndex.set_length(downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands);

      ivec tbSizePerCQI = zeros_i(16);
      ivec nLayersPerCodeword = getNumLayersPerCodeWord(nCodewords,nLayers/getSymbolRepetitionFactor(txScheme,nLayers));

      for(int icw=0; icw<nCodewords; icw++)
      {
	mmib_qpsk=0;mmib_sxfr=0;mmib_sxtn=0;
	for(int Cnt=0;Cnt<downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands;Cnt++)
	{
	  for(int cqi=1;cqi<tbSizePerCQI.length();cqi++)
	    tbSizePerCQI(cqi) = tbTables->cqiToMaxMCSRate.modulationIndex(cqi-1) * tbTables->cqiToMaxMCSRate.codeRate(cqi-1) * nREsPerRB * downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected(Cnt) * nLayersPerCodeword(icw);

	  mmib_sxfr += tbTables->awgnTablesConvCoder.sinrToEffMMIB(sinrPerCodewordForAllSubbandCQI(selectedPrecIndex)(subbandIndices(selectedPrecIndex)(Cnt))(icw),_MODULATION_SCHEME_64_QAM_);
	  mmib_sxtn += tbTables->awgnTablesConvCoder.sinrToEffMMIB(sinrPerCodewordForAllSubbandCQI(selectedPrecIndex)(subbandIndices(selectedPrecIndex)(Cnt))(icw),_MODULATION_SCHEME_16_QAM_);
	  mmib_qpsk += tbTables->awgnTablesConvCoder.sinrToEffMMIB(sinrPerCodewordForAllSubbandCQI(selectedPrecIndex)(subbandIndices(selectedPrecIndex)(Cnt))(icw),_MODULATION_SCHEME_QPSK_);
	  // adding the CQI value for the best numberOfUESelectedSubbands for each codeword to cqiIndexPerCodeword
	  perUserFeedback.subbandCqiInfoPerGroupUESelected.subbandIndex(Cnt)=subbandIndices(selectedPrecIndex)(Cnt); // storing the indices of the best numberOfUESelectedSubband
	}
	mmib_qpsk=mmib_qpsk/downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands;
	mmib_sxtn=mmib_sxtn/downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands;
	mmib_sxfr=mmib_sxfr/downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands;
	perUserFeedback.subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord(icw)=tbTables->awgnTablesConvCoder.CQISelect(exp_spec_eff, mmib_256, mmib_sxfr, mmib_sxtn, mmib_qpsk,tbSizePerCQI,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
      }

      // the avg cqi value across the UE selected subbands for each codeword is calculated
      perUserFeedback.subbandCqiInfoPerGroupUESelected.PMI=selectedPrecIndex;

      vec SpectralEfficiencyOverPMIs=zeros(codebookLength)-1;
      Array<ivec> tempCQIperCodewords(codebookLength);
      Array<vec> MMIBvaluesPermodscheme(codebookLength);

      vec effSINRsPerCWTemp;
      ivec modOrderPerCWTemp;
      for(int precIndex=0;precIndex<codebookLength;precIndex++)
      {
	//! identity matrix precoder is restricted from the selection..
	if(precIndex==0 && nPorts==2 && nLayers==2)
	  continue;

	tempCQIperCodewords(precIndex)=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQItemp(precIndex),MMIBvaluesPermodscheme(precIndex),modOrderPerCWTemp,effSINRsPerCWTemp,DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
	SpectralEfficiencyOverPMIs(precIndex)=exp_spec_eff;
      }

      int tPMI = getBestPMIindex(SpectralEfficiencyOverPMIs,MMIBvaluesPermodscheme);

      perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord=tempCQIperCodewords(tPMI);
      perUserFeedback.widebandCqiInfoPerGroupAperiodic.widebandPMI=precoderInd(tPMI);
      perUserFeedback.widebandCqiInfoPerGroupAperiodic.isValid=true;

      Array<ivec> SBCQI;
      SBCQI.set_size(1);
      SBCQI(0)=perUserFeedback.subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord;
      ivec CQIperCW=getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForWidebandCQItemp(tPMI),MMIBvaluesPermodscheme(tPMI),modOrderPerCWTemp,effSINRsPerCWTemp,DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
      ivec sb_pmi;
      sb_pmi.set_length(1);
      sb_pmi=perUserFeedback.subbandCqiInfoPerGroupUESelected.PMI;
      if(traceValuePerTraceName["EnableUElogs"]=="true")
	PrintCQIindices(downlinkCQIInfo->cqiComputationInfo.aperiodicMode,nodeID,perUserFeedback.widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord,modOrderPerCWTemp,effSINRsPerCWTemp, subframeNumber,AperiodicCQILogsFileName,perUserFeedback.widebandCqiInfoPerGroupAperiodic.widebandPMI,SBCQI,sb_pmi);

      break;

    }
    default :
    {
      cout<<" [both:] not ready for other aperiodic cases"<<endl;abort();
    }
  }
  // switch loop ends for aperiodic mode


  // CQI Group loop ends
  perUserFeedback.isValid=true;
}

SubbandULCQIInfo_S CQIEstimator::computeRankAndCQIFromIdealChannelForUplink(FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ResourceMapper* resourceMapper, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, TransmissionScheme_E txScheme,Waveform_E waveform, int maxLayers, ivec cqiPorts, int maxCodewords, double noiseVariance, double NIValueForTheServerNode, bool enableCQI0){
  
  Array<SubbandULCQIInfo_S> subbandCqiInfo(maxLayers);

  //Table 7.3.1.3-1: Codeword-to-layer mapping for spatial multiplexing  
  ivec layers=getIntegers(1,8);
  ivec codewords="1 1 1 1 2 2 2 2";
  
  vec specEff(maxLayers);
  
  for(int rank_cnt=0;rank_cnt<maxLayers;rank_cnt++)
  {
    subbandCqiInfo(rank_cnt)=computeCQIFromIdealChannelForUplink(frameStructure, demodulationScheme, portToAntennaMapper, resourceMapper, myChannel, myTxSCPowerInDB, txScheme, waveform, layers(rank_cnt), cqiPorts, codewords(rank_cnt), noiseVariance, NIValueForTheServerNode, enableCQI0);
    specEff(rank_cnt)=mean(subbandCqiInfo(rank_cnt).specEff);
  }
    
  return(subbandCqiInfo(max_index(specEff)));
}

//reciprocity
SubbandULCQIInfo_S CQIEstimator::computeRankAndCQIFromIdealChannelForUplinkForDLReci(FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ResourceMapper* resourceMapper, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, TransmissionScheme_E txScheme,Waveform_E waveform, int maxLayers, ivec cqiPorts, int maxCodewords, double noiseVariance, double NIValueForTheServerNode, ReciprocityPrecodersInfo_S &reciprocityPrecoders,  double weakIntr, Array<cmat> intrCov, double noiseVar, bool enableCQI0){
  
  Array<SubbandULCQIInfo_S> subbandCqiInfoEachRank(maxLayers);

  //Table 7.3.1.3-1: Codeword-to-layer mapping for spatial multiplexing  
  ivec layers=getIntegers(1,8);
  ivec codewords="1 1 1 1 2 2 2 2";
  
  vec specEff(maxLayers);
  Array<cmat> precoderPerRB=reciprocityPrecoders.precoderPerRB;
     
   for(int rank_cnt=0;rank_cnt<maxLayers;rank_cnt++)
  {
  int nLayers=layers(rank_cnt);
  int nCodewords=codewords(rank_cnt);
  SubbandULCQIInfo_S subbandCqiInfo;
  Size_S rbSize=resourceMapper->ulResourceMapper.getRBSize();
  /// Computing for all RB Groups irrespective of allocation Type...
  
  int nPorts=cqiPorts.length();//! may change
  int nPortsDL=myChannel.length(); //Considering number of DL ports equal to number of Tx antennas in DL
  //nPorts=nPortsDL;//Change
  
  int nRBs=resourceMapper->ulResourceMapper.getResourceBlocksPerSubframe();
  //int rbGroupSize=resourceMapper->ulResourceMapper.getResourceBlockGroupingSize();
  int subbandSize=4; //hardcoded
  int nSubbands=nRBs/subbandSize;//13
  int rbGroupSize=1;
  int rbGroupCount=nRBs/rbGroupSize;
  rbGroupSize=subbandSize;
  rbGroupCount=nSubbands;
  
  
  //if(rbGroupCount*rbGroupSize<nRBs) rbGroupCount+=1;
  subbandCqiInfo.effSINRPerCodeWord.set_length(rbGroupCount);
  subbandCqiInfo.specEff.set_length(rbGroupCount);
  subbandCqiInfo.vrbIndex=getIntegers(0,nRBs-1);
  subbandCqiInfo.subbandIndexPerRB=zeros_i(nRBs)-1;
  subbandCqiInfo.cqiIndxPerRB.set_length(1);
  
//   if(txScheme==_TRANSMISSION_SCHEME_CL_SM_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_ )
//     subbandCqiInfo.pmiIndex=zeros_i(rbGroupCount);
//   
//   subbandCqiInfo.channelPerSubBand.set_size(rbGroupCount);
   
  Array< cmat > ruPrecodedChannel(0);
  Array<cmat> signalChannel(0);
  Array<Array<Array<vec> > > sinrPerCodewordForSubbandCQI;// <precoder<rbGroup<CQI<nLayers>>>>
  Array<Array<vec> >effSINRSubband;// <precoder<rbGrp<CQI>>>
  Array<Array<ivec> > cqiIndexSubband;
  double exp_spec_eff;
  
  int totalRe_cnt=0;
  int rsCountPerBand;
  DataBlkInfo_S dataBlkInfoInInterferingTx;
  int totReCountInRB=rbSize.numberOfSubcarriers*rbSize.numberOfSymbols;
  //Array<cmat> codeBook;
  cmat precoder(0,0);
   
  sinrPerCodewordForSubbandCQI.set_size(1);
  effSINRSubband.set_size(1);
  cqiIndexSubband.set_size(1);
  
  Array<vec> SpectralEfficiencyOverPMIsSubband(rbGroupCount);
  for(int i=0;i<SpectralEfficiencyOverPMIsSubband.length();i++)
    SpectralEfficiencyOverPMIsSubband(i)=zeros(1);
  
  Array< Array<vec> > MMIBvaluesPermodscheme(rbGroupCount);
  for(int i=0;i<MMIBvaluesPermodscheme.length();i++)
    MMIBvaluesPermodscheme(i).set_length(1);
  
  //! interfering channel info
  //double weakIntrPwr = NIValueForTheServerNode-2*noiseVariance;
  
  Array<string> rbName(nPorts);
  
  Array<ivec> rsPositions(nPorts);
  ivec scPositions=zeros_i(nPorts);
  for(int port_cnt=0;port_cnt<nPorts;port_cnt++)
  {
    rbName(port_cnt)="port"+toString(cqiPorts(port_cnt));
    
    if( txScheme==_TRANSMISSION_SCHEME_NR_CL_ ) // using dmrs ports as cqi ports . Need to change -  Dhiv
      rsPositions(port_cnt)="2";
  }
  
  
  imat rsRBStructure;
  rsRBStructure=frameStructure.getMergedResourceUnit("nullRB",rbName,"data",rsPositions,scPositions);
  
  rsRBStructure = rsRBStructure.get_cols(3,rsRBStructure.cols()-1);
  
  ivec dataTones = find(rsRBStructure,0,true);
  int nREsPerRB = dataTones.length();
  nREsPerRB=nREsPerRB-nPortsDL; //132-32=100, need to be fixed
  //! Assuming SRS is present in all RBS in the last symbol in all subframes (not true as per implementation) - Dhiv
  int rsCountPerRb=rbSize.numberOfSubcarriers;
  ivec portLocations=getIntegers(totReCountInRB-rsCountPerRb,totReCountInRB-1);
  
/*  Array< Array<cmat> > intrfrChannel; intrfrChannel.set_length(0);
  vec interfererTxSCPowerInDB;
  interfererTxSCPowerInDB.set_length(0); */ 
  ivec rbs=getIntegers(0,nRBs-1);
 Array<ivec> rbsForEachRBGroup=splitVectorToArray(rbs,rbGroupSize);
  ivec rbgsForCQI=getIntegers(0,rbGroupCount-1);
  
  
//   #ifdef USING_SRS
//   if(isSRSEnabled)
//   {
//     rbgsForCQI=findUniqueNumbers(srsRBs/rbGroupSize);
//     portLocations=getIntegers(0,rbSize.numberOfSubcarriers-1,2)+(int)isOddSCs+(rbSize.numberOfSymbols-1)*rbSize.numberOfSubcarriers;
//     rsCountPerRb = portLocations.length();
//   }
//   #endif

  // Storing only the first subcarrier raw channel per subband/rbg
//   subbandCqiInfo.channelPerSubBand.set_length(rbGroupCount);
//   for(int rbg_cnt=0;rbg_cnt<rbGroupCount;rbg_cnt++)
//     subbandCqiInfo.channelPerSubBand(rbg_cnt)=getRawPhysicalChannel(resourceMapper,myChannel,rbsForEachRBGroup(rbg_cnt)(0),0,portLocations);
// cout<<"intr"<<intrCov.length()<<endl;
// cout<<"intrCov(0).rows()"<<intrCov(0).rows()<<endl;
// cout<<"intrCov(0).cols()"<<intrCov(0).cols()<<endl;
 cmat sum=zeros_c(intrCov(0).rows(),intrCov(0).cols());
      for(int i=0;i<intrCov.length();i++)
          sum+=intrCov(i);
      cmat interCov=sum/intrCov.length();
      
  for(int prec_ind=0;prec_ind<1;prec_ind++)
  {
    
      
    sinrPerCodewordForSubbandCQI(prec_ind).set_size(rbGroupCount);
    effSINRSubband(prec_ind).set_size(rbGroupCount);
    cqiIndexSubband(prec_ind).set_size(rbGroupCount);
    subbandCqiInfo.cqiIndxPerRB(prec_ind).set_length(nRBs);
    for(int rbg_cnt=0;rbg_cnt<rbgsForCQI.length();rbg_cnt++)
    {
      int rbgIndx=rbgsForCQI(rbg_cnt);
       // int rbgIndx=rbg_cnt;
      ivec rbsInRBGroup=rbsForEachRBGroup(rbgIndx);
      
      rsCountPerBand=rsCountPerRb*rbsInRBGroup.length();
      
      signalChannel.set_length(rsCountPerBand);
      
      totalRe_cnt=0;
      
    for(int rb_cnt=0;rb_cnt<rbsInRBGroup.length();rb_cnt++)
      {
          precoder=precoderPerRB((rbg_cnt*rbsInRBGroup.length())+rb_cnt);
          precoder=precoder.get_cols(0,nLayers-1);
          //normalising the precoder
          double sum=0.0;
          for(int i=0;i<precoder.rows();i++)
          {
              for(int j=0;j<precoder.cols();j++)
              {
                  sum=sum+(abs(precoder(i,j))* abs(precoder(i,j)));
              }
          }
          precoder=precoder/(sqrt(sum));                 
          
//cout<<"precoder"<<precoder<<endl;
ruPrecodedChannel=getPrecodedPhysicalChannel(resourceMapper,portToAntennaMapper,myChannel,rbs((rbg_cnt*rbsInRBGroup.length())+rb_cnt),txScheme,nLayers,nPortsDL,(precoder.size()!=0),-1,1,portLocations,precoder); 
	
	for(int re_cnt=0;re_cnt <rsCountPerRb;re_cnt++, totalRe_cnt++)
	  signalChannel(totalRe_cnt)=ruPrecodedChannel(re_cnt);
    
      }
     
     // if(waveform==_Waveform_OFDMA_)
      {  
	//int rxAntenna=myChannel(0).length();
	Array<cmat> perToneInterferenceCovariance=repeatPerToneInterferenceCovariance(interCov,rsCountPerRb*rbsInRBGroup.length());
	sinrPerCodewordForSubbandCQI(prec_ind)(rbgIndx)=calculateSinrWithIdealChannelperSubband(txScheme,nCodewords,nLayers,demodulationScheme,rsCountPerRb*rbsInRBGroup.length(),signalChannel, inv_dB(myTxSCPowerInDB),perToneInterferenceCovariance); //<codeword<layers*rscount>>
      }
      
      //cout<<"sinr2"<<sinrPerCodewordForSubbandCQI(0)(rbg_cnt)(0)<<endl;
//       else
// 	sinrPerCodewordForSubbandCQI(prec_ind)(rbgIndx)=calculateSinrWithIdealChannelperSubbandForSCFDMA(txScheme,nCodewords,nLayers,demodulationScheme,rsCountPerRb*rbsInRBGroup.length(),signalChannel,intrfrChannel,interCov, inv_dB(myTxSCPowerInDB),inv_dB(interfererTxSCPowerInDB));
      
      effSINRSubband(prec_ind)(rbgIndx).set_length(sinrPerCodewordForSubbandCQI(prec_ind)(rbgIndx).length());
      cqiIndexSubband(prec_ind)(rbgIndx).set_length(sinrPerCodewordForSubbandCQI(prec_ind)(rbgIndx).length());
      
      ivec modOrderPerCWTemp;
      vec effSINRsPerCWTemp;
           
      // OLLA offset is added inside getCQIPerCodewords
      cqiIndexSubband(prec_ind)(rbgIndx) = getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForSubbandCQI(prec_ind)(rbgIndx),MMIBvaluesPermodscheme(rbgIndx)(prec_ind),modOrderPerCWTemp,effSINRsPerCWTemp,rbsInRBGroup.length(),nREsPerRB,txScheme,nLayers,enableCQI0); //eachprecoder, eachRB, eachcodeword
      
      SpectralEfficiencyOverPMIsSubband(rbgIndx)(prec_ind)=exp_spec_eff;
    
      for(int rb_cnt=0;rb_cnt<rbsInRBGroup.length();rb_cnt++)
      subbandCqiInfo.cqiIndxPerRB(prec_ind)((rbg_cnt*rbsInRBGroup.length())+rb_cnt)=cqiIndexSubband(prec_ind)(rbgIndx);
      //cout<<"cqi2"<<cqiIndexSubband(prec_ind)(rbgIndx)<<endl;
      // In SC-FDMA, the PPSINR will be same for all subcarriers, Hence MMIB averging is not needed - Chandru
//       for(int cw_cnt = 0; cw_cnt < effSINRSubband(prec_ind)(rbgIndx).length(); cw_cnt++)
//       {
// 	effSINRSubband(prec_ind)(rbgIndx)(cw_cnt) = dB(sinrPerCodewordForSubbandCQI(prec_ind)(rbgIndx)(cw_cnt)(0));
//       }
      
//       if(prec_ind==0)
// 	for(int rb_cnt=0;rb_cnt<rbsInRBGroup.length();rb_cnt++)
// 	  subbandCqiInfo.subbandIndexPerRB(rbsInRBGroup(rb_cnt))=rbgIndx;
    }
  }
  

  //else if(txScheme==_TRANSMISSION_SCHEME_CL_SM_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_)
  {
    //if(nPorts>1)
    {
      //int tempPMI;
      // to choose the best PMI by choosing the codebook which gives max SE
     // ivec possiblePMIs;
      
      // to choose the best PMI by choosing the codebook which gives max average MMIB
      for(int rbg_cnt=0;rbg_cnt<rbgsForCQI.length();rbg_cnt++)
      {
	    int rbgIndx=rbgsForCQI(rbg_cnt);
	//possiblePMIs = find(SpectralEfficiencyOverPMIsSubband(rbgIndx),max(SpectralEfficiencyOverPMIsSubband(rbgIndx)),true);
	
// 	if(possiblePMIs.length()==1)
// 	  tempPMI=possiblePMIs(0);
// 	else
// 	{
// 	  //Array<vec> MMIBvaluesPermodschemeTemp = getFromArray(MMIBvaluesPermodscheme(rbgIndx),possiblePMIs);
// 	  int tempPMIforRBG=max_index(SpectralEfficiencyOverPMIsSubband(rbgIndx));
// 	  ivec tempCQIforRBG=cqiIndexSubband(tempPMIforRBG)(rbgIndx);
// 	  //tempPMI=PMIselect(possiblePMIs,MMIBvaluesPermodschemeTemp);
// 	}
	//subbandCqiInfo.effSINRPerCodeWord(rbgIndx)=effSINRSubband(0)(rbgIndx);
	//subbandCqiInfo.pmiIndex(rbgIndx)=tempPMI;
	    subbandCqiInfo.specEff(rbgIndx)=max(SpectralEfficiencyOverPMIsSubband(rbgIndx));
        if(cqiIndexSubband(0)(rbgIndx)(0)<4)
          subbandCqiInfo.specEff(rbgIndx)=subbandCqiInfo.specEff(rbgIndx)/((double)nLayers);//Rank restriction  
	
      }
    }
    //else
      //subbandCqiInfo.effSINRPerCodeWord=effSINRSubband(0);
  }
  
  //   cout<<"CQI \n"<<subbandCqiInfo.effSINRPerCodeWord<<endl;
  subbandCqiInfo.rank=nLayers;
  subbandCqiInfoEachRank(rank_cnt)=subbandCqiInfo;
  
    
    specEff(rank_cnt)=mean(subbandCqiInfoEachRank(rank_cnt).specEff);
    
  }
    
  return(subbandCqiInfoEachRank(max_index(specEff)));
}


SubbandULCQIInfo_S CQIEstimator::computeCQIFromIdealChannelForUplink(FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ResourceMapper* resourceMapper, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, TransmissionScheme_E txScheme,Waveform_E waveform, int nLayers, ivec cqiPorts, int nCodewords, double noiseVariance, double NIValueForTheServerNode, bool enableCQI0){
  
  SubbandULCQIInfo_S subbandCqiInfo;
  Size_S rbSize=resourceMapper->ulResourceMapper.getRBSize();
  /// Computing for all RB Groups irrespective of allocation Type...
  
  int nPorts=cqiPorts.length();//! may change
  int nRBs=resourceMapper->ulResourceMapper.getResourceBlocksPerSubframe();
  int rbGroupSize=resourceMapper->ulResourceMapper.getResourceBlockGroupingSize();
  int rbGroupCount=nRBs/rbGroupSize;
  if(rbGroupCount*rbGroupSize<nRBs) rbGroupCount+=1;
  subbandCqiInfo.effSINRPerCodeWord.set_length(rbGroupCount);
  subbandCqiInfo.specEff.set_length(rbGroupCount);
  subbandCqiInfo.vrbIndex=getIntegers(0,nRBs-1);
  subbandCqiInfo.subbandIndexPerRB=zeros_i(nRBs)-1;
  
  if(txScheme==_TRANSMISSION_SCHEME_CL_SM_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_ )
    subbandCqiInfo.pmiIndex=zeros_i(rbGroupCount);
  
  subbandCqiInfo.channelPerSubBand.set_size(rbGroupCount);
   
  Array< cmat > ruPrecodedChannel;
  Array<cmat> signalChannel(0);
  Array<Array<Array<vec> > > sinrPerCodewordForSubbandCQI;// <precoder<rbGroup<CQI<nLayers>>>>
  Array<Array<vec> >effSINRSubband;// <precoder<rbGrp<CQI>>>
  Array<Array<ivec> > cqiIndexSubband;
  double exp_spec_eff;
  
  int totalRe_cnt=0;
  int rsCountPerBand;
  DataBlkInfo_S dataBlkInfoInInterferingTx;
  int totReCountInRB=rbSize.numberOfSubcarriers*rbSize.numberOfSymbols;
  Array<cmat> codeBook;
  cmat precoder(0,0);
  if(txScheme==_TRANSMISSION_SCHEME_CL_SM_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_ )
  {
    if(nPorts>1)
      codeBook=mCodebook->getCodebook(nPorts,nLayers,0);
    else
      codeBook.set_size(0);
  }
  else if(txScheme==_TRANSMISSION_SCHEME_SINGLE_PORT_CRS_)
    codeBook.set_size(0);
  else
  {
    cout<<"[detl:]Error. Given transmisssion mode not supported in uplink.... From computeCQIFromIdealChannelForUplink"<<endl;
    abort();
  }
  
  sinrPerCodewordForSubbandCQI.set_size(max(codeBook.length(),1));
  effSINRSubband.set_size(max(codeBook.length(),1));
  cqiIndexSubband.set_size(max(codeBook.length(),1));
  
  Array<vec> SpectralEfficiencyOverPMIsSubband(rbGroupCount);
  for(int i=0;i<SpectralEfficiencyOverPMIsSubband.length();i++)
    SpectralEfficiencyOverPMIsSubband(i)=zeros(max(codeBook.length(),1));
  
  Array< Array<vec> > MMIBvaluesPermodscheme(rbGroupCount);
  for(int i=0;i<MMIBvaluesPermodscheme.length();i++)
    MMIBvaluesPermodscheme(i).set_length(max(codeBook.length(),1));
  
  //! interfering channel info
  double weakIntrPwr = NIValueForTheServerNode-2*noiseVariance;
  
  Array<string> rbName(cqiPorts.length());
  
  Array<ivec> rsPositions(cqiPorts.length());
  ivec scPositions=zeros_i(cqiPorts.length());
  for(int port_cnt=0;port_cnt<cqiPorts.length();port_cnt++)
  {
    rbName(port_cnt)="port"+toString(cqiPorts(port_cnt));
    
    if( txScheme==_TRANSMISSION_SCHEME_NR_CL_ ) // using dmrs ports as cqi ports . Need to change -  Dhiv
      rsPositions(port_cnt)="2";
  }
  
  
  imat rsRBStructure;
  rsRBStructure=frameStructure.getMergedResourceUnit("nullRB",rbName,"data",rsPositions,scPositions);
  
  rsRBStructure = rsRBStructure.get_cols(3,rsRBStructure.cols()-1);
  
  ivec dataTones = find(rsRBStructure,0,true);
  int nREsPerRB = dataTones.length();
  //! Assuming SRS is present in all RBS in the last symbol in all subframes (not true as per implementation) - Dhiv
  int rsCountPerRb=rbSize.numberOfSubcarriers;
  ivec portLocations=getIntegers(totReCountInRB-rsCountPerRb,totReCountInRB-1);
  
  Array< Array<cmat> > intrfrChannel; intrfrChannel.set_length(0);
  vec interfererTxSCPowerInDB;
  interfererTxSCPowerInDB.set_length(0);  
  ivec rbs=getIntegers(0,nRBs-1);
  Array<ivec> rbsForEachRBGroup=splitVectorToArray(rbs,rbGroupSize);
  ivec rbgsForCQI=getIntegers(0,rbGroupCount-1);
  
  
  #ifdef USING_SRS
  if(isSRSEnabled)
  {
    rbgsForCQI=findUniqueNumbers(srsRBs/rbGroupSize);
    portLocations=getIntegers(0,rbSize.numberOfSubcarriers-1,2)+(int)isOddSCs+(rbSize.numberOfSymbols-1)*rbSize.numberOfSubcarriers;
    rsCountPerRb = portLocations.length();
  }
  #endif

  // Storing only the first subcarrier raw channel per subband/rbg
  subbandCqiInfo.channelPerSubBand.set_length(rbGroupCount);
  for(int rbg_cnt=0;rbg_cnt<rbGroupCount;rbg_cnt++)
    subbandCqiInfo.channelPerSubBand(rbg_cnt)=getRawPhysicalChannel(resourceMapper,myChannel,rbsForEachRBGroup(rbg_cnt)(0),0,portLocations);
 
  for(int prec_ind=0;prec_ind<max(1,codeBook.length());prec_ind++)
  {
    if(codeBook.length()>0)
      precoder=codeBook(prec_ind);
    sinrPerCodewordForSubbandCQI(prec_ind).set_size(rbGroupCount);
    effSINRSubband(prec_ind).set_size(rbGroupCount);
    cqiIndexSubband(prec_ind).set_size(rbGroupCount);
    
    for(int rbg_cnt=0;rbg_cnt<rbgsForCQI.length();rbg_cnt++)
    {
      int rbgIndx=rbgsForCQI(rbg_cnt);
      ivec rbsInRBGroup=rbsForEachRBGroup(rbgIndx);
      rsCountPerBand=rsCountPerRb*rbsInRBGroup.length();
      
      signalChannel.set_length(rsCountPerBand);
      
      totalRe_cnt=0;
      
      for(int rb_cnt=0;rb_cnt<rbsInRBGroup.length();rb_cnt++)
      {
	ruPrecodedChannel=getPrecodedPhysicalChannel(resourceMapper,portToAntennaMapper,myChannel,rbsInRBGroup(rb_cnt),txScheme,nLayers,nPorts,(precoder.size()!=0),-1,0,portLocations,precoder);
	
	for(int re_cnt=0;re_cnt <rsCountPerRb;re_cnt++, totalRe_cnt++)
	  signalChannel(totalRe_cnt)=ruPrecodedChannel(re_cnt);
      }
      
      
      cmat interCov=uplinkAvgIntrCov;
      if(waveform==_Waveform_OFDMA_)
      {  
	int rxAntenna=myChannel(0).length();
	Array<cmat> perToneInterferenceCovariance=repeatPerToneInterferenceCovariance(interCov,rsCountPerRb*rbsInRBGroup.length());
	sinrPerCodewordForSubbandCQI(prec_ind)(rbgIndx)=calculateSinrWithIdealChannelperSubband(txScheme,nCodewords,nLayers,demodulationScheme,rsCountPerRb*rbsInRBGroup.length(),signalChannel, inv_dB(myTxSCPowerInDB),perToneInterferenceCovariance);
      }
      else
	sinrPerCodewordForSubbandCQI(prec_ind)(rbgIndx)=calculateSinrWithIdealChannelperSubbandForSCFDMA(txScheme,nCodewords,nLayers,demodulationScheme,rsCountPerRb*rbsInRBGroup.length(),signalChannel,intrfrChannel,interCov, inv_dB(myTxSCPowerInDB),inv_dB(interfererTxSCPowerInDB));
      
      effSINRSubband(prec_ind)(rbgIndx).set_length(sinrPerCodewordForSubbandCQI(prec_ind)(rbgIndx).length());
      cqiIndexSubband(prec_ind)(rbgIndx).set_length(sinrPerCodewordForSubbandCQI(prec_ind)(rbgIndx).length());
      
      ivec modOrderPerCWTemp;
      vec effSINRsPerCWTemp;
      
      // OLLA offset is added inside getCQIPerCodewords
      cqiIndexSubband(prec_ind)(rbgIndx) = getCQIPerCodewords(exp_spec_eff,sinrPerCodewordForSubbandCQI(prec_ind)(rbgIndx),MMIBvaluesPermodscheme(rbgIndx)(prec_ind),modOrderPerCWTemp,effSINRsPerCWTemp,rbsInRBGroup.length(),nREsPerRB,txScheme,nLayers,enableCQI0);
      SpectralEfficiencyOverPMIsSubband(rbgIndx)(prec_ind)=exp_spec_eff;
      
      // In SC-FDMA, the PPSINR will be same for all subcarriers, Hence MMIB averging is not needed - Chandru
      for(int cw_cnt = 0; cw_cnt < effSINRSubband(prec_ind)(rbgIndx).length(); cw_cnt++)
      {
	effSINRSubband(prec_ind)(rbgIndx)(cw_cnt) = dB(sinrPerCodewordForSubbandCQI(prec_ind)(rbgIndx)(cw_cnt)(0));
      }
      
      if(prec_ind==0)
	for(int rb_cnt=0;rb_cnt<rbsInRBGroup.length();rb_cnt++)
	  subbandCqiInfo.subbandIndexPerRB(rbsInRBGroup(rb_cnt))=rbgIndx;
    }
  }
  
  if(txScheme==_TRANSMISSION_SCHEME_SINGLE_PORT_CRS_)
  {
    subbandCqiInfo.effSINRPerCodeWord=effSINRSubband(0);
  }
  else if(txScheme==_TRANSMISSION_SCHEME_CL_SM_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_)
  {
    if(nPorts>1)
    {
      int tempPMI;
      // to choose the best PMI by choosing the codebook which gives max SE
      ivec possiblePMIs;
      
      // to choose the best PMI by choosing the codebook which gives max average MMIB
      for(int rbg_cnt=0;rbg_cnt<rbgsForCQI.length();rbg_cnt++)
      {
	int rbgIndx=rbgsForCQI(rbg_cnt);
	possiblePMIs = find(SpectralEfficiencyOverPMIsSubband(rbgIndx),max(SpectralEfficiencyOverPMIsSubband(rbgIndx)),true);
	
	if(possiblePMIs.length()==1)
	  tempPMI=possiblePMIs(0);
	else
	{
	  Array<vec> MMIBvaluesPermodschemeTemp = getFromArray(MMIBvaluesPermodscheme(rbgIndx),possiblePMIs);
	  int tempPMIforRBG=max_index(SpectralEfficiencyOverPMIsSubband(rbgIndx));
	  ivec tempCQIforRBG=cqiIndexSubband(tempPMIforRBG)(rbgIndx);
	  tempPMI=PMIselect(possiblePMIs,MMIBvaluesPermodschemeTemp);
	}
	subbandCqiInfo.effSINRPerCodeWord(rbgIndx)=effSINRSubband(tempPMI)(rbgIndx);
	subbandCqiInfo.pmiIndex(rbgIndx)=tempPMI;
	subbandCqiInfo.specEff(rbgIndx)=max(SpectralEfficiencyOverPMIsSubband(rbgIndx));
	
      }
    }
    else
      subbandCqiInfo.effSINRPerCodeWord=effSINRSubband(0);
  }
  else
    cout<<"given Transmission scheme is not supported in computeCQIFromIdealChannelForUplink"<<endl;
  
  //   cout<<"CQI \n"<<subbandCqiInfo.effSINRPerCodeWord<<endl;
  subbandCqiInfo.rank=nLayers;
  return(subbandCqiInfo);
}

ReciprocityPrecodersInfo_S CQIEstimator::computeReciprocitybasedPrecoderFromIdealChannelForUplinkPerRBG(Framehandler* myTxFrameHandler,DemodulationScheme_E demodulationScheme,PortToAntennaMapper_S *portToAntennaMapper,ResourceMapper *resourceMapper, Array< Array<cmat> > &myChannel, double myTxSCPowerInDB,TransmissionScheme_E txScheme, int nLayers,ivec cqiPorts , int nCodewords,double noiseVariance,double NIValueForTheServerNode,bool isDownlink,bool enableCQI0){
  
  ReciprocityPrecodersInfo_S reciprocityPrecodersInfo;
  cout<<"hey ! i entered in your module for calculating precoders.calculate now"<<endl;
  Size_S rbSize=resourceMapper->ulResourceMapper.getRBSize();
  //cout<<"rb_size"<<rbSize<<endl;
  /// Computing for all RB Groups irrespective of allocation Type...
  
  int nPorts=cqiPorts.length();//! may change
//   cout<<"nPorts:"<<nPorts<<endl;
  int nRBs=resourceMapper->ulResourceMapper.getResourceBlocksPerSubframe();
//   cout<<"resource blocks per subframe"<<nRBs<<endl;
  int rbGroupSize=resourceMapper->ulResourceMapper.getResourceBlockGroupingSize();
//   cout<<"rb_group_size"<<rbGroupSize<<endl;
  int rbGroupCount=nRBs/rbGroupSize;
//   cout<<"rb_group_count"<<rbGroupCount<<endl;
  if(rbGroupCount*rbGroupSize<nRBs) rbGroupCount+=1;
//   reciprocityPrecodersInfo.effSINRPerCodeWord.set_length(rbGroupCount);
  reciprocityPrecodersInfo.vrbIndex=getIntegers(0,nRBs-1);
//   reciprocityPrecodersInfo.subbandIndexPerRB=zeros_i(nRBs)-1;
  
  reciprocityPrecodersInfo.precoderPerRBG.set_size(rbGroupCount);
  Array< cmat > channelPerRE;
  Array<cmat> signalChannel(0);
  Array<Array<Array<vec> > > sinrPerCodewordForSubbandCQI;// <precoder<rbGroup<CQI<nLayers>>>>
  Array<Array<vec> >effSINRSubband;// <precoder<rbGrp<CQI>>>
  Array<Array<ivec> > cqiIndexSubband;
  double exp_spec_eff;
  
  int totalRe_cnt=0;
  int rsCountPerBand;
  DataBlkInfo_S dataBlkInfoInInterferingTx;
  int totReCountInRB=rbSize.numberOfSubcarriers*rbSize.numberOfSymbols;
  Array<cmat> codeBook;
  
  if(txScheme==_TRANSMISSION_SCHEME_CL_SM_)
  {
    codeBook=mCodebook->getCodebook(nPorts,nLayers,0);
  }
  else if(txScheme==_TRANSMISSION_SCHEME_SINGLE_PORT_CRS_)
    codeBook.set_size(0);
  else
  {
    cout<<"[detl:]Error. Given transmisssion mode not supported in uplink.... From computeCQIFromIdealChannelForUplink"<<endl;
    abort();
  }
  
  sinrPerCodewordForSubbandCQI.set_size(max(codeBook.length(),1));
  effSINRSubband.set_size(max(codeBook.length(),1));
  cqiIndexSubband.set_size(max(codeBook.length(),1));
  //! interfering channel info
  double weakIntrPwr = NIValueForTheServerNode-2*noiseVariance;
  
  Array<string> rbName(cqiPorts.length());
  rbName.set_length(cqiPorts.length());
  for(int port_cnt=0;port_cnt<cqiPorts.length();port_cnt++)
    rbName(port_cnt)="port"+toString(cqiPorts(port_cnt));
  imat rsRBStructure;
  FrameStructure *frameStructure=myTxFrameHandler->getFrameStructurePointer();
  if(rbName.length()>1)
    rsRBStructure=frameStructure->getMergedResourceUnit(rbName(0),rbName(1,rbName.length()-1),"data");
  else if(rbName.length()==1)
  {
    Array<string> dummy(0);
    rsRBStructure=frameStructure->getMergedResourceUnit(rbName(0),dummy,"data");
  }
  
  rsRBStructure = rsRBStructure.get_cols(3,rsRBStructure.cols()-1);
  ivec dataTones = find(rsRBStructure,0,true);
  int nREsPerRB = dataTones.length();
  //! Assuming SRS is present in all RBS in the last symbol in all subframes (not true as per implementation) - Dhiv
  int rsCountPerRb=rbSize.numberOfSubcarriers;
  ivec portLocations=getIntegers(totReCountInRB-rsCountPerRb,totReCountInRB-1);
  cout<<"nREsPerRB"<<nREsPerRB<<endl;
  cout<<"totReCountInRB"<<totReCountInRB<<endl;
  cout<<"rsCountPerRb"<<rsCountPerRb<<endl;
  cout<<"portLocations"<<portLocations<<endl;
  Array< Array<cmat> > intrfrChannel; intrfrChannel.set_length(0);
  vec interfererTxSCPowerInDB;
  interfererTxSCPowerInDB.set_length(0);  
  ivec rbs=getIntegers(0,nRBs-1);
  Array<ivec> rbsForEachRBGroup=splitVectorToArray(rbs,rbGroupSize);
  ivec rbgsForCQI=getIntegers(0,rbGroupCount-1);
  
  
  /*cmat x1=sizeof(myChannel);
  cout <<"yessssssss=x1="<<x1<<endl;
  cmat x2=sizeof(myChannel(0));//sizeof(myChannel(0))/sizeof(myChannel(0)(0));
  //cout <<"yessssssss=x2="<<x2<<endl;
  //cmat x3=sizeof(myChannel(0)(0))/sizeof(myChannel(0)(0)(0));
  //cout <<"yessssssss=x3="<<x3<<endl;
  */
  //cout<<"yessssssssss"<<sizeof(myChannel[0])<<endl;
  #ifdef USING_SRS
  if(isSRSEnabled)
  {
    rbgsForCQI=findUniqueNumbers(srsRBs/rbGroupSize);
    portLocations=getIntegers(0,rbSize.numberOfSubcarriers-1,2)+(int)isOddSCs+(rbSize.numberOfSymbols-1)*rbSize.numberOfSubcarriers;
    rsCountPerRb = portLocations.length();
  }
  #endif
 
 
    int nT=myChannel.size();
    int nR=myChannel(0).size();
    cmat addchannelperRE=zeros_c(nR,nT),Precoder=zeros_c(nR,nR),U_ch,  V_ch,avgDLchannelPerRE=zeros_c(nT,nR);   ////// Doubt
      vec S_ch;
  
  
    
    for(int rbg_cnt=0;rbg_cnt<rbgsForCQI.length();rbg_cnt++)
    {
      int rbgIndx=rbgsForCQI(rbg_cnt);
      ivec rbsInRBGroup=rbsForEachRBGroup(rbgIndx);
      rsCountPerBand=rsCountPerRb*rbsInRBGroup.length();
      
      signalChannel.set_length(rsCountPerBand);
      totalRe_cnt=0;
      
      for(int rb_cnt=0;rb_cnt<rbsInRBGroup.length();rb_cnt++)
      {

	channelPerRE=getRawPhysicalChannel(resourceMapper,myChannel,rbsInRBGroup(rb_cnt),0,portLocations);
		
	for(int re_cnt=0;re_cnt <rsCountPerRb;re_cnt++, totalRe_cnt++)
	{
	  signalChannel(totalRe_cnt)=channelPerRE(re_cnt);	
	  // Channel is added across all REs
	  addchannelperRE=addchannelperRE + signalChannel(totalRe_cnt);	
	
	  std::ofstream fout("./Results/channelperRE.txt", std::ios::app);
	  fout<<"Raw physical channel per RE="<<signalChannel(totalRe_cnt)<<endl;
	  fout.close();
	}
      }
      avgDLchannelPerRE=hermitian_transpose(addchannelperRE)/totalRe_cnt;
      svd(avgDLchannelPerRE,U_ch,S_ch,V_ch); 
      Precoder=V_ch;
      std::ofstream fout("./Results/Precoder_per_RBG.txt", std::ios::app);
      fout<<"Precoder per RBG="<<Precoder<<endl;
      fout.close();
      reciprocityPrecodersInfo.precoderPerRBG(rbg_cnt)=Precoder;
    }  
  return(reciprocityPrecodersInfo); 
}


// Compute Reciprocity based precoder per RB from Downlink ideal channel
ReciprocityPrecodersInfo_S CQIEstimator::computeReciprocitybasedPrecoderFromIdealChannelForDownlink(DLCQIMeasurement_S &perUserFeedback, FrameStructure& frameStructure,DemodulationScheme_E demodulationScheme,PortToAntennaMapper_S *portToAntennaMapper,ReferenceSignal_S *refSignal,ResourceMapper *resourceMapper,Array<cmat> &rxFrame,Array< Array< Array<cmat> > > &allChannels,int desiredTxNode,ivec txNodeIDSForChannel,CQIMeasurementConfig_S &cqiMeasurementConfig,double noiseVariance, double restOfInterferenceInDB,int nodeID,int subframeNumber, string AperiodicCQILogsFileName)
{
  ReciprocityPrecodersInfo_S reciprocityPrecodersInfo ;
  TransmissionScheme_E txScheme=cqiMeasurementConfig.transmissionScheme;
  int nLayers=cqiMeasurementConfig.numberOfLayers;
  ivec cqiPorts=cqiMeasurementConfig.cqiPorts;
  int nCodewords=cqiMeasurementConfig.numberOfCodeWords;


  /// New for aperiodic CQI
  int desiredTxNodeIndx = find(txNodeIDSForChannel,desiredTxNode);

  if(desiredTxNodeIndx == -1)
  {
    cout<<"[detl:] desiredTxNode is not available in txNodeIDSForChannel, Aborting in computeCQIFromIdealChannelForDownlinkAperiodicCQIReport()"<<endl;
    cout<<"desiredTxNode : "<<desiredTxNode<<endl;
    cout<<"txNodeIDSForChannel : "<<txNodeIDSForChannel<<endl;
    abort();
  }

  double myTxSCPowerInDB=cqiMeasurementConfig.txSubcarrierPowerIndBm-30;
  ivec portLocations=getPortLocationsInRB(frameStructure, cqiPorts,0,true); // assuming ports are available in all RBs and taking locations from Oth RB
  int reCountPerRb=portLocations.length();
  int nPorts=cqiPorts.length();
  
  Array<string> rbName(cqiPorts.length());
  rbName.set_length(cqiPorts.length());
  for(int port_cnt=0;port_cnt<cqiPorts.length();port_cnt++)
    rbName(port_cnt)="port"+toString(cqiPorts(port_cnt));
  imat rsRBStructure;
  rsRBStructure=frameStructure.getMergedResourceUnit("nullRB",rbName,"data",cqiMeasurementConfig.rsSymPositions,cqiMeasurementConfig.rsScPositions);
  
  
  rsRBStructure = rsRBStructure.get_cols(3,rsRBStructure.cols()-1);
  ivec dataTones = find(rsRBStructure,0,true);
  int nREsPerRB = dataTones.length();

  // obtaining the channel matrix for a node under consideration
  
  Array< Array<cmat> > myChannel=allChannels(desiredTxNodeIndx);
  int rbs=resourceMapper->dlResourceMapper.getResourceBlocksPerSubframe();
  Size_S rbSize=resourceMapper->dlResourceMapper.getRBSize();
  Array<Array<ivec> > phyLoc;
  phyLoc.set_size(rbs);
  
  reciprocityPrecodersInfo.precoderPerRB.set_size(rbs); 
  reciprocityPrecodersInfo.vrbIndex=getIntegers(0,rbs-1); 
  ivec RBs= getIntegers(0,rbs-1); 
  for(int vrbNumber=0;vrbNumber<rbs;vrbNumber++)
  {
    phyLoc(vrbNumber)=resourceMapper->dlResourceMapper.getPRBSubcarrierStartLocationinEachSlot(vrbNumber);
  }
  generateReciprocityBasedPrecoderUsingSVD(reciprocityPrecodersInfo,myChannel,RBs,rbSize,phyLoc,portLocations,1); 
  return reciprocityPrecodersInfo;
}



// Compute Reciprocity based precoder per RB from Uplink idea channel
ReciprocityPrecodersInfo_S CQIEstimator::computeReciprocitybasedPrecoderFromIdealChannelForUplink(FrameStructure& frameStructure,DemodulationScheme_E demodulationScheme,PortToAntennaMapper_S *portToAntennaMapper,ResourceMapper *resourceMapper, Array< Array<cmat> > &myChannel, double myTxSCPowerInDB,TransmissionScheme_E txScheme, int nLayers,ivec cqiPorts , int nCodewords,double noiseVariance,double NIValueForTheServerNode,bool isDownlink,bool enableCQI0){
  
  cout<<"hey ! i entered in your module for calculating precoders.calculate now"<<endl;
  Size_S rbSize;
  Array<Array<ivec> > phyLoc;
  ReciprocityPrecodersInfo_S reciprocityPrecodersInfo;  
  /// Computing for all RB Groups irrespective of allocation Type...  
  // Assuming sounding on complete bandwidth part
  int nRBs=resourceMapper->ulResourceMapper.getResourceBlocksPerSubframe();
  rbSize=resourceMapper->ulResourceMapper.getRBSize();
//   cout<<"nRBs"<<nRBs<<endl;
 
  phyLoc.set_length(nRBs);
  reciprocityPrecodersInfo.precoderPerRB.set_size(nRBs); 
  reciprocityPrecodersInfo.vrbIndex=getIntegers(0,nRBs-1); 
  for(int vrbNumber=0;vrbNumber<nRBs;vrbNumber++)
  {
    phyLoc(vrbNumber)=resourceMapper->ulResourceMapper.getPRBSubcarrierStartLocationinEachSlot(vrbNumber);
  }
//   cout<<"resource blocks per subframe"<<nRBs<<endl;
  int rbGroupSize=resourceMapper->ulResourceMapper.getResourceBlockGroupingSize();
//   cout<<"rb_group_size"<<rbGroupSize<<endl;
  int rbGroupCount=nRBs/rbGroupSize;
//   cout<<"rb_group_count"<<rbGroupCount<<endl;
  if(rbGroupCount*rbGroupSize<nRBs) rbGroupCount+=1;
  int rsCountPerBand;
  DataBlkInfo_S dataBlkInfoInInterferingTx;
//   cout<<"rbSize.numberOfSubcarriers:"<<rbSize.numberOfSubcarriers<<"\t rbSize.numberOfSymbols:"<<rbSize.numberOfSymbols<<endl;
  int totReCountInRB=rbSize.numberOfSubcarriers*rbSize.numberOfSymbols;

  //! interfering channel info
  double weakIntrPwr = NIValueForTheServerNode-2*noiseVariance;  
  Array<string> rbName(cqiPorts.length());
  Array<ivec> rsPositions(cqiPorts.length());
  ivec scPositions=zeros_i(cqiPorts.length());
  for(int port_cnt=0;port_cnt<cqiPorts.length();port_cnt++)
  {
    rbName(port_cnt)="port"+toString(cqiPorts(port_cnt));
    
    if( txScheme==_TRANSMISSION_SCHEME_NR_CL_ ) // using dmrs ports as cqi ports . Need to change -  Dhiv
      rsPositions(port_cnt)="2";
  }
  
  imat rsRBStructure;
  rsRBStructure=frameStructure.getMergedResourceUnit("nullRB",rbName,"data",rsPositions,scPositions);
  rsRBStructure = rsRBStructure.get_cols(3,rsRBStructure.cols()-1);
  ivec dataTones = find(rsRBStructure,0,true);
  int nREsPerRB = dataTones.length();
    
  //!TODO Set portLocation to SRS REs in RB or any other pattern (RS)
  //! Assuming SRS is present in all RBS in the last symbol in all subframes (not true as per implementation) - Dhiv
  int rsCountPerRb=rbSize.numberOfSubcarriers;
  ivec portLocations=getIntegers(totReCountInRB-rsCountPerRb,totReCountInRB-1);
  Array< Array<cmat> > intrfrChannel; intrfrChannel.set_length(0);
  vec interfererTxSCPowerInDB;
  interfererTxSCPowerInDB.set_length(0);  
  ivec rbs=getIntegers(0,nRBs-1);
  
  
//   Array<ivec> rbsForEachRBGroup=splitVectorToArray(rbs,rbGroupSize);
  ivec rbgsForCQI=getIntegers(0,rbGroupCount-1);
  
  #ifdef USING_SRS
  if(isSRSEnabled)
  {
    rbgsForCQI=findUniqueNumbers(srsRBs/rbGroupSize);
    portLocations=getIntegers(0,rbSize.numberOfSubcarriers-1,2)+(int)isOddSCs+(rbSize.numberOfSymbols-1)*rbSize.numberOfSubcarriers;
    rsCountPerRb = portLocations.length();
  }
  #endif    
  generateReciprocityBasedPrecoderUsingSVD(reciprocityPrecodersInfo,myChannel,rbs,rbSize,phyLoc,portLocations,isDownlink);

  return(reciprocityPrecodersInfo); 
}

//Precoder calculation based on channel
void generateReciprocityBasedPrecoderUsingSVD(ReciprocityPrecodersInfo_S & reciprocityPrecodersInfo,Array< Array <cmat > >& channel,ivec vrbs,Size_S rbSize,Array<Array<ivec> > & prbSCStartLocInEachSlot,ivec portLocations,bool isDownlink)
{
  cmat addchannelperRE,Precoder,U_ch,V_ch,avgDLchannelPerRE;
  vec S_ch;  
  int nT=channel.size();
  int nR=channel(0).size();
  int nRBs =vrbs.length();
  Array<cmat> signalChannel(0);
  Array< cmat > channelPerRE; 
  int rsCountPerRb=rbSize.numberOfSubcarriers;
  
  // Calculating Precoder per RB
  for(int rb_cnt=0;rb_cnt<vrbs.length();rb_cnt++)
  {
    signalChannel.set_length(rsCountPerRb);   
    channelPerRE=getRawPhysicalChannel(channel,rbSize,prbSCStartLocInEachSlot(vrbs(rb_cnt)),isDownlink,portLocations);
    addchannelperRE=zeros_c(nR,nT);
    avgDLchannelPerRE=zeros_c(nT,nR);
    for(int re_cnt=0;re_cnt <portLocations.length();re_cnt++)
    {
      signalChannel(re_cnt)=channelPerRE(re_cnt);	
      // Channel is added across all REs in RB
      addchannelperRE=addchannelperRE + signalChannel(re_cnt);	
      
      std::ofstream fout("./Results/channelperRE.txt", std::ios::app);
      fout<<"Raw physical channel per RE="<<signalChannel(re_cnt)<<endl;
      fout.close();
    }
    avgDLchannelPerRE=(addchannelperRE)/rsCountPerRb;
    avgDLchannelPerRE=avgDLchannelPerRE.transpose();
    svd(avgDLchannelPerRE,U_ch,S_ch,V_ch); 
    Precoder=V_ch;
    std::ofstream fout("./Results/Precoder_per_RB_FromDLtoUL.txt", std::ios::app);
    fout<<"% Precoder per RB="<<"\n"<<Precoder<<endl;
    fout<<"% Eigen value= "<<"\n"<<S_ch<<endl;
    fout.close();
    reciprocityPrecodersInfo.precoderPerRB(rb_cnt)=Precoder;
  }
}
// Computes the postprocessing SINR per codeword as per the transmission scheme,the input ideal channel (desired / interferer) is precoded but not scaled with tx subcarrier power
Array<vec> calculateSinrWithIdealChannelperSubband(TransmissionScheme_E transmissionScheme, int nCodeWords, int nLayers, DemodulationScheme_E demodulationScheme,int nTones, Array<cmat> myPrecodedChannel, double sigpowInWatts, Array<cmat> &estimatedIntrCovariance)
{

  if(myPrecodedChannel.length() != estimatedIntrCovariance.length())
  {
    cout<<"myPrecodedChannel.length() : "<<myPrecodedChannel.length()<<endl;
    cout<<"estimatedIntrCovariance.length() : "<<estimatedIntrCovariance.length()<<endl;
    cout<<"[detl:]Precoded channel length and Interference Covariance length should match"<<endl;abort();
  }

  int noRxant;

  int totalRe_cnt;
  if(transmissionScheme!=_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
    noRxant= myPrecodedChannel(0).rows();
  else
    noRxant= myPrecodedChannel(0).rows()/nLayers;

  Array<vec> sinr(nLayers); // <layer<re>>
  for(int iLayer=0;iLayer<nLayers;iLayer++)
    sinr(iLayer).set_length(nTones);

  Array<vec> sinrPerCodeword(nCodeWords); //  <codeword<layer>>
  vec sinrtemp(nLayers);

  switch(transmissionScheme)
  {
    case _TRANSMISSION_SCHEME_SINGLE_PORT_CRS_:
    case _TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_:
    {

      if(nLayers!=1)
      {
	cout<<"[both:] Invalid cqiComputationInfo.numberOfLayers exiting in calculateSinrWithIdealChannelperSubband()..."<<endl;
	abort();
      }
      cmat chan(noRxant,1);

      for(int tone_cnt=0;tone_cnt <nTones;tone_cnt++)
      {
	for(int rxant=0;rxant<noRxant;rxant++)
	  chan(rxant,0) = sqrt(sigpowInWatts)*myPrecodedChannel(tone_cnt).get(rxant,0);

	estimatedIntrCovariance(tone_cnt)(0)=(complex< double >)real(estimatedIntrCovariance(tone_cnt)(0));
	sinrtemp = getSINRperLayer(demodulationScheme,chan,estimatedIntrCovariance(tone_cnt),nLayers,sigpowInWatts);
	for(int iLayer=0;iLayer<nLayers;iLayer++)
	  sinr(iLayer)[tone_cnt] = sinrtemp(iLayer);

      }
    }
    break;
    case _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_:
    {
      for(int tone_cnt=0;tone_cnt <nTones;tone_cnt++)
      {
	cmat chan = sqrt(sigpowInWatts)*(myPrecodedChannel(tone_cnt));
	sinrtemp = getSINRperLayer(demodulationScheme,chan,estimatedIntrCovariance(tone_cnt),nLayers,sigpowInWatts);

	for(int iLayer=0;iLayer<nLayers;iLayer++)
	  sinr(iLayer)[tone_cnt] = sinrtemp(iLayer);
      }

    }
    break;
    case _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_:
    case _TRANSMISSION_SCHEME_CL_BF_:
    case _TRANSMISSION_SCHEME_CL_SM_:
    case _TRANSMISSION_SCHEME_CL_MU_MIMO_:
    case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
    case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
    case _TRANSMISSION_SCHEME_COMP_:
    case _TRANSMISSION_SCHEME_OLMUMIMO_:
    case _TRANSMISSION_SCHEME_NR_CL_:
    {

      for(int tone_cnt=0;tone_cnt <nTones;tone_cnt++)
      {
	cmat chan = sqrt(sigpowInWatts)*myPrecodedChannel(tone_cnt);
	sinrtemp = getSINRperLayer(demodulationScheme,chan,estimatedIntrCovariance(tone_cnt),nLayers,sigpowInWatts);
	for(int iLayer=0;iLayer<nLayers;iLayer++)
	  sinr(iLayer)[tone_cnt] = sinrtemp(iLayer);
      }
    }
    break;

    default:
    {
      cout<<"[both:] Undefined Transmission scheme .... exiting in calculateSinrWithIdealChannelperSubband()"<<endl;abort();
    }
  }

  // Post processing SINR per layer is mapped to codeword
  switch(nCodeWords)
  {
    case 1 :
      sinrPerCodeword(0).set_length(nLayers*sinr(0).length());
      totalRe_cnt = 0;
      for(int cnt = 0; cnt < sinr(0).length(); cnt++)
      {
	for(int iLayer=0;iLayer<nLayers;iLayer++)
	{
	  sinrPerCodeword(0)(totalRe_cnt) = sinr(iLayer)(cnt);
	  totalRe_cnt++;
	}
      }
      break;
    case 2 :
      switch(transmissionScheme)
      {
	case _TRANSMISSION_SCHEME_CL_SM_:
	case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
	case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
	case _TRANSMISSION_SCHEME_COMP_:
	case _TRANSMISSION_SCHEME_OLMUMIMO_:
	case _TRANSMISSION_SCHEME_NR_CL_:
	{
	  switch(nLayers)
	  {
	    case 2:
	      sinrPerCodeword(0)=sinr(0);
	      sinrPerCodeword(1)=sinr(1);
	      break;
	    case 3:
	      sinrPerCodeword(0)=sinr(0);
	      append(sinrPerCodeword(1),sinr(1),sinr(2));
	      break;
	    case 4:
	      append(sinrPerCodeword(0),sinr(0),sinr(1));
	      append(sinrPerCodeword(1),sinr(2),sinr(3));
	      break;
	    case 5:
	      append(sinrPerCodeword(0),sinr(0),sinr(1));
	      append(sinrPerCodeword(1),sinr(2),sinr(3),sinr(4));
	      break;
	    case 6:
	      append(sinrPerCodeword(0),sinr(0),sinr(1),sinr(2));
	      append(sinrPerCodeword(1),sinr(3),sinr(4),sinr(5));
	      break;
	    case 7:
	      append(sinrPerCodeword(0),sinr(0),sinr(1),sinr(2));
	      append(sinrPerCodeword(1),sinr(3),sinr(4),sinr(5),sinr(6));
	      break;
	    case 8:
	      append(sinrPerCodeword(0),sinr(0),sinr(1),sinr(2),sinr(3));
	      append(sinrPerCodeword(1),sinr(4),sinr(5),sinr(6),sinr(7));
	      break;
	    default:
	      cout<<"[both:] Error: nLayers > 8 not supported in calculateSinrWithIdealChannelperSubband"<<endl;abort();
	  }
	  break;
	case _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_:
	  sinrPerCodeword(0).set_length(nLayers*sinr(0).length());
	  totalRe_cnt = 0;
	  for(int cnt = 0; cnt < sinr(0).length(); cnt++)
	  {
	    for(int iLayer=0;iLayer<nLayers;iLayer++)
	    {
	      sinrPerCodeword(0)(totalRe_cnt) = sinr(iLayer)(cnt);
	      totalRe_cnt++;
	    }
	  }
	  sinrPerCodeword(1) = sinrPerCodeword(0);

	  break;
	default:
	  cout<<"[both:] Error: transmissionScheme is not supported in calculateSinrWithIdealChannelperSubband"<<endl;abort();
      }
    }
      break;
    default:
      cout<<"[both:] Error: #Codewords not supported in calculateSinrWithIdealChannelperSubband"<<endl;abort();
  }

  return(sinrPerCodeword);
}
/*
Array<cmat> calculateIdealInterferenceCoVariance(ResourceMapper *resourceMapper,Array<Framehandler*> interferingTxFrameHandlers,ivec interfererChannelIndices,PortToAntennaMapper_S *portToAntennaMapper ,Array< Array< Array<cmat> > > &allChannels, int vrbNumber,bool isDownlink,int reCountPerRb,int numOfRbs,int nRxAntenna,int nLayers,ivec positionsToConsider,TransmissionScheme_E transmissionScheme, double noiseVarWithROI, vec interfererTxScPowInWatts)
{
  int nTones = reCountPerRb*numOfRbs;
  Array< Array<cmat> > IntfPrecodedChannel;
  CQIEstimator::getPrecodedIntrfrChannelAcrossRE(IntfPrecodedChannel,resourceMapper,interferingTxFrameHandlers,interfererChannelIndices,portToAntennaMapper,allChannels,vrbNumber,isDownlink,reCountPerRb,numOfRbs,nRxAntenna,nLayers,positionsToConsider);

  Array<cmat> IntrCov(nTones);
  cmat noise;

  int nInterferers=interfererTxScPowInWatts.length();

  switch(transmissionScheme)
  {
    case _TRANSMISSION_SCHEME_SINGLE_PORT_CRS_:
    case _TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_:
    {

      if(nLayers!=1)
      {cout<<"[both:] Invalid nLayers exiting in calculateIdealInterferenceCoVariance()..."<<endl;abort();}

      noise = noiseVarWithROI*eye_c(nRxAntenna);

      for(int tone_cnt=0;tone_cnt <nTones;tone_cnt++)
      {
	IntrCov(tone_cnt) = zeros_c(nRxAntenna,nRxAntenna);

	for(int intr=0;intr <nInterferers;intr++)
	  IntrCov(tone_cnt) = IntrCov(tone_cnt) + interfererTxScPowInWatts(intr)*IntfPrecodedChannel(intr)(tone_cnt)*IntfPrecodedChannel(intr)(tone_cnt).hermitian_transpose();

	IntrCov(tone_cnt) += noise;

	for(int rx_ant = 0; rx_ant < nRxAntenna; rx_ant++)
	  IntrCov(tone_cnt)(rx_ant,rx_ant) = (complex< double >)real(IntrCov(tone_cnt)(rx_ant,rx_ant));

      }

    }
    break;
    case _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_:
    {
      noise = noiseVarWithROI*eye_c(nRxAntenna*nLayers);
      for(int tone_cnt=0;tone_cnt <nTones;tone_cnt++)
      {
	IntrCov(tone_cnt)=zeros_c(nRxAntenna*nLayers,nRxAntenna*nLayers);

	for(int intr=0;intr<nInterferers;intr++)
	{
	  cmat intrCov;
	  if(IntfPrecodedChannel(intr)(tone_cnt).rows()!=nRxAntenna)
	  {
	    int intrLayers=IntfPrecodedChannel(intr)(tone_cnt).rows()/nRxAntenna;
	    if(nLayers==intrLayers)
	    {
	      intrCov=interfererTxScPowInWatts(intr)*IntfPrecodedChannel(intr)(tone_cnt)*IntfPrecodedChannel(intr)(tone_cnt).H();
	    }
	    else
	    {
	      intrCov=interfererTxScPowInWatts(intr)*IntfPrecodedChannel(intr)(tone_cnt)*IntfPrecodedChannel(intr)(tone_cnt).H();
	      if(nLayers==2 && intrLayers==4)
		intrCov=(intrCov.get_cols(0,nRxAntenna*nLayers-1)).get_rows(0,nRxAntenna*nLayers-1);
	      else //(nLayers==4 && intrLayers==2)
		intrCov=formBlockMatrix(intrCov,intrCov);
	    }
	  }
	  else
	  {
	    cmat tempChan=IntfPrecodedChannel(intr)(tone_cnt);
	    cmat intrCov1=interfererTxScPowInWatts(intr)*tempChan*tempChan.hermitian_transpose();
	    cmat intrCov2=conj(intrCov1); // conj as we take conjugate for the second layer while precoding/detection
	    intrCov=formBlockMatrix(intrCov1,intrCov2);
	    if(nLayers==4)
	      intrCov=formBlockMatrix(intrCov,intrCov);
	  }
	  IntrCov(tone_cnt) +=intrCov;
	}

	IntrCov(tone_cnt)+=noise;

	for(int rx_ant = 0; rx_ant < nRxAntenna*nLayers; rx_ant++)
	  IntrCov(tone_cnt)(rx_ant,rx_ant) = (complex< double >)real(IntrCov(tone_cnt)(rx_ant,rx_ant));

      }

    }
    break;
    case _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_:
    case _TRANSMISSION_SCHEME_CL_BF_:
    case _TRANSMISSION_SCHEME_CL_SM_:
    case _TRANSMISSION_SCHEME_CL_MU_MIMO_:
    case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
    case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
    case _TRANSMISSION_SCHEME_COMP_:
    case _TRANSMISSION_SCHEME_OLMUMIMO_:
    case _TRANSMISSION_SCHEME_NR_CL_:
    {
      noise = noiseVarWithROI*eye_c(nRxAntenna);

      for(int tone_cnt=0;tone_cnt <nTones;tone_cnt++)
      {
	IntrCov(tone_cnt) = zeros_c(nRxAntenna,nRxAntenna);

	for(int intr=0;intr <nInterferers;intr++)
	{
	  cmat intrchan=IntfPrecodedChannel(intr)(tone_cnt).get_rows(0,nRxAntenna-1);
	  IntrCov(tone_cnt) += interfererTxScPowInWatts(intr)*intrchan*intrchan.hermitian_transpose();
	}
	IntrCov(tone_cnt) += noise;

	for(int rx_ant = 0; rx_ant < nRxAntenna; rx_ant++)
	  IntrCov(tone_cnt)(rx_ant,rx_ant) = (complex< double >)real(IntrCov(tone_cnt)(rx_ant,rx_ant));

      }
    }
    break;

    default:
    {
      cout<<"[both:] Undefined Transmission scheme .... Aborting in calculateIdealInterferenceCoVariance()"<<endl;abort();
    }
  }

  return IntrCov;
}*/

Array<vec> calculateSinrWithIdealChannelForAbstraction(TransmissionScheme_E transmissionScheme, int nCodeWords, int nLayers, DemodulationScheme_E demodulationScheme,ivec intrSFBCLayers, Array<cmat> myPrecodedChannel, Array< Array<cmat> > IntfPrecodedChannel, double noiseVarWithROI, double sigpowInWatts, vec interfererTxScPowInWatts,bool isEstimatedCovariance, Array<cmat> &estimatedIntrCovariance)
{

  int noRxant;
  if(transmissionScheme!=_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
    noRxant= myPrecodedChannel(0).rows();
  else
    noRxant= myPrecodedChannel(0).rows()/nLayers;

  int nInterferers=interfererTxScPowInWatts.length();
  int totalRe_cnt;

  DemodulationScheme_E desc=demodulationScheme;
  Array<vec> sinr(nLayers); // <layer<re>>
  Array<vec> cci(nLayers); // not used
  Array<vec> sinrPerCodeword(nCodeWords); //  <codeword<layer>>
  vec sinrtemp(nLayers);
  int nTones=myPrecodedChannel.length();
  switch(transmissionScheme)
  {
    case _TRANSMISSION_SCHEME_SINGLE_PORT_CRS_:
    case _TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_:
    {
      for(int iLayer=0;iLayer<nLayers;iLayer++)
	sinr(iLayer).set_length(nTones);
      if(nLayers!=1)
      {cout<<"[both:] Invalid cqiComputationInfo.numberOfLayers exiting in calculateSinrWithIdealChannelperSubband()..."<<endl;abort();}
      cmat IntrCov(noRxant,noRxant);
      cmat chan(noRxant,1);// <nRx,nLayer=1>
      cmat noise;// = noiseVarWithROI*eye_c(noRxant);

      for(int tone_cnt=0;tone_cnt <nTones;tone_cnt++)
      {
	noise = noiseVarWithROI*eye_c(noRxant);
	IntrCov.clear();
	// scaling the precoded channel with signal power
	for(int rxant=0;rxant<noRxant;rxant++)
	{
	  chan(rxant,0) = sqrt(sigpowInWatts)*myPrecodedChannel(tone_cnt).get(rxant,0);
	}
	if(!isEstimatedCovariance)
	{
	    for(int intr=0;intr <nInterferers;intr++)
	    {
	      if(intrSFBCLayers(intr))
	      {
		int intrLayers=intrSFBCLayers(intr);
		int scindx=tone_cnt/intrLayers;
		int scLocalIndx=tone_cnt%intrLayers;
		cmat intrChannel=sqrt(interfererTxScPowInWatts(intr))*IntfPrecodedChannel(intr)(scindx);
		cmat interferenceCov=intrChannel*intrChannel.H();

		interferenceCov=interferenceCov.get_cols(scLocalIndx*noRxant,(scLocalIndx+1)*noRxant-1);
		interferenceCov = interferenceCov.get_rows(scLocalIndx*noRxant,(scLocalIndx+1)*noRxant-1);

		if(scLocalIndx==1 || scLocalIndx==3)
		  interferenceCov=conj(interferenceCov);

		IntrCov = IntrCov + interferenceCov;
	      }
	      else
	      {
		cmat intrChannel=sqrt(interfererTxScPowInWatts(intr))*IntfPrecodedChannel(intr)(tone_cnt);
		IntrCov = IntrCov + intrChannel*intrChannel.hermitian_transpose();
	      }
	    }
	    IntrCov+=noise;
	    noise.clear();
	}
	else
	{
	  IntrCov=estimatedIntrCovariance(tone_cnt);
	  noise.clear();
	}
	sinrtemp = getSINRperLayer(desc,chan,IntrCov,nLayers,sigpowInWatts);
	for(int iLayer=0;iLayer<nLayers;iLayer++)
	{
	  sinr(iLayer)[tone_cnt] = sinrtemp(iLayer);
	}
      }

    }
    break;
    case _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_:
    {
      for(int iLayer=0;iLayer<nLayers;iLayer++)
	sinr(iLayer).set_length(nTones);
      // the precoded channel is scaled with signal power and coded with SFBC
      for(int tone_cnt=0;tone_cnt <nTones;tone_cnt++)
      {
	cmat chan = sqrt(sigpowInWatts)*(myPrecodedChannel(tone_cnt));

	cmat IntrCov=zeros_c(noRxant*nLayers,noRxant*nLayers);
	cmat noise = noiseVarWithROI*eye_c(noRxant*nLayers);
	if(!isEstimatedCovariance)
	{
	    for(int intr=0;intr<nInterferers;intr++)
	    {
	      cmat interferenceCov;
	      if(intrSFBCLayers(intr))
	      {
		int intrLayers=intrSFBCLayers(intr);
		if(intrLayers==nLayers)
		{
		  cmat intrprecodedChannel=sqrt(interfererTxScPowInWatts(intr))*IntfPrecodedChannel(intr)(tone_cnt);
		  interferenceCov=intrprecodedChannel*intrprecodedChannel.H();
		}
		else
		{
		  if(intrLayers==4 && nLayers==2)
		  {
		    cmat intrprecodedChannel=sqrt(interfererTxScPowInWatts(intr))*IntfPrecodedChannel(intr)(tone_cnt/2);
		    interferenceCov=intrprecodedChannel*intrprecodedChannel.H();
		    if(tone_cnt%2==0)
		    {
		      interferenceCov=interferenceCov.get_cols(0,interferenceCov.cols()/2-1);
		      interferenceCov = interferenceCov.get_rows(0,interferenceCov.rows()/2-1);
		    }
		    else
		    {
		      interferenceCov=interferenceCov.get_cols(interferenceCov.cols()/2,interferenceCov.cols()-1);
		      interferenceCov = interferenceCov.get_rows(interferenceCov.rows()/2,interferenceCov.rows()-1);
		    }
		  }
		  else
		  {
		    cmat tempChannel1=IntfPrecodedChannel(intr)(2*tone_cnt);
		    cmat tempChannel2=IntfPrecodedChannel(intr)(2*tone_cnt+1);
		    cmat intrprecodedChannel=sqrt(interfererTxScPowInWatts(intr))*formBlockMatrix(tempChannel1,tempChannel2);
		    interferenceCov=intrprecodedChannel*intrprecodedChannel.H();
		  }
		}
	      }
	      else
	      {
		Array<cmat> tempChannel(nLayers);
		for(int l_cnt=0;l_cnt<nLayers;l_cnt++)
		  tempChannel(l_cnt)=IntfPrecodedChannel(intr)(nLayers*tone_cnt+l_cnt);

		cmat interferenceCov1=tempChannel(0)*tempChannel(0).H();
		cmat interferenceCov2=tempChannel(1)*tempChannel(1).H();
		interferenceCov2=conj(interferenceCov2);
		cmat intrCov1=formBlockMatrix(interferenceCov1,interferenceCov2);
		interferenceCov=interfererTxScPowInWatts(intr)*intrCov1;
		if(nLayers==4)
		{
		  cmat interferenceCov3=tempChannel(2)*tempChannel(2).H();
		  cmat interferenceCov4=tempChannel(3)*tempChannel(3).H();
		  interferenceCov4=conj(interferenceCov4);
		  cmat intrCov2=formBlockMatrix(interferenceCov3,interferenceCov4);
		  interferenceCov=interfererTxScPowInWatts(intr)*formBlockMatrix(intrCov1,intrCov2);
		}
	      }

	      IntrCov += interferenceCov;
	    }

	    IntrCov+=noise;
	    noise.clear();
	  }
	else
	{
	  IntrCov=estimatedIntrCovariance(tone_cnt);
	  noise.clear();
	}

	sinrtemp = getSINRperLayer(desc,chan,IntrCov,nLayers,sigpowInWatts);

	for(int iLayer=0;iLayer<nLayers;iLayer++)
	  sinr(iLayer)[tone_cnt] = sinrtemp(iLayer);
      }

    }
    break;
    case _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_:
    case _TRANSMISSION_SCHEME_CL_BF_:
    case _TRANSMISSION_SCHEME_CL_SM_:
    case _TRANSMISSION_SCHEME_CL_MU_MIMO_:
    case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
    case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
    case _TRANSMISSION_SCHEME_COMP_:
    case _TRANSMISSION_SCHEME_OLMUMIMO_:
    case _TRANSMISSION_SCHEME_NR_CL_:
    {
      sinr.set_size(nLayers);
      for(int iLayer=0;iLayer<nLayers;iLayer++)
	sinr(iLayer).set_size(nTones);
      cmat IntrCov(noRxant,noRxant);
      cmat chan;chan.set_size(noRxant,nLayers);
      cmat noise;

      // scaling the signal and interferer precoded channel with respective powers
      for(int tone_cnt=0;tone_cnt <nTones;tone_cnt++)
      {
	noise = noiseVarWithROI*eye_c(noRxant);
	for(int rxant=0;rxant<noRxant;rxant++)
	  for(int iLayer=0;iLayer<nLayers;iLayer++)
	    chan(rxant,iLayer) = sqrt((double)sigpowInWatts)*myPrecodedChannel(tone_cnt).get(rxant,iLayer);

	  if(!isEstimatedCovariance)
	  {
	    //if(nInterferers)
	    //{
	      IntrCov.clear();
	      for(int intr=0;intr <nInterferers;intr++)
	      {
		if(intrSFBCLayers(intr))
		{
		  int intrLayers=intrSFBCLayers(intr);
		  int scindx=tone_cnt/intrLayers;
		  int scLocalIndx=tone_cnt%intrLayers;
		  cmat intrChannel=sqrt(interfererTxScPowInWatts(intr))*IntfPrecodedChannel(intr)(scindx);
		  cmat interferenceCov=intrChannel*intrChannel.H();

		  interferenceCov=interferenceCov.get_cols(scLocalIndx*noRxant,(scLocalIndx+1)*noRxant-1);
		  interferenceCov = interferenceCov.get_rows(scLocalIndx*noRxant,(scLocalIndx+1)*noRxant-1);

		  if(scLocalIndx==1 || scLocalIndx==3)
		    interferenceCov=conj(interferenceCov);

		  IntrCov = IntrCov + interferenceCov;
		}
		else
		{
		  cmat intrChannel=sqrt(interfererTxScPowInWatts(intr))*IntfPrecodedChannel(intr)(tone_cnt);
		  IntrCov = IntrCov + intrChannel*intrChannel.hermitian_transpose();
		}
	      }
	      IntrCov+=noise;
	      noise.clear();
	    //}
	  }
	  else
	  {
	    IntrCov=estimatedIntrCovariance(tone_cnt);
	    noise.clear();
	  }

	  sinrtemp = getSINRperLayer(desc,chan,IntrCov,nLayers,sigpowInWatts);

	  for(int iLayer=0;iLayer<nLayers;iLayer++)
	    sinr(iLayer)[tone_cnt] = sinrtemp(iLayer);
      }

    }
    break;

    default:
    {
      cout<<"[both:] Undefined Transmission scheme .... exiting in calculateSinrWithIdealChannelperSubband()"<<endl;abort();
    }
  }


  // Post processing SINR per layer is mapped to codeword
  switch(nCodeWords)
  {
    case 1 :
      for(int iLayer=0;iLayer<nLayers;iLayer++)
	append(sinrPerCodeword(0),sinr(iLayer));
      break;

    case 2 :
      switch(transmissionScheme)
      {
	case _TRANSMISSION_SCHEME_CL_SM_:
	case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
	case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
	case _TRANSMISSION_SCHEME_COMP_:
	case _TRANSMISSION_SCHEME_OLMUMIMO_:
	case _TRANSMISSION_SCHEME_NR_CL_:
	  switch(nLayers)
	  {
	    case 2:
	      sinrPerCodeword(0)=sinr(0);
	      sinrPerCodeword(1)=sinr(1);
	      break;
	    case 3:
	      sinrPerCodeword(0)=sinr(0);
	      append(sinrPerCodeword(1),sinr(1),sinr(2));
	      break;
	    case 4:
	      append(sinrPerCodeword(0),sinr(0),sinr(1));
	      append(sinrPerCodeword(1),sinr(2),sinr(3));
	      break;
	    case 5:
	      append(sinrPerCodeword(0),sinr(0),sinr(1));
	      append(sinrPerCodeword(1),sinr(2),sinr(3),sinr(4));
	      break;
	    case 6:
	      append(sinrPerCodeword(0),sinr(0),sinr(1),sinr(2));
	      append(sinrPerCodeword(1),sinr(3),sinr(4),sinr(5));
	      break;
	    case 7:
	      append(sinrPerCodeword(0),sinr(0),sinr(1),sinr(2));
	      append(sinrPerCodeword(1),sinr(3),sinr(4),sinr(5),sinr(6));
	      break;
	    case 8:
	      append(sinrPerCodeword(0),sinr(0),sinr(1),sinr(2),sinr(3));
	      append(sinrPerCodeword(1),sinr(4),sinr(5),sinr(6),sinr(7));
	      break;
	    default:
	      cout<<"[both:] Error: nLayers > 8 not supported in calculateSinrWithIdealChannelperSubband"<<endl;abort();
	  }
	  break;
	case _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_:
	  sinrPerCodeword(0).set_length(nLayers*sinr(0).length());
	  totalRe_cnt = 0;
	  for(int cnt = 0; cnt < sinr(0).length(); cnt++)
	  {
	    for(int iLayer=0;iLayer<nLayers;iLayer++)
	    {
	      sinrPerCodeword(0)(totalRe_cnt) = sinr(iLayer)(cnt);
	      totalRe_cnt++;
	    }
	  }
	  sinrPerCodeword(1) = sinrPerCodeword(0);
	  break;
	default:
	  cout<<"[both:] Error: transmissionScheme is not supported in calculateSinrWithIdealChannelperSubband"<<endl;abort();
      }
      break;
    default:
      cout<<"[both:] Error: #Codewords not supported in calculateSinrWithIdealChannelperSubband"<<endl;abort();
  }
  return(sinrPerCodeword);
}

cmat computePostProcessingChannelCoeffForUplink(int nLayers, DemodulationScheme_E demodulationScheme,int nTones, Array<cmat> myPrecodedChannel, Array< Array<cmat> > IntfPrecodedChannel, cmat ROICov, double sigpowInWatts, vec interfererTxScPowInWatts,cmat& ppIntrCovChannel)
{
  //!This module assumes the input is always in the same OFDM symbol and also over a single allocation (DFT taken over the entire vec) - 10010
  
  int noRxant = myPrecodedChannel(0).rows();
  int nInterferers=interfererTxScPowInWatts.length();
  
  DemodulationScheme_E desc=demodulationScheme;
  cmat ppChannel=zeros_c(nLayers,nLayers);
  cmat WFilter;
  
  
  cmat IntrCov(noRxant,noRxant);
  cmat chan(noRxant,nLayers);// <nRx,nLayer=1>
  cmat noise = ROICov;
  complex< double > sigPow=complex< double >(sigpowInWatts,0);
  
  for(int tone_cnt=0;tone_cnt <nTones;tone_cnt++)
  {
    IntrCov.clear();
    chan=myPrecodedChannel(tone_cnt);
    if(nInterferers)
    {
      for(int intr=0;intr <nInterferers;intr++)
	IntrCov = IntrCov + interfererTxScPowInWatts(intr)*IntfPrecodedChannel(intr)(tone_cnt)*IntfPrecodedChannel(intr)(tone_cnt).hermitian_transpose();
    }
    if(desc==_DEMODULATION_SCHEME_MMSE_)
      WFilter=sigPow*chan.H()*inv(chan*sigPow*chan.hermitian_transpose() + IntrCov + noise);
    else if(desc==_DEMODULATION_SCHEME_MRC_)
      WFilter=chan.hermitian_transpose(); 
    else if(desc==_DEMODULATION_SCHEME_WHITE_MRC_)
    {
      complex< double > noiseVariance= (trace(IntrCov+noise)/complex< double >(noise.rows(),0));// Taking avg interference power across antennas
      WFilter  =  sigPow*chan.H()/(sigPow*((chan.H()*chan)(0)) + noiseVariance);
    }
    else
    {
      cout<<"[detl:]Unknown DemodulationScheme_E to calculateSinrWithIdealChannelperSubbandForUplink()"<<endl;
      abort();
    }
    ppChannel+=WFilter*chan;
    ppIntrCovChannel+=WFilter*(IntrCov+noise)*WFilter.hermitian_transpose();
  }
  ppChannel/=nTones;
  ppIntrCovChannel/=nTones;
  
  return ppChannel;
  
}

Array<vec> calculateSinrWithIdealChannelperSubbandForSCFDMA(TransmissionScheme_E transmissionScheme, int nCodeWords, int nLayers, DemodulationScheme_E demodulationScheme,int nTones, Array<cmat> myPrecodedChannel, Array< Array<cmat> > IntfPrecodedChannel, cmat ROICov, double sigpowInWatts, vec interfererTxScPowInWatts)
{
  
  //!This module assumes the input is always in the same OFDM symbol and also over a single allocation (DFT taken over the entire vec) - Dhiv
  
  DemodulationScheme_E desc=demodulationScheme;
  Array<vec> sinr(nLayers); // <layer<re>>
  Array<vec> sinrPerCodeword(nCodeWords); //  <codeword<layer>>
  vec sinrtemp(nLayers);
  cmat ppChannel=zeros_c(nLayers,nLayers);
  cmat ppIntrCovChannel=zeros_c(nLayers,nLayers);
  cmat WFilter;
  for(int iLayer=0;iLayer<nLayers;iLayer++)
    sinr(iLayer).set_length(nTones);
  
  ppChannel = computePostProcessingChannelCoeffForUplink(nLayers, demodulationScheme,nTones, myPrecodedChannel, IntfPrecodedChannel, ROICov, sigpowInWatts, interfererTxScPowInWatts, ppIntrCovChannel);
  
  vec ppSINR(nLayers);
  if(desc==_DEMODULATION_SCHEME_MMSE_)
  {
    for(int iLayer=0;iLayer<nLayers;iLayer++)
    {
      ppSINR(iLayer)=real(ppChannel(iLayer,iLayer))/(1.0-real(ppChannel(iLayer,iLayer)));
    }
  }
//   else if(desc==_DEMODULATION_SCHEME_MRC_ || desc==_DEMODULATION_SCHEME_WHITE_MRC_)
//   {
//     for(int iLayer=0;iLayer<nLayers;iLayer++)
//     {
//       cvec tempChannel=ppChannel.get_col(iLayer);
//       double sigPower=sigpowInWatts*sqr(norm(tempChannel));
//       double intrfPower=abs(ppIntrCovChannel(iLayer,iLayer));
//       for(int iLayer1=0;iLayer1<nLayers;iLayer1++)
//       {
// 	if(iLayer!=iLayer1)
// 	{
// 	  tempChannel=ppChannel.get_col(iLayer1);
// 	  intrfPower+=sigpowInWatts*sqr(norm(tempChannel));
// 	}
//       }
//       ppSINR(iLayer)=sigPower/intrfPower;
//     }
//   }
  else
  {
    cout<<"[detl:]Unknown DemodulationScheme_E to calculateSinrWithIdealChannelperSubbandForUplink()"<<endl;
    abort();
  }
  for(int tone_cnt=0;tone_cnt <nTones;tone_cnt++)
    for(int iLayer=0;iLayer<nLayers;iLayer++)
      sinr(iLayer)[tone_cnt] = ppSINR(iLayer);
    
    // Post processing SINR per layer is mapped to codeword
    switch(nCodeWords)
    {
      case 1 :
	for(int iLayer=0;iLayer<nLayers;iLayer++)
	  append(sinrPerCodeword(0),sinr(iLayer));
	break;
	
      case 2 :
	switch(transmissionScheme)
	{
	  case _TRANSMISSION_SCHEME_CL_SM_:
	  case _TRANSMISSION_SCHEME_NR_CL_:
	    
	    //Table 7.3.1.3-1: Codeword-to-layer mapping for spatial multiplexing
	    
	    
	    ivec layersPerCW(nCodeWords);
	    layersPerCW=nLayers/nCodeWords;
	    if(sum(layersPerCW)!=nLayers)
	      layersPerCW(1)+=1;
	    
	    for(int cwd_cnt=0,tot_layer=0;cwd_cnt<nCodeWords;cwd_cnt++)
	      for(int iLayer=0;iLayer<layersPerCW(cwd_cnt);iLayer++,tot_layer++)
		append(sinrPerCodeword(cwd_cnt),sinr(tot_layer));
	      
	    break;
	}
	break;
    }
    return(sinrPerCodeword);
}

ivec getPortLocationsInRB(FrameStructure &frameStructure, ivec requiredPorts,int RBNumber, bool excludeCCHCRS)
{
  ivec portLocations(0);string toneType;
  for(int cnt=0;cnt<requiredPorts.length();cnt++)
  {
    toneType="port"+toString(requiredPorts(cnt));
    append(portLocations,frameStructure.getTones(RBNumber,toneType)); //Indices of the ports in RB
    if(requiredPorts(cnt)<4 && excludeCCHCRS)
    {
      imat RBStructure = frameStructure.getResourceUnitMatrix(RBNumber);
      RBStructure = RBStructure.get_cols(0,1);

      portLocations = remove(portLocations,find(RBStructure,requiredPorts(cnt)+1,true)); //Indices of the ports in RB
    }
  }
  portLocations=findUniqueNumbers(portLocations);
  sort(portLocations);
  return portLocations;
}

void CQIEstimator::ManageDLCQIReportTrigger(int DLRBs,int subframeNumber,bool isAperiodicCQICompEnabled){
  
  // #ifdef USING_CALIBRATION 
  if(isAperiodicCQICompEnabled==true) 
  {    
    downlinkCQIInfo->cqiComputationInfo.isPeriodicConfigured=false;
    if (downlinkCQIInfo->cqiComputationInfo.isToTransmitCQIPorts(subframeNumber))
    {
      downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered = true;
      downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQIConfigured = true;
      if(downlinkCQIInfo->cqiComputationInfo.isRankAdaptationEnabled)
      {	  
	{
	  downlinkCQIInfo->cqiComputationInfo.isrankTrigger=true;
	  downlinkCQIInfo->cqiComputationInfo.isRankComputed=true;	     
	  downlinkCQIInfo->perUserFeedback.isRankFeedback=true;	      
	}
      }
    }
    else
    {
      downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered = false;
      downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQIConfigured = false;
      downlinkCQIInfo->cqiComputationInfo.isrankTrigger=false;
    }
  }
  // #endif
  if(downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered)
  {
    if(downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQIConfigured)
    {
      if(downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_1_2_||downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_3_0_||downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_3_1_||downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_3_2_)
	downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic=getSubbandSizeAperiodic(DLRBs);
      else
	downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected=getSubbandSizeAperiodicUESelected(DLRBs); // since the subband size for higher layer configured mode (mode 3_0 and mode 3_1)
	if(downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_2_0_||downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_2_2_)
	{
	  if(DLRBs<8)
	  {
	    cout<<"Given Aperiodic mode not supported for this Bandwidth"<<endl;
	    exit(0);
	  }
	  downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands=getNumberOfUESelectedSubbands(DLRBs);
	}
	downlinkCQIInfo->subframeNumberToReportAperiodicCQI=subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay+downlinkCQIInfo->cqiComputationInfo.CQIProcessingDelay;// subframenumber is the subframe where CQI computation is triggered
	downlinkCQIInfo->perUserFeedback.APeriodicReportedSubframe=subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay+downlinkCQIInfo->cqiComputationInfo.CQIProcessingDelay;
	
    }
    else
    {
      downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered=false;
      cout<<"APeriodicCQIConfigured not configured but APeriodicCQITriggered "<<endl;
    }
   // if(downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length()==0)
        
  }
   
  //Periodic mode trigger commented as Periodic Mode CQI is not yet ready.
  //While uncommenting need to take care of Rank Periodicity and Rank Offset - Dhiv
   
//     if(downlinkCQIInfo->cqiComputationInfo.isPeriodicConfigured)
//     {
//       
//       if(downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_2_0_||downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_2_1_)
// 	if(downlinkCQIInfo->cqiComputationInfo.numberOfBWParts<=0){// initializing the variables as per the configuration
// 	  getSubbandsAndBWParts(downlinkCQIInfo->cqiComputationInfo.subbandSizePeriodic,downlinkCQIInfo->cqiComputationInfo.numberOfBWParts,DLRBs);// number of Bandwidth parts and subband size based on system bandwidth
// 	  for(int cnt=0;cnt<downlinkCQIInfo->perUserFeedback.dlCQIMeasurement.length();cnt++){
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(cnt).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord.set_size(downlinkCQIInfo->cqiComputationInfo.numberOfBWParts);// <BWPart<nCodeword>>
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(cnt).subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart.set_size(downlinkCQIInfo->cqiComputationInfo.numberOfBWParts);// <BWPart>
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(cnt).subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart.clear();
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(cnt).subbandCQIInfoPerGroupBWPart.subbandPrecoderIndi2.set_size(downlinkCQIInfo->cqiComputationInfo.numberOfBWParts);
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(cnt).subbandCQIInfoPerGroupBWPart.PMI.set_size(downlinkCQIInfo->cqiComputationInfo.numberOfBWParts);
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(cnt).subbandCQIInfoPerGroupBWPart.isValidPerBWPart.set_size(downlinkCQIInfo->cqiComputationInfo.numberOfBWParts);
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(cnt).subbandCQIInfoPerGroupBWPart.isValidPerBWPart=zeros_b(downlinkCQIInfo->cqiComputationInfo.numberOfBWParts);
// 	    if((downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_  || downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_COMP_) && downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).cqiPorts.length()==8 && subframeNumber==0 && downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_2_1_)
// 	      downlinkCQIInfo->perUserFeedback.PTI=0;
// 	  }
// 	}
// 	if(downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_1_0_||downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_1_1_)
// 	{
// 	  if(((subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay-downlinkCQIInfo->cqiComputationInfo.CQIPMIOffset-downlinkCQIInfo->cqiComputationInfo.RIOffset)%(downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity*downlinkCQIInfo->cqiComputationInfo.RIPeriodicity)==0))
// 	  {
// 	    
// 	    downlinkCQIInfo->cqiComputationInfo.isrankTrigger=true;
// 	  }
// 	  else
// 	    downlinkCQIInfo->cqiComputationInfo.isrankTrigger=false;
// 	  	  
// 	  if(downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_OLMUMIMO_)
// 	  {
// 	    if(((subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay-downlinkCQIInfo->cqiComputationInfo.CQIPMIOffset)%(downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity)==0))
// 	    {
// 	      downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger=true;
// 	      downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=true;
// 	    }
// 	    else
// 	    {
// 	      downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger=false;
// 	      downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=false;
// 	    }
// 	  }
// 	  else if(downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_  || downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_COMP_)
// 	  {
// 	    if(((subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay-downlinkCQIInfo->cqiComputationInfo.CQIPMIOffset)%(downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity)==0))
// 	    {
// 	      
// 	      downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger=true;
// 	      downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=true;
// 	    }
// 	    else
// 	    {
// 	      downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger=false;
// 	      downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=false;
// 	    }
// 	  }
// 	  else
// 	  {
// 	    if(((subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay-downlinkCQIInfo->cqiComputationInfo.CQIPMIOffset)%(downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity)==0))
// 	      downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=true;
// 	    else
// 	      downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=false;
// 	  }
//  
// 	}
// 	else if(downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_2_0_||downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_2_1_)
// 	{
// 	  if(((subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay-downlinkCQIInfo->cqiComputationInfo.CQIPMIOffset-downlinkCQIInfo->cqiComputationInfo.RIOffset)%((downlinkCQIInfo->cqiComputationInfo.BWPartCycles*downlinkCQIInfo->cqiComputationInfo.numberOfBWParts+1)*downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity*downlinkCQIInfo->cqiComputationInfo.RIPeriodicity)==0))
// 	  {
// 	    downlinkCQIInfo->cqiComputationInfo.isrankTrigger=true;
// 	    
// 	    
// 	    
// 	    if((downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_ )&& downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).cqiPorts.length()==8)
// 	      {
// 		if(subframeNumber==0)
// 		  downlinkCQIInfo->perUserFeedback.PTI=0;
// 		if(subframeNumber!=0)
// 		{
// 		  if(downlinkCQIInfo->cqiComputationInfo.i1diffcounter==0)
// 		    downlinkCQIInfo->perUserFeedback.PTI=1;
// 		  else
// 		    downlinkCQIInfo->perUserFeedback.PTI=0;
// 		}
// 	      }
// 	    downlinkCQIInfo->cqiComputationInfo.i1diffcounter=0;
// 	    
// 	    
// 	  }
// 	  else
// 	    downlinkCQIInfo->cqiComputationInfo.isrankTrigger=false;
// 	  
// 	  
// 	if((downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_ || downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_COMP_) && downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).cqiPorts.length()==8 && downlinkCQIInfo->perUserFeedback.PTI==0)
// 	  {  
// 	   if((subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay-downlinkCQIInfo->cqiComputationInfo.CQIPMIOffset)%(downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity)==0)
// 	   {	    
// 	    downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=true;
// 	    downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger=0;
// 	    downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger=false;
// 	   }
// 	  else
// 	    downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=false;
// 	  }
// 	  else 
// 	  {
// 	    if((subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay-downlinkCQIInfo->cqiComputationInfo.CQIPMIOffset)%((downlinkCQIInfo->cqiComputationInfo.BWPartCycles*downlinkCQIInfo->cqiComputationInfo.numberOfBWParts+1)*downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity)==0)
// 	    {
// 	      downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=true;
// 	      downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger=0;
// 	      downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger=false;
// 	    }
// 	    else
// 	      downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=false;
// 	  }
// 	  
// 	  
// 	  
// 	  
// 	  if((downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_ || downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_COMP_) && downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).cqiPorts.length()==8)// removed the pti check
// 	  {
// 	    if((subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay-downlinkCQIInfo->cqiComputationInfo.CQIPMIOffset)%(downlinkCQIInfo->cqiComputationInfo.FirstWideBandPMIPeriod*downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity)==0)
// 	    {	      
// 	      downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger=true;
// 	      //commented for i1 and i2 changes
// 	      //downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=false;
// 	    }
// 	    else
// 	      downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger=false;
// 	  }
// 	  else 
// 	    downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger=false;
// 	  
// 	  
// 	  // Added for i1 and i2 changes
// 	  if(downlinkCQIInfo->perUserFeedback.PTI==0 && downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger)
// 	    downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=false;
// 	  
// 	  
// 	  if(!((downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_ || downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_COMP_) && downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).cqiPorts.length()==8 && downlinkCQIInfo->perUserFeedback.PTI==0))
// 	  {
// 	    if((subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay-downlinkCQIInfo->cqiComputationInfo.CQIPMIOffset)%(downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity)==0)
// 	    {
// 	      if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger==false){
// 		downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger=true;
// 		if(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger<downlinkCQIInfo->cqiComputationInfo.numberOfBWParts)
// 		  downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger++;
// 		else
// 		  downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger=1;
// 	      }
// 	      else
// 		downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger=false;
// 	    }
// 	    else
// 	      downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger=false;
// 	  }
// 	  else
// 	    downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger=false;  
// 	}
// 	else{
// 	  cout<<"downlinkCQIInfo->cqiComputationInfo.periodicMode uninitialized"<<endl;
// 	}
// 	
// 	if(downlinkCQIInfo->cqiComputationInfo.isRankAdaptationEnabled)
// 	{
// 	  if(downlinkCQIInfo->cqiComputationInfo.isrankTrigger)
// 	    downlinkCQIInfo->subframeNumberToReportPeriodicRI=subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay;
// 	  
// 	  if(downlinkCQIInfo->cqiComputationInfo.isrankTrigger)
// 	    downlinkCQIInfo->cqiComputationInfo.isRankComputed=true;
// 	  if(downlinkCQIInfo->cqiComputationInfo.isRankComputed==false)
// 	  {
// 	    downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=false;
// 	    downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger=false;
// 	    downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger=false;
// 	  }
// 	  downlinkCQIInfo->perUserFeedback.isRankFeedback=downlinkCQIInfo->cqiComputationInfo.isRankComputed; 
// 	}
// 	
// 	
// 	if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger || downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger)
// 	{
// 	  downlinkCQIInfo->subframeNumberToReportPeriodicCQI=subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay;
// 	}
// 	
// 	if(downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger && downlinkCQIInfo->perUserFeedback.PTI==0)
// 	{
// 	  downlinkCQIInfo->perUserFeedback.isWideBandi1Feedback=true;
// 	}
// 	    
// 	if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger)
// 	  downlinkCQIInfo->perUserFeedback.isWideBandFeedback=true;
// 	
// 	else if(downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger){
// 	  downlinkCQIInfo->perUserFeedback.isWideBandi1Feedback=false;
// 	  downlinkCQIInfo->perUserFeedback.isWideBandFeedback=false;
// 	  downlinkCQIInfo->perUserFeedback.currentBWPart=downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1;
// 	}
// 	
// 	
//     }
}
/*
void CQIEstimator::FormPeriodicCQIPDU(requestID)
{
//   for(int Cnt=0;Cnt<downlinkCQIInfo->perUserFeedback.dlCQIMeasurement.length();Cnt++)
//   {
//     FormPeriodicCQIPDU(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(Cnt).requestID);
    FormPeriodicCQIPDU(requestID);
    
//   }
}*/

// Forms the CQI PDU(in bits as per the spec) to be encoded and sent in PUCCH/PUSCH (Incomplete and need to be tested)
//!CQI1 not done for TM9 in periodic case and all modes in aperiodic case
void CQIEstimator::FormPeriodicCQIPDU(int requestID)
{
  
//   int measurementIndx=downlinkCQIInfo->perUserFeedback.getMeasurementIndx(requestID);
//   int nAntennaPorts=downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(measurementIndx).cqiPorts.length();
//   int nCodewords=downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(measurementIndx).numberOfCodeWords;
//   ReportingModeFor_1_1_E sMode=downlinkCQIInfo->cqiComputationInfo.submode;
//   TransmissionScheme_E txScheme=downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(measurementIndx).transmissionScheme;
// 
//   bool FlagWideBandPMI=false;
//   if(downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_1_1_ || downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_2_1_)
//     FlagWideBandPMI=true;
// 
//   //   if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger)
//   {
//     if(nCodewords==1)
//     {
//       
//       if(txScheme==_TRANSMISSION_SCHEME_SINGLE_PORT_CRS_||txScheme==_TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_||txScheme==_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_
// 	||txScheme==_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_ || (txScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ && (!FlagWideBandPMI)) )
//       {
// 	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4);
// 	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic=(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0)));
// 	
//       }
//       else if(txScheme==_TRANSMISSION_SCHEME_CL_MU_MIMO_||txScheme== _TRANSMISSION_SCHEME_CL_BF_ || txScheme== _TRANSMISSION_SCHEME_CL_SM_ || (txScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ && FlagWideBandPMI))
//       {
// 	if(nAntennaPorts==2){
// 	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+2);// both wideband CQI and PMI (4 possible precoders )
// 	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
// 	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,(dec2bin(2,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.pmiIndex)));
// 	}
// 	else{
// 	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+4);// both wideband CQI and PMI (16 possible precoders )
// 	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
// 	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.pmiIndex)));
// 	}
//       }
//       else{
// 	
// 	cout<<"FormPeriodicCQIPDU() not written for the configured transmision mode"<<endl;
//       }
//     }
//     else if(nCodewords==2)
//     {
//       if(txScheme== _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_|| (txScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ && (!FlagWideBandPMI)) )
//       {
// 	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+3); // (CQI for codeword1, offset CQI for codeword2).
// 	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
// 	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,(dec2bin(3,getDifferentialCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord))));
//       }
//       else if(txScheme== _TRANSMISSION_SCHEME_CL_SM_ || (txScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ && FlagWideBandPMI ))
//       {
// 	  if(nAntennaPorts==2){
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+3+1); // (CQI for codeword1, offset CQI for codeword2,PMI). out of 3 precoders identity precoder is not used
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,(dec2bin(3,getDifferentialCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord))));
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(7,(dec2bin(1,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.pmiIndex-1))); //! Identity precoder (index 0) is not considered
// 	  }
// 	  else{
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+3+4);
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,(dec2bin(3,getDifferentialCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord))));
// 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(7,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.pmiIndex)));
// 	  }
//       }
//       else{
// 	//! no of bits for precoder (precoders used for TM8 is same as TM4)
// 	cout<<"FormPeriodicCQIPDU() not written for the configured transmision mode"<<txScheme<<"\t nCodeWords"<<nCodewords<<endl;abort();
//       } 
//     }
//   }
//   
//   if(downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger)
//   {
//     if(nCodewords==1)
//     {
//      int numOfBitsForSubbandindex=0;
// 	int BWcnt=downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1;
// 	numOfBitsForSubbandindex=int2bits(downlinkCQIInfo->cqiComputationInfo.subbandSizePeriodic(BWcnt).length());	     
// 	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart.set_length(4+numOfBitsForSubbandindex);
// 	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)(0))));
// 	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart.set_subvector(4,(dec2bin(numOfBitsForSubbandindex,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1))));
// 	//}
//     }
//     else if(nCodewords==2)
//     {
//       if(txScheme== _TRANSMISSION_SCHEME_CL_SM_ || txScheme==_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_ || txScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ || txScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || || txScheme==_TRANSMISSION_SCHEME_NR_CL_)
//       {
// 	int numOfBitsForSubbandindex=0;
// 	int BWcnt=downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1;
// 	numOfBitsForSubbandindex=dec2bin(downlinkCQIInfo->cqiComputationInfo.subbandSizePeriodic(BWcnt).length(),true).length();	     
// 	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart.set_length(4+3+numOfBitsForSubbandindex);// (CQI for codeword1, offset CQI for codeword2,subband index)
// 	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)(0))));
// 	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart.set_subvector(4,(dec2bin(3,getDifferentialCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)))));
// 	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart.set_subvector(7,(dec2bin(numOfBitsForSubbandindex,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1))));
//       }
//       else
//       {
// 	cout<<"FormPeriodicCQIPDU() not written for the configured transmision mode"<<endl;
//       }
//     }
//   }
  
  int measurementIndx=downlinkCQIInfo->perUserFeedback.getMeasurementIndx(requestID);
  int nLayers=downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(measurementIndx).numberOfLayers;
  int nAntennaPorts=downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(measurementIndx).cqiPorts.length();
  TransmissionScheme_E TxScheme=downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(measurementIndx).transmissionScheme;
  PeriodicReportingMode_E pMode=downlinkCQIInfo->cqiComputationInfo.periodicMode;
  ReportingModeFor_1_1_E sMode=downlinkCQIInfo->cqiComputationInfo.submode;
  bool FlagWideBandPMI=false;
  if(pMode==_PERIODIC_MODE_1_1_ || pMode==_PERIODIC_MODE_2_1_)
    FlagWideBandPMI=true;
  if(nAntennaPorts==8 && downlinkCQIInfo->perUserFeedback.PTI==0 && downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger && pMode==_PERIODIC_MODE_2_1_)
  {
    int noOfbitsForPrecoderi1;
    if((1<=nLayers) && (nLayers<=2))
      noOfbitsForPrecoderi1=4;
    else if((3<=nLayers) && (nLayers<=7))
      noOfbitsForPrecoderi1=2;
    else if(nLayers==8)
      noOfbitsForPrecoderi1=0;
    else
    {cout<<"[both:] Invalid no. of layers in FormPeriodicCQIPDU"<<endl;abort();}
    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(noOfbitsForPrecoderi1);
    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,dec2bin(noOfbitsForPrecoderi1,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1));
  }
  if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger)
  {
    if(downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(measurementIndx).numberOfCodeWords==1)
    {
      
      if(TxScheme==_TRANSMISSION_SCHEME_SINGLE_PORT_CRS_||TxScheme==_TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_||TxScheme==_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_
	||TxScheme==_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_ ||  ((TxScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || TxScheme==_TRANSMISSION_SCHEME_NR_CL_ )&& (nAntennaPorts==1 || (!FlagWideBandPMI))) || (TxScheme==_TRANSMISSION_SCHEME_COMP_ && (nAntennaPorts==1 || (!FlagWideBandPMI))))
      {
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4);
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0)));
	
      }
      else if(TxScheme==_TRANSMISSION_SCHEME_CL_MU_MIMO_||TxScheme== _TRANSMISSION_SCHEME_CL_BF_ || TxScheme== _TRANSMISSION_SCHEME_CL_SM_ || (TxScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ && FlagWideBandPMI) || ((TxScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || TxScheme==_TRANSMISSION_SCHEME_NR_CL_)&& FlagWideBandPMI && nAntennaPorts<8) || (TxScheme==_TRANSMISSION_SCHEME_COMP_ && FlagWideBandPMI && nAntennaPorts<8))
      {
	int noOfbitsForPrecoder;
	if(nAntennaPorts==2)
	  noOfbitsForPrecoder=2;
	else if(nAntennaPorts==4)
	  noOfbitsForPrecoder=4;
	else
	{cout<<"[both:] Invalid no. of Antenna Ports"<<endl;abort();}
	
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+noOfbitsForPrecoder);//CQI1 both wideband CQI and PMI (4 possible precoders )
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,(dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.pmiIndex)));
	  
	
      }
      else if((TxScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_|| TxScheme==_TRANSMISSION_SCHEME_NR_CL_ || TxScheme==_TRANSMISSION_SCHEME_NR_CL_  || TxScheme==_TRANSMISSION_SCHEME_COMP_) && (FlagWideBandPMI && pMode==_PERIODIC_MODE_1_1_ && sMode==_SUBMODE_1_ && nAntennaPorts==8))
      {
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+4);//CQI1 both wideband CQI and PMI (4 possible precoders )
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi2)));
      }
     
     else if((TxScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || TxScheme==_TRANSMISSION_SCHEME_NR_CL_ || TxScheme==_TRANSMISSION_SCHEME_COMP_) && (FlagWideBandPMI && pMode==_PERIODIC_MODE_1_1_ && sMode==_SUBMODE_2_ && nAntennaPorts==8))
      {
	int Encodedindi1,Encodedindi2;
	getEncodedPrecoderIndForSubmode2(Encodedindi1,Encodedindi2,nLayers,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi2);
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+3+1);//CQI1 both wideband CQI and PMI (4 possible precoders )
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,(dec2bin(3,Encodedindi1)));    
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4+3,(dec2bin(1,Encodedindi2)));    
      }
      else if((TxScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_|| TxScheme==_TRANSMISSION_SCHEME_NR_CL_ || TxScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_) && (nAntennaPorts==8 && pMode==_PERIODIC_MODE_2_1_))
      {
	if(downlinkCQIInfo->perUserFeedback.PTI==1)
	{
	 
	 downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+4);//CQI1 both wideband CQI and PMI (4 possible precoders )
	 downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
	 downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi2)));
	 
	}
	else if(downlinkCQIInfo->perUserFeedback.PTI==0)
	{
	  int noOfbitsForPrecoder=4;
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+noOfbitsForPrecoder);
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,(dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi2)));
	  
	}
	  
      }
      else	
      {
	cout<<"[both:] FormPeriodicCQIPDU() not written for the configured transmision mode"<<endl;
      }
    }
    else if(downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(measurementIndx).numberOfCodeWords==2) 
    {
      if(TxScheme== _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_ || (TxScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ && (!FlagWideBandPMI)))
      {
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+3); //CQI1 (CQI for codeword1, offset CQI for codeword2). 
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(3,(dec2bin(4,getDifferentialCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord))));	
	
      }
      else if(TxScheme== _TRANSMISSION_SCHEME_CL_SM_ || (TxScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ && FlagWideBandPMI ) || ((TxScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || TxScheme==_TRANSMISSION_SCHEME_NR_CL_) && FlagWideBandPMI && nAntennaPorts<8) || (TxScheme==_TRANSMISSION_SCHEME_COMP_ && FlagWideBandPMI && nAntennaPorts<8))
      {
	// 	if(downlinkCQIInfo.cqiComputationInfo.numberOfCodeWords!=2)
	// 	{
	  // 	  cout<<"[both:] number of codewords for CQI computation should be 2 for tx Scheme CL SM"<<endl;abort();
	  // 	}
	  int precInd;
	  int noOfbitsForPrecoder;
	  if(nAntennaPorts==2)
	  {
	    noOfbitsForPrecoder=1;
	    precInd=downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.pmiIndex-1;
	  }
	  else if(nAntennaPorts==4)
	  {
	    noOfbitsForPrecoder=4;
	    precInd=downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.pmiIndex;
	  }
	  else
	  {cout<<"[both:] Invalid no. of Antenna Ports"<<endl;abort();}	  
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+3+noOfbitsForPrecoder); //CQI1 (CQI for codeword1, offset CQI for codeword2,PMI). out of 3 precoders identity precoder is not used 
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,(dec2bin(3,getDifferentialCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord))));
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4+3,(dec2bin(noOfbitsForPrecoder,precInd)));
	 
      }
      else if((TxScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_|| TxScheme==_TRANSMISSION_SCHEME_NR_CL_  || TxScheme==_TRANSMISSION_SCHEME_COMP_) && (FlagWideBandPMI && pMode==_PERIODIC_MODE_1_1_ && sMode==_SUBMODE_1_ && nAntennaPorts==8))
      {
	int noOfbitsForPrecoder;
	if(nLayers==2 || nLayers==3)
	  noOfbitsForPrecoder=4;
	else if(nLayers==4)
	  noOfbitsForPrecoder=3;
	else if(nLayers<=8 && nLayers>4)
	  noOfbitsForPrecoder=0;
	else
	{cout<<"[both:] Invalid number oof layers in FormPeriodicPDU"<<endl;abort();}
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+3+noOfbitsForPrecoder);//CQI1 both wideband CQI and PMI (4 possible precoders )
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,(dec2bin(3,getDifferentialCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4+3,(dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi2)));
      }
      else if((TxScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || TxScheme==_TRANSMISSION_SCHEME_NR_CL_ || TxScheme==_TRANSMISSION_SCHEME_COMP_) && (nAntennaPorts==8 && FlagWideBandPMI && pMode==_PERIODIC_MODE_1_1_ && sMode==_SUBMODE_2_))
      {
	int Encodedindi1,Encodedindi2;
	getEncodedPrecoderIndForSubmode2(Encodedindi1,Encodedindi2,nLayers,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi2);
	int noOfbitsForPrecoderi1,noOfbitsForPrecoderi2;
	switch(nLayers)
	{
	  case 1:
	  case 2: noOfbitsForPrecoderi1=3;noOfbitsForPrecoderi2=1;break;
	  case 3:
	  case 4: noOfbitsForPrecoderi1=1;noOfbitsForPrecoderi2=3;break;
	  case 5:
	  case 6:
	  case 7:noOfbitsForPrecoderi1=2;noOfbitsForPrecoderi2=0;break;
	  case 8: noOfbitsForPrecoderi1=0;noOfbitsForPrecoderi2=0;break;
	  default:
	    cout<<"[both:] Error: nLayers > 8 not supported in Form Periodic CQI PDU"<<endl;abort(); 
	}
	
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+3+noOfbitsForPrecoderi1+noOfbitsForPrecoderi2);//CQI1 both wideband CQI and PMI (4 possible precoders )
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,(dec2bin(3,getDifferentialCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4+3,(dec2bin(noOfbitsForPrecoderi1,Encodedindi1)));    
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4+3+noOfbitsForPrecoderi1,(dec2bin(noOfbitsForPrecoderi2,Encodedindi2)));    
      }
      else if((TxScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || TxScheme==_TRANSMISSION_SCHEME_NR_CL_) && (nAntennaPorts==8 && pMode==_PERIODIC_MODE_2_1_))
      {
	if(downlinkCQIInfo->perUserFeedback.PTI==1)
	{
	  int noOfbitsForPrecoder;
	  if((2<=nLayers) && (nLayers<=3))
	    noOfbitsForPrecoder=4;
	  else if(nLayers==4)
	    noOfbitsForPrecoder=3;
	  else if((5<=nLayers) && (nLayers<=8))
	    noOfbitsForPrecoder=0;
	 else
	 {cout<<"[both:] Invalid number of layers in FormPeriodicCQIPDU"<<endl;abort();}
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+3+noOfbitsForPrecoder);//CQI1 both wideband CQI and PMI (4 possible precoders )
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,(dec2bin(3,getDifferentialCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4+3,(dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi2)));    
	
	}
	else if(downlinkCQIInfo->perUserFeedback.PTI==0)
	{
	  
	    int noOfbitsForPrecoder;
	    if((2<=nLayers) && (nLayers<=3))
	      noOfbitsForPrecoder=4;
	    else if(nLayers==4)
	      noOfbitsForPrecoder=3;
	    else if((5<=nLayers) && (nLayers<=8))
	      noOfbitsForPrecoder=0;
	    else
	    {cout<<"[both:] Invalid no. of layers in FormPeriodicCQIPDU"<<endl;abort();}
	    cout << "cqiatPDU :" << downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord << endl;
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_length(4+3+noOfbitsForPrecoder);
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0))));
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4,dec2bin(3,getDifferentialCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord)));
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic.set_subvector(4+3,(dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi2)));
	  
	}
      }
      else{
	//! no of bits for precoder (precoders used for TM8 is same as TM4)
	cout<<"[both:] FormPeriodicCQIPDU() not written for the configured transmision mode"<<endl; 
	// cout<<"FormPeriodicCQIPDU() not written for the configured transmision mode"<<txScheme<<"\t nCodeWords"<<nCodewords<<endl;abort();
      } 
    }
    
    //cout << "PDU :" << downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCQIPDUPerGroupPeriodic << endl;
  }
  else if(downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger)
  {
    if(downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(measurementIndx).numberOfCodeWords==1)
    {
      //if(TxScheme==_TRANSMISSION_SCHEME_SINGLE_PORT_CRS_||TxScheme==_TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_||TxScheme==_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_
      //	||TxScheme==_TRANSMISSION_SCHEME_CL_MU_MIMO_||TxScheme== _TRANSMISSION_SCHEME_CL_BF_||TxScheme==_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_){
	int numOfBitsForSubbandindex=0;
      int BWcnt=downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1;
      numOfBitsForSubbandindex=int2bits(downlinkCQIInfo->cqiComputationInfo.subbandSizePeriodic(BWcnt).length());	     
	//       if(downlinkCQIInfo.cqiComputationInfo.numberOfBWParts<=2)
	// 	numOfBitsForSubbandindex=1; 
	//       else if(downlinkCQIInfo.cqiComputationInfo.numberOfBWParts>2&&downlinkCQIInfo.cqiComputationInfo.numberOfBWParts<=4)
	// 	numOfBitsForSubbandindex=2;
	if(nAntennaPorts<8)
	{
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart.set_length(4+numOfBitsForSubbandindex);
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)(0))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_subvector(4,(dec2bin(numOfBitsForSubbandindex,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1))));
	}
	else
	{
	  if(FlagWideBandPMI)
	  {
	    if(downlinkCQIInfo->perUserFeedback.PTI==1)
	    {	       
	      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_length(4+4+numOfBitsForSubbandindex);
	      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)(0))));
	      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_subvector(4,dec2bin(4,getMode2_1EncodedPrecoderInd(1,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.subbandPrecoderIndi2(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1))));
	      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_subvector(4+4,(dec2bin(numOfBitsForSubbandindex,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1))));
	    }
	   	   
	  }
	}
      //}
    }
    else if(downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(measurementIndx).numberOfCodeWords==2)
    {
      if(TxScheme== _TRANSMISSION_SCHEME_CL_SM_ || TxScheme==_TRANSMISSION_SCHEME_CL_MU_MIMO_ || TxScheme==_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_ || TxScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ || TxScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || TxScheme==_TRANSMISSION_SCHEME_NR_CL_)
      {
	int numOfBitsForSubbandindex=0;
	int BWcnt=downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1;
	numOfBitsForSubbandindex=dec2bin(downlinkCQIInfo->cqiComputationInfo.subbandSizePeriodic(BWcnt).length(),true).length();
	if(nAntennaPorts<8)
	{
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_length(4+3+numOfBitsForSubbandindex);//CQI1 (CQI for codeword1, offset CQI for codeword2,subband index)
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)(0))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_subvector(4,(dec2bin(3,getDifferentialCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_subvector(7,(dec2bin(numOfBitsForSubbandindex,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1))));
	}
	else
	{
	  if(FlagWideBandPMI)
	  {
	    if(downlinkCQIInfo->perUserFeedback.PTI==1)
	    {	       
	      int noOfbitsForPrecoder;
	      if((2<=nLayers) && (nLayers<=4))
		noOfbitsForPrecoder=2;
	      else if((5<=nLayers) && (nLayers<=8))
		noOfbitsForPrecoder=0;
	      else
	      {cout<<"[both:] Invalid number of layers in FormPeriodicPDU"<<endl;abort();}
	      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_length(4+3+noOfbitsForPrecoder+numOfBitsForSubbandindex);
	      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_subvector(0,(dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)(0))));
	      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_subvector(4,(dec2bin(3,getDifferentialCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1)))));
	      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_subvector(4+3,dec2bin(noOfbitsForPrecoder,getMode2_1EncodedPrecoderInd(nLayers,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.subbandPrecoderIndi2(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1))));
	      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIPDUPerGroupBWPart .set_subvector(4+3+noOfbitsForPrecoder,(dec2bin(numOfBitsForSubbandindex,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1))));
	    }
	    	    
	  }
	}
      }
      else
      {
	cout<<"[both:] FormPeriodicCQIPDU() not written for the configured transmision mode"<<endl;
      }
    }
  }
}

// void CQIEstimator::FormAPeriodicCQIPDU(requestID)
// {
//   cout<<"perUserFeedback.dlCQIMeasurement.length"<<downlinkCQIInfo->perUserFeedback.dlCQIMeasurement.length()<<endl;
// //   for(int Cnt=0;Cnt<downlinkCQIInfo->perUserFeedback.dlCQIMeasurement.length();Cnt++)
// //   {
// //     cout<<"downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(Cnt).requestID"<<downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(Cnt).requestID<<endl;
//     
// //     FormAPeriodicCQIPDU(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(Cnt).requestID);
//     FormAPeriodicCQIPDU(requestID);
// 
// //   }
// }

//!NOTE rank reporting to be done
//!  need to be tested PDU is just formed. Need to be decoded at appropriate place
//!For 8 ports case Aperiodic CQI is not computed properly as per sec
void CQIEstimator::FormAPeriodicCQIPDU(int requestID)
{
  int measurementIndx=downlinkCQIInfo->perUserFeedback.getMeasurementIndx(requestID);
  int nAntennaPorts=downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(measurementIndx).cqiPorts.length();
  int nCodewords=downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(measurementIndx).numberOfCodeWords;
  TransmissionScheme_E txScheme=downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(measurementIndx).transmissionScheme;
  int nLayers=downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(measurementIndx).numberOfLayers;
  
  int noOfSubbands,noOfbitsForPosition;
  int noOfbitsForPrecoder,noOfbitsForPrecoderi1,noOfbitsForPrecoderi2,TotalnoOfbitsForPrecoderi2;
  //! higher layer configured
  if(downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_1_2_)
  {
    noOfSubbands=downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length();
    if(txScheme==_TRANSMISSION_SCHEME_CL_SM_ || txScheme==_TRANSMISSION_SCHEME_CL_BF_ || txScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ || ((txScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_) && (nAntennaPorts==2 || nAntennaPorts==4)) || (txScheme==_TRANSMISSION_SCHEME_COMP_ && (nAntennaPorts==2 || nAntennaPorts==4)))
    {
      noOfbitsForPrecoder=getNumOfBitsForPrecoder(nAntennaPorts,nCodewords);
      if(nCodewords==1)
      {
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+(noOfbitsForPrecoder*noOfSubbands));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
	for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(subband_cnt*noOfbitsForPrecoder),dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPMI(subband_cnt)));
      }
      if(nCodewords>1)
      {
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+4+(noOfbitsForPrecoder*noOfSubbands));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(1)));
	for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+4+(subband_cnt*noOfbitsForPrecoder),dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPMI(subband_cnt)-1));
      }
    }
    if((txScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_ || txScheme==_TRANSMISSION_SCHEME_COMP_) &&  nAntennaPorts==8)
    {
      int noOfbitsForSubbandPrecoderi2;
      getMaxNumberOfBitsFori1Andi2(nLayers,noOfbitsForPrecoderi1,noOfbitsForSubbandPrecoderi2);
      
      if(nCodewords==1)
      {
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+noOfbitsForPrecoderi1+(noOfbitsForSubbandPrecoderi2*noOfSubbands));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4,dec2bin(noOfbitsForPrecoderi1,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.WidebandPrecoderIndi1));
	for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+noOfbitsForPrecoderi1+(subband_cnt*noOfbitsForSubbandPrecoderi2),dec2bin(noOfbitsForSubbandPrecoderi2,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPrecIndi2(subband_cnt)));
      }
      if(nCodewords>1)
      {
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+4+noOfbitsForPrecoderi1+(noOfbitsForSubbandPrecoderi2*noOfSubbands));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(1)));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+4,dec2bin(noOfbitsForPrecoderi1,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.WidebandPrecoderIndi1));
	for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+4+noOfbitsForPrecoderi1+(subband_cnt*noOfbitsForSubbandPrecoderi2),dec2bin(noOfbitsForSubbandPrecoderi2,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPrecIndi2(subband_cnt)));
      }
    }
  }
  if(downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_3_0_ || downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_3_1_ ||downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_3_2_)
  {
    noOfSubbands=downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length();
    //!NOTE have to fix for TDD (mode 8 without PMI RI reporting)
    if(txScheme==_TRANSMISSION_SCHEME_SINGLE_PORT_CRS_ || txScheme==_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_ || txScheme==_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_ || txScheme==_TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_ || ((txScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_) && nAntennaPorts==1) || (txScheme==_TRANSMISSION_SCHEME_COMP_ && nAntennaPorts==1))//! add mode 10 here later
    {
      // 	if(nCodewords==1) //! need to fix for TM3. bits for second CW is not transmitted... need to be corrected
      // 	{
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+(2*noOfSubbands));
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
      for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(subband_cnt*2),dec2bin(2,getHigherLayerSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(subband_cnt)(0))));
      // 	}
      // 	if(nCodewords==2)
      // 	{
      // 	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+(2*noOfSubbands)+4+(2*noOfSubbands));
      // 	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
      // 	  for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
      // 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(subband_cnt*2),dec2bin(2,getHigherLayerSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(subband_cnt)(0))));
      // 	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands),dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(1)));
      // 	  for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
      // 	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands)+4,dec2bin(2,getHigherLayerSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(1),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(subband_cnt)(1))));
      // 	}
    }
    if(txScheme==_TRANSMISSION_SCHEME_CL_SM_ || txScheme==_TRANSMISSION_SCHEME_CL_MU_MIMO_ || txScheme==_TRANSMISSION_SCHEME_CL_BF_ || txScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ || ((txScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_) && (nAntennaPorts==2 || nAntennaPorts==4)) || (txScheme==_TRANSMISSION_SCHEME_COMP_ && (nAntennaPorts==2 || nAntennaPorts==4)))
    {
      noOfbitsForPrecoder=getNumOfBitsForPrecoder(nAntennaPorts,nCodewords);
      if(nCodewords==1)
      {
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+(2*noOfSubbands)+(noOfSubbands*noOfbitsForPrecoder));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
	for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(subband_cnt*2),dec2bin(2,getHigherLayerSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(subband_cnt)(0))));
	if(downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_3_2_)
	{
	  for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	  {
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands)+(subband_cnt*noOfbitsForPrecoder),dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPMI(subband_cnt)));
	  }
	}else{
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands),dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI));
	}
      }
      if(nCodewords>1)
      {

	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+(2*noOfSubbands)+4+(2*noOfSubbands)+(noOfbitsForPrecoder*noOfSubbands));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
	for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(subband_cnt*2),dec2bin(2,getHigherLayerSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(subband_cnt)(0))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands),dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(1)));
	for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands)+4+(subband_cnt*2),dec2bin(2,getHigherLayerSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(1),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(subband_cnt)(1))));
	if(downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_3_2_)
	{
	  for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	  {
	    //!FIXME : incese of 2 port to avoid unitary mat report in cseof 2 port PMI -1 is reported but not true for 4 port case need to fix
	    if(nAntennaPorts == 2)
	    {
	      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands)+4+(2*noOfSubbands)+(noOfbitsForPrecoder*subband_cnt),dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPMI(subband_cnt)-1));
	    }
	    else
	    {
	      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands)+4+(2*noOfSubbands)+(noOfbitsForPrecoder*subband_cnt),dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.subbandPMI(subband_cnt)));
	    }
	  }
	}else{
	  if(nAntennaPorts == 2)
	  {
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands)+4+(2*noOfSubbands),dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI-1));
	  }
	  else
	  {
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands)+4+(2*noOfSubbands),dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI));
	  }
	}

      }
    }
    if((txScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_ || txScheme==_TRANSMISSION_SCHEME_COMP_) && nAntennaPorts==8)
    {
      getMaxNumberOfBitsFori1Andi2(nLayers,noOfbitsForPrecoderi1,noOfbitsForPrecoderi2);
      
      if(downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_3_2_)
      {
	// modfifing bit cout of i2 in case of multiple PMI reporting case to No of subband times
	TotalnoOfbitsForPrecoderi2= noOfSubbands*noOfbitsForPrecoderi2;
      }else{
	TotalnoOfbitsForPrecoderi2= noOfbitsForPrecoderi2;
      }
      
      if(nCodewords==1)
      {
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+(2*noOfSubbands)+noOfbitsForPrecoderi1+TotalnoOfbitsForPrecoderi2);
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
	for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	{
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(subband_cnt*2),dec2bin(2,getHigherLayerSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(subband_cnt)(0))));
	}
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands),dec2bin(noOfbitsForPrecoderi1,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.WidebandPrecoderIndi1));
	if(downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_3_2_)
	{
	  for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	  {
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands)+noOfbitsForPrecoderi1+(noOfbitsForPrecoderi2*subband_cnt),dec2bin(noOfbitsForPrecoderi2,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.subbandPrecoderIndi2(subband_cnt)));
	  }
	}else{
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands)+noOfbitsForPrecoderi1,dec2bin(noOfbitsForPrecoderi2,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.WideBandPrecoderindi2));
	}
      }
      if(nCodewords>1)
      {
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+(2*noOfSubbands)+4+(2*noOfSubbands)+noOfbitsForPrecoderi1+TotalnoOfbitsForPrecoderi2);
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
	for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	{
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(subband_cnt*2),dec2bin(2,getHigherLayerSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(subband_cnt)(0))));
	}
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands),dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(1)));
	for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	{
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands)+4+(2*subband_cnt),dec2bin(2,getHigherLayerSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(1),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(subband_cnt)(1))));
	}
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands)+4+(2*noOfSubbands),dec2bin(noOfbitsForPrecoderi1,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.WidebandPrecoderIndi1));
	if(downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_3_2_)
	{
	  for(int subband_cnt=0;subband_cnt<noOfSubbands;subband_cnt++)
	  {
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands)+4+(2*noOfSubbands)+noOfbitsForPrecoderi1+(noOfbitsForPrecoderi2*subband_cnt),dec2bin(noOfbitsForPrecoderi2,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupHigherLayer.subbandPrecoderIndi2(subband_cnt)));
	  }
	}else{
	  downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+(2*noOfSubbands)+4+(2*noOfSubbands)+noOfbitsForPrecoderi1,dec2bin(noOfbitsForPrecoderi2,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.WideBandPrecoderindi2));
	}
      }
    }
    
  }
  if(downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_2_0_ || downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_2_2_)
  {
    noOfSubbands=downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected.length();
    noOfbitsForPosition=ceil(log2( NCr(noOfSubbands,downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands)));
    //! FIXME: have to fix for TDD case for mode 8
    if(txScheme==_TRANSMISSION_SCHEME_SINGLE_PORT_CRS_ || txScheme==_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_ || txScheme==_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_ || txScheme==_TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_ || ((txScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_) && nAntennaPorts==1) || (txScheme==_TRANSMISSION_SCHEME_COMP_ && nAntennaPorts==1))//! add mode 10 here later
    {
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+2+noOfbitsForPosition);
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4,dec2bin(2,getUESelectedSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord(0))));
      downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2,dec2bin(noOfbitsForPosition,getCombinatorialindex(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,noOfSubbands)));//! FIXME no of bits allocated are double the normal case (i think for WB PMI n SB PMI .. need to check)
    }
    if(txScheme==_TRANSMISSION_SCHEME_CL_SM_ || txScheme==_TRANSMISSION_SCHEME_CL_BF_ || txScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_ || ((txScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_)&& (nAntennaPorts==2 || nAntennaPorts==4)) || (txScheme==_TRANSMISSION_SCHEME_COMP_ && (nAntennaPorts==2 || nAntennaPorts==4)))
    {
      noOfbitsForPrecoder=getNumOfBitsForPrecoder(nAntennaPorts,nCodewords);
      
      if(nCodewords==1)
      {
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+2+noOfbitsForPosition+(2*noOfbitsForPrecoder));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4,dec2bin(2,getUESelectedSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord(0))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2,dec2bin(noOfbitsForPosition,getCombinatorialindex(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,noOfSubbands)));//! FIXME
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2+noOfbitsForPosition,dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2+noOfbitsForPosition+noOfbitsForPrecoder,dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.PMI));
      }
      if(nCodewords>1)
      {
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+2+4+2+noOfbitsForPosition+(2*noOfbitsForPrecoder));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4,dec2bin(2,getUESelectedSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord(0))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(1)));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2+4,dec2bin(2,getUESelectedSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(1),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord(1))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2+4+2,dec2bin(noOfbitsForPosition,getCombinatorialindex(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,noOfSubbands)));//! FIXME
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2+4+2+noOfbitsForPosition,dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.widebandPMI-1));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2+4+2+noOfbitsForPosition+noOfbitsForPrecoder,dec2bin(noOfbitsForPrecoder,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.PMI-1));
      }
    }
    if((txScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || txScheme==_TRANSMISSION_SCHEME_NR_CL_ || txScheme==_TRANSMISSION_SCHEME_COMP_)&& nAntennaPorts==8)
    {
      int noOfbitsForSubbandPrecoderi2;
      getMaxNumberOfBitsFori1Andi2(nLayers,noOfbitsForPrecoderi1,noOfbitsForPrecoderi2);
      noOfbitsForSubbandPrecoderi2=noOfbitsForPrecoderi2;
      if(nCodewords==1)
      {
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+2+noOfbitsForPosition+noOfbitsForPrecoderi1+noOfbitsForPrecoderi2+noOfbitsForSubbandPrecoderi2);
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4,dec2bin(2,getUESelectedSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord(0))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2,dec2bin(noOfbitsForPosition,getCombinatorialindex(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,noOfSubbands)));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2+noOfbitsForPosition,dec2bin(noOfbitsForPrecoderi1,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.WidebandPrecoderIndi1));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2+noOfbitsForPosition+noOfbitsForPrecoderi1,dec2bin(noOfbitsForPrecoderi2,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.WideBandPrecoderindi2));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2+noOfbitsForPosition+noOfbitsForPrecoderi1+noOfbitsForPrecoderi2,dec2bin(noOfbitsForSubbandPrecoderi2,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandPrecIndi2));
      }
      if(nCodewords>1)
      {
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_length(4+2+4+2+noOfbitsForPosition+noOfbitsForPrecoderi1+noOfbitsForPrecoderi2+noOfbitsForSubbandPrecoderi2);
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(0,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0)));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4,dec2bin(2,getUESelectedSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(0),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord(0))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2,dec2bin(4,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(1)));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2+4,dec2bin(2,getUESelectedSubbandDiffCQIvalue(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(1),downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.cqiIndexPerCodeWord(1))));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2+4+2,dec2bin(noOfbitsForPosition,getCombinatorialindex(downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandIndex,noOfSubbands)));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2+4+2+noOfbitsForPosition,dec2bin(noOfbitsForPrecoderi1,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.WidebandPrecoderIndi1));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2+4+2+noOfbitsForPosition+noOfbitsForPrecoderi1,dec2bin(noOfbitsForPrecoderi2,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).widebandCqiInfoPerGroupAperiodic.WideBandPrecoderindi2));
	downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).CQIPDUPerGroupAperiodic.set_subvector(4+2+4+2+noOfbitsForPosition+noOfbitsForPrecoderi1+noOfbitsForPrecoderi2,dec2bin(noOfbitsForSubbandPrecoderi2,downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx).subbandCqiInfoPerGroupUESelected.subbandPrecIndi2));
      }
    }
  }
}


//! while forming CQI PDU, the  offset of the CQI for 2nd CW from the 1st CW is reported using quantised levels
int getDifferentialCQIvalue(ivec CQIindexPerCW)
{
  int CQIoffset;
  CQIoffset=CQIindexPerCW(0)-CQIindexPerCW(1);
  if(CQIoffset>=0 && CQIoffset<3)
    return(CQIoffset);
  else if(CQIoffset>=3)
    return(3);
  else if(CQIoffset<=-4)
    return(4);
  else
    return(CQIoffset+8);
}

// int getModulationOrderFromCQIIndex(bool is256QAMEnables, int CQI, int& linearIndex){
//   
//   int modOrder;
//   
//   if(CQI>=0 && CQI<7)
//   {
//     modOrder=2;
//     linearIndex=0;
//   }
//   else if(CQI>6 && CQI<10)
//   {
//     modOrder=4;
//     linearIndex=1;
//   }
//   else if(CQI>9 && CQI<16)
//   {
//     modOrder=6;
//     linearIndex=2;
//   }
//   else
//   {
//     cout<<"[both:] Error.. Unsupported CQI in getModulationOrderFromCQIIndex"<<endl; abort();
//   }
//   return modOrder;
// }

//! generates trigger for Pico UEs when ABS feature is enabled
void CQIEstimator::ManageDLCQIReportTriggerForABS(int DLRBs,int subframeNumber,bool isAperiodicCQICompEnabled, int offset)
{
  
  
  // #ifdef USING_CALIBRATION 
  if(isAperiodicCQICompEnabled==true)
  {    
    downlinkCQIInfo->cqiComputationInfo.isPeriodicConfigured=false;
    if ((subframeNumber-offset)%downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity==offset)
    {
      downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered = true;
      downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQIConfigured = true;
      if(downlinkCQIInfo->cqiComputationInfo.isRankAdaptationEnabled)
      {	  
	{
	  downlinkCQIInfo->cqiComputationInfo.isrankTrigger=true;
	  downlinkCQIInfo->cqiComputationInfo.isRankComputed=true;	     
	  downlinkCQIInfo->perUserFeedback.isRankFeedback=true;
	}
      }
    }
    else
    {
      downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered = false;
      downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQIConfigured = false;
    }
  }
  // #endif
 
  if(downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered)
  {
    if(downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQIConfigured)
    {
      if(downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_1_2_||downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_3_0_||downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_3_1_)
	downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic=getSubbandSizeAperiodic(DLRBs);
      else
	downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodicUESelected=getSubbandSizeAperiodicUESelected(DLRBs); // since the subband size for higher layer configured mode (mode 3_0 and mode 3_1)
	if(downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_2_0_||downlinkCQIInfo->cqiComputationInfo.aperiodicMode==_APERIODIC_MODE_2_2_)
    {
      if(DLRBs<8)
      {
	cout<<"Given Aperiodic mode not supported for this Bandwidth"<<endl;
	exit(0);
      }
	  downlinkCQIInfo->cqiComputationInfo.numberOfUESelectedSubbands=getNumberOfUESelectedSubbands(DLRBs);
    }
	downlinkCQIInfo->subframeNumberToReportAperiodicCQI=subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay;// subframenumber is the subframe where CQI computation is triggered
	downlinkCQIInfo->perUserFeedback.APeriodicReportedSubframe=subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay;
	
    }
    else
    {
      downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered=false;
      cout<<"APeriodicCQIConfigured not configured but APeriodicCQITriggered "<<endl;
    }
  }   
    
 
    if(downlinkCQIInfo->cqiComputationInfo.isPeriodicConfigured)
    {
      
      if(downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_2_0_||downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_2_1_)
	if(downlinkCQIInfo->cqiComputationInfo.numberOfBWParts<=0){// initializing the variables as per the configuration
	  getSubbandsAndBWParts(downlinkCQIInfo->cqiComputationInfo.subbandSizePeriodic,downlinkCQIInfo->cqiComputationInfo.numberOfBWParts,DLRBs);// number of Bandwidth parts and subband size based on system bandwidth
	  for(int Cnt=0;Cnt<downlinkCQIInfo->perUserFeedback.dlCQIMeasurement.length();Cnt++){
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(Cnt).subbandCQIInfoPerGroupBWPart.cqiIndexPerCodeWord.set_size(downlinkCQIInfo->cqiComputationInfo.numberOfBWParts);// <BWPart<nCodeword>>
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(Cnt).subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart.set_size(downlinkCQIInfo->cqiComputationInfo.numberOfBWParts);// <BWPart>
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(Cnt).subbandCQIInfoPerGroupBWPart.subbandIndexPerBWPart.clear();
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(Cnt).subbandCQIInfoPerGroupBWPart.isValidPerBWPart.set_size(downlinkCQIInfo->cqiComputationInfo.numberOfBWParts);
	    downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(Cnt).subbandCQIInfoPerGroupBWPart.isValidPerBWPart=zeros_b(downlinkCQIInfo->cqiComputationInfo.numberOfBWParts);
	  }
	}
	if(downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_1_0_||downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_1_1_)
	{
	  // subFrameNumber+downlinkCQIInfo->subFrameOffsetFDD===> reporting subframe
	  if((subframeNumber-offset)%(downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity*downlinkCQIInfo->cqiComputationInfo.RIPeriodicity)==0)
	  {
	    downlinkCQIInfo->cqiComputationInfo.isrankTrigger=true;
	  }
	  else
	    downlinkCQIInfo->cqiComputationInfo.isrankTrigger=false;
	  if((subframeNumber-offset)%(downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity)==0)
	    downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=true;
	  else
	    downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=false;
	  
	}
	else if(downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_2_0_||downlinkCQIInfo->cqiComputationInfo.periodicMode==_PERIODIC_MODE_2_1_)
	{
	  if((subframeNumber-offset)%((downlinkCQIInfo->cqiComputationInfo.BWPartCycles*downlinkCQIInfo->cqiComputationInfo.numberOfBWParts+1)*downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity*downlinkCQIInfo->cqiComputationInfo.RIPeriodicity)==0)
	    downlinkCQIInfo->cqiComputationInfo.isrankTrigger=true;
	  else
	    downlinkCQIInfo->cqiComputationInfo.isrankTrigger=false;
	  
	  if((subframeNumber-offset)%((downlinkCQIInfo->cqiComputationInfo.BWPartCycles*downlinkCQIInfo->cqiComputationInfo.numberOfBWParts+1)*downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity)==0){
	    
	    downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=true;
	    downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger=0;
	    downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger=false;
	  }
	  else
	    downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=false;
	  
	  if((subframeNumber-offset)%(downlinkCQIInfo->cqiComputationInfo.CQIPMIPeriodicity)==0){
	    if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger==false){
	      downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger=true;
	      if(downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger<downlinkCQIInfo->cqiComputationInfo.numberOfBWParts)
		downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger++;
	      else
		downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger=1;
	    }
	    else
	      downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger=false;
	  }
	  else
	    downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger=false;
	}
	else{
	  cout<<"downlinkCQIInfo->cqiComputationInfo.periodicMode uninitialized"<<endl;
	}
	
	if(downlinkCQIInfo->cqiComputationInfo.isRankAdaptationEnabled)
	{
	  if(downlinkCQIInfo->cqiComputationInfo.isrankTrigger)
	    downlinkCQIInfo->subframeNumberToReportPeriodicRI=subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay; // NOTE: It is not used right now, rank is being reported with the cqi reporting 
	    
	    
	    // 	    if(subframeNumber==offset){
	    // 	      downlinkCQIInfo->cqiComputationInfo.isrankTrigger=true;
	    // 	      downlinkCQIInfo->cqiComputationInfo.isRankComputed=true;
	    // 	    }
	    if(downlinkCQIInfo->cqiComputationInfo.isrankTrigger)
	    {
	      downlinkCQIInfo->cqiComputationInfo.isRankComputed=true;
	    }
	    
	    if(downlinkCQIInfo->cqiComputationInfo.isRankComputed==false)
	    {
	      downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=false;
	      downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger=false;
	    }
	    downlinkCQIInfo->perUserFeedback.isRankFeedback=downlinkCQIInfo->cqiComputationInfo.isRankComputed;
	    
	}
	if(downlinkCQIInfo->cqiComputationInfo.isRankAdaptationEnabled)
	{
	  if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger || downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger)
	  {
	    downlinkCQIInfo->subframeNumberToReportPeriodicCQI=subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay+1;
	  }
	
	}
	else
	{
	  if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger || downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger)
	  {
	    downlinkCQIInfo->subframeNumberToReportPeriodicCQI=subframeNumber+downlinkCQIInfo->cqiComputationInfo.CQIReportingDelay;//! offset w.r.t RI reporting is hardcoded  to 1
	  }
	}
	
	if(downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger)
	  downlinkCQIInfo->perUserFeedback.isWideBandFeedback=true;
	else if(downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger){
	  downlinkCQIInfo->perUserFeedback.isWideBandFeedback=false;
	  downlinkCQIInfo->perUserFeedback.currentBWPart=downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger-1;
	}
	
    }
    
}

void getMaxNumberOfBitsFori1Andi2(int nLayers, int &noOfbitsForPrecoderi1,int &noOfbitsForPrecoderi2)
{
  switch(nLayers)
  {
    case 1:
    case 2:noOfbitsForPrecoderi1=4;noOfbitsForPrecoderi2=4;break;
    case 3:noOfbitsForPrecoderi1=2;noOfbitsForPrecoderi2=4;break;
    case 4:noOfbitsForPrecoderi1=2;noOfbitsForPrecoderi2=3;break;
    case 5:
    case 6:
    case 7:noOfbitsForPrecoderi1=2;noOfbitsForPrecoderi2=0;break;
    case 8:noOfbitsForPrecoderi1=0;noOfbitsForPrecoderi2=0;break;
    default:
      cout<<"[both:] Error: nLayers > 8 not supported in Form Aperiodic CQI PDU"<<endl;abort(); 	    
  }
}


Array< cmat > getRawPhysicalChannel(Array< Array<cmat> > &channel, Size_S rbSize,Array<ivec> prbSCStartLocInEachSlot,bool isDownlink,ivec positionsToConsider){
  
  Array< Array<cmat> > myChannel;
  getPhysicalChannel(myChannel,channel,rbSize,prbSCStartLocInEachSlot,isDownlink);
  
  Array< cmat > rawChannel(positionsToConsider.length());
  for(int tone_cnt=0;tone_cnt<positionsToConsider.length();tone_cnt++)
    rawChannel(tone_cnt)=getAcrossArray(myChannel,positionsToConsider(tone_cnt));
  
  return rawChannel;
}

void getPhysicalChannel(Array< Array<cmat> >& outputChannel,Array< Array<cmat> > &channel, Size_S rbSize,Array<ivec> prbSCStartLocInEachSlot,bool isDownlink){
  
  Array<ivec> phyLoc = prbSCStartLocInEachSlot;
  
  int txAntenna=channel.length();
  int rxAntenna=channel(0).length();
  
  outputChannel.set_size(txAntenna);//CQI1 <nTx<nRx<nSubcarrier,nSymbol>>>
  cmat temp=zeros_c(rbSize.numberOfSubcarriers,rbSize.numberOfSymbols);
  
  // obtaining the values of the channel for the PRB under consideration from the complete channel matrix
  int nSymbolsInOneSlot=rbSize.numberOfSymbols/2;
  int r1=0,r2=rbSize.numberOfSubcarriers-1;
  for(int tx_cnt=0;tx_cnt<txAntenna;tx_cnt++)
  {
    outputChannel(tx_cnt).set_length(rxAntenna);
    for(int rx_cnt=0;rx_cnt<rxAntenna;rx_cnt++)
    {
      cmat tempMat0,tempMat1;
      for(int slot_cnt=0;slot_cnt<2;slot_cnt++)
      {
	if(slot_cnt==0)
	  tempMat0=channel(tx_cnt)(rx_cnt)(phyLoc(slot_cnt)(r1),phyLoc(slot_cnt)(r2),slot_cnt*nSymbolsInOneSlot,slot_cnt*nSymbolsInOneSlot+nSymbolsInOneSlot-1);
	else
	  tempMat1=channel(tx_cnt)(rx_cnt)(phyLoc(slot_cnt)(r1),phyLoc(slot_cnt)(r2),slot_cnt*nSymbolsInOneSlot,slot_cnt*nSymbolsInOneSlot+nSymbolsInOneSlot-1);
      }
      outputChannel(tx_cnt)(rx_cnt)=concat_horizontal(tempMat0,tempMat1);
    }
  }
}





int getCombinatorialindex(ivec subbandIndex,int noOfSubbands)
{
  int r=0;
  sort(subbandIndex);
  subbandIndex=subbandIndex+1;
  int N=noOfSubbands,M=subbandIndex.length();
  //       cout<<"N:"<<N<<endl;
  //     cout<<"subbandIndex:"<<subbandIndex<<endl;
  for(int i=0;i<subbandIndex.length();i++)
  {
    if((N-subbandIndex(i))>=(M-i))
      r=r+(factorial(N-subbandIndex(i))/(factorial(M-i)*factorial(N-subbandIndex(i)-M+i)));
    // 	  cout<<"r:	"<<r<<"	i:	"<<i<<endl;
  }
  
  return(r);
}

      
void getEncodedPrecoderIndForSubmode2(int &IPMI1,int &IPMI2,int RI,int codeBookIndexi1,int CodeBookIndexi2)
{
  if((1<=RI) && (RI<=4))
  IPMI1=(int)(codeBookIndexi1/2);
  else if((5<=RI) && (RI<=7))
  IPMI1=codeBookIndexi1;
  else if(RI==8)
   IPMI1=0;
  else
  {cout<<"[both:] Invalid Rank in getEncodedPrecoderIndForSubmode2"<<endl;abort();}
  ivec i2ValuesForRI3=ivec("0 1 2 3 8 9 10 11");
  if((5<=RI) && (RI<=8))
    IPMI2=0;
  else if(RI==2 || RI==4)
    IPMI2=CodeBookIndexi2;
  else if(RI==1)
    IPMI2=(int)(CodeBookIndexi2/2);
  else if(RI==3)
    IPMI2=find(i2ValuesForRI3,CodeBookIndexi2);
  else
  { cout<<"[both:] Invalid Rank in getEncodedPrecoderIndForSubmode2"<<endl;abort();}
  
}
int getMode2_1EncodedPrecoderInd(int RI,int codeBookIndexi2)
{
  ivec i2ValuesForRI3=ivec("2 3 10 11");
  if(RI==1)
  return(codeBookIndexi2);
  else if(RI==2 || RI==4)
  return((int)(codeBookIndexi2/2));
  else if(RI==3)
    return(find(i2ValuesForRI3,codeBookIndexi2));
  else if((5<=RI) && (RI<=8))
    return(0);
  else
  { cout<<"[both:] Invalid rank in getMode2_1EncodedPrecoderInd"<<endl;abort();}
  
}


