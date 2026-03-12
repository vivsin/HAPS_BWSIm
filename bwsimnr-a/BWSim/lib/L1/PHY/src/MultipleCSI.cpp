#include "../include/MultipleCSI.h"

// This function sets the multiple CSI process per UE

void initCSIProcesses( CQIComputationConfig_S &cqiComputationConfig, int nuID, TransmissionConfig_S &txConfig, imat &NZPCSIConfigurations)
{
  
  if(txConfig.multipleCSIProcessFlag && (txConfig.transmissionMode == _TRANSMISSION_MODE_9_ || txConfig.transmissionMode == _TRANSMISSION_MODE_NR_))
  {  
    Array<cCSIProcessesInfo > csiprocessesPerUE;
    imat NZPCSIConfig;

    if(NZPCSIConfigurations.cols() == 1 && NZPCSIConfigurations.rows()==1)
    {
      cout<<"[Error:] single CSIRSProcess with one CSIRS configuration is not supported. \n\tTo run with one Non-zero power CSIRS configuration make 'multipleCSIProcessFlag = false' in myDLTxRXConfig and set 'CSIRSConfig' flag with desired CSI RS configuration.."<<endl;
      abort();
    }
    csiprocessesPerUE.set_length(txConfig.nCSIProcess);
    
    for(int cnt_csipro = 0; cnt_csipro<txConfig.nCSIProcess; cnt_csipro++)
    {
      ivec row_num = to_ivec(cnt_csipro);
      NZPCSIConfig = NZPCSIConfigurations.get_rows(row_num);
      
      ivec CSIConfig;
      CSIConfig.set_length(NZPCSIConfig.cols());
      
      for(int configCnt= 0;configCnt<NZPCSIConfig.cols();configCnt++)
	CSIConfig(configCnt) = NZPCSIConfig(0,configCnt);
      
      updateCSIProcessInfo(csiprocessesPerUE(cnt_csipro),txConfig.nPorts,cnt_csipro,CSIConfig);
    }
      
    addCSIProcessesPerUE(cqiComputationConfig,csiprocessesPerUE, nuID); 
  }
  
}
void addCSIProcessesPerUE(CQIComputationConfig_S &cqiComputationConfig, Array<cCSIProcessesInfo > &csiProcessInfo, int nuID){
  
  CQIComputationConfig_S tempcqiComputationConfig = cqiComputationConfig;
  int startIndx= 0 ;
  ivec processIDs;
  
  
  for(int processCnt=0;processCnt<csiProcessInfo.length();processCnt++)
  {
    
    int nPorts = csiProcessInfo(processCnt).getportCount();
    
    if(processCnt>0) // calculate startIndex except for 1st Iteration
      startIndx= cqiComputationConfig.cqiMeasurementConfig.length();
    
    Array<CQIMeasurementConfig_S> CSIProcessMeasurementConfigs(csiProcessInfo(processCnt).getCSIConfigCount());
    if(processCnt>0) 
      cqiComputationConfig.cqiMeasurementConfig.set_size(startIndx+csiProcessInfo(processCnt).getCSIConfigCount(),true);
    else
      cqiComputationConfig.cqiMeasurementConfig.set_size(startIndx+csiProcessInfo(processCnt).getCSIConfigCount(),false);
    
    
    //NOTE : see the need for modification of totalCSIRSConfigs variable 
    ivec totalCSIRSConfigs;
    if(find(csiProcessInfo(processCnt).getCSIRSCount_NZP(),-1)!=0 )
    {
      if(find(csiProcessInfo(processCnt).getCSIRSCount_ZP(),-1)!=0)
	totalCSIRSConfigs = concat(csiProcessInfo(processCnt).getCSIRSCount_NZP(),csiProcessInfo(processCnt).getCSIRSCount_ZP());
      else
	totalCSIRSConfigs = csiProcessInfo(processCnt).getCSIRSCount_NZP();
    }
    
    for(int cnt=0;cnt<CSIProcessMeasurementConfigs.length();cnt++)
    {
      int indx = startIndx + cnt;
      cqiComputationConfig.cqiMeasurementConfig(indx).requestID=totalCSIRSConfigs(cnt);//rand();
      cqiComputationConfig.cqiMeasurementConfig(indx).nuID=nuID;
      
      cqiComputationConfig.cqiMeasurementConfig(indx).transmissionScheme =_TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_;
      cqiComputationConfig.cqiMeasurementConfig(indx).cqiPortCountPerServerNode = nPorts;
      // int nPorts=sum(cqiComputationConfig.cqiMeasurementConfig(indx).cqiPortCountPerServerNode);
      // int nPorts = csiprocessesPerUE.getportCount();
      
      
      cqiComputationConfig.cqiMeasurementConfig(indx).cqiPorts = getIntegers(15,15+nPorts-1)*100 + int(totalCSIRSConfigs(cnt));
      
      if(nPorts>1)
	cqiComputationConfig.cqiMeasurementConfig(indx).numberOfCodeWords=2;
      else
	cqiComputationConfig.cqiMeasurementConfig(indx).numberOfCodeWords=2;
      
      
      cqiComputationConfig.cqiMeasurementConfig(indx).transmissionScheme=tempcqiComputationConfig.cqiMeasurementConfig(0).transmissionScheme;
      cqiComputationConfig.cqiMeasurementConfig(indx).numberOfCodeWords=tempcqiComputationConfig.cqiMeasurementConfig(0).numberOfCodeWords;
      cqiComputationConfig.cqiMeasurementConfig(indx).numberOfLayers=tempcqiComputationConfig.cqiMeasurementConfig(0).numberOfLayers;
      cqiComputationConfig.cqiMeasurementConfig(indx).serverNodeGroup = tempcqiComputationConfig.cqiMeasurementConfig(0).serverNodeGroup;
      cqiComputationConfig.cqiMeasurementConfig(indx).CSIProcessId= csiProcessInfo(processCnt).getCQIProcessId();
      cqiComputationConfig.cqiMeasurementConfig(indx).CSIConfiguration = totalCSIRSConfigs(cnt);
      
    }  
  }
  
  //test patch
  for(int i =0;i<cqiComputationConfig.cqiMeasurementConfig.size();i++)
    cout<<"cqiComputationConfig.cqiMeasurementConfig.CSIConfiguration:"<<cqiComputationConfig.cqiMeasurementConfig(i).CSIConfiguration<<endl;
  cout<<"cqiComputationConfig.cqiMeasurementConfig.size():"<<cqiComputationConfig.cqiMeasurementConfig.size()<<endl;
  
}

void updateCSIProcessInfo(cCSIProcessesInfo &csiProcessInfo,int portCount, int proID, ivec NZPCSIConfig)
{
  
  int CSIConfigCnt = NZPCSIConfig.length();
  ivec ZPCSIConfig = "";
  
  csiProcessInfo.setportCount(portCount);
  csiProcessInfo.setCQIProcessId(proID);
  csiProcessInfo.setCSIConfigCount( CSIConfigCnt);
  csiProcessInfo.setCSIRSCount_NZP(NZPCSIConfig);
  csiProcessInfo.setCSIRSCount_ZP(ZPCSIConfig);
}
//

