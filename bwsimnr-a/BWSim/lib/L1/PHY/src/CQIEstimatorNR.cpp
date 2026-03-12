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

#include "../include/CQIEstimatorNR.h"

CQIEstimatorNR::CQIEstimatorNR()
{
    geometrySINRindB=-10;
    sinrOffsetForOLLA=0.0;
    ackCount=0;
    nackCount=0;
    //isWithAbstraction=false;
    sinrOverWideband.set_length(0);
    estimateInterference=true;
    entireRawChannel.set_length(0);
    perToneInterferenceCovariance.set_length(0);
    
    #ifdef USING_SRS
    isSRSEnabled=false;
    srsRBs.set_size(0);
    isOddSCs=false;
    #endif
    
    LoopsforWideband = 10;
}

void CQIEstimatorNR::configureCQIEstimatorNR(ivec cqiPorts, FrameStructure& frameStructure){
  
  CSIRSPorts = cqiPorts;
  nCSIRSPorts = cqiPorts.length();
  portReferenceNumbers = getPortReferenceNumbers(frameStructure,cqiPorts);
  portLocations = getPortLocationsInRB(frameStructure, cqiPorts,0,true);

  //Making CSI-RS density per PRB as 1
  portLocations = portLocations(0,0);

  rbRSPattern = frameStructure.getResourceUnitMatrix(0);
  reCountPerRb = portLocations.length();
}

CSI_Info_S CQIEstimatorNR::computeCQIPerCodewords(Array<vec>& sinrPerCodeword,int rbCount,int nREsPerRB,TransmissionScheme_E txScheme,int nLayers, bool enableCQI0){
    
    CSI_Info_S cqiInfo;
    
    int nCodeWords = sinrPerCodeword.length();
    cqiInfo.initCSI_Info(nCodeWords,nLayers);
  //cout<<"I2 size : "<<CBParam.i2Size<<endl;
    
    cqiInfo.nLayersPerCW = getNumLayersPerCodeWord(nCodeWords,nLayers/getSymbolRepetitionFactor(txScheme,nLayers));
    
    double exp_spec_eff1;
    
    for(int icw=0; icw<nCodeWords; icw++)
    {
        ivec tbSizePerCQI = zeros_i(16);
        for(int cqi=1;cqi<tbSizePerCQI.length();cqi++)
            tbSizePerCQI(cqi) = tbTables->cqiToMaxMCSRate.modulationIndex(cqi-1) * tbTables->cqiToMaxMCSRate.codeRate(cqi-1) * nREsPerRB * rbCount * cqiInfo.nLayersPerCW(icw);
        
        // Using QPSK MI curve even for BPSK enabled case - Dhiv
        cqiInfo.MMIBperCW(icw)[_MODULATION_SCHEME_QPSK_]   = tbTables->awgnTablesConvCoder.sinrToEffMMIB(sinrPerCodeword(icw),_MODULATION_SCHEME_QPSK_);
        cqiInfo.MMIBperCW(icw)[_MODULATION_SCHEME_16_QAM_] = tbTables->awgnTablesConvCoder.sinrToEffMMIB(sinrPerCodeword(icw),_MODULATION_SCHEME_16_QAM_);
        cqiInfo.MMIBperCW(icw)[_MODULATION_SCHEME_64_QAM_] = tbTables->awgnTablesConvCoder.sinrToEffMMIB(sinrPerCodeword(icw),_MODULATION_SCHEME_64_QAM_);
        
        if(tbTables->awgnTablesConvCoder.is256QAMenabled)
            cqiInfo.MMIBperCW(icw)[_MODULATION_SCHEME_256_QAM_] = tbTables->awgnTablesConvCoder.sinrToEffMMIB(sinrPerCodeword(icw),_MODULATION_SCHEME_256_QAM_);
        
        cqiInfo.CQIindexPerCW(icw) = tbTables->awgnTablesConvCoder.CQISelect(exp_spec_eff1, cqiInfo.MMIBperCW(icw),tbSizePerCQI,enableCQI0);
        
        cqiInfo.modSchemePerCW(icw) = ModulationScheme_E(tbTables->cqiToMaxMCSRate.getCQIModulationAlphabet(cqiInfo.CQIindexPerCW(icw)));
        
        cqiInfo.effSINRsPerCW(icw) = tbTables->awgnTablesConvCoder.mmibToSinr(cqiInfo.modSchemePerCW(icw),cqiInfo.MMIBperCW(icw)[cqiInfo.modSchemePerCW(icw)]);
        
        cqiInfo.specEff += log2(1.0 + inv_dB(cqiInfo.effSINRsPerCW(icw))) * cqiInfo.nLayersPerCW(icw);   
        
    }
    
    return cqiInfo;
    
}

CSI_Info_S CQIEstimatorNR::getBestPMIandCQI(Array<Array<vec> > sinrPerPMI,int rbCount,int nREsPerRB,TransmissionScheme_E txScheme,int nLayers, bool enableCQI0 )
{
  int codebookLength = sinrPerPMI.length();
  Array<CSI_Info_S> csiInfoPerPMI;
  csiInfoPerPMI.set_length(codebookLength);
  vec spectralEfficiencyOverPMIs = zeros(codebookLength);
  
  for(int precIndex=0; precIndex < codebookLength; precIndex++)
  {
    csiInfoPerPMI(precIndex) = computeCQIPerCodewords(sinrPerPMI(precIndex),rbCount,nREsPerRB,txScheme,nLayers,enableCQI0);
    csiInfoPerPMI(precIndex).PMI = precIndex; 
    csiInfoPerPMI(precIndex).sinrPerCodeword = sinrPerPMI(precIndex);
    spectralEfficiencyOverPMIs(precIndex) = csiInfoPerPMI(precIndex).specEff;
  }
  
  int tempPMI = max_index(spectralEfficiencyOverPMIs);
  
  return csiInfoPerPMI(tempPMI);
}

CSI_Info_S CQIEstimatorNR::getBestPMIandCQI(Array<Array<vec> > sinrPerPMI,int rbCount,int nREsPerRB,TransmissionScheme_E txScheme,int nLayers, bool enableCQI0,vec& spectralEfficiencyOverPMIs,Array<ivec>& cqiIndexPerCWPerPrecoder,Array<CSI_Info_S>& csiInfoPerPMI )
{
    int codebookLength = sinrPerPMI.length();
    csiInfoPerPMI.set_length(codebookLength);
    spectralEfficiencyOverPMIs = zeros(codebookLength);
    cqiIndexPerCWPerPrecoder.set_length(codebookLength);
    for(int precIndex=0; precIndex < codebookLength; precIndex++)
    {
        csiInfoPerPMI(precIndex) = computeCQIPerCodewords(sinrPerPMI(precIndex),rbCount,nREsPerRB,txScheme,nLayers,enableCQI0);
        csiInfoPerPMI(precIndex).PMI = precIndex;
        csiInfoPerPMI(precIndex).sinrPerCodeword = sinrPerPMI(precIndex);
        spectralEfficiencyOverPMIs(precIndex) = csiInfoPerPMI(precIndex).specEff;
        cqiIndexPerCWPerPrecoder(precIndex) = csiInfoPerPMI(precIndex).CQIindexPerCW;
    }
    
    int tempPMI = max_index(spectralEfficiencyOverPMIs);
    
    return csiInfoPerPMI(tempPMI);
}

CSI_Info_S CQIEstimatorNR::getBestPMIandCQIOverWideband(Link_Param_S* linkParam,int nLayers)
{
    Array<cmat> codebook;
    type1PMI_S precIndex;int widebandPMI=0;
    sinrOverWideband.set_length(0);
    
    if(nCSIRSPorts>1){
    if(linkParam->typeIMethod ==_SEARCHFREE_ && nLayers<=4) //SearchFree 
    {
        return computeWBCSIinfoUsingSearchFree(nLayers,linkParam); 
    }
    else if(nLayers==1 && mCodebook->getCodeBookConfig().codeBookMode == _NR_CODEBOOK_MODE_1_) //Go for best i1
    {
        precIndex = getBestI1();
        widebandPMI = mCodebook->getPrecoderIndxAbsNR(nLayers,precIndex);
        codebook.set_length(1);
        codebook(0) = mCodebook->getPrecoderNR(nLayers,precIndex);
    }
    else //Exhaustive Search where the Searching Codebook has all the precoders 
    {
        widebandPMI=0;
        codebook = mCodebook->getCodebook(nCSIRSPorts,nLayers,1);
    }
    }
    else
    {
        codebook.set_length(1);
        codebook(0) = ones_c(1,1);
    }
    CSI_Info_S csiInfo = getCSIinfoOfBestPrecoder(nLayers,codebook,linkParam);
        //Copying Wideband PMI to csiInfo.PMI
    csiInfo.PMI +=widebandPMI;
    csiInfo.type1PMI = mCodebook->getPMIIndices(nLayers,csiInfo.PMI);
    return csiInfo;
}

int getLayerIndicatorNR(Array<vec>& sinrPerCodeword,ivec& CQIindexPerCW,int nCodewords,int nLayers)
{
    Array<ivec> layerIndication = getLayerIndication(sinrPerCodeword,nCodewords,nLayers,true);
    int layerIndicator = layerIndication(0)(0);
    if(nCodewords==2)
    {
        layerIndicator = (CQIindexPerCW(0)>=CQIindexPerCW(1)) ? layerIndication(0)(0) : layerIndication(1)(0) + floor_i((double)nLayers/(double)nCodewords);
    }
    return layerIndicator; // following zero-based indexing
}

ivec getLayerIndicatorPerPrecoderNR(Array<CSI_Info_S>& csiInfoPerPMI)
{
    int codebookLength = csiInfoPerPMI.length();
    ivec layerIndicatorPerPrecoder(codebookLength);
    for(int precIndex=0;precIndex<codebookLength;precIndex++)
    {
        csiInfoPerPMI(precIndex).layerIndicator = getLayerIndicatorNR(csiInfoPerPMI(precIndex).sinrPerCodeword,csiInfoPerPMI(precIndex).CQIindexPerCW,csiInfoPerPMI(precIndex).nCodeWords,csiInfoPerPMI(precIndex).nLayers);
        layerIndicatorPerPrecoder(precIndex) = csiInfoPerPMI(precIndex).layerIndicator;
    }
    return layerIndicatorPerPrecoder;
}

CSI_Info_S CQIEstimatorNR::getCSIinfoOfBestPrecoderUsingAvgChannel(int nLayers,Array<cmat> codebook,Link_Param_S* linkParam)
{
    int start = linkParam->rbNumber*reCountPerRb;
    int stop = (linkParam->rbNumber+linkParam->nRBs)*reCountPerRb-1;
    cmat avgChannel = mean(entireRawChannel(start,stop));

    cmat IntrCov = mean(perToneInterferenceCovariance(start,stop));
    int nCodewords = (nLayers>4) ? 2 : 1;
    int codebookLength = codebook.length();
    double sigpowInWatts = inv_dB(linkParam->myTxSCPowerInDB);
    cmat normalisedChannel = sqrt(sigpowInWatts)*avgChannel*linkParam->portToAntennaMapper->getPortToAntennaMapper(nCSIRSPorts,avgChannel.cols());
    
    Array<CSI_Info_S> csiInfoPerPMI(codebookLength);
    
    vec spectralEfficiencyOverPMIs(codebookLength);
    
    for(int precIndex=0;precIndex<codebookLength;precIndex++)
    {
        vec sinrPerLayer = getSINRperLayer(linkParam->demodulationScheme,normalisedChannel*codebook(precIndex),IntrCov,nLayers,sigpowInWatts);
        Array<vec> sinrPerCodeword = performLayerDeMapping(sinrPerLayer,nCodewords,nLayers);
        csiInfoPerPMI(precIndex) = computeCQIPerCodewords(sinrPerCodeword,linkParam->nRBs,linkParam->nREsPerRB,linkParam->txScheme,nLayers,linkParam->enableCQI0);
        csiInfoPerPMI(precIndex).PMI = precIndex; 
        csiInfoPerPMI(precIndex).sinrPerCodeword = sinrPerCodeword;
        spectralEfficiencyOverPMIs(precIndex) = csiInfoPerPMI(precIndex).specEff;
    }
    CSI_Info_S csiInfo = csiInfoPerPMI(max_index(spectralEfficiencyOverPMIs));
    
    if(linkParam->enableLayerIndication)
        csiInfo.layerIndicator = getLayerIndicatorNR(csiInfo.sinrPerCodeword,csiInfo.CQIindexPerCW,nCodewords,nLayers);
    
    return csiInfo;
}

CSI_Info_S CQIEstimatorNR::getCSIinfoOfBestPrecoder(int nLayers,Array<cmat>& codebook,Link_Param_S* linkParam)
{
    int nCodewords = (nLayers > 4) ? 2 :1;
    
    Array<Array<vec>> sinrPerPMI = computeSinrPerCWForCodeBookNR(linkParam->txScheme,linkParam->myTxSCPowerInDB, linkParam->demodulationScheme, linkParam->portToAntennaMapper,linkParam->rbNumber,nLayers,nCodewords,linkParam->nRBs,1, codebook);
    CSI_Info_S csiInfo = getBestPMIandCQI(sinrPerPMI,linkParam->nRBs,linkParam->nREsPerRB,linkParam->txScheme,nLayers,linkParam->enableCQI0);
    
    if(linkParam->enableLayerIndication)
        csiInfo.layerIndicator = getLayerIndicatorNR(csiInfo.sinrPerCodeword,csiInfo.CQIindexPerCW,nCodewords,nLayers);
    
    return csiInfo;
}

CSI_Param_S* CQIEstimatorNR::computeCSIparamsForCodebook(int nLayers,Array<cmat> codebook,Link_Param_S* linkParam)
{
    CSI_Param_S* csiParam = new CSI_Param_S(linkParam);
    int nCodewords = (nLayers > 4) ? 2 :1;
    Array<Array<ivec> > layerIndicationPerCWPerPrecoder;
    csiParam->sinrPerPMI = computeSinrPerCWForCodeBookNR(linkParam->txScheme,linkParam->myTxSCPowerInDB, linkParam->demodulationScheme, linkParam->portToAntennaMapper,linkParam->rbNumber,nLayers,nCodewords,linkParam->nRBs,1, codebook);
    
    CSI_Info_S csiInfo = getBestPMIandCQI(csiParam->sinrPerPMI,linkParam->nRBs,linkParam->nREsPerRB,linkParam->txScheme,nLayers,linkParam->enableCQI0,csiParam->spectralEfficiencyOverPMIs,csiParam->cqiIndexPerCWPerPrecoder,csiParam->csiInfoPerPMI);
    csiParam->Codebook=codebook;
    csiParam->maxIndex = csiInfo.PMI;
    if(linkParam->enableLayerIndication)
        csiParam->layerIndicatorPerPrecoder = getLayerIndicatorPerPrecoderNR(csiParam->csiInfoPerPMI);

    return csiParam;
}

void CQIEstimatorNR::computeDLAperiodicCSIForTypeIMultiPanel(DLCQIMeasurement_S& perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, CQIMeasurementConfig_S& cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int nodeID,int eNBID, int subframeNumber, string AperiodicCQILogsFileName)
{
    TransmissionScheme_E txScheme = cqiMeasurementConfig.transmissionScheme;
    checkNRTransmissionScheme(txScheme);
    
    configureCQIEstimatorNR(cqiMeasurementConfig.cqiPorts,frameStructure);
    
    int nLayers = cqiMeasurementConfig.numberOfLayers;
    if(nLayers>4)
    {
        cout<<"[both:] Type-I Multipanel Codebook doesnot Support Rank greater than 4..Aborting!!."<<endl;abort();
    }
    int nCodewords = cqiMeasurementConfig.numberOfCodeWords;
    int nREsPerRB = getNumOfREsPerRB(CSIRSPorts,frameStructure,cqiMeasurementConfig.rsSymPositions,cqiMeasurementConfig.rsScPositions);
    
    int DLRBs = cqiMeasurementConfig.nRBs;
    int rbNumber = cqiMeasurementConfig.startRB;
    PMIFormatIndicator pmiFormatIndicator = cqiMeasurementConfig.pmiFormatIndicator;
    CQIFormatIndicator cqiFormatIndicator = cqiMeasurementConfig.cqiFormatIndicator;
    TypeIMethod typeIMethod = cqiMeasurementConfig.typeIMethod;
    int nSubbands = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length();
    
    //CQI extracting the channel over all the DLRBS on positions corresponding to CSIRS Ports 
    if(CQIInfo.subframeNumber != subframeNumber) //Channel and Interference are computed only when they are not computed in that subframe
    {
        getRawPhysicalChannelAcrossRE(entireRawChannel,resourceMapper,myChannel,rbNumber,true,reCountPerRb,DLRBs,portLocations);
        perToneInterferenceCovariance=getPerToneInterferenceCovariance(rxFrame,frameStructure,resourceMapper,true,myChannel,portToAntennaMapper,refSignal,rbRSPattern, portReferenceNumbers,txScheme,reCountPerRb,rbNumber,DLRBs,CSIRSPorts,inv_dB(myTxSCPowerInDB),RSRP);
        avgWidebandChannel = mean(entireRawChannel); // --Add if needed.
        cqiMeasurementConfig.noiseVar=noiseVariance;
        cqiMeasurementConfig.intrCov=perToneInterferenceCovariance;
    }
        
    switch(downlinkCQIInfo->cqiComputationInfo.aperiodicMode)
    {
        case _APERIODIC_MODE_3_2_:
        {
            CSI_Info_S wb_csiInfo;
            clock_t t = clock();
            
            if(CQIInfo.isCQIComputed && CQIInfo.subframeNumber == subframeNumber) //Skipping Wideband PMI and CQI Computations if they are already computed in Rank Computation
            {
                wb_csiInfo = CQIInfo.csiInfo;
            }
            else 
            {
                Link_Param_S* linkParam = new Link_Param_S(txScheme,demodulationScheme,portToAntennaMapper,myTxSCPowerInDB,rbNumber,DLRBs,nREsPerRB,downlinkCQIInfo->cqiComputationInfo.enableCQI0,typeIMethod);
                linkParam->enableLayerIndication = (cqiFormatIndicator == _WIDEBAND_CQI_ && pmiFormatIndicator == _WIDEBAND_PMI_); //should be enabled only if both PMI and CQI are Wideband reported.
                wb_csiInfo = getBestPMIandCQIOverWideband(linkParam,nLayers);
                delete linkParam;
            }
            
            type1PMI_S precIndex = wb_csiInfo.type1PMI;
            perUserFeedback.nrMPCQIInfoAPeriodic.wideBandPrecoderIndi1_1 = precIndex.i11;
            perUserFeedback.nrMPCQIInfoAPeriodic.wideBandPrecoderIndi1_2 = precIndex.i12;
            perUserFeedback.nrMPCQIInfoAPeriodic.wideBandPrecoderIndi1_3 = precIndex.i13;
            perUserFeedback.nrMPCQIInfoAPeriodic.wideBandPrecoderIndi1_4 = precIndex.i14;
            
            perUserFeedback.nrMPCQIInfoAPeriodic.widebandPMI = wb_csiInfo.PMI;
            
            Array<cmat> i2CodeBook;
            i1index_S bestI1 = i1index_S(precIndex.i11,precIndex.i12,precIndex.i13,precIndex.i14);
            ivec precIndex_i2 = mCodebook->geti2CmatIndicesIn_W(bestI1,nLayers);
            if(typeIMethod == _SPECTRAL_EFFICIENCY_)
            {
                i2CodeBook = mCodebook->getDLi2Codebook(bestI1,nLayers);
            }
            
            perUserFeedback.nrMPCQIInfoAPeriodic.sbCqiIndexPerCodeWord.set_size(nSubbands); // <subband<nCodeword>>
            perUserFeedback.nrMPCQIInfoAPeriodic.subbandPrecIndi2.set_size(nSubbands);
            perUserFeedback.nrMPCQIInfoAPeriodic.subbandPMI.set_size(nSubbands);
            perUserFeedback.nrMPCQIInfoAPeriodic.subbandIndex.set_size(nSubbands);
            perUserFeedback.nrMPCQIInfoAPeriodic.channelPerSubBand.set_size(nSubbands);
            
            Array<vec>  sinrPerCodewordForwidebandCQIperCWtemp(nCodewords);   // <codewords<REs>>
            
            Array <vec> sbEffSINRsPerCW(nSubbands);
            
            int currentRB=0, localRB=0;
            for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
            {
                int nRBs = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(subbandCnt);
                if(currentRB>=rbNumber && currentRB<(rbNumber+DLRBs)) // Valid subband for the measurement config
                {
                    int start = localRB*reCountPerRb;
                    int stop = (localRB+nRBs)*reCountPerRb-1;
                    
                    Array<cmat> currentChannel=entireRawChannel(start,stop);
                    
                    int i2 = precIndex.i2,subbandPMI = wb_csiInfo.PMI;ivec subbandCQIperCW = wb_csiInfo.CQIindexPerCW;
                    vec subbandEffSinrperCW = wb_csiInfo.effSINRsPerCW;
                    
                    CSI_Info_S sb_csiInfo;
                    
                    if(pmiFormatIndicator == _SUBBAND_PMI_ || cqiFormatIndicator == _SUBBAND_CQI_)
                    {                    
                        Link_Param_S* linkParam = new Link_Param_S(txScheme,demodulationScheme,portToAntennaMapper,myTxSCPowerInDB,currentRB,nRBs,nREsPerRB,downlinkCQIInfo->cqiComputationInfo.enableCQI0,typeIMethod);
                        
                        if(typeIMethod == _SEARCHFREE_) // i2 is computed for subband reporting and searchfree is enabled
                        {
                            sb_csiInfo = computei2forSubband(nLayers,precIndex,linkParam);
                        }
                        else
                        {
                            sb_csiInfo = getCSIinfoOfBestPrecoder(nLayers,i2CodeBook,linkParam);
                        }
                        if(cqiFormatIndicator == _SUBBAND_CQI_)
                        {
                            subbandCQIperCW = sb_csiInfo.CQIindexPerCW;
                            subbandEffSinrperCW = sb_csiInfo.effSINRsPerCW;
                        }
                        
                        if(pmiFormatIndicator == _SUBBAND_PMI_)
                        {
                            i2 = sb_csiInfo.PMI;
                            subbandPMI = precIndex_i2(i2);
                        }
                        
                        // concatinating subband sinr based on chosen PMI for respective subband
                        concatInArray(sinrPerCodewordForwidebandCQIperCWtemp,sb_csiInfo.sinrPerCodeword);
                        delete linkParam;
                    }
                    
                    perUserFeedback.nrMPCQIInfoAPeriodic.sbCqiIndexPerCodeWord(subbandCnt) = subbandCQIperCW;
                    perUserFeedback.nrMPCQIInfoAPeriodic.subbandPMI(subbandCnt) = subbandPMI;
                    perUserFeedback.nrMPCQIInfoAPeriodic.subbandPrecIndi2(subbandCnt) = i2;
                    perUserFeedback.nrMPCQIInfoAPeriodic.subbandIndex(subbandCnt)=subbandCnt;
                    sbEffSINRsPerCW(subbandCnt) = subbandEffSinrperCW; 
                    
                    if(currentChannel.length()>10)
                        perUserFeedback.nrMPCQIInfoAPeriodic.channelPerSubBand(subbandCnt)=randPick(currentChannel,10);
                    else
                        perUserFeedback.nrMPCQIInfoAPeriodic.channelPerSubBand(subbandCnt)=currentChannel;
                    
                    localRB+=nRBs;
                }
                currentRB += nRBs;
            }
            
            if(cqiFormatIndicator == _SUBBAND_CQI_ || pmiFormatIndicator == _SUBBAND_PMI_)
            {
                wb_csiInfo = computeCQIPerCodewords(sinrPerCodewordForwidebandCQIperCWtemp,DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
            }
            
            perUserFeedback.nrMPCQIInfoAPeriodic.wbCqiIndexPerCodeWord=wb_csiInfo.CQIindexPerCW;
            perUserFeedback.nrMPCQIInfoAPeriodic.isValid=true;
      
            double computationTime = (double)(clock()-t)/CLOCKS_PER_SEC;
            
            if(CQIInfo.subframeNumber==subframeNumber){computationTime += CQIInfo.computationTime;}
            
            #ifdef CSI_DEBUG
            cout<<"\nnLayers: "<<nLayers<<"\tReported PMI Indices "<<precIndex.printValues()<<" \tWideband PMI: "<<perUserFeedback.nrMPCQIInfoAPeriodic.widebandPMI;
            if(pmiFormatIndicator){cout<<" \tSubband i2: "<<perUserFeedback.nrMPCQIInfoAPeriodic.subbandPrecIndi2;}
            cout<<" \tWideband CQI : "<<perUserFeedback.nrMPCQIInfoAPeriodic.wbCqiIndexPerCodeWord<<" \teffSINRsPerCW : "<<wb_csiInfo.effSINRsPerCW;
            if(cqiFormatIndicator){cout<<" \tSubband CQI : "<<perUserFeedback.nrMPCQIInfoAPeriodic.sbCqiIndexPerCodeWord;}
            else if(!pmiFormatIndicator){cout<<"\tLayerIndicator: "<<wb_csiInfo.layerIndicator;}
            cout<<" \tSpectral Efficiency: "<<wb_csiInfo.specEff<<" \tComputation Time: "<<computationTime<<" seconds\n"<<endl;
            #endif
            
            #ifndef CSI_DEBUG
            cout<<"\tRank: "<<nLayers<<"\tWideband PMI: "<<perUserFeedback.nrMPCQIInfoAPeriodic.widebandPMI<<"\t Indices: "<<precIndex.printValues()<<endl;
            cout<<"\tWideband CQI: "<<perUserFeedback.nrMPCQIInfoAPeriodic.wbCqiIndexPerCodeWord<<"\tSpecEff: "<<wb_csiInfo.specEff<<endl;
            #endif
          
            if(traceValuePerTraceName["EnableUElogs"]=="true") 
            {
               // string AperiodicPMILogsFileName = "./Results/NU_1/AperiodicPMILogsFileName.txt";
                printCQIindices(downlinkCQIInfo->cqiComputationInfo.aperiodicMode,nodeID,eNBID,nLayers,perUserFeedback.nrMPCQIInfoAPeriodic.wbCqiIndexPerCodeWord,wb_csiInfo.modOrderPerCW(),wb_csiInfo.effSINRsPerCW, subframeNumber,AperiodicCQILogsFileName,perUserFeedback.nrMPCQIInfoAPeriodic.widebandPMI,perUserFeedback.nrMPCQIInfoAPeriodic.sbCqiIndexPerCodeWord, perUserFeedback.nrMPCQIInfoAPeriodic.subbandPMI,computationTime,getVectorFromPMIIndices(precIndex,_NR_CODEBOOK_TYPE_I_MULTIPANEL_));
               // PrintPMILog(downlinkCQIInfo->cqiComputationInfo.aperiodicMode,nodeID,subframeNumber,AperiodicPMILogsFileName,perUserFeedback.nrMPCQIInfoAPeriodic);
            }
            break;
        }
        
        default :
        {
            cout<<"[both:] not ready for other aperiodic cases"<<endl;abort();
        }
    }
    perUserFeedback.isValid=true;
}

void CQIEstimatorNR::computeDLAperiodicCSIForTypeI(DLCQIMeasurement_S& perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, CQIMeasurementConfig_S& cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int nodeID,int eNBID, int subframeNumber, string AperiodicCQILogsFileName)
{
  TransmissionScheme_E txScheme = cqiMeasurementConfig.transmissionScheme;
  checkNRTransmissionScheme(txScheme);
  
  configureCQIEstimatorNR(cqiMeasurementConfig.cqiPorts,frameStructure);
  
  int nLayers    = cqiMeasurementConfig.numberOfLayers;
  int nCodewords = cqiMeasurementConfig.numberOfCodeWords;
  int nREsPerRB = getNumOfREsPerRB(CSIRSPorts,frameStructure,cqiMeasurementConfig.rsSymPositions,cqiMeasurementConfig.rsScPositions);
  
  int DLRBs = cqiMeasurementConfig.nRBs;
  int rbNumber = cqiMeasurementConfig.startRB;
  PMIFormatIndicator pmiFormatIndicator = cqiMeasurementConfig.pmiFormatIndicator;
  CQIFormatIndicator cqiFormatIndicator = cqiMeasurementConfig.cqiFormatIndicator;
  TypeIMethod typeIMethod = cqiMeasurementConfig.typeIMethod;
  int nSubbands = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length();
  
  CBParam_S cbParms=mCodebook->getCodebookParam(nLayers);
  if(cqiMeasurementConfig.isCodebookRestrictionEnabled)
  {
    //string file=toString(currentTaskid)+"range.txt";
    //std::ofstream fout(file.c_str(), std::ios::app);
    if(cqiMeasurementConfig.isPMIHistoryCollected && !cqiMeasurementConfig.isFeedbackAvailable)
    {
        ivec wbPMIHistoryDiffi11(cqiMeasurementConfig.wbPMIHistory.length());
        ivec wbPMIHistoryDiffi12(cqiMeasurementConfig.wbPMIHistory.length());
        ivec wbPMIHistoryDiffi13(cqiMeasurementConfig.wbPMIHistory.length());
        wbPMIHistoryDiffi11(0)=0;
        wbPMIHistoryDiffi12(0)=0;
        wbPMIHistoryDiffi13(0)=0;
        
        for(int FeedVar=0;FeedVar<cqiMeasurementConfig.wbPMIHistory.length();FeedVar++)
        {
            if(FeedVar>0)
            {
                wbPMIHistoryDiffi11(FeedVar)=abs(cqiMeasurementConfig.wbPMIHistory(FeedVar).i11-cqiMeasurementConfig.wbPMIHistory(FeedVar-1).i11);
                
                if(wbPMIHistoryDiffi11(FeedVar)>cbParms.i11Size/2)
                    wbPMIHistoryDiffi11(FeedVar)=cbParms.i11Size-wbPMIHistoryDiffi11(FeedVar);
                
                wbPMIHistoryDiffi12(FeedVar)=abs(cqiMeasurementConfig.wbPMIHistory(FeedVar).i12-cqiMeasurementConfig.wbPMIHistory(FeedVar-1).i12);

                if(wbPMIHistoryDiffi12(FeedVar)>cbParms.i12Size/2)
                    wbPMIHistoryDiffi12(FeedVar)=cbParms.i12Size-wbPMIHistoryDiffi12(FeedVar);
                
                wbPMIHistoryDiffi13(FeedVar)=abs(cqiMeasurementConfig.wbPMIHistory(FeedVar).i13-cqiMeasurementConfig.wbPMIHistory(FeedVar-1).i13);
                
                if(wbPMIHistoryDiffi13(FeedVar)>cbParms.i13Size/2)
                    wbPMIHistoryDiffi13(FeedVar)=cbParms.i13Size-wbPMIHistoryDiffi13(FeedVar);
                
            }
            
            
        }
        cqiMeasurementConfig.i11Range=max(wbPMIHistoryDiffi11);
        cqiMeasurementConfig.i12Range=max(wbPMIHistoryDiffi12);
        cqiMeasurementConfig.i13Range=max(wbPMIHistoryDiffi13);      
        cqiMeasurementConfig.isFeedbackAvailable=true;
//         fout<<cqiMeasurementConfig.i11Range<<cqiMeasurementConfig.i12Range<<cqiMeasurementConfig.i13Range<<endl; 
    }
    //fout.close();
  }
  int i2Range=cbParms.i2Size;
  
  
  //CQI extracting the channel over all the DLRBS on positions corresponding to CSIRS Ports
  if(CQIInfo.subframeNumber != subframeNumber) //Channel and interference are calculated only when they are not computed in that subframe
  {
    getRawPhysicalChannelAcrossRE(entireRawChannel,resourceMapper,myChannel,rbNumber,true,reCountPerRb,DLRBs,portLocations);
    perToneInterferenceCovariance=getPerToneInterferenceCovariance(rxFrame,frameStructure,resourceMapper,true,myChannel,portToAntennaMapper,refSignal,rbRSPattern, portReferenceNumbers,txScheme,reCountPerRb,rbNumber,DLRBs,CSIRSPorts,inv_dB(myTxSCPowerInDB),RSRP);
    avgWidebandChannel = mean(entireRawChannel);
  }
	 
  switch(downlinkCQIInfo->cqiComputationInfo.aperiodicMode)
  {
    case _APERIODIC_MODE_3_2_:
    {
      CSI_Info_S wb_csiInfo;
      
      clock_t t = clock();
      
      if(CQIInfo.isCQIComputed && CQIInfo.subframeNumber == subframeNumber) //Skipping Wideband PMI and CQI Computations if they are already computed in Rank Computation
      {
          wb_csiInfo = CQIInfo.csiInfo;
      }
      else      //Compute Wideband PMI and CQI using the typeIMethod
      {
          Link_Param_S* linkParam = new Link_Param_S(txScheme,demodulationScheme,portToAntennaMapper,myTxSCPowerInDB,rbNumber,DLRBs,nREsPerRB,downlinkCQIInfo->cqiComputationInfo.enableCQI0,typeIMethod);
          linkParam->enableLayerIndication = (cqiFormatIndicator == _WIDEBAND_CQI_ && pmiFormatIndicator == _WIDEBAND_PMI_);  //should be enabled only if both PMI and CQI are Wideband reported.
          wb_csiInfo = getBestPMIandCQIOverWideband(linkParam,nLayers);
          delete linkParam;
      }
      
      type1PMI_S precIndex = wb_csiInfo.type1PMI;
      perUserFeedback.nrCQIInfoAPeriodic.wideBandPrecoderIndi1_1 = precIndex.i11;
      perUserFeedback.nrCQIInfoAPeriodic.wideBandPrecoderIndi1_2 = precIndex.i12;
      perUserFeedback.nrCQIInfoAPeriodic.wideBandPrecoderIndi1_3 = precIndex.i13;
      
      perUserFeedback.nrCQIInfoAPeriodic.widebandPMI = wb_csiInfo.PMI;
      
      perUserFeedback.nrCQIInfoAPeriodic.sbCqiIndexPerCodeWord.set_size(nSubbands); // <subband<nCodeword>>
      perUserFeedback.nrCQIInfoAPeriodic.subbandPrecIndi2.set_size(nSubbands);
      perUserFeedback.nrCQIInfoAPeriodic.subbandPMI.set_size(nSubbands);
      perUserFeedback.nrCQIInfoAPeriodic.subbandIndex.set_size(nSubbands);
      perUserFeedback.nrCQIInfoAPeriodic.channelPerSubBand.set_size(nSubbands);
      Array<vec>  sinrPerCodewordForwidebandCQIperCWtemp(nCodewords);   // <codewords<REs>>
      
      ivec precIndex_i2;
      
      Array<cmat> i2CodeBook;
      if(nCSIRSPorts <= 2)
      {
        // Assuming I1 and I2 has same index for port less than 4
        i2CodeBook = mCodebook->getCodebook(nCSIRSPorts,nLayers,1);
        precIndex_i2 = getIntegers(0,i2CodeBook.length()-1);
      }
      else
      {
        i1index_S bestI1 = i1index_S(precIndex.i11,precIndex.i12,precIndex.i13,-1);   
        precIndex_i2 = mCodebook->geti2CmatIndicesIn_W(bestI1,nLayers);
        i2CodeBook = mCodebook->getDLi2Codebook(bestI1,nLayers);
      }
      
      Array <vec> sbeffSINRsPerCW(nSubbands);
      
      int currentRB=0, localRB=0;
      for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
      {
        int nRBs = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(subbandCnt);
        if(currentRB>=rbNumber && currentRB<(rbNumber+DLRBs)) // Valid subband for the measurement config
        {
          int start = localRB*reCountPerRb;
          int stop = (localRB+nRBs)*reCountPerRb-1;
          
          Array<cmat> currentChannel=entireRawChannel(start,stop);
          
          int subbandPMI = wb_csiInfo.PMI,i2 = precIndex.i2;ivec subbandCQIperCW = wb_csiInfo.CQIindexPerCW;
          vec subbandEffSinrperCW = wb_csiInfo.effSINRsPerCW;
          
          if(cqiFormatIndicator == _SUBBAND_CQI_ || pmiFormatIndicator == _SUBBAND_PMI_)
          {
                CSI_Info_S sb_csiInfo;
                Link_Param_S* linkParam = new Link_Param_S(txScheme,demodulationScheme,portToAntennaMapper,myTxSCPowerInDB,currentRB,nRBs,nREsPerRB,downlinkCQIInfo->cqiComputationInfo.enableCQI0,typeIMethod);
            
                if(typeIMethod == _SEARCHFREE_ && nCSIRSPorts>2 && nLayers<=4)
                {
                    sb_csiInfo = computei2forSubband(nLayers,precIndex,linkParam); //Computed by Search Free algorithm
                }
                else
                {
                    sb_csiInfo = getCSIinfoOfBestPrecoder(nLayers,i2CodeBook,linkParam);
                }
                if(cqiFormatIndicator == _SUBBAND_CQI_)
                {
                    subbandCQIperCW = sb_csiInfo.CQIindexPerCW;
                    subbandEffSinrperCW = sb_csiInfo.effSINRsPerCW;
                }
          
                if(pmiFormatIndicator == _SUBBAND_PMI_)
                {
                    i2 = sb_csiInfo.PMI;
                    subbandPMI = precIndex_i2(i2);
                }
          
                // concatinating subband sinr based on chosen PMI for respective subband
                concatInArray(sinrPerCodewordForwidebandCQIperCWtemp,sb_csiInfo.sinrPerCodeword);
                delete linkParam;
          }
	       
          perUserFeedback.nrCQIInfoAPeriodic.sbCqiIndexPerCodeWord(subbandCnt) = subbandCQIperCW;
          perUserFeedback.nrCQIInfoAPeriodic.subbandPMI(subbandCnt) = subbandPMI;
          perUserFeedback.nrCQIInfoAPeriodic.subbandPrecIndi2(subbandCnt) = i2;
          perUserFeedback.nrCQIInfoAPeriodic.subbandIndex(subbandCnt)=subbandCnt;
          sbeffSINRsPerCW(subbandCnt) = subbandEffSinrperCW; 
          
          if(currentChannel.length()>10)
              perUserFeedback.nrCQIInfoAPeriodic.channelPerSubBand(subbandCnt)=randPick(currentChannel,10);
          else
              perUserFeedback.nrCQIInfoAPeriodic.channelPerSubBand(subbandCnt)=currentChannel;
          
          localRB+=nRBs;
        }
        currentRB += nRBs;
      }
      
      if(cqiFormatIndicator == _SUBBAND_CQI_ || pmiFormatIndicator == _SUBBAND_PMI_)
      {
          wb_csiInfo = computeCQIPerCodewords(sinrPerCodewordForwidebandCQIperCWtemp,DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
      }
      
      perUserFeedback.nrCQIInfoAPeriodic.wbCqiIndexPerCodeWord=wb_csiInfo.CQIindexPerCW;
      perUserFeedback.nrCQIInfoAPeriodic.isValid=true;
       
      double computationTime = (double)(clock()-t)/CLOCKS_PER_SEC;
      
      if(CQIInfo.subframeNumber==subframeNumber){computationTime += CQIInfo.computationTime;}
      
      #ifdef CSI_DEBUG
      cout<<"\nnLayers: "<<nLayers<<"\tReported PMI Indices "<<precIndex.printValues()<<" \tWideband PMI: "<<perUserFeedback.nrCQIInfoAPeriodic.widebandPMI;
      if(pmiFormatIndicator){cout<<"\tSubband i2: "<<perUserFeedback.nrCQIInfoAPeriodic.subbandPrecIndi2;}
      cout<<"\tWideband CQI : "<<perUserFeedback.nrCQIInfoAPeriodic.wbCqiIndexPerCodeWord<<" \t effSINRsPerCW : "<<wb_csiInfo.effSINRsPerCW;
      if(cqiFormatIndicator){cout<<"\tSubband CQI : "<<perUserFeedback.nrCQIInfoAPeriodic.sbCqiIndexPerCodeWord;}
      else if(!pmiFormatIndicator){cout<<"\tLayerIndicator: "<<wb_csiInfo.layerIndicator;}
      cout<<" \tSpectral Efficiency: "<<wb_csiInfo.specEff<<" \tComputation Time: "<<computationTime<<" seconds\n"<<endl;
      #endif
      
      #ifndef CSI_DEBUG
      cout<<"\tRank: "<<nLayers<<"\tWideband PMI: "<<perUserFeedback.nrCQIInfoAPeriodic.widebandPMI<<"\t Indices: "<<precIndex.printValues()<<endl;
      cout<<"\tWideband CQI: "<<perUserFeedback.nrCQIInfoAPeriodic.wbCqiIndexPerCodeWord<<"\tSpecEff: "<<wb_csiInfo.specEff<<endl;
      #endif
      
      if(traceValuePerTraceName["EnableUElogs"]=="true") 
      {
        //string AperiodicPMILogsFileName = "./Results/NU_1/AperiodicPMILogsFileName.txt";
        printCQIindices(downlinkCQIInfo->cqiComputationInfo.aperiodicMode,nodeID,eNBID,nLayers,perUserFeedback.nrCQIInfoAPeriodic.wbCqiIndexPerCodeWord,wb_csiInfo.modOrderPerCW(),wb_csiInfo.effSINRsPerCW, subframeNumber,AperiodicCQILogsFileName,perUserFeedback.nrCQIInfoAPeriodic.widebandPMI,perUserFeedback.nrCQIInfoAPeriodic.sbCqiIndexPerCodeWord, perUserFeedback.nrCQIInfoAPeriodic.subbandPMI,computationTime,getVectorFromPMIIndices(precIndex,_NR_CODEBOOK_TYPE_I_));
       // PrintPMILog(downlinkCQIInfo->cqiComputationInfo.aperiodicMode,nodeID,subframeNumber,AperiodicPMILogsFileName,perUserFeedback.nrCQIInfoAPeriodic);
      }
      break;
    }
  
    default :
    {
        cout<<"[both:] not ready for other aperiodic cases"<<endl;abort();
    }
  }
  perUserFeedback.isValid=true;
    cqiMeasurementConfig.previousFeedback=perUserFeedback.nrCQIInfoAPeriodic;
    
   // cout<<"FB Info : "<<cqiMeasurementConfig.previousFeedback.wideBandPrecoderIndi1_1<<" , "<<cqiMeasurementConfig.previousFeedback.wideBandPrecoderindi1_2<<" , "<<cqiMeasurementConfig.previousFeedback.wideBandPrecoderindi1_3<<endl;
  
  if(cqiMeasurementConfig.isCodebookRestrictionEnabled)  
    if(!cqiMeasurementConfig.isPMIHistoryCollected) 
        if(subframeNumber>=cqiMeasurementConfig.trainingDurationInSlots)  
        cqiMeasurementConfig.isPMIHistoryCollected=true;
}

void CQIEstimatorNR::computeDLAperiodicCSIForTypeII(DLCQIMeasurement_S& perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, CQIMeasurementConfig_S& cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int nodeID,int eNBID, int subframeNumber, string AperiodicCQILogsFileName)
{
  TransmissionScheme_E txScheme = cqiMeasurementConfig.transmissionScheme;
  checkNRTransmissionScheme(txScheme);

  configureCQIEstimatorNR(cqiMeasurementConfig.cqiPorts,frameStructure);
  
  int nLayers    = cqiMeasurementConfig.numberOfLayers;
  if(nLayers > 2)
  {
    cout << "[both:] Type II doesn't support nLayers > 2. Aborting !! "<<endl;abort();
  }
  
  int nCodewords = cqiMeasurementConfig.numberOfCodeWords;
  int nREsPerRB = getNumOfREsPerRB(CSIRSPorts,frameStructure,cqiMeasurementConfig.rsSymPositions,cqiMeasurementConfig.rsScPositions);

  int DLRBs = cqiMeasurementConfig.nRBs;
  int rbNumber = cqiMeasurementConfig.startRB;
  int nSubbands = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length();
  
  CodeBookConfig_S codebookConfig = mCodebook->getCodeBookConfig();
  
  int Lbeams = codebookConfig.LBeams;
  int nPSK = codebookConfig.nPSK;

  //CQI extracting the channel over all the DLRBS on positions corresponding to CSIRS Ports
  if(CQIInfo.subframeNumber!=subframeNumber)
  {
    getRawPhysicalChannelAcrossRE(entireRawChannel,resourceMapper,myChannel,rbNumber,true,reCountPerRb,DLRBs,portLocations);
    perToneInterferenceCovariance=getPerToneInterferenceCovariance(rxFrame,frameStructure,resourceMapper,true,myChannel,portToAntennaMapper,refSignal,rbRSPattern, portReferenceNumbers,txScheme,reCountPerRb,rbNumber,DLRBs,CSIRSPorts,inv_dB(myTxSCPowerInDB),RSRP);
    //avgWidebandChannel = average(entireRawChannel);  --Add if needed.
  }
  
  Array<Type1HalfBeamWithi1> type1halfBeamswithi1 = mCodebook->getType1HalfBeam(); //cbMode 0 only used
  Type2coefficients ampAndPhaseCooeff = getBestAmplitudeAndPhase(type1halfBeamswithi1,Lbeams,nPSK,demodulationScheme,inv_dB(myTxSCPowerInDB),nLayers,rbNumber,DLRBs);
  
  Array<cmat> orthBeams(Lbeams);
  
  for(int i=0;i<Lbeams;i++)
    orthBeams(i) = type1halfBeamswithi1(ampAndPhaseCooeff.orthBeamIndices(i)).Halfbeam;

  switch(downlinkCQIInfo->cqiComputationInfo.aperiodicMode)
  {
    case _APERIODIC_MODE_3_2_:
    {
      Array<vec> sinrPerCodewordForwidebandCQIperCWtemp(nCodewords);   	   // <codewords<REs>>

      perUserFeedback.nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord.set_size(nSubbands); // <subband<nCodeword>>
      perUserFeedback.nrTypeIICQIInfoAPeriodic.sbPrecoder.set_size(nSubbands);
      perUserFeedback.nrTypeIICQIInfoAPeriodic.channelPerSubBand.set_size(nSubbands);
      
      CSI_Info_S csiInfo;
      ivec subbandPMIIndex(nSubbands);

      Array <vec> sbeffSINRsPerCW(nSubbands);
      Array<ivec> layerIndicationPerCW;
      int currentRB=0, localRB=0;
      for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
      {
        int nRBs = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(subbandCnt);
        if(currentRB>=rbNumber && currentRB<(rbNumber+DLRBs))  // Valid subband for the measurement config
        {
          int start = localRB*reCountPerRb;
          int stop = (localRB+nRBs)*reCountPerRb-1;
          
          Array<cmat> currentRawChannel = entireRawChannel(start,stop);
          
          Array<cmat> type2Precoder = mCodebook->getType2PrecoderbyCombineLbeams(orthBeams,ampAndPhaseCooeff.finalAmplitude,ampAndPhaseCooeff.finalSubBandPhasePerOrthBeamPerPolarization,nLayers,subbandCnt,nPSK);
          Array< Array<vec> >sinrPerCodewordtemp1 = computeSinrPerCWForCodeBookNR(txScheme, myTxSCPowerInDB,demodulationScheme,portToAntennaMapper,currentRB,nLayers,nCodewords,nRBs,1,type2Precoder);   // <pmi<codewords<REs>>>>
          
          csiInfo = getBestPMIandCQI(sinrPerCodewordtemp1,nRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
          sbeffSINRsPerCW(subbandCnt) = csiInfo.effSINRsPerCW;
          
          subbandPMIIndex(subbandCnt) = csiInfo.PMI;
          
          perUserFeedback.nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord(subbandCnt)=csiInfo.CQIindexPerCW;
          
          perUserFeedback.nrTypeIICQIInfoAPeriodic.sbPrecoder(subbandCnt) = type2Precoder(csiInfo.PMI);
          
          perUserFeedback.nrTypeIICQIInfoAPeriodic.channelPerSubBand(subbandCnt) = currentRawChannel;
          
          // concatinating subband sinr based on chosen PMI for respective subband
          concatInArray(sinrPerCodewordForwidebandCQIperCWtemp,sinrPerCodewordtemp1(csiInfo.PMI));
          
          localRB+=nRBs;
        }
        currentRB+=nRBs;
      }
      
      // Calculation of wide band CQI : calculation is done by choosing best PMI per subband

      csiInfo = computeCQIPerCodewords(sinrPerCodewordForwidebandCQIperCWtemp,DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
      perUserFeedback.nrTypeIICQIInfoAPeriodic.wbCqiIndexPerCodeWord = csiInfo.CQIindexPerCW;
      
      perUserFeedback.nrTypeIICQIInfoAPeriodic.isValid=true;
      
      #ifdef CSI_DEBUG
      cout<<"\nSubband Precoder Index: "<<subbandPMIIndex;
      cout<<"\tType II wb CQI : "<<perUserFeedback.nrTypeIICQIInfoAPeriodic.wbCqiIndexPerCodeWord<<" \t effSINRsPerCW : "<<csiInfo.effSINRsPerCW;
      cout<<"\tType 1I sb CQI : "<<perUserFeedback.nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord<<" \t sbeffSINRsPerCW : "<<sbeffSINRsPerCW<<endl;
      #endif
      
      #ifndef CSI_DEBUG
      cout<<"\tRank: "<<nLayers<<"\t Subband Precoder Index: "<<subbandPMIIndex<<endl;
      #endif
      
      if(traceValuePerTraceName["EnableUElogs"]=="true") 
      {
        ivec subbandPMI = subbandPMIIndex;
        printCQIindices(downlinkCQIInfo->cqiComputationInfo.aperiodicMode,nodeID,eNBID,nLayers,perUserFeedback.nrTypeIICQIInfoAPeriodic.wbCqiIndexPerCodeWord,csiInfo.modOrderPerCW(),csiInfo.effSINRsPerCW,subframeNumber,AperiodicCQILogsFileName,-1,perUserFeedback.nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord,subbandPMI,0,"");
      }
      break;
    }
    
    default :
    {
      cout<<" [both:] not ready for other aperiodic cases"<<endl;abort();
    }
  }
  perUserFeedback.isValid=true;

}

Type2coefficients_OMP_S CQIEstimatorNR::getType2Avik(const int Lbeams, const int nLayers, const int nPSK)
{
    Type2coefficients_OMP_S type2Coefficients;
    int nTones = entireRawChannel.length();
    int nTx = entireRawChannel(0).cols();
    int nSubbands = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length();
    // Average channel covariance
    // Computing average of wideband channel (one polarizartion's channel)
    cmat avgChannelCovar = avgCovariance(entireRawChannel);
    cmat U,V;  vec S; cvec eigenValues;
    bool isSuccess = eig(avgChannelCovar, eigenValues, V);
    
    cvec firstSingularVector = (1/norm(V.get_col(0)))*V.get_col(0);
    //     cout << "Eigen V: "<< V<<endl;
    int selectedSet = -1;
    
    CodeBookConfig_S codebookConfig = mCodebook->getCodeBookConfig();
    
    int N1=codebookConfig.N1, N2=codebookConfig.N2, O1=codebookConfig.O1, O2 = codebookConfig.O2; 
    Array<ivec> i11_IndicesPerSet(O1*O2);  // Stores corresponding i11 index of the beams in set
    Array<ivec> i12_IndicesPerSet(O1*O2);  // Stores corresponding i12 index of the beams in set
    Array<cmat> orthBeamsTotalSets = type2OrthogonalBeamsSet(N1, N2, i11_IndicesPerSet, i12_IndicesPerSet); // Generating orthogonal beam sets
    cvec halfSingularVector = firstSingularVector(0,nTx/2-1);
    //     halfSingularVector = (1/norm(halfSingularVector))*halfSingularVector;  // normalization
    
    vec quantizedAmplitudes(N1*N2), quatizedPhases(N1*N2);
    cvec quantizedCoeffiencts(N1*N2);
    
    selectedSet = selectSet(orthBeamsTotalSets, halfSingularVector);
    mCodebook->computeCoeff(orthBeamsTotalSets(selectedSet), halfSingularVector, Lbeams, quantizedAmplitudes, quatizedPhases, quantizedCoeffiencts);
    
    ivec sortIndx = reverse(sort_index(quantizedAmplitudes)); // descending order
    quantizedAmplitudes = quantizedAmplitudes(sortIndx);
    quatizedPhases = quatizedPhases(sortIndx);
    quantizedCoeffiencts = quantizedCoeffiencts(sortIndx);
    
    ivec selectedColumnsIndicesInLibrary = sortIndx(to_ivec(find(quantizedAmplitudes!=0)));
    
    //     cout<<"Selected set: "<< selectedSet<<endl;
    
    cmat orthogonalBeams = orthBeamsTotalSets(selectedSet).get_cols(selectedColumnsIndicesInLibrary);
    type2Coefficients.i11Indices = i11_IndicesPerSet(selectedSet)(selectedColumnsIndicesInLibrary);
    type2Coefficients.i12Indices = i12_IndicesPerSet(selectedSet)(selectedColumnsIndicesInLibrary);
    
    
    if(selectedColumnsIndicesInLibrary.length()<Lbeams)
    {
        cout <<"Can't find Lbeams, appending zero vectors to make Lbeams("<<Lbeams<<")"<<endl;
        int numOfMissingBeams = Lbeams-selectedColumnsIndicesInLibrary.length();
        cmat zeroMatrix = zeros_c(N1*N2, numOfMissingBeams);
        orthogonalBeams = concat_horizontal(orthogonalBeams, zeroMatrix);
        ivec makeUpVec = zeros_i(numOfMissingBeams)-1;    // Making vector with -1's 
        append(type2Coefficients.i11Indices, makeUpVec); // Indices with -1 should not be used while constructing type 2 precoder
        append(type2Coefficients.i12Indices, makeUpVec); // Indices with -1 should not be used while constructing type 2 precoder
    }
    // Storing orthogonal beams in the returning variable
    type2Coefficients.orthBeams = orthogonalBeams;
    // checkpoint
    //     cout<<endl<<"Type2 OMP: "<< endl;
    //     type2Coefficients.printFirstBeami1value();
    //     cout<< endl;
    // checkpoint
    //     cout << "orthogonalBeams: "<<orthogonalBeams<<endl;
    type2Coefficients.amplitudeOverSubbands.set_length(nSubbands);   //  Dimensions: <subbands<Layers<polarozations<Lbeams>>>
    type2Coefficients.phasesOverSubbands.set_length(nSubbands);  //  Dimensions: <subbands<Layers<polarozations<Lbeams>> 
    type2Coefficients.coefficientsOverSubbands.set_length(nSubbands);  //  Dimensions: <subbands<Layers<polarozations<Lbeams>>
    type2Coefficients.sortedBeamsOverSubbands.set_length(nSubbands); //  Dimensions: <subbands<Layers<polarozations<cmat>>
    cmat avgSubbandChannel = zeros_c(entireRawChannel(0).rows(),entireRawChannel(0).cols());
    cmat avgSubbandChannelCovar = zeros_c(entireRawChannel(0).cols(),entireRawChannel(0).cols());
    
    Array<Type1HalfBeamWithi1> type1halfBeamswithi1 = mCodebook->getType1HalfBeam(); 
    cmat totalbeams=zeros_c(N1*N2,type1halfBeamswithi1.length());
    for(int kk=0;kk<type1halfBeamswithi1.length();kk++)
    {
        totalbeams.set_col(kk,type1halfBeamswithi1(kk).Halfbeam.get_col(0));
    }
    
    for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
    {
        type2Coefficients.sortedBeamsOverSubbands(subbandCnt).set_length(nLayers);
        type2Coefficients.amplitudeOverSubbands(subbandCnt).set_length(nLayers);
        type2Coefficients.phasesOverSubbands(subbandCnt).set_length(nLayers);
        type2Coefficients.coefficientsOverSubbands(subbandCnt).set_length(nLayers);
        
        int start = subbandCnt*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(0)*reCountPerRb;
        int stop = (subbandCnt+1)*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(0)*reCountPerRb-1;
        
        if(subbandCnt==nSubbands-1)
            stop = nTones-1;
        //         Array<cmat> currentChannel=entireRawChannel(start,stop);
        //         vec zeroVec = zeros(Lbeams-selectedColumnsIndicesInLibrary.length());
        //         vec tmpVec(selectedColumnsIndicesInLibrary.length());
        double dummyResidue=0.0;
        cvec tmpCoeff(Lbeams);
        // Average channel over subband
        int numOfTonesinSubband=0;
        for(int toneCnt = start; toneCnt <= stop; toneCnt++)
        {
            avgSubbandChannel += entireRawChannel(toneCnt);
            avgSubbandChannelCovar += entireRawChannel(toneCnt).hermitian_transpose()*entireRawChannel(toneCnt);
            numOfTonesinSubband++;
        }
        avgSubbandChannel = avgSubbandChannel/numOfTonesinSubband;
        avgSubbandChannelCovar = avgSubbandChannelCovar/numOfTonesinSubband;
        
        ivec tmpV = getIntegers(0,N1*N2-1,1);
        cmat R11 = avgSubbandChannelCovar.get_cols(tmpV);
        R11 = R11.get_rows(tmpV);
        tmpV = getIntegers(N1*N2,2*N1*N2-1,1);
        cmat R22 = avgSubbandChannelCovar.get_cols(tmpV);
        R22 = R22.get_rows(tmpV);
        cvec quantity(type1halfBeamswithi1.length()),dummy;
        for(int kk=0;kk<type1halfBeamswithi1.length();kk++)
        {
            dummy = totalbeams.get_col(kk).hermitian_transpose()*(R11+R22)*totalbeams.get_col(kk);
            quantity(kk)=dummy(0);
        }
        vec absQuantity=abs(quantity);
        int b1indx = max_index(absQuantity);
        cmat b1 = totalbeams.get_cols(to_ivec(b1indx));
        quantity = zeros_c(quantity.length());
        for(int kk=0;kk<type1halfBeamswithi1.length();kk++)
        {
            if(kk!=b1indx)
            {
                cmat temp_beam = totalbeams.get_cols(to_ivec(kk));
                cmat Dummy = temp_beam.hermitian_transpose()*(R11+R22)*temp_beam+2*real(temp_beam.hermitian_transpose()*R11*b1);
                quantity(kk)=Dummy(0,0);
            }
            else
                quantity(kk) = dComplex(0,0);
        }
        
        absQuantity=abs(quantity);
        int b2indx = max_index(absQuantity);
        cmat b2 = totalbeams.get_cols(to_ivec(b2indx));
        
        cmat tmpCmat = concat_horizontal(b1,b2);
        cmat W1=concat_horizontal(tmpCmat,zeros_c(N1*N2,Lbeams));
        W1=concat_vertical(W1, concat_horizontal(zeros_c(N1*N2,Lbeams),tmpCmat));
        cmat metricMtx = W1.H()*avgSubbandChannelCovar*W1;
        cmat coefAvik;
        isSuccess = eig(metricMtx, eigenValues,coefAvik);
        isSuccess = eig(avgSubbandChannelCovar, eigenValues, V);
        //         cout<<"W1: "<<W1<<endl;
        //         cout<<"avik coeff: "<< coefAvik<<endl;
        //         cvec pre = W1*coefAvik.get_col(0);
        //         pre= pre/dComplex(norm(pre),0);
        //         cout<<"singular: "<<  V.get_col(0)<<endl;
        //         cout<<"precoder avik: "<<  pre<< endl<< " norm: "<<norm(pre)<<endl;
        //         cout<< "residue: "<< norm(V.get_col(0)-pre)<<endl;
        
        cvec fullSingularVector;
        for(int layerCnt=0;layerCnt<nLayers;layerCnt++)
        {
            type2Coefficients.sortedBeamsOverSubbands(subbandCnt)(layerCnt).set_length(2); // Two polarizations
            type2Coefficients.amplitudeOverSubbands(subbandCnt)(layerCnt).set_length(2); // Two polarizations
            type2Coefficients.phasesOverSubbands(subbandCnt)(layerCnt).set_length(2); // Two polarizations
            type2Coefficients.coefficientsOverSubbands(subbandCnt)(layerCnt).set_length(2); // Two polarizations
            fullSingularVector = V.get_col(layerCnt);
            fullSingularVector = (1/norm(fullSingularVector))*fullSingularVector;
            for(int polznCnt=0; polznCnt<2; polznCnt++)
            {
                type2Coefficients.amplitudeOverSubbands(subbandCnt)(layerCnt)(polznCnt).set_length(Lbeams);
                type2Coefficients.phasesOverSubbands(subbandCnt)(layerCnt)(polznCnt).set_length(Lbeams);
                type2Coefficients.coefficientsOverSubbands(subbandCnt)(layerCnt)(polznCnt).set_length(Lbeams);
                // halfSingularVector = For polarization 0 top half and for polrization 1 bottom half
                halfSingularVector = fullSingularVector(getIntegers(polznCnt*V.rows()/2, V.rows()/2+polznCnt*V.rows()/2-1));
                //                 halfSingularVector = (1/norm(halfSingularVector))*halfSingularVector;
                tmpCoeff = coeffAndResidueUsingOMP(halfSingularVector,orthogonalBeams,dummyResidue, Lbeams);
                mCodebook->quantizeCoefficients(tmpCoeff, type2Coefficients.amplitudeOverSubbands(subbandCnt)(layerCnt)(polznCnt), type2Coefficients.phasesOverSubbands(subbandCnt)(layerCnt)(polznCnt), type2Coefficients.coefficientsOverSubbands(subbandCnt)(layerCnt)(polznCnt));
                // Sorting the beams in descending order of amplitudes,  and amplitudes, phases, and coeffs are also sorted accordingly
                sortIndx = reverse(sort_index(type2Coefficients.amplitudeOverSubbands(subbandCnt)(layerCnt)(polznCnt)));
                type2Coefficients.amplitudeOverSubbands(subbandCnt)(layerCnt)(polznCnt) =  zeros(Lbeams);
                type2Coefficients.phasesOverSubbands(subbandCnt)(layerCnt)(polznCnt) =  zeros(Lbeams);
                type2Coefficients.coefficientsOverSubbands(subbandCnt)(layerCnt)(polznCnt) =  coefAvik.get_col(0)(polznCnt*Lbeams,polznCnt*Lbeams+Lbeams-1);
                type2Coefficients.sortedBeamsOverSubbands(subbandCnt)(layerCnt)(polznCnt) = tmpCmat;
                
                //                 cout<< "coeff: "<< type2Coefficients.coefficientsOverSubbands(subbandCnt)(layerCnt)(polznCnt)<<endl;
            }
            
        }
        
        //         checkpoint
        cmat type2Precoder = mCodebook->getType2SubbandPrecoder_OMP(type2Coefficients(subbandCnt), nLayers, Lbeams);
        //         cout<<"singular: "<<  V.get_col(0)<<endl;
        //                 cout<<"precoder for: "<<  sqrt(nLayers)*type2Precoder.get_col(0)<<endl;abort();
        cvec residueVec = V.get_col(0) - sqrt(nLayers)*type2Precoder.get_col(0);
        double residueType2 = norm(residueVec);
        //        cout<<"subband: "<<subbandCnt<<" Avik  Type2 residue: "<<residueType2<<endl;abort();
        
        //         checkpoint
    }
    return type2Coefficients;
}

void CQIEstimatorNR::computeDLAperidicCSIForTypeII_Avik(DLCQIMeasurement_S& perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, CQIMeasurementConfig_S& cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int nodeID,int eNBID, int subframeNumber, string AperiodicCQILogsFileName)
{
    TransmissionScheme_E txScheme = cqiMeasurementConfig.transmissionScheme;
    checkNRTransmissionScheme(txScheme);
    
    configureCQIEstimatorNR(cqiMeasurementConfig.cqiPorts,frameStructure);
    
    int nLayers    = cqiMeasurementConfig.numberOfLayers;
    if(nLayers > 2)
    {
        cout << "[both:] Type II doesn't support nLayers > 2. Aborting !! "<<endl;abort();
    }
    
    int nCodewords = cqiMeasurementConfig.numberOfCodeWords;
    int nREsPerRB = getNumOfREsPerRB(CSIRSPorts,frameStructure,cqiMeasurementConfig.rsSymPositions,cqiMeasurementConfig.rsScPositions);
    
    int DLRBs = cqiMeasurementConfig.nRBs;
    int rbNumber = cqiMeasurementConfig.startRB;
    int nSubbands = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length();
    
    CodeBookConfig_S codebookConfig = mCodebook->getCodeBookConfig();
    
    int Lbeams = codebookConfig.LBeams;
    int nPSK = codebookConfig.nPSK;
    bool isType2IdealPrecoder = codebookConfig.isType2IdealPrecoder;
    
    //CQI extracting the channel over all the DLRBS on positions corresponding to CSIRS Ports 
    getRawPhysicalChannelAcrossRE(entireRawChannel,resourceMapper,myChannel,rbNumber,true,reCountPerRb,DLRBs,portLocations);
    perToneInterferenceCovariance=getPerToneInterferenceCovariance(rxFrame,frameStructure,resourceMapper,true,myChannel,portToAntennaMapper,refSignal,rbRSPattern, portReferenceNumbers,txScheme,reCountPerRb,rbNumber,DLRBs,CSIRSPorts,inv_dB(myTxSCPowerInDB),RSRP);
    //avgWidebandChannel = average(entireRawChannel);  --Add if needed.
    
    Type2coefficients_OMP_S type2Coefficients = getType2Avik(Lbeams, nLayers, nPSK);
    cmat orthBeams = type2Coefficients.orthBeams;
    
    switch(downlinkCQIInfo->cqiComputationInfo.aperiodicMode)
    {
        case _APERIODIC_MODE_3_2_:
        {
            Array<vec> sinrPerCodewordForWidebandCQItemp(nCodewords);   // <codewords<REs>>
            
            perUserFeedback.nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord.set_size(nSubbands); // <subband<nCodeword>>
            perUserFeedback.nrTypeIICQIInfoAPeriodic.sbPrecoder.set_size(nSubbands);
            perUserFeedback.nrTypeIICQIInfoAPeriodic.subBandType2Coeff_OMP.set_size(nSubbands);
            perUserFeedback.nrTypeIICQIInfoAPeriodic.channelPerSubBand.set_size(nSubbands);
            
            CSI_Info_S csiInfo;
            
            Array <vec> sbEffSINRsPerCW(nSubbands);
            Array<ivec> layerIndicationPerCW;
            int currentRB=0, localRB=0, nRBs=0;
            // To make uniform type 2 precoder argument for 'computeSinrPerCWForCodeBookNR' OMP precoder is ...
            // declared as array<cmat> type2Precoder(1), but actually there is only one cmat.
            Array<cmat> type2Precoder(1);  
            for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
            {
                nRBs = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(subbandCnt);
                if(currentRB>=rbNumber && currentRB<(rbNumber+DLRBs))  // Valid subband for the measurement config
                {
                    int start = localRB*reCountPerRb;
                    int stop = (localRB+nRBs)*reCountPerRb-1;
                    
                    Array<cmat> currentRawChannel = entireRawChannel(start,stop);
                    // Use operator '()' on type2Coefficients to extract subband coeff. for ex: type2Coefficients(subbandCnt) 
                    if(isType2IdealPrecoder)
                    {
                        type2Precoder(0) = type2Coefficients.idealPrecoderperSubband(subbandCnt);
                    }
                    else
                    {
                        type2Precoder(0) = mCodebook->getType2SubbandPrecoder_OMP(type2Coefficients(subbandCnt), nLayers, Lbeams);
                    }
                    // checkpoint
                    //                     amplitude = getAmplitude(type2Precoder(0), currentRawChannel);
                    //                     cout << "Type2 Amplitude for subband: "<<subbandCnt<<"is: "<<amplitude<<endl;
                    // checkpoint
                    
                    // Dimensions: <pmi<codewords<REs>>>>
                    Array< Array<vec> >sinrPerCodewordtemp1 = computeSinrPerCWForCodeBookNR(txScheme, myTxSCPowerInDB,demodulationScheme,portToAntennaMapper,currentRB,nLayers,nCodewords,nRBs,1,type2Precoder);   
                    csiInfo = getBestPMIandCQI(sinrPerCodewordtemp1,nRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
                    sbEffSINRsPerCW(subbandCnt) = csiInfo.effSINRsPerCW;
                    
                    perUserFeedback.nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord(subbandCnt)=csiInfo.CQIindexPerCW;
                    
                    perUserFeedback.nrTypeIICQIInfoAPeriodic.sbPrecoder(subbandCnt) = type2Precoder(csiInfo.PMI);//csiInfo.PMI = 0 always, in case of type2 OMP
                    
                    perUserFeedback.nrTypeIICQIInfoAPeriodic.subBandType2Coeff_OMP(subbandCnt) = type2Coefficients(subbandCnt);
                    
                    perUserFeedback.nrTypeIICQIInfoAPeriodic.channelPerSubBand(subbandCnt) = currentRawChannel;
                    
                    // concatinating subband sinr based on chosen PMI for respective subband
                    concatInArray(sinrPerCodewordForWidebandCQItemp,sinrPerCodewordtemp1(csiInfo.PMI));
                    
                    localRB+=nRBs;
                }
                currentRB+=nRBs;
            }
            
            // Calculation of wide band CQI : calculation is done by choosing best PMI per subband
            csiInfo = computeCQIPerCodewords(sinrPerCodewordForWidebandCQItemp,DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
            perUserFeedback.nrTypeIICQIInfoAPeriodic.wbCqiIndexPerCodeWord = csiInfo.CQIindexPerCW;
            
            perUserFeedback.nrTypeIICQIInfoAPeriodic.isValid=true;
            
            #ifdef CSI_DEBUG
            cout<<"\nAvik: Type II wb CQI : "<<perUserFeedback.nrTypeIICQIInfoAPeriodic.wbCqiIndexPerCodeWord<<" \t effSINRsPerCW : "<<csiInfo.effSINRsPerCW<<endl;
            cout<<"Type 1I sb CQI : "<<perUserFeedback.nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord<<" \t sbEffSINRsPerCW : "<<sbEffSINRsPerCW<<endl<<endl;
            #endif
            
            #ifndef CSI_DEBUG
            cout<<"\tRank: "<<nLayers<<"\tI1indices: "<<type2Coefficients.i11Indices<<"\tI2indices: "<<type2Coefficients.i12Indices<<endl;
            #endif
            
            if(traceValuePerTraceName["EnableUElogs"]=="true") 
            {
                ivec subbandPMI = -1*ones_i(nSubbands);
                printCQIindices(downlinkCQIInfo->cqiComputationInfo.aperiodicMode,nodeID,eNBID,nLayers,perUserFeedback.nrTypeIICQIInfoAPeriodic.wbCqiIndexPerCodeWord,csiInfo.modOrderPerCW(),csiInfo.effSINRsPerCW,subframeNumber,AperiodicCQILogsFileName,-1,perUserFeedback.nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord,subbandPMI,0,"");
            }
            break;
        }
        default :
        {
            cout<<" [both:] not ready for other aperiodic cases"<<endl;abort();
        }
    }
}

Type2coefficients_OMP_S CQIEstimatorNR::getType2OMPCoefficients(const int Lbeams, const int nLayers, const int nPSK)
{
    Type2coefficients_OMP_S type2Coefficients;
    int nTones = entireRawChannel.length();
    int nTx = entireRawChannel(0).cols();
    Array<cmat> rawChannelCopy=entireRawChannel;
    int nSubbands = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length();
    // Average channel covariance
    cmat avgChannelCovar = zeros_c(entireRawChannel(0).cols(), entireRawChannel(0).cols());
    // Computing average of wideband channel (one polarizartion's channel)
    for(int cnt = 0; cnt < nTones; cnt++)
    {
        avgChannelCovar += entireRawChannel(cnt).hermitian_transpose()*entireRawChannel(cnt);
    }
    avgChannelCovar = avgChannelCovar/nTones;
    cmat U,V;  vec S; cvec eigenValues;
    bool isSuccess = eig(avgChannelCovar, eigenValues, V);
    
    cvec firstSingularVector = (1/norm(V.get_col(0)))*V.get_col(0);
    //     cout << "Eigen V: "<< V<<endl;
    int selectedSet = -1;
    CodeBookConfig_S codebookConfig = mCodebook->getCodeBookConfig();
    int N1=codebookConfig.N1, N2=codebookConfig.N2, O1=codebookConfig.O1, O2 = codebookConfig.O2; 
    Array<ivec> i11_IndicesPerSet(O1*O2);  // Stores corresponding i11 index of the beams in set
    Array<ivec> i12_IndicesPerSet(O1*O2);  // Stores corresponding i12 index of the beams in set
    Array<cmat> orthBeamsTotalSets = type2OrthogonalBeamsSet(N1, N2, i11_IndicesPerSet, i12_IndicesPerSet); // Generating orthogonal beam sets
    cvec halfSingularVector = firstSingularVector(0,nTx/2-1);
//     halfSingularVector = (1/norm(halfSingularVector))*halfSingularVector;  // normalization

    vec quantizedAmplitudes(N1*N2), quatizedPhases(N1*N2);
    cvec quantizedCoeffiencts(N1*N2);

    selectedSet = selectSet(orthBeamsTotalSets, halfSingularVector);
    mCodebook->computeCoeff(orthBeamsTotalSets(selectedSet), halfSingularVector, Lbeams, quantizedAmplitudes, quatizedPhases, quantizedCoeffiencts);
    
    ivec sortIndx = reverse(sort_index(quantizedAmplitudes)); // descending order
    quantizedAmplitudes = quantizedAmplitudes(sortIndx);
    quatizedPhases = quatizedPhases(sortIndx);
    quantizedCoeffiencts = quantizedCoeffiencts(sortIndx);
    
    ivec selectedColumnsIndicesInLibrary = sortIndx(to_ivec(find(quantizedAmplitudes!=0)));
    
    cmat orthogonalBeams = orthBeamsTotalSets(selectedSet).get_cols(selectedColumnsIndicesInLibrary);
    type2Coefficients.i11Indices = i11_IndicesPerSet(selectedSet)(selectedColumnsIndicesInLibrary);
    type2Coefficients.i12Indices = i12_IndicesPerSet(selectedSet)(selectedColumnsIndicesInLibrary);
    
    if(selectedColumnsIndicesInLibrary.length()<Lbeams)
    {
        cout <<"Can't find Lbeams, appending zero vectors to make Lbeams"<<endl;
        int numOfMissingBeams = Lbeams-selectedColumnsIndicesInLibrary.length();
        cmat zeroMatrix = zeros_c(N1*N2, numOfMissingBeams);
        orthogonalBeams = concat_horizontal(orthogonalBeams, zeroMatrix);
        ivec makeUpVec = zeros_i(numOfMissingBeams)-1;    // Making vector with -1's 
        append(type2Coefficients.i11Indices, makeUpVec); // Indices with -1 should not be used while constructing type 2 precoder
        append(type2Coefficients.i12Indices, makeUpVec); // Indices with -1 should not be used while constructing type 2 precoder
    }
    // Storing orthogonal beams in the returning variable
    type2Coefficients.orthBeams = orthogonalBeams;
    // checkpoint
//     cout<<endl<<"Type2 OMP: "<< endl;
//     type2Coefficients.printFirstBeami1value();
//     cout<< endl;
    // checkpoint
    //     cout << "orthogonalBeams: "<<orthogonalBeams<<endl;
    type2Coefficients.amplitudeOverSubbands.set_length(nSubbands);   //  Dimensions: <subbands<Layers<polarozations<Lbeams>>>
    type2Coefficients.phasesOverSubbands.set_length(nSubbands);  //  Dimensions: <subbands<Layers<polarozations<Lbeams>> 
    type2Coefficients.coefficientsOverSubbands.set_length(nSubbands);  //  Dimensions: <subbands<Layers<polarozations<Lbeams>>
    type2Coefficients.sortedBeamsOverSubbands.set_length(nSubbands); //  Dimensions: <subbands<Layers<polarozations<cmat>>
    type2Coefficients.idealPrecoderperSubband.set_length(nSubbands);
    cmat avgSubbandChannel = zeros_c(entireRawChannel(0).rows(),entireRawChannel(0).cols());
    cmat avgSubbandChannelCovar = zeros_c(entireRawChannel(0).cols(),entireRawChannel(0).cols());
    
    for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
    {
        type2Coefficients.sortedBeamsOverSubbands(subbandCnt).set_length(nLayers);
        type2Coefficients.amplitudeOverSubbands(subbandCnt).set_length(nLayers);
        type2Coefficients.phasesOverSubbands(subbandCnt).set_length(nLayers);
        type2Coefficients.coefficientsOverSubbands(subbandCnt).set_length(nLayers);
        
        int start = subbandCnt*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(0)*reCountPerRb;
        int stop = (subbandCnt+1)*downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(0)*reCountPerRb-1;
        
        if(subbandCnt==nSubbands-1)
            stop = nTones-1;
//         Array<cmat> currentChannel=entireRawChannel(start,stop);
//         vec zeroVec = zeros(Lbeams-selectedColumnsIndicesInLibrary.length());
//         vec tmpVec(selectedColumnsIndicesInLibrary.length());
        double dummyResidue=0.0;
        cvec tmpCoeff(Lbeams);
        // Average channel over subband
        int numOfTonesinSubband=0;
        for(int toneCnt = start; toneCnt <= stop; toneCnt++)
        {
            avgSubbandChannel += entireRawChannel(toneCnt);
            avgSubbandChannelCovar += entireRawChannel(toneCnt).hermitian_transpose()*entireRawChannel(toneCnt);
            numOfTonesinSubband++;
        }
        avgSubbandChannel = avgSubbandChannel/numOfTonesinSubband;
        avgSubbandChannelCovar = avgSubbandChannelCovar/numOfTonesinSubband;
        isSuccess = eig(avgSubbandChannelCovar, eigenValues, V);
        type2Coefficients.idealPrecoderperSubband(subbandCnt) = (1/sqrt(nLayers))*V.get_cols(0,nLayers-1);
        
        cvec fullSingularVector;
        for(int layerCnt=0;layerCnt<nLayers;layerCnt++)
        {
            type2Coefficients.sortedBeamsOverSubbands(subbandCnt)(layerCnt).set_length(2); // Two polarizations
            type2Coefficients.amplitudeOverSubbands(subbandCnt)(layerCnt).set_length(2); // Two polarizations
            type2Coefficients.phasesOverSubbands(subbandCnt)(layerCnt).set_length(2); // Two polarizations
            type2Coefficients.coefficientsOverSubbands(subbandCnt)(layerCnt).set_length(2); // Two polarizations
            fullSingularVector = V.get_col(layerCnt);
            fullSingularVector = (1/norm(fullSingularVector))*fullSingularVector;
            for(int polznCnt=0; polznCnt<2; polznCnt++)
            {
                type2Coefficients.amplitudeOverSubbands(subbandCnt)(layerCnt)(polznCnt).set_length(Lbeams);
                type2Coefficients.phasesOverSubbands(subbandCnt)(layerCnt)(polznCnt).set_length(Lbeams);
                type2Coefficients.coefficientsOverSubbands(subbandCnt)(layerCnt)(polznCnt).set_length(Lbeams);
                // halfSingularVector = For polarization 0 top half and for polrization 1 bottom half
                halfSingularVector = fullSingularVector(getIntegers(polznCnt*V.rows()/2, V.rows()/2+polznCnt*V.rows()/2-1));
//                 halfSingularVector = (1/norm(halfSingularVector))*halfSingularVector;
                tmpCoeff = coeffAndResidueUsingOMP(halfSingularVector,orthogonalBeams,dummyResidue, Lbeams);
                mCodebook->quantizeCoefficients(tmpCoeff, type2Coefficients.amplitudeOverSubbands(subbandCnt)(layerCnt)(polznCnt), type2Coefficients.phasesOverSubbands(subbandCnt)(layerCnt)(polznCnt), type2Coefficients.coefficientsOverSubbands(subbandCnt)(layerCnt)(polznCnt));
                // Sorting the beams in descending order of amplitudes,  and amplitudes, phases, and coeffs are also sorted accordingly
                sortIndx = reverse(sort_index(type2Coefficients.amplitudeOverSubbands(subbandCnt)(layerCnt)(polznCnt)));
                type2Coefficients.amplitudeOverSubbands(subbandCnt)(layerCnt)(polznCnt) =  type2Coefficients.amplitudeOverSubbands(subbandCnt)(layerCnt)(polznCnt)(sortIndx);
                type2Coefficients.phasesOverSubbands(subbandCnt)(layerCnt)(polznCnt) =  type2Coefficients.phasesOverSubbands(subbandCnt)(layerCnt)(polznCnt)(sortIndx);
                type2Coefficients.coefficientsOverSubbands(subbandCnt)(layerCnt)(polznCnt) =  type2Coefficients.coefficientsOverSubbands(subbandCnt)(layerCnt)(polznCnt)(sortIndx);
                type2Coefficients.sortedBeamsOverSubbands(subbandCnt)(layerCnt)(polznCnt) = orthogonalBeams.get_cols(sortIndx);
            }
        }
        // checkpoint
//         cmat type2Precoder = mCodebook->getType2SubbandPrecoder_OMP(type2Coefficients(subbandCnt), nLayers, Lbeams);
//         cvec residueVec = V.get_col(0) - type2Precoder.get_col(0);
//         double residueType2 = norm(residueVec);
//         double amplitude = getAmplitude(type2Precoder, currentChannel);
//         cout<<"subband: "<<subbandCnt<<"   Type2 residue: "<<residueType2<<"   Amplitude for "<<amplitude<<endl;
        // checkpoint
        //         checkpoint
        cmat type2Precoder = mCodebook->getType2SubbandPrecoder_OMP(type2Coefficients(subbandCnt), nLayers, Lbeams);
        //         cout<<"singular: "<<  V.get_col(0)<<endl;
        //         cout<<"precoder: "<<  sqrt(2)*type2Precoder.get_col(0)<<endl;
        cvec residueVec = V.get_col(0) - sqrt(nLayers)*type2Precoder.get_col(0);
        double residueType2 = norm(residueVec);
        //        cout<<"subband: "<<subbandCnt<<"   Type2 residue: "<<residueType2<<endl;abort();
        
        //         checkpoint
    }
    return type2Coefficients;
}

void CQIEstimatorNR::computeDLAperiodicCSIForTypeII_OMP(DLCQIMeasurement_S& perUserFeedback, FrameStructure& frameStructure, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper, ReferenceSignal_S* refSignal, ResourceMapper* resourceMapper, Array< cmat >& rxFrame, Array< Array< cmat > >& myChannel, double myTxSCPowerInDB, CQIMeasurementConfig_S& cqiMeasurementConfig, double noiseVariance, double restOfInterferenceInDB, int nodeID,int eNBID, int subframeNumber, string AperiodicCQILogsFileName)
{
  TransmissionScheme_E txScheme = cqiMeasurementConfig.transmissionScheme;
  checkNRTransmissionScheme(txScheme);
  
  configureCQIEstimatorNR(cqiMeasurementConfig.cqiPorts,frameStructure);
  
  int nLayers    = cqiMeasurementConfig.numberOfLayers;
  if(nLayers > 2)
  {
    cout << "[both:] Type II doesn't support nLayers > 2. Aborting !! "<<endl;abort();
  }
  
  int nCodewords = cqiMeasurementConfig.numberOfCodeWords;
  int nREsPerRB = getNumOfREsPerRB(CSIRSPorts,frameStructure,cqiMeasurementConfig.rsSymPositions,cqiMeasurementConfig.rsScPositions);
  
  int DLRBs = cqiMeasurementConfig.nRBs;
  int rbNumber = cqiMeasurementConfig.startRB;
  int nSubbands = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length();
  
  CodeBookConfig_S codebookConfig = mCodebook->getCodeBookConfig();
  int Lbeams = codebookConfig.LBeams;
  int nPSK = codebookConfig.nPSK;
  bool isType2IdealPrecoder = codebookConfig.isType2IdealPrecoder;
  
  //CQI extracting the channel over all the DLRBS on positions corresponding to CSIRS Ports
  if(CQIInfo.subframeNumber!=subframeNumber)
  {
    getRawPhysicalChannelAcrossRE(entireRawChannel,resourceMapper,myChannel,rbNumber,true,reCountPerRb,DLRBs,portLocations);
    perToneInterferenceCovariance=getPerToneInterferenceCovariance(rxFrame,frameStructure,resourceMapper,true,myChannel,portToAntennaMapper,refSignal,rbRSPattern, portReferenceNumbers,txScheme,reCountPerRb,rbNumber,DLRBs,CSIRSPorts,inv_dB(myTxSCPowerInDB),RSRP);
    //avgWidebandChannel = average(entireRawChannel);  --Add if needed.
  }
  Type2coefficients_OMP_S type2Coefficients = getType2OMPCoefficients(Lbeams, nLayers, nPSK);
  cmat orthBeams = type2Coefficients.orthBeams;

  switch(downlinkCQIInfo->cqiComputationInfo.aperiodicMode)
  {
    case _APERIODIC_MODE_3_2_:
    {
      Array<vec> sinrPerCodewordForwidebandCQIperCWtemp(nCodewords);   // <codewords<REs>>
      
      perUserFeedback.nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord.set_size(nSubbands); // <subband<nCodeword>>
      perUserFeedback.nrTypeIICQIInfoAPeriodic.sbPrecoder.set_size(nSubbands);
      perUserFeedback.nrTypeIICQIInfoAPeriodic.subBandType2Coeff_OMP.set_size(nSubbands);
      perUserFeedback.nrTypeIICQIInfoAPeriodic.channelPerSubBand.set_size(nSubbands);
      
      CSI_Info_S csiInfo;
      ivec subbandPMIIndex(nSubbands);
      
      Array <vec> sbeffSINRsPerCW(nSubbands);
      Array<ivec> layerIndicationPerCW;
      int currentRB=0, localRB=0, nRBs=0;
      // To make uniform type 2 precoder argument for 'computeSinrPerCWForCodeBookNR' OMP precoder is ...
      // declared as array<cmat> type2Precoder(1), but actually there is only one cmat.
      Array<cmat> type2Precoder(1);  
      for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
      {
        nRBs = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(subbandCnt);
        if(currentRB>=rbNumber && currentRB<(rbNumber+DLRBs))  // Valid subband for the measurement config
        {
          int start = localRB*reCountPerRb;
          int stop = (localRB+nRBs)*reCountPerRb-1;
          
          Array<cmat> currentRawChannel = entireRawChannel(start,stop);
          // Use operator '()' on type2Coefficients to extract subband coeff. for ex: type2Coefficients(subbandCnt) 
          if(isType2IdealPrecoder)
          {
              type2Precoder(0) = type2Coefficients.idealPrecoderperSubband(subbandCnt);
          }
          else
          {
            type2Precoder(0) = mCodebook->getType2SubbandPrecoder_OMP(type2Coefficients(subbandCnt), nLayers, Lbeams);
          }
          // checkpoint
          //                     amplitude = getAmplitude(type2Precoder(0), currentRawChannel);
          //                     cout << "Type2 Amplitude for subband: "<<subbandCnt<<"is: "<<amplitude<<endl;
          // checkpoint
          
          // Dimensions: <pmi<codewords<REs>>>>
          Array< Array<vec> >sinrPerCodewordtemp1 = computeSinrPerCWForCodeBookNR(txScheme, myTxSCPowerInDB,demodulationScheme,portToAntennaMapper,currentRB,nLayers,nCodewords,nRBs,1,type2Precoder);   
          csiInfo = getBestPMIandCQI(sinrPerCodewordtemp1,nRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
          sbeffSINRsPerCW(subbandCnt) = csiInfo.effSINRsPerCW;
          
          subbandPMIIndex(subbandCnt) = csiInfo.PMI;
          
          perUserFeedback.nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord(subbandCnt)=csiInfo.CQIindexPerCW;
          
          perUserFeedback.nrTypeIICQIInfoAPeriodic.sbPrecoder(subbandCnt) = type2Precoder(csiInfo.PMI);//csiInfo.PMI = 0 always, in case of type2 OMP
          
          perUserFeedback.nrTypeIICQIInfoAPeriodic.subBandType2Coeff_OMP(subbandCnt) = type2Coefficients(subbandCnt);
          
          perUserFeedback.nrTypeIICQIInfoAPeriodic.channelPerSubBand(subbandCnt) = currentRawChannel;
          
          // concatinating subband sinr based on chosen PMI for respective subband
          concatInArray(sinrPerCodewordForwidebandCQIperCWtemp,sinrPerCodewordtemp1(csiInfo.PMI));
          
          localRB+=nRBs;
        }
        currentRB+=nRBs;
      }
      
      // Calculation of wide band CQI : calculation is done by choosing best PMI per subband
      csiInfo = computeCQIPerCodewords(sinrPerCodewordForwidebandCQIperCWtemp,DLRBs,nREsPerRB,txScheme,nLayers,downlinkCQIInfo->cqiComputationInfo.enableCQI0);
      perUserFeedback.nrTypeIICQIInfoAPeriodic.wbCqiIndexPerCodeWord = csiInfo.CQIindexPerCW;
      
      perUserFeedback.nrTypeIICQIInfoAPeriodic.isValid=true;
      // cout<<"Type II wb CQI : "<<perUserFeedback.nrTypeIICQIInfoAPeriodic.wbCqiIndexPerCodeWord<<" \t effSINRsPerCW : "<<csiInfo.effSINRsPerCW<<endl;
      // cout<<"Type 1I sb CQI : "<<perUserFeedback.nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord<<" \t sbeffSINRsPerCW : "<<sbeffSINRsPerCW<<endl;
      
      #ifdef CSI_DEBUG
      cout<<"\nSubband Precoder Index: "<<subbandPMIIndex;
      cout<<"\tType II wb CQI : "<<perUserFeedback.nrTypeIICQIInfoAPeriodic.wbCqiIndexPerCodeWord<<" \t effSINRsPerCW : "<<csiInfo.effSINRsPerCW;
      cout<<"\tType II sb CQI : "<<perUserFeedback.nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord<<" \t sbeffSINRsPerCW : "<<sbeffSINRsPerCW<<endl;
      #endif
      
      #ifndef CSI_DEBUG
      cout<<"\tRank: "<<nLayers<<"\tI1indices: "<<type2Coefficients.i11Indices<<"\tI2indices: "<<type2Coefficients.i12Indices<<endl;
      #endif
      
      if(traceValuePerTraceName["EnableUElogs"]=="true") 
      {
        ivec subbandPMI = -1*ones_i(nSubbands);
        printCQIindices(downlinkCQIInfo->cqiComputationInfo.aperiodicMode,nodeID,eNBID,nLayers,perUserFeedback.nrTypeIICQIInfoAPeriodic.wbCqiIndexPerCodeWord,csiInfo.modOrderPerCW(),csiInfo.effSINRsPerCW,subframeNumber,AperiodicCQILogsFileName,-1,perUserFeedback.nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord,subbandPMI,0,"");
      }
      break;
    }
    default :
    {
      cout<<" [both:] not ready for other aperiodic cases"<<endl;abort();
    }
  }
}

Array<vec> CQIEstimatorNR::calculateSinrPerCodewordNR(int nLayers,int nCodeWords, TransmissionScheme_E txScheme, DemodulationScheme_E demodulationScheme, Array<cmat>& myPrecodedChannel, double sigpowInWatts, cmat estimatedIntrCovariance)
{
    int nTones = myPrecodedChannel.length();
    
    Array<vec> sinr(nLayers); // <layer<re>>
    for(int iLayer=0;iLayer<nLayers;iLayer++)
        sinr(iLayer).set_length(nTones);
    
    vec sinrtemp(nLayers);
    
    for(int tone_cnt=0;tone_cnt <nTones;tone_cnt++)
    {
        cmat chan = sqrt(sigpowInWatts)*myPrecodedChannel(tone_cnt);
        sinrtemp = getSINRperLayer(demodulationScheme,chan,estimatedIntrCovariance,nLayers,sigpowInWatts);
        for(int iLayer=0;iLayer<nLayers;iLayer++)
            sinr(iLayer)[tone_cnt] = sinrtemp(iLayer);
    }
    
    Array<vec> sinrPerCodeword = performLayerDeMapping(sinr, nCodeWords, nLayers);
    
    return(sinrPerCodeword);
}

Array < Array< vec > > CQIEstimatorNR::computeSinrPerCWForCodeBookNR(TransmissionScheme_E txScheme,double myTxSCPowerInDB, DemodulationScheme_E demodulationScheme, PortToAntennaMapper_S* portToAntennaMapper,int rbNumber, int nLayers, int nCodewords,int NumOfRbs,bool isDownlink, Array<cmat>& tCodeBook)
{

  Array < Array<vec > > sinrPerCodeword;
  
  int codebookLength=tCodeBook.length();
  
  sinrPerCodeword.set_length(codebookLength);
  
  int start = rbNumber*reCountPerRb;
  int stop = (rbNumber+NumOfRbs)*reCountPerRb-1;
    
  Array<cmat> currentChannel = entireRawChannel(start,stop);
  cmat IntrCov = mean(perToneInterferenceCovariance(start,stop));
  
  //loop over all the codebooks to compute the post processing SINR
  for(int precIndex=0; precIndex<codebookLength; precIndex++)
  {
    Array<cmat> signalChannel = precodeChannelForCQI(currentChannel,portToAntennaMapper,txScheme,nCSIRSPorts,tCodeBook(precIndex));
    sinrPerCodeword(precIndex) = calculateSinrPerCodewordNR(nLayers,nCodewords,txScheme,demodulationScheme,signalChannel, inv_dB(myTxSCPowerInDB),IntrCov);
  }
  
  return(sinrPerCodeword);
}

double findSpectralEfficiency(DemodulationScheme_E demodulationScheme, cmat channel, cmat IplusNoise, double sigpowInWatts, int nLayers){
  
  vec SINR = getSINRperLayer(demodulationScheme,channel,IplusNoise,nLayers,sigpowInWatts); //abs(H.hermitian_transpose()*inv(IplusNoise)*H)(0);
  double SE = 0.0;
  
  for(int iLayer = 0; iLayer < nLayers; iLayer++)
    SE += log2(1.0 + SINR(iLayer));
  
  return SE;
}

double findSpectralEfficiency(DemodulationScheme_E demodulationScheme, Array<cmat> channel, Array<cmat> IplusNoise, double sigpowInWatts, int nLayers, cmat precoder){
  
  double SE = 0.0;
  
  int nTones = channel.length();
  for(int tone_cnt = 0; tone_cnt < nTones; tone_cnt++)
  {
    cmat HP = channel(tone_cnt)*precoder;
    SE += findSpectralEfficiency(demodulationScheme,HP,IplusNoise(tone_cnt),sigpowInWatts,nLayers);
  }
  
  SE = SE/nTones;
  
  return SE;
  
}

Type2coefficients CQIEstimatorNR::getBestAmplitudeAndPhase(Array< Type1HalfBeamWithi1 >& type1halfBeamswithi1, int &Lbeams, int nPSK, DemodulationScheme_E demodulationScheme, double sigpowInWatts, int nLayers, int rbNumber, int DLRBs){
  
  if(nLayers>Lbeams)
  {
    cout<<"Error.. Lbeams expected to be greater than nLayers. "<<endl;abort();
  }
  
  vec amplitudeWideBand;
  Type2coefficients type2Co_eff;
  Array<Array<vec>> finalAmplitude;
  Array<Array<ivec>> finalSubBandPhasePerOrthBeam;   // Polarization Beam Subband
  vec tempAmp;
  
  int nTones = entireRawChannel.length();
  int nTx = entireRawChannel(0).cols();
  Array<cmat> rawChannelCopy=entireRawChannel;
  Array<cmat> halfChannel1(nTones),halfChannel2(nTones);
  finalSubBandPhasePerOrthBeam.set_length(2);
  
  int nSubbands = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length();
  
  for(int cnt = 0; cnt < nTones; cnt++)
  {
    halfChannel1(cnt) = entireRawChannel(cnt).get_cols(0,(nTx/2)-1);
    halfChannel2(cnt) = entireRawChannel(cnt).get_cols((nTx/2),nTx-1); // other half of the channel
  }
  Array<cmat> halfChannel1Copy = halfChannel1;
  Array<cmat> halfChannel2Copy = halfChannel2;
  ivec orthTopLBeamIndices;
  orthTopLBeamIndices.set_size(0);
  
  finalAmplitude.set_size(nSubbands);
  
  amplitudeWideBand = getAmplitude(type1halfBeamswithi1,halfChannel1);
  orthTopLBeamIndices = getBestBeams(type1halfBeamswithi1,amplitudeWideBand,Lbeams); 
  
  
  //     for(int i=0;i<Lbeams;i++)
  //     {
  //         amplitudeWideBand = getAmplitude(type1halfBeamswithi1,halfChannel1);
  //         cout<<"amplitudeWideBand : "<<amplitudeWideBand<<endl;
  //         orthTopLBeamIndices = getBestBeam(type1halfBeamswithi1,amplitudeWideBand, orthTopLBeamIndices);  // only one value
  //         
  //         if(orthTopLBeamIndices.length()<i+1)
  //         {
  //             Lbeams = orthTopLBeamIndices.length();
  //         }
  //         else
  //         {
  // //             for(int cnt = 0; cnt < nTones; cnt++)
  // //             {
  // //                 halfChannel1(cnt) = halfChannel1(cnt) - halfChannel1(cnt) * type1halfBeamswithi1(orthTopLBeamIndices(i)).Halfbeam * type1halfBeamswithi1(orthTopLBeamIndices(i)).Halfbeam.hermitian_transpose();
  // //             }
  //         }
  //     }
  
  //   amplitudeWideBand = getAmplitude(type1halfBeamswithi1,beamIndices,halfChannel1);
  //   orthTopLBeamIndices = getLbestPMIIndices(type1halfBeamswithi1,amplitudeWideBand, Lbeams);  // only one value
  
  
  // Finding subband phase for each orthognal beam for one polarization
  
  for(int pol_cnt=0; pol_cnt<2; pol_cnt++)
    finalSubBandPhasePerOrthBeam(pol_cnt).set_length(Lbeams);
  
  for(int i=0;i<nSubbands;i++)
  {
    finalAmplitude(i).set_length(nLayers);
    for(int j=0;j<nLayers;j++)
      finalAmplitude(i)(j)=zeros(2*Lbeams);
  }
  ivec layers=getIntegers(0,nLayers-1);
  ivec layerPerBeam(0);
  for(int cnt=0; cnt<Lbeams/nLayers; cnt++)
    append(layerPerBeam,layers);
  
  for(int beam_cnt=0; beam_cnt<Lbeams; beam_cnt++)
  {
    for(int pol_cnt=0; pol_cnt<2; pol_cnt++)
      finalSubBandPhasePerOrthBeam(pol_cnt)(beam_cnt).set_size(nSubbands);
    
    int localRB=0, currentRB=0;
    for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
    {
      int nRBs = downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic(subbandCnt);
      if(currentRB>=rbNumber && currentRB<(rbNumber+DLRBs))  // Valid subband for the measurement config
      {
        int start = localRB*reCountPerRb;
        int stop = (localRB+nRBs)*reCountPerRb-1;
        
        Array<cmat> currentRawChannel = rawChannelCopy(start,stop);
        Array<cmat> currentIntrCov = perToneInterferenceCovariance(start,stop);
        
        vec SE=zeros(nPSK);
        cmat P1,P2,P;
        
        finalAmplitude(subbandCnt)(layerPerBeam(beam_cnt))(beam_cnt) =  getAmplitude(type1halfBeamswithi1(orthTopLBeamIndices(beam_cnt)).Halfbeam,halfChannel1Copy(start,stop));
        finalAmplitude(subbandCnt)(layerPerBeam(beam_cnt))(beam_cnt+Lbeams) = getAmplitude(type1halfBeamswithi1(orthTopLBeamIndices(beam_cnt)).Halfbeam,halfChannel2Copy(start,stop));
        
        for(int psk_cnt=0;psk_cnt<nPSK;psk_cnt++)
        {
          P1 = type1halfBeamswithi1(orthTopLBeamIndices(beam_cnt)).Halfbeam;
          if(nLayers==1)
          {  
            P2=ones_c(2,1); P2(1,0)=Phi(psk_cnt,nPSK);
          }
          else 
          {  
            P2=ones_c(2,2); 
            P2(0,0)*=finalAmplitude(subbandCnt)(0)(beam_cnt);
            P2(0,1)*=finalAmplitude(subbandCnt)(1)(beam_cnt);
            P2(1,0)=Phi(psk_cnt,nPSK)*finalAmplitude(subbandCnt)(0)(beam_cnt); 
            P2(1,1)=(-Phi(psk_cnt,nPSK))*finalAmplitude(subbandCnt)(1)(beam_cnt); 
          }
          
          P = kron(P2,P1);
          P = (1/sqrt(abs(trace(P.hermitian_transpose()*P)))) * P;
          
          SE(psk_cnt) = findSpectralEfficiency(demodulationScheme,currentRawChannel,currentIntrCov,sigpowInWatts,nLayers,P);
        }
        
        finalSubBandPhasePerOrthBeam(0)(beam_cnt)(subbandCnt)=0;
        finalSubBandPhasePerOrthBeam(1)(beam_cnt)(subbandCnt)=max_index(SE);
        
        localRB+=nRBs;
      }
      currentRB+=nRBs;
    }
  }
  
  for(int subbandCnt=0;subbandCnt<nSubbands;subbandCnt++)
    for(int j=0;j<nLayers;j++)
    {
      vec tempVec = finalAmplitude(subbandCnt)(j)(0,Lbeams-1);
      tempVec = tempVec/max(tempVec);
      setInVec(finalAmplitude(subbandCnt)(j),getIntegers(0,Lbeams-1),tempVec);
      tempVec = finalAmplitude(subbandCnt)(j)(Lbeams,2*Lbeams-1);
      tempVec = tempVec/max(tempVec);
      setInVec(finalAmplitude(subbandCnt)(j),getIntegers(Lbeams,2*Lbeams-1),tempVec);
    }
    
    type2Co_eff.finalAmplitude = finalAmplitude;
    type2Co_eff.finalSubBandPhasePerOrthBeamPerPolarization=finalSubBandPhasePerOrthBeam;
    type2Co_eff.orthBeamIndices=orthTopLBeamIndices;
    return type2Co_eff;
}

vec getAmplitude(Array<Type1HalfBeamWithi1>& type1halfBeamswithi1, Array<cmat> halfChannel)
{
  int nHalfBeams = type1halfBeamswithi1.length();
  vec amplitude = zeros(nHalfBeams);
  
  
  // Finding Amplitude for each Half beam for one polarization will be stored in amplitude. This is used in selecting L beams
  for(int beamIndx = 0; beamIndx < nHalfBeams; beamIndx ++)
  {
    amplitude(beamIndx) = getAmplitude(type1halfBeamswithi1(beamIndx).Halfbeam,halfChannel);
  }
  
  return amplitude;
}

ivec getLayerIndicator(Array<vec> sinrPerLayer){
  
  ivec layerIndication;
  vec avgSinr(sinrPerLayer.length());
  
  for(int iLayer = 0; iLayer < sinrPerLayer.length(); iLayer++)
    avgSinr(iLayer) = mean(sinrPerLayer(iLayer));
  
  layerIndication = reverse(sort_index(avgSinr));
  
  return layerIndication;
  
}

Array<ivec> getLayerIndication(Array<vec> &ppSINR, int nCodewords, int nLayers,bool isPerCodeword)
{
  Array<ivec> layerIndication(nCodewords);
  Array<vec> ppSINRperLayer = ppSINR;
  if(isPerCodeword)
  {  
    ppSINRperLayer = performLayerMapping(ppSINR,nCodewords,nLayers); //convert SinrPerCW to SinrPerLayer
  }
  ivec indices;
  switch(nLayers)
  {
    case 1:
    case 2:
    case 3:
    case 4:
        layerIndication(0) = getLayerIndicator(ppSINRperLayer);
      break;

    case 5:
      indices = ivec("0 1");
      layerIndication(0) = getLayerIndicator(getFromArray(ppSINRperLayer,indices));
      indices = ivec("2 3 4");
      layerIndication(1) = getLayerIndicator(getFromArray(ppSINRperLayer,indices));
      break;
    
    case 6:
      indices = ivec("0 1 2");
      layerIndication(0) = getLayerIndicator(getFromArray(ppSINRperLayer,indices));
      indices = ivec("3 4 5");
      layerIndication(1) = getLayerIndicator(getFromArray(ppSINRperLayer,indices));
      break;
      
    case 7:
      indices = ivec("0 1 2");
      layerIndication(0) = getLayerIndicator(getFromArray(ppSINRperLayer,indices));
      indices = ivec("3 4 5 6");
      layerIndication(1) = getLayerIndicator(getFromArray(ppSINRperLayer,indices));
      break;
      
    case 8:
      indices = ivec("0 1 2 3");
      layerIndication(0) = getLayerIndicator(getFromArray(ppSINRperLayer,indices));
      indices = ivec("4 5 6 7");
      layerIndication(1) = getLayerIndicator(getFromArray(ppSINRperLayer,indices));
      break;
      
    default:
      cout<<"[both:] Invalid number of layers."<<endl;abort();
  }
  
  return layerIndication;
}

Array< Array< ivec > > getLayerIndicationPerCWPerPrecoder(Array<Array<vec>>& sinrPerCWPerPrecoder,int nCodewords,int nLayers)
{
    int codebookLength = sinrPerCWPerPrecoder.length();
    Array< Array< ivec > > layerIndicationPerCWPerPrecoder(codebookLength);
    for(int precIndex=0;precIndex<codebookLength;precIndex++)
    {
        layerIndicationPerCWPerPrecoder(precIndex) = getLayerIndication(sinrPerCWPerPrecoder(precIndex),nCodewords,nLayers,true);
    }
    return layerIndicationPerCWPerPrecoder;
}

ivec getLbestPMIIndices(Array< Type1HalfBeamWithi1 > type1halfBeamswithi1, vec amplitudesOverBeams, int L)
{
  std::vector<int> orthLpmi;
  orthLpmi.reserve(amplitudesOverBeams.length());
//   cout <<"length: "<<amplitudesOverPMIs.length()<<endl;
  std::vector<int> Lpmi;
  Lpmi.reserve(amplitudesOverBeams.length());
  int exhaustCnt=0;
  if(type1halfBeamswithi1.length() != amplitudesOverBeams.length())
  {
    cout << "# of Beams and # of amps mismatch.. aborting.." << endl;abort();
  }
  int cnt=0;
  for(int i=0;i<type1halfBeamswithi1.length();i++)
  {
    if(type1halfBeamswithi1(i).i12==0)
      cnt++;
  }
  bool isN2zero = (cnt == type1halfBeamswithi1.length())? true:false;
  bool check;
  while(orthLpmi.size()<=L && exhaustCnt<amplitudesOverBeams.length())
  {
    Lpmi = ITPPVectorstdVector(find(amplitudesOverBeams,max(amplitudesOverBeams),true));
    bool isOrthogonal = true;
    int indx=0;
    if(orthLpmi.size()==0)
    {
      orthLpmi.push_back(Lpmi[0]);
      amplitudesOverBeams(Lpmi[0])=-100;
      indx =1;
    }
    for(int h=indx;h<Lpmi.size();h++)
    {
      for(int g=0;g<orthLpmi.size();g++)
      {
    // check should be always false when N2 =0;
	if(isN2zero)
	  check = false;
	else
	  check =  (abs(type1halfBeamswithi1(orthLpmi[g]).i12-type1halfBeamswithi1(Lpmi[h]).i12)<4);

	if(abs(type1halfBeamswithi1(orthLpmi[g]).i11-type1halfBeamswithi1(Lpmi[h]).i11)<4 || check)
	{
	  isOrthogonal = false;
	}
      }
      if(isOrthogonal)
	orthLpmi.push_back(Lpmi[h]);
      
      amplitudesOverBeams(Lpmi[h])=-100;
    }
    if(orthLpmi.size()>=L)
    {
      orthLpmi.resize(L);
      break;
    }
    exhaustCnt++;
  }
  return stdVectortoITPPVec(orthLpmi);
}

ivec getBestBeams(Array< Type1HalfBeamWithi1 > type1halfBeamswithi1, vec amplitudesOverBeams, int &L)
{
    int firstBeamIndx=max_index(amplitudesOverBeams);
    ivec orthogonalBeams=to_ivec(firstBeamIndx);
    
    for(int b=0;b<type1halfBeamswithi1.length();b++)
        if(abs((type1halfBeamswithi1(firstBeamIndx).Halfbeam.H()*type1halfBeamswithi1(b).Halfbeam)(0))<1e-3)
           append(orthogonalBeams,b);
    
    vec amplitudeofOrthoBeams=amplitudesOverBeams(orthogonalBeams);
    ivec sortIndx=reverse(sort_index(amplitudeofOrthoBeams));
    if(orthogonalBeams.length()>L)
        return(orthogonalBeams(sortIndx.left(L)));
    else
    {
        //Changing L if unable to find L orthogonal Beams
        L=orthogonalBeams.length();
        return(orthogonalBeams(sortIndx));
    }
        
}

// first computes i11,i12 then using that halfbeam it computes i14

type1PMI_S CQIEstimatorNR::getBestI1(){
    
    type1PMI_S i1Index = getBestI1forSinglePanel();
    CodeBookConfig_S codebookConfig = mCodebook->getCodeBookConfig();
    int Ng = codebookConfig.Ng;
    if(Ng==1){return i1Index;}
    
    Array<Type1HalfBeamWithi1> type1halfBeamswithi1 = mCodebook->getType1HalfBeam();
    
    int N2 = codebookConfig.N2,O2 = codebookConfig.O2;
    cmat tVlm = mCodebook->getType1HalfBeam()(i1Index.i11*N2*O2 + i1Index.i12).Halfbeam;
    cmat phi_n(2,1);phi_n.set_col(0,concat(dComplex(1,0),Phi(to_ivec(i1Index.i2))));
    cmat W = kron(phi_n,tVlm);
    
    ivec i14Sizes = 4*ones_i(Ng-1);
    vec amp = zeros(prod(i14Sizes)); 
    for(int i14=0;i14<prod(i14Sizes);i14++)
    {
        ivec p = encode(i14Sizes,i14);
        cmat phi_p = concat(dComplex(1,0),Phi(p));
        cmat P = kron(phi_p,W);
        amp(i14) = getAmplitude(P,entireRawChannel);
    }
    i1Index.i14 =  max_index(amp);
    return i1Index;
}

type1PMI_S CQIEstimatorNR::getBestI1forSinglePanel(){
  
    type1PMI_S i1Index;
    
    Array<Type1HalfBeamWithi1> type1halfBeamswithi1 = mCodebook->getType1HalfBeam();
    int Ng = mCodebook->getCodeBookConfig().Ng;
    
    int nTones = entireRawChannel.length();
    int nTx = entireRawChannel(0).cols();
    Array<cmat> halfChannel(nTones);
    
    for(int cnt = 0; cnt < nTones; cnt++)
    {
        halfChannel(cnt) = entireRawChannel(cnt).get_cols(0,(nTx/(2*Ng))-1);
    }
    
    vec amplitudeWideBand = getAmplitude(type1halfBeamswithi1,halfChannel);
    
    int bestBeamIndx = max_index(amplitudeWideBand);
    
    for(int cnt = 0; cnt < nTones; cnt++)
    {
        halfChannel(cnt) = entireRawChannel(cnt).get_cols(0,(nTx/Ng)-1);
    }
    
    cmat tVlm = type1halfBeamswithi1(bestBeamIndx).Halfbeam;
    vec Amp(4);
    for(int n=0;n<4;n++)
    {
        cmat phi_n(2,1);phi_n.set_col(0,concat(dComplex(1,0),Phi(to_ivec(n))));
        Amp(n) = getAmplitude(kron(phi_n,tVlm),halfChannel);
    }
    
    i1Index.i11 = type1halfBeamswithi1(bestBeamIndx).i11;
    i1Index.i12 = type1halfBeamswithi1(bestBeamIndx).i12;
    i1Index.i13 = 0;
    i1Index.i2 = max_index(Amp);
  return i1Index;
}

type1PMI_S CQIEstimatorNR::getBestI1forMultiPanel()
{
    type1PMI_S i1Index;
    
    Array<Type1HalfBeamWithi1> type1halfBeamswithi1 = mCodebook->getType1HalfBeam();
    int nTones = entireRawChannel.length();
    int nTx = entireRawChannel(0).cols();
    int bestBeamIndx;
    int Ng = mCodebook->getCodeBookConfig().Ng;
    int N1N2 = nTx/(2*Ng);
    int nHalfBeams = type1halfBeamswithi1.length();
    
    vec amplitudeWideBand = zeros(nHalfBeams);
    
    Array<cmat> halfChannel(nTones);
    
    for(int i=0;i<Ng;i++)
    {
        int index = i*2*N1N2;
        for(int cnt = 0; cnt < nTones; cnt++)
        {
            halfChannel(cnt) = concat_horizontal(halfChannel(cnt),entireRawChannel(cnt).get_cols(index,index+N1N2-1));
        }
    }
    
    Array<cmat> Halfbeams(nHalfBeams);
    
    for(int i=0;i<nHalfBeams;i++)  
    {
        cmat A = ones_c(Ng,1);
        Halfbeams(i) = kron(A,type1halfBeamswithi1(i).Halfbeam);
    }
    
    amplitudeWideBand = getAmplitude(Halfbeams,halfChannel);
    bestBeamIndx = max_index(amplitudeWideBand);
    ivec i14Sizes = 4*ones_i(Ng-1);
    cmat tVlm = type1halfBeamswithi1(bestBeamIndx).Halfbeam;
    vec amp = zeros(prod(i14Sizes)); 
    
    for(int i14=0;i14<prod(i14Sizes);i14++)
    {
        ivec p = encode(i14Sizes,i14);
        cmat phi_p = concat(dComplex(1,0),Phi(p));
        cmat P = kron(phi_p,tVlm);
        amp(i14) = getAmplitude(P,halfChannel);
    }
    
    cmat Phi_p = concat(dComplex(1,0),Phi(encode(i14Sizes,max_index(amp))));
    
    vec Amp(4);
    for(int n=0;n<4;n++)
    {
        cmat phi_n(2,1);phi_n.set_col(0,concat(dComplex(1,0),Phi(to_ivec(n))));
        Amp(n) = getAmplitude(kron(Phi_p,kron(phi_n,tVlm)),entireRawChannel);
    }
    
    i1Index.i11 = type1halfBeamswithi1(bestBeamIndx).i11;
    i1Index.i12 = type1halfBeamswithi1(bestBeamIndx).i12;
    i1Index.i13 = 0;
    i1Index.i14 = max_index(amp);
    i1Index.i2  = max_index(Amp);
    
    return i1Index;
}

