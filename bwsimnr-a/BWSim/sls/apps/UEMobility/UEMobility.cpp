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
#include "../../Mcell/include/MCell.h"
#include "../../sim/include/SupportingFunctions.h"
#include "include/class_layer3.h"
#include "include/L3mobilitySM.h"
using namespace std;

struct BeamRSRPInfo_S
{
    ivec txNodeIDs;
    ivec txBeamIDs;
    ivec rxBeamIDs;
    vec RSRPindB;

    BeamRSRPInfo_S()
    {
        txNodeIDs.set_length(0);
        txBeamIDs.set_length(0);
        rxBeamIDs.set_length(0);
        RSRPindB.set_length(0);
    }
    
    BeamRSRPInfo_S(int count)
    {
        txNodeIDs.set_length(count);
        txBeamIDs.set_length(count);
        rxBeamIDs.set_length(count);
        RSRPindB.set_length(count);
    }
    
    BeamRSRPInfo_S getBeamRSRPInfo(int cellID)
    {
        if(find(txNodeIDs,cellID)!=-1)
        {
            int nBeams = count(txNodeIDs,cellID);
            BeamRSRPInfo_S beamRSRPInfo;
            beamRSRPInfo.txNodeIDs=zeros_i(nBeams)+cellID;
            
            ivec cellIndices = find(txNodeIDs,cellID,true);
            beamRSRPInfo.txBeamIDs=txBeamIDs(cellIndices);
            beamRSRPInfo.rxBeamIDs=rxBeamIDs(cellIndices);
            beamRSRPInfo.RSRPindB=RSRPindB(cellIndices);
            
            return beamRSRPInfo;
        }
        
        cout<<"Undefined CellID..."<<endl;abort();
    }
};


BeamRSRPInfo_S computeBeamRSRP(int serviceNodeID , McellSystem& mySys,AssociationInfo_S& associationInfo,double txSCPowerIndBm, double currentTimeInSec)
{
    ivec nTxBeamsPerServerNode(0);
    int nRxBeams = -1;

    ivec txNodelist = associationInfo.serverNodes;

    cout<<"Generating channel for UE: "<<serviceNodeID<<" at time "<<currentTimeInSec<<endl;
    ChannelMatrix_S channelMatrix = mySys.generateChannelForAnalogBeams(serviceNodeID,txNodelist,nTxBeamsPerServerNode,nRxBeams,currentTimeInSec,false);

    vec couplingGain = zeros(channelMatrix.txNodeIDs.length());
    vec RSRP = zeros(channelMatrix.txNodeIDs.length());
    vec linkGain = zeros(channelMatrix.txNodeIDs.length());

    BeamRSRPInfo_S beamRSRPInfo(channelMatrix.txNodeIDs.length());

    for(int txCnt = 0; txCnt <channelMatrix.txNodeIDs.length();txCnt++)
    {
        beamRSRPInfo.txNodeIDs(txCnt) = channelMatrix.txNodeIDs(txCnt)(0);
        beamRSRPInfo.txBeamIDs(txCnt) = channelMatrix.txNodeIDs(txCnt)(1);
        beamRSRPInfo.rxBeamIDs(txCnt) = channelMatrix.txNodeIDs(txCnt)(2);
//         cout<<"Channel : "<<channelMatrix.channels_forRSRP(txCnt)(0)(0)(0,4)<<endl;
        int txAntNo = 0;// couplingGain calculation for Port 0 -> assuption is Port 0 is Tx antenna 0
        for(int rxAntCnt = 0;rxAntCnt <channelMatrix.channels_forRSRP(txCnt)(txAntNo).length();rxAntCnt ++)
        {
            double tempAvgTap = 0.0;
            for(int tapCnt = 0;tapCnt <channelMatrix.channels_forRSRP(txCnt)(txAntNo)(rxAntCnt).length();tapCnt++)
            {
                tempAvgTap += sqr(abs(channelMatrix.channels_forRSRP(txCnt)(txAntNo)(rxAntCnt)(tapCnt)));
            }
            couplingGain(txCnt) += tempAvgTap;
        }
        couplingGain(txCnt) /= channelMatrix.channels_forRSRP(txCnt)(txAntNo).length();
        
        // converting into dB
        couplingGain(txCnt) = dB(abs(couplingGain(txCnt)));

        beamRSRPInfo.RSRPindB(txCnt) = couplingGain(txCnt) + (txSCPowerIndBm-30);

    }

    return beamRSRPInfo;
}

int main(int argc, char** argv){
    
    #ifdef USING_MPI
    initMPI(&(nTasks),&(currentTaskid),argc,argv);
    #endif

    ConfigFileNames_S myConfigFiles;
    ResultFileNames_S resultFiles;



    initAllFiles(argc,argv,myConfigFiles,resultFiles);
    // const std::string filename = "UEcoord.txt";

    int status = remove("UEcoord.txt");
    cout<< "Code Running 1" <<endl;

    MCell mcell = createMcellNetwork(myConfigFiles,resultFiles);

    cout<<endl<<"Code Running 2 "<<endl;

    ivec serviceNodes= mcell.associationInfo.serviceNodes;
    ivec serverNodes= mcell.associationInfo.serverNodes;



    abort();


    Array<MeasurementModel> measModelPerUE(serviceNodes.length());
    MeasurementModel measModel(mcell.associationInfo.serverNodes);
    for(int srvc_cnt=0;srvc_cnt<serviceNodes.length();srvc_cnt++)
        measModelPerUE(srvc_cnt) = measModel;
    
    ivec srvcNodeIndices=find(mcell.associationInfo.serviceNodes,serviceNodes);
    ivec nativeServerNodeIndices=find(mcell.associationInfo.serverNodes,mcell.associationInfo.associatedServerNodesForEachServiceNode(srvcNodeIndices));
    std::ofstream fout;

    fout.open("mobilityTrack.txt", std::ios::out);
//     fout<<"ServiceNode\tTimeInSec\txLoc\t\tyLoc\t\tAssociatedServerNode\t\tLinkGains"<<endl;

    double refreshDistanceInMetres;
    parse(myConfigFiles.simulationConfigFileName,"refreshDistanceInMetres",refreshDistanceInMetres);

    vec nodeTransmitPowerInDBm;
    parse("./configFiles/mySysConfig.txt","nodeTransmitPowerInDBm",nodeTransmitPowerInDBm);
    double txPowerIndBm = nodeTransmitPowerInDBm(0);
    double txSCPowerIndBm = txPowerIndBm - 10*log10(600); // assuming 50RBs in 10MHz
    double simulationTimeInSec;
    parse("./configFiles/mySimulationConfig.txt","simulationTimeInSec",simulationTimeInSec);
    vector<HandoverStateMachine> hsm; // Create a vector for HandoverStateMachine objects -CHANDINI
    for (int i = 0; i < serviceNodes.size(); i++)
    {
        // cout<<"number of server nodes"<<mcell.associationInfo.serverNodes.length()<<endl;
        // cout<<"server node id for servicenode "<<i<<"is"<<mcell.associationInfo.associatedServerNodesForEachServiceNode(i);
        hsm.emplace_back(HandoverStateMachine(serviceNodes[i], mcell.associationInfo.serverNodes, mcell.associationInfo.associatedServerNodesForEachServiceNode(i))); // adding HandoverStateMachine object to the hsm vector by passing the constructor of HandoverStateMachine called with two parameters
    }
    double l1SamplePeridicityInmilliSec = 20;
    int nServiceNodesToSimulate = 20;
    ivec serviceNodesToSimulate;
    
    if(serviceNodes.length()>nServiceNodesToSimulate)
        serviceNodesToSimulate= randPick(serviceNodes,nServiceNodesToSimulate);
    else
        serviceNodesToSimulate = serviceNodes;
    
    for(int srvc_cnt=0;srvc_cnt<serviceNodesToSimulate.length();srvc_cnt++)
    {
        for(double currentTime=0;currentTime<simulationTimeInSec;currentTime = currentTime + l1SamplePeridicityInmilliSec*1e-3)
        {
            if(mcell.mySys.getMovableDistance(serviceNodesToSimulate(srvc_cnt),currentTime)>=refreshDistanceInMetres)
            {
                double deltaT = mcell.mySys.getDeltaT(serviceNodesToSimulate(srvc_cnt),currentTime);
                ChannelGainInfo_S dummy= mcell.mySys.moveServiceNodeForMobility(serviceNodesToSimulate(srvc_cnt),currentTime);
//                 cout<<dummy.downlinkGainsInDB<<endl;
                mcell.mySys.updateSCMLinkForMobility(serviceNodesToSimulate(srvc_cnt),mcell.associationInfo.serverNodes,deltaT);
                cout<<"enough distance moved"<<endl;
            }
            for(int bs=0; bs<=20; bs++)
            {
                cout<< "cell "<<bs<< " | antenna gain : "<< mcell.mySys.aNodes(21).aLinkInfo(bs).txAntennaGain << " | pathloss : "<<mcell.mySys.aNodes(21).aLinkInfo(bs).pathLoss<<endl;
            }
            BeamRSRPInfo_S beamRSRP=computeBeamRSRP(serviceNodesToSimulate(srvc_cnt),mcell.mySys,mcell.associationInfo,txSCPowerIndBm,currentTime);
            cout<<"txBeamIDs = "<<beamRSRP.txBeamIDs<<"\ntxNodeIDs = "<<beamRSRP.txNodeIDs<<"\nRSRP (dBm) ="<<beamRSRP.RSRPindB-30<<endl;
//             fout<<serviceNodesToSimulate(srvc_cnt)<<"\t"<<currentTime<<"\t"<<mcell.mySys.getNodeLocation(serviceNodesToSimulate(srvc_cnt)).x<<"\t"<<mcell.mySys.getNodeLocation(serviceNodesToSimulate(srvc_cnt)).y<<"\t"<<mcell.associationInfo.associatedServerNodesForEachServiceNode(srvcNodeIndices(srvc_cnt))<<endl;
//            
            for(int srvr_cnt=0;srvr_cnt<serverNodes.length();srvr_cnt++)
            {
              BeamRSRPInfo_S beamRSRPInfoPerCell = beamRSRP.getBeamRSRPInfo(serverNodes(srvr_cnt));
              measModelPerUE(srvc_cnt).update(serverNodes(srvr_cnt),beamRSRPInfoPerCell.RSRPindB);
              cout<<"server count : "<< srvr_cnt<<" | beam RSRP : "<<beamRSRPInfoPerCell.RSRPindB<<endl;
            }

             // Retrieve cell measurements for the current service node
            vec cellMeasurements=measModelPerUE(srvc_cnt).getCellMeasurements();
            hsm[srvc_cnt].cellrsrp.clear();//clearing it before adding more elements
            for(int cellrsrpPos=0; cellrsrpPos<cellMeasurements.size(); cellrsrpPos++)
            {
                cout<<"user "<<srvc_cnt<< " | cell rsrp in "<<cellrsrpPos<<" : "<<cellMeasurements[cellrsrpPos]<<endl;
                hsm[srvc_cnt].cellrsrp.push_back(cellMeasurements[cellrsrpPos]);
            }
            // Update state machine for the service node -CHANDINI
            cout << "Updating state machine for user " << srvc_cnt << endl;
            hsm[srvc_cnt].updateSM();

            if (!hsm[srvc_cnt].running)
            {
                cout << "Service node " << srvc_cnt << " has dropped.\n"
                     << endl;
                int reasonCode = hsm[srvc_cnt].getFailureReasonCode();
                cout << "Failure reason code for" << srvc_cnt << "is:" << "\t" << reasonCode << "\n";
                cout << "Number of RLFs for " << srvc_cnt << "is:" << "\t" << hsm[srvc_cnt].rlf_count << "\n";
            }

            // Pause here to ensure sequential execution
            cout << "Completed processing for service node " << srvc_cnt << ". Moving to the next node.\n";
        
        }
        
    }
    fout.close();
    #ifdef USING_MPI
    MPI_Finalize();
    #endif
}

