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

#include "../include/Receiver.h"
#include "../include/NodeDevice.h"
#include <lib/L1/PDCCH/include/coreset.hpp>
#include <lib/L1/PDCCH/include/dmrs.hpp>

Receiver::Receiver(){}

EqualizerOutput_S performLayerDeMapping(EqualizerOutput_S &equalizerOutputPerLayer, DataGenInfo_S &dataGenInfo){

  EqualizerOutput_S equalizerOutputPerCodeWord;
  equalizerOutputPerCodeWord.equalizedChannel=performLayerDeMapping(equalizerOutputPerLayer.equalizedChannel,dataGenInfo.nCodeWords, dataGenInfo.nLayers);
  equalizerOutputPerCodeWord.equalizedSymbols=performLayerDeMapping(equalizerOutputPerLayer.equalizedSymbols,dataGenInfo.nCodeWords, dataGenInfo.nLayers);
  equalizerOutputPerCodeWord.equalizedNoiseVariance=performLayerDeMapping(equalizerOutputPerLayer.equalizedNoiseVariance,dataGenInfo.nCodeWords,dataGenInfo.nLayers);
  equalizerOutputPerCodeWord.postProcessingSinr=performLayerDeMapping(equalizerOutputPerLayer.postProcessingSinr,dataGenInfo.nCodeWords,dataGenInfo.nLayers);
  equalizerOutputPerCodeWord.preProcessingSinr=equalizerOutputPerLayer.preProcessingSinr;
  equalizerOutputPerCodeWord.avgIntrCov=equalizerOutputPerLayer.avgIntrCov;
  
  return(equalizerOutputPerCodeWord);
}


void Receiver::idealChannelMode(bool istrue) {

  receiverConfig.idealChannel=istrue;
}

void Receiver::enableLinkAdaptation(){

  receiverConfig.enableLinkAdaptation=true;
}

void Receiver::disableLinkAdaptation(){

  receiverConfig.enableLinkAdaptation=false;
}

// void Receiver::enableEstimateInterference(){
// 
//   receiverConfig.estimateInterference=true;
// }
// 
// void Receiver::disableEstimateInterference(){
// 
//   receiverConfig.estimateInterference=false;
// }

bool Receiver::isIdealChannel() {

  return(receiverConfig.idealChannel);
}

bool Receiver::isLinkAdaptationEnabled(){

  return(receiverConfig.enableLinkAdaptation);
}

void Receiver::setFrameHandler(Framehandler &newFrameHandler){

  frameHandler=newFrameHandler;
  frameHandler.freeFrameMemory();
}


void Receiver::initReceiver(int rxID, int antCount, L1Core_S& coreModules, bool isServerNode, UlDlMode_E ulDlMode, bool isWithAbstraction) {
  nodeID=rxID;
  frameHandler.freeFrameMemory();
  antennaCount=antCount;
  if(isServerNode)
    receiverConfig=coreModules.ulRxConfig;
  else
  {
      if(ulDlMode==_ULDL_MODE_DOWNLINK_)  
        receiverConfig=coreModules.dlRxConfig;
      else
         receiverConfig=coreModules.ulRxConfig;
  }     
  nuParams=coreModules.nuParameters;
  // Init Channel Estimator
  downlinkCQIInfo.set_length(2);
  
  cqiEstimator.channelEstimator=&channelEstimator;
  cqiEstimator.downlinkCQIInfo=&downlinkCQIInfo(0);
  cqiEstimator.uplinkCQIInfo=&uplinkCQIInfo;
  
  if(isServerNode)
  {
      //NR Codebook related
      cqiEstimator.mCodebook = coreModules.codebookPerBeam(0); // initializing with beam0 codebook.
      cqiEstimator.tbTables=coreModules.uplinkSourceAndSink.tbTables;
  }
  else if(ulDlMode==_ULDL_MODE_DOWNLINK_)
  {
    for(int cnt=0;cnt<downlinkCQIInfo.length();cnt++)
    {
      downlinkCQIInfo(cnt).cqiComputationInfo.init(coreModules.dlCQIComputationConfig);
      downlinkCQIInfo(cnt).perUserFeedback.init(coreModules.dlCQIComputationConfig,rxID);
    }
    
    cqiEstimator.estimateInterference=coreModules.dlRxConfig.estimateInterference;
    cqiEstimator.tbTables=coreModules.downlinkSourceAndSink.tbTables;
    
    //NR Codebook related
    cqiEstimator.mCodebook = coreModules.codebookPerBeam(0); // initializing with beam0 codebook.
    //cqiEstimator.isWithAbstraction=isWithAbstraction;
//     cqiEstimator.geometrySINRindB=geometrySINRindB;
  }
  else
  {
      //NR Codebook related
      cqiEstimator.mCodebook = coreModules.codebookPerBeam(0); // initializing with beam0 codebook.
      cqiEstimator.tbTables=coreModules.uplinkSourceAndSink.tbTables;
  }
  
}

int Receiver::getRxNodeID() {

  return(nodeID);
}

int Receiver::getRxAntennaCount(){

  return(antennaCount);
}

void Receiver::setRxAntennaCount(int count){
  
  antennaCount=count;
}

void Receiver:: selectDownlinkCQIInfo(int carrierInd,bool isABSInfo)
{
  if(carrierInd>=downlinkCQIInfo.length())
  {
    cout<<"carrierInd : "<<carrierInd<<" , downlinkCQIInfo.length : "<<downlinkCQIInfo.length()<<endl;
    cout<<"Invalid carrierInd"<<endl;
    abort();
  }
  if(isABSInfo)
    cqiEstimator.downlinkCQIInfo=&downlinkCQIInfo(carrierInd);
  else
    cqiEstimator.downlinkCQIInfo=&downlinkCQIInfo(carrierInd);
}

EqualizerOutput_S Receiver::processReceivedRB(RxFrameInfo_S& myRxFrameInfo,bool isDownlink, SchedulerToReceiverInfo_S &schedulerToReceiverInfo,ChannelCloud &myChannelCloud, Array< Array<cmat> > &wideBandChannel,int myChannelIndx , int blkNumber ,Size_S rbSize, double restOfInterferenceScRxPowerInWatts) {
  
  //int currentSubframe=frameHandler.getRUOffset(rbNumber).symbolOffset/rbSize.numberOfSymbols;
  ivec portIndices = schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.DeModPorts;
  ivec portReferenceNumbers = schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.getPortReferenceNumbers(portIndices);
  TransmissionScheme_E myTxScheme=schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.transmissionScheme;
  double myScTxPowerInWatts=inv_dB(schedulerToReceiverInfo.txSubcarrierPowerInDBmPerAntenna(blkNumber)-30);
  EqualizerOutput_S myRBprocessedDataPerLayer;
  FadingParameters_S fadingParameters = myChannelCloud.getLinkSpecificFadingParameters(schedulerToReceiverInfo.txNodeIDs(blkNumber), nodeID);
  
  //double rsPowerBoostInDB = schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.rsBoostInDB;
  //int rxAntenna = myRxFrameInfo.rxFrame.length();
  
  //This receiver is for each user.
  ivec tsympos= schedulerToReceiverInfo.dataBlkInfo(blkNumber).symPos; 
  int nuIndx=myNodeDevice->getNUIndx(schedulerToReceiverInfo.rxNUID);
  double nMSE=0,nMSENum = 0,nMSEDen=0;
  
  int nBundles=1;
  Array<ivec> rbsPerBundle(1);
  rbsPerBundle(0)=schedulerToReceiverInfo.dataBlkInfo(blkNumber).resourceBlocks;
  
  
  if(schedulerToReceiverInfo.dataBlkInfo(blkNumber).prbBundlingSize!=0)
  {
    nBundles=ceil_i((double)schedulerToReceiverInfo.dataBlkInfo(blkNumber).resourceBlocks.length()/(double)schedulerToReceiverInfo.dataBlkInfo(blkNumber).prbBundlingSize);
    rbsPerBundle=splitVectorToArray(schedulerToReceiverInfo.dataBlkInfo(blkNumber).resourceBlocks,schedulerToReceiverInfo.dataBlkInfo(blkNumber).prbBundlingSize);
  }
  
  double avgRSRP = 0.0; 
  cmat avgIntrCov;
  for(int bundle_cnt=0;bundle_cnt<nBundles;bundle_cnt++)
  {    
    int nRBsInBundle=rbsPerBundle(bundle_cnt).length();
    Array< Array<cmat> > receivedRB(nRBsInBundle);
    Array< Array< Array<cmat> > > myChannelPerRB(nRBsInBundle);
    Array< Array<cmat> > myPrecodedChannel(nRBsInBundle);
    cmat estimatedInterferenceCovariance;
    double RSRP = 0.0;
    for (int rb_cnt=0; rb_cnt<nRBsInBundle; rb_cnt++)
    { 
      cmat estimatedInterferenceCovariancePerRB;
      double RSRPperRB;
      
      int rbNumber=rbsPerBundle(bundle_cnt)(rb_cnt);
      cmat precoder;
      if(isDownlink)
        receivedRB(rb_cnt) = extractFromRBArea(myRxFrameInfo.rxFrame,myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,rbNumber,isDownlink);
      else
        receivedRB(rb_cnt) = extractFromRBArea(myRxFrameInfo.rxFrame,myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,rbNumber,isDownlink);
      
      if(traceValuePerTraceName["frameHandlerLevel01TraceInfo"]=="printInRx")
      {
        checkFrameHandler(frameHandler,rbNumber,receivedRB(rb_cnt));
        traceValuePerTraceName["frameHandlerLevel01TraceInfo"]="";
      }
      
      Array<cvec> txRSPerPort;
      imat rbRSPattern;
	if(isDownlink)
	  rbRSPattern= myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure.getResourceUnitMatrix(rbNumber);
	else  
	  rbRSPattern= myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure.getResourceUnitMatrix(rbNumber);
	

        
      //     rbRSPattern=rbRSPattern.get_cols(tsympos(0),tsympos(tsympos.length()-1)); // assuming contiguous time allocation
#ifdef SCHEDULER_DEBUG
    cout<<"rbRSPattern in processReceivedRB() :\n"<<rbRSPattern<<endl;
#endif
	//abort();
	std::ofstream fout1("RBStructureInRX.txt", std::ios::out);
	fout1 << rbRSPattern << "\t";
	fout1<<std::endl;
	fout1<<"\n";
	fout1.close();
      
      txRSPerPort=getTxRSPerPort(&myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,rbNumber,portIndices,rbRSPattern,isDownlink);
      for(int Cnt=0; Cnt<txRSPerPort.length(); Cnt++)
        txRSPerPort(Cnt)=sqrt(myScTxPowerInWatts)*sqrt(inv_dB(schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.rsBoostInDB))*txRSPerPort(Cnt);
      
      
      if (!receiverConfig.idealChannel)
      {
        cout<<"No ideal channel disabled."<<endl;abort();
//               double dopplerSpreadInHz = myChannelCloud.getNodeVelocity(schedulerToReceiverInfo.txNodeIDs(blkNumber),nodeID)*myChannelCloud.getNodeCarrierFrequency(nodeID)/lightSpeed;
//         
//               if(schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.DeModrsType == _RSTYPE_DMRS_)
//               {
//         	channelEstimator.estimateMyChannelInRB(myChannelPerRB(rb_cnt), receivedRB(rb_cnt), myChannelCloud.getLinkSpecificChannelGainInDB(txFrameInfo(txFrameInfoIndx).nodeID,nodeID), dB(intrScRxPowerInWatts), txRSPerPort,myChannelCloud.getNoiseVariancePerDimension(nodeID)*2 ,restOfInterferenceScRxPowerInWatts, dopplerSpreadInHz, fadingParameters, portReferenceNumbers, rbRSPattern, portIndices, rbNumber);
//         	myPrecodedChannel(rb_cnt) = getAcrossArray(myChannelPerRB(rb_cnt));
//         	//For API
//         // 	if(isAPIForDMRSBasedChannelEstimation)
//         // 	  estimateMyChannelInRBForAPI(myChannelPerRB(rb_cnt), receivedRB(rb_cnt), myChannelCloud.getLinkSpecificChannelGainInDB(txFrameInfo(txFrameInfoIndx(signalFrameInfoIndx)).nodeID,nodeID), dB(intrScRxPowerInWatts), txRSPerPort,myChannelCloud.getNoiseVariancePerDimension(nodeID)*2 ,restOfInterferenceScRxPowerInWatts, dopplerSpreadInHz, fadingParameters, portReferenceNumbers, rbRSPattern, portIndices, rbNumber);
//               }
//               else if(schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.DeModrsType == _RSTYPE_CRS_) // CRS = > using dlFrameStructure
//               {
//         	myChannelPerRB(rb_cnt)=extractFromRBArea(wideBandChannel,myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,rbNumber,isDownlink);
//               }
//               else
//               {
//         	cout<<"[both:]Error. Improper setting of DeModrsType exiting in processReceivedRB()."<<endl;
//         	abort();
//               }
      }
      else
      {
        
        Array<cmat> myIdealChannel;
        double dopplerSpreadInHz = myChannelCloud.getNodeVelocity(schedulerToReceiverInfo.txNodeIDs(blkNumber),nodeID)*myChannelCloud.getNodeCarrierFrequency(nodeID)/lightSpeed;
        
        if(isDownlink)
          myChannelPerRB(rb_cnt)=extractFromRBArea(myRxFrameInfo.channelHandler.channel(myChannelIndx),myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,rbNumber,isDownlink);
        else
          myChannelPerRB(rb_cnt)=extractFromRBArea(myRxFrameInfo.channelHandler.channel(myChannelIndx),myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,rbNumber,isDownlink);
        
        myIdealChannel = precodeMyRBChannel(myChannelPerRB(rb_cnt), &myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper, rbNumber, schedulerToReceiverInfo.dataBlkInfo(blkNumber));
        
        
        //cout<<myIdealChannel.length()<<'\t'<<myIdealChannel(0).rows()<<'\t'<<myIdealChannel(0).cols()<<'\t'<<portIndices.length()<<'\t'<<myRxFrameInfo.rxFrame.length();
        
        
        //   cout<<"\n estimated covariance:"<<estimatedInterferenceCovariancePerRB<<endl;
        //  cout<<"\n demodports"<<schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.DeModPorts<<endl;
        
        if (receiverConfig.errModelFlag)
        {    
          
          Array< Array<cmat> > myChannelPerRBOverPorts;
          if(schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.DeModrsType==_RSTYPE_DMRS_)
          {
            myChannelPerRBOverPorts.set_length(portIndices.length());
            for(int cnt1=0,tot_cnt=0;cnt1<myChannelPerRBOverPorts.length();cnt1++)
            {
              myChannelPerRBOverPorts(cnt1).set_length(myRxFrameInfo.rxFrame.length());
              for(int cnt2=0;cnt2<myChannelPerRBOverPorts(cnt1).length();cnt2++,tot_cnt++)
              {
                myChannelPerRBOverPorts(cnt1)(cnt2)=reshape(getAcrossArray(myIdealChannel,tot_cnt),rbSize.numberOfSubcarriers,rbSize.numberOfSymbols);
                //cout<<'\n'<<reshape(getAcrossArray(myPrecodedChannel(rb_cnt),tot_cnt),rbSize.numberOfSubcarriers,rbSize.numberOfSymbols)<<endl;
              }
            }
          }
          
          //cout<<myChannelPerRBOverPorts.length()<<'\t'<<myChannelPerRBOverPorts(0).length()<<'\t'<<myChannelPerRBOverPorts(0)(0).rows()<<'\t'<<myChannelPerRBOverPorts(0)(0).cols();
          cmat estIntrCovForErrorModeling;
          channelEstimator.estimateInterferenceCovarianceInRB(estIntrCovForErrorModeling, receivedRB(rb_cnt), myChannelPerRBOverPorts, txRSPerPort, myTxScheme, rbRSPattern,schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.DeModPorts, portReferenceNumbers);
          
          double interferencePwr = 0;
          
          for (int pid = 0; pid < estIntrCovForErrorModeling.rows();pid++ )
            interferencePwr += abs(estIntrCovForErrorModeling(pid,pid));
          
          interferencePwr = interferencePwr/estIntrCovForErrorModeling.rows();
          
          // 	 cout<<"\n Interference Power="<<interferencePwr;
          
          myIdealChannel = channelEstimator.errModelMyChannelInRB(myIdealChannel, receivedRB(rb_cnt), 0, dB(interferencePwr), 0.0 ,0.0 , dopplerSpreadInHz, fadingParameters.nTaps,fadingParameters.tapDel,fadingParameters.tapGain , portReferenceNumbers, rbRSPattern, portIndices);
          // cout<<"\n after Err model";
          
        }
        
        myPrecodedChannel(rb_cnt) = myIdealChannel;
        
      }
      
      if(traceValuePerTraceName["channelEstimator_01_00_traceFlag"]=="true" || traceValuePerTraceName["channelEstimator_01_01_traceFlag"]=="true")
      {
        Array<cmat>  idealChan, estChan;
        Array<Array<cmat>> myIdealChannel;
        
        if(isDownlink)
          myIdealChannel = extractFromRBArea(myRxFrameInfo.channelHandler.channel(myChannelIndx),myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,rbNumber,isDownlink);
        else
          myIdealChannel= extractFromRBArea(myRxFrameInfo.channelHandler.channel(myChannelIndx),myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,rbNumber,isDownlink);
        
        
        idealChan = precodeMyRBChannel(myIdealChannel, &myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper, rbNumber, schedulerToReceiverInfo.dataBlkInfo(blkNumber));
        
        estChan =  myPrecodedChannel(rb_cnt);
        
        nMSENum = 0;
        nMSEDen = 0;
        for(int re_cnt=0; re_cnt<estChan.length(); re_cnt++)
          for(int rx_cnt=0; rx_cnt<estChan(re_cnt).rows(); rx_cnt++)
            for(int tx_cnt=0; tx_cnt<estChan(re_cnt).cols(); tx_cnt++)
            {
              nMSENum += pow(abs(idealChan(re_cnt)(rx_cnt,tx_cnt)-estChan(re_cnt)(rx_cnt,tx_cnt)), 2);
              nMSEDen += pow(abs(idealChan(re_cnt)(rx_cnt,tx_cnt)),2);
            }
            
            
            nMSE = nMSENum /nMSEDen;
          //cout<<"\n Mean square error value="<<nMSENum<<'\t'<<nMSEDen<<'\t'<<nMSE;
          //       cout<<"[detl:]Normalized MSE in RB "<<rbNumber<<" for Level 1 Test 1 is "<<nMSE<<endl;
          if(traceValuePerTraceName["channelEstimator_01_00_traceFlag"]=="true")
          {
            cout<<"[detl:]Normalized MSE in RB "<<rbNumber<<" for Level 1 Test 1 is "<<nMSE<<endl;
          }
          if(traceValuePerTraceName["channelEstimator_01_01_traceFlag"]=="true")
          {
            if(nMSE > 1e-3)
            {
              cout<<"[both:]Normalized MSE in RB "<<rbNumber<<" for Level 1 Test 2 is "<<nMSE<<endl;
              exit(-1);
            }
            else
            {
              cout<<"[both:]Channel Estimator passed the Level 1 Test 2 for nMSE=0 successfully for RB "<<rbNumber<<" in processReceivedRB()."<<endl;
              exit(0);
            }
          }
          
      }
      
      if(receiverConfig.estimateInterference)
      {
        ivec otherDemodPorts=schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.otherDemodPorts;
        ivec otherPortRefNumbers(0);
        if(otherDemodPorts.length())
          otherPortRefNumbers=schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.getPortReferenceNumbers(otherDemodPorts);
        
        if(receiverConfig.idealChannel)
        {
          Array< Array<cmat> > myChannelPerRBOverPorts;
          if(schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.DeModrsType==_RSTYPE_DMRS_)
          {
            myChannelPerRBOverPorts.set_length(portIndices.length());
            for(int cnt1=0,tot_cnt=0;cnt1<myChannelPerRBOverPorts.length();cnt1++)
            {
              myChannelPerRBOverPorts(cnt1).set_length(myRxFrameInfo.rxFrame.length());
              for(int cnt2=0;cnt2<myChannelPerRBOverPorts(cnt1).length();cnt2++,tot_cnt++)
              {
                myChannelPerRBOverPorts(cnt1)(cnt2)=reshape(getAcrossArray(myPrecodedChannel(rb_cnt),tot_cnt),rbSize.numberOfSubcarriers,rbSize.numberOfSymbols);
              }
            }
          }
          else //CRS
          {
            //FIXME Assuming CRS always traverse in single Tx antenna (need to change for multiple TxRUs)
            if(myChannelPerRB(rb_cnt).length()==schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.DeModPorts.length())
              myChannelPerRBOverPorts=myChannelPerRB(rb_cnt);
            else
              myChannelPerRBOverPorts=myChannelPerRB(rb_cnt).left(schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.DeModPorts.length());
            //myChannelPerRBOverPorts=applyPortToAntennaMapperOnMyRBChannel(myChannelPerRBOverPorts,schedulerToReceiverInfo.portToPhysicalAntennaMapper,schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.crsPorts.length());
          }
          
          RSRPperRB = channelEstimator.estimateInterferenceCovarianceInRB(estimatedInterferenceCovariancePerRB, receivedRB(rb_cnt), myChannelPerRBOverPorts, txRSPerPort, myTxScheme, rbRSPattern,schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.DeModPorts, portReferenceNumbers,otherDemodPorts,otherPortRefNumbers);
          
        }
        else
          RSRPperRB = channelEstimator.estimateInterferenceCovarianceInRB(estimatedInterferenceCovariancePerRB, receivedRB(rb_cnt), myChannelPerRB(rb_cnt), txRSPerPort, myTxScheme, rbRSPattern,schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.DeModPorts, portReferenceNumbers,otherDemodPorts,otherPortRefNumbers);
        
        if(rb_cnt==0)
        {
          estimatedInterferenceCovariance=estimatedInterferenceCovariancePerRB;
          RSRP = RSRPperRB;
        }
        else
        {
          estimatedInterferenceCovariance+=estimatedInterferenceCovariancePerRB;
          RSRP += RSRPperRB;
        }
      }
      else
      {
        cout<<"Ideal Interference not supported anymore.."<<endl;
        abort();
      }
    } 
    
    estimatedInterferenceCovariance/=nRBsInBundle;
    RSRP /= nRBsInBundle;
    
    
    //   cout<<"estimatedInterferenceCovariance : "<<estimatedInterferenceCovariance<<endl;
    
    for (int rb_cnt=0; rb_cnt<nRBsInBundle; rb_cnt++)
    {
      Array<cmat> perToneEstimatedInterferenceCovariance(0);
      int rbNumber=rbsPerBundle(bundle_cnt)(rb_cnt);
      //geto over the required RBS.
      ivec dataTones=frameHandler.getRow1stToneIndicesInRUovereSymbols(rbNumber,tsympos,"data");
      Array< cmat > myPrecodedChannelPerRB;
      if(schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.rsInfo.DeModrsType==_RSTYPE_DMRS_)
        myPrecodedChannelPerRB=getFromArray(myPrecodedChannel(rb_cnt),dataTones);
      else
        myPrecodedChannelPerRB=precodeMyRBChannel(myChannelPerRB(rb_cnt), &myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper, rbNumber, schedulerToReceiverInfo.dataBlkInfo(blkNumber),dataTones);
      
      if(receiverConfig.estimateInterference)
        perToneEstimatedInterferenceCovariance=repeatPerToneInterferenceCovariance(estimatedInterferenceCovariance,dataTones.length());
      
      //!DL power-allocation
      if(isDownlink)
        if(schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.dlPowMask.length()!=0)
        {
          //int nLayers = schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.nLayers;
          
          cvec PowerVec; PowerVec.set_size(dataTones.length());PowerVec.zeros();
          //        if (!receiverConfig.idealChannel)
          //          for(int dataToneInd = 0 ; dataToneInd < dataTones.length();dataToneInd++)
          //            PowerVec(dataToneInd) = sqrt(schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.dlPowMask(rb_cnt)(dataTones(dataToneInd)))/sqrt(inv_dB(schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.crsPowBoostInDB));
          //        else
          for(int dataToneInd = 0 ; dataToneInd < dataTones.length();dataToneInd++)
            PowerVec(dataToneInd) = sqrt(schedulerToReceiverInfo.dataBlkInfo(blkNumber).dataGenInfo.rbInfo.dlPowMask(rb_cnt)(dataTones(dataToneInd)));
          
          for(int re_cnt=0;re_cnt<myPrecodedChannelPerRB.length();re_cnt++)
            myPrecodedChannelPerRB(re_cnt) = myPrecodedChannelPerRB(re_cnt)*PowerVec(re_cnt);
        }
        //!-----------------------------
        
        Array<cvec> receivedDataSymbols =getFromArrayOfMatrix(receivedRB(rb_cnt),dataTones);
        
        //! Below variables will be used for MMSE-IRC
        Array< Array< cmat >  > intrPrecodedChannelPerRB(0);
        ivec intrSFBCLayers(0);
        vec intrTxPowInWatts(0);
        
        EqualizerOutput_S equOutputPerRB=equalizeMyDataInRB(receivedDataSymbols, myPrecodedChannelPerRB, intrPrecodedChannelPerRB,intrSFBCLayers, myScTxPowerInWatts,intrTxPowInWatts, perToneEstimatedInterferenceCovariance, myChannelCloud.getNoiseVariancePerDimension(nodeID)*2.0+restOfInterferenceScRxPowerInWatts, receiverConfig.demodulationScheme, receiverConfig.estimateInterference,schedulerToReceiverInfo.dataBlkInfo(blkNumber).waveform);
        equOutputPerRB.nMSEPerRB=to_vec(nMSE);
        myRBprocessedDataPerLayer.concatTo(equOutputPerRB);
    }
    
    
    if(bundle_cnt==0)
    {
      avgRSRP = RSRP;
      avgIntrCov=estimatedInterferenceCovariance;
    }
    else
    {
      avgRSRP += RSRP;
      avgIntrCov+=estimatedInterferenceCovariance;
    }
  }
  avgRSRP = avgRSRP/nBundles;
  myRBprocessedDataPerLayer.avgIntrCov=avgIntrCov/nBundles;
  
  double IplusN = (1.0/(double)myRBprocessedDataPerLayer.avgIntrCov.cols())*abs(trace(myRBprocessedDataPerLayer.avgIntrCov));

  myRBprocessedDataPerLayer.preProcessingSinr = avgRSRP/IplusN;
  
  return myRBprocessedDataPerLayer;
}


Array<TransmissionStatus_S> Receiver::processReceivedSubFrame(RxFrameInfo_S& myRxFrameInfo, SystemInfo_S& mySysInfo, bool isDownlink, PerformanceMetrics_S* performanceMetrics, SchedulerToReceiverInfo_S& schedulerToReceiverInfo, ChannelCloud& myChannelCloud, string MCSlogsFileName) {
  

  Size_S rbSize;/// RB-pair: 12 X 14
  ivec txNodesCopy;
  EqualizerOutput_S myRBprocessedDataPerLayer,equalizerTemp;
  EqualizerOutput_S myRBprocessedDataPerCodeWord;
  ivec myRBindices;
  Array< Array<cvec> > txRSPerPortPerSymbol;
  Array<ivec> RSsymbolsPerPort;
  ivec portIndices;
  ivec CQIportReferenceNumbers;
  FadingParameters_S fadingParameters;
  Array< Array<cmat> > wideBandChannel;
  int guardSCs;
  int nuIndx=myNodeDevice->getNUIndx(schedulerToReceiverInfo.rxNUID);
  
  int ttiCount=schedulerToReceiverInfo.ttiCount;
  
  if(isDownlink)
    frameHandler.pointFrameStructure(&myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure);
  else
    frameHandler.pointFrameStructure(&myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure);
  
  //   frameHandler.setDataBlkInfo(schedulerToReceiverInfo.dataBlkInfo);
  
  Array<TransmissionStatus_S> txStatusPerServiceNode(schedulerToReceiverInfo.dataBlkInfo.length());
  
  if(schedulerToReceiverInfo.dataBlkInfo.length()!=0)
  {
    if(isDownlink)
      guardSCs= myNodeDevice->L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getGuardSubcarriersInOneSide();
    else
      guardSCs= myNodeDevice->L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getGuardSubcarriersInOneSide();
    
    cmat guard1(guardSCs,myRxFrameInfo.rxFrame(0).cols()), guard2(guardSCs-1,myRxFrameInfo.rxFrame(0).cols());
    guard1.clear();guard2.clear();
    double roiScRxPowInWatts=myRxFrameInfo.ROIpowerInWatts;
    
    if(isDownlink)
      rbSize=myNodeDevice->L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getRBSize();
    else
      rbSize=myNodeDevice->L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getRBSize();
    if (isDownlink)
    {
      ivec portsUsedInSubframe(0);
      for (int blk_cnt=0;blk_cnt<schedulerToReceiverInfo.dataBlkInfo.length();blk_cnt++)
        append(portsUsedInSubframe,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.rsInfo.rsPorts);
      append(portsUsedInSubframe,myNodeDevice->L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.rsInfo.rsPorts);
      
      portsUsedInSubframe=findUniqueNumbers(portsUsedInSubframe);
      generateRS(myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,frameHandler.getAllRuCount(),myNodeDevice->L1CorePerNU(nuIndx).cellID,schedulerToReceiverInfo.ttiCount%mySysInfo.subFramesPerFrame, portsUsedInSubframe,myNodeDevice->L1CorePerNU(nuIndx).dlRbInfoForUnScheduled,schedulerToReceiverInfo.dataBlkInfo, schedulerToReceiverInfo.serviceNodesForSourceAndSink,isDownlink);
    }
    else
    {
      ivec portsUsedInSubframe(0);
      for (int blk_cnt=0;blk_cnt<schedulerToReceiverInfo.dataBlkInfo.length();blk_cnt++)
        append(portsUsedInSubframe,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.rsInfo.rsPorts);
      append(portsUsedInSubframe,myNodeDevice->L1CorePerNU(nuIndx).ulRbInfoForUnScheduled.rsInfo.rsPorts);
      portsUsedInSubframe=findUniqueNumbers(portsUsedInSubframe);
      
      ivec schRBs(0);
      for (int blk_cnt=0;blk_cnt<schedulerToReceiverInfo.dataBlkInfo.length();blk_cnt++)
        append(schRBs,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks);
      
      //       generateUplinkReferenceSignal(myNodeDevice->L1CorePerNU(nuIndx).cellID,schRBs,myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure.getAllRuCount(),portsUsedInSubframe,&myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,&schedulerToReceiverInfo.dataBlkInfo(0).dataGenInfo.rbInfo);
      generateRS(myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,frameHandler.getAllRuCount(),myNodeDevice->L1CorePerNU(nuIndx).cellID,schedulerToReceiverInfo.ttiCount%mySysInfo.subFramesPerFrame, portsUsedInSubframe,myNodeDevice->L1CorePerNU(nuIndx).ulRbInfoForUnScheduled,schedulerToReceiverInfo.dataBlkInfo, schedulerToReceiverInfo.serviceNodesForSourceAndSink,isDownlink);
      
      
    }
    
    myRBindices.set_size(0);
    for(int blk_cnt=0;blk_cnt<schedulerToReceiverInfo.dataBlkInfo.length();blk_cnt++)
    {
      append(myRBindices,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks);
    }
    
    ivec myChannelIndices=find(myRxFrameInfo.channelHandler.txNodeIDs,schedulerToReceiverInfo.txNodeIDs);
    //     ivec interfererChannelIndices=findNot(myRxFrameInfo.channelHandler.txNodeIDs,schedulerToReceiverInfo.txNodeIDs); // Assuming all txNodes in Sch-To-Rx info are scheduled orthogonally .. Need to revisit for Joint-Transmission
    
    
    cmat uplinkIntrCov;
    for (int blk_cnt=0;blk_cnt<schedulerToReceiverInfo.dataBlkInfo.length();blk_cnt++)
    {
      
      if(traceValuePerTraceName["prinTxNodeIDWhileReception"]=="true")
        cout<<"\n[detl:]Receiving for txNodeID = "<<schedulerToReceiverInfo.txNodeIDs(blk_cnt)<<endl;
      if(traceValuePerTraceName["prinRxNodeIDWhileReception"]=="true")
        cout<<"\n[detl:]Receiving for rxNodeID = "<<schedulerToReceiverInfo.rxNodeID<<endl;
      
      NUParams_S nuParams=myNodeDevice->L1CorePerNU(nuIndx).nuParameters;
      channelEstimator.loadParams(1.0/nuParams.samplingFrequencyInHz,nuParams.subcarrierSpacingInHz,nuParams.fftSize);
      
      if (!receiverConfig.idealChannel & isDownlink)
      {
        cout<<"Non-ideal channel mode disabled. "<<endl;abort();
        // 	if(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.rsInfo.CQIrsType == _RSTYPE_CRS_/*||schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.rsInfo.CQIrsType == _RSTYPE_CSIRS_*/)
        // 	{
        // 	  portIndices = schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.rsInfo.CQIports;
        // 	  CQIportReferenceNumbers = schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.getPortReferenceNumbers(portIndices);
        // 
        // 	  if(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.rsInfo.CQIrsType == _RSTYPE_CRS_)
        // 	  {
        // 	    txRSPerPortPerSymbol=myNodeDevice->L1CorePerNU(nuIndx).referenceSignal.CRSsequencePerPortPerSymbol;
        // 	    RSsymbolsPerPort=myNodeDevice->L1CorePerNU(nuIndx).referenceSignal.CRSsymbolsPerPort;
        // 	  }
        // 	  else
        // 	  {
        // 	    txRSPerPortPerSymbol=myNodeDevice->L1CorePerNU(nuIndx).referenceSignal.CSIRSsequencePerPortPerSymbol;
        // 	    cout<<"txRSPerPortPerSymbol length :: "<<txRSPerPortPerSymbol.length()<<endl;
        // 	    abort();
        // 	    RSsymbolsPerPort=myNodeDevice->L1CorePerNU(nuIndx).referenceSignal.CSIRSsymbolsPerPort;
        // 	  }
        // 
        // 	  for(int temp1=0;temp1<txRSPerPortPerSymbol.length();temp1++)
        // 	    for(int temp2=0;temp2<txRSPerPortPerSymbol(temp1).length();temp2++)
        // 	      txRSPerPortPerSymbol(temp1)(temp2)=sqrt(inv_dB(schedulerToReceiverInfo.txSubcarrierPowerInDBmPerAntenna(0)+myNodeDevice->L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.crsPowBoostInDB -30))*txRSPerPortPerSymbol(temp1)(temp2);
        // 
        // 	  fadingParameters= myChannelCloud.getLinkSpecificFadingParameters(txFrameInfo(myFrameInfoIndices(blk_cnt)).nodeID, nodeID);
        // 	  double dopplerSpreadInHz = myChannelCloud.getNodeVelocity(schedulerToReceiverInfo.txNodeIDs(blk_cnt),nodeID)*myChannelCloud.getNodeCarrierFrequency(nodeID)/lightSpeed;
        // 	  wideBandChannel=channelEstimator.estimateWideBandChannel(txRSPerPortPerSymbol,guardSCs, myRxFrameInfo.rxFrame, schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure, RSsymbolsPerPort, intrScRxPowerInWatts,roiScRxPowInWatts, (myChannelCloud.getLinkSpecificChannelGainInDB(txFrameInfo(myFrameInfoIndices(blk_cnt)).nodeID,nodeID)),myChannelCloud.getNoiseVariancePerDimension(nodeID)*2, dopplerSpreadInHz, CQIportReferenceNumbers, fadingParameters, schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.rsInfo.CQIrsType, portIndices);
        // 	 
        // 	  //For API
        // // 	  if(isAPIForCRSBasedChannelEstimation)
        // // 	    wideBandChannel=estimateWideBandChannelForAPI(txRSPerPortPerSymbol,guardSCs, myRxFrameInfo.rxFrame, *schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.logicalSubFrameStructure, RSsymbolsPerPort, intrScRxPowerInWatts,roiScRxPowInWatts, (myChannelCloud.getLinkSpecificChannelGainInDB(txFrameInfo(myFrameInfoIndices(blk_cnt)).nodeID,nodeID)),myChannelCloud.getNoiseVariancePerDimension(nodeID)*2, dopplerSpreadInHz, CQIportReferenceNumbers, fadingParameters, schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.rbInfo.rsInfo.CQIrsType, portIndices);
        // 	  	  
        // 	  for(int i1=0; i1<wideBandChannel.size(); i1++)
        // 	    for(int i2=0; i2<wideBandChannel(i1).size(); i2++)
        // 	      wideBandChannel(i1)(i2) = concat_vertical(concat_vertical(guard1, wideBandChannel(i1)(i2)), guard2);
        // 	}
      }
      
      myRBprocessedDataPerLayer=processReceivedRB(myRxFrameInfo,isDownlink, schedulerToReceiverInfo, myChannelCloud,wideBandChannel,myChannelIndices(blk_cnt),  blk_cnt , rbSize, roiScRxPowInWatts);
      
      if(!isDownlink)
      {
        if(blk_cnt==0)
          uplinkIntrCov=myRBprocessedDataPerLayer.avgIntrCov;
        else
          uplinkIntrCov+=myRBprocessedDataPerLayer.avgIntrCov;
      }
      
      if(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).waveform == _Waveform_SCFDMA_)
        applyIDFT(myRBprocessedDataPerLayer, schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks.length()*rbSize.numberOfSubcarriers/schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.symbolRepetitionFactor);
      
      myRBprocessedDataPerCodeWord=performLayerDeMapping(myRBprocessedDataPerLayer,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo);
      
      bool decodeAndUpdateMetric=false;
      if(isDownlink && (mySysInfo.linkSimulationInfo.isEnabled || receiverConfig.enableLinkAdaptation==false || ttiCount>=receiverConfig.dlCQIModeInfo.CQIPMIOffset/*+receiverConfig.dlCQIModeInfo.CQIProcessingDelay*/)) // currently assuming CQIPMIOffset takes care of CQIProcessingDelay also - Dhiv 
        decodeAndUpdateMetric=true;
      else if(!isDownlink && (mySysInfo.linkSimulationInfo.isEnabled || receiverConfig.enableLinkAdaptation==false || ttiCount>=receiverConfig.ulCQIModeInfo.CQIPMIOffset/*+receiverConfig.ulCQIModeInfo.CQIProcessingDelay*/))
        decodeAndUpdateMetric=true;
      
      
      SourceAndSink *sourceAndSink;
      if(schedulerToReceiverInfo.txType==_TRANSMISSION_TYPE_DL_)
        sourceAndSink=&myNodeDevice->L1CorePerNU(nuIndx).downlinkSourceAndSink;
    else if(schedulerToReceiverInfo.txType==_TRANSMISSION_TYPE_UL_)
        sourceAndSink=&myNodeDevice->L1CorePerNU(nuIndx).uplinkSourceAndSink;
    else if(schedulerToReceiverInfo.txType==_TRANSMISSION_TYPE_SL_)
        sourceAndSink=&myNodeDevice->L1CorePerNU(nuIndx).sidelinkSourceAndSink;
      
    sourceAndSink->initializeDecoder(schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt),&schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo,ttiCount,isDownlink);
      
      
      if(decodeAndUpdateMetric)
      {
        bvec isNewTransmission=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.isNewTransmission;
        
        if(!mySysInfo.isWithAbstraction)
        {
          txStatusPerServiceNode(blk_cnt)=sourceAndSink->demodulateAndDecode(myRBprocessedDataPerCodeWord,schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt));
        }
        else
        {
          if(mySysInfo.AbstractionModel == _ABSTRACTION_DECODER_)
            txStatusPerServiceNode(blk_cnt)=sourceAndSink->getAbstractedTransmissionStatus(schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt),schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo,myRBprocessedDataPerCodeWord.postProcessingSinr,isNewTransmission,isDownlink);
          else if(mySysInfo.AbstractionModel == _ABSTRACTION_IDEAL_)
            txStatusPerServiceNode(blk_cnt)=sourceAndSink->getIdealAbstractedTransmissionStatus(schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt),schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo,myRBprocessedDataPerCodeWord.postProcessingSinr,isNewTransmission,isDownlink);
          else
          {
            cout<<"Invalid AbstractionModel... Aborting in processReceivedSubFrame() "<<endl;
            abort();
          }
        }
        // 	cout<<"myRBprocessedDataPerCodeWord.postProcessingSinr : "<<dB(myRBprocessedDataPerCodeWord.postProcessingSinr(0))<<endl;
        
        // effective SINR
        vec effSINRperCWindB=-100*ones(myRBprocessedDataPerCodeWord.postProcessingSinr.length());
        
        for(int codeWord_cnt=0;codeWord_cnt<txStatusPerServiceNode(blk_cnt).isSuccess.length();codeWord_cnt++)
        {
          if(traceValuePerTraceName["printCorrectBitCount"]=="true") {
            if(txStatusPerServiceNode(blk_cnt).isSuccess(codeWord_cnt))
              cout<<"[detl:]correctBitCount = "<<txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword(codeWord_cnt)<<endl;
            else
              cout<<"[detl:]correctBitCount = 0"<<endl;
          }
          if(traceValuePerTraceName["EnablePrints"]=="true"){
            if(!mySysInfo.isWithAbstraction)
            {
              if(!isNewTransmission(codeWord_cnt))
                cout<<"[detl:]BruteForce Decoding for "<< txStatusPerServiceNode(blk_cnt).serviceNodeID <<" in CodeWord "<<codeWord_cnt<<" and layers : "<<schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.nLayers<<", with bits in error  "<<setw(6)<<txStatusPerServiceNode(blk_cnt).nBitsInError(codeWord_cnt)<<" out of "<<setw(6)<<txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword(codeWord_cnt)<<" bits.. \tRetransmission with RV : "<<schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.rvIndicesPerCodeword(codeWord_cnt)<<endl;
              else
                cout<<"[detl:]BruteForce Decoding for "<< txStatusPerServiceNode(blk_cnt).serviceNodeID <<" in CodeWord "<<codeWord_cnt<<" and layers : "<<schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.nLayers<<", with bits in error  "<<setw(6)<<txStatusPerServiceNode(blk_cnt).nBitsInError(codeWord_cnt)<<" out of "<<setw(6)<<txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword(codeWord_cnt)<<" bits.. "<<endl;
            }
            else
            {
              if(!isNewTransmission(codeWord_cnt))
                cout<<"[detl:]Abstract Decoding for "<< txStatusPerServiceNode(blk_cnt).serviceNodeID <<" in CodeWord "<<codeWord_cnt<<" and layers : "<<schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.nLayers<<", with bits in error  "<<setw(6)<<txStatusPerServiceNode(blk_cnt).nBitsInError(codeWord_cnt)<<" out of "<<setw(6)<<txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword(codeWord_cnt)<<" bits.. \tRetransmission with RV : "<<schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.rvIndicesPerCodeword(codeWord_cnt)<<endl;
              else
                cout<<"[detl:]Abstract Decoding for "<< txStatusPerServiceNode(blk_cnt).serviceNodeID <<" in CodeWord "<<codeWord_cnt<<" and layers : "<<schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.nLayers<<", with bits in error  "<<setw(6)<<txStatusPerServiceNode(blk_cnt).nBitsInError(codeWord_cnt)<<" out of "<<setw(6)<<txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword(codeWord_cnt)<<" bits.. "<<endl;
            } 
          }
          int modOrder = sourceAndSink->tbTables->getModOrder(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.mcsIndices(codeWord_cnt),isDownlink);
          effSINRperCWindB(codeWord_cnt) = sourceAndSink->tbTables->awgnTablesConvCoder.sinrToEffSINRindB(myRBprocessedDataPerCodeWord.postProcessingSinr(codeWord_cnt),modOrder);
        }
        
	//AvgSINR for CLPC TPC
	if(myNodeDevice->L1CorePerNU(nuIndx).ulTxConfig.powerControlInfo.enableClosedLoopPowerControl)
	{
	  double recievedSINR= dB(mean(effSINRperCWindB));
	  myNodeDevice->L2CorePerNU(nuIndx)./*ulScheduler.powerControlInfo.TPCcmdField*/TPCcmdFieldPerUE(find(myNodeDevice->L2CorePerNU(nuIndx).associatedUEs,schedulerToReceiverInfo.txNodeIDs(blk_cnt))) = generateTPCcmd(myNodeDevice->L1CorePerNU(nuIndx).ulTxConfig.powerControlInfo, myNodeDevice->L1CorePerNU(nuIndx).NI, recievedSINR, myNodeDevice->L1CorePerNU(nuIndx).currentTimeForSINR, ttiCount, myNodeDevice->L1CorePerNU(nuIndx).SINRdiffForK_Slots); // code to generate TPC command in BS
	}
        //AvgSINR
        double ppSINR = inv_dB(computeAvgSINRindB(effSINRperCWindB,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.mimoRate));
        
        //  cout<<"effSINRperCWindB : "<<effSINRperCWindB<<endl;
        // 	cout<<"BruteForce ppSINR : "<<dB(ppSINR)<<endl;

        int eNBid;
        if(isDownlink)
          eNBid = schedulerToReceiverInfo.txNodeIDs(blk_cnt);
        else
          eNBid = nodeID;
        
        PrintMCSindices(schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt),eNBid,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.nLayers,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.mcsIndices,isNewTransmission,ttiCount,effSINRperCWindB,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks.length(),txStatusPerServiceNode(blk_cnt).isSuccess,txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword,MCSlogsFileName);
        
        if((schedulerToReceiverInfo.ttiCount*myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec)>=mySysInfo.settlingTimeInSec)
        {
          if(isDownlink)
          {
            performanceMetrics->update(schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt),schedulerToReceiverInfo.rxNUID,txStatusPerServiceNode(blk_cnt).isSuccess,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.bandwidthInHz/myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure.getRuCountPerLevel(),txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword,txStatusPerServiceNode(blk_cnt).nBitsInError,myRBprocessedDataPerCodeWord.preProcessingSinr,ppSINR,sum(myRBprocessedDataPerLayer.nMSEPerRB),isNewTransmission,schedulerToReceiverInfo.ttiCount*myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec,mySysInfo.resultFiles.dlWindowBLERstatsFileName);
            performanceMetrics->update(schedulerToReceiverInfo.txNodeIDs(blk_cnt),schedulerToReceiverInfo.rxNUID,txStatusPerServiceNode(blk_cnt).isSuccess,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.bandwidthInHz/myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure.getRuCountPerLevel(),txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword,txStatusPerServiceNode(blk_cnt).nBitsInError,myRBprocessedDataPerCodeWord.preProcessingSinr,ppSINR,sum(myRBprocessedDataPerLayer.nMSEPerRB),isNewTransmission,schedulerToReceiverInfo.ttiCount*myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec,mySysInfo.resultFiles.dlWindowBLERstatsFileName);
          }
          else
          {
            performanceMetrics->update(schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt),schedulerToReceiverInfo.rxNUID,txStatusPerServiceNode(blk_cnt).isSuccess,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.bandwidthInHz/myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure.getRuCountPerLevel(),txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword,txStatusPerServiceNode(blk_cnt).nBitsInError,myRBprocessedDataPerCodeWord.preProcessingSinr,ppSINR,sum(myRBprocessedDataPerLayer.nMSEPerRB),isNewTransmission,schedulerToReceiverInfo.ttiCount*myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec,mySysInfo.resultFiles.ulWindowBLERstatsFileName);
            performanceMetrics->update(nodeID,schedulerToReceiverInfo.rxNUID,txStatusPerServiceNode(blk_cnt).isSuccess,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).resourceBlocks,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.bandwidthInHz/myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure.getRuCountPerLevel(),txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword,txStatusPerServiceNode(blk_cnt).nBitsInError,myRBprocessedDataPerCodeWord.preProcessingSinr,ppSINR,sum(myRBprocessedDataPerLayer.nMSEPerRB),isNewTransmission,schedulerToReceiverInfo.ttiCount*myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec,myNodeDevice->L1CorePerNU(nuIndx).nuParameters.ttiDurationInSec,mySysInfo.resultFiles.ulWindowBLERstatsFileName);
          }
        }
      }
      else
      {
        txStatusPerServiceNode(blk_cnt).serviceNodeID=schedulerToReceiverInfo.serviceNodesForSourceAndSink(blk_cnt);
        txStatusPerServiceNode(blk_cnt).isSuccess=ones_b(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.nCodeWords);
        txStatusPerServiceNode(blk_cnt).isSuccessPerCB.set_length(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.nCodeWords);
        for(int cwd_cnt=0;cwd_cnt<txStatusPerServiceNode(blk_cnt).txInfoBitsPerCodeWord.length();cwd_cnt++)
          txStatusPerServiceNode(blk_cnt).isSuccessPerCB(cwd_cnt)=ones_b(sourceAndSink->getCodeBlocksPerCodeword(txStatusPerServiceNode(blk_cnt).serviceNodeID,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID)(cwd_cnt));
        txStatusPerServiceNode(blk_cnt).txInfoBitsPerCodeWord=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.infoBitsperCodeWord;
        txStatusPerServiceNode(blk_cnt).rxInfoBitsPerCodeWord=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.infoBitsperCodeWord;
        txStatusPerServiceNode(blk_cnt).nBitsInError=0;
        txStatusPerServiceNode(blk_cnt).harqProcessID=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.harqProcessID;
        txStatusPerServiceNode(blk_cnt).codeWordIndicesToConsider=schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.codeWordIndicesToConsider;
        txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword.set_size(txStatusPerServiceNode(blk_cnt).txInfoBitsPerCodeWord.length());
        for(int cwd_cnt=0;cwd_cnt<txStatusPerServiceNode(blk_cnt).txInfoBitsPerCodeWord.length();cwd_cnt++)
          txStatusPerServiceNode(blk_cnt).infoBitsBlockSizePerCodeword(cwd_cnt)=txStatusPerServiceNode(blk_cnt).txInfoBitsPerCodeWord(cwd_cnt).length();
      }
      
      txStatusPerServiceNode(blk_cnt).mcsRatePerCW=elem_div(to_vec(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).harqInfo.TBSizePerCodewordInBits),to_vec(sum(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.resourceElementsPerRB)*getNumLayersPerCodeWord(schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.nCodeWords,schedulerToReceiverInfo.dataBlkInfo(blk_cnt).dataGenInfo.nLayers)));
      
    }
    
    if(!isDownlink)
    {
      uplinkIntrCov/=txStatusPerServiceNode.length();
      if(cqiEstimator.uplinkAvgIntrCov.size()==0)
        cqiEstimator.uplinkAvgIntrCov=uplinkIntrCov;
      else
        cqiEstimator.uplinkAvgIntrCov = (1.0-1.0/10.0)*cqiEstimator.uplinkAvgIntrCov + (1.0/10.0)*uplinkIntrCov;
    }
    
    if(isDownlink)
      myNodeDevice->setDLHarqStatus(ttiCount,txStatusPerServiceNode(0).isSuccess);
    
  }
  

   if(isDownlink && mySysInfo.isControlChannelEnabled) {
      std::cout << "Trying to decode CORESET" << std::endl;

      for (int blk_cnt=0;blk_cnt<schedulerToReceiverInfo.dataBlkInfo.length();blk_cnt++) {
         processCoreset(myRxFrameInfo, schedulerToReceiverInfo, myChannelCloud,
                        blk_cnt, rbSize);
      }
   }

   // abort();
  return txStatusPerServiceNode;
}

bool
Receiver::processCoreset(RxFrameInfo_S& myRxFrameInfo,
               SchedulerToReceiverInfo_S& schedulerToReceiverInfo,
               ChannelCloud& myChannelCloud,
               int blkNumber, Size_S rbSize) {

  std::cout << "In processCoreset() for blkNumber = " << blkNumber << std::endl;
  int pdcchCount = 2;
  // CORESET parameters.
  // Part of ControlResourceSet in TS 38.311
  int controlResourceSetId = 0; // TODO: Always coreset 0 is sent.
  // bit map frequencyDomainResources which gives N_rb_coreset.
  int duration = pdcchCount;
  // cce-REG-MappingType which gives isInterleaved, regBundleSize,
  // interleaverSize and shiftIndex.

  // Variables got by phrasing the details of ControlResourceSet IE.
  Cce_REG_MappingType cce_REG_MappingType = nonInterleaved;

  int regBundleSize   = 6;
  int interleaverSize = 2;
  int shiftIndex      = 0;

  int aggregationLevel = 8; // TODO: change of needed
  bool rbs[45]         = {0};
  // TODO: Make sure that this size and the expected size matches.
  int N_CCE            = 5;

  for (int i = 0; i < N_CCE; ++i) {
    rbs[i] = true;
  }
  int pdcch_DMRS_ScramblingID             = -1;
  PrecoderGranularity precoderGranularity = allContiguousRBs;

  ControlResourceSetId id;
  id.ControlResourceSetId = controlResourceSetId;

  Interleaved interleaved;
  interleaved.regBundleSize   = regBundleSize;
  interleaved.interleaverSize = interleaverSize;
  interleaved.shiftIndex      = shiftIndex;

  ControlResourceSetIE IE;
  IE.controlResourceSetId = id;
  for (int i = 0; i < 45; ++i)
    IE.frequencyDomainResources[i] = rbs[i];
  IE.duration                = duration;
  IE.cce_REG_MappingType     = cce_REG_MappingType;
  IE.interleaved             = interleaved;
  IE.precoderGranularity     = precoderGranularity;
  IE.pdcch_DMRS_ScramblingID = pdcch_DMRS_ScramblingID;

  // TODO: Only transmitting CORESET 0 no
  //       If needed, make changes for UE CORESET.

  // Create the coreset object
  int subframeNumber = schedulerToReceiverInfo.ttiCount;
  int nuIndx = this->myNodeDevice->getNUIndx(schedulerToReceiverInfo.rxNUID);
  int cellID = this->myNodeDevice->L1CorePerNU(nuIndx).cellID;
  int nodeID = schedulerToReceiverInfo.serviceNodesForSourceAndSink(blkNumber);
  Coreset<bool> coreset0(IE);
  coreset0.setSlotNumber(subframeNumber);
  coreset0.setCellID(cellID);
  coreset0.setC_RNTI(0);


  double myScTxPowerInWatts = inv_dB(schedulerToReceiverInfo
                              .txSubcarrierPowerInDBmPerAntenna(blkNumber)
                              - 30);

  // TODO: Use the frame filling method used in processReceivedRB().
  // TODO: We use  N_CCE * 6 * 12 * pdcchCount RE's
  //       Since we know the size of Coreset being transmitted use that.
  // TODO: Figure out how to get the number of unused RE's
  // int nUnusedREs  = 0;
  int ReceivedpdcchCount = pdcchCount;
  int nREs               = N_CCE * 6 * 12;
  int usedSubcarriers    = nREs * ReceivedpdcchCount;
  imat& logicalSubFrameStructure = this->myNodeDevice->L1CorePerNU(nuIndx)
                                  .dlRbInfoForUnScheduled
                                  .logicalSubFrameStructure;
  ivec pdcchTones = find(logicalSubFrameStructure,
                         50 + ReceivedpdcchCount - 1, true);

  int fftSize = this->myNodeDevice->L1CorePerNU(nuIndx).nuParameters.fftSize;

  ivec pdcchFFTTones(pdcchTones.length());
  int guardSubcarriersInOneSide = (fftSize - usedSubcarriers) / 2;
  for (int i = 0; i < pdcchTones.length(); i++) {
    pdcchFFTTones(i) = pdcchTones(i) + guardSubcarriersInOneSide;
    if (pdcchFFTTones(i) >=  fftSize / 2)
      pdcchFFTTones(i) += 1;
  }

  // TODO: Replace usedSubcarriers UsedREs by nUnusedREs
  // TODO: The tones do not correspond to multiple symbols
  pdcchFFTTones = pdcchFFTTones(0, usedSubcarriers - 1);

  Array<cvec> receivedPDCCHsymbols =
      getFromArrayOfMatrix(myRxFrameInfo.rxFrame, pdcchFFTTones);

  // Get the channel
  ivec myChannelIndices = find(myRxFrameInfo.channelHandler.txNodeIDs,
                               schedulerToReceiverInfo.txNodeIDs);
  Array<cmat> mychannel(pdcchFFTTones.length());
  for (int tone_cnt = 0; tone_cnt < pdcchFFTTones.length(); tone_cnt++)
    mychannel(tone_cnt) = getAcrossArray(
        myRxFrameInfo.channelHandler.channel(myChannelIndices(0)),
        pdcchFFTTones(tone_cnt));


  // Precode the channel since we used a all one precoder while tranmitting
  int nTxAntenna = mychannel(0).cols();
  int nRxAntenna = mychannel(0).rows();
  Array<cmat> myChannelPrecoded(mychannel.length());
  cmat precoder = ones_c(mychannel(0).cols(), 1) / sqrt(nTxAntenna);
  for (int i = 0; i < mychannel.length(); ++i) {
    myChannelPrecoded(i) = mychannel(i) * precoder;
  }

  // Estimate interference covariance
  // First get the dmrs
  // TODO: the below code is assumes no interleaving and same precoder
  // is applied to all the CCE.
  std::complex<double> txPowerScaleVal(sqrt(myScTxPowerInWatts), 0);
  int rxAntenna = receivedPDCCHsymbols.length();
  cmat estimatedInterferenceCovariance(rxAntenna, rxAntenna);
  double rsrp = 0;
  int N_rb = N_CCE * 6;
  int  dmrs_cnt = 0;
  for (int n_symb = 0; n_symb < ReceivedpdcchCount; n_symb++) {
    // Generate the dmrs sequence that was transmitted
    auto _dmrsSeq = generateDMRS(n_symb, cellID, subframeNumber, 500);
    for (int n_rb = 0; n_rb < N_rb; ++n_rb) {
      // DMRS is always at pos 0, 4 and 8
      for (int n_re : {0, 4, 8} ) {
        int tone_cnt = (n_symb * nREs) + (n_rb * 12) + n_re;

        // The transitted symbol
        // TODO: What about the power scaling factor.
        cmat X = ones_c(8, 1) * _dmrsSeq[dmrs_cnt++] * txPowerScaleVal;
        cmat H = mychannel(tone_cnt);
        cmat Y(receivedPDCCHsymbols.length(), 1);
        Y.set_col(0, getAcrossArray(receivedPDCCHsymbols, tone_cnt));

        // calculate the rsrp
        rsrp += mean(sqr(abs((H * X).get_col(0))));

        // Interfernce plus noise
        auto interferncePlusNoise = Y - H*X;
        estimatedInterferenceCovariance +=
            interferncePlusNoise * interferncePlusNoise.hermitian_transpose();
      }
    }
  }
  rsrp /= (double)(dmrs_cnt);
  estimatedInterferenceCovariance /= (double)(dmrs_cnt);

  double IplusN = (1.0/(double)estimatedInterferenceCovariance.cols()) *
                  abs(trace(estimatedInterferenceCovariance));
  double prePSINR = rsrp / IplusN;

  //! Below variables will be used for MMSE-IRC
  Array< Array< cmat >  > intrPrecodedChannelPerRB(0);
  ivec intrSFBCLayers(0);
  vec intrTxPowInWatts(0);
  double restOfInterferenceWithNoiseVariance = 0.0;
  Array<cmat> perToneEstimatedInterferenceCovariance(0);
  perToneEstimatedInterferenceCovariance = repeatPerToneInterferenceCovariance(
      estimatedInterferenceCovariance, pdcchFFTTones.length());

  // Equalize the data and channel
  EqualizerOutput_S pdcchEquOut = equalizeMyDataInRB(
      receivedPDCCHsymbols, myChannelPrecoded,
      intrPrecodedChannelPerRB,intrSFBCLayers,
      myScTxPowerInWatts,intrTxPowInWatts,
      perToneEstimatedInterferenceCovariance,
      restOfInterferenceWithNoiseVariance,
      receiverConfig.demodulationScheme,
      receiverConfig.estimateInterference,
      schedulerToReceiverInfo.dataBlkInfo(blkNumber).waveform);

  // Get the equalized symbols, channel, noise variance
  cvec coresetSymbols = pdcchEquOut.equalizedSymbols(0);
  cvec coresetChannel = pdcchEquOut.equalizedChannel(0);
  // TODO: Get the N0 value for equalizer output.
  double N0 = IplusN;

  // TODO: Make provisions for abstract decoder.
  // Decode the symbols to get the dci bits
  int dciSize = 40;
  bvec decodedDciBits;
  bool crcPass = coreset0.decodeCoreset(coresetSymbols, coresetChannel,
                                        dciSize, aggregationLevel, N0, decodedDciBits);

  if (crcPass) {
    std::cout << "\t\t CORESET decode : SUCCESS" << std::endl;
  } else {
    std::cout << "\t\t CORESET decode : FAIL" << std::endl;
  }

  // Dump stats to a file
  // TODO: Make some aggregated logs
  std::ofstream coresetLogs;
  string filename = "./Results/Coreset/serverNode_ID_" + 
                    toString(cellID) + ".txt";
  system(" mkdir ./Results/Coreset");
  if (!IsFileExists(filename)) {
    coresetLogs.open(filename.c_str(), ios::out);
    coresetLogs << "ServerNodeId, \tServiceNodeId, \t SubframeNumber, \t";
    coresetLogs << "coresetId, \t aggregationLevel, \t prePSINR, \t";
    coresetLogs << "isSuccess" << endl;
    coresetLogs.close();
  }

  // Write the values
  coresetLogs.open(filename.c_str(), ios::app);
  coresetLogs << cellID << ", \t " << nodeID << ", \t ";
  coresetLogs << subframeNumber << ", \t " << controlResourceSetId << ", \t ";
  coresetLogs << aggregationLevel << ", \t " << prePSINR << ", \t ";
  coresetLogs << crcPass << endl;
  coresetLogs.close();

  // TODO: check if the decoded bits are in error.
  return crcPass;
}

void Receiver::generateFeedback(int nuID, int txBeamID, int subframeNumber,RxFrameInfo_S &myRxFrameInfo,ChannelCloud &myChannelCloud,bool isDownlink, PerformanceMetrics_S &performanceMetrics, SystemInfo_S mySysInfo,string WideBandCQIlogsFileName,string RIlogsFileName,string SubbandCQIlogsFileName,string AperiodicCQILogsFileName,double NIValueForTheServerNode) {
  
  int nuIndx=myNodeDevice->getNUIndx(nuID);
  if(isDownlink)
  {
    if(cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig.length()==0)
    {
      cout<<"Error. CQI Measurement Configuration not available at the receiver.."<<endl;
      abort();
    }
    Array< Array<cmat> > wideBandChannel;
    cqiEstimator.mCodebook=myNodeDevice->L1CorePerNU(nuIndx).codebookPerBeam(txBeamID);
    cqiEstimator.crsPowBoostInDB=myNodeDevice->L1CorePerNU(nuIndx).dlRbInfoForUnScheduled.crsPowBoostInDB;
    frameHandler.pointFrameStructure(&myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure);
   
//     ivec txFrameInfoNodes(0);
//     for(int cnt=0;cnt<txFrameInfo.length();cnt++)
//       append(txFrameInfoNodes,txFrameInfo(cnt).nodeID);
    FadingParameters_S fadingParameters;
//     int desiredFrameInfoIndex=find(txFrameInfoNodes,rxFrameInfo.signalNodeIDs(0));
    
//     ivec interfererFrameInfoIndices;
//     interfererFrameInfoIndices=findNot(txFrameInfoNodes,rxFrameInfo.signalNodeIDs(0));
//     vec intrScRxPowerInWatts;
//     intrScRxPowerInWatts.set_length(interfererFrameInfoIndices.length());
//     for(int intr_cnt=0;intr_cnt<interfererFrameInfoIndices.length();intr_cnt++)
//       intrScRxPowerInWatts(intr_cnt)=inv_dB(txFrameInfo(interfererFrameInfoIndices(intr_cnt)).txSubcarrierPowerInDBmPerAntenna+myChannelCloud.getLinkSpecificChannelGainInDB(txFrameInfo(interfererFrameInfoIndices(intr_cnt)).nodeID,nodeID)-30);
//     
    for(int cqi_cnt=0;cqi_cnt<cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig.length();cqi_cnt++)
    {
      if(cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(cqi_cnt).nuID==nuID)  
      {
	Array<DataBlkInfo_S> dummyDataBlkInfo(0);
	if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered||cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger||cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger ||cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger ||cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isrankTrigger)
	{ 
	  generateRS(myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,frameHandler.getAllRuCount(),myNodeDevice->L1CorePerNU(nuIndx).cellID,myNodeDevice->ttiCountPerNU(nuIndx)%mySysInfo.subFramesPerFrame, cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(cqi_cnt).cqiPorts,myNodeDevice->L1CorePerNU(nuIndx).dlRbInfoForUnScheduled,dummyDataBlkInfo, "",true);
	  fadingParameters= myChannelCloud.getLinkSpecificFadingParameters(rxFrameInfo.signalNodeIDs(0), nodeID);
	  double dopplerSpreadInHz = myChannelCloud.getNodeVelocity(rxFrameInfo.signalNodeIDs(0),nodeID)*myChannelCloud.getNodeCarrierFrequency(nodeID)/lightSpeed;
	  double roiScRxPowInWatts=myRxFrameInfo.ROIpowerInWatts;
	  NUParams_S nuParams=myNodeDevice->L1CorePerNU(nuIndx).nuParameters;
	  channelEstimator.loadParams(1.0/nuParams.samplingFrequencyInHz,nuParams.subcarrierSpacingInHz,nuParams.fftSize);
	  if(!receiverConfig.idealChannel)
	  {
// 	    double txPowPerSC=sqrt(inv_dB(txFrameInfo(desiredFrameInfoIndex).txSubcarrierPowerInDBmPerAntenna));
// 	     Array< Array<cvec> > csiRs=myNodeDevice->L1CorePerNU(nuIndx).referenceSignal.CSIRSsequencePerPortPerSymbol;
// 	     for(int i=0;i<csiRs.length();i++)
// 	       for(int j=0;j<csiRs(i).length();j++)
// 		 for(int k=0;k<csiRs(i)(j).length();k++)
// 		   csiRs(i)(j)(k)=txPowPerSC*csiRs(i)(j)(k);
	    
	    cout<<"Code commented as TxFrameInfo was removed from input."<<endl;abort();
	  double nMSE=0, nMSENum =0, nMSEDen =0;
	/*
	  for(int tx_cnt=0; tx_cnt<wideBandChannel.length(); tx_cnt++)
	    for(int rx_cnt=0; rx_cnt<wideBandChannel(0).length(); rx_cnt++)
	    {
	      idealChanmodified(tx_cnt)(rx_cnt).del_rows(0,211);
	      idealChanmodified(tx_cnt)(rx_cnt).del_row(299);
	            
	    }
		  
	  for(int tx_cnt=0; tx_cnt<estChan.length(); tx_cnt++)
	    for(int sc_cnt=0; sc_cnt<estChan(0)(0).rows(); sc_cnt++)
	      for(int sym_cnt=0; sym_cnt<estChan(0)(0).cols(); sym_cnt++)
		for(int rx_cnt=0; rx_cnt<estChan(0).length(); rx_cnt++)
		{
		//  if(sym_cnt==8 && (sc_cnt%12)==0)
		   //cout<<'\n'<<sc_cnt<<'\t'<<(idealChanmodified(tx_cnt)(rx_cnt)(sc_cnt, sym_cnt)/estChan(tx_cnt)(rx_cnt)(sc_cnt, sym_cnt))<<endl;
		     nMSENum += pow(abs(idealChanmodified(tx_cnt)(rx_cnt)(sc_cnt, sym_cnt)-estChan(tx_cnt)(rx_cnt)(sc_cnt, sym_cnt)), 2);
		     nMSEDen += pow(abs(idealChanmodified(tx_cnt)(rx_cnt)(sc_cnt, sym_cnt)),2);
	    */
// 	     wideBandChannel=channelEstimator.estimateWideBandChannel(myNodeDevice->L1CorePerNU(nuIndx).referenceSignal.CSIRSsequencePerPortPerSymbol/**txPowPerSC*/,myNodeDevice->L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getGuardSubcarriersInOneSide(), myRxFrameInfo.rxFrame, *myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer(), myNodeDevice->L1CorePerNU(nuIndx).referenceSignal.CSIRSsymbolsPerPort, intrScRxPowerInWatts,roiScRxPowInWatts, (myChannelCloud.getLinkSpecificChannelGainInDB(rxFrameInfo.signalNodeIDs(0),nodeID)),myChannelCloud.getNoiseVariancePerDimension(nodeID)*2, dopplerSpreadInHz, getPortReferenceNumbers(myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(cqi_cnt).cqiPorts), fadingParameters, _RSTYPE_CSIRS_, cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(cqi_cnt).cqiPorts);

// 	     Array< Array<cmat> > idealChan, estChan;
// 	  
// 	      idealChan=myRxFrameInfo.channelHandler.channel(0);
// 	      cout<<"txFrameInfo.power : "<<txFrameInfo(0).txSubcarrierPowerInDBmPerAntenna<<endl;
// 	      double txSubcarrierPowerInWattsPerAntenna=inv_dB(txFrameInfo(0).txSubcarrierPowerInDBmPerAntenna-30);
// 		
// 	      estChan=wideBandChannel;
// 	      
// 	      double nMSE = 0, nMSENum = 0, nMSEDen = 0;
// 	      
// 	      Array< Array<cmat> > idealChanmodified;
// 	      idealChanmodified=idealChan;
// 	      for(int tx_cnt=0; tx_cnt<wideBandChannel.length(); tx_cnt++)
// 		for(int rx_cnt=0; rx_cnt<wideBandChannel(0).length(); rx_cnt++)
// 		{
// 		  idealChanmodified(tx_cnt)(rx_cnt).del_rows(0,211);
// 		  idealChanmodified(tx_cnt)(rx_cnt).del_row(299);
// 			
// 		}
// 	       
// 	      for(int tx_cnt=0; tx_cnt<wideBandChannel.length(); tx_cnt++)
// 		for(int rx_cnt=0; rx_cnt<wideBandChannel(0).length(); rx_cnt++)
// 		  for(int sc_cnt=0; sc_cnt<wideBandChannel(0)(0).rows(); sc_cnt++)
// 		    for(int sym_cnt=0; sym_cnt<wideBandChannel(0)(0).cols(); sym_cnt++)
// 		      {
// 			idealChanmodified(tx_cnt)(rx_cnt)(sc_cnt, sym_cnt)=idealChanmodified(tx_cnt)(rx_cnt)(sc_cnt, sym_cnt)*sqrt(txSubcarrierPowerInWattsPerAntenna);
// 		   //  if(sym_cnt==8 && (sc_cnt%12)==0)
// 		     {
// 		      // cout<<'\n'<<idealChanmodified(tx_cnt)(rx_cnt)(sc_cnt, sym_cnt)<<'\t'<<estChan(tx_cnt)(rx_cnt)(sc_cnt, sym_cnt);
// 			nMSENum += pow(abs(idealChanmodified(tx_cnt)(rx_cnt)(sc_cnt, sym_cnt)-estChan(tx_cnt)(rx_cnt)(sc_cnt, sym_cnt)), 2);
// 			nMSEDen += pow(abs(idealChanmodified(tx_cnt)(rx_cnt)(sc_cnt, sym_cnt)),2);
// 		 
// 		     }
// 		    }
// 		
// 	nMSE = nMSENum/nMSEDen;
// 		  
// 	cout<<"[detl:]Normalized MSE in wideband for Level 1 Test 1 is "<<nMSE<<endl;
	 
	}
	  else
	  {
// 	    Array< Array<cmat> > idealChan;
// 	    idealChan=myRxFrameInfo.channelHandler.channel(0);
// 	    for(int tx_cnt=0; tx_cnt<idealChan.length(); tx_cnt++)
// 	      for(int rx_cnt=0; rx_cnt<idealChan(0).length(); rx_cnt++)
// 	      {
// 		int nGuardSubcarriers=myNodeDevice->L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getGuardSubcarriersInOneSide();
// 		int fftSize=idealChan(tx_cnt)(rx_cnt).rows();
// 		idealChan(tx_cnt)(rx_cnt).del_rows(fftSize-nGuardSubcarriers,fftSize-1);
// 		idealChan(tx_cnt)(rx_cnt).del_row(fftSize/2);
// 		idealChan(tx_cnt)(rx_cnt).del_rows(0,nGuardSubcarriers-1);
// 	      }
// 	  
// // 	    if (receiverConfig.errModelFlag)
// // 	      wideBandChannel=channelEstimator.errModelWideBandChannel(idealChan, *myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure.getCurrentFrameStructurePointer(), intrScRxPowerInWatts,roiScRxPowInWatts, (myChannelCloud.getLinkSpecificChannelGainInDB(rxFrameInfo.signalNodeIDs(0),nodeID)),myChannelCloud.getNoiseVariancePerDimension(nodeID)*2, dopplerSpreadInHz, getPortReferenceNumbers(myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(cqi_cnt).cqiPorts), fadingParameters,cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(cqi_cnt).cqiPorts);
// // 	    else
// 	      wideBandChannel = idealChan;
	      
		 
	


// double mse=0.0;int row_cnt;
 // double mse1=0.0; double mse2=0.0;
  /*for(int txAnt=0;txAnt<idealChan.length();txAnt++)
      for (int rxAnt=0;rxAnt<idealChan(0).length();rxAnt++)
      {*/   /*       
              for (int col=0;col<idealChan(0)(0).cols();col++)
              {
                  row_cnt=0;*/
              //    for (int row=0;row<estChan(0)(0).rows();row++)
               //         {
                   /*
                            if(abs(estiChannel(txAnt)(rxAnt)(row,col))!=0.0 && row_cnt<48)
                            {
                                //mse+=pow(abs(idealChannel(txAnt)(rxAnt)(row,col)-estiChannel(txAnt)(rxAnt)(row,col)),2)/pow(abs(idealChannel(txAnt)(rxAnt)(row,col)),2);
                                mse1+=pow(abs(idealChannel(txAnt)(rxAnt)(row,col)-estiChannel(txAnt)(rxAnt)(row,col)),2);
                                mse2+=pow(abs(idealChannel(txAnt)(rxAnt)(row,col)),2);
                                row_cnt++; */ 
                            // print channels
                            //     std::ofstream channel;
                        //        static bool isNewPrintChn=1;
                          //      if(isNewPrintChn)
                      //          {
                                 //   channel.open("channel",ios::out);

                               //     isNewPrintChn=0;
                             //   }
                           //     else
                         //       channel.open("channel",ios::app);
                            
                       //         channel<<estChan(0)(0)(row,4)<<endl;
                                
                            //}
                            
                     //   }
     /*             
            }

      }*/
              //mse=mse/(double)(estiChannel.length()*estiChannel(0).length()*row_cnt*estiChannel(0)(0).cols());
//                 mse=mse1/mse2;
//   std::ofstream channelMSE;
//   static bool isNewPrintMse=1;
//   if(isNewPrintMse)
//   {
//     channelMSE.open("mse",ios::out);
// 
//     isNewPrintMse=0;
//   }
//   else
//     channelMSE.open("mse",ios::app);
//  
//   channelMSE<<mse<<endl;
	 
	 
	 
	 
	
	generateDLFeedback(cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(cqi_cnt),subframeNumber,myRxFrameInfo,myChannelCloud, performanceMetrics,mySysInfo,WideBandCQIlogsFileName,RIlogsFileName,SubbandCQIlogsFileName,AperiodicCQILogsFileName);
    
    //Storing DL interference for reciprocity
    
   if(mySysInfo.ulDlMode == _ULDL_MODE_TDD_)
   {
   
   myRxFrameInfo.weakIntr=roiScRxPowInWatts;
  
   if(cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(cqi_cnt).intrCov.length()!=0)
   {
   myRxFrameInfo.intrCov= cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(cqi_cnt).intrCov;
   myRxFrameInfo.noiseVar=cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(cqi_cnt).noiseVar;
   //cout<<"successfully stored intr values in rxFrame info"<<endl;
   }
    else
   {
       cout<<"Error in storing of interference values for DL reciprocity"<<endl;
       abort();
   }
   }
  
	  // Reciprocity Precoder calculation is applicable only for TDD mode
// 	  if(mySysInfo.ulDlMode == _ULDL_MODE_TDD_)
// 	  {
// 	    // write function for reciprocity based precoder
// 	    // Reciprocity precoder based on UplInk channel For subsequent downlink 
// 	    //FIXME: shift this variable assignmnt at appropriate place
// 	    cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(0).isAssociatedWithUL = true;
// 	    if(cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(cqi_cnt).isAssociatedWithUL)
// 	    {
// 	      ReciprocityPrecodersInfo_S reciprocityPrecoders = generateReciprocityPrecodersForUL(cqiEstimator.downlinkCQIInfo->perUserFeedback.cqiMeasurementConfig(cqi_cnt),subframeNumber,myRxFrameInfo,myChannelCloud, performanceMetrics,mySysInfo,WideBandCQIlogsFileName,RIlogsFileName,SubbandCQIlogsFileName,AperiodicCQILogsFileName);
// 	      cqiEstimator.downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(cqi_cnt).reciproPrecoder = reciprocityPrecoders.precoderPerRB;
// 	    } 
// 	  }
	}
      }
    }    
 }
  }
  else
  {
    frameHandler.pointFrameStructure(&myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure);
    int nRBs = myNodeDevice->L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe();
    int nSb=nRBs/myNodeDevice->L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getResourceBlockGroupingSize();
//     myNodeDevice->L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.perform_UL_Freq_hopping(subframeNumber,0,myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure.getRuCountPerLevel(),nSb,myNodeDevice->L1CorePerNU(nuIndx).cellID,false);
        
    ivec serviceNodes=myRxFrameInfo.signalNodeIDs;
    ivec srvcNodeIndices=find(performanceMetrics.performanceLogPerNU(nuIndx).txNodes,serviceNodes);
    bvec feedbackPeriodicity=zeros_b(serviceNodes.length())+(bin)(myNodeDevice->L1CorePerNU(nuIndx).ulRxConfig.ulCQIModeInfo.isToTransmitCQIPorts(subframeNumber));

    ivec srvcNodeIndxInRx=find(uplinkCQIInfo.serviceNodeIDs,serviceNodes);

    for(int srvcNode_cnt=0;srvcNode_cnt<serviceNodes.length();srvcNode_cnt++)
    {
      #ifdef USING_SRS
      if(txFrameInfo(myTxFrameInfoIndx(srvcNode_cnt)).isSRSEnabled)
      {
	feedbackPeriodicity=txFrameInfo(myTxFrameInfoIndx(srvcNode_cnt)).srsSequence.sequencePerPort.length();
	cqiEstimator.isSRSEnabled=true;
	cqiEstimator.srsRBs=txFrameInfo(myTxFrameInfoIndx(srvcNode_cnt)).srsSequence.rbIndices;
      }
      #endif
      if(feedbackPeriodicity(srvcNode_cnt) && uplinkCQIInfo.isToReportCQI(srvcNodeIndxInRx(srvcNode_cnt))==0)
      {
	
	int srvcNodeIndx=srvcNodeIndices(srvcNode_cnt);
	int nuIndx=find(performanceMetrics.nuIDs,nuID);
	//OOLA
	if(receiverConfig.isOuterLoopLinkAdaptationEnabled)
	{
	  double sinrDown=0.5;
	  double sinrUp=sinrDown/((1.0/receiverConfig.targetBLER)-1.0);
	  if(performanceMetrics.performanceLogPerNU(nuIndx).packetsWithinCQIPeriodicityPerServiceNode(srvcNodeIndx))
	  {
	    performanceMetrics.performanceLogPerNU(nuIndx).sinroffsetPerServiceNode(srvcNodeIndx)+=(performanceMetrics.performanceLogPerNU(nuIndx).acksWithinCQIPeriodicityPerServiceNode(srvcNodeIndx)/performanceMetrics.performanceLogPerNU(nuIndx).packetsWithinCQIPeriodicityPerServiceNode(srvcNodeIndx))*sinrUp - ((performanceMetrics.performanceLogPerNU(nuIndx).packetsWithinCQIPeriodicityPerServiceNode(srvcNodeIndx) - performanceMetrics.performanceLogPerNU(nuIndx).acksWithinCQIPeriodicityPerServiceNode(srvcNodeIndx))/performanceMetrics.performanceLogPerNU(nuIndx).packetsWithinCQIPeriodicityPerServiceNode(srvcNodeIndx))*sinrDown;
	  }
	  cqiEstimator.sinrOffsetForOLLA=performanceMetrics.performanceLogPerNU(nuIndx).sinroffsetPerServiceNode(srvcNodeIndx);
	  performanceMetrics.performanceLogPerNU(nuIndx).acksWithinCQIPeriodicityPerServiceNode(srvcNodeIndx)=0.0;
	  performanceMetrics.performanceLogPerNU(nuIndx).packetsWithinCQIPeriodicityPerServiceNode(srvcNodeIndx)=0.0;
	}

	SubbandULCQIInfo_S subbandULCQIInfo= generateULFeedback(serviceNodes(srvcNode_cnt),nuID,subframeNumber,myRxFrameInfo,myChannelCloud,NIValueForTheServerNode);
	PerUserULCQIFeedback_S perUserULCqiInfo;
	perUserULCqiInfo.serviceNodeID=serviceNodes(srvcNode_cnt);
	perUserULCqiInfo.isValid=true;
	perUserULCqiInfo.cqiMeasurementConfig=cqiEstimator.uplinkCQIInfo->cqiComputationInfoPerServiceNode(srvcNodeIndxInRx(srvcNode_cnt)).cqiMeasurementConfig;
	perUserULCqiInfo.subbandCqiInfoPerGroup.set_length(1);
	perUserULCqiInfo.subbandCqiInfoPerGroup(0)=subbandULCQIInfo;
	perUserULCqiInfo.subbandCqiInfoPerGroup(0).requestID=perUserULCqiInfo.cqiMeasurementConfig(0).requestID;
 
	// Reciprocity precoder based on UplInk channel For subsequent downlink in TDD mode
	if(mySysInfo.ulDlMode == _ULDL_MODE_TDD_ && mySysInfo.intrCov(serviceNodes(srvcNode_cnt)).length()!=0)
	{
	  ReciprocityPrecodersInfo_S reciprocityPrecoders = generateReciprocityPrecoders(serviceNodes(srvcNode_cnt),nuID,subframeNumber,myRxFrameInfo,myChannelCloud,NIValueForTheServerNode,perUserULCqiInfo.subbandCqiInfoPerGroup(0).requestID,isDownlink);
	  perUserULCqiInfo.subbandCqiInfoPerGroup(0).reciproPrecoderPerRB = reciprocityPrecoders.precoderPerRB;
    
     
      SubbandULCQIInfo_S  dlCQIForReci=updateDLCQIForReci(serviceNodes(srvcNode_cnt),nuID,subframeNumber,myRxFrameInfo,myChannelCloud,NIValueForTheServerNode,mySysInfo,reciprocityPrecoders);
      perUserULCqiInfo.subbandCqiInfoPerGroup(0).cqiIndxPerRBForDLReci=dlCQIForReci.cqiIndxPerRB;
      perUserULCqiInfo.subbandCqiInfoPerGroup(0).rankForDLReci=dlCQIForReci.rank;
      //cout<<"dlCQIForReci.cqiIndxPerRB"<<dlCQIForReci.cqiIndxPerRB<<endl;
        
	}
	uplinkCQIInfo.perUserULCQIFeedback(srvcNodeIndxInRx(srvcNode_cnt)) = perUserULCqiInfo;
	uplinkCQIInfo.isToReportCQI(srvcNodeIndxInRx(srvcNode_cnt))=(bin)1;
      }
    }
  }

  cqiEstimator.clear();
}
 

void Receiver::generateDLFeedback(CQIMeasurementConfig_S &cqiMeasurementConfig, int subframeNumber,RxFrameInfo_S &myRxFrameInfo,ChannelCloud &myChannelCloud, PerformanceMetrics_S &performanceMetrics,SystemInfo_S &mySysInfo,string WideBandCQIlogsFileName, string RIlogsFileName,string SubbandCQIlogsFileName, string AperiodicCQILogsFileName) {

  int nuIndx=myNodeDevice->getNUIndx(cqiMeasurementConfig.nuID);
  
  // 	if (receiverConfig.idealChannel)
  {
    int measurementIndx = cqiEstimator.downlinkCQIInfo->perUserFeedback.getMeasurementIndx(cqiMeasurementConfig.requestID);
    if(measurementIndx==-1)
    {
      cout<<"Undefined measurement config to generateFeedback()."<<endl;
      abort();
    }
    
    double restOfInterferenceInDB=dB(myRxFrameInfo.ROIpowerInWatts);
    //! Rank computation
    bool isRankComputed = false;
    if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isRankAdaptationEnabled)
    {
      if(cqiMeasurementConfig.transmissionScheme==_TRANSMISSION_SCHEME_CL_SM_ || cqiMeasurementConfig.transmissionScheme==_TRANSMISSION_SCHEME_LARGE_DELAY_CDD_ || cqiMeasurementConfig.transmissionScheme==_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_ || cqiMeasurementConfig.transmissionScheme==_TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_|| cqiMeasurementConfig.transmissionScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || cqiMeasurementConfig.transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_ || cqiMeasurementConfig.transmissionScheme==_TRANSMISSION_SCHEME_COMP_ || cqiMeasurementConfig.transmissionScheme==_TRANSMISSION_SCHEME_OLMUMIMO_)
      {
	if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isrankTrigger)
	{
	  /*if(traceValuePerTraceName["EnablePrints"]=="true")
	    cout<<"[detl:]Computing Downlink Rank for User : "<<nodeID<<" in subframe "<<subframeNumber<<" from "<<cqiMeasurementConfig.startRB<<" to  "<<cqiMeasurementConfig.startRB+cqiMeasurementConfig.nRBs-1<<endl;
      */
      if(traceValuePerTraceName["EnablePrints"]=="true")
          cout<<"[detl:]Computing Downlink CSI Parameters for User : "<<nodeID<<" in subframe "<<subframeNumber<<" from "<<cqiMeasurementConfig.startRB<<" to  "<<cqiMeasurementConfig.startRB+cqiMeasurementConfig.nRBs-1<<endl;
      
	  int RI=-1;
	  int desiredTxNodeIndx = find(myRxFrameInfo.channelHandler.txNodeIDs,cqiMeasurementConfig.serverNodeGroup(0));
    if(desiredTxNodeIndx==-1)
    {
      cout<<"Error:: desired tx node is not set for node "<<nodeID <<", txNodeIDs : "<<myRxFrameInfo.channelHandler.txNodeIDs<<", serverNodeGroup : "<<cqiMeasurementConfig.serverNodeGroup<<endl;
      abort();
    }
	  if(cqiMeasurementConfig.transmissionScheme ==_TRANSMISSION_SCHEME_NR_CL_)
	  {
        if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length()==0)
        {
            if(cqiMeasurementConfig.cqiFormatIndicator == _SUBBAND_CQI_ || cqiMeasurementConfig.pmiFormatIndicator == _SUBBAND_PMI_)
            {
                cout<<"Wideband Transmission doesn't support subband formatIndicators...Setting them to wideband..."<<endl;
            }
            cqiMeasurementConfig.cqiFormatIndicator=_WIDEBAND_CQI_;
            cqiMeasurementConfig.pmiFormatIndicator=_WIDEBAND_PMI_;
        }
        RI = cqiEstimator.computeRIForDownlinkRankReportNR(cqiMeasurementConfig,myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,receiverConfig.demodulationScheme,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,&myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,myRxFrameInfo.rxFrame,myRxFrameInfo.channelHandler.channel(desiredTxNodeIndx),cqiMeasurementConfig.txSubcarrierPowerIndBm-30,myChannelCloud.getNoiseVariancePerDimension(nodeID),restOfInterferenceInDB,nodeID,myRxFrameInfo.channelHandler.txNodeIDs(desiredTxNodeIndx),subframeNumber,WideBandCQIlogsFileName,RIlogsFileName,cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered);
	  }
	  else
	    RI = cqiEstimator.computeRIForDownlinkPeriodicRIReport(cqiMeasurementConfig,myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,receiverConfig.demodulationScheme,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,&myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,myRxFrameInfo.rxFrame,myRxFrameInfo.channelHandler.channel,cqiMeasurementConfig.txSubcarrierPowerIndBm-30,myRxFrameInfo.channelHandler.txNodeIDs,myChannelCloud.getNoiseVariancePerDimension(nodeID),restOfInterferenceInDB,nodeID,subframeNumber,WideBandCQIlogsFileName,RIlogsFileName,cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered);
	  
      isRankComputed = true;
      
// 	  #ifdef USING_L2
// 	  if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isPeriodicConfigured)
// 	  {
// 	    if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger)
// 	    {	    
// 	      if(cqiMeasurementConfig.transmissionScheme!=_TRANSMISSION_SCHEME_OLMUMIMO_)
// 	      {
// 		cqiEstimator.FormPeriodicCQIPDU(cqiMeasurementConfig.requestID);
// 	      }
// 	      
// 	    }
// // 	    cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=false;
// 	  }
// 	  #endif
	  int indx=find(performanceMetrics.performanceLogPerNU(nuIndx).rxNodes,nodeID);
	  
	  performanceMetrics.performanceLogPerNU(nuIndx).rankComputationInstant(indx)+=1.0;
	  performanceMetrics.performanceLogPerNU(nuIndx).sumRankPerServiceNode(indx)+=RI;
	}
      }
    }
    
    bool isCQIComputed = false;
//     cout<<"getting the tranmission mode ::"<<cqiEstimator.downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme<<endl;
    // I1 selection
    if(!isRankComputed && (cqiEstimator.downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_COMP_ || cqiEstimator.downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_ || cqiEstimator.downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_ || cqiEstimator.downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_COMP_ || cqiEstimator.downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).transmissionScheme==_TRANSMISSION_SCHEME_OLMUMIMO_))
    {
      if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger && cqiEstimator.downlinkCQIInfo->cqiComputationInfo.cqiMeasurementConfig(0).cqiPorts.length()==8)
      {
	if(traceValuePerTraceName["EnablePrints"]=="true") 
	{
	  cout<<"[both:] Computing periodic Wideband i1 for user :"<<nodeID<<" in subframe "<<subframeNumber<<" from "<<cqiMeasurementConfig.startRB<<" to  "<<cqiMeasurementConfig.startRB+cqiMeasurementConfig.nRBs<<endl;
	}
	//cqiEstimator.computeCQIFromIdealChannelForDownlinkPeriodicCQIReport(txFrameHandlers,receiverConfig.demodulationScheme,schedulerToReceiverInfo.portToPhysicalAntennaMapper,schedulerToReceiverInfo.referenceSignal,schedulerToReceiverInfo.resourceMapper,myRxFrameInfo.rxFrame,myRxFrameInfo.channelHandler.channel,txSignalPowInDB,myRxFrameInfo.signalNodeIDs(0),txNodesToReceiveSignal,myChannelCloud.getNoiseVariancePerDimension(nodeID),restOfInterferenceInDB,nodeID,subframeNumber,WideBandCQIlogsFileName,SubbandCQIlogsFileName);
	cqiEstimator.computeCQIFromIdealChannelForDownlinkPeriodicCQIReport(cqiEstimator.downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx),myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,receiverConfig.demodulationScheme,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,&myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,myRxFrameInfo.rxFrame,myRxFrameInfo.channelHandler.channel,cqiMeasurementConfig.txSubcarrierPowerIndBm-30,cqiMeasurementConfig.serverNodeGroup(0),myRxFrameInfo.channelHandler.txNodeIDs,cqiMeasurementConfig,myChannelCloud.getNoiseVariancePerDimension(nodeID),restOfInterferenceInDB,nodeID,subframeNumber,WideBandCQIlogsFileName,SubbandCQIlogsFileName);
	cqiEstimator.downlinkCQIInfo->perUserFeedback.isValid=true;
	
	if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.periodicMode == _PERIODIC_MODE_1_1_ && cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger)
	{
	  isCQIComputed = true;
	}
      }
    }
    
    
    if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered)
    {
//       if(traceValuePerTraceName["EnablePrints"]=="true")
//         cout<<"[detl:]Computing Aperiodic Downlink CQI for User : "<<nodeID<<" in subframe "<<subframeNumber<<endl;
      if(cqiMeasurementConfig.transmissionScheme ==_TRANSMISSION_SCHEME_NR_CL_) // For NR
      {
        int desiredTxNodeIndx = find(myRxFrameInfo.channelHandler.txNodeIDs,cqiMeasurementConfig.serverNodeGroup(0));

        int txNodeID = myRxFrameInfo.channelHandler.txNodeIDs(desiredTxNodeIndx);
        
        switch(cqiEstimator.mCodebook->getCodeBookConfig().codeBookType)
        {
            case _NR_CODEBOOK_TYPE_I_:
                cqiEstimator.computeDLAperiodicCSIForTypeI(cqiEstimator.downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx),myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,receiverConfig.demodulationScheme,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,&myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,myRxFrameInfo.rxFrame,myRxFrameInfo.channelHandler.channel(desiredTxNodeIndx),cqiMeasurementConfig.txSubcarrierPowerIndBm-30,cqiMeasurementConfig,myChannelCloud.getNoiseVariancePerDimension(nodeID),restOfInterferenceInDB,nodeID,txNodeID,subframeNumber,AperiodicCQILogsFileName);
                break;
                
            case _NR_CODEBOOK_TYPE_I_MULTIPANEL_:
                cqiEstimator.computeDLAperiodicCSIForTypeIMultiPanel(cqiEstimator.downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx),myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,receiverConfig.demodulationScheme,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,&myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,myRxFrameInfo.rxFrame,myRxFrameInfo.channelHandler.channel(desiredTxNodeIndx),cqiMeasurementConfig.txSubcarrierPowerIndBm-30,cqiMeasurementConfig,myChannelCloud.getNoiseVariancePerDimension(nodeID),restOfInterferenceInDB,nodeID,txNodeID,subframeNumber,AperiodicCQILogsFileName);        
                break;
                
            case _NR_CODEBOOK_TYPE_II_:
                if(!cqiEstimator.downlinkCQIInfo->cqiComputationInfo.subbandSizeAperiodic.length())
                {
                    cout<<"Codebook TypeII doesn't support Wideband Transmission.."<<endl;abort();
                }
                
                switch(cqiMeasurementConfig.typeIIMethod)
                {
                    case _OMP_:
                        cqiEstimator.computeDLAperiodicCSIForTypeII_OMP(cqiEstimator.downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx),myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,receiverConfig.demodulationScheme,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,&myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,myRxFrameInfo.rxFrame,myRxFrameInfo.channelHandler.channel(desiredTxNodeIndx),cqiMeasurementConfig.txSubcarrierPowerIndBm-30,cqiMeasurementConfig,myChannelCloud.getNoiseVariancePerDimension(nodeID),restOfInterferenceInDB,nodeID,txNodeID,subframeNumber,AperiodicCQILogsFileName);
                        break;
                    case _AVIK_:
                        cqiEstimator.computeDLAperidicCSIForTypeII_Avik(cqiEstimator.downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx),myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,receiverConfig.demodulationScheme,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,&myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,myRxFrameInfo.rxFrame,myRxFrameInfo.channelHandler.channel(desiredTxNodeIndx),cqiMeasurementConfig.txSubcarrierPowerIndBm-30,cqiMeasurementConfig,myChannelCloud.getNoiseVariancePerDimension(nodeID),restOfInterferenceInDB,nodeID,txNodeID,subframeNumber,AperiodicCQILogsFileName);
                        break;
                    case _HALFBEAM_:
                        cqiEstimator.computeDLAperiodicCSIForTypeII(cqiEstimator.downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx),myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,receiverConfig.demodulationScheme,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,&myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,myRxFrameInfo.rxFrame,myRxFrameInfo.channelHandler.channel(desiredTxNodeIndx),cqiMeasurementConfig.txSubcarrierPowerIndBm-30,cqiMeasurementConfig,myChannelCloud.getNoiseVariancePerDimension(nodeID),restOfInterferenceInDB,nodeID,txNodeID,subframeNumber,AperiodicCQILogsFileName);
                        break;
                    default:
                        cout<<"Invalid TypeIIMethod...Aborting..."<<endl;abort();
                        break;
                }
                break;
                
            default:
                cout<<"[detl:]Codebook Type not set.."<<endl;abort();
                break;
        }
        
        // For NR CQI not forming PDU if needed add later
      }
      else // For LTE
      {
        cqiEstimator.computeCQIFromIdealChannelForDownlinkAperiodicCQIReport(cqiEstimator.downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx),myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,receiverConfig.demodulationScheme,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,&myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,myRxFrameInfo.rxFrame,myRxFrameInfo.channelHandler.channel,cqiMeasurementConfig.txSubcarrierPowerIndBm-30,cqiMeasurementConfig.serverNodeGroup(0),myRxFrameInfo.channelHandler.txNodeIDs,cqiMeasurementConfig,myChannelCloud.getNoiseVariancePerDimension(nodeID),restOfInterferenceInDB,nodeID,subframeNumber,AperiodicCQILogsFileName);
        cqiEstimator.FormAPeriodicCQIPDU(cqiMeasurementConfig.requestID);
      }
      cqiEstimator.downlinkCQIInfo->perUserFeedback.isValid=true; 
      cqiEstimator.downlinkCQIInfo->perUserFeedback.isAperiodicFeedback=true;     
    }
    
    if((!isRankComputed && !isCQIComputed && cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger) || cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger)
    {
      if(traceValuePerTraceName["EnablePrints"]=="true") 
      {
	if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger)
	  cout<<"[detl:]Computing Wideband Periodic Downlink CQI for User : "<<nodeID<<" in subframe "<<subframeNumber<<" from "<<cqiMeasurementConfig.startRB<<" to  "<<cqiMeasurementConfig.startRB+cqiMeasurementConfig.nRBs<<endl;
	else
	  cout<<"[detl:]Computing "<<cqiEstimator.downlinkCQIInfo->cqiComputationInfo.BWPartNumberTrigger<<" BW PART Periodic Downlink CQI for User : "<<nodeID<<" in subframe "<<subframeNumber<<endl;
      }

      //OLLA
      if(receiverConfig.isOuterLoopLinkAdaptationEnabled)
      {
	double sinrDown=0.5;
	double sinrUp=sinrDown/((1.0/receiverConfig.targetBLER)-1.0);
	int indx=find(performanceMetrics.performanceLogPerNU(nuIndx).rxNodes,nodeID);
	if(performanceMetrics.performanceLogPerNU(nuIndx).packetsWithinCQIPeriodicityPerServiceNode(indx)>1)
	  performanceMetrics.performanceLogPerNU(nuIndx).sinroffsetPerServiceNode(indx)+=(performanceMetrics.performanceLogPerNU(nuIndx).acksWithinCQIPeriodicityPerServiceNode(indx)/performanceMetrics.performanceLogPerNU(nuIndx).packetsWithinCQIPeriodicityPerServiceNode(indx))*sinrUp - ((performanceMetrics.performanceLogPerNU(nuIndx).packetsWithinCQIPeriodicityPerServiceNode(indx) - performanceMetrics.performanceLogPerNU(nuIndx).acksWithinCQIPeriodicityPerServiceNode(indx))/performanceMetrics.performanceLogPerNU(nuIndx).packetsWithinCQIPeriodicityPerServiceNode(indx))*sinrDown;

	cqiEstimator.sinrOffsetForOLLA=performanceMetrics.performanceLogPerNU(nuIndx).sinroffsetPerServiceNode(indx);
	performanceMetrics.performanceLogPerNU(nuIndx).acksWithinCQIPeriodicityPerServiceNode(indx)=0.0;
	performanceMetrics.performanceLogPerNU(nuIndx).packetsWithinCQIPeriodicityPerServiceNode(indx)=0.0;
      }
      
      cqiEstimator.computeCQIFromIdealChannelForDownlinkPeriodicCQIReport(cqiEstimator.downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx),myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,receiverConfig.demodulationScheme,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,&myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,myRxFrameInfo.rxFrame,myRxFrameInfo.channelHandler.channel,cqiMeasurementConfig.txSubcarrierPowerIndBm-30,cqiMeasurementConfig.serverNodeGroup(0),myRxFrameInfo.channelHandler.txNodeIDs,cqiMeasurementConfig,myChannelCloud.getNoiseVariancePerDimension(nodeID),restOfInterferenceInDB,nodeID,subframeNumber,WideBandCQIlogsFileName,SubbandCQIlogsFileName);
      cqiEstimator.downlinkCQIInfo->perUserFeedback.isValid=true;
      abort();
//       //#ifdef USING_L2
// 	  if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isPeriodicConfigured)
// 	  {
// 	    if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger)
// 	    {
// 	      //if(mySysInfo.fapiInfo.isEnabled)
// 	      {
// 		cqiEstimator.FormPeriodicCQIPDU();
// 	      }
// 	      
// 	    }
// 	    //cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger=false;
// 	  }
// 	  //#endif
      
    }
    
    
// #ifdef USING_L2
//   if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isPeriodicConfigured)
//     {      
//      // if((!isRankComputed &&cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger) ||cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger /* || (mySysInfo.enableRankAdaptation && cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isrankTrigger)*/)
//       {	    
// 	if(cqiMeasurementConfig.transmissionScheme != _TRANSMISSION_SCHEME_OLMUMIMO_)
// 	{	  
// // 	  cqiEstimator.FormPeriodicCQIPDU();
// 	  cqiEstimator.FormPeriodicCQIPDU(cqiMeasurementConfig.requestID);
// 	}	  
//       }
//     }
// #endif
  }
}

ReciprocityPrecodersInfo_S Receiver::generateReciprocityPrecodersForUL(CQIMeasurementConfig_S &cqiMeasurementConfig, int subframeNumber,RxFrameInfo_S &myRxFrameInfo,ChannelCloud &myChannelCloud, PerformanceMetrics_S &performanceMetrics,SystemInfo_S &mySysInfo,string WideBandCQIlogsFileName, string RIlogsFileName,string SubbandCQIlogsFileName, string AperiodicCQILogsFileName)
{  
  int nuIndx=myNodeDevice->getNUIndx(cqiMeasurementConfig.nuID);
  
  int measurementIndx = cqiEstimator.downlinkCQIInfo->perUserFeedback.getMeasurementIndx(cqiMeasurementConfig.requestID);
  if(measurementIndx==-1)
  {
    cout<<"Undefined measurement config to generateFeedback()."<<endl;
    abort();
  }
  double restOfInterferenceInDB=dB(myRxFrameInfo.ROIpowerInWatts);
  
  if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered)
  {
    if(traceValuePerTraceName["EnablePrints"]=="true")
      cout<<"[detl:]Computing DL reciprocity precoder for User : "<<nodeID<<" in subframe "<<subframeNumber<<endl;
    
    return(cqiEstimator.computeReciprocitybasedPrecoderFromIdealChannelForDownlink(cqiEstimator.downlinkCQIInfo->perUserFeedback.dlCQIMeasurement(measurementIndx),myNodeDevice->L1CorePerNU(nuIndx).dlFrameStructure,receiverConfig.demodulationScheme,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,&myNodeDevice->L1CorePerNU(nuIndx).referenceSignal,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,myRxFrameInfo.rxFrame,myRxFrameInfo.channelHandler.channel,cqiMeasurementConfig.serverNodeGroup(0),myRxFrameInfo.channelHandler.txNodeIDs,cqiMeasurementConfig,myChannelCloud.getNoiseVariancePerDimension(nodeID),restOfInterferenceInDB,nodeID,subframeNumber,AperiodicCQILogsFileName));
  }
  else
  {
    ReciprocityPrecodersInfo_S dummyReciprocityPrecoderStruct;
    return (dummyReciprocityPrecoderStruct);
    
  } 
}
SubbandULCQIInfo_S Receiver::generateULFeedback(int serviceNode,int nuID, int subframeNumber,RxFrameInfo_S &myRxFrameInfo,ChannelCloud &myChannelCloud,double NIValueForTheServerNode, int requestID) {

  
  int nuIndx=myNodeDevice->getNUIndx(nuID);
  
    if(traceValuePerTraceName["EnablePrints"]=="true")
    {
      cout<<"[detl:]Computing Uplink CQI for User : "<<serviceNode<<" in subframe "<<subframeNumber;
#ifdef USING_SRS
      if(cqiEstimator.srsRBs.length())
	cout<<" over RBS : "<<cqiEstimator.srsRBs<<endl;
      else
#endif
	cout<<" over all RBS "<<endl;
    }

    int measurementIndx=0;
    int srvcNodeIndx=find(cqiEstimator.uplinkCQIInfo->serviceNodeIDs,serviceNode);
    if(requestID!=-1)
      measurementIndx=cqiEstimator.uplinkCQIInfo->cqiComputationInfoPerServiceNode(srvcNodeIndx).getMeasurementIndx(requestID);

    CQIMeasurementConfig_S *cqiMeasurementConfig=&cqiEstimator.uplinkCQIInfo->cqiComputationInfoPerServiceNode(srvcNodeIndx).cqiMeasurementConfig(measurementIndx);
    if(receiverConfig.idealChannel)
    {
      int myChannelIndx=find(myRxFrameInfo.channelHandler.txNodeIDs,serviceNode);

   /*   cout<<"Computing CQI at : "<<nodeID<<endl;
      cout<<"NIValueForTheServerNode : "<<dB(NIValueForTheServerNode)<<endl;
      cout<<"Pow : "<<txFrameInfo(myTxFrameInfoIndx).txSubcarrierPowerInDBmPerAntenna<<endl;
   */ 
   
   int nR=myRxFrameInfo.rxFrame.length();
   int maxLayers=cqiMeasurementConfig->cqiPorts.length();
   if(maxLayers>nR) maxLayers=nR;

      SubbandULCQIInfo_S uplinkCQIInfo = cqiEstimator.computeRankAndCQIFromIdealChannelForUplink(myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure,receiverConfig.demodulationScheme,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper
      ,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,myRxFrameInfo.channelHandler.channel(myChannelIndx),cqiMeasurementConfig->
      txSubcarrierPowerIndBm-30,cqiMeasurementConfig->transmissionScheme,cqiMeasurementConfig->waveform,maxLayers,cqiMeasurementConfig->cqiPorts,cqiMeasurementConfig->numberOfCodeWords,myChannelCloud.getNoiseVariancePerDimension(nodeID),NIValueForTheServerNode);
        
      if(traceValuePerTraceName["EnablePrints"]=="true")
      {
          cout<<"[detl:] Uplink Rank for Node "<<serviceNode<<" is: "<<uplinkCQIInfo.rank<<endl;
      }
      return uplinkCQIInfo;
    }
    else
    {
      cout<<"[both:]Non Ideal CQI mode, not yet ready."<<endl;abort();
    }
  
}
//reciprocity

SubbandULCQIInfo_S Receiver::updateDLCQIForReci(int serviceNode,int nuID, int subframeNumber,RxFrameInfo_S &myRxFrameInfo,ChannelCloud &myChannelCloud,double NIValueForTheServerNode, SystemInfo_S &mySysInfo, ReciprocityPrecodersInfo_S &reciprocityPrecoders, int requestID) {

  
  int nuIndx=myNodeDevice->getNUIndx(nuID);
  
    if(traceValuePerTraceName["EnablePrints"]=="true")
    {
      cout<<"[detl:]Computing Downlink CQI for reciprocity for User : "<<serviceNode<<" in subframe "<<subframeNumber;
#ifdef USING_SRS
      if(cqiEstimator.srsRBs.length())
	cout<<" over RBS : "<<cqiEstimator.srsRBs<<endl;
      else
#endif
	cout<<" over all RBS "<<endl;
    }

    int measurementIndx=0;
    int srvcNodeIndx=find(cqiEstimator.uplinkCQIInfo->serviceNodeIDs,serviceNode);
    if(requestID!=-1)
      measurementIndx=cqiEstimator.uplinkCQIInfo->cqiComputationInfoPerServiceNode(srvcNodeIndx).getMeasurementIndx(requestID);

    CQIMeasurementConfig_S *cqiMeasurementConfig=&cqiEstimator.uplinkCQIInfo->cqiComputationInfoPerServiceNode(srvcNodeIndx).cqiMeasurementConfig(measurementIndx);
    if(receiverConfig.idealChannel)
    {
      int myChannelIndx=find(myRxFrameInfo.channelHandler.txNodeIDs,serviceNode);

   /*   cout<<"Computing CQI at : "<<nodeID<<endl;
      cout<<"NIValueForTheServerNode : "<<dB(NIValueForTheServerNode)<<endl;
      cout<<"Pow : "<<txFrameInfo(myTxFrameInfoIndx).txSubcarrierPowerInDBmPerAntenna<<endl;
   */ 
   
   //int nR=myRxFrameInfo.rxFrame.length();
 
   //int maxLayers=cqiMeasurementConfig->cqiPorts.length();
   //int maxLayers=4; //hardcoded
   //if(maxLayers>nR) maxLayers=nR;
   double weakIntr=0.0;
   Array<cmat> intrCov(0);
   double noiseVar=0.0;

   if(mySysInfo.intrCov(serviceNode).length()!=0)
   {
   weakIntr=mySysInfo.weakIntr(serviceNode);
   intrCov=mySysInfo.intrCov(serviceNode);
   noiseVar=mySysInfo.noiseVar(serviceNode);
   }
   else
   {
       cout<<"Error in DL interference for reciprocity"<<endl;
       abort();
   }
       
   double txPowerDLindB=mySysInfo.serverNodeTxPowersInDBm(0)-10*log10(myNodeDevice->L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getUsedSubcarriers())-30;
  
   
   //Obtaining DL channel
   Array<Array <cmat>> ULchannel = myRxFrameInfo.channelHandler.channel(myChannelIndx);
   int l1=ULchannel.length();
   int l2=ULchannel(0).length();
   Array<Array <cmat>> DLchannel;
   DLchannel.set_length(l2);
       
   for(int i=0;i<DLchannel.length();i++)
   {
       DLchannel(i).set_length(l1);
       for(int j=0;j<DLchannel(i).length();j++)
       {
           DLchannel(i)(j).set_size(ULchannel(0)(0).rows(),ULchannel(0)(0).cols());
       }
   }
   int totalRECount=ULchannel(0)(0).size();
   ivec RELocations=getIntegers(0,totalRECount-1);
   Array< cmat > rawChannel(totalRECount);
  for(int tone_cnt=0;tone_cnt<totalRECount;tone_cnt++)
  {
    rawChannel(tone_cnt)=getAcrossArray(ULchannel,RELocations(tone_cnt));
    rawChannel(tone_cnt)=(rawChannel(tone_cnt)).transpose();
  }
  for(int tone_cnt=0;tone_cnt<totalRECount;tone_cnt++)
  {
    setAcrossArray(DLchannel,tone_cnt, rawChannel(tone_cnt));
    
  }
   
  int maxLayers=l1;

      return(cqiEstimator.computeRankAndCQIFromIdealChannelForUplinkForDLReci(myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure,receiverConfig.demodulationScheme,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper
      ,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,DLchannel,txPowerDLindB,cqiMeasurementConfig->transmissionScheme,cqiMeasurementConfig->waveform,maxLayers,cqiMeasurementConfig->cqiPorts,cqiMeasurementConfig->numberOfCodeWords,myChannelCloud.getNoiseVariancePerDimension(nodeID),NIValueForTheServerNode,reciprocityPrecoders, weakIntr,intrCov,noiseVar));
    }
   else
    {
      cout<<"[both:]Non Ideal CQI mode, not yet ready."<<endl;abort();
    } 
  
}

ReciprocityPrecodersInfo_S Receiver::generateReciprocityPrecoders(int serviceNode,int nuID, int subframeNumber,RxFrameInfo_S &myRxFrameInfo,ChannelCloud &myChannelCloud,double NIValueForTheServerNode, int requestID, bool isDownlink) {
  int nuIndx=myNodeDevice->getNUIndx(nuID);
  
  if(!isDownlink) // for Uplink
  {
    if(traceValuePerTraceName["EnablePrints"]=="true")
    {
      cout<<"[detl:]Computing reciprocity precoder in UL subframe for User : "<<serviceNode<<" in subframe "<<subframeNumber;
      #ifdef USING_SRS
      if(cqiEstimator.srsRBs.length())
	cout<<"over RBS : "<<cqiEstimator.srsRBs<<endl;
      else
      #endif
	cout<<" over all RBS "<<endl;
    }
    int measurementIndx=0;
    int srvcNodeIndx=find(cqiEstimator.uplinkCQIInfo->serviceNodeIDs,serviceNode);
    if(requestID!=-1)
      measurementIndx=cqiEstimator.uplinkCQIInfo->cqiComputationInfoPerServiceNode(srvcNodeIndx).getMeasurementIndx(requestID);

    CQIMeasurementConfig_S *cqiMeasurementConfig=&cqiEstimator.uplinkCQIInfo->cqiComputationInfoPerServiceNode(srvcNodeIndx).cqiMeasurementConfig(measurementIndx);
    if(receiverConfig.idealChannel)
    {
      int myChannelIndx=find(myRxFrameInfo.channelHandler.txNodeIDs,serviceNode);
      //cout<<"[detl]Entered in UL reciprocity based precoder calculation for DL..."<<endl;
      return(cqiEstimator.computeReciprocitybasedPrecoderFromIdealChannelForUplink(myNodeDevice->L1CorePerNU(nuIndx).ulFrameStructure,receiverConfig.demodulationScheme,&myNodeDevice->L1CorePerNU(nuIndx).portToAntennaMapper,&myNodeDevice->L1CorePerNU(nuIndx).resourceMapper,myRxFrameInfo.channelHandler.channel(myChannelIndx),cqiMeasurementConfig->txSubcarrierPowerIndBm-30,cqiMeasurementConfig->transmissionScheme,cqiMeasurementConfig->numberOfLayers,cqiMeasurementConfig->cqiPorts,cqiMeasurementConfig->numberOfCodeWords,myChannelCloud.getNoiseVariancePerDimension(nodeID),NIValueForTheServerNode,isDownlink,true)); //enableCQI0
    }
    else
    {
      cout<<"[both:]Non Ideal Precoder calculation mode is not yet ready."<<endl;abort();
    } 
  }
  return ReciprocityPrecodersInfo_S();
}


void checkFrameHandler(Framehandler &frameHandler, int rbNumber, Array<cmat> & receivedRB){

  if(crossCheckTones("fhTraces.txt",frameHandler,rbNumber,receivedRB))
  {
    cout<<"////////////////////////////////////////////////////////////////////////////"<<endl;
    cout<<"//////////////Frame Handler Level 1 testing successfull..../////////////////"<<endl;
    cout<<"////////////////////////////////////////////////////////////////////////////\n"<<endl;
  }
  else
  {
    cout<<"////////////////////////////////////////////////////////////////////////////"<<endl;
    cout<<"//////////////Frame Handler Level 1 testing failed..../////////////////"<<endl;
    cout<<"////////////////////////////////////////////////////////////////////////////\n"<<endl;
  }
  cout<<"[both:] Exiting in checkFrameHandler()...\n"<<endl;exit(0);
}

// bool Receiver::isToReportRI(int subframeNumber)
// {
//   if(subframeNumber==cqiEstimator.downlinkCQIInfo->subframeNumberToReportPeriodicRI)
//     return(true);
//   else
//     return(false);
// };

// bool Receiver::isToReportDLCQI(int subframeNumber)
// {
//   
// //   cout<<"subframeNumber--->"<<subframeNumber<<endl;  
// //   cout<<"subframeNumberToReportPeriodicCQI--->"<<cqiEstimator.downlinkCQIInfo->subframeNumberToReportPeriodicCQI<<endl;  
// //   cout<<"subframeNumberToReportAperiodicCQI--->"<<cqiEstimator.downlinkCQIInfo->subframeNumberToReportAperiodicCQI<<endl<<endl;
//  
//   if(subframeNumber==cqiEstimator.downlinkCQIInfo->subframeNumberToReportPeriodicCQI||subframeNumber==cqiEstimator.downlinkCQIInfo->subframeNumberToReportAperiodicCQI)
//     return(true);
//   else
//     return(false);  
// };

bool Receiver::isToComputeDLFeedback()
{
  if(cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isAPeriodicCQITriggered||cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isBWPartTrigger||cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandTrigger ||cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isWidebandi1Trigger ||cqiEstimator.downlinkCQIInfo->cqiComputationInfo.isrankTrigger)
    return(true);
  else
    return(false);
};
// Array<cvec> Receiver::computeInterference(Array<cmat> &receivedFrame,Array< Array<cmat> > &channel, double txPowerInWatts,SchedulerToReceiverInfo_S &schToRxInfo){
// 
// 
//   Array<cvec> receivedVec(receivedFrame.length());
//   Array< Array<cvec> > receivedChn(channel.length());
//   for(int txAnt_cnt=0;txAnt_cnt<channel.length();txAnt_cnt++)
//     receivedChn(txAnt_cnt).set_length(channel(txAnt_cnt).length());
// 
//   ivec scheduledRBs=schToRxInfo.dataBlkInfo(0).resourceBlocks;
//   imat rbStructure=frameHandler.getResourceUnitMatrix(scheduledRBs(0));
//   ivec dataTones=find(rbStructure,0,true);
//   for(int rb_cnt=0;rb_cnt<scheduledRBs.length();rb_cnt++)
//   {
//     //Extracting Y
//     Array<cmat> receivedRB = extractFromRBArea(receivedFrame,&frameHandler,schToRxInfo.resourceMapper,scheduledRBs(rb_cnt),1);
//     Array<cvec> rxTones=getFromArrayOfMatrix(receivedRB,dataTones);
//     for(int rxAnt_cnt=0;rxAnt_cnt<receivedVec.length();rxAnt_cnt++)
//       append(receivedVec(rxAnt_cnt),rxTones(rxAnt_cnt));
// 
//     //Extracting H
//     Array< Array<cmat> > channelPerRB = extractFromRBArea(channel,&frameHandler,schToRxInfo.resourceMapper,scheduledRBs(rb_cnt),1);
//     Array< Array<cvec> > channelTones=getFromArrayOfArrayOfMatrix(channelPerRB,dataTones);
//     for(int txAnt_cnt=0;txAnt_cnt<channelTones.length();txAnt_cnt++)
//       for(int rxAnt_cnt=0;rxAnt_cnt<channelTones(txAnt_cnt).length();rxAnt_cnt++)
// 	append(receivedChn(txAnt_cnt)(rxAnt_cnt),channelTones(txAnt_cnt)(rxAnt_cnt));
// 
//   }
//   Array<cvec> interferenceSignal(receivedFrame.length());
//   for(int rxAnt_cnt=0;rxAnt_cnt<receivedFrame.length();rxAnt_cnt++)
//     interferenceSignal(rxAnt_cnt).set_length(dataTones.length()*scheduledRBs.length());
//   cmat H,Y(receivedFrame.length(),1),X(schToRxInfo.dataBlkInfo(0).modulatedSymbols.length(),1);
//   cmat Intr;
//   complex< double > sigmaX(sqrt(txPowerInWatts),0);
//   for(int re_cnt=0;re_cnt<dataTones.length()*scheduledRBs.length();re_cnt++)
//   {
// 
//     Y.set_col(0,getAcrossArray(receivedVec,re_cnt));
//     H=getAcrossArray(receivedChn,re_cnt);
//     X.set_col(0,getAcrossArray(schToRxInfo.dataBlkInfo(0).modulatedSymbols,re_cnt));
//     Intr=Y-H*X*sigmaX;
//     setAcrossArray(interferenceSignal,re_cnt,Intr.get_col(0));
//   }
//   return(interferenceSignal);
// }

// double Receiver::getWidebandSINR(RxFrameInfo_S &myRxFrameInfo, SchedulerToReceiverInfo_S &schedulerToReceiverInfo,Array<TxFrameInfo_S> &txFrameInfo,ChannelCloud &myChannelCloud)
// {
//   ivec txNodesToReceiveSignal=myRxFrameInfo.txNodesToReceiveSignalFrom;
//   int numCRSPorts;
//   if(schedulerToReceiverInfo.dataBlkInfo.length())
//     numCRSPorts=schedulerToReceiverInfo.dataBlkInfo(0).dataGenInfo.rbInfo.rsInfo.crsPorts.length();
//   else
//     numCRSPorts=txFrameInfo(0).nTxAntenna;
// 
//   TransmissionScheme_E transmissionScheme;
//   DemodulationScheme_E desc;
//   ivec portReferenceNumbers;
//   if(numCRSPorts==1)
//   {
//     transmissionScheme=_TRANSMISSION_SCHEME_SINGLE_PORT_CRS_;
//     desc=_DEMODULATION_SCHEME_MMSE_;
//   }
//   else
//   {
//     transmissionScheme=_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_;
//     desc=_DEMODULATION_SCHEME_MRC_;
//   }
// 
//   int numDLRBs=myNodeDevice->L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getResourceBlocksPerSubframe();
// 
//   int associatedServerNodeID=myRxFrameInfo.signalNodeIDs(0);
// 
//   int desiredChannelIndex=find(myRxFrameInfo.channelHandler.txNodeIDs,associatedServerNodeID);
// 
// 
// 
//   int desiredTxFrameInfoIndex=getFrameInfoIndx(txFrameInfo,to_ivec(associatedServerNodeID))(0);
// 
//   double txSubcarrierPowerInDB=txFrameInfo(desiredTxFrameInfoIndex).txSubcarrierPowerInDBmPerAntenna-30;
// 
// 
//   Array< Array<cmat> > myChannel=myRxFrameInfo.channelHandler.channel(desiredChannelIndex);
// 
//   Array< Array<cmat> > signalChannel;
//   Array< Array<cvec> > signalChannelonRSLocations;
//   cmat myEstIntrCov, myEstIntrCov2;
//   Array<cmat> receivedRB;
// 
// 
//   ivec CRSPorts=getIntegers(0,numCRSPorts-1);
//   portReferenceNumbers=CRSPorts+ones_i(numCRSPorts);
//   ivec portLocations=getPortLocationsInRB(&frameHandler,CRSPorts);
//   imat rbRSPattern;
//   Array<cvec> txRSPerPort;
// 
//   vec sinroverCRSPorts(0);
//   vec tempSINR(0);
//   double widebandSINR;
//   for(int rbCnt=0;rbCnt<numDLRBs;rbCnt++)
//   {
//     rbRSPattern = schedulerToReceiverInfo.frameStructure->getResourceUnitMatrix(rbCnt);
//     txRSPerPort= myNodeDevice->L1CorePerNU(nuIndx).referenceSignal.getTxRSPerPort(rbCnt,CRSPorts);
//     for(int Cnt=0;Cnt<txRSPerPort.length();Cnt++)
//       txRSPerPort(Cnt)=sqrt(inv_dB(txSubcarrierPowerInDB))*txRSPerPort(Cnt);
// 
//     signalChannel=extractFromRBArea(myChannel,&frameHandler,schedulerToReceiverInfo.resourceMapper,rbCnt,1);
//     receivedRB=extractFromRBArea(myRxFrameInfo.rxFrame,&frameHandler,schedulerToReceiverInfo.resourceMapper,rbCnt,1);
// 
//     switch(transmissionScheme)
//     {
//       /// tx modes 1,2,3,7,...,8,9
//       case _TRANSMISSION_SCHEME_SINGLE_PORT_CRS_:
//       {	
// 	channelEstimator.estimateInterferenceCovarianceInRB(myEstIntrCov,receivedRB,signalChannel,txRSPerPort,portReferenceNumbers, transmissionScheme, rbRSPattern);
// 	for(int reCnt=0;reCnt<portLocations.length();reCnt++)
// 	{
// 	  cmat H=sqrt(inv_dB(txSubcarrierPowerInDB))*getAcrossArray(signalChannel,portLocations(reCnt));
// 	  tempSINR=concat(tempSINR,getSINRperLayer(desc,H,myEstIntrCov,zeros_c(receivedRB.length(),receivedRB.length()),1,1));
// 	}
//       break;
//       }
//       case _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_:
//       {
// 	cmat precodedChannel;
// 	ivec usedLayers="0 1";
// 	if(signalChannel(0).size()==2)
// 	  channelEstimator.estimateInterferenceCovarianceInRB(myEstIntrCov,receivedRB,signalChannel,txRSPerPort,portReferenceNumbers, transmissionScheme, rbRSPattern, usedLayers);
// 	else if(signalChannel(0).size()==4)
// 	{
// 	  usedLayers="0 2";
// 	  channelEstimator.estimateInterferenceCovarianceInRB(myEstIntrCov,receivedRB,signalChannel,txRSPerPort,portReferenceNumbers, transmissionScheme, rbRSPattern, usedLayers);
// 	  usedLayers="1 3";
// 	  channelEstimator.estimateInterferenceCovarianceInRB(myEstIntrCov2,receivedRB,signalChannel,txRSPerPort,portReferenceNumbers, transmissionScheme, rbRSPattern, usedLayers);
// 	}
// 	for(int reCnt=0;reCnt<portLocations.length();reCnt++)
// 	{
// 	  cmat H=sqrt(inv_dB(txSubcarrierPowerInDB))*getAcrossArray(signalChannel,portLocations(reCnt));	  
// 	  precodedChannel=zeros_c(2*H.rows(), 2);
// 	  if(H.rows()==4)
// 	  {
// 	    if(reCnt%2==0)
// 	      usedLayers="0 2";
// 	    else
// 	      usedLayers="1 3";
// 	  }
// 	  for(int rxant=0;rxant<H.rows();rxant++)
// 	  {
// 	    precodedChannel(rxant*2,0) = H(rxant,usedLayers(0));
// 	    precodedChannel(rxant*2,1) =  -H(rxant,usedLayers(1));
// 	    precodedChannel(rxant*2+1,0) = conj(H(rxant,usedLayers(1)));
// 	    precodedChannel(rxant*2+1,1) = conj(H(rxant,usedLayers(0)));
// 	  }
// 	  if(H.rows()==2)	    tempSINR=concat(tempSINR,getSINRperLayer(desc,precodedChannel,myEstIntrCov,zeros_c(2*receivedRB.length(),2*receivedRB.length()),usedLayers.length(),1));
// 	  else if(signalChannel(0).size()==4)
// 	  {
// 	    if(reCnt%2==0)	       tempSINR=concat(tempSINR,getSINRperLayer(desc,precodedChannel,myEstIntrCov,zeros_c(2*receivedRB.length(),2*receivedRB.length()),usedLayers.length(),1));
// 	    else	      tempSINR=concat(tempSINR,getSINRperLayer(desc,precodedChannel,myEstIntrCov2,zeros_c(2*receivedRB.length(),2*receivedRB.length()),usedLayers.length(),1));  
// 	  }
// 	}
// 	break;
//       }
//       default:
// 	cout<<"[both:] Unknown transmissionScheme ... aborting at getWidebandSINR() "<<endl;abort();
//     }
//     sinroverCRSPorts=concat(sinroverCRSPorts,tempSINR);
//   }
// 
//   widebandSINR=abs(sum(sinroverCRSPorts)/sinroverCRSPorts.length());
//   widebandSINR=10*log10(widebandSINR);
//   return (widebandSINR);
// }

// double Receiver::getWidebandSINR(RxFrameInfo_S &myRxFrameInfo, SchedulerToReceiverInfo_S &schedulerToReceiverInfo,Array<TxFrameInfo_S> &txFrameInfo,ChannelCloud &myChannelCloud)
// {
//    ivec txNodesToReceiveSignal=myRxFrameInfo.txNodesToReceiveSignalFrom;
//   int numCRSPorts;
//   if(schedulerToReceiverInfo.dataBlkInfo.length())
//     numCRSPorts=schedulerToReceiverInfo.dataBlkInfo(0).dataGenInfo.rbInfo.rsInfo.crsPorts.length();
//   else
//     numCRSPorts=txFrameInfo(0).nTxAntenna;
// 
//   TransmissionScheme_E transmissionScheme;
//   DemodulationScheme_E desc;
//   int nLayers;
//   ivec portReferenceNumbers;
//   if(numCRSPorts==1)
//   {
//     transmissionScheme=_TRANSMISSION_SCHEME_SINGLE_PORT_CRS_;
//     desc=_DEMODULATION_SCHEME_MMSE_;
//     nLayers=1;
//   }
//   else
//   {
//     transmissionScheme=_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_;
//     desc=_DEMODULATION_SCHEME_MRC_;
//     nLayers=2;
//   }
//   int associatedServerNodeID=myRxFrameInfo.signalNodeIDs(0);
//   int desiredChannelIndex=find(myRxFrameInfo.channelHandler.txNodeIDs,associatedServerNodeID);
//   ivec interfererChannelIndices=find(myRxFrameInfo.channelHandler.txNodeIDs,myRxFrameInfo.interfererNodeIDs);
//   int desiredTxFrameInfoIndex=getFrameInfoIndx(txFrameInfo,to_ivec(associatedServerNodeID))(0);
//   double txSubcarrierPowerInDB=txFrameInfo(desiredTxFrameInfoIndex).txSubcarrierPowerInDBmPerAntenna-30;
//   ivec intrTxFrameInfoIndex=getFrameInfoIndx(txFrameInfo,myRxFrameInfo.interfererNodeIDs);
//   Array<Framehandler *> interferingTxFrameHandlers(intrTxFrameInfoIndex.length());
//   vec interfererTxSCPowerInDB(intrTxFrameInfoIndex.length());
//   for(int cnt=0;cnt<intrTxFrameInfoIndex.length();cnt++)
//   {
//     interferingTxFrameHandlers(cnt)=txFrameInfo(intrTxFrameInfoIndex(cnt)).frameHandler;
//     interfererTxSCPowerInDB(cnt)=txFrameInfo(intrTxFrameInfoIndex(cnt)).txSubcarrierPowerInDBmPerAntenna-30;
//   }
//   double restOfInterferenceInDB=dB(myRxFrameInfo.ROIpowerInWatts);
//   Array<cmat> rxFrame;
//   Array< Array<cmat> > myChannel=myRxFrameInfo.channelHandler.channel(desiredChannelIndex);
//   int nRBs=myNodeDevice->L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getResourceBlocksPerSubframe();
//   Array<vec>sinr= cqiEstimator.computeSinrPerCW(txFrameInfo(desiredTxFrameInfoIndex).frameHandler,txSubcarrierPowerInDB, interferingTxFrameHandlers, interfererTxSCPowerInDB, interfererChannelIndices,myChannel,rxFrame, myRxFrameInfo.channelHandler.channel, desc, schedulerToReceiverInfo.portToPhysicalAntennaMapper,schedulerToReceiverInfo.referenceSignal, schedulerToReceiverInfo.resourceMapper, myChannelCloud.getNoiseVariancePerDimension(nodeID), restOfInterferenceInDB,0,transmissionScheme, nLayers,schedulerToReceiverInfo.rbInfoForUnScheduled.rsInfo.crsPorts,1,nRBs,false,-1,true); 
//   double widebandSINR=mean(sinr(0));
//   widebandSINR=10*log10(widebandSINR);
//   return (widebandSINR);
// }

// Array< Array<cmat> > Receiver::computeWideBandChannelOverUplinkDMRS(RxFrameInfo_S& myRxFrameInfo, SystemInfo_S& mySysInfo, bool isDownlink, PerformanceMetrics_S* performanceMetrics, Array< TxFrameInfo_S >& txFrameInfo, SchedulerToReceiverInfo_S& schedulerToReceiverInfo, ChannelCloud& myChannelCloud, string MCSlogsFileName)
// {
//     NUParams_S nuParams=myNodeDevice->L1CorePerNU(0).nuParameters;
//     channelEstimator.loadParams(1.0/nuParams.samplingFrequencyInHz,nuParams.subcarrierSpacingInHz,nuParams.fftSize);
//     
//     int guardSCs= myNodeDevice->L1CorePerNU(0).resourceMapper.ulResourceMapper.getGuardSubcarriersInOneSide();
//     cmat guard1(guardSCs,myRxFrameInfo.rxFrame(0).cols()), guard2(guardSCs-1,myRxFrameInfo.rxFrame(0).cols());
//     guard1.clear();guard2.clear();
//     
//    ivec txFrameInfoNodes(0);
//     for(int cnt=0;cnt<txFrameInfo.length();cnt++)
//       append(txFrameInfoNodes,txFrameInfo(cnt).nodeID);
//    
//     ivec interfererFrameInfoIndices;
//     interfererFrameInfoIndices=findNot(txFrameInfoNodes,schedulerToReceiverInfo.txNodeIDs);
//     
//     vec intrScRxPowerInWatts;
//     intrScRxPowerInWatts.set_length(interfererFrameInfoIndices.length());
//     for(int intr_cnt=0;intr_cnt<interfererFrameInfoIndices.length();intr_cnt++)
//       intrScRxPowerInWatts(intr_cnt)=inv_dB(txFrameInfo(interfererFrameInfoIndices(intr_cnt)).txSubcarrierPowerInDBmPerAntenna+myChannelCloud.getLinkSpecificChannelGainInDB(txFrameInfo(interfererFrameInfoIndices(intr_cnt)).nodeID,nodeID,txFrameInfo(interfererFrameInfoIndices(intr_cnt)).txPanelID,schedulerToReceiverInfo.rxPanelID)-30);
//     
//     double roiScRxPowInWatts=myRxFrameInfo.ROIpowerInWatts;
//     
//     ivec myFrameInfoIndices;
//     myFrameInfoIndices=find(txFrameInfoNodes,schedulerToReceiverInfo.txNodeIDs);
//     
//     ivec portIndices = schedulerToReceiverInfo.dataBlkInfo(0).dataGenInfo.rbInfo.rsInfo.DeModPorts;
//     ivec portReferenceNumbers = schedulerToReceiverInfo.dataBlkInfo(0).dataGenInfo.rbInfo.getPortReferenceNumbers(portIndices);
//     
//     Array< Array<cvec> > txRSPerPortPerSymbol;
//     Array<ivec> RSsymbolsPerPort;
// 
//     txRSPerPortPerSymbol=myNodeDevice->L1CorePerNU(0).referenceSignal.DMRSsequencePerPortPerSymbol;
//     RSsymbolsPerPort=myNodeDevice->L1CorePerNU(0).referenceSignal.DMRSsymbolsPerPort;
//     
//     for(int temp1=0;temp1<txRSPerPortPerSymbol.length();temp1++)
//       for(int temp2=0;temp2<txRSPerPortPerSymbol(temp1).length();temp2++)
// 	txRSPerPortPerSymbol(temp1)(temp2)=sqrt(inv_dB(schedulerToReceiverInfo.txSubcarrierPowerInDBmPerAntenna(0)+myNodeDevice->L1CorePerNU(0).dlRbInfoForUnScheduled.crsPowBoostInDB -30))*txRSPerPortPerSymbol(temp1)(temp2);
//       
//     FadingParameters_S fadingParameters= myChannelCloud.getLinkSpecificFadingParameters(txFrameInfo(myFrameInfoIndices(0)).nodeID, nodeID);
//     double dopplerSpreadInHz = myChannelCloud.getNodeVelocity(schedulerToReceiverInfo.txNodeIDs(0),nodeID)*myChannelCloud.getNodeCarrierFrequency(nodeID)/lightSpeed;
//     Array< Array<cmat> > wideBandChannel=channelEstimator.estimateWideBandChannel(txRSPerPortPerSymbol,guardSCs, myRxFrameInfo.rxFrame, schedulerToReceiverInfo.dataBlkInfo(0).dataGenInfo.rbInfo.logicalSubFrameStructure, RSsymbolsPerPort, intrScRxPowerInWatts,roiScRxPowInWatts, (myChannelCloud.getLinkSpecificChannelGainInDB(txFrameInfo(myFrameInfoIndices(0)).nodeID,nodeID,txFrameInfo(myFrameInfoIndices(0)).txPanelID,schedulerToReceiverInfo.rxPanelID)),myChannelCloud.getNoiseVariancePerDimension(nodeID)*2, dopplerSpreadInHz, portReferenceNumbers, fadingParameters.nTaps,fadingParameters.tapDel,fadingParameters.tapGain , schedulerToReceiverInfo.dataBlkInfo(0).dataGenInfo.rbInfo.rsInfo.CQIrsType, portIndices);
//     
//     for(int i1=0; i1<wideBandChannel.size(); i1++)
//       for(int i2=0; i2<wideBandChannel(i1).size(); i2++)
// 	wideBandChannel(i1)(i2) = concat_vertical(concat_vertical(guard1, wideBandChannel(i1)(i2)), guard2);
//     
//       return wideBandChannel;
//   
// }
