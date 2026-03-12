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


#include "../include/NodeDevice.h"

NodeDevice::NodeDevice(){
    nodeID=-1;
    nuIDs.set_length(0);
    receiverPerNU.set_length(0);
    transmitterPerNU.set_length(0);
    
    L1CorePerNU.set_length(0);
    L2CorePerNU.set_length(0);
    
    actualLoopCountPerNU.set_length(0);
    ttiCountPerNU.set_length(0);
  
};

//!\brief getTransmitterPointer() function will be called from main.cpp  
Array< Array<Transmitter*> > getTransmitterPointer(Array<NodeDevice> &ServerNodeDevices,Array<NodeDevice> &ServiceNodeDevices,UlDlServerNodes_S TDDserverNodes,Array< ivec >associatedServiceNodesPerServerNode,ivec serverNodes,ivec serviceNodes){
 
  Array< Array<Transmitter*> > txPointer; 
  int indx;
  Array<Transmitter*> emptyArray(0);
  int array_cnt=0;
  
 /*!\brief  in downlink server nodes are transmitters,copying server nodes pointers */
 
 for(int svrIndx =0;svrIndx<serverNodes.length();svrIndx++)
 {
  if(TDDserverNodes.dlServerNodes.length())  
    if(find(TDDserverNodes.dlServerNodes,serverNodes(svrIndx))!=-1)
    {
      append(txPointer,emptyArray);
      for(int nu_cnt=0;nu_cnt<ServerNodeDevices(svrIndx).transmitterPerNU.length();nu_cnt++) 
	append(txPointer(array_cnt),&ServerNodeDevices(svrIndx).transmitterPerNU(nu_cnt));
      array_cnt++;
      
    }
      
      /*!\brief  in uplink service nodes are transmitters,copying service nodes pointers  */
  if(TDDserverNodes.ulServerNodes.length())   
    if(find(TDDserverNodes.ulServerNodes,serverNodes(svrIndx))!=-1)
      for(int servicetx_cnt=0;servicetx_cnt<associatedServiceNodesPerServerNode(svrIndx).length();servicetx_cnt++) 
      {
      indx=find(serviceNodes,associatedServiceNodesPerServerNode((svrIndx))(servicetx_cnt));
      append(txPointer,emptyArray);
      for(int nu_cnt=0;nu_cnt<ServiceNodeDevices(indx).transmitterPerNU.length();nu_cnt++)
        append(txPointer(array_cnt),&ServiceNodeDevices(indx).transmitterPerNU(nu_cnt));
      array_cnt++;
      }
    }
  return(txPointer);
}


//!\brief getReceiverPointer() function will be called from main.cpp  
Array< Array<Receiver*> > getReceiverPointer(Array<NodeDevice> &ServerNodeDevices,Array<NodeDevice> &ServiceNodeDevices,UlDlServerNodes_S TDDserverNodes,Array< ivec >associatedServiceNodesPerServerNode,ivec serverNodes,ivec serviceNodes){
 
  Array< Array<Receiver*> > rxPointer(0); 
 int indx;
 Array<Receiver*> emptyArray(0);
 int array_cnt=0;
 /*!\brief in downlink service nodes are receivers,copying service nodes pointers  */
 for(int svrIndx =0;svrIndx<serverNodes.length();svrIndx++)
 {
   if(TDDserverNodes.dlServerNodes.length())  
    if(find(TDDserverNodes.dlServerNodes,serverNodes(svrIndx))!=-1)
	for(int service_cnt=0;service_cnt<associatedServiceNodesPerServerNode(svrIndx).length();service_cnt++) 
	{
	   append(rxPointer,emptyArray);
	   indx=find(serviceNodes,associatedServiceNodesPerServerNode((svrIndx))(service_cnt));
	   for(int nu_cnt=0;nu_cnt<ServiceNodeDevices(indx).receiverPerNU.length();nu_cnt++)
	    append(rxPointer(array_cnt),&ServiceNodeDevices(indx).receiverPerNU(nu_cnt));
	   array_cnt++;
	}
  
    
    /*!\brief in uplink server nodes are recivers,copying server nodes pointers */
    if(TDDserverNodes.ulServerNodes.length())  
      if(find(TDDserverNodes.ulServerNodes,serverNodes(svrIndx))!=-1)
      {
	append(rxPointer,emptyArray);
	for(int nu_cnt=0;nu_cnt<ServerNodeDevices(svrIndx).receiverPerNU.length();nu_cnt++)
	  append(rxPointer(array_cnt),&ServerNodeDevices(svrIndx).receiverPerNU(nu_cnt));  
	array_cnt++;
      }
 }
  return(rxPointer);
}


Array< Array<Transmitter*> > getTransmitterPointer(Array<NodeDevice> &nodeDevices){
  
  Array< Array<Transmitter*> > txPointer(nodeDevices.length());
  for(int tx_cnt=0;tx_cnt<txPointer.length();tx_cnt++)
  {
    txPointer(tx_cnt).set_length(nodeDevices(tx_cnt).transmitterPerNU.length());
    for(int nu_cnt=0;nu_cnt<nodeDevices(tx_cnt).transmitterPerNU.length();nu_cnt++)
      txPointer(tx_cnt)=&nodeDevices(tx_cnt).transmitterPerNU(nu_cnt);
  }
  return(txPointer);
}

Array< Array<Receiver*> > getReceiverPointer(Array<NodeDevice> &nodeDevices){
  
  Array< Array<Receiver*> > rxPointer(nodeDevices.length());
  for(int rx_cnt=0;rx_cnt<rxPointer.length();rx_cnt++)
  {
    rxPointer(rx_cnt).set_length(nodeDevices(rx_cnt).receiverPerNU.length());
    for(int nu_cnt=0;nu_cnt<nodeDevices(rx_cnt).receiverPerNU.length();nu_cnt++)
      rxPointer(rx_cnt)(nu_cnt)=&nodeDevices(rx_cnt).receiverPerNU(nu_cnt);
  }
  return(rxPointer);
}

Array<Transmitter*> getTransmitterPointer(Array<NodeDevice> &nodeDevices, ivec nodeIDs, ivec txNodes, int nuID){
  
  ivec nodeIndices=find(nodeIDs,txNodes);
  Array<Transmitter*> txPointer(txNodes.length());
  for(int tx_cnt=0;tx_cnt<txPointer.length();tx_cnt++)
  {
    for(int nu_cnt=0;nu_cnt<nodeDevices(nodeIndices(tx_cnt)).transmitterPerNU.length();nu_cnt++)
    {
      if(nodeDevices(nodeIndices(tx_cnt)).transmitterPerNU(nu_cnt).nuParams.nuID==nuID) 
      {
	txPointer(tx_cnt)=&nodeDevices(nodeIndices(tx_cnt)).transmitterPerNU(nu_cnt);
	break;
      }
    }
  }
  return(txPointer);
}

Array<Receiver*> getReceiverPointer(Array<NodeDevice> &nodeDevices, ivec nodeIDs, ivec rxNodes, int nuID){
  
  ivec nodeIndices=find(nodeIDs,rxNodes);
  Array<Receiver*> rxPointer(rxNodes.length());
  for(int rx_cnt=0;rx_cnt<rxPointer.length();rx_cnt++)
  {
    for(int nu_cnt=0;nu_cnt<nodeDevices(nodeIndices(rx_cnt)).receiverPerNU.length();nu_cnt++)
      if(nodeDevices(nodeIndices(rx_cnt)).receiverPerNU(nu_cnt).nuParams.nuID==nuID)
      {
	rxPointer(rx_cnt)=&nodeDevices(nodeIndices(rx_cnt)).receiverPerNU(nu_cnt);
	break;
      }
  }
  return(rxPointer);
}

// void moveUserNodes(McellSystem &mySys,SystemInfo_S &mySysInfo,AssociationInfo_S &associationInfo,Array<NodeDevice> &myNodeDevices, ivec nodeDeviceIDs , double currentTime, ivec serviceNodes){
//   
//   if(serviceNodes.length()==0)
//     serviceNodes=associationInfo.serviceNodes;
//   
//   ivec srvcNodeIndices=find(associationInfo.serviceNodes,serviceNodes);
//   ivec nativeServerNodeIndices=find(associationInfo.serverNodes,associationInfo.associatedServerNodesForEachServiceNode(srvcNodeIndices));
//   ivec srvcNodeDeviceIndices=find(nodeDeviceIDs,serviceNodes);
//   std::ofstream fout;
//   if(currentTime==0)
//   {
//     fout.open("./Results/mobilityTrack.txt", std::ios::out);
//     fout<<"ServiceNode\tTimeInSec\txLoc\t\tyLoc\t\tAssociatedServerNode\t\tLinkGains"<<endl;
//   }
//   else
//     fout.open("./Results/mobilityTrack.txt", std::ios::app);
//   
//   for(int srvc_cnt=0;srvc_cnt<serviceNodes.length();srvc_cnt++)
//   {
//     if(mySys.getMovableDistance(serviceNodes(srvc_cnt),currentTime)>=mySysInfo.mobilityConfig.refreshDistanceInMetres)
//     {
// 
//       ChannelGainInfo_S updatedChannelGainInfo= mySys.moveServiceNode(serviceNodes(srvc_cnt),currentTime);
//       Array<vec> linkGain(2);
//       linkGain(0)=updatedChannelGainInfo.downlinkGainsInDB;
//       linkGain(1)=updatedChannelGainInfo.uplinkGainsInDB;
//       // Link Gain is not same for all carriers.. Below code needs to be modified - Dhiv
//       for(int carrier_cnt=0;carrier_cnt<mySys.channelCloud.length();carrier_cnt++)
//       {
//         mySys.channelCloud(carrier_cnt).updateLinkGainInfo(serviceNodes(srvc_cnt),updatedChannelGainInfo.linkedNodeIDs,linkGain);
//         // SCM Initialization is independent of previous realization. Spacial consistency model may help - Dhiv
//         mySys.initSCMLink(to_ivec(serviceNodes(srvc_cnt)),updatedChannelGainInfo.linkedNodeIDs,carrier_cnt,true,true);
//       }
//       int oldServerNodeID = associationInfo.associatedServerNodesForEachServiceNode(srvcNodeIndices(srvc_cnt));
//       int newServerNodeID;
//       if(mySysInfo.enableBeamManagement)
//       {
//           newServerNodeID=performBeamBasedAssociation(serviceNodes(srvc_cnt),associationInfo.serverNodes,mySys,associationInfo).updatedCellID;
//       }
//       else
//       {
//         vec serverLinkGains=linkGain(0)(find(updatedChannelGainInfo.linkedNodeIDs,mySysInfo.serverNodes));
//         int maxIndx=max_index(serverLinkGains+mySysInfo.serverNodeTxPowersInDBm);
//         newServerNodeID=associationInfo.serverNodes(maxIndx);
//         associationInfo.reassociate(associationInfo.serviceNodes(srvc_cnt),newServerNodeID);
//       }
//       
//       if(oldServerNodeID!=newServerNodeID) // Reassociation happened
//       {
//         mySys.updateCarrierAssociationInfo(associationInfo.serviceNodes(srvc_cnt),newServerNodeID);
//         int newSrvrNodeDeviceIndx=find(nodeDeviceIDs,newServerNodeID);
//         int oldSrvrNodeDeviceIndx=find(nodeDeviceIDs,oldServerNodeID);
//         
//         ivec newNUIndices=find(myNodeDevices(newSrvrNodeDeviceIndx).nuIDs,myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).nuIDs);
//         ivec oldNUIndices=find(myNodeDevices(oldSrvrNodeDeviceIndx).nuIDs,myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).nuIDs);
//         
//         for(int nu_cnt=0;nu_cnt<myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU.length();nu_cnt++)
//         {
//             if(newNUIndices(nu_cnt)!=-1) // Nu supported at the handoff eNodeB
//             {
//                 if(myNodeDevices(newSrvrNodeDeviceIndx).L1CorePerNU(newNUIndices(nu_cnt)).downlinkSourceAndSink.isInitialized)
//                     myNodeDevices(newSrvrNodeDeviceIndx).L1CorePerNU(newNUIndices(nu_cnt)).downlinkSourceAndSink.moveServiceNode(associationInfo.serviceNodes(srvc_cnt),&(myNodeDevices(oldSrvrNodeDeviceIndx).L1CorePerNU(oldNUIndices(nu_cnt)).downlinkSourceAndSink));
//             
//                 if(myNodeDevices(newSrvrNodeDeviceIndx).L1CorePerNU(newNUIndices(nu_cnt)).uplinkSourceAndSink.isInitialized)
//                     myNodeDevices(newSrvrNodeDeviceIndx).L1CorePerNU(newNUIndices(nu_cnt)).uplinkSourceAndSink.moveServiceNode(associationInfo.serviceNodes(srvc_cnt),&(myNodeDevices(oldSrvrNodeDeviceIndx).L1CorePerNU(oldNUIndices(nu_cnt)).uplinkSourceAndSink));
//                 TransmissionConfig_S defaultUlTxConfig=myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulTxConfig;
//                 myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt)=myNodeDevices(newSrvrNodeDeviceIndx).L1CorePerNU(newNUIndices(nu_cnt));
//                 if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
//                 {
//                     myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulTxConfig=defaultUlTxConfig;
//                     int nuCaID=myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).nuIDs(nu_cnt);
//                     int configNUIndx=find(mySysInfo.myConfigFiles.nuIDs,getNUID(nuCaID));
//                     double ulRSBoostInDB; 
//                     parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName,"rsBoostInDB",ulRSBoostInDB);
//                     setRsInfo(myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulTxConfig.rsInfo,myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulPortInfo,myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulFrameStructure,myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).cellID,getTransmissionScheme(myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulTxConfig.transmissionMode),LayerInfo_S(myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulTxConfig.layerInfo), mySysInfo.serviceNodeAntenna(srvcNodeIndices(srvc_cnt)),ulRSBoostInDB,0,1,0);//dataStartSymbol hardcoded as zero - Dhiv
//                     myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulTxConfig.rsInfo.portReferenceNumbers=getPortReferenceNumbers(myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulFrameStructure,myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulTxConfig.rsInfo.rsPorts);
//                     // 	    checkConfigFiles(myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulTxConfig, mySysInfo.serviceNodeAntenna(srvcNodeIndices(srvc_cnt)),isDlMode);
//                     myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulRSInfoForUnScheduled=getRSInfoForUnScheduled(myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulTxConfig.rsInfo,myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulFrameStructure);
//                     myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulRSInfoForUnScheduled.portReferenceNumbers=getPortReferenceNumbers(myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulFrameStructure,myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulRSInfoForUnScheduled.rsPorts);
//                     //         
//                     double srsSubcarrierPowerInDBm=mySysInfo.serviceNodeTxPowersInDBm(srvc_cnt)-10*log10(300); // Hardcoding 300 for SRS subcarriers
//                     myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulCQIComputationConfig.init(nuCaID,myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulTxConfig.nCodeWords,myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulTxConfig.nLayers,srsSubcarrierPowerInDBm,false,false,0,getTransmissionScheme(myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulTxConfig.transmissionMode),_Waveform_OFDMA_,newServerNodeID,myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).ulTxConfig.rsInfo);
//                 } 
//                 
// //                 myNodeDevices(newSrvrNodeDeviceIndx).L2CorePerNU(newNUIndices(nu_cnt)).HandOffUE(associationInfo.serviceNodes(srvc_cnt),myNodeDevices(oldSrvrNodeDeviceIndx).L2CorePerNU(oldNUIndices(nu_cnt)));
//             }
//             
//             
//         }
//           
//     }
//       fout<<serviceNodes(srvc_cnt)<<"\t"<<currentTime<<"\t"<<mySys.getNodeLocation(serviceNodes(srvc_cnt)).x<<"\t"<<mySys.getNodeLocation(serviceNodes(srvc_cnt)).y<<"\t"<<associationInfo.associatedServerNodesForEachServiceNode(srvcNodeIndices(srvc_cnt))<<endl;
//     
//     }
//   }
//   fout.close();
// }

// void moveUserNodeByGivenDistance(int serviceNodeID,McellSystem &mySys,SystemInfo_S &mySysInfo,AssociationInfo_S &associationInfo,Array<CentralUnit> &myCentralUnits,Array<NodeDevice> &myNodeDevices, ivec nodeDeviceIDs,Array<CQIInfo_S> &cqiInfo , double currentTime, double distance)
//  { 
//   ivec nativeServerNodeIndices=find(associationInfo.serverNodes,associationInfo.associatedServerNodesForEachServiceNode);
//   std::ofstream fout;
//   if(currentTime==0)
//   {
//     fout.open("./Results/mobilityTrack.txt", std::ios::out);
//     fout<<"ServiceNode\tTimeInSec\txLoc\t\tyLoc\t\tAssociatedServerNode\t\tLinkGains"<<endl;
//   }
//   else
//     fout.open("./Results/mobilityTrack.txt", std::ios::app);
//   
//       int indx=find(associationInfo.serviceNodes,serviceNodeID);
//       if(distance==-1)
//       {
//        cout<<"Moving user "<<associationInfo.serviceNodes(indx)<<" to random location"<<endl;
//       }
//       else
//       cout<<"Moving user "<<associationInfo.serviceNodes(indx)<<" by "<<distance<< " mts"<<endl;
// 
//       ChannelGainInfo_S updatedChannelGainInfo= mySys.moveServiceNode(serviceNodeID,currentTime,distance);
// 
//       Array<vec> linkGain(2);
//       linkGain(0)=updatedChannelGainInfo.downlinkGainsInDB;
//       linkGain(1)=updatedChannelGainInfo.uplinkGainsInDB;
//       // Link Gain is not same for all carriers.. Below code needs to be modofied - Dhiv
//       for(int carrier_cnt=0;carrier_cnt<mySys.channelCloud.length();carrier_cnt++)
// 	mySys.channelCloud(carrier_cnt).updateLinkGainInfo(serviceNodeID,updatedChannelGainInfo.linkedNodeIDs,linkGain);
//       
//       vec serverLinkGains=linkGain(0)(find(updatedChannelGainInfo.linkedNodeIDs,mySysInfo.serverNodes));
//       int maxIndx=max_index(serverLinkGains+mySysInfo.serverNodeTxPowersInDBm);
//       reassociateNode(serviceNodeID,associationInfo.serverNodes(maxIndx),associationInfo.serverNodes(nativeServerNodeIndices(indx)),mySysInfo,mySys,associationInfo,myCentralUnits,myNodeDevices,nodeDeviceIDs,cqiInfo);
//      
//       fout<<serviceNodeID<<"\t"<<currentTime<<"\t"<<mySys.getNodeLocation(serviceNodeID).x<<"\t"<<mySys.getNodeLocation(serviceNodeID).y<<"\t"<<associationInfo.associatedServerNodesForEachServiceNode(indx)<<"\t\t"<<serverLinkGains<<endl;
//    
//   
//   fout.close();
//    
//  }
  
ivec getTTICount(ivec nodeDeviceIDs, Array<NodeDevice> &nodeDevices, ivec nodeIDs, int nuID)
{
  ivec ttiCount(nodeIDs.length());
  ivec nodeIndices=find(nodeDeviceIDs,nodeIDs);
  for(int node_cnt=0;node_cnt<nodeIDs.length();node_cnt++)
  {
    int nuIndx=nodeDevices(nodeIndices(node_cnt)).getNUIndx(nuID);
    ttiCount(node_cnt)=nodeDevices(nodeIndices(node_cnt)).ttiCountPerNU(nuIndx);
  }
  return(ttiCount);
}
ivec getLoopCount(ivec nodeDeviceIDs, Array<NodeDevice> &nodeDevices, ivec nodeIDs, int nuID)
{
  ivec loopCount(nodeIDs.length());
  ivec nodeIndices=find(nodeDeviceIDs,nodeIDs);
  for(int node_cnt=0;node_cnt<nodeIDs.length();node_cnt++)
  {
    int nuIndx=nodeDevices(nodeIndices(node_cnt)).getNUIndx(nuID);
    loopCount(node_cnt)=nodeDevices(nodeIndices(node_cnt)).actualLoopCountPerNU(nuIndx);
  }
  return(loopCount);
}

void updateSpecialSubFrameInfo_DwPts(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, int nuID, int serverNode, int actualLoopCount, int nDwpts)
{  
  int nsymbols;
  int deviceIndx=find(nodeDeviceIDs,serverNode);
  int nuIndx=nodeDevices(deviceIndx).getNUIndx(nuID);
  int frameConfig=nodeDevices(deviceIndx).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx(actualLoopCount%nodeDevices(deviceIndx).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx.length());
  if(frameConfig==21)
  {
    nsymbols=nodeDevices(deviceIndx).L1CorePerNU(nuIndx).dlFrameStructure.getFrameSize().numberOfSymbols-1;
    append(nodeDevices(deviceIndx).L1CorePerNU(nuIndx).dlFrameReservationInfo.reservedSymbols,getIntegers(nDwpts,nsymbols));
    nodeDevices(deviceIndx).L1CorePerNU(nuIndx).dlFrameReservationInfo.reservedSymbols=findUniqueNumbers(nodeDevices(deviceIndx).L1CorePerNU(nuIndx).dlFrameReservationInfo.reservedSymbols);
    
  }
}

void updateSpecialSubFrameInfo_UpPts(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, int nuID, int serverNode, int actualLoopCount, int nUpPts)
{   
  int nsymbols;
  int deviceIndx=find(nodeDeviceIDs,serverNode);
  int nuIndx=nodeDevices(deviceIndx).getNUIndx(nuID);
  int frameConfig=nodeDevices(deviceIndx).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx(actualLoopCount%nodeDevices(deviceIndx).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx.length());
  if(frameConfig==20)
  {
    nsymbols=nodeDevices(deviceIndx).L1CorePerNU(nuIndx).ulFrameStructure.getFrameSize().numberOfSymbols-nUpPts-1;
    append(nodeDevices(deviceIndx).L1CorePerNU(nuIndx).ulFrameReservationInfo.reservedSymbols,getIntegers(0,nsymbols));
    nodeDevices(deviceIndx).L1CorePerNU(nuIndx).ulFrameReservationInfo.reservedSymbols=findUniqueNumbers(nodeDevices(deviceIndx).L1CorePerNU(nuIndx).ulFrameReservationInfo.reservedSymbols);
  }
}

void updateFlexibleSubFrameInfo_DL(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, int nuID, int nodeID, int actualLoopCount, int nDwpts, ivec SFI)
{  
 
  int deviceIndices=find(nodeDeviceIDs,nodeID);
 
    int nuIndx=nodeDevices(deviceIndices).getNUIndx(nuID);
    int frameConfig=nodeDevices(deviceIndices).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx(actualLoopCount%nodeDevices(deviceIndices).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx.length());

    if(frameConfig==21)
    {
        if(nDwpts!=0)
            {
                for(int i=0;i<nDwpts;i++)
                    {
                        if(SFI(i)!=1)
                        {
                        cout<<"Mismatch in SFI and CellConfig not allowed"<<endl;
                            abort();  
                        }
                    }
            }
       
                for(int i=0;i<SFI.length();i++)
                {
                    if(SFI(i)==-2 || SFI(i)==0)
                        append(nodeDevices(deviceIndices).L1CorePerNU(nuIndx).dlFrameReservationInfo.reservedSymbols,i);
                }  
                
            nodeDevices(deviceIndices).L1CorePerNU(nuIndx).dlFrameReservationInfo.reservedSymbols=findUniqueNumbers(nodeDevices(deviceIndices).L1CorePerNU(nuIndx).dlFrameReservationInfo.reservedSymbols);
      
    }
   
  
}

void updateFlexibleSubFrameInfo_UL(Array< NodeDevice >& nodeDevices, ivec nodeDeviceIDs, int nuID, int nodeID, int actualLoopCount, int nUpPts, ivec SFI)
{   
 int deviceIndices=find(nodeDeviceIDs,nodeID);
 
    int nuIndx=nodeDevices(deviceIndices).getNUIndx(nuID);
    int frameConfig=nodeDevices(deviceIndices).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx(actualLoopCount%nodeDevices(deviceIndices).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx.length());
if(frameConfig==20)
    {
        if(nUpPts!=0)
            {
                for(int i=14-nUpPts;i<14;i++)//considering 14 symbols in a slot
                    {
                        if(SFI(i)!=0)
                        {
                        cout<<"Mismatch in SFI and CellConfig not allowed"<<endl;
                            abort();  
                        }
                    }
            }
       
                for(int i=0;i<SFI.length();i++)
                {
                    if(SFI(i)==-2 || SFI(i)==1)
                        append(nodeDevices(deviceIndices).L1CorePerNU(nuIndx).ulFrameReservationInfo.reservedSymbols,i);
                }  
            
      nodeDevices(deviceIndices).L1CorePerNU(nuIndx).ulFrameReservationInfo.reservedSymbols=findUniqueNumbers(nodeDevices(deviceIndices).L1CorePerNU(nuIndx).ulFrameReservationInfo.reservedSymbols);
    }
}

void splitBWP(NodeDevice &nodeDevice, SystemInfo_S &mySysInfo)
{
  vec tsubcarrierSpacingInKHz(0);
  ivec ueCountPerNU(0);
  ivec tNUIDS=nodeDevice.nuIDs;
  ivec carrierInMHz(0);
  vec bandwidthsPerNU(0);
  vec bwUsage(0);
  for(int nu_cnt=0;nu_cnt<tNUIDS.length();nu_cnt++)
  {
    append(tsubcarrierSpacingInKHz,nodeDevice.L1CorePerNU(nu_cnt).nuParameters.subcarrierSpacingInHz/1e3);
    append(carrierInMHz,(int)nodeDevice.L1CorePerNU(nu_cnt).nuParameters.carrierFrequencyInMHz);
    append(ueCountPerNU,nodeDevice.L2CorePerNU(nu_cnt).associatedUEs.length());
    append(bandwidthsPerNU,nodeDevice.L1CorePerNU(nu_cnt).nuParameters.bandwidthInHz);
    int nuID=getNUID(tNUIDS(nu_cnt));
    append(bwUsage,mySysInfo.nuConfig(find(mySysInfo.nuIDs,nuID)).dlUsableBW); //assuming dl and ul bw usage will be same
  }
  
  ivec uniqueCarriers=findUniqueNumbers(carrierInMHz);
  
  int maxsc=max(tsubcarrierSpacingInKHz);
  int minsc=15;
  int RBG_max = maxsc/minsc;
  
  for(int carrier_cnt=0; carrier_cnt < uniqueCarriers.size(); carrier_cnt++)
  {
    ivec nuIndices=find(carrierInMHz,uniqueCarriers(carrier_cnt),true);
    
    vec bwUsagePerCarrier=findUniqueNumbers(bwUsage(nuIndices));
    if(bwUsagePerCarrier.length()>1)
    {
        cout<<"BW usage across numerologies inside a given carrier cant be different.. "<<endl;
        abort();
    }
    
    
    vec bwpProportion=to_vec(ueCountPerNU(nuIndices))/((double)sum(ueCountPerNU(nuIndices))); // Need to compute specifically for each carrier based on supported NUs
    int tNRBIn15k=(int)((bwUsagePerCarrier(0) * bandwidthsPerNU(nuIndices(0)))/(15*1000.0));
    tNRBIn15k = tNRBIn15k/12;
    int RBGs=tNRBIn15k/RBG_max;
    ivec rbsPerNUIn15k=floor_i(bwpProportion*(double)RBGs)*RBG_max;
//     rbsPerNUIn15k(rbsPerNUIn15k.length()-1)+=(tNRBIn15k-sum(rbsPerNUIn15k));
    
   bool applyFilter;
    if(find(rbsPerNUIn15k>0).length()>1)
      applyFilter=true;
    
    int startRB=0,endRB=0;
    for(int nu_cnt=0; nu_cnt < nuIndices.length(); nu_cnt++)
    { 
      endRB=startRB+rbsPerNUIn15k(nu_cnt)-1;
      ivec bwp(2);
      bwp(0)=startRB;
      bwp(1)=endRB;
      startRB=endRB+1;
      
      int nuFactor=nodeDevice.L1CorePerNU(nuIndices(nu_cnt)).nuParameters.subcarrierSpacingInHz/15000;
      bwp=floor_i(to_vec(bwp) / (double)nuFactor);
      append(nodeDevice.L1CorePerNU(nuIndices(nu_cnt)).nuParameters.BSBWPLocations,bwp);   
      
      if(rbsPerNUIn15k(nu_cnt)>0)
	nodeDevice.L1CorePerNU(nuIndices(nu_cnt)).nuParameters.applyFilter=applyFilter;
    }
  }
}



