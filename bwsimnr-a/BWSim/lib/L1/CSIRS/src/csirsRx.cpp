/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2014 CEWiT, India
 *
 * \ NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */

#include "../include/csirsRx.h"



Array<cmat> retrieveCSIRSFromResourceGrid(int nRBsAllocated,Array<cmat>& rxGrid, Array<ivec> allocatedRBs, imat rbRSPattern)
{
    int nRxAntenna = rxGrid.length();
    
    int nSymbolsPerSlot = 14,nSubCarriersPerRB = 12;
    int fftSize  = rxGrid(0).rows();
    int gSc = (fftSize-nRBsAllocated*nSubCarriersPerRB)/2;
    
    Array<cmat> rxRSPerRB(nRBsAllocated);
    for(int sym_cnt=0;sym_cnt<nSymbolsPerSlot;sym_cnt++)
        for(int rb_cnt=0;rb_cnt<nRBsAllocated;rb_cnt++)
        {
            ivec scIndices = allocatedRBs(0)(rb_cnt)*nSubCarriersPerRB + getIntegers(0,nSubCarriersPerRB-1);
            if(sym_cnt==0)
            {
                rxRSPerRB(rb_cnt) = zeros_c(nRxAntenna,0);
            }
            for(int sc_cnt=0;sc_cnt<scIndices.length();sc_cnt++)
            {
                if(rbRSPattern(sc_cnt,sym_cnt)>=3400) // Retrieving only from CSIRS positions
                {
                    cmat sym = zeros_c(nRxAntenna,1);
                    for(int rxAntCnt=0;rxAntCnt<nRxAntenna;rxAntCnt++)
                    {
                        sym(rxAntCnt,0) = rxGrid(rxAntCnt)(gSc + scIndices(sc_cnt),sym_cnt);
                    }
                    rxRSPerRB(rb_cnt) = concat_horizontal(rxRSPerRB(rb_cnt),sym);
                }
            }
        }
        
        
//         cout<<"rxGrid : "<<rxGrid(0).get_rows(gSc,gSc+11)<<endl;
        return rxRSPerRB;
}

cmat computeRxIntrCov(int nRBs,Array<cmat>& rxGrid, ivec allocatedRBs, imat rbRSPattern, double boostForCDM)
{
    int nRxAntenna = rxGrid.length();
    
    int nSymbolsPerSlot = 14,nSubCarriersPerRB = 12;
    
    cmat intrCov=zeros_c(nRxAntenna,nRxAntenna);
    int csirs_cnt=0;
    int fftSize  = rxGrid(0).rows();
    int gSc = (fftSize-nRBs*nSubCarriersPerRB)/2;
    
    
    for(int sym_cnt=0;sym_cnt<nSymbolsPerSlot;sym_cnt++)
        for(int rb_cnt=0;rb_cnt<allocatedRBs.length();rb_cnt++)
        {
            ivec scIndices = allocatedRBs(rb_cnt)*nSubCarriersPerRB + getIntegers(0,nSubCarriersPerRB-1);
            for(int sc_cnt=0;sc_cnt<scIndices.length();sc_cnt++)
            {
                if(rbRSPattern(sc_cnt,sym_cnt)>=3400) // Retrieving only from CSIRS positions
                {
                    
                    cmat sym = zeros_c(nRxAntenna,1);
                    for(int rxAntCnt=0;rxAntCnt<nRxAntenna;rxAntCnt++)
                        sym(rxAntCnt,0) = rxGrid(rxAntCnt)(gSc + scIndices(sc_cnt),sym_cnt);
                    
                    intrCov+=sym*sym.H();
                    csirs_cnt++;
                }
            }
        }

        intrCov*=dComplex(boostForCDM / (double)csirs_cnt,0);
    return intrCov;    
}

cmat computeRxIntrCov(Array<cmat>& rxGrid)
{
    int nRxAntenna = rxGrid.length();
    
    cmat intrCov=zeros_c(nRxAntenna,nRxAntenna);
    for(int re_cnt=0;re_cnt<rxGrid.length();re_cnt++)
    {          
        cmat sym = rxGrid(re_cnt);
        intrCov+=sym*sym.H();
    }
    intrCov=intrCov/rxGrid.length();
    return intrCov;    
}


// Array<cmat> estimateChannelFromCSIRS(Array<cmat>& rxRSPerRB, Array<cvec> csirsPerPort,cmat intCov)
// {
//     Array<cmat> estimatedChannelPerRB(rxRSPerRB.length());
//     int nCSIRSPort = csirsPerPort.length(),nRxAntenna = rxRSPerRB(0).rows();
//     complex<double>i1= dComplex(0,1);
//     for(int rb_cnt=0;rb_cnt<rxRSPerRB.length();rb_cnt++)
//     {
//         estimatedChannelPerRB(rb_cnt) = zeros_c(nRxAntenna,nCSIRSPort);
//         for(int port_cnt=0; port_cnt<nCSIRSPort; port_cnt= port_cnt+2) //considering CDM-2
//         {
//             cmat Y1 = rxRSPerRB(rb_cnt).get_col(port_cnt);
//             cmat Y2 = rxRSPerRB(rb_cnt).get_col(port_cnt+1);
//             
//             
//             cmat y1 = (Y1-Y2)/2;  //manualy removing the OCC. 
//             cmat y2 = (Y1+Y2)/2;
//             
//                         
//             cmat x1(1,1);x1(0,0) = csirsPerPort(port_cnt)(2*rb_cnt);
//             cmat x2(1,1);x2(0,0) = csirsPerPort(port_cnt+1)(2*rb_cnt);
//             
//                    
//             cmat h1 = y1*x1.H()*inv(x1*x1.H()+intCov);
//             cmat h2 = y2*x2.H()*inv(x2*x2.H()+intCov);
//             
//             cout<<"info :"<<Y1<<" \n "<<Y2<<" \n "<<y1<<" \n "<<y2<<" \n "<<x1<<" \n "<<x2<<" \n "<<intCov<<" \n "<<h1<<" \n "<<h2<<endl;
//             
//             
//             
//             cmat h1c = inv(x1)*y1;
//             cmat h2c = inv(x2)*y2;
//            
//             cout<<h1c<<" \n "<<h2c<<endl;
// 
//             
// //             if(rb_cnt==0)
// //             {
// //                 cout<<"Y1Y2 : "<<Y1<<" , "<<Y2<<endl;
// //                 cout<<"y1y2 : "<<y1<<" , "<<y2<<endl;
// //                 cout<<"x1x2 : "<<x1<<" , "<<x2<<endl;
// //                 cout<<"h1h2 : "<<h1<<" , "<<h2<<endl;
// //                 cout<<"h1h2c : "<<h1c<<" , "<<h2c<<endl;
// //             }
// //             
//             estimatedChannelPerRB(rb_cnt).set_col(port_cnt,h1.get_col(0));
//             estimatedChannelPerRB(rb_cnt).set_col(port_cnt+1,h2.get_col(0));
//         }
//     }
//     return estimatedChannelPerRB;
// }


Array<cmat> estimateChannelFromCSIRS(Array<cmat>& rxRSPerRB, Array<cvec> csirsPerPort,cmat intCov)
{
    Array<cmat> estimatedChannelPerRB(rxRSPerRB.length());
    int nCSIRSPort = csirsPerPort.length(),nRxAntenna = rxRSPerRB(0).rows();
    complex<double>i1= dComplex(0,1);
    for(int rb_cnt=0;rb_cnt<rxRSPerRB.length();rb_cnt++)
    {
        estimatedChannelPerRB(rb_cnt) = zeros_c(nRxAntenna,nCSIRSPort);
        for(int port_cnt=0; port_cnt<nCSIRSPort; port_cnt= port_cnt+2) //considering CDM-2
        {
            cmat Y1 = rxRSPerRB(rb_cnt).get_col(port_cnt);
            cmat Y2 = rxRSPerRB(rb_cnt).get_col(port_cnt+1);
            
            cmat Y=concat_vertical(Y1,Y2);
            
            cmat x(2,2);
            x(0,0) = csirsPerPort(port_cnt)(2*rb_cnt);
            x(1,0) = csirsPerPort(port_cnt)(2*rb_cnt+1);
            x(0,1) = csirsPerPort(port_cnt+1)(2*rb_cnt);
            x(1,1) = csirsPerPort(port_cnt+1)(2*rb_cnt+1);
            
            cmat H = inv(x)*Y;
            
            estimatedChannelPerRB(rb_cnt).set_col(port_cnt,H.get_row(0));
            estimatedChannelPerRB(rb_cnt).set_col(port_cnt+1,H.get_row(1));
        }
    }
    return estimatedChannelPerRB;
}

double computeCQI(CSI_Info_S& cqiInfo, Array<vec>& sinrPerCodeword,int nLayers,cTBTables* tbTables,int nREsPerRB,int rbCount)
{
    int nCodeWords = sinrPerCodeword.length();
    cqiInfo.initCSI_Info(nCodeWords,nLayers);
    
    cqiInfo.nLayersPerCW = getNumLayersPerCodeWord(nCodeWords,nLayers);
    cqiInfo.specEff=0;
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
        cqiInfo.CQIindexPerCW(icw) = tbTables->awgnTablesConvCoder.CQISelect(exp_spec_eff1, cqiInfo.MMIBperCW(icw),tbSizePerCQI,false);
        
        cqiInfo.modSchemePerCW(icw) = ModulationScheme_E(tbTables->cqiToMaxMCSRate.getCQIModulationAlphabet(cqiInfo.CQIindexPerCW(icw)));
        
        cqiInfo.effSINRsPerCW(icw) = tbTables->awgnTablesConvCoder.mmibToSinr(cqiInfo.modSchemePerCW(icw),cqiInfo.MMIBperCW(icw)[cqiInfo.modSchemePerCW(icw)]);
        cqiInfo.specEff +=exp_spec_eff1 * cqiInfo.nLayersPerCW(icw);//log2(1.0 + inv_dB(cqiInfo.effSINRsPerCW(icw))) * cqiInfo.nLayersPerCW(icw);   
        
    }
    
    return cqiInfo.specEff;
}

Array<vec> calculateSinrPerCodewordNR(int nLayers, Array<cmat>& rawChannel,cmat precoder, double sigpowInWatts, cmat estimatedIntrCovariance,DemodulationScheme_E desc)
{
    int nTones = rawChannel.length();
    Array<vec> sinr(nLayers); // <layer<re>>
    for(int iLayer=0;iLayer<nLayers;iLayer++)
        sinr(iLayer).set_length(nTones);
    vec sinrtemp(nLayers);
//     cout<<"nTones : "<<nTones<<endl;
    for(int tone_cnt=0;tone_cnt <nTones;tone_cnt++)
    {
        cmat chan = sqrt(sigpowInWatts)*rawChannel(tone_cnt)*precoder;
//         cmat tempChan, tempPrec, temp1;
//         tempChan = rawChannel(tone_cnt).get_cols(0,15);
//         tempPrec = precoder.get_rows(0,15);
//         temp1 = tempChan*tempPrec;
//         cout<<endl<<"channel gain: "<<abs(temp1(0,0))<<endl;abort();
//         cout<<"info : "<<chan<<"\n\n"<<estimatedIntrCovariance<<" \n\n "<<nLayers<<endl;
        sinrtemp = getSINRperLayer(desc,chan,estimatedIntrCovariance,nLayers,sigpowInWatts);
        for(int iLayer=0;iLayer<nLayers;iLayer++)
            sinr(iLayer)[tone_cnt] = sinrtemp(iLayer);
    }
//     cout<<"sinr : "<<sinr<<endl;
Array<vec> sinrPerCodeword = performLayerDeMapping(sinr,(nLayers>4) ? 2 : 1, nLayers);
    return(sinrPerCodeword);
}

CSI_Info_S computeBestPMIAndCQI(Array<cmat>& currentRawChannel,Array<cmat>& codeBook,cTBTables* tbTables,cmat intrCov,int subbandSize,int nSubbands,int nREsPerRB,DemodulationScheme_E desc)
{
    int rank = codeBook(0).cols();
    Array<CSI_Info_S> csiInfoPerPrecoder(codeBook.length());
    vec specEffPerPMI(codeBook.length());
    for(int pmi=0;pmi<codeBook.length();pmi++)
    {
        cmat precoder = codeBook(pmi);
        csiInfoPerPrecoder(pmi).sinrPerCodeword = calculateSinrPerCodewordNR(rank,currentRawChannel,precoder,1.0,intrCov,desc);
        specEffPerPMI(pmi)= computeCQI(csiInfoPerPrecoder(pmi),csiInfoPerPrecoder(pmi).sinrPerCodeword,rank,tbTables,nREsPerRB,nSubbands*subbandSize);
        csiInfoPerPrecoder(pmi).PMI = pmi;
    }
    return csiInfoPerPrecoder(max_index(specEffPerPMI));
}

CSI_Feedback_Info_S doLinkAdaptationforUL(Array<cmat>& estimatedChannelPerRB, int ulNRBs, int prgSize, int nPorts, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,int& nLayers)
{
    int nSubbands = ulNRBs/prgSize;
    int subbandSize = prgSize;
    vec specEffPerRank = zeros(maxRank);
    ivec pmiIndicesPerRank(maxRank);
    Array<ivec> cqiPerRank(maxRank);
    Array<cmat> precoderPerRank(maxRank);
    
    CSI_Feedback_Info_S csiFeedbackInfo;
    csiFeedbackInfo.cqiPerSubbandPerCW.set_size(nSubbands);
    csiFeedbackInfo.specEffPerSubband.set_size(nSubbands);
    csiFeedbackInfo.precoderPerSubband.set_size(nSubbands);
    
    for(int rank=1;rank<=maxRank;rank++)
    {
        Array<cmat> codeBook = codebook.getCodebook(nPorts,rank,false);
        int nREsPerRB = 2*12; //Two OFDM symbols in every RB
        CSI_Info_S cqiInfo = computeBestPMIAndCQI(estimatedChannelPerRB,codeBook,&tbTables,intCov,subbandSize,nSubbands,nREsPerRB,_DEMODULATION_SCHEME_MMSE_);
        pmiIndicesPerRank(rank-1) = cqiInfo.PMI;
        
        
        Array<vec> avgSinrPerCW(nCodeWords);
        for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++)
        {
            Array<cmat> currentRawChannel = estimatedChannelPerRB(subbandSize*sb_cnt,(sb_cnt+1)*subbandSize-1);
            CSI_Info_S cqiInfo = computeBestPMIAndCQI(currentRawChannel,codeBook,&tbTables,intCov,subbandSize,1,nREsPerRB,_DEMODULATION_SCHEME_MMSE_);
            for(int cw_cnt=0;cw_cnt<nCodeWords;cw_cnt++)
                append(avgSinrPerCW(cw_cnt),cqiInfo.sinrPerCodeword(cw_cnt));
        }
        
        specEffPerRank(rank-1) = computeCQI(cqiInfo,avgSinrPerCW,rank,&tbTables,nREsPerRB,nSubbands*subbandSize);
        cqiPerRank(rank-1) = cqiInfo.CQIindexPerCW;
        precoderPerRank(rank-1) = codeBook(cqiInfo.PMI);
    }
//     cout<<"specEffPerRank : "<<specEffPerRank<<endl;
    nLayers = max_index(specEffPerRank)+1;
//     cout<<"Selected Rank : "<<nLayers<<endl;
    
    int selectedRankIndx = max_index(specEffPerRank);
    csiFeedbackInfo.specEffWideband = specEffPerRank(selectedRankIndx);
    csiFeedbackInfo.nLayers = selectedRankIndx+1;
    for(int sbIndx = 0;sbIndx < nSubbands;sbIndx++){
        csiFeedbackInfo.cqiPerSubbandPerCW(sbIndx) = cqiPerRank(selectedRankIndx);
        csiFeedbackInfo.precoderPerSubband(sbIndx) = precoderPerRank(selectedRankIndx);
        csiFeedbackInfo.specEffPerSubband(sbIndx) = specEffPerRank(selectedRankIndx);
    }
    
    return csiFeedbackInfo;
}

vec doLinkAdaptationforTypeI(Array<cmat>& estimatedChannelPerRB, int dlNRBs, int prgSize, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,int& nLayers,Array<cmat> &precoderPerSubband,Array<ivec> &cqi)
{
    type1PMI_S pmiIndices;
    ivec wbcqiPerCW;
    double specEff = doLinkAdaptationforTypeI(estimatedChannelPerRB, dlNRBs, prgSize, maxRank, nCodeWords, intCov,tbTables,codebook,nLayers,pmiIndices,wbcqiPerCW);
    precoderPerSubband = codebook.getNRPrecoderPerSubband(nLayers,pmiIndices);
    cqi=repeatAsArray(wbcqiPerCW,precoderPerSubband.length());
    vec specEffPerSubband = repeatAsVector(specEff,precoderPerSubband.length());
    return specEffPerSubband;
}

double doLinkAdaptationforTypeI(Array<cmat>& estimatedChannelPerRB, int dlNRBs, int prgSize, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,int& nLayers,type1PMI_S& pmiIndices,ivec &cqi)
{
    int nSubbands = dlNRBs/prgSize;
    int subbandSize = prgSize;
    vec specEffPerRank = zeros(maxRank);
    Array<type1PMI_S> pmiIndicesPerRank(maxRank);
    Array<ivec> cqiPerRank(maxRank);
    
    for(int rank=1;rank<=maxRank;rank++)
    {
        Array<cmat> codeBook = codebook.getCodebook(codebook.dlCodeBookConfig.nCSIRSPorts,rank,1);
        int nREsPerRB = 2*12; //Two OFDM symbols in every RB
        CSI_Info_S cqiInfo = computeBestPMIAndCQI(estimatedChannelPerRB,codeBook,&tbTables,intCov,subbandSize,nSubbands,nREsPerRB,_DEMODULATION_SCHEME_MMSE_);
        pmiIndicesPerRank(rank-1) = codebook.getPMIIndices(rank,cqiInfo.PMI);
        
        ivec i2Indices = codebook.geti2CmatIndicesIn_W(pmiIndicesPerRank(rank-1),rank);
        codeBook = getFromArray(codeBook,i2Indices);
        pmiIndicesPerRank(rank-1).i2Indices.set_size(nSubbands);
        Array<vec> avgSinrPerCW(nCodeWords);
        for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++)
        {
            Array<cmat> currentRawChannel = estimatedChannelPerRB(subbandSize*sb_cnt,(sb_cnt+1)*subbandSize-1);
            CSI_Info_S cqiInfo = computeBestPMIAndCQI(currentRawChannel,codeBook,&tbTables,intCov,subbandSize,1,nREsPerRB,_DEMODULATION_SCHEME_MMSE_);
            pmiIndicesPerRank(rank-1).i2Indices(sb_cnt) = cqiInfo.PMI;
            for(int cw_cnt=0;cw_cnt<nCodeWords;cw_cnt++)
                append(avgSinrPerCW(cw_cnt),cqiInfo.sinrPerCodeword(cw_cnt));
        }
        specEffPerRank(rank-1) = computeCQI(cqiInfo,avgSinrPerCW,rank,&tbTables,nREsPerRB,nSubbands*subbandSize);
        cqiPerRank(rank-1) = cqiInfo.CQIindexPerCW;
        
    }
//     cout<<"specEffPerRank : "<<specEffPerRank<<endl;
    nLayers = max_index(specEffPerRank)+1;
//     cout<<"Selected Rank : "<<nLayers<<endl;
    pmiIndices = pmiIndicesPerRank(nLayers-1);
    cqi = cqiPerRank(nLayers-1);
    return specEffPerRank(nLayers-1);
}

vec doLinkAdaptationforTypeII(Array<cmat>& estimatedChannelPerRB,Array<cmat>& precoderPerSubband, int dlNRBs, int prgSize, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,int& nLayers,Array<ivec> &cqi)
{
    Type2coefficients_N_OMP_S finalCoefficients;
    ivec wbcqiPerCW;
    CSI_Feedback_Info_S csiFeedback;
    csiFeedback = doLinkAdaptationforTypeII(estimatedChannelPerRB,finalCoefficients,dlNRBs, prgSize, maxRank, nCodeWords, intCov,tbTables,codebook,nLayers,wbcqiPerCW);
    double specEff = csiFeedback.specEffWideband;
    precoderPerSubband = finalCoefficients.precoderPerSubband;
    cqi=repeatAsArray(wbcqiPerCW,precoderPerSubband.length());
    vec specEffPerSubband = repeatAsVector(specEff,precoderPerSubband.length());
    return specEffPerSubband;
}

double computeSEforRank1Type1BruteForceWideband(Array<cmat> idealChannel, Array<cmat> orthBeamsTotalSets, int bestBeamSet, ivec bestBeams, cmat coPhase, cmat IntrCov, cTBTables tbTables, int dlRBs){
    int rank = 1;
    double se;
    vec allSE;
    cmat beams = orthBeamsTotalSets(bestBeamSet).get_cols(bestBeams);
    for(int beamIndx = 0; beamIndx < bestBeams.length();beamIndx++){
        for(int coPhaseIndx = 0;coPhaseIndx < coPhase.cols();coPhaseIndx++){
            cmat precoder = kron(coPhase.get_cols(coPhaseIndx,coPhaseIndx),beams.get_cols(beamIndx,beamIndx));
            Array<vec> sinrPerCW = calculateSinrPerCodewordNR(rank,idealChannel,precoder,1.0,IntrCov,_DEMODULATION_SCHEME_MMSE_);
            
            CSI_Info_S cqiInfo;
            double SEtemp;
            SEtemp = computeCQI(cqiInfo,sinrPerCW,rank,&tbTables,90,dlRBs);
            allSE = concat(allSE,SEtemp);
        }
    }
    se = max(allSE);
    return se;
}

double computeSEforRank2Type1BruteForceWideband(Array<cmat> idealChannel, Array<cmat> orthBeamsTotalSets, int bestBeamSet, ivec bestBeams, cmat coPhase, cmat IntrCov, cTBTables tbTables, int dlRBs, int N1, int N2){
    int rank = 2;
    double se;
    vec allSE;
    ivec k1 = "0,1,0,2";
    ivec k2 = "0,0,1,0";
    for(int beamIndx = 0; beamIndx < bestBeams.length();beamIndx++){
        int beam1Indx = bestBeams(beamIndx);
        cmat beam1 = orthBeamsTotalSets(bestBeamSet).get_cols(beam1Indx,beam1Indx);
        int n1Beam1 = beam1Indx/N2;int n2Beam1 = mod(beam1Indx,N2);
        for(int offsetIndx = 0;offsetIndx < k1.length();offsetIndx++){
            int n1Beam2 = mod(n1Beam1+k1(offsetIndx),N1);
            int n2Beam2 = mod(n2Beam1+k2(offsetIndx),N2);
            int beam2Indx = (n1Beam2*N2)+n2Beam2;
            cmat beam2 = orthBeamsTotalSets(bestBeamSet).get_cols(beam2Indx,beam2Indx);
            for(int coPhaseIndx = 0;coPhaseIndx < coPhase.cols();coPhaseIndx++){
                cmat layer1Precoder = kron(coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam1);
                cmat temp = "1 0;0 -1";
                cmat layer2Precoder = kron(temp*coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam2);
                cmat precoder = concat_horizontal(layer1Precoder,layer2Precoder);
                Array<vec> sinrPerCW = calculateSinrPerCodewordNR(rank,idealChannel,precoder,1.0,IntrCov,_DEMODULATION_SCHEME_MMSE_);
                
                CSI_Info_S cqiInfo;
                double SEtemp;
                SEtemp = computeCQI(cqiInfo,sinrPerCW,rank,&tbTables,90,dlRBs);
                allSE = concat(allSE,SEtemp);
            }
        }
    }
    se = max(allSE);
    return se;
}

double computeSEforRank1Type1R15(Array<cmat> idealChannel, Array<cmat> orthBeamsTotalSets, int bestBeamSet, ivec bestBeams, cmat coPhase, cmat IntrCov, cTBTables tbTables, int dlRBs, int subbandSize){
    int rank = 1;
    double se;
    int nSubbands = dlRBs/subbandSize;
    cmat beams = orthBeamsTotalSets(bestBeamSet).get_cols(bestBeams);
    vec sePerBeam(beams.cols());
    for(int beamIndx = 0;beamIndx < bestBeams.length();beamIndx++){
        Array<vec> sinrWideband(1);  // 1 because single codeword
        for(int sbIndx = 0;sbIndx < nSubbands;sbIndx++){
            ivec scIndicesWithinSubband;
            scIndicesWithinSubband = getIntegers(0,subbandSize-1)+(sbIndx*subbandSize);
            Array<cmat> sbChannel = getFromArray(idealChannel,scIndicesWithinSubband);
            vec sePerCophase(coPhase.cols());
            Array<Array<vec>> sinrPerCophase(coPhase.cols());
            for(int coPhaseIndx = 0;coPhaseIndx < coPhase.cols();coPhaseIndx++){
                cmat precoder = kron(coPhase.get_cols(coPhaseIndx,coPhaseIndx),beams.get_cols(beamIndx,beamIndx));
                sinrPerCophase(coPhaseIndx) = calculateSinrPerCodewordNR(rank,sbChannel,precoder,1.0,IntrCov,_DEMODULATION_SCHEME_MMSE_);
                CSI_Info_S cqiInfo;
                double SEtemp;
                SEtemp = computeCQI(cqiInfo,sinrPerCophase(coPhaseIndx),rank,&tbTables,90,dlRBs);
                sePerCophase(coPhaseIndx) = SEtemp;
            }
            int bestCophaseIndx = max_index(sePerCophase);
            sinrWideband(0) = concat(sinrWideband(0),sinrPerCophase(bestCophaseIndx)(0));
        }
        CSI_Info_S cqiInfo;
        double SEtemp;
        SEtemp = computeCQI(cqiInfo,sinrWideband,rank,&tbTables,90,dlRBs);
        sePerBeam(beamIndx) = SEtemp;
    }
    se = max(sePerBeam);
    return se;
}

double computeSEforRank2to8Type1R15(Array<cmat> idealChannel, Array<cmat> orthBeamsTotalSets, int bestBeamSet, ivec bestBeams, cmat coPhase, cmat IntrCov, cTBTables tbTables, int dlRBs, int subbandSize, int N1, int N2, int rank){
    ivec k1,k2,k3,k4,k5,k6;  //Beam Offset Indices
    if(rank <= 4){
        k1 = "0,1,0,2";
        k2 = "0,0,1,0";
    }
    else if((rank >= 5) && (rank <= 6)){
        k1 = "1";k2 = "0";
        k3 = "1";k4 = "1";
    }
    else{
        k1 = "1";k2 = "0";
        k3 = "0";k4 = "1";
        k5 = "1";k6 = "1";
    }
    cmat temp = "1 0;0 -1";
    double se;
    int nSubbands = dlRBs/subbandSize;
    cmat beams = orthBeamsTotalSets(bestBeamSet).get_cols(bestBeams);
    vec sePerBeam(beams.cols());
    cmat beam1, beam2, beam3, beam4;
    for(int beamIndx = 0;beamIndx < bestBeams.length();beamIndx++){
        int beam1Indx = bestBeams(beamIndx);
        beam1 = orthBeamsTotalSets(bestBeamSet).get_cols(beam1Indx,beam1Indx);
        int n1Beam1 = beam1Indx/N2;int n2Beam1 = mod(beam1Indx,N2);
        vec sePerBeamOffset(k1.length());
        for(int offsetIndx = 0;offsetIndx < k1.length();offsetIndx++){
            int n1Beam2 = mod(n1Beam1+k1(offsetIndx),N1);
            int n2Beam2 = mod(n2Beam1+k2(offsetIndx),N2);
            int beam2Indx = (n1Beam2*N2)+n2Beam2;
            beam2 = orthBeamsTotalSets(bestBeamSet).get_cols(beam2Indx,beam2Indx);
            
            if(k3.length() != 0){
                int n1Beam3 = mod(n1Beam1+k3(0),N1);
                int n2Beam3 = mod(n2Beam1+k4(0),N2);
                int beam3Indx = (n1Beam3*N2)+n2Beam3;
                beam3 = orthBeamsTotalSets(bestBeamSet).get_cols(beam3Indx,beam3Indx);
            }
            if(k5.length() != 0){
                int n1Beam4 = mod(n1Beam1+k5(0),N1);
                int n2Beam4 = mod(n2Beam1+k6(0),N2);
                int beam4Indx = (n1Beam4*N2)+n2Beam4;
                beam4 = orthBeamsTotalSets(bestBeamSet).get_cols(beam4Indx,beam4Indx);
            }
            
            int nCodeWords = (rank>4) ? 2 : 1;
            Array<vec> sinrWideband(nCodeWords);
            
            for(int sbIndx = 0;sbIndx < nSubbands;sbIndx++){
                ivec scIndicesWithinSubband;
                scIndicesWithinSubband = getIntegers(0,subbandSize-1)+(sbIndx*subbandSize);
                Array<cmat> sbChannel = getFromArray(idealChannel,scIndicesWithinSubband);
                vec sePerCophase(coPhase.cols());
                Array<Array<vec>> sinrPerCophase(coPhase.cols());
                for(int coPhaseIndx = 0;coPhaseIndx < coPhase.cols();coPhaseIndx++){
                    cmat precoder;
                    cmat layer1Precoder = kron(coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam1);
                    precoder = concat_horizontal(precoder,layer1Precoder);
                    if(rank >= 2){
                        cmat layer2Precoder;
                        if(rank == 2){
                            layer2Precoder = kron(temp*coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam2);
                        }
                        else if((rank == 3) || (rank == 4)){
                            layer2Precoder = kron(coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam2);
                        }
                        else{
                            layer2Precoder = kron(temp*coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam1);
                        }
                        precoder = concat_horizontal(precoder,layer2Precoder);
                    }
                    if(rank >= 3){
                        cmat layer3Precoder;
                        if((rank == 3)||(rank == 4)){
                            layer3Precoder = kron(temp*coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam1);
                        }
                        else if(rank == 5){
                            layer3Precoder = kron(coPhase.get_cols(0,0),beam2);
                        }
                        else{
                            layer3Precoder = kron(temp*coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam2);
                        }
                        precoder = concat_horizontal(precoder,layer3Precoder);
                    }
                    if(rank >= 4){
                        cmat layer4Precoder;
                        if((rank == 4)||(rank == 6)||(rank == 8)){
                            layer4Precoder = kron(temp*coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam2);
                        }
                        else if(rank == 5){
                            layer4Precoder = kron(coPhase.get_cols(2,2),beam2);
                        }
                        else{
                            layer4Precoder = kron(coPhase.get_cols(0,0),beam3);
                        }
                        precoder = concat_horizontal(precoder,layer4Precoder);
                    }
                    if(rank >= 5){
                        cmat layer5Precoder;
                        if((rank == 5)||(rank == 6)||(rank == 8)){
                            layer5Precoder = kron(coPhase.get_cols(0,0),beam3);
                        }
                        else{
                            layer5Precoder = kron(coPhase.get_cols(2,2),beam3);
                        }
                        precoder = concat_horizontal(precoder,layer5Precoder);
                    }
                    if(rank >= 6){
                        cmat layer6Precoder;
                        if((rank == 6)||(rank == 8)){
                            layer6Precoder = kron(coPhase.get_cols(2,2),beam3);
                        }
                        else{
                            layer6Precoder = kron(coPhase.get_cols(0,0),beam4);
                        }
                        precoder = concat_horizontal(precoder,layer6Precoder);
                    }
                    if(rank >= 7){
                        cmat layer7Precoder;
                        if(rank == 7){
                            layer7Precoder = kron(coPhase.get_cols(2,2),beam4);
                        }
                        else{
                            layer7Precoder = kron(coPhase.get_cols(0,0),beam4);
                        }
                        precoder = concat_horizontal(precoder,layer7Precoder);
                    }
                    if(rank == 8){
                        cmat layer8Precoder;
                        layer8Precoder = kron(coPhase.get_cols(2,2),beam4);
                        precoder = concat_horizontal(precoder,layer8Precoder);
                    }
                    precoder = 1/sqrt(rank)*precoder;
                    
                    sinrPerCophase(coPhaseIndx) = calculateSinrPerCodewordNR(rank,sbChannel,precoder,1.0,IntrCov,_DEMODULATION_SCHEME_MMSE_);
                    CSI_Info_S cqiInfo;
                    sePerCophase(coPhaseIndx) = computeCQI(cqiInfo,sinrPerCophase(coPhaseIndx),rank,&tbTables,90,dlRBs);
                }
                int bestCophaseIndx = max_index(sePerCophase);
                sinrWideband(0) = concat(sinrWideband(0),sinrPerCophase(bestCophaseIndx)(0));
                if(rank > 4){sinrWideband(1) = concat(sinrWideband(1),sinrPerCophase(bestCophaseIndx)(1));}
            }
            CSI_Info_S cqiInfo;
            double SEtemp;
            sePerBeamOffset(offsetIndx) = computeCQI(cqiInfo,sinrWideband,rank,&tbTables,90,dlRBs);
        }
        sePerBeam(beamIndx) = max(sePerBeamOffset);
    }
    
    se = max(sePerBeam);
    return se;
}


double computeSEforRank2to8Type1R19Scheme2(Array<cmat> idealChannel, Array<cmat> orthBeamsTotalSets, int bestBeamSet, ivec bestBeams, cmat coPhase, cmat IntrCov, cTBTables tbTables, int dlRBs, int subbandSize, int N1, int N2, int rank){
    cmat temp = "1 0;0 -1";
    double se;
    int nSubbands = dlRBs/subbandSize;
    cmat beams = orthBeamsTotalSets(bestBeamSet).get_cols(bestBeams);
    cmat beam1, beam2, beam3, beam4;
    beam1 = beams.get_cols(0,0);
    beam2 = beams.get_cols(1,1);
    beam3 = beams.get_cols(2,2);
    beam4 = beams.get_cols(3,3);
    int nCodeWords = (rank>4) ? 2 : 1;
    Array<vec> sinrWideband(nCodeWords);
    
    for(int sbIndx = 0;sbIndx < nSubbands;sbIndx++){
        ivec scIndicesWithinSubband;
        scIndicesWithinSubband = getIntegers(0,subbandSize-1)+(sbIndx*subbandSize);
        Array<cmat> sbChannel = getFromArray(idealChannel,scIndicesWithinSubband);
        vec sePerCophase(coPhase.cols());
        Array<Array<vec>> sinrPerCophase(coPhase.cols());
        for(int coPhaseIndx = 0;coPhaseIndx < coPhase.cols();coPhaseIndx++){
            cmat precoder;
            cmat layer1Precoder = kron(coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam1);
            precoder = concat_horizontal(precoder,layer1Precoder);
            if(rank >= 2){
                cmat layer2Precoder;
                if(rank == 2){
                    layer2Precoder = kron(temp*coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam2);
                }
                else if((rank == 3) || (rank == 4)){
                    layer2Precoder = kron(coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam2);
                }
                else{
                    layer2Precoder = kron(temp*coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam1);
                }
                precoder = concat_horizontal(precoder,layer2Precoder);
            }
            if(rank >= 3){
                cmat layer3Precoder;
                if((rank == 3)||(rank == 4)){
                    layer3Precoder = kron(temp*coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam1);
                }
                else if(rank == 5){
                    layer3Precoder = kron(coPhase.get_cols(0,0),beam2);
                }
                else{
                    layer3Precoder = kron(temp*coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam2);
                }
                precoder = concat_horizontal(precoder,layer3Precoder);
            }
            if(rank >= 4){
                cmat layer4Precoder;
                if((rank == 4)||(rank == 6)||(rank == 8)){
                    layer4Precoder = kron(temp*coPhase.get_cols(coPhaseIndx,coPhaseIndx),beam2);
                }
                else if(rank == 5){
                    layer4Precoder = kron(coPhase.get_cols(2,2),beam2);
                }
                else{
                    layer4Precoder = kron(coPhase.get_cols(0,0),beam3);
                }
                precoder = concat_horizontal(precoder,layer4Precoder);
            }
            if(rank >= 5){
                cmat layer5Precoder;
                if((rank == 5)||(rank == 6)||(rank == 8)){
                    layer5Precoder = kron(coPhase.get_cols(0,0),beam3);
                }
                else{
                    layer5Precoder = kron(coPhase.get_cols(2,2),beam3);
                }
                precoder = concat_horizontal(precoder,layer5Precoder);
            }
            if(rank >= 6){
                cmat layer6Precoder;
                if((rank == 6)||(rank == 8)){
                    layer6Precoder = kron(coPhase.get_cols(2,2),beam3);
                }
                else{
                    layer6Precoder = kron(coPhase.get_cols(0,0),beam4);
                }
                precoder = concat_horizontal(precoder,layer6Precoder);
            }
            if(rank >= 7){
                cmat layer7Precoder;
                if(rank == 7){
                    layer7Precoder = kron(coPhase.get_cols(2,2),beam4);
                }
                else{
                    layer7Precoder = kron(coPhase.get_cols(0,0),beam4);
                }
                precoder = concat_horizontal(precoder,layer7Precoder);
            }
            if(rank == 8){
                cmat layer8Precoder;
                layer8Precoder = kron(coPhase.get_cols(2,2),beam4);
                precoder = concat_horizontal(precoder,layer8Precoder);
            }
            precoder = 1/sqrt(rank)*precoder;
            
            sinrPerCophase(coPhaseIndx) = calculateSinrPerCodewordNR(rank,sbChannel,precoder,1.0,IntrCov,_DEMODULATION_SCHEME_MMSE_);
            CSI_Info_S cqiInfo;
            sePerCophase(coPhaseIndx) = computeCQI(cqiInfo,sinrPerCophase(coPhaseIndx),rank,&tbTables,90,dlRBs);
        }
        int bestCophaseIndx = max_index(sePerCophase);
        sinrWideband(0) = concat(sinrWideband(0),sinrPerCophase(bestCophaseIndx)(0));
        if(rank > 4){sinrWideband(1) = concat(sinrWideband(1),sinrPerCophase(bestCophaseIndx)(1));}
    }
    CSI_Info_S cqiInfo;
    se = computeCQI(cqiInfo,sinrWideband,rank,&tbTables,90,dlRBs);
    
    return se;
}

cmat getR15Type1SPPrecoder(cmat beams, cmat coPhaseBeam1, int rank){
    
    cmat temp = "1 0;0 -1";
    int nBeams = beams.cols();
    cmat beam1, beam2, beam3, beam4;
    beam1 = beams.get_cols(0,0);
    if(nBeams > 1){beam2 = beams.get_cols(1,1);}
    if(nBeams > 2){beam3 = beams.get_cols(2,2);}
    if(nBeams > 3){beam4 = beams.get_cols(3,3);}
    cmat coPhase = "1+0i 1+0i 1+0i 1+0i;1+0i 0+1i -1+0i 0-1i";
   
    cmat precoder;
    cmat layer1Precoder = kron(coPhaseBeam1,beam1);
    precoder = concat_horizontal(precoder,layer1Precoder);
    if(rank >= 2){
        cmat layer2Precoder;
        if(rank == 2){
            layer2Precoder = kron(temp*coPhaseBeam1,beam2);
        }
        else if((rank == 3) || (rank == 4)){
            layer2Precoder = kron(coPhaseBeam1,beam2);
        }
        else{
            layer2Precoder = kron(temp*coPhaseBeam1,beam1);
        }
        precoder = concat_horizontal(precoder,layer2Precoder);
    }
    if(rank >= 3){
        cmat layer3Precoder;
        if((rank == 3)||(rank == 4)){
            layer3Precoder = kron(temp*coPhaseBeam1,beam1);
        }
        else if(rank == 5){
            layer3Precoder = kron(coPhase.get_cols(0,0),beam2);
        }
        else{
            layer3Precoder = kron(temp*coPhaseBeam1,beam2);
        }
        precoder = concat_horizontal(precoder,layer3Precoder);
    }
    if(rank >= 4){
        cmat layer4Precoder;
        if((rank == 4)||(rank == 6)||(rank == 8)){
            layer4Precoder = kron(temp*coPhaseBeam1,beam2);
        }
        else if(rank == 5){
            layer4Precoder = kron(coPhase.get_cols(2,2),beam2);
        }
        else{
            layer4Precoder = kron(coPhase.get_cols(0,0),beam3);
        }
        precoder = concat_horizontal(precoder,layer4Precoder);
    }
    if(rank >= 5){
        cmat layer5Precoder;
        if((rank == 5)||(rank == 6)||(rank == 8)){
            layer5Precoder = kron(coPhase.get_cols(0,0),beam3);
        }
        else{
            layer5Precoder = kron(coPhase.get_cols(2,2),beam3);
        }
        precoder = concat_horizontal(precoder,layer5Precoder);
    }
    if(rank >= 6){
        cmat layer6Precoder;
        if((rank == 6)||(rank == 8)){
            layer6Precoder = kron(coPhase.get_cols(2,2),beam3);
        }
        else{
            layer6Precoder = kron(coPhase.get_cols(0,0),beam4);
        }
        precoder = concat_horizontal(precoder,layer6Precoder);
    }
    if(rank >= 7){
        cmat layer7Precoder;
        if(rank == 7){
            layer7Precoder = kron(coPhase.get_cols(2,2),beam4);
        }
        else{
            layer7Precoder = kron(coPhase.get_cols(0,0),beam4);
        }
        precoder = concat_horizontal(precoder,layer7Precoder);
    }
    if(rank == 8){
        cmat layer8Precoder;
        layer8Precoder = kron(coPhase.get_cols(2,2),beam4);
        precoder = concat_horizontal(precoder,layer8Precoder);
    }
    precoder = 1/sqrt(rank)*precoder;
    return precoder;
}


CSI_Feedback_Info_S computeSEforType1Rel19_5_8(CodeBookConfig_S codebookConfig, Array<cmat> idealChannel, cmat IntrCov, cTBTables tbTables, int dlRBs,int subbandSize, bool isR15, int maxRank){
    cmat temp = "1 0;0 -1";
    cmat coPhase = "1+0i 1+0i 1+0i 1+0i;1+0i 0+1i -1+0i 0-1i";
    int nTx = idealChannel(0).cols();
    cmat avgChannelCovar = avgCovariance(idealChannel);
    int nSubbands = dlRBs/subbandSize;
    Codebook codebook;
    vec se(maxRank);
    CSI_Feedback_Info_S csiFeedbackInfo;
    csiFeedbackInfo.cqiPerSubbandPerCW.set_size(nSubbands);
    csiFeedbackInfo.specEffPerSubband.set_size(nSubbands);
    csiFeedbackInfo.precoderPerSubband.set_size(nSubbands);
    
    Array<Array<CSI_Info_S>> cqiInfoPerRankPerSB(8);
    Array<Array<cmat>> precoderPerRankPerSB(maxRank);
    for(int rankIndx = 0;rankIndx < se.length();rankIndx++){
        //         cout<<endl<<"rank Index: "<<rankIndx<<endl;
        int rank = rankIndx+1;
        int nCodeWords = (rank>4) ? 2 : 1;
        cqiInfoPerRankPerSB(rankIndx).set_length(nSubbands);
        precoderPerRankPerSB(rankIndx).set_length(nSubbands);
        
        //SD Basis Calculation

        Type2coefficients_N_OMP_S type2Coefficients;
        int Lbeams = 1;
        int nTx = idealChannel(0).cols();
        int selectedSet = -1;
        int N1=codebookConfig.N1, N2=codebookConfig.N2;
        int O1=4,O2=(N2==1) ? 1:4;
        Array<ivec> i11_IndicesPerSet(O1*O2);  // Stores corresponding i11 index of the beams in set
        Array<ivec> i12_IndicesPerSet(O1*O2);  // Stores corresponding i12 index of the beams in set
        Array<cmat> orthBeamsTotalSets = type2OrthogonalBeamsSet(N1, N2, i11_IndicesPerSet, i12_IndicesPerSet); // Generating orthogonal beam sets
        ivec indices;
        
        cmat avgChannelCovar = avgCovariance(idealChannel);
            
        cmat U,V;  vec S; cvec eigenValues;
        bool isSuccess = eig(avgChannelCovar, eigenValues, V);
        V = V.get_cols(reverse(sort_index(abs(eigenValues))));
        cmat normMatrix = V.H()*V;
        cvec normVec = diag(normMatrix);
        V = inv(diag(normVec))*V; //Normalising individual singular vector with its norm. 
        selectedSet = selectSetWithChannelCovariance(orthBeamsTotalSets,avgChannelCovar,Lbeams,indices); // Beamset Calculation
        cmat temp1 = orthBeamsTotalSets(selectedSet).H()*avgChannelCovar(0,nTx/2-1,0,nTx/2-1)*orthBeamsTotalSets(selectedSet);
        cmat temp2 = orthBeamsTotalSets(selectedSet).H()*avgChannelCovar(nTx/2,nTx-1,nTx/2,nTx-1)*orthBeamsTotalSets(selectedSet);
        cmat temp = temp1+temp2;
        vec beamStrength;
        beamStrength=abs(diag(temp));
        ivec BeamIndices = reverse(sort_index(beamStrength));
        int no_beams=ceil((rank+1)/2);
        ivec finalBeamIndices=BeamIndices(0,no_beams-1);
        cmat bestBeams=orthBeamsTotalSets(selectedSet).get_cols(finalBeamIndices);
        cmat channeleigen=V.get_cols(0,rankIndx);
        cmat beams;
        int cnt=0;
        //Repeating the beams in alternate layers
        ivec selected_columns(rank);
        for(int layercnt=0;layercnt<rank;layercnt++){
            if(layercnt%2==0){
                beams.append_col(bestBeams.get_col(cnt));
                selected_columns(layercnt)=cnt;
                
            }
            else{
                beams.append_col(beams.get_col(layercnt-1));
                selected_columns(layercnt)=cnt;
                cnt=cnt+1;
            }
        }
        Array<vec> sinrWideband(nCodeWords);
        for(int sbIndx = 0;sbIndx < nSubbands;sbIndx++){
            ivec scIndicesWithinSubband;
            scIndicesWithinSubband = getIntegers(0,subbandSize-1)+(sbIndx*subbandSize);
            Array<cmat> sbChannel = getFromArray(idealChannel,scIndicesWithinSubband);
            cmat avgSBChannelCovar = avgCovariance(sbChannel);
            cmat U,V;  vec S; cvec eigenValues;
            bool isSuccess = eig(avgSBChannelCovar, eigenValues, V);
            V = V.get_cols(reverse(sort_index(abs(eigenValues))));
            cmat normMatrix = V.H()*V;
            cvec normVec = diag(normMatrix);
            V = inv(diag(normVec))*V;
           
            Array<vec> metric(rank);
            ivec coPhaseIndxRel19(rank);
                                 
            map<int, int> beamUsage;  // key:beam index Pair:cophase coloumn index
            cmat precoder;

            for (int rni = 0; rni < rank; rni++) {
                metric(rni).set_length(coPhase.cols());

                for (int coPhaseIndx = 0; coPhaseIndx < coPhase.cols(); coPhaseIndx++) {
                    cmat tempPrecoder = kron(coPhase.get_cols(coPhaseIndx, coPhaseIndx), bestBeams.get_cols(rni, rni));
                    cmat temp = tempPrecoder.H() * V.get_cols(0, 0);
                    metric(rni)(coPhaseIndx) = abs(temp(0, 0));
                }

                coPhaseIndxRel19(rni) = max_index(metric(rni));

                // Extract beam index (assuming a valid method to get it)
                int beamIndex = selected_columns(rni);

                // Check if beam is already used
                if (beamUsage.find(beamIndex) == beamUsage.end()) {
                    // if that key is not present in dictionary it will return beamUsage.end()
                    beamUsage[beamIndex] = coPhaseIndxRel19(rni);
                }
                else {
                    // Key is not present means same beam is using twice
                    int firstCoPhaseIndx = beamUsage[beamIndex];
                    // Addign Pi to the first occurance of the beam cophase through the cophase table
                    coPhaseIndxRel19 = (firstCoPhaseIndx + coPhase.cols() / 2) % coPhase.cols();
                }
                cmat layerPrecoder = kron(coPhase.get_cols(coPhaseIndxRel19(rni), coPhaseIndxRel19(rni)), bestBeams.get_cols(rni, rni));
                precoder = concat_horizontal(precoder, layerPrecoder);
            }

            precoderPerRankPerSB(rankIndx)(sbIndx) =  1/sqrt(rank)*precoder;
            
            Array<vec> sinrPerSB = calculateSinrPerCodewordNR(rank,sbChannel,precoderPerRankPerSB(rankIndx)(sbIndx),1.0,IntrCov,_DEMODULATION_SCHEME_MMSE_);
             // if(sbIndx==0)
            sinrWideband(0) = concat(sinrWideband(0),sinrPerSB(0));
            if(rank > 4){sinrWideband(1) = concat(sinrWideband(1),sinrPerSB(1));}
            
            double seTemp = computeCQI(cqiInfoPerRankPerSB(rankIndx)(sbIndx),sinrPerSB,rank,&tbTables,90,subbandSize);
        }
        CSI_Info_S cqiInfo;
        se(rankIndx) = computeCQI(cqiInfo,sinrWideband,rank,&tbTables,90,dlRBs);

    }
    int selectedRankIndx = max_index(se);
    csiFeedbackInfo.specEffWideband = se(selectedRankIndx);
    csiFeedbackInfo.nLayers = selectedRankIndx+1;
    for(int sbIndx = 0;sbIndx < nSubbands;sbIndx++){
        csiFeedbackInfo.cqiPerSubbandPerCW(sbIndx) = cqiInfoPerRankPerSB(selectedRankIndx)(sbIndx).CQIindexPerCW;
        csiFeedbackInfo.precoderPerSubband(sbIndx) = precoderPerRankPerSB(selectedRankIndx)(sbIndx);
        csiFeedbackInfo.specEffPerSubband(sbIndx) = cqiInfoPerRankPerSB(selectedRankIndx)(sbIndx).specEff;
    }
    return csiFeedbackInfo;
}


CSI_Feedback_Info_S computeSEforType1Rel19(CodeBookConfig_S codebookConfig, Array<cmat> idealChannel, cmat IntrCov, cTBTables tbTables, int dlRBs,int subbandSize, bool isR15, int maxRank){
    cmat temp = "1 0;0 -1";
    cmat coPhase = "1+0i 1+0i 1+0i 1+0i;1+0i 0+1i -1+0i 0-1i";
    int nTx = idealChannel(0).cols();
    cmat avgChannelCovar = avgCovariance(idealChannel);
    int nSubbands = dlRBs/subbandSize;
    Codebook codebook;
    vec se(maxRank);
    CSI_Feedback_Info_S csiFeedbackInfo;
    csiFeedbackInfo.cqiPerSubbandPerCW.set_size(nSubbands);
    csiFeedbackInfo.specEffPerSubband.set_size(nSubbands);
    csiFeedbackInfo.precoderPerSubband.set_size(nSubbands);
    
    Array<Array<CSI_Info_S>> cqiInfoPerRankPerSB(8);
    Array<Array<cmat>> precoderPerRankPerSB(maxRank);
    for(int rankIndx = 0;rankIndx < se.length();rankIndx++){
//         cout<<endl<<"rank Index: "<<rankIndx<<endl;
        int rank = rankIndx+1;
        int nCodeWords = (rank>4) ? 2 : 1;
        cqiInfoPerRankPerSB(rankIndx).set_length(nSubbands);
        precoderPerRankPerSB(rankIndx).set_length(nSubbands);
        
        //SD Basis Calculation

        Type2coefficients_N_OMP_S type2Coefficients;
        int Lbeams = rank;
        int nTx = idealChannel(0).cols();
        int selectedSet = -1;
        int N1=codebookConfig.N1, N2=codebookConfig.N2;
        int O1=4,O2=(N2==1) ? 1:4;
        Array<ivec> i11_IndicesPerSet(O1*O2);  // Stores corresponding i11 index of the beams in set
        Array<ivec> i12_IndicesPerSet(O1*O2);  // Stores corresponding i12 index of the beams in set
        Array<cmat> orthBeamsTotalSets = type2OrthogonalBeamsSet(N1, N2, i11_IndicesPerSet, i12_IndicesPerSet); // Generating orthogonal beam sets
        ivec indices;
        
        cmat avgChannelCovar = avgCovariance(idealChannel);
            
        cmat U,V;  vec S; cvec eigenValues;
        bool isSuccess = eig(avgChannelCovar, eigenValues, V);
//         cout<<endl<<"eigenValues Rel 19: "<<abs(eigenValues)<<endl;
        V = V.get_cols(reverse(sort_index(abs(eigenValues))));
        cmat normMatrix = V.H()*V;
        cvec normVec = diag(normMatrix);
        V = inv(diag(normVec))*V; //Normalising individual singular vector with its norm. 
        selectedSet = selectSetWithChannelCovariance(orthBeamsTotalSets,avgChannelCovar,Lbeams,indices); // Beamset Calculation
//         cout<<endl<<"selected set 19: "<<selectedSet<<endl;
//         cout<<endl<<"indices: "<<indices<<endl;
        cmat temp1 = orthBeamsTotalSets(selectedSet).H()*avgChannelCovar(0,nTx/2-1,0,nTx/2-1)*orthBeamsTotalSets(selectedSet);
        cmat temp2 = orthBeamsTotalSets(selectedSet).H()*avgChannelCovar(nTx/2,nTx-1,nTx/2,nTx-1)*orthBeamsTotalSets(selectedSet);
        cmat temp = temp1+temp2;
        vec beamStrength;
        beamStrength=abs(diag(temp));
        ivec BeamIndices = reverse(sort_index(beamStrength));
        ivec finalBeamIndices=BeamIndices(0,rankIndx);
        cmat beams=orthBeamsTotalSets(selectedSet).get_cols(finalBeamIndices);
//         cout<<endl<<"Rel 19 Beams: "<<endl<<beams<<endl;
        // Including the chance to pick the layer 1 beam for remaining layers
        cmat channeleigen=V.get_cols(0,rankIndx);
//         cout<<endl<<"channle Dim: "<<channeleigen.rows()<<" "<<channeleigen.cols()<<endl;
        map<int, int> index_count;  // Dictonary key:coloumn number and pair:number of times coloumn repeats
        ivec selected_columns(rank); // Stores selected column indices
        cmat bestBeams;
        
        for(int i=0;i<channeleigen.cols();i++){
            cmat tempstrngth1=beams.H()*channeleigen(0,nTx/2-1,i,i);
            cmat tempstrngth2=beams.H()*channeleigen(nTx/2,nTx-1,i,i);
            mat tempstrngth=abs(tempstrngth1)+abs(tempstrngth2);
            vec dumm(tempstrngth.rows());
            for(int j=0;j<tempstrngth.rows();j++)
                dumm(j)=abs(tempstrngth(j,0));
//             cout<<endl<<"i: "<<i<<endl<<"dumm: "<<dumm<<endl;
            ivec sorted_indices = reverse(sort_index(dumm));
//             cout<<endl<<"sorted_indices: "<<sorted_indices<<endl;
        // Select the first available column that has been picked at most 2 times
            for (int j = 0; j < sorted_indices.length(); j++) {
                int idx = sorted_indices[j];
//                 cout<<endl<<"idx: "<<idx<<endl;
                if (index_count[idx] < 2) {
                    selected_columns[i] = idx;
                    index_count[idx]++;
                    break;
                }
            }
            bestBeams.append_col(beams.get_col(selected_columns(i)));
        }
//         cout<<endl<<"selected_columns: "<<selected_columns<<endl;
        Array<vec> sinrWideband(nCodeWords);
        for(int sbIndx = 0;sbIndx < nSubbands;sbIndx++){
            ivec scIndicesWithinSubband;
            scIndicesWithinSubband = getIntegers(0,subbandSize-1)+(sbIndx*subbandSize);
            Array<cmat> sbChannel = getFromArray(idealChannel,scIndicesWithinSubband);
            cmat avgSBChannelCovar = avgCovariance(sbChannel);
            cmat U,V;  vec S; cvec eigenValues;
            bool isSuccess = eig(avgSBChannelCovar, eigenValues, V);
            V = V.get_cols(reverse(sort_index(abs(eigenValues))));
            cmat normMatrix = V.H()*V;
            cvec normVec = diag(normMatrix);
            V = inv(diag(normVec))*V;
           
            Array<vec> metric(rank);
            ivec coPhaseIndxRel19(rank);
                                 
            map<int, int> beamUsage;  // key:beam index Pair:cophase coloumn index
            cmat precoder;

            for (int rni = 0; rni < rank; rni++) {
                metric(rni).set_length(coPhase.cols());

                for (int coPhaseIndx = 0; coPhaseIndx < coPhase.cols(); coPhaseIndx++) {
                    cmat tempPrecoder = kron(coPhase.get_cols(coPhaseIndx, coPhaseIndx), bestBeams.get_cols(rni, rni));
                    cmat temp = tempPrecoder.H() * V.get_cols(0, 0);
                    metric(rni)(coPhaseIndx) = abs(temp(0, 0));
                }

                coPhaseIndxRel19(rni) = max_index(metric(rni));

                // Extract beam index (assuming a valid method to get it)
                int beamIndex = selected_columns(rni);

                // Check if beam is already used
                if (beamUsage.find(beamIndex) == beamUsage.end()) {
                    // if that key is not present in dictionary it will return beamUsage.end()
                    beamUsage[beamIndex] = coPhaseIndxRel19(rni);
                }
                else {
                    // Key is not present means same beam is using twice
                    int firstCoPhaseIndx = beamUsage[beamIndex];
                    // Addign Pi to the first occurance of the beam cophase through the cophase table
                    coPhaseIndxRel19 = (firstCoPhaseIndx + coPhase.cols() / 2) % coPhase.cols();
                }
                cmat layerPrecoder = kron(coPhase.get_cols(coPhaseIndxRel19(rni), coPhaseIndxRel19(rni)), bestBeams.get_cols(rni, rni));
//                 if(sbIndx==0){
//                     cout<<"Rel 19 Cophase: "<<endl<<coPhase.get_cols(coPhaseIndxRel19(rni), coPhaseIndxRel19(rni))<<endl;
//                 }
                precoder = concat_horizontal(precoder, layerPrecoder);
            }

            precoderPerRankPerSB(rankIndx)(sbIndx) =  1/sqrt(rank)*precoder;
            
            Array<vec> sinrPerSB = calculateSinrPerCodewordNR(rank,sbChannel,precoderPerRankPerSB(rankIndx)(sbIndx),1.0,IntrCov,_DEMODULATION_SCHEME_MMSE_);
             // if(sbIndx==0)
            sinrWideband(0) = concat(sinrWideband(0),sinrPerSB(0));
            if(rank > 4){sinrWideband(1) = concat(sinrWideband(1),sinrPerSB(1));}
            
            double seTemp = computeCQI(cqiInfoPerRankPerSB(rankIndx)(sbIndx),sinrPerSB,rank,&tbTables,90,subbandSize);
        }
        CSI_Info_S cqiInfo;
        se(rankIndx) = computeCQI(cqiInfo,sinrWideband,rank,&tbTables,90,dlRBs);
//         cout<<endl<<"SE Scheme B: "<<se(rankIndx)<<endl;

    }
    
    int selectedRankIndx = max_index(se);
    csiFeedbackInfo.specEffWideband = se(selectedRankIndx);
    csiFeedbackInfo.nLayers = selectedRankIndx+1;
    for(int sbIndx = 0;sbIndx < nSubbands;sbIndx++){
        csiFeedbackInfo.cqiPerSubbandPerCW(sbIndx) = cqiInfoPerRankPerSB(selectedRankIndx)(sbIndx).CQIindexPerCW;
        csiFeedbackInfo.precoderPerSubband(sbIndx) = precoderPerRankPerSB(selectedRankIndx)(sbIndx);
        csiFeedbackInfo.specEffPerSubband(sbIndx) = cqiInfoPerRankPerSB(selectedRankIndx)(sbIndx).specEff;
    }
    return csiFeedbackInfo;
}
    
CSI_Feedback_Info_S computeSEforType1Simplifed(CodeBookConfig_S codebookConfig, Array<cmat> idealChannel, cmat IntrCov, cTBTables tbTables, int dlRBs, int subbandSize, bool isR15, int maxRank){
    
    cmat temp = "1 0;0 -1";
    cmat coPhase = "1+0i 1+0i 1+0i 1+0i;1+0i 0+1i -1+0i 0-1i";
    int nTx = idealChannel(0).cols();
    cmat avgChannelCovar = avgCovariance(idealChannel);
    int nSubbands = dlRBs/subbandSize;
    cmat U,V;  vec S; cvec eigenValues;
        bool isSuccess = eig(avgChannelCovar, eigenValues, V);
//         cout<<endl<<"eigenValues Rel 15: "<<abs(eigenValues)<<endl;
    vec se(maxRank);
    CSI_Feedback_Info_S csiFeedbackInfo;
    csiFeedbackInfo.cqiPerSubbandPerCW.set_size(nSubbands);
    csiFeedbackInfo.specEffPerSubband.set_size(nSubbands);
    csiFeedbackInfo.precoderPerSubband.set_size(nSubbands);
    
    Array<Array<CSI_Info_S>> cqiInfoPerRankPerSB(8);
    Array<Array<cmat>> precoderPerRankPerSB(8);
    for(int rankIndx = 0;rankIndx < se.length();rankIndx++){
        int rank = rankIndx+1;
        int nCodeWords = (rank>4) ? 2 : 1;
        cqiInfoPerRankPerSB(rankIndx).set_length(nSubbands);
        precoderPerRankPerSB(rankIndx).set_length(nSubbands);
        
        // Selecting SD Basis
        cmat beams = getBeamsForR15Type1SP(codebookConfig,rank,avgChannelCovar);
     //  cout<<"TypeI beam : "<<beams<<endl;
        Array<vec> sinrWideband(nCodeWords);
        vec sey(nSubbands);
        for(int sbIndx = 0;sbIndx < nSubbands;sbIndx++){
            ivec scIndicesWithinSubband;
            scIndicesWithinSubband = getIntegers(0,subbandSize-1)+(sbIndx*subbandSize);
            Array<cmat> sbChannel = getFromArray(idealChannel,scIndicesWithinSubband);
            cmat avgSBChannelCovar = avgCovariance(sbChannel);
            cmat U,V;  vec S; cvec eigenValues;
            bool isSuccess = eig(avgSBChannelCovar, eigenValues, V);
            V = V.get_cols(reverse(sort_index(abs(eigenValues))));
            cmat normMatrix = V.H()*V;
            cvec normVec = diag(normMatrix);
            V = inv(diag(normVec))*V;
            
            vec metric(4);
            for(int coPhaseIndx = 0;coPhaseIndx < coPhase.cols();coPhaseIndx++){
                cmat tempPrecoder = kron(coPhase.get_cols(coPhaseIndx,coPhaseIndx),beams.get_cols(0,0));
                cmat temp = tempPrecoder.H()*V.get_cols(0,0);
                
                metric(coPhaseIndx) = abs(temp(0,0));
            }
//             if(sbIndx==0)
//             cout<<endl<<"metric: "<<metric<<endl;
           // cout<<endl<<"max metric: "<<max(metric)<<endl;
            cmat coPhaseBeam1 = coPhase.get_cols(max_index(metric),max_index(metric));
//              cout<<endl<<"coPhaseBeam: "<<endl<<coPhaseBeam1<<endl;
            precoderPerRankPerSB(rankIndx)(sbIndx) = getR15Type1SPPrecoder(beams,coPhaseBeam1,rank);
          
     
//                                 cout<<endl<<"Type I Norm: "<<norm(precoderPerRankPerSB(rankIndx)(0),"fro")<<endl;

            
            Array<vec> sinrPerSB = calculateSinrPerCodewordNR(rank,sbChannel,precoderPerRankPerSB(rankIndx)(sbIndx),1.0,IntrCov,_DEMODULATION_SCHEME_MMSE_);
//               if(sbIndx==0)
             //   cout<<endl<<"SINR TypeI: "<<sinrPerSB(0)<<endl;
            sinrWideband(0) = concat(sinrWideband(0),sinrPerSB(0));
            if(rank > 4){sinrWideband(1) = concat(sinrWideband(1),sinrPerSB(1));}
            
            double seTemp = computeCQI(cqiInfoPerRankPerSB(rankIndx)(sbIndx),sinrPerSB,rank,&tbTables,90,subbandSize);
            sey(sbIndx)=seTemp;
        }
//          cout<<endl<<"sey Type I: "<<sey<<endl<<"mean: "<<mean(sey)<<endl;
      // cout<<endl<<"Precoder Type I: "<<endl<<precoderPerRankPerSB(rankIndx)(0)<<endl;
        CSI_Info_S cqiInfo;
        se(rankIndx) = computeCQI(cqiInfo,sinrWideband,rank,&tbTables,90,dlRBs);
        
//          cout<<endl<<"Spec Eff TypeI: "<<se(rankIndx)<<endl;
        //if(rank==1)
        //cout<<endl<<"sinr: "<<sinrWideband<<endl;
    }
    int selectedRankIndx = max_index(se);
    csiFeedbackInfo.specEffWideband = se(selectedRankIndx);
    csiFeedbackInfo.nLayers = selectedRankIndx+1;
    for(int sbIndx = 0;sbIndx < nSubbands;sbIndx++){
        csiFeedbackInfo.cqiPerSubbandPerCW(sbIndx) = cqiInfoPerRankPerSB(selectedRankIndx)(sbIndx).CQIindexPerCW;
        csiFeedbackInfo.precoderPerSubband(sbIndx) = precoderPerRankPerSB(selectedRankIndx)(sbIndx);
        csiFeedbackInfo.specEffPerSubband(sbIndx) = cqiInfoPerRankPerSB(selectedRankIndx)(sbIndx).specEff;
    }
    return csiFeedbackInfo;
}

CSI_Feedback_Info_S computeSEforSinglePort(Array<cmat> idealChannel, cmat IntrCov, cTBTables tbTables, int dlRBs, int subbandSize){

    int nTx = idealChannel(0).cols();
    int maxRank = 1;
    cmat avgChannelCovar = avgCovariance(idealChannel);
    int nSubbands = dlRBs/subbandSize;
    vec se(maxRank);
    CSI_Feedback_Info_S csiFeedbackInfo;
    csiFeedbackInfo.cqiPerSubbandPerCW.set_size(nSubbands);
    csiFeedbackInfo.specEffPerSubband.set_size(nSubbands);
    csiFeedbackInfo.precoderPerSubband.set_size(nSubbands);

    Array<Array<CSI_Info_S>> cqiInfoPerRankPerSB(8);
    Array<Array<cmat>> precoderPerRankPerSB(8);
    for(int rankIndx = 0;rankIndx < se.length();rankIndx++){
        int rank = rankIndx+1;
        int nCodeWords = (rank>4) ? 2 : 1;
        cqiInfoPerRankPerSB(rankIndx).set_length(nSubbands);
        precoderPerRankPerSB(rankIndx).set_length(nSubbands);

     //  cout<<"TypeI beam : "<<beams<<endl;
        Array<vec> sinrWideband(nCodeWords);
        vec sey(nSubbands);
        for(int sbIndx = 0;sbIndx < nSubbands;sbIndx++){
            ivec scIndicesWithinSubband;
            scIndicesWithinSubband = getIntegers(0,subbandSize-1)+(sbIndx*subbandSize);
            Array<cmat> sbChannel = getFromArray(idealChannel,scIndicesWithinSubband);
            cmat avgSBChannelCovar = avgCovariance(sbChannel);
            precoderPerRankPerSB(rankIndx)(sbIndx) = ones_c(1,1);
            Array<vec> sinrPerSB = calculateSinrPerCodewordNR(rank,sbChannel,precoderPerRankPerSB(rankIndx)(sbIndx),1.0,IntrCov,_DEMODULATION_SCHEME_MMSE_);
            sinrWideband(0) = concat(sinrWideband(0),sinrPerSB(0));
            if(rank > 4){sinrWideband(1) = concat(sinrWideband(1),sinrPerSB(1));}

            double seTemp = computeCQI(cqiInfoPerRankPerSB(rankIndx)(sbIndx),sinrPerSB,rank,&tbTables,90,subbandSize);
            sey(sbIndx)=seTemp;
        }
        CSI_Info_S cqiInfo;
        se(rankIndx) = computeCQI(cqiInfo,sinrWideband,rank,&tbTables,90,dlRBs);
    }
    int selectedRankIndx = max_index(se);
    csiFeedbackInfo.specEffWideband = se(selectedRankIndx);
    csiFeedbackInfo.nLayers = selectedRankIndx+1;
    for(int sbIndx = 0;sbIndx < nSubbands;sbIndx++){
        csiFeedbackInfo.cqiPerSubbandPerCW(sbIndx) = cqiInfoPerRankPerSB(selectedRankIndx)(sbIndx).CQIindexPerCW;
        csiFeedbackInfo.precoderPerSubband(sbIndx) = precoderPerRankPerSB(selectedRankIndx)(sbIndx);
        csiFeedbackInfo.specEffPerSubband(sbIndx) = cqiInfoPerRankPerSB(selectedRankIndx)(sbIndx).specEff;
    }
    return csiFeedbackInfo;
}

int getBeamIndxWithOffset(int refBeamIndx, int k1, int k2, int N1, int N2){
    int newBeam;
    int n1refBeam = refBeamIndx/N2;int n2refBeam = mod(refBeamIndx,N2);
    int n1newBeam = mod(n1refBeam+k1,N1);
    int n2newBeam = mod(n2refBeam+k2,N2);
    newBeam = (n1newBeam*N2)+n2newBeam;
    return newBeam;
}


cmat getBeamsForR15Type1SP(CodeBookConfig_S codebookConfig, int rank, cmat avgChannelCovar){
    
    cmat beams;ivec finalBeamIndices;int selectedBeamSet;
    ivec k1 = "0,1,0,2";
    ivec k2 = "0,0,1,0";
//     ivec k1 = "";
//     ivec k2 = "";
//     for(int i = 0;i < 8;i++){
//         k1 = concat(k1,ones_i(8)*i);
//        k2 = concat(k2,getIntegers(0,7));
//    }
    int N1=codebookConfig.N1, N2=codebookConfig.N2;int O1=4,O2=(N2==1) ? 1:4;
    int nTx = 2*N1*N2;
    Array<ivec> i11_IndicesPerSet(O1*O2);
    Array<ivec> i12_IndicesPerSet(O1*O2);
    Array<cmat> orthBeamsTotalSets = type2OrthogonalBeamsSet(N1, N2, i11_IndicesPerSet, i12_IndicesPerSet);
    
    Array<vec> beamStrengthPerBeamSet(orthBeamsTotalSets.length());
    for(int beamSetIndx = 0;beamSetIndx < orthBeamsTotalSets.length();beamSetIndx++){
        cmat temp1 = orthBeamsTotalSets(beamSetIndx).H()*avgChannelCovar(0,nTx/2-1,0,nTx/2-1)*orthBeamsTotalSets(beamSetIndx);
        cmat temp2 = orthBeamsTotalSets(beamSetIndx).H()*avgChannelCovar(nTx/2,nTx-1,nTx/2,nTx-1)*orthBeamsTotalSets(beamSetIndx);
        cmat temp = temp1+temp2;
        beamStrengthPerBeamSet(beamSetIndx) = abs(diag(temp));
    }
    if(rank == 1){
        vec metric(orthBeamsTotalSets.length());
        ivec bestBeamsPerBeamSet(orthBeamsTotalSets.length());
        for(int beamSetIndx = 0;beamSetIndx < orthBeamsTotalSets.length();beamSetIndx++){
            metric(beamSetIndx) = max(beamStrengthPerBeamSet(beamSetIndx));
            bestBeamsPerBeamSet(beamSetIndx) = max_index(beamStrengthPerBeamSet(beamSetIndx));
        }
        selectedBeamSet = max_index(metric);
        
        finalBeamIndices = to_ivec(bestBeamsPerBeamSet(selectedBeamSet));
        
//          cout<<"selectedBeamSet Type I : "<<selectedBeamSet<<endl;
//            cout<<"finalBeamIndices Type I: "<<finalBeamIndices<<endl;
//       cout<<endl<<"beamsreengths: "<<beamStrengthPerBeamSet(selectedBeamSet)<<endl<<"Indices: "<<metric<<endl ;
        
    }
    if((rank == 2)||(rank == 3)||(rank == 4)){
        vec metric(orthBeamsTotalSets.length());
        Array<ivec> bestBeamsPerBeamSet(orthBeamsTotalSets.length());
        for(int beamSetIndx = 0;beamSetIndx < orthBeamsTotalSets.length();beamSetIndx++){
            int beam1Indx = max_index(beamStrengthPerBeamSet(beamSetIndx));
            append(bestBeamsPerBeamSet(beamSetIndx),beam1Indx);
            metric(beamSetIndx) = max(beamStrengthPerBeamSet(beamSetIndx));
            vec beam2Metric = zeros(k1.length());
            ivec beam2Indices(k1.length());
            for(int k1Indx = 0;k1Indx < k1.length();k1Indx++){
                beam2Indices(k1Indx) = getBeamIndxWithOffset(beam1Indx,k1(k1Indx),k2(k1Indx),N1,N2);
                if(beam2Indices(k1Indx) == beam1Indx){
                    beam2Metric(k1Indx) = metric(beamSetIndx)*0;  //0.25 is selected randomly
                }
                else{
                    beam2Metric(k1Indx) = beamStrengthPerBeamSet(beamSetIndx)(beam2Indices(k1Indx));
                }
            }
//             cout<<endl<<"beam2Indices(k1Indx): "<<beam2Indices<<endl<<"beam2Metric: "<<beam2Metric<<endl;
            metric(beamSetIndx) = metric(beamSetIndx)+max(beam2Metric);

            append(bestBeamsPerBeamSet(beamSetIndx),beam2Indices(max_index(beam2Metric)));
        }
        selectedBeamSet = max_index(metric);
        finalBeamIndices = bestBeamsPerBeamSet(selectedBeamSet);
//         cout<<"selectedBeamSet 2: "<<selectedBeamSet<<endl;
//          cout<<"finalBeamIndices2 : "<<finalBeamIndices<<endl;
    }
    
    if((rank == 5)||(rank == 6)){
        vec metric(orthBeamsTotalSets.length());
        Array<ivec> bestBeamsPerBeamSet(orthBeamsTotalSets.length());
        for(int beamSetIndx = 0;beamSetIndx < orthBeamsTotalSets.length();beamSetIndx++){
            int beam1Indx, beam2Indx, beam3Indx;
            beam1Indx = max_index(beamStrengthPerBeamSet(beamSetIndx));        
            append(bestBeamsPerBeamSet(beamSetIndx),beam1Indx);
            beam2Indx = getBeamIndxWithOffset(beam1Indx,1,0,N1,N2);
            append(bestBeamsPerBeamSet(beamSetIndx),beam2Indx);
            beam3Indx = getBeamIndxWithOffset(beam1Indx,1,1,N1,N2);
            append(bestBeamsPerBeamSet(beamSetIndx),beam3Indx);
            metric(beamSetIndx) = sum(beamStrengthPerBeamSet(beamSetIndx)(bestBeamsPerBeamSet(beamSetIndx)));
        }
        selectedBeamSet = max_index(metric);
        finalBeamIndices = bestBeamsPerBeamSet(selectedBeamSet);
    }
    
    if((rank == 7)||(rank == 8)){
        vec metric(orthBeamsTotalSets.length());
        Array<ivec> bestBeamsPerBeamSet(orthBeamsTotalSets.length());
        for(int beamSetIndx = 0;beamSetIndx < orthBeamsTotalSets.length();beamSetIndx++){
            int beam1Indx, beam2Indx, beam3Indx, beam4Indx;
            beam1Indx = max_index(beamStrengthPerBeamSet(beamSetIndx));        
            append(bestBeamsPerBeamSet(beamSetIndx),beam1Indx);
            beam2Indx = getBeamIndxWithOffset(beam1Indx,1,0,N1,N2);
            append(bestBeamsPerBeamSet(beamSetIndx),beam2Indx);
            beam3Indx = getBeamIndxWithOffset(beam1Indx,0,1,N1,N2);
            append(bestBeamsPerBeamSet(beamSetIndx),beam3Indx);
            beam4Indx = getBeamIndxWithOffset(beam1Indx,1,1,N1,N2);
            append(bestBeamsPerBeamSet(beamSetIndx),beam4Indx);
            metric(beamSetIndx) = sum(beamStrengthPerBeamSet(beamSetIndx)(bestBeamsPerBeamSet(beamSetIndx)));
        }
        selectedBeamSet = max_index(metric);
        finalBeamIndices = bestBeamsPerBeamSet(selectedBeamSet);
    }
    beams = orthBeamsTotalSets(selectedBeamSet).get_cols(finalBeamIndices);
    return beams;
}


CSI_Feedback_Info_S doLinkAdaptationforTypeII(Array<cmat>& estimatedChannelPerRB,Type2coefficients_N_OMP_S& finalCoefficients, int dlNRBs, int prgSize, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,int& nLayers,ivec &cqi)
{
    int nSubbands = dlNRBs/prgSize;
    int subbandSize = prgSize;
    CSI_Feedback_Info_S csiFeedbackInfo;
    csiFeedbackInfo.cqiPerSubbandPerCW.set_size(nSubbands);
    csiFeedbackInfo.specEffPerSubband.set_size(nSubbands);
    csiFeedbackInfo.precoderPerSubband.set_size(nSubbands);
    
    vec specEffPerRank = zeros(maxRank);
    Array<Type2coefficients_N_OMP_S> type2Coefficients(maxRank);
    Array<ivec> cqiPerRank(maxRank);
    
    Array<Array<CSI_Info_S>> cqiInfoPerRankPerSB(maxRank);
    Array<Array<cmat>> precoderPerRankPerSB(maxRank);
    for(int rank=1;rank<=maxRank;rank++)
    {
//         cout<<endl<<"-----------RANK = "<<rank<<"-----------"<<endl;
        cqiInfoPerRankPerSB(rank-1).set_length(nSubbands);
        precoderPerRankPerSB(rank-1).set_length(nSubbands);
        switch(codebook.dlCodeBookConfig.codeBookType)
        {
            case _NR_CODEBOOK_TYPE_II_:
            case _NR_CODEBOOK_TYPE_II_ENHANCED_:
            case _NR_CODEBOOK_TYPE_II_ENHANCED_CJT_:
            case _NR_CODEBOOK_PORT_SELECTION_:
            case _NR_CODEBOOK_PORT_SELECTION_ENHANCED_:
            case _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_:
                type2Coefficients(rank-1) = codebook.getType2OMPCoefficients(estimatedChannelPerRB,nSubbands,subbandSize,1,rank);
                break;
            default:
                cout<<"Invalid Type II Codebook Type."<<endl;abort();
        }
        Array<vec> avgSinrPerCW(nCodeWords);
        vec sey(nSubbands);
        for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++)
        {
            Array<cmat> currentRawChannel = estimatedChannelPerRB(subbandSize*sb_cnt,(sb_cnt+1)*subbandSize-1);
            precoderPerRankPerSB(rank-1)(sb_cnt) = type2Coefficients(rank-1).precoderPerSubband(sb_cnt);
           
            Array<vec> sinrPerCW = calculateSinrPerCodewordNR(rank,currentRawChannel,precoderPerRankPerSB(rank-1)(sb_cnt),1.0,intCov,_DEMODULATION_SCHEME_MMSE_);
             //   cout<<endl<<"SINR TypeII: "<<sinrPerCW(0)<<endl;
            double seTemp = computeCQI(cqiInfoPerRankPerSB(rank-1)(sb_cnt),sinrPerCW,rank,&tbTables,90,subbandSize);
            sey(sb_cnt)=seTemp;
            for(int cw_cnt=0;cw_cnt<nCodeWords;cw_cnt++)
                append(avgSinrPerCW(cw_cnt),sinrPerCW(cw_cnt));
        }
//         cout<<endl<<"se: "<<sey<<endl<<"mean: "<<mean(sey)<<endl;
          //cout<<endl<<"Precoder Type II: "<<endl<<precoderPerRankPerSB(rank-1)(0)<<endl;
//          cout<<endl<<"Preocer norm: "<<norm(precoderPerRankPerSB(rank-1)(0))<<endl;
        CSI_Info_S cqiInfo;
        specEffPerRank(rank-1) = computeCQI(cqiInfo,avgSinrPerCW,rank,&tbTables,90,nSubbands*subbandSize);
//          cout<<endl<<"Spec Eff Per Rank Type II: "<<specEffPerRank(rank-1)<<endl;
        //if(rank==1)
        //cout<<endl<<"avgSinrPerCW: "<<avgSinrPerCW<<endl;
        cqiPerRank(rank-1) = cqiInfo.CQIindexPerCW;
        
    }
    
    nLayers = max_index(specEffPerRank)+1;
    finalCoefficients = type2Coefficients(nLayers-1);
    cqi = cqiPerRank(nLayers-1);
    
    int selectedRankIndx = max_index(specEffPerRank);
    csiFeedbackInfo.specEffWideband = specEffPerRank(selectedRankIndx);
    csiFeedbackInfo.nLayers = selectedRankIndx+1;
    for(int sbIndx = 0;sbIndx < nSubbands;sbIndx++){
        csiFeedbackInfo.cqiPerSubbandPerCW(sbIndx) = cqiInfoPerRankPerSB(selectedRankIndx)(sbIndx).CQIindexPerCW;
        csiFeedbackInfo.precoderPerSubband(sbIndx) = precoderPerRankPerSB(selectedRankIndx)(sbIndx);
        csiFeedbackInfo.specEffPerSubband(sbIndx) = cqiInfoPerRankPerSB(selectedRankIndx)(sbIndx).specEff;
    }
    
    return csiFeedbackInfo;
}

double doLinkAdaptationforTypeIMultiTRPNCJT(Array<Array<cmat>>& estimatedChannelPerTRP, int dlNRBs, int prgSize, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,Array<type1PMI_S>& finalPMIIndices,ivec& nLayersPerTRP,ivec &cqi)
{
    int nSubbands = dlNRBs/prgSize;
    int subbandSize = prgSize;
    int nCSIRSPorts = codebook.dlCodeBookConfig.nCSIRSPorts;
    int nTRPs = estimatedChannelPerTRP.length();
    
    Array<Array<type1PMI_S>> type1PMIIndices(maxRank);
    
    for(int rank=1;rank<=maxRank;rank++)
    {
        type1PMIIndices(rank-1).set_length(nTRPs);
        double specEff;
        for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
        {
            Array<cmat> codeBook = codebook.getCodebook(codebook.dlCodeBookConfig.nCSIRSPorts,rank,1);
            {
                    int nREsPerRB = 2*12; //Two OFDM symbols in every RB
                    Array<cmat> codeBook = codebook.getCodebook(codebook.dlCodeBookConfig.nCSIRSPorts,rank,1);
                    CSI_Info_S cqiInfo = computeBestPMIAndCQI(estimatedChannelPerTRP(trp_cnt),codeBook,&tbTables,intCov,subbandSize,nSubbands,nREsPerRB,_DEMODULATION_SCHEME_MMSE_);
                    type1PMIIndices(rank-1)(trp_cnt) = codebook.getPMIIndices(rank,cqiInfo.PMI);
                    
                    ivec i2Indices = codebook.geti2CmatIndicesIn_W(type1PMIIndices(rank-1)(trp_cnt),rank);
                    codeBook = getFromArray(codeBook,i2Indices);
                    type1PMIIndices(rank-1)(trp_cnt).i2Indices.set_size(nSubbands);
                    Array<vec> avgSinrPerCW(nCodeWords);
                    for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++)
                    {
                        Array<cmat> currentRawChannel = estimatedChannelPerTRP(trp_cnt)(subbandSize*sb_cnt,(sb_cnt+1)*subbandSize-1);
                        CSI_Info_S cqiInfo = computeBestPMIAndCQI(currentRawChannel,codeBook,&tbTables,intCov,subbandSize,1,nREsPerRB,_DEMODULATION_SCHEME_MMSE_);
                        type1PMIIndices(rank-1)(trp_cnt).i2Indices(sb_cnt) = cqiInfo.PMI;
                        for(int cw_cnt=0;cw_cnt<nCodeWords;cw_cnt++)
                            append(avgSinrPerCW(cw_cnt),cqiInfo.sinrPerCodeword(cw_cnt));
                    }
                    specEff = computeCQI(cqiInfo,avgSinrPerCW,rank,&tbTables,nREsPerRB,nSubbands*subbandSize); 
                    
            }
        }
    }
    
    imat possibleRankPerTRP = computePermutations(maxRank,nTRPs)+1;
    int nComb = possibleRankPerTRP.rows();
    
    vec specEffPerRankCombination = zeros(nComb);
    Array<ivec> cqiPerComb(nComb);
    
    for(int comb_cnt=0;comb_cnt<nComb;comb_cnt++)
    {
        Array<vec> avgSinrPerCW(nCodeWords);
        for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++)
        {
            Array<cmat> currentRawChannel = estimatedChannelPerTRP(0)(subbandSize*sb_cnt,(sb_cnt+1)*subbandSize-1);
            cmat precoder = codebook.getPrecoder(nCSIRSPorts,possibleRankPerTRP(comb_cnt,0),type1PMIIndices(possibleRankPerTRP(comb_cnt,0)-1)(0).i2Indices(sb_cnt));
            
            // Forming the composite channel and precoder for the subband. 
            for(int trp_cnt=1;trp_cnt<nTRPs;trp_cnt++)
            {
                Array<cmat> rawChannel = estimatedChannelPerTRP(trp_cnt)(subbandSize*sb_cnt,(sb_cnt+1)*subbandSize-1);
                for(int tone_cnt=0;tone_cnt<rawChannel.length();tone_cnt++)
                {
                    currentRawChannel(tone_cnt) = concat_horizontal(currentRawChannel(tone_cnt),rawChannel(tone_cnt));
                }
                cmat tempPrecoder = codebook.getPrecoder(nCSIRSPorts,possibleRankPerTRP(comb_cnt,trp_cnt),type1PMIIndices(possibleRankPerTRP(comb_cnt,trp_cnt)-1)(trp_cnt).i2Indices(sb_cnt));
                
                cmat N = zeros_c(trp_cnt*nCSIRSPorts,possibleRankPerTRP(comb_cnt,trp_cnt));
                cmat M = zeros_c(nCSIRSPorts,sum(possibleRankPerTRP.get_row(comb_cnt)(getIntegers(0,trp_cnt-1))));
                precoder = concat_horizontal(precoder,N);
                precoder = concat_vertical(precoder,concat_horizontal(M,tempPrecoder));
            }
            
            Array<vec> sinrPerCW = calculateSinrPerCodewordNR(sum(possibleRankPerTRP.get_row(comb_cnt)),currentRawChannel,precoder,1.0,intCov,_DEMODULATION_SCHEME_MMSE_);
            for(int cw_cnt=0;cw_cnt<nCodeWords;cw_cnt++)
                append(avgSinrPerCW(cw_cnt),sinrPerCW(cw_cnt));
        }
        CSI_Info_S cqiInfo; 
        int nREsPerRB = 2*12; //Two OFDM symbols in every RB
        specEffPerRankCombination(comb_cnt) = computeCQI(cqiInfo,avgSinrPerCW,sum(possibleRankPerTRP.get_row(comb_cnt)),&tbTables,nREsPerRB,nSubbands*subbandSize);
        cqiPerComb(comb_cnt) = cqiInfo.CQIindexPerCW;
        
    }
    int maxComb = max_index(specEffPerRankCombination);
    nLayersPerTRP = possibleRankPerTRP.get_row(maxComb);
    finalPMIIndices.set_length(nTRPs);
    for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
    {
        finalPMIIndices(trp_cnt) = type1PMIIndices(nLayersPerTRP(trp_cnt)-1)(trp_cnt);
    }
    cqi = cqiPerComb(maxComb);
    return specEffPerRankCombination(maxComb);
}


double doLinkAdaptationforTypeIIMultiTRPNCJT(Array<Array<cmat>>& estimatedChannelPerTRP,Array<Type2coefficients_N_OMP_S>& finalCoefficients, int dlNRBs, int prgSize, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,ivec& nLayersPerTRP,ivec &cqi)
{
    int nSubbands = dlNRBs/prgSize;
    int subbandSize = prgSize;
    int nCSIRSPorts = codebook.dlCodeBookConfig.nCSIRSPorts;
    int nTRPs = estimatedChannelPerTRP.length();
    
    Array<Array<Type2coefficients_N_OMP_S>> type2Coefficients(maxRank);
    
    for(int rank=1;rank<=maxRank;rank++)
    {
        type2Coefficients(rank-1).set_length(nTRPs);
        for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
        {
            switch(codebook.dlCodeBookConfig.codeBookType)
            {
                case _NR_CODEBOOK_TYPE_II_:
                case _NR_CODEBOOK_TYPE_II_ENHANCED_:
                case _NR_CODEBOOK_PORT_SELECTION_:
                case _NR_CODEBOOK_PORT_SELECTION_ENHANCED_:
                case _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_:
                    type2Coefficients(rank-1)(trp_cnt) = codebook.getType2OMPCoefficients(estimatedChannelPerTRP(trp_cnt),nSubbands,subbandSize,1,rank);
                    break;
                default:
                    cout<<"Invalid CodebookType"<<endl;abort();
            }
        }
    }
    
    imat possibleRankPerTRP = computePermutations(maxRank,nTRPs)+1;
    int nComb = possibleRankPerTRP.rows();
    
    vec specEffPerRankCombination = zeros(nComb);
    
    Array<ivec> cqiPerComb(nComb);
    
    for(int comb_cnt=0;comb_cnt<nComb;comb_cnt++)
    {
        Array<vec> avgSinrPerCW(nCodeWords);
        for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++)
        {
            Array<cmat> currentRawChannel = estimatedChannelPerTRP(0)(subbandSize*sb_cnt,(sb_cnt+1)*subbandSize-1);
            cmat precoder = type2Coefficients(possibleRankPerTRP(comb_cnt,0)-1)(0).precoderPerSubband(sb_cnt);
            
            // Forming the composite channel and precoder for the subband. 
            for(int trp_cnt=1;trp_cnt<nTRPs;trp_cnt++)
            {
                Array<cmat> rawChannel = estimatedChannelPerTRP(trp_cnt)(subbandSize*sb_cnt,(sb_cnt+1)*subbandSize-1);
                for(int tone_cnt=0;tone_cnt<rawChannel.length();tone_cnt++)
                {
                    currentRawChannel(tone_cnt) = concat_horizontal(currentRawChannel(tone_cnt),rawChannel(tone_cnt));
                }
                cmat tempPrecoder = type2Coefficients(possibleRankPerTRP(comb_cnt,trp_cnt)-1)(trp_cnt).precoderPerSubband(sb_cnt);
                
                cmat N = zeros_c(trp_cnt*nCSIRSPorts,possibleRankPerTRP(comb_cnt,trp_cnt));
                cmat M = zeros_c(nCSIRSPorts,sum(possibleRankPerTRP.get_row(comb_cnt)(getIntegers(0,trp_cnt-1))));
                precoder = concat_horizontal(precoder,N);
                precoder = concat_vertical(precoder,concat_horizontal(M,tempPrecoder));
            }
            
            Array<vec> sinrPerCW = calculateSinrPerCodewordNR(possibleRankPerTRP(comb_cnt,0),currentRawChannel,precoder,1.0,intCov,_DEMODULATION_SCHEME_MMSE_);
            for(int cw_cnt=0;cw_cnt<nCodeWords;cw_cnt++)
                append(avgSinrPerCW(cw_cnt),sinrPerCW(cw_cnt));
        }
        CSI_Info_S cqiInfo;
        int nREsPerRB = 2*12; //Two OFDM symbols in every RB
        specEffPerRankCombination(comb_cnt) = computeCQI(cqiInfo,avgSinrPerCW,sum(possibleRankPerTRP.get_row(comb_cnt)),&tbTables,nREsPerRB,nSubbands*subbandSize);
        cqiPerComb(comb_cnt) = cqiInfo.CQIindexPerCW;
        
    }
    int maxComb = max_index(specEffPerRankCombination);
    nLayersPerTRP = possibleRankPerTRP.get_row(maxComb);
    finalCoefficients.set_length(nTRPs);
    for(int trp_cnt=0;trp_cnt<nTRPs;trp_cnt++)
    {
        finalCoefficients(trp_cnt) = type2Coefficients(nLayersPerTRP(trp_cnt)-1)(trp_cnt);
    }
    cqi = cqiPerComb(maxComb);
    return specEffPerRankCombination(maxComb);
}

double doLinkAdaptationforMultiTRPCJT(Array<Array<cmat>>& estimatedChannelPerTRP,Array<Type2coefficients_N_OMP_S>& finalCoefficients, int dlNRBs, int prgSize, int maxRank, int nCodeWords, cmat intCov,cTBTables tbTables,Codebook codebook,int& nLayers,ivec& coPhasingFactors,ivec &cqi,double& BLER)
{
    int nSubbands = dlNRBs/prgSize;
    int subbandSize = prgSize;
    
    double N_phase = codebook.dlCodeBookConfig.N_Phase;
    int nTRPs = estimatedChannelPerTRP.length();
//     cout<<"nTRPs : "<<nTRPs<<endl;
    imat coPhaseCoeffPerTRP = (nTRPs==1) ? "[[0]]": computePermutations(N_phase,max(nTRPs-1,1));
    int minRank = 1;
    
    int nRanks = maxRank;
    vec specEffPerRank = zeros(nRanks);
    Array<ivec> cqiPerRank(nRanks);
    Array<Array<Type2coefficients_N_OMP_S>> type2Coefficients(nRanks);
    ivec coPhaseCombPerRank(nRanks);
    
    for(int rank_cnt=0;rank_cnt<nRanks;rank_cnt++)
    {
        int rank = rank_cnt+minRank;
        codebook.getType2OMPCoefficientsForMultiTRPCJT(type2Coefficients(rank_cnt),estimatedChannelPerTRP,nSubbands,subbandSize,1,rank);
        int nComb = coPhaseCoeffPerTRP.rows();
        vec specEffPerComb(nComb);
        Array<ivec> cqiPerComb(nComb);
        vec blerPerComb = zeros(nComb);
        for(int comb_cnt=0;comb_cnt<nComb;comb_cnt++)
        {
            cvec coPhasePerTRP = expj(2*pi*coPhaseCoeffPerTRP.get_row(comb_cnt)/N_phase);
            Array<vec> avgSinrPerCW(nCodeWords);
            for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++)
            {
                Array<cmat> currentRawChannel = estimatedChannelPerTRP(0)(subbandSize*sb_cnt,(sb_cnt+1)*subbandSize-1);
                cmat precoder = type2Coefficients(rank_cnt)(0).precoderPerSubband(sb_cnt);
                // Forming the composite channel and precoder for the subband. 
                for(int trp_cnt=1;trp_cnt<nTRPs;trp_cnt++)
                {
                    Array<cmat> rawChannel = estimatedChannelPerTRP(trp_cnt)(subbandSize*sb_cnt,(sb_cnt+1)*subbandSize-1);
                    for(int tone_cnt=0;tone_cnt<rawChannel.length();tone_cnt++)
                    {
                        currentRawChannel(tone_cnt) = concat_horizontal(currentRawChannel(tone_cnt),rawChannel(tone_cnt));
                    }
                    precoder = concat_vertical(precoder,coPhasePerTRP(trp_cnt-1)*type2Coefficients(rank_cnt)(trp_cnt).precoderPerSubband(sb_cnt));
                }
                Array<vec> sinrPerCW = calculateSinrPerCodewordNR(rank,currentRawChannel,precoder,1.0,intCov,_DEMODULATION_SCHEME_MMSE_);
                
                for(int cw_cnt=0;cw_cnt<nCodeWords;cw_cnt++)
                    append(avgSinrPerCW(cw_cnt),sinrPerCW(cw_cnt));
            }
            
            CSI_Info_S cqiInfo;
            int nREsPerRB = 2*12; //Two OFDM symbols in every RB
//             cout<<"avgSinrPerCW : "<<dB(mean(avgSinrPerCW(0)))<<endl;
            specEffPerComb(comb_cnt) = computeCQI(cqiInfo,avgSinrPerCW,rank,&tbTables,nREsPerRB,nSubbands*subbandSize)/nTRPs;
//             cout<<"specEffPerComb(comb_cnt) : "<<specEffPerComb(comb_cnt)<<endl;
            cqiPerComb(comb_cnt) = cqiInfo.CQIindexPerCW;
            
        }
        specEffPerRank(rank_cnt) = max(specEffPerComb);
        coPhaseCombPerRank(rank_cnt) = max_index(specEffPerComb);
        cqiPerRank(rank_cnt) = cqiPerComb(coPhaseCombPerRank(rank_cnt));
    }
    int maxIndex = max_index(specEffPerRank);
    nLayers = maxIndex+minRank;
    finalCoefficients = type2Coefficients(maxIndex);
    coPhasingFactors = coPhaseCoeffPerTRP.get_row(coPhaseCombPerRank(maxIndex));
    cqi = cqiPerRank(maxIndex);
    
//     cout<<"specEffPerRank : "<<specEffPerRank<<endl;
    
    return specEffPerRank(maxIndex);
}



