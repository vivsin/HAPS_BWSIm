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


#include "../../include/LLS_System.h"
#include "../../../lib/L1/SCH/include/schTx.h"
#include "../../../lib/L1/SCH/include/schRx.h"

#define FREQ_DOMAIN_CHANNEL

int main(int argc, char** argv){
  
    
    LLS_System llsSystem;
    llsSystem.initSystem();
    
    bool isDownlink = true;
    
    string dirPrefix = "./configFiles/NU1/LTETables/";
    
    cTBTables tbTables;
    AWGNTablesConvCoder awgnTables;
    awgnTables.init_AWGNTables(isDownlink,dirPrefix+"transportBlockMCSMapping.txt",dirPrefix+"cqiToMaxMCSRate.txt");
    tbTables.load(isDownlink,dirPrefix+"interleaverParameters.txt",dirPrefix+"transportBlockMCSMapping.txt",dirPrefix+"tbsTranslation.txt",dirPrefix+"cqiToMaxMCSRate.txt",awgnTables);
    Codebook codebook;
    ChannelEstimator channelEstimator;
    codebook.dlCodeBookConfig.parseCodeBookConfig("./configFiles/NU1/myDLTxRxConfig.txt","./configFiles/ActiveAntennaSystemConfig.txt");
    codebook.initializeDlCodebook();
    codebook.initializeUlCodebook();
        
    int nTxAntenna = llsSystem.simParam.antennaParam.numTXRUs(0);
    
    Array<cmat> resourceGridPerTxAntenna(nTxAntenna);
    
    int fftSize = 1024;
    int nRBs = 50;
    int nSlots = 10;
    int nRBsAllocated = 8;
    
    double samplingFrequencyInMHz = 15.36;
    double samplingTimeinSec = 1/(double)(samplingFrequencyInMHz*1e6);	//Parameters are hardcoded
    
    ivec RBs = getIntegers(0,nRBs-1);
    int nCodewords = 1;
    int nLayers = 1;
    int mcsIndex = 1;
    int nSymbols = 14;
    int ueID = 1;
    vec bler=zeros(llsSystem.simParam.snr.length());
    for(int snr_cnt=0;snr_cnt<llsSystem.simParam.snr.length();snr_cnt++)
    {
        
        double snrInDB = llsSystem.simParam.snr(snr_cnt);
        
        cout<<"Simulating for SNR : "<<snrInDB<<endl;
        
        double perSubcarrierNoiseVarianceInWatts = inv_dB(-snrInDB); // Scaling added to make Subcarrier noise power as -snrInDB - Dhiv
        int err_cnt=0;
        for(int slot_cnt=0;slot_cnt<nSlots;slot_cnt++)
        {
            int slotIndx = randi(0,1000000);
//             int slotIndx = slot_cnt;
            double currentTimeInSec = (((fftSize+80)*2+(fftSize+72)*12)*slotIndx)*samplingTimeinSec;
            double endTimeInSec = (((fftSize+80)*2+(fftSize+72)*12)*(slotIndx+1))*samplingTimeinSec;
                    
            for(int tx_cnt=0;tx_cnt<nTxAntenna;tx_cnt++)
                resourceGridPerTxAntenna(tx_cnt)=zeros_c(fftSize,nSymbols);
            
            int gNBIndx=0;
            //Scheduling//////////////////////////////////////////////////////////////
            SCHGrant_S grantInfo;
            
            //Round-robin scheduling
            ivec allocatedRBs = getIntegers(0,nRBsAllocated-1); // 12 RBs
            
            // Generating MCS, TBSize, Precoder
            grantInfo.ueID = ueID;
            
            grantInfo.gNBID = gNBIndx;
            
            grantInfo.mcsIndicesPerCW=ones_i(nCodewords)*mcsIndex;
            
            grantInfo.allocatedRBs=allocatedRBs;
            
            grantInfo.nLayers = nLayers;
            
            int nRBsAllocated = grantInfo.allocatedRBs.length();
            imat rbRSPattern=getDMRSPattern(1,nLayers);
            int nREsPerRB=count(rbRSPattern,0);
            int nREsAllocated = nRBsAllocated*nREsPerRB;
            grantInfo.nREsAllocated = nREsAllocated;
            grantInfo.harqInfo.TBSizePerCodewordInBits.set_length(nCodewords);
            
            for(int cw_cnt=0;cw_cnt<nCodewords;cw_cnt++)
                grantInfo.harqInfo.TBSizePerCodewordInBits(cw_cnt)=tbTables.getTBSizeForAllLayersCombined(grantInfo.mcsIndicesPerCW(cw_cnt), nREsAllocated, nRBsAllocated, nLayers, nSymbols)*8;
            
            ivec nLayersPerCW = getNumLayersPerCodeWord(nCodewords,nLayers);
            
            grantInfo.modulationOrderPercw.set_length(nCodewords);
            grantInfo.codeRatePercw.set_length(nCodewords);
            for(int cw_cnt=0;cw_cnt<nCodewords;cw_cnt++)
            {
                grantInfo.modulationOrderPercw(cw_cnt) = tbTables.transportBlockMCSMapping.modulationAlphabetPDSCH(mcsIndex);
                grantInfo.codeRatePercw(cw_cnt) = (double)grantInfo.harqInfo.TBSizePerCodewordInBits(cw_cnt)/(double)(nREsAllocated*grantInfo.modulationOrderPercw(cw_cnt)*nLayersPerCW(cw_cnt));
            }
            
            //Precoder cycling
            grantInfo.precoderPerRB.set_length(allocatedRBs.length());
            for(int rb_cnt=0;rb_cnt<allocatedRBs.length();rb_cnt++)
                grantInfo.precoderPerRB(rb_cnt) = codebook.getRandomPrecoder(nTxAntenna,nLayers);
                
            
            //PDSCH Transmission//////////////////////////////////////////////////////
            SCHGrant_S ueGrantInfo = grantInfo;
            
            CodingParameters codingParam;
            codingParam.load(grantInfo.harqInfo.TBSizePerCodewordInBits, grantInfo.modulationOrderPercw, grantInfo.codeRatePercw, grantInfo.nREsAllocated, nLayers);
            
            generateSCH(resourceGridPerTxAntenna , slotIndx, codingParam, nRBs, ueGrantInfo.allocatedRBs, ueGrantInfo.precoderPerRB, 1);
            
            
            Array<Array<Array<cvec>>> channelPerAntennaPerSymbol;
            
            //Passing through Channel /////////////////////////////////////////////////////////
            
            #ifdef FREQ_DOMAIN_CHANNEL
            Array<cmat> resourceGridPerRxAntenna = llsSystem.multipleWithFDChannel(resourceGridPerTxAntenna,channelPerAntennaPerSymbol,perSubcarrierNoiseVarianceInWatts,currentTimeInSec);
            #else
            Array<cmat> timeDomainFrame = getTimeDomainFrameMat(resourceGridPerTxAntenna,fftSize);
            Array<cmat> rxTimeDomainFrame = llsSystem.passThroughChannel(timeDomainFrame,channelPerAntennaPerSymbol,noiseVarianceInWatts,currentTimeInSec);
            Array<cmat> resourceGridPerRxAntenna = getFreqDomainFrame(rxTimeDomainFrame,fftSize);
            #endif
            
            
            //PDSCH Reception/////////////////////////////////////////////////////////
            cmat subcarrierInterferenceCovariance = perSubcarrierNoiseVarianceInWatts*eye_c(resourceGridPerRxAntenna.length());
            
            FadingParameters_S fadingParam = llsSystem.channel(0).getFadingParams();
//             fadingParam.nTaps=1;
//             fadingParam.tapDel=zeros(1);
//             fadingParam.tapGain=ones(1);
            
            
            double dopplerSpreadInHz = llsSystem.getDopplerSpreadInHz();
            
            ivec modulationOrderPercw(nCodewords);
            vec codeRatePercw(nCodewords);
            Array<vec> harqBuffer(nCodewords);
            for(int cw_cnt=0;cw_cnt<nCodewords;cw_cnt++)
            {
                modulationOrderPercw(cw_cnt) = tbTables.transportBlockMCSMapping.modulationAlphabetPDSCH(mcsIndex);
                codeRatePercw(cw_cnt) = tbTables.transportBlockMCSMapping.nrMCSCodeRatePDSCH(mcsIndex);
                harqBuffer(cw_cnt).set_length(0);
            }
            DecoderOutput_S decoderOutput = processSCH(resourceGridPerRxAntenna, harqBuffer, subcarrierInterferenceCovariance , slotIndx, codingParam, fadingParam, dopplerSpreadInHz, nRBs, ueGrantInfo.allocatedRBs , 1,channelPerAntennaPerSymbol,ueGrantInfo.precoderPerRB);
        
            cout<<"Decode status for slot : "<<slotIndx<<" is "<< decoderOutput.isSuccess<<endl;
            
            err_cnt+=sum(1-to_ivec(decoderOutput.isSuccess));    
        }
        bler(snr_cnt) = (double)err_cnt / (double)nSlots;
        
    }
    
    cout<<"BLER : "<< bler <<endl;
    
}
