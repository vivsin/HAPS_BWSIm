#include <iostream>
#include "../../../lib/L2/MAC/include/MAC.h"
#include "../../Link/include/csi.h"
#include "../../Link/include/sch.h"

#include <fstream>
#include <chrono>
#include <sstream>

#include "../../Mcell/include/MCell.h"
#include "../../../lib/L1/SRS/include/srsTx.h"
#include "../../../lib/L1/SRS/include/srsRx.h"
// #include "../../Mcell/include/MCell.h"
#include "../../sim/include/SupportingFunctions.h"

#include <omp.h>

cTBTables tbTables;
NRTables_S nrTables;

AWGNTablesConvCoder awgnTables;
Codebook codebook;
ChannelEstimator channelEstimator;
PowerControl_S powerControl;

MCell initBWSim(int argc, char **argv)
{
    #ifdef USING_MPI
    initMPI(&(nTasks), &(currentTaskid), argc, argv);
    MPI_Barrier(MPI_COMM_WORLD);
    #endif
    
    ConfigFileNames_S myConfigFiles;
    ResultFileNames_S resultFiles;
    initAllFiles(argc, argv, myConfigFiles, resultFiles);
    MCell mcell = createMcellNetwork(myConfigFiles, resultFiles);
    
    if ((argc > 1 && strcmp(argv[1], "mcell") == 0))
    {
        #ifdef USING_MPI
        MPI_Finalize();
        #endif
        exit(0);
    }
    return mcell;
    
}



int main(int argc, char *argv[])
{
    MCell mcell=initBWSim(argc, argv);
        
    bool isDownlink = false;
    bool isPF = true;
    bool enableTraffic=false;
    
    SystemParams_S sysParams(10,14,1024,15,8,48,5);//slots, symbols, fftsize,  scsinkhz, rbgSize, rbs, csiFreq
    int nSlots=sysParams.nSlots;
    int nSymbols = sysParams.nSymbols;
    int nSubcarriers = sysParams.fftSize;
    int RBGSize = sysParams.RBGSize;
    int nRBs = sysParams.nRBs;
    int nRBGs = sysParams.nRBGs;
    int nREsPerRBG = sysParams.nREsPerRBG;
    int csiFrequency = sysParams.csiFrequency;
    
    
    int rbBW = 12*sysParams.scsInKhz*1000;
    double slotDuration = 1e-3/15*sysParams.scsInKhz;
    
    string dirPrefix = "./configFiles/NU1/LTETables/";
    
    nrTables.init();
    awgnTables.init_AWGNTables(isDownlink, dirPrefix + "transportBlockMCSMapping.txt", dirPrefix + "cqiToMaxMCSRate.txt");
    tbTables.load(isDownlink, dirPrefix + "interleaverParameters.txt", dirPrefix + "transportBlockMCSMapping.txt", dirPrefix + "tbsTranslation.txt", dirPrefix + "cqiToMaxMCSRate.txt", awgnTables);
    codebook.dlCodeBookConfig.parseCodeBookConfig("./configFiles/NU1/myDLTxRxConfig.txt","./configFiles/ActiveAntennaSystemConfig.txt");
    codebook.initializeDlCodebook();
    codebook.initializeUlCodebook();
    
    vec nodeTransmitPowerInDBm;
    parse("./configFiles/mySysConfig.txt","nodeTransmitPowerInDBm",nodeTransmitPowerInDBm);
    double txPowerIndBm = nodeTransmitPowerInDBm(1);
    cout<<"Using TxPow : "<<txPowerIndBm<<endl;
    
    Array<Array<string>> trafficTypeParsedString;
    parse("../sls/configFiles/NU1/myL2Support.txt","mTrafficType",trafficTypeParsedString);
    // parse(sL2SimConfig,"mTrafficType",trafficTypeParsedString);
    eTrafficType trafficTypeCommon = freturnTrafficType(trafficTypeParsedString(0)(0));
    
        
    bool isWithAbstraction;
    parse("./configFiles/mySimulationConfig.txt","isWithAbstraction",isWithAbstraction);
    
    
    
    /*Initialising gNBs and UEs associated with each*/
    int numGnb = mcell.associationInfo.serverNodes.length();
    ivec gNBs = mcell.associationInfo.serverNodes;
    Array<cMAC> MACObjs(numGnb);
    
    //     cout<<avgSpecEffperUE<<endl;
    ivec uesForMCS(0);
    
    int nSRSports=mcell.mySys.aNodes(numGnb+1).getAntennaCount();
    Array<srsConfig> srsconfig;
    srsconfig = generateSRSconfig(mcell.associationInfo,nSRSports);
    
    Array<vec> rxBitsPerUE(numGnb);
    Array<vec> bler(numGnb);
    Array<ivec> txBlockCount(numGnb);
    Array<ivec> freshTxAckBlockCount(numGnb);
    Array<vec> rbUtil(numGnb);
    Array<vec> bwTimeProduct(numGnb);
    
    for(int gnBIndx=0; gnBIndx < numGnb; gnBIndx++)
    {
        cout<< " ***************** gnb "<<gnBIndx<<" *****************"<<endl;
        ivec availableUEs = mcell.associationInfo.associatedServiceNodesPerServerNode(gnBIndx);
        ivec ueIndices = find(mcell.associationInfo.serviceNodes,availableUEs);
        append(uesForMCS,availableUEs);
        
        MACObjs(gnBIndx) = cMAC(gnBIndx, nSlots, availableUEs,trafficTypeCommon);
        MACObjs(gnBIndx).myScheduler.setParams(isDownlink,RBGSize);
        
        rxBitsPerUE(gnBIndx) = zeros(availableUEs.length());
        bler(gnBIndx) = zeros(availableUEs.length());
        txBlockCount(gnBIndx) = zeros_i(availableUEs.length());
        freshTxAckBlockCount(gnBIndx) = zeros_i(availableUEs.length());
        rbUtil(gnBIndx) = zeros(availableUEs.length());
        bwTimeProduct(gnBIndx) = zeros(availableUEs.length());
        
        vec pathloss = zeros(availableUEs.length());
        
        for(int UEindx = 0;UEindx < availableUEs.size();UEindx++)
        {
            sBeamPairLink beamPairLink;
            beamPairLink = mcell.associationInfo.associatedBeamPairLinkforEachServiceNode(ueIndices(UEindx));
            pathloss(UEindx) = beamPairLink.rspIndBm - beamPairLink.rsrpIndBm;
        }
        
        MACObjs(gnBIndx).myScheduler.powerControl.init(0.8, srsconfig(0).p0dBm, txPowerIndBm, availableUEs, pathloss);
    }
    vec pathloss(mcell.associationInfo.serviceNodes.length());
    for(int UEindx = 0;UEindx < pathloss.size();UEindx++)
    {
        sBeamPairLink beamPairLink;
        beamPairLink = mcell.associationInfo.associatedBeamPairLinkforEachServiceNode(UEindx);
        pathloss(UEindx) = beamPairLink.rspIndBm - beamPairLink.rsrpIndBm;
    }
    powerControl.init(0.8, srsconfig(0).p0dBm, txPowerIndBm, mcell.associationInfo.serviceNodes, pathloss);
    
    Array<CSI_Feedback_Info_S> csiFeedbackInfoPerUE(uesForMCS.length());
    Array<Array<CSI_Feedback_Info_S>> csiFeedbackInfoPerBSPerUE(numGnb);
    
    Array<CSI_Feedback_Info_S> csiFeedbackInfoPerUERel19;    
    Array<Array<vec>> currMCS(numGnb);
    Array<vec> currMeanMCS(numGnb);
    
    
    Array<vec> avgMCS(numGnb);
    for(int gnBIndx=0; gnBIndx < numGnb; gnBIndx++)
        avgMCS(gnBIndx)=zeros(mcell.associationInfo.associatedServiceNodesPerServerNode(gnBIndx).length());
    
    ivec sltIndxVec = randi(nSlots,0,10000);
    sort(sltIndxVec);
    
    for (int slot_cnt = 0; slot_cnt < nSlots; slot_cnt++)
    {
        
//         int slotIndx = sltIndxVec(slot_cnt) ;
        int slotIndx = slot_cnt ;
        
        cout<<"****************************************************** Instant "<<slotIndx<<" **********************************************************"<<endl;
        
        // CSI Computation //////////////////////////////////////////////////////////////////////////////////////
        if(slotIndx%csiFrequency == 0)
        {
            int gNBForCSI = numGnb;
            if(slotIndx>0)
                gNBForCSI = 1;
            
            fTransmitSRS_UL(sysParams,slotIndx,mcell,srsconfig);
            
            int nThreads = numGnb;
            if(nThreads > omp_get_num_procs())
                nThreads = omp_get_num_procs();
            cout<<"Using "<<nThreads<<" threads for CSI computation "<<endl;
            
            omp_set_num_threads(nThreads); 
            {
                #pragma omp parallel for
                for(int gnBIndx=0; gnBIndx < gNBForCSI; gnBIndx++)
                {
                    printf(" Computing CSI in Thread %d\n", omp_get_thread_num());
                    csiFeedbackInfoPerBSPerUE(gnBIndx) = fReturnCurrentMCSperUE_UL(sysParams,slot_cnt,mcell,mcell.associationInfo.serverNodes(gnBIndx,gnBIndx),srsconfig);
                }
            }
            for(int gnBIndx=0, ueIndx=0; gnBIndx < gNBForCSI; gnBIndx++)
            {
                csiFeedbackInfoPerUE.set_subarray(ueIndx,ueIndx+csiFeedbackInfoPerBSPerUE(gnBIndx).length()-1,csiFeedbackInfoPerBSPerUE(gnBIndx));
                ueIndx+=csiFeedbackInfoPerBSPerUE(gnBIndx).length();
            }
            
//             csiFeedbackInfoPerUE = fReturnCurrentMCSperUE_UL(slotIndx,mcell,mcell.associationInfo.serverNodes,srsconfig);
        }
        //csiFeedbackInfoPerUE=csiFeedbackInfoPerUERel19;
        //Scheduling ///////////////////////////////////////////////////////////////////////////////////////////
        Array<schInfo_S> schInfo(numGnb);
        Array<Array<HarqInfo>> harqInfoPerAssociatedUE(numGnb);
        
        for(int gnBIndx=0; gnBIndx < numGnb; gnBIndx++)
        {
            ivec availableUEs = mcell.associationInfo.associatedServiceNodesPerServerNode(gnBIndx);
            ivec ueIndices = find(uesForMCS,availableUEs);
            Array<CSI_Feedback_Info_S> associatedCSIFeedback = getFromArray(csiFeedbackInfoPerUE,ueIndices);
            MACObjs(gnBIndx).myScheduler.latestCSIFeedbackInfoPerAssociatedUE = associatedCSIFeedback;
            
            currMCS(gnBIndx).set_length(ueIndices.length());
            currMeanMCS(gnBIndx)=zeros(ueIndices.length());
            
            vec srsTXSCPowerInWatts(availableUEs.length()); 
            for(int UEindx = 0;UEindx < availableUEs.size();UEindx++)
                srsTXSCPowerInWatts(UEindx) = srsconfig(ueIndices(UEindx)).srsTXpowerInWatts/(srsconfig(ueIndices(UEindx)).nRBs*(12/srsconfig(ueIndices(UEindx)).K_TC)*srsconfig(ueIndices(UEindx)).nSRSports);
                
            for(int ue_cnt=0;ue_cnt<ueIndices.length();ue_cnt++)
            {
            	currMCS(gnBIndx)(ue_cnt) = associatedCSIFeedback(ue_cnt).specEffPerSubband;
                currMeanMCS(gnBIndx)(ue_cnt) = mean(associatedCSIFeedback(ue_cnt).specEffPerSubband);
            }
            ivec bufferSizesPerAttachedUE(0);
            if(enableTraffic)
            {
                MACObjs(gnBIndx).fInsertCurrentSlotPackets(slotIndx);
                bufferSizesPerAttachedUE = MACObjs(gnBIndx).fBufferSizesPerAttachedUE();
                
            }
            
            ivec codewordsRequired(availableUEs.length());
            for(int ue_cnt=0;ue_cnt<availableUEs.length();ue_cnt++)
                codewordsRequired(ue_cnt) = (associatedCSIFeedback(ue_cnt).nLayers>4) ? 2 : 1;
            
            harqInfoPerAssociatedUE(gnBIndx) = collectHARQInfo(MACObjs(gnBIndx).harqManager,slot_cnt,codewordsRequired);
            
            avgMCS(gnBIndx) = avgMCS(gnBIndx) + currMeanMCS(gnBIndx);
            Array<vec> ratePerRBGPerUE = swapDimensions(currMCS(gnBIndx));
            schInfo(gnBIndx) = MACObjs(gnBIndx).myScheduler.schedule(nRBGs,isPF,availableUEs,ratePerRBGPerUE,harqInfoPerAssociatedUE(gnBIndx),bufferSizesPerAttachedUE,srsTXSCPowerInWatts);
            
        }
        
        Array<Array<SCHGrant_S>> grantInfo = generateGrantInfo(nRBs,RBGSize,nSymbols,schInfo,uesForMCS,csiFeedbackInfoPerUE,mcell.associationInfo.associatedServiceNodesPerServerNode,harqInfoPerAssociatedUE);
            
        Array<Array<DecoderOutput_S>> decoderOutput(numGnb);
        
        if(!isWithAbstraction)
        {
            transmitPUSCH(mcell, MACObjs, slotIndx,nSubcarriers, nSymbols, nRBs , grantInfo,1);
            for(int gNB_cnt=0;gNB_cnt<numGnb;gNB_cnt++)
            {
                bool withActualReception = (gNB_cnt==0);
                Array<cmat> intCovPerUE(grantInfo(gNB_cnt).length());
                ChannelOutput_S channelOutput;
                decoderOutput(gNB_cnt).set_length(grantInfo(gNB_cnt).length());
                
                if(withActualReception)
                    channelOutput = receivePUSCH(mcell, gNB_cnt, slotIndx,nSubcarriers, nSymbols, nRBs , grantInfo,intCovPerUE);
                
                int nUE = grantInfo(gNB_cnt).length();
                
                int nThreads = nUE;
                if(nThreads > omp_get_num_procs())
                    nThreads = omp_get_num_procs();
                
                cout<<"Using "<<nThreads<<" threads for decoding under gNB : "<<gNB_cnt<<" withActualReception : "<<withActualReception<<endl;
                omp_set_num_threads(nThreads); 
                {
                    #pragma omp parallel for
                    for(int ue_cnt=0;ue_cnt<grantInfo(gNB_cnt).length();ue_cnt++)
                    {
                        decoderOutput(gNB_cnt)(ue_cnt) = processPUSCH(mcell, MACObjs(gNB_cnt), channelOutput, intCovPerUE(ue_cnt), slotIndx,nSubcarriers, nSymbols, nRBs , grantInfo(gNB_cnt)(ue_cnt),withActualReception);
                    }
                }
            }
        }
        
        for(int gNB_cnt=0;gNB_cnt<numGnb;gNB_cnt++)
        {
            ivec availableUEs = mcell.associationInfo.associatedServiceNodesPerServerNode(gNB_cnt);
            bvec isSuccess;
            
            for(int ue_cnt=0;ue_cnt<grantInfo(gNB_cnt).length();ue_cnt++)
            {
                if(isWithAbstraction)
                    isSuccess = randu(grantInfo(gNB_cnt)(ue_cnt).harqInfo.TBSizePerCodewordInBits.length())>0.1;
                else
                    isSuccess = decoderOutput(gNB_cnt)(ue_cnt).isSuccess;
                
                int ueIndx = find(availableUEs , grantInfo(gNB_cnt)(ue_cnt).ueID);
                
                rxBitsPerUE(gNB_cnt)(ueIndx)=rxBitsPerUE(gNB_cnt)(ueIndx)+sum(elem_mult(to_ivec(isSuccess),grantInfo(gNB_cnt)(ue_cnt).harqInfo.TBSizePerCodewordInBits));
            
                txBlockCount(gNB_cnt)(ueIndx) += sum(to_ivec(grantInfo(gNB_cnt)(ue_cnt).harqInfo.isNewTransmission));
                
                freshTxAckBlockCount(gNB_cnt)(ueIndx) += sum(elem_mult(to_ivec(isSuccess),to_ivec(grantInfo(gNB_cnt)(ue_cnt).harqInfo.isNewTransmission)));
                
                rbUtil(gNB_cnt)(ueIndx) +=grantInfo(gNB_cnt)(ue_cnt).allocatedRBs.length();
                
                bwTimeProduct(gNB_cnt)(ueIndx) +=(grantInfo(gNB_cnt)(ue_cnt).allocatedRBs.length()*rbBW*slotDuration);
                
                
            }
        }
        
    }
    
    Array<vec> upt(numGnb);
    Array<vec> uSE(numGnb);
    
    vec cellThrpt(numGnb);
    
    vec allUPT(0);
    vec allSE(0);
    
    
    for(int gnBIndx=0; gnBIndx < numGnb; gnBIndx++)
    {
        upt(gnBIndx) = rxBitsPerUE(gnBIndx)/(nSlots*1e-3);
        uSE(gnBIndx) = elem_div(rxBitsPerUE(gnBIndx),bwTimeProduct(gnBIndx));
        bler(gnBIndx) = elem_div(to_vec(txBlockCount(gnBIndx)-freshTxAckBlockCount(gnBIndx)) , to_vec(txBlockCount(gnBIndx))) ;
        
        ivec zeroIndices=find(txBlockCount(gnBIndx),0,true);
        setInVec(uSE(gnBIndx),zeroIndices,0.0);
        setInVec(bler(gnBIndx),zeroIndices,0.0);
        
        rbUtil(gnBIndx) = rbUtil(gnBIndx) / (nSlots*sysParams.nRBs);
        
        cellThrpt(gnBIndx) = sum(rxBitsPerUE(gnBIndx))/(nSlots*1e-3);
        avgMCS(gnBIndx) = avgMCS(gnBIndx) / nSlots;
        
        append(allUPT,upt(gnBIndx));
        append(allSE,avgMCS(gnBIndx));
        
    }
    
    mat allUPTMat = mat(allUPT);
    mat allSEMat = mat(allSE);
    mat allOutput = concat_horizontal(allUPTMat,allSEMat);
    
    cout<<"UPT : "<<upt(0)<<endl;
    cout<<"SE : "<<uSE(0)<<endl;
    
    cout<<"Bler : "<<bler(0)<<endl;
    cout<<"Tx Block Count : "<<txBlockCount(0)<<endl;
    cout<<"Fresh Ack Count : "<<freshTxAckBlockCount(0)<<endl;
    
    cout<<"RB Utilization : "<<rbUtil(0)<<endl;
    
    
    cout<<"AvgMCS : "<<avgMCS(0)<<endl;
    cout<<"cell Thrpt : "<<cellThrpt(0)<<endl;
    
    
    ofstream fout;
    fout.open("./Results/UPT",ios::out);
    fout<<allOutput<<endl;
    fout.close();
    
    cout<<briefSimulationDetails(nSlots*1e-3)<<endl;
}
