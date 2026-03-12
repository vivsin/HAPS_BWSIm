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

#ifndef _DL_REF_SIG_GEN_H_
#define _DL_REF_SIG_GEN_H_

#include "FrameHandler.h"
#include "ResourceMapper.h"
#include "Precoder.h"
#include "SupportingFunctions.h"
#include "AWGNTables.h"


struct ReferenceSignal_S{
  
  Array<Array< cvec >> rsSequencePerRB;
  Array< Array<cvec> > CRSsequencePerPortPerSymbol;
  Array<ivec> CRSsymbolsPerPort;
  Array< Array<cvec> > CSIRSsequencePerPortPerSymbol;
  Array<ivec> CSIRSsymbolsPerPort;
  Array< Array<cvec> > DMRSsequencePerPortPerSymbol;
  Array<ivec> DMRSsymbolsPerPort;
  ivec portIndices;
  
  ReferenceSignal_S()
  {
    rsSequencePerRB.set_length(0);
    CRSsequencePerPortPerSymbol.set_length(0);
    CRSsymbolsPerPort.set_length(0);
    CSIRSsequencePerPortPerSymbol.set_length(0);
    CSIRSsymbolsPerPort.set_length(0);
    DMRSsequencePerPortPerSymbol.set_length(0);
    DMRSsymbolsPerPort.set_length(0);
    portIndices.set_length(0);
  }
  void clear(){
    
    rsSequencePerRB.set_length(0);
    CRSsequencePerPortPerSymbol.set_length(0);
    CRSsymbolsPerPort.set_length(0);
    CSIRSsequencePerPortPerSymbol.set_length(0);
    CSIRSsymbolsPerPort.set_length(0);
    DMRSsequencePerPortPerSymbol.set_length(0);
    DMRSsymbolsPerPort.set_length(0);
    portIndices.set_length(0);
  }
  Array<cvec> getTxRSPerPort(int rbNumber, ivec rsPorts)
  {
    ivec indices=find(portIndices,rsPorts);
    if(find(indices,-1)!=-1)
    { 
      cout<<"[both:] Reference signal for some ports not available in ReferenceSignal_S for getTxRSPerPort()..."<<endl;
      abort();
    }
    Array<cvec> txRS(rsPorts.length());
    for(int port_cnt=0;port_cnt<rsPorts.length();port_cnt++)
      txRS(port_cnt)=rsSequencePerRB(indices(port_cnt))(rbNumber);
    
    return(txRS);
  }
};

int getCDMGroupIndx(int portNumber);

/// SK: RSInfo_S is analogous to PilotPatternInfo_S
ivec getDMRSPortIndices(ivec portsUsed);
ivec getCRSPortIndices(ivec portsUsed);
ivec getCSIRSPortIndices(ivec portsUsed);
ivec getNRCSIRSPortIndices(ivec portsUsed);

ivec getDMRSPortNumbers(ivec portsUsed);
ivec getCRSPortNumbers(ivec portsUsed);
ivec getCSIRSPortNumbers(ivec portsUsed);
ivec getNRCSIRSPortNumbers(ivec portsUsed);

cvec getCRSsequence(int cellID, int symbolIndex, int slotIndex, int nPhysicalRBs);
cvec getDMRSsequence(int cellID, int nRNTI, int nSCID, int subFrameIndex, int portIndex, int nPhysicalRBs, ivec resourceBlockIDs);
cvec getNRDMRSsequence(int cellID, int nRNTI, int nSCID, int subFrameIndex, int portIndex, int nPhysicalRBs, ivec resourceBlockIDs, int nDMRSPerRB, int dmrssymbolnumber, int symno, int dmrstype, int nCDM);
cvec getCSIRSsequence(int cellID, int symbolIndex, int slotIndex, int nPhysicalRBs);
cvec getNRCSIRSsequence(int cellID, int symbolIndex, int slotIndex, int nPhysicalRBs, int rsCountPerSym, int symno, int FDCDM, int TDCDM, int portIndex);
cvec getMBSFNsequence(int MBSFNid, int symbolIndex, int slotIndex);
cvec getPRSsequence(int cellID, int symbolIndex, int slotIndex, int nPhysicalRBs);
void generateRS(ReferenceSignal_S& referenceSignal, int nRB, int cellID, int subFrameIndexInFrame, ivec portsUsedInSubframe, RBInfo_S& crsRBInfo, Array< DataBlkInfo_S >& dataBlkInfo, ivec nRNTI, bool isDownlink);
void populateRS(Array<cmat> &logicalFrame, RSInfo_S rsInfo, imat subframeStructure);
void populateCRS(Array<cmat> &logicalFrame, RSInfo_S rsInfo, imat  subframeStructure);
void fillRS(ReferenceSignal_S& referenceSignal, int logicalRUIndex, RBInfo_S RBinfo, Size_S rbSize, Framehandler& framehandler, ResourceMapper* resourceMapper, PortToAntennaMapper_S* portToPhysicalAntennaMapper, bool isDownlink, cmat precoder = "");
Array<cvec> getUplinkDMRS(int cellID, ivec scheduledRBs, Size_S rbSize, ivec portIndices, ivec rsCountPerRBPerPort, string referenceSignalScheme="");
void getCSIRSoffset(int CSIRSconfig, int nCSIports, int &symOffset, int &SCoffset, int frameType=2);/// SK: frametype hardcoded to 2
void getNRCSIRSoffset(int CSIRSconfig, int nNRCSIports, int &symOffset, int &SCoffset, int frameType=2);/// SK: frametype hardcoded to 2
void fillMyUplinkReferenceSignal(int cellID,Framehandler *frameHandler,PortToAntennaMapper_S *portToPhysicalAntennaMapper,ivec scheduledRBs,int totalRBs , ivec portNumbers,ReferenceSignal_S *referenceSignal,RBInfo_S *rbInfo,  string uplinkReferenceSignalScheme="");
void generateUplinkReferenceSignal(int cellID, ivec scheduledRBs, int totalRBs, ivec portNumbers, ReferenceSignal_S* referenceSignal, RBInfo_S* rbInfo, string uplinkReferenceSignalScheme = "");
void getRSportShifts(ivec &cellSpecificSubcarrierUpShift, ivec &cellSpecificSymbolRightShift, ivec rsPorts, int cellID, int CSIRSconfig=-1);
void getCSIRSportShifts(int &CSIRSConfigSpecificSubcarrierUpShift, int &CSIRSConfigSpecificSymbolRightShift, int nCSIRSPorts, int CSIRSconfig);
void getNRCSIRSportShifts(int &CSIRSConfigSpecificSubcarrierUpShift, int &CSIRSConfigSpecificSymbolRightShift, int nCSIRSPorts, int CSIRSconfig);
Array<cvec> getTxRSPerPort(ReferenceSignal_S *refSignal,ResourceMapper *resourceMapper,int vrbNumber,ivec portIndices,imat rbRSPattern, bool isDownlink);
cvec getPTRSsequenceTPenabled(int cellID, int symbolIndex, int slotIndex, int nSymbolPerSlot, int nRNTI, int puschSC, int scheduledBW, ModulationScheme_E MCS);
ivec getOrthoCoverCode(int portIndex);
Array<ivec> getNROrthoCoverCode(int portIndex);
Array<ivec> getNROrthoCoverCodeforCSIRS(int portIndex,int FDCDM, int TDCDM);
#endif
