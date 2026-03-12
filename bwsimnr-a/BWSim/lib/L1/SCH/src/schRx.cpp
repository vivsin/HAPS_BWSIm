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

#include "../include/schTx.h"
#include "../include/schRx.h"

Array<cmat> retrieveDataFromResourceGrid(Array<cmat>& rxGrid, ivec allocatedRBs, imat rbPattern, int guardSCs)
{
    int nREs = count(rbPattern,0)*allocatedRBs.length();
    
    int nRxAntenna = rxGrid.length();
    
    int nSymbolsPerSlot = rxGrid(0).cols(),nSubCarriersPerRB = 12;
    
    Array<cmat> rxSymbolsPerSubCarrier; rxSymbolsPerSubCarrier.set_length(nREs);
    
    int nRBsAllocated = allocatedRBs.length();
    
    for(int sym_cnt=0,pos_cnt=0;sym_cnt<nSymbolsPerSlot;sym_cnt++)
    {
        ivec dataSCIndices = find(rbPattern.get_col(sym_cnt)==0);
        for(int rb_cnt=0;rb_cnt<nRBsAllocated;rb_cnt++)
        {
            ivec scIndices = guardSCs + allocatedRBs(rb_cnt)*nSubCarriersPerRB + dataSCIndices;
            for(int sc_cnt=0;sc_cnt<scIndices.length();sc_cnt++)
            {
                cmat sym = zeros_c(nRxAntenna,1);
                for(int rxAntCnt=0;rxAntCnt<nRxAntenna;rxAntCnt++)
                {
                    sym(rxAntCnt,0) = rxGrid(rxAntCnt)(scIndices(sc_cnt),sym_cnt);
                }
                rxSymbolsPerSubCarrier(pos_cnt) = sym;
                pos_cnt++;
            }
        }
    }
    return rxSymbolsPerSubCarrier;
}

Array<cmat> getPrecodedChannel(Array<Array<Array<cvec>>>& channelPerAntennaPerSymbol, ivec allocatedRBs, Array<cmat>& precoderPerRB, int guardSCs)
{
    int nSymbolsPerSlot=14,nSubCarriersPerRB = 12;
    
    int nRBsAllocated = allocatedRBs.length();
    
    Array<cmat> channelPerRE;channelPerRE.set_length(nSymbolsPerSlot*nRBsAllocated*nSubCarriersPerRB);
    for(int sym_cnt=0,pos_cnt=0;sym_cnt<nSymbolsPerSlot;sym_cnt++)
    {
        int nRxAntenna = channelPerAntennaPerSymbol(sym_cnt).length();
        int nTxAntenna = channelPerAntennaPerSymbol(sym_cnt)(0).length();
        
        for(int rb_cnt=0;rb_cnt<nRBsAllocated;rb_cnt++)
        {
            ivec scIndices = guardSCs + allocatedRBs(rb_cnt)*nSubCarriersPerRB + getIntegers(0,nSubCarriersPerRB-1);
            cmat antennaPrecoder = precoderPerRB(rb_cnt);
            for(int sc_cnt=0;sc_cnt<scIndices.length();sc_cnt++)
            {
                int scIndx = scIndices(sc_cnt);
                cmat instantChannel = zeros_c(nRxAntenna,nTxAntenna);
                for(int rxAntenna_cnt=0;rxAntenna_cnt<nRxAntenna;rxAntenna_cnt++)
                for(int txAntenna_cnt=0;txAntenna_cnt<nTxAntenna;txAntenna_cnt++)
                    instantChannel(rxAntenna_cnt,txAntenna_cnt)=channelPerAntennaPerSymbol(sym_cnt)(rxAntenna_cnt)(txAntenna_cnt)(scIndx);
                
                channelPerRE(pos_cnt) = instantChannel*antennaPrecoder;
                pos_cnt++;
            }
        }
    }
    return channelPerRE;
}

Array<cmat> getEstimatedChannel(Array<cmat>& resourceGridPerRxAntenna, ivec allocatedRBs, DMRSInfo_S dmrsInfo, cmat interferenceCovariancePerRE, FadingParameters_S fadingParam, double dopplerSpreadInHz, int guardSCs)
{
    double slotDurationInSec = 0.001;
    int nSymbolsPerSlot = 14;
    int subCarrierCountPerRB = 12;
    int subCarrierSpacingInHz = 15000;
    
    int nRxAntenna = resourceGridPerRxAntenna.length();
    int fftSize = resourceGridPerRxAntenna(0).rows();
    
    ChannelEstimator channelEstimator;
    channelEstimator.loadParams(slotDurationInSec/nSymbolsPerSlot,subCarrierSpacingInHz,fftSize);
       
    ivec portReferenceNumbers(dmrsInfo.dmrsPorts.length());
    for(int port_cnt=0;port_cnt<dmrsInfo.dmrsPorts.length();port_cnt++)
    {
        portReferenceNumbers(port_cnt) = getDMRSPortReferenceNumber(dmrsInfo.dmrsPorts(port_cnt),dmrsInfo.dmrsType);
    }
    
    int nRBsAllocated = allocatedRBs.length();
    
    Array<Array<Array<cmat>>> estimatedChannelPerRB;estimatedChannelPerRB.set_length(nRBsAllocated);
    for(int rb_cnt=0;rb_cnt<nRBsAllocated;rb_cnt++)
    {
        Array<cmat> receivedRB(nRxAntenna);
        ivec scIndices = guardSCs + allocatedRBs(rb_cnt)*subCarrierCountPerRB + getIntegers(0,subCarrierCountPerRB-1);
        for(int ant_cnt=0;ant_cnt<nRxAntenna;ant_cnt++)
            receivedRB(ant_cnt) = resourceGridPerRxAntenna(ant_cnt).get_rows(scIndices);
        
        Array<cvec> txRSPerPort(dmrsInfo.dmrsPerPort.length());
        for(int port_cnt=0;port_cnt<dmrsInfo.dmrsPorts.length();port_cnt++)
        {
            int nDMRSSymbols = (dmrsInfo.dmrsType==1) ? ((find(dmrsInfo.dmrsPorts,1004)!=-1) ? 2 : 1) : ((find(dmrsInfo.dmrsPorts,1106)!=-1) ? 2 : 1);
            int nDMRSPositionsPerRB = (dmrsInfo.dmrsType==1) ? 6 : 4;
            ivec scIndices = rb_cnt*nDMRSPositionsPerRB + getIntegers(0,nDMRSPositionsPerRB-1);
            txRSPerPort(port_cnt).set_length(0);
            for(int sym_cnt=0;sym_cnt<nDMRSSymbols;sym_cnt++)
            {
                append(txRSPerPort(port_cnt),dmrsInfo.dmrsPerPort(port_cnt)(sym_cnt*nRBsAllocated*nDMRSPositionsPerRB+scIndices));
            }
        }
        channelEstimator.estimateMyChannelInRB(estimatedChannelPerRB(rb_cnt),receivedRB,0,interferenceCovariancePerRE,txRSPerPort,dopplerSpreadInHz,fadingParam.nTaps,fadingParam.tapDel,fadingParam.tapGain,portReferenceNumbers,dmrsInfo.rbRSPattern,dmrsInfo.dmrsPorts,allocatedRBs(rb_cnt));
    }
    Array<cmat> channelPerRE(nRBsAllocated*subCarrierCountPerRB*nSymbolsPerSlot);
    for(int sym_cnt=0,pos_cnt=0;sym_cnt<nSymbolsPerSlot;sym_cnt++)
    {
        for(int rb_cnt=0;rb_cnt<nRBsAllocated;rb_cnt++)
        {
            int startIndx = allocatedRBs(rb_cnt)*subCarrierCountPerRB;
            ivec scIndices = startIndx + getIntegers(0,subCarrierCountPerRB-1);
            
            for(int sc_cnt=0;sc_cnt<subCarrierCountPerRB;sc_cnt++)
            {
                channelPerRE(pos_cnt) = zeros_c(nRxAntenna,dmrsInfo.dmrsPorts.length());
                for(int ant_cnt=0;ant_cnt<nRxAntenna;ant_cnt++)
                    for(int port_cnt=0;port_cnt<dmrsInfo.dmrsPorts.length();port_cnt++)
                    {
                        channelPerRE(pos_cnt)(ant_cnt,port_cnt) = estimatedChannelPerRB(rb_cnt)(port_cnt)(ant_cnt)(sc_cnt,sym_cnt);
                    }
                pos_cnt++;
            }
        }
    }
    return channelPerRE;
}

Array<cvec> equalizeAndEstimateData(Array<cmat>& rxSymbolsPerSubCarrier,Array<cmat>& channelPerSubCarrier,Array<cvec>& eqChannelPerCW, int nCodeWords, ivec allocatedRBs,cmat interferenceCovariance)
{
    
    if(rxSymbolsPerSubCarrier.length()!=channelPerSubCarrier.length())
    {
        cout<<"Length mismatch in equalizeAndEstimateData().."<<endl;
        abort();
    }
        
    cvec rxSymbols(0);
    int nLayers = channelPerSubCarrier(0).cols(),nRxAntenna = channelPerSubCarrier(0).rows();
    int nSymbolsPerSlot = 14,nSubCarriersPerRB = 12;
    int nRBsAllocated = allocatedRBs.length();
    if(rxSymbolsPerSubCarrier.length()!=channelPerSubCarrier.length())
    {
        ivec indices(0);
        for(int sym_cnt=0,pos_cnt=0;sym_cnt<nSymbolsPerSlot;sym_cnt++)
        for(int rb_cnt=0;rb_cnt<nRBsAllocated;rb_cnt++)
        {
            ivec scIndices = allocatedRBs(rb_cnt)*nSubCarriersPerRB + getIntegers(0,nSubCarriersPerRB-1);
            
            for(int sc_cnt=0;sc_cnt<scIndices.length();sc_cnt++,pos_cnt++)
                append(indices,pos_cnt);
            
        }
        channelPerSubCarrier = getFromArray(channelPerSubCarrier,indices);
        channelPerSubCarrier = channelPerSubCarrier.left(rxSymbolsPerSubCarrier.length());
    }
    if(channelPerSubCarrier.length()!=rxSymbolsPerSubCarrier.length())
    {
        cout<<"Info : "<<channelPerSubCarrier.length()<<" , "<<rxSymbolsPerSubCarrier.length()<<endl;
        cout<<"Lengths mismatch in equalizeAndEstimateData().."<<endl;
        abort();
    }
    Array<cvec> symbolsPerLayer;symbolsPerLayer = repeatAsArray(rxSymbols,nLayers);
    Array<cvec> channelPerLayer;channelPerLayer = repeatAsArray(rxSymbols,nLayers);
    for(int sc_cnt=0;sc_cnt<rxSymbolsPerSubCarrier.length();sc_cnt++)
    {
        cmat EquNoise,estsym,EqChn;
        vec ppSINR;
        MMSErx(channelPerSubCarrier(sc_cnt),rxSymbolsPerSubCarrier(sc_cnt),EquNoise,estsym,EqChn,ppSINR,interferenceCovariance,nLayers,1.0);
        for(int layer_cnt=0;layer_cnt<nLayers;layer_cnt++)
        {
            append(symbolsPerLayer(layer_cnt),estsym(layer_cnt,0));
            append(channelPerLayer(layer_cnt),EqChn(layer_cnt,layer_cnt));
        }
    }
    Array<cvec> symbolsPerCW = performLayerDeMapping(symbolsPerLayer,nCodeWords,nLayers);
    eqChannelPerCW = performLayerDeMapping(channelPerLayer,nCodeWords,nLayers);
    return symbolsPerCW;
}


vec demodulate(int Q,cvec ip,cvec channel,double N0)
{
    QAM qamModem;
    qamModem.set_M(pow2i(Q));
    if(channel.length())
        return qamModem.demodulate_soft_bits(ip,channel,N0,APPROX);
    else    
        return qamModem.demodulate_soft_bits(ip,N0,APPROX);
}

int getNumCRCBits1(CRCTYPE crcType) {
  switch(crcType) {
    case CRC8: return 8;
    case CRC11: return 11;
    case CRC16: return 16;
    case CRC24A: return 24;
    case CRC24B: return 24;
    case CRC24C: return 24;
    default: cout<<"[both:] Error in getNumOfCRCBits: specify proper crcType"; abort();
  }
}

bvec decode(vec LLR, int rvid,vector<double> &hip,int E,int Q,double codeRate,CoderType_E coderType,CRCTYPE crcType,bool& isSuccess, bvec &isSuccessPerCB)
{
    int dataLength = round_i(E*codeRate);
    vector<bool> decoded;
    if(coderType == _CODERTYPE_LDPC_)
    {
        int Bg = getBG(dataLength,codeRate);
        decoded = dataChannelDecoder(ITPPVectorstdVector(LLR),hip,E/Q,findB_(dataLength,Bg),Q,rvid,Bg,isSuccessPerCB,crcType);
        
        NrCRC crc;crc.setCRCTypeNR(crcType);
        isSuccess = crc.performCRCCheckNR(decoded);
        
        if(isSuccessPerCB.length()==1)
            isSuccessPerCB(0) = isSuccess;
        
        decoded = crc.removeCRCNR(decoded);
    }
    else if (coderType == _CODERTYPE_POLAR_)
    {
        int RNTI = 0;

        int I_seg = 0; // Higher layer parameter indicating code block segmentation.
        int I_IL  = 0; // Higher layer parameter indicating intereleaving.
        int I_BIL = 1; // Higher layer parameter indicating triangular intereleaving.

        int n_PC    = 0; // Number of additional parity bits for early termination.
        int n_PC_wm = 0; // Number of PC bits to be kept together.

        int n_max            = 10;  // Max allowed mother code lenght.
        int L                = getNumCRCBits1(crcType); // number of CRC parity bits
        bool padOneBeforeCRC = false;

        PolarCodec<double, bool> codec(I_seg, I_IL, I_BIL, n_PC, n_PC_wm, n_max, L,
                                        padOneBeforeCRC);
        vector<double> llr = ITPPVectorstdVector(LLR);
        decoded = codec.polarDecode(llr,isSuccess, dataLength, E, RNTI, SCL);
    }
    return BoolVectortoBVec(decoded);
}

Array<cmat> retrieveChannelFromDataREPos(Array<cmat> channel, int nRBsAllocated, imat rbPattern)
{
    imat allocationPattern=rbPattern;
    for(int rb_cnt=0;rb_cnt<(nRBsAllocated-1);rb_cnt++)
        appendVertical(allocationPattern,rbPattern);
    
    
    
    ivec rePos = find(allocationPattern,0,true);
    Array<cmat> retrivedChannel = getFromArray(channel, rePos);
    return retrivedChannel;
}

DecoderOutput_S processSCH(Array<cmat> & resourceGridPerRxAntenna, Array<vec> &harqBuffer, cmat interferenceCovariance , int slotIndex, CodingParameters &codingParam, FadingParameters_S fadingParam, double dopplerSpreadInHz, int nRBs, ivec allocatedRBs , int dmrsType, Array<Array<Array<cvec>>> channelPerAntennaPerSymbol , Array<cmat> precoderPerRB)
{
     
        DMRSInfo_S dmrsInfo = generateDMRS(slotIndex, dmrsType, codingParam.nLayers, nRBs, allocatedRBs);

        int usedSubcarriers = 12 * nRBs;
        int fftSize = resourceGridPerRxAntenna(0).rows();
        int guardSubcarriersInOneSide = (fftSize - usedSubcarriers)/2;
        Array<cmat> rxDataPerRE = retrieveDataFromResourceGrid(resourceGridPerRxAntenna,allocatedRBs,dmrsInfo.rbRSPattern,guardSubcarriersInOneSide);
    
        Array<cmat> precodedChannel;
        if(channelPerAntennaPerSymbol.length())
            precodedChannel= getPrecodedChannel(channelPerAntennaPerSymbol,allocatedRBs,precoderPerRB,guardSubcarriersInOneSide);
        
        Array<cmat> estimatedChannel = getEstimatedChannel(resourceGridPerRxAntenna,allocatedRBs,dmrsInfo,interferenceCovariance,fadingParam,dopplerSpreadInHz,guardSubcarriersInOneSide);
    
        
        
//         if(simParam.channelParam.fadingModel!=_FADING_CHANNEL_MODEL_AWGN_)
//         {
//             ofstream out("./Results/channelNMSE.txt",std::ios::app);
//             vec estError = computeNMSE(precodedChannel,estimatedChannel);
//             out<<dB(snr)<<",\t"<<currentTimeInSeconds<<",\t"<<estError(0)/estError(1)<<endl;
//             out.close();
//             cumEstError +=estError;
//         }
        bool useChannelEstimation = false;
        
        Array<cmat> channelForEqu = (!useChannelEstimation) ? precodedChannel : estimatedChannel;
        channelForEqu = retrieveChannelFromDataREPos(channelForEqu, allocatedRBs.length(), dmrsInfo.rbRSPattern);

//         cout<<"rxDataPerRE : "<<rxDataPerRE<<endl;
//         cout<<"precodedChannel : "<<precodedChannel(0)<<endl;
        
//         cout<<endl<<"rxDataPerRE(0): "<<rxDataPerRE(0)<<endl;abort();
        
        Array<cvec> rxSymbols,eqChannel;
        rxSymbols = equalizeAndEstimateData(rxDataPerRE,channelForEqu,eqChannel,codingParam.nCodewords,allocatedRBs,interferenceCovariance);
        
//         cout<<endl<<"rxSymbols: "<<rxSymbols(0)(0,1000)<<endl;abort();
//         cout<<endl<<"rxSymbols: "<<rxSymbols(0).length()<<endl;abort();
//         cout<<endl<<"rxSymbols(0).length(): "<<rxSymbols(0)(0,1000)<<endl;abort();
//         cout<<endl<<"rxSymbols(0)(0,10): "<<rxSymbols(0)(0,10)<<endl;
//         cout<<endl<<"eqChannel(0)(0,10): "<<eqChannel(0)(0,10)<<endl;abort();
//         cout<<endl<<"rxSymbols(0)(0)/2.7012: "<<rxSymbols(0)(0)/2.7012<<endl;abort();
        Array<vec> LLRs(codingParam.nCodewords);
        for(int cw=0;cw<codingParam.nCodewords;cw++)
            LLRs(cw) = demodulate(codingParam.Q(cw),rxSymbols(cw),eqChannel(cw));
        
//         cout<<"LLRs : "<<(LLRs(0)(0,100)<0)<<endl;abort();
        
//         cout<<"rxSymbols : "<<rxSymbols<<endl;
//         cout<<"eqChannel : "<<eqChannel<<endl;
//         cout<<"encoded bits: "<<(LLRs(0)(0,100)<0)<<endl;abort();
        
        DecoderOutput_S decoderOutput;
        decoderOutput.outputBits.set_length(codingParam.nCodewords); 
        decoderOutput.isSuccessPerCB.set_length(codingParam.nCodewords); 
        decoderOutput.isSuccess=zeros_b(codingParam.nCodewords);
       
        ivec Q = codingParam.Q,E = elem_mult(codingParam.Q,codingParam.nLayersPerCW)*codingParam.nREs;
        vec codeRate = elem_div(to_vec(codingParam.tbSizePerCWInBits),to_vec(E));
        
        
        for(int cw=0;cw<codingParam.nCodewords;cw++)
        {
            bool isReceived=false;
            bvec isSuccessPerCB;
            
            vector<double> hip;hip.resize(0);
            if(harqBuffer(cw).length())
                hip = ITPPVectorstdVector(harqBuffer(cw));
            
            decoderOutput.outputBits(cw) = decode(LLRs(cw),codingParam.rvidPerCW(cw),hip,E(cw),Q(cw),codeRate(cw),codingParam.coderType,codingParam.crcType,isReceived,isSuccessPerCB);
            decoderOutput.isSuccess(cw) = (bin) isReceived;
            decoderOutput.isSuccessPerCB(cw) = isSuccessPerCB;
            
            harqBuffer(cw) = stdVectortoITPPVec(hip);
        }
        
        return decoderOutput;
}

