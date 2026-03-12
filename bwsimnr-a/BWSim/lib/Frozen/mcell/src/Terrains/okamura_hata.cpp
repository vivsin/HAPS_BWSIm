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
bool isDebugEnabled=false;
bool isInHataBounds(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS)
{
double d=findDistance(nodeLoc1,nodeLoc2);//Hata expect input in km
	  if(d<10)
	  {
	    if(isDebugEnabled)cout<<"[both:] The Hata pathLoss model is for a minimum distance of 1km but the input values is even less than 10m exiting....."<< endl;exit(1);
	  }
	  if(d<1000)
	  {
	    if(isDebugEnabled)cout<<"[both:] Warning ...! originally okamura-hata model is defined for a minimum distance of  1 km......still continuing with distance "<<d<<endl; 
	  }
	  
	  
   if(nodeLoc1.isIndoor==1||nodeLoc2.isIndoor==1)
    {
        if(isDebugEnabled)cout<<"[both:]  Warning ! ...... HataModel ignores the Indoor flag"<<endl;

    }
    if(isLOS==-1)
  {
    if(isDebugEnabled)cout<<"[both:] Warning....! the passed isLOS value will be ignored  in Hata model  "<<isLOS<<endl;
  }

    bool isBsFound=false;
    //1. check for minimum bs height;
    if(nodeLoc2.z>=10||nodeLoc1.z>=10)
    {
        if(nodeLoc2.z>1000||nodeLoc1.z>1000)
        {
            if(isDebugEnabled)cout<<"[both:] Error !.... Okamura-Hata Model is supported for a BS height of 30-1000m, but the value is higher than 1000 m exiting..."<<endl;
            exit(1);
        }
        else  isBsFound=true;
    }
    else
    {

        if(isDebugEnabled)cout<<"[both:] Error !.... Okamura-Hata Model is supported for a BS height of 30-1000m,this function supports  BS height as low as 10  based on winner+ studies D5.3, but the value is lower than 10 m exiting....."<<endl;
        exit(1);
    }
    bool isMsFound=false;//
    //2. check for maxMS height
    if(nodeLoc2.z<10||nodeLoc1.z<10)
    {
        if(nodeLoc2.z>=1&&nodeLoc1.z>=1)
        {

            isMsFound=true;
        }

        else
        {
            if(isDebugEnabled)cout<<"[both:] Error !.... Okamura-Hata Model is supported for a MS height of 1 to 10m, but the value entered is less than 1 m exiting..."<<endl;
            exit(1);
        }
    }
    else
    {

        if(isDebugEnabled)cout<<"[both:] Error !.... Okamura-Hata Model is supported for a MS height of 1-10m, but the value is greater than or equlal to 10 m"<<endl;
        exit(1);
    }
    bool isCarrierFreqinRange=false;
    if(carrierFreqInHz>=1.5e8 &&carrierFreqInHz <=1.5e9)
    {
      isCarrierFreqinRange=true;
    }
    else
    {
      if(isDebugEnabled)cout<<"[both:] Error !.... carrier Frequency  is not in the expected range of 150MHz to 1.5 GHz for Okamura Hata Model, please consider using models such as ITU, Winner+,Cost231 etc if avalible exiting......"<<endl;exit(1); 
    }
    return(isBsFound&isMsFound&isCarrierFreqinRange);
}

/*!
       *  \fn NodeLinkInfo_S getHataPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr)

 *  \brief calculates the path loss using Okamura-Hata Model
 **  \param nodeLoc1 --> location of the base station/mobile station 
 *  \param nodeLoc2 --> location of the basestion/mobile  station
 *  \param carrierFreqInHz --> carrier frequency in Hz.acceptable range is 150MHz to 1.5GHz
 *  \param isLOS --> Whether the links is LOS or not(-1 not set)
 *  \param nodeLoc2 --> location of the basestion/mobile  station
 * \param nodeLoc2 --> location of the basestion/mobile  station
 * \param nodeLoc2 --> location of the basestion/mobile  station
  
 *	The function uses Okamura-Hata model to calculate the pathloss between a base station and
 * subscriber station. The hata offset is calculated depending upon the terrain which is used to
 * calculate the pathloss. There are different terrains defined in the function as Urban
 * Macrocell, Suburban Macrocell and Open Rural Macrocell.
 * 
 * 
 * this function will try to detect bs based height as bs height of original Okamura-Hata model is defined for BS height of 30-1000m and MS height from 1-10m. 
 * 
 * (Note: this function will accept bs height of 20m and above but no gurarantee of its validity,though it is used in winner+ deliverable D5.3 for 25m, other reference is )
   
   Reference is http://en.wikipedia.org/wiki/Hata_Model


      */

NodeLinkInfo_S getHataPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int /*txNodeType*/, int /*rxNodeType*/, const Terrain * /*terr*/,NodeCategory_E /*nodeCategory1*/,NodeCategory_E /*nodeCategory2*/)
{
 

	if(traceValuePerTraceName["printHataDebug"]=="true")isDebugEnabled=true;
	else isDebugEnabled=false;
  //!Okamura-Hata Model for Urban Areas
    if(! isInHataBounds( nodeLoc1, nodeLoc2,  carrierFreqInHz,  isLOS))
    {
        if(isDebugEnabled)cout<<"[both:] Returned values for Boundary check for hata indicated a failure ..... exiting"<<endl;
        exit(1);
    }
  //
  
 
        Location_S msLoc;
        Location_S bsLoc;
       
        if(nodeLoc2.z>=10){
        msLoc=nodeLoc1;
        bsLoc=nodeLoc2;
        }
        else if(nodeLoc1.z>=10){
            msLoc=nodeLoc2;
            bsLoc=nodeLoc1;
          }
          
          double  f=carrierFreqInHz/1.0e6;//carrierFrequency in MHz
      
	  double hB=bsLoc.z;
	  double  hM=msLoc.z;
	  double d=(findDistance(msLoc,bsLoc))/1000.0;//Hata expect input in km
	  
	  //for small and medium cities
	   double CH;
      CH=0.8+(1.1*log10(f)-0.7)*hM-1.56*log10(f);
    //for big cities ---will not be default
//     if(f>=150&&f<=200)CH=8.29*pow(log10(1.54*hM),2)-1.1;
//     else if(f<=1500&&f>200)CH=3.2*pow2(log10(11.75*hM))-4.97;
	  
	  double Lu=69.55+26.16*log10(f)-13.82*log10(hB)-CH+ (44.9-6.55*log10(hB))*log10(d);
	  
	  
	  
	  NodeLinkInfo_S nodeInfo;
	  
	  nodeInfo.distance=d;
	  nodeInfo.isLOS=0;
	  nodeInfo.pathLoss=Lu;
	  nodeInfo.shadowLoss=8;//based on values given in old BWSIM files
	  
	  
	  return nodeInfo;
	  
}

NodeLinkInfo_S getHataPathLoss_S(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr,NodeCategory_E /*nodeCategory1*/,NodeCategory_E /*nodeCategory2*/)
{
  double  f=carrierFreqInHz/1.0e6;
  double LSUcorrection=-2*sqr(log10(f/28))-5.4;
  NodeLinkInfo_S nodeInfo=getHataPathLoss(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,txNodeType,rxNodeType,terr);
  if(isDebugEnabled)cout<<"LSUcorrection in dB "<<LSUcorrection<<endl;
  nodeInfo.pathLoss+=LSUcorrection;//pathLoss correction 'C' for suburban areas
  return nodeInfo;
}

NodeLinkInfo_S getHataPathLoss_O(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr,NodeCategory_E /*nodeCategory1*/,NodeCategory_E /*nodeCategory2*/)
{
  double  f=carrierFreqInHz/1.0e6;
  double LOcorrection=-4.78*sqr(log10(f))+18.33*log10(f)-40.94;
  NodeLinkInfo_S nodeInfo=getHataPathLoss(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,txNodeType,rxNodeType,terr);
  if(isDebugEnabled)cout<<"LOcorrection in dB "<<LOcorrection<<endl;
  nodeInfo.pathLoss+=LOcorrection;
  return nodeInfo;
  
  
}

void loadHataTerrains(Terrain &terrain)
{
    terrain.addTerrain((string)"_TERRAIN_HATA_URBAN_",getHataPathLoss);
    terrain.addTerrain((string)"_TERRAIN_HATA_SUBURBAN_",getHataPathLoss_S);
    terrain.addTerrain((string)"_TERRAIN_HATA_OPEN_",getHataPathLoss_O);
}
