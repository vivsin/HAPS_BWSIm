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


#include "../../../lib/L1/SCH/include/schTx.h"
#include "../../../lib/L1/SCH/include/schRx.h"
#include "../../Mcell/include/MCell.h"
#include "../../sim/include/SupportingFunctions.h"



int main(int argc, char** argv){
       
    
    #ifdef USING_MPI
    initMPI(&(nTasks),&(currentTaskid),argc,argv);
    MPI_Barrier(MPI_COMM_WORLD);
    #endif
    
    
    ConfigFileNames_S myConfigFiles;
    ResultFileNames_S resultFiles;
    initAllFiles(argc,argv,myConfigFiles,resultFiles);
    
    MCell mcell = createMcellNetwork(myConfigFiles,resultFiles);
       
    bool isDownlink = true;
    
    if((argc>1 && strcmp(argv[1],"mcell")==0))
    {
        #ifdef USING_MPI
        MPI_Finalize();
        #endif
        exit(0);
    }
    
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
    
    
    int txNodeID = 0;
    ivec associatedUEs = mcell.associationInfo.getMyAssociatedServiceNodes(txNodeID);
    int nUEs = associatedUEs.length();
    int nTxAntenna = mcell.mySys.aNodes(0).getAntennaCount();
    
    Array<cmat> resourceGridPerTxAntenna(nTxAntenna);
    
    int fftSize = 1024;
    int nRBs = 50;
    int nSlots = 10;
    
    double samplingFrequencyInMHz;
    parse("./configFiles/mySysConfig.txt","samplingFrequencyInMHz",samplingFrequencyInMHz);
    double samplingTimeinSec = 1/(double)(samplingFrequencyInMHz*1e6);	//Parameters are hardcoded
    
    ivec RBs = getIntegers(0,nRBs-1);
    int nCodewords = 1;
    int nLayers = 1;
    int mcsIndex = 2;
    int nSymbols = 14;
    int nSubcarriersPerRB = 12;
  
    double txPowerIndBm = 46;

    ivec nLayersPerCW = getNumLayersPerCodeWord(nCodewords,nLayers);
    
    
    for(int slot_cnt=0;slot_cnt<nSlots;slot_cnt++)
    {
        cout<<"Running slot : "<<slot_cnt<<endl;
        double currentTimeInSec = (((fftSize+80.0/1024.0*fftSize)*2.0+(fftSize+72.0/1024.0*fftSize)*12)*slot_cnt)*samplingTimeinSec;
        double endTimeInSec = (((fftSize+80.0/1024.0*fftSize)*2+(fftSize+72.0/1024.0*fftSize)*12)*(slot_cnt+1))*samplingTimeinSec;
        
        
        for(int tx_cnt=0;tx_cnt<nTxAntenna;tx_cnt++)
            resourceGridPerTxAntenna(tx_cnt)=zeros_c(fftSize,nSymbols);
        
        int gNBIndx=0;
        //Scheduling//////////////////////////////////////////////////////////////
        Array<SCHGrant_S> grantInfo(nUEs);
        
        //Round-robin scheduling
        Array<ivec> allocatedRBs = splitVectorToUniformArray(RBs,nUEs);
        
        // Generating MCS, TBSize, Precoder
        for(int ue_cnt=0;ue_cnt<nUEs;ue_cnt++)
        {
            
            grantInfo(ue_cnt).ueID = associatedUEs(ue_cnt);
            
            grantInfo(ue_cnt).gNBID = gNBIndx;
            
            grantInfo(ue_cnt).mcsIndicesPerCW=ones_i(nCodewords)*mcsIndex;
            
            grantInfo(ue_cnt).allocatedRBs=allocatedRBs(ue_cnt);
            
            grantInfo(ue_cnt).nLayers = nLayers;
            
            int nRBsAllocated = grantInfo(ue_cnt).allocatedRBs.length();
            imat rbRSPattern=getDMRSPattern(1,nLayers);
            int nREsPerRB=count(rbRSPattern,0);
            int nREsAllocated = nRBsAllocated*nREsPerRB;
            
            grantInfo(ue_cnt).nREsAllocated = nREsAllocated;
            grantInfo(ue_cnt).tbSizePercwInBits.set_length(nCodewords);
            
            for(int cw_cnt=0;cw_cnt<nCodewords;cw_cnt++)
                grantInfo(ue_cnt).tbSizePercwInBits(cw_cnt)=tbTables.getTBSizeForAllLayersCombined(grantInfo(ue_cnt).mcsIndicesPerCW(cw_cnt), nREsAllocated, nRBsAllocated, nLayers, nSymbols)*8;

            grantInfo(ue_cnt).modulationOrderPercw.set_length(nCodewords);
            grantInfo(ue_cnt).codeRatePercw.set_length(nCodewords);
            for(int cw_cnt=0;cw_cnt<nCodewords;cw_cnt++)
            {
               grantInfo(ue_cnt).modulationOrderPercw(cw_cnt) = tbTables.transportBlockMCSMapping.modulationAlphabetPDSCH(mcsIndex);
               grantInfo(ue_cnt).codeRatePercw(cw_cnt) = (double)grantInfo(ue_cnt).tbSizePercwInBits(cw_cnt)/(double)(nREsAllocated*grantInfo(ue_cnt).modulationOrderPercw(cw_cnt)*nLayersPerCW(cw_cnt));
            }
                       
            //Precoder cycling
            grantInfo(ue_cnt).precoderPerRB.set_length(allocatedRBs(ue_cnt).length());
            for(int rb_cnt=0;rb_cnt<allocatedRBs(ue_cnt).length();rb_cnt++)
                grantInfo(ue_cnt).precoderPerRB(rb_cnt) = codebook.getRandomPrecoder(nTxAntenna,nLayers);
            
        }
        
        //PDSCH Transmission//////////////////////////////////////////////////////
        for(int ue_cnt=0;ue_cnt<nUEs;ue_cnt++)
        {
            SCHGrant_S ueGrantInfo = grantInfo(ue_cnt);
            
            CodingParameters codingParam;
            codingParam.load(grantInfo(ue_cnt).tbSizePercwInBits, grantInfo(ue_cnt).modulationOrderPercw, grantInfo(ue_cnt).codeRatePercw, grantInfo(ue_cnt).nREsAllocated, nLayers);
            
            generateSCH(resourceGridPerTxAntenna , slot_cnt, codingParam, nRBs, ueGrantInfo.allocatedRBs, ueGrantInfo.precoderPerRB, 1);
        }
        
        Array<cvec> timeDomainFrame = getTimeDomainFrame(resourceGridPerTxAntenna,fftSize);
        for(int portIndx = 0;portIndx < timeDomainFrame.length();portIndx++)
            timeDomainFrame(portIndx) = sqrt(inv_dB(txPowerIndBm-30))*timeDomainFrame(portIndx);
        mcell.mySys.transmitData(gNBIndx,0,fftSize,timeDomainFrame,currentTimeInSec,1);
    
        //PDSCH Reception/////////////////////////////////////////////////////////
        
        for(int ue_cnt=0;ue_cnt<nUEs;ue_cnt++)
        {
            ChannelOutput_S channelOutput = mcell.mySys.receiveData(associatedUEs(ue_cnt),0,_ANTENNA_CORRELATION_MEDIUM_,0,currentTimeInSec,endTimeInSec,false,mcell.associationInfo.serverNodes(0,0));
            Array<cmat> resourceGridPerRxAntenna = getFreqDomainFrame(channelOutput.data,fftSize,0,nSymbols);
            
            cmat interferenceCovariance = mcell.mySys.channelCloud.getNoiseVariancePerDimension(associatedUEs(ue_cnt))*eye_c(resourceGridPerRxAntenna.length());
            
            FadingParameters_S fadingParam = mcell.mySys.channelCloud.getLinkSpecificFadingParameters(gNBIndx,associatedUEs(ue_cnt));
            double dopplerSpreadInHz = mcell.mySys.channelCloud.getDopplerSpreadInHz(gNBIndx,associatedUEs(ue_cnt));
            
            ivec modulationOrderPercw(nCodewords);
            vec codeRatePercw(nCodewords);
            for(int cw_cnt=0;cw_cnt<nCodewords;cw_cnt++)
            {
               modulationOrderPercw(cw_cnt) = tbTables.transportBlockMCSMapping.modulationAlphabetPDSCH(mcsIndex);
               codeRatePercw(cw_cnt) = tbTables.transportBlockMCSMapping.nrMCSCodeRatePDSCH(mcsIndex);
            }
            CodingParameters codingParam;
            codingParam.load(grantInfo(ue_cnt).tbSizePercwInBits, modulationOrderPercw, codeRatePercw,grantInfo(ue_cnt).nREsAllocated, nLayers);
            
            DecoderOutput_S decoderOutput = processSCH(resourceGridPerRxAntenna, interferenceCovariance , slot_cnt, codingParam, fadingParam, dopplerSpreadInHz, nRBs, allocatedRBs(ue_cnt) , 1);
        }
        
    }
    	  
    
}






