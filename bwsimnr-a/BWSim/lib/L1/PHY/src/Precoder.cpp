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

#include "../include/Precoder.h"


/*! \fn getLargeDelayCDDmatrixD(...)
 * \brief Generates Large Delay CDD D-Matrix for given #ports and #layers
 * \param [in] toneIndex ---> tone index for the allocated resource blocks
 * \param [in] nLayers ---> Number of layers, this should be 2/3/4
 * \return Large Delay CDD D-Matrix
 */
cmat getLargeDelayCDDmatrixD(int toneIndex, int nLayers)
{
  cmat W=zeros_c(nLayers, nLayers);
  for (int i=0; i<nLayers; i++)
    W(i,i)=complex< double >(cos(2.0*pi*i*toneIndex/(double)nLayers),sin(-2*pi*i*toneIndex/(double)nLayers));
  if((nLayers>4)||(nLayers<2)) {
    cout<<"[both:] nLayers is incorrect ... exiting in getLargeDelayCDDmatrixD()"<<endl;abort();
  }
  return(W);
}

/*! \fn getLargeDelayCDDmatrixU(...)
 * \brief Generates Large Delay CDD U-Matrix for given #ports and #layers
 * \param nLayers ---> Number of layers, this should be 2/3/4
 * \return Large Delay CDD U-Matrix
 */
cmat getLargeDelayCDDmatrixU(int nLayers)
{
  cmat W=zeros_c(nLayers, nLayers);
  for (int i=0; i<nLayers; i++)
    for (int j=0; j<nLayers; j++)
      W(i,j)=complex< double >(cos(2.0*pi*i*j/(double)nLayers),sin(-2.0*pi*i*j/(double)nLayers));
    
    W=W/sqrt((double)nLayers);
  if((nLayers>4)||(nLayers<2)) {
    cout<<"[both:] nLayers is incorrect ... exiting in getLargeDelayCDDmatrixU()"<<endl;abort();
  }
  return(W);
}

/*! \fn precodeDataInRB(...)
 * \brief Takes the output of layer mapper and precodes them to the antenna ports
 * \param [in] modulatedSymbolsPerLayerPerRB ---> modulated symbols of all layers in an RB
 * \param [in] rbNumber ---> resource block number in frequency domain 
 * \return Precoded Layers(precoded symbols for all layers in an RB)
 */

Array<cvec> precodeDataInRB(Array<cvec> &modulatedSymbolsPerLayerPerRB , DataBlkInfo_S &dataBlkInfo, int rbNumber)
{
  int nSymbolsmappedRB, nSymbolsmappedPerLayer;
  cmat precoder;
  cmat layervec, precodedlayervec;
  int nLayers;

  nLayers=dataBlkInfo.dataGenInfo.nLayers;

  int nPorts;
  if(dataBlkInfo.dataGenInfo.transmissionScheme<6)
    nPorts=dataBlkInfo.dataGenInfo.rbInfo.rsInfo.crsPorts.length();
  else
    nPorts=dataBlkInfo.dataGenInfo.rbInfo.rsInfo.nCQIports;
   Array<cvec> precodedLayers(nPorts);
  nSymbolsmappedRB = dataBlkInfo.dataGenInfo.resourceElementsPerRB(find(dataBlkInfo.resourceBlocks,rbNumber))*nLayers/dataBlkInfo.dataGenInfo.symbolRepetitionFactor;
  nSymbolsmappedPerLayer = nSymbolsmappedRB/nLayers;
  
  if(nSymbolsmappedRB!=nLayers*modulatedSymbolsPerLayerPerRB(0).length())
  {
    cout<<"[both:] Error... Length mismatch in precodeDataInRB() between modulatedSymbolsPerLayerPerRB and nSymbolsmapped..."<<endl;
    abort();
  }
  switch (dataBlkInfo.dataGenInfo.transmissionScheme)
  {
    // Transmission modes 1 and 7 are single layer modes, no precoding is required
    case _TRANSMISSION_SCHEME_SINGLE_PORT_CRS_:
    case _TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_:
    {
      precodedLayers = modulatedSymbolsPerLayerPerRB;
      break;
    }
    
    case _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_:
    {
      // gives the precoding matrix for transmit diversity scheme
      precoder = getSFBCprecoder(nPorts);
      layervec.set_size(nLayers*2,1);
      precodedlayervec.set_size(nPorts*nPorts,1);
      for (int j=0; j<nPorts; j++)
	precodedLayers(j).set_size(nPorts*nSymbolsmappedPerLayer);
      for (int i=0; i<nSymbolsmappedPerLayer; i++)
      {
	//arrange the data as a vector(refer TS 36.211 version 10.0.0 Release 10)
	for (int j=0; j<nLayers; j++)
	{
	  layervec(j,0) = modulatedSymbolsPerLayerPerRB(j)[i].real();
	  layervec(j+nLayers,0) = modulatedSymbolsPerLayerPerRB(j)[i].imag();
	}
	precodedlayervec = precoder*layervec;
	// arrange the precoded symbols to the respective layers
	for (int j=0; j<nPorts; j++)
	  for (int k=0; k<nPorts; k++)
	    precodedLayers(j)[nPorts*i+k] = precodedlayervec(nPorts*k+mod(j,nPorts),0);
      }
      break;
    }
    // for mode-3
    case _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_:/// SK: 2<= nLayers <=4, nPorts = 2/4
    {
      ivec reIndxPerRB=dataBlkInfo.reIndxForPrecoding(find(dataBlkInfo.resourceBlocks,rbNumber));
      cmat largeDelayCDDmatrix;
      Array<cmat> largedelaycodebook;
      layervec.set_size(nLayers,1);
      precodedlayervec.set_size(nPorts,1);
      for (int j=0; j<nPorts; j++)
	precodedLayers(j).set_size(nSymbolsmappedPerLayer);
      switch (nPorts)
      {
	case 2:
	{
	  largedelaycodebook = generateCodebookForDLTwoPorts(2);
	  // generates the codebook for 2-layer and 2-antenna port
	  for (int i=0; i<nSymbolsmappedPerLayer; i++)
	  {
	    largeDelayCDDmatrix = getLargeDelayCDDmatrixD(reIndxPerRB(i), nLayers) * getLargeDelayCDDmatrixU(nLayers);
	    precoder = largedelaycodebook(0)*largeDelayCDDmatrix;
	    // taking a single RE symbols from all layers
	    for (int j=0; j<nLayers; j++)
	      layervec(j,0)= modulatedSymbolsPerLayerPerRB(j)[i];
	    precodedlayervec = precoder*layervec;
	    for (int j=0; j<nPorts; j++)
	      precodedLayers(j)[i] = precodedlayervec(j,0);
	  }
	}
	break;
	case 4:
	{
	  largedelaycodebook = generateCodebookForDLFourPorts(nLayers);
	  for (int i=0; i<nSymbolsmappedPerLayer; i++)
	  {
	    
	    largeDelayCDDmatrix = getLargeDelayCDDmatrixD(reIndxPerRB(i), nLayers) * getLargeDelayCDDmatrixU(nLayers);
	    int precindx;
	    precindx = mod(floor(((double)reIndxPerRB(i))/(double)nLayers),4.0)+12.0;
	    precoder =  largedelaycodebook(precindx) * largeDelayCDDmatrix;
	    for (int j=0; j<nLayers; j++)
	      layervec(j,0) = modulatedSymbolsPerLayerPerRB(j)[i];
	    precodedlayervec = precoder*layervec;
	    for (int j=0; j<nPorts; j++)
	      precodedLayers(j)[i] = precodedlayervec(j,0);
	  }
	}break;
	
	default:
	{cout<<"[both:] Undefined #nPorts .... exiting in  precodeDataInRB()"<<endl;abort();}
      }
      break;
    }
    
	case _TRANSMISSION_SCHEME_CL_SM_:
	case _TRANSMISSION_SCHEME_CL_MU_MIMO_:
	case _TRANSMISSION_SCHEME_CL_BF_:
	case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
	case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
	case _TRANSMISSION_SCHEME_NR_CL_:
	{ 
	  precoder = dataBlkInfo.getMyPrecoder(rbNumber);
	  layervec.set_size(nLayers,1);
	  precodedlayervec.set_size(nPorts,1);
	  for (int j=0; j<nPorts; j++)
	    precodedLayers(j).set_size(nSymbolsmappedPerLayer);
	   for (int i=0; i<nSymbolsmappedPerLayer; i++)
	  { for (int j=0; j<nLayers; j++)
	    {
	      layervec(j,0) = modulatedSymbolsPerLayerPerRB(j)[i];
	     }
	      precodedlayervec =precoder*layervec;
	    for (int j=0; j<nPorts; j++)
	    {
	      precodedLayers(j)[i] = precodedlayervec(j,0);
	    }
	    
	  }
	  break;
	}
	case _TRANSMISSION_SCHEME_OLMUMIMO_:
	{
	  if(dataBlkInfo.isSemiOpenLoopScheme)
	  {
	    precoder = dataBlkInfo.getMyPrecoder(rbNumber);
	    layervec.set_size(nLayers,1);
	    precodedlayervec.set_size(nPorts,1);
	    for (int j=0; j<nPorts; j++)
	      precodedLayers(j).set_size(nSymbolsmappedPerLayer);
	    for (int i=0; i<nSymbolsmappedPerLayer; i++)
	    {
	      for (int j=0; j<nLayers; j++)
		layervec(j,0) = modulatedSymbolsPerLayerPerRB(j)[i];
	      precodedlayervec =precoder*layervec;
	      for (int j=0; j<nPorts; j++)
		precodedLayers(j)[i] = precodedlayervec(j,0);
	    }
	  }
	  else
	  {
	    cout<<"[both:] isSemiOpenLoopScheme need to be true for UE "<<dataBlkInfo.dataGenInfo.rnti<<endl;
	    abort();
	  }
	  break;
	}
	
	
	default:
	{
	  cout<<"[both:] Undefined Transmission scheme .... exiting in precodeDataInRB()"<<endl;
	  abort();
	}
  }
  
  return precodedLayers;
}
/*! \fn getSFBCprecoder(...)
 * \brief Takes the output of layer mapper and precodes them to the antenna ports
 * \param [in] nPorts ---> number of transmitting antenna ports
 * \return SFBC precoding matrix
 */
cmat getSFBCprecoder(int nPorts)
{
  cmat precoder=zeros_c(nPorts*nPorts, nPorts*2);
  const dComplex R = dComplex(1.0,0.0);
  const dComplex I = dComplex(0.0,1.0);
  
  switch(nPorts)
  {
    case 2:
    {
      precoder(0,0) = R; precoder(0,2) = I;
      precoder(1,1) = -R; precoder(1,3) = I;
      precoder(2,1) = R; precoder(2,3) = I;
      precoder(3,0) = R; precoder(3,2) = -I;
    }
    break;
    
    case 4:
    {
      precoder(0,0) = R; precoder(0,4) = I;
      precoder(2,1) = -R; precoder(2,5) = I;
      precoder(4,1) = R; precoder(4,5) = I;
      precoder(6,0) = R; precoder(6,4) = -I;
      precoder(9,2) = R; precoder(9,6) = I;
      precoder(11,3) = -R; precoder(11,7) = I;
      precoder(13,3) = R; precoder(13,7) = I;
      precoder(15,2) = R; precoder(15,6) = -I;
    }
    break;
    
    default:
    {
      cout<<"[both:] Undefined #Ports .... exiting in getSFBCprecoder()"<<endl;
      abort();
    }
  }
  
  precoder = sqrt(1/2.0)*precoder;
  
  return precoder;
}


Array< Array<cmat> >  applyPortToAntennaMapperOnMyRBChannel(Array< Array<cmat> > &myRbChannel, PortToAntennaMapper_S *portToAntennaMapper, int nPorts,bool isCRSMode)
{
  
  int nTxAntenna, nRxAntenna;
  nTxAntenna=myRbChannel.length();
  nRxAntenna=myRbChannel(0).length();
  if(nPorts!=nTxAntenna)
  {
    cmat portToAntMapper;
    if(isCRSMode)
      portToAntMapper=portToAntennaMapper->getCRSPortToAntennaMapper(nPorts, nTxAntenna);
    else
      portToAntMapper=portToAntennaMapper->getPortToAntennaMapper(nPorts, nTxAntenna);
    Array< Array<cmat> > myRbChannelOverPorts(nPorts);
    for(int port_cnt=0;port_cnt<nPorts;port_cnt++)
    {
      myRbChannelOverPorts(port_cnt).set_length(nRxAntenna);
      for(int rx_cnt=0;rx_cnt<nRxAntenna;rx_cnt++)
	myRbChannelOverPorts(port_cnt)(rx_cnt).set_size(myRbChannel(0)(0).rows(),myRbChannel(0)(0).cols());
    }
    for(int elem_cnt=0;elem_cnt<myRbChannel(0)(0).size();elem_cnt++)
    {
      cmat temp=getAcrossArray(myRbChannel,elem_cnt);
      temp=temp*portToAntMapper;
      setAcrossArray(myRbChannelOverPorts,elem_cnt,temp);
    }
    return(myRbChannelOverPorts);
  }
  else
    return(myRbChannel);
}

Array<cmat> performSFBCPrecoding(Array<cmat>& unprecodedChannel){
  
  int nLayers = unprecodedChannel(0).cols();
  int nRxAntenna = unprecodedChannel(0).rows();
  Array<cmat> precodedChannel;
  precodedChannel.set_size(unprecodedChannel.length()/nLayers);
  
  for(int i=0;i<precodedChannel.length();i++)
  {
    precodedChannel(i) = zeros_c(nRxAntenna*nLayers,nLayers);
    for(int rxant=0;rxant<nRxAntenna;rxant++)
    {
      if(nLayers==2)
      {
	// arranging the unprecodedChannel matrix in SFBC matrix form 
	precodedChannel(i)(rxant,0) = sqrt(1/2.0)*unprecodedChannel(nLayers*i)(rxant,0);
	precodedChannel(i)(rxant,1) =  sqrt(1/2.0)*(-unprecodedChannel(nLayers*i)(rxant,1));
	precodedChannel(i)(rxant+nRxAntenna,0) = sqrt(1/2.0)*conj(unprecodedChannel(nLayers*i+1)(rxant,1));
	precodedChannel(i)(rxant+nRxAntenna,1) = sqrt(1/2.0)*conj(unprecodedChannel(nLayers*i+1)(rxant,0));
      }
      else //nLayers==4
      {
	precodedChannel(i)(rxant,0) = sqrt(1/2.0)*unprecodedChannel(nLayers*i)(rxant,0);
	precodedChannel(i)(rxant,1) = sqrt(1/2.0)*(-unprecodedChannel(nLayers*i)(rxant,2));
	precodedChannel(i)(rxant+nRxAntenna,0) = sqrt(1/2.0)*conj(unprecodedChannel(nLayers*i+1)(rxant,2));
	precodedChannel(i)(rxant+nRxAntenna,1) = sqrt(1/2.0)*conj(unprecodedChannel(nLayers*i+1)(rxant,0));
	
	precodedChannel(i)(rxant+2*nRxAntenna,2) = sqrt(1/2.0)*unprecodedChannel(nLayers*i+2)(rxant,1);
	precodedChannel(i)(rxant+2*nRxAntenna,3) = sqrt(1/2.0)*(-unprecodedChannel(nLayers*i+2)(rxant,3));
	precodedChannel(i)(rxant+3*nRxAntenna,2) = sqrt(1/2.0)*conj(unprecodedChannel(nLayers*i+3)(rxant,3));
	precodedChannel(i)(rxant+3*nRxAntenna,3) = sqrt(1/2.0)*conj(unprecodedChannel(nLayers*i+3)(rxant,1));
      }
      
      
    }
  }
  
  return precodedChannel;
}


/*! \fn precodeMyRBChannel(...)
 * \brief multiplys the channel with the appropriate precoder
 * \param [in] myRbChannel ---> contains the raw channel of an RB
 * \param [in] rbNumber ---> resource block number
 * \param [in] dataTones ---> number of data tones in an RB 
 * \return precodedChannel
 */
Array< cmat >  precodeMyRBChannel(Array< Array<cmat> > &myRbChannel, PortToAntennaMapper_S *portToAntennaMapper, int rbNumber, DataBlkInfo_S &dataBlkInfo,ivec dataTones)
{
  int nTxAntenna, nLayers, nPorts, nCRSPorts, precindx;
  nTxAntenna=myRbChannel.length();
  
  nLayers=dataBlkInfo.dataGenInfo.nLayers;

  nPorts=dataBlkInfo.dataGenInfo.rbInfo.rsInfo.nCQIports;
  nCRSPorts=dataBlkInfo.dataGenInfo.rbInfo.rsInfo.crsPorts.length();
  cmat precoder;
  cmat largeDelayCDDmatrix;
  Array<cmat> largedelaycodebook;
  
  
  if(dataTones.length()==0)
    dataTones=getIntegers(0,myRbChannel(0)(0).size()-1);
  
  cmat portToAntMapper;
  if(nCRSPorts && dataBlkInfo.dataGenInfo.transmissionScheme < 6)
    portToAntMapper = portToAntennaMapper->getCRSPortToAntennaMapper(nCRSPorts,nTxAntenna);
  else
    portToAntMapper = portToAntennaMapper->getPortToAntennaMapper(nPorts,nTxAntenna);
  
  Array< cmat > channel(dataTones.length());
  for(int tone_cnt=0;tone_cnt<dataTones.length();tone_cnt++)
    channel(tone_cnt)=getAcrossArray(myRbChannel,dataTones(tone_cnt))*portToAntMapper;
  
  Array< cmat > precodedChannel;
  
  if(dataBlkInfo.dataGenInfo.transmissionScheme == _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_)
  {
    precodedChannel.set_size(dataTones.length());
    ivec reIndxPerRB=dataBlkInfo.reIndxForPrecoding(find(dataBlkInfo.resourceBlocks,rbNumber));
    switch(nPorts)
    {
      case 2:
	
	largedelaycodebook = generateCodebookForDLTwoPorts(nPorts);
	for(int i=0;i<dataTones.length();i++)
	{
	  largeDelayCDDmatrix = getLargeDelayCDDmatrixD(reIndxPerRB(i),nLayers) * getLargeDelayCDDmatrixU(nLayers);
	  precoder =  largedelaycodebook(0)*largeDelayCDDmatrix;
	  precodedChannel(i)=channel(i)*precoder;
	}
	break;
      case 4:
	
	largedelaycodebook = generateCodebookForDLFourPorts(nLayers);
	for(int i=0;i<dataTones.length();i++)
	{
	  largeDelayCDDmatrix = getLargeDelayCDDmatrixD(reIndxPerRB(i),nLayers) * getLargeDelayCDDmatrixU(nLayers);
	  precindx = mod(floor(((double)reIndxPerRB(i))/(double)nLayers),4.0)+12.0;
	  precoder =  largedelaycodebook(precindx) * largeDelayCDDmatrix;
	  precodedChannel(i)=channel(i)*precoder;
	}
	break;
      default:
      {cout<<"[both:] Undefined nPorts ... exiting in getPrecoder()..."<<endl;abort();}
    }
    
  }
  
  else if(dataBlkInfo.dataGenInfo.transmissionScheme == _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
  {
    precodedChannel = performSFBCPrecoding(channel);
  }
  
  else if(dataBlkInfo.dataGenInfo.transmissionScheme == _TRANSMISSION_SCHEME_OLMUMIMO_)
  {
    precodedChannel.set_size(dataTones.length());
    
    if(dataBlkInfo.isSemiOpenLoopScheme)
    {
      precoder = getPrecoder(dataBlkInfo, NULL,nTxAntenna,dataBlkInfo.dataGenInfo.transmissionScheme, rbNumber);//including port to antenna mapping
    }
    else
    {
      cout<<"only SemiOpenLoopScheme supported.. "<<endl;abort();
    }
    for(int i=0;i<dataTones.length();i++)
    {
      precodedChannel(i) = channel(i)*precoder;
    }
  }
  
  else
  {
    precodedChannel.set_size(dataTones.length());
    // portToAntennaMapper taken care separately
    precoder = getPrecoder(dataBlkInfo, NULL,nTxAntenna,dataBlkInfo.dataGenInfo.transmissionScheme, rbNumber);//including port to antenna mapping
    for(int i=0;i<dataTones.length();i++)
      precodedChannel(i) = channel(i)*precoder;
  }
  return precodedChannel;
}




Array< cmat >  precodeIntrRBChannel(Array< Array<cmat> > &myRbChannelOverAntennas, PortToAntennaMapper_S *portToAntennaMapper, int rbNumber, DataBlkInfo_S &dataBlkInfo,ivec dataTones)
{
  int nTxAntenna, nRxAntenna, nLayers, nPorts,nCRSPorts, precindx;
  nTxAntenna=myRbChannelOverAntennas.length();
  nRxAntenna=myRbChannelOverAntennas(0).length();
  nLayers=dataBlkInfo.dataGenInfo.nLayers;
  nPorts=dataBlkInfo.dataGenInfo.rbInfo.rsInfo.nCQIports;
  nCRSPorts=dataBlkInfo.dataGenInfo.rbInfo.rsInfo.crsPorts.length();

  cmat precoder;
  cmat largeDelayCDDmatrix;
  Array<cmat> largedelaycodebook;
  imat rbStructure=dataBlkInfo.dataGenInfo.rbInfo.logicalSubFrameStructure.get_rows(rbNumber*12,rbNumber*12+11);
  ivec allTones=getIntegers(0,rbStructure.size()-1);
  ivec precodedTones(0), unprecodedTones(0);
  ivec unPrecodedPortNumber(0);
  
  cmat portToAntMapper;
  Array< Array<cmat> > myRbChannel;
  if(nCRSPorts != 0 && dataBlkInfo.dataGenInfo.transmissionScheme < 6) // CRS modes
  {
    portToAntMapper = portToAntennaMapper->getCRSPortToAntennaMapper(nCRSPorts,nTxAntenna);
    myRbChannel.set_length(nCRSPorts);
  }
  else
  {
    portToAntMapper = portToAntennaMapper->getPortToAntennaMapper(nPorts,nTxAntenna);
    myRbChannel.set_length(nPorts);
  }
  //cout<<" nLayers : "<<nLayers<<" nPorts : "<<nPorts<<" txScheme : "<<dataBlkInfo.dataGenInfo.transmissionScheme<<" nRxAntenna : "<<nRxAntenna<<" r : "<<myRbChannelOverAntennas(0)(0).rows()<<" c : "<<myRbChannelOverAntennas(0)(0).cols()<<endl;

  for(int port_cnt=0;port_cnt<myRbChannel.length();port_cnt++)
  {
    myRbChannel(port_cnt).set_length(nRxAntenna);
    for(int cnt=0;cnt<myRbChannel(port_cnt).length();cnt++)
      myRbChannel(port_cnt)(cnt).set_size(myRbChannelOverAntennas(0)(0).rows(),myRbChannelOverAntennas(0)(0).cols());
  }
  for(int i=0;i<allTones.length();i++)
  {
    cmat tempChannel=getAcrossArray(myRbChannelOverAntennas,i)*portToAntMapper;
    setAcrossArray(myRbChannel,i,tempChannel);
  }

  ivec unprecodedPorts;
  if(dataBlkInfo.dataGenInfo.transmissionScheme < 8)
    unprecodedPorts=dataBlkInfo.dataGenInfo.rbInfo.rsInfo.crsPorts;
  else
    unprecodedPorts=remove(dataBlkInfo.dataGenInfo.rbInfo.rsInfo.rsPorts,dataBlkInfo.dataGenInfo.rbInfo.rsInfo.DeModPorts);

  unprecodedPorts=findUniqueNumbers(unprecodedPorts);
  ivec portIndices = find(dataBlkInfo.dataGenInfo.rbInfo.rsInfo.rsPorts,unprecodedPorts);
  unprecodedPorts=dataBlkInfo.dataGenInfo.rbInfo.rsInfo.rsPorts(remove(portIndices,-1));
  ivec portRefNumbers=dataBlkInfo.dataGenInfo.rbInfo.getPortReferenceNumbers(unprecodedPorts);
  for(int port_cnt=0;port_cnt<unprecodedPorts.length();port_cnt++)
  {
    ivec  temp=find(rbStructure,portRefNumbers(port_cnt),true);
    append(unprecodedTones,temp);
    append(unPrecodedPortNumber,ones_i(temp.length())*unprecodedPorts(port_cnt));
  }
  
  if(dataBlkInfo.dataGenInfo.rbInfo.rsInfo.DeModrsType==_RSTYPE_DMRS_)
  {
    ivec precodedPorts=dataBlkInfo.dataGenInfo.rbInfo.rsInfo.DeModPorts;
    ivec portRefNumbers=dataBlkInfo.dataGenInfo.rbInfo.getPortReferenceNumbers(precodedPorts);
    for(int port_cnt=0;port_cnt<precodedPorts.length();port_cnt++)
    {
      ivec  temp=find(rbStructure,portRefNumbers(port_cnt),true);
      append(precodedTones,temp);
    }
  }
  //!Adding control signal to unprecodedChannel - 10010 
  ivec isCCH(rbStructure.size());
  for(int c=0,totalC=0;c<rbStructure.cols();c++)
    for(int r=0;r<rbStructure.rows();r++,totalC++)
      isCCH(totalC)=(rbStructure(r,c)>=50);
    
    ivec cchTones=find(isCCH,1,true);
  if(cchTones.length())
  {
    append(unprecodedTones,cchTones);
    ivec temp=zeros_i(cchTones.length())-1;
    append(unPrecodedPortNumber,temp);
  }
  
  //!///////////////////////////////////////////////////
  //!Adding control signal to unprecodedChannel - 10010 
  ivec isblank(rbStructure.size());
  for(int c=0,totalC=0;c<rbStructure.cols();c++)
    for(int r=0;r<rbStructure.rows();r++,totalC++)
      isblank(totalC)=(rbStructure(r,c)==-1);
    
    ivec blankTones=find(isblank,1,true);
  if(blankTones.length())
  {
    append(unprecodedTones,blankTones);
    ivec temp=zeros_i(blankTones.length())-2;
    append(unPrecodedPortNumber,temp);
  }
  
  //!///////////////////////////////////////////////////
  
  ivec sIndx=sort_index(unprecodedTones);
  sort(unprecodedTones);
  unPrecodedPortNumber=unPrecodedPortNumber(sIndx);
  append(precodedTones,find(rbStructure,0,true));  
  
  
  Array< cmat > precodedChannel;
  Array< cmat > unprecodedChannel;
  
  {  
    
    Array< cmat > channel(precodedTones.length());
    
    for(int i=0;i<precodedTones.length();i++)
      channel(i)=getAcrossArray(myRbChannel,precodedTones(i));
    
    
    if(dataBlkInfo.dataGenInfo.transmissionScheme == _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_)
    {
      ivec reIndxPerRB=dataBlkInfo.reIndxForPrecoding(find(dataBlkInfo.resourceBlocks,rbNumber));
      precodedChannel.set_length(precodedTones.length());
      switch(nPorts)
      {
	case 2:
	  
	  largedelaycodebook = generateCodebookForDLTwoPorts(2);  
	  for(int i=0;i<precodedTones.length();i++)
	  {
	    largeDelayCDDmatrix = getLargeDelayCDDmatrixD(reIndxPerRB(i),nLayers) * getLargeDelayCDDmatrixU(nLayers);
	    precoder =  largedelaycodebook(0)*largeDelayCDDmatrix;
	    precodedChannel(i)=channel(i)*precoder;
	  }
	  break;
	case 4:
	  
	  largedelaycodebook = generateCodebookForDLFourPorts(nLayers);
	  for(int i=0;i<precodedTones.length();i++)
	  {
	    largeDelayCDDmatrix = getLargeDelayCDDmatrixD(reIndxPerRB(i),nLayers) * getLargeDelayCDDmatrixU(nLayers);
	    precindx = mod(floor(((double)reIndxPerRB(i))/(double)nLayers),4.0)+12.0;
	    precoder =  largedelaycodebook(precindx) * largeDelayCDDmatrix;
	    precodedChannel(i)=channel(i)*precoder;
	  }
	  break;
	default:
	{cout<<"[both:] Undefined nPorts ... exiting in getPrecoder()..."<<endl;abort();}
      }
      
    }
    
    else if(dataBlkInfo.dataGenInfo.transmissionScheme == _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
    {
      precodedChannel.set_length(precodedTones.length()/2);
      for(int i=0;i<precodedTones.length()/nLayers;i++)
      {
	if(nLayers==2)
	{
	  precodedChannel(i)=zeros_c(nRxAntenna*2,2);
	}
	else
	{
	  precodedChannel(2*i)=zeros_c(nRxAntenna*2,2);
	  precodedChannel(2*i+1)=zeros_c(nRxAntenna*2,2);
	}
	for(int rxant=0;rxant<nRxAntenna;rxant++)
	{
	  if(nLayers==2)
	  {
	    // arranging the channel matrix in SFBC matrix form 
	    precodedChannel(i)(rxant,0) = sqrt(1/2.0)*channel(nLayers*i)(rxant,0);
	    precodedChannel(i)(rxant,1) =  sqrt(1/2.0)*(-channel(nLayers*i)(rxant,1));
	    precodedChannel(i)(rxant+nRxAntenna,0) = sqrt(1/2.0)*conj(channel(nLayers*i+1)(rxant,1));
	    precodedChannel(i)(rxant+nRxAntenna,1) = sqrt(1/2.0)*conj(channel(nLayers*i+1)(rxant,0));
	  }
	  else //nLayers==4
	  {
	    
	    precodedChannel(2*i)(rxant,0) = sqrt(1/2.0)*channel(nLayers*i)(rxant,0);
	    precodedChannel(2*i)(rxant,1) = sqrt(1/2.0)*(-channel(nLayers*i)(rxant,2));
	    precodedChannel(2*i)(rxant+nRxAntenna,0) = sqrt(1/2.0)*conj(channel(nLayers*i+1)(rxant,2));
	    precodedChannel(2*i)(rxant+nRxAntenna,1) = sqrt(1/2.0)*conj(channel(nLayers*i+1)(rxant,0));
	    
	    precodedChannel(2*i+1)(rxant,0) = sqrt(1/2.0)*channel(nLayers*i+2)(rxant,1);
	    precodedChannel(2*i+1)(rxant,1) = sqrt(1/2.0)*(-channel(nLayers*i+2)(rxant,3));
	    precodedChannel(2*i+1)(rxant+nRxAntenna,0) = sqrt(1/2.0)*conj(channel(nLayers*i+3)(rxant,3));
	    precodedChannel(2*i+1)(rxant+nRxAntenna,1) = sqrt(1/2.0)*conj(channel(nLayers*i+3)(rxant,1));
	  } 
	}
      }
    }
    //RB level cycling
    else if(dataBlkInfo.dataGenInfo.transmissionScheme == _TRANSMISSION_SCHEME_OLMUMIMO_)
    {
      precodedChannel.set_size(channel.length());
      
      if(dataBlkInfo.isSemiOpenLoopScheme)
      {
	precoder = getPrecoder(dataBlkInfo, NULL,nTxAntenna,dataBlkInfo.dataGenInfo.transmissionScheme, rbNumber);//including port to antenna mapping
      }
      else
      {
	cout<<"only SemiOpenLoopScheme supported "<<endl;abort();
      }
      
      for(int i=0;i<precodedTones.length();i++)
      {
	precodedChannel(i) = channel(i)*precoder;
      }
    }
    else
    {
      precodedChannel.set_size(channel.length());
	//portToAntennaMapper handled separately
	precoder = getPrecoder(dataBlkInfo, NULL,nTxAntenna,dataBlkInfo.dataGenInfo.transmissionScheme, rbNumber);//including port to antenna mapping
      for(int i=0;i<precodedTones.length();i++)
	precodedChannel(i)=channel(i)*precoder;	
    }
    
  }
  
  //if(unprecodedTones.length()!=0)
  {
    unprecodedChannel.set_length(unprecodedTones.length());
    //     if(dataBlkInfo.dataGenInfo.rbInfo.rsInfo.CQIrsType==_RSTYPE_CSIRS_)
    //     {
    //       cout<<"CSI RS handling not yet done in Precoder."<<endl;
    //       abort();
    //     }
    //     cout<<portNumber<<unprecodedTones<<rbStructure<<endl;
    if(dataBlkInfo.dataGenInfo.transmissionScheme != _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_ || (dataBlkInfo.dataGenInfo.transmissionScheme == _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_ && nLayers==2))
    {  
      for(int tone_cnt=0;tone_cnt<unprecodedTones.length();tone_cnt++)
	if(dataBlkInfo.dataGenInfo.transmissionScheme == _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_ )
	  unprecodedChannel(tone_cnt)=zeros_c(nRxAntenna,nLayers);
	else
	  unprecodedChannel(tone_cnt)=zeros_c(nRxAntenna,nPorts);
      
      //! This loop assumes portNumber=antenna number (taking only the channel corresponding to that antenna of the CRS)
      for(int tone_cnt=0;tone_cnt<unprecodedTones.length();tone_cnt++)
	if(unPrecodedPortNumber(tone_cnt)>=0 && unPrecodedPortNumber(tone_cnt)<4) //CRS
	  for(int rx_ant=0; rx_ant<nRxAntenna; rx_ant++)
	    unprecodedChannel(tone_cnt)(rx_ant,unPrecodedPortNumber(tone_cnt))=myRbChannel(unPrecodedPortNumber(tone_cnt))(rx_ant)(unprecodedTones(tone_cnt));
	  else if(unPrecodedPortNumber(tone_cnt)>=15 && unPrecodedPortNumber(tone_cnt)<23) //CSI-RS
	    for(int rx_ant=0; rx_ant<nRxAntenna; rx_ant++)
	    {
	      unprecodedChannel(tone_cnt)(rx_ant,unPrecodedPortNumber(tone_cnt)-15)=myRbChannel(unPrecodedPortNumber(tone_cnt)-15)(rx_ant)(unprecodedTones(tone_cnt));  
	    }
	    else if(unPrecodedPortNumber(tone_cnt)==-1) //non blank tone
	    {
	    for(int tx_cnt=0;tx_cnt<myRbChannel.length();tx_cnt++)
		for(int rx_ant=0; rx_ant<nRxAntenna; rx_ant++)
		  unprecodedChannel(tone_cnt)(rx_ant,tx_cnt)=myRbChannel(tx_cnt)(rx_ant)(unprecodedTones(tone_cnt));
	    }
	    
	    
    }
    else
    {  
      // Here 0,2 are paired for first 2 layers and 1,3 are paired for 2nd 2 layers... So 0,1=>1st ports, 2,3=>2nd ports
      for(int tone_cnt=0;tone_cnt<unprecodedTones.length();tone_cnt++)
      {
	if(unPrecodedPortNumber(tone_cnt)>=0)
	{
	  //Matrix formed only for 2 layers for convenience
	  unprecodedChannel(tone_cnt)=zeros_c(nRxAntenna,2);
	  if(unPrecodedPortNumber(tone_cnt)==0 || unPrecodedPortNumber(tone_cnt)==1)
	    for(int rx_ant=0; rx_ant<nRxAntenna; rx_ant++)
	      unprecodedChannel(tone_cnt)(rx_ant,0)=myRbChannel(unPrecodedPortNumber(tone_cnt))(rx_ant)(unprecodedTones(tone_cnt));
	    else
	      for(int rx_ant=0; rx_ant<nRxAntenna; rx_ant++)
		unprecodedChannel(tone_cnt)(rx_ant,1)=myRbChannel(unPrecodedPortNumber(tone_cnt))(rx_ant)(unprecodedTones(tone_cnt));
	}
	else if(unPrecodedPortNumber(tone_cnt)==-1) //non blank tone
	{
	  unprecodedChannel(tone_cnt)=zeros_c(nRxAntenna,myRbChannel.length());
	  for(int tx_cnt=0;tx_cnt<myRbChannel.length();tx_cnt++)
	    for(int rx_ant=0; rx_ant<nRxAntenna; rx_ant++)
	      unprecodedChannel(tone_cnt)(rx_ant,tx_cnt)=myRbChannel(tx_cnt)(rx_ant)(unprecodedTones(tone_cnt));
	}
      }
    } 
  }
  
  Array< cmat > finalChannel; 
  if(dataBlkInfo.dataGenInfo.transmissionScheme != _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
  {
    Array<cmat> allChannel(allTones.length());
    setInArray(allChannel,precodedTones,precodedChannel);
    setInArray(allChannel,unprecodedTones,unprecodedChannel);
    finalChannel=getFromArray(allChannel,dataTones);
    
  }
  else
  {
    finalChannel.set_length(dataTones.length()/2); //For 4 layers this will be rearranged in the end
    int tempNLayers=2; //To use the code common for 2/4 layers
    Array<ivec> dataTonePairs=splitVectorToArray(dataTones,tempNLayers);
    Array<cmat> outputChannel(dataTonePairs.length());
    for(int cnt=0;cnt<dataTonePairs.length();cnt++)
    {
      int pcindx1=find(precodedTones,dataTonePairs(cnt)(0));
      int pcindx2=find(precodedTones,dataTonePairs(cnt)(1));
      if(pcindx1!=-1 && pcindx2!=-1 && pcindx1%2==0 && pcindx2%2==1) // the subcarriers are SFBC pairs
	outputChannel(cnt)=precodedChannel(pcindx1/2);
      else // the subcarriers are not SFBC pairs
      {	
	outputChannel(cnt)=zeros_c(nRxAntenna*tempNLayers,tempNLayers*tempNLayers);
	int uncindx1=find(unprecodedTones,dataTonePairs(cnt)(0));
	int uncindx2=find(unprecodedTones,dataTonePairs(cnt)(1));
	if(uncindx1==-1 && uncindx2!=-1)
	{
	  if(pcindx1%2==0) //sc1 of sfbc pair
	    outputChannel(cnt).set_submatrix(0,0,precodedChannel(pcindx1/2).get_rows(0,nRxAntenna-1)); //-ve sign doesn't matter for GG`
	    else //sc2 of sfbc pair
	    {
	      cmat temp=precodedChannel(pcindx1/2).get_rows(nRxAntenna,nRxAntenna*tempNLayers-1);
	      outputChannel(cnt).set_submatrix(0,0,conj(temp)); //conj() for compensation and col swaping not required
	    } 
	    outputChannel(cnt).set_submatrix(nRxAntenna,tempNLayers,conj(unprecodedChannel(uncindx2)));  
	}
	else if(uncindx1!=-1 && uncindx2==-1)
	{
	  outputChannel(cnt).set_submatrix(0,0,unprecodedChannel(uncindx1));  
	  
	  if(pcindx2%2==0) //sc1 of sfbc pair
	  {	
	    cmat temp=precodedChannel(pcindx2/2).get_rows(0,nRxAntenna-1);
	    outputChannel(cnt).set_submatrix(nRxAntenna,tempNLayers,conj(temp));
	  }
	  else //sc2 of sfbc pair
	    outputChannel(cnt).set_submatrix(nRxAntenna,tempNLayers,precodedChannel(pcindx2/2).get_rows(nRxAntenna,nRxAntenna*tempNLayers-1)); //col swaping not required
	    
	}
	else	//(uncindx1==-1 && uncindx2==-1) 
	{
	  cmat temp=precodedChannel(pcindx1/2).get_rows(nRxAntenna,nRxAntenna*tempNLayers-1);
	  outputChannel(cnt).set_submatrix(0,0,conj(temp));
	  temp=precodedChannel(pcindx2/2).get_rows(0,nRxAntenna-1);
	  outputChannel(cnt).set_submatrix(nRxAntenna,tempNLayers,conj(temp));  
	}
	
      }
    }
    if(nLayers==4)
    {
      finalChannel.set_length(dataTones.length()/4);
      for(int cnt=0;cnt<dataTones.length()/4;cnt++)
	finalChannel(cnt)=formBlockMatrix(outputChannel(2*cnt),outputChannel(2*cnt+1));
    }
    else
      finalChannel=outputChannel;
  }
  
  
  return finalChannel;
}



Array< cmat >  precodeIntrRBChannelOverRS(Array< Array<cmat> > &myRbChannelOverAntennas, PortToAntennaMapper_S *portToAntennaMapper, int rbNumber, DataBlkInfo_S &dataBlkInfo,ivec rsTones)
{
  int nTxAntenna, nRxAntenna, nLayers, nPorts,nCRSPorts, precindx;
  nTxAntenna=myRbChannelOverAntennas.length();
  nRxAntenna=myRbChannelOverAntennas(0).length();

  nLayers=dataBlkInfo.dataGenInfo.nLayers;

  nPorts=dataBlkInfo.dataGenInfo.rbInfo.rsInfo.nCQIports;
  nCRSPorts=dataBlkInfo.dataGenInfo.rbInfo.rsInfo.crsPorts.length();

  cmat precoder;
  cmat largeDelayCDDmatrix;
  Array<cmat> largedelaycodebook;
  imat rbStructure=dataBlkInfo.dataGenInfo.rbInfo.logicalSubFrameStructure.get_rows(rbNumber*12,rbNumber*12+11);
  ivec allTones=getIntegers(0,rbStructure.size()-1);
  ivec precodedTones(0),unprecodedTones(0);
  ivec portNumber(0);
  ivec unprecodedPorts;
  
  cmat portToAntMapper;
  Array< Array<cmat> > myRbChannel;
  if(dataBlkInfo.dataGenInfo.transmissionScheme < 6)
  {
    portToAntMapper = portToAntennaMapper->getCRSPortToAntennaMapper(nCRSPorts,nTxAntenna);
    myRbChannel.set_length(nCRSPorts);
  }
  else
  {
    portToAntMapper = portToAntennaMapper->getPortToAntennaMapper(nPorts,nTxAntenna);
    myRbChannel.set_length(nPorts);
  }


  for(int port_cnt=0;port_cnt<myRbChannel.length();port_cnt++)
  {
    myRbChannel(port_cnt).set_length(nRxAntenna);
    for(int cnt=0;cnt<myRbChannel(port_cnt).length();cnt++)
      myRbChannel(port_cnt)(cnt).set_size(myRbChannelOverAntennas(0)(0).rows(),myRbChannelOverAntennas(0)(0).cols());
  }
  for(int i=0;i<allTones.length();i++)
  {
    cmat tempChannel=getAcrossArray(myRbChannelOverAntennas,i)*portToAntMapper;
    setAcrossArray(myRbChannel,i,tempChannel);
  }
  
  if(dataBlkInfo.dataGenInfo.transmissionScheme != _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
  {
    
    if(dataBlkInfo.dataGenInfo.transmissionScheme < 8)
      unprecodedPorts=dataBlkInfo.dataGenInfo.rbInfo.rsInfo.crsPorts;
    else
      unprecodedPorts=remove(dataBlkInfo.dataGenInfo.rbInfo.rsInfo.rsPorts,dataBlkInfo.dataGenInfo.rbInfo.rsInfo.DeModPorts);
    
    unprecodedPorts=findUniqueNumbers(unprecodedPorts);
    ivec portRefNumbers=dataBlkInfo.dataGenInfo.rbInfo.getPortReferenceNumbers(unprecodedPorts);
    for(int port_cnt=0;port_cnt<unprecodedPorts.length();port_cnt++)
    {
      ivec  temp=find(rbStructure,portRefNumbers(port_cnt),true);
      append(unprecodedTones,temp);
      append(portNumber,ones_i(temp.length())*unprecodedPorts(port_cnt));
    }
    
    //!Adding control signal to unprecodedChannel - 10010 
    ivec isCCH(rbStructure.size());
    for(int c=0,totalC=0;c<rbStructure.cols();c++)
      for(int r=0;r<rbStructure.rows();r++,totalC++)
	isCCH(totalC)=(rbStructure(r,c)>=50);
      
    ivec cchTones=find(isCCH,1,true);
    if(cchTones.length())
    {
      append(unprecodedTones,cchTones);
      ivec temp=zeros_i(cchTones.length())-1;
      append(portNumber,temp);
    }
    
    //!///////////////////////////////////////////////////
    //!Adding control signal to unprecodedChannel - 10010 
    ivec isblank(rbStructure.size());
    for(int c=0,totalC=0;c<rbStructure.cols();c++)
      for(int r=0;r<rbStructure.rows();r++,totalC++)
	isblank(totalC)=(rbStructure(r,c)==-1);
      
    ivec blankTones=find(isblank,1,true);
    if(blankTones.length())
    {
      append(unprecodedTones,blankTones);
      ivec temp=zeros_i(blankTones.length())-2;
      append(portNumber,temp);
    }
    
    //!///////////////////////////////////////////////////
    
    ivec sIndx=sort_index(unprecodedTones);
    sort(unprecodedTones);
    portNumber=portNumber(sIndx);
    precodedTones=find(rbStructure,0,true);
    sort(precodedTones);
  }
  else
  {
    //! considering rs tones as precoded for SFBC
    precodedTones=rsTones;
  }
  
  Array< cmat > precodedChannel;
  Array< cmat > unprecodedChannel;
  
  
  //if(precodedTones.length()!=0)
  {  
    
    Array< cmat > channel(precodedTones.length());
    
    for(int i=0;i<precodedTones.length();i++)
      channel(i)=getAcrossArray(myRbChannel,precodedTones(i));
    
    
    if(dataBlkInfo.dataGenInfo.transmissionScheme == _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_)
    {
      //       ivec reIndxPerRB=dataBlkInfo.reIndxForPrecoding(find(dataBlkInfo.resourceBlocks,rbNumber));
      precodedChannel.set_length(precodedTones.length());
      switch(nPorts)
      {
	case 2:
	  
	  largedelaycodebook = generateCodebookForDLTwoPorts(2);  
	  for(int i=0;i<precodedTones.length();i++)
	  {
	    largeDelayCDDmatrix = getLargeDelayCDDmatrixD(/*reIndxPerRB*/(i),nLayers) * getLargeDelayCDDmatrixU(nLayers);
	    precoder =  largedelaycodebook(0)*largeDelayCDDmatrix;
	    precodedChannel(i)=channel(i)*precoder;
	  }
	  break;
	case 4:
	  
	  largedelaycodebook = generateCodebookForDLFourPorts(nLayers);
	  for(int i=0;i<precodedTones.length();i++)
	  {
	    largeDelayCDDmatrix = getLargeDelayCDDmatrixD(/*reIndxPerRB*/(i),nLayers) * getLargeDelayCDDmatrixU(nLayers);
	    precindx = mod(floor(((double)/*reIndxPerRB*/(i))/(double)nLayers),4.0)+12.0;
	    precoder =  largedelaycodebook(precindx) * largeDelayCDDmatrix;
	    precodedChannel(i)=channel(i)*precoder;
	  }
	  break;
	default:
	{cout<<"[both:] Undefined nPorts ... exiting in getPrecoder()..."<<endl;abort();}
      }
      
    }
    
    else if(dataBlkInfo.dataGenInfo.transmissionScheme == _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
    {
      precodedChannel.set_length(precodedTones.length());
      for(int i=0;i<precodedTones.length();i++)
      {
	precodedChannel(i) = zeros_c(nRxAntenna*nLayers,nLayers);
	
	for(int rxant=0;rxant<nRxAntenna;rxant++)
	{
	  if(nLayers==2)
	  {
	    // arranging the channel matrix in SFBC matrix form 
	    precodedChannel(i)(rxant,0) = sqrt(1/2.0)*channel(i)(rxant,0);
	    precodedChannel(i)(rxant,1) =  sqrt(1/2.0)*(-channel(i)(rxant,1));
	    precodedChannel(i)(rxant+nRxAntenna,0) = sqrt(1/2.0)*conj(channel(i)(rxant,1));
	    precodedChannel(i)(rxant+nRxAntenna,1) = sqrt(1/2.0)*conj(channel(i)(rxant,0));
	  }
	  else //nLayers==4
	  {
	    precodedChannel(i)(rxant,0) = sqrt(1/2.0)*channel(i)(rxant,0);
	    precodedChannel(i)(rxant,1) = sqrt(1/2.0)*(-channel(i)(rxant,2));
	    precodedChannel(i)(rxant+nRxAntenna,0) = sqrt(1/2.0)*conj(channel(i)(rxant,2));
	    precodedChannel(i)(rxant+nRxAntenna,1) = sqrt(1/2.0)*conj(channel(i)(rxant,0));
	    
	    precodedChannel(i)(rxant+2*nRxAntenna,2) = sqrt(1/2.0)*channel(i)(rxant,1);
	    precodedChannel(i)(rxant+2*nRxAntenna,3) = sqrt(1/2.0)*(-channel(i)(rxant,3));
	    precodedChannel(i)(rxant+3*nRxAntenna,2) = sqrt(1/2.0)*conj(channel(i)(rxant,3));
	    precodedChannel(i)(rxant+3*nRxAntenna,3) = sqrt(1/2.0)*conj(channel(i)(rxant,1));
	    
	  } 
	}
      }
    }
    //RB level cycling
    else if(dataBlkInfo.dataGenInfo.transmissionScheme == _TRANSMISSION_SCHEME_OLMUMIMO_)
    {
      precodedChannel.set_size(channel.length());
      
      if(dataBlkInfo.isSemiOpenLoopScheme)
      {
	precoder = getPrecoder(dataBlkInfo, NULL,nTxAntenna,dataBlkInfo.dataGenInfo.transmissionScheme, rbNumber);//including port to antenna mapping
      }
      else
      {
	cout<<"Only SemiOpenLoopScheme supported"<<endl;abort();
      }
      for(int i=0;i<precodedTones.length();i++)
      {
	precodedChannel(i) = channel(i)*precoder;
      }
    }
    else
    {
      precodedChannel.set_size(channel.length());
      
      precoder = getPrecoder(dataBlkInfo, NULL,nTxAntenna,dataBlkInfo.dataGenInfo.transmissionScheme, rbNumber);//including port to antenna mapping
      for(int i=0;i<precodedTones.length();i++)
	precodedChannel(i)=channel(i)*precoder;
    }
    
  }
  
  //   if(unprecodedTones.length()!=0)
  {
    unprecodedChannel.set_length(unprecodedTones.length());
    //     if(dataBlkInfo.dataGenInfo.rbInfo.rsInfo.CQIrsType==_RSTYPE_CSIRS_)
    //     {
    //       cout<<"CSI RS handling not yet done in Precoder."<<endl;
    //       abort();
    //     }
    //     cout<<portNumber<<unprecodedTones<<rbStructure<<unprecodedTones<<endl;
    for(int tone_cnt=0;tone_cnt<unprecodedTones.length();tone_cnt++)
      unprecodedChannel(tone_cnt)=zeros_c(nRxAntenna,nPorts);
    
    for(int tone_cnt=0;tone_cnt<unprecodedTones.length();tone_cnt++)
      for(int rx_ant=0; rx_ant<nRxAntenna; rx_ant++)
	if(portNumber(tone_cnt)>=0 && portNumber(tone_cnt)<4) //CRS
	  unprecodedChannel(tone_cnt)(rx_ant,portNumber(tone_cnt))=myRbChannel(portNumber(tone_cnt))(rx_ant)(unprecodedTones(tone_cnt));
	else if(portNumber(tone_cnt)>=15 && portNumber(tone_cnt)<23) //CSI-RS
	  unprecodedChannel(tone_cnt)(rx_ant,portNumber(tone_cnt)-15)=myRbChannel(portNumber(tone_cnt)-15)(rx_ant)(unprecodedTones(tone_cnt));
	else
	{
	    // power scaling is not correct for control channels.. Need to fix 
	  if(portNumber(tone_cnt)==-1) //non blank tone
	    for(int tx_cnt=0;tx_cnt<unprecodedChannel(tone_cnt).cols();tx_cnt++)
	      unprecodedChannel(tone_cnt)(rx_ant,tx_cnt)=myRbChannel(tx_cnt)(rx_ant)(unprecodedTones(tone_cnt));
	}
  }
  
  if(dataBlkInfo.dataGenInfo.rbInfo.rsInfo.CQIrsType==_RSTYPE_CSIRS_)
  {
    ivec uniqueUnPrecodedTones = findUniqueNumbers(unprecodedTones);
    Array<cmat> uniqueUnPrecodedChannel(uniqueUnPrecodedTones.length());
    for(int cnt=0;cnt<uniqueUnPrecodedTones.length();cnt++)
    {
      ivec ToneIndices = find(unprecodedTones,uniqueUnPrecodedTones(cnt),true);
      uniqueUnPrecodedChannel(cnt) = unprecodedChannel(ToneIndices(0));
      if(ToneIndices.length()>1)
      {
	for(int indx = 1;indx<ToneIndices.length();indx++)
	  uniqueUnPrecodedChannel(cnt) +=  unprecodedChannel(ToneIndices(indx));
	
	uniqueUnPrecodedChannel(cnt) = (1.0/sqrt(ToneIndices.length()))*uniqueUnPrecodedChannel(cnt);
      }
    }
    unprecodedTones = uniqueUnPrecodedTones;
    unprecodedChannel = uniqueUnPrecodedChannel;
  }
  
  Array< cmat > finalChannel(rsTones.length()); 
  if(dataBlkInfo.dataGenInfo.transmissionScheme != _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
  {
    ivec precodedIndx=find(precodedTones,rsTones);
    precodedIndx=remove(precodedIndx,-1);
    Array<cmat> allChannel(allTones.length());
    setInArray(allChannel,precodedTones,precodedChannel);
    setInArray(allChannel,unprecodedTones,unprecodedChannel);
    finalChannel=getFromArray(allChannel,rsTones);

  }
  else
  {
    //     for(int rs_cnt=0;rs_cnt<rsTones.length();rs_cnt++)
    //     {
    //       int precodedIndx=find(precodedTones,rsTones(rs_cnt));
    //       if(precodedIndx==-1)
    //       {
    // 	int unprecodedIndx=find(unprecodedTones,rsTones(rs_cnt));
    // 	cmat temp=conj(unprecodedChannel(unprecodedIndx));
    // 	finalChannel(rs_cnt)=formBlockMatrix(unprecodedChannel(unprecodedIndx),temp);
    // 	if(nLayers==4)
    // 	  finalChannel(rs_cnt)=formBlockMatrix(finalChannel(rs_cnt),finalChannel(rs_cnt));
    //       }
    //       else
    // 	finalChannel(rs_cnt)=precodedChannel(precodedIndx);
    //       
    //     }
    
    finalChannel=precodedChannel;
  }
  return finalChannel;
}

/*! \fn precodeMyRBChannelForCQI(...)
 * \brief multiplies the channel with the appropriate precoder in CQI computation
 * \param [in] myRbChannel ---> contains the raw channel of an RB
 * \param [in] rbNumber ---> resource block number
 * \param [in] cqiComputationInfo ---> gives the number of CQI ports
 * \param [in] isCloseLoop ---> bool to decided wether closed loop or open loop
 * \param [in] serverNodeGroupIndex ---> provides the set of eNodeBs from which UE gets good signal strength 
 * \param [in] dataTones ---> number of data tones in an RB 
 * \param [in] precoderForClosedLoop ---> gives the precoder for closed loop schemes
 * \return precodedChannel
 */
Array< cmat >  precodeMyRBChannelForCQI(Array< Array<cmat> > &myRbChannel, PortToAntennaMapper_S *portToAntennaMapper, int rbNumber, TransmissionScheme_E txScheme, int nLayers, int nPorts,bool isCloseLoop,int pmi_i1, ivec dataTones,cmat precoderForClosedLoop)
{
  int nTxAntenna, nRxAntenna, precindx;
  nTxAntenna=myRbChannel.length();
  nRxAntenna=myRbChannel(0).length();

  cmat precoder;
  cmat largeDelayCDDmatrix;
  Array<cmat> largedelaycodebook;

  if(dataTones.length()==0)
    dataTones=getIntegers(0,myRbChannel(0)(0).size()-1);

  cmat portToAntMapper;
  if(txScheme < 6)
    portToAntMapper = portToAntennaMapper->getCRSPortToAntennaMapper(nPorts,nTxAntenna);
  else
    portToAntMapper = portToAntennaMapper->getPortToAntennaMapper(nPorts,nTxAntenna);

  Array< cmat > channel(dataTones.length());
  for(int tone_cnt=0;tone_cnt<dataTones.length();tone_cnt++)
    channel(tone_cnt)=getAcrossArray(myRbChannel,dataTones(tone_cnt))*portToAntMapper;
     
  Array< cmat > precodedChannel(dataTones.length());

  if(txScheme == _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_)
  {
    switch(nPorts)
    {
      case 2:

	largedelaycodebook = generateCodebookForDLTwoPorts(2);
	for(int i=0;i<dataTones.length();i++)
	{
	  largeDelayCDDmatrix = getLargeDelayCDDmatrixD(i,nLayers) * getLargeDelayCDDmatrixU(nLayers);
	  precoder =  largedelaycodebook(0)*largeDelayCDDmatrix;
	  precodedChannel(i)=channel(i)*precoder;
	}
	break;
      case 4:

	largedelaycodebook = generateCodebookForDLFourPorts(nLayers);
	for(int i=0;i<dataTones.length();i++)
	{
	  largeDelayCDDmatrix = getLargeDelayCDDmatrixD(i,nLayers) * getLargeDelayCDDmatrixU(nLayers);
	  precindx = mod(floor(((double)i)/(double)nLayers),4.0)+12.0;
	  precoder =  largedelaycodebook(precindx) * largeDelayCDDmatrix;
	  precodedChannel(i)=channel(i)*precoder;
	}
	break;
      default:
      {cout<<"[both:] Undefined nPorts ... exiting in getPrecoder()..."<<endl;abort();}
    }

  }

  else if(txScheme == _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_)
  {

    for(int i=0;i<precodedChannel.length();i++)
    {
      precodedChannel(i)=zeros_c(nRxAntenna*nLayers,nLayers);
      for(int rxant=0;rxant<nRxAntenna;rxant++)
      {
	if(nLayers==2)
	{
	  // arranging the channel matrix in SFBC matrix form
	  precodedChannel(i)(rxant,0) = sqrt(1/2.0)*channel(i)(rxant,0);
	  precodedChannel(i)(rxant,1) =  sqrt(1/2.0)*(-channel(i)(rxant,1));
	  precodedChannel(i)(rxant+nRxAntenna,0) = sqrt(1/2.0)*conj(channel(i)(rxant,1));
	  precodedChannel(i)(rxant+nRxAntenna,1) = sqrt(1/2.0)*conj(channel(i)(rxant,0));
	}
	else //nLayers==4
	{
	  precodedChannel(i)(rxant,0) = sqrt(1/2.0)*channel(i)(rxant,0);
	  precodedChannel(i)(rxant,1) = sqrt(1/2.0)*(-channel(i)(rxant,2));
	  precodedChannel(i)(rxant+nRxAntenna,0) = sqrt(1/2.0)*conj(channel(i)(rxant,2));
	  precodedChannel(i)(rxant+nRxAntenna,1) = sqrt(1/2.0)*conj(channel(i)(rxant,0));

	  precodedChannel(i)(rxant+2*nRxAntenna,2) = sqrt(1/2.0)*channel(i)(rxant,1);
	  precodedChannel(i)(rxant+2*nRxAntenna,3) = sqrt(1/2.0)*(-channel(i)(rxant,3));
	  precodedChannel(i)(rxant+3*nRxAntenna,2) = sqrt(1/2.0)*conj(channel(i)(rxant,3));
	  precodedChannel(i)(rxant+3*nRxAntenna,3) = sqrt(1/2.0)*conj(channel(i)(rxant,1));
	}
      }
    }
  }
  // RB level cycling
  else if(txScheme == _TRANSMISSION_SCHEME_OLMUMIMO_)
  {
    //SemiOpenLoopScheme
    
    Array<cmat> codebook = generateCodebookForDLEightPorts(nLayers);
    if(pmi_i1 != -1 && precoderForClosedLoop.size()==0)
    {
      precoder = codebook(pmi_i1*16 + (rbNumber%4));
    }
    else if(pmi_i1 == -2 && precoderForClosedLoop.size()>0)
    {
      precoder = precoderForClosedLoop;
    }
    else
    {
      cout<<"Not supported "<<endl;abort();
    }
    for(int i=0;i<dataTones.length();i++)
    {
      precodedChannel(i) = channel(i)*precoder;
    }
  }
  else
  {
    if(isCloseLoop && precoderForClosedLoop.size()>0)
    {
      for(int i=0;i<dataTones.length();i++)
	precodedChannel(i) = channel(i)*precoderForClosedLoop;
    }
    else
      precodedChannel = channel;

  }
  return precodedChannel;
}

Array< cmat >  precodeChannelForCQI(Array<cmat> &rawChannel, PortToAntennaMapper_S *portToAntennaMapper,TransmissionScheme_E txScheme, int nPorts,cmat precoderForClosedLoop)
{
  int nTxAntenna = rawChannel(0).cols();

  cmat portToAntMapper;
  if(txScheme < 6)
    portToAntMapper = portToAntennaMapper->getCRSPortToAntennaMapper(nPorts,nTxAntenna);
  else
    portToAntMapper = portToAntennaMapper->getPortToAntennaMapper(nPorts,nTxAntenna);

  Array< cmat > precodedChannel(rawChannel.length());
  for(int tone_cnt=0;tone_cnt<rawChannel.length();tone_cnt++)
    precodedChannel(tone_cnt)=(rawChannel(tone_cnt)*portToAntMapper)*precoderForClosedLoop;

  return precodedChannel;
}




/*! \fn getPrecoder(...)
 * \brief gives the appropriate precoder for all Transmission Modes
 * \param [in] dataBlkInfo ---> provides the number of layers and CQI ports
 * \param [in] nTxAntenna ---> number of transmit antennas
 * \param [in] rbNumber ---> resource block number
 * \param [in] txScheme ---> Transmission Scheme
 * \param [in] rbNumber ---> Resource block number in freq domain
 * \return precoder
 */
cmat getPrecoder(DataBlkInfo_S &dataBlkInfo,PortToAntennaMapper_S *portToAntennaMapper, int nTxAntenna , TransmissionScheme_E txScheme, int rbNumber)
{
  int nLayers, nPorts, nCRSPorts;
  nLayers=dataBlkInfo.dataGenInfo.nLayers;
  nPorts=dataBlkInfo.dataGenInfo.rbInfo.rsInfo.nCQIports;
  nCRSPorts=dataBlkInfo.dataGenInfo.rbInfo.rsInfo.crsPorts.length();
  cmat precoder = ones_c(nPorts, nLayers);
  
  switch(txScheme)
  {
    case _TRANSMISSION_SCHEME_SINGLE_PORT_CRS_:
    case _TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_:
      precoder = ones_c(1,1);
      break;
    case _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_:
      precoder = eye_c(nPorts);
      break;
    case _TRANSMISSION_SCHEME_CL_SM_:
    case _TRANSMISSION_SCHEME_CL_MU_MIMO_:
    case _TRANSMISSION_SCHEME_CL_BF_:
    case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
    case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
    case _TRANSMISSION_SCHEME_COMP_:
    case _TRANSMISSION_SCHEME_OLMUMIMO_:
    case _TRANSMISSION_SCHEME_NR_CL_:
    {
      precoder = dataBlkInfo.getMyPrecoder(rbNumber);
    }
    break;
    default:
    {cout<<"[both:] Undefined txnScheme ... exiting in getPrecoder()..."<<endl;abort();}
  }
  
  if(portToAntennaMapper!=NULL)
  {
    cmat mapper;
    if(txScheme < 6)
      mapper = portToAntennaMapper->getCRSPortToAntennaMapper(nCRSPorts, nTxAntenna);
    else
      mapper = portToAntennaMapper->getPortToAntennaMapper(nPorts, nTxAntenna);

    return(mapper*precoder);
  }
  else
    return precoder;
}

/*! \fn getPrecoder(...)
 * \brief gives the appropriate precoder in CQI computation
 * \param [in] cqiComputationInfo ---> provides the number of layers and CQI ports
 * \param [in] nTxAntenna ---> number of transmit antennas
 * \param [in] rbNumber ---> resource block number
 * \param [in] txScheme ---> Transmission Scheme
 * \param [in] rbNumber ---> Resource block number in freq domain
 * \return precoder
 */
cmat getPrecoder(PortToAntennaMapper_S *portToAntennaMapper,int nTxAntenna,int nLayers,int nPorts, TransmissionScheme_E txScheme)
{
  cmat precoder(nPorts, nLayers);
  
  switch(txScheme)
  {
    case _TRANSMISSION_SCHEME_SINGLE_PORT_CRS_:
    case _TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_:
      precoder = ones_c(1,1);
      break;
    case _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_:
    case _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_:
    case _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_:
    case _TRANSMISSION_SCHEME_NR_CL_:
      precoder = eye_c(nPorts);
      break;
      
    default:
    {cout<<"[both:] Undefined txnScheme ... exiting in getPrecoder()..."<<endl;abort();}
  }
  
  
  if(portToAntennaMapper!=NULL)
  return(portToAntennaMapper->getPortToAntennaMapper(nPorts, nTxAntenna)*precoder);
  else
    return precoder;
}


/*! \fn getEffectiveChanForTxDiv(...)
 * \brief forms the effective channel for SFBC
 * \param [in] precodedChanPerRxAnt ---> precoded channel w.r.t single receiving antenna
 * \param [in] nLayers ---> number of transmit layers
 * \return effChanPerRxAnt
 */
cmat getEffectiveChanForTxDiv(cvec precodedChanPerRxAnt, int nLayers)
{
  cmat effChanPerRxAnt(nLayers, nLayers);
  effChanPerRxAnt.clear();
  
  switch(nLayers)
  {
    case 2:
      effChanPerRxAnt(0,0) = sqrt(1/2.0)*precodedChanPerRxAnt(0);
      effChanPerRxAnt(0,1) = -sqrt(1/2.0)*precodedChanPerRxAnt(1);
      effChanPerRxAnt(1,0) = sqrt(1/2.0)*conj(precodedChanPerRxAnt(1));
      effChanPerRxAnt(1,1) = sqrt(1/2.0)*conj(precodedChanPerRxAnt(0));
      break;
    case 4:
      effChanPerRxAnt(0,0) = sqrt(1/2.0)*precodedChanPerRxAnt(0);
      effChanPerRxAnt(0,1) = -sqrt(1/2.0)*precodedChanPerRxAnt(2);
      effChanPerRxAnt(1,0) = sqrt(1/2.0)*conj(precodedChanPerRxAnt(2));
      effChanPerRxAnt(1,1) = sqrt(1/2.0)*conj(precodedChanPerRxAnt(0));
      effChanPerRxAnt(2,2) = sqrt(1/2.0)*precodedChanPerRxAnt(1);
      effChanPerRxAnt(2,3) = -sqrt(1/2.0)*precodedChanPerRxAnt(3);
      effChanPerRxAnt(3,2) = sqrt(1/2.0)*conj(precodedChanPerRxAnt(3));
      effChanPerRxAnt(3,3) = sqrt(1/2.0)*conj(precodedChanPerRxAnt(1));
      break;
    default:
      cout<<"[both:] Invalid nLayers ... exiting in txDivDecoder()"<<endl;abort();
  }
  
  return effChanPerRxAnt;
}

/*! \fn getSymbolRepetitionFactor(...)
 * \brief number of times the symbols are repeating incase of SFBC
 * \param [in] txnScheme ---> transmission scheme
 * \param [in] nLayers ---> number of transmit layers
 * \return repetition factor 
 */


Array<cvec> performPrecodingforControlChannels(Array<cvec> &layeredData,int &nLayers)
{
  if(nLayers==1)
    return layeredData;
  else {
    
    int nPorts=nLayers;
    cmat precoder=getSFBCprecoder(nPorts);
    cmat layervec, precodedlayervec;
    Array<cvec> precodedLayers(nPorts);
    int nSymbolsmappedRB=nLayers*layeredData(0).length();
    int nSymbolsmappedPerLayer = nSymbolsmappedRB/nLayers;
    
    layervec.set_size(nLayers*2,1);
    precodedlayervec.set_size(nPorts*nPorts,1);
    
    for (int j=0; j<nPorts; j++)
      precodedLayers(j).set_size(nPorts*nSymbolsmappedPerLayer);
    
    for (int i=0; i<nSymbolsmappedPerLayer; i++)
    {
      for (int j=0; j<nLayers; j++)
      {
	layervec(j,0) = layeredData(j)[i].real();
	layervec(j+nLayers,0) = layeredData(j)[i].imag();
      }
      precodedlayervec = precoder*layervec;
      for (int j=0; j<nPorts; j++)
	for (int k=0; k<nPorts; k++)
	  precodedLayers(j)[nPorts*i+k] = precodedlayervec(nPorts*k+mod(j,nPorts),0);
    }
    return precodedLayers;
  }
}


