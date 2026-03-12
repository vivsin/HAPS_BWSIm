/*************************************************************************
*
* CEWiT CONFIDENTIAL
* __________________
*
* All Rights Reserved © 2012 CEWiT, India
*
* NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
* and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
* Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
* express, printed and signed license for use is strictly forbidden.
*/

#include "../include/SupportingFunctions.h"

  
void associateNodes(AssociationInfo_S& associationInfo,string linkTableFileName,CarrierInfo_S carrierInfo,int strongInterfererCount, Array<ivec> &strongServerNodesPerServiceNode,Array<string> filenames,int maximumAssociationPerServerNode, InitialAssociationMetric initialAssociatioMetric)
{ 
  if(currentTaskid==0)
    cout<<"[detl:]Associating ServiceNodes to ServerNodes using Metric "<<initialAssociatioMetric<<endl;
  
  ivec serverNodes,serviceNodes,serverIDsPerServiceNode,serverNodeNetworkIDs,serviceNodeNetworkIDs;
  vec serverNodeTransmitPowerInDBm,serviceNodeTransmitPowerInDBm;
  mat downlinkLinkGainInDB,uplinkLinkGainInDB;
  mat downlinkPowerMatrixInDBm;
  ivec serverNodeType;
  vec serverNodeCarrierFreqInHz;  
  
  if(parse(linkTableFileName,"serverNodeList",serverNodes)==false)  {	cout<<"[both:]Error... Unable to load serverNodes from the file in associateNodes()..."<<endl;	abort();  }
  if(parse(linkTableFileName,"serviceNodeList",serviceNodes)==false)  {	cout<<"[both:]Error... Unable to load serviceNodes from the file in associateNodes()..."<<endl;	abort();  }
  if(parse(linkTableFileName,"serverNodeType",serverNodeType)==false)  {	cout<<"[both:]Error... Unable to load serverNodeType from the file in associateNodes()..."<<endl;	abort();  }
  if(parse(linkTableFileName,"serverNodeTransmitPowerInDBm",serverNodeTransmitPowerInDBm)==false)  {	cout<<"[both:]Error... Unable to load serverNodeTransmitPowerInDBm from the file in associateNodes()..."<<endl;	abort();  }
  if(parse(linkTableFileName,"serviceNodeTransmitPowerInDBm",serviceNodeTransmitPowerInDBm)==false)  {	cout<<"[both:]Error... Unable to load serviceNodeTransmitPowerInDBm from the file in associateNodes()..."<<endl;	abort();  }
  if(parse(linkTableFileName,"serverToServiceNodeLinkGainInDB",downlinkLinkGainInDB)==false)  {	cout<<"[both:]Error... Unable to load serverToServiceNodeLinkGainInDB from the file in associateNodes()..."<<endl;	abort();  }
  if(parse(linkTableFileName,"serviceToServerNodeLinkGainInDB",uplinkLinkGainInDB)==false)  {	cout<<"[both:]Error... Unable to load serviceToServerNodeLinkGainInDB from the file in associateNodes()..."<<endl;	abort();  }
  if(parse(linkTableFileName,"serverNodeCarrierFreqInHz",serverNodeCarrierFreqInHz)==false) {    cout<<"[both:]Error... Unable to load serverNodeCarrierFreqInHz from the file in associateNodes()..."<<endl;    abort();  }
  
  //cout<<"check the count of service nodes :"<<serviceNodes.length()<<endl;
  
  //cout<<"link table file name ::"<<linkTableFileName<<endl;
  
  vec serviceNodeReceiveNoiseFigureInDB;
  if(parse(linkTableFileName,"serviceNodeReceiveNoiseFigureInDB",serviceNodeReceiveNoiseFigureInDB)==false)  {	cout<<"[both:] Error... Unable to load serviceNodeReceiveNoiseFigureInDB from the file in associateNodes()..."<<endl;	abort();  }
  
  //! for Second BS Network
  if(parse(linkTableFileName,"serverNodeNetworkIDs",serverNodeNetworkIDs)==false) {    cout<<"Error... Unable to load serverNodeNetworkIDs from the file in associateNodes()..."<<endl;    abort();  }
  if(parse(linkTableFileName,"serviceNodeNetworkIDs",serviceNodeNetworkIDs)==false) {    cout<<"Error... Unable to load serviceNodeNetworkIDs from the file in associateNodes()..."<<endl;    abort();  }
  //! end for Second BS Network

  associationInfo.serverNodes=serverNodes;
  associationInfo.serviceNodes=serviceNodes;
  downlinkPowerMatrixInDBm=downlinkLinkGainInDB;
  for(int col_cnt=0;col_cnt<downlinkPowerMatrixInDBm.cols();col_cnt++)
    downlinkPowerMatrixInDBm.set_col(col_cnt,downlinkPowerMatrixInDBm.get_col(col_cnt)+serverNodeTransmitPowerInDBm(col_cnt));
  
  associationInfo.associatedServiceNodesPerServerNode.set_size(associationInfo.serverNodes.length());
  for(int serverNode_cnt=0;serverNode_cnt<associationInfo.associatedServiceNodesPerServerNode.length();serverNode_cnt++)
    associationInfo.associatedServiceNodesPerServerNode(serverNode_cnt).set_size(0);
  
  vec powerVector;
  ivec sortIndex;
  
  int bestServerNodeIndx;
  associationInfo.associatedServerNodesForEachServiceNode.set_length(associationInfo.serviceNodes.length());
  associationInfo.associatedBeamPairLinkforEachServiceNode.set_length(associationInfo.serviceNodes.length());
  associationInfo.coordinatingServerNodesForEachServiceNode.set_length(associationInfo.serviceNodes.length());
  ivec selectedServiceNodesIndx(0);
  ivec allIndices=getIntegers(0,associationInfo.serverNodes.length()-1);
  strongServerNodesPerServiceNode.set_length(associationInfo.serviceNodes.length());
  ivec picoIndices = find(serverNodeType,2,true);
 
  double picoCarrierFrequencyInHz = 4e9;
  
  for (int serviceNode_cnt=0;serviceNode_cnt<serviceNodeNetworkIDs.length();serviceNode_cnt++) 
  {
    powerVector=downlinkPowerMatrixInDBm.get_row(serviceNode_cnt);
    vec powerVectorInW=inv_dB(powerVector-30);
    
    //! Filtering serverNodes with same networkID as the serviceNode
    ivec validServerNodeIndices=find(serverNodeNetworkIDs,serviceNodeNetworkIDs(serviceNode_cnt),true);
    /*!\brief Collecting powerVector of first Network */
    //vec powerVectorForserver1 = powerVector.get(0,serverNodes.length()/2-1);
    vec validPowerVector = powerVectorInW(validServerNodeIndices);
    vec validCarrierFreq=serverNodeCarrierFreqInHz(validServerNodeIndices);
    vec uniqueCarrierFreqs=findUniqueNumbers(validCarrierFreq);
    Array<ivec> validServerNodeIndicesPerCF(uniqueCarrierFreqs.length());
    ivec sortedServerNodeIndicesInMyCarrier;
    
    if(uniqueCarrierFreqs.length() == 1)
      initialAssociatioMetric = _RSRP_;

    if(initialAssociatioMetric==_RSRQ_)
    {
      //       cout<<"powerVector\n"<<powerVector<<"\nvalidPowerVector\n"<<validPowerVector<<endl;
      vec carrierWiseRSRQ(0);
      ivec carrierWiseBestServerNodeIndx(0);
      for(int freq_cnt=0;freq_cnt<uniqueCarrierFreqs.length();freq_cnt++)
      {
	ivec freqIndices=find(validCarrierFreq,uniqueCarrierFreqs(freq_cnt),true);
	validServerNodeIndicesPerCF(freq_cnt)=validServerNodeIndices(freqIndices);
	vec carrierPowerVec=validPowerVector(freqIndices);
	ivec sortIndex=reverse(sort_index(carrierPowerVec));
	double bestPow=carrierPowerVec(sortIndex(0));
	append(carrierWiseRSRQ,bestPow/sum(carrierPowerVec));
	append(carrierWiseBestServerNodeIndx,validServerNodeIndices(freqIndices(sortIndex(0))));
	validServerNodeIndicesPerCF(freq_cnt)=validServerNodeIndicesPerCF(freq_cnt)(sortIndex);
      }
      //       cout<<"carrierWiseRSRQ\n"<<carrierWiseRSRQ<<"\ncarrierWiseBestServerNodeIndx\n"<<carrierWiseBestServerNodeIndx<<endl;
      ivec sortIndex=reverse(sort_index(carrierWiseRSRQ));
      bestServerNodeIndx=carrierWiseBestServerNodeIndx(sortIndex(0));
      sortedServerNodeIndicesInMyCarrier=validServerNodeIndicesPerCF(sortIndex(0)); 
    }
    else
    {
      ivec sortIndex=reverse(sort_index(validPowerVector));
      bestServerNodeIndx=validServerNodeIndices(sortIndex(0));
      sortedServerNodeIndicesInMyCarrier=validServerNodeIndices(sortIndex);
   }

    associationInfo.associatedServerNodesForEachServiceNode(serviceNode_cnt)=associationInfo.serverNodes(bestServerNodeIndx);
    append(associationInfo.associatedServiceNodesPerServerNode(bestServerNodeIndx),associationInfo.serviceNodes(serviceNode_cnt));
      
    if(sortedServerNodeIndicesInMyCarrier.length()<=(strongInterfererCount+1))
      strongServerNodesPerServiceNode(serviceNode_cnt)=associationInfo.serverNodes(sortedServerNodeIndicesInMyCarrier);
    else
      strongServerNodesPerServiceNode(serviceNode_cnt)=associationInfo.serverNodes(sortedServerNodeIndicesInMyCarrier.left((strongInterfererCount+1)));
      
    //!Adjusting downlinkPowerMatrixInDBm according to the associated serverNode Frequency
    double desiredCarrierFreq=serverNodeCarrierFreqInHz(bestServerNodeIndx);
    vec sameNetworkCarrierFreq=serverNodeCarrierFreqInHz(validServerNodeIndices);
    ivec sameCarrierIndices=find(sameNetworkCarrierFreq,desiredCarrierFreq,true);
    validServerNodeIndices=validServerNodeIndices(sameCarrierIndices);
    ivec invalidServerNodeIndices=findNot(allIndices,validServerNodeIndices);
    setInVec(powerVector,invalidServerNodeIndices,-500.0);//Setting -500 over all invalid Links
    downlinkPowerMatrixInDBm.set_row(serviceNode_cnt,powerVector);
    
    picoCarrierFrequencyInHz = desiredCarrierFreq;
  }// end for loop
  
  double BandwidthInHz = carrierInfo.carrierBandwidthInMHz*1e6; 
  vec serviceNodeReceiveNoiseVariance=inv_dB(-203.8 + 10*log10(BandwidthInHz) + serviceNodeReceiveNoiseFigureInDB);
  
  
  if(maximumAssociationPerServerNode!=-1)
  {
    
    for(int serverNode_cnt=0;serverNode_cnt<associationInfo.serverNodes.length();serverNode_cnt++)
    {
      ivec associatedServiceNodes;
      if(associationInfo.associatedServiceNodesPerServerNode(serverNode_cnt).length()>maximumAssociationPerServerNode)
      {
	associatedServiceNodes=associationInfo.associatedServiceNodesPerServerNode(serverNode_cnt);
	associationInfo.associatedServiceNodesPerServerNode(serverNode_cnt)=randPick(associatedServiceNodes,maximumAssociationPerServerNode);
	  }
      
      append(selectedServiceNodesIndx,find(associationInfo.serviceNodes,associationInfo.associatedServiceNodesPerServerNode(serverNode_cnt)));
      
    }
    
    associationInfo.serviceNodes=associationInfo.serviceNodes(selectedServiceNodesIndx);
    downlinkLinkGainInDB=downlinkLinkGainInDB.get_rows(selectedServiceNodesIndx);
    serviceNodeReceiveNoiseVariance=serviceNodeReceiveNoiseVariance(selectedServiceNodesIndx);
    uplinkLinkGainInDB=uplinkLinkGainInDB.get_cols(selectedServiceNodesIndx);   
    downlinkPowerMatrixInDBm=downlinkPowerMatrixInDBm.get_rows(selectedServiceNodesIndx);
    associationInfo.associatedServerNodesForEachServiceNode=associationInfo.associatedServerNodesForEachServiceNode(selectedServiceNodesIndx);
    associationInfo.coordinatingServerNodesForEachServiceNode=getFromArray(associationInfo.coordinatingServerNodesForEachServiceNode,selectedServiceNodesIndx);
    strongServerNodesPerServiceNode=getFromArray(strongServerNodesPerServiceNode,selectedServiceNodesIndx);
    
  }
  else
  {
  }
  associationInfo.computeGeometrySINR(downlinkPowerMatrixInDBm,serviceNodeReceiveNoiseVariance);
    //cout<<"Geometry SINR"<<associationInfo.computeGeometrySINR(downlinkPowerMatrixInDBm,serviceNodeReceiveNoiseVariance);
  if(currentTaskid==0 && traceValuePerTraceName["printInitialAssociationFiles"]=="true")
  {
      //if(traceValuePerTraceName["printCLandSINRLogs"]=="true")
      {
        printCouplingGain(associationInfo,downlinkLinkGainInDB,filenames(0));
        printCouplingGain(associationInfo,transpose(uplinkLinkGainInDB),filenames(1));
        printSINRDistribution(associationInfo,downlinkPowerMatrixInDBm,serviceNodeReceiveNoiseVariance,filenames(2));
      }
    //printAssociationLogs(downlinkLinkGainInDB,downlinkPowerMatrixInDBm,associationInfo,serviceNodeReceiveNoiseVariance,filenames(3));
    associationInfo.printAssociationInfo(filenames(4),"");
  }

  #ifdef USING_MPI
  MPI_Barrier(MPI_COMM_WORLD);
  #endif 
 
}



void printDetailedAssociationLogs(mat& couplingGain,mat& downlinkPowerMatrixInDBm,AssociationInfo_S &associationInfo,vec serviceNodeNoiseVariance,string fileName,bool isBeam)
{
    ofstream fout;
    if(!isBeam)
        fout.open(fileName.c_str(),ios::out);
    else
    {
        string tempFileName = fileName+".temp"+toString(currentTaskid);
        fout.open(tempFileName.c_str(),ios::out);
    }
    if(currentTaskid==0)
        fout<<"%serviceNodeID,serverNodeID,couplingGain(dB),RSRP(dB),RSSI(dB),RSRQ(dB),SIR(dB),noiseVariance(dB),SINR(dB)"<<endl;

    for(int srvc_cnt=0;srvc_cnt<couplingGain.rows();srvc_cnt++)
    {
        double RSSI = sum(inv_dB(downlinkPowerMatrixInDBm.get_row(srvc_cnt)));
        int srvcNodeIndx = (!associationInfo.serviceNodesPerTask.length()) ? srvc_cnt : find(associationInfo.serviceNodes,associationInfo.serviceNodesPerTask(currentTaskid)(srvc_cnt));
        for(int srvr_cnt=0;srvr_cnt<associationInfo.serverNodes.length();srvr_cnt++)
        {
            double RSRP = inv_dB(downlinkPowerMatrixInDBm(srvc_cnt,srvr_cnt));
            double RSRQ = (double) RSRP/RSSI;
            double SIR = (double) RSRP / (RSSI - RSRP);
            double SINR = (double) RSRP / (RSSI - RSRP + serviceNodeNoiseVariance(srvc_cnt));
            fout<<associationInfo.serviceNodes(srvcNodeIndx)<<","<<associationInfo.serverNodes(srvr_cnt)<<","<<couplingGain(srvc_cnt,srvr_cnt)<<","<<dB(RSRP)<<","<<dB(RSSI)<<","<<dB(RSRQ)<<","<<dB(SIR)<<","<<dB(serviceNodeNoiseVariance(srvc_cnt))<<","<<dB(SINR)<<endl;
        }
    }
    fout.close();
}
void printAssociationLogs(mat& couplingGainInDB,mat& downlinkPowerMatrixInDBm,AssociationInfo_S &associationInfo,vec serviceNodeNoiseVariance,string fileName,McellSystem* mySysPtr,bool isBeam)
{
    ofstream fout;
    if(!isBeam)
        fout.open(fileName.c_str(),ios::out);
    else
    {
        string tempFileName = fileName+".temp"+toString(currentTaskid);
        fout.open(tempFileName.c_str(),ios::out);
    }
    if(currentTaskid==0)
        fout<<"%serviceNodeID,associatedServerNodeID,txBeamID,rxBeamID,pathLoss,shadowLoss,channelGain(PL+SF),txAntGain,rxAntGain,linkGain,couplingGain,RSRP,RSSI,RSRQ,SIR,noiseVariance,SINR,NOTE: all the values are in dB"<<endl;
    for(int srvc_cnt=0;srvc_cnt<couplingGainInDB.rows();srvc_cnt++)
    {
        int srvcNodeIndx = (!associationInfo.serviceNodesPerTask.length()) ? srvc_cnt : find(associationInfo.serviceNodes,associationInfo.serviceNodesPerTask(currentTaskid)(srvc_cnt));
        int srvrNodeIndx = find(associationInfo.serverNodes,associationInfo.associatedServerNodesForEachServiceNode(srvcNodeIndx));
        //"associationInfo.associatedServerNodesForEachServiceNode(srvcNodeIndx)" checkpoint-DD!

        //cout<<"Debug server node"<<associationInfo.associatedServerNodesForEachServiceNode(srvcNodeIndx)<<","<<srvcNodeIndx<<endl;

        double RSRP = inv_dB(downlinkPowerMatrixInDBm(srvc_cnt,srvrNodeIndx));
        //cout<<"RSRP : "<<dB(abs(RSRP))<<endl;
        double RSSI = sum(inv_dB(downlinkPowerMatrixInDBm.get_row(srvc_cnt)));
        //cout << "RSSI :" <<downlinkPowerMatrixInDBm.get_row(srvc_cnt) <<endl;
        double RSRQ = (double) RSRP/RSSI;
        double SIR = (double) RSRP/(RSSI-RSRP);
        double SINR = (double) RSRP / (RSSI - RSRP + serviceNodeNoiseVariance(srvc_cnt));
        //cout<<"Noise variance"<<serviceNodeNoiseVariance(srvc_cnt)<<endl;
        //cout << "RSRP :" << RSRP << " RSSI: " << RSSI << " serviceNodeNoiseVariance "<< serviceNodeNoiseVariance(srvc_cnt) << endl;
        //cout<<"SRVC NODE INDX"<<srvcNodeIndx<<endl;
        //cout<<"SRVR NODE INDX"<<srvrNodeIndx<<endl;
        //cout << "RSRP :" << dB(RSRP) <<endl<< " RSSI: " << dB(RSSI) <<endl<< " serviceNodeNoiseVariance "<< dB(serviceNodeNoiseVariance(srvc_cnt)) << endl;
        NodeLinkInfo_S linkInfo = mySysPtr->getLinkInfo(associationInfo.serverNodes(srvrNodeIndx),associationInfo.serviceNodes(srvcNodeIndx));
        sBeamPairLink beamPairLink = associationInfo.associatedBeamPairLinkforEachServiceNode(srvcNodeIndx);
        //cout<<"COUPLING GAIN IN dB"<<couplingGainInDB(srvc_cnt,srvrNodeIndx)<<endl;
        fout<<associationInfo.serviceNodes(srvcNodeIndx)<<","<<associationInfo.serverNodes(srvrNodeIndx)<<","<<beamPairLink.srvrBeamID<<","<<beamPairLink.srvcBeamID;
        fout<<","<<linkInfo.pathLoss<<","<<linkInfo.shadowLoss<<","<<-(linkInfo.pathLoss+linkInfo.shadowLoss)<<","<<linkInfo.txAntennaGain<<","<<linkInfo.rxAntennaGain<<","<<linkInfo.txAntennaGain+linkInfo.rxAntennaGain-(linkInfo.pathLoss+linkInfo.shadowLoss);
        fout<<","<<couplingGainInDB(srvc_cnt,srvrNodeIndx)<<","<<dB(RSRP)<<","<<dB(RSSI)<<","<<dB(RSRQ)<<","<<dB(SIR)<<","<<dB(serviceNodeNoiseVariance(srvc_cnt))<<","<<dB(SINR)<<endl;
        double calcLinkGain = linkInfo.txAntennaGain+linkInfo.rxAntennaGain-(linkInfo.pathLoss+linkInfo.shadowLoss);
//         cout << "[LinkGain Debug] SrvcNode " << associationInfo.serviceNodes(srvcNodeIndx) << " <-> SrvrNode " << associationInfo.serverNodes(srvrNodeIndx) << " LinkGain: " << calcLinkGain << " dB" << endl;
    }
    
    fout.close();
}


void computeRSRPforAllNodes(McellSystem& mySys,AssociationInfo_S& associationInfo,InitialAssociationMetric associationMetric,mat& couplingGainInDB,mat& RSRPindB,string beamRSRPfileName,string linkTableFileName)
{
    vec serverNodeTransmitPowerInDB;
    //cout << "serverNodeTransmitPowerInDB : " << serverNodeTransmitPowerInDB <<endl;
    if(!parse(linkTableFileName,"serverNodeTransmitPowerInDBm",serverNodeTransmitPowerInDB))
    {
        cout<<"Unable to parse serverNodeTransmitPowerInDBm from "<<linkTableFileName<<endl;abort();
    }
    //cout << "serverNodeTransmitPowerInDB v1 : " << serverNodeTransmitPowerInDB <<endl;
    serverNodeTransmitPowerInDB -=30;
    //cout << "serverNodeTransmitPowerInDB v2 : " << serverNodeTransmitPowerInDB <<endl;
    ivec nTxBeamsPerServerNode(0);
    //cout << "nTxBeamsPerServerNode: " << nTxBeamsPerServerNode <<endl;
    int nRxBeams = -1;
    
    ivec updatedCellIds(associationInfo.serviceNodesPerTask(currentTaskid).length());//UE
    //cout << "associationInfo.serviceNodesPerTask(currentTaskid)" << associationInfo.serviceNodesPerTask(currentTaskid) <<endl<< "currentTaskid"<<currentTaskid<< endl<<"length"<<associationInfo.serviceNodesPerTask(currentTaskid).length()<<endl;
    couplingGainInDB=zeros(associationInfo.serviceNodesPerTask(currentTaskid).length(),associationInfo.serverNodes.length());
    RSRPindB=zeros(associationInfo.serviceNodesPerTask(currentTaskid).length(),associationInfo.serverNodes.length());
                
    std::ofstream fout;
    bool toPrintBeamRSRP = traceValuePerTraceName["printBeamRSRP"]=="true";
    if(toPrintBeamRSRP)
    {
        string tempFileName = beamRSRPfileName+".temp"+toString(currentTaskid);
        fout.open(tempFileName.c_str(),std::ios::out);
        if(currentTaskid==0)
            fout<<"%ServiceNode,\t ServerNode,\t srvrBeamID,\t srvcBeamID,\t TxPanelID,\t RxPanelID,\t couplingGainIndB,\t RSRPindBm"<<endl;
    }
    ivec txNodelist = associationInfo.serverNodes;
    //cout<<"TX NODE LIST :"<<associationInfo.serverNodes<<endl;
    //cout << "txNodelist: " << txNodelist <<endl;
    Array < Array < mat > > couplingGainwithRandomInterfererBeams(associationInfo.serviceNodesPerTask(currentTaskid).length()); //srvcNode, srvrNode,TxOrientation,RxOrientation, for Matrix RxBeams,TxBeams
    //cout << "couplingGainwithRandomInterfererBeams: " << couplingGainwithRandomInterfererBeams <<endl;
//     std::ofstream fout1;
//     string tempFileName = "top3Association.txt";
//     fout1.open(tempFileName.c_str(),std::ios::out);
    
//      vec timeInstances = linspace(0,0.2,5);// assuming 50ms measurements with averaging over 200ms
   vec timeInstances = to_vec(randi(10,0,10000))*0.001;
//     vec timeInstances = vec("0");
    
    for(int rx_cnt=0; rx_cnt<associationInfo.serviceNodesPerTask(currentTaskid).length(); rx_cnt++)
    {   
        vec avgCouplingGain;
        vec avgRSRP;
        couplingGainwithRandomInterfererBeams(rx_cnt).set_length(txNodelist.length());
        ChannelMatrix_S channelMatrix;
        for(int timeInstant=0;timeInstant<timeInstances.length();timeInstant++)
        {
            channelMatrix = mySys.generateChannelForAnalogBeams(associationInfo.serviceNodesPerTask(currentTaskid)(rx_cnt),txNodelist,nTxBeamsPerServerNode,nRxBeams,timeInstances(timeInstant),false);
            vec couplingGain = zeros(channelMatrix.txNodeIDs.length());
            vec RSRP = zeros(channelMatrix.txNodeIDs.length());
            vec linkGain = zeros(channelMatrix.txNodeIDs.length());
            
            if(timeInstant==0)
                for(int srvrCnt = 0; srvrCnt <txNodelist.length();srvrCnt++)
                    couplingGainwithRandomInterfererBeams(rx_cnt)(srvrCnt) = zeros(nRxBeams,nTxBeamsPerServerNode(srvrCnt));
            
            for(int txCnt = 0; txCnt <channelMatrix.txNodeIDs.length();txCnt++)
            {
                int nodeID = channelMatrix.txNodeIDs(txCnt)(0);
                int txBeamID = channelMatrix.txNodeIDs(txCnt)(1);
                int rxBeamID = channelMatrix.txNodeIDs(txCnt)(2);
                if(associationMetric != _PATHLOSS_){
                    //cout << "associationMetric:" << associationMetric<< ","<<_PATHLOSS_ <<endl;
                    int txAntNo = 0;// couplingGain calculation for Port 0 -> assuption is Port 0 is Tx antenna 0
                    vec rsrpPerRx(channelMatrix.channels_forRSRP(txCnt)(txAntNo).length());
                    for(int rxAntCnt = 0;rxAntCnt <channelMatrix.channels_forRSRP(txCnt)(txAntNo).length();rxAntCnt ++)
                        rsrpPerRx(rxAntCnt) = sum(sqr(abs(channelMatrix.channels_forRSRP(txCnt)(txAntNo)(rxAntCnt))));
                    
                    couplingGain(txCnt) = max(rsrpPerRx);
                    //cout << "CG : "<< couplingGain(txCnt) <<endl;
                    // converting into dB
                    couplingGain(txCnt) = dB(couplingGain(txCnt));
                    //cout << "CG in dB: "<< couplingGain(txCnt)<< endl;
                    //abort();
                }

                else
                {
                    //abort();

                    NodeLinkInfo_S linkInfo = mySys.getLinkInfo(nodeID,associationInfo.serviceNodesPerTask(currentTaskid)(rx_cnt));
                    //cout<<"node ID"<<nodeID<<endl;
                    couplingGain(txCnt) = linkInfo.txAntennaGain + linkInfo.rxAntennaGain - (linkInfo.pathLoss + linkInfo.shadowLoss);
    //                 cout<<"txAntennaGain "<<linkInfo.txAntennaGain <<endl<<"rxAntennaGain"<<linkInfo.rxAntennaGain<<endl<< " pathLoss"<<linkInfo.pathLoss<<endl<< "shadowLoss"<<linkInfo.shadowLoss<<endl;
    //                 couplingGain(txCnt) = dB(abs(couplingGain(txCnt)));
                    //cout << "CouplingGain in dB: "<< couplingGain(txCnt)<< endl;
                }

                RSRP(txCnt) = couplingGain(txCnt) + serverNodeTransmitPowerInDB(find(txNodelist,nodeID));
                
                
                if(toPrintBeamRSRP)
                    fout<<associationInfo.serviceNodesPerTask(currentTaskid)(rx_cnt)<<",\t "<<nodeID<<",\t "<<txBeamID<<",\t "<<rxBeamID<<",\t "<<couplingGain(txCnt)<<",\t "<<RSRP(txCnt)+30<<endl;
                
//                 couplingGainwithRandomInterfererBeams(rx_cnt)(find(txNodelist,nodeID))(rxBeamID,txBeamID) = couplingGain(txCnt);
            }
            
            
            
            if(timeInstant==0)
            {
                avgCouplingGain = couplingGain;
                avgRSRP = RSRP;
            }
            else
            {
                avgRSRP = dB((inv_dB(avgRSRP) + inv_dB(RSRP))/2);
                avgCouplingGain = dB((inv_dB(avgCouplingGain) + inv_dB(couplingGain))/2);
            }
            
        }
        
        for(int txCnt = 0; txCnt <channelMatrix.txNodeIDs.length();txCnt++)
        {
            int nodeID = channelMatrix.txNodeIDs(txCnt)(0);
            int txBeamID = channelMatrix.txNodeIDs(txCnt)(1);
            int rxBeamID = channelMatrix.txNodeIDs(txCnt)(2);
            
            couplingGainwithRandomInterfererBeams(rx_cnt)(find(txNodelist,nodeID))(rxBeamID,txBeamID) = avgCouplingGain(txCnt);
        }
        
        //cout<<"RSRP and couplingGain : "<<RSRP<<"\n"<<couplingGain<<endl;
        int maxIndx = max_index(avgRSRP); //checkpoint-DD!
        //cout<<"maxIndex: "<<maxIndx<<endl;
        ivec beamPanelIndices = channelMatrix.txNodeIDs(maxIndx);
        //cout<<"BEAM PANNEL INDICES: "<<beamPanelIndices<<endl;
        updatedCellIds(rx_cnt) = beamPanelIndices(0); 
        //cout<<"updated cell ID"<<updatedCellIds(rx_cnt)<<endl;  //checkpoint-DD!
        int associatedTxIndx = find(txNodelist,updatedCellIds(rx_cnt));
        
        couplingGainInDB(rx_cnt,associatedTxIndx) = avgCouplingGain(maxIndx);
        RSRPindB(rx_cnt,associatedTxIndx) = avgRSRP(maxIndx);
        
        int rxIndx = find(associationInfo.serviceNodes,associationInfo.serviceNodesPerTask(currentTaskid)(rx_cnt));
        associationInfo.associatedServerNodesForEachServiceNode(rxIndx) = updatedCellIds(rx_cnt);
//         associationInfo.coordinatingServerNodesForEachServiceNode(rxIndx) = coordinatingSrvrNodes;        
        associationInfo.associatedBeamPairLinkforEachServiceNode(rxIndx).srvrBeamID = beamPanelIndices(1);
        associationInfo.associatedBeamPairLinkforEachServiceNode(rxIndx).srvcBeamID = beamPanelIndices(2);
        associationInfo.associatedBeamPairLinkforEachServiceNode(rxIndx).rsrpIndBm = RSRPindB(rx_cnt,associatedTxIndx) +30;
        associationInfo.associatedBeamPairLinkforEachServiceNode(rxIndx).rspIndBm = RSRPindB(rx_cnt,associatedTxIndx) +30 - mySys.channelCloud.getChannelGain(associationInfo.serviceNodes(rxIndx),updatedCellIds(rx_cnt)); // need to set app value - Dhiv
    }
    
//     fout1.close();
    
    if(toPrintBeamRSRP)
        fout.close();
    
    for(int rx_cnt=0; rx_cnt<associationInfo.serviceNodesPerTask(currentTaskid).length(); rx_cnt++)
    {
        ivec interfererIndices = findNot(txNodelist,updatedCellIds(rx_cnt));
        
        int rxIndx = find(associationInfo.serviceNodes,associationInfo.serviceNodesPerTask(currentTaskid)(rx_cnt));
        //cout <<"interfererIndices : " << rxIndx <<endl;
        for(int interfererCnt = 0; interfererCnt <interfererIndices.length();interfererCnt++)
        {
            int nodeIndx = interfererIndices(interfererCnt);
            int rxBeamID = associationInfo.associatedBeamPairLinkforEachServiceNode(rxIndx).srvcBeamID;
            
            int randomTxBeamID = randi(0,nTxBeamsPerServerNode(nodeIndx)-1);
            //cout <<"randomTxBeamID : " << randomTxBeamID <<endl;
            couplingGainInDB(rx_cnt,nodeIndx) = couplingGainwithRandomInterfererBeams(rx_cnt)(nodeIndx)(rxBeamID,randomTxBeamID);
            RSRPindB(rx_cnt,nodeIndx) = couplingGainInDB(rx_cnt,nodeIndx) + serverNodeTransmitPowerInDB(nodeIndx);
        }
        double RSSI = dB(sum(inv_dB(RSRPindB.get_row(rx_cnt))));
        //cout <<"RSRPindB :" << RSRPindB << " rx_cnt :" <<rx_cnt << " RSRPindB.get_row :" <<RSRPindB.get_row(rx_cnt) << " Size(RSRPindB.get_row :)" <<RSRPindB.get_row(rx_cnt).size() <<endl;
        associationInfo.associatedBeamPairLinkforEachServiceNode(rxIndx).rsrqIndB = RSRPindB(rx_cnt,find(txNodelist,updatedCellIds(rx_cnt))) - RSSI;
    }
}

void computeMaxRSRPforAllNodes(McellSystem& mySys,AssociationInfo_S& associationInfo,InitialAssociationMetric associationMetric,mat& couplingGainInDB,mat& RSRPindB,string beamRSRPfileName,string linkTableFileName)
{
    vec serverNodeTransmitPowerInDB;
    if(!parse(linkTableFileName,"serverNodeTransmitPowerInDBm",serverNodeTransmitPowerInDB))
    {
        cout<<"Unable to parse serverNodeTransmitPowerInDBm from "<<linkTableFileName<<endl;abort();
    }
    serverNodeTransmitPowerInDB -=30;
    
    ivec nTxBeamsPerServerNode(0);
    int nRxBeams = -1;
    
    ivec updatedCellIds(associationInfo.serviceNodesPerTask(currentTaskid).length());
    
    couplingGainInDB.set_size(associationInfo.serviceNodesPerTask(currentTaskid).length(),associationInfo.serverNodes.length());
    RSRPindB.set_size(associationInfo.serviceNodesPerTask(currentTaskid).length(),associationInfo.serverNodes.length());
    
    std::ofstream fout;
    string tempFileName = beamRSRPfileName;
    fout.open(tempFileName.c_str(),std::ios::out);
    
    std::ofstream fout1;
    string tempFileName1 = beamRSRPfileName+"Dist";
    fout1.open(tempFileName1.c_str(),std::ios::out);
    
    
    ivec txNodelist = associationInfo.serverNodes;
    mat allMaxRSRP=zeros(associationInfo.serviceNodes.length(),associationInfo.serverNodes.length());
    mat distanceInm=zeros(associationInfo.serviceNodes.length(),associationInfo.serverNodes.length());
    for(int rx_cnt=0; rx_cnt<associationInfo.serviceNodesPerTask(currentTaskid).length(); rx_cnt++)
    {     
        ChannelMatrix_S channelMatrix = mySys.generateChannelForAnalogBeams(associationInfo.serviceNodesPerTask(currentTaskid)(rx_cnt),txNodelist,nTxBeamsPerServerNode,nRxBeams);
        vec couplingGain = zeros(channelMatrix.txNodeIDs.length());
        vec RSRP = zeros(channelMatrix.txNodeIDs.length());
        vec linkGain = zeros(channelMatrix.txNodeIDs.length());
        
        for(int txCnt = 0; txCnt <channelMatrix.txNodeIDs.length();txCnt++)
        {
            int nodeID = channelMatrix.txNodeIDs(txCnt)(0);
            int txBeamID = channelMatrix.txNodeIDs(txCnt)(1);
            int rxBeamID = channelMatrix.txNodeIDs(txCnt)(2);
            
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
            
            RSRP(txCnt) = couplingGain(txCnt) + serverNodeTransmitPowerInDB(find(txNodelist,nodeID));
        }
       
       ivec allNodeIDS=getAcrossArray(channelMatrix.txNodeIDs,0); 
        for(int srvrCnt = 0; srvrCnt <associationInfo.serverNodes.length();srvrCnt++)
        {
            
            allMaxRSRP(rx_cnt,srvrCnt) = max(RSRP(find(allNodeIDS,associationInfo.serverNodes(srvrCnt),true)));  
            NodeLinkInfo_S linkInfo = mySys.getLinkInfo(associationInfo.serverNodes(srvrCnt),associationInfo.serviceNodesPerTask(currentTaskid)(rx_cnt));
            distanceInm(rx_cnt,srvrCnt) = linkInfo.distance;        
        }
        
        
    }
    
    fout<<allMaxRSRP<<endl;
    fout.close();
    
    fout1<<distanceInm<<endl;
    fout1.close();
    
}


void printLinkGain(string linkGainFileName,string linkLogsFileName,AssociationInfo_S& associationInfo,ChannelCloud* channelCloud)
{
    std::ofstream fout;
    string tempFileName = linkLogsFileName+".temp"+toString(currentTaskid);
    fout.open(tempFileName.c_str(),ios::out);
    if(currentTaskid==0)
        fout<<"%rxNodeID ,\t txNodeID ,\t rxPanelID ,\t txPanelID ,\t linkGainIndB ,\tLinkCondition"<<endl;
    
    for(int srvc_cnt=0;srvc_cnt<associationInfo.serviceNodesPerTask(currentTaskid).length();srvc_cnt++)
    {
        int srvcID = associationInfo.serviceNodesPerTask(currentTaskid)(srvc_cnt);
        int srvcIndx = find(associationInfo.serviceNodes,srvcID);
        sBeamPairLink beamPair =associationInfo.associatedBeamPairLinkforEachServiceNode(srvcIndx);
        int srvrID = associationInfo.associatedServerNodesForEachServiceNode(srvcIndx);
        double linkGain = beamPair.rsrpIndBm-beamPair.rspIndBm;       
        fout<<associationInfo.serviceNodes(srvcIndx)<<",\t"<<srvrID<<",\t"<<linkGain;
        int scmLinkIndx = channelCloud->getScmLinkIndx(srvcID,srvrID);
        fout<<",\t"<<getString(channelCloud->scmLinks(scmLinkIndx).getSCMLinkCondition())<<endl;
    }
    fout.close();
    
    if(nTasks==1)
    {
        mat linkGain = channelCloud->getLinkSpecificChannelGainInDB(associationInfo.serverNodes,associationInfo.serviceNodes,true);
        fout.open(linkGainFileName.c_str(), std::ios::out);
        fout<<linkGain;
        fout.close();
    }
}


void printLinkLogs(McellSystem* mySysPtr,AssociationInfo_S* associationInfoPtr,mat couplingGain,mat RSRPinDB,vec serviceNodeReceiveNoiseVariance,string fileName)
{
    ofstream op(fileName+".temp"+toString(currentTaskid),std::ios::out);
    op<<"%srvcID,\tsrvrID,\tpathLoss,\tshadowLoss,\tchannelGain,\ttxAntGain(3Dport0),\trxAntGain(3Dport0),\tlinkGain,\tscmGain,\tcouplingGain,\tRSRP,\tRSSI,\tRSRQ,\tSINR\t NOTE: all the values are in dB"<<endl;
    for(int srvc_cnt=0;srvc_cnt<associationInfoPtr->serviceNodesPerTask(currentTaskid).length();srvc_cnt++)
    {
        for(int srvr_cnt=0;srvr_cnt<associationInfoPtr->serverNodes.length();srvr_cnt++)
        {
            int srvcID = associationInfoPtr->serviceNodesPerTask(currentTaskid)(srvc_cnt);
            int srvrID = associationInfoPtr->serverNodes(srvr_cnt);
            NodeLinkInfo_S linkInfo = mySysPtr->getLinkInfo(srvrID,srvcID);
            double RSRP = inv_dB(RSRPinDB(srvc_cnt,srvr_cnt));
            double RSSI = sum(inv_dB(RSRPinDB.get_row(srvc_cnt)));
            double RSRQ = RSRP/RSSI;
            double SINR = RSRP/(RSSI-RSRP+serviceNodeReceiveNoiseVariance(srvc_cnt));
            double linkGain = mySysPtr->channelCloud.getLinkSpecificChannelGainInDB(srvrID,srvcID,0,0,false);
            SCMLink scmLink = mySysPtr->channelCloud.getSCMLink(srvcID,srvrID);
            double channelGain = mySysPtr->channelCloud.getChannelGain(srvcID,srvrID);
            op<<srvcID<<",\t"<<srvrID<<",\t"<<linkInfo.pathLoss<<",\t"<<linkInfo.shadowLoss<<",\t"<<channelGain<<",\t"<<linkInfo.txAntennaGain<<",\t"<<linkInfo.rxAntennaGain<<",\t"<<linkGain<<",\t"<<scmLink.getAvgAntennaGainInDB(0,0)<<",\t"<<couplingGain(srvc_cnt,srvr_cnt)<<",\t"<<dB(RSRP)<<",\t"<<dB(RSSI)<<",\t"<<dB(RSRQ)<<",\t"<<dB(SINR)<<endl;
        }
    }
    op.close();
}

void printClusterAngleSpread(ChannelCloud* channelCloud,AssociationInfo_S* associationInfoPtr,string fileName)
{
    ofstream op(fileName+".temp"+toString(currentTaskid),std::ios::out);
    op<<"%srvcID,\tsrvrID,\tmuDS,\tsigmaDS,\tmuAOD,\tsigmaAOD,\tmuZOD,\tsigmaZOD,\tmuAOA,\tsigmaAOA,\tmuZOA,\tsigmaZOA"<<endl;
    for(int srvc_cnt=0;srvc_cnt<associationInfoPtr->serviceNodesPerTask(currentTaskid).length();srvc_cnt++)
    {
        for(int srvr_cnt=0;srvr_cnt<associationInfoPtr->serverNodes.length();srvr_cnt++)
        {
            int srvcID = associationInfoPtr->serviceNodesPerTask(currentTaskid)(srvc_cnt);
            int srvrID = associationInfoPtr->serverNodes(srvr_cnt);
            SCMLink scmLink = channelCloud->getSCMLink(srvcID,srvrID);
            op<<srvcID<<",\t"<<srvrID<<",\t"<<scmLink.getClusterSpread()<<endl;
        }
    }
    op.close();
}

void printSCMparams(McellSystem* mySysPtr,ivec serviceNodes,ivec serverNodes,string filename)
{
    string tempFileName = filename+".temp"+toString(currentTaskid);
    ofstream os(tempFileName.c_str(),std::ios::out);
    if(currentTaskid==0)
    {
        os<<"%srvcNode,\tsrvrNode,\tDS (ns),\tASD,\tZSD,\tASA,\tZSA,\tSF,\tK_rice \t NOTE: All the angles are in degree"<<endl;
    }
    double UE_sigma_AOD,UE_sigma_AOA,UE_sigma_DS,UE_sigma_ZOA,UE_sigma_ZOD;
    for(int srvc_cnt=0;srvc_cnt<serviceNodes.length();srvc_cnt++)
    {
        for(int srvr_cnt=0;srvr_cnt<serverNodes.length();srvr_cnt++)
        {
            NodeLinkInfo_S linkInfo = mySysPtr->getLinkInfo(serverNodes(srvr_cnt),serviceNodes(srvc_cnt));
            SCMLargeScaleParameters_S channel5GParameters = linkInfo.scmLargeScaleParams;
            
            UE_sigma_DS = pow(10.0,channel5GParameters.ds); 
            UE_sigma_AOD = pow(10.0,channel5GParameters.asd); 
            UE_sigma_AOA = pow(10.0,channel5GParameters.asa);  
            UE_sigma_ZOA = pow(10.0,channel5GParameters.zsa); 
            UE_sigma_ZOD = pow(10.0,channel5GParameters.zsd);
            
            if(UE_sigma_AOA >104.0)
                UE_sigma_AOA = 104.0;
            if(UE_sigma_AOD >104.0)
                UE_sigma_AOD = 104.0;
            if(UE_sigma_ZOA >52.0)
                UE_sigma_ZOA = 52.0;
            if(UE_sigma_ZOD >52.0)
                UE_sigma_ZOD = 52.0;
            
            os<<serviceNodes(srvc_cnt)<<",\t"<<serverNodes(srvr_cnt)<<",\t"<<UE_sigma_DS*1e9<<",\t"<<UE_sigma_AOD<<",\t"<<UE_sigma_ZOD<<",\t";
            os<<UE_sigma_AOA<<",\t"<<UE_sigma_ZOA<<",\t"<<channel5GParameters.sf<<",\t"<<inv_dB(channel5GParameters.k)<<endl;
        }
    }
    os.close();
}

/*
void printSingularValues(ChannelCloud* channelCloud,AssociationInfo_S& associationInfo,NUParams_S* nuParams,string fileName)
{
    string tempFileName = fileName+".temp"+toString(currentTaskid);
    std::ofstream fout(tempFileName.c_str(),std::ios::out);
    if(currentTaskid==0)
        fout<<"%srvcNode,\tassociatedSrvrNode,\tRBCnt,\tmaxSV,\tminSV,\tRatio...NOTE: all the values are in dB.."<<endl;
    
    for(int srvc_cnt=0;srvc_cnt<associationInfo.serviceNodesPerTask(currentTaskid).length();srvc_cnt++)
    {
        int srvcNodeID = associationInfo.serviceNodesPerTask(currentTaskid)(srvc_cnt);
        int srvcNodeIndx = find(associationInfo.serviceNodes,srvcNodeID);
        int srvrNodeID = associationInfo.associatedServerNodesForEachServiceNode(srvcNodeIndx);
        sBeamPairLink beamPair = associationInfo.associatedBeamPairLinkforEachServiceNode(srvcNodeIndx);
        
        ivec txTXRUs = channelCloud->nodesInfo.getTXRUs(srvrNodeID,beamPair.srvrBeamID,beamPair.txPanelID);
        ivec rxTXRUs = channelCloud->nodesInfo.getTXRUs(srvcNodeID,beamPair.srvcBeamID,beamPair.rxPanelID);
        
        int txAntennaCount = txTXRUs.length();
        int rxAntennaCount = rxTXRUs.length();
        int txOrientationID = channelCloud->nodesInfo.getOrientationID(srvrNodeID,beamPair.txPanelID);
        int rxOrientationID = channelCloud->nodesInfo.getOrientationID(srvcNodeID,beamPair.rxPanelID);
        
        Array<Array<cvec>> channel(txAntennaCount);
        
        for(int txAntCnt=0;txAntCnt<txAntennaCount;txAntCnt++)
        {
            channel(txAntCnt).set_length(rxAntennaCount);
            for(int rxAntCnt=0;rxAntCnt<rxAntennaCount;rxAntCnt++)
            {
                channel(txAntCnt)(rxAntCnt) = channelCloud->generate5GAASChannelTapGains(srvrNodeID,srvcNodeID,txTXRUs(txAntCnt),rxTXRUs(rxAntCnt),0.0,false,beamPair.srvrBeamID,beamPair.srvcBeamID,beamPair.txPanelID,beamPair.rxPanelID,txOrientationID,rxOrientationID);
            }
        }
        vec tapDelays = channelCloud->getSCMLink(srvcNodeID,srvrNodeID).getTapDelays();
        Array<vec> singularValuesInDB = computeSingularValuesPerRB(channel,tapDelays,nuParams,txAntennaCount,rxAntennaCount);
        
        for(int rbCnt=0;rbCnt<singularValuesInDB.length();rbCnt++)
        {
            fout<<srvcNodeID<<",\t"<<srvrNodeID<<",\t"<<rbCnt<<",\t";
            for(int rxAntCnt=0;rxAntCnt<rxAntennaCount;rxAntCnt++)
            {
                fout<<singularValuesInDB(rbCnt)(rxAntCnt)<<",\t";
            }
            fout<<max(singularValuesInDB(rbCnt)) - min(singularValuesInDB(rbCnt))<<endl;
        }
    }
    fout.close();
}*/

void printAngleSpread(ChannelCloud* channelCloud,AssociationInfo_S& associationInfo,string fileName,string clusterSpreadFileName)
{
    string tempFileName = fileName +".temp"+toString(currentTaskid);
    ofstream op(tempFileName.c_str(),std::ios::out);
    ofstream out(clusterSpreadFileName+".temp"+toString(currentTaskid),std::ios::out);
    if(currentTaskid==0)
    {
        op<<"%srvcNode,\tassociatedSrvrNode,\tmuDS(ns),\tsigmaDS(ns),\tmuAOD,\tsigmaAOD,\tmuZOD,\tsigmaZOD,\tmuAOA,\tsigmaAOA,\tmuZOA,\tsigmaZOA"<<endl;
        out<<"%srvcNode,\tassociatedSrvrNode,\tmuDS(ns),\tsigmaDS(ns),\tmuAOD,\tsigmaAOD,\tmuZOD,\tsigmaZOD,\tmuAOA,\tsigmaAOA,\tmuZOA,\tsigmaZOA"<<endl;
    }
    for(int srvc_cnt=0;srvc_cnt<associationInfo.serviceNodesPerTask(currentTaskid).length();srvc_cnt++)
    {
        int srvcNodeID = associationInfo.serviceNodesPerTask(currentTaskid)(srvc_cnt);
        int srvcNodeIndx = find(associationInfo.serviceNodes,srvcNodeID);
        int srvrNodeID = associationInfo.associatedServerNodesForEachServiceNode(srvcNodeIndx);
        sBeamPairLink beamPair = associationInfo.associatedBeamPairLinkforEachServiceNode(srvcNodeIndx);

        int scmLinkIndx = channelCloud->getScmLinkIndx(srvcNodeID,srvrNodeID);
        
        SCMLink* scmLink = &channelCloud->scmLinks(scmLinkIndx);
        if(scmLink->isSCMChannelInitialized())
        {
            op<<srvcNodeID<<",\t"<<srvrNodeID<<",\t"<<scmLink->getAngleAndDelaySpread()<<endl;
            out<<srvcNodeID<<",\t"<<srvrNodeID<<",\t"<<scmLink->getClusterSpread()<<endl;
        }
        else
        {
            return;
        }
    }
    op.close();out.close();
}
void printCouplingGain(AssociationInfo_S& associationInfo,mat linkGainInDB,string filename,bool isBeam)
{
    ofstream fout;
    if(!isBeam)
        fout.open(filename.c_str(),ios::out);
    else
    {
        string tempFileName = filename+".temp"+toString(currentTaskid);
        fout.open(tempFileName.c_str(),ios::out);
    }
    if(currentTaskid==0)
        fout<<"%serviceNode,\t\tassociatedServerNode,\t\tcouplingGainInDB"<<endl;
    
    for(int iServiceNode=0;iServiceNode<linkGainInDB.rows(); iServiceNode++)
    {
        int serviceNodeIndx = (!associationInfo.serviceNodesPerTask.length()) ? iServiceNode : find(associationInfo.serviceNodes,associationInfo.serviceNodesPerTask(currentTaskid)(iServiceNode));
        fout<<associationInfo.serviceNodes(serviceNodeIndx)<<",\t\t"<<associationInfo.associatedServerNodesForEachServiceNode(serviceNodeIndx)<<
        ",\t\t"<<linkGainInDB(iServiceNode,find (associationInfo.serverNodes,associationInfo.associatedServerNodesForEachServiceNode(serviceNodeIndx)))<<endl;
        
    }
    fout.close();
}

void printSINRDistribution(AssociationInfo_S& associationInfo,mat downlinkPowerMatrixInDBm,vec serviceNodeNoiseVariance,string filename,bool isBeam){
    
    double SINR=0.0;
    double desiredRSSI,RSRP;
    vec preprocessingSINRvec(0);
    string tempFileName;
    ofstream fout;
    if(!isBeam)
        fout.open(filename.c_str(),ios::out);
    else
    {
        tempFileName = filename+".temp"+toString(currentTaskid);
        fout.open(tempFileName.c_str(),ios::out);
    }
    if(currentTaskid==0)
        fout<<"%serviceNodeID ,\t associatedServerNodeID ,\t RSRP(dB) ,\t SINR(dB)"<<endl;
    
    for(int iServiceNode=0;iServiceNode<downlinkPowerMatrixInDBm.rows(); iServiceNode++)
    {
        int srvcNodeIndx = (!associationInfo.serviceNodesPerTask.length()) ? iServiceNode : find(associationInfo.serviceNodes,associationInfo.serviceNodesPerTask(currentTaskid)(iServiceNode));
        int associatedServerNode=associationInfo.associatedServerNodesForEachServiceNode(srvcNodeIndx);
        int associatedServerNodeIdx=find(associationInfo.serverNodes,associatedServerNode);
        desiredRSSI=downlinkPowerMatrixInDBm(iServiceNode,associatedServerNodeIdx);
        SINR=inv_dB(desiredRSSI)/(sum(inv_dB(downlinkPowerMatrixInDBm.get_row(iServiceNode)))-inv_dB(desiredRSSI) + serviceNodeNoiseVariance(iServiceNode));
        append(preprocessingSINRvec,SINR);
        fout<<setw(5)<<associationInfo.serviceNodes(srvcNodeIndx)<<","<<setw(22)<<associationInfo.associatedServerNodesForEachServiceNode(srvcNodeIndx)<<","<<setw(22)<<desiredRSSI<<","<<setw(22)<<dB(SINR)<<endl;
    }
    fout.close();
    tempFileName=filename+"Temp";
    fout.open(tempFileName.c_str(), std::ios::app);
    fout<<"serviceNodes="<<associationInfo.serviceNodes<<endl;
    fout<<"preprocessingSINRData="<<dB(preprocessingSINRvec)<<endl;
    fout.close();
}
