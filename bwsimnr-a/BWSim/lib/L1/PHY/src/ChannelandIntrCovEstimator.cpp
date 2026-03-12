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

#include "../include/ChannelandIntrCovEstimator.h"
#include "../include/ReferenceSignalGenerator.h"

ChannelEstimator::ChannelEstimator(){

  samplingPeriod=-1;
  samplingFrequency=-1;
  frequencySpacing=-1;
  fftSize=-1;
}

bool ChannelEstimator::isReady(){

  bool ready=true;
  if(samplingPeriod==-1)
  {
    cout<<"[both:] Error... Sampling Period not set in ChannelEstimator... Use setChannelSamplingPeriod()..."<<endl;
    ready=false;
  }
  if(samplingFrequency==-1)
  {
    cout<<"[both:] Error... Sampling Frequency not set in ChannelEstimator... Use setChannelSamplingFrequency()..."<<endl;
    ready=false;
  }
  if(frequencySpacing==-1)
  {
    cout<<"[both:] Error... Frequency Spacing not set in ChannelEstimator... Use setChannelFrequencySpacing()..."<<endl;
    ready=false;
  }
  if(fftSize==-1)
  {
    cout<<"[both:] Error...FFT Size not set in ChannelEstimator... Use setFFTSize()..."<<endl;
    ready=false;
  }
  return(ready);
}

void ChannelEstimator::loadParams(double channelSamplingPeriod, double channelFrequencySpacing, int framefftSize){

  samplingPeriod=channelSamplingPeriod;
  samplingFrequency=channelFrequencySpacing*framefftSize;
  frequencySpacing=channelFrequencySpacing;
  fftSize=framefftSize;

}



void ChannelEstimator::setChannelSamplingPeriod(double period){

  samplingPeriod=period;
}

void ChannelEstimator::setChannelFrequencySpacing(double freqSpacing){

  frequencySpacing=freqSpacing;
}

void ChannelEstimator::setFFTSize(int size){

  fftSize=size;
}

/*! \fn estimateMyChannelinRB(...)
* \brief estimates the raw channel across all tones in a given RB using CRS.
* \param [in,out] estimatedChannelInRB ---> it contains the estimated channel in an RB and it dimension depends on number of ports and number of receiver antennas 
* \param [in,out] receivedRB --> Received RB.
* \param [in] myAttenuationFactorIndB ---> it is the link gain between server node to service node
* \param [in] myScRxPowerInWatts ---> received desired subcarrier power
* \param [in] interfererRxSCPowerInDB ---> received interference subcarrier power
* \param [in,out] txRSPerPort --> Transmitted RS sequence for given ports in given RB, with freq first and time next.
* \param [in] noiseVariance ---> noise variance
* \param [in] restOfintereferencePowerInWatts ---> weak interference signal power (otherthan 8 strongest interference signals)
* \param [in] rsPowerBoostInDB --> To boost RS power for better channel estimation
* \param [in] dopplerSpreadInHz --> Used by 2D-MMSE to calculate frequency correlation
* \param [in] fadingParameters --> Used by 2D-MMSE to calculate frequency correlation
* \param [in] portReferenceNumbers --> Index of ports for which channel is to estimated
* \param [in,out] rbRSPattern --> Matrix of RB dimensions with data indicated as "0" and ports indicated as "1, 2, 3, 4"
* \param [in] portIndices ---> port numbers for the given transmission mode
* \param [in] rbNumber ---> resource block number 
* \return returns the estimated channel for an RB
* \note Make sure that 6 interlaces are taken care in the rbRSPattern ///SK: check
*/

void ChannelEstimator::estimateMyChannelInRB(Array< Array<cmat> > &estimatedChannelInRB, Array<cmat> &receivedRB, double myAttenuationFactorIndB, vec interfererRxSCPowerInDB, Array<cvec> &txRSPerPort,double noiseVariance, double restOfintereferencePowerInWatts, double dopplerSpreadInHz, int nTaps, vec tapDel, vec tapGain, ivec portReferenceNumbers, imat &rbRSPattern, ivec portIndices, int rbNumber, bool isEPDCCH)
{

  if(!isReady())
  {
    cout<<"[both:] Please pass a fully configured channel estimator to estimateMyChannelInRB()..."<<endl;
    abort();
  }
  int rxAntenna=receivedRB.length(), cover, nSCs=receivedRB(0).rows(), portID=0;
  cvec rxRS;
  ivec orthogonalCover, repPorts=findUniqueNumbers(portReferenceNumbers);//portReferenceNumbers=repPorts=[6];for mode-8=portReferenceNumbers=[8 8]repPorts=[8]

  estimatedChannelInRB.set_size(portReferenceNumbers.length());
   for(int ip=0; ip<portReferenceNumbers.length(); ip++)
    estimatedChannelInRB(ip).set_size(rxAntenna);
   for(int iport=0; iport<repPorts.length(); iport++)
     {
      // cout<<"\n repPorts: \t"<<repPorts(iport);
      ivec portLocations=find(rbRSPattern, repPorts(iport), true);
   
      int nReps=find(portReferenceNumbers, repPorts(iport), true).length(), nTones=portLocations.length();// nReps=1 and nTones=12 for mode 7
      ivec portOffset1(nTones), portOffset2(nTones), portTimeOffset, portFreqOffset;
      int portNumber=portIndices(find(portReferenceNumbers, repPorts(iport), false).get(0));
      
    if((nReps==1) && (portNumber>=7 && portNumber<=14) )
    {
	orthogonalCover=getOrthoCoverCode(portNumber);
	orthogonalCover=ivec("1 -1 1 -1");
	for(int ploc=0; ploc<nTones; ploc++)
	{
	  portOffset1(ploc)=int(portLocations(ploc)/nSCs);
	  portOffset2(ploc)=mod(portLocations(ploc), nSCs);
	}
	portTimeOffset=findUniqueNumbers(portOffset1);
	portFreqOffset=findUniqueNumbers(portOffset2);
	sort(portTimeOffset);
	sort(portFreqOffset);
      
      for(int rx_ant=0;rx_ant<rxAntenna;rx_ant++)
       { 
	rxRS=getFromMatrix(receivedRB(rx_ant), portLocations);
	
	  for(int rxSym=0; rxSym<rxRS.length(); rxSym++)
	  { 
	    if(mod(mod(portLocations(rxSym), nSCs)-portFreqOffset(rxSym/portFreqOffset.length())+rbNumber,2)==0)
	    cover=orthogonalCover(int(portLocations(rxSym)/nSCs)-portTimeOffset(int(rxSym/portFreqOffset.length())));
	    else// if(mod(mod(portLocations(rxSym), nSCs)-portFreqOffset(rxSym/portFreqOffset.length())+rbNumber,2)==1)
	      cover=orthogonalCover(3-int(portLocations(rxSym)/nSCs)+portTimeOffset(int(rxSym/portFreqOffset.length())));
	    rxRS(rxSym)=rxRS(rxSym)*(double)cover;
	    txRSPerPort(portID)(rxSym)=txRSPerPort(portID)(rxSym)*(double)cover;
	  }
	  estimatedChannelInRB(portID)(rx_ant).set_size(rbRSPattern.rows(), rbRSPattern.cols());
	estimateBy2DMMSE(estimatedChannelInRB(portID)(rx_ant) ,txRSPerPort(portID), rxRS, rbRSPattern, portLocations, noiseVariance, inv_dB(myAttenuationFactorIndB), inv_dB(interfererRxSCPowerInDB), restOfintereferencePowerInWatts, dopplerSpreadInHz, nTaps,tapDel,tapGain);
      }
      portID++;
        
    }
    
           
   else if( (nReps>1) && (portNumber>=7 && portNumber<=14) ) 
    {
      ivec portNumbers=portIndices(find(portReferenceNumbers, repPorts(iport), true)), portLoc;
      for(int ploc=0; ploc<nTones; ploc++)
      {
	portOffset1(ploc)=int(portLocations(ploc)/nSCs);
	portOffset2(ploc)=mod(portLocations(ploc), nSCs);
      }
      portTimeOffset=findUniqueNumbers(portOffset1);
      portFreqOffset=findUniqueNumbers(portOffset2);
      sort(portTimeOffset);
      sort(portFreqOffset);

      int coverSize,nPortSCs;
      
      coverSize=portTimeOffset.length(); nPortSCs=nTones/coverSize;
      
      imat coverPerPort(portNumbers.length(), coverSize);
      Array<cvec> txRS1(nPortSCs), rxRS1(nPortSCs);
      for(int sc=0; sc<nPortSCs; sc++)
      {
	txRS1(sc).set_size(coverSize);
	rxRS1(sc).set_size(coverSize);
      }
      cvec txRS2(nPortSCs*portTimeOffset.length()), rxRS2(nPortSCs*portTimeOffset.length());
      cvec txRS3(coverSize);
      
      
   
      for(int pid=0; pid<portNumbers.length(); pid++)
	 coverPerPort.set_row(pid, getOrthoCoverCode(portNumbers(pid)));
	
//	append(coverPerPort,getNROrthoCoverCode(portNumbers(pid))(0)(0));
	
      for(int iport1=0; iport1<portNumbers.length(); iport1++)
      {
	
	 for(int sc=0; sc<nPortSCs; sc++)
	  for(int sym=0; sym<coverSize; sym++)
	    txRS1(sc)(sym) = txRSPerPort(portID)(sc+sym*nPortSCs);

	 
	for(int rx_ant=0;rx_ant<rxAntenna;rx_ant++)
	{
	  estimatedChannelInRB(portID)(rx_ant).set_size(rbRSPattern.rows(), rbRSPattern.cols());
	  rxRS=getFromMatrix(receivedRB(rx_ant), portLocations);
	 
	  for (int symCnt = 0; symCnt < portTimeOffset.length();symCnt++){
	   
	    for(int sc=0; sc<nPortSCs; sc++)
	      for(int sym=0; sym<coverSize; sym++)
		rxRS1(sc)(sym) = rxRS(sc+sym*nPortSCs);
	  
		
	    for(int sc=0; sc<nPortSCs; sc++)
	    {
	      if(mod(sc+rbNumber,2)==1)
	      for(int r=0; r<coverPerPort.rows(); r++)
		coverPerPort.set_row(r, reverse(coverPerPort.get_row(r)));
	    txRS1(sc) = elem_mult(txRS1(sc), to_cvec(coverPerPort.get_row(iport1)));
	    txRS2(sc+symCnt*nPortSCs) = txRS1(sc)(iport1);
	    txRS3 = elem_div(ones_c(coverSize)*prod(txRS1(sc)), txRS1(sc));
	    rxRS2(sc+symCnt*nPortSCs) = sum(elem_mult(elem_mult(txRS3, to_cvec(coverPerPort.get_row(iport1))), rxRS1(sc)))*(txRS1(sc)(iport1)/((double)coverSize*prod(txRS1(sc))));
	  }
	}
	 
	 portLoc.set_size(nPortSCs);
	  for(int sc=0; sc<nPortSCs; sc++)
	    portLoc(sc)=portLocations(iport1*nPortSCs+sc);
	 	  
	estimateBy2DMMSE(estimatedChannelInRB(portID)(rx_ant) ,txRS2, rxRS2, rbRSPattern, portLoc, noiseVariance, inv_dB(myAttenuationFactorIndB), inv_dB(interfererRxSCPowerInDB), restOfintereferencePowerInWatts, dopplerSpreadInHz,  nTaps,tapDel,tapGain);
	
	  
	}
	
	portID++;
      }
    }
    else if ( (find(repPorts,1000)!=-1) || (find(repPorts,1100)!=-1) )
    {
      
      // cout<<"\n inside nrpes>4";
      ivec portNumbers=portIndices(find(portReferenceNumbers, repPorts(iport), true));
      int portFirst;
     // cout<<"\n portNumbers=\t"<<portNumbers<<endl;
      for(int ploc=0; ploc<nTones; ploc++)
      {
	portOffset1(ploc)=int(portLocations(ploc)/nSCs);
	portOffset2(ploc)=mod(portLocations(ploc), nSCs);
      }
      portTimeOffset=findUniqueNumbers(portOffset1);
      portFreqOffset=findUniqueNumbers(portOffset2);
      sort(portTimeOffset);
      sort(portFreqOffset);
      
      int coverSizeFreq,coverSizeTime,nPortSCs, maxPortsInOneSym;
      
      nPortSCs=portFreqOffset.length();
      
      
      if (find(repPorts,1000)!=-1)
      {maxPortsInOneSym = 2; portFirst = 1000; }
      else if (find(repPorts,1100)!=-1)
      {maxPortsInOneSym = 3; portFirst = 1100; }
      
      
      if (portNumbers.length() > maxPortsInOneSym)
      { coverSizeFreq=2;  coverSizeTime=2; }
      
      else
      { 
	coverSizeFreq=2;  coverSizeTime=1;
      }
      
      int numRSSym = portTimeOffset.length()/coverSizeTime;
      Array<Array<ivec>> coverPerPort(portNumbers.length());
      
      for (int pid = 0; pid < portNumbers.length(); pid++)
      {
	coverPerPort(pid).set_size(coverSizeFreq);
	for (int ind1 = 0; ind1 < coverSizeFreq; ind1++)
	  coverPerPort(pid)(ind1).set_size(coverSizeTime);
	
	if (portReferenceNumbers.length() > maxPortsInOneSym) 
	  coverPerPort(pid) = getNROrthoCoverCode(portNumbers(pid));
	else {
	  Array<ivec> coverCode = getNROrthoCoverCode(portNumbers(pid));
	  
	  coverPerPort(pid)(0)= coverCode(0)(0) ;
	  coverPerPort(pid)(1) = coverCode(1)(0) ; }
	  
      }
      
      
      Array<cvec> txRS1(nPortSCs), rxRS1(nPortSCs),txRS3(coverSizeFreq);
      for(int sc=0; sc<nPortSCs; sc++)
      {
	txRS1(sc).set_size(coverSizeTime);
	rxRS1(sc).set_size(coverSizeTime);
      }
      
      for(int sc=0; sc<coverSizeFreq; sc++)
      {
	txRS3(sc).set_size(coverSizeTime);
      }
      
      cvec txRS2(nPortSCs*portTimeOffset.length() ), rxRS2(nPortSCs*portTimeOffset.length() );
      
      for(int iport1=0; iport1<portNumbers.length(); iport1++)
      {
	//cout<<"\n portnumber\t"<< portNumbers(iport1)<<'\t'<<portNumbers(iport1)-portFirst;
	Array<ivec> coverCode;
	
	coverCode = coverPerPort(iport1);
	for(int sc=0; sc<nPortSCs; sc++)
	  for (int sym = 0; sym < portTimeOffset.length();sym++ )
	    txRS1(sc)(sym) = txRSPerPort((portNumbers(iport1)-portFirst))(sc+sym*nPortSCs);
	  
	  for(int rx_ant=0;rx_ant<rxAntenna;rx_ant++)
	  {
	    estimatedChannelInRB((portNumbers(iport1)-portFirst))(rx_ant).set_size(rbRSPattern.rows(), rbRSPattern.cols());
	    rxRS=getFromMatrix(receivedRB(rx_ant), portLocations);
	    
	    for(int sc=0; sc<nPortSCs; sc++)
	      for(int sym=0; sym<portTimeOffset.length(); sym++)
		rxRS1(sc)(sym) = rxRS(sc+sym*nPortSCs);
	      
	      for(int symCnt = 0; symCnt<numRSSym ;symCnt++)
	      { 
		Array<cvec> matRx(coverSizeFreq),matTx(coverSizeFreq);
		for (int ind = 0; ind < coverSizeFreq;ind++)
		{ 
		  matRx(ind).set_size(coverSizeTime);
		  matTx(ind).set_size(coverSizeTime);
		}
		
		for(int sc=0; sc<nPortSCs; sc++)
		{
		  
		  if (portNumbers.length() > maxPortsInOneSym){
		    int colInd1 = 2*symCnt, colInd2 = 2*symCnt+1;
		    int rowInd1 = sc - mod(sc,2), rowInd2 = rowInd1+1;
		    
		    matRx(0)(0) = rxRS1(rowInd1)(colInd1)*(double)coverCode(0)(0);
		    matRx(0)(1) = rxRS1(rowInd1)(colInd2)*(double)coverCode(0)(1);
		    matRx(1)(0) = rxRS1(rowInd2)(colInd1)*(double)coverCode(1)(0);
		    matRx(1)(1) = rxRS1(rowInd2)(colInd2)*(double)coverCode(1)(1);
		    
		    matTx(0)(0) = txRS1(rowInd1)(colInd1)*(double)coverCode(0)(0);
		    matTx(0)(1) = txRS1(rowInd1)(colInd2)*(double)coverCode(0)(1);
		    matTx(1)(0) = txRS1(rowInd2)(colInd1)*(double)coverCode(1)(0);
		    matTx(1)(1) = txRS1(rowInd2)(colInd2)*(double)coverCode(1)(1);
		    
		    txRS3(0)(0) = matRx(0)(0) / matTx(0)(0);
		    txRS3(0)(1) = matRx(0)(1) / matTx(0)(1);
		    txRS3(1)(0) = matRx(1)(0) / matTx(1)(0);
		    txRS3(1)(1) = matRx(1)(1) / matTx(1)(1);
		    
		    cvec sumMat(2);		               			
		    
		    sumMat(0) = txRS3(0)(0) + txRS3(0)(1) + txRS3(1)(0) + txRS3(1)(1);
		    
		    rxRS2(sc+colInd1*nPortSCs) = (sumMat(0) * matTx(mod(sc,2))(0))/(double)(coverSizeFreq * coverSizeTime);
		    rxRS2(sc+colInd2*nPortSCs) = (sumMat(0) * matTx(mod(sc,2))(1))/(double)(coverSizeFreq * coverSizeTime);
		    txRS2(sc+colInd1*nPortSCs) = matTx(mod(sc,2))(0);
		    txRS2(sc+colInd2*nPortSCs) = matTx(mod(sc,2))(1);
		    
		  }
		  else{
		    int colInd1 = symCnt;
		    int rowInd1 = sc - mod(sc,2), rowInd2 = rowInd1+1;   
		    
		    matRx(0)(0) = rxRS1(rowInd1)(colInd1)*(double)coverCode(0)(0);
		    matRx(1)(0) = rxRS1(rowInd2)(colInd1)*(double)coverCode(1)(0);
		    
		    matTx(0)(0) = txRS1(rowInd1)(colInd1)*(double)coverCode(0)(0);
		    matTx(1)(0) = txRS1(rowInd2)(colInd1)*(double)coverCode(1)(0);
		    
		    txRS3(0)(0) = matRx(0)(0) / matTx(0)(0);
		    txRS3(1)(0) = matRx(1)(0) / matTx(1)(0);
		    
		    rxRS2(sc+symCnt*nPortSCs) = ((txRS3(0)(0)+txRS3(1)(0)) * matTx(mod(sc,2))(0))/(double)(coverSizeFreq * coverSizeTime);
		    txRS2(sc+symCnt*nPortSCs) = matTx(mod(sc,2))(0);
		    
		  }
		  
		}
	      }
	      //cout<<"\n port filles=\t"<<portNumbers(iport1)-portFirst;
	      estimateBy2DMMSE(estimatedChannelInRB((portNumbers(iport1)-portFirst))(rx_ant) ,txRS2, rxRS2, rbRSPattern, portLocations, noiseVariance, inv_dB(myAttenuationFactorIndB), inv_dB(interfererRxSCPowerInDB), restOfintereferencePowerInWatts, dopplerSpreadInHz,  nTaps,tapDel,tapGain);
	  }
       }  
     }
    
   }
}


void ChannelEstimator::estimateMyChannelInRB(Array< Array<cmat> > &estimatedChannelInRB, Array<cmat> &receivedRB, double myAttenuationFactorIndB, cmat estimatedIntrCovInRB, Array<cvec> &txRSPerPort, double dopplerSpreadInHz, int nTaps, vec tapDel, vec tapGain, ivec portReferenceNumbers, imat &rbRSPattern, ivec portIndices, int rbNumber, bool isEPDCCH)
{
    
    if(!isReady())
    {
        cout<<"[both:] Please pass a fully configured channel estimator to estimateMyChannelInRB()..."<<endl;
        abort();
    }
    int rxAntenna=receivedRB.length(), cover, nSCs=receivedRB(0).rows(), portID=0;
    cvec rxRS;
    ivec orthogonalCover, repPorts=findUniqueNumbers(portReferenceNumbers);//portReferenceNumbers=repPorts=[6];for mode-8=portReferenceNumbers=[8 8]repPorts=[8]
    
    estimatedChannelInRB.set_size(portReferenceNumbers.length());
    for(int ip=0; ip<portReferenceNumbers.length(); ip++)
        estimatedChannelInRB(ip).set_size(rxAntenna);
    for(int iport=0; iport<repPorts.length(); iport++)
    {
        // cout<<"\n repPorts: \t"<<repPorts(iport);
        ivec portLocations=find(rbRSPattern, repPorts(iport), true);
        
        int nReps=find(portReferenceNumbers, repPorts(iport), true).length(), nTones=portLocations.length();// nReps=1 and nTones=12 for mode 7
        ivec portOffset1(nTones), portOffset2(nTones), portTimeOffset, portFreqOffset;
        int portNumber=portIndices(find(portReferenceNumbers, repPorts(iport), false).get(0));
        
        if((nReps==1) && (portNumber>=7 && portNumber<=14) )
        {
            orthogonalCover=getOrthoCoverCode(portNumber);
            orthogonalCover=ivec("1 -1 1 -1");
            for(int ploc=0; ploc<nTones; ploc++)
            {
                portOffset1(ploc)=int(portLocations(ploc)/nSCs);
                portOffset2(ploc)=mod(portLocations(ploc), nSCs);
            }
            portTimeOffset=findUniqueNumbers(portOffset1);
            portFreqOffset=findUniqueNumbers(portOffset2);
            sort(portTimeOffset);
            sort(portFreqOffset);
            
            for(int rx_ant=0;rx_ant<rxAntenna;rx_ant++)
            { 
                rxRS=getFromMatrix(receivedRB(rx_ant), portLocations);
                for(int rxSym=0; rxSym<rxRS.length(); rxSym++)
                { 
                    if(mod(mod(portLocations(rxSym), nSCs)-portFreqOffset(rxSym/portFreqOffset.length())+rbNumber,2)==0)
                        cover=orthogonalCover(int(portLocations(rxSym)/nSCs)-portTimeOffset(int(rxSym/portFreqOffset.length())));
                    else// if(mod(mod(portLocations(rxSym), nSCs)-portFreqOffset(rxSym/portFreqOffset.length())+rbNumber,2)==1)
                        cover=orthogonalCover(3-int(portLocations(rxSym)/nSCs)+portTimeOffset(int(rxSym/portFreqOffset.length())));
                    rxRS(rxSym)=rxRS(rxSym)*(double)cover;
                    txRSPerPort(portID)(rxSym)=txRSPerPort(portID)(rxSym)*(double)cover;
                }
                estimatedChannelInRB(portID)(rx_ant).set_size(rbRSPattern.rows(), rbRSPattern.cols());
                estimateBy2DMMSE(estimatedChannelInRB(portID)(rx_ant) ,txRSPerPort(portID), rxRS, rbRSPattern, portLocations, inv_dB(myAttenuationFactorIndB), abs(estimatedIntrCovInRB(rx_ant,rx_ant)), dopplerSpreadInHz, nTaps,tapDel,tapGain);
            }
            portID++;
            
        }
        
        
        else if( (nReps>1) && (portNumber>=7 && portNumber<=14) ) 
        {
            ivec portNumbers=portIndices(find(portReferenceNumbers, repPorts(iport), true)), portLoc;
            for(int ploc=0; ploc<nTones; ploc++)
            {
                portOffset1(ploc)=int(portLocations(ploc)/nSCs);
                portOffset2(ploc)=mod(portLocations(ploc), nSCs);
            }
            portTimeOffset=findUniqueNumbers(portOffset1);
            portFreqOffset=findUniqueNumbers(portOffset2);
            sort(portTimeOffset);
            sort(portFreqOffset);
            
            int coverSize,nPortSCs;
            
            coverSize=portTimeOffset.length(); nPortSCs=nTones/coverSize;
            
            imat coverPerPort(portNumbers.length(), coverSize);
            Array<cvec> txRS1(nPortSCs), rxRS1(nPortSCs);
            for(int sc=0; sc<nPortSCs; sc++)
            {
                txRS1(sc).set_size(coverSize);
                rxRS1(sc).set_size(coverSize);
            }
            cvec txRS2(nPortSCs*portTimeOffset.length()), rxRS2(nPortSCs*portTimeOffset.length());
            cvec txRS3(coverSize);
            
            
            
            for(int pid=0; pid<portNumbers.length(); pid++)
                coverPerPort.set_row(pid, getOrthoCoverCode(portNumbers(pid)));
            
            //	append(coverPerPort,getNROrthoCoverCode(portNumbers(pid))(0)(0));
            
            for(int iport1=0; iport1<portNumbers.length(); iport1++)
            {
                
                for(int sc=0; sc<nPortSCs; sc++)
                    for(int sym=0; sym<coverSize; sym++)
                        txRS1(sc)(sym) = txRSPerPort(portID)(sc+sym*nPortSCs);
                    
                    
                    for(int rx_ant=0;rx_ant<rxAntenna;rx_ant++)
                    {
                        estimatedChannelInRB(portID)(rx_ant).set_size(rbRSPattern.rows(), rbRSPattern.cols());
                        rxRS=getFromMatrix(receivedRB(rx_ant), portLocations);
                        
                        for (int symCnt = 0; symCnt < portTimeOffset.length();symCnt++){
                            
                            for(int sc=0; sc<nPortSCs; sc++)
                                for(int sym=0; sym<coverSize; sym++)
                                    rxRS1(sc)(sym) = rxRS(sc+sym*nPortSCs);
                                
                                
                                for(int sc=0; sc<nPortSCs; sc++)
                                {
                                    if(mod(sc+rbNumber,2)==1)
                                        for(int r=0; r<coverPerPort.rows(); r++)
                                            coverPerPort.set_row(r, reverse(coverPerPort.get_row(r)));
                                        txRS1(sc) = elem_mult(txRS1(sc), to_cvec(coverPerPort.get_row(iport1)));
                                    txRS2(sc+symCnt*nPortSCs) = txRS1(sc)(iport1);
                                    txRS3 = elem_div(ones_c(coverSize)*prod(txRS1(sc)), txRS1(sc));
                                    rxRS2(sc+symCnt*nPortSCs) = sum(elem_mult(elem_mult(txRS3, to_cvec(coverPerPort.get_row(iport1))), rxRS1(sc)))*(txRS1(sc)(iport1)/((double)coverSize*prod(txRS1(sc))));
                                }
                        }
                        
                        portLoc.set_size(nPortSCs);
                        for(int sc=0; sc<nPortSCs; sc++)
                            portLoc(sc)=portLocations(iport1*nPortSCs+sc);
                        
                        estimateBy2DMMSE(estimatedChannelInRB(portID)(rx_ant) ,txRS2, rxRS2, rbRSPattern, portLoc, inv_dB(myAttenuationFactorIndB), abs(estimatedIntrCovInRB(rx_ant,rx_ant)), dopplerSpreadInHz,  nTaps,tapDel,tapGain);
                        
                        
                    }
                    
                    portID++;
            }
        }
        else if ( (find(repPorts,1000)!=-1) || (find(repPorts,1100)!=-1) )
        {
            
            // cout<<"\n inside nrpes>4";
            ivec portNumbers=portIndices(find(portReferenceNumbers, repPorts(iport), true));
            int portFirst;
            for(int ploc=0; ploc<nTones; ploc++)
            {
                portOffset1(ploc)=int(portLocations(ploc)/nSCs);
                portOffset2(ploc)=mod(portLocations(ploc), nSCs);
            }
            portTimeOffset=findUniqueNumbers(portOffset1);
            portFreqOffset=findUniqueNumbers(portOffset2);
            sort(portTimeOffset);
            sort(portFreqOffset);
            
            int coverSizeFreq,coverSizeTime,nPortSCs, maxPortsInOneSym;
            
            nPortSCs=portFreqOffset.length();
            
            
            if (find(repPorts,1000)!=-1)
            {maxPortsInOneSym = 2; portFirst = 1000; }
            else if (find(repPorts,1100)!=-1)
            {maxPortsInOneSym = 3; portFirst = 1100; }
            
            
            if (portNumbers.length() > maxPortsInOneSym)
            { coverSizeFreq=2;  coverSizeTime=2; }
            
            else
            { 
                coverSizeFreq=2;  coverSizeTime=1;
            }
            
            int numRSSym = portTimeOffset.length()/coverSizeTime;
            Array<Array<ivec>> coverPerPort(portNumbers.length());
            
            for (int pid = 0; pid < portNumbers.length(); pid++)
            {
                coverPerPort(pid).set_size(coverSizeFreq);
                for (int ind1 = 0; ind1 < coverSizeFreq; ind1++)
                    coverPerPort(pid)(ind1).set_size(coverSizeTime);
                
                if (portReferenceNumbers.length() > maxPortsInOneSym) 
                    coverPerPort(pid) = getNROrthoCoverCode(portNumbers(pid));
                else {
                    Array<ivec> coverCode = getNROrthoCoverCode(portNumbers(pid));
                    
                    coverPerPort(pid)(0)= coverCode(0)(0) ;
                    coverPerPort(pid)(1) = coverCode(1)(0) ; }
                    
            }
            
            
            Array<cvec> txRS1(nPortSCs), rxRS1(nPortSCs),txRS3(coverSizeFreq);
            for(int sc=0; sc<nPortSCs; sc++)
            {
                txRS1(sc).set_size(coverSizeTime);
                rxRS1(sc).set_size(coverSizeTime);
            }
            
            for(int sc=0; sc<coverSizeFreq; sc++)
            {
                txRS3(sc).set_size(coverSizeTime);
            }
            
            cvec txRS2(nPortSCs*portTimeOffset.length() ), rxRS2(nPortSCs*portTimeOffset.length() );
            
            for(int iport1=0; iport1<portNumbers.length(); iport1++)
            {
                //cout<<"\n portnumber\t"<< portNumbers(iport1)<<'\t'<<portNumbers(iport1)-portFirst;
                Array<ivec> coverCode;
                
                coverCode = coverPerPort(iport1);
                for(int sc=0; sc<nPortSCs; sc++)
                    for (int sym = 0; sym < portTimeOffset.length();sym++ )
                        txRS1(sc)(sym) = txRSPerPort((portNumbers(iport1)-portFirst))(sc+sym*nPortSCs);
                    
                    for(int rx_ant=0;rx_ant<rxAntenna;rx_ant++)
                    {
                        estimatedChannelInRB((portNumbers(iport1)-portFirst))(rx_ant).set_size(rbRSPattern.rows(), rbRSPattern.cols());
                        rxRS=getFromMatrix(receivedRB(rx_ant), portLocations);
                        for(int sc=0; sc<nPortSCs; sc++)
                            for(int sym=0; sym<portTimeOffset.length(); sym++)
                                rxRS1(sc)(sym) = rxRS(sc+sym*nPortSCs);
                            
                            for(int symCnt = 0; symCnt<numRSSym ;symCnt++)
                            { 
                                Array<cvec> matRx(coverSizeFreq),matTx(coverSizeFreq);
                                for (int ind = 0; ind < coverSizeFreq;ind++)
                                { 
                                    matRx(ind).set_size(coverSizeTime);
                                    matTx(ind).set_size(coverSizeTime);
                                }
                                
                                for(int sc=0; sc<nPortSCs; sc++)
                                {
                                    
                                    if (portNumbers.length() > maxPortsInOneSym){
                                        int colInd1 = 2*symCnt, colInd2 = 2*symCnt+1;
                                        int rowInd1 = sc - mod(sc,2), rowInd2 = rowInd1+1;
                                        
                                        matRx(0)(0) = rxRS1(rowInd1)(colInd1)*(double)coverCode(0)(0);
                                        matRx(0)(1) = rxRS1(rowInd1)(colInd2)*(double)coverCode(0)(1);
                                        matRx(1)(0) = rxRS1(rowInd2)(colInd1)*(double)coverCode(1)(0);
                                        matRx(1)(1) = rxRS1(rowInd2)(colInd2)*(double)coverCode(1)(1);
                                        
                                        matTx(0)(0) = txRS1(rowInd1)(colInd1)*(double)coverCode(0)(0);
                                        matTx(0)(1) = txRS1(rowInd1)(colInd2)*(double)coverCode(0)(1);
                                        matTx(1)(0) = txRS1(rowInd2)(colInd1)*(double)coverCode(1)(0);
                                        matTx(1)(1) = txRS1(rowInd2)(colInd2)*(double)coverCode(1)(1);
                                        
                                        txRS3(0)(0) = matRx(0)(0) / matTx(0)(0);
                                        txRS3(0)(1) = matRx(0)(1) / matTx(0)(1);
                                        txRS3(1)(0) = matRx(1)(0) / matTx(1)(0);
                                        txRS3(1)(1) = matRx(1)(1) / matTx(1)(1);
                                        
                                        cvec sumMat(2);		               			
                                        
                                        sumMat(0) = txRS3(0)(0) + txRS3(0)(1) + txRS3(1)(0) + txRS3(1)(1);
                                        
                                        rxRS2(sc+colInd1*nPortSCs) = (sumMat(0) * matTx(mod(sc,2))(0))/(double)(coverSizeFreq * coverSizeTime);
                                        rxRS2(sc+colInd2*nPortSCs) = (sumMat(0) * matTx(mod(sc,2))(1))/(double)(coverSizeFreq * coverSizeTime);
                                        txRS2(sc+colInd1*nPortSCs) = matTx(mod(sc,2))(0);
                                        txRS2(sc+colInd2*nPortSCs) = matTx(mod(sc,2))(1);
                                        
                                    }
                                    else{
                                        int colInd1 = symCnt;
                                        int rowInd1 = sc - mod(sc,2), rowInd2 = rowInd1+1;   
                                        
                                        matRx(0)(0) = rxRS1(rowInd1)(colInd1)*(double)coverCode(0)(0);
                                        matRx(1)(0) = rxRS1(rowInd2)(colInd1)*(double)coverCode(1)(0);
                                        
                                        matTx(0)(0) = txRS1(rowInd1)(colInd1)*(double)coverCode(0)(0);
                                        matTx(1)(0) = txRS1(rowInd2)(colInd1)*(double)coverCode(1)(0);
                                        
                                        txRS3(0)(0) = matRx(0)(0) / matTx(0)(0);
                                        txRS3(1)(0) = matRx(1)(0) / matTx(1)(0);
                                        
                                        rxRS2(sc+symCnt*nPortSCs) = ((txRS3(0)(0)+txRS3(1)(0)) * matTx(mod(sc,2))(0))/(double)(coverSizeFreq * coverSizeTime);
                                        txRS2(sc+symCnt*nPortSCs) = matTx(mod(sc,2))(0);
                                        
                                    }
                                    
                                }
                            }
                            //cout<<"\n port filles=\t"<<portNumbers(iport1)-portFirst;
                            
                            estimateBy2DMMSE(estimatedChannelInRB((portNumbers(iport1)-portFirst))(rx_ant) ,txRS2, rxRS2, rbRSPattern, portLocations, inv_dB(myAttenuationFactorIndB), abs(estimatedIntrCovInRB(rx_ant,rx_ant)), dopplerSpreadInHz,  nTaps,tapDel,tapGain);
                    }
            }  
        }
        
    }
}

/*! \fn errModelMyChannelinRB(...)
 * \brief Model the error and add it to the ideal channel across all tones in a given RB.
 * \param [out] estimatedChannelInRB ---> it contains the error added channel in an RB and it dimension depends on number of ports and number of receiver antennas 
 * \param [in] idealChannelInRB ---> Contains the ideal channel coefficients
 * \param [in] receivedRB --> Received RB.
 * \param [in] myAttenuationFactorIndB ---> it is the link gain between server node to service node
 * \param [in] myScRxPowerInWatts ---> received desired subcarrier power
 * \param [in] interfererRxSCPowerInDB ---> received interference subcarrier power
 * \param [in,out] txRSPerPort --> Transmitted RS sequence for given ports in given RB, with freq first and time next.
 * \param [in] noiseVariance ---> noise variance
 * \param [in] restOfintereferencePowerInWatts ---> weak interference signal power (otherthan 8 strongest interference signals)
 * \param [in] rsPowerBoostInDB --> To boost RS power for better channel estimation
 * \param [in] dopplerSpreadInHz --> Used by 2D-MMSE to calculate frequency correlation
 * \param [in] fadingParameters --> Used by 2D-MMSE to calculate frequency correlation
 * \param [in] portReferenceNumbers --> Index of ports for which channel is to estimated
 * \param [in,out] rbRSPattern --> Matrix of RB dimensions with data indicated as "0" and ports indicated as "1, 2, 3, 4"
 * \param [in] portIndices ---> port numbers for the given transmission mode
 * \param [in] rbNumber ---> resource block number 
 * \return returns the estimated channel for an RB
 * \note Make sure that 6 interlaces are taken care in the rbRSPattern ///SK: check
 */

 Array< cmat>  ChannelEstimator::errModelMyChannelInRB(Array<cmat>  &idealChannelInRB, Array<cmat> &receivedRB, double myAttenuationFactorIndB, double interfererRxSCPowerInDB, double noiseVariance, double restOfintereferencePowerInWatts, double dopplerSpreadInHz, int nTaps, vec tapDel, vec tapGain, ivec portReferenceNumbers, imat &rbRSPattern, ivec portIndices)
{
  
  if(!isReady())
  {
    cout<<"[both:] ChannelEstimator not fully configured..."<<endl;
    abort();
  }
  
  Array<cmat> channelWithErr;
  mat errVar;
  //cout<<"\n inside err model";
  int rxAntenna = receivedRB.length();
  
  ivec repPorts=findUniqueNumbers(portReferenceNumbers);//portReferenceNumbers=repPorts=[6];for mode-8=portReferenceNumbers=[8 8]repPorts=[8]
  
  channelWithErr.set_size(idealChannelInRB.length());
  for(int ip=0; ip<idealChannelInRB.length(); ip++)
    channelWithErr(ip).set_size(rxAntenna,portReferenceNumbers.length());
  
  int portID = 0;
 /* double chanPower = 0,totLen = 0;
  
  for (int rowNum = 0;rowNum< idealChannelInRB.length();rowNum++)
      for (int colNum = 0;colNum < idealChannelInRB(0).rows();colNum++)
	for (int symNum = 0;symNum < idealChannelInRB(0).cols();symNum++){
	  chanPower = chanPower + pow(abs(idealChannelInRB(rowNum)(colNum,symNum)),2);
	  totLen = totLen + 1;
	}
	
	chanPower = chanPower/totLen;
 */   
 for(int iport=0; iport<repPorts.length(); iport++)
  {
     double chanPower = 0,totLen = 0;
  
     for (int rowNum = 0;rowNum< idealChannelInRB.length();rowNum++)
 	chanPower += real(idealChannelInRB(rowNum).get_col(iport).H()*idealChannelInRB(rowNum).get_col(iport))(0)/idealChannelInRB(rowNum).rows();
	
    chanPower = chanPower/idealChannelInRB.length();
 
//     cout<<"\n channel power="<<chanPower;
    
    ivec portLocations=find(rbRSPattern, repPorts(iport), true);
  //  errVar = chanErrModel(rbRSPattern, portLocations, noiseVariance, inv_dB(myAttenuationFactorIndB), inv_dB(interfererRxSCPowerInDB), restOfintereferencePowerInWatts, dopplerSpreadInHz, fadingParameters);
//     errVar = chanErrModel(rbRSPattern, portLocations, noiseVariance, inv_dB(myAttenuationFactorIndB), inv_dB(interfererRxSCPowerInDB), restOfintereferencePowerInWatts, dopplerSpreadInHz, fadingParameters);  
       errVar = chanErrModel(rbRSPattern, portLocations, noiseVariance, chanPower, inv_dB(interfererRxSCPowerInDB), restOfintereferencePowerInWatts, dopplerSpreadInHz,  nTaps,tapDel,tapGain);  
        
    int nReps=find(portReferenceNumbers, repPorts(iport), true).length();
    
    for (int iport1 = 0; iport1 < nReps; iport1++){
      for(int rxAntInd = 0;rxAntInd < rxAntenna ;rxAntInd++)
	for(int reCnt = 0; reCnt < idealChannelInRB.length(); reCnt++){
	  //double errVariance = errVar(reCnt) * chanPower;
	  double errVariance = errVar(reCnt);
// cout<<"\n channel err var="<<errVariance;
	  complex< double > EstError = randn_c() * complex< double >((double)sqrt(errVariance),0.0);
	  channelWithErr(reCnt)(rxAntInd,portID) = idealChannelInRB(reCnt)(rxAntInd,portID) + EstError;
	  
	}
	portID++;
    }
  }
  return(channelWithErr);
}


/*! \fn errModelWideBandChannel(...)
 * \brief Error Modeling for wide band channel
 * \param [out] channelErr ---> Contains channel matrix with modeled error. It's dimensions depend on number of ports and number of receive antennas
 * \param [in] idealChannel ---> Matrix of ideal channel values
 * \param [in] subFramePattern --> Matrix of Subframe dimensions with data indicated as "0" and ports indicated as "1, 2, 3, 4"
 * \param [in] interferersRxPowerInWatts ---> interference power in the received interference symbols
 * \param [in] restOfintereferencePowerInWatts ---> weak interference power
 * \param [in] myAttenuationFactorIndB --> To account for LArge scale fading seen by RS
 * \param [in] myScRxPowerInWatts ---> per subcarrier received power in watts
 * \param [in] rsPowerBoostInDB --> To boost RS power for better channel estimation
 * \param [in] noiseVariance ---> noise variance
 * \param [in] dopplerSpreadInHz --> Used by 1D-MMSE to calculate frequency correlation
 * \param [in] CQIportReferenceNumbers ---> The CQI port reference symbol indication number
 * \param [in,out] fadingParameters --> Used by 1D-MMSE to calculate frequency correlation
 * \param [in] portIndices ---> port numbers
 * \return Channel (with Error) of dimension (#Ports, #RxAntenna, #SubcarriersInSubframe, #SymbolsInSubframe)
 * \note Make sure that 6 interlaces are taken care in the rbRSPattern ///SK: check
 * \warning This does not work for Subcarrier distributed mode. We assume Localized and Block-Distributed Channel.
 */

Array< Array<cmat> > ChannelEstimator::errModelWideBandChannel(Array< Array<cmat> > &idealChannel,imat &subFramePattern,  double interferersRxPowerInWatts,double restOfintereferencePowerInWatts, double myAttenuationFactorIndB,double noiseVariance, double dopplerSpreadInHz, ivec CQIportReferenceNumbers ,int nTaps, vec tapDel, vec tapGain, ivec portIndices)
{
  
  if(!isReady())
  {
    cout<<"[both:] ChannelEstimator not fully configured..."<<endl;
    abort();
  }
  
  Array< Array<cmat> > channelWithErr;
  mat errVar;
  
  //cout<<"\n inside wide band err model";
  int rxAntenna = idealChannel(0).length();
  ivec repPorts = findUniqueNumbers(CQIportReferenceNumbers);
  
  imat tempPartSubframe=subFramePattern;
  
  channelWithErr.set_size(CQIportReferenceNumbers.length());
  for(int ip=0; ip<CQIportReferenceNumbers.length(); ip++)
  {
    channelWithErr(ip).set_size(rxAntenna);
    for (int ind1 = 0;ind1 < rxAntenna; ind1++)
      channelWithErr(ip)(ind1).set_size(subFramePattern.rows(),subFramePattern.cols());
    
  }
  
  double chanPower = 0,totLen = 0;
   totLen;
  
  for (int rowNum = 0;rowNum< idealChannel.length();rowNum++)
    for (int colNum = 0;colNum < idealChannel(0).length();colNum++)
      for (int scNum = 0;scNum < idealChannel(0)(0).rows();scNum++)
	for (int symNum = 0;symNum < idealChannel(0)(0).cols();symNum++){
	  chanPower = chanPower + pow(abs(idealChannel(rowNum)(colNum)(scNum,symNum)),2);
	  totLen = totLen + 1;
	}
         
         chanPower = chanPower/totLen;
	 
	 
  int portID = 0;
  for (int portgrpind = 0; portgrpind < repPorts.length(); portgrpind++)
  {
    ivec portLoc=find(tempPartSubframe, repPorts(portgrpind), true);
   // errVar = chanErrModel(tempPartSubframe, portLoc, noiseVariance, inv_dB(myAttenuationFactorIndB), interferersRxPowerInWatts, restOfintereferencePowerInWatts, dopplerSpreadInHz, fadingParameters);
    errVar = chanErrModel(tempPartSubframe, portLoc, noiseVariance,   chanPower, interferersRxPowerInWatts, restOfintereferencePowerInWatts, dopplerSpreadInHz,  nTaps,tapDel,tapGain); 
   
    
    ivec portNumbers=portIndices(find(CQIportReferenceNumbers, repPorts(portgrpind), true));
    
    for (int portind = 0; portind< portNumbers.length(); portind++,portID++)
      for(int rxAntInd = 0; rxAntInd < rxAntenna; rxAntInd++)	
	for(int scCnt = 0; scCnt < errVar.rows(); scCnt++)
	  for(int symNum = 0;symNum < errVar.cols();symNum++)
	  {
	   // double errVariance = errVar(scCnt,symNum) * (chanPower);
	   double errVariance = errVar(scCnt,symNum);
	    complex< double > EstError = randn_c() * complex< double >((double)sqrt(errVariance),0.0);
	    channelWithErr(portID)(rxAntInd)(scCnt,symNum) = idealChannel(portID)(rxAntInd)(scCnt,symNum) + EstError;
	    
	  }
  }
  return(channelWithErr);
}


/*! \fn chanErrModel(...)
 * \brief Calculate the mean square error for all REs in the required grid; Generate a random error matrix: zero mean unit 
 *  variance Gaussian Random variable.  Multiple the random error matrix to the sqrt(MSE). Add error matrix to ideal channel.
 * \param [out] errVar ---> Vector of error variance values for different RE locations in time-frequency grid
 * \param [in,out] rbRSPattern --> Matrix of RB dimensions with data indicated as "0" and ports indicated as "1, 2, 3, 4"
 * \param [in] portLocations --> Indicates tone locations used by CRS in a given RB for a given port
 * \param [in] rsPowerBoostInDB --> To boost RS power for better channel estimation\
 * \param [in] noiseVariance ---> noise variance
 * \param [in] myAttenuationFactorInWatts --> To account for Large scale fading seen by RS
 * \param [in] interferersPowerInWatts --> Power of strong interferers to CRS
 * \param [in] restOfintereferencePowerInWatts ---> weak interference power
 * \param [in] dopplerSpreadInHz --> Used by 2D-MMSE to calculate frequency correlation
 * \param [in] fadingParameters --> Used by 2D-MMSE to calculate frequency correlation
 * \return Raw Channel of dimension for a given (Port, RxAntenna) pair across all tones in a given RB
 */

mat ChannelEstimator::chanErrModel(imat &rbRSPattern, ivec portLocations, double noiseVariance, double myAttenuationFactorInWatts, double interferersPowerInWatts, double restOfintereferencePowerInWatts, double dopplerSpread, int nTaps, vec tapDel, vec tapGain)
{

  if(!isReady())
  {
    cout<<"[both:] ChannelEstimator not fully configured..."<<endl;
    abort();
  }
  
  ivec autocorrpattern,crosscorrpattern;
  int nRStones=portLocations.length();
  cmat X(nRStones,nRStones),Y(nRStones,1),estimatedIntrCovInRB(nRStones,nRStones),CORR_Hpi_Hpi,CORR_H_Hpi,Hest;
  cmat noise =(2*noiseVariance+restOfintereferencePowerInWatts)*eye_c(nRStones);
  int NoInterFer = 1;// interferersPowerInWatts.length();
  estimatedIntrCovInRB.clear();
  ivec rsloc(portLocations.length()*2);
  
  
  for(int rs_cnt=0;rs_cnt<portLocations.length();rs_cnt++)
  {
    rsloc(2*rs_cnt)=portLocations(rs_cnt)/rbRSPattern.rows();
    rsloc(2*rs_cnt+1)=portLocations(rs_cnt)%rbRSPattern.rows();
  }
  
  autocorrpattern  = generateautocorrpattern(rsloc,nRStones);
  crosscorrpattern = generatecrosscorrpattern(rsloc,rbRSPattern.rows(),rbRSPattern.cols(),nRStones);
  
  // CORR_Hpi_Hpi = myAttenuationFactorInWatts*(findchannelcorr(autocorrpattern,nRStones,nRStones,dopplerSpread,fadingParameters,samplingPeriod,frequencySpacing)+0.01*eye_c(nRStones));
  // CORR_H_Hpi =   myAttenuationFactorInWatts*findchannelcorr(crosscorrpattern,rbRSPattern.size(),nRStones,dopplerSpread,fadingParameters,samplingPeriod,frequencySpacing);
  
   CORR_Hpi_Hpi = myAttenuationFactorInWatts*(findchannelcorr(autocorrpattern,nRStones,nRStones,dopplerSpread, nTaps,tapDel,tapGain,samplingPeriod,frequencySpacing)+0.0001*eye_c(nRStones));
   CORR_H_Hpi =  (myAttenuationFactorInWatts)*findchannelcorr(crosscorrpattern,rbRSPattern.size(),nRStones,dopplerSpread, nTaps,tapDel,tapGain,samplingPeriod,frequencySpacing);
 /*
  if(NoInterFer)
  {
    for(int intr =0;intr <NoInterFer;intr++)
      estimatedIntrCovInRB += interferersPowerInWatts[intr]*eye_c(nRStones);
  }
  */
 
 estimatedIntrCovInRB += interferersPowerInWatts*eye_c(nRStones);
 
  int len1 = CORR_H_Hpi.rows();
  int len2 = CORR_Hpi_Hpi.cols();
  cmat wienerFilt(len1,len2);
  mat errVar(rbRSPattern.rows(),rbRSPattern.cols());
  
 
  cmat temp = CORR_Hpi_Hpi + estimatedIntrCovInRB + noise;
  wienerFilt = CORR_H_Hpi * inv(temp);
  cmat CORR_H_Herm  = CORR_H_Hpi.hermitian_transpose();
  
//   cout<<"\n Wiener filter"<< rbRSPattern<<endl;
 double sumpwr = 0;
  for(int ind1 = 0; ind1 < rbRSPattern.rows();ind1++)
    for(int ind2 = 0;ind2 < rbRSPattern.cols();ind2++){
      sumpwr +=  abs(sum(elem_mult(wienerFilt.get_row(ind1*rbRSPattern.cols()+ind2), CORR_H_Herm.get_col(ind1*rbRSPattern.cols()+ind2))));
     errVar(ind1,ind2) = myAttenuationFactorInWatts - abs(sum(elem_mult(wienerFilt.get_row(ind1*rbRSPattern.cols()+ind2), CORR_H_Herm.get_col(ind1*rbRSPattern.cols()+ind2))));
     //errVar(ind1,ind2) = 1.0 - abs(sum(elem_mult(wienerFilt.get_row(ind1*rbRSPattern.cols()+ind2), CORR_H_Herm.get_col(ind1*rbRSPattern.cols()+ind2))));
      
    }
//     cout<<"\n sumpower = "<< sumpwr<<'\t'<<errVar;
    return(errVar);
}

/*! \fn estimateBy2DMMSE(...)
* \brief estimates the raw channel for a given (Port, RxAntenna) pair across all tones in a given RB using CRS.
* \param [in,out] estimatedChannel ---> it contains the estimated channel, in case of DMRS it contains the channel per RB where as in case of CSIRS/CRS channel of a band  
* \param [in,out] txrs ---> transmitted reference sequence
* \param [in] rxRS --> Received RS sequence for a given port in RB.
* \param [in,out] rbRSPattern --> Matrix of RB dimensions with data indicated as "0" and ports indicated as "1, 2, 3, 4"
* \param [in] portLocations --> Indicates tone locations used by CRS in a given RB for a given port
* \param [in] rsPowerBoostInDB --> To boost RS power for better channel estimation\
* \param [in] noiseVariance ---> noise variance
* \param [in] myAttenuationFactorInWatts --> To account for Large scale fading seen by RS
* \param [in] interferersPowerInWatts --> Power of strong interferers to CRS
* \param [in] restOfintereferencePowerInWatts ---> weak interference power
* \param [in] dopplerSpreadInHz --> Used by 2D-MMSE to calculate frequency correlation
* \param [in] fadingParameters --> Used by 2D-MMSE to calculate frequency correlation
* \return Raw Channel of dimension for a given (Port, RxAntenna) pair across all tones in a given RB
*/

void ChannelEstimator::estimateBy2DMMSE(cmat &estimatedChannel, cvec &txRS, cvec rxRS, imat &rbRSPattern, ivec portLocations, double myAttenuationFactorInWatts, double interferencePowerInWatts, double dopplerSpread, int nTaps, vec tapDel, vec tapGain){

  if(!isReady())
  {
    cout<<"[both:] ChannelEstimator not fully configured..."<<endl;
    abort();
  }
  
  ivec autocorrpattern,crosscorrpattern;
  int nRStones=portLocations.length();
  cmat X(nRStones,nRStones),Y(nRStones,1),CORR_Hpi_Hpi,CORR_H_Hpi,Hest;

  ivec rsloc(portLocations.length()*2);
  for(int rs_cnt=0;rs_cnt<portLocations.length();rs_cnt++)
  {
    rsloc(2*rs_cnt)=portLocations(rs_cnt)/rbRSPattern.rows();
    rsloc(2*rs_cnt+1)=portLocations(rs_cnt)%rbRSPattern.rows();
  }
 
  autocorrpattern  = generateautocorrpattern(rsloc,nRStones);
  crosscorrpattern = generatecrosscorrpattern(rsloc,rbRSPattern.rows(),rbRSPattern.cols(),nRStones);

  CORR_Hpi_Hpi = myAttenuationFactorInWatts*(findchannelcorr(autocorrpattern,nRStones,nRStones,dopplerSpread, nTaps,tapDel,tapGain,samplingPeriod,frequencySpacing)+0.01*eye_c(nRStones));
  CORR_H_Hpi =   myAttenuationFactorInWatts*findchannelcorr(crosscorrpattern,rbRSPattern.size(),nRStones,dopplerSpread, nTaps,tapDel,tapGain,samplingPeriod,frequencySpacing);
  
  X=diag(txRS);Y=rxRS;
  cmat estimatedIntrCovInRB = eye_c(nRStones)*dComplex(interferencePowerInWatts,0.0);
  cmat temp=(1/myAttenuationFactorInWatts)*(X*CORR_Hpi_Hpi*X.hermitian_transpose() + estimatedIntrCovInRB);
  Hest = (CORR_H_Hpi*X.hermitian_transpose())*((1/myAttenuationFactorInWatts)*inv(temp))*Y;
   
  
  for(int tone_cnt=0;tone_cnt <rbRSPattern.size();tone_cnt++)
    estimatedChannel(tone_cnt)=Hest(tone_cnt) ;
}

void ChannelEstimator::estimateBy2DMMSE(cmat &estimatedChannel, cvec &txRS, cvec rxRS, imat &rbRSPattern, ivec portLocations, double noiseVariance, double myAttenuationFactorInWatts, vec interferersPowerInWatts, double restOfintereferencePowerInWatts, double dopplerSpread, int nTaps, vec tapDel, vec tapGain){

  if(!isReady())
  {
    cout<<"[both:] ChannelEstimator not fully configured..."<<endl;
    abort();
  }
  
  ivec autocorrpattern,crosscorrpattern;
  int nRStones=portLocations.length();
  cmat X(nRStones,nRStones),Y(nRStones,1),estimatedIntrCovInRB(nRStones,nRStones),CORR_Hpi_Hpi,CORR_H_Hpi,Hest;
  cmat noise =(2*noiseVariance+restOfintereferencePowerInWatts)*eye_c(nRStones);
  int NoInterFer = interferersPowerInWatts.length();
  estimatedIntrCovInRB.clear();
  ivec rsloc(portLocations.length()*2);
  for(int rs_cnt=0;rs_cnt<portLocations.length();rs_cnt++)
  {
    rsloc(2*rs_cnt)=portLocations(rs_cnt)/rbRSPattern.rows();
    rsloc(2*rs_cnt+1)=portLocations(rs_cnt)%rbRSPattern.rows();
  }
 
  autocorrpattern  = generateautocorrpattern(rsloc,nRStones);
  crosscorrpattern = generatecrosscorrpattern(rsloc,rbRSPattern.rows(),rbRSPattern.cols(),nRStones);


  CORR_Hpi_Hpi = myAttenuationFactorInWatts*(findchannelcorr(autocorrpattern,nRStones,nRStones,dopplerSpread, nTaps,tapDel,tapGain,samplingPeriod,frequencySpacing)+0.01*eye_c(nRStones));
  CORR_H_Hpi =   myAttenuationFactorInWatts*findchannelcorr(crosscorrpattern,rbRSPattern.size(),nRStones,dopplerSpread, nTaps,tapDel,tapGain,samplingPeriod,frequencySpacing);
  
  X=diag(txRS);Y=rxRS;
  if(NoInterFer)
  {
    for(int intr =0;intr <NoInterFer;intr++)
      estimatedIntrCovInRB += interferersPowerInWatts[intr]*eye_c(nRStones);
  }

  cmat temp=(1/myAttenuationFactorInWatts)*(X*CORR_Hpi_Hpi*X.hermitian_transpose() + estimatedIntrCovInRB + noise);
  Hest = (CORR_H_Hpi*X.hermitian_transpose())*((1/myAttenuationFactorInWatts)*inv(temp))*Y;

  for(int tone_cnt=0;tone_cnt <rbRSPattern.size();tone_cnt++)
    estimatedChannel(tone_cnt)=Hest(tone_cnt) ;
}

/*! \fn generateautocorrpattern(...)
* \param [in] rsloc ---> gives the DMRS reference signal positions in an RB in both time and freq 
* \param [in] nRStones ---> number of port locations 
* \return pattern ---> returns time and frequency offsets within RS tone locations 
* \brief Calculates the time and frequency offsets within RS tone locations
*/
ivec ChannelEstimator::generateautocorrpattern(ivec rsloc,int nRStones)
{
  ivec pattern;
  pattern.set_length(nRStones*nRStones*2);
  for(int i=0;i<nRStones;i++)
  {
    for(int j=0;j<nRStones;j++)
    {
      pattern(2*(i*nRStones+j)+1) = rsloc[2*i] -
      rsloc[2*j];

      pattern(2*(i*nRStones+j)) = rsloc[2*i+1] -
      rsloc[2*j+1];

    }
  }
  return pattern;
}

/*! \fn generatecrosscorrpattern(...)
 * * \brief Calculates the time and frequency offsets of RS tone locations with the channel locations need to be estimated.
* \param [in] rsloc ---> gives the DMRS reference signal positions in an RB in both time and freq 
* \param [in] channelheight ---> number of subcarriers per RB
* \param [in] channelwidth ---> number of OFDM symbols per RB
* \param [in] nRStones ---> number of port locations 
* \return ---> returns time and frequency offsets of RS tone locations with the channel locations need to be estimated
*/
ivec ChannelEstimator::generatecrosscorrpattern(ivec rsloc,int channelheight,int channelwidth,int nRStones)
{
  ivec pattern;
  pattern.set_length(channelheight*channelwidth*nRStones*2);
  for(int i=0;i<channelheight*channelwidth;i++)
  {
    for(int j=0;j<nRStones;j++)
    {
      pattern(2*(i*nRStones+j)+1) = (int)(i/channelheight) -
      rsloc[2*j];
      pattern(2*(i*nRStones+j)) = mod(i,channelheight) -
      rsloc[2*j+1];
    }
  }
  return pattern;
}

/*! \fn findchannelcorr(...)
* \param [in] correlationPattern ---> contains the time and frequency offsets of each pilot with eighter other pilot symbol locations or all tone locations in an RB
* \param [in] rows ---> number of subcarriers in an RB
* \param [in] cols ---> number of OFDM symbols in an RB
* \param [in] doppler ---> doppler shift
* \param [in] fadingParameters ---> gives number of taps, tap delay and gain of each tap
* \param [in] samplingperiod ---> sampling time period 
* \param [in] freqspacing ---> subcarrier bandwidth
* \return corrmatx ---> correlation matrix
* \brief Calculates the Correlation Matrix (time and frequency) for given offsets and (rows, cols).
*/

cmat ChannelEstimator::findchannelcorr(ivec correlationPattern,int rows,int cols,double doppler, int nTaps, vec tapDel, vec tapGain,double samplingperiod,double freqspacing)
{
  cmat corrmatx(rows,cols);
  corrmatx.clear();
  for(int i=0;i<rows;i++)
    for(int j=0;j<cols;j++)
    {
      corrmatx(i,j) =
      correlationmatx(correlationPattern[2*(i*cols+j)],correlationPattern[2*(i*cols+j)+1],doppler, nTaps,tapDel,tapGain,samplingperiod,freqspacing);
    }
    return(corrmatx);

}

/*! \fn correlationmatx(...)
* \param [in] freqdiff ---> difference in the number of subcarriers from desired subcarrier to pilot subcarrier 
* \param [in] timediff ---> difference in the number of symbols from desired symbol to pilot symbol
* \param [in] doppler ---> doppler shift
* \param [in] fadingParameters ---> gives number of taps, tap delay and gain of each tap
* \param [in] samplingperiod ---> sampling time period 
* \param [in] freqspacing ---> subcarrier bandwidth
* \return timecorr*freqcorr(0,0) ---> multiplication of time and frequency correlation functions
* \brief Calculates the Correlation between two tones (time and frequency) for given time and freq offsets.
*/
complex<double> ChannelEstimator::correlationmatx(double freqdiff,double timediff,double doppler, int nTaps, vec tapDel, vec tapGain,double samplingperiod,double freqspacing)
{
  int IDEAL =1;///SK: check this 
  double timecorr; 
  double estimationErr = (0/100.0) + 1.0;
  double dopplerWithEstError = doppler * estimationErr;
  timecorr = sinc(2.*dopplerWithEstError*samplingperiod*timediff);
  double int_del; 
  complex< double > freqcorr=0.0;
  complex< double > exp;
  if(IDEAL)
  {
    for(int tap =0; tap <nTaps;tap++)
    {
      int_del=(tapDel[tap]*samplingFrequency);
      exp = complex< double>(cos(2*pi*(freqdiff/fftSize)*int_del),sin(-2*pi*(freqdiff/fftSize)*int_del));
      freqcorr += sqr(tapGain[tap])*exp;
    }
  }
  else
  {
    double delaySprdWithEstError = 5.2e-6;
    exp = complex< double>(cos(pi*delaySprdWithEstError*freqspacing*freqdiff),sin(-pi*delaySprdWithEstError*freqspacing*freqdiff));
    freqcorr =  sinc(delaySprdWithEstError*freqspacing*freqdiff); 
  }
//      cout<<"\n corr="<<timecorr<<'\t'<<freqcorr;
  return(timecorr*freqcorr);  
}

/*! \fn estimateInterferenceCovarianceInRB(...)
* \brief estimates the raw channel across all tones in a given RB using CRS.
* \param [in,out] estimatedIntrCovInRB ---> estimated interference covariance matrix (G)
* \param [in,out] receivedRB --> Received RB.
* \param [in,out] txRSPerPort ---> transmitted reference signal per port
* \param [in,out] myChannelForRB --> Estimated channel in an RB for all ports and all rx antennas
* \param [in,out] portReferenceNumbers --> Index of ports for which channel is to estimated.
* \param [in] myTransmissionScheme --> Transmission Scheme
* \param [in] rbRSPattern --> Matrix of RB dimensions with data indicated as "0" and ports indicated as "1, 2, 3, 4"
* \return Average IntrCov of dimension (#RxAntenna, #RxAntenna)
* \note Make sure that 6 interlaces are taken care in the rbRSPattern ///SK: check
*/


// void ChannelEstimator::estimateInterferenceCovarianceInRB(cmat &estimatedIntrCovInRB, Array<cmat> &receivedRB, Array< Array<cmat> > &myChannelForRB, Array<cvec> &txRSPerPort , TransmissionScheme_E myTransmissionScheme, imat &rbRSPattern, ivec ports, ivec portReferenceNumbers, ivec otherPorts, ivec otherPortRefNumbers)
// {
//     
//     if(myTransmissionScheme!=_TRANSMISSION_SCHEME_NR_CL_)
//     {
//         cout<<"IntrCov with ZP ports supported only in _TRANSMISSION_SCHEME_NR_CL_.. "<<endl;abort();
//     }
//     
//     int rxAntenna=receivedRB.length();
//     bvec ZPPortFlag=zeros_b(ports.length());
//     if(otherPorts.length())
//     {
//         ivec otherPortIndicesToRemove=remove(find(otherPortRefNumbers,portReferenceNumbers),-1);
//         deleteInVec(otherPorts,otherPortIndicesToRemove);
//         deleteInVec(otherPortRefNumbers,otherPortIndicesToRemove);
//         append(ZPPortFlag,ones_b(otherPorts.length()));
//         append(ports,otherPorts);
//         append(portReferenceNumbers,otherPortRefNumbers);
//     }
//     
//     int nPorts=portReferenceNumbers.length();
//     ivec uniquePortReferenceNumbers=findUniqueNumbers(portReferenceNumbers);
//     int nCDMs=uniquePortReferenceNumbers.length();
//     bvec ZPCDMFlag=zeros_b(nCDMs);
//     Array<ivec> portsToHandle(nCDMs);
//     for(int cnt=0;cnt<nCDMs;cnt++)
//     {
//         ivec portIndices=find(portReferenceNumbers,uniquePortReferenceNumbers(cnt),true);
//         portsToHandle(cnt)=ports(portIndices);
//         ZPCDMFlag(cnt)=ZPPortFlag(portIndices(0)); // all ports will have same flag
//     }
//     
//     bool useotherport=false;
//     if(find(ZPCDMFlag).length())
//         useotherport=true;
//     
//     cvec myRxPilots, myRxChannel;
//     if(myTransmissionScheme == _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
//     {
//         estimatedIntrCovInRB = zeros_c(2*rxAntenna, 2*rxAntenna);
//     }
//     else
//     {
//         estimatedIntrCovInRB = zeros_c(rxAntenna,rxAntenna);
//     }
//     
//     Array< Array<cvec> > Intrference(nCDMs);
//     for(int cdm_cnt=0;cdm_cnt<uniquePortReferenceNumbers.length();cdm_cnt++)
//         Intrference(cdm_cnt).set_size(rxAntenna);
//     
//     for(int cdm_cnt=0;cdm_cnt<uniquePortReferenceNumbers.length();cdm_cnt++)
//     {
//         ivec rsloc=find(rbRSPattern,uniquePortReferenceNumbers(cdm_cnt),true);
//         
//         if(portsToHandle(cdm_cnt).length()==1 && portsToHandle(cdm_cnt)(0)<4) //CRS
//         {
//             for(int rx_cnt=0;rx_cnt<rxAntenna;rx_cnt++)
//                 Intrference(0)(rx_cnt).set_length(rsloc.length());
//             
//             for(int re_cnt=0;re_cnt<rsloc.length();re_cnt++)
//             {
//                 cmat Y(rxAntenna,1); Y.set_col(0,getAcrossArray(receivedRB,rsloc(re_cnt)));
//                 cmat H=getAcrossArray(myChannelForRB,rsloc(re_cnt));
//                 cmat P=zeros_c(H.cols(),1);
//                 P(portsToHandle(cdm_cnt)(0),0)=txRSPerPort(portsToHandle(cdm_cnt)(0))(re_cnt);
//                 cmat interference=Y-H*P;
//                 
//                 setAcrossArray(Intrference(0),re_cnt-0,interference.get_col(0));
//             }
//             //}
//         }
//         else
//         {
//             
//             if(useotherport && ZPCDMFlag(cdm_cnt)==0)
//                 continue;
//             
//             if(!useotherport && ZPCDMFlag(cdm_cnt)==1)
//                 continue;
//             
//             
//             for(int rx_cnt=0;rx_cnt<rxAntenna;rx_cnt++)
//                 Intrference(cdm_cnt)(rx_cnt).set_length(rsloc.length());
//             
//             for(int re_cnt=0;re_cnt<rsloc.length();re_cnt++)
//             {
//                 cmat Y(rxAntenna,1); Y.set_col(0,getAcrossArray(receivedRB,rsloc(re_cnt)));
//                 cmat interference=Y;
//                 if(ZPCDMFlag(cdm_cnt)==0)
//                 {
//                     cmat H=getAcrossArray(myChannelForRB,rsloc(re_cnt));
//                     cmat P=zeros_c(H.cols(),1);
//                     for(int port_cnt=0;port_cnt<portsToHandle(cdm_cnt).length();port_cnt++)
//                     {
//                         int portIndx=find(ports,portsToHandle(cdm_cnt)(port_cnt));
//                         P(portIndx,0)=txRSPerPort(portIndx)(re_cnt);
//                     }
//                     interference=Y-H*P;
//                 }
//                 
//                 if(re_cnt==0)cout<<"interference : "<<interference<<endl;
//                 
//                 setAcrossArray(Intrference(cdm_cnt),re_cnt,interference.get_col(0));
//             }
//         }
//         cmat estimatedIntrCovPerLayer;
//         
//         if(myTransmissionScheme!=_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
//         {
//             
//             estimatedIntrCovPerLayer=zeros_c(rxAntenna,rxAntenna);
//             
//             for(int j=0;j<rsloc.length();j++){
//                 //cout<<"\nRStones"<<iport<<'\t'<<rsTonesPerPort(iport);
//                 estimatedIntrCovPerLayer +=  constructCovMatritx(Intrference(cdm_cnt,cdm_cnt),j,rxAntenna,1);
//                 //cout<<"\n estimated Cov matrix"<<estimatedIntrCovPerLayer<<endl;
//             }
//             estimatedIntrCovPerLayer= (1.0/(double)(rsloc.length()))*estimatedIntrCovPerLayer;
//         }
//         else
//         {
//             Array< Array<cvec> > intrSFBCChannel(2);
//             for(int l_cnt=0;l_cnt<2;l_cnt++)
//                 intrSFBCChannel(l_cnt).set_size(2*rxAntenna);
//             if(ports(cdm_cnt)==0 || ports(cdm_cnt)==1)
//                 for(int rxant=0;rxant<rxAntenna;rxant++)
//                 {
//                     intrSFBCChannel(0)(rxant*2) = Intrference(0)(rxant);		
//                     intrSFBCChannel(1)(rxant*2) = zeros_c(Intrference(0)(rxant).length());
//                     
//                     intrSFBCChannel(0)(rxant*2+1) = zeros_c(Intrference(0)(rxant).length());
//                     intrSFBCChannel(1)(rxant*2+1).set_length(Intrference(0)(rxant).length());
//                     for(int re_cnt=0;re_cnt<Intrference(0)(rxant).length();re_cnt++)
//                         intrSFBCChannel(1)(rxant*2+1)(re_cnt) = conj(Intrference(0)(rxant)(re_cnt));
//                 }
//                 
//                 else if(ports(cdm_cnt)==2 || ports(cdm_cnt)==3)
//                     for(int rxant=0;rxant<rxAntenna;rxant++)
//                     {
//                         intrSFBCChannel(0)(rxant*2) = zeros_c(Intrference(0)(rxant).length());		
//                         intrSFBCChannel(1)(rxant*2) = -Intrference(0)(rxant);		
//                         
//                         intrSFBCChannel(0)(rxant*2+1).set_length(Intrference(0)(rxant).length());
//                         for(int re_cnt=0;re_cnt<Intrference(0)(rxant).length();re_cnt++)
//                             intrSFBCChannel(0)(rxant*2+1)(re_cnt) = conj(Intrference(0)(rxant)(re_cnt));
//                         intrSFBCChannel(1)(rxant*2+1)=zeros_c(Intrference(0)(rxant).length());
//                     }
//                     estimatedIntrCovPerLayer=zeros_c(2*rxAntenna,2*rxAntenna);
//                     for(int j=0;j<rsloc.length();j++)
//                         estimatedIntrCovPerLayer+=  constructCovMatritx(intrSFBCChannel,j,2*rxAntenna,2);
//                     estimatedIntrCovPerLayer= (1.0/(double)(rsloc.length()))*estimatedIntrCovPerLayer;
//         }
//         
//         estimatedIntrCovInRB+= estimatedIntrCovPerLayer;
//     }
//     //     if(otherPorts.length())
//     //     {
//     //         cout<<"Adding additional interference"<<endl;
//     //         int rsCount=find(rbRSPattern,uniquePortReferenceNumbers(0),true).length(); // same density for all ports
//     //         cmat diferentialInterferenceCovariance=zeros_c(rxAntenna,rxAntenna);
//     //         
//     //         for(int j=0;j<rsCount;j++)
//     //         {
//     //             cmat allInterference=getAcrossArray(Intrference,j);
//     //             for(int cdm_cnt=0;cdm_cnt<(uniquePortReferenceNumbers.length()-1);cdm_cnt++)
//     //             {
//     //                 cmat differentialInterference=allInterference.get_cols(cdm_cnt,cdm_cnt)-allInterference.get_cols(cdm_cnt+1,cdm_cnt+1);
//     //                 diferentialInterferenceCovariance+=differentialInterference*differentialInterference.H();
//     //             }
//     //         }
//     //         diferentialInterferenceCovariance= (1.0/(double)(rsCount))*diferentialInterferenceCovariance;
//     //         estimatedIntrCovInRB+=diferentialInterferenceCovariance;
//     //     }
//     
//     //     estimatedIntrCovInRB = (1.0/(double)portsToHandle.length())*estimatedIntrCovInRB;
// }

double ChannelEstimator::estimateInterferenceCovarianceInRB(cmat &estimatedIntrCovInRB, Array<cmat> &receivedRB, Array< Array<cmat> > &myChannelForRB, Array<cvec> &txRSPerPort , TransmissionScheme_E myTransmissionScheme, imat &rbRSPattern, ivec ports, ivec portReferenceNumbers, ivec otherPorts, ivec otherPortRefNumbers)
{
    
    if(myTransmissionScheme!=_TRANSMISSION_SCHEME_NR_CL_)
    {
        cout<<"IntrCov with ZP ports supported only in _TRANSMISSION_SCHEME_NR_CL_.. "<<endl;abort();
    }
    bool isDMRS = ports(0)<2000 && ports(0)>=1000;
    int rxAntenna=receivedRB.length();
    bvec ZPPortFlag=zeros_b(ports.length());
    if(otherPorts.length())
    {
        if(!isDMRS)
        {
        ivec otherPortIndicesToRemove=remove(find(otherPortRefNumbers,portReferenceNumbers),-1);
        deleteInVec(otherPorts,otherPortIndicesToRemove);
        deleteInVec(otherPortRefNumbers,otherPortIndicesToRemove);
        }
        append(ZPPortFlag,ones_b(otherPorts.length()));
        append(ports,otherPorts);
        append(portReferenceNumbers,otherPortRefNumbers);
    }
    
    //int nPorts=portReferenceNumbers.length();
    ivec uniquePortReferenceNumbers=findUniqueNumbers(portReferenceNumbers);
    
    int nCDMs=uniquePortReferenceNumbers.length();
    bvec ZPCDMFlag=zeros_b(nCDMs);
    Array<ivec> portsToHandle(nCDMs);
    for(int cnt=0;cnt<nCDMs;cnt++)
    {
        ivec portIndices=find(portReferenceNumbers,uniquePortReferenceNumbers(cnt),true);
        portsToHandle(cnt)=ports(portIndices);
        //ZPCDMFlag(cnt)=ZPPortFlag(portIndices(0)); // all ports will have same flag
        ZPCDMFlag(cnt) = prod(to_ivec(ZPPortFlag(portIndices))); //if atleast one port in a CDM has data then it should be considered - Goraknath.
    }
    
    cvec myRxPilots, myRxChannel;
    if(myTransmissionScheme == _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
    {
        estimatedIntrCovInRB = zeros_c(2*rxAntenna, 2*rxAntenna);
    }
    else
    {
        estimatedIntrCovInRB = zeros_c(rxAntenna,rxAntenna);
    }
    double rsrp=0;
    Array< Array<cvec> > Intrference(nCDMs);
    for(int cdm_cnt=0;cdm_cnt<uniquePortReferenceNumbers.length();cdm_cnt++)
        Intrference(cdm_cnt).set_size(rxAntenna);
// cout<<"ZPCDMFlag : "<<ZPCDMFlag<<endl;
    for(int cdm_cnt=0;cdm_cnt<uniquePortReferenceNumbers.length();cdm_cnt++)
    {
        ivec rsloc=find(rbRSPattern,uniquePortReferenceNumbers(cdm_cnt),true);
        
        if(portsToHandle(cdm_cnt).length()==1 && portsToHandle(cdm_cnt)(0)<4) //CRS
        {
            for(int rx_cnt=0;rx_cnt<rxAntenna;rx_cnt++)
                Intrference(0)(rx_cnt).set_length(rsloc.length());
            
            for(int re_cnt=0;re_cnt<rsloc.length();re_cnt++)
            {
                cmat Y(rxAntenna,1); Y.set_col(0,getAcrossArray(receivedRB,rsloc(re_cnt)));
                cmat H=getAcrossArray(myChannelForRB,rsloc(re_cnt));
                cmat P=zeros_c(H.cols(),1);
                P(portsToHandle(cdm_cnt)(0),0)=txRSPerPort(portsToHandle(cdm_cnt)(0))(re_cnt);
                cmat interference=Y-H*P;
                
                setAcrossArray(Intrference(0),re_cnt-0,interference.get_col(0));
            }
        }
        else
        {
            for(int rx_cnt=0;rx_cnt<rxAntenna;rx_cnt++)
                Intrference(cdm_cnt)(rx_cnt).set_length(rsloc.length());
            
            for(int re_cnt=0;re_cnt<rsloc.length();re_cnt++)
            {
                cmat Y(rxAntenna,1); Y.set_col(0,getAcrossArray(receivedRB,rsloc(re_cnt)));
                cmat interference=Y;
                if(ZPCDMFlag(cdm_cnt)==0)
                {
                    cmat H=getAcrossArray(myChannelForRB,rsloc(re_cnt));
                    cmat P=zeros_c(H.cols(),1);
                    for(int port_cnt=0;port_cnt<portsToHandle(cdm_cnt).length();port_cnt++)
                    {
                        int portIndx=find(ports,portsToHandle(cdm_cnt)(port_cnt));
                        if(ZPPortFlag(portIndx)==0) // P is non zero only if that port has data. -Goraknath.
                        {
                            P(portIndx,0)=txRSPerPort(portIndx)(re_cnt);
                        }
                    }
                    interference=Y-H*P;
                    rsrp+=mean(sqr(abs((H*P).get_col(0))));
                }
                setAcrossArray(Intrference(cdm_cnt),re_cnt,interference.get_col(0));
            }
            rsrp/=(double)(rsloc.length());
        }
        cmat estimatedIntrCovPerLayer;
        
        if(myTransmissionScheme!=_TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
        {
            
            estimatedIntrCovPerLayer=zeros_c(rxAntenna,rxAntenna);
            
            for(int j=0;j<rsloc.length();j++){
                estimatedIntrCovPerLayer +=  constructCovMatritx(Intrference(cdm_cnt,cdm_cnt),j,rxAntenna,1);
                //cout<<"\n estimated Cov matrix"<<estimatedIntrCovPerLayer<<endl;
            }
            estimatedIntrCovPerLayer= (1.0/(double)(rsloc.length()))*estimatedIntrCovPerLayer;
        }
        else
        {
            Array< Array<cvec> > intrSFBCChannel(2);
            for(int l_cnt=0;l_cnt<2;l_cnt++)
                intrSFBCChannel(l_cnt).set_size(2*rxAntenna);
            if(ports(cdm_cnt)==0 || ports(cdm_cnt)==1)
                for(int rxant=0;rxant<rxAntenna;rxant++)
                {
                    intrSFBCChannel(0)(rxant*2) = Intrference(0)(rxant);		
                    intrSFBCChannel(1)(rxant*2) = zeros_c(Intrference(0)(rxant).length());
                    
                    intrSFBCChannel(0)(rxant*2+1) = zeros_c(Intrference(0)(rxant).length());
                    intrSFBCChannel(1)(rxant*2+1).set_length(Intrference(0)(rxant).length());
                    for(int re_cnt=0;re_cnt<Intrference(0)(rxant).length();re_cnt++)
                        intrSFBCChannel(1)(rxant*2+1)(re_cnt) = conj(Intrference(0)(rxant)(re_cnt));
                }
                
                else if(ports(cdm_cnt)==2 || ports(cdm_cnt)==3)
                    for(int rxant=0;rxant<rxAntenna;rxant++)
                    {
                        intrSFBCChannel(0)(rxant*2) = zeros_c(Intrference(0)(rxant).length());		
                        intrSFBCChannel(1)(rxant*2) = -Intrference(0)(rxant);		
                        
                        intrSFBCChannel(0)(rxant*2+1).set_length(Intrference(0)(rxant).length());
                        for(int re_cnt=0;re_cnt<Intrference(0)(rxant).length();re_cnt++)
                            intrSFBCChannel(0)(rxant*2+1)(re_cnt) = conj(Intrference(0)(rxant)(re_cnt));
                        intrSFBCChannel(1)(rxant*2+1)=zeros_c(Intrference(0)(rxant).length());
                    }
                    estimatedIntrCovPerLayer=zeros_c(2*rxAntenna,2*rxAntenna);
                    for(int j=0;j<rsloc.length();j++)
                        estimatedIntrCovPerLayer+=  constructCovMatritx(intrSFBCChannel,j,2*rxAntenna,2);
                    estimatedIntrCovPerLayer= (1.0/(double)(rsloc.length()))*estimatedIntrCovPerLayer;
        }
        
//         cout<<"estimatedIntrCovPerLayer : "<<cdm_cnt<<" , "<<estimatedIntrCovPerLayer<<endl;
       estimatedIntrCovInRB+= estimatedIntrCovPerLayer;
    }
//     if(otherPorts.length())
//     {
//         int rsCount=find(rbRSPattern,uniquePortReferenceNumbers(0),true).length(); // same density for all ports
//         cmat diferentialInterferenceCovariance=zeros_c(rxAntenna,rxAntenna);
//         
//         for(int j=0;j<rsCount;j++)
//         {
//             cmat allInterference=getAcrossArray(Intrference,j);
//             for(int cdm_cnt=0;cdm_cnt<(uniquePortReferenceNumbers.length()-1);cdm_cnt++)
//             {
//                 cmat differentialInterference=allInterference.get_cols(cdm_cnt,cdm_cnt)-allInterference.get_cols(cdm_cnt+1,cdm_cnt+1);
//                 diferentialInterferenceCovariance+=differentialInterference*differentialInterference.H();
//             }
//         }
//         diferentialInterferenceCovariance= (1.0/(double)(rsCount))*diferentialInterferenceCovariance;
//        
// //         cout<<"diferentialInterferenceCovariance : "<<diferentialInterferenceCovariance<<endl;
//         
//         estimatedIntrCovInRB+=diferentialInterferenceCovariance;
//     }
    
    estimatedIntrCovInRB = (1.0/(double)portsToHandle.length())*estimatedIntrCovInRB;
    rsrp = (1.0/(double)portsToHandle.length())*rsrp;
    return rsrp;
//     cout<<"estimatedIntrCovInRB : "<<estimatedIntrCovInRB<<endl;
    
//     if(otherPorts.length()) abort();
}



/*! \fn constructCovMatritx(...)
* \brief constructs the interference matrix in transmit diversity form and returns GG^H
* \param [in,out] Intrference ---> interference covariance matrix (G*G^H)
* \param [in] txnScheme ---> transmission scheme
* \param [in] rsloc ---> reference signal locations
* \param [in] rxAntenna ---> receiver antennas
* \param [in] nPorts ---> number of ports
* \return IntrCov ---> returns the constructed interference covaraiance matrix for transmit diversity
*/
cmat ChannelEstimator::constructCovMatritx(Array< Array< cvec > > Intrference, int rsloc, int rxAntenna, int nPorts)
{
    cmat IntrChan = zeros_c(rxAntenna,nPorts);
    cmat IntrCov;
    
     for(int iport=0;iport <nPorts;iport++)
      for(int rxant=0;rxant <rxAntenna;rxant++)
      {
	IntrChan(rxant,iport) = Intrference(iport)(rxant)(rsloc);
      }
	//calculating Interference covaraiance (G*G^H)
      IntrCov = IntrChan * IntrChan.hermitian_transpose();
    return(IntrCov);
}


/*!~~~~~~~~~~~~~~~~~~~~~ WIDEBAND CHANNEL ESTIMATOR ~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*! \fn estimateWideBandChannel(...)
* \brief estimates the raw channel across all tones in a given SubFrame using CRS.
* \param [in,out] txRSPerPortPerSymbol --> Transmitted RS sequence for all ports and all RS symbols in given SubFrame (nPorts, nRSsymbolsforgivenport, RSingivenportingivensymbolinSF).
* \param [in] guardSubCarriersOnOneSide ---> guard band subcarriers on each side(212 subcarriers for 10Mhz BW)
* \param [in,out] rxSubFrame --> Received SubFrame.
* \param [in,out] subFramePattern --> Matrix of Subframe dimensions with data indicated as "0" and ports indicated as "1, 2, 3, 4"
* \param [in,out] RSsymbolsPerPort --> Indicates symbol locations used by all ports (nPorts, nRSsymbolpositionsforgivenport).
* \param [in] interferersRxPowerInWatts ---> interference power in the received interference symbols
* \param [in] restOfintereferencePowerInWatts ---> weak interference power
* \param [in] myAttenuationFactorIndB --> To account for LArge scale fading seen by RS
* \param [in] myScRxPowerInWatts ---> per subcarrier received power in watts
* \param [in] rsPowerBoostInDB --> To boost RS power for better channel estimation
* \param [in] noiseVariance ---> noise variance
* \param [in] dopplerSpreadInHz --> Used by 1D-MMSE to calculate frequency correlation
* \param [in] CQIportReferenceNumbers ---> The CQI port reference symbol indication number
* \param [in,out] fadingParameters --> Used by 1D-MMSE to calculate frequency correlation
* \param [in] portIndices ---> port numbers
* \return Raw Channel of dimension (#Ports, #RxAntenna, #SubcarriersInSubframe, #SymbolsInSubframe)
* \note Make sure that 6 interlaces are taken care in the rbRSPattern ///SK: check
* \warning This does not work for Subcarrier distributed mode. Here we are estimating Localized and Block-Distributed Channel.
*/
Array< Array<cmat> > ChannelEstimator::estimateWideBandChannel(Array< Array<cvec> > &txRSPerPortPerSymbol,int guardSubCarriersOnOneSide, Array<cmat> &rxSubFrame, imat &subFramePattern, Array<ivec> &RSsymbolsPerPort, vec interferersRxPowerInWatts,double restOfintereferencePowerInWatts, double myAttenuationFactorIndB,double noiseVariance, double dopplerSpreadInHz, ivec CQIportReferenceNumbers ,int nTaps, vec tapDel, vec tapGain, RSType_E CQIrsType, ivec portIndices)
{
  cout<<"Doing wideband channel estimation"<<endl;
  int rxAntenna=rxSubFrame.length();
  Array< Array<cmat> > channel;
  int nPorts=CQIportReferenceNumbers.length();
  channel.set_size(nPorts);
  imat rsSymbol;
  ivec allSymbols, dataSymbols;
  allSymbols = getIntegers(0,subFramePattern.cols()-1);
  ivec rsToInterpolate, dc(subFramePattern.cols());
  ivec portLocations;
  ivec portLocations_temp;
  cvec TempChan;
  cvec txRS, rxRS, rxRS1;
  cmat tempChan;
  int NoOfBandsInPartsToBeEStimated=1;
  
  if(CQIrsType==_RSTYPE_CRS_)
  {
    if ((subFramePattern.rows()/12)==6 || (subFramePattern.rows()/12)==15 || (subFramePattern.rows()/12)==1)
      NoOfBandsInPartsToBeEStimated=1;
    else
      NoOfBandsInPartsToBeEStimated=5;
  }
  
  else if(CQIrsType==_RSTYPE_CSIRS_)
    NoOfBandsInPartsToBeEStimated=1;
  
  else
      {
    cout<<"[both:] CQI RS type is not supported in estimateWideBandChannel()"<<endl;
    abort();
  } 
  
  if((100%NoOfBandsInPartsToBeEStimated)!=0)
  {
    cout<<"[both:] error in estimateWideBandChannel, not Proper number of parts"<<endl;
    abort();
  }
  bool twoDMMSE=true;
  if(!twoDMMSE)
  for(int iport=0; iport<nPorts; iport++)
  {
    txRS.set_size(0);
    channel(iport).set_length(rxAntenna);
    for(int rx_ant=0;rx_ant<rxAntenna;rx_ant++)
      channel(iport)(rx_ant).set_size(subFramePattern.rows(), subFramePattern.cols());/// Channel estimation includes DC Subcarrier
    dataSymbols = findNot(allSymbols, RSsymbolsPerPort(iport));
    for (int sym=0; sym<RSsymbolsPerPort(iport).length(); sym++)
    {
      rsSymbol.set_size(subFramePattern.rows(),1);
      rsSymbol.clear();
      rsSymbol= subFramePattern.get_col(RSsymbolsPerPort(iport)(sym));
      portLocations=find(rsSymbol,CQIportReferenceNumbers(iport),true);
      portLocations_temp.set_length(portLocations.length());
      portLocations_temp=portLocations + guardSubCarriersOnOneSide;
      portLocations_temp +=to_ivec(portLocations_temp>=rxSubFrame(0).rows()/2);
      rsSymbol.clear();
      portLocations=portLocations_temp-guardSubCarriersOnOneSide;
      int LengthOfEachPartPilot=(int)(portLocations.length()/NoOfBandsInPartsToBeEStimated);
      int LengthOfEachPartData=(int)(subFramePattern.rows()/NoOfBandsInPartsToBeEStimated);
     for(int rx_ant=0;rx_ant<rxAntenna;rx_ant++)
      {
	TempChan.set_length(0);
	for(int nPart=0;nPart<NoOfBandsInPartsToBeEStimated;nPart++)
	{
	  rsSymbol.set_size(LengthOfEachPartData+(nPart==0),1);
	  append(TempChan,estimateChannelinSymbolBy1DMMSE(txRSPerPortPerSymbol(iport)(sym).get(nPart*LengthOfEachPartPilot,(nPart+1)*LengthOfEachPartPilot-1), rxSubFrame(rx_ant).get_col(RSsymbolsPerPort(iport)(sym))(portLocations_temp.get(nPart*LengthOfEachPartPilot,(nPart+1)*LengthOfEachPartPilot-1)), rsSymbol, portLocations.get(nPart*LengthOfEachPartPilot,(nPart+1)*LengthOfEachPartPilot-1), noiseVariance,  inv_dB(myAttenuationFactorIndB),  interferersRxPowerInWatts,restOfintereferencePowerInWatts, dopplerSpreadInHz,  nTaps,tapDel,tapGain));
	}
	channel(iport)(rx_ant).set_col(RSsymbolsPerPort(iport)(sym),TempChan);
      }
    }
    for (int sym=0; sym<dataSymbols.length(); sym++)
    {
      rsToInterpolate = getRStoInterpolate(dataSymbols(sym), RSsymbolsPerPort(iport));
      for(int rx_ant=0;rx_ant<rxAntenna;rx_ant++)
	for(int i=0; i<subFramePattern.rows(); i++){
	  channel(iport)(rx_ant)(i,dataSymbols(sym)) = ((double)(dataSymbols(sym)-rsToInterpolate(0))/((double)(rsToInterpolate(1)-rsToInterpolate(0))))*((channel(iport)(rx_ant)(i,rsToInterpolate(1)))-channel(iport)(rx_ant)(i,rsToInterpolate(0)))+channel(iport)(rx_ant)(i,rsToInterpolate(0));
	}
    }
 }
 else
 {
   if((CQIrsType==_RSTYPE_CRS_ || (CQIrsType==_RSTYPE_CSIRS_ && nPorts==1)) && CQIportReferenceNumbers(0)<3000 && CQIportReferenceNumbers(0)>3432)
    for(int iport=0; iport<nPorts; iport++)
    {
      channel(iport).set_length(rxAntenna);
      imat tempSubFramePattern=concat_vertical(zeros_i(guardSubCarriersOnOneSide,subFramePattern.cols()),subFramePattern);
      tempSubFramePattern=concat_vertical(tempSubFramePattern.get_rows(0,(rxSubFrame(0).rows()/2)-1),concat_vertical(zeros_i(1,tempSubFramePattern.cols()),tempSubFramePattern.get_rows((rxSubFrame(0).rows()/2),tempSubFramePattern.rows()-1)));
      imat subFramePatternWithDcCarrier=tempSubFramePattern.get_rows(guardSubCarriersOnOneSide,tempSubFramePattern.rows()-1);  
      tempSubFramePattern=concat_vertical(tempSubFramePattern,zeros_i(guardSubCarriersOnOneSide-1,tempSubFramePattern.cols()));
      portLocations_temp=find(tempSubFramePattern,CQIportReferenceNumbers(iport),true);
      for(int rx_ant=0;rx_ant<rxAntenna;rx_ant++)
      {
	channel(iport)(rx_ant).set_size(0,0);
	for(int nPart=0;nPart<NoOfBandsInPartsToBeEStimated;nPart++)
	{
	  txRS.set_size(0);rxRS.set_size(0);
	  imat tempPartSubframe;
	  tempPartSubframe=subFramePatternWithDcCarrier.get_rows(nPart*(subFramePatternWithDcCarrier.rows()/NoOfBandsInPartsToBeEStimated) + ((nPart>=(NoOfBandsInPartsToBeEStimated/2+1)) && NoOfBandsInPartsToBeEStimated > 1),(nPart+1)*(subFramePatternWithDcCarrier.rows()/NoOfBandsInPartsToBeEStimated)-1+ ((nPart>=(NoOfBandsInPartsToBeEStimated/2)) && NoOfBandsInPartsToBeEStimated > 1));
	  portLocations=find(tempPartSubframe,CQIportReferenceNumbers(iport),true);
	  for(int sym=0;sym<txRSPerPortPerSymbol(iport).length(); sym++)
	    append(txRS , txRSPerPortPerSymbol(iport)(sym).get(nPart*portLocations.length()/RSsymbolsPerPort(iport).length(),(nPart+1)*(portLocations.length()/RSsymbolsPerPort(iport).length())-1));
          for(int sym=0;sym<txRSPerPortPerSymbol(iport).length(); sym++)
	  {
	    rxRS1=getFromMatrix(rxSubFrame(rx_ant),(portLocations_temp.get(sym*txRSPerPortPerSymbol(iport)(sym).length(),(sym+1)*txRSPerPortPerSymbol(iport)(sym).length()-1)).get(nPart*(txRSPerPortPerSymbol(iport)(sym).length()/NoOfBandsInPartsToBeEStimated),(nPart+1)*(txRSPerPortPerSymbol(iport)(sym).length()/NoOfBandsInPartsToBeEStimated)-1));
           
	    if((portIndices(iport)==16 || portIndices(iport)==18 || portIndices(iport)==20 || portIndices(iport)==22) && (RSsymbolsPerPort(iport)(sym)==RSsymbolsPerPort(iport)(RSsymbolsPerPort(iport).length()-1)))
	      rxRS1=-rxRS1;

	    append(rxRS ,rxRS1);
	  }
           
	  tempChan.set_size(tempPartSubframe.rows(),tempPartSubframe.cols());
	  tempChan.clear();
	  estimateBy2DMMSE(tempChan, txRS, rxRS, tempPartSubframe, portLocations, noiseVariance, inv_dB(myAttenuationFactorIndB), interferersRxPowerInWatts, restOfintereferencePowerInWatts,dopplerSpreadInHz,  nTaps,tapDel,tapGain);
	  channel(iport)(rx_ant)=concat_vertical(channel(iport)(rx_ant), tempChan);
	}
      }
    }
    else if(CQIrsType==_RSTYPE_CSIRS_ && nPorts>1 && CQIportReferenceNumbers(0)>=15 && CQIportReferenceNumbers(0)<=22)
    {
      cvec rxRS;
      ivec orthogonalCover, repPorts=findUniqueNumbers(CQIportReferenceNumbers);
      imat tempSubFramePattern=concat_vertical(zeros_i(guardSubCarriersOnOneSide,subFramePattern.cols()),subFramePattern);
      tempSubFramePattern=concat_vertical(tempSubFramePattern.get_rows(0,(rxSubFrame(0).rows()/2)-1),concat_vertical(zeros_i(1,tempSubFramePattern.cols()),tempSubFramePattern.get_rows((rxSubFrame(0).rows()/2),tempSubFramePattern.rows()-1)));      
      imat subFramePatternWithDcCarrier=tempSubFramePattern.get_rows(guardSubCarriersOnOneSide,tempSubFramePattern.rows()-1);
      tempSubFramePattern=concat_vertical(tempSubFramePattern,zeros_i(guardSubCarriersOnOneSide-1,tempSubFramePattern.cols()));
      for(int iport=0; iport<repPorts.length(); iport++)
      {
	ivec portLocations=find(tempSubFramePattern, repPorts(iport), true);
	int nTones=portLocations.length();
	ivec portNumbers=portIndices(find(CQIportReferenceNumbers, repPorts(iport), true)), portLoc;
	int coverSize=2, nPortSCs=nTones/coverSize;
	Array<cvec> txRS1(nPortSCs), rxRS1(nPortSCs);
	for(int sc=0; sc<nPortSCs; sc++)
	{
	  txRS1(sc).set_size(coverSize);
	  rxRS1(sc).set_size(coverSize);
	}
	cvec txRS2(nPortSCs), txRS3(coverSize), rxRS2(nPortSCs);
	imat coverPerPort(portNumbers.length(), coverSize);
	for(int pid=0; pid<portNumbers.length(); pid++)
	  if(portNumbers(pid)>=15 && portNumbers(pid)<=22)
	    coverPerPort.set_row(pid, getOrthoCoverCode(portNumbers(pid)));
	for(int iport1=0; iport1<portNumbers.length(); iport1++)
	{
	  int portID=find(portIndices, portNumbers(iport1));
	  channel(portID).set_length(rxAntenna);
	  portLocations_temp=find(tempSubFramePattern,CQIportReferenceNumbers(portID),true);
	  imat tempPartSubframe=subFramePatternWithDcCarrier;
	  tempPartSubframe.del_row((tempPartSubframe.rows()-1)/2);
	  portLocations=find(tempPartSubframe,CQIportReferenceNumbers(iport),true);
	  for(int sc=0; sc<nPortSCs; sc++)
	    for(int sym=0; sym<coverSize; sym++)
	    { txRS1(sc)(sym) = txRSPerPortPerSymbol(portID)(sym)(sc);
	      if((portNumbers(iport1)==16 || portNumbers(iport1)==18 || portNumbers(iport1)==20 ||portNumbers(iport1)==22) && (sym==1))
		txRS1(sc)(sym) = -txRS1(sc)(sym);
	    }
	    for(int rx_ant=0;rx_ant<rxAntenna;rx_ant++)
	    {
	      rxRS.set_length(0);
	      channel(portID)(rx_ant).set_size(tempPartSubframe.rows(), tempPartSubframe.cols());
	      for(int sym=0;sym<txRSPerPortPerSymbol(portID).length(); sym++)
	      {	append(rxRS,getFromMatrix(rxSubFrame(rx_ant),(portLocations_temp.get(sym*txRSPerPortPerSymbol(portID)(sym).length(),(sym+1)*txRSPerPortPerSymbol(portID)(sym).length()-1)).get(0,(txRSPerPortPerSymbol(portID)(sym).length()/NoOfBandsInPartsToBeEStimated)-1)));
		
	      }
		for(int sc=0; sc<nPortSCs; sc++)
		for(int sym=0; sym<coverSize; sym++)
		  rxRS1(sc)(sym) = rxRS(sc+sym*nPortSCs);
              for(int sc=0; sc<nPortSCs; sc++)
	      {
		txRS1(sc) = elem_mult(txRS1(sc), to_cvec(coverPerPort.get_row(iport1)));
		txRS2(sc) = txRS1(sc)(iport1);
		txRS3 = reverse(txRS1(sc));
		rxRS2(sc) = sum(elem_mult(elem_mult(txRS3, to_cvec(coverPerPort.get_row(iport1))), rxRS1(sc)))*(txRS1(sc)(iport1)/((double)coverSize*prod(txRS1(sc))));		
	      }
	      portLoc.set_size(nPortSCs);
	      for(int sc=0; sc<nPortSCs; sc++)
		portLoc(sc)=portLocations(iport1*nPortSCs+sc);
		estimateBy2DMMSE(channel(portID)(rx_ant) ,txRS2, rxRS2, tempPartSubframe, portLoc, noiseVariance, inv_dB(myAttenuationFactorIndB), interferersRxPowerInWatts, restOfintereferencePowerInWatts, dopplerSpreadInHz,  nTaps,tapDel,tapGain);
	    
	      

	    }
	}
      }
    }
else if(CQIrsType==_RSTYPE_CSIRS_ /*&& nPorts>1*/ && CQIportReferenceNumbers(0)>=3000 && CQIportReferenceNumbers(0)<3432/*&& (portIndices(iport)>=3000 && portIndices(iport)<=3432)*/)
    {
      cvec rxRS;
      ivec orthogonalCover, repPorts=findUniqueNumbers(CQIportReferenceNumbers);
      
      imat tempSubFramePattern=concat_vertical(zeros_i(guardSubCarriersOnOneSide,subFramePattern.cols()),subFramePattern);
      tempSubFramePattern=concat_vertical(tempSubFramePattern.get_rows(0,(rxSubFrame(0).rows()/2)-1),concat_vertical(zeros_i(1,tempSubFramePattern.cols()),tempSubFramePattern.get_rows((rxSubFrame(0).rows()/2),tempSubFramePattern.rows()-1)));      
      imat subFramePatternWithDcCarrier=tempSubFramePattern.get_rows(guardSubCarriersOnOneSide,tempSubFramePattern.rows()-1);
	    
      tempSubFramePattern=concat_vertical(tempSubFramePattern,zeros_i(guardSubCarriersOnOneSide-1,tempSubFramePattern.cols()));
      
      
      for(int iport=0; iport<repPorts.length(); iport++)
      {
	ivec portLocations=find(tempSubFramePattern, repPorts(iport), true);
	int nTones=portLocations.length();
	ivec portNumbers=portIndices(find(CQIportReferenceNumbers, repPorts(iport), true)), portLoc;
	
	int FDCDM=1, TDCDM=1;
	ivec cuVec, ruVec;
	cuVec=cvectorize(tempSubFramePattern);
	
	int i=find(cuVec,repPorts(iport));
	//cout<<"i : "<<i<<endl;
	
	i++;
	while(1)
	{
	  if(cuVec(i)==repPorts(iport))
	  {
	    FDCDM++;
	    i++;
	  }
	  else
	    break;
	}
	ruVec=rvectorize(tempSubFramePattern);
	
	int j = find(ruVec,repPorts(iport));
	j++;
	while(1)
	{
	  if(ruVec(j)==repPorts(iport))
	  {
	    TDCDM++;
	    j++;
	  }
	  else
	    break;
	}
	cout<<"TDCDM,FDCDM : "<<TDCDM<<" ,  "<<FDCDM<<endl;
	
	int coverSize=TDCDM*FDCDM;
	int nPortSCs=nTones/coverSize; //nPortSCs=nTones/FDCDM;
	Array <cmat> txRS1(nPortSCs), rxRS1(nPortSCs),rxRS3(nPortSCs),txRS3(nPortSCs);
	cvec txRS2(nPortSCs), rxRS2(nPortSCs);
	cvec txRS44(nPortSCs);
	Array <ivec> coverPerPort1(FDCDM);
	
	for(int loop=0;loop<FDCDM;loop++)
	  coverPerPort1(loop).set_length(TDCDM);
	
	cmat coverPerPort(FDCDM,TDCDM);
	for(int sc=0; sc<nPortSCs; sc++)
	{
	  txRS1(sc).set_size(FDCDM,TDCDM);
	  rxRS1(sc).set_size(FDCDM,TDCDM);
	  txRS3(sc).set_size(FDCDM,TDCDM);
	  rxRS3(sc).set_size(FDCDM,TDCDM);
	}
	//cmat coverPerPort(FDCDM,TDCDM);
		
	for(int iport1=0; iport1<portNumbers.length(); iport1++)
	{ 
          coverPerPort1=getNROrthoCoverCodeforCSIRS(portNumbers(iport1),FDCDM,TDCDM);
	 
	  for(int loop1=0;loop1<TDCDM;loop1++)
	    for(int loop2=0;loop2<FDCDM;loop2++)
	    for(int loop2=0;loop2<FDCDM;loop2++)
	      coverPerPort(loop2,loop1)=coverPerPort1(loop2)(loop1);
	  
	   int portID=find(portIndices, portNumbers(iport1));
	  
	   channel(portID).set_length(rxAntenna);
	 
	  portLocations_temp=find(tempSubFramePattern,CQIportReferenceNumbers(portID),true);
	  imat tempPartSubframe=subFramePatternWithDcCarrier;
	  tempPartSubframe.del_row((tempPartSubframe.rows()-1)/2);
	  portLocations=find(tempPartSubframe,CQIportReferenceNumbers(portID),true);
	
	  for(int symno=0, sc=0;symno<txRSPerPortPerSymbol(portID).length();symno=symno+TDCDM)
	    {
	      for(int freqno=0;freqno<txRSPerPortPerSymbol(portID)(symno).length();freqno=freqno+FDCDM)
	      {
		for(int t=0;t<TDCDM;t++)
		{
		  for(int f=0;f<FDCDM;f++)
		  {	
		    txRS1(sc)(f,t) = txRSPerPortPerSymbol(portID)(symno+t)(freqno+f);
		  }
	
		 }
	
	    sc++;
	    
	      }
  
	    }
	   
	   	  
	  for(int rx_ant=0;rx_ant<rxAntenna;rx_ant++)
	    {
	    rxRS.set_length(0);
	      channel(portID)(rx_ant).set_size(tempPartSubframe.rows(), tempPartSubframe.cols());
	  int rsinonesymbol;
	    append(rxRS,getFromMatrix(rxSubFrame(rx_ant),portLocations_temp));
	   
	    for(int symno=0, sc=0;symno<txRSPerPortPerSymbol(portID).length();symno++)
	  { 
	    rsinonesymbol=portLocations_temp.length()/TDCDM;
	    
	      for(int freqno=0;freqno<txRSPerPortPerSymbol(portID)(symno).length();freqno++)
	    { 
		rxRS1(((txRSPerPortPerSymbol(portID)(symno).length()/FDCDM)*(symno/TDCDM))+freqno/FDCDM)(freqno%FDCDM,symno%TDCDM)=rxRS(symno*rsinonesymbol+freqno);
	      }
	    }
	  	
	  for(int sc=0; sc<nPortSCs; sc++)
	    {
	      rxRS1(sc)=elem_mult(rxRS1(sc),coverPerPort);
	      txRS3(sc)=elem_mult(txRS1(sc),coverPerPort);
	      rxRS3(sc)=elem_div(rxRS1(sc),txRS3(sc));
	      txRS2(sc)=txRS3(sc)(iport1%FDCDM,iport1/FDCDM);
	      rxRS2(sc)=txRS2(sc)*sum(sum(rxRS3(sc),1))/(double)coverSize;
	      txRS44(sc) = sum(sum(rxRS3(sc),1))/(double)coverSize;
	  
	    }
		
	
		int rowstoskip=0;
		for(int loop=0;;loop++)
		{ if(find(tempSubFramePattern.get_row(loop), repPorts(iport))==-1)
		  {
		    rowstoskip++;
		    
		  }
		    else
		    break;
		}
	    
	      portLoc.set_size(nPortSCs);
	      for(int sc=0; sc<nPortSCs; sc++)
		portLoc(sc)=portLocations((coverSize*sc)); //portLoc(sc)=portLocations(FDCDM*sc);
	      cout<<"\n portlocations \t"<<portID<<portLocations<<endl;
	     	      
	       estimateBy2DMMSE(channel(portID)(rx_ant) ,txRS2, rxRS2, tempPartSubframe, portLoc, noiseVariance, inv_dB(myAttenuationFactorIndB), interferersRxPowerInWatts, restOfintereferencePowerInWatts, dopplerSpreadInHz,  nTaps,tapDel,tapGain);
	    
	      for(int sc=0; sc<nPortSCs; sc++)
	      {
		int rowID = portLoc(sc)/tempPartSubframe.rows();
		int colID = portLoc(sc)%tempPartSubframe.rows();
		channel(portID)(rx_ant)(colID,rowID)= txRS44(sc);
	//	cout<<'\n'<<sc<<'\t'<<portLoc(sc);
	      }
	      	 
		 
	    }
	    
	
	}//end of iport1 loop
      }//end of iport loop
    }////end else if(3000)
}
  return(channel);
}

/*! \fn estimateChannelinSymbolBy1dMMSE(...)
* \brief estimates the raw channel across all tones in a given symbol for a given port using CRS.
* \param [in] txRSPerPort --> Transmitted RS sequence for given port in a given symbol.
* \param [in] rxRS --> Received RS sequence for given port in a given symbol.
* \param [in] rsSymbol --> Vector of Symbol dimensions with data indicated as "0" and ports indicated as "1, 2, 3, 4"
* \param [in] portLocations --> Indicates subcarrier locations used by CRS in a given symbol for a given port
* \param [in] rsPowerBoostInDB --> To boost RS power for better channel estimation
* \param [in] noiseVariance ---> noise variance
* \param [in] myAttenuationFactorInWatts --> To account for LArge scale fading seen by RS
* \param [in] interferersPowerInWatts --> Power of strong interferers to CRS
* \param [in] restOfintereferencePowerInWatts ---> power of weak interferences to CRS
* \param [in] dopplerSpreadInHz --> Used by 1D-MMSE to calculate frequency correlation
* \param [in] fadingParameters --> Used by 1D-MMSE to calculate frequency correlation
* \return Raw Channel of dimension (#SubcarriersInSubframe,1) for given port in a given symbol.
*/

cvec ChannelEstimator::estimateChannelinSymbolBy1DMMSE(cvec txRSPerPort, cvec rxRS, imat rsSymbol, ivec portLocations, double noiseVariance, double myAttenuationFactorInWatts, vec interferersPowerInWatts,double restOfintereferencePowerInWatts, double dopplerSpread, int nTaps, vec tapDel, vec tapGain)
{
  ivec autocorrpattern,crosscorrpattern;
  int nRStones=portLocations.length();
  cmat Y(nRStones,1), CORR_Hpi_Hpi(nRStones,nRStones), CORR_H_Hpi(rsSymbol.rows(),nRStones);
  cmat X(nRStones,nRStones), estimatedChannel;
  cmat noise =(2*noiseVariance+restOfintereferencePowerInWatts)*eye_c(nRStones);
  cmat IntrCov(nRStones,nRStones), estimator;
  int NoInterFer = interferersPowerInWatts.length();


  IntrCov.clear();

  ivec rsloc(portLocations.length()*2);
  for(int rs_cnt=0;rs_cnt<portLocations.length();rs_cnt++)
  {
    rsloc(2*rs_cnt)=0;
    rsloc(2*rs_cnt+1)=portLocations(rs_cnt);
  }
  
  autocorrpattern  = generateautocorrpattern(rsloc,nRStones);
  crosscorrpattern = generatecrosscorrpattern(rsloc,rsSymbol.rows(),rsSymbol.cols(),nRStones);
  CORR_Hpi_Hpi = myAttenuationFactorInWatts*findchannelcorr(autocorrpattern,nRStones,nRStones,dopplerSpread, nTaps,tapDel,tapGain,samplingPeriod,frequencySpacing);
  CORR_H_Hpi =   myAttenuationFactorInWatts*findchannelcorr(crosscorrpattern,rsSymbol.size(),nRStones,dopplerSpread, nTaps,tapDel,tapGain,samplingPeriod,frequencySpacing);//cout<<"CORR_Hpi_Hpi "<<CORR_Hpi_Hpi<<"\nCORR_H_Hpi"<<CORR_H_Hpi<<endl;
  X.clear(); Y.clear();IntrCov.clear();
  Y=rxRS;
  X=diag(txRSPerPort);
  if(NoInterFer)
    for(int intr=0;intr <NoInterFer;intr++)
      IntrCov += interferersPowerInWatts[intr]*eye_c(nRStones);

  estimator = (CORR_H_Hpi*X.hermitian_transpose())*inv(X*CORR_Hpi_Hpi*X.hermitian_transpose()+ IntrCov + noise);
  estimatedChannel = estimator*Y;
  return(estimatedChannel.get_col(0));
}


/*! \fn getRStoInterpolate(...)
* \brief Indicates the two RS symbol locations in a given subframe to be used for channel estimation using Linear Interpolation
* \param sym --> Index of Data symbol where the channel need to be interpolated using two RS symbol locations.
* \param RSsymbolsPerPort --> Indicates the locations of RS symbols for a gien port and subframe.
* \return two RS symbol locations in a given subframe to be used for channel estimation.
*/
ivec ChannelEstimator::getRStoInterpolate(int sym, ivec RSsymbolsPerPort)
{
  ivec rsToInterpolate(2);
  if(sym<RSsymbolsPerPort(0))
  {
    rsToInterpolate(0)=RSsymbolsPerPort(0);
    rsToInterpolate(1)=RSsymbolsPerPort(1);
    return(rsToInterpolate);
  }
  for(int i=0; i<(RSsymbolsPerPort.length()-1); i++)
  {
    if(RSsymbolsPerPort(i)<sym && sym<RSsymbolsPerPort(i+1))
    {
      rsToInterpolate(0)=RSsymbolsPerPort(i);
      rsToInterpolate(1)=RSsymbolsPerPort(i+1);
      return(rsToInterpolate);
    }
  }
  if(sym>RSsymbolsPerPort(RSsymbolsPerPort.length()-1))
  {
    rsToInterpolate(0)=RSsymbolsPerPort(RSsymbolsPerPort.length()-2);
    rsToInterpolate(1)=RSsymbolsPerPort(RSsymbolsPerPort.length()-1);
    return(rsToInterpolate);
  }
  else
  {
    cout<<"[both:] Invalid symbol Index to getRStoInterpolate()..."<<endl;
    abort();
  }
}



Array<cmat> repeatPerToneInterferenceCovariance(cmat interferenceCovariance, int dataTonesPerRB)
{
  Array<cmat> perToneInterferenceCovariance;
  perToneInterferenceCovariance.set_size(dataTonesPerRB);
  for(int tone_cnt=0;tone_cnt<dataTonesPerRB;tone_cnt++)
    perToneInterferenceCovariance(tone_cnt) = interferenceCovariance;
  return(perToneInterferenceCovariance);

}

