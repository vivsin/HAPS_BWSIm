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

#ifndef _PRECODER_H_
#define _PRECODER_H_

#include "StructsAndEnums.h"
#include "Codebook.h"

struct PortToAntennaMapper_S{

  Array<cmat> portToAntennaMapper;
  imat mapperIndex;

  PortToAntennaMapper_S(){

    portToAntennaMapper.set_length(0);
    mapperIndex.set_size(0,0);
  }
  void init(string fileName){

    int maxPorts,maxAntennas;
    string parseName;
    cmat temp;
    parse(fileName,"maxPorts",maxPorts);
    parse(fileName,"maxAntennas",maxAntennas);
    mapperIndex=zeros_i(maxPorts+1,maxAntennas+1)-1;
    portToAntennaMapper.set_length(0);

    for(int port_cnt=1;port_cnt<=maxPorts;port_cnt++)
      for(int ant_cnt=1;ant_cnt<=maxPorts;ant_cnt++)
      {
	parseName="map"+toString(port_cnt)+"to"+toString(ant_cnt);
	if(parse(fileName,parseName,temp))
	{
	  mapperIndex(port_cnt,ant_cnt)=portToAntennaMapper.length();
	  append(portToAntennaMapper,temp);
	}
      }
  }
//   cmat getPortToAntennaMapper(int nDataPorts, int nPhysicalAntennas){
//     cout<<"mapperIndex.cols() : "<<mapperIndex.cols()<<endl;
//     if(nDataPorts<mapperIndex.rows() && nPhysicalAntennas<mapperIndex.cols())
//     {
//       if(mapperIndex(nDataPorts,nPhysicalAntennas)!=-1)
// 	return(portToAntennaMapper(mapperIndex(nDataPorts,nPhysicalAntennas)));
//       else
//       {
// 	cout<<"[both:] nDataPorts = "<<nDataPorts<<" nPhysicalAntennas = "<<nPhysicalAntennas<<endl;
// 	cout<<"[both:] Unsupported nDataPorts-nPhysicalAntennas combination to getPortToAntennaMapper()..."<<endl;
// 	abort();
//       }
//     }
//     else
//     {
//       cout<<"[both:] nDataPorts = "<<nDataPorts<<" nPhysicalAntennas = "<<nPhysicalAntennas<<endl;
//       cout<<"[both:] Unsupported nDataPorts-nPhysicalAntennas combination to getPortToAntennaMapper()..."<<endl;
//       abort();
//     }
//   }
  cmat getPortToAntennaMapper(int nDataPorts, int nPhysicalAntennas){

    if(nDataPorts >= 1 && nDataPorts <= 32)
    {
      cmat toReturn = zeros_c(nPhysicalAntennas,nDataPorts);
      if(nPhysicalAntennas==nDataPorts)
      {
	for(int i = 0; i < nDataPorts; i++)
      {
	toReturn(i,i) = complex< double >(1,0);
      }
      
      }
      else if(nPhysicalAntennas!=nDataPorts)
      { for(int i = 0; i < nDataPorts; i++)
	{
	for(int j=0; j<nPhysicalAntennas/nDataPorts;j++)
	{  
	   toReturn(i*(nPhysicalAntennas/nDataPorts)+j,i) = complex< double >(1,0);
	   if((j==(nPhysicalAntennas/nDataPorts-1)) && (i<nPhysicalAntennas%nDataPorts))
	   {toReturn(i*(nPhysicalAntennas/nDataPorts)+j+1,i) = complex< double >(1,0);
	   }
	}
	}
      }
      return toReturn;

    }
    else
    {
      cout<<"[both:] nCSIRSPorts = "<<nDataPorts<<" nPhysicalAntennas = "<<nPhysicalAntennas<<endl;
      cout<<"[both:] Unsupported nCSIRSPorts... Aborted in getPortToAntennaMapper()..."<<endl;
      abort();
    }
  }
  
  cmat getCRSPortToAntennaMapper(int nCRSPorts, int nPhysicalAntennas){

    if(nCRSPorts == 1 || nCRSPorts == 2 || nCRSPorts == 4)
    {
      cmat toReturn = zeros_c(nPhysicalAntennas,nCRSPorts);

      for(int i = 0; i < nCRSPorts; i++)
      {
	toReturn(i,i) = complex< double >(1,0);
      }

      return toReturn;

    }
    else
    {
      cout<<"[both:] nCRSPorts = "<<nCRSPorts<<" nPhysicalAntennas = "<<nPhysicalAntennas<<endl;
      cout<<"[both:] Unsupported nCRSPorts... Aborted in getCRSPortToAntennaMapper()..."<<endl;
      abort();
    }
  }

  Array<cvec> mapPortsToPhysicalAntennas(Array<cvec> precodedLayersPerDataPort, int nPhysicalAntennas,bool isCRSMode){

    Array<cvec> precodedLayersAcrossPhysicalAntennas(nPhysicalAntennas);
    cmat portToAntennaMapper;
    int nDataPorts=precodedLayersPerDataPort.length();
    if(isCRSMode)
      portToAntennaMapper = getCRSPortToAntennaMapper(nDataPorts, nPhysicalAntennas);
    else
      portToAntennaMapper = getPortToAntennaMapper(nDataPorts, nPhysicalAntennas);
    int vecLength = precodedLayersPerDataPort(0).length();///SK: Assuming all precoded layers has same cvec length
    cvec tempVec(nDataPorts), txdata(nPhysicalAntennas);
    for (int i=0; i<nPhysicalAntennas; i++)
      precodedLayersAcrossPhysicalAntennas(i)=zeros_c(vecLength);
    complex< double > czero=complex< double >(0,0);
    for (int p=0; p<nDataPorts; p++)
      for (int a=0; a<nPhysicalAntennas; a++)
      if(portToAntennaMapper(a,p)!=czero)	
      {
	precodedLayersAcrossPhysicalAntennas(a) += portToAntennaMapper(a,p)*precodedLayersPerDataPort(p);
      }

    return precodedLayersAcrossPhysicalAntennas;

  }
};


cmat getLargeDelayCDDmatrixD(int toneIndex, int nLayers);
cmat getLargeDelayCDDmatrixU(int nLayers);
cmat getSFBCprecoder(int nPorts);
Array<cvec> precodeDataInRB(Array<cvec> &modulatedSymbolsPerLayerPerRB , DataBlkInfo_S &dataBlkInfo, int rbNumber);
cmat getPrecoder(DataBlkInfo_S &dataBlkInfo,PortToAntennaMapper_S *portToAntennaMapper, int nTxAntenna, TransmissionScheme_E txScheme, int rbNumber);
cmat getPrecoder(PortToAntennaMapper_S *portToAntennaMapper,int nTxAntenna,int nLayers,int nPorts, TransmissionScheme_E txScheme);
Array< Array<cmat> >  applyPortToAntennaMapperOnMyRBChannel(Array< Array<cmat> > &myRbChannel, PortToAntennaMapper_S *portToAntennaMapper, int nPorts,bool isCRSMode);
Array<cmat> performSFBCPrecoding(Array<cmat>& unprecodedChannel);
Array< cmat >  precodeMyRBChannel(Array< Array< cmat > >& myRbChannel, PortToAntennaMapper_S* portToAntennaMapper, int rbNumber, DataBlkInfo_S& dataBlkInfo, ivec dataTones = "");
Array< cmat >  precodeIntrRBChannel(Array< Array< cmat > >& myRbChannel, PortToAntennaMapper_S* portToAntennaMapper, int rbNumber, DataBlkInfo_S& dataBlkInfo, ivec dataTones );
Array< Array<cvec> >  precodeMyRBChannelOverRS(Array< Array<cmat> > &myRbChannel, PortToAntennaMapper_S *portToAntennaMapper, int rbNumber, DataBlkInfo_S &dataBlkInfo,ivec rsTones);
Array< cmat >  precodeIntrRBChannelOverRS(Array< Array<cmat> > &myRbChannel, PortToAntennaMapper_S *portToAntennaMapper, int rbNumber, DataBlkInfo_S &dataBlkInfo,ivec rsTones);
Array< cmat >  precodeMyRBChannelForCQI(Array< Array<cmat> > &myRbChannel, PortToAntennaMapper_S *portToAntennaMapper, int rbNumber,TransmissionScheme_E txScheme,int nLayers, int nPorts,bool isCloseLoop,int pmi_i1,ivec dataTones="",cmat precoderForClosedLoop="");
Array< cmat >  precodeChannelForCQI(Array< cmat >& rawChannel, PortToAntennaMapper_S* portToAntennaMapper, TransmissionScheme_E txScheme, int nPorts, cmat precoderForClosedLoop);
cmat getEffectiveChanForTxDiv(cvec precodedChanPerRxAnt, int nLayers);

Array< cvec > performPrecodingforControlChannels(Array<cvec>& layeredData,int& nLayers);


#endif

