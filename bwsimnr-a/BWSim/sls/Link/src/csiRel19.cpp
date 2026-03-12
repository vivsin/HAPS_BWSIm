#include "../include/csi.h"

extern cTBTables tbTables;
extern AWGNTablesConvCoder awgnTables;
extern Codebook codebook;
extern Codebook codebookTypeII;
extern Codebook codebookTypeIIEnhanced;
extern ChannelEstimator channelEstimator;




Array<CSI_Feedback_Info_S> fReturnCurrentMCSperUERel19(int slotIndx,MCell &mcell,ivec uesForMCS, double txPowerIndBm)
{
    Array<CSI_Feedback_Info_S> csiFeedbackInfoRel19(uesForMCS.length());
    
    bool isDownlink = true;
    string dirPrefix = "./configFiles/NU1/LTETables/";
    CodeBookConfig_S codebookConfig = codebook.dlCodeBookConfig;
    int nCSIRSPorts = codebook.dlCodeBookConfig.nCSIRSPorts;
    
    int fftSize = 1024;
    int scsInKhz = 15;
    int dlRBs = 52;
    int prgSize = 4;
    int gc = (fftSize-dlRBs*12)/2;
    double samplingTimeinSec = 1/(double)(fftSize*scsInKhz*1000);	//Parameters are hardcoded
    double nSamplesPerSlot = ((fftSize+80*(double)fftSize/1024.0)*2+(fftSize+72*(double)fftSize/1024.0)*12);            
    ivec scIndices;
    scIndices = getIntegers(0,dlRBs*12-1,12);
    scIndices = gc+mod(scIndices,dlRBs*12);
    
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
    ivec nLayersPerUE=zeros_i(uesForMCS.length());
    
    vec specEffToBeReturnedTypeII = zeros(uesForMCS.length());
        vec specEffToBeReturnedTypeIIEnhanced = zeros(uesForMCS.length());

    ivec nLayersPerUETypeII=zeros_i(uesForMCS.length());
    ivec nLayersPerUETypeIIEnhanced=zeros_i(uesForMCS.length());
    vec avgSpecEffPerUE=zeros(UE.length());
    vec NoSelectionSpecEff=zeros(UE.length());
    

    currentTimeInSec = (nSamplesPerSlot*slotIndx)*samplingTimeinSec;
    endTimeInSec = (nSamplesPerSlot*(slotIndx+1))*samplingTimeinSec;
    
    NRCodeBookType cbType = codebook.dlCodeBookConfig.codeBookType;
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

            Array<cvec> timeDomainFramePerResource = getTimeDomainFrame(resourceGridPerTxAntenna,fftSize);
            for(int portIndx1 = 0;portIndx1 < timeDomainFramePerResource.length();portIndx1++){
                timeDomainFrame(rsrc_cnt*nCSIRSPortsPerResource+portIndx1).set_subvector(rsrc_cnt*nSamplesPerSlot,sqrt(inv_dB(txPowerIndBm-30))*timeDomainFramePerResource(portIndx1));
            }
        }
        mcell.mySys.transmitData(gNB(gNBindx),0,fftSize,timeDomainFrame,currentTimeInSec,1);
    }
    
    //Rx and CSI measurement Loop
    ivec UEindices = find(UE,uesForMCS);
    for(int ue = 0; ue < uesForMCS.size(); ue++)
    {
        int UEindx = UEindices(ue);
        int associatedgNB  = mcell.associationInfo.associatedServerNodesForEachServiceNode(UEindx);
        int gNBIndx = find(gNB,associatedgNB);
        int ueAntenna = mcell.mySys.getNodeAntennaCountFromList(UE(UEindx,UEindx))(0);
        int gNBAntenna = mcell.mySys.getNodeAntennaCountFromList(to_ivec(associatedgNB))(0);
        
        cout<<"Computing CQI for UE : "<<UE(UEindx)<<endl;
        
        
        cmat IntrCov(0,0);
        Array<Array<cmat>> idealChannelPerCSIresource(nCSIResources);

        cout<<"\t Receiving Signal .. "<<endl;
        
        for(int rsrc_cnt=0;rsrc_cnt<nCSIResources;rsrc_cnt++){
            
            //Interference covariance computation 
            // Equivalent of performing YY* in CSI-IM
            
            ivec intrgNBs = gNB;
            intrgNBs.del(gNBIndx);
            if(intrgNBs.length())
            {
                ChannelOutput_S channelOutput = mcell.mySys.receiveData(UE(UEindx),0,_ANTENNA_CORRELATION_MEDIUM_,0,currentTimeInSec+rsrc_cnt*1e-3,endTimeInSec+rsrc_cnt*1e-3,false,intrgNBs);
                Array<cmat> resourceGridPerRxAntenna = getFreqDomainFrame(channelOutput.data,fftSize,0,14);
                IntrCov = computeRxIntrCov(dlRBs,resourceGridPerRxAntenna,allocatedRBs,csi_output(gNBIndx).rbRSPattern,(double)nCSIRSPorts/(double)csi_output(gNBIndx).nCDM); 
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
                
                Array<Array<cvec>> idealChannelFD(nCSIRSPortsPerResource);
                for(int txAntIndx = 0;txAntIndx < idealChannelFD.length();txAntIndx++){
                    idealChannelFD(txAntIndx).set_length(idealChannelTD(0).length());
                    for(int rxAntIndx = 0;rxAntIndx < idealChannelTD(0).length();rxAntIndx++){
                        cvec temp = fft(sqrt(inv_dB(txPowerIndBm-30))*idealChannelTD(txAntIndx+(nCSIRSPortsPerResource*rsrc_cnt))(rxAntIndx),fftSize);
                        temp = fftShift(temp);
                        idealChannelFD(txAntIndx)(rxAntIndx) = temp(scIndices);
                    }
                }
                idealChannelPerCSIresource(rsrc_cnt) = getAcrossArray(idealChannelFD);
                
            }
            
        }
        
        Array<cmat> idealChannel(52);
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
        
        cout<<"\t Computing CSI .. "<<endl;
        
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
                switch(cbType)
                {
                    case _NR_CODEBOOK_TYPE_I_:
                    case _NR_CODEBOOK_TYPE_I_MULTIPANEL_:
                        
//                         csiFeedbackInfo(ue) = doLinkAdaptationforTypeII(channel,finalCoefficients,dlRBs,prgSize,maxRank,1,IntrCov,tbTables,codebookTypeII,nLayers,cqi);
//                         specEffToBeReturnedTypeII(ue) = csiFeedbackInfo(ue).specEffWideband;
//                         nLayersPerUETypeII(ue) = csiFeedbackInfo(ue).nLayers;
//                         
//                         csiFeedbackInfo(ue) = doLinkAdaptationforTypeII(channel,finalCoefficients,dlRBs,prgSize,maxRank,1,IntrCov,tbTables,codebookTypeIIEnhanced,nLayers,cqi);
//                         specEffToBeReturnedTypeIIEnhanced(ue) = csiFeedbackInfo(ue).specEffWideband;
//                         nLayersPerUETypeIIEnhanced(ue) = csiFeedbackInfo(ue).nLayers;
//                         
                        
                        csiFeedbackInfoRel19(ue) = computeSEforType1Simplifed(codebookConfig,idealChannel,IntrCov,tbTables,dlRBs,prgSize,1,maxRank);
                        specEffToBeReturned(ue) = csiFeedbackInfoRel19(ue).specEffWideband;
                        nLayersPerUE(ue) = csiFeedbackInfoRel19(ue).nLayers;
                        
                        break;
                    case _NR_CODEBOOK_TYPE_II_:
                    case _NR_CODEBOOK_TYPE_II_ENHANCED_:
                    case _NR_CODEBOOK_PORT_SELECTION_:
                    case _NR_CODEBOOK_PORT_SELECTION_ENHANCED_:
                    case _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_: 
                        csiFeedbackInfoRel19(ue) = doLinkAdaptationforTypeII(channel,finalCoefficients,dlRBs,prgSize,maxRank,1,IntrCov,tbTables,codebook,nLayers,cqi);
                        break;
                    default:
                        cout<<"Invalid Codebook Type..."<<endl;
                        abort();
                }
            }
            //cout<<endl<<"ue: "<<ue<<", se: "<<csiFeedbackInfo(ue).specEffWideband<<endl;
            
            avgSpecEffPerUE(UEindx) += csiFeedbackInfoRel19(ue).specEffWideband;
            
        }
    }
    cout<<"UEs: "<<uesForMCS<<endl;
   
    cout<<"SpecEff TypeI : "<<specEffToBeReturned<<endl;
    cout<<"Layers TypeI : "<<nLayersPerUE<<endl;
    
//     cout<<"SpecEff TypeII : "<<specEffToBeReturnedTypeII<<endl;
//     cout<<"Layers TypeII : "<<nLayersPerUETypeII<<endl;
//     
//     cout<<"SpecEff TypeII Enhanced : "<<specEffToBeReturnedTypeIIEnhanced<<endl;
//     cout<<"Layers TypeII Enhanced : "<<nLayersPerUETypeIIEnhanced<<endl;
    abort();
    return csiFeedbackInfoRel19;
}
