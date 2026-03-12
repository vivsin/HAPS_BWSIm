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

#include "../include/FadingChannel.h"

void syncServiceNodesWithAssociatedServerNodes(ChannelCloud &channelCloud,AssociationInfo_S &associationInfo){

  for(int srvc_cnt=0;srvc_cnt<associationInfo.serviceNodes.length();srvc_cnt++)
    channelCloud.syncLink(associationInfo.associatedServerNodesForEachServiceNode(srvc_cnt),associationInfo.serviceNodes(srvc_cnt));
}

void loadLinkSpecificChannelInfo(string linkTableFileName,bool isFixedVelocity,ChannelCloud &myChannel,ivec serverNodeList,ivec &serviceNodeList,SCMPropagationScenario scmScenario){

  //! FIXME CrossLink Doppler issu needs to be addressed .. 10010

  
  vec  serviceNodeVelocity(0);
  if(parse(linkTableFileName,"serviceNodeVelocity",serviceNodeVelocity)==false)
  {	
    cout<<"[both:]Error. Unable to load serviceNodeVelocity from the file in loadLinkSpecificChannelInfo()."<<endl;	
    abort();  
  }
  if(serviceNodeList.length()!=serviceNodeVelocity.length())
  {
    cout<<"[both:]Length mismatch in inputs to loadNodeVelocity()."<<endl;abort();
  }
  FadingType_E sysWideFadingType= myChannel.getSysWideFadingType();
  ChannelGenerationMethod_E  chnGenMethod=myChannel.getChannelGenerationMethod();
  
  if(chnGenMethod==_CHANNEL_GENERATION_METHOD_SCM_)
    if(scmScenario==_SCM_UNINIT_)
      scmScenario=myChannel.getSysWideSCMPropagationScenario();
  
  //ServerNodes are always immobile and with velocity 0 - 10010
    {
  if(chnGenMethod==_CHANNEL_GENERATION_METHOD_SCM_)
     myChannel.setLinkSpecificChannelInfo(serverNodeList,serverNodeList,scmScenario,0);
  else
    myChannel.setLinkSpecificChannelInfo(serverNodeList,serverNodeList,0,_FADING_TYPE_CORRELATED_);
  
  if(isFixedVelocity)
  {
    if(chnGenMethod==_CHANNEL_GENERATION_METHOD_SCM_)
    {
      myChannel.setLinkSpecificChannelInfo(serverNodeList,serviceNodeList,scmScenario,serviceNodeVelocity(0));
      myChannel.setLinkSpecificChannelInfo(serviceNodeList,serviceNodeList,scmScenario,0);
     }
    else
    {
      myChannel.setLinkSpecificChannelInfo(serverNodeList,serviceNodeList,serviceNodeVelocity(0),_FADING_TYPE_CORRELATED_);
      myChannel.setLinkSpecificChannelInfo(serviceNodeList,serviceNodeList,0,_FADING_TYPE_CORRELATED_);
    }
  }
  else
  {  
    ivec validEntries=find(serviceNodeVelocity>=0);
    if(validEntries.length())
    {
      ivec validServiceNodes=serviceNodeList(validEntries);
      vec validVelocities=serviceNodeVelocity(validEntries);
      cout<<"[detl:]Loading Random velocity into the channel."<<endl;
      
      if(sysWideFadingType==_FADING_TYPE_CORRELATED_)
	{
	  for(int cnt=0;cnt<validEntries.length();cnt++)
	  {
	    ///Setting velocity w.r.t serverNodes
	    if(chnGenMethod==_CHANNEL_GENERATION_METHOD_SCM_)
	      myChannel.setLinkSpecificChannelInfo(serverNodeList,to_ivec(validServiceNodes(cnt)),scmScenario,validVelocities(cnt));
	    else
	      myChannel.setLinkSpecificChannelInfo(serverNodeList,to_ivec(validServiceNodes(cnt)),validVelocities(cnt),_FADING_TYPE_CORRELATED_);
	    
	    ///Setting relative velocity w.r.t serviceNodes
	    ivec otherServiceNodes=validServiceNodes.right(validEntries.length()-cnt);
	    //Taking only abs to avoid negative velocities (-ve doppler not modelled) - 10010
	    vec relativeVelocity=abs(validVelocities-validVelocities(cnt)).right(validEntries.length()-cnt);
	    
	    if(chnGenMethod==_CHANNEL_GENERATION_METHOD_SCM_)
	    {
	      for(int other_cnt=0;other_cnt<otherServiceNodes.length();other_cnt++)
		myChannel.setLinkSpecificChannelInfo(to_ivec(otherServiceNodes(other_cnt)),to_ivec(serviceNodeList(validEntries(cnt))),scmScenario,relativeVelocity(other_cnt));
	    }
	    else
	    {
	      for(int other_cnt=0;other_cnt<otherServiceNodes.length();other_cnt++)
		myChannel.setLinkSpecificChannelInfo(to_ivec(otherServiceNodes(other_cnt)),to_ivec(serviceNodeList(validEntries(cnt))),relativeVelocity(other_cnt),_FADING_TYPE_CORRELATED_);
	    }
	  }
	  
	}
    }
    }
  }
}


/*void loadDoppler(ChannelCloud &myChannel,ivec &serviceNodeList,vec &serviceNodeDopplerInHz){
  
  if(serviceNodeList.length()!=serviceNodeDopplerInHz.length())
  {
    cout<<"[both:]Length mismatch in inputs to loadDoppler()."<<endl;abort();
  }
  ivec validEntries=find(serviceNodeDopplerInHz>=0);// This is not the right way of filtering .. Need to correct - 10010
  if(validEntries.length())
  {
    cout<<"[detl:]Loading Random doppler into the channel."<<endl;
    FadingType_E sysWideFadingType= myChannel.getSysWideFadingType();

    ChannelGenerationMethod_E  chnGenMethod=myChannel.getChannelGenerationMethod();
    ivec txNodeList=myChannel.getNodes();
    if(sysWideFadingType==_FADING_TYPE_CORRELATED_)
    {
      if(chnGenMethod==_CHANNEL_GENERATION_METHOD_SCM_)
      {
	SCMPropagationScenario scmScenario=myChannel.getSysWideSCMPropagationScneario();
	for(int cnt=0;cnt<validEntries.length();cnt++)
	  myChannel.setLinkSpecificChannelInfo(txNodeList,to_ivec(serviceNodeList(validEntries(cnt))),scmScenario,serviceNodeDopplerInHz(validEntries(cnt)));
      }
      else
	for(int cnt=0;cnt<validEntries.length();cnt++)
	  myChannel.setLinkSpecificChannelInfo(txNodeList,to_ivec(serviceNodeList(validEntries(cnt))),serviceNodeDopplerInHz(validEntries(cnt)),_FADING_TYPE_CORRELATED_);
    }
  }
}
*/
bool isSCMLinksToBeInitialized(ChannelCloud& channelCloud,ivec &rxNodeIDs, Array<ivec> &txNodeIDsPerRxNode)
{
  bool isToBeInitialized=false;
  for(int rx_cnt=rxNodeIDs.length()-1;rx_cnt>=0;rx_cnt--)
  {
    for(int tx_cnt=txNodeIDsPerRxNode(rx_cnt).length()-1;tx_cnt>=0;tx_cnt--)
      if(!channelCloud.isSCMLinkInitialized(txNodeIDsPerRxNode(rx_cnt)(tx_cnt),rxNodeIDs(rx_cnt)))
	 return true;
  }
  return isToBeInitialized;
}
void printBeamManagementInfo(string filename,string nodeType,Antenna* antenna,double carrierFrequencyInHz)
 {
    ifstream fin;fin.open(filename.c_str());
    ofstream fout;
    fout.open(filename.c_str(),std::ios::app);
    
    sBeamParameters beamParameter = antenna->getBeamParameters(carrierFrequencyInHz);
        
    fout<<nodeType<<" Antenna and BeamInfo: "<<endl<<endl;
    
    antenna->printAntennaInfo(fout,beamParameter.TXRUparameters.lamda);
    
    fout<<"\nAntennaStructure:\n\n";
    antenna->printAntennaStructure(fout);
    
    fout<<"\nnTXRUs: "<<beamParameter.TXRUparameters.numberOfTXRU<<"\t nElementsPerTXRU: ["<<beamParameter.TXRUparameters.numberOfRowsPerTXRU<<","<<beamParameter.TXRUparameters.numberOfColsPerTXRU<<"]\t nBeams: "<<beamParameter.nTotalBeams<<"\t nPanels: "<<beamParameter.TXRUparameters.Ng*beamParameter.TXRUparameters.Mg<<endl;
    
    fout<<"AntennaElementsPerTXRU {[txruIDs],[elements]}:\n"<<endl;
    beamParameter.TXRUparameters.printTXRUStructure(fout);
    
    beamParameter.printOrientations(fout);
    fout<<"\npanelID,\t orientation"<<endl;
    for(int i=0;i<antenna->orientationInfo.orientationPerPanel.length();i++)
    {
        fout<<i<<",\t"<<antenna->orientationInfo.orientationPerPanel(i)<<endl;
    }
    
    fout<<"\n%beamID,\t azimuthAngle,\t zenithAngle,\t Associated TXRUs"<<endl;
    for(int beam_cnt=0;beam_cnt<beamParameter.nTotalBeams;beam_cnt++)
    {
        fout<<beam_cnt<<",\t "<<beamParameter.electricalTilts(beam_cnt).horizontalTilt<<",\t "<<beamParameter.electricalTilts(beam_cnt).verticalTilt<<endl;
    }
    
    fout<<"\n\n\n";
    fout.close();
}
void printAntennaLocations(string aasFolderName,int nodeType,double carrierFrequencyInHz,Location_S loc,Antenna* antennaPtr)
{
    string filename = aasFolderName+"NODE_TYPE_"+toString(nodeType)+".txt";
    string title = "%elementCnt,\tX_coordinate,\tY_coordinate,\tZ_coordinate,\tpolarisationAngle";
    ofstream op(filename.c_str(),std::ios::out);
    Array<mat> locationVectorPerAntennaElement;vec crossPolarisationAngles(0);
    
    calculateLocationVectorsAndPolAngles(antennaPtr,carrierFrequencyInHz,locationVectorPerAntennaElement,crossPolarisationAngles);
    
    op<<"\nnodeType: "<<nodeType<<endl;
    antennaPtr->printAntennaInfo(op,lightSpeed/carrierFrequencyInHz);
    op<<"\nAntennaStructure:\n\n";antennaPtr->printAntennaStructure(op);
    op<<endl<<title<<endl;
    
    for(int antCount=0;antCount<locationVectorPerAntennaElement.length();antCount++)
    {
        op<<antCount<<",\t"<<locationVectorPerAntennaElement(antCount)(0,0)<<",\t"<<locationVectorPerAntennaElement(antCount)(1,0)<<",\t"<<locationVectorPerAntennaElement(antCount)(2,0);
        op<<",\t"<<crossPolarisationAngles(antCount)<<endl;
    }
    op.close();
}

void initSCMLink(McellSystem &mySys,ivec &serviceNodeIDs, Array<ivec> &serverNodeIDsPerServiceNode,bool initializeReverseLink , bool maintainReciprocity, bool reinitialize)
{
  
  ivec revTxNodes;
  Array<ivec> revRxNodesPerTxNode;
  if(initializeReverseLink)
  {
    revTxNodes=serviceNodeIDs;
    revRxNodesPerTxNode=serverNodeIDsPerServiceNode;
  }

  // Removing unnecessary links
  if(reinitialize==false)
  {
    for(int rx_cnt=serviceNodeIDs.length()-1;rx_cnt>=0;rx_cnt--)
    {
      for(int tx_cnt=serverNodeIDsPerServiceNode(rx_cnt).length()-1;tx_cnt>=0;tx_cnt--)
          if(mySys.channelCloud.isSCMLinkInitialized(serverNodeIDsPerServiceNode(rx_cnt)(tx_cnt),serviceNodeIDs(rx_cnt)))
	  serverNodeIDsPerServiceNode(rx_cnt).del(tx_cnt);
      
    }
    
    if(initializeReverseLink)
      for(int tx_cnt=revTxNodes.length()-1;tx_cnt>=0;tx_cnt--)
      {
	for(int rx_cnt=revRxNodesPerTxNode(tx_cnt).length()-1;rx_cnt>=0;rx_cnt--)
        if(mySys.channelCloud.isSCMLinkInitialized(revTxNodes(tx_cnt),revRxNodesPerTxNode(tx_cnt)(rx_cnt)))
	    revRxNodesPerTxNode(tx_cnt).del(rx_cnt);
	
      }
  }
  
  if(serviceNodeIDs.length()!=0 || revTxNodes.length()!=0)
  {
    reinitialize=true; // Preallocation will make SCM look initialized
    
    // If reciprocity is true, both forward and reverse scm links will be initialized here. If not only forward is initialzied
    mySys.channelCloud.preAllocateSCMLink(serviceNodeIDs,serverNodeIDsPerServiceNode,maintainReciprocity);
    for(int rx_cnt=0;rx_cnt<serviceNodeIDs.length();rx_cnt++)
    {
        mySys.initSCMLink(to_ivec(serviceNodeIDs(rx_cnt)),serverNodeIDsPerServiceNode(rx_cnt),maintainReciprocity,reinitialize);
    }

    if(initializeReverseLink && maintainReciprocity == false) // To initialize reverlink if reciprocity is false
    {
        mySys.channelCloud.preAllocateSCMLink(serverNodeIDsPerServiceNode,serviceNodeIDs,maintainReciprocity);
      for(int tx_cnt=0;tx_cnt<revTxNodes.length();tx_cnt++)
          mySys.initSCMLink(revRxNodesPerTxNode(tx_cnt),to_ivec(revTxNodes(tx_cnt)),maintainReciprocity,reinitialize);
    }
  }

}

void retainSCMLinks(ChannelCloud& channelCloud, ivec& rxNodeIDs, Array<ivec> &txNodeIDs)
{
    Array<SCMLink> desiredSCMLinks(0);
    imat scmLinkIndx=zeros_i(channelCloud.nodesInfo.nodeList.length(),channelCloud.nodesInfo.nodeList.length())-1;
    int scmNewIndx=0;
    
    ivec rxNodeIndices=find(channelCloud.nodesInfo.nodeList,rxNodeIDs);
    
    for(int rxNodeCount =0; rxNodeCount < rxNodeIDs.length();rxNodeCount++) // assuming nodes in Rx mode
    {
        ivec txNodeIndices=find(channelCloud.nodesInfo.nodeList,txNodeIDs(rxNodeCount));
        
        for(int txNodeCount =0; txNodeCount <txNodeIDs(rxNodeCount).length(); txNodeCount++)
            if(channelCloud.scmLinkIndx(rxNodeIndices(rxNodeCount),txNodeIndices(txNodeCount))!=-1)
            {
                append(desiredSCMLinks,channelCloud.scmLinks(channelCloud.scmLinkIndx(rxNodeIndices(rxNodeCount),txNodeIndices(txNodeCount))));
                scmLinkIndx(rxNodeIndices(rxNodeCount),txNodeIndices(txNodeCount))=scmNewIndx;
                
                if(channelCloud.channelReciprocity==true)
                {
                    scmLinkIndx(txNodeIndices(txNodeCount),rxNodeIndices(rxNodeCount))=scmNewIndx;
                    scmNewIndx++;
                }
                else
                {
                    scmNewIndx++;
                    append(desiredSCMLinks,channelCloud.scmLinks(channelCloud.scmLinkIndx(txNodeIndices(txNodeCount),rxNodeIndices(rxNodeCount))));
                    scmLinkIndx(txNodeIndices(txNodeCount),rxNodeIndices(rxNodeCount))=scmNewIndx;
                    scmNewIndx++;
                }
            }
    }
    channelCloud.scmLinks=desiredSCMLinks;  
    channelCloud.scmLinkIndx=scmLinkIndx;
}


vec noiseFigureToNoiseVariancePerDimensionInWatts(vec noiseFigureInDB,double frequencySpacingInHz){
    
    return(inv_dB(-203.8 + 10*log10(frequencySpacingInHz) + noiseFigureInDB)/2);
}

void createChannelCloud(McellSystem &mySys , ConfigFileNames_S myConfigFiles , ResultFileNames_S resultFiles, double samplingFreqInHz){
    
    ConfigFileNames_S &configFileNames=myConfigFiles;
    string linkTableFileName = resultFiles.linkTableFileName;
    #ifdef USING_MPI
    if(currentTaskid==0) {
        #endif
        cout<<"\n\n#################################################"<<endl;
        cout<<"[both:]Initializing Channel Cloud"<<endl;
        cout<<"#################################################\n\n"<<endl;
        #ifdef USING_MPI
    }
    #endif
    
    bool enableLinkLevelSimulation=false;
    parse(myConfigFiles.simulationConfigFileName,"enableLinkLevelSimulation",enableLinkLevelSimulation);
    
    bool enableAntCorrelation;
    AntennaCorrelationType_E antenneCorrelation;
    
    if(!parse(myConfigFiles.sysConfigFileName,"enableAntCorrelation",enableAntCorrelation))
    {
        cout<<"Disabling Antenna Correlation by default..."<<endl;
        enableAntCorrelation=false;
    }
    else if(enableAntCorrelation)
    {
        string antennaCorrelation;
        if(parse(myConfigFiles.sysConfigFileName,"antenneCorrelation",antennaCorrelation))
        {
            if(antennaCorrelation=="_ANTENNA_CORRELATION_UNCORRELATED_")
                antenneCorrelation=_ANTENNA_CORRELATION_UNCORRELATED_;
            else if(antennaCorrelation=="_ANTENNA_CORRELATION_MEDIUM_")
                antenneCorrelation=_ANTENNA_CORRELATION_MEDIUM_;
            else if(antennaCorrelation=="_ANTENNA_CORRELATION_HIGH_")
                antenneCorrelation=_ANTENNA_CORRELATION_HIGH_;
            else
            {
                cout<<"Setting Antenna Correlationas _ANTENNA_CORRELATION_UNCORRELATED_ by default..."<<antenneCorrelation<<endl;
                antenneCorrelation=_ANTENNA_CORRELATION_UNCORRELATED_;
            }
        }
        else
        {
            cout<<"Setting Antenna Correlationas _ANTENNA_CORRELATION_UNCORRELATED_ by default..."<<antenneCorrelation<<endl;
            antenneCorrelation=_ANTENNA_CORRELATION_UNCORRELATED_;
        }
    }
    
    mySys.channelCloud.loadChannel(configFileNames.sysConfigFileName);
    if(enableAntCorrelation==true)
        mySys.channelCloud.loadCorrMatrix(antenneCorrelation);
    
    if(enableLinkLevelSimulation)
    {
        int addNoise;
        parse(configFileNames.sysConfigFileName,"addNoise",addNoise);
        if(addNoise==0)
        {
            cout<<"[detl:]Warning.. Can't disable noise in link-level..Overriding the setting.."<<endl;
            mySys.channelCloud.enableNoise();
        }
        ChannelGenerationMethod_E channelGenerationMethod=parseChannelGenerationMethod(configFileNames.sysConfigFileName);
        if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
        {
            cout<<"[detl:]Error.. SCM Channel Generation not supported for link-level simulation."<<endl;
            abort();
        }
    }
    
    if(samplingFreqInHz != -1.0)
        mySys.channelCloud.setSamplingFrequencyInHz(samplingFreqInHz);
    else
    {
        // mySys.channelCloud.setSamplingFrequencyInHz(BandwidthInfo.samplingFrequencyInHz);
        cout<<"samplingFreqInHz is not set currectly.. aborting..."<<endl;abort();
    }
    
    ivec serviceNodeList; 
    if(parse(linkTableFileName,"serviceNodeList",serviceNodeList)==false)
    {
        cout<<"[both:]Error. Unable to load serviceNodeList from the file in createChannelCloud()."<<endl;
        abort();
    }
    ivec serverNodeList; 
    if(parse(linkTableFileName,"serverNodeList",serverNodeList)==false)
    {
        cout<<"[both:]Error. Unable to load serverNodeList from the file in createChannelCloud()."<<endl;
        abort();
    }
    ivec nodeList; // In the order of concat(serverNodes, serviceNodes)
    if(parse(linkTableFileName,"nodeList",nodeList)==false)
    {
        cout<<"[both:]Error. Unable to load nodeList from the file in createChannelCloud()."<<endl;
        abort();
    }
    ivec serverNodeAntennaCount,serviceNodeAntennaCount;
    if(parse(linkTableFileName,"serverNodeAntennaCount",serverNodeAntennaCount)==false)
    {	cout<<"[both:]Error. Unable to load serverNodeAntennaCount from the file in createChannelCloud()."<<endl;	abort();  }
    if(parse(linkTableFileName,"serviceNodeAntennaCount",serviceNodeAntennaCount)==false)
    {	cout<<"[both:]Error. Unable to load serviceNodeAntennaCount from the file in createChannelCloud()."<<endl;	abort();  }
    
    vec serverNodeReceiveNoiseFigureInDB,serviceNodeReceiveNoiseFigureInDB;
    if(parse(linkTableFileName,"serverNodeReceiveNoiseFigureInDB",serverNodeReceiveNoiseFigureInDB)==false)
    {	cout<<"[both:]Error. Unable to load serverNodeReceiveNoiseFigureInDB from the file in createChannelCloud()."<<endl;	abort();  }
    if(parse(linkTableFileName,"serviceNodeReceiveNoiseFigureInDB",serviceNodeReceiveNoiseFigureInDB)==false)
    {	cout<<"[both:]Error. Unable to load serviceNodeReceiveNoiseFigureInDB from the file in createChannelCloud()."<<endl;	abort();  }
    
    bool addChannelGain;
    if(parse(configFileNames.sysConfigFileName,"addChannelGain",addChannelGain)==false)
        mySys.channelCloud.setSysLinkGainInfo(linkTableFileName);
    else
    {
        //abort();
        if(addChannelGain)
            mySys.channelCloud.setSysLinkGainInfo(linkTableFileName);
        else
        {
            mat linkGainInDB;
            mat pathDelay;
            if(parse(linkTableFileName,"propagationDelayInSec",pathDelay)==false)
            {
                cout<<"[detl:]Unable to load propagationDelayInSec from the file in createChannelCloud(). Using zero delay."<<endl;
                pathDelay=zeros(nodeList.length(),nodeList.length());
            }
            linkGainInDB=zeros(nodeList.length(),nodeList.length());
            mySys.channelCloud.setSysLinkGainInfo(nodeList,pathDelay,linkGainInDB);
        }
    }
    bool fixedVelocity;
    parse(configFileNames.sysConfigFileName,"fixedUserVelocity",fixedVelocity);
    
    if(fixedVelocity==false && enableLinkLevelSimulation)
    {
        cout<<"[detl:]Warning.. Only fixed velocity supported for link-level simulation.. "<<endl;
        fixedVelocity=true;
    }
    
    mySys.channelCloud.setNodesAntennaCount(serverNodeList,serverNodeAntennaCount);
    mySys.channelCloud.setNodesAntennaCount(serviceNodeList,serviceNodeAntennaCount);
    
    vec carrierFrequencyInHz;
    if(!parse(linkTableFileName,"carrierFrequencyInHz",carrierFrequencyInHz))
    {
        cout<<"[both:]Error. Unable to parse carrierFrequencyInHz from link table."<<endl;
        abort();
    }
    mySys.channelCloud.setNodesCarrierFrequency(nodeList,carrierFrequencyInHz);
    
    vec noiseVar=concat(serverNodeReceiveNoiseFigureInDB,serviceNodeReceiveNoiseFigureInDB);
    
    ivec subcarrierSpacingInKHz;
    parse(configFileNames.nuConfigFileName,"subcarrierSpacingInKHz",subcarrierSpacingInKHz);
    double frequencySpacingInHz = subcarrierSpacingInKHz(0)*1e3;
    
    mySys.channelCloud.setNodesNoiseVariancePerDimension(nodeList,noiseFigureToNoiseVariancePerDimensionInWatts(noiseVar,frequencySpacingInHz));
    
    string ulDlMode;
    parse(myConfigFiles.simulationConfigFileName,"ulDlMode",ulDlMode);
    
    bool channelReciprocity;
    if(ulDlMode=="_ULDL_MODE_FDD_")	channelReciprocity=false;
    else	channelReciprocity=true;
    
    bool initializeReverseLink=true;
    if(ulDlMode=="_ULDL_MODE_DOWNLINK_")	initializeReverseLink=false;
    
    
    bool enableQuasistaticChannel = false;
    int nSubframesForStaticChannel = 0;
    
    if(parse(configFileNames.sysConfigFileName,"enableQuasistaticChannel",enableQuasistaticChannel)==false)
    {
        cout<<"[detl:]Unable to load enableQuasistaticChannel from the file in createChannelCloud(). Disabling it by default."<<endl;
        enableQuasistaticChannel=false;
    }
    
    if(enableQuasistaticChannel)
        if(parse(configFileNames.sysConfigFileName,"nSubframesForStaticChannel",nSubframesForStaticChannel)==false)
        {
            cout<<"[both:]Unable to load nSubframesForStaticChannel from the file in createChannelCloud(). Default value not possible..."<<endl;
            abort();
        }
        
        double quasiStaticDurationinSec=((double)nSubframesForStaticChannel) *1e-3; 
    //There is difference between scm and chanell
    mySys.channelCloud.initChannel(channelReciprocity,enableQuasistaticChannel,quasiStaticDurationinSec);
    
    FadingChannelModel_E fadingModel = mySys.channelCloud.sysWideChannelInfo.channelModel;
    if(enableLinkLevelSimulation && isCDLModel(fadingModel))
    {
        double delaySpread = mySys.channelCloud.sysWideChannelInfo.delaySpreadTDL;
        mat relVel(3,1); relVel.set_col(0,mySys.aNodes(1).getVelocityVector(0));
        mySys.channelCloud.llsChannel.initChannel(fadingModel,carrierFrequencyInHz(0),delaySpread,relVel,&mySys.aNodes(0).txAntenna,&mySys.aNodes(1).rxAntenna);
    }
    
    if(mySys.channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_)
        mySys.initBeamParameters(concat(serverNodeList,serviceNodeList));
    
    if(currentTaskid==0 && mySys.channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_)
    {
        //Printing beam Management Info for each node type.
        if(traceValuePerTraceName["EnableBeamInfo"]=="true")  
        {
            Array<string> nodeTypes = mySys.getNodeTypes();
            for(int type_cnt=0; type_cnt<nodeTypes.length();type_cnt++)
            {
                ivec nodeIndices = mySys.getNodes(nodeTypes(type_cnt));
                if(nodeIndices.length())
                {
                    int nodeIndx = nodeIndices(0);
                    printBeamManagementInfo(resultFiles.beamManagementInfoFileName,nodeTypes(type_cnt),&mySys.aNodes(nodeIndx).txAntenna,mySys.aNodes(nodeIndx).getCarrierFreqInHz());
                }
            }
        }
        //Printing antenna element Locations for each node.
        if(traceValuePerTraceName["printAntennaLocations"]=="true")
        {
            createDirectory(resultFiles.antennaInfoFileName);
            ivec nNodes = mySys.getNodeCounts();
            for(int nodeType=0;nodeType<nNodes(find(nNodes>0)).length();nodeType++)
            {
                int node_cnt = (nodeType==0) ? 0 : sum(nNodes(0,nodeType-1));
                printAntennaLocations(resultFiles.antennaInfoFileName,nodeType,mySys.aNodes(node_cnt).getCarrierFreqInHz(),mySys.aNodes(node_cnt).getLoc(),&mySys.aNodes(node_cnt).txAntenna);
            }
        }
    }
    
    //!Included to have same SCM-link profile in abstraction and normal simulations - 10010
    //! Note : Due to random seed variations between abstraction and normal simulations, instantaneous results will vary even with same simulation seed
    //Using MPI_Barrier
    
    int nTasksForAssociation = 1;
    
    ivec serverNodes = mySys.getNodes(_NODECATEGORY_SERVERNODE_);
    ivec serviceNodes = mySys.getNodes(_NODECATEGORY_SERVICENODE_);
    
    
    Array<ivec> serviceNodesPerTask = splitVectorToUniformArray(serviceNodes,nTasksForAssociation); //NOTE: this Function is different from splitVectorToArray.
    
    #ifdef USING_MPI
    MPI_Barrier(MPI_COMM_WORLD);
    #endif
    if(mySys.channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_)
    {

        if(serviceNodesPerTask.length()>currentTaskid)
        {
            if(traceValuePerTraceName["EnablePrints"]=="true")
                if(currentTaskid==0)
                    cout<<"[detl:]Initializing SCM Links."<<endl;
                
            Array<ivec> allServerNodesForEachServiceNode=repeatAsArray(serverNodes,serviceNodesPerTask(currentTaskid).length());
            initSCMLink(mySys,serviceNodesPerTask(currentTaskid),allServerNodesForEachServiceNode,initializeReverseLink,channelReciprocity); 
                
        }
    }
    
    mySys.channelCloud.disablePathDelay();
    
}


