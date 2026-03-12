#include "../include/csi.h"

extern cTBTables tbTables;
extern AWGNTablesConvCoder awgnTables;
extern Codebook codebook;
extern Codebook codebookTypeII;
extern Codebook codebookTypeIIEnhanced;
extern Codebook codebookTypeII_Ideal;
extern ChannelEstimator channelEstimator;

void fTransmitCSIRS_DL(SystemParams_S sysParams,int slotIndx,MCell &mcell, double txPowerIndBm)
{
    int nCSIRSPorts = codebook.dlCodeBookConfig.nCSIRSPorts;
    
    int fftSize = sysParams.fftSize;
    int scsInKhz = sysParams.scsInKhz;
    int dlRBs = sysParams.nRBs;
    int gc = (fftSize-dlRBs*12)/2;
    double samplingTimeinSec = 1/(double)(fftSize*scsInKhz*1000);	//Parameters are hardcoded
    double nSamplesPerSlot = ((fftSize+80*(double)fftSize/1024.0)*2+(fftSize+72*(double)fftSize/1024.0)*12);            // For 1024 FFT
    ivec scIndices;
    scIndices = getIntegers(0,dlRBs*12-1,12);
    scIndices = gc+mod(scIndices,dlRBs*12);
    
    double txSubcarrierPowerInWatts = inv_dB(txPowerIndBm-30)/(dlRBs*12*nCSIRSPorts);
    
    
    //cout<<endl<<"Number of Ports: "<<nCSIRSPorts<<endl;
    int nCSIResources = 1;
    int nCSIRSPortsPerResource = nCSIRSPorts;
    if (nCSIRSPorts>32)
    {
        if(nCSIRSPorts==64)
        {
            nCSIResources = 2;
            nCSIRSPortsPerResource=32;
        }
        else if(nCSIRSPorts==128)
        {
            nCSIResources = 4;
            nCSIRSPortsPerResource=32;
        }
        else
        {
            cout<<"Unsupported number of CSI-RS ports..."<<endl;abort();
        }
    }
    cout<<endl<<"nCSIRSPorts: "<<nCSIRSPorts<<" : "<<"nCSIResources: "<<nCSIResources<<" : "<<"nCSIRSPortsPerResource: "<<nCSIRSPortsPerResource<<endl;
    //double txPowerIndBm=46;
    channelEstimator.loadParams(1e-3/14,scsInKhz*1e3,fftSize);
    
    Array<ivec> allocatedRBs(1);
    allocatedRBs(0) = getIntegers(0,dlRBs-1);
    
    double currentTimeInSec, endTimeInSec;
    
    
    ivec UE = mcell.associationInfo.serviceNodes;
    ivec gNB = mcell.associationInfo.serverNodes;
    
    currentTimeInSec = (nSamplesPerSlot*slotIndx)*samplingTimeinSec;
    endTimeInSec = (nSamplesPerSlot*(slotIndx+1))*samplingTimeinSec;
    
    Array<CSIRS_output> csi_output(gNB.size());
    //Tx Loop
    for(int gNBindx = 0;gNBindx < gNB.size();gNBindx++)
    {
        Array<cvec> timeDomainFrame(nCSIRSPorts);
        for(int portIndx = 0;portIndx < timeDomainFrame.length();portIndx++)
            timeDomainFrame(portIndx) = zeros_c(nSamplesPerSlot*nCSIResources);
        for(int rsrc_cnt=0;rsrc_cnt<nCSIResources;rsrc_cnt++)
        {
            csi_output(gNBindx)=generateCSIRS(slotIndx,dlRBs,nCSIRSPortsPerResource,gNB(gNBindx));
            Array<cmat> resourceGridPerTxAntenna=fillCSIRS(csi_output(gNBindx),fftSize);
            
            for(int txAnt_cnt=0;txAnt_cnt<resourceGridPerTxAntenna.length();txAnt_cnt++)
                resourceGridPerTxAntenna(txAnt_cnt) = resourceGridPerTxAntenna(txAnt_cnt)*dComplex(sqrt(txSubcarrierPowerInWatts),0.0);
            
            Array<cvec> timeDomainFramePerResource = getTimeDomainFrame(resourceGridPerTxAntenna,fftSize);
            for(int portIndx1 = 0;portIndx1 < timeDomainFramePerResource.length();portIndx1++)
            {
                timeDomainFrame(rsrc_cnt*nCSIRSPortsPerResource+portIndx1).set_subvector(rsrc_cnt*nSamplesPerSlot,timeDomainFramePerResource(portIndx1));
            }
        }
        mcell.mySys.transmitData(gNB(gNBindx),0,fftSize,timeDomainFrame,currentTimeInSec,1);
    }
    
}

void fTransmitSRS_UL(SystemParams_S sysParams,int slotIndx,MCell &mcell, Array<srsConfig> &srsconfig)
{
    int fftSize = sysParams.fftSize;
    int scsInKhz = sysParams.scsInKhz;
    int ulRBs = sysParams.nRBs;
    
    int gc = (fftSize-ulRBs*12)/2;
    double samplingTimeinSec = 1/(double)(fftSize*scsInKhz*1000);	//Parameters are hardcoded
    double currentTimeInSec, endTimeInSec;
    
    // Generating SRS config for each UE
    ivec UE = mcell.associationInfo.serviceNodes;
    vec srsTXSCPowerInWattsForAllUEs=zeros(UE.length()); 
    
    
    for(int UEindx = 0;UEindx < UE.size();UEindx++)
    {
        
        // Calculating SRS Tx power
        double pathloss;
        sBeamPairLink beamPairLink;
        beamPairLink = mcell.associationInfo.associatedBeamPairLinkforEachServiceNode(UEindx);
        pathloss = beamPairLink.rspIndBm - beamPairLink.rsrpIndBm;
        srsTXSCPowerInWattsForAllUEs(UEindx) = srsconfig(UEindx).srsTXpowerInWatts/(srsconfig(UEindx).nRBs*(12/srsconfig(UEindx).K_TC)*srsconfig(UEindx).nSRSports);
        
    }
    
    double nSamplesPerSlot = ((fftSize+80*(double)fftSize/1024.0)*2+(fftSize+72*(double)fftSize/1024.0)*12);            
    
    currentTimeInSec = (nSamplesPerSlot*slotIndx)*samplingTimeinSec;
    endTimeInSec = (nSamplesPerSlot*(slotIndx+1))*samplingTimeinSec;
    
    for(int UE_cnt = 0;UE_cnt < UE.size();UE_cnt++)
    {        
        // Sequence Generation
        srsConfig srsConTemp;
        srsConTemp = srsconfig(UE_cnt);
        cmat srsSymbols(srsConTemp.nSRSports,srsConTemp.seqLength);
        if(srsConTemp.nSRSports == 8){
            srsConTemp.nSRSports = 4;
            cmat srsSymbTemp;
            srsSymbTemp = generateZCseq(srsConTemp);
            srsSymbols.set_submatrix(0,0,srsSymbTemp);
            srsSymbols.set_submatrix(4,0,srsSymbTemp);
        }
        else if(srsConTemp.nSRSports == 4){
            cmat srsSymbTemp;
            srsSymbTemp = generateZCseq(srsConTemp);
            srsSymbols.set_submatrix(0,0,srsSymbTemp);
        }
        else{
            
            cout<<"srsConTemp.nSRSports : "<<srsConTemp.nSRSports<<endl;
            cout<<endl<<"Abort: Function not yet written for < 4 Ports"<<endl;abort();
        }
        
        // Resource Mapping
        Array<cmat> txGrid(srsConTemp.nSRSports);
        for(int portIndx = 0;portIndx < srsConTemp.nSRSports;portIndx++){
            txGrid(portIndx) = zeros_c(fftSize,14);
        }
        
        for(int symbIndx = 0;symbIndx < srsConTemp.symbols.size();symbIndx++){
            for(int portIndx = 0;portIndx < srsConTemp.nSRSports;portIndx++){
                int combOffset;
                if(portIndx < 4){combOffset = srsConTemp.combOffset;}
                else{combOffset = srsConTemp.combOffset+2;}
                ivec scIndices=gc+getIntegers(0,srsConTemp.nRBs*12-1,srsConTemp.K_TC)+combOffset+(srsConTemp.rbOffset+symbIndx*srsConTemp.nRBs)*12;
                scIndices = mod(scIndices,48*12);
                for(int scIndx = 0;scIndx < scIndices.size();scIndx++){
                    if(portIndx >= 0){
                        txGrid(portIndx)(scIndices(scIndx),srsConTemp.symbols(symbIndx)) = sqrt(srsTXSCPowerInWattsForAllUEs(UE_cnt))*srsSymbols(portIndx,scIndx);
                    }
                    else{
                        txGrid(portIndx)(scIndices(scIndx),srsConTemp.symbols(symbIndx)) = 0;
                    }
                }
            }
        }
        // Power amplification and transmission in time domain
        Array<cvec> timeDomainFrame;
        timeDomainFrame = getTimeDomainFrame(txGrid, fftSize, 0);
        mcell.mySys.transmitData(UE(UE_cnt),0,fftSize,timeDomainFrame,currentTimeInSec,2);
    }
}


Array<CSI_Feedback_Info_S> fReturnCurrentMCSperUE_DL(SystemParams_S sysParams,int slotIndx,MCell &mcell,ivec uesForMCS, double txPowerIndBm, Array<cmat> intCovPerUE)
{
    Array<CSI_Feedback_Info_S> csiFeedbackInfo(uesForMCS.length());
    string dirPrefix = "./configFiles/NU1/LTETables/";
    CodeBookConfig_S codebookConfig = codebook.dlCodeBookConfig;
    int nCSIRSPorts = codebook.dlCodeBookConfig.nCSIRSPorts;
    
    int fftSize = sysParams.fftSize;
    int scsInKhz = sysParams.scsInKhz;
    int dlRBs = sysParams.nRBs;
    int prgSize = sysParams.RBGSize;
    int nSlots = sysParams.nSlots;
    
    int gc = (fftSize-dlRBs*12)/2;
    
    double samplingTimeinSec = 1/(double)(fftSize*scsInKhz*1000);	//Parameters are hardcoded
    double nSamplesPerSlot = ((fftSize+80*(double)fftSize/1024.0)*2+(fftSize+72*(double)fftSize/1024.0)*12);            // For 1024 FFT
    ivec scIndices;
    scIndices = getIntegers(0,dlRBs*12-1,12);
    scIndices = gc+mod(scIndices,dlRBs*12);
    
    double txSubcarrierPowerInWatts = inv_dB(txPowerIndBm-30)/(dlRBs*12*nCSIRSPorts);
    
    
    //cout<<endl<<"Number of Ports: "<<nCSIRSPorts<<endl;
    int nCSIResources = 1;
    int nCSIRSPortsPerResource = nCSIRSPorts;
    if (nCSIRSPorts>32)
    {
        if(nCSIRSPorts==64)
        {
            nCSIResources = 2;
            nCSIRSPortsPerResource=32;
        }
        else if(nCSIRSPorts==128)
        {
            nCSIResources = 4;
            nCSIRSPortsPerResource=32;
        }
        else
        {
            cout<<"Unsupported number of CSI-RS ports..."<<endl;abort();
        }
    }
    cout<<endl<<"nCSIRSPorts: "<<nCSIRSPorts<<" : "<<"nCSIResources: "<<nCSIResources<<" : "<<"nCSIRSPortsPerResource: "<<nCSIRSPortsPerResource<<endl;
    //double txPowerIndBm=46;
    channelEstimator.loadParams(1e-3/14,scsInKhz*1e3,fftSize);
    
    ivec allocatedRBs;
    allocatedRBs = getIntegers(0,dlRBs-1);
    
    double currentTimeInSec, endTimeInSec;
    
    
    ivec UE = mcell.associationInfo.serviceNodes;
    ivec gNB = mcell.associationInfo.serverNodes;
    vec SINR=zeros(uesForMCS.length());
    
    vec specEffToBeReturned = zeros(uesForMCS.length());
    vec specEffToBeReturnedRel19=zeros(uesForMCS.length());
     vec specEffToBeReturnedRel19_5_8=zeros(uesForMCS.length());
    ivec nLayersPerUE=zeros_i(uesForMCS.length());
        ivec nLayersPerUERel19=zeros_i(uesForMCS.length());
         ivec nLayersPerUERel19_5_8=zeros_i(uesForMCS.length());

    vec specEffToBeReturnedTypeII = zeros(uesForMCS.length());
        vec specEffToBeReturnedTypeIIEnhanced = zeros(uesForMCS.length());
     vec specEffToBeReturnedTypeIIIdeal = zeros(uesForMCS.length());
    ivec nLayersPerUETypeII=zeros_i(uesForMCS.length());
    ivec nLayersPerUETypeIIEnhanced=zeros_i(uesForMCS.length());
    ivec nLayersPerUETypeIIIdeal=zeros_i(uesForMCS.length());
    vec avgSpecEffPerUE=zeros(UE.length());
    vec NoSelectionSpecEff=zeros(UE.length());
    

    currentTimeInSec = (nSamplesPerSlot*slotIndx)*samplingTimeinSec;
    endTimeInSec = (nSamplesPerSlot*(slotIndx+1))*samplingTimeinSec;
    
    NRCodeBookType cbType = codebook.dlCodeBookConfig.codeBookType;
    
    
    //Rx and CSI measurement Loop
    ivec UEindices = find(UE,uesForMCS);
    int nue=uesForMCS.size();
    // nue=1;    //For Single User
    Array<CSIRS_output> csi_output(gNB.size());
    for(int gNBindx = 0;gNBindx < gNB.size();gNBindx++)
    {
        csi_output(gNBindx)=generateCSIRS(slotIndx,dlRBs,nCSIRSPortsPerResource,gNB(gNBindx));
    }
    
    for(int ue = 0; ue < nue; ue++)
    {
        int UEindx = UEindices(ue);
        int associatedgNB  = mcell.associationInfo.associatedServerNodesForEachServiceNode(UEindx);
        int gNBIndx = find(gNB,associatedgNB);
        int ueAntenna = mcell.mySys.getNodeAntennaCountFromList(UE(UEindx,UEindx))(0);
        int gNBAntenna = mcell.mySys.getNodeAntennaCountFromList(to_ivec(associatedgNB))(0);
        
        cout<<"Computing CQI for UE : "<<UE(UEindx)<<endl;
        
        cmat IntrCov;

        Array<Array<cmat>> idealChannelPerCSIresource(nCSIResources);

        cout<<"\t Receiving Signal .. "<<endl;
        
        for(int rsrc_cnt=0;rsrc_cnt<nCSIResources;rsrc_cnt++){
            
            //Interference covariance computation 
            // Equivalent of performing YY* in CSI-IM
            
            if(intCovPerUE.length())
            {
                IntrCov = intCovPerUE(ue);
            }
            else
            {
                ivec intrgNBs = gNB;
                intrgNBs.del(gNBIndx);
                if(intrgNBs.length())
                {
                    ChannelOutput_S channelOutput = mcell.mySys.receiveData(UE(UEindx),0,_ANTENNA_CORRELATION_MEDIUM_,0,currentTimeInSec+rsrc_cnt*1e-3,endTimeInSec+rsrc_cnt*1e-3,false,intrgNBs);
                    Array<cmat> resourceGridPerRxAntenna = getFreqDomainFrame(channelOutput.data,fftSize,0,14);
                    IntrCov = computeRxIntrCov(dlRBs,resourceGridPerRxAntenna,allocatedRBs,csi_output(gNBIndx).rbRSPattern,1.0/*(double)nCSIRSPorts/(double)csi_output(gNBIndx).nCDM*/); 
                }
                else
                {
                    double N0=mcell.mySys.channelCloud.getNoiseVariancePerDimension(UE(UEindx))*2.0;
                    cout<<"N0 : "<<dB(N0)<<endl;
                    IntrCov=N0*eye_c(mcell.mySys.aNodes(UE(UEindx)).getAntennaCount());
                }
            }
            
            ivec txNodes = gNB;
            
//             ChannelOutput_S channelOutput = mcell.mySys.receiveData(UE(UEindx),0,_ANTENNA_CORRELATION_MEDIUM_,0,currentTimeInSec+rsrc_cnt*1e-3,endTimeInSec+rsrc_cnt*1e-3,false,txNodes);
// //             ChannelOutput_S channelOutput = mcell.mySys.receiveData(UE(UEindx),0,associatedgNB ,0,currentTimeInSec+rsrc_cnt*1e-3,endTimeInSec+rsrc_cnt*1e-3,false);
// 
//             Array<cmat> resourceGridPerRxAntenna = getFreqDomainFrame(channelOutput.data,fftSize,0,14);
//             
// 
//             Array<cmat> rxRSPerRB = retrieveCSIRSFromResourceGrid(dlRBs,resourceGridPerRxAntenna,allocatedRBs,csi_output(gNBIndx).rbRSPattern);
//             
//             
//             //Only noise case
//             if(IntrCov.size()==0){
//                 IntrCov = mcell.mySys.channelCloud.getNoiseVariancePerDimension(UE(UEindx))*2.0*eye_c(resourceGridPerRxAntenna.length());
//                
//             }
            
            cout<<endl<<"\t Estimating Channel .. "<<endl;
            
            {
                ChannelOutput_S channelOutput = mcell.mySys.receiveData(UE(UEindx),0,associatedgNB ,0,currentTimeInSec+rsrc_cnt*1e-3,endTimeInSec+rsrc_cnt*1e-3,false);

//                 Array<cmat> resourceGridPerRxAntennaTemp = getFreqDomainFrame(channelOutput.data,fftSize,0,14);
//                 
//                 Array<cmat> rxRSPerRBTemp = retrieveCSIRSFromResourceGrid(dlRBs,resourceGridPerRxAntennaTemp,allocatedRBs,csi_output(gNBIndx).rbRSPattern);
//                 
//                 estimatedChannelPerResource(rsrc_cnt) = estimateChannelFromCSIRS(rxRSPerRBTemp,csi_output(gNBIndx).csirsPerPort,0*IntrCov);
                
                Array<Array<cvec>> idealChannelTD = channelOutput.channels(0);
//                 cout<<endl<<idealChannelTD(0)(0)<<endl;abort();
                Array<Array<cvec>> idealChannelFD(nCSIRSPortsPerResource);
                for(int txAntIndx = 0;txAntIndx < idealChannelFD.length();txAntIndx++){
                    idealChannelFD(txAntIndx).set_length(idealChannelTD(0).length());
                    for(int rxAntIndx = 0;rxAntIndx < idealChannelTD(0).length();rxAntIndx++){
//                         cvec temp = fft(sqrt(inv_dB(txPowerIndBm-30))*idealChannelTD(txAntIndx+(nCSIRSPortsPerResource*rsrc_cnt))(rxAntIndx),fftSize);
                        cvec temp = fft(idealChannelTD(txAntIndx+(nCSIRSPortsPerResource*rsrc_cnt))(rxAntIndx),fftSize);
                        temp = fftShift(temp);
                        idealChannelFD(txAntIndx)(rxAntIndx) = temp(scIndices)*dComplex(sqrt(txSubcarrierPowerInWatts),0.0);

                    }
                }
                idealChannelPerCSIresource(rsrc_cnt) = getAcrossArray(idealChannelFD);
                
            }
            
        }
        
        Array<cmat> idealChannel(dlRBs);
        if(nCSIRSPorts>32){
            for(int RB_cnt=0;RB_cnt<idealChannelPerCSIresource(0).size();RB_cnt++){
                cmat ConcatArray;
                for(int rsrc_cnt=0;rsrc_cnt<nCSIResources;rsrc_cnt++){
                    ConcatArray=concat_horizontal(ConcatArray,idealChannelPerCSIresource(rsrc_cnt)(RB_cnt));
                }
                idealChannel(RB_cnt)=ConcatArray;
            }
            
        }
        else{
            idealChannel=idealChannelPerCSIresource(0);
        }
        
//         cout<<"\t Computing CSI .. "<<endl;
        
        {
            double specEff;
            ivec cqi;
            Type2coefficients_N_OMP_S finalCoefficients;
            type1PMI_S finalPMIIndices;
            ivec coPhaseFactor;
            double bler;
            ivec nLayers;
            double SpectralEfficiency;

            {
                Array<cmat> channel = idealChannel;
                int nLayers = ueAntenna;
                int maxRank = min(gNBAntenna,ueAntenna);
                if(gNBAntenna>1)
                {
                    switch(cbType)
                    {
                        case _NR_CODEBOOK_TYPE_I_:
                        case _NR_CODEBOOK_TYPE_I_MULTIPANEL_:
                            cout<<endl<<" Codebook"<<endl;
    //                        cout<<endl<<">------------Ideal------------<"<<endl;
    //                         csiFeedbackInfo(ue) = doLinkAdaptationforTypeII(channel,finalCoefficients,dlRBs,prgSize,maxRank,1,IntrCov,tbTables,codebookTypeII_Ideal,nLayers,cqi);
    //                         specEffToBeReturnedTypeIIIdeal(ue) = csiFeedbackInfo(ue).specEffWideband;
    //                         nLayersPerUETypeIIIdeal(ue) = csiFeedbackInfo(ue).nLayers;
    //
    //                         cout<<endl<<">---------R15 Type-II Codebook-----------<"<<endl;
    // //
    //                         csiFeedbackInfo(ue) = doLinkAdaptationforTypeII(channel,finalCoefficients,dlRBs,prgSize,maxRank,1,IntrCov,tbTables,codebookTypeII,nLayers,cqi);
    //                         specEffToBeReturnedTypeII(ue) = csiFeedbackInfo(ue).specEffWideband;
    //                         nLayersPerUETypeII(ue) = csiFeedbackInfo(ue).nLayers;
    // //
    //                         cout<<endl<<"------------R16 Type-II Codebook---------------"<<endl;
    //                         csiFeedbackInfo(ue) = doLinkAdaptationforTypeII(channel,finalCoefficients,dlRBs,prgSize,maxRank,1,IntrCov,tbTables,codebookTypeIIEnhanced,nLayers,cqi);
    //                         specEffToBeReturnedTypeIIEnhanced(ue) = csiFeedbackInfo(ue).specEffWideband;
    //                         nLayersPerUETypeIIEnhanced(ue) = csiFeedbackInfo(ue).nLayers;
    // //
                            cout<<endl<<">--------Rel 15 Type I----------<"<<endl;
                            csiFeedbackInfo(ue) = computeSEforType1Simplifed(codebookConfig,idealChannel,IntrCov,tbTables,dlRBs,prgSize,1,maxRank);
                            specEffToBeReturned(ue) = csiFeedbackInfo(ue).specEffWideband;
                            nLayersPerUE(ue) = csiFeedbackInfo(ue).nLayers;
    //                         cout<<endl<<">--------Rel 19 Type I------<"<<endl;
    //                          if(maxRank<5){
    //                             csiFeedbackInfo(ue) = computeSEforType1Rel19(codebookConfig,idealChannel,IntrCov,tbTables,dlRBs,prgSize,1,maxRank);
    //                         specEffToBeReturnedRel19(ue) = csiFeedbackInfo(ue).specEffWideband;
    //                         nLayersPerUERel19(ue) = csiFeedbackInfo(ue).nLayers;
    //                          }
    //                          else{
    //                          csiFeedbackInfo(ue) = computeSEforType1Rel19_5_8(codebookConfig,idealChannel,IntrCov,tbTables,dlRBs,prgSize,1,maxRank);
    //                         specEffToBeReturnedRel19(ue) = csiFeedbackInfo(ue).specEffWideband;
    //                         nLayersPerUERel19(ue) = csiFeedbackInfo(ue).nLayers;
    //                          }
                            break;
                        case _NR_CODEBOOK_TYPE_II_:
                        case _NR_CODEBOOK_TYPE_II_ENHANCED_:
                        case _NR_CODEBOOK_PORT_SELECTION_:
                        case _NR_CODEBOOK_PORT_SELECTION_ENHANCED_:
                        case _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_:
                            csiFeedbackInfo(ue) = doLinkAdaptationforTypeII(channel,finalCoefficients,dlRBs,prgSize,maxRank,1,IntrCov,tbTables,codebook,nLayers,cqi);
                            break;
                        default:
                            cout<<"Invalid Codebook Type..."<<endl;
                            abort();
                    }
                }
                else
                {
                    csiFeedbackInfo(ue) = computeSEforSinglePort(idealChannel,IntrCov,tbTables,dlRBs,prgSize);
                    specEffToBeReturned(ue) = csiFeedbackInfo(ue).specEffWideband;
                    nLayersPerUE(ue) = csiFeedbackInfo(ue).nLayers;
                }

            }
            //cout<<endl<<"ue: "<<ue<<", se: "<<csiFeedbackInfo(ue).specEffWideband<<endl;
            
            avgSpecEffPerUE(UEindx) += csiFeedbackInfo(ue).specEffWideband;
            
        }
//         cout<<endl<<"----------Aborting UE Loop----------"<<endl;abort();
    }
    cout<<"UEs: "<<uesForMCS<<endl;
   
    cout<<"SpecEff TypeI : "<<specEffToBeReturned<<endl;
    cout<<"Layers TypeI : "<<nLayersPerUE<<endl;
    
//     cout<<"SpecEff TypeII Ideal : "<<specEffToBeReturnedTypeIIIdeal<<endl;
//     cout<<"Layers TypeI Ideal : "<<nLayersPerUETypeIIIdeal<<endl;
//     
//     cout<<"SpecEff TypeII       : "<<specEffToBeReturnedTypeII<<endl;
//     cout<<"Layers TypeII      : "<<nLayersPerUETypeII<<endl;
//     
//     cout<<"SpecEff TypeII Enhanced : "<<specEffToBeReturnedTypeIIEnhanced<<endl;
//     cout<<"Layers TypeII Enhanced : "<<nLayersPerUETypeIIEnhanced<<endl;
//      
//     cout<<"SpecEff TypeI Rel19 Scheme B : "<<specEffToBeReturnedRel19<<endl;
//     cout<<"Layers TypeI Rel 19 Scheme B: "<<nLayersPerUERel19<<endl;
   
    // Static to retain across calls
    static vec SE_TypeISum;  
    static vec SE_TypeII_IdealSum;
    static vec SE_TypeIISum;
    static vec SE_TypeII_EnhancedSum;
    static vec SE_TypeI_19_BSum;
    static vec SE_TypeIAvg;  
    static vec SE_TypeII_IdealAvg;
    static vec SE_TypeIIAvg;
    static vec SE_TypeII_EnhancedAvg;
    static vec SE_TypeI_19_BAvg;
    static int count = 0;  // Counter for iterations

    SE_TypeISum+=specEffToBeReturned;
    SE_TypeII_IdealSum+=specEffToBeReturnedTypeIIIdeal;
    SE_TypeIISum+=specEffToBeReturnedTypeII;
    SE_TypeII_EnhancedSum+=specEffToBeReturnedTypeIIEnhanced;
    SE_TypeI_19_BSum+=specEffToBeReturnedRel19;
    count++;  // Increment count

    
    if (count == nSlots) {
        
        SE_TypeIAvg=SE_TypeISum/nSlots;
        SE_TypeII_IdealAvg=SE_TypeII_IdealSum/nSlots;
        SE_TypeIIAvg=SE_TypeIISum/nSlots;
        SE_TypeII_EnhancedAvg=SE_TypeII_EnhancedSum/nSlots;
        SE_TypeI_19_BAvg=SE_TypeI_19_BSum/nSlots;
        
        // Reset sum for next cycle
        SE_TypeISum.zeros();
        SE_TypeII_IdealSum.zeros();
        SE_TypeIISum.zeros();
        SE_TypeII_EnhancedSum.zeros();
        SE_TypeI_19_BSum.zeros();
        
        mat Avg_SE_TypeI = mat(SE_TypeIAvg);
        mat Avg_SE_TypeII = mat(SE_TypeIIAvg);
        mat Avg_SE_TypeII_Enhanced=mat(SE_TypeII_EnhancedAvg);
        mat Avg_SE_TypeII_Ideal=mat(SE_TypeII_IdealAvg);
        mat Avg_SE_TypeI_19_B=mat(SE_TypeI_19_BAvg);
        
        
        cout<<"Avg SpecEff TypeI : "<<SE_TypeIAvg<<endl;
//         cout<<"Avg SpecEff TypeII Ideal : "<<SE_TypeII_IdealAvg<<endl;
//         cout<<"Avg SpecEff TypeII       : "<<SE_TypeIIAvg<<endl;
//         cout<<"Avg SpecEff TypeII Enhanced : "<<SE_TypeII_EnhancedAvg<<endl;
//         cout<<"Avg SpecEff TypeI Rel19 Scheme B : "<<SE_TypeI_19_BAvg<<endl;
        
        ofstream fout;
        fout.open("./Results/Avg_SE_TypeI",ios::app);
        fout<<Avg_SE_TypeI<<endl;
        fout.close();


        fout.open("./Results/Avg_SE_TypeII",ios::app);
        fout<<Avg_SE_TypeII<<endl;
        fout.close();


        fout.open("./Results/Avg_SE_TypeII_Enhanced",ios::app);
        fout<<Avg_SE_TypeII_Enhanced<<endl;
        fout.close();

        fout.open("./Results/Avg_SE_TypeII_Ideal",ios::app);
        fout<<Avg_SE_TypeII_Ideal<<endl;
        fout.close();
        
        fout.open("./Results/Avg_SE_TypeI_19_B",ios::app);
        fout<<Avg_SE_TypeI_19_B<<endl;
        fout.close();
        
        count = 0;  // Reset counter
    }



    mat SE_TypeI = mat(specEffToBeReturned);
    mat SE_TypeII = mat(specEffToBeReturnedTypeII);
    mat SE_TypeII_Enhanced=mat(specEffToBeReturnedTypeIIEnhanced);
    mat SE_TypeII_Ideal=mat(specEffToBeReturnedTypeIIIdeal);
    mat SE_TypeI_19_B=mat(specEffToBeReturnedRel19);



    ofstream fout;
    fout.open("./Results/SE_TypeI",ios::app);
    fout<<SE_TypeI<<endl;
    fout.close();


    fout.open("./Results/SE_TypeII",ios::app);
    fout<<SE_TypeII<<endl;
    fout.close();


    fout.open("./Results/SE_TypeII_Enhanced",ios::app);
    fout<<SE_TypeII_Enhanced<<endl;
    fout.close();

    fout.open("./Results/SE_TypeII_Ideal",ios::app);
    fout<<SE_TypeII_Ideal<<endl;
    fout.close();
    
    fout.open("./Results/SE_TypeI_19_B",ios::app);
    fout<<SE_TypeI_19_B<<endl;
    fout.close();
    
// abort();
    return csiFeedbackInfo;
}


Array<CSI_Feedback_Info_S> fReturnCurrentMCSperUE_UL(SystemParams_S sysParams,int slotIndx,MCell &mcell,ivec gNBsForMCS, Array<srsConfig> &srsconfig)
{
    int fftSize = sysParams.fftSize;
    int scsInKhz = sysParams.scsInKhz;
    int ulRBs = sysParams.nRBs;
    
    int gc = (fftSize-ulRBs*12)/2;
    double samplingTimeinSec = 1/(double)(fftSize*scsInKhz*1000);	//Parameters are hardcoded
    double currentTimeInSec, endTimeInSec;
    
    // Generating SRS config for each UE
    ivec UE = mcell.associationInfo.serviceNodes;
    vec srsTXpowerInWattsForAllUEs=zeros(UE.length()); 
    
    
    for(int UEindx = 0;UEindx < UE.size();UEindx++)
    {
        
        // Calculating SRS Tx power
        double pathloss;
        sBeamPairLink beamPairLink;
        beamPairLink = mcell.associationInfo.associatedBeamPairLinkforEachServiceNode(UEindx);
        pathloss = beamPairLink.rspIndBm - beamPairLink.rsrpIndBm;
        srsTXpowerInWattsForAllUEs(UEindx) = srsconfig(UEindx).srsTXpowerInWatts;
        
    }
    
    double nSamplesPerSlot = ((fftSize+80*(double)fftSize/1024.0)*2+(fftSize+72*(double)fftSize/1024.0)*12);            
    
    currentTimeInSec = (nSamplesPerSlot*slotIndx)*samplingTimeinSec;
    endTimeInSec = (nSamplesPerSlot*(slotIndx+1))*samplingTimeinSec;
    
    // Receiving SRS
    ChannelOutput_S channelOutput;
    
    Array<CSI_Feedback_Info_S> csiFeedbackInfo(0);
    
    
    for(int gNBindx = 0;gNBindx < gNBsForMCS.size();gNBindx++)
    {
        ivec associatedUEs=mcell.associationInfo.associatedServiceNodesPerServerNode(gNBsForMCS(gNBindx)); 
        Array<CSI_Feedback_Info_S> csiFeedbackInfoTemp(associatedUEs.length());
        vec srsTXSCPowerInWatts(associatedUEs.length());
        cout<<endl<<"Receiving by gNB"<<gNBsForMCS(gNBindx)<<endl;
        channelOutput = mcell.mySys.receiveData(gNBsForMCS(gNBindx),0,_ANTENNA_CORRELATION_MEDIUM_,0,currentTimeInSec,endTimeInSec,false,UE);
        
        ivec intrUEs = UE;
        ivec associatedUEIndices =find(UE,associatedUEs); 
        deleteInVec(intrUEs,associatedUEIndices);
        cout<<"intrUEs : "<<intrUEs<<endl;
        int nRxAntenna = mcell.mySys.aNodes(gNBsForMCS(gNBindx)).getAntennaCount();
        Array<cmat> intrResourceGridPerRxAntenna;

        
        if(intrUEs.length())
        {
            ChannelOutput_S intrChannelOutput;
            if (intrUEs.length())
                intrChannelOutput = mcell.mySys.receiveData(gNBsForMCS(gNBindx), 0, _ANTENNA_CORRELATION_MEDIUM_, 0, currentTimeInSec, endTimeInSec, false, intrUEs);
            intrResourceGridPerRxAntenna = getFreqDomainFrame(intrChannelOutput.data, fftSize, 0, 14);
        }
       
        
        // Channel estimation using SRS
        for(int UEindx = 0;UEindx < associatedUEIndices.size();UEindx++)
        {
            int assoc_gNB;
            assoc_gNB = gNBsForMCS(gNBindx);
            srsConfig srsConTemp;
            srsConTemp = srsconfig(associatedUEIndices(UEindx));
            
            //Extract Desired Channel 
            srsTXSCPowerInWatts(UEindx) = srsConTemp.srsTXpowerInWatts/(srsConTemp.nRBs*(12/srsConTemp.K_TC)*srsConTemp.nSRSports);
            Array<Array<cvec>> desiredChannel = channelOutput.channels(associatedUEIndices(UEindx));
            for(int d1_cnt=0;d1_cnt<desiredChannel.length();d1_cnt++)
                for(int d2_cnt=0;d2_cnt<desiredChannel(d1_cnt).length();d2_cnt++)
                {
                    //! Channel won't have any scaling - 10010
                    desiredChannel(d1_cnt)(d2_cnt)=dComplex(sqrt(srsTXSCPowerInWatts(UEindx)),0.0)*fft(desiredChannel(d1_cnt)(d2_cnt),fftSize);
                    desiredChannel(d1_cnt)(d2_cnt)=fftShift(desiredChannel(d1_cnt)(d2_cnt));
                }
                
            Array<cmat> freqDomainIdealChannel = getAcrossArray(desiredChannel);
            cmat IntrCov=zeros_c(nRxAntenna, nRxAntenna);
            
            //IntrCov Calculation in the SRS locations
            if(intrUEs.length())
            {
                int re_cnt=0;
                for(int portIndx = 0;portIndx < srsConTemp.nSRSports;portIndx++)
                    for(int symbIndx = 0;symbIndx < srsConTemp.symbols.size();symbIndx++)
                    {
                        int combOffset;
                        if(portIndx < 4)
                            combOffset = srsConTemp.combOffset;
                        else
                            combOffset = srsConTemp.combOffset+2;
                        
                        ivec scIndices=gc+getIntegers(0,srsConTemp.nRBs*12-1,srsConTemp.K_TC)+combOffset+(srsConTemp.rbOffset+symbIndx*srsConTemp.nRBs)*12;
                        
                        scIndices = mod(scIndices,48*12);
                        for(int scIndx = 0;scIndx < scIndices.size();scIndx++,re_cnt++)
                        {
                            cmat sym(nRxAntenna,1);
                            for(int rx_ant=0;rx_ant<nRxAntenna;rx_ant++)
                                sym(rx_ant,0)=intrResourceGridPerRxAntenna(rx_ant)(scIndices(scIndx),srsConTemp.symbols(symbIndx));
                            IntrCov=IntrCov+sym*sym.H();
                        }
                    }
                    
                IntrCov *= dComplex(1.0/(double)re_cnt,0.0);
            }
            else
            {
                double N0=mcell.mySys.channelCloud.getNoiseVariancePerDimension(gNBsForMCS(gNBindx))*2.0;
                IntrCov=N0*eye_c(nRxAntenna);
            }
            
            double meanIntrference = mean(abs(diag(IntrCov)));
            IntrCov=meanIntrference*eye_c(nRxAntenna);
            
//             cout << "\t Computing CSI .. " << endl;
            
            ivec scIndicesPerRB = gc+getIntegers(0,ulRBs*12,12);
            
            Array<cmat> channel = getFromArray(freqDomainIdealChannel,scIndicesPerRB);
//             cout<<"IntrCov : "<<IntrCov<<endl;
            int nUEAntenna = channel(0).cols();
            int nLayers = nUEAntenna;
            int prgSize = 4;
            csiFeedbackInfoTemp(UEindx) = doLinkAdaptationforUL(channel, ulRBs, prgSize,nUEAntenna,nLayers, 1, IntrCov, tbTables, codebook, nLayers);
            
        }
        append(csiFeedbackInfo,csiFeedbackInfoTemp);
    }
    
    return csiFeedbackInfo;
}
                
           
