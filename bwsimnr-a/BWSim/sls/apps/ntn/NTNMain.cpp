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


#include "../../../lib/L1/SRS/include/srsTx.h"
#include "../../../lib/L1/SRS/include/srsRx.h"
#include "../../Mcell/include/MCell.h"
#include "../../sim/include/SupportingFunctions.h"

int main(int argc, char** argv){
    
    
    ConfigFileNames_S myConfigFiles;
    ResultFileNames_S resultFiles;
    initAllFiles(argc,argv,myConfigFiles,resultFiles);
    
    MCell mcell = createMcellNetwork(myConfigFiles,resultFiles);
  
  
  int fftSize = 1024;
  int nSlots = 1;
  int scsInKhz = 15;
  int ulBW = 52;
  int gc = (fftSize-ulBW*12)/2;
  double samplingTimeinSec = 1/(double)(fftSize*scsInKhz*1000);	//Parameters are hardcoded
  
  
  double currentTimeInSec, endTimeInSec;
  
  
  // Generating SRS config for each UE
  Array<srsConfig> srsconfig;
  srsconfig = generateSRSconfig(mcell.associationInfo);
  ivec UE = mcell.associationInfo.serviceNodes;
  vec srsTXpower(UE.length());  
  

  for(int UEindx = 0;UEindx < UE.size();UEindx++){
    
  
  // Calculating SRS Tx power
  double pathloss;
  sBeamPairLink beamPairLink;
  beamPairLink = mcell.associationInfo.associatedBeamPairLinkforEachServiceNode(UEindx);
  pathloss = beamPairLink.rspIndBm - beamPairLink.rsrpIndBm;
  srsTXpower(UEindx) = getSRSUplinkPower(23,0.8,pathloss,srsconfig(UEindx));
  
  }
  
  // Slot level transmission and reception
  for(int slotIndx = 0;slotIndx < nSlots; slotIndx++){
    
    currentTimeInSec = (((1024+80)*2+(1024+72)*12)*slotIndx)*samplingTimeinSec;
    endTimeInSec = (((1024+80)*2+(1024+72)*12)*(slotIndx+1))*samplingTimeinSec;

    
    ivec gNB = mcell.associationInfo.serverNodes;
    Array<Array<cvec>> rxData(gNB.size());
    for(int UEindx = 0;UEindx < UE.size();UEindx++){
      
      // Sequence Generation
      srsConfig srsConTemp;
      srsConTemp = srsconfig(UEindx);
      cmat srsSymbols(srsConTemp.nSRSports,srsConTemp.seqLength);
      if(srsConTemp.nSRSports == 8){
	srsConTemp.nSRSports = 4;
	cmat srsSymbTemp;
	srsSymbTemp = generateZCseq(srsConTemp);
	srsSymbols.set_submatrix(0,3,0,srsConTemp.seqLength-1,srsSymbTemp);
	srsSymbols.set_submatrix(4,7,0,srsConTemp.seqLength-1,srsSymbTemp);
      }
      else{
	cout<<endl<<"Abort: Function not yet written for < 4 Ports"<<endl;abort();
      }
      
      // Resource Mapping
      Array<cmat> txGrid(srsconfig(UEindx).nSRSports);
      for(int portIndx = 0;portIndx < srsconfig(UEindx).nSRSports;portIndx++){
	txGrid(portIndx) = zeros_c(fftSize,14);
      }
      
      for(int symbIndx = 0;symbIndx < srsconfig(UEindx).symbols.size();symbIndx++){
	for(int portIndx = 0;portIndx < srsconfig(UEindx).nSRSports;portIndx++){
	  int combOffset;
	  if(portIndx < 4){combOffset = srsconfig(UEindx).combOffset;}
	  else{combOffset = srsconfig(UEindx).combOffset+2;}
	  ivec scIndices=gc+getIntegers(0,srsconfig(UEindx).nRBs*12-1,srsconfig(UEindx).K_TC)+combOffset+srsconfig(UEindx).rbOffset+symbIndx*srsconfig(UEindx).nRBs;
	  scIndices = mod(scIndices,48*12);
	  for(int scIndx = 0;scIndx < scIndices.size();scIndx++){
	    if(portIndx >= 0){
	      txGrid(portIndx)(scIndices(scIndx),srsconfig(UEindx).symbols(symbIndx)) = srsSymbols(portIndx,scIndx);
	    }
	    else{
	      txGrid(portIndx)(scIndices(scIndx),srsconfig(UEindx).symbols(symbIndx)) = 0;
	    }
	  }
	}
      }

      
      // Power amplification and transmission in time domain
      Array<cvec> timeDomainFrame;
      timeDomainFrame = getTimeDomainFrame(txGrid, fftSize, 0);
      for(int portIndx = 0;portIndx < timeDomainFrame.length();portIndx++){
	timeDomainFrame(portIndx) = sqrt(srsTXpower(UEindx)/srsconfig(UEindx).nSRSports)*timeDomainFrame(portIndx);
      }
      mcell.mySys.transmitData(UE(UEindx),0,0,fftSize,timeDomainFrame,currentTimeInSec,2);
    }
    
    Array<Array<Array<Array<cvec>>>> channels;
    Array<ivec> assocTxNodeIds;
    channels.set_size(gNB.size());
    assocTxNodeIds.set_size(gNB.size());
    // Receiving SRS
    ChannelOutput_S channelOutput;
    for(int gNBindx = 0;gNBindx < gNB.size();gNBindx++){
      cout<<endl<<"Receiving by gNB"<<gNBindx;
      channelOutput = mcell.mySys.receiveData(gNB(gNBindx),0,0,_ANTENNA_CORRELATION_MEDIUM_,0,currentTimeInSec,endTimeInSec,false,to_ivec(gNB(gNBindx)+3));
      rxData(gNBindx) = channelOutput.data;
      channels(gNBindx) = channelOutput.channels;
      assocTxNodeIds(gNBindx) = channelOutput.txNodeIDs;
    }
    
    Array<cvec> idealChannel;
    cvec estChannel, freqChannel;
    idealChannel = channels(0)(0)(0);
    int txNode = assocTxNodeIds(0)(0);
    ofstream fout0("PerfectChannel.txt");
    fout0.close();
      
    
    // Channel estimation using SRS
    for(int UEindx = 0;UEindx < UE.size();UEindx++){
      int assoc_gNB;
      assoc_gNB = mcell.associationInfo.getMyAssociatedServerNode(UE(UEindx));
      Array<cvec> gNBrxData;
      gNBrxData = rxData(assoc_gNB);
      
      for(int rxPortIndx = 0;rxPortIndx < gNBrxData.length();rxPortIndx++){
	gNBrxData(rxPortIndx) = gNBrxData(rxPortIndx)/sqrt(srsTXpower(UEindx)/srsconfig(UEindx).nSRSports);
      }
      
      //Getting Frequency domain slot
      Array<cmat> rxGrid;
      rxGrid = getFreqDomainFrame(gNBrxData,fftSize,0,14);
      
      
      srsConfig assoc_srsCon;
      assoc_srsCon = srsconfig(UEindx);
      
      // Getting base sequence
      cvec baseSeq;
      srsConfig srsTemp;
      srsTemp = assoc_srsCon;
      srsTemp.cyclicShift = 0;
      srsTemp.nSRSports = 1;
      cmat temp = generateZCseq(srsTemp);
      baseSeq = temp.get_row(0);
      
      
      Array<Array<cmat>> freqDomainChannel(assoc_srsCon.nSRSports);
      Array<Array<cmat>> timeDomainChannel(assoc_srsCon.nSRSports);
      for(int txPortIndx = 0;txPortIndx < assoc_srsCon.nSRSports;txPortIndx++){
	freqDomainChannel(txPortIndx).set_size(fftSize);
	timeDomainChannel(txPortIndx).set_size(gNBrxData.length());
	for(int rxPortIndx = 0;rxPortIndx < gNBrxData.size();rxPortIndx++){
	  freqDomainChannel(txPortIndx)(rxPortIndx) = zeros_c(fftSize,assoc_srsCon.symbols.size());
	  timeDomainChannel(txPortIndx)(rxPortIndx) = zeros_c(assoc_srsCon.seqLength/4,assoc_srsCon.symbols.size());
	}
      }
  
  
      for(int txPortIndx = 0;txPortIndx < assoc_srsCon.nSRSports;txPortIndx++){
	for(int rxPortIndx = 0;rxPortIndx < gNBrxData.size();rxPortIndx++){
	  for(int symbIndx = 0;symbIndx < assoc_srsCon.symbols.size();symbIndx++){
	    int combOffset;
	    if(txPortIndx < 4){combOffset = assoc_srsCon.combOffset;}
	    else{combOffset = assoc_srsCon.combOffset+2;}
	    ivec scIndices;
	    scIndices = gc+getIntegers(0,assoc_srsCon.nRBs*12-1,assoc_srsCon.K_TC)+combOffset+assoc_srsCon.rbOffset+symbIndx*assoc_srsCon.nRBs;
	    scIndices = mod(scIndices,48*12);
	    cvec temp(scIndices.size());
	    for(int scIndx = 0;scIndx < scIndices.size();scIndx++){
	      temp(scIndx) = (rxGrid(rxPortIndx)(scIndices(scIndx),assoc_srsCon.symbols(symbIndx))/baseSeq(scIndx));
	    }    
	    temp = ifft(temp);
	    
	    int cycShift = (txPortIndx*3*assoc_srsCon.seqLength/4)%assoc_srsCon.seqLength;
	    cvec temp1 = temp(cycShift,cycShift+assoc_srsCon.seqLength/4-1);
	    
	    if((txPortIndx == 0) && (rxPortIndx == 0)){
	      cout<<endl<<"Estimated Channel: "<<temp1<<endl;
	    }
	    
	    timeDomainChannel(txPortIndx)(rxPortIndx).set_col(symbIndx,temp1);
	    temp1 = fft(temp1,fftSize);	    
	    freqDomainChannel(txPortIndx)(rxPortIndx).set_col(symbIndx,temp1);
	  }
	}
      }
    }
    
    
  }
/*  
  // Print Results
  string toExec = "rm "+mcell.mySysInfo.resultFiles.linkTableFileName;  
  system_exec(toExec.c_str());  
  completeSimulation(mcell.mySysInfo.myConfigFiles.directory,mcell.mySysInfo.resultFiles.directory,mcell.mySysInfo.simulationTimeInSec);
  
*/
}






