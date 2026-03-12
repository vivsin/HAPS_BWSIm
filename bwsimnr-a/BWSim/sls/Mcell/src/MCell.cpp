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


#include "../include/MCell.h"
// #define DisablePathSpecificGainInSCM


bvec selectNodesforSimulation(string sysConfigFileName,bool isWithPicos, bool isWithRelays){

  ivec nodeCounts;
  Array<string> nodeTypeNames;
  parse(sysConfigFileName,"nodeCounts",nodeCounts);
  parse(sysConfigFileName,"nodeTypeNames",nodeTypeNames);
  bvec nodeTypeSelector=ones_b(nodeCounts.length());
  int indx;

  indx=findInStringArray(nodeTypeNames,"PICO");
  if(indx!=-1)
  {
    if(nodeCounts(indx)==0)
    {
      isWithPicos=false;
      nodeTypeSelector(indx)=0; //Disabling Picos

//       //Disabling PicoMS as there are no Picos
//       indx=findInStringArray(nodeTypeNames,"PicoUE");
//       if(indx!=-1)
//       {
// 	if(nodeCounts(indx)!=0)	cout<<"[detl:]Diabling all PicoUE in the config file as per the configuration."<<endl;
// 	nodeTypeSelector(indx)=0;
//       }
    }
    else
      isWithPicos=true;
  }
  else isWithPicos=false;

  indx=findInStringArray(nodeTypeNames,"RS");
  if(indx!=-1)
  {
    if(nodeCounts(indx)==0)
    {
      isWithRelays=false;
      nodeTypeSelector(indx)=0; //Disabling RS

      //Disabling RT as there are no RS
      indx=findInStringArray(nodeTypeNames,"RT");
      if(indx!=-1)
      {
	if(nodeCounts(indx)!=0)	cout<<"[detl:]Diabling all RT in the config file as per the configuration."<<endl;
	nodeTypeSelector(indx)=0;
      }
      //Disabling RelayMS as there are no RS
      indx=findInStringArray(nodeTypeNames,"RelayUE");
      if(indx!=-1)
      {
	if(nodeCounts(indx)!=0)	cout<<"[detl:]Diabling all RelayUE in the config file as per the configuration."<<endl;
	nodeTypeSelector(indx)=0;
      }
    }
    else
      isWithRelays=true;
  }
  else isWithRelays=false;


 return(nodeTypeSelector);
}


void cropFilesForMaximumServiceNodes(McellSystem &mcellSystem,string linkTableFileName, string nodeLocationsFileName, ivec serverNodes, ivec serviceNodes){
    
    string nativeFile=linkTableFileName+"Org";
    string exe="mv "+linkTableFileName+" "+nativeFile;
    system_exec(exe.c_str());
    std::ofstream fout(linkTableFileName.c_str(), std::ios::out);
    
    ivec nativeServerList, nativeServiceList,srvrIndx, srvcIndx;
    
    if(parse(nativeFile,"serverNodeList",nativeServerList)==false)  {	cout<<"[both:]Error... Unable to load serverNodeList from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    if(parse(nativeFile,"serviceNodeList",nativeServiceList)==false)  {	cout<<"[both:]Error... Unable to load serviceNodeList from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    
    if(serverNodes.length()!=0)	srvrIndx=find(nativeServerList,serverNodes);
    else				srvrIndx=getIntegers(0,nativeServerList.length()-1);
    if(serviceNodes.length()!=0)	srvcIndx=find(nativeServiceList,serviceNodes);
    else				srvcIndx=getIntegers(0,nativeServiceList.length()-1);
    
    ivec temp1;
    vec temp2;
    mat temp3;
    
    fout<<"serverNodeList="<<nativeServerList(srvrIndx)<<endl;
    fout<<"serviceNodeList="<<nativeServiceList(srvcIndx)<<endl;
    
    ivec nativeNodeList;
    if(parse(nativeFile,"nodeList",nativeNodeList)==false)  {	cout<<"[both:]Error... Unable to load nodeList from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    
    ivec nodeList=concat(nativeServerList(srvrIndx),nativeServiceList(srvcIndx));
    fout<<"nodeList="<<nodeList<<endl;
    
    ivec nodeIndx=find(nativeNodeList,nodeList);
    
    vec carrierFrequencyInHz;
    if(parse(nativeFile,"carrierFrequencyInHz",carrierFrequencyInHz)==false)  {	cout<<"[both:]Error... Unable to load carrierFrequencyInHz from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"carrierFrequencyInHz="<<carrierFrequencyInHz(nodeIndx)<<endl;
    
    vec nodeVelocity;
    if(parse(nativeFile,"nodeVelocity",nodeVelocity)==false)  {	cout<<"[both:]Error... Unable to load nodeVelocity from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"nodeVelocity="<<nodeVelocity(nodeIndx)<<endl;
    
    if(parse(nativeFile,"serverNodeType",temp1)==false)  {	cout<<"[both:]Error... Unable to load serverNodeType from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serverNodeType="<<temp1(srvrIndx)<<endl;
    
    if(parse(nativeFile,"serviceNodeTypes",temp1)==false)  {	cout<<"[both:]Error... Unable to load serviceNodeType from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serviceNodeTypes="<<temp1(srvcIndx)<<endl;
    
    
    if(parse(nativeFile,"TDDConfigPerServerNode",temp1)==false)  {	cout<<"[both:]Error... Unable to load TDDConfigPerServerNode from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"TDDConfigPerServerNode="<<temp1(srvrIndx)<<endl;
    
    if(parse(nativeFile,"serverNodeIDCell",temp1)==false)  {	cout<<"[both:]Error... Unable to load serverNodeIDCell from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serverNodeIDCell="<<temp1(srvrIndx)<<endl;
    
    if(parse(nativeFile,"serviceNodeIDCell",temp1)==false)  {	cout<<"[both:]Error... Unable to load serviceNodeIDCell from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serviceNodeIDCell="<<temp1(srvcIndx)<<endl;
    
    if(parse(nativeFile,"serverNodeNetworkIDs",temp1)==false)  {	cout<<"[both:]Error... Unable to load serverNodeNetworkIDs from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serverNodeNetworkIDs="<<temp1(srvrIndx)<<endl;
    
    if(parse(nativeFile,"serviceNodeNetworkIDs",temp1)==false)  {	cout<<"[both:]Error... Unable to load serviceNodeNetworkIDs from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serviceNodeNetworkIDs="<<temp1(srvcIndx)<<endl;
    
    
    if(parse(nativeFile,"serverNodeCentralUnitID",temp1)==false)  {	cout<<"[both:]Error... Unable to load serverNodeCentralUnitID from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serverNodeCentralUnitID="<<temp1(srvrIndx)<<endl;
    if(parse(nativeFile,"serviceNodeCentralUnitID",temp1)==false)  {	cout<<"[both:]Error... Unable to load serviceNodeCentralUnitID from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serviceNodeCentralUnitID="<<temp1(srvcIndx)<<endl;
    
    Array<ivec> temp111;
    if(parse(nativeFile,"serverNodeNumerologies",temp111)==false)  {	cout<<"[both:]Error... Unable to load serverNodeNumerologies from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serverNodeNumerologies="<<getFromArray(temp111,srvrIndx)<<endl;
    if(parse(nativeFile,"serviceNodeNumerologies",temp111)==false)  {	cout<<"[both:]Error... Unable to load serviceNodeNumerologies from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serviceNodeNumerologies="<<getFromArray(temp111,srvcIndx)<<endl;
    
    
    if(parse(nativeFile,"serviceNodeVelocity",temp2)==false)  {	cout<<"[both:]Error... Unable to load serviceNodeVelocity from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serviceNodeVelocity="<<temp2(srvcIndx)<<endl;
    
    if(parse(nativeFile,"serverNodeAntennaCount",temp1)==false)  {	cout<<"[both:]Error... Unable to load serverNodeAntennaCount from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serverNodeAntennaCount="<<temp1(srvrIndx)<<endl;
    if(parse(nativeFile,"serviceNodeAntennaCount",temp1)==false)  {	cout<<"[both:]Error... Unable to load serviceNodeAntennaCount from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serviceNodeAntennaCount="<<temp1(srvcIndx)<<endl;
    
    if(parse(nativeFile,"serverNodeTransmitPowerInDBm",temp2)==false)  {	cout<<"[both:]Error... Unable to load serverNodeTransmitPowerInDBm from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serverNodeTransmitPowerInDBm="<<temp2(srvrIndx)<<endl;
    if(parse(nativeFile,"serviceNodeTransmitPowerInDBm",temp2)==false)  {	cout<<"[both:]Error... Unable to load serviceNodeTransmitPowerInDBm from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serviceNodeTransmitPowerInDBm="<<temp2(srvcIndx)<<endl;
    
    if(parse(nativeFile,"serverNodeReceiveNoiseFigureInDB",temp2)==false)  {	cout<<"[both:]Error... Unable to load serverNodeReceiveNoiseFigureInDB from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serverNodeReceiveNoiseFigureInDB="<<temp2(srvrIndx)<<endl;
    if(parse(nativeFile,"serviceNodeReceiveNoiseFigureInDB",temp2)==false)  {	cout<<"[both:]Error... Unable to load serviceNodeReceiveNoiseFigureInDB from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    fout<<"serviceNodeReceiveNoiseFigureInDB="<<temp2(srvcIndx)<<endl;
    
    if(parse(nativeFile,"serverToServiceNodeLinkGainInDB",temp3)==false)  {	cout<<"[both:]Error... Unable to load serverToServiceNodeLinkGainInDB from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    
    temp3=temp3.get_rows(srvcIndx);
    temp3=temp3.get_cols(srvrIndx);
    fout<<"serverToServiceNodeLinkGainInDB=\"";
    for(int row_cnt=0;row_cnt<temp3.rows();row_cnt++)
    {
        for(int col_cnt=0;col_cnt<temp3.cols();col_cnt++)
            fout<<temp3(row_cnt,col_cnt)<<" ";
        fout<<";";
    }
    fout<<"\""<<endl;
    
    if(parse(nativeFile,"serviceToServerNodeLinkGainInDB",temp3)==false)  {	cout<<"[both:]Error... Unable to load serviceToServerNodeLinkGainInDB from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    
    temp3=temp3.get_rows(srvrIndx);
    temp3=temp3.get_cols(srvcIndx);
    fout<<"serviceToServerNodeLinkGainInDB=\"";
    for(int row_cnt=0;row_cnt<temp3.rows();row_cnt++)
    {
        for(int col_cnt=0;col_cnt<temp3.cols();col_cnt++)
            fout<<temp3(row_cnt,col_cnt)<<" ";
        fout<<";";
    }
    fout<<"\""<<endl;
    
    if(parse(nativeFile,"linkGainInDB",temp3)==false)  {	cout<<"[both:]Error... Unable to load linkGainInDB from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    temp3=temp3.get_rows(nodeIndx);
    temp3=temp3.get_cols(nodeIndx);
    fout<<"linkGainInDB=\"";
    for(int row_cnt=0;row_cnt<temp3.rows();row_cnt++)
    {
        for(int col_cnt=0;col_cnt<temp3.cols();col_cnt++)
            fout<<temp3(row_cnt,col_cnt)<<" ";
        fout<<";";
    }
    fout<<"\""<<endl;
    
    if(parse(nativeFile,"propagationDelayInSec",temp3)==false)  {	cout<<"[both:]Error... Unable to load propagationDelayInSec from the file in cropFilesForMaximumServiceNodes()..."<<endl;	abort();  }
    temp3=temp3.get_rows(nodeIndx);
    temp3=temp3.get_cols(nodeIndx);
    fout<<"propagationDelayInSec=\"";
    for(int row_cnt=0;row_cnt<temp3.rows();row_cnt++)
    {
        for(int col_cnt=0;col_cnt<temp3.cols();col_cnt++)
            fout<<temp3(row_cnt,col_cnt)<<" ";
        fout<<";";
    }
    fout<<"\""<<endl;
    
    fout.close();
    exe="rm "+nativeFile;
    system_exec(exe.c_str());
    
    /// Regenerating Node Locations file ...
    
    mcellSystem.printNodeLocations(nodeLocationsFileName,concat(nativeServerList(srvrIndx),nativeServiceList(srvcIndx)));
    
}

// void generateLinkTableAndChannelCloudPerCarrier(McellSystem &mySys, AssociationInfo_S &associationInfo, ConfigFileNames_S myConfigFiles , ResultFileNames_S resultFiles){
//     
//     if(currentTaskid==0) cout<<"[detl:]Generating Link Table and Channel Cloud per Component Carrier"<<endl;
//     bool regenLinkInfo=true;
//     CarrierInfo_S carrierInfo(myConfigFiles.sysConfigFileName);
//     double samplingFreqInHz=carrierInfo.samplingFrequencyInMHz*1e6;
//     vec carrierFreqInMHzPerServerNodeType = carrierInfo.carrierFreqInMHzPerServerNodeType;
//     mySys.carrierFreqInMHzPerServerNodeType=carrierInfo.carrierFreqInMHzPerServerNodeType;
//         
//     {
//         string tempLinkTableFileName = resultFiles.directory + "myLinkTable_carrier"+toString(carr_cnt)+".txt";
//         
//         ivec srvrNodeTypes = mySys.getServerNodeTypes();
//         for(int srvrType_cnt=0;srvrType_cnt<srvrNodeTypes.length();srvrType_cnt++)
//         {
//             ivec srvrnodes = mySys.getNodes(srvrNodeTypes(srvrType_cnt)); 
//             mySys.setNodeCarrierFrequencyByIndex(srvrnodes , carrierFreqInMHzPerServerNodeType(srvrType_cnt) * 1e6);
//         }
//         
//         if(currentTaskid == 0)
//         {
//             mySys.printLinkGainTable(tempLinkTableFileName,associationInfo.serverNodes,associationInfo.serviceNodes,true);
//             //       mcellCopy.printDetailedLinkInfo("./Results/detailedLinkInfoe_"+toString(carrierFreqInMHz(carr_cnt))+".txt");
//         }
//         #ifdef USING_MPI
//         MPI_Barrier(MPI_COMM_WORLD);
//         #endif
//         if(regenLinkInfo)
//             mySys.generateCrossLinkInfo(true,true);
//         
//         if(currentTaskid == 0 && nCarriersForCA>1)
//         {
//             mySys.printCrossLinkGainTable(tempLinkTableFileName,associationInfo.serverNodes,associationInfo.serviceNodes,true);
//             mySys.printCarrierFrequencyInfo(tempLinkTableFileName,associationInfo.serverNodes,associationInfo.serviceNodes,true);
//         }
//         #ifdef USING_MPI
//         MPI_Barrier(MPI_COMM_WORLD);
//         #endif
//         createChannelCloud(mySys,myConfigFiles,resultFiles,samplingFreqInHz);
//     }
// }

void loadCarrierAssociationInfo(McellSystem &mySys, AssociationInfo_S &associationInfo, string linkTableFileName){
    
    mySys.loadCarrierAssociationInfo(associationInfo.serviceNodes,associationInfo.associatedServerNodesForEachServiceNode);
    mySys.generateCrossLinkInfo();
    #ifdef USING_MPI
    if(currentTaskid==0)
    {
        #endif
        mySys.printCrossLinkGainTable(linkTableFileName,"","",true);
        mySys.printCarrierFrequencyInfo(linkTableFileName,"","",true);
        #ifdef USING_MPI
    }
    MPI_Barrier(MPI_COMM_WORLD);
    #endif
}

void generateMyMultiCell(McellSystem &mySys, ConfigFileNames_S myConfigFiles , ResultFileNames_S resultFiles, unsigned int simulationSeed) {
  cout<<"Inside generateMyMultiCell function"<<"\n"<<endl;
  bool createBuildings=false;
  parse(myConfigFiles.sysConfigFileName,"createBuildings", createBuildings);

  if(simulationSeed==0) 
  {
      cout<<"Invalid Random seed inside Mcell "<<endl;abort();
  }
  
  cout<<"[detl:]Using "<<simulationSeed<<" as Simulation Seed."<<endl;
  setSeed(simulationSeed);
  
  bvec nodeTypeSelector=selectNodesforSimulation(myConfigFiles.sysConfigFileName);

  	#ifdef USING_MPI
	if(currentTaskid==0) {
	#endif

	    cout<<"\n\n#################################################"<<endl;
	    cout<<"[both:]Initializing MultiCell System"<<endl;
	    cout<<"#################################################\n\n"<<endl;

	#ifdef USING_MPI
	}
	#endif
	// Below functions assumes hardcoded names for Pico and Relay related nodes ...
	mySys.initSystem(myConfigFiles.sysConfigFileName, myConfigFiles.AASConfigFileName, nodeTypeSelector);
	int nNodes = mySys.aNodes.length();
    cout<<"\nNumber of nodes:"<<nNodes<<"\n"<<"\n"<<endl;
    imat rndSeed = randi(nNodes,nNodes,10000,99999);
    mySys.rndSeedForPL = (rndSeed+rndSeed.T())/2;  //FIXME Using same seed for both downlink and uplink
    // Called outside loop to make sure Indoor parameters are set irrespective of whether buildings are there or not  -10010
	
	//!------------------------------- API Related---------------------------------------------------------------
	
// 	bool isAPIForPathLoss;
// 	parse(myConfigFiles.simulationConfigFileName,"isAPIForPathLoss", isAPIForPathLoss);
// 	Array<string> terrainNames="{ _TERRAIN_IMT_URBAN_MICRO_ _TERRAIN_IMT_URBAN_MACRO_ _TERRAIN_IMT_SUBURBAN_MACRO_ _TERRAIN_IMT_RURAL_MACRO_ }";
// 	if(isAPIForPathLoss)
// 	  mySys.sysTerrain.addTerrain(terrainNames,&getIMTPathLossForAPI);
	//--------------------End For API-------------------------------------------------------------------------
	mySys.initIndoorInfo(myConfigFiles.buildingInfoFileName,createBuildings); 
    mySys.initNodes(myConfigFiles.sysConfigFileName,myConfigFiles.AASConfigFileName);
	cout<<"dropNodes"<<endl;
    mySys.dropNodes();
    if(mySys.getCellLayout()==_CELLLAYOUT_HEXAGONAL_ || mySys.getCellLayout()==_CELLLAYOUT_NTN_)
        mySys.copySCMGridsForIntrasiteCorrelation();
    
    string ulDlMode;
    parse(myConfigFiles.simulationConfigFileName,"ulDlMode",ulDlMode);
      
//     if(ulDlMode == "_ULDL_MODE_FDD_")
//     {
//       ivec nodeIndices = mySys.getNodes(_NODECATEGORY_SERVICENODE_);
//       double bandGapInMHz;
//       parse(myConfigFiles.carrierAggregationFileName,"FDDBandGapInMHz",bandGapInMHz);
//       double carrierFreqInHz = mySys.getCarrierFreqInHz()(0);
//       mySys.setNodeCarrierFrequencyByIndex(nodeIndices,carrierFreqInHz+bandGapInMHz*1e6);
//     }
//     else 
//     {
    
//       ivec nodeIndices = mySys.getNodes(_NODECATEGORY_SERVICENODE_);
//       double carrierFreqInHz = mySys.getCarrierFreqInHz();
//       mySys.setNodeCarrierFrequencyByIndex(nodeIndices,carrierFreqInHz);

//     }
    
// //Antenna Pattern Check
// 	ivec UENodes = mySys.getNodes(1);
// // 	cout<<"UENodes:"<<UENodes<<endl;abort();
// 	for(int i=0;i<UENodes.length();i++)
// 	{
// 	  double x,y,r,theta;
// 	  r = 50;
// 	  theta = deg_to_rad(i);
// 	  x = r*cos(theta);
// 	  y = r*sin(theta);
// 	  mySys.moveNode(UENodes(i),x,y);
// 	}
// 	
	assignIDCell(mySys);

    assignNodeVelocity(mySys,myConfigFiles.sysConfigFileName); 
 
  
  if(createBuildings)
  {
    mySys.dropInBuildingNodes();// Moving inbuilding nodes inside building
    mySys.setNonInBuildingIndoorNodes();//Moving non-inbuilding nodes in the building footprint inside building
  }
//   bool useUpdatedLinkInfo = false;
//   parse(myConfigFiles.sysConfigFileName,"useUpdatedLinkInfo",useUpdatedLinkInfo);
//   if(!useUpdatedLinkInfo)
//   {
   mySys.updateNodeLocations(myConfigFiles.directory+"updatedNodeLocations.txt",false);
//   }
//   else
//   {
//       string locationFileName,lspFileName,linkInfoFileName;
//       parse(myConfigFiles.sysConfigFileName,"locationsFile",locationFileName);
//       parse(myConfigFiles.sysConfigFileName,"lspFile",lspFileName);
//       parse(myConfigFiles.sysConfigFileName,"linkInfoFile",linkInfoFileName);
//       useUpdatedLinkInfo = mySys.updateLocations(myConfigFiles.directory+locationFileName);
//       
//       if(useUpdatedLinkInfo) //NOTE: Since linkInfo depends on locations...linkInfo will be updated only if locations are updated. - Goraknath. 
//         useUpdatedLinkInfo = mySys.updateLinkInfo(myConfigFiles.directory+linkInfoFileName,myConfigFiles.directory+lspFileName);
//   }
  
  if(currentTaskid==0)
  {
      mySys.printNodeLocations(resultFiles.nodeLocationsFileName);
  }
  
    int nTasksForLinkInfoGeneration = 1; // Number of cores used to generate link info. 
    ivec serviceNodes = mySys.getNodes(_NODECATEGORY_SERVICENODE_);
    mySys.serviceNodesPerTask = splitVectorToUniformArray(serviceNodes,min(nTasksForLinkInfoGeneration,nTasks)); //NOTE: this Function is different from splitVectorToArray - Goraknath.
    ivec srvrNodeTypes = mySys.getServerNodeTypes();
    
    if(currentTaskid==0)
        mySys.generateLinkInfo(false,false);

    if(nTasks>1 && mySys.serviceNodesPerTask.length())
    {
    #ifdef USING_MPI
    MPI_Barrier(MPI_COMM_WORLD);
    MPIBCastLinkInfo(mySys);
    MPI_Barrier(MPI_COMM_WORLD);
    #endif
    }

    setSeed(simulationSeed);
  

  if(currentTaskid==0)
  { 
    mySys.printLinkGainTable(resultFiles.linkTableFileName,"","",true);
//     mySys.printDownlinkPowerMatrix(mySysInfo.resultFiles.directory+"DownlinkPowerMatrix.txt"); 
    mySys.printDetailedLinkInfo(resultFiles.directory+"detailedLinkInfo.txt");   
  }
  #ifdef USING_MPI
  MPI_Barrier(MPI_COMM_WORLD);
  #endif

  if(currentTaskid==0)
  {
    printTDDConfig(resultFiles.linkTableFileName,myConfigFiles.simulationConfigFileName);
  }
  #ifdef USING_MPI
  MPI_Barrier(MPI_COMM_WORLD);
  #endif
}


void performBeamBasedAssociation(McellSystem &mySys, AssociationInfo_S &associationInfo, ConfigFileNames_S myConfigFiles , ResultFileNames_S resultFiles)
{
    int nREsPerRB =12;
    vec serviceNodeReceiveNoiseFigureInDB;
    if(parse(resultFiles.linkTableFileName,"serviceNodeReceiveNoiseFigureInDB",serviceNodeReceiveNoiseFigureInDB)==false)  {	cout<<"[both:] Error... Unable to load serviceNodeReceiveNoiseFigureInDB from the file in associateNodes()..."<<endl;	abort();  }
    
    CarrierInfo_S carrierInfo(myConfigFiles.sysConfigFileName);
    
    int nodeIndx=0,nuIndx = 0; //FIXME Assuming 0th numerology and all Server Nodes use same numerology ...Must be modified accordingly for multiple numerology cases.. - Goraknath.
//     double carrierFreqInMHz = carrierInfo.carrierFreqInMHz;
    double bandwidthInHz = carrierInfo.carrierBandwidthInMHz*1e6;
    
    mat couplingGainInDB,RSRPindB;
    //cout << "RSRPindB: " << RSRPindB <<endl;
    computeRSRPforAllNodes(mySys,associationInfo, _RSRP_,couplingGainInDB,RSRPindB,resultFiles.beamRSRPfileName,resultFiles.linkTableFileName);
    computeMaxRSRPforAllNodes(mySys,associationInfo, _RSRP_,couplingGainInDB,RSRPindB,resultFiles.beamRSRPfileName,resultFiles.linkTableFileName);
    //cout<<"CG(dB)"<<couplingGainInDB<<endl;
    vec serviceNodeReceiveNoiseVariance = inv_dB(-203.8 + 10*log10(bandwidthInHz) + serviceNodeReceiveNoiseFigureInDB(find(associationInfo.serviceNodes,associationInfo.serviceNodesPerTask(currentTaskid))));
    associationInfo.computeGeometrySINR(RSRPindB,serviceNodeReceiveNoiseVariance);
    //cout<<"Geometry SINR"<<associationInfo.computeGeometrySINR(RSRPindB,serviceNodeReceiveNoiseVariance);
    if(currentTaskid==0)
        cout<<"[detl]: Printing Re-Association..."<<endl;
    
    string couplingGainFileName = resultFiles.directory+"CouplingGain_Downlink_RSRP.txt";
    string sinrDistributionFileName = resultFiles.directory+"SINRDistribution_RSRP.txt";
    string detailedLogsFileName = resultFiles.directory+"detailedBeamAssociationLogs.txt";
    string scmFileName = resultFiles.directory+"associatedLSPs.txt";
    string prbSVFileName = resultFiles.directory+"prbSingularValues.txt";
    string scmASFileName = resultFiles.directory+"scmAngleSpreads.txt";
    string scmCSFileName = resultFiles.directory+"rmsClusterSpreads.txt";
    string linkLogsFileName = resultFiles.directory+"detailedLinkLogs.txt";
    string scmLSPFileName = resultFiles.directory+"scmLSPs.txt";
    string angleSpreadFile = resultFiles.directory+"allClusterSpreads.txt";
    
    printAssociationLogs(couplingGainInDB,RSRPindB,associationInfo,serviceNodeReceiveNoiseVariance,resultFiles.associationLogsFileName,&mySys,1);
    printLinkGain(resultFiles.directory+"linkGainFile.txt",resultFiles.linkGainFileName,associationInfo,&mySys.channelCloud);
    
    if(traceValuePerTraceName["printCLandSINRLogs"]=="true")
    {
        printCouplingGain(associationInfo,couplingGainInDB,couplingGainFileName,1);
        printSINRDistribution(associationInfo,RSRPindB,serviceNodeReceiveNoiseVariance,sinrDistributionFileName,1);
    }
    
    if(traceValuePerTraceName["printDetailedLogs"]=="true")
    {
        printDetailedAssociationLogs(couplingGainInDB,RSRPindB,associationInfo,serviceNodeReceiveNoiseVariance,detailedLogsFileName,1);
        printSCMparams(&mySys,associationInfo.serviceNodesPerTask(currentTaskid),associationInfo.serverNodes,scmLSPFileName);
        printLinkLogs(&mySys,&associationInfo,couplingGainInDB,RSRPindB,serviceNodeReceiveNoiseVariance,linkLogsFileName);
        printClusterAngleSpread(&mySys.channelCloud,&associationInfo,angleSpreadFile);
    }
    
    if(traceValuePerTraceName["printAntennaGains"]=="true")
        mySys.channelCloud.printAntennaGains(associationInfo.associatedServerNodesForEachServiceNode(find(associationInfo.serviceNodes,associationInfo.serviceNodesPerTask(currentTaskid))),associationInfo.serviceNodesPerTask(currentTaskid),resultFiles.antennaGainsFileName);
    
    if(traceValuePerTraceName["printSCMparams"]=="true")
    {
        mySys.printSCMparams(associationInfo.serviceNodesPerTask(currentTaskid),associationInfo.associatedServerNodesForEachServiceNode(find(associationInfo.serviceNodes,associationInfo.serviceNodesPerTask(currentTaskid))),scmFileName);
        printAngleSpread(&mySys.channelCloud,associationInfo,scmASFileName,scmCSFileName);
    }
    //! Commented temporarily till find a way to eliminate nuConfig - Dhiv
//     if(traceValuePerTraceName["printSingularValues"]=="true")
//     {
//         double samplingFrequencyInHz = carrierInfo.samplingFrequencyInMHz*1e6;
//         NUParams_S nuParams; nuParams.init(getNUCaID(1,carrierFreqInMHz),nuConfig,nREsPerRB,carrierFreqInMHz,bandwidthInHz,samplingFrequencyInHz,0);
//         printSingularValues(&mySys.channelCloud,associationInfo,&nuParams,prbSVFileName);
//     }
    if(associationInfo.serviceNodesPerTask.length()> 1)
    {
        #ifdef USING_MPI
        MPI_Barrier(MPI_COMM_WORLD);
        #endif
    }
    if(currentTaskid==0)
    {
        mergeFiles(couplingGainFileName);
        mergeFiles(sinrDistributionFileName);
        mergeFiles(detailedLogsFileName);
        mergeFiles(scmFileName);
        mergeFiles(prbSVFileName);
        mergeFiles(scmASFileName);
        mergeFiles(scmCSFileName);
        mergeFiles(linkLogsFileName);
        mergeFiles(scmLSPFileName);
        mergeFiles(angleSpreadFile);
        mergeFiles(resultFiles.beamRSRPfileName);
        mergeFiles(resultFiles.associationLogsFileName);
        mergeFiles(resultFiles.linkGainFileName);
        mergeFiles(resultFiles.antennaGainsFileName);
    }
}


MCell createMcellNetwork(ConfigFileNames_S myConfigFiles , ResultFileNames_S resultFiles, unsigned int simSeed)
{
    McellSystem mySys;
    AssociationInfo_S associationInfo;
    cout<<"Inside createMcellNetwork function"<<endl;
    randomizeSeed();
    unsigned int simulationSeed=(unsigned int)randi(1,99999);
    
    if(simSeed==0)
    {
        
        bool useFixedSeed;
        if(!parse(myConfigFiles.simulationConfigFileName,"useFixedSeed",useFixedSeed))
            useFixedSeed=false;
        
        if(useFixedSeed)
            if(!parse(myConfigFiles.simulationConfigFileName,"seed",simulationSeed))
            {
                simulationSeed=12345;//(unsigned int)randi(0,99999);
                cout<<"[detl:]Loading default Fixed simulation seed as "<<simulationSeed<<endl;
            }
    }      
    else
    {
                simulationSeed=simSeed;//(unsigned int)randi(0,99999);
                cout<<"[detl:]Loading default Fixed simulation seed as "<<simulationSeed<<endl;
        
    }
            
        
    if(simulationSeed==0)
        simulationSeed=(unsigned int)randi(1,99999);

    
    // Generating Multi-Cell System
    generateMyMultiCell(mySys,myConfigFiles,resultFiles,simulationSeed);
    
    #ifdef USING_MPI
    MPI_Barrier(MPI_COMM_WORLD);
    #endif
    
    
    // Associate UEs to eNBs ///////////////////////////////////////////////////////////////////////////////////////////////////////
    Array<ivec> strongServerNodesPerServiceNode;
    
    associationInfo.load(resultFiles.linkTableFileName);
    strongServerNodesPerServiceNode = repeatAsArray(associationInfo.serverNodes,associationInfo.serviceNodes.length());
    
    loadCarrierAssociationInfo(mySys,associationInfo,resultFiles.linkTableFileName);
    
    associationInfo.serviceNodesPerTask=mySys.serviceNodesPerTask;
    
    // Adjusting LinkTable for restricted number of UEs per eNBs (Dropping in excess and removing others)
    if(currentTaskid==0){
        
        int maximumServiceNodesPerServerNode;
        parse(myConfigFiles.simulationConfigFileName,"maximumServiceNodesPerServerNode",maximumServiceNodesPerServerNode);
        if(maximumServiceNodesPerServerNode!=-1)   
            cropFilesForMaximumServiceNodes(mySys,resultFiles.linkTableFileName,resultFiles.nodeLocationsFileName,"",associationInfo.serviceNodes);
    }
    #ifdef USING_MPI
    MPI_Barrier(MPI_COMM_WORLD);
    #endif
    
    /// ---- feature/CarrierAggregation
    ivec serverNodeType;
    
    parse(resultFiles.linkTableFileName,"serverNodeType",serverNodeType);
    // Initializing channel cloud
    //Generating Link Table and Channel cloud per component carrier
    CarrierInfo_S carrierInfo(myConfigFiles.sysConfigFileName);
    double samplingFreqInHz=carrierInfo.samplingFrequencyInMHz*1e6;
    createChannelCloud(mySys,myConfigFiles,resultFiles,samplingFreqInHz);
    
    
    
    // RSRP (Formula based) ReAssociation
    //associateNodesBasedOnRSRPFormula(mySys,associationInfo,mySysInfo.myConfigFiles.linkTableFileName,strongServerNodesPerServiceNode);
    
    //   performLinkSpecificAntGainAssociation(mySys,associationInfo,mySysInfo.myConfigFiles.linkTableFileName,strongServerNodesPerServiceNode);
    //   abort();
    /// Perform initial-beam sweeping and association based on RSRP
    
    //   MPI_Bcast(mySysInfo.mpiInfo.schInfoField,sum(mySysInfo.mpiInfo.countsPerCU)*sizeof(MPI_SchedulingInfo_S),MPI_BYTE,0,MPI_COMM_WORLD);
    
    //FOr 
    #ifdef USING_MPI
    MPI_Barrier(MPI_COMM_WORLD);
    #endif
    
    if(currentTaskid==0)  
    {
        cout<<"Associating Nodes based on Beam sweeping"<<endl;
        performBeamBasedAssociation(mySys,associationInfo,myConfigFiles,resultFiles);
    }    
    
    #ifdef USING_MPI
    if(nTasks>1)
    {
        if(currentTaskid==0)
            cout<<"Exchanging Association Info across Processes..."<<endl;
        
        MPI_Barrier(MPI_COMM_WORLD);
        if(associationInfo.serviceNodesPerTask.length()>1)
            MPIExchangeAssociationInfo(associationInfo);
        else
            MPIBCastAssociationInfo(associationInfo);
        cout<<"Association Updated..."<<currentTaskid<<endl;
    }
    #endif
    

    associationInfo.update();
    associationInfo.printAssociationInfo(resultFiles.associationInfoFileName,resultFiles.beamAssociationInfoFileName);
    
    #ifdef USING_MPI
    MPI_Barrier(MPI_COMM_WORLD);
    #endif
    
    bool isForCalibration=false;
    parse(myConfigFiles.simulationConfigFileName,"isForCalibration",isForCalibration);
    
    #ifdef _USING_CALIBRATION_
    isForCalibration = true;
    #endif
    
    #ifdef USING_MPI
    MPI_Barrier(MPI_COMM_WORLD);
    #endif
    
    ivec currentSrvcNodes(0),currentSrvrNodes(0); 
    
    int srvrCount=associationInfo.serverNodes.length();
    ivec srvrCountPerTaskid=zeros_i(nTasks);
    for(int srvr_cnt=0;srvr_cnt<srvrCount;srvr_cnt++)
        srvrCountPerTaskid(mod(srvr_cnt,nTasks))++;
    
    int srvr_num=0;
    Array<ivec> srvrIndxListPerTaskid(nTasks);
    for(int task=0;task<nTasks;task++)
    {
        srvrIndxListPerTaskid(task).set_size(srvrCountPerTaskid(task));
        for(int srvr_indx=0;srvr_indx<srvrCountPerTaskid(task);srvr_indx++)
            srvrIndxListPerTaskid(task).set(srvr_indx,srvr_num++);
    }

    ivec temp=srvrIndxListPerTaskid(currentTaskid);
    for(int cnt=0;cnt<temp.length();cnt++)
    {
        append(currentSrvcNodes,associationInfo.associatedServiceNodesPerServerNode(temp(cnt)));
        append(currentSrvrNodes,associationInfo.serverNodes(temp(cnt)));
    }
    if(mySys.channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_)
    {
        Array<ivec> strongSrvrNodes;
        strongSrvrNodes=getFromArray(strongServerNodesPerServiceNode,find(associationInfo.serviceNodes,currentSrvcNodes));
        
        string ulDlMode;
        parse(myConfigFiles.simulationConfigFileName,"ulDlMode",ulDlMode);
        
        bool maintainReciprocity;
        if(ulDlMode=="_ULDL_MODE_FDD_")	maintainReciprocity=false;
        else	maintainReciprocity=true;
        
        bool initializeReverseLink=true;
        if(ulDlMode=="_ULDL_MODE_DOWNLINK_")	initializeReverseLink=false;
        
        if(associationInfo.serviceNodesPerTask.length()==1)
        {
            
            if(currentTaskid != 0 )
                initSCMLink(mySys,currentSrvcNodes,strongSrvrNodes,initializeReverseLink,maintainReciprocity,false);
            else
                retainSCMLinks(mySys.channelCloud,currentSrvcNodes,strongSrvrNodes); // need to change this for cross link simulation and CA simulation - Dhiv
        }
        else
        {
            retainSCMLinks(mySys.channelCloud,currentSrvcNodes,strongSrvrNodes);
            initSCMLink(mySys,currentSrvcNodes,strongSrvrNodes,initializeReverseLink,maintainReciprocity);
        }
        mySys.clearRedundantMemory(currentSrvcNodes,strongSrvrNodes);
    }
    
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    mySys.initMobilityInfo(myConfigFiles.simulationConfigFileName);
    
    MCell mcellReturn;
    mcellReturn.mySys=mySys;
    mcellReturn.associationInfo = associationInfo;
    return mcellReturn;
    
}



void assignIDCell(McellSystem &mySys) {

    ivec serverNodes=mySys.getNodes(_NODECATEGORY_SERVERNODE_);
    mySys.setNodeIDCellByIndex(serverNodes,getIntegers(0,serverNodes.length()-1));
}

void printTDDConfig(string linkTableFileName, string simulationConfigFileName) {
 
  ivec serverNodeType;
  parse(linkTableFileName,"serverNodeType",serverNodeType);
  ivec uniqueTypes=findUniqueNumbers(serverNodeType);
  
  ivec TDDConfigPerServerNodeType;
  parse(simulationConfigFileName,"TDDConfigPerServerNodeType",TDDConfigPerServerNodeType);
  if(TDDConfigPerServerNodeType.length()<uniqueTypes.length())
  {
    cout<<"Loading 0 as default TDD mode for uninitialized nodes.."<<endl;
    append(TDDConfigPerServerNodeType,zeros_i(uniqueTypes.length()-TDDConfigPerServerNodeType.length()));
  }
  else if(TDDConfigPerServerNodeType.length()>uniqueTypes.length())
    TDDConfigPerServerNodeType=TDDConfigPerServerNodeType.left(uniqueTypes.length());
  
  if(findUniqueNumbers(TDDConfigPerServerNodeType).length()!=1)
  {
    cout<<"[detl]: Mixed TDD configuration not supported due to special subframe constraint."<<endl;
    abort();
  }
  
  ivec tddConfigPerNodeType=zeros_i(max(uniqueTypes)+1);
  setInVec(tddConfigPerNodeType,uniqueTypes,TDDConfigPerServerNodeType);
  ivec TDDConfigPerServerNode=tddConfigPerNodeType(serverNodeType);
  
  std::ofstream fout(linkTableFileName.c_str(), std::ios::app);
  fout<<"TDDConfigPerServerNode="<<TDDConfigPerServerNode<<endl;
  fout.close();
}

void assignNodeVelocity(McellSystem &mySys,string sysConfigFileName) {

  bool fixedUserVelocity=true;
  parse(sysConfigFileName,"fixedUserVelocity",fixedUserVelocity);
  ivec serviceNodes=mySys.getNodes(_NODECATEGORY_SERVICENODE_);
  if(fixedUserVelocity==false)
  {
    double maximumUserVelocity,minimumUserVelocity;
    if(!parse(sysConfigFileName,"maximumUserVelocity",maximumUserVelocity))
    {
      cout<<"[both:]Unable to parse maximumUserVelocity from configFile for random Doppler. Default value not possible."<<endl;
      abort();
    }
    if(!parse(sysConfigFileName,"minimumUserVelocity",minimumUserVelocity))
    {
      cout<<"[both:]Unable to parse minimumUserVelocity from configFile for random Doppler. Default value not possible."<<endl;
      abort();
    }
    double range;
    range=maximumUserVelocity-minimumUserVelocity;
    
    mySys.setNodeVelocityByIndex(serviceNodes,randu(serviceNodes.length())*range+minimumUserVelocity);
  }
  else  
  {
    ivec serviceNodeTypes=mySys.getServiceNodeTypes();
    vec serviceNodeVelocity;
    if(!parse(sysConfigFileName,"serviceNodeVelocity",serviceNodeVelocity))
    {
      cout<<"[both:]Unable to parse serviceNodeVelocity from configFile. Default value not possible."<<endl;
      abort();
    }
    for(int cnt=0;cnt<serviceNodeTypes.length();cnt++)
      mySys.setNodeVelocity(serviceNodeTypes(cnt),serviceNodeVelocity(cnt)*ones(mySys.getNodeCount(serviceNodeTypes(cnt))));
  }
}


void initMcellForLinkSimulation(McellSystem &mySys,string sysConfigFileName,string AASFileName){
  
  mySys.setNodeCount("1 1");
  mySys.nameNodeType(0,"BS");
  mySys.nameNodeType(1,"MS");
  mySys.setNodeDropMethod("BS",_DROPMETHOD_RANDOM_);
  mySys.setNodeDropMethod("MS",_DROPMETHOD_RANDOM_);
  mySys.setTerrain("_TERRAIN_NOLOSS_");
  loadNoLossTerrain(mySys.sysTerrain);
  
  mySys.initSystem();
  mySys.setNodeCategory("BS",_NODECATEGORY_SERVERNODE_);
  mySys.setNodeCategory("MS",_NODECATEGORY_SERVICENODE_);
  
  //Setting carrierFreq
  double carrierFreqInMHz;
  parse(sysConfigFileName,"carrierFreqInMHz", carrierFreqInMHz);
  mySys.setNodeCarrierFrequencyInHz(0,carrierFreqInMHz*1e6);
  
  //Setting velocity
  assignNodeVelocity(mySys,sysConfigFileName);
  
  mySys.dropNodes();
  
  mySys.aNodes(0).setAntenna(_ANTENNA_TYPE_OMNI_,0,0,-1,0,-1,-1,90,-1); 
  mySys.aNodes(1).setAntenna(_ANTENNA_TYPE_OMNI_,0,0,-1,0,-1,-1,90,-1); 
  mySys.loadAASConfig(AASFileName,SCMPropagationScenario(0),1);
  
  //Wont' be used parameters////
  mySys.setShadowType(_SHADOWTYPE_UNCORRELATED_);
  mySys.setCoverageArea(100,100);
  mySys.setNodeHeight("BS", 25);
  mySys.setNodeHeight("MS", 1.5);
  mySys.setNodeTxPowerInDBm("BS",0);
  mySys.setNodeNoiseFigure(0,5);
  mySys.setNodeNoiseFigure(1,7);
  ///////////////////////////////
  
  ivec nodeAntennaCount;
//   if(!parse(AASFileName,"numTxRUs", nodeAntennaCount))
//   {
//     cout<<"no nodeAntennaCount variable found ..."<<endl;abort();
//   }
  
  Array<ivec > TXRUMapTemp;Array<ivec> AntStructureTemp;
  if(parse(AASFileName,"TXRUMap",TXRUMapTemp)&&parse(AASFileName,"AntStructure",AntStructureTemp)){
      nodeAntennaCount.set_length(TXRUMapTemp.length());
      for(int nodeTypeCnt = 0;nodeTypeCnt < TXRUMapTemp.length();nodeTypeCnt++){
          nodeAntennaCount(nodeTypeCnt) = prod(TXRUMapTemp(nodeTypeCnt))*AntStructureTemp(nodeTypeCnt)(4);
      }
  }else{cout<<"Unable to parse TXRUMap and AntStructure from ActiveAntennaSystemConfig.txt .. aborting"<<endl; abort();}
  cout<<endl<<"nodeAntennaCount 1: "<<nodeAntennaCount<<endl;abort();
  
  if(nodeAntennaCount.length()<2)
  {cout<<"[both:]Error .. Insufficient nodeAntennaCount length for link-level simulation..."<<endl;abort();}
    
  if(nodeAntennaCount.length()>2)
    cout<<"[both:]Considering only two nodeTypes \"BS\" and \"MS\" and only the first two nodeAntennaCount respectively .. "<<endl;
  
  mySys.setNodeAntennaCount(0,nodeAntennaCount(0));  
  mySys.setNodeAntennaCount(1,nodeAntennaCount(1));  
  
//   Array<vec> antennaWavelengthSpacing;
//   parse(AASFileName,"AntSpacing", antennaWavelengthSpacing);
//   mySys.setNodeAntennaSpacing(0,antennaWavelengthSpacing(0)(3));
//   mySys.setNodeAntennaSpacing(1,antennaWavelengthSpacing(1)(3));
  mySys.setNodeNumerologiesByIndex("0 1",to_ivec(1)); 
  assignIDCell(mySys);
}

