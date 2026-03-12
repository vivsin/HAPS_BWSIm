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


#include "../../../lib/L1/CSIRS/include/csirsTx.h"
#include "../../../lib/L1/CSIRS/include/csirsRx.h"
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
    
    
    int fftSize = 1024;
    int nSlots = 10;
    int scsInKhz = 15;
    int dlRBs = 52;
    int prgSize=4;
    int gc = (fftSize-dlRBs*12)/2;
    double samplingTimeinSec = 1/(double)(fftSize*scsInKhz*1000);	//Parameters are hardcoded
    int nCSIRSPorts = codebook.dlCodeBookConfig.nCSIRSPorts;
    double txPowerIndBm=46;
    channelEstimator.loadParams(1e-3/14,scsInKhz*1e3,fftSize);
    
    Array<ivec> allocatedRBs(1);
    allocatedRBs(0) = getIntegers(0,dlRBs-1);
    
    double currentTimeInSec, endTimeInSec;
    
    
    // Generating SRS config for each UE
    ivec UE = mcell.associationInfo.serviceNodes;
    ivec gNB = mcell.associationInfo.serverNodes;
    vec avgSpecEffPerUE=zeros(UE.length());
    vec NoSelectionSpecEff=zeros(UE.length());
    Array<ivec> coordTRPs(UE.length());
    for(int UEindx = 0;UEindx < UE.size();UEindx++)
        coordTRPs(UEindx)=zeros_i(3);
        
    
    ivec randSlots = randi(nSlots,0,1000000);
    sort(randSlots);
    
    // Slot level transmission and reception
    for(int slot_cnt = 0;slot_cnt < nSlots; slot_cnt++)
    {
        int slotIndx = randSlots(slot_cnt);
     
        mcell.mySys.channelCloud.clearChannelBuffer();
        
        cout<<"Running for slotIndx : "<<slotIndx<<endl;
        
        currentTimeInSec = (((fftSize+80.0/1024.0*fftSize)*2.0+(fftSize+72.0/1024.0*fftSize)*12)*slotIndx)*samplingTimeinSec;
        endTimeInSec = (((fftSize+80.0/1024.0*fftSize)*2+(fftSize+72.0/1024.0*fftSize)*12)*(slotIndx+1))*samplingTimeinSec;
        
        NRCodeBookType cbType = codebook.dlCodeBookConfig.codeBookType;
        
        Array<CSIRS_output> csi_output(gNB.size());
        //Tx Loop
        for(int gNBindx = 0;gNBindx < gNB.size();gNBindx++)
        {
            csi_output(gNBindx)=generateCSIRS(slotIndx,dlRBs,nCSIRSPorts,gNB(gNBindx));
            Array<cmat> resourceGridPerTxAntenna=fillCSIRS(csi_output(gNBindx),fftSize);
            Array<cvec> timeDomainFrame = getTimeDomainFrame(resourceGridPerTxAntenna,fftSize);
            for(int portIndx = 0;portIndx < timeDomainFrame.length();portIndx++)
                 timeDomainFrame(portIndx) = sqrt(inv_dB(txPowerIndBm-30))*timeDomainFrame(portIndx);
            
            mcell.mySys.transmitData(gNB(gNBindx),0,fftSize,timeDomainFrame,currentTimeInSec,1);
        }
        
        //Rx and CSI measurement Loop
        for(int UEindx = 0;UEindx < UE.size();UEindx++)
        {
            
            int associatedgNB  = mcell.associationInfo.associatedServerNodesForEachServiceNode(UEindx);
            int gNBIndx = find(gNB,associatedgNB);
            int ueAntenna = mcell.mySys.getNodeAntennaCountFromList(UE(UEindx,UEindx))(0);
            Array<cmat> estimatedChannel;
            
            cout<<"Receiving in UE : "<<UE(UEindx)<<" from gNodeB : "<<associatedgNB<<endl;
            
            cmat IntrCov(0,0);
            //Interference covariance computation 
            // Equivalent of performing YY* in CSI-IM
            ivec intrgNBs = gNB;
            intrgNBs.del(gNBIndx);
            
            if(intrgNBs.length())
            {
                ChannelOutput_S channelOutput = mcell.mySys.receiveData(UE(UEindx),0,_ANTENNA_CORRELATION_MEDIUM_,0,currentTimeInSec,endTimeInSec,false,intrgNBs);
                Array<cmat> resourceGridPerRxAntenna = getFreqDomainFrame(channelOutput.data,fftSize,0,14);
                IntrCov = computeRxIntrCov(dlRBs,resourceGridPerRxAntenna,allocatedRBs,csi_output(gNBIndx).rbRSPattern,(double)nCSIRSPorts/(double)csi_output(gNBIndx).nCDM); 
            }
            
            ivec txNodes = gNB;
            ChannelOutput_S channelOutput = mcell.mySys.receiveData(UE(UEindx),0,_ANTENNA_CORRELATION_MEDIUM_,0,currentTimeInSec,endTimeInSec,false,txNodes);
            Array<cmat> resourceGridPerRxAntenna = getFreqDomainFrame(channelOutput.data,fftSize,0,14);
            Array<cmat> rxRSPerRB = retrieveCSIRSFromResourceGrid(dlRBs,resourceGridPerRxAntenna,allocatedRBs,(double)nCSIRSPorts/(double)csi_output(gNBIndx).rbRSPattern);
                
                //Only noise case
            if(IntrCov.size()==0)
                IntrCov = mcell.mySys.channelCloud.getNoiseVariancePerDimension(UE(UEindx))*2.0*eye_c(resourceGridPerRxAntenna.length());
               
            cout<<"\t Estimating Channel .. "<<endl;
            estimatedChannel = estimateChannelFromCSIRS(rxRSPerRB,csi_output(gNBIndx).csirsPerPort,IntrCov); 
                
        
            cout<<"\t Computing CSI .. "<<endl;
            
            {
                double specEff;
                ivec cqi;
                
                Type2coefficients_N_OMP_S finalCoefficients;
                type1PMI_S finalPMIIndices;
                ivec coPhaseFactor;
                double bler;
                ivec nLayers;
                
                {
                    Array<cmat> channel = estimatedChannel;
                    int nLayers = ueAntenna;
                    int maxRank=ueAntenna;
                    
                    switch(cbType)
                    {
                        case _NR_CODEBOOK_TYPE_I_:
                        case _NR_CODEBOOK_TYPE_I_MULTIPANEL_:
                            specEff = doLinkAdaptationforTypeI(channel,dlRBs,prgSize,nLayers,1,IntrCov,tbTables,codebook,nLayers,finalPMIIndices,cqi);
                            break;
                        case _NR_CODEBOOK_TYPE_II_:
                        case _NR_CODEBOOK_TYPE_II_ENHANCED_:
                        case _NR_CODEBOOK_PORT_SELECTION_:
                        case _NR_CODEBOOK_PORT_SELECTION_ENHANCED_:
                        case _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_:
                            specEff = doLinkAdaptationforTypeII(channel,finalCoefficients,dlRBs,prgSize,nLayers,1,IntrCov,tbTables,codebook,nLayers,cqi);
                            break;
                        default:
                            cout<<"Invalid Codebook Type..."<<endl;
                            abort();
                    }
                }
                
                avgSpecEffPerUE(UEindx)+=specEff;
            }
        }
    }  
    
    avgSpecEffPerUE/=(double)nSlots;

    cout<<"avgSpecEffPerUE : "<<avgSpecEffPerUE<<endl;
    
}






