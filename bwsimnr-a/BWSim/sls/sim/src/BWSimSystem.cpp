


#include "../include/BWSimSystem.h"

extern ivec powerHeadroomNodeIDs; 
extern vec powerHeadroomInDBPerNode;
extern ivec powerHeadroomInDBPerNode_subframeCount;
cLTETables dlLteTables,ulLteTables;
cTBTables dlNRTables,ulNRTables;
CodebookCollection codebookCollection;

//wraper to convert column of mat to standard vector for specific 
template<class T>std::vector<T> && matColToStdVector(Mat<T> a, int col=0)
{
  std::vector<T> toReturn;toReturn.reserve(a.rows());
  for(int i=0;i<a.rows();++i)
  {
    toReturn.push_back(a(i,col));
  }
  return std::move(toReturn);  
}

void info()
{
  cout << "Build :  " << __DATE__  << " : " << __TIME__ << endl;
  #ifdef GITVERSION 
  cout << "Git Build :  " << WRAP(GITVERSION)  << endl;
  #endif
}


void performLinkGainBasedAssociation(ChannelCloud* channelCloud, AssociationInfo_S& associationInfo, SystemInfo_S& mySysInfo)
{
    string filename = mySysInfo.resultFiles.directory+"linkGainAssociationLogs.txt";
    ofstream fout(filename.c_str(),std::ios::out);
    fout<<"%srvcNode,\tassociatedSrvrNode,\tlinkGain,\tRSRP,\tRSSI,\tRSRQ,\tSIR,\tnoiseVariance,\tSINR...NOTE: all are in dB"<<endl;
    mat linkGain = channelCloud->getLinkSpecificChannelGainInDB(associationInfo.serverNodes,associationInfo.serviceNodes,true);
    ivec associatedSrvrIndx=find(associationInfo.serverNodes,associationInfo.associatedServerNodesForEachServiceNode);
    
    vec serviceNodeReceiveNoiseFigureInDB,serverNodeTransmitPowerInDB;
    if(parse(mySysInfo.resultFiles.linkTableFileName,"serviceNodeReceiveNoiseFigureInDB",serviceNodeReceiveNoiseFigureInDB)==false)  {	cout<<"[both:] Error... Unable to load serviceNodeReceiveNoiseFigureInDB from the file in associateNodes()..."<<endl;	abort();  }
    if(!parse(mySysInfo.resultFiles.linkTableFileName,"serverNodeTransmitPowerInDBm",serverNodeTransmitPowerInDB))
    {
        cout<<"Unable to parse serverNodeTransmitPowerInDBm from "<<mySysInfo.resultFiles.linkTableFileName<<endl;abort();
    }
    serverNodeTransmitPowerInDB -=30;
    
//     double carrierFreqInMHz = mySysInfo.carrierInfo.carrierFreqInMHz;
    double bandwidthInHz = mySysInfo.carrierInfo.carrierBandwidthInMHz*1e6;
    
    vec serviceNodeReceiveNoiseVariance = inv_dB(-203.8 + 10*log10(bandwidthInHz) + serviceNodeReceiveNoiseFigureInDB);
    
    for(int ue_cnt=0;ue_cnt<associationInfo.serviceNodes.length();ue_cnt++)
    {
        double RSRPinDB = linkGain(ue_cnt,associatedSrvrIndx(ue_cnt)) + serverNodeTransmitPowerInDB(associatedSrvrIndx(ue_cnt));
        associationInfo.associatedBeamPairLinkforEachServiceNode(ue_cnt).srvrBeamID=0;
        associationInfo.associatedBeamPairLinkforEachServiceNode(ue_cnt).srvcBeamID=0;
        associationInfo.associatedBeamPairLinkforEachServiceNode(ue_cnt).rspIndBm=0;
        associationInfo.associatedBeamPairLinkforEachServiceNode(ue_cnt).rsrpIndBm=RSRPinDB+30;
        
        double RSRP = inv_dB(RSRPinDB);
        double RSSI = sum(inv_dB(linkGain.get_row(ue_cnt)));
        double SIR = RSRP/(RSSI - RSRP);
        double RSRQ = RSRP/RSSI;
        double SINR = RSRP/(RSSI-RSRP+serviceNodeReceiveNoiseVariance(ue_cnt));
        
        fout<<associationInfo.serviceNodes(ue_cnt)<<",\t"<<associationInfo.associatedServerNodesForEachServiceNode(ue_cnt)<<",\t"<<linkGain(ue_cnt,associatedSrvrIndx(ue_cnt))<<",\t";
        fout<<dB(RSRP)<<",\t"<<dB(RSSI)<<",\t"<<dB(RSRQ)<<",\t"<<dB(SIR)<<",\t"<<dB(serviceNodeReceiveNoiseVariance(ue_cnt))<<",\t"<<dB(SINR)<<endl;
    }
    fout.close();
}

// void printAngleSpread(McellSystem &mySys,AssociationInfo_S associationInfo)
// {
//   Array <vec > subpath_Boresight_5GAoD_angle,subpath_Boresight_5GAoA_angle,subpath_Boresight_5GZoD_angle,subpath_Boresight_5GZoA_angle;
//   for(int rx_cnt=0; rx_cnt<associationInfo.serviceNodes.length(); rx_cnt++)
//   {
//     int rxNodeId = associationInfo.serviceNodes(rx_cnt);
//     int txNodeId = associationInfo.getMyAssociatedServerNode(rxNodeId);
//     int scmLinkIndx = mySys.channelCloud.getScmLinkIndx(rxNodeId,txNodeId);
//     
//       if(scmLinkIndx!=-1)
//       {
//       
// 	SCMLink scmLink =  mySys.channelCloud.scmLinks(scmLinkIndx);
// 	
// 	//AOD spread 
// 	
// 	subpath_Boresight_5GAoD_angle  = scmLink.subpath_Boresight_5GAoD_angle; 
// 	subpath_Boresight_5GAoA_angle  = scmLink.subpath_Boresight_5GAoA_angle; 
// 	subpath_Boresight_5GZoD_angle  = scmLink.subpath_Boresight_5GZoD_angle; 
// 	subpath_Boresight_5GZoA_angle  = scmLink.subpath_Boresight_5GZoA_angle; 
// 	
// 	// Full calibration Metric 3 
// 	double mu_theta_1 = 0.0, mu_theta_2 = 0.0, mu_theta_3 = 0.0, mu_theta_4 = 0.0;
// 	double theta_nm_mu_1 = 0.0, theta_nm_mu_2 = 0.0, theta_nm_mu_3 = 0.0, theta_nm_mu_4 = 0.0;
// 	double AS_AOD = 0.0,avgAOD = 0.0;
// 	double AS_AOA = 0.0,avgAOA = 0.0;
// 	double AS_ZOD = 0.0,avgZOD = 0.0;
// 	double AS_ZOA = 0.0,avgZOA = 0.0;
// 	double TotalPN = 0.0;
// 	int nTaps = scmLink.tapPow5GChannel.length();
// 	ivec idx;
// 	for(int i=0; i<nTaps; i++)
// 	{
// 	  double Pn = scmLink.tapPow5GChannel(i);
// 	  idx = scmLink.get5GSubPathIndicesofClusters(scmLink.SubPathtypeinCluster5G(i));
// 	  for(int j=0; j<idx.length(); j++)
// 	  {
// 	    mu_theta_1 += subpath_Boresight_5GAoD_angle(i)(j)*Pn/idx.length();
// 	    mu_theta_2 += subpath_Boresight_5GAoA_angle(i)(j)*Pn/idx.length();
// 	    mu_theta_3 += subpath_Boresight_5GZoD_angle(i)(j)*Pn/idx.length();
// 	    mu_theta_4 += subpath_Boresight_5GZoA_angle(i)(j)*Pn/idx.length();
// 	    
// 	    TotalPN  +=Pn/idx.length();
// 	  }
// 	}
// 	
// 	mu_theta_1 = mu_theta_1/TotalPN;
// 	mu_theta_2 = mu_theta_2/TotalPN;
// 	mu_theta_3 = mu_theta_3/TotalPN;
// 	mu_theta_4 = mu_theta_4/TotalPN;
// 	for(int i=0; i<nTaps; i++)
// 	{
// 	  double Pn = scmLink.tapPow5GChannel(i);
// 	  idx = scmLink.get5GSubPathIndicesofClusters(scmLink.SubPathtypeinCluster5G(i));
// 	  for(int j=0; j<idx.length(); j++)
// 	  {
// 	    theta_nm_mu_1 = 0.0;
// 	    theta_nm_mu_2 = 0.0;
// 	    theta_nm_mu_3 = 0.0;
// 	    theta_nm_mu_4 = 0.0;
// 	    
// 	    if((subpath_Boresight_5GAoD_angle(i)(j) - mu_theta_1)< -180)
// 	    {
// 	      theta_nm_mu_1 = 360 +(subpath_Boresight_5GAoD_angle(i)(j) - mu_theta_1);
// 	      theta_nm_mu_1 = 360 +(subpath_Boresight_5GAoA_angle(i)(j) - mu_theta_2);
// 	      theta_nm_mu_3 = 360 +(subpath_Boresight_5GZoD_angle(i)(j) - mu_theta_3);
// 	      theta_nm_mu_4 = 360 +(subpath_Boresight_5GZoA_angle(i)(j) - mu_theta_4);
// 	    }
// 	    else if(abs(subpath_Boresight_5GAoD_angle(i)(j) - mu_theta_1)<=180)
// 	    {
// 	      theta_nm_mu_1 = (subpath_Boresight_5GAoD_angle(i)(j) - mu_theta_1);
// 	    }
// 	    else{
// 	      theta_nm_mu_1 = 360 - (subpath_Boresight_5GAoD_angle(i)(j) - mu_theta_1);
// 	    }
// 	    AS_AOD += pow(theta_nm_mu_1,2)*Pn/idx.length();
// 	    // AOA 
// 	    if((subpath_Boresight_5GAoA_angle(i)(j) - mu_theta_2)< -180)
// 	    {
// 	      theta_nm_mu_2 = 360 +(subpath_Boresight_5GAoA_angle(i)(j) - mu_theta_2);
// 	    }
// 	    else if(abs(subpath_Boresight_5GAoA_angle(i)(j) - mu_theta_2)<=180)
// 	    {
// 	      theta_nm_mu_2 = (subpath_Boresight_5GAoA_angle(i)(j) - mu_theta_2);
// 	    }
// 	    else{
// 	      theta_nm_mu_2 = 360 - (subpath_Boresight_5GAoA_angle(i)(j) - mu_theta_2);
// 	    }
// 	    
// 	    AS_AOA += pow(theta_nm_mu_2,2)*Pn/idx.length();
// 	    // ZOD 
// 	    if((subpath_Boresight_5GZoD_angle(i)(j) - mu_theta_3)< -180)
// 	    {
// 	      theta_nm_mu_3 = 360 +(subpath_Boresight_5GZoD_angle(i)(j) - mu_theta_3);
// 	    }
// 	    else if(abs(subpath_Boresight_5GZoD_angle(i)(j) - mu_theta_3)<=180)
// 	    {
// 	      theta_nm_mu_3 = (subpath_Boresight_5GZoD_angle(i)(j) - mu_theta_3);
// 	    }
// 	    else{
// 	      theta_nm_mu_3 = 360 - (subpath_Boresight_5GZoD_angle(i)(j) - mu_theta_3);
// 	    }
// 	    
// 	    AS_ZOD += pow(theta_nm_mu_3,2)*Pn/idx.length();
// 	    // ZOA 
// 	    if((subpath_Boresight_5GZoA_angle(i)(j) - mu_theta_4)< -180)
// 	    {
// 	      theta_nm_mu_4 = 360 +(subpath_Boresight_5GZoA_angle(i)(j) - mu_theta_4);
// 	    }
// 	    else if(abs(subpath_Boresight_5GZoA_angle(i)(j) - mu_theta_4)<=180)
// 	    {
// 	      theta_nm_mu_4 = (subpath_Boresight_5GZoA_angle(i)(j) - mu_theta_4);
// 	    }
// 	    else{
// 	      theta_nm_mu_4 = 360 - (subpath_Boresight_5GZoA_angle(i)(j) - mu_theta_4);
// 	    }
// 	    
// 	    AS_ZOA += pow(theta_nm_mu_4,2)*Pn/idx.length();
// 	    
// 	    TotalPN += Pn/idx.length();
// 	  }
// 	}
// 	AS_AOD = sqrt(AS_AOD/TotalPN);
// 	AS_AOA = sqrt(AS_AOA/TotalPN);
// 	AS_ZOD = sqrt(AS_ZOD/TotalPN);
// 	AS_ZOA = sqrt(AS_ZOA/TotalPN);
// 	
// 	// print in file
// 	string angleSpread= "angleSpread_AOD_AOA_ZOD_ZOA.txt";
// 	std::ofstream ASFile;
// 	static bool isNewPrint=1;
// 	if(isNewPrint)
// 	{
// 	  ASFile.open(angleSpread.c_str(),ios::out);
// 	  ASFile<<"% mu"<<";\t AS_AOD"<<";\t AS_AOA"<<";\t AS_ZOD"<<";\t AS_ZOA"<<endl;
// 	  isNewPrint=0;
// 	}
// 	else
// 	{
// 	  ASFile.open(angleSpread.c_str(),ios::app);
// 	}
// 	
// 	ASFile<<mu_theta_1<<",\t"<<AS_AOD<<",\t"<<AS_AOA<<"\t"<<AS_ZOD<<"\t"<<AS_ZOA<<endl;
//       }
//   }
// }

// vec svdChannelMat(Array<Array <Array <cvec> > >& channel, vec tapDelay, int fftsize, int noRePerRB, int noOfRBs,double samplingFreqToUse)
// {  
//     cvec channelTapGains;
//     cvec frqDomainChannel;
//     
//     int txAntennaCount = 2, rxAntennaCount = 2,beamId =0;
//     Array< Array <cmat > > frqChannel;
//     
//     frqChannel.set_length(noOfRBs);
//     for(int j= 0;j<noOfRBs;j++)
//     {
//         frqChannel(j).set_length(noRePerRB);
//         for (int i=0;i<noRePerRB;i++)
//         {
//             frqChannel(j)(i).set_size(rxAntennaCount,txAntennaCount);
//         }
//     }
//     
//     for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
//     {
//         for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
//         {
//             //       cout<<"beamId:"<<beamId<<endl;
//             //       cout<<"txAntenna_cnt:"<<txAntenna_cnt<<endl;
//             //       cout<<"rxAntenna_cnt:"<<rxAntenna_cnt<<endl;
//             //       cout<<"channel:"<<channel<<endl;
//             
//             channelTapGains = channel(beamId)(txAntenna_cnt)(rxAntenna_cnt);
//             //       cout<<"channel:"<<channel(beamId).length()<<endl;
//             //       cout<<"channelTapGains:"<<channelTapGains<<endl;
//             //       cout <<"channelTapGains:"<<channelTapGains<<endl;
//             //       cout <<"channelTapGains:"<<channelTapGains.length()<<endl;
//             
//             cvec sampleSpacedChannel=zeros_c(floor_i(tapDelay(tapDelay.length()-1)*samplingFreqToUse)+1);
//             
//             int channelIndx=0;
//             if(tapDelay.length() != channelTapGains.length())
//             {
//                 abort();
//             }
//             for(int tap_cnt=0;tap_cnt< channelTapGains.length();tap_cnt++)
//             {
//                 channelIndx=floor_i(tapDelay(tap_cnt)*samplingFreqToUse);
//                 if(channelIndx>=sampleSpacedChannel.length())
//                 {
//                     cout<<"[both:] Insufficient Channel length to represent the Channel Model at the given sampling Frequency."<<endl;
//                     abort();
//                 }    
//                 sampleSpacedChannel(channelIndx) = sampleSpacedChannel(channelIndx) + channelTapGains(tap_cnt);  
//                 // 	cout<<"sampleSpacedChannel:"<<sampleSpacedChannel.length()<<endl;
//                 // 	cout<<"sampleSpacedChannel:"<<sampleSpacedChannel<<endl;
//             }
//             //       cout<<"sampleSpacedChannel:"<<sampleSpacedChannel<<endl;
//             frqDomainChannel=fft(sampleSpacedChannel,fftsize); 
//             frqDomainChannel=fftShift(frqDomainChannel);
//             
//             //             cout<<"frqDomainChannel:"<<frqDomainChannel.length()<<endl;
//             
//             for(int j= 0;j<noOfRBs;j++)
//             {
//                 for (int i=0;i<noRePerRB;i++)
//                 {	  
//                     frqChannel(j)(i)(rxAntenna_cnt,txAntenna_cnt) = frqDomainChannel(i+212+j*noRePerRB);
//                 }
//             }
//         }
//     }  
//     //   cout<<"frqChannelLength:"<<frqChannel.length()<<frqChannel(0).length()<<frqChannel(0)(0).rows()<<frqChannel(0)(0).cols()<<endl;
//     // Singular value decomposition
//     Array <vec > sigularValuesTemp;
//     vec singularValues,temp;
//     cmat svdMat;
//     for(int j= 0;j<noOfRBs;j++)
//     {
//         svdMat = zeros_c(rxAntennaCount,rxAntennaCount);
//         for (int i=0;i<noRePerRB;i++)
//         {
//             //       cout<<"frqChannel(j)(i):"<<frqChannel(j)(i)<<endl;
//             svdMat += frqChannel(j)(i)*hermitian_transpose(frqChannel(j)(i));
//         }
//         temp = svd(svdMat);
//         //     cout<<"svdMat:"<<svdMat<<endl;
//         //     cout<<"temp:"<<temp<<endl;
//         append(singularValues,10*log10(temp/noRePerRB));
//         //     cout<<"singularValues:"<<singularValues<<endl;  abort();
//     }
//     
//     return singularValues;
// }

// double getAngleSpread(vec tapPow,Array<vec>& angle,double& mu,SCM5GLink* scmLink,bool isZenith)
// {
//     int nTaps = scmLink->tapPow5GChannel.length();
//     ivec aMap = scmLink->aMap5G;
//     double AS = 0.0;
//     mu = 0.0;
//     ivec idx;
//     for(int tap_cnt=0;tap_cnt<nTaps;tap_cnt++)
//     {
//         idx = scmLink->get5GSubPathIndicesofClusters(scmLink->SubPathtypeinCluster5G(tap_cnt));
//         mu += mean(angle(aMap(tap_cnt))(idx))*tapPow(tap_cnt); 
//     }
//     mu = mu/sum(tapPow);
//     
//     for(int i=0;i<nTaps;i++)
//     {
//         double temp = 0.0;
//         idx = scmLink->get5GSubPathIndicesofClusters(scmLink->SubPathtypeinCluster5G(i));
//         for(int j=0;j<idx.length();j++)
//         {
//             double theta = angle(aMap(i))(idx(j))-mu;
//             if(isZenith)
//             {
//                 while(theta<-180)
//                     theta += 360;
//                 while(theta>180)
//                     theta =360-theta;
//             }
//             temp += sqr(theta);
//         }
//         AS += temp*tapPow(i)/idx.length();
//     }
//     AS = sqrt(AS/sum(tapPow));
//     return AS;
// }

// void printAngleSpread(ChannelCloud* channelCloud,AssociationInfo_S& associationInfo,string fileName)
// {
//     string tempFileName = fileName +".temp"+toString(currentTaskid);
//     ofstream op(tempFileName.c_str(),std::ios::out);
//     if(currentTaskid==0)
//         op<<"%srvcNode,\tassociatedSrvrNode,\tmuAOD,\tsigmaAOD,\tmuZOD,\tsigmaZOD,\tmuAOA,\tsigmaAOA,\tmuZOA,\tsigmaZOA"<<endl;
//     for(int srvc_cnt=0;srvc_cnt<associationInfo.serviceNodesPerTask(currentTaskid).length();srvc_cnt++)
//     {
//         int srvcNodeID = associationInfo.serviceNodesPerTask(currentTaskid)(srvc_cnt);
//         int srvcNodeIndx = find(associationInfo.serviceNodes,srvcNodeID);
//         int srvrNodeID = associationInfo.associatedServerNodesForEachServiceNode(srvcNodeIndx);
//         sBeamPairLink beamPair = associationInfo.associatedBeamPairLinkforEachServiceNode(srvcNodeIndx);
//         int txOrientationID = channelCloud->nodesInfo.getOrientationID(srvrNodeID,beamPair.txPanelID);
//         int rxOrientationID = channelCloud->nodesInfo.getOrientationID(srvcNodeID,beamPair.rxPanelID);
//         int scmLinkIndx = channelCloud->getScmLinkIndx(srvcNodeID,srvrNodeID);
//         
//         SCM5GLink* scmLink = channelCloud->scmLinks(scmLinkIndx).scm5GLink;
//         if(scmLink->is5GChannelInitialized())
//         {
//             ivec aMap = scmLink->aMap5G;
//             vec tapPow = scmLink->tapPow5GChannel;
// //             if(scmLink->scmLinkCondition == _LOS_)
// //             {
// //                 double losPower = (scmLink->K_rice_5G)/(scmLink->K_rice_5G+1);
// //                 tapPow = tapPow/(scmLink->K_rice_5G+1);
// //                 ivec subClustersOf0thCluster = find(aMap==0);
// //                 for(int i=0;i<subClustersOf0thCluster.length();i++)
// //                 {
// //                     tapPow(subClustersOf0thCluster(i)) += losPower*scmLink->get5GSubPathIndicesofClusters(scmLink->SubPathtypeinCluster5G(subClustersOf0thCluster(i))).length()/20;
// //                 }
// //             }
//             double muAOD,muAOA,muZOD,muZOA;
//             double sigmaAOD = getAngleSpread(tapPow,scmLink->subpath_Boresight_5GAoD_angle(txOrientationID),muAOD,scmLink,1);
//             double sigmaZOD = getAngleSpread(tapPow,scmLink->subpath_Boresight_5GZoD_angle(txOrientationID),muZOD,scmLink,1);
//             double sigmaAOA = getAngleSpread(tapPow,scmLink->subpath_Boresight_5GAoA_angle(rxOrientationID),muAOA,scmLink,1);
//             double sigmaZOA = getAngleSpread(tapPow,scmLink->subpath_Boresight_5GZoA_angle(rxOrientationID),muZOA,scmLink,1);
//             
//             op<<srvcNodeID<<",\t"<<srvrNodeID<<",\t"<<muAOD<<",\t"<<sigmaAOD<<",\t"<<muZOD<<",\t"<<sigmaZOD<<",\t"<<muAOA<<",\t"<<sigmaAOA<<",\t"<<muZOA<<",\t"<<sigmaZOA<<endl;
//         }
//         else
//         {
//             return;
//         }
//     }
//     op.close();
// }



Array<vec> computeSingularValuesPerRB(Array <Array <cvec>>& channel, vec tapDelay,NUParams_S* nuParams,int txAntennaCount,int rxAntennaCount)
{
    int nREsPerRB = nuParams->subCarrierCountPerRB;
    double samplingFrequencyInHz = nuParams->samplingFrequencyInHz;
    
    int nRBs = nuParams->dlNRBs;
    int fftSize = nuParams->fftSize;
    int guardSubcarriersInOneSide = (fftSize - nRBs*nREsPerRB)/2;
    
    cvec channelTapGains;
    cvec frqDomainChannel;
    
    Array< Array <cmat > > frqChannel;
    frqChannel.set_length(nRBs);
    for(int j= 0;j<nRBs;j++)
    {
        frqChannel(j).set_length(nREsPerRB);
        for (int i=0;i<nREsPerRB;i++)
        {
            frqChannel(j)(i).set_size(rxAntennaCount,txAntennaCount);
        }
    }
    for(int txAntCnt=0;txAntCnt<txAntennaCount;txAntCnt++)
    {
        for(int rxAntCnt=0;rxAntCnt<rxAntennaCount;rxAntCnt++)
        {
            channelTapGains = channel(txAntCnt)(rxAntCnt);
            
            cvec sampleSpacedChannel=zeros_c(floor_i(tapDelay(tapDelay.length()-1)*samplingFrequencyInHz)+1);
            
            int channelIndx=0;
            if(tapDelay.length() != channelTapGains.length())
            {
                abort();
            }
            for(int tap_cnt=0;tap_cnt< channelTapGains.length();tap_cnt++)
            {
                channelIndx=floor_i(tapDelay(tap_cnt)*samplingFrequencyInHz);
                if(channelIndx>=sampleSpacedChannel.length())
                {
                    cout<<"[both:] Insufficient Channel length to represent the Channel Model at the given sampling Frequency."<<endl;
                    abort();
                }    
                sampleSpacedChannel(channelIndx) = sampleSpacedChannel(channelIndx) + channelTapGains(tap_cnt);
            }
            frqDomainChannel=fft(sampleSpacedChannel,fftSize); 
            frqDomainChannel=fftShift(frqDomainChannel);
            
            for(int j=0;j<nRBs;j++)
            {
                for (int i=0;i<nREsPerRB;i++)
                {	  
                    frqChannel(j)(i)(rxAntCnt,txAntCnt) = frqDomainChannel(i+guardSubcarriersInOneSide+j*nREsPerRB);
                }
            }
        }
    }
    Array<vec> SVs(nRBs);
    vec singularValues = zeros(rxAntennaCount);
    cmat svdMat= zeros_c(rxAntennaCount,rxAntennaCount);
    for(int j= 0;j<nRBs;j++)
    {
        svdMat= zeros_c(rxAntennaCount,rxAntennaCount);
        for (int i=0;i<nREsPerRB;i++)
        {
            svdMat += frqChannel(j)(i)*frqChannel(j)(i).H();
        }
        vec prbSingularValues = svd(svdMat/nREsPerRB);
        append(SVs(j),dB(prbSingularValues));
    }
    return SVs;
}




/*
void performBeamBasedAssociation(McellSystem& mySys, AssociationInfo_S& associationInfo, int nRBs, string linkTableFileName,ResultFileNames_S& resultFiles)
    vec serverNodeTransmitPowerInDBm; 
    if(parse(linkTableFileName,"serverNodeTransmitPowerInDBm",serverNodeTransmitPowerInDBm)==false)
    {
        cout<<"[both:]Error... Unable to load serverNodeTransmitPowerInDBm from the file in performBeamBasedAssociation()..."<<endl;	abort();
    }
    
    // FIXME txSubcarrierPowerindBm is not used for SINR computation, it is not correct for pico simulations where the TxPower is different
    //   int nSubcarriers =  nRBs * 12;
    //   vec txSubcarrierPowerindBm = serverNodeTransmitPowerInDBm - 10*log10(nSubcarriers);
    
    ivec nTxPanelsPerServerNode(0), nTxBeamsPerServerNode(0);
    int nRxPanels = -1, nRxBeams = -1;
    
    ivec updatedCellIds(associationInfo.serviceNodes.length());
    ivec initCellIds = associationInfo.associatedServerNodesForEachServiceNode;
    Array< Array< vec> > RSRPdesired;
    RSRPdesired.set_length(associationInfo.serviceNodes.length());
    
    printLinkGain(resultFiles.linkGainFileName,associationInfo.serviceNodes,associationInfo.serverNodes,mySys.channelCloud);

    std::ofstream fout;
    bool toPrintBeamRSRP = traceValuePerTraceName["EnableBeamInfo"]=="true";
    if(toPrintBeamRSRP)
    {
        fout.open(resultFiles.beamRSRPfileName.c_str(),std::ios::out);
        fout<<"%ServiceNode,\t ServerNode,\t srvrBeamID,\t srvcBeamID,\t TxPanelID,\t RxPanelID,\t RSRPindB"<<endl;
    }
    ivec txNodelist = associationInfo.serverNodes;
    
    Array < Array <Array< Array< mat > > > > RSRPwithRandomInterfererBeams(associationInfo.serviceNodes.length()); //srvcNode, srvrNode,TxOrientation,RxOrientation, for Matrix RxBeams,TxBeams
    int nTrials=1;
    
    for(int rx_cnt=0; rx_cnt<associationInfo.serviceNodes.length(); rx_cnt++)
    {     
        ChannelMatrix_S channelMatrix = mySys.generateChannelForAnalogBeams(associationInfo.serviceNodes(rx_cnt),0,txNodelist,nTxPanelsPerServerNode,nTxBeamsPerServerNode,nRxPanels,nRxBeams);
        
        RSRPwithRandomInterfererBeams(rx_cnt).set_length(txNodelist.length());
        
        // RSRP calculation for Port 0 -> assuption is Port 0 is Tx antenna 0
        vec RSRP = zeros(channelMatrix.txNodeIDs.length());
        RSRPdesired(rx_cnt).set_length(txNodelist.length());
        for(int srvrCnt = 0; srvrCnt <txNodelist.length();srvrCnt++)
        {
            RSRPwithRandomInterfererBeams(rx_cnt)(srvrCnt).set_length(nTxPanelsPerServerNode(srvrCnt));
            for(int txOrient_cnt=0 ; txOrient_cnt < nTxPanelsPerServerNode(srvrCnt); txOrient_cnt++)
            {
                RSRPwithRandomInterfererBeams(rx_cnt)(srvrCnt)(txOrient_cnt).set_length(nRxPanels);
                
                for(int rxOrient_cnt=0; rxOrient_cnt < nRxPanels; rxOrient_cnt++)
                {
                    RSRPwithRandomInterfererBeams(rx_cnt)(srvrCnt)(txOrient_cnt)(rxOrient_cnt) = zeros(nRxBeams,nTxBeamsPerServerNode(srvrCnt));
                }
            }
            RSRPdesired(rx_cnt)(srvrCnt)=zeros(nTrials);
        }
        
        for(int txCnt = 0; txCnt <channelMatrix.txNodeIDs.length();txCnt++)
        {
            int nodeID = channelMatrix.txNodeIDs(txCnt)/1e8;
            sBeamPairLink tempBeamPairLink;
            decodeBeamPairLinkIDs(channelMatrix.txNodeIDs(txCnt),tempBeamPairLink);
            int txPanelID = tempBeamPairLink.txPanelID;
            int txBeamID = tempBeamPairLink.srvrBeamID;
            int rxPanelID = tempBeamPairLink.rxPanelID;
            int rxBeamID = tempBeamPairLink.srvcBeamID;
            
            int txAntNo = 0;
            for(int rxAntCnt = 0;rxAntCnt <channelMatrix.channels_forRSRP(txCnt)(txAntNo).length();rxAntCnt ++)
            {
                double tempAvgTap = 0.0;
                for(int tapCnt = 0;tapCnt <channelMatrix.channels_forRSRP(txCnt)(txAntNo)(rxAntCnt).length();tapCnt ++)
                {
                    tempAvgTap += sqr(abs(channelMatrix.channels_forRSRP(txCnt)(txAntNo)(rxAntCnt)(tapCnt)));
                }
                RSRP(txCnt) += tempAvgTap;
            }
            RSRP(txCnt) /= channelMatrix.channels_forRSRP(txCnt)(txAntNo).length();
            // converting into dB
            RSRP(txCnt) = dB(abs(RSRP(txCnt)));
            
            if(toPrintBeamRSRP)
                fout<<associationInfo.serviceNodes(rx_cnt)<<",\t "<<nodeID<<",\t "<<txBeamID<<",\t "<<rxBeamID<<",\t"<<txPanelID<<",\t"<<rxPanelID<<",\t "<<RSRP(txCnt)<<endl;
            
            RSRPwithRandomInterfererBeams(rx_cnt)(find(txNodelist,nodeID))(txPanelID)(rxPanelID)(rxBeamID,txBeamID) = RSRP(txCnt);
        }
        int maxIndx = max_index(RSRP);
        updatedCellIds(rx_cnt) = channelMatrix.txNodeIDs(maxIndx)/1e8; // "txNodeID + txO (2 digit) + txBeamID (2 digit) + rxO (2 digit) + rxBeamID (2 digit)
        int associatedTxIndx = find(txNodelist,updatedCellIds(rx_cnt));
        RSRPdesired(rx_cnt)(associatedTxIndx) = RSRP(maxIndx);
        associationInfo.reassociate(associationInfo.serviceNodes(rx_cnt),updatedCellIds(rx_cnt));
        
        decodeBeamPairLinkIDs(channelMatrix.txNodeIDs(maxIndx),associationInfo.associatedBeamPairLinkforEachServiceNode(rx_cnt));
        
        associationInfo.associatedBeamPairLinkforEachServiceNode(rx_cnt).rsrpIndBm=RSRP(maxIndx);
        associationInfo.associatedBeamPairLinkforEachServiceNode(rx_cnt).rspIndBm=0; // need to set app value - Dhiv
        
        mat tempInterferenceRSRP;
        tempInterferenceRSRP.set_size(1,1);
        
    }
    if(toPrintBeamRSRP)
    {
        fout.close();
        cout<<"Beam RSRP printed to "<<resultFiles.beamRSRPfileName<<endl;
    }
    for(int rx_cnt=0; rx_cnt<associationInfo.serviceNodes.length(); rx_cnt++)
    {
        
        ivec interfererIndices = findNot(txNodelist,updatedCellIds(rx_cnt));
        
        for(int interfererCnt = 0; interfererCnt <interfererIndices.length();interfererCnt++)
        {
            int nodeIndx = interfererIndices(interfererCnt);
            int rxBeamID = associationInfo.associatedBeamPairLinkforEachServiceNode(rx_cnt).srvcBeamID;
            int rxPanelID = associationInfo.associatedBeamPairLinkforEachServiceNode(rx_cnt).rxPanelID;
            for(int trial_cnt=0;trial_cnt<nTrials;trial_cnt++)
            {
                //Picking a random UE and his corresponding beam
                // 	int randomScheduleUE = randPick(associationInfo.associatedServiceNodesPerServerNode(interfererIndices(interfererCnt)),1)(0);
                // 	int rndSchUEIndx=find(associationInfo.serviceNodes,randomScheduleUE);
                // 	int randomTxBeamID=associationInfo.associatedBeamPairLinkforEachServiceNode(rndSchUEIndx).txBeamID;
                int randomTxBeamID=randi(0,nTxBeamsPerServerNode(nodeIndx)-1);
                int randomTxPanelID=randi(0,nTxPanelsPerServerNode(nodeIndx)-1);
                RSRPdesired(rx_cnt)(interfererIndices(interfererCnt))(trial_cnt) = RSRPwithRandomInterfererBeams(rx_cnt)(nodeIndx)(randomTxPanelID)(rxPanelID)(rxBeamID,randomTxBeamID);
            }
        }
    }  
    //   cout<<"RSRP in beam sweeping : "<<RSRPdesired<<endl;  
    //   cout<<"initial servernodes = "<<initCellIds<<endl;
    //   cout<<"Reassoc servernodes = "<<updatedCellIds<<endl;
    //   cout<<"Changed num = "<<findNot((initCellIds-updatedCellIds),0).length()<<endl;
    
    printReAssociation(associationInfo,linkTableFileName,RSRPdesired,resultFiles.rsrpDistributionFileName,resultFiles.dlRSRPcouplingFileName);
    //   cout<<"RSRPdesired:"<<RSRPdesired<<endl;
    
    //   string tempFileRSRP="./Results/RSRPFile.txt";
    //   std::ofstream fout1(tempFileRSRP.c_str(), std::ios::out);
    //   mat varRSRPmat(RSRPdesired.length(),RSRPdesired(0).length());  
    //   for(int i=0;i<RSRPdesired.length();i++)
    //   {
    //     varRSRPmat.set_row(i,RSRPdesired(i));
    //   }
    //   fout1<<varRSRPmat;
    //   fout1.close();
}*/
// void performLinkSpecificAntGainAssociation(McellSystem& mySys, AssociationInfo_S& associationInfo, string linkTableFileName, Array <ivec >  &strongServerNodesPerServiceNode)
// {
//  
//   ivec updatedCellIds(associationInfo.serviceNodes.length());
//   ivec initCellIds = associationInfo.associatedServerNodesForEachServiceNode;
//   
//   Array< vec> RSRP;
//   RSRP.set_length(associationInfo.serviceNodes.length());
//   
//   mat linkGain = mySys.channelCloud.getLinkSpecificChannelGainInDB(associationInfo.serverNodes,associationInfo.serviceNodes,true);
//   string tempFileName="./Results/linkGainFile.txt";
//   std::ofstream fout(tempFileName.c_str(), std::ios::out);
//   fout<<linkGain;
//   fout.close();
//   
//   for(int rx_cnt=0; rx_cnt<associationInfo.serviceNodes.length(); rx_cnt++)
//   {     
//     
//     // RSRP calculation for Port 0 -> assuption is Port 0 is Tx antenna 0
//     RSRP(rx_cnt)=zeros(associationInfo.serverNodes.length());
//     
//     for(int txCnt = 0; txCnt <associationInfo.serverNodes.length();txCnt++)
//     {
// 
//       RSRP(rx_cnt)(txCnt) = linkGain(rx_cnt,txCnt);
// 
//     }
//     
//     int maxIndx = max_index(RSRP(rx_cnt));
//     updatedCellIds(rx_cnt) = associationInfo.serverNodes(maxIndx);
//     associationInfo.reassociate(associationInfo.serviceNodes(rx_cnt),updatedCellIds(rx_cnt));
//   }
//   
//   cout<<"RSRP  : "<<RSRP<<endl;
//   
//   
//   cout<<"initial servernodes= "<<initCellIds<<endl;
//   cout<<"Reassoc servernodes= "<<updatedCellIds<<endl;
//   cout<<"changed num= "<<findNot((initCellIds-updatedCellIds),0).length()<<endl;
//   
//   printReAssociation(associationInfo,linkTableFileName,RSRP);
//  
// 
//   
// }

/*
void performBeamBasedAssociationOld(McellSystem& mySys, AssociationInfo_S& associationInfo, string linkTableFileName)
{
  
  vec serverNodeTransmitPowerInDBm; 
  if(parse(linkTableFileName,"serverNodeTransmitPowerInDBm",serverNodeTransmitPowerInDBm)==false)
  {
    cout<<"[both:]Error... Unable to load serverNodeTransmitPowerInDBm from the file in performBeamBasedAssociation()..."<<endl;	abort();
  }
  
  // Harcodings as SSB
  vec txSubcarrierPowerindBm = serverNodeTransmitPowerInDBm - 10*log10(50*12);
  int fftsize = 1024;
  double subframeDuration = 1e-3;
  double subcarrierSpacing = 15e3;
  double avgWindow = 200;
  
  ivec updatedCellIds(associationInfo.serviceNodes.length());
  ivec initCellIds = associationInfo.associatedServerNodesForEachServiceNode;
  Array<PerCellL1Measurements> servingCellL1Measurements(associationInfo.serviceNodes.length());
  
  associationInfo.associatedBeamPairLinkforEachServiceNode.set_length(associationInfo.serviceNodes.length());
  
  ivec portLocations = getIntegers(0,fftsize-1,6);
  
  std::ofstream fout("./Results/BeamMeasurement.txt",std::ios::out);
  
  for(int rx_cnt=0; rx_cnt<associationInfo.serviceNodes.length(); rx_cnt++)
  {
    Array<PerCellL1Measurements> measurements;  
    measurements.set_length(0);
    ivec beamIDs(0);
    
    for(int subFrm_cnt=0; subFrm_cnt<avgWindow; subFrm_cnt+=40)
    { 
      double samplingFrequencyInHz = (double)fftsize*subcarrierSpacing;
      double currentTime=getCurrentTime(subFrm_cnt,0,subframeDuration,fftsize,samplingFrequencyInHz);// Assuming all ports are in the same ofdm symbols
      
      ChannelMatrix_S channelMatrix = mySys.generateChannelForAnalogBeams(associationInfo.serviceNodes(rx_cnt),0, associationInfo.serverNodes);
      
      if(measurements.length() == 0)
      {
        measurements.set_length(channelMatrix.txNodeIDs.length());
        for(int cnt=0; cnt<channelMatrix.txNodeIDs.length(); cnt++)
          measurements(cnt).phyCellId=channelMatrix.txNodeIDs(cnt);
        beamIDs = channelMatrix.txNodeIDs;
      }
      
      getIntialMeasurements(channelMatrix.txNodeIDs,channelMatrix,subFrm_cnt,portLocations,associationInfo,txSubcarrierPowerindBm, measurements);
    }
    
    vec l1Rsrp(0);
    for(int indx = 0; indx < beamIDs.length(); indx++)
    {
      measurements(indx).rsrp_l1Avg = dB(sum(inv_dB(measurements(indx).rsrp))/measurements(indx).rsrp.length());
      
      fout<<associationInfo.serviceNodes(rx_cnt)<<",\t"<<beamIDs(indx)<<",\t"<< measurements(indx).rsrp_l1Avg<<endl;
      
      append(l1Rsrp,measurements(indx).rsrp_l1Avg);
    }
    
    cout<<"RSRP = "<<l1Rsrp<<endl;
    int maxIndx = max_index(l1Rsrp);
    updatedCellIds(rx_cnt) = beamIDs(maxIndx)/10000;
    servingCellL1Measurements(rx_cnt) = measurements(maxIndx);
    associationInfo.reassociate(associationInfo.serviceNodes(rx_cnt),updatedCellIds(rx_cnt));
    associationInfo.associatedBeamPairLinkforEachServiceNode(rx_cnt).txBeamID = mod(beamIDs(maxIndx),10000)/100;
    associationInfo.associatedBeamPairLinkforEachServiceNode(rx_cnt).rxBeamID = mod(beamIDs(maxIndx),100);
  }
  
  
  cout<<"initial servernodes= "<<initCellIds<<endl;
  cout<<"Reassoc servernodes= "<<updatedCellIds<<endl;
  cout<<"changed num= "<<findNot((initCellIds-updatedCellIds),0).length()<<endl;
  
  fout.close();
  
  //abort();
}


void getIntialMeasurements(ivec beamIDs,ChannelMatrix_S& channelMat, int subFrameNum,ivec& portLocations, AssociationInfo_S& associationInfo, vec txSubcarrierPowersindBm, Array<PerCellL1Measurements>& measurements)
{
  vec RSRPPerCell, RSRQPerCell,RSSIPerCell ;
  RSRPPerCell=zeros(beamIDs.length());
  RSRQPerCell=zeros(beamIDs.length());
  RSSIPerCell=zeros(beamIDs.length());
  //vec txpowers=txSubcarrierPowersindBm(find(associationInfo.serverNodes,channelMat.txNodeIDs));
  double txSubcarrierPowerInDBm; //=46-10*log10(600);
  for(int cell_cnt=0;cell_cnt<beamIDs.length();cell_cnt++)
  {
    int chnIndx=find(channelMat.txNodeIDs,beamIDs(cell_cnt));
    
    if(chnIndx!=-1)
    {
      txSubcarrierPowerInDBm=txSubcarrierPowersindBm(0)-30; //FIXME Hardcoded txSubcarrierPowerindBm
      
      Array< Array<cmat> > rxChn=channelMat.channels(chnIndx);
      double temp=0;

        for(int tone_cnt=0;tone_cnt<portLocations.length();tone_cnt++)
        {      
          cmat rxChnMatrix=getAcrossArray(rxChn,portLocations(tone_cnt));
          cvec rxChnVec=rxChnMatrix.get_col(0);
          temp+=((sqr(norm(rxChnVec))/rxChnVec.length())*inv_dB(txSubcarrierPowerInDBm));
        }

        RSRPPerCell(cell_cnt)=10*log10(temp/portLocations.length());
    }
    else
    {
      RSRPPerCell(cell_cnt)=-1000;
      RSSIPerCell(cell_cnt)=-1000;
      RSRQPerCell(cell_cnt)=-1000; 
    }
    for(int id=0; id<measurements.length(); id++)
    {
      if(measurements(id).phyCellId==beamIDs(cell_cnt))
      {
        append(measurements(id).rsrp,RSRPPerCell(cell_cnt));
        append(measurements(id).rsrq,RSRQPerCell(cell_cnt));
        append(measurements(id).rssi,RSSIPerCell(cell_cnt));
        measurements(id).lastMeasuredTimeInstant=subFrameNum;
        break;
      }
    }
  } 
  
}


double getCurrentTime(int currentFrame,int currentSymbol,double subframeDuration,int fftSize ,double samplingFrequencyInHz)
{
  int CPlengthForFirstSymbol=160, CPlength=144, TDsamples = 2048;
  int currentSymbolCP;
  double currentTime=currentFrame*subframeDuration;
  for(int sym_cnt=0;sym_cnt<currentSymbol;sym_cnt++)
  {
    if(sym_cnt==0 || sym_cnt==7)
      currentSymbolCP=(int)(CPlengthForFirstSymbol*((double)fftSize/(double)TDsamples));
    else
      currentSymbolCP=(int)(CPlength*((double)fftSize/(double)TDsamples));
    
    currentTime+=(fftSize+currentSymbolCP)*(1/samplingFrequencyInHz);
  }
  return(currentTime);
}
*/



void BWSimSystem_S::initializeSystem(int argc, char** argv)
{
  info();
  
  initializeMcell(argc,argv);
      
  #ifdef USING_MPI
  if(nTasks!=1)
  {
    if(mySysInfo.useFixedSeed==false)
    {
      cout<<"[both:] Simulations with Multi-cores doesn't support random seed... Please run with single core"<<endl;abort();
    }
    if(mySysInfo.enableUserMobility)
    {
      cout<<"[both:] Simulations with Multi-cores doesn't support user mobility... Please run with single core"<<endl;abort();
    }
    if(mySysInfo.linkSimulationInfo.isEnabled)
    {
      cout<<"[both:] Link-Level simulation with Multi-cores doesn't support... Please run with single core"<<endl;abort();
    }
  }
  loadMPIInfo(mySysInfo.mpiInfo,associationInfo);
  #endif
  
  
  #ifdef USING_SRS
  std::ofstream foutSRS("./Results/SRSStats.txt", std::ios::out);
  foutSRS<<"SFN, \t"<<"eNBID," << "\t" << " No.of Users success,"<<"\t"<<"Failure,"<<endl;
  
  BSL3SRS bsL3SRS;
  if(mySysInfo.isSRSEnabled)
      bsL3SRS.intialiseBSSRSModule(associationInfo,mySysInfo.myConfigFiles.linkTableFileName,mySysInfo.myConfigFiles.simulationConfigFileName);
  
  UEL3SRS ueL3SRS;
  if(mySysInfo.isSRSEnabled)
      ueL3SRS.intialiseUESRSModule(associationInfo,mySysInfo.myConfigFiles.simulationConfigFileName,mySysInfo.myConfigFiles.linkTableFileName);
  #elseif
  if(mySysInfo.isSRSEnabled)
  {
    cout<<"[both:] /////////////////////////////////////////////////////////////////////////////////////////////"<<endl;
    cout<<"[both:]Error .. SRS is enabled in the simulation but the Simulator is missing SRS Library ... \nPlease compile the simulator with SRS Support.."<<endl;
    cout<<"[both:] /////////////////////////////////////////////////////////////////////////////////////////////"<<endl;
    abort();
  }
  #endif
  
  
  //! Initializing CoMP sets
//   if(mySysInfo.compInfo.isWithCoMP)
//     initCompSets(mySys.channelCloud,mySysInfo,myCentralUnits);
  
  
  // Initializing NodeDevices ////////////////////////////////////////////////////////////////////////////////////////////////////
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Need to check below line - Dhiv
  ivec carrierFrequenciesinMHz = to_ivec(mySysInfo.carrierInfo.carrierFreqInMHzPerServerNodeType);

  initAllNodeDevice(); //Check Deepak
  
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
//   initializeHigherLayers(*this);
  
  schedulerOutput.set_length(associationInfo.serverNodes.length());
  
  // Initialization of local variables
  ivec nuIDs=mySysInfo.nuIDs;
  ivec nuCaIDs=getNUCaIDs(nuIDs,carrierFrequenciesinMHz);
  
  vec dlTDDScaling=ones(nuIDs.length());
  vec ulTDDScaling=ones(nuIDs.length());
   if(mySysInfo.ulDlMode==_ULDL_MODE_TDD_)
       getTDDScaling(mySysInfo,dlTDDScaling,ulTDDScaling);
  
  if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
  {
      dlPerformanceMetric.init(mySysInfo.serverNodes,mySysInfo.serviceNodes,nuCaIDs,true,dlTDDScaling);
      for(int nu_cnt=0;nu_cnt<mySysInfo.serverNodeNumerologies(0).length();nu_cnt++)
          dlPerformanceMetric.setNUParams(myNodeDevices(0).L1CorePerNU(nu_cnt).nuParameters);
  }
  if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
  {
    ulPerformanceMetric.init(mySysInfo.serverNodes,mySysInfo.serviceNodes,nuCaIDs,false,ulTDDScaling);  
    slPerformanceMetric.init(mySysInfo.serverNodes,mySysInfo.serviceNodes,nuCaIDs,false,ulTDDScaling);  
      for(int nu_cnt=0;nu_cnt<mySysInfo.serverNodeNumerologies(0).length();nu_cnt++)
          ulPerformanceMetric.setNUParams(myNodeDevices(0).L1CorePerNU(nu_cnt).nuParameters);
      
//     downlinkSIRInDBPerServiceNode=getDownLinkSIRInDBForUplinkPowerControl(mySys.channelCloud,associationInfo);
    // Downlink SIR is not properly computed for Target SINR power control
  }
  
  
//   cqiInfoForScheduler.set_length(nCarriersForCA);
//   for(int carrier_cnt=0;carrier_cnt<nCarriersForCA;carrier_cnt++)
//     cqiInfoForScheduler(carrier_cnt).init(mySysInfo.serviceNodes,4,1);
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   
  /// CA 
  
//   if(mySysInfo.hasFeature("isWithABS"))
//     ABSparameters.set_length(nCarriersForCA);
  
  srvrNodesToConsiderForScheduling=associationInfo.serverNodes;
  
  #ifdef USING_MPI
  filterServerNodesForMPI(srvrNodesToConsiderForScheduling,associationInfo,mySysInfo);
  #endif
  
  // Assuming same RBs for all CUs
//    if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
//     mySysInfo.FFR_RBGScaling=initializeFFR(mySysInfo.myConfigFiles.simulationConfigFileName, myCentralUnits(0).resourceMapper.dlResourceMapper.getResourceBlockGroupingSize(), myCentralUnits(0).resourceMapper.dlResourceMapper.getResourceBlocksPerSubframe());
//   
  ivec txNodes, rxNodes, subframe1=-ones_i(mySysInfo.serviceNodes.length());
  
//   if(mySysInfo.cchInfo.enablePssSss && mySysInfo.isControlChannelEnabled)
//     cellSearch(mySys,associationInfo,mySysInfo,myCentralUnits,myNodeDevices,nodeDeviceIDs,myCentralUnits(0).getFrameSize(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_));
//   
  if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
    linkGainInDB=mySys.channelCloud.getLinkSpecificChannelGainInDB(mySysInfo.serviceNodes,mySysInfo.serverNodes,mySys.channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_);
   if(mySysInfo.linkSimulationInfo.isEnabled) snrLoopCount=mySysInfo.linkSimulationInfo.snrRangeInDB.length();
   else  snrLoopCount=1;
   
   
//   if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
//   {
//     powerHeadroomNodeIDs=associationInfo.serviceNodes;
//     powerHeadroomInDBPerNode = -1*ones(associationInfo.serviceNodes.length());
//     powerHeadroomInDBPerNode_subframeCount = -1*ones_i(associationInfo.serviceNodes.length());
//   }
}

void BWSimSystem_S::initializeMcell(int argc, char** argv)
{
    info();
    
    mySysInfo.init(argc,argv);
    
    MCell mcellReturn = createMcellNetwork(mySysInfo.myConfigFiles,mySysInfo.resultFiles);
    mySys = mcellReturn.mySys;
    associationInfo = mcellReturn.associationInfo;
        
    mySysInfo.parseNodesInfo();
    
    bool isForCalibration=false;
    parse(mySysInfo.myConfigFiles.simulationConfigFileName,"isForCalibration",isForCalibration);
    
    if((argc>1 && strcmp(argv[1],"mcell")==0) || isForCalibration)
    {
        if(currentTaskid==0)
        {
            cout<<"Mcell Channel Generation Completed.."<<endl;
            completeSimulation(mySysInfo.myConfigFiles.directory,mySysInfo.resultFiles.directory,0);
        }
        
        #ifdef USING_MPI
        MPI_Finalize();
        #endif
        exit(0);
    }
    
    // Seed set again here as Channelcloud resets seed during SCM initialization -  Dhiv
    if(mySysInfo.useFixedSeed) 
        setSeed(mySysInfo.simulationSeed);
    
}

void nodeUpdate(BWSimSystem_S& bwsimSystem, ivec serverNodes, int nuCaID)
{
  if(serverNodes.length()==0)
    serverNodes=bwsimSystem.associationInfo.serverNodes;
  
  //int nCarriersForCA = bwsimSystem.mySysInfo.carrierAggregation.getNumberOfCarriersForCA();
  
  ivec deviceIndx=find(bwsimSystem.nodeDeviceIDs,serverNodes);  
  ivec srvrNodeIndx=find(bwsimSystem.associationInfo.serverNodes,serverNodes);
 
  for(int srvr_cnt=0;srvr_cnt<serverNodes.length();srvr_cnt++)
  {
    //if(bwsimSystem.carrier_cnt == 0)
    {
      int nuIndx=bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).getNUIndx(nuCaID);
      if(bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).actualLoopCountPerNU(nuIndx)!=-1)
      {
	if(bwsimSystem.mySysInfo.ulDlMode==_ULDL_MODE_FDD_)
	{
	  if(bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).actualLoopCountPerNU(nuIndx)%2)
	    bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).ttiCountPerNU(nuIndx)++;
	}
	else if(bwsimSystem.mySysInfo.ulDlMode==_ULDL_MODE_TDD_)
	{
	  int frameConfig=bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx(bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).actualLoopCountPerNU(nuIndx)%bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx.length());
	  
        if(frameConfig!=21) // Here frameConfig is assumed to be the same for all serverNodes (will create issue for Dynamic/Mixed TDD)
            bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).ttiCountPerNU(nuIndx)++;
      
	}
	else
	  bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).ttiCountPerNU(nuIndx)++;
	
	bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).actualLoopCountPerNU(nuIndx)++;
      }
      else
      {
	bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).ttiCountPerNU(nuIndx)++;
	bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).actualLoopCountPerNU(nuIndx)++;
      }
      
     ivec tassociatesrvcnode= bwsimSystem.associationInfo.associatedServiceNodesPerServerNode(srvrNodeIndx(srvr_cnt));
     ivec srvcDeviceIndx=find(bwsimSystem.nodeDeviceIDs,tassociatesrvcnode);
     for(int srvc_cnt =0; srvc_cnt < tassociatesrvcnode.length(); srvc_cnt++)
     {
       int ueNuIndx=bwsimSystem.myNodeDevices(srvcDeviceIndx(srvc_cnt)).getNUIndx(nuCaID);
       if(ueNuIndx!=-1)
       {
	bwsimSystem.myNodeDevices(srvcDeviceIndx(srvc_cnt)).ttiCountPerNU(ueNuIndx) = bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).ttiCountPerNU(nuIndx);
	bwsimSystem.myNodeDevices(srvcDeviceIndx(srvc_cnt)).actualLoopCountPerNU(ueNuIndx) = bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).actualLoopCountPerNU(nuIndx);
       }
	 
      }
    }
  //}
  // User Mobility Modelling
// 	if(bwsimSystem.mySysInfo.enableUserMobility)
// 	{
// 	  for(int srvr_cnt=0;srvr_cnt<serverNodes.length();srvr_cnt++)
// 	    moveUserNodes(bwsimSystem.mySys,bwsimSystem.mySysInfo,bwsimSystem.associationInfo,bwsimSystem.myCentralUnits,bwsimSystem.myNodeDevices,bwsimSystem.nodeDeviceIDs,bwsimSystem.cqiInfoForScheduler,(double)bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).ttiCountPerNU(nuIndx)*bwsimSystem.mySysInfo.subFrameDuration(srvr_cnt),bwsimSystem.associationInfo.associatedServiceNodesPerServerNode(find(bwsimSystem.associationInfo.serverNodes,serverNodes(srvr_cnt))));
// 	}


  //!\brief separating uplink and downlink server nodes
  UlDlServerNodes_S uldlServerNodes;
  int frameConfig=-1; // 1=>DL , 0=>UL, 21=>Dwpts , 20=>Uppts 
  for(int srvr_cnt=0;srvr_cnt<serverNodes.length();srvr_cnt++)
  {
    int nuIndx=bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).getNUIndx(nuCaID);
    bool isDownlink=false;
    frameConfig=bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx(bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).actualLoopCountPerNU(nuIndx)%bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx.length());
    if(frameConfig%2==1) {isDownlink=true;} 
    
    if(isDownlink)
      append(uldlServerNodes.dlServerNodes,serverNodes(srvr_cnt));
    else
      append(uldlServerNodes.ulServerNodes,serverNodes(srvr_cnt));	 
  }

  ivec actualLoopCount=getLoopCount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,serverNodes,nuCaID);
  ivec ttiCount=getTTICount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,serverNodes,nuCaID);
  
  if(bwsimSystem.mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
  {
    for(int srvr_cnt=0;srvr_cnt<serverNodes.length();srvr_cnt++)
    {
//       int nuIndx=bwsimSystem.myNodeDevices(deviceIndx(srvr_cnt)).getNUIndx(nuID);
      ivec srvcNodeDeviceIndx=find(bwsimSystem.nodeDeviceIDs,bwsimSystem.associationInfo.associatedServiceNodesPerServerNode(srvrNodeIndx(srvr_cnt)));
      //segreagate srvc node devices w.r.t numerlogy.
      ivec srvcNodedeviceinx;
       for(int src=0; src < srvcNodeDeviceIndx.length(); src++)
       {
	 if(find(bwsimSystem.myNodeDevices(srvcNodeDeviceIndx(src)).nuIDs,nuCaID) != -1)
	 {
		   append(srvcNodedeviceinx,srvcNodeDeviceIndx(src));   
	  }
      }  
      srvcNodeDeviceIndx.clear();;
      srvcNodeDeviceIndx= srvcNodedeviceinx;   
      for(int srvcNode_cnt=0;srvcNode_cnt<srvcNodeDeviceIndx.length();srvcNode_cnt++)
      {
	int nuIndx=bwsimSystem.myNodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).getNUIndx(nuCaID);
	if(bwsimSystem.myNodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).receiverPerNU(nuIndx).isLinkAdaptationEnabled())
	{
	  int nDLRBs =bwsimSystem.myNodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getResourceBlocksPerSubframe();
	  bool isAperiodicCQICompEnabled = bwsimSystem.myNodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).L1CorePerNU(nuIndx).dlRxConfig.dlCQIModeInfo.isAperiodicCQICompEnabled;
	  bwsimSystem.myNodeDevices(srvcNodeDeviceIndx(srvcNode_cnt)).receiverPerNU(nuIndx).cqiEstimator.ManageDLCQIReportTrigger(nDLRBs,ttiCount(srvr_cnt),isAperiodicCQICompEnabled);									
	}
      }
    }
  }
  
}


// void nodeScheduleData(BWSimSystem_S& bwsimSystem, ivec serverNodes, int nuID)
// {
//    if(serverNodes.length()==0)
//     serverNodes=bwsimSystem.associationInfo.serverNodes;
//  
//    ivec deviceIndices=find(bwsimSystem.nodeDeviceIDs,serverNodes);  
//    for(int srvr_cnt=0;srvr_cnt<serverNodes.size();srvr_cnt++)
//    {
//     #ifdef USING_L2
//     if(bwsimSystem.lteL2sim.tL2SimConfig.isTCPEnabled)
//     {
//       int nuIndx=bwsimSystem.myNodeDevices(deviceIndices(srvr_cnt)).getNUIndx(nuID);
//       bwsimSystem.lteL2sim.fInitTCPConnectionSubframe(bwsimSystem.myNodeDevices(deviceIndices(srvr_cnt)).ttiCountPerNU(nuIndx),bwsimSystem.associationInfo.associatedServiceNodesPerServerNode(find(bwsimSystem.associationInfo.serverNodes,serverNodes(srvr_cnt))));
//     }
//     #endif
//    }
//       
//   ivec ttiCountPerServerNode=getTTICount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,serverNodes,nuID);
//   ivec loopCountPerServerNode=getLoopCount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,serverNodes,nuID);
//   
//   UlDlServerNodes_S uldlServerNodes;
//     
//   if(bwsimSystem.mySysInfo.ulDlMode==_ULDL_MODE_TDD_)
//   {
//     SpecialSubFrameConfig_S SSFConfig;
//     getSpecialSubFrameConfig(&SSFConfig);
//     updateSpecialSubFrameInfo_DwPts(bwsimSystem.myNodeDevices,bwsimSystem.nodeDeviceIDs,nuID,serverNodes,loopCountPerServerNode,SSFConfig.nDwPts);
//     updateSpecialSubFrameInfo_UpPts(bwsimSystem.myNodeDevices,bwsimSystem.nodeDeviceIDs,nuID,serverNodes,loopCountPerServerNode,SSFConfig.nUpPts);
//   }
//   
//   // Incrementing HARQ timers functionality is moved here for TDD case
// 
//   ivec frameConfigPerSrvr(serverNodes.length());
//   for(int srvrNode_cnt=0;srvrNode_cnt<serverNodes.size();srvrNode_cnt++)
//   {
//     int nuIndx=bwsimSystem.myNodeDevices(deviceIndices(srvrNode_cnt)).getNUIndx(nuID);
//     frameConfigPerSrvr(srvrNode_cnt)=bwsimSystem.myNodeDevices(deviceIndices(srvrNode_cnt)).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx(loopCountPerServerNode(srvrNode_cnt)%bwsimSystem.myNodeDevices(deviceIndices(srvrNode_cnt)).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx.length());
//     bool isDownlink=false;
//     if(frameConfigPerSrvr(srvrNode_cnt)%2==1)
//       isDownlink=true;
//         
// //      cout<<"isDownlink : "<<isDownlink<<endl;
//     
//     if(isDownlink)
//       append(uldlServerNodes.dlServerNodes,serverNodes(srvrNode_cnt));
//     else
//       append(uldlServerNodes.ulServerNodes,serverNodes(srvrNode_cnt));	 
//     
//     if(bwsimSystem.mySysInfo.ulDlMode==_ULDL_MODE_TDD_)
//     {
//       if(frameConfigPerSrvr(srvrNode_cnt) != 20)
//       {
// 	bwsimSystem.lteL2sim.fHigherLayerTimerUpdateENBTx_TDD(serverNodes[srvrNode_cnt],ttiCountPerServerNode(srvrNode_cnt),isDownlink);
// 	bwsimSystem.lteL2sim.fHigherLayerTimerUpdateUERx_TDD(serverNodes[srvrNode_cnt],ttiCountPerServerNode(srvrNode_cnt),isDownlink);
//       }
//     }
//     else
//     {
//       bwsimSystem.lteL2sim.fHigherLayerTimerUpdateENBTx(serverNodes[srvrNode_cnt],ttiCountPerServerNode(srvrNode_cnt),isDownlink);
//       bwsimSystem.lteL2sim.fHigherLayerTimerUpdateUERx(serverNodes[srvrNode_cnt],ttiCountPerServerNode(srvrNode_cnt),isDownlink);
//     }
//    
//   }
//   
//   
//   
//   if(uldlServerNodes.dlServerNodes.length())  //! L2 DL Scheduling for TDD
//   {
//     ivec dlTTICountPerServerNode=getTTICount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,uldlServerNodes.dlServerNodes,nuID);
//     
//     ivec dlDeviceIndices=find(bwsimSystem.nodeDeviceIDs,uldlServerNodes.dlServerNodes);
//     for(int srvr_cnt=0;srvr_cnt<uldlServerNodes.dlServerNodes.size();srvr_cnt++)
//     {
//       int nuIndx=bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).getNUIndx(nuID);
//       int carrier_cnt=find(bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).nuParameters.carrierFrequenciesInMHz,bwsimSystem.mySysInfo.carrierAggregation.getUniqueCarrierFreqsInMHz()(bwsimSystem.carrier_cnt));
//       bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.setCurrentFrameType("dlFrame");
//       bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.loadDLResourceMapperForBandwidth(bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getRuCountPerLevel());
//     }
//     updateDLFrameReservationInfo(bwsimSystem.myNodeDevices,bwsimSystem.nodeDeviceIDs,uldlServerNodes.dlServerNodes,nuID,bwsimSystem.associationInfo);
//     runDownlinkL2TX(bwsimSystem.mySysInfo, bwsimSystem.associationInfo, bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices, bwsimSystem.schedulerOutput, bwsimSystem.lteL2sim, uldlServerNodes.dlServerNodes,bwsimSystem.mySysInfo.carrierAggregation.getUniqueCarrierFreqsInMHz()(bwsimSystem.carrier_cnt),dlTTICountPerServerNode,nuID);
//   }
//   
//   if(uldlServerNodes.ulServerNodes.length())  //! L2 DL Scheduling for TDD
//   {
//     ivec ulTTICountPerServerNode=getTTICount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,uldlServerNodes.ulServerNodes,nuID);
//     ivec ulDeviceIndices=find(bwsimSystem.nodeDeviceIDs,uldlServerNodes.ulServerNodes);
//     for(int srvr_cnt=0;srvr_cnt<uldlServerNodes.ulServerNodes.size();srvr_cnt++)
//     {
//       int nuIndx=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).getNUIndx(nuID);
//       int carrier_cnt=find(bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).nuParameters.carrierFrequenciesInMHz,bwsimSystem.mySysInfo.carrierAggregation.getUniqueCarrierFreqsInMHz()(bwsimSystem.carrier_cnt));
//       bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.setCurrentFrameType("ulFrame");
//       bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.loadULResourceMapperForBandwidth(bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getRuCountPerLevel());
//       bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameReservationInfo.clear();
//     }
//     
//     runUplinkL2Tx(bwsimSystem.mySysInfo,bwsimSystem.associationInfo, bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices, bwsimSystem.schedulerOutput, bwsimSystem.lteL2sim,  uldlServerNodes.ulServerNodes,bwsimSystem.mySysInfo.carrierAggregation.getUniqueCarrierFreqsInMHz()(bwsimSystem.carrier_cnt),ulTTICountPerServerNode,nuID);
//   }
//   
//   if(!bwsimSystem.mySysInfo.linkSimulationInfo.isEnabled)
//     if(uldlServerNodes.ulServerNodes.length())  // check for uplink
//     {
//       ivec cuIDs=bwsimSystem.associationInfo.centralUnitIDsPerServerNode(find(bwsimSystem.associationInfo.serverNodes,uldlServerNodes.ulServerNodes));
//       for(int rx_cnt=0;rx_cnt<uldlServerNodes.ulServerNodes.length();rx_cnt++)
//       {
// 	int schToRxInfoIndx=bwsimSystem.schedulerOutput(cuIDs(rx_cnt)).getSchedulerToReceiverInfoIndx(uldlServerNodes.ulServerNodes(rx_cnt),nuID);
// 	for(int tx_cnt=0;tx_cnt<bwsimSystem.schedulerOutput(cuIDs(rx_cnt)).schedulerToTransmitterInfo.length();tx_cnt++)
// 	if(bwsimSystem.schedulerOutput(cuIDs(rx_cnt)).schedulerToTransmitterInfo(tx_cnt).rxNodeIDs(0)==uldlServerNodes.ulServerNodes(rx_cnt) && bwsimSystem.schedulerOutput(cuIDs(rx_cnt)).schedulerToTransmitterInfo(tx_cnt).txNUID==nuID)  
// 	{
// 	  int deviceIndx=find(bwsimSystem.nodeDeviceIDs,bwsimSystem.schedulerOutput(cuIDs(rx_cnt)).schedulerToTransmitterInfo(tx_cnt).txNodeID); 
// 	  int nuIndx=bwsimSystem.myNodeDevices(deviceIndx).getNUIndx(nuID);
// 	  int txIndx=find(bwsimSystem.mySysInfo.serviceNodes,bwsimSystem.schedulerOutput(cuIDs(rx_cnt)).schedulerToTransmitterInfo(tx_cnt).txNodeID);
// 	  int rxIndx=find(bwsimSystem.mySysInfo.serverNodes,bwsimSystem.schedulerOutput(cuIDs(rx_cnt)).schedulerToTransmitterInfo(tx_cnt).rxNodeIDs(0));
// 	  doUplinkPowerControl(bwsimSystem.schedulerOutput(cuIDs(rx_cnt)).schedulerToTransmitterInfo(tx_cnt),bwsimSystem.schedulerOutput(cuIDs(rx_cnt)).schedulerToReceiverInfo(schToRxInfoIndx),bwsimSystem.NIValuesPerServerNode(bwsimSystem.carrier_cnt)(rxIndx),bwsimSystem.downlinkSIRInDBPerServiceNode(txIndx),bwsimSystem.mySysInfo.serverNodes,bwsimSystem.mySysInfo.serviceNodes,bwsimSystem.linkGainInDB,bwsimSystem.myNodeDevices(deviceIndx).L1CorePerNU(nuIndx).ulTxConfig.powerControlInfo,bwsimSystem.mySysInfo.isSRSEnabled);
// 	}
//       }
//     }
//   
//   #ifdef USING_MPI
//   if(nTasks>1)
//   {
//     if(traceValuePerTraceName["EnablePrints"]=="true")
//       cout<<"Exchanging MPI Data ..."<<endl;
//     
//     
//     ///rsPowerBoost hardcoded
//     exchangeMPIDataNew(bwsimSystem.myNodeDevices,bwsimSystem.nodeDeviceIDs,serverNodes,bwsimSystem.associationInfo,bwsimSystem.mySysInfo,bwsimSystem.schedulerOutput,nuID);
//     // 	      updateFrameStructure=true;
//   }
//   #endif
//  
// }
}

// 


// void scheduleSidelink(BWSimSystem_S& bwsimSystem, ivec serverNodes, double currentTime, int nuID, int beamID,int panelID)
// {
//    
//   if(serverNodes.length()==0)
//   serverNodes=bwsimSystem.associationInfo.serverNodes;
//   
//      
//   ivec deviceIndices=find(bwsimSystem.nodeDeviceIDs,serverNodes); 
//   
//   
//   ivec loopCountPerServerNode=getLoopCount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,serverNodes,nuID);
//   
//   UlDlServerNodes_S uldlServerNodes;
//   // Incrementing HARQ timers functionality is moved here for TDD case
// 
//   ivec frameConfigPerSrvr(serverNodes.length());
//   
//   for(int srvrNode_cnt=0;srvrNode_cnt<serverNodes.size();srvrNode_cnt++)
//   {
//    
//     int nuIndx=bwsimSystem.myNodeDevices(deviceIndices(srvrNode_cnt)).getNUIndx(nuID);
//     frameConfigPerSrvr(srvrNode_cnt)=bwsimSystem.myNodeDevices(deviceIndices(srvrNode_cnt)).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx(loopCountPerServerNode(srvrNode_cnt)%bwsimSystem.myNodeDevices(deviceIndices(srvrNode_cnt)).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx.length());
//     bool isDownlink=false;
//     if(frameConfigPerSrvr(srvrNode_cnt)%2==1)
//       isDownlink=true;
//       if(isDownlink)
//       append(uldlServerNodes.dlServerNodes,serverNodes(srvrNode_cnt));
//     else
//       append(uldlServerNodes.ulServerNodes,serverNodes(srvrNode_cnt));	 
//   } 
//    
//   if(uldlServerNodes.ulServerNodes.length())  //! UL Scheduling
//   {
//     ivec srvrIndices=find(bwsimSystem.associationInfo.serverNodes,uldlServerNodes.ulServerNodes);
//     
//     ivec ulTTICountPerServerNode=getTTICount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,uldlServerNodes.ulServerNodes,nuID);
//     ivec ulDeviceIndices=find(bwsimSystem.nodeDeviceIDs,uldlServerNodes.ulServerNodes);
//      for(int srvr_cnt=0;srvr_cnt<uldlServerNodes.ulServerNodes.size();srvr_cnt++)
//     {
//       int nuIndx=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).getNUIndx(nuID);
//       bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.setCurrentFrameType("ulFrame");
//     }
//     updateFrameReservationInfo(bwsimSystem.myNodeDevices,bwsimSystem.nodeDeviceIDs,uldlServerNodes.ulServerNodes,nuID,false);
//     
//     for(int srvr_cnt=0;srvr_cnt<uldlServerNodes.ulServerNodes.size();srvr_cnt++)
//     {
//       int nuIndx1=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).getNUIndx(nuID);
//       ivec ttiCount=getTTICount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,uldlServerNodes.ulServerNodes,nuID);
//       
//       //Skipping scheduling if not enough symbols are in the subframe.
//       if((bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx1).ulFrameStructure.getFrameSize().numberOfSymbols-bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx1).ulFrameReservationInfo.reservedSymbols.length())<4)
//       {
// 	continue;
//       }
//       
//       #ifdef USING_MPI
//       if(nTasks>1)
//       {
// 	int srvrIndx=find(bwsimSystem.associationInfo.serverNodes,uldlServerNodes.ulServerNodes(srvr_cnt));
//     if(find(bwsimSystem.mySysInfo.mpiInfo.srvrIndxListPerTaskid(currentTaskid),srvrIndx)==-1)
// 	  continue;
//       }
//       #endif
// 	 int nuIndx=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).getNUIndx(nuID);
//   
//    
//     ivec ActiveUEs=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).activeUEs;
//   
// 
//     ivec SidelinkUEs=randPick(ActiveUEs, floor_i(((double)ActiveUEs.length())/4.0)*2.0);
//    ivec uplinkUEsindices=findNot(ActiveUEs,SidelinkUEs);
//    ivec SidelinkTxUEs=randPick(SidelinkUEs,SidelinkUEs.length()/2);
//    ivec SidelinkRxUEsindices=findNot(SidelinkUEs,SidelinkTxUEs);
//    ivec SidelinkRxUEs=SidelinkUEs(SidelinkRxUEsindices);
//    
//   
//    cout<<"SidelinkTxUEs : "<<SidelinkTxUEs<<endl;
//    cout<<"SidelinkRxUEs : "<<SidelinkRxUEs<<endl;
// 
//   //  ivec uplinkUESindices= findNot(ActiveUEs, SidelinkUEs);
//    
//   bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).activeUEs=SidelinkTxUEs;
//    
//   Array<UEBufferInfo_S> ueBufferInfo=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).getUEBufferInfo(ulTTICountPerServerNode(srvr_cnt),_TRANSMISSION_TYPE_SL_,beamID,panelID);
//  
//   
//   // ReTxInfo_S for SL should be handled properly as the packet belongs to rx-UE - Dhiv
//   
//   Array<ReTxInfo_S> ueReTxInfo=getUEReTxInfo(bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)),nuID,ulTTICountPerServerNode(srvr_cnt),_TRANSMISSION_TYPE_SL_,beamID,panelID);
//  
//   for(int ue_cnt=0;ue_cnt<ueBufferInfo.length();ue_cnt++)
// 	for(int cwd_cnt=0;cwd_cnt<ueReTxInfo(ue_cnt).isNewTransmission.length();cwd_cnt++)
// 	    if(!ueReTxInfo(cwd_cnt).isNewTransmission(cwd_cnt))
// 	      append(ueBufferInfo(ue_cnt).tbSizeInBitsForReTx,ueReTxInfo(ue_cnt).tbSizeInBitsForReTx(cwd_cnt));
// 	    
// 	 
//         SchedulingInfo_S schInfo=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).slScheduler.schedule(ulTTICountPerServerNode(srvr_cnt),beamID,ueBufferInfo,uldlServerNodes.ulServerNodes(srvr_cnt),bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulTxConfig.ulCQIModeInfo.isToTransmitCQIPorts(ulTTICountPerServerNode(srvr_cnt)),true);
//       // schedulingInfo_S schInfo=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).ulScheduler.schedule(ulTTICountPerServerNode(srvr_cnt),beamID,ueBufferInfo,bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).outerLoopLinkAdaptationInfoForUL,uldlServerNodes.ulServerNodes(srvr_cnt),bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulTxConfig.ulCQIModeInfo.isToTransmitCQIPorts(ulTTICountPerServerNode(srvr_cnt)),false);
// 
//     ivec uplinkUESindices= findNot(ActiveUEs, SidelinkUEs);
//   
//     bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).activeUEs=ActiveUEs(uplinkUEsindices);
//     bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).InactiveUEs=SidelinkUEs;
// 
//     cout<<"UplinkUEs : "<<bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).activeUEs<<endl;
//     
//       if(traceValuePerTraceName["EnablePrints"]=="true")
// 	cout<<" with schInfo.ueIDs : "<<schInfo.ueIDs<<endl;
// 	
//       ivec srvcNodeIndices=find(bwsimSystem.mySysInfo.serviceNodes,schInfo.ueIDs);
//       bvec isTheRBScheduled=zeros_b(bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getAllRuCount());
//       
//       for(int sch_cnt=0;sch_cnt<schInfo.ueIDs.length();sch_cnt++)
//       {
// 	bvec isAlreadyScheduled=isTheRBScheduled(schInfo.dataBlkInfo(sch_cnt).resourceBlocks);
// 	setInVec(isTheRBScheduled,schInfo.dataBlkInfo(sch_cnt).resourceBlocks,ones_b(schInfo.dataBlkInfo(sch_cnt).resourceBlocks.length()));
// 	RBMergeInfo_S mergedRbInfo=generateRBStructure(&bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameStructure,schInfo.dataBlkInfo(sch_cnt).dataGenInfo.rbInfo.rsInfo,schInfo.dataBlkInfo(sch_cnt).resourceBlocks,isAlreadyScheduled,false,schInfo.dataBlkInfo(sch_cnt).isLocalised,&bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameReservationInfo);
// 	ivec REsPerRB(0);
// 	for(int rb_cnt=0;rb_cnt<schInfo.dataBlkInfo(sch_cnt).resourceBlocks.length();rb_cnt++)
// 	{
// 	  bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.updateRUType(schInfo.dataBlkInfo(sch_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
// 	  append(REsPerRB,bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getToneCountPerRU(schInfo.dataBlkInfo(sch_cnt).resourceBlocks(rb_cnt),"data",schInfo.dataBlkInfo(sch_cnt).symPos));
// 	}
// 	
// 	
// 	int ueIndx=find(bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).associatedUEs,schInfo.ueIDs(sch_cnt));
//     schInfo.dataBlkInfo(sch_cnt).harqInfo=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).l2Device.eNB.fGetHarqManager(SidelinkRxUEs(sch_cnt),_TRANSMISSION_TYPE_SL_).getCurrentHarqInfo(ulTTICountPerServerNode(srvr_cnt),schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nCodeWords);
//     
//     schInfo.dataBlkInfo(sch_cnt).harqInfo.TBSizePerCodewordInBits.set_length(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nCodeWords);
// 	schInfo.dataBlkInfo(sch_cnt).harqInfo.infoBitsperCodeWord.set_length(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nCodeWords);
// 	for(int cwd_cnt=0;cwd_cnt<schInfo.dataBlkInfo(sch_cnt).harqInfo.isNewTransmission.length();cwd_cnt++)
// 	if(schInfo.dataBlkInfo(sch_cnt).harqInfo.isNewTransmission(cwd_cnt))
// 	{
// 	  int tbindex=-1;
// 	  if(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_)
// 	    tbindex = schInfo.dataBlkInfo(sch_cnt).dataGenInfo.mcsIndices(cwd_cnt);
// 	  else
// 	    tbindex = bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).sidelinkSourceAndSink.tbTables->transportBlockMCSMapping.transportBlockIndexPUSCH(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.mcsIndices[cwd_cnt]);
//             
// 	  schInfo.dataBlkInfo(sch_cnt).harqInfo.TBSizePerCodewordInBits(cwd_cnt)=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).sidelinkSourceAndSink.tbTables->getTBSizeForAllLayersCombined(tbindex,sum(REsPerRB),schInfo.dataBlkInfo(sch_cnt).resourceBlocks.length(),getNumLayersPerCodeWord(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nCodeWords,schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nLayers)(cwd_cnt),schInfo.dataBlkInfo(sch_cnt).dataGenInfo.scheduledSymbols,false);
// 	    
// 	  schInfo.dataBlkInfo(sch_cnt).harqInfo.infoBitsperCodeWord(cwd_cnt)=randb(schInfo.dataBlkInfo(sch_cnt).harqInfo.TBSizePerCodewordInBits(cwd_cnt));
// 	}
// 	bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).updateUEBufferInfo(schInfo.ueIDs(sch_cnt),sum(schInfo.dataBlkInfo(sch_cnt).harqInfo.TBSizePerCodeword)/8,_TRANSMISSION_TYPE_SL_);
//     bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).l2Device.eNB.fGetHarqManager(SidelinkRxUEs(sch_cnt),_TRANSMISSION_TYPE_SL_).setTransmissionStatus(schInfo.dataBlkInfo(sch_cnt).harqInfo,ulTTICountPerServerNode(srvr_cnt));
// 	double txSubcarrierPowerInDBm=bwsimSystem.mySysInfo.serviceNodeTxPowersInDBm(srvcNodeIndices(sch_cnt))-10*log10(schInfo.dataBlkInfo(sch_cnt).resourceBlocks.length()*12); //12 hardcoded  -Dhiv
// 	double carrierInMHz=getCarrierFreqInMHz(nuID);
// 	
//     if(bwsimSystem.mySysInfo.linkSimulationInfo.isEnabled)
//         txSubcarrierPowerInDBm=30;/// Hard-coded to 0 dBW for link level simulations
//     
// 	if(!bwsimSystem.mySysInfo.receptionModel.enableRestrictedRecepition || (bwsimSystem.mySysInfo.receptionModel.enableRestrictedRecepition && find(bwsimSystem.mySysInfo.receptionModel.serverNodesWithActualReception,uldlServerNodes.ulServerNodes(srvr_cnt))!=-1))
//         bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).addRxDataBlk(SidelinkRxUEs(sch_cnt),schInfo.ueIDs(sch_cnt),nuID,carrierInMHz,ulTTICountPerServerNode(srvr_cnt),schInfo.dataBlkInfo(sch_cnt),txSubcarrierPowerInDBm,_TRANSMISSION_TYPE_SL_,bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).beamPairLinkperUE(ueIndx).srvrBeamID,bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).beamPairLinkperUE(ueIndx).txPanelID);
// 	
//     bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).addTxDataBlk(schInfo.ueIDs(sch_cnt),SidelinkRxUEs(sch_cnt),nuID,carrierInMHz,ulTTICountPerServerNode(srvr_cnt),schInfo.dataBlkInfo(sch_cnt),txSubcarrierPowerInDBm,_TRANSMISSION_TYPE_SL_,bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).beamPairLinkperUE(ueIndx).srvcBeamID,bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).beamPairLinkperUE(ueIndx).rxPanelID);
//       
// // 	if(!bwsimSystem.mySysInfo.linkSimulationInfo.isEnabled)
// // 	{
// // 	  double time1=1; // current time ??
// // 	  
// // 	  int schToRxInfoIndx=bwsimSystem.schedulerOutput(cuIndices(srvr_cnt)).getSchedulerToReceiverInfoIndx(uldlServerNodes.ulServerNodes(srvr_cnt),nuID,beamID);
// // 	  int schToTxInfoIndx=bwsimSystem.schedulerOutput(cuIndices(srvr_cnt)).getSchedulerToTransmitterInfoIndx(schInfo.ueIDs(sch_cnt),nuID,beamID);
// // 	  int txDeviceIndx=find(bwsimSystem.nodeDeviceIDs,bwsimSystem.schedulerOutput(cuIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx).txNodeID); 
// // 	  int txNuIndx=bwsimSystem.myNodeDevices(txDeviceIndx).getNUIndx(nuID);
// // 	  
// // 	  int rxDeviceIndx=find(bwsimSystem.nodeDeviceIDs,bwsimSystem.schedulerOutput(cuIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx).rxNodeIDs(0)); 
// // 	  int rxNuIndx=bwsimSystem.myNodeDevices(rxDeviceIndx).getNUIndx(nuID);
// // 	  int ueIndxInAssociation=find(bwsimSystem.associationInfo.serviceNodes,bwsimSystem.schedulerOutput(cuIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx).txNodeID);
// // 	  sBeamPairLink beamLink=bwsimSystem.associationInfo.associatedBeamPairLinkforEachServiceNode(ueIndxInAssociation);
// // 	  
// // 	  int txIndx=find(bwsimSystem.mySysInfo.serviceNodes,bwsimSystem.schedulerOutput(cuIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx).txNodeID);
// // 	  if(schToRxInfoIndx!=-1)
// // 	  {
// // 	    if(bwsimSystem.schedulerOutput(cuIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx).dataBlkInfo(0).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_)
// //             doUplinkPowerControlNR(bwsimSystem.schedulerOutput(cuIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx), bwsimSystem.schedulerOutput(cuIndices(srvr_cnt)).schedulerToReceiverInfo(schToRxInfoIndx),bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe(), beamLink.rspIndBm, beamLink.rsrpIndBm, bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).ulTxConfig.powerControlInfo, bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).sidelinkSourceAndSink.tbTables->awgnTablesConvCoder.is256QAMenabled, bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).sidelinkSourceAndSink.tbTables->awgnTablesConvCoder.isPi2BPSKenabled, bwsimSystem.mySysInfo.isSRSEnabled, time1, bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).nuParameters.subcarrierSpacingInHz/15000 , bwsimSystem.myNodeDevices(txDeviceIndx).poAlphaSet);
// // 	    else
// // 	      doUplinkPowerControl(bwsimSystem.schedulerOutput(cuIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx),bwsimSystem.schedulerOutput(cuIndices(srvr_cnt)).schedulerToReceiverInfo(schToRxInfoIndx),bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).NI,bwsimSystem.downlinkSIRInDBPerServiceNode(txIndx),bwsimSystem.mySysInfo.serverNodes,bwsimSystem.mySysInfo.serviceNodes,bwsimSystem.linkGainInDB,bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).ulTxConfig.powerControlInfo,bwsimSystem.mySysInfo.isSRSEnabled);
// // 	  }
// // 	  else
// // 	  {
// // 	    SchedulerToReceiverInfo_S dummy;
// // 	    if(bwsimSystem.schedulerOutput(cuIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx).dataBlkInfo(0).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_)
// //             doUplinkPowerControlNR(bwsimSystem.schedulerOutput(cuIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx), dummy,bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe(), beamLink.rspIndBm, beamLink.rsrpIndBm, bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).ulTxConfig.powerControlInfo, bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).sidelinkSourceAndSink.tbTables->awgnTablesConvCoder.is256QAMenabled, bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).sidelinkSourceAndSink.tbTables->awgnTablesConvCoder.isPi2BPSKenabled, bwsimSystem.mySysInfo.isSRSEnabled, time1, bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).nuParameters.subcarrierSpacingInHz/15000 , bwsimSystem.myNodeDevices(txDeviceIndx).poAlphaSet);
// // 	    else
// // 	      doUplinkPowerControl(bwsimSystem.schedulerOutput(cuIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx),dummy,bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).NI,bwsimSystem.downlinkSIRInDBPerServiceNode(txIndx),bwsimSystem.mySysInfo.serverNodes,bwsimSystem.mySysInfo.serviceNodes,bwsimSystem.linkGainInDB,bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).ulTxConfig.powerControlInfo,bwsimSystem.mySysInfo.isSRSEnabled);
// // 	  }
// // 	  
// // 	}
// 	
//       } 
//     }  
//     
//   }
//   
// }

ivec nodeScheduleData(BWSimSystem_S& bwsimSystem, ivec serverNodes, double currentTime, int nuID, int beamID)
{
   
  if(serverNodes.length()==0)
  serverNodes=bwsimSystem.associationInfo.serverNodes;
  
  ivec deviceIndices=find(bwsimSystem.nodeDeviceIDs,serverNodes); 
 
    // abort();
  ivec loopCountPerServerNode=getLoopCount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,serverNodes,nuID);
  
  UlDlServerNodes_S uldlServerNodes;

  // Incrementing HARQ timers functionality is moved here for TDD case

  ivec frameConfigPerSrvr(serverNodes.length());
  
  for(int srvrNode_cnt=0;srvrNode_cnt<serverNodes.size();srvrNode_cnt++)
  {
   
    int nuIndx=bwsimSystem.myNodeDevices(deviceIndices(srvrNode_cnt)).getNUIndx(nuID);
    frameConfigPerSrvr(srvrNode_cnt)=bwsimSystem.myNodeDevices(deviceIndices(srvrNode_cnt)).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx(loopCountPerServerNode(srvrNode_cnt)%bwsimSystem.myNodeDevices(deviceIndices(srvrNode_cnt)).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx.length());
    bool isDownlink=false;
    if(frameConfigPerSrvr(srvrNode_cnt)%2==1)
      isDownlink=true;
 
    if(isDownlink)
      append(uldlServerNodes.dlServerNodes,serverNodes(srvrNode_cnt));
    else
      append(uldlServerNodes.ulServerNodes,serverNodes(srvrNode_cnt));	 
  } 
    // abort();
  if(uldlServerNodes.dlServerNodes.length())  //! DL Scheduling
  {
    ivec srvrIndices=find(bwsimSystem.associationInfo.serverNodes,uldlServerNodes.dlServerNodes);
    
     
    ivec dlTTICountPerServerNode=getTTICount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,uldlServerNodes.dlServerNodes,nuID);
    ivec dlDeviceIndices=find(bwsimSystem.nodeDeviceIDs,uldlServerNodes.dlServerNodes);
    for(int srvr_cnt=0;srvr_cnt<uldlServerNodes.dlServerNodes.size();srvr_cnt++)
    {
      int nuIndx=bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).getNUIndx(nuID);
      bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.setCurrentFrameType("dlFrame");
    }
    updateFrameReservationInfo(bwsimSystem.myNodeDevices,bwsimSystem.nodeDeviceIDs,uldlServerNodes.dlServerNodes,nuID,true);
    
    for(int srvr_cnt=0;srvr_cnt<uldlServerNodes.dlServerNodes.size();srvr_cnt++)
    {
      int nuIndx1=bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).getNUIndx(nuID);
      
      //Skipping scheduling if not enough symbols are in the subframe.
      if((bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx1).dlFrameStructure.getFrameSize().numberOfSymbols-bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx1).dlFrameReservationInfo.reservedSymbols.length())<4)
      {
	continue;
      }
      
      #ifdef USING_MPI
      if(nTasks>1)
      {
	int srvrIndx=find(bwsimSystem.associationInfo.serverNodes,uldlServerNodes.dlServerNodes(srvr_cnt));
	if(find(bwsimSystem.mySysInfo.mpiInfo.srvrIndxListPerTaskid(currentTaskid),srvrIndx)==-1)
	  continue;
      }
      #endif
      
      int nuIndx=bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).getNUIndx(nuID);
      Array<UEBufferInfo_S> ueBufferInfo=bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).getUEBufferInfo(dlTTICountPerServerNode(srvr_cnt),_TRANSMISSION_TYPE_DL_,beamID);

  
      //If end IF beam scheduling 
//       for(int i=0; i<bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).beamPairLinkperUE.length(); i++)
//       {
// 	if(bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).beamPairLinkperUE(i).txBeamID == beamID);
// 	{
// 	  bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).associatedUEs;
// 	}
//       }
      
      Array<ReTxInfo_S> ueReTxInfo=getUEReTxInfo(bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)),nuID,dlTTICountPerServerNode(srvr_cnt),_TRANSMISSION_TYPE_DL_,beamID);
      
      for(int ue_cnt=0;ue_cnt<ueBufferInfo.length();ue_cnt++)
	for(int cwd_cnt=0;cwd_cnt<ueReTxInfo(ue_cnt).isNewTransmission.length();cwd_cnt++)
	  if(!ueReTxInfo(ue_cnt).isNewTransmission(cwd_cnt))
      {
	    append(ueBufferInfo(ue_cnt).tbSizeInBitsForReTx,ueReTxInfo(ue_cnt).tbSizeInBitsForReTx(cwd_cnt));
        append(ueBufferInfo(ue_cnt).codewordIndices,ueReTxInfo(ue_cnt).codewordIndices(cwd_cnt));
      }
       SchedulingInfo_S schInfo=bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).dlScheduler.schedule(dlTTICountPerServerNode(srvr_cnt),beamID,ueBufferInfo,bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).outerLoopLinkAdaptationInfoForDL,uldlServerNodes.dlServerNodes(srvr_cnt),bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).dlTxConfig.dlCQIModeInfo.isToTransmitCQIPorts(dlTTICountPerServerNode(srvr_cnt)),false,"",bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).dlTxConfig.mTRSConf,bwsimSystem.mySysInfo.resultFiles.MUMIMOpairingFileName(getNUID(nuID)-1),bwsimSystem.mySysInfo.resultFiles.SchInfoFileName(getNUID(nuID)-1));
      if(traceValuePerTraceName["EnablePrints"]=="true")
        cout<<" Scheduled UEs in the current TTI : "<<schInfo.ueIDs<<endl;
      
      
      bvec isTheRBScheduled=zeros_b(bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getAllRuCount());
      
       for(int sch_cnt=0;sch_cnt<schInfo.ueIDs.length();sch_cnt++)
       {
	 bvec isAlreadyScheduled=isTheRBScheduled(schInfo.dataBlkInfo(sch_cnt).resourceBlocks);
	 setInVec(isTheRBScheduled,schInfo.dataBlkInfo(sch_cnt).resourceBlocks,ones_b(schInfo.dataBlkInfo(sch_cnt).resourceBlocks.length()));
	
	 RBMergeInfo_S mergedRbInfo=generateRBStructure(&bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).dlFrameStructure,schInfo.dataBlkInfo(sch_cnt).dataGenInfo.rbInfo.rsInfo,schInfo.dataBlkInfo(sch_cnt).resourceBlocks,isAlreadyScheduled,true,schInfo.dataBlkInfo(sch_cnt).isLocalised,&bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).dlFrameReservationInfo);
	 ivec REsPerRB(0);
	 //cout<<"chkloop:"<<endl;
	 for(int rb_cnt=0;rb_cnt<schInfo.dataBlkInfo(sch_cnt).resourceBlocks.length();rb_cnt++)
	 {
	   
	   bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.updateRUType(schInfo.dataBlkInfo(sch_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
	   append(REsPerRB,bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).dlFrameStructure.getToneCountPerRU(schInfo.dataBlkInfo(sch_cnt).resourceBlocks(rb_cnt),"data",schInfo.dataBlkInfo(sch_cnt).symPos));
	 }
	
	 int ueIndx=find(bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).associatedUEs,schInfo.ueIDs(sch_cnt));
     schInfo.dataBlkInfo(sch_cnt).harqInfo=bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).dlHarqManager(ueIndx).getCurrentHarqInfo(dlTTICountPerServerNode(srvr_cnt),schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nCodeWords);
     schInfo.dataBlkInfo(sch_cnt).harqInfo.TBSizePerCodewordInBits.set_length(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nCodeWords);
	 schInfo.dataBlkInfo(sch_cnt).harqInfo.infoBitsperCodeWord.set_length(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nCodeWords);
	 for(int cwd_cnt=0;cwd_cnt<schInfo.dataBlkInfo(sch_cnt).harqInfo.isNewTransmission.length();cwd_cnt++)
	 if(schInfo.dataBlkInfo(sch_cnt).harqInfo.isNewTransmission(cwd_cnt))
	 {
	   int tbindex=-1,mimoRate = getNumLayersPerCodeWord(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nCodeWords,schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nLayers)(cwd_cnt);
	   if(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_)
	     tbindex = schInfo.dataBlkInfo(sch_cnt).dataGenInfo.mcsIndices(cwd_cnt);
	   else
	     tbindex = bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).downlinkSourceAndSink.tbTables->transportBlockMCSMapping.transportBlockIndexPDSCH(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.mcsIndices[cwd_cnt]);
	   
	  if(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
	    mimoRate = 1;
	    
      schInfo.dataBlkInfo(sch_cnt).harqInfo.TBSizePerCodewordInBits(cwd_cnt)=bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).downlinkSourceAndSink.tbTables->getTBSizeForAllLayersCombined(tbindex,sum(REsPerRB),schInfo.dataBlkInfo(sch_cnt).resourceBlocks.length(),mimoRate,schInfo.dataBlkInfo(sch_cnt).dataGenInfo.scheduledSymbols,true);
	   
      schInfo.dataBlkInfo(sch_cnt).harqInfo.infoBitsperCodeWord(cwd_cnt)=randb(schInfo.dataBlkInfo(sch_cnt).harqInfo.TBSizePerCodewordInBits(cwd_cnt));
	 }	
	 
	 bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).updateUEBufferInfo(schInfo.ueIDs(sch_cnt),sum(schInfo.dataBlkInfo(sch_cnt).harqInfo.TBSizePerCodewordInBits)/8,schInfo.rbIDs(sch_cnt),_TRANSMISSION_TYPE_DL_);
     double txSubcarrierPowerInDBm=bwsimSystem.mySysInfo.serverNodeTxPowersInDBm(srvrIndices(srvr_cnt))-10*log10(bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getUsedSubcarriers());
     
     if(bwsimSystem.mySysInfo.linkSimulationInfo.isEnabled)
         txSubcarrierPowerInDBm=30;/// Hard-coded to 0 dBW for link level simulations
     
     double carrierInMHz=(double)getCarrierFreqInMHz(nuID);
     bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).addTxDataBlk(uldlServerNodes.dlServerNodes(srvr_cnt),schInfo.ueIDs(sch_cnt),nuID,carrierInMHz,dlTTICountPerServerNode(srvr_cnt),schInfo.dataBlkInfo(sch_cnt),txSubcarrierPowerInDBm,_TRANSMISSION_TYPE_DL_,bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).beamPairLinkperUE(ueIndx).srvrBeamID);
	 if(!bwsimSystem.mySysInfo.receptionModel.enableRestrictedRecepition || (bwsimSystem.mySysInfo.receptionModel.enableRestrictedRecepition && find(bwsimSystem.mySysInfo.receptionModel.serverNodesWithActualReception,uldlServerNodes.dlServerNodes(srvr_cnt))!=-1))
         bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).addRxDataBlk(schInfo.ueIDs(sch_cnt),uldlServerNodes.dlServerNodes(srvr_cnt),nuID,carrierInMHz,dlTTICountPerServerNode(srvr_cnt),schInfo.dataBlkInfo(sch_cnt),txSubcarrierPowerInDBm,_TRANSMISSION_TYPE_DL_,bwsimSystem.myNodeDevices(dlDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).beamPairLinkperUE(ueIndx).srvcBeamID);
       }
    }
  }
  
  if(uldlServerNodes.ulServerNodes.length())  //! UL Scheduling
  {
    ivec srvrIndices=find(bwsimSystem.associationInfo.serverNodes,uldlServerNodes.ulServerNodes);
    
    ivec ulTTICountPerServerNode=getTTICount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,uldlServerNodes.ulServerNodes,nuID);
    ivec ulDeviceIndices=find(bwsimSystem.nodeDeviceIDs,uldlServerNodes.ulServerNodes);
     for(int srvr_cnt=0;srvr_cnt<uldlServerNodes.ulServerNodes.size();srvr_cnt++)
    {
      int nuIndx=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).getNUIndx(nuID);
      bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.setCurrentFrameType("ulFrame");
    }
    updateFrameReservationInfo(bwsimSystem.myNodeDevices,bwsimSystem.nodeDeviceIDs,uldlServerNodes.ulServerNodes,nuID,false);
    
    for(int srvr_cnt=0;srvr_cnt<uldlServerNodes.ulServerNodes.size();srvr_cnt++)
    {
      int nuIndx1=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).getNUIndx(nuID);
      ivec ttiCount=getTTICount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,uldlServerNodes.ulServerNodes,nuID);
      
      //Skipping scheduling if not enough symbols are in the subframe.
      if((bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx1).ulFrameStructure.getFrameSize().numberOfSymbols-bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx1).ulFrameReservationInfo.reservedSymbols.length())<4)
      {
	continue;
      }
      
      #ifdef USING_MPI
      if(nTasks>1)
      {
	int srvrIndx=find(bwsimSystem.associationInfo.serverNodes,uldlServerNodes.ulServerNodes(srvr_cnt));
	if(find(bwsimSystem.mySysInfo.mpiInfo.srvrIndxListPerTaskid(currentTaskid),srvrIndx)==-1)
	  continue;
      }
      #endif
	 int nuIndx=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).getNUIndx(nuID);
      Array<UEBufferInfo_S> ueBufferInfo=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).getUEBufferInfo(ulTTICountPerServerNode(srvr_cnt),_TRANSMISSION_TYPE_UL_,beamID);
      Array<ReTxInfo_S> ueReTxInfo=getUEReTxInfo(bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)),nuID,ulTTICountPerServerNode(srvr_cnt),_TRANSMISSION_TYPE_UL_,beamID);
	
      for(int ue_cnt=0;ue_cnt<ueBufferInfo.length();ue_cnt++)
	for(int cwd_cnt=0;cwd_cnt<ueReTxInfo(ue_cnt).isNewTransmission.length();cwd_cnt++)
	    if(!ueReTxInfo(cwd_cnt).isNewTransmission(cwd_cnt))
        {
            append(ueBufferInfo(ue_cnt).tbSizeInBitsForReTx,ueReTxInfo(ue_cnt).tbSizeInBitsForReTx(cwd_cnt));
            append(ueBufferInfo(ue_cnt).codewordIndices,ueReTxInfo(ue_cnt).codewordIndices(cwd_cnt));
        }
    SchedulingInfo_S schInfo=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).ulScheduler.schedule(ulTTICountPerServerNode(srvr_cnt),beamID,ueBufferInfo,bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).outerLoopLinkAdaptationInfoForUL,uldlServerNodes.ulServerNodes(srvr_cnt),bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulTxConfig.ulCQIModeInfo.isToTransmitCQIPorts(ulTTICountPerServerNode(srvr_cnt)),true, bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).TPCcmdFieldPerUE, TRSConfig_S(),bwsimSystem.mySysInfo.resultFiles.MUMIMOpairingFileName(getNUID(nuID)-1),bwsimSystem.mySysInfo.resultFiles.SchInfoFileName(getNUID(nuID)-1));
//       if(traceValuePerTraceName["EnablePrints"]=="true")
// 	cout<<" with schInfo.ueIDs : "<<schInfo.ueIDs<<endl;
	
      ivec srvcNodeIndices=find(bwsimSystem.mySysInfo.serviceNodes,schInfo.ueIDs);
      bvec isTheRBScheduled=zeros_b(bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getAllRuCount());
      
      for(int sch_cnt=0;sch_cnt<schInfo.ueIDs.length();sch_cnt++)
      {
	bvec isAlreadyScheduled=isTheRBScheduled(schInfo.dataBlkInfo(sch_cnt).resourceBlocks);
	setInVec(isTheRBScheduled,schInfo.dataBlkInfo(sch_cnt).resourceBlocks,ones_b(schInfo.dataBlkInfo(sch_cnt).resourceBlocks.length()));
	RBMergeInfo_S mergedRbInfo=generateRBStructure(&bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameStructure,schInfo.dataBlkInfo(sch_cnt).dataGenInfo.rbInfo.rsInfo,schInfo.dataBlkInfo(sch_cnt).resourceBlocks,isAlreadyScheduled,false,schInfo.dataBlkInfo(sch_cnt).isLocalised,&bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameReservationInfo);
	ivec REsPerRB(0);
	for(int rb_cnt=0;rb_cnt<schInfo.dataBlkInfo(sch_cnt).resourceBlocks.length();rb_cnt++)
	{
	  bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.updateRUType(schInfo.dataBlkInfo(sch_cnt).resourceBlocks(rb_cnt),mergedRbInfo.mergedRBName(rb_cnt));
	  append(REsPerRB,bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.getToneCountPerRU(schInfo.dataBlkInfo(sch_cnt).resourceBlocks(rb_cnt),"data",schInfo.dataBlkInfo(sch_cnt).symPos));
	}
	
	
	int ueIndx=find(bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).associatedUEs,schInfo.ueIDs(sch_cnt));
    schInfo.dataBlkInfo(sch_cnt).harqInfo=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).ulHarqManager(ueIndx).getCurrentHarqInfo(ulTTICountPerServerNode(srvr_cnt),schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nCodeWords);
    schInfo.dataBlkInfo(sch_cnt).harqInfo.TBSizePerCodewordInBits.set_length(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nCodeWords);
	schInfo.dataBlkInfo(sch_cnt).harqInfo.infoBitsperCodeWord.set_length(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nCodeWords);
	for(int cwd_cnt=0;cwd_cnt<schInfo.dataBlkInfo(sch_cnt).harqInfo.isNewTransmission.length();cwd_cnt++)
	if(schInfo.dataBlkInfo(sch_cnt).harqInfo.isNewTransmission(cwd_cnt))
	{
	  int tbindex=-1;
	  if(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_NR_CL_)
	    tbindex = schInfo.dataBlkInfo(sch_cnt).dataGenInfo.mcsIndices(cwd_cnt);
	  else
	    tbindex = bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).uplinkSourceAndSink.tbTables->transportBlockMCSMapping.transportBlockIndexPUSCH(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.mcsIndices[cwd_cnt]);

      schInfo.dataBlkInfo(sch_cnt).harqInfo.TBSizePerCodewordInBits(cwd_cnt)=bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L1CorePerNU(nuIndx).uplinkSourceAndSink.tbTables->getTBSizeForAllLayersCombined(tbindex,sum(REsPerRB),schInfo.dataBlkInfo(sch_cnt).resourceBlocks.length(),getNumLayersPerCodeWord(schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nCodeWords,schInfo.dataBlkInfo(sch_cnt).dataGenInfo.nLayers)(cwd_cnt),schInfo.dataBlkInfo(sch_cnt).dataGenInfo.scheduledSymbols,false);
	    
      schInfo.dataBlkInfo(sch_cnt).harqInfo.infoBitsperCodeWord(cwd_cnt)=randb(schInfo.dataBlkInfo(sch_cnt).harqInfo.TBSizePerCodewordInBits(cwd_cnt));
	}
	bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).updateUEBufferInfo(schInfo.ueIDs(sch_cnt),sum(schInfo.dataBlkInfo(sch_cnt).harqInfo.TBSizePerCodewordInBits)/8,schInfo.rbIDs(sch_cnt),_TRANSMISSION_TYPE_UL_);
    double txSubcarrierPowerInDBm=bwsimSystem.mySysInfo.serviceNodeTxPowersInDBm(srvcNodeIndices(sch_cnt))-10*log10(schInfo.dataBlkInfo(sch_cnt).resourceBlocks.length()*12); //12 hardcoded  -Dhiv
	double carrierInMHz=getCarrierFreqInMHz(nuID);
	
    if(bwsimSystem.mySysInfo.linkSimulationInfo.isEnabled)
        txSubcarrierPowerInDBm=30;/// Hard-coded to 0 dBW for link level simulations
    
	if(!bwsimSystem.mySysInfo.receptionModel.enableRestrictedRecepition || (bwsimSystem.mySysInfo.receptionModel.enableRestrictedRecepition && find(bwsimSystem.mySysInfo.receptionModel.serverNodesWithActualReception,uldlServerNodes.ulServerNodes(srvr_cnt))!=-1))
        bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).addRxDataBlk(uldlServerNodes.ulServerNodes(srvr_cnt),schInfo.ueIDs(sch_cnt),nuID,carrierInMHz,ulTTICountPerServerNode(srvr_cnt),schInfo.dataBlkInfo(sch_cnt),txSubcarrierPowerInDBm,_TRANSMISSION_TYPE_UL_,bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).beamPairLinkperUE(ueIndx).srvrBeamID);
	
    bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).addTxDataBlk(schInfo.ueIDs(sch_cnt),uldlServerNodes.ulServerNodes(srvr_cnt),nuID,carrierInMHz,ulTTICountPerServerNode(srvr_cnt),schInfo.dataBlkInfo(sch_cnt),txSubcarrierPowerInDBm,_TRANSMISSION_TYPE_UL_,bwsimSystem.myNodeDevices(ulDeviceIndices(srvr_cnt)).L2CorePerNU(nuIndx).beamPairLinkperUE(ueIndx).srvcBeamID);
      
	if(!bwsimSystem.mySysInfo.linkSimulationInfo.isEnabled)
	{
	  double time1=1; // current time ??
	  
	  int schToRxInfoIndx=bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).getSchedulerToReceiverInfoIndx(uldlServerNodes.ulServerNodes(srvr_cnt),nuID,beamID);
	  int schToTxInfoIndx=bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).getSchedulerToTransmitterInfoIndx(schInfo.ueIDs(sch_cnt),nuID,beamID);
      if(schToTxInfoIndx!=-1){
	  int txDeviceIndx=find(bwsimSystem.nodeDeviceIDs,bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx).txNodeID); 
	  int txNuIndx=bwsimSystem.myNodeDevices(txDeviceIndx).getNUIndx(nuID);
	  
	  int rxDeviceIndx=find(bwsimSystem.nodeDeviceIDs,bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx).rxNodeIDs(0)); 
	  int rxNuIndx=bwsimSystem.myNodeDevices(rxDeviceIndx).getNUIndx(nuID);
	  int ueIndxInAssociation=find(bwsimSystem.associationInfo.serviceNodes,bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx).txNodeID);
	  sBeamPairLink beamLink=bwsimSystem.associationInfo.associatedBeamPairLinkforEachServiceNode(ueIndxInAssociation);

	  int txIndx=find(bwsimSystem.mySysInfo.serviceNodes,bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx).txNodeID);
	  SchedulerToTransmitterInfo_S schToTxInfo = bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx); 
	   
	  double txSubcarrierPowerInDBmPerAntenna=doUplinkPowerControlNR(schToTxInfo.txSubcarrierPowerInDBmPerAntenna,schToTxInfo.dataBlkInfo(0).resourceBlocks ,bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).resourceMapper.ulResourceMapper.getResourceBlocksPerSubframe(),schToTxInfo.dataBlkInfo(0).dataGenInfo.mcsIndices(0),schToTxInfo.dataBlkInfo(0).waveform, schToTxInfo.dataBlkInfo(0).TPCcmdFieldIndividualUE, beamLink.rspIndBm, beamLink.rsrpIndBm, bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).ulTxConfig.powerControlInfo, bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).uplinkSourceAndSink.tbTables->awgnTablesConvCoder.is256QAMenabled, bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).uplinkSourceAndSink.tbTables->awgnTablesConvCoder.isPi2BPSKenabled, bwsimSystem.mySysInfo.isSRSEnabled, time1, bwsimSystem.myNodeDevices(txDeviceIndx).L1CorePerNU(txNuIndx).nuParameters.subcarrierSpacingInHz/15000 , bwsimSystem.myNodeDevices(txDeviceIndx).poAlphaSet);
	  bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToTransmitterInfo(schToTxInfoIndx).txSubcarrierPowerInDBmPerAntenna = txSubcarrierPowerInDBmPerAntenna;
	  
	  if(schToRxInfoIndx!=-1)
	  {
	    int txIndx= find(bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToReceiverInfo(schToRxInfoIndx).txNodeIDs,schToTxInfo.txNodeID);
	    bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).schedulerToReceiverInfo(schToRxInfoIndx).txSubcarrierPowerInDBmPerAntenna(txIndx) = txSubcarrierPowerInDBmPerAntenna;
	  }
	  
         }
	}
	
      } 
    }  
    
  }
  
  return uldlServerNodes.dlServerNodes;
}


void nodeScheduleRS(BWSimSystem_S& bwsimSystem, ivec serverNodes, double currentTime, int nuID, int beamID)
{
  if(serverNodes.length()==0)
    serverNodes=bwsimSystem.associationInfo.serverNodes;
  
  ivec deviceIndices=find(bwsimSystem.nodeDeviceIDs,serverNodes); 
  
  ivec loopCountPerServerNode=getLoopCount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,serverNodes,nuID);
  
  UlDlServerNodes_S uldlServerNodes;
  
  // Incrementing HARQ timers functionality is moved here for TDD case
  
  ivec frameConfigPerSrvr(serverNodes.length());
  
  for(int srvrNode_cnt=0;srvrNode_cnt<serverNodes.size();srvrNode_cnt++)
  {
    
    int nuIndx=bwsimSystem.myNodeDevices(deviceIndices(srvrNode_cnt)).getNUIndx(nuID);
    frameConfigPerSrvr(srvrNode_cnt)=bwsimSystem.myNodeDevices(deviceIndices(srvrNode_cnt)).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx(loopCountPerServerNode(srvrNode_cnt)%bwsimSystem.myNodeDevices(deviceIndices(srvrNode_cnt)).L1CorePerNU(nuIndx).frameConfig.dlFrameIndx.length());
    bool isDownlink=false;
    if(frameConfigPerSrvr(srvrNode_cnt)%2==1)
      isDownlink=true;
    
    if(isDownlink)
      append(uldlServerNodes.dlServerNodes,serverNodes(srvrNode_cnt));
    else
      append(uldlServerNodes.ulServerNodes,serverNodes(srvrNode_cnt));	 
  }
  
  if(uldlServerNodes.dlServerNodes.length())  //! DL CSI-RS Scheduling
  {
    ivec srvrIndices=find(bwsimSystem.associationInfo.serverNodes,uldlServerNodes.dlServerNodes);
    ivec dlDeviceIndices=find(bwsimSystem.nodeDeviceIDs,uldlServerNodes.dlServerNodes);
    ivec dlTTICountPerServerNode=getTTICount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,uldlServerNodes.dlServerNodes,nuID);
    
    for(int srvrIndx=0;srvrIndx<srvrIndices.length();srvrIndx++)
    {
      int nuIndx=bwsimSystem.myNodeDevices(dlDeviceIndices(srvrIndx)).getNUIndx(nuID);
      ivec activeUEs=bwsimSystem.myNodeDevices(dlDeviceIndices(srvrIndx)).L2CorePerNU(nuIndx).activeUEs;
      
      ivec beamAssociatedUsers;
  beamAssociatedUsers.set_length(0);  
  //ivec index;
  //index.set_length(0);
  
  for(int i=0; i<bwsimSystem.myNodeDevices(dlDeviceIndices(srvrIndx)).L2CorePerNU(nuIndx).beamPairLinkperUE.length(); i++)
  {
      if(bwsimSystem.myNodeDevices(dlDeviceIndices(srvrIndx)).L2CorePerNU(nuIndx).beamPairLinkperUE(i).srvrBeamID == beamID)
          if(find(activeUEs,bwsimSystem.myNodeDevices(dlDeviceIndices(srvrIndx)).L2CorePerNU(nuIndx).associatedUEs(i))!=-1)    
          {
              append(beamAssociatedUsers,bwsimSystem.myNodeDevices(dlDeviceIndices(srvrIndx)).L2CorePerNU(nuIndx).associatedUEs(i));
              //append(index, i);
          }
  }
  
  activeUEs=beamAssociatedUsers;
  
      if(bwsimSystem.myNodeDevices(dlDeviceIndices(srvrIndx)).L1CorePerNU(nuIndx).dlTxConfig.dlCQIModeInfo.isToTransmitCQIPorts(dlTTICountPerServerNode(srvrIndx)))
      {
	
	int schToTxInfoIndx=bwsimSystem.schedulerOutput(srvrIndices(srvrIndx)).getSchedulerToTransmitterInfoIndx(uldlServerNodes.dlServerNodes(srvrIndx),nuID,beamID);
	bwsimSystem.myNodeDevices(dlDeviceIndices(srvrIndx)).L1CorePerNU(nuIndx).dlFrameStructure.setCurrentFrameType("dlFrame");
	
	double csirsSubcarrierPowerInDBm=bwsimSystem.mySysInfo.serverNodeTxPowersInDBm(srvrIndices(srvrIndx))-10*log10(bwsimSystem.myNodeDevices(dlDeviceIndices(srvrIndx)).L1CorePerNU(nuIndx).resourceMapper.dlResourceMapper.getUsedSubcarriers());
	double carrierInMHz=(double)getCarrierFreqInMHz(nuID);
	if(schToTxInfoIndx==-1) // BS Unscheduled
	{
	  bwsimSystem.schedulerOutput(srvrIndices(srvrIndx)).addTxForRSTransmission(uldlServerNodes.dlServerNodes(srvrIndx),activeUEs,nuID,carrierInMHz,dlTTICountPerServerNode(srvrIndx),csirsSubcarrierPowerInDBm,_TRANSMISSION_TYPE_DL_,beamID);
	  
	  #ifdef USING_MPI
	  if(nTasks>1)
	  {
	    if(find(bwsimSystem.mySysInfo.mpiInfo.srvrIndxListPerTaskid(currentTaskid),srvrIndices(srvrIndx))==-1)
	      continue;
	  }
	  #endif
	  if(!bwsimSystem.mySysInfo.receptionModel.enableRestrictedRecepition || (bwsimSystem.mySysInfo.receptionModel.enableRestrictedRecepition && find(bwsimSystem.mySysInfo.receptionModel.serverNodesWithActualReception,uldlServerNodes.dlServerNodes(srvrIndx))!=-1))
	  {  
          for(int srvc_cnt=0;srvc_cnt<activeUEs.length();srvc_cnt++)
              bwsimSystem.schedulerOutput(srvrIndices(srvrIndx)).addRxForRSTransmission(activeUEs(srvc_cnt),uldlServerNodes.dlServerNodes(srvrIndx),nuID,carrierInMHz,dlTTICountPerServerNode(srvrIndx),csirsSubcarrierPowerInDBm,_TRANSMISSION_TYPE_DL_,beamID);
	  }
	}
	else
	{
	  ivec unscheduledUEs=remove(activeUEs,bwsimSystem.schedulerOutput(srvrIndices(srvrIndx)).schedulerToTransmitterInfo(schToTxInfoIndx).rxNodeIDs);
	  if(unscheduledUEs.length())
	  {
	    bwsimSystem.schedulerOutput(srvrIndices(srvrIndx)).addTxForRSTransmission(uldlServerNodes.dlServerNodes(srvrIndx),unscheduledUEs,nuID,carrierInMHz,dlTTICountPerServerNode(srvrIndx),csirsSubcarrierPowerInDBm,_TRANSMISSION_TYPE_DL_,beamID);
	    #ifdef USING_MPI
	    if(nTasks>1)
	    {
	      if(find(bwsimSystem.mySysInfo.mpiInfo.srvrIndxListPerTaskid(currentTaskid),srvrIndices(srvrIndx))==-1)
		continue;
	    }
	    #endif
	    if(!bwsimSystem.mySysInfo.receptionModel.enableRestrictedRecepition || (bwsimSystem.mySysInfo.receptionModel.enableRestrictedRecepition && find(bwsimSystem.mySysInfo.receptionModel.serverNodesWithActualReception,uldlServerNodes.dlServerNodes(srvrIndx))!=-1))
	    {  
	      for(int srvc_cnt=0;srvc_cnt<unscheduledUEs.length();srvc_cnt++)
		bwsimSystem.schedulerOutput(srvrIndices(srvrIndx)).addRxForRSTransmission(unscheduledUEs(srvc_cnt),uldlServerNodes.dlServerNodes(srvrIndx),nuID,carrierInMHz,dlTTICountPerServerNode(srvrIndx),csirsSubcarrierPowerInDBm,_TRANSMISSION_TYPE_DL_,beamID);
	    }
	  }
	}
      }
    }
  }
  
  if(uldlServerNodes.ulServerNodes.length())  //! UL SRS Scheduling
  {
    ivec srvrIndices=find(bwsimSystem.associationInfo.serverNodes,uldlServerNodes.ulServerNodes);
    ivec srvrDeviceIndices=find(bwsimSystem.nodeDeviceIDs,uldlServerNodes.ulServerNodes);
    ivec ulTTICountPerServerNode=getTTICount(bwsimSystem.nodeDeviceIDs,bwsimSystem.myNodeDevices,uldlServerNodes.ulServerNodes,nuID);
    
    for(int srvr_cnt=0;srvr_cnt<srvrIndices.length();srvr_cnt++)
    {
      int srvrNuIndx=bwsimSystem.myNodeDevices(srvrDeviceIndices(srvr_cnt)).getNUIndx(nuID);
      ivec activeUEs=bwsimSystem.myNodeDevices(srvrDeviceIndices(srvr_cnt)).L2CorePerNU(srvrNuIndx).activeUEs;
      ivec ulDeviceIndices=find(bwsimSystem.nodeDeviceIDs,activeUEs);
      ivec srvcIndices=find(bwsimSystem.associationInfo.serviceNodes,activeUEs);

//       ivec beamAssociatedUsers;
//   beamAssociatedUsers.set_length(0);  
//   ivec index;
//   index.set_length(0);
//   
//        for(int i=0; i<bwsimSystem.myNodeDevices(srvrDeviceIndices(srvrIndx)).L2CorePerNU(srvrNuIndx).beamPairLinkperUE.length(); i++)
//   {
//       if(bwsimSystem.myNodeDevices(srvrDeviceIndices(srvrIndx)).L2CorePerNU(srvrNuIndx).beamPairLinkperUE(i).rxBeamID == beamID)
//           if(find(activeUEs,bwsimSystem.myNodeDevices(srvrDeviceIndices(srvrIndx)).L2CorePerNU(srvrNuIndx).associatedUEs(i))!=-1)    
//           {
//               append(beamAssociatedUsers,bwsimSystem.myNodeDevices(srvrDeviceIndices(srvrIndx)).L2CorePerNU(srvrNuIndx).associatedUEs(i));
//               append(index, i);
//           }
//   }
//     
  
  
  for(int srvc_cnt=0;srvc_cnt<ulDeviceIndices.length();srvc_cnt++)
      {
	int nuIndx=bwsimSystem.myNodeDevices(ulDeviceIndices(srvc_cnt)).getNUIndx(nuID);
	if(bwsimSystem.myNodeDevices(ulDeviceIndices(srvc_cnt)).L1CorePerNU(nuIndx).ulTxConfig.ulCQIModeInfo.isToTransmitCQIPorts(ulTTICountPerServerNode(srvr_cnt)))
	{
        int schToTxInfoIndx=bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).getSchedulerToTransmitterInfoIndx(activeUEs(srvc_cnt),nuID,beamID);
	  if(schToTxInfoIndx==-1) // UE Unscheduled
	  {
	    bwsimSystem.myNodeDevices(ulDeviceIndices(srvc_cnt)).L1CorePerNU(nuIndx).ulFrameStructure.setCurrentFrameType("ulFrame");
	    
	    //!FIXME Power control needs to be properly taken care - Dhiv
	    double srsSubcarrierPowerInDBm=bwsimSystem.mySysInfo.serviceNodeTxPowersInDBm(srvcIndices(srvc_cnt))-10*log10(300); // Hardcoding for SRS subcarriers
	    double carrierInMHz=(double)getCarrierFreqInMHz(nuID);
        bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).addTxForRSTransmission(activeUEs(srvc_cnt),uldlServerNodes.ulServerNodes(srvr_cnt,srvr_cnt),nuID,carrierInMHz,ulTTICountPerServerNode(srvr_cnt),srsSubcarrierPowerInDBm,_TRANSMISSION_TYPE_UL_,beamID);
	 
	    #ifdef USING_MPI
	    if(nTasks>1)
	    {
	      if(find(bwsimSystem.mySysInfo.mpiInfo.srvrIndxListPerTaskid(currentTaskid),srvrIndices(srvr_cnt))==-1)
		continue;
	    }
	    #endif
	    
	    if(!bwsimSystem.mySysInfo.receptionModel.enableRestrictedRecepition || (bwsimSystem.mySysInfo.receptionModel.enableRestrictedRecepition && find(bwsimSystem.mySysInfo.receptionModel.serverNodesWithActualReception,uldlServerNodes.ulServerNodes(srvr_cnt))!=-1))
            bwsimSystem.schedulerOutput(srvrIndices(srvr_cnt)).addRxForRSTransmission(uldlServerNodes.ulServerNodes(srvr_cnt),activeUEs(srvc_cnt),nuID,carrierInMHz,ulTTICountPerServerNode(srvr_cnt),srsSubcarrierPowerInDBm,_TRANSMISSION_TYPE_UL_,beamID);
	  }
	}
      }
    }
  }
  
}

Array<TxFrameInfo_S> nodeGenerate(BWSimSystem_S &bwsimSystem, ivec txNodeIDs,double currentTime, int nuID,ivec txBeamIDs)
{
  nodePrepare(bwsimSystem.schedulerOutput,bwsimSystem.associationInfo,bwsimSystem.myNodeDevices,bwsimSystem.nodeDeviceIDs,txNodeIDs,bwsimSystem.mySysInfo.isWithAbstraction,bwsimSystem.mySysInfo.AbstractionModel,nuID,currentTime,txBeamIDs,true);
  Array< Transmitter* > myTransmitters=getTransmitterPointer(bwsimSystem.myNodeDevices,bwsimSystem.nodeDeviceIDs,txNodeIDs,nuID);
  ivec txAntennaCountForTheBeam=getAntennaCountForBeam(bwsimSystem.mySys,txNodeIDs,nuID,txBeamIDs);
  for(int tx_cnt=0;tx_cnt<myTransmitters.length();tx_cnt++)
  {
    myTransmitters(tx_cnt)->setAntennaCount(txAntennaCountForTheBeam(tx_cnt));
    myTransmitters(tx_cnt)->frameHandler.setAntennaCount(txAntennaCountForTheBeam(tx_cnt),true);
  }
  return(nodeGenerate(myTransmitters,bwsimSystem.associationInfo,bwsimSystem.schedulerOutput,nuID,bwsimSystem.mySysInfo,txNodeIDs,txBeamIDs));
}

void nodeTransmit(BWSimSystem_S &bwsimSystem, ivec txNodeIDs,int nuID,ivec txBeamIDs)
{
  
  nodeTransmit(bwsimSystem.mySysInfo,bwsimSystem.myNodeDevices,bwsimSystem.nodeDeviceIDs,bwsimSystem.mySys,bwsimSystem.myTxFrameInfo,txNodeIDs,nuID,txBeamIDs);
}

Array<StrongAndWeakTransmittersInfo_S> getStrongAndWeakTransmittersInfo(BWSimSystem_S &bwsimSystem, int nuID, ivec rxNodeIDs,double currentTime)
{
  double carrierInMHz=(double)getCarrierFreqInMHz(nuID);
  Array<StrongAndWeakTransmittersInfo_S> strongAndWeakTransmittersInfo=getStrongAndWeakTransmittersInfo(rxNodeIDs,bwsimSystem.mySysInfo,bwsimSystem.mySys,bwsimSystem.schedulerOutput,bwsimSystem.associationInfo, currentTime);
  return strongAndWeakTransmittersInfo;
}

ivec nodeReceive(BWSimSystem_S& bwsimSystem, ivec rxNodeIDs, int nuID, double currentTime, ivec &rxBeamIDs)
{
    Array<StrongAndWeakTransmittersInfo_S> strongAndWeakTransmittersInfo=getStrongAndWeakTransmittersInfo(bwsimSystem,nuID, rxNodeIDs,currentTime);
      
    bvec forceReceptionFlagForCoMPRx(0);

    ivec rxDeviceIndx=find(bwsimSystem.nodeDeviceIDs,rxNodeIDs);
    ivec rxNodesToProcess(0);
    for(int rx_cnt=0;rx_cnt<rxNodeIDs.length();rx_cnt++)
    {
      int nuIndx=bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).getNUIndx(nuID);  
      if(nuIndx == -1)
      {
	continue;
      }
       int rxNodeID=rxNodeIDs(rx_cnt);
//        bwsimSystem.mySys.aNodes(rxNodeID).setCarrierFreq(bwsimSystem.mySysInfo.carrierAggregation.getUniqueCarrierFreqsInMHz()(bwsimSystem.carrier_cnt)*1e6);
      int srvrIndx;
      srvrIndx=getServerIndx(bwsimSystem.associationInfo,rxNodeID);
      
 
      if(traceValuePerTraceName["EnablePrints"]=="true")
          cout<<"[detl:]Receiving subframe for "<< rxNodeIDs(rx_cnt) <<" from : "<<strongAndWeakTransmittersInfo(rx_cnt).strongTransmitters<<" in  nuID : "<<nuID<<" using beam : "<<rxBeamIDs(rx_cnt)<<endl;
      
      int schToRxInfoIndx=bwsimSystem.schedulerOutput(srvrIndx).getSchedulerToReceiverInfoIndx(rxNodeID,nuID,rxBeamIDs(rx_cnt));
      SchedulerToReceiverInfo_S schToRxInfo;
      bool isScheduled=false;

      schToRxInfo=bwsimSystem.schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoIndx);
      if(schToRxInfo.dataBlkInfo.length())
        isScheduled=true;
      
      
      //       int nuIndx=bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).getNUIndx(nuID);  
//       cout<<"nuIndex ::"<<nuIndx<<endl;
      int forceReception=0;		

      bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).enterReceiveLoop= false;
     
      if(bwsimSystem.mySysInfo.compInfo.isWithCoMP && bwsimSystem.mySysInfo.compInfo.compScheme==_COMP_SCHEME_JOINT_RX_)
	forceReception=(int)forceReceptionFlagForCoMPRx(rx_cnt);
    
     TransmissionType txType=bwsimSystem.schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoIndx).txType;
   
	bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).enterReceiveLoop=receiveSubframe(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).rxFrameInfo,&bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx),bwsimSystem.mySysInfo,bwsimSystem.mySys,bwsimSystem.associationInfo,strongAndWeakTransmittersInfo(rx_cnt),txType,isScheduled,currentTime,schToRxInfo,rxBeamIDs(rx_cnt),forceReception);
      
      // Happens during restricted reception and nodes belonging to other MPI tasks
      if(!bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).enterReceiveLoop && bwsimSystem.schedulerOutput(srvrIndx).getSchedulerToReceiverInfoIndx(rxNodeID, nuID,rxBeamIDs(rx_cnt))!=-1)
      {
	bwsimSystem.schedulerOutput(srvrIndx).deleteSchedulerToReceiverInfo(rxNodeID, nuID,rxBeamIDs(rx_cnt));
      }
      else
	append(rxNodesToProcess,rxNodeID);

      bool isDownlink=(find(bwsimSystem.mySysInfo.serviceNodes,rxNodeID)!=-1);
    if(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).enterReceiveLoop && isScheduled && !isDownlink)
      {
	double avgInterferencePowerPerREInWatts;
	if(bwsimSystem.mySys.channelCloud.isTimeDomain())
	  avgInterferencePowerPerREInWatts=mean(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).rxFrameInfo.summedUpPowerOverREs)/(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).rxFrameInfo.rxFrame(0).cols()*bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getUsedSubcarriers());
	else
	  avgInterferencePowerPerREInWatts=sum(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).rxFrameInfo.summedUpPowerOverREs.get(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getGuardSubcarriersInOneSide(),bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).rxFrameInfo.summedUpPowerOverREs.length()-1-bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getGuardSubcarriersInOneSide()))/(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).rxFrameInfo.rxFrame(0).cols()*bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).L1CorePerNU(nuIndx).resourceMapper.ulResourceMapper.getUsedSubcarriers());
	
// 	cout<<"avgInterferencePowerPerREInWatts : "<<avgInterferencePowerPerREInWatts<<endl;abort();
	
	
	double noiseVariance = 2.0*bwsimSystem.mySys.channelCloud.getNoiseVariancePerDimension(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).rxFrameInfo.rxNodeID);
	double instantIoTperRE = avgInterferencePowerPerREInWatts/noiseVariance;
	updateNIforUplink(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).L1CorePerNU(nuIndx).NI,avgInterferencePowerPerREInWatts,bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).L1CorePerNU(nuIndx).ulTxConfig.powerControlInfo,bwsimSystem.schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoIndx).ttiCount);
	
	double avgIoTperRE = bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).L1CorePerNU(nuIndx).NI/noiseVariance;
	printIoTvalues(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).receiverPerNU(nuIndx).rxFrameInfo.rxNodeID,bwsimSystem.schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoIndx).ttiCount,avgInterferencePowerPerREInWatts,bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt)).L1CorePerNU(nuIndx).NI,instantIoTperRE,avgIoTperRE,bwsimSystem.mySysInfo.resultFiles.directory+"IoTLogs/");
      }
    }
    
    return rxNodesToProcess;
	
}

void nodeProcess(BWSimSystem_S& bwsimSystem, ivec rxNodeIDs, double currentTime, int nuID, ivec rxBeamIDs)
{
  nodePrepare(bwsimSystem.schedulerOutput,bwsimSystem.associationInfo,bwsimSystem.myNodeDevices,bwsimSystem.nodeDeviceIDs,rxNodeIDs,bwsimSystem.mySysInfo.isWithAbstraction,bwsimSystem.mySysInfo.AbstractionModel,nuID,currentTime,rxBeamIDs,false);
  ivec rxDeviceIndx=find(bwsimSystem.nodeDeviceIDs,rxNodeIDs);
  for(int rx_cnt1=0;rx_cnt1<rxNodeIDs.length();rx_cnt1++)
  {
    int rxNodeID=rxNodeIDs(rx_cnt1);
    int srvrIndx;
    srvrIndx = getServerIndx(bwsimSystem.associationInfo,rxNodeID);
    int associatedServerNode=bwsimSystem.associationInfo.serverNodes(srvrIndx);
    int schToRxInfoIndx=bwsimSystem.schedulerOutput(srvrIndx).getSchedulerToReceiverInfoIndx(rxNodeID,nuID,rxBeamIDs(rx_cnt1));
    bool isScheduled=false;
    if(schToRxInfoIndx!=-1 && bwsimSystem.schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoIndx).dataBlkInfo.length())
      isScheduled=true;
    
    int rxSrvcIndx=find(bwsimSystem.mySysInfo.serviceNodes,rxNodeID);
    TransmissionType txType=bwsimSystem.schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoIndx).txType;
   
    int nuIndx=bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).getNUIndx(nuID);
    PerformanceMetrics_S *performanceMetric;
//     if(find(bwsimSystem.mySysInfo.multiopInfo.secondNetworkNodes,rxNodeID)==-1) //! This condition for not to simulate second-network when "isMultiOperator" falg is true since ACI frame overlaps the Tx frame of Second-Network
    {
      
      int txBeamIDForCodebook=0;
      if(txType==_TRANSMISSION_TYPE_DL_)
      {
        performanceMetric=&bwsimSystem.dlPerformanceMetric;
        txBeamIDForCodebook=bwsimSystem.associationInfo.associatedBeamPairLinkforEachServiceNode(rxSrvcIndx).srvrBeamID;
      }
      else if(txType==_TRANSMISSION_TYPE_UL_)
      {
        performanceMetric=&bwsimSystem.ulPerformanceMetric;
        txBeamIDForCodebook=0;  //BeamID considered as for UL as codebook is beam independent
      }
      else if(txType==_TRANSMISSION_TYPE_SL_)
      {
          performanceMetric=&bwsimSystem.slPerformanceMetric;
          txBeamIDForCodebook=0;  //BeamID considered as for UL as codebook is beam independent
      }
      
//       if(traceValuePerTraceName["frameHandlerLevel01TraceInfo"]=="onHold")
// 	if(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).getRxNodeID()==bwsimSystem.schedulerOutput(bwsimSystem.myTransmitters(0)->getCentralUnitIndx()).getSchedulerToTransmitterInfo(bwsimSystem.myTransmitters(0)->getNodeID()).rxNodeIDs(0))	    traceValuePerTraceName["frameHandlerLevel01TraceInfo"]="printInRx";
// 	
	
	/// updating CQI Config if changed by Scheduler(RRC)

	if(txType==_TRANSMISSION_TYPE_DL_ && bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).isLinkAdaptationEnabled())
	{  
	  bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).cqiEstimator.downlinkCQIInfo->updateCQIComputationInfoFromCQIConfig(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).L1CorePerNU(nuIndx).dlCQIComputationConfig,rxNodeID);
	  bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).L1CorePerNU(nuIndx).dlCQIComputationConfig.isChanged=false; //Added for mode adaptation
	}
	bool isDownlink = txType==_TRANSMISSION_TYPE_DL_;
	int nu_Indx = getNUID(nuID) -1;
	if(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).enterReceiveLoop)
	{
	  bool genFB=true;
      if(txType==_TRANSMISSION_TYPE_UL_)
	  {
	    if(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).isLinkAdaptationEnabled())
	    {
	      if(bwsimSystem.mySysInfo.compInfo.isWithCoMP)
		bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).generateFeedback(nuID,txBeamIDForCodebook,bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).ttiCountPerNU(nuIndx),bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo,bwsimSystem.mySys.channelCloud,isDownlink,*performanceMetric,bwsimSystem.mySysInfo,bwsimSystem.mySysInfo.resultFiles.WideBandCQIlogsFileName(nu_Indx),bwsimSystem.mySysInfo.resultFiles.RIlogsFileName(nu_Indx),bwsimSystem.mySysInfo.resultFiles.SubbandCQIlogsFileName(nu_Indx),bwsimSystem.mySysInfo.resultFiles.AperiodicCQIlogsFileName(nu_Indx),bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).L1CorePerNU(nuIndx).NI);
	      else
		bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).generateFeedback(nuID,txBeamIDForCodebook,bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).ttiCountPerNU(nuIndx),bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo,bwsimSystem.mySys.channelCloud,isDownlink,*performanceMetric,bwsimSystem.mySysInfo,bwsimSystem.mySysInfo.resultFiles.WideBandCQIlogsFileName(nu_Indx),bwsimSystem.mySysInfo.resultFiles.RIlogsFileName(nu_Indx),bwsimSystem.mySysInfo.resultFiles.SubbandCQIlogsFileName(nu_Indx),bwsimSystem.mySysInfo.resultFiles.AperiodicCQIlogsFileName(nu_Indx),bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).L1CorePerNU(nuIndx).NI);    
	    } 
	  }
	  else if(txType==_TRANSMISSION_TYPE_DL_)
	  {
	    //CQI The computation of periodic CQI/PMI or Aperiodic CQI/PMI if triggered is done in generateFeedback
	    if(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).isLinkAdaptationEnabled())
	    {
	      if(genFB)
	      {
		if(bwsimSystem.mySysInfo.compInfo.isWithCoMP)
		  bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).generateFeedback(nuID,txBeamIDForCodebook,bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).ttiCountPerNU(nuIndx),bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo,bwsimSystem.mySys.channelCloud,isDownlink,*performanceMetric,bwsimSystem.mySysInfo,bwsimSystem.mySysInfo.resultFiles.WideBandCQIlogsFileName(nu_Indx),bwsimSystem.mySysInfo.resultFiles.RIlogsFileName(nu_Indx),bwsimSystem.mySysInfo.resultFiles.SubbandCQIlogsFileName(nu_Indx),bwsimSystem.mySysInfo.resultFiles.AperiodicCQIlogsFileName(nu_Indx));
		else
		  bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).generateFeedback(nuID,txBeamIDForCodebook,bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).ttiCountPerNU(nuIndx),bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo,bwsimSystem.mySys.channelCloud,isDownlink,*performanceMetric,bwsimSystem.mySysInfo,bwsimSystem.mySysInfo.resultFiles.WideBandCQIlogsFileName(nu_Indx),bwsimSystem.mySysInfo.resultFiles.RIlogsFileName(nu_Indx),bwsimSystem.mySysInfo.resultFiles.SubbandCQIlogsFileName(nu_Indx),bwsimSystem.mySysInfo.resultFiles.AperiodicCQIlogsFileName(nu_Indx));
        
   if(bwsimSystem.mySysInfo.ulDlMode == _ULDL_MODE_TDD_)
   {
   bwsimSystem.mySysInfo.weakIntr(rxNodeID)=bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo.weakIntr;
  //cout<<"itr"<<bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo.intrCov.length()<<endl;
   if(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo.intrCov.length()!=0)
   {
   bwsimSystem.mySysInfo.intrCov(rxNodeID)= bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo.intrCov;
   bwsimSystem.mySysInfo.noiseVar(rxNodeID)=bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo.noiseVar;
   //cout<<"successfully stored intr values in system info"<<endl;
   //clearing variables
   bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo.weakIntr=0.0;
   bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo.intrCov.set_length(0);
   bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo.noiseVar=0.0;
   }
    else
   {
       cout<<"Error in storing of interference values for DL reciprocity"<<endl;
       //abort();
   }
   }
	      }
	    }
	  }
	  
		
		Array<TransmissionStatus_S> txStatus(0);		
		if(isScheduled)
		{
		  SchedulerToReceiverInfo_S mySchedulerToReceiverInfo=bwsimSystem.schedulerOutput(srvrIndx).schedulerToReceiverInfo(schToRxInfoIndx);
		  bwsimSystem.schedulerOutput(srvrIndx).deleteSchedulerToReceiverInfo(rxNodeID,nuID,rxBeamIDs(rx_cnt1));
		 
          bool isDownlink=(txType==_TRANSMISSION_TYPE_DL_); // SL considered as uplink for reception procedure. For IAB need to change
          if(bwsimSystem.mySysInfo.compInfo.isWithCoMP)
		  {
		    txStatus = bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).processReceivedSubFrame(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo,bwsimSystem.mySysInfo,isDownlink,performanceMetric,mySchedulerToReceiverInfo,bwsimSystem.mySys.channelCloud,bwsimSystem.mySysInfo.resultFiles.MCSlogsFileName(getNUID(nuID)-1));
		  }
		  else 
		  {
              txStatus = bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).processReceivedSubFrame(bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo,bwsimSystem.mySysInfo,isDownlink,performanceMetric,mySchedulerToReceiverInfo,bwsimSystem.mySys.channelCloud,bwsimSystem.mySysInfo.resultFiles.MCSlogsFileName(getNUID(nuID)-1));
		  }
		 
		for(int blk_cnt=0;blk_cnt<mySchedulerToReceiverInfo.dataBlkInfo.length();blk_cnt++)
		{
            if(txType==_TRANSMISSION_TYPE_DL_)
              nodeReportHARQ(bwsimSystem,mySchedulerToReceiverInfo.rxNodeID,mySchedulerToReceiverInfo.txNodeIDs(blk_cnt),bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).ttiCountPerNU(nuIndx),txStatus(blk_cnt),txType,nuID);
            else if(txType==_TRANSMISSION_TYPE_UL_)
              nodeReportHARQ(bwsimSystem,mySchedulerToReceiverInfo.txNodeIDs(blk_cnt),mySchedulerToReceiverInfo.rxNodeID,bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).ttiCountPerNU(nuIndx),txStatus(blk_cnt),txType,nuID);
		    else if(txType==_TRANSMISSION_TYPE_SL_) // Harq report updated at the associatedServerNode - Dhiv
                nodeReportHARQ(bwsimSystem,mySchedulerToReceiverInfo.rxNodeID,associatedServerNode,bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).ttiCountPerNU(nuIndx),txStatus(blk_cnt),txType,nuID);
          }
		
		bwsimSystem.myNodeDevices(rxDeviceIndx(rx_cnt1)).receiverPerNU(nuIndx).rxFrameInfo.clear();

	    }
        }
    }

  }
}

void nodeReportHARQ(BWSimSystem_S& bwsimSystem, int srvcNode, int srvrNode, int ttiCount, TransmissionStatus_S &txStatus, TransmissionType txType, int nuID)
{
  int srvrNodeDeviceIndx=find(bwsimSystem.nodeDeviceIDs,srvrNode);
  int srvrNuIndx=bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).getNUIndx(nuID);
  
  int srvcIndx=find(bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).associatedUEs,srvcNode);
  if(txType==_TRANSMISSION_TYPE_DL_)
  {
    bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).dlHarqManager(srvcIndx).setReceptionStatus(txStatus.isSuccess,txStatus.isSuccessPerCB,txStatus.harqProcessID);
    if(bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).outerLoopLinkAdaptationInfoForDL.isEnabled)
      bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).outerLoopLinkAdaptationInfoForDL.update(srvcNode,txStatus.isSuccess,txStatus.isNewTransmission,ttiCount,mean(txStatus.mcsRatePerCW));
  }
  else
  {
    bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).ulHarqManager(srvcIndx).setReceptionStatus(txStatus.isSuccess,txStatus.isSuccessPerCB,txStatus.harqProcessID);
    if(bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).outerLoopLinkAdaptationInfoForUL.isEnabled)
      bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).outerLoopLinkAdaptationInfoForUL.update(srvcNode,txStatus.isSuccess,txStatus.isNewTransmission,ttiCount,mean(txStatus.mcsRatePerCW));
  }
//   else if(txType==_TRANSMISSION_TYPE_SL_)
//   {
//       bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).l2Device.eNB.fGetHarqManager(srvcNode,txType).setReceptionStatus(txStatus.isSuccess,txStatus.isSuccessPerCB,txStatus.harqProcessID);
//       if(bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).l2Device.eNB.slScheduler.tOuterLoopLinkAdaptation.isEnabled)
//           bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).l2Device.eNB.slScheduler.tOuterLoopLinkAdaptation.update(srvcNode,txStatus.isSuccess,txStatus.isNewTransmission,ttiCount,mean(txStatus.mcsRatePerCW));
//   }
}

sCqiList makeDLCqiList(PerUserDLCQIFeedback_S dlCQI)
{
  sCqiList tCqiList;

  tCqiList.rnti = dlCQI.serviceNodeID;
  tCqiList.tx_scheme = _MODE_NR_;
  
  if(tCqiList.tx_scheme == _MODE_NR_)
  {
    if(dlCQI.isAperiodicFeedback)
    {
      if(dlCQI.aPeriodicReportingMode == _APERIODIC_MODE_3_0_)
      {
	
      }
      else if(dlCQI.aPeriodicReportingMode == _APERIODIC_MODE_3_1_)
      {
	
      }  
      else if(dlCQI.aPeriodicReportingMode == _APERIODIC_MODE_3_2_) // at present only 3_2 updated
      {
	tCqiList.multiCqiList.resize(dlCQI.dlCQIMeasurement.length());
	
	if(dlCQI.dlCQIMeasurement.length() == 0)
	{
	  cout<<"[both:] At least one dlCQIMeasurement should be available"<<endl;abort();
	}
	tCqiList.cqiType = A32;
	tCqiList.multiCqiList[0].cqiType = A32;
	  
	if(dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.isValid)
	{
	  tCqiList.multiCqiList[0].nrCodeBookType = _eNR_CODEBOOK_TYPE_I_;
	  tCqiList.multiCqiList[0].wb_pmi = (uint8_t)dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.widebandPMI;
	  tCqiList.multiCqiList[0].wb_i1_1 = (uint8_t)dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.wideBandPrecoderIndi1_1;
	  tCqiList.multiCqiList[0].wb_i1_2 = (uint8_t)dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.wideBandPrecoderIndi1_2;
	  tCqiList.multiCqiList[0].wb_i1_3 = (uint8_t)dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.wideBandPrecoderIndi1_3;
      
	  for(int cw_cnt=0;cw_cnt<dlCQI.cqiMeasurementConfig(0).numberOfCodeWords;cw_cnt++)
	  {
	    tCqiList.multiCqiList[0].wb_cqi[cw_cnt] = (uint8_t)dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.wbCqiIndexPerCodeWord(cw_cnt);
	  }
	  
	  for(int nSubband=0;nSubband<dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.sbCqiIndexPerCodeWord.size();nSubband++)
	  {
	    if(dlCQI.cqiMeasurementConfig(0).numberOfCodeWords == 1)
	    {
	      if(dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband).length())
		tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[0]=(uint8_t)dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband)(0);
	      else
		tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[0] = NOT_DEFINED_8BIT;
	      
	      tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[1] = NOT_DEFINED_8BIT;
	    }
	    else if(dlCQI.cqiMeasurementConfig(0).numberOfCodeWords == 2)
	    {
	      if(dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband).length())
	      {
		tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[0] = (uint8_t)dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband)(0);
		tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[1] = (uint8_t)dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband)(1);
	      }
	      else
	      {
		tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[0] = NOT_DEFINED_8BIT;
		tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[1] = NOT_DEFINED_8BIT;
	      }
	    }
	    else{
	      cout<<"Unsupported number of codewords .. Aborting ..."<<endl;
	      abort();
	    }	
	    
	    if(dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.channelPerSubBand.length()!= 0)
	    {
	      tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_channel = dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.channelPerSubBand(nSubband);
	    }
	    
	    
	    // Higher layer selected PMIs for subbands
	    if(dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.subbandPrecIndi2.length()!= 0)
	    {
	      tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_pmi =(uint8_t)dlCQI.dlCQIMeasurement(0).nrCQIInfoAPeriodic.subbandPrecIndi2(nSubband);
	    }
	    else{
	      cout<<"_APERIODIC_MODE_3_2_ mode should report subband pmi..  aborting..."<<endl;abort();
	    }
	  }
	  
	  if(dlCQI.dlCQIMeasurement(0).rankAperiodic != -1)
	  {
	    tCqiList.multiCqiList[0].ri = (uint8_t)dlCQI.dlCQIMeasurement(0).rankAperiodic;
	  }
	  else
	  {
	    tCqiList.multiCqiList[0].ri = NOT_DEFINED_8BIT;
	  }
	}
	else if(dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.isValid)
    {
        tCqiList.multiCqiList[0].nrCodeBookType = _eNR_CODEBOOK_TYPE_I_MULTIPANEL_;
        tCqiList.multiCqiList[0].wb_pmi = (uint8_t)dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.widebandPMI;
        tCqiList.multiCqiList[0].wb_i1_1 = (uint8_t)dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.wideBandPrecoderIndi1_1;
        tCqiList.multiCqiList[0].wb_i1_2 = (uint8_t)dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.wideBandPrecoderIndi1_2;
        tCqiList.multiCqiList[0].wb_i1_3 = (uint8_t)dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.wideBandPrecoderIndi1_3;
        tCqiList.multiCqiList[0].wb_i1_4 = (uint8_t)dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.wideBandPrecoderIndi1_4;
        
        for(int cw_cnt=0;cw_cnt<dlCQI.cqiMeasurementConfig(0).numberOfCodeWords;cw_cnt++)
        {
            tCqiList.multiCqiList[0].wb_cqi[cw_cnt] = (uint8_t)dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.wbCqiIndexPerCodeWord(cw_cnt);
        }
        
        for(int nSubband=0;nSubband<dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.sbCqiIndexPerCodeWord.size();nSubband++)
        {
            if(dlCQI.cqiMeasurementConfig(0).numberOfCodeWords == 1)
            {
                if(dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband).length())
                    tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[0]=(uint8_t)dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband)(0);
                else
                    tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[0] = NOT_DEFINED_8BIT;
                
                tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[1] = NOT_DEFINED_8BIT;
            }
            else if(dlCQI.cqiMeasurementConfig(0).numberOfCodeWords == 2)
            {
                if(dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband).length())
                {
                    tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[0] = (uint8_t)dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband)(0);
                    tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[1] = (uint8_t)dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband)(1);
                }
                else
                {
                    tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[0] = NOT_DEFINED_8BIT;
                    tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[1] = NOT_DEFINED_8BIT;
                }
            }
            else{
                cout<<"Unsupported number of codewords .. Aborting ..."<<endl;
                abort();
            }	
            
            if(dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.channelPerSubBand.length()!= 0)
            {
                tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_channel = dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.channelPerSubBand(nSubband);
            }
            
            
            // Higher layer selected PMIs for subbands
            if(dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.subbandPrecIndi2.length()!= 0)
            {
                tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_pmi =(uint8_t)dlCQI.dlCQIMeasurement(0).nrMPCQIInfoAPeriodic.subbandPrecIndi2(nSubband);
            }
            else{
                cout<<"_APERIODIC_MODE_3_2_ mode should report subband pmi..  aborting..."<<endl;abort();
            }
        }
        
        if(dlCQI.dlCQIMeasurement(0).rankAperiodic != -1)
        {
            tCqiList.multiCqiList[0].ri = (uint8_t)dlCQI.dlCQIMeasurement(0).rankAperiodic;
        }
        else
        {
            tCqiList.multiCqiList[0].ri = NOT_DEFINED_8BIT;
        }
    }
	else if(dlCQI.dlCQIMeasurement(0).nrTypeIICQIInfoAPeriodic.isValid)
	{
	  tCqiList.multiCqiList[0].nrCodeBookType = _eNR_CODEBOOK_TYPE_II_;
	  for(int cw_cnt=0;cw_cnt<dlCQI.cqiMeasurementConfig(0).numberOfCodeWords;cw_cnt++)
	  {
	    tCqiList.multiCqiList[0].wb_cqi[cw_cnt] = (uint8_t)dlCQI.dlCQIMeasurement(0).nrTypeIICQIInfoAPeriodic.wbCqiIndexPerCodeWord(cw_cnt);
	  }
	  
	  for(int nSubband=0;nSubband<dlCQI.dlCQIMeasurement(0).nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord.size();nSubband++)
	  {
	    if(dlCQI.cqiMeasurementConfig(0).numberOfCodeWords == 1)
	    {
	      if(dlCQI.dlCQIMeasurement(0).nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband).length())
		tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[0]=(uint8_t)dlCQI.dlCQIMeasurement(0).nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband)(0);
	      else
		tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[0] = NOT_DEFINED_8BIT;

	      tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[1] = NOT_DEFINED_8BIT;
	    }
	    else if(dlCQI.cqiMeasurementConfig(0).numberOfCodeWords == 2)
	    {
	      if(dlCQI.dlCQIMeasurement(0).nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband).length())
	      {
		tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[0] = (uint8_t)dlCQI.dlCQIMeasurement(0).nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband)(0);
		tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[1] = (uint8_t)dlCQI.dlCQIMeasurement(0).nrTypeIICQIInfoAPeriodic.sbCqiIndexPerCodeWord(nSubband)(1);
	      }
	      else
	      {
		tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[0] = NOT_DEFINED_8BIT;
		tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_cqi[1] = NOT_DEFINED_8BIT;
	      }
		
	    }
	    else{
	      cout<<"Unsupported number of codewords .. Aborting ..."<<endl;
	      abort();
	    }	
	    
	    if(dlCQI.dlCQIMeasurement(0).nrTypeIICQIInfoAPeriodic.channelPerSubBand.length()!= 0)
	    {
	      tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_channel = dlCQI.dlCQIMeasurement(0).nrTypeIICQIInfoAPeriodic.channelPerSubBand(nSubband);
	  
         }
	    
	    
	    // Higher layer selected precoders for subbands
	    if(dlCQI.dlCQIMeasurement(0).nrTypeIICQIInfoAPeriodic.sbPrecoder.length()!= 0)
	    {
	      tCqiList.multiCqiList[0].higherLyrSelected[nSubband].sb_precoder = dlCQI.dlCQIMeasurement(0).nrTypeIICQIInfoAPeriodic.sbPrecoder(nSubband);
	    }
	    else
	    {
	      cout<<"_APERIODIC_MODE_3_2_ mode should report subband precoder..  aborting..."<<endl;abort();
	    }
	  }
	  
	  if(dlCQI.dlCQIMeasurement(0).rankAperiodic != -1)
	  {
	    tCqiList.multiCqiList[0].ri = (uint8_t)dlCQI.dlCQIMeasurement(0).rankAperiodic;
	  }
	  else
	  {
	    tCqiList.multiCqiList[0].ri = NOT_DEFINED_8BIT;
	  }
	}
      }
    }
    else // Periodic
    {
    
      if(dlCQI.periodicReportingMode == _PERIODIC_MODE_1_0_)
      {
	tCqiList.cqiType = P10;
	tCqiList.wb_cqi[0] = (uint8_t)dlCQI.dlCQIMeasurement(0).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0);
	if(dlCQI.dlCQIMeasurement(0).rankPeriodic == 1)
	{
	  tCqiList.ri = 1;
	}
	else if(dlCQI.dlCQIMeasurement(0).rankPeriodic > 1)
	{
	  tCqiList.ri = (uint8_t)dlCQI.dlCQIMeasurement(0).rankPeriodic;
	  tCqiList.wb_cqi[1] = (uint8_t)dlCQI.dlCQIMeasurement(0).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(1);
	}
	else
	{
	  tCqiList.ri = NOT_DEFINED_8BIT;
	}
      }
      else if(dlCQI.periodicReportingMode == _PERIODIC_MODE_1_1_)
      {
	tCqiList.cqiType = P11;
	tCqiList.wb_cqi[0] = (uint8_t)dlCQI.dlCQIMeasurement(0).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(0);
	tCqiList.wb_pmi = (uint8_t)dlCQI.dlCQIMeasurement(0).widebandCqiInfoPerGroupPeriodic.pmiIndex;
	tCqiList.wb_i1 = (uint8_t)dlCQI.dlCQIMeasurement(0).widebandCqiInfoPerGroupPeriodic.WideBandPrecoderIndi1;
	if(dlCQI.dlCQIMeasurement(0).rankPeriodic == 1)
	{
	  tCqiList.ri = 1;
	}
	else if(dlCQI.dlCQIMeasurement(0).rankPeriodic > 1)
	{
	  tCqiList.ri = (uint8_t)dlCQI.dlCQIMeasurement(0).rankPeriodic;
	  tCqiList.wb_cqi[1] = (uint8_t)dlCQI.dlCQIMeasurement(0).widebandCqiInfoPerGroupPeriodic.cqiIndexPerCodeWord(1);
	}
	else
	{
	  tCqiList.ri = NOT_DEFINED_8BIT;
	}
      }

    }
  }
  else // LTE tx modes
  {
    if(dlCQI.isAperiodicFeedback)
    {
      if(dlCQI.aPeriodicReportingMode == _APERIODIC_MODE_3_0_)
      {
	tCqiList.cqiType = A30;
	for(int cw_cnt=0;cw_cnt<dlCQI.cqiMeasurementConfig(0).numberOfCodeWords;cw_cnt++)
	{
	  tCqiList.wb_cqi[cw_cnt] = (uint8_t)dlCQI.dlCQIMeasurement(0).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(cw_cnt);
	}
	
	for(int nSubband=0;nSubband<dlCQI.dlCQIMeasurement(0).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord.size();nSubband++)
	{
	  for(int cw_cnt=0;cw_cnt<dlCQI.cqiMeasurementConfig(0).numberOfCodeWords;cw_cnt++)
	  {
	    tCqiList.higherLyrSelected[nSubband].sb_cqi[cw_cnt]=(uint8_t)dlCQI.dlCQIMeasurement(0).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(nSubband)(cw_cnt);
	  }
	}
	
	if(dlCQI.dlCQIMeasurement(0).rankAperiodic != -1)
	{
	  tCqiList.ri = (uint8_t)dlCQI.dlCQIMeasurement(0).rankAperiodic;
	}
	else
	{
	  tCqiList.ri = NOT_DEFINED_8BIT;
	}
      }
      else if(dlCQI.aPeriodicReportingMode == _APERIODIC_MODE_3_1_)
      {
	tCqiList.cqiType = A31;
	tCqiList.wb_pmi = (uint8_t)dlCQI.dlCQIMeasurement(0).widebandCqiInfoPerGroupAperiodic.widebandPMI;
	tCqiList.wb_i1 = (uint8_t)dlCQI.dlCQIMeasurement(0).widebandCqiInfoPerGroupAperiodic.WidebandPrecoderIndi1;
	for(int cw_cnt=0;cw_cnt<dlCQI.cqiMeasurementConfig(0).numberOfCodeWords;cw_cnt++)
	{
	  tCqiList.wb_cqi[cw_cnt] = (uint8_t)dlCQI.dlCQIMeasurement(0).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(cw_cnt);
	}
	cout<<"dlCQI.serviceNodeID:"<<dlCQI.serviceNodeID<<endl;
	cout<<"dlCQI.cqiMeasurementConfig(0).numberOfCodeWords"<<dlCQI.cqiMeasurementConfig(0).numberOfCodeWords<<endl;
	for(int nSubband=0;nSubband<dlCQI.dlCQIMeasurement(0).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord.size();nSubband++)
	{
	  // 	for(int cw_cnt=0;cw_cnt<dlCQI.cqiMeasurementConfig(0).numberOfCodeWords;cw_cnt++)
	  // 	{
	  if(dlCQI.cqiMeasurementConfig(0).numberOfCodeWords == 1)
	  {
	    tCqiList.higherLyrSelected[nSubband].sb_cqi[0]=(uint8_t)dlCQI.dlCQIMeasurement(0).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(nSubband)(0);
	    tCqiList.higherLyrSelected[nSubband].sb_cqi[1] = NOT_DEFINED_8BIT;
	  }
	  else if(dlCQI.cqiMeasurementConfig(0).numberOfCodeWords == 2)
	  {
	    tCqiList.higherLyrSelected[nSubband].sb_cqi[0] = (uint8_t)dlCQI.dlCQIMeasurement(0).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(nSubband)(0);
	    tCqiList.higherLyrSelected[nSubband].sb_cqi[1] = (uint8_t)dlCQI.dlCQIMeasurement(0).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(nSubband)(1);
	  }
	  else{
	    cout<<"Unsupported number of codewords .. Aborting ..."<<endl;
	    abort();
	  }
	}
	if(dlCQI.dlCQIMeasurement(0).rankAperiodic != -1)
	{
	  tCqiList.ri = (uint8_t)dlCQI.dlCQIMeasurement(0).rankAperiodic;
	}
	else
	{
	  tCqiList.ri = NOT_DEFINED_8BIT;
	}
      }  
      else if(dlCQI.aPeriodicReportingMode == _APERIODIC_MODE_3_2_) // FIXME copie ad 3_1 modify as per 3_2
      {
	
	tCqiList.cqiType = A32;
	tCqiList.wb_pmi = (uint8_t)dlCQI.dlCQIMeasurement(0).widebandCqiInfoPerGroupAperiodic.widebandPMI;
	tCqiList.wb_i1 = (uint8_t)dlCQI.dlCQIMeasurement(0).widebandCqiInfoPerGroupAperiodic.WidebandPrecoderIndi1;
    cout<<"dlCQI.serviceNodeID:"<<dlCQI.serviceNodeID<<endl;
    cout<<"dlCQI.cqiMeasurementConfig(0).numberOfCodeWords"<<dlCQI.cqiMeasurementConfig(0).numberOfCodeWords<<endl;
    for(int cw_cnt=0;cw_cnt<dlCQI.cqiMeasurementConfig(0).numberOfCodeWords;cw_cnt++)
	{
	  tCqiList.wb_cqi[cw_cnt] = (uint8_t)dlCQI.dlCQIMeasurement(0).widebandCqiInfoPerGroupAperiodic.cqiIndexPerCodeWord(cw_cnt);
	}
	cout<<"dlCQI.serviceNodeID:"<<dlCQI.serviceNodeID<<endl;
	cout<<"dlCQI.cqiMeasurementConfig(0).numberOfCodeWords"<<dlCQI.cqiMeasurementConfig(0).numberOfCodeWords<<endl;
	for(int nSubband=0;nSubband<dlCQI.dlCQIMeasurement(0).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord.size();nSubband++)
	{
	  // 	for(int cw_cnt=0;cw_cnt<dlCQI.cqiMeasurementConfig(0).numberOfCodeWords;cw_cnt++)
	  // 	{
	  if(dlCQI.cqiMeasurementConfig(0).numberOfCodeWords == 1)
	  {
	    tCqiList.higherLyrSelected[nSubband].sb_cqi[0]=(uint8_t)dlCQI.dlCQIMeasurement(0).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(nSubband)(0);
	    tCqiList.higherLyrSelected[nSubband].sb_cqi[1] = NOT_DEFINED_8BIT;
	  }
	  else if(dlCQI.cqiMeasurementConfig(0).numberOfCodeWords == 2)
	  {
	    tCqiList.higherLyrSelected[nSubband].sb_cqi[0] = (uint8_t)dlCQI.dlCQIMeasurement(0).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(nSubband)(0);
	    tCqiList.higherLyrSelected[nSubband].sb_cqi[1] = (uint8_t)dlCQI.dlCQIMeasurement(0).subbandCqiInfoPerGroupHigherLayer.cqiIndexPerCodeWord(nSubband)(1);
	  }
	  else{
	    cout<<"Unsupported number of codewords .. Aborting ..."<<endl;
	    abort();
	  }
	}
	if(dlCQI.dlCQIMeasurement(0).rankAperiodic != -1)
	{
	  tCqiList.ri = (uint8_t)dlCQI.dlCQIMeasurement(0).rankAperiodic;
	}
	else
	{
	  tCqiList.ri = NOT_DEFINED_8BIT;
	}
      
      }
    }
      
  }
  
  return tCqiList;
  
}

sCqiList makeULCqiList(PerUserULCQIFeedback_S ulCQI)
{
  sCqiList tCqiList;
  
  tCqiList.rnti = ulCQI.serviceNodeID;
  tCqiList.tx_scheme = (eTransmissionScheme)ulCQI.cqiMeasurementConfig(0).transmissionScheme;
  tCqiList.ri=ulCQI.subbandCqiInfoPerGroup(0).rank;
  tCqiList.cqiType = ULCQI;
  tCqiList.ulCqiReportList.resize(ulCQI.subbandCqiInfoPerGroup(0).subbandIndexPerRB.size());
  for (int iSB = 0; iSB < (int)tCqiList.ulCqiReportList.size(); iSB++)
  {
    sULCqiReport tULCqiReport;
    tULCqiReport.rb = iSB;
    tULCqiReport.sb = ulCQI.subbandCqiInfoPerGroup(0).subbandIndexPerRB(iSB);
    
    if(tULCqiReport.sb != -1)
    {  
      if(ulCQI.subbandCqiInfoPerGroup(0).pmiIndex.length()!=0)
      {
// 	if(ulCQI.subbandCqiInfoPerGroup(0).reciproPrecoderPerRB.length()!= 0)
// 	{
// 	  // If reciprocity based precoder available then precoder is set_length 
// 	  // pmi index also assigned in this case
// 	  // scheduler can select among them
// 	  
// 	  for(int j =0;j<ulCQI.subbandCqiInfoPerGroup(0).reciproPrecoderPerRB(tULCqiReport.sb).cols();++j)
// 	  {
// 	    vector <complex< double > > temp;
// 	    for(int i=0;i<ulCQI.subbandCqiInfoPerGroup(0).reciproPrecoderPerRB(tULCqiReport.sb).rows();++i)
// 	    {
// 	      temp.push_back(ulCQI.subbandCqiInfoPerGroup(0).reciproPrecoderPerRB(tULCqiReport.sb)(i,j));
// 	    }
// 	    tULCqiReport.sb_precoder.push_back(temp); 
// 	  }
// 
// 	  tULCqiReport.sb_pmi = ulCQI.subbandCqiInfoPerGroup(0).pmiIndex(tULCqiReport.sb);
// 	}
// 	else
	  tULCqiReport.sb_pmi = ulCQI.subbandCqiInfoPerGroup(0).pmiIndex(tULCqiReport.sb);
      }
      else
	tULCqiReport.sb_pmi = NOT_DEFINED_8BIT;
      
      if(ulCQI.subbandCqiInfoPerGroup(0).channelPerSubBand.length())
	tULCqiReport.sb_channel = ulCQI.subbandCqiInfoPerGroup(0).channelPerSubBand(tULCqiReport.sb);
      
      tULCqiReport.sb_SINR.push_back(ulCQI.subbandCqiInfoPerGroup(0).effSINRPerCodeWord(tULCqiReport.sb)(0));
      
      if (ulCQI.subbandCqiInfoPerGroup(0).effSINRPerCodeWord(tULCqiReport.sb).size() == 2)
      {
	tULCqiReport.sb_SINR.push_back(ulCQI.subbandCqiInfoPerGroup(0).effSINRPerCodeWord(tULCqiReport.sb)(1));
      }  
    }
  
    tCqiList.ulCqiReportList[iSB] = tULCqiReport;
  }
  return tCqiList;
}


void nodeReportDLCQI(BWSimSystem_S& bwsimSystem, int srvcNode, int srvrNode, int ttiCount, PerUserDLCQIFeedback_S dlCQI, int nuID)
{
  int srvrNodeDeviceIndx=find(bwsimSystem.nodeDeviceIDs,srvrNode);
  int srvrNuIndx=bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).getNUIndx(nuID);
  
  sCqiList cqiList = makeDLCqiList(dlCQI);
  bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).dlScheduler.fUpdateCQI(srvcNode,cqiList);
//   if(bwsimSystem.mySysInfo.ulDlMode == _ULDL_MODE_TDD_)
//   {
//     sCqiList ulCQIList =bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).l2Device.eNB.ulScheduler.fGetCQI(srvcNode);
//     for(int i=0;i<dlCQI.dlCQIMeasurement.length();i++)
//     {
//       if(dlCQI.cqiMeasurementConfig(i).isAssociatedWithUL && dlCQI.dlCQIMeasurement(i).reciproPrecoder.length() != 0)
//       {
//         for(int j= 0;j<ulCQIList.ulCqiReportList.size();j++)
//         {
//     // 	cout<<"Entered here .. 3:"<<endl;
//     // 	cout<<"dlCQI.dlCQIMeasurement(i).reciproPrecoder:"<<dlCQI.dlCQIMeasurement(i).reciproPrecoder<<endl;
//         ulCQIList.ulCqiReportList[j].reciprocity_precoder =  ITPPMatTostdVectOfVect(dlCQI.dlCQIMeasurement(i).reciproPrecoder(j));
//         }
//       }
//     }
//     bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).l2Device.eNB.ulScheduler.fUpdateCQI(srvcNode,ulCQIList);
//   }
}

void nodeReportULCQI(BWSimSystem_S& bwsimSystem, int srvcNode, int srvrNode, int ttiCount, PerUserULCQIFeedback_S ulCQI, int nuID)
{
  int srvrNodeDeviceIndx=find(bwsimSystem.nodeDeviceIDs,srvrNode);
  int srvrNuIndx=bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).getNUIndx(nuID);
  
  int srvcIndx=find(bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).associatedUEs,srvcNode);
  
  sCqiList cqiList = makeULCqiList(ulCQI);
 
  bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).ulScheduler.fUpdateCQI(srvcNode,cqiList);

  //reciprocity
  if(bwsimSystem.mySysInfo.ulDlMode == _ULDL_MODE_TDD_)
  {
    sCqiList dlCQIList =bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).dlScheduler.fGetCQI(srvcNode);
    dlCQIList.ulCqiReportList=cqiList.ulCqiReportList;
    
    for(int j= 0;j<ulCQI.subbandCqiInfoPerGroup(0).reciproPrecoderPerRB.length();j++)
    {
      if(ulCQI.subbandCqiInfoPerGroup(0).reciproPrecoderPerRB(j).size() !=0)
      {
  //     cout<<"Entered here .. 2:"<<endl;
  //     cout<<"reciproPrecoderPerRB:"<<ulCQI.subbandCqiInfoPerGroup(0).reciproPrecoderPerRB<<endl;
	dlCQIList.ulCqiReportList[j].reciprocity_precoder =  ITPPMatTostdVectOfVect(ulCQI.subbandCqiInfoPerGroup(0).reciproPrecoderPerRB(j));
    
    
      }
      if(ulCQI.subbandCqiInfoPerGroup(0).cqiIndxPerRBForDLReci(0)(j).size()!=0)
      {
         dlCQIList.ulCqiReportList[j].reciprocity_DLCQI= ulCQI.subbandCqiInfoPerGroup(0).cqiIndxPerRBForDLReci(0)(j);
         dlCQIList.ulCqiReportList[j].reciprocity_DLRank= ulCQI.subbandCqiInfoPerGroup(0).rankForDLReci;
         cout<<"Succesfully transferred in UL report"<<endl;
      }
    
    }
  
  
  
    bwsimSystem.myNodeDevices(srvrNodeDeviceIndx).L2CorePerNU(srvrNuIndx).dlScheduler.fUpdateCQI(srvcNode,dlCQIList);
  }
  
}


void printOutputFiles(BWSimSystem_S& bwsimSystem, double currentTime, int snrIndx)
{
    cout<<"Warning .. Output files printd assuming all subframe numbers are same.."<<endl;
  
    //Printing final results
    printOutputFiles(currentTime,bwsimSystem.dlPerformanceMetric,bwsimSystem.ulPerformanceMetric,bwsimSystem.slPerformanceMetric,bwsimSystem.mySysInfo,bwsimSystem.mySys,bwsimSystem.associationInfo,snrIndx);

}

void fGetBeamsPerBS(BWSimSystem_S& bwsimSystem)
{
  cout<<"Find the total beams per Basestation."<<endl;
 
 Array<sBeamPairLink> tBeamPairLink=bwsimSystem.associationInfo.associatedBeamPairLinkforEachServiceNode;
 
}

// void nodeTransmitSSB(BWSimSystem_S &bwsimSystem, ivec txNodeIDs,ivec txBeamIDs,ivec txPanelIDs, double currentTime)
// {
//     for(int nTxNode=0; nTxNode<length(txNodeIDs); nTxNode++)
//         nodeTxSSB(bwsimSystem.myNodeDevices(txNodeIDs(nTxNode)).SSBurstSetConfig,bwsimSystem.myNodeDevices(txNodeIDs(nTxNode)).L1CorePerNU(0).cellID,txNodeIDs(nTxNode), txBeamIDs(nTxNode), txPanelIDs(nTxNode), currentTime,bwsimSystem.mySys.channelCloud);
// }
// 
// void nodeReceiveSSB(BWSimSystem_S &bwsimSystem, ivec rxNodeIDs,double currentTime)
// {
//     
// }

double get_nextSSBlockTime(ssBurstSetConfig& ssbConfig,bool isSSBlockTime)
{
    double Tsa = 1/ssbConfig.getSamplingFrequency();
    int nfftSize=1/(Tsa*ssbConfig.getSSBscs()*1e3);
    ivec ssStartSymbols=ssbConfig.getssBurstSetConfig();
    int currentSSBstartSym=ssStartSymbols((ssStartSymbols.length()+ssbConfig.ssbIndx-1)%ssStartSymbols.length());
    ivec nextSymbols;
    double nextTiming=0.0;
    bool isSSB0=0;
    if(isSSBlockTime) 
        nextSymbols=getIntegers(currentSSBstartSym,currentSSBstartSym+3);
    else
    {
        if(ssbConfig.ssbIndx==0)
        {
            isSSB0=1;
            nextSymbols=getIntegers(ssStartSymbols(0),ssStartSymbols(ssStartSymbols.length()-1)-1);
        }
        
        else
            nextSymbols=getIntegers(currentSSBstartSym,ssStartSymbols(ssbConfig.ssbIndx)-1);
    }  
    
    nextSymbols=mod(nextSymbols,14);
    
    int totalCPSamples=0;
    
    for(int sym_cnt=0; sym_cnt<nextSymbols.length(); sym_cnt++)
        totalCPSamples=totalCPSamples+getCPLength(nfftSize,sym_cnt,ssbConfig.getSSBscs());
    
    nextTiming=(totalCPSamples+(nextSymbols.length()*nfftSize))*Tsa;
    
    if (isSSB0) nextTiming=ssbConfig.getSSBPeriodicity()*5*1e-3-nextTiming;
    
    return (nextTiming);
}

void BWSimSystem_S::initCoreModules(int nuCaID){
  
  #ifdef USING_MPI
  if(currentTaskid==0) {
    #endif
    cout<<"\n\n#################################################"<<endl;
    cout<<"[both:]Initializing CoreModules....."<<endl;
    cout<<"#################################################\n\n"<<endl;
    #ifdef USING_MPI
  }
  #endif
  
  bool fillPRS;
  int PRSperiodicity, PRSoffset;
  
  int nuID=getNUID(nuCaID);
  
  //From the config Files 
  
  
 Array <ivec> CellConfig_temp(mySysInfo.nuIDs.length());
  Array <ivec> SFI_temp;
  if(mySysInfo.ulDlMode==_ULDL_MODE_TDD_)
    parse(mySysInfo.myConfigFiles.nuConfigFileName,"CellConfig",CellConfig_temp);
  parse(mySysInfo.myConfigFiles.nuConfigFileName,"SFI",SFI_temp);
  
  //   cout<<"mySysInfo.myConfigFiles.nuIDs : "<<mySysInfo.myConfigFiles.nuIDs<<endl;
  
  int configNUIndx=find(mySysInfo.myConfigFiles.nuIDs,nuID);
  //   cout<<"configNUIndx:"<<configNUIndx<<endl;
  AWGNTablesConvCoder dlAWGNTables;
  dlAWGNTables.init_AWGNTables(true, mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).transportBlockMCSMappingFileName,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).cqiToMaxMCSRateFileName);
  dlLteTables.load(true,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).turboInterleaverTableFileName,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).transportBlockMCSMappingFileName,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).layerMappingTBSTranslationFileName, mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).cqiToMaxMCSRateFileName,dlAWGNTables);
  dlNRTables.load(true,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).turboInterleaverTableFileName,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).transportBlockMCSMappingFileName,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).layerMappingTBSTranslationFileName, mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).cqiToMaxMCSRateFileName,dlAWGNTables);
  
  AWGNTablesConvCoder ulAWGNTables;
  ulAWGNTables.init_AWGNTables(false, mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).transportBlockMCSMappingFileName, mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).cqiToMaxMCSRateFileName);
  ulLteTables.load(false,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).turboInterleaverTableFileName,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).transportBlockMCSMappingFileName,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).layerMappingTBSTranslationFileName, mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).cqiToMaxMCSRateFileName,ulAWGNTables);
  ulNRTables.load(false,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).turboInterleaverTableFileName,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).transportBlockMCSMappingFileName,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).layerMappingTBSTranslationFileName, mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).cqiToMaxMCSRateFileName,ulAWGNTables);
  
  bool enableVarPowInRB;
  parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,"enableVarPowInRB",enableVarPowInRB);
  if(enableVarPowInRB==true) //! DL power-allocation 
  {
    dlLteTables.dlVarPowMask.isInitialized=true; 
    dlNRTables.dlVarPowMask.isInitialized=true; 
    if(mySysInfo.isLocalised==false)
    {cout<<"Error... DL Power Allocation is defined only for Localized Type"<<endl; abort();}
  }
  
  TransmissionConfig_S defaultDlTxConfig,defaultUlTxConfig;
  ReceptionConfig_S defaultDlRxConfig,defaultUlRxConfig;
  double dlRSBoostInDB, ulRSBoostInDB;
  
  bool isOuterLoopLinkAdaptationEnabledForDL=false,isOuterLoopLinkAdaptationEnabledForUL=false;
  vec TargetBLERForDL,TargetBLERForUL;
  vec rateThresholdForDL,rateThresholdForUL;
  int windowSizeForDL=2,windowSizeForUL=2;
  int SRSbandwidthinRBs=-1;
  int SRScomb=-1;
  int SRSsoundingbandwidthinRBs=-1, SRSsoundingbandwidthFirstRB=-1, SRSCount=-1;
  int Pb;//! DL power-allocation 
  
  
  if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
  {
    defaultDlTxConfig=parseTxConfig(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName, mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).L2SupportFileName,true,mySysInfo.isSSBlockEnabled);
    defaultDlRxConfig=parseRxConfig(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,true);
    
    
    parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,"rsBoostInDB",dlRSBoostInDB);
    
    parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,"fillPRS",fillPRS);
    parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,"PRSperiodicity",PRSperiodicity);
    parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,"PRSoffset",PRSoffset);
    
    //! DL power-allocation 
    parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,"Pb",Pb);
    
    if(parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,"DL_enableOuterLoopLinkAdaptationAtScheduler",isOuterLoopLinkAdaptationEnabledForDL))
    {
      if(isOuterLoopLinkAdaptationEnabledForDL)
      {
        if(currentTaskid==0)
          cout<<"[detl:]Outer-Loop Link Adaptation is enabled. \n"<<endl;
        
        if(!parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,"DL_ollaRateThresholds",rateThresholdForDL))
        {
          cout<<"[detl:]Cannot parse DL_ollaRateThresholds from myDLTxRxConfig.txt. "<<endl; abort();
        }
        
        if(!parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,"DL_targetBLER",TargetBLERForDL))
        {
          cout<<"[detl:]Cannot parse DL_targetBLER from myDLTxRxConfig.txt. "<<endl; abort();
        }
        
        if(TargetBLERForDL.length()!=(rateThresholdForDL.length()+1))
        {
          cout<<"[detl:]Length mismatch between DL_ollaRateThresholds and DL_targetBLER "<<endl; abort();
        }
        
        if(!parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,"DL_windowSize",windowSizeForDL))
        {
          cout<<"[detl:]Warning. Cannot parse DL_windowSize from myDLTxRxConfig.txt. Default windowSize is 2"<<endl;
        }
        if(mySysInfo.linkSimulationInfo.isEnabled)
        {
          cout<<"[detl:]Warning... Outer-Loop Link Adaptation is enabled in Link Level Simulation.. \n"<<endl;
          //sleep(10);
        }
      }
      else
        if(currentTaskid==0)
          cout<<"[detl:]Outer-Loop Link Adaptation at scheduler is disabled. \n"<<endl;
    }
    else
    {
      cout<<"[detl:]Warning. Cannot parse DL_enableOuterLoopLinkAdaptationAtScheduler from myDLTxRxConfig.txt. Default is disabled"<<endl;
      isOuterLoopLinkAdaptationEnabledForDL=false;
    }
    
  }
  if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
  {
    defaultUlTxConfig=parseTxConfig(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName ,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).L2SupportFileName,false);
    defaultUlRxConfig=parseRxConfig(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName,false);
    parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName,"rsBoostInDB",ulRSBoostInDB);
    
    
    
    if(parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName,"UL_enableOuterLoopLinkAdaptationAtScheduler",isOuterLoopLinkAdaptationEnabledForUL))
    {
      if(isOuterLoopLinkAdaptationEnabledForUL)
      {
        if(currentTaskid==0)
          cout<<"[detl:]Outer-Loop Link Adaptation is enabled. \n"<<endl;
        if(!parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName,"UL_ollaRateThresholds",rateThresholdForUL))
        {
          cout<<"[detl:]Cannot parse UL_ollaRateThresholds from myULTxRxConfig.txt. "<<endl; abort();
        }
        
        if(!parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName,"UL_targetBLER",TargetBLERForUL))
        {
          cout<<"[detl:]Cannot parse UL_targetBLER from myULTxRxConfig.txt. "<<endl; abort();
        }
        
        if(TargetBLERForUL.length()!=(rateThresholdForUL.length()+1))
        {
          cout<<"[detl:]Length mismatch between UL_ollaRateThresholds and UL_targetBLER "<<endl; abort();
        }
        if(!parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName,"UL_windowSize",windowSizeForUL))
        {
          cout<<"[detl:]Warning. Cannot parse UL_windowSize from myULTxRxConfig.txt. Default windowSize is 2"<<endl;
        }
        if(mySysInfo.linkSimulationInfo.isEnabled)
        {
          cout<<"[detl:]Warning... Outer-Loop Link Adaptation is enabled in Link Level Simulation.. \n"<<endl;
          //sleep(10);
        }
      }
      else
        if(currentTaskid==0)
          cout<<"[detl:]Outer-Loop Link Adaptation at scheduler is disabled. \n"<<endl;
    }
    else
    {
      isOuterLoopLinkAdaptationEnabledForUL=false;
    }
    
  }
  
  
  PortToAntennaMapper_S portToAntennaMapper;
  portToAntennaMapper.init(mySysInfo.myConfigFiles.portToPhysicalAntennaMapperFileName);
  
  ivec idCells=findUniqueNumbers(mySysInfo.serverNodeIDCells);
  ivec crsInterlaces=findUniqueNumbers(mod(idCells,6));
  ivec portReferenceNumber;
  ivec CSIRSconfig=zeros_i(idCells.length());///SK: CSIRSconfig hardcoded to 0 in all cells.
  Array<FrameStructure> dlFrameStructurePerIDCell(0);
  FrameStructure ulFrameStructure;
  
  if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
  {
    ivec allPorts=getIntegers(0,22);
    ivec portsToRemove="4 6"; //Port4 structure undefined in downlink
    allPorts=remove(allPorts,portsToRemove);
    ivec prsPorts="61 62";
    if(fillPRS) 
    {
      append(allPorts,prsPorts);
    }
    
    ivec crsPorts=getIntegers(0,3);
    ivec dmrsPorts=getIntegers(5,14);dmrsPorts.del(1);
    ivec csirsPorts=getIntegers(15,22);
    
    // CSIRSconfiguaration are given different port names with first two digit will deside actual CSI-RS 
    // port number and next two digits as config numbers
    // Note: TDD configurations are not hadled (config from 21 to 32 ):-> Need to modify later 
    
    for (int i = 0; i<csirsPorts.length();i++)
    {
      if(csirsPorts(i) >=15 && csirsPorts(i)<= 16)
      {
        append(allPorts,getIntegers(csirsPorts(i)*100,csirsPorts(i)*100+20-1));
      }
      else if(csirsPorts(i) >=17 && csirsPorts(i)<= 18)
      {
        append(allPorts,getIntegers(csirsPorts(i)*100,csirsPorts(i)*100+10-1));
      }
      else if(csirsPorts(i) >=19 && csirsPorts(i)<= 22)
      {
        append(allPorts,getIntegers(csirsPorts(i)*100,csirsPorts(i)*100+5-1));
      }
      else
      {
        //do nothing 
      }
    }
    ivec NRcsirsPorts;//=getIntegers(3000,3032);
    //append(NRcsirsPorts,3000);
    //append(NRcsirsPorts,3100);
    //ivec NRbitmap = "0 1 0 0 0 0 0 0 0 0 0 0";
    //     for (int i = 0; i<NRcsirsPorts.length();i++)
    //     {
    //       if(NRcsirsPorts(i) == 3000) /*&& bitmap.length()== 12)*/
    //       {
    // 	append(allPorts,getIntegers(NRcsirsPorts(i)*100,NRcsirsPorts(i)*100+12-1));
    //       }
    //       if(NRcsirsPorts(i) == 3100) /*&& bitmap.length()== 12)*/
    //       {
    // 	append(allPorts,getIntegers(NRcsirsPorts(i)*100,NRcsirsPorts(i)*100+4-1));
    //       }
    //       else
    //       {
    // 	//do nothing 
    //       }
    //     }
    //      ivec srvrNodeDeviceIndx=find(nodeDeviceIDs,associationInfo.serverNodes);
    //   for(int srvr_cnt=0;srvr_cnt<associationInfo.serverNodes.length();srvr_cnt++)
    //   {
    //     int nuIndx=myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).getNUIndx(nuID); // Deciding supported NUs of serviceNode based only on associated serverNode !!  -Dhiv
    //     if(nuIndx!=-1)
    //     {
    //      myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuIndx).
    //     }
    //   }
    
    
    FrameStructure commonDlFrameStructure;
    
    if(defaultDlTxConfig.cchInfo.enableSSB)
      loadSSBPatterns(commonDlFrameStructure,mySysInfo.myConfigFiles.ssbPatternPath);
    
    loadAllResourceBlockStructures(commonDlFrameStructure,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).rbPatternPathForDownlink,0,dmrsPorts,1);
    
    ivec NRdmrsPorts=getIntegers(1000,1007);
    append(NRdmrsPorts,getIntegers(1100,1111));
    loadAllResourceBlockStructures(commonDlFrameStructure,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).NRrbPatternPath,0,NRdmrsPorts,1);
    ivec NRZPcsiports=getIntegers(3500,3501);
    loadAllResourceBlockStructures(commonDlFrameStructure,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).NRrbPatternPath,0,NRZPcsiports,1);
    ivec NRptrsPorts=getIntegers(1200,1201);
    loadAllResourceBlockStructures(commonDlFrameStructure,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).NRrbPatternPath,0,NRptrsPorts,1);
    loadNRCSIRSPatterns(commonDlFrameStructure,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).NRrbPatternPath);
    
    Array<FrameStructure> dlFrameStructurePerInterlace(crsInterlaces.length());
    for(int cnt=0;cnt<crsInterlaces.length();cnt++)
    {
      dlFrameStructurePerInterlace(cnt)=commonDlFrameStructure;
      // Loading RB strctures with CRS interlace
      loadAllResourceBlockStructures(dlFrameStructurePerInterlace(cnt),mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).rbPatternPathForDownlink,crsInterlaces(cnt),crsPorts,1);
      //       if(fillPRS)
      //loadAllResourceBlockStructures(dlFrameStructurePerInterlace(cnt),mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).rbPatternPathForDownlink,crsInterlaces(cnt),prsPorts,1);
      //if(defaultDlTxConfig.cchInfo.enablePBCH)
      //loadPBCHPatterns(dlFrameStructurePerInterlace(cnt),mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).cchPatternPath,crsInterlaces(cnt));
      if(defaultDlTxConfig.cchInfo.pdcchCount>0)
        loadAllCCHPatterns(dlFrameStructurePerInterlace(cnt),mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).cchPatternPath,crsInterlaces(cnt));
    }
    
    dlFrameStructurePerIDCell.set_length(idCells.length());
    for(int cnt=0;cnt<idCells.length();cnt++)
    {
      dlFrameStructurePerIDCell(cnt)=dlFrameStructurePerInterlace(find(crsInterlaces,idCells(cnt)%6));
      loadAllResourceBlockStructures(dlFrameStructurePerIDCell(cnt),mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).rbPatternPathForDownlink,idCells(cnt) ,csirsPorts,1);
      //loadAllResourceBlockStructures(dlFrameStructurePerIDCell(cnt),mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).NRrbPatternPath,idCells(cnt) ,NRcsirsPorts,1);
      
    }
  }
  if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
  {
    
    loadAllResourceBlockStructures(ulFrameStructure,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).rbPatternPathForUplink,0 ,getIntegers(0,3),0);
    
    ivec NRdmrsPorts=getIntegers(1000,1007);
    append(NRdmrsPorts,getIntegers(1100,1111));
    append(NRdmrsPorts,-2);
    append(NRdmrsPorts,-3);
    loadAllResourceBlockStructures(ulFrameStructure,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).NRrbPatternPath,0 ,NRdmrsPorts,0); //! Port numbers hardcoded !!! - Dhiv
    loadNRSRSPatterns(ulFrameStructure,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).NRrbPatternPath); 
    ivec NRptrsPorts=getIntegers(1200,1201);
    loadAllResourceBlockStructures(ulFrameStructure,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).NRrbPatternPath,0 ,NRptrsPorts,0);
    
   
       
    //     loadAllResourceBlockStructures(ulFrameStructure,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).NRrbPatternPath,0 ,NRdmrsPorts,0); 
    loadShortPUCCHpattern(ulFrameStructure,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).cchPatternPath);
    
    #ifdef USING_SRS    
    if(mySysInfo.isSRSEnabled)
    {
      string srsFile=configFileNames.NRrbPatternPath+"srs.txt";
      ulFrameStructure.loadResourceUnit("srs",srsFile);
    }
    #endif
    
  }
  
  bool enableCQI0=false;
  if(!parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,"DL_enableCQI0Support",enableCQI0))
  {
    if(currentTaskid==0)
      cout<<"[detl:]CQI0 Support disabled by default"<<endl;
  }
  int maximumActiveUEs=10;
  if(!parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).L2SupportFileName,"maximumActiveUEs",maximumActiveUEs))
  {
    if(currentTaskid==0)
      cout<<"[detl:]maximumActiveUEs set to 10 by default.. "<<endl;
  }
  
  //------------------------------------------------------------
  ivec serviceNodeIndices;
  
  ivec srvrNodeDeviceIndx=find(nodeDeviceIDs,associationInfo.serverNodes);
  
  
  // Parsing codebook parameterts
  CodeBookConfig_S dlCodeBookConfig = parseDlCodeBookConfig(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,mySysInfo.myConfigFiles.AASConfigFileName,_TRANSMISSION_MODE_NR_,mySysInfo.myConfigFiles.AASConfigFileName,mySys.channelCloud.nodesInfo.getBeamParameters(0).nTotalBeams);
  sL2SimConfig l2SimConfig(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).L2SupportFileName,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).TCPSupportFilesName,associationInfo.serviceNodes,associationInfo.serverNodes.length(),ceil_i(mySysInfo.simulationTimeInSec/mySysInfo.nuConfig(configNUIndx).slotDuraton),mySysInfo.nuConfig(configNUIndx).slotDuraton);
        
  for(int srvr_cnt=0;srvr_cnt<associationInfo.serverNodes.length();srvr_cnt++)
  {
    int nuCaIndx=myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).getNUIndx(nuCaID); // Deciding supported NUs of serviceNode based only on associated serverNode !!  -Dhiv
    int nuIndx=find(mySysInfo.nuIDs,nuID); // Deciding supported NUs of serviceNode based only on associated serverNode !!  -Dhiv
    
    if(nuCaIndx!=-1)
    {
      myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).cellID=mySysInfo.serverNodeIDCells(srvr_cnt);
      myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).CellConfig=CellConfig_temp(nuIndx);
      myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).SFI=SFI_temp(nuIndx);
      myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).CellConfigPeriodicityInSlots=mySysInfo.nuConfig(nuIndx).CellConfigPeriodicityInSlots;
      
      //! DL power-allocation 
      myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).Pb = Pb;
      
      myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).NI=0;
      
      myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).tddConfig=TDDConfiguration_E(mySysInfo.serverNodeTDDConf(srvr_cnt));
      getSpecialSubFrameConfig(&myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ssfConfig,mySysInfo.myConfigFiles.simulationConfigFileName);
      myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).frameConfig=getFrameConfigurationInfo(mySysInfo.ulDlMode,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).tddConfig,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).CellConfig,mySysInfo.nuConfig(nuIndx).CellConfigPeriodicityInSlots);
      double carrierFrequencyinMHz=(double)getCarrierFreqInMHz(nuCaID);
      double BWPerCarrierInHz=mySysInfo.carrierInfo.carrierBandwidthInMHz*1e6;
      double samplingFrequency=mySysInfo.carrierInfo.samplingFrequencyInMHz*1e6;
      int rbSubcarrierCount;
      if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
        rbSubcarrierCount=dlFrameStructurePerIDCell(find(idCells,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).cellID)).getResourceUnitSize("nullRB").numberOfSubcarriers;
      if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
        rbSubcarrierCount=ulFrameStructure.getResourceUnitSize("nullRB").numberOfSubcarriers;
      
      myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).nuParameters.init(nuCaID,mySysInfo.nuConfig(find(mySysInfo.nuIDs,nuID)), rbSubcarrierCount, carrierFrequencyinMHz, BWPerCarrierInHz, samplingFrequency,srvr_cnt==0);
      
      ivec srvcNodeDeviceIndx=find(nodeDeviceIDs,associationInfo.associatedServiceNodesPerServerNode(srvr_cnt));
      ivec srvcNodeIndices=find(associationInfo.serviceNodes,associationInfo.associatedServiceNodesPerServerNode(srvr_cnt));
      for(int srvc_cnt=0;srvc_cnt<associationInfo.associatedServiceNodesPerServerNode(srvr_cnt).length();srvc_cnt++)
      {
        int nuIndxInSrvc=myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).getNUIndx(nuCaID);
        if(nuIndxInSrvc!=-1)
        {
          myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuIndxInSrvc).CellConfig=CellConfig_temp(nuIndx);
          myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuIndxInSrvc).SFI=SFI_temp(nuIndx);
          myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuIndxInSrvc).CellConfigPeriodicityInSlots=mySysInfo.nuConfig(nuIndx).CellConfigPeriodicityInSlots;
        } 	
      }
      if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
      {
        string frameStructureFileName="dlFrame.txt";
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlFrameStructure=dlFrameStructurePerIDCell(find(idCells,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).cellID));
        string frameName="dlFrame";
        createFrameStructure(frameStructureFileName,frameName,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).nuParameters.dlNRBs,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).nuParameters.slotsPerTTI);
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlFrameStructure.loadFrameStructure(frameName,frameStructureFileName,true);
        
        /// For Four ports
        bool flag = false;
        if(parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,"isAltCodeBookEnabledFor4TXEnabled",flag)==false)
        {
          cout<<"[detl:]Warning...  unable to parse isAltCodeBookEnabledFor4TXEnabled.. Assumed alternate code book flag is disable ... "<<endl;
        }	
        
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlFrameStructure.initFrameStructure(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).cellID);
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlFrameStructure.setCurrentFrameType("dlFrame");// assuming zeroth carrier is primary
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlPortInfo.load(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName);
        
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlRxConfig=defaultDlRxConfig;
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).resourceMapper.dlResourceMapper.init(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).nuParameters.fftSize,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).nuParameters.dlNRBs,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlFrameStructure.getResourceUnitSize("nullRB"),mySysInfo.isLocalised);	
        //FIXME nPorts and N1N2P are assumed same for all beams
        int nBeams= mySys.channelCloud.nodesInfo.getNumberOfBeams(associationInfo.serverNodes(srvr_cnt));
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).codebookPerBeam.set_length(nBeams);
        for(int beam_cnt=0;beam_cnt<nBeams;beam_cnt++)
        {
            if(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlPortInfo.nPorts != dlCodeBookConfig.P*dlCodeBookConfig.N1*dlCodeBookConfig.N2*dlCodeBookConfig.Ng)
          {
            int NP = myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlPortInfo.nPorts;
            cout<<"DL_nPorts = should be equal  to P*N1*N2*Ng "<<"\nDL_nPorts = "<<NP<<"\t P*N1*N2*Ng ="<<dlCodeBookConfig.P*dlCodeBookConfig.N1*dlCodeBookConfig.N2*dlCodeBookConfig.Ng<<" Check in myDLTxRxConfig.txt.. aborting..."<<endl;
            abort();
          }
          
          codebookCollection.addCodebook(dlCodeBookConfig,flag);
          myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).codebookPerBeam(beam_cnt)=codebookCollection.getCodebookPointer(dlCodeBookConfig.N1,dlCodeBookConfig.N2,dlCodeBookConfig.Ng);
        }
      }
      
      if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
      {
        string frameStructureFileName="ulFrame.txt";
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ulFrameStructure=ulFrameStructure;
        
        string frameName="ulFrame";
        createFrameStructure(frameStructureFileName,frameName,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).nuParameters.ulNRBs,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).nuParameters.slotsPerTTI);
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ulFrameStructure.loadFrameStructure(frameName,frameStructureFileName,false);
        
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ulFrameStructure.initFrameStructure(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).cellID);
        
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ulFrameStructure.setCurrentFrameType("ulFrame");// assuming zeroth carrier is primary
        
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ulPortInfo.load(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName);
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ulRxConfig=defaultUlRxConfig;
        
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).resourceMapper.ulResourceMapper.init(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).nuParameters.fftSize,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).nuParameters.ulNRBs,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ulFrameStructure.getResourceUnitSize("nullRB"));
        
        if(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).codebookPerBeam.length()==0)
        {
          int nBeams = 1;
          myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).codebookPerBeam.set_length(nBeams); // No beam based codebook in uplink
          for(int beam_cnt=0;beam_cnt<nBeams;beam_cnt++)
          {
            codebookCollection.addCodebook(dlCodeBookConfig);
            myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).codebookPerBeam(beam_cnt)=codebookCollection.getCodebookPointer(dlCodeBookConfig.N1,dlCodeBookConfig.N2,dlCodeBookConfig.Ng);
          }
        }
      }
      
      // Initialize PortToAntennaMapper
      myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).portToAntennaMapper=portToAntennaMapper;
      
      //Create service nodes info
      serviceNodeIndices=find(associationInfo.serviceNodes,associationInfo.associatedServiceNodesPerServerNode(srvr_cnt));
      ivec serverNodesPerServiceNode=associationInfo.associatedServerNodesForEachServiceNode(serviceNodeIndices);
      
      bool isTurboCoderEnabled=true;
      bool isSubblockInterleaverEnabled=true;
      bool isScramblerEnabled=true;
      if(mySysInfo.linkSimulationInfo.isEnabled) {
        isTurboCoderEnabled=mySysInfo.linkSimulationInfo.enableFEC;
        isSubblockInterleaverEnabled=mySysInfo.linkSimulationInfo.enableFEC;
        isScramblerEnabled=mySysInfo.linkSimulationInfo.enableFEC;
      }
      
      bool  isDlMode;
      ivec srvrAntennaCount=mySys.channelCloud.nodesInfo.getTXRUCountForAllBeams(associationInfo.serverNodes(srvr_cnt));
      ivec srvcNodeAntenna(associationInfo.associatedServiceNodesPerServerNode(srvr_cnt).length());
      for(int srvc_cnt=0;srvc_cnt<associationInfo.associatedServiceNodesPerServerNode(srvr_cnt).length();srvc_cnt++)
      {
          int srvcNodeID = associationInfo.associatedServiceNodesPerServerNode(srvr_cnt)(srvc_cnt);
          sBeamPairLink beamPair = associationInfo.associatedBeamPairLinkforEachServiceNode(find(mySysInfo.serviceNodes,srvcNodeID));
          srvcNodeAntenna(srvc_cnt) = mySys.channelCloud.nodesInfo.getTXRUCount(srvcNodeID,beamPair.srvcBeamID);
      }
      
      bool initDecoder=true;
      #ifdef USING_MPI
      if(nTasks>1 && find(mySysInfo.mpiInfo.srvrIndxListPerTaskid(currentTaskid),(srvr_cnt))==-1)
        initDecoder=false;
      #endif
      if(initDecoder && mySysInfo.receptionModel.enableRestrictedRecepition && find(mySysInfo.receptionModel.serverNodesWithActualReception,associationInfo.serverNodes(srvr_cnt))==-1)
        initDecoder=false;
      
      
      //L2Core Initialize
      myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs=associationInfo.associatedServiceNodesPerServerNode(srvr_cnt);
      for(int srvc_cnt=myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs.length()-1;srvc_cnt>=0;srvc_cnt--)
      {
        int nuIndxInSrvc=myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).getNUIndx(nuCaID);
        if(nuIndxInSrvc==-1)
          myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs.del(srvc_cnt);
      }
      
      myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).activeUEs=myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs;
      if(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs.length()>maximumActiveUEs)
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).activeUEs=randPick(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs,maximumActiveUEs);
      
      
      if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
      {
        isDlMode = true;
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig=defaultDlTxConfig;
        int nLayers = myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.nLayers;
        
        string dmrstype,configFileName = mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName;
        int dmrsType,dmrsLength;
        parse(configFileName,"DL_DMRSType",dmrstype);
        parse(configFileName,"DL_DMRSMaxLength",dmrsLength);
        dmrsType = (dmrstype=="_DMRSTYPE_1_") ? 1 : 2;
        
        LayerInfo_S layerInfo(nLayers,dmrsType,dmrsLength);
        
        setRsInfo(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.rsInfo,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlPortInfo,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlFrameStructure,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).cellID,getTransmissionScheme(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.transmissionMode),layerInfo,mySysInfo.serverNodeAntenna(srvr_cnt),dlRSBoostInDB,1,1,0,1,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.mTRSConf);//dataStartSymbol hardcoded as zero - Dhiv
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.rsInfo.portReferenceNumbers=getPortReferenceNumbers(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlFrameStructure,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.rsInfo.rsPorts);
        // 	checkConfigFiles(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig, mySysInfo.serverNodeAntenna(srvr_cnt),isDlMode);
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlRSInfoForUnScheduled=getRSInfoForUnScheduled(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.rsInfo,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlFrameStructure);
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlRSInfoForUnScheduled.portReferenceNumbers=getPortReferenceNumbers(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlFrameStructure,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlRSInfoForUnScheduled.rsPorts);
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlRSInfoForUnScheduled.fillPRS=fillPRS;
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlRSInfoForUnScheduled.PRSperiodicity=PRSperiodicity;
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlRSInfoForUnScheduled.PRSoffset=PRSoffset;
        double txSubcarrierPowerInDBm=mySysInfo.serverNodeTxPowersInDBm(srvr_cnt)-10*log10(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).resourceMapper.dlResourceMapper.getUsedSubcarriers()); // Need to add RS boosting here - Dhiv
        
        if(mySysInfo.linkSimulationInfo.isEnabled)
          txSubcarrierPowerInDBm=30;
        
    myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlCQIComputationConfig.init(nuCaID,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.nCodeWords,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.nLayers,txSubcarrierPowerInDBm,defaultDlRxConfig.enableRankAdaptation,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlRxConfig.enableCodebookRestriction,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlRxConfig.trainingDurationInSlots,getTransmissionScheme(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.transmissionMode),_Waveform_OFDMA_,associationInfo.serverNodes(srvr_cnt),myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.rsInfo,enableCQI0,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.dlCQIModeInfo);	

        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlBWPartInfo=initBWPartInfo(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).nuParameters);
        
        CoderType_E coderType;
        if(defaultDlTxConfig.transmissionMode==_TRANSMISSION_MODE_NR_)
        {
          coderType=_CODERTYPE_LDPC_;
          
          if(coderType==_CODERTYPE_TURBO_)
            cout<<"Warning.. Using Turbo for NR.."<<endl;  
        }
        else
          coderType=_CODERTYPE_TURBO_;
        
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).downlinkSourceAndSink.init(associationInfo.associatedServiceNodesPerServerNode(srvr_cnt),&dlLteTables,&dlNRTables,associationInfo.associatedServiceNodesPerServerNode(srvr_cnt),initDecoder,defaultDlTxConfig.harqType,defaultDlTxConfig.maximumHarqProcesses,defaultDlTxConfig.minDelayBetweenSuccessiveTransmissions,defaultDlTxConfig.maximumReTransmissions,isTurboCoderEnabled,isSubblockInterleaverEnabled,isScramblerEnabled,defaultDlRxConfig.UEcategory,coderType);
        
        initCSIProcesses( myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlCQIComputationConfig,nuCaID,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlPortInfo.NZPCSIConfigurations);
        
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).outerLoopLinkAdaptationInfoForDL.isEnabled=isOuterLoopLinkAdaptationEnabledForDL;
        
        if(isOuterLoopLinkAdaptationEnabledForDL)
          myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).outerLoopLinkAdaptationInfoForDL.AddTo(associationInfo.associatedServiceNodesPerServerNode(srvr_cnt),TargetBLERForDL,rateThresholdForDL,windowSizeForDL);
        
        ivec mySrvcIndx=find(associationInfo.serviceNodes,associationInfo.associatedServiceNodesPerServerNode(srvr_cnt));
        
        double srvrTxSubcarrierPowerInDBm=mySysInfo.serverNodeTxPowersInDBm(srvr_cnt)-10*log10(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).resourceMapper.dlResourceMapper.getUsedSubcarriers());
        vec srvcTxSubcarrierPowerInDBm=zeros(mySrvcIndx.length()); // uplink power not initialized for downlink scheduler
        
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).dlScheduler.initScheduler(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).dlTxRxConfigFileName,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).L2SupportFileName,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).cellID,associationInfo.associatedServiceNodesPerServerNode(srvr_cnt),getFromArray(associationInfo.associatedBeamPairLinkforEachServiceNode,mySrvcIndx),srvrAntennaCount,srvcNodeAntenna,srvrTxSubcarrierPowerInDBm,srvcTxSubcarrierPowerInDBm,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).downlinkSourceAndSink.tbTables,&myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlFrameStructure,&myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlFrameReservationInfo,&myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlPortInfo,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).codebookPerBeam,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlBWPartInfo,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).resourceMapper.dlResourceMapper.getResourceBlockGroupingSize(),defaultDlTxConfig.prbBundlingSize,true);
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).dlHarqManager.set_length(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs.length());
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).dlRadioBearer.set_length(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs.length());
// 	myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).TPCcmdFieldPerUE.set_length(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs.length()); //TPC BY BS
// 	myNodeDevices(srvrNodeDeviceIndx(srvr_cnt))/*.L1CorePerNU(nuCaIndx).ulTxConfig.powerControlInfo*/.TPCcmdFieldPerUE.set_length(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs.length());
//	cout<<"TPCcmdFieldPerUE length \t"<<myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx)./*.ulTxConfig.powerControlInfo.*/TPCcmdFieldPerUE.length()<<endl;

        for(int ue_cnt=0;ue_cnt<myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs.length();ue_cnt++)
        {
          myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).dlHarqManager(ue_cnt).initializeHarqManager("IR",myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.maximumHarqProcesses,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.minDelayBetweenSuccessiveTransmissions,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).dlTxConfig.maximumReTransmissions+1);
          myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).dlRadioBearer(ue_cnt).init(associationInfo.serverNodes(srvr_cnt),myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs(ue_cnt),&l2SimConfig,1);
        }
    }
      if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
      {
        isDlMode = false;
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ulTxConfig=defaultUlTxConfig;
        
        CoderType_E coderType;
        if(defaultUlTxConfig.transmissionMode==_TRANSMISSION_MODE_NR_)
        {
          coderType=_CODERTYPE_LDPC_;
          
          if(coderType==_CODERTYPE_TURBO_)
            cout<<"Warning.. Using Turbo for NR.."<<endl;  
        }
        else
          coderType=_CODERTYPE_TURBO_;
        
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).uplinkSourceAndSink.init(associationInfo.associatedServiceNodesPerServerNode(srvr_cnt),&ulLteTables,&ulNRTables,associationInfo.associatedServiceNodesPerServerNode(srvr_cnt),initDecoder,defaultUlTxConfig.harqType,defaultUlTxConfig.maximumHarqProcesses,defaultUlTxConfig.minDelayBetweenSuccessiveTransmissions,defaultUlTxConfig.maximumReTransmissions,isTurboCoderEnabled,isSubblockInterleaverEnabled,isScramblerEnabled,defaultUlRxConfig.UEcategory,coderType);
    myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).sidelinkSourceAndSink.init(associationInfo.associatedServiceNodesPerServerNode(srvr_cnt),&ulLteTables,&ulNRTables,associationInfo.associatedServiceNodesPerServerNode(srvr_cnt),initDecoder,defaultUlTxConfig.harqType,defaultUlTxConfig.maximumHarqProcesses,defaultUlTxConfig.minDelayBetweenSuccessiveTransmissions,/*defaultUlTxConfig.maximumReTransmissions*/0,isTurboCoderEnabled,isSubblockInterleaverEnabled,isScramblerEnabled,defaultUlRxConfig.UEcategory,coderType);
        
        //L2Core Initialize
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuIndx).associatedUEs=associationInfo.associatedServiceNodesPerServerNode(srvr_cnt);
	ivec srvcNodeDeviceIndx=find(nodeDeviceIDs,associationInfo.associatedServiceNodesPerServerNode(srvr_cnt));
	for(int srvc_cnt=myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuIndx).associatedUEs.length()-1;srvc_cnt>=0;srvc_cnt--)
	{
	  int nuIndxInSrvc=myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).getNUIndx(nuCaID);
	  if(nuIndxInSrvc==-1)
	    myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuIndx).associatedUEs.del(srvc_cnt);
	}
      
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).outerLoopLinkAdaptationInfoForUL.isEnabled=isOuterLoopLinkAdaptationEnabledForUL;
        if(isOuterLoopLinkAdaptationEnabledForUL)
          myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).outerLoopLinkAdaptationInfoForUL.AddTo(associationInfo.associatedServiceNodesPerServerNode(srvr_cnt),TargetBLERForUL,rateThresholdForUL,windowSizeForUL);
        
        ivec mySrvcIndx=find(associationInfo.serviceNodes,associationInfo.associatedServiceNodesPerServerNode(srvr_cnt));
        double srvrTxSubcarrierPowerInDBm=0; // downlink power not initialized for uplink scheduler
        vec srvcTxSubcarrierPowerInDBm=mySysInfo.serviceNodeTxPowersInDBm(mySrvcIndx)-10*log10(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).resourceMapper.ulResourceMapper.getUsedSubcarriers());
        
	myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).TPCcmdFieldPerUE=ones_i(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs.length()); // TPC BY BS
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).ulScheduler.initScheduler(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName,mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).L2SupportFileName,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).cellID,associationInfo.associatedServiceNodesPerServerNode(srvr_cnt),getFromArray(associationInfo.associatedBeamPairLinkforEachServiceNode,mySrvcIndx),srvrAntennaCount,srvcNodeAntenna,srvrTxSubcarrierPowerInDBm,srvcTxSubcarrierPowerInDBm,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).uplinkSourceAndSink.tbTables,&myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ulFrameStructure,&myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ulFrameReservationInfo,&myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ulPortInfo,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).codebookPerBeam,sBWPartInfo(),myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).resourceMapper.ulResourceMapper.getResourceBlockGroupingSize(),defaultUlTxConfig.prbBundlingSize,false,defaultUlTxConfig.powerControlInfo);
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).ulHarqManager.set_length(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs.length());
        myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).ulRadioBearer.set_length(myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs.length());
    
	
	// myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).outerLoopLinkAdaptationInfoForSL.isEnabled=isOuterLoopLinkAdaptationEnabledForUL;
	// if(outerLoopLinkAdaptationInfoForSL)
	 // myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).outerLoopLinkAdaptationInfoForSL.AddTo(associationInfo.associatedServiceNodesPerServerNode(srvr_cnt),TargetBLERForUL,rateThresholdForUL,windowSizeForUL);
        
        for(int ue_cnt=0;ue_cnt<myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs.length();ue_cnt++)
        {
          myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).ulHarqManager(ue_cnt).initializeHarqManager("IR",myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ulTxConfig.maximumHarqProcesses,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ulTxConfig.minDelayBetweenSuccessiveTransmissions,myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx).ulTxConfig.maximumReTransmissions+1);
          myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).ulRadioBearer(ue_cnt).init(associationInfo.serverNodes(srvr_cnt),myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L2CorePerNU(nuCaIndx).associatedUEs(ue_cnt),&l2SimConfig,0);
        }
	  }
      
      parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName,"SRSbandwidthinRBs",SRSbandwidthinRBs);
      parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName,"SRScomb",SRScomb);
      parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName,"SRSsoundingbandwidthinRBs",SRSsoundingbandwidthinRBs);
      parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName,"SRSsoundingbandwidthFirstRB",SRSsoundingbandwidthFirstRB);
	parse(mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName,"SRSCount",SRSCount);   
      
      for(int srvc_cnt=0;srvc_cnt<associationInfo.associatedServiceNodesPerServerNode(srvr_cnt).length();srvc_cnt++)
      {
        int nuCaIndxInSrvc=myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).getNUIndx(nuCaID);
        if(nuCaIndxInSrvc!=-1)
        {
          myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc)=myNodeDevices(srvrNodeDeviceIndx(srvr_cnt)).L1CorePerNU(nuCaIndx);
          if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
          {
              string dmrstype,configFileName = mySysInfo.myConfigFiles.nuConfigFiles(configNUIndx).ulTxRxConfigFileName;
              int dmrsType,dmrsLength;
              parse(configFileName,"UL_DMRSType",dmrstype);
              parse(configFileName,"UL_DMRSMaxLength",dmrsLength);
              dmrsType = (dmrstype=="_DMRSTYPE_1_") ? 1 : 2;
            myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulTxConfig=defaultUlTxConfig;
            int nLayers = myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulTxConfig.nLayers;
            LayerInfo_S layerInfo(nLayers,dmrsType,dmrsLength);
            
            setRsInfo(myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulTxConfig.rsInfo,myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulPortInfo,myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulFrameStructure,myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).cellID,getTransmissionScheme(myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulTxConfig.transmissionMode),layerInfo, mySysInfo.serviceNodeAntenna(srvcNodeIndices(srvc_cnt)),ulRSBoostInDB,0,1,0);//dataStartSymbol hardcoded as zero - Dhiv
            myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulTxConfig.rsInfo.portReferenceNumbers=getPortReferenceNumbers(myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulFrameStructure,myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulTxConfig.rsInfo.rsPorts);
	  // SRSconfig_S srsConfig= getSRSBW(associationInfo,SRSbandwidthinRBs,SRScomb,SRSsoundingbandwidthinRBs,SRSsoundingbandwidthFirstRB,srvc_cnt,SRSCount);
	  // 	    checkConfigFiles(myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulTxConfig, mySysInfo.serviceNodeAntenna(srvcNodeIndices(srvc_cnt)),isDlMode);
            myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulRSInfoForUnScheduled=getRSInfoForUnScheduled(myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulTxConfig.rsInfo,myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulFrameStructure);
            myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulRSInfoForUnScheduled.portReferenceNumbers=getPortReferenceNumbers(myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulFrameStructure,myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulRSInfoForUnScheduled.rsPorts);
            //         
            double srsSubcarrierPowerInDBm=mySysInfo.serviceNodeTxPowersInDBm(srvc_cnt)-10*log10(300); // Hardcoding 300 for SRS subcarriers
	    myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulCQIComputationConfig.init(nuCaID,myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulTxConfig.nCodeWords,myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulTxConfig.nLayers,srsSubcarrierPowerInDBm,false,false,0,getTransmissionScheme(myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulTxConfig.transmissionMode),_Waveform_OFDMA_,associationInfo.serverNodes(srvr_cnt),myNodeDevices(srvcNodeDeviceIndx(srvc_cnt)).L1CorePerNU(nuCaIndxInSrvc).ulTxConfig.rsInfo);
          }
        }
      }
     }
  }
}

void BWSimSystem_S::initAllNodeDevice(){
  
  //Unconditionally initializing all Transmitters and Receivers
  
  string linkTableFileName=mySysInfo.resultFiles.linkTableFileName;
  nodeDeviceIDs=concat(mySysInfo.serverNodes,mySysInfo.serviceNodes);
  myNodeDevices.set_length(nodeDeviceIDs.length());
  ivec srvrIndices=find(nodeDeviceIDs,mySysInfo.serverNodes);
  ivec srvcIndices=find(nodeDeviceIDs,mySysInfo.serviceNodes);
  
   
  //reciprocity
  mySysInfo.weakIntr.set_length(nodeDeviceIDs.length());
  mySysInfo.intrCov.set_length(nodeDeviceIDs.length());
  mySysInfo.noiseVar.set_length(nodeDeviceIDs.length());
  
  Array< Array< Transmitter*> > txPointer;
  Array< Array< Receiver*> > rxPointer;
  bool initSrvrTx=false, initSrvrRx=false, initSrvcTx=false, initSrvcRx=false;
  
  if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
  { 
    initSrvrTx=true; 
    initSrvcRx=true;
  }
  if(mySysInfo.ulDlMode!=_ULDL_MODE_DOWNLINK_)
  { 
    initSrvrRx=true;
    initSrvcTx=true;
    if(mySysInfo.hasFeature("isPUCCHEnabled") || mySysInfo.enableSidelink)
      initSrvcRx=true;
  }
  
  Array<ivec> serverNodeNumerologies=mySysInfo.serverNodeNumerologies;
  Array<ivec> serviceNodeNumerologies=mySysInfo.serviceNodeNumerologies;

  if(mySysInfo.linkSimulationInfo.isEnabled)
  {
    serverNodeNumerologies.set_length(1);
    serviceNodeNumerologies.set_length(1);

    Array<ivec> nodeNumerologies;
    parse(mySysInfo.myConfigFiles.sysConfigFileName,"nodeNumerologies",nodeNumerologies);
    serverNodeNumerologies(0)=nodeNumerologies(0);
    serviceNodeNumerologies(0)=nodeNumerologies(1);
  }
  
  ivec srvrDeviceIndices=find(nodeDeviceIDs,mySysInfo.serverNodes);
  ivec srvcDeviceIndices=find(nodeDeviceIDs,mySysInfo.serviceNodes);
  
  ivec uniqueNumerologies(0);
  Array<ivec> nodesPerNu(0);
  ivec emptyVec(0);

  bool isSSBEnabled=false;
  if(!mySysInfo.linkSimulationInfo.isEnabled)
      parse(mySysInfo.myConfigFiles.ssbConfigFileName,"isSSBEnabled",isSSBEnabled);
  
  
  for(int srvr_cnt=0;srvr_cnt<mySysInfo.serverNodes.length();srvr_cnt++)
  {
  //Harsha :
    
     //Need to check below line - Dhiv

    ivec carrierFrequenciesinMHz=to_ivec(mySysInfo.carrierInfo.carrierFreqInMHzPerServerNodeType);
    ivec nuIDs=serverNodeNumerologies(srvr_cnt);
    
    ivec nuCaIDs=getNUCaIDs(nuIDs,carrierFrequenciesinMHz);
    
    myNodeDevices(srvrDeviceIndices(srvr_cnt)).nodeID=mySysInfo.serverNodes(srvr_cnt);
    myNodeDevices(srvrDeviceIndices(srvr_cnt)).nuIDs=nuCaIDs;
    myNodeDevices(srvrDeviceIndices(srvr_cnt)).ttiCountPerNU=zeros_i(nuCaIDs.length())-1;
    myNodeDevices(srvrDeviceIndices(srvr_cnt)).actualLoopCountPerNU=zeros_i(nuCaIDs.length())-1;
    myNodeDevices(srvrDeviceIndices(srvr_cnt)).L1CorePerNU.set_length(nuCaIDs.length());
    myNodeDevices(srvrDeviceIndices(srvr_cnt)).L2CorePerNU.set_length(nuCaIDs.length());
    
    for(int nu_cnt=0;nu_cnt<nuCaIDs.length();nu_cnt++)  
    {
      if(find(uniqueNumerologies,nuCaIDs(nu_cnt))==-1)
	append(uniqueNumerologies,nuCaIDs(nu_cnt));
    }
  
  if(isSSBEnabled) //SSB independent of Numerology and so out of L1Core - Dhiv
  {
      double carrierInGHz=carrierFrequenciesinMHz(0)/1e3; // Taking the first carrier assuming all carriers are in the same range
      double samplingFrequency=mySysInfo.carrierInfo.samplingFrequencyInMHz*1e6;
        myNodeDevices(srvrDeviceIndices(srvr_cnt)).SSBurstSetConfig.init(mySysInfo.myConfigFiles.ssbConfigFileName,mySysInfo.myConfigFiles.ssbPatternPath, samplingFrequency, carrierInGHz);
  }
  
  } 
  for(int srvc_cnt=0;srvc_cnt<mySysInfo.serviceNodes.length();srvc_cnt++)
  {
        //Need to check below line - Dhiv

      ivec carrierFrequenciesinMHz=to_ivec(mySysInfo.carrierInfo.carrierFreqInMHzPerServerNodeType);
    ivec nuIDs=serviceNodeNumerologies(srvc_cnt);
    
    ivec newNUIDs = getNUCaIDs(nuIDs,carrierFrequenciesinMHz);
        
    myNodeDevices(srvcDeviceIndices(srvc_cnt)).nodeID=mySysInfo.serviceNodes(srvc_cnt);
    myNodeDevices(srvcDeviceIndices(srvc_cnt)).nuIDs=newNUIDs;
    myNodeDevices(srvcDeviceIndices(srvc_cnt)).ttiCountPerNU=zeros_i(newNUIDs.length())-1; 
    myNodeDevices(srvcDeviceIndices(srvc_cnt)).actualLoopCountPerNU=zeros_i(newNUIDs.length())-1;
    myNodeDevices(srvcDeviceIndices(srvc_cnt)).L1CorePerNU.set_length(newNUIDs.length());
    myNodeDevices(srvcDeviceIndices(srvc_cnt)).L2CorePerNU.set_length(newNUIDs.length());
   
    for(int nu_cnt=0;nu_cnt<newNUIDs.length();nu_cnt++)  
    {
      if(find(uniqueNumerologies,newNUIDs(nu_cnt))==-1)
	append(uniqueNumerologies,newNUIDs(nu_cnt));
    }
    
    if(isSSBEnabled) //SSB independent of Numerology and so out of L1Core - Dhiv
    {
        double carrierInGHz=carrierFrequenciesinMHz(0)/1e3; // Taking the first carrier assuming all carriers are in the same range
        double samplingFrequency=mySysInfo.carrierInfo.samplingFrequencyInMHz*1e6;
        myNodeDevices(srvcDeviceIndices(srvc_cnt)).SSBurstSetConfig.init(mySysInfo.myConfigFiles.ssbConfigFileName,mySysInfo.myConfigFiles.ssbPatternPath,samplingFrequency,carrierInGHz);
    }
    
  }
  for(int nu_cnt=0;nu_cnt<uniqueNumerologies.length();nu_cnt++)  
    initCoreModules(uniqueNumerologies(nu_cnt));
  
  for(int srvr_cnt=0;srvr_cnt<mySysInfo.serverNodes.length();srvr_cnt++)
  {
    splitBWP(myNodeDevices(srvrDeviceIndices(srvr_cnt)),mySysInfo);
    for(int nu_cnt=0;nu_cnt<myNodeDevices(srvrDeviceIndices(srvr_cnt)).L1CorePerNU.length();nu_cnt++)
    {
      ivec srvcNodeDeviceIndices=find(nodeDeviceIDs,myNodeDevices(srvrDeviceIndices(srvr_cnt)).L2CorePerNU(nu_cnt).associatedUEs);
      int nuFactor=myNodeDevices(srvrDeviceIndices(srvr_cnt)).L1CorePerNU(nu_cnt).nuParameters.subcarrierSpacingInHz/15000;
      
      for(int srvc_cnt=0;srvc_cnt<myNodeDevices(srvrDeviceIndices(srvr_cnt)).L2CorePerNU(nu_cnt).associatedUEs.length();srvc_cnt++)
      {
	int srvcNuIndx=myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).getNUIndx(myNodeDevices(srvrDeviceIndices(srvr_cnt)).nuIDs(nu_cnt));
	if(srvcNuIndx!=-1)
	{
	  //Updating BSBWPLocations in srvcNode
	  myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(srvcNuIndx).nuParameters.BSBWPLocations=myNodeDevices(srvrDeviceIndices(srvr_cnt)).L1CorePerNU(nu_cnt).nuParameters.BSBWPLocations;
	  if(mySysInfo.ulDlMode!=_ULDL_MODE_UPLINK_)
	  {
	    // This allows only contiguous measurement of CQI. Need to change -  Dhiv
	    
	    myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(srvcNuIndx).dlCQIComputationConfig.cqiMeasurementConfig(0).startRB=myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(srvcNuIndx).nuParameters.BSBWPLocations(0)(0);
	    myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(srvcNuIndx).dlCQIComputationConfig.cqiMeasurementConfig(0).nRBs=(myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(srvcNuIndx).nuParameters.BSBWPLocations(0)(1)-myNodeDevices(srvcNodeDeviceIndices(srvc_cnt)).L1CorePerNU(srvcNuIndx).nuParameters.BSBWPLocations(0)(0))+1;
	  }
	  
	}
      }
    }
    
  }
  
  
  for(int srvr_cnt=0;srvr_cnt<mySysInfo.serverNodes.length();srvr_cnt++)
  {
    for(int nu_cnt=0;nu_cnt<myNodeDevices(srvrDeviceIndices(srvr_cnt)).L1CorePerNU.length();nu_cnt++)
    {
      for(int srvc_cnt=0;srvc_cnt<myNodeDevices(srvrDeviceIndices(srvr_cnt)).L2CorePerNU(nu_cnt).associatedUEs.length();srvc_cnt++)
      {
	int index=find(associationInfo.serviceNodes,myNodeDevices(srvrDeviceIndices(srvr_cnt)).L2CorePerNU(nu_cnt).associatedUEs(srvc_cnt));
	if(index != -1)
	{
	  append(myNodeDevices(srvrDeviceIndices(srvr_cnt)).L2CorePerNU(nu_cnt).beamPairLinkperUE,associationInfo.associatedBeamPairLinkforEachServiceNode(index));
	  
	  if(find(myNodeDevices(srvrDeviceIndices(srvr_cnt)).beamIDs,associationInfo.associatedBeamPairLinkforEachServiceNode(index).srvrBeamID) == -1)
	    {
	      append(myNodeDevices(srvrDeviceIndices(srvr_cnt)).beamIDs,associationInfo.associatedBeamPairLinkforEachServiceNode(index).srvrBeamID);
	    }
    
	}
      }
    }
    
  }
  
  
  
  
  if(initSrvrTx)
  {
    for(int srvr_cnt=0;srvr_cnt<mySysInfo.serverNodes.length();srvr_cnt++)
    {
      myNodeDevices(srvrDeviceIndices(srvr_cnt)).transmitterPerNU.set_length(myNodeDevices(srvrDeviceIndices(srvr_cnt)).nuIDs.length());
      for(int nu_cnt=0;nu_cnt<myNodeDevices(srvrDeviceIndices(srvr_cnt)).nuIDs.length();nu_cnt++)
      {
	myNodeDevices(srvrDeviceIndices(srvr_cnt)).transmitterPerNU(nu_cnt).myNodeDevice=&myNodeDevices(srvrDeviceIndices(srvr_cnt));
	myNodeDevices(srvrDeviceIndices(srvr_cnt)).transmitterPerNU(nu_cnt).initTransmitter(mySysInfo.serverNodes(srvr_cnt),mySysInfo.serverNodeAntenna(srvr_cnt),myNodeDevices(srvrDeviceIndices(srvr_cnt)).L1CorePerNU(nu_cnt).nuParameters);
      }
    }
  }
  
  if(initSrvcTx)
  {
    for(int srvc_cnt=0;srvc_cnt<mySysInfo.serviceNodes.length();srvc_cnt++)
    {
      myNodeDevices(srvcDeviceIndices(srvc_cnt)).transmitterPerNU.set_length(myNodeDevices(srvcDeviceIndices(srvc_cnt)).nuIDs.length());
      for(int nu_cnt=0;nu_cnt<myNodeDevices(srvcDeviceIndices(srvc_cnt)).nuIDs.length();nu_cnt++)
      {
	myNodeDevices(srvcDeviceIndices(srvc_cnt)).transmitterPerNU(nu_cnt).myNodeDevice=&myNodeDevices(srvcDeviceIndices(srvc_cnt));
	int srvrIndx=find(associationInfo.serverNodes,associationInfo.associatedServerNodesForEachServiceNode(srvc_cnt));
	if(find(serverNodeNumerologies(srvrIndx),serviceNodeNumerologies(srvc_cnt)(nu_cnt))!=-1) // initializing only the NUs available with associated serverNode
	  myNodeDevices(srvcDeviceIndices(srvc_cnt)).transmitterPerNU(nu_cnt).initTransmitter(mySysInfo.serviceNodes(srvc_cnt),mySysInfo.serviceNodeAntenna(srvc_cnt),myNodeDevices(srvcDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt).nuParameters);
	
      }
    }
  }

  if(initSrvrRx)
  {
    for(int srvr_cnt=0;srvr_cnt<mySysInfo.serverNodes.length();srvr_cnt++)
    {
      ivec associatedSrvcDeviceIndices=find(nodeDeviceIDs,associationInfo.associatedServiceNodesPerServerNode(srvr_cnt));
      myNodeDevices(srvrDeviceIndices(srvr_cnt)).receiverPerNU.set_length(myNodeDevices(srvrDeviceIndices(srvr_cnt)).nuIDs.length());
      for(int nu_cnt=0;nu_cnt<myNodeDevices(srvrDeviceIndices(srvr_cnt)).nuIDs.length();nu_cnt++)
      {
	myNodeDevices(srvrDeviceIndices(srvr_cnt)).receiverPerNU(nu_cnt).myNodeDevice=&myNodeDevices(srvrDeviceIndices(srvr_cnt));
    myNodeDevices(srvrDeviceIndices(srvr_cnt)).receiverPerNU(nu_cnt).initReceiver(mySysInfo.serverNodes(srvr_cnt),mySysInfo.serverNodeAntenna(srvr_cnt),myNodeDevices(srvrDeviceIndices(srvr_cnt)).L1CorePerNU(nu_cnt),true,mySysInfo.ulDlMode,mySysInfo.isWithAbstraction);

	for(int srvc_cnt=0;srvc_cnt<associatedSrvcDeviceIndices.length();srvc_cnt++)
	{
	  int srvcNuIndx=myNodeDevices(associatedSrvcDeviceIndices(srvc_cnt)).getNUIndx(myNodeDevices(srvrDeviceIndices(srvr_cnt)).nuIDs(nu_cnt));
	  if(srvcNuIndx!=-1)
	    myNodeDevices(srvrDeviceIndices(srvr_cnt)).receiverPerNU(nu_cnt).uplinkCQIInfo.addServiceNode(myNodeDevices(associatedSrvcDeviceIndices(srvc_cnt)).L1CorePerNU(srvcNuIndx).ulCQIComputationConfig,associationInfo.associatedServiceNodesPerServerNode(srvr_cnt)(srvc_cnt));
	}
	
      }
    }
  }

  
  if(initSrvcRx)
  {
    for(int srvc_cnt=0;srvc_cnt<srvcDeviceIndices.length();srvc_cnt++)
    {
      myNodeDevices(srvcDeviceIndices(srvc_cnt)).receiverPerNU.set_length(myNodeDevices(srvcDeviceIndices(srvc_cnt)).nuIDs.length());
      for(int nu_cnt=0;nu_cnt<myNodeDevices(srvcDeviceIndices(srvc_cnt)).nuIDs.length();nu_cnt++)
      {
	myNodeDevices(srvcDeviceIndices(srvc_cnt)).receiverPerNU(nu_cnt).myNodeDevice=&myNodeDevices(srvcDeviceIndices(srvc_cnt));
	int srvrNodeDeviceIndx=find(nodeDeviceIDs,associationInfo.associatedServerNodesForEachServiceNode(srvc_cnt));
	if(find(myNodeDevices(srvrNodeDeviceIndx).nuIDs,myNodeDevices(srvcDeviceIndices(srvc_cnt)).nuIDs(nu_cnt))!=-1) // initializing only the NUs available with associated serverNode
        myNodeDevices(srvcDeviceIndices(srvc_cnt)).receiverPerNU(nu_cnt).initReceiver(mySysInfo.serviceNodes(srvc_cnt),mySysInfo.serviceNodeAntenna(srvc_cnt),myNodeDevices(srvcDeviceIndices(srvc_cnt)).L1CorePerNU(nu_cnt),false,mySysInfo.ulDlMode,mySysInfo.isWithAbstraction);
      }
    }
  }
}


