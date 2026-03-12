#ifndef MCSI_H
#define MCSI_H

#include "StructsAndEnums.h"



class cCSIProcessesInfo
{
  int noCSIConfig;
  //Array<cCSIProcess > csiProcess;
  int processID;
  int portCountPerCSIProcess;
  
  ivec CSIRSConfig_NZP;
  ivec CSIRSCongfig_ZP;
  
public :
  cCSIProcessesInfo()
  {
    processID = 0;
    noCSIConfig =1;
    CSIRSConfig_NZP ="0";
    portCountPerCSIProcess = 8;
    
  };
  
  int getportCount(){return portCountPerCSIProcess;}
  int getCQIProcessId(){return processID;}
  int getCSIConfigCount(){return noCSIConfig;}
  ivec getCSIRSCount_NZP(){return CSIRSConfig_NZP;}
  ivec getCSIRSCount_ZP(){return CSIRSCongfig_ZP;}
  
  void setportCount(int  PortCount){portCountPerCSIProcess = PortCount;}
  void setCQIProcessId(int  proID){processID = proID;}
  void setCSIConfigCount(int CSIConfigCnt){ noCSIConfig = CSIConfigCnt;}
  void setCSIRSCount_NZP(ivec  NZPCSIConfig){ CSIRSConfig_NZP =NZPCSIConfig;}
  void setCSIRSCount_ZP(ivec ZPCSIConfig){ CSIRSCongfig_ZP = ZPCSIConfig;}
  
};
void addCSIProcessesPerUE(CQIComputationConfig_S& cqiComputationConfig, Array< cCSIProcessesInfo >& csiProcessInfo, int nuID);
void updateCSIProcessInfo(cCSIProcessesInfo &csiprocessesPerUE,int portCount, int proID, ivec NZPCSIConfig);
void initCSIProcesses( CQIComputationConfig_S& cqiComputationConfig, int nuID, TransmissionConfig_S& txConfig, imat& NZPCSIConfigurations);


#endif