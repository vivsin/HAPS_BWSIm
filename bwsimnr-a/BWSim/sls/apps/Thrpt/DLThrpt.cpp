// #include <iostream>
#include "../../Link/include/csi.h"
#include "../../Link/include/sch.h"

#include <fstream>
#include <chrono>
#include <sstream>
#include "../../../lib/L2/MAC/include/MAC.h"
#include "../../sim/include/SupportingFunctions.h"

#include "../../Mcell/include/MCell.h"
#include <omp.h>



cTBTables tbTables;
AWGNTablesConvCoder awgnTables;
NRTables_S nrTables;
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

    bool isDownlink = true;
    bool isPF = true;
    bool enableTraffic=false;
    
    string dirPrefix = "./configFiles/NU1/LTETables/";

    nrTables.init();
    awgnTables.init_AWGNTables(isDownlink, dirPrefix + "transportBlockMCSMapping.txt", dirPrefix + "cqiToMaxMCSRate.txt");
    tbTables.load(isDownlink, dirPrefix + "interleaverParameters.txt", dirPrefix + "transportBlockMCSMapping.txt", dirPrefix + "tbsTranslation.txt", dirPrefix + "cqiToMaxMCSRate.txt", awgnTables);
    codebook.dlCodeBookConfig.parseCodeBookConfig("./configFiles/NU1/myDLTxRxConfig.txt","./configFiles/ActiveAntennaSystemConfig.txt");
    codebook.initializeDlCodebook();
    codebook.initializeUlCodebook();

    vec nodeTransmitPowerInDBm;
    parse("./configFiles/mySysConfig.txt","nodeTransmitPowerInDBm",nodeTransmitPowerInDBm);
    double txPowerIndBm = nodeTransmitPowerInDBm(0);
    cout<<"Using TxPow : "<<txPowerIndBm<<endl;
    Array<Array<string>> trafficTypeParsedString;
    parse("../sls/configFiles/NU1/myL2Support.txt","mTrafficType",trafficTypeParsedString);
    // parse(sL2SimConfig,"mTrafficType",trafficTypeParsedString);
    eTrafficType trafficTypeCommon = freturnTrafficType(trafficTypeParsedString(0)(0));
    
        
    bool isWithAbstraction;
    parse("./configFiles/mySimulationConfig.txt","isWithAbstraction",isWithAbstraction);
    
    
    /*Initialising gNBs and UEs associated with each*/
    int numGnb = mcell.associationInfo.serverNodes.length();
    int numUE = mcell.associationInfo.serviceNodes.length();
    
    
    ivec uesForMCS(0);
//     vec avgSpecEffperUE = fCSIRSmain(mcell);
//     cout<<avgSpecEffperUE<<endl;
    Array<cMAC> MACObjs(numGnb);
    // Array<Scheduler_C> schedulerPergNB(numGnb);
    Array<vec> rxBitsPerUE(numGnb);
    Array<vec> bler(numGnb);
    Array<ivec> txBlockCount(numGnb);
    Array<ivec> freshTxAckBlockCount(numGnb);
    Array<vec> rbUtil(numGnb);
    Array<vec> bwTimeProduct(numGnb);
    
    
    SystemParams_S sysParams(10,14,1024,15,4,48,5); //slots, symbols, fftsize,  scsinkhz, rbgSize, rbs, csiFreq
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
    
    for(int gnBIndx=0; gnBIndx < numGnb; gnBIndx++)
    {
        cout<< " ***************** gnb "<<gnBIndx<<" *****************"<<endl;
        ivec availableUEs = mcell.associationInfo.associatedServiceNodesPerServerNode(gnBIndx);
        //instantiating gNB objects. Associated UEs are also instantiated along with their traffic type
        MACObjs(gnBIndx) = cMAC(gnBIndx, nSlots, availableUEs,trafficTypeCommon);
        MACObjs(gnBIndx).myScheduler.setParams(isDownlink,RBGSize);
        
        append(uesForMCS,availableUEs);
        // schedulerPergNB(gnBIndx)=Scheduler_C(0.001,availableUEs);
        rxBitsPerUE(gnBIndx) = zeros(availableUEs.length());
        bler(gnBIndx) = zeros(availableUEs.length());
        txBlockCount(gnBIndx) = zeros_i(availableUEs.length());
        freshTxAckBlockCount(gnBIndx) = zeros_i(availableUEs.length());
        rbUtil(gnBIndx) = zeros(availableUEs.length());
        bwTimeProduct(gnBIndx) = zeros(availableUEs.length());
        
        
    }
    
    
    Array<CSI_Feedback_Info_S> csiFeedbackInfoPerUE(uesForMCS.length());
    Array<Array<CSI_Feedback_Info_S>> csiFeedbackInfoPerBSPerUE(numGnb);
    for(int gnBIndx=0; gnBIndx < numGnb; gnBIndx++)
    {
        int nUE = mcell.associationInfo.associatedServiceNodesPerServerNode(gnBIndx).length();
        csiFeedbackInfoPerBSPerUE(gnBIndx).set_length(nUE);
    }
    
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
        cout<<"****************************************************** Instant "<<slot_cnt<<" **********************************************************"<<endl;
        
        // CSI Computation //////////////////////////////////////////////////////////////////////////////////////
        // int sltIndx;
        // sltIndx=randi(0,10000);
        // cout<<endl<<"slot_cnt: "<<sltIndx<<endl;
        int sltIndx = slot_cnt ; // 1ms per slot
        cout << endl << "slot_cnt: " << slot_cnt << ", time: " << sltIndx << "s" << endl;
        if(slot_cnt%csiFrequency == 0)
        {
            int gNBForCSI = numGnb;
            if(slot_cnt>0)
                gNBForCSI = 1;
            
            ivec uesForCSI(0);
            for(int gnBIndx=0; gnBIndx < gNBForCSI; gnBIndx++)
                append(uesForCSI,mcell.associationInfo.associatedServiceNodesPerServerNode(gnBIndx));
            ivec ueIndicesForCSI = find(mcell.associationInfo.serviceNodes,uesForCSI);
            
            fTransmitCSIRS_DL(sysParams,sltIndx,mcell,txPowerIndBm);
            
            int nThreads = uesForCSI.length();
            if(nThreads > omp_get_num_procs())
                nThreads = omp_get_num_procs();
            cout<<"Using "<<nThreads<<" threads for CSI computation "<<endl;
            
            omp_set_num_threads(nThreads); 
            {
                #pragma omp parallel for  
                for(int totUE_cnt=0; totUE_cnt < uesForCSI.length(); totUE_cnt++)
                {
                    printf(" Computing CSI in Thread %d\n", omp_get_thread_num());
                    int gnBIndx = mcell.associationInfo.associatedServerNodesForEachServiceNode(ueIndicesForCSI(totUE_cnt));
                    int ue_cnt = find(mcell.associationInfo.associatedServiceNodesPerServerNode(gnBIndx),uesForCSI(totUE_cnt));
                    csiFeedbackInfoPerBSPerUE(gnBIndx)(ue_cnt) = fReturnCurrentMCSperUE_DL(sysParams,sltIndx,mcell,mcell.associationInfo.associatedServiceNodesPerServerNode(gnBIndx)(ue_cnt,ue_cnt),txPowerIndBm)(0);
                }
               
            }
            
            for(int gnBIndx=0, ueIndx=0; gnBIndx < gNBForCSI; gnBIndx++)
            {
                csiFeedbackInfoPerUE.set_subarray(ueIndx,ueIndx+csiFeedbackInfoPerBSPerUE(gnBIndx).length()-1,csiFeedbackInfoPerBSPerUE(gnBIndx));
                ueIndx+=csiFeedbackInfoPerBSPerUE(gnBIndx).length();
                
            }
            //csiFeedbackInfoPerUERel19=fReturnCurrentMCSperUERel19(slot_cnt,mcell,uesForMCS,txPowerIndBm);
        }
        //csiFeedbackInfoPerUE=csiFeedbackInfoPerUERel19;
        
        //Scheduling ///////////////////////////////////////////////////////////////////////////////////////////
        Array<schInfo_S> schInfo(numGnb);
        Array<Array<HarqInfo>> harqInfoPerAssociatedUE(numGnb);
        
        for(int gnBIndx=0; gnBIndx < numGnb; gnBIndx++)
        {
            ivec availableUEs = mcell.associationInfo.associatedServiceNodesPerServerNode(gnBIndx);
            ivec ueIndices = find(uesForMCS,availableUEs);
            ivec bufferSizesPerAttachedUE;
            if(enableTraffic)
            {
                MACObjs(gnBIndx).fInsertCurrentSlotPackets(slot_cnt);
                bufferSizesPerAttachedUE = MACObjs(gnBIndx).fBufferSizesPerAttachedUE();
            }
            
            Array<CSI_Feedback_Info_S> associatedCSIFeedback = getFromArray(csiFeedbackInfoPerUE,ueIndices);
            MACObjs(gnBIndx).myScheduler.latestCSIFeedbackInfoPerAssociatedUE = associatedCSIFeedback;
            
            ivec codewordsRequired(availableUEs.length());
            for(int ue_cnt=0;ue_cnt<availableUEs.length();ue_cnt++)
                codewordsRequired(ue_cnt) = (associatedCSIFeedback(ue_cnt).nLayers>4) ? 2 : 1;
            
            harqInfoPerAssociatedUE(gnBIndx) = collectHARQInfo(MACObjs(gnBIndx).harqManager,slot_cnt,codewordsRequired);
            
            currMCS(gnBIndx).set_length(ueIndices.length());
            currMeanMCS(gnBIndx)=zeros(ueIndices.length());
            for(int ue_cnt=0;ue_cnt<ueIndices.length();ue_cnt++)
            {
                currMCS(gnBIndx)(ue_cnt) = associatedCSIFeedback(ue_cnt).specEffPerSubband;
                currMeanMCS(gnBIndx)(ue_cnt) = mean(associatedCSIFeedback(ue_cnt).specEffPerSubband);
            }
            avgMCS(gnBIndx) = avgMCS(gnBIndx) + currMeanMCS(gnBIndx);
            Array<vec> ratePerRBGPerUE = swapDimensions(currMCS(gnBIndx));
            // schInfo(gnBIndx) = schedulerPergNB(gnBIndx).schedule(nRBGs,isPF,availableUEs,ratePerRBGPerUE);
            schInfo(gnBIndx) = MACObjs(gnBIndx).myScheduler.schedule(nRBGs,isPF,availableUEs,ratePerRBGPerUE,harqInfoPerAssociatedUE(gnBIndx),bufferSizesPerAttachedUE);
        }
           
        Array<Array<SCHGrant_S>> grantInfo = generateGrantInfo(nRBs,RBGSize,nSymbols,schInfo,uesForMCS,csiFeedbackInfoPerUE,mcell.associationInfo.associatedServiceNodesPerServerNode,harqInfoPerAssociatedUE);
        
        Array<Array<DecoderOutput_S>> decoderOutput(numGnb);
        
        if(!isWithAbstraction)
        {
            transmitPDSCH(mcell,MACObjs, sltIndx,nSubcarriers, nSymbols, nRBs, txPowerIndBm , grantInfo,1);
//             decoderOutput = receiveAndProcessPDSCH(mcell,MACObjs, sltIndx,nSubcarriers, nSymbols, nRBs, txPowerIndBm , grantInfo,1);
            
            for(int gnBIndx=0; gnBIndx < numGnb; gnBIndx++)
            {
                int nUE = grantInfo(gnBIndx).length();
                decoderOutput(gnBIndx).set_length(nUE);
                
                bool withActualReception = (gnBIndx==0);
                
                int nThreads = nUE;
                if(nThreads > omp_get_num_procs())
                    nThreads = omp_get_num_procs();
                
                cout<<"Using "<<nThreads<<" threads for decoding under gNB : "<<gnBIndx<<endl;
                omp_set_num_threads(nThreads); 
                {
                    #pragma omp parallel for
                    for(int ue_cnt=0; ue_cnt < nUE; ue_cnt++)
                    {
                        decoderOutput(gnBIndx)(ue_cnt) = receiveAndProcessPDSCH(mcell,MACObjs(gnBIndx), sltIndx,nSubcarriers, nSymbols, nRBs, txPowerIndBm , grantInfo(gnBIndx)(ue_cnt),withActualReception);
                    }
                }
            }
            
            //intCovPerUE = calculateWBIntCovPerUE(mcell, sltIndx,nSubcarriers, nSymbols, nRBs, uesForMCS);
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
        upt(gnBIndx) = rxBitsPerUE(gnBIndx)/(nSlots*slotDuration);
        uSE(gnBIndx) = elem_div(rxBitsPerUE(gnBIndx),bwTimeProduct(gnBIndx));
        bler(gnBIndx) = elem_div(to_vec(txBlockCount(gnBIndx)-freshTxAckBlockCount(gnBIndx)) , to_vec(txBlockCount(gnBIndx))) ;
        
        ivec zeroIndices=find(txBlockCount(gnBIndx),0,true);
        setInVec(uSE(gnBIndx),zeroIndices,0.0);
        setInVec(bler(gnBIndx),zeroIndices,0.0);
                
        rbUtil(gnBIndx) = rbUtil(gnBIndx) / (nSlots*sysParams.nRBs);
        
        cellThrpt(gnBIndx) = sum(rxBitsPerUE(gnBIndx))/(nSlots*slotDuration);
        avgMCS(gnBIndx) = avgMCS(gnBIndx) / nSlots;
        
        append(allUPT,upt(gnBIndx));
        append(allSE,avgMCS(gnBIndx));
        
    }
    
    mat allUPTMat = mat(allUPT);
    mat allSEMat = mat(allSE);
    mat allOutput = concat_horizontal(allUPTMat,allSEMat);
        
    cout<<"UEs : "<<mcell.associationInfo.associatedServiceNodesPerServerNode(0)<<endl;
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
    
    cout<<briefSimulationDetails(nSlots*slotDuration)<<endl;
    
}
