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


#include "../../include/Terrain.h"



void load3gppTerrain(Terrain &terrain){
  
  Array<string> terrainNames="{ _TERRAIN_3GPP_CASE1_ _TERRAIN_3GPP_CASE3_ }";
  terrain.addTerrain(terrainNames,&getHetNetPathLoss);
}


/*!
*  \fn double het_3gpp_femto_model1(double fc,struct CartCoord bsloc,struct CartCoord ssloc)
*  \brief calculates the path loss using  Model1 given in 3gpp Het Evaluation Methodology 36.814 table A.2.1.1.2-3
* This also sets the St_Dev of  shadowing appropriately, which will be used later while calculating shadowing
*
*  \param bsloc --> location of the base station
*  \param ssloc --> location of the subscriber station
*  \param R     --> Distance between BS/PICO and SS
*  \param L     --> Pathloss
*	The function uses 36-814 model 1 to calculate the pathloss between a base station and
* subscriber station.   This is a simplified model with the BS and Pico heights are fixed as per 3gpp evalution methodology

fc --> carrier frequency is assumed to be 2GHZ

*/


NodeLinkInfo_S getHetNetPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int /*isLOS*/, int /*txNodeType*/, int /*rxNodeType*/, const Terrain * terr,NodeCategory_E /*nodeCategory1*/,NodeCategory_E /*nodeCategory2*/)
{
  if(terr->getTerrain ()=="_TERRAIN_3GPP_CASE1_")
  {
	if((nodeLoc2.z>=25 &&nodeLoc2.z<=32)|| (nodeLoc1.z>=25&& nodeLoc1.z<=32))	//For supporting both Uplink / Downlink //updated base station heights to a range from 25 -32 , where 32 meter comes from 25.814, but there are ambiguities...
	  return het3gppMacroUeModel1(nodeLoc1,nodeLoc2,carrierFreqInHz);
	else if(nodeLoc2.z==5 || nodeLoc1.z==5)
	{
	  //tempLinkInfo.shadowStdDev=stDevPicoMs;//if you want from cFile
	  return het3gppPicoUeModel1(nodeLoc1,nodeLoc2,carrierFreqInHz);
	}
	else
	{
	  cout<<"[both:] Error... Unsupported Node Height for 3gpp Pathloss model.. Supported Heights are BS: 25-32m, Pico/Relay:5m...nodeheights are "<<nodeLoc1.z<<" "<<nodeLoc2.z<<endl;
	  exit(1);
	}
	
  }
  else if(terr->getTerrain ()=="_TERRAIN_3GPP_CASE3_")
  {
	cout<<" [both:] Terrain _TERRAIN_3GPP_CASE3_ is not implemented"<<endl;
	exit(1);
  }
  cout<<" [both:] Unsupported terrain"<<endl;
  abort();
}

NodeLinkInfo_S het3gppPicoUeModel1(Location_S nodeLoc1, Location_S nodeLoc2, double carrierFreqInHz)
{
  
  NodeLinkInfo_S tempLinkInfo;
  tempLinkInfo.isLOS=0;
  
  tempLinkInfo.distance=findDistance(nodeLoc1,nodeLoc2);
  
  
  if(carrierFreqInHz != 2.0e9)
	cout<<"[both:]  Warning.... 3gpp Model 1 is defined for 2GHz carrier frequency, assuming 2GHz carrier"<<endl;
  
  tempLinkInfo.shadowStdDev=10;//used the same std_dev as UMACRO
  
  double R=1.0*tempLinkInfo.distance/1000.0;
  tempLinkInfo.pathLoss=140.7+36.7*log10(R);
 
  
  
  //According to 3GPP TR 36.814 3GPP case one have penetration loss of 20 dB  
  //According to 3GPP TR 25.814 section A.2.1.1.1  100% of the users for a given simulation case are assigned the same ‘PLoss’ and speed.
  tempLinkInfo.pathLoss+=20.0; //Penetration loss added to all UEs.  this is a BugFix as SINR was matching but coupling loss was off by 20dB in calibration
  
  
  
  return (tempLinkInfo);
  
}

NodeLinkInfo_S het3gppMacroUeModel1(Location_S nodeLoc1, Location_S nodeLoc2, double carrierFreqInHz)
{
  NodeLinkInfo_S tempLinkInfo;
  tempLinkInfo.isLOS=0;
  tempLinkInfo.distance=findDistance(nodeLoc1,nodeLoc2);
  
  if(carrierFreqInHz != 2.0e9)
	cout<<"[both:]  Warning.... 3gpp Model 1 is defined for 2GHz carrier frequency, assuming 2GHz carrier"<<endl;
  
  tempLinkInfo.shadowStdDev=8;
  //based on R1-105354
  double R=1.0*tempLinkInfo.distance/1000.0;
  tempLinkInfo.pathLoss=128.1+37.6*log10(R);

  
  
  //According to 3GPP TR 36.814 3GPP case one have penetration loss of 20 dB  
  //According to 3GPP TR 25.814 section A.2.1.1.1  100% of the users for a given simulation case are assigned the same ‘PLoss’ and speed.
  tempLinkInfo.pathLoss+=20.0; //Penetration loss added to all UEs.  this is a BugFix as SINR was matching but coupling loss was off by 20dB in calibration

  
  return  (tempLinkInfo);
}
