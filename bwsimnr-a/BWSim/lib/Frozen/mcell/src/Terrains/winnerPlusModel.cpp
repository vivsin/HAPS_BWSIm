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



void loadWinnerPlusTerrains(Terrain &terrain){
  
  Array<string> terrainNames="{ _TERRAIN_WINNER_PLUS_URBAN_MICRO_ _TERRAIN_WINNER_PLUS_URBAN_MACRO_  _TERRAIN_WINNER_PLUS_SUBURBAN_MACRO_ }";
  terrain.addTerrain(terrainNames,&getWinnerPlusPathloss);
}

bool bsSearch(Location_S nodeLoc1,Location_S nodeLoc2, int txNodeType, int rxNodeType)
{
  //This is a local function to find BS location return true if nodeLoc2 corresponds to a BS, false if nodeLoc1 corresponds to BS
  
  if(txNodeType==0)  return true;//0th node  is by default is BS
  if(rxNodeType==0) return false;
  if(nodeLoc2.z>nodeLoc1.z) return true;//Assume BS have heigt > MS 
  else  return false;
}

/*!    *  \fn NodeLinkInfo_S getWinnerPlusPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr)

 *  \brief calculates the path loss using Winner Plus pathLoss models
 **  \param nodeLoc1 --> location of the base station/mobile station 
 *  \param nodeLoc2 --> location of the basestion/mobile  station
 *  \param carrierFreqInHz --> carrier frequency in Hz.acceptable range is 150MHz to 1.5GHz
 *  \param isLOS --> Whether the links is LOS or not(-1 not set)
 *  \param nodeLoc2 --> location of the basestion/mobile  station
 * \param nodeLoc2 --> location of the basestion/mobile  station
 * \param nodeLoc2 --> location of the basestion/mobile  station
  
 *	The function uses Winner+ pathloss models to calculate the pathloss between a base station and
 * subscriber station. The hata offset is calculated depending upon the terrain which is used to
 * calculate the pathloss. There are different terrains defined in the function as Urban
 * Macrocell, Suburban Macrocell and Urban Micro.
 * 
 *   Reference is Winner + deliverable D5.3: Winner + Final Channel Models available in http://projects.celtic-initiative.org/winner+/deliverables.html


      */



NodeLinkInfo_S getWinnerPlusPathloss(Location_S nodeLoc1o,Location_S nodeLoc2o, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType,  const Terrain * terr,NodeCategory_E /*nodeCategory1*/,NodeCategory_E /*nodeCategory2*/)
{
  

	
  int linkType=nodeLoc2o.isIndoor*10+nodeLoc1o.isIndoor;
  if (terr==0)
  {
	cout<<"[both:] please pass Terrain object as a member function to getPathLoss_Basic (...,terr)"<<endl;
	exit(1);
  }
  Location_S nodeLoc1,nodeLoc2;
  //Rearranging nodeLocation to MS,BS order if not....
  if(bsSearch(nodeLoc1,nodeLoc2,txNodeType,rxNodeType))
  {
    nodeLoc1=nodeLoc1o;
    nodeLoc2=nodeLoc2o;
  }
  else{
  
  nodeLoc1=nodeLoc2o;
  nodeLoc2=nodeLoc1o;

  }
  string myTerrain=terr->getTerrain();
  
  switch(linkType)
  {
	case 0:
	  if(myTerrain=="_TERRAIN_WINNER_PLUS_URBAN_MICRO_")
		return(PlusUMicroOutdoorToOutdoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS));
	  else if(myTerrain=="_TERRAIN_WINNER_PLUS_URBAN_MACRO_")
		return(PlusUMacroOutdoorToOutdoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS));
	  else if(myTerrain=="_TERRAIN_WINNER_PLUS_SUBURBAN_MACRO_")
		return(PlusSMacroOutdoorToOutdoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS));
	  else
	  {
		cout<<"[both:] Unsupported Terrain type for pathloss calculation..."<<endl;
		abort();
	  }
	case 1:
	  if(myTerrain=="_TERRAIN_WINNER_PLUS_URBAN_MICRO_")
		return(PlusUMicroIndoorToOutdoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,terr));
	  else if(myTerrain=="_TERRAIN_WINNER_PLUS_URBAN_MACRO_")
		return(PlusUMacroOutdoorToIndoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,terr));
	  else
	  {
		cout<<"[both:] Unsupported Terrain type for pathloss calculation..."<<endl;
		abort();
	  }
	case 10:
	  if(myTerrain=="_TERRAIN_WINNER_PLUS_URBAN_MICRO_")
		return(PlusUMicroIndoorToOutdoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,terr));
	  else if(myTerrain=="_TERRAIN_WINNER_PLUS_URBAN_MACRO_")
		return(PlusUMacroOutdoorToIndoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,terr));
	  else
	  {
		cout<<"[both:] Unsupported Terrain type for pathloss calculation..."<<endl;
		abort();
	  }
// 	case 11:
// 	  return(PlusIndoorToIndoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr));
  }
  
  cout<<" [both:] The code should not reach this point getPathLoss_Basic () exiting....."<<endl;
  exit(1);
}

NodeLinkInfo_S PlusUMicroIndoorToOutdoorPathloss(Location_S nodeLoc1, Location_S nodeLoc2, double carrierFreqInHz, const Terrain * terr)
{
  if(terr->getRoomSize ().length==-1)
  {
	cout<<"[both:] Error... Room Size not set for Indoor Pathloss Computations...."<<endl;
	abort();
  }
  
  NodeLinkInfo_S tempLinkInfo,tempLinkInfoB1;
  double dIn;
  double PLb,PLtw,PLin;
  
  
  double distanceOfSeparation,carrierFreqInGHz;
  
  distanceOfSeparation=findDistance(nodeLoc1,nodeLoc2);
  carrierFreqInGHz = carrierFreqInHz / 1e9;
  dIn=randu()*min((double)terr->getRoomSize ().length,distanceOfSeparation);//this is to make sure total distance is greater or equal to dIn
  
  
  tempLinkInfoB1=PlusUMicroOutdoorToOutdoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz);
  
  PLb=tempLinkInfoB1.pathLoss;
  PLtw=21.04+14.0*(1-1.8*log10(carrierFreqInGHz));
		  if(!tempLinkInfoB1.isLOS) PLtw-=0.8*nodeLoc1.z;
  PLin=0.5*dIn;
  
  tempLinkInfo.isLOS=0;
  tempLinkInfo.pathLoss=PLb+PLtw+PLin;
  tempLinkInfo.distance=tempLinkInfoB1.distance;
  tempLinkInfo.shadowStdDev=7;
  return(tempLinkInfo);
}

NodeLinkInfo_S PlusUMicroOutdoorToOutdoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS)
{
  double distanceOfSeparation,carrierFreqInGHz;
  NodeLinkInfo_S tempLinkInfo;
  
  carrierFreqInGHz = carrierFreqInHz / 1e9;
  distanceOfSeparation = findDistance(nodeLoc1,nodeLoc2);
  tempLinkInfo.distance=distanceOfSeparation;
  
  tempLinkInfo.isLOS = false;
  
  double d_BP = 4*(nodeLoc1.z-1)*(nodeLoc2.z-1)*carrierFreqInHz/lightSpeed;
  if(isLOS==-1)
  {
	double PLos=std::min(18/distanceOfSeparation,(double)1.0)*(1-exp(-distanceOfSeparation/36))+exp(-distanceOfSeparation/36);
	tempLinkInfo.isLOS=(randu()<PLos);    
  }
  else
    tempLinkInfo.isLOS=isLOS;
  double maxDist=(tempLinkInfo.isLOS)? 5000:2000;
  if(distanceOfSeparation<maxDist && distanceOfSeparation>10)
  {
    if(tempLinkInfo.isLOS)
    {
     if(distanceOfSeparation<d_BP)
     {
       tempLinkInfo.pathLoss=22.7*log10(distanceOfSeparation)+27.0+20.0*log10(carrierFreqInGHz);
       tempLinkInfo.shadowStdDev=3.0;
     }
     else
     {
       tempLinkInfo.pathLoss=40.0*log10(distanceOfSeparation)+7.56-17.3*log10(nodeLoc2.z-1)-17.3*log10(nodeLoc1.z-1)+2.7*log10(carrierFreqInGHz);
       tempLinkInfo.shadowStdDev=3.0;
     }
    }
    else
    {
      if(carrierFreqInGHz>=.45 && carrierFreqInGHz <1.5)
      {
	tempLinkInfo.pathLoss=(44.9-6.55*log10(nodeLoc2.z)) *log10(distanceOfSeparation)+5.83*log10(nodeLoc2.z)+16.33+26.16*log10(carrierFreqInGHz);
      	tempLinkInfo.shadowStdDev=4.0;     
      }
      else if(carrierFreqInGHz>=1.5 && carrierFreqInGHz <2.0)
      {
	tempLinkInfo.pathLoss=(44.9-6.55*log10(nodeLoc2.z)) *log10(distanceOfSeparation)+5.83*log10(nodeLoc2.z)+14.78+34.97*log10(carrierFreqInGHz);
       	tempLinkInfo.shadowStdDev=4.0;     
      
      }
      else if(carrierFreqInGHz>=2.0 && carrierFreqInGHz <6.0)
      {
	tempLinkInfo.pathLoss=(44.9-6.55*log10(nodeLoc2.z)) *log10(distanceOfSeparation)+5.83*log10(nodeLoc2.z)+18.38+23.0*log10(carrierFreqInGHz);
      	tempLinkInfo.shadowStdDev=4.0;     
      }
      else
      {
	cout<<"[both:] Warning... frequency is not supported for winner +  model... "<<endl;
	tempLinkInfo.isLOS=0;
	tempLinkInfo.pathLoss=10e6;
	tempLinkInfo.shadowStdDev=0;
      }
    }
    
    
  }
  else
  {
    cout<<"[both:] Warning... Distance of separation greater than maximum distance/ less than minimum distance in Urban-Micro Outdoor to Outdoor model... "<<endl;
    tempLinkInfo.isLOS=0;
    tempLinkInfo.pathLoss=10e6;
    tempLinkInfo.shadowStdDev=0;
    
  }

	  
	  return(tempLinkInfo);
}

NodeLinkInfo_S PlusUMacroOutdoorToOutdoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS)
{
  double distanceOfSeparation,carrierFreqInGHz;
  NodeLinkInfo_S tempLinkInfo;
  
  carrierFreqInGHz = carrierFreqInHz / 1e9;
  distanceOfSeparation = findDistance(nodeLoc1,nodeLoc2);
  tempLinkInfo.distance=distanceOfSeparation;
  
  double d_BP = 4*(nodeLoc1.z-1)*(nodeLoc2.z-1)*carrierFreqInHz/lightSpeed;
  
  if(distanceOfSeparation<10)
  {
 	cout<<"[both:] Warning... Distance of separation less than minimum distance in Urban-Macro Outdoor to Outdoor model... "<<endl;
  	tempLinkInfo.isLOS=1;
  	tempLinkInfo.pathLoss=10e6;//putting a high pathloss
  	tempLinkInfo.shadowStdDev=0;
   }
    else 
    {
      if(isLOS==-1)
      {
	double PLos=std::min(18/distanceOfSeparation,(double)1.0)*(1-exp(-distanceOfSeparation/63))+exp(-distanceOfSeparation/63);
	tempLinkInfo.isLOS=(randu()<PLos);
	
      }
      else
      {
	tempLinkInfo.isLOS=isLOS;
	
      }
      
      if(distanceOfSeparation<5000)
      {
	if(tempLinkInfo.isLOS)
	{
	  if(distanceOfSeparation<d_BP)
	  {
	    tempLinkInfo.pathLoss=26.0*log10(distanceOfSeparation)+25.0+20.0*log10(carrierFreqInGHz);
	    tempLinkInfo.shadowStdDev=4;
	    
	  }
	  else
	  {
	    tempLinkInfo.pathLoss = 40.0*log10(distanceOfSeparation) + 9.27 - 14.0*log10(nodeLoc2.z-1)- 14.0*log10(nodeLoc1.z-1) + 6.0*log10(carrierFreqInGHz);
	    tempLinkInfo.shadowStdDev= 6;
	    
	  }
	  
	}
	else
	{
	  if(carrierFreqInGHz>=.45 && carrierFreqInGHz <1.5)
	  {
	    tempLinkInfo.pathLoss=(44.9-6.55*log10(nodeLoc2.z)) *log10(distanceOfSeparation)+5.83*log10(nodeLoc2.z)+16.33+26.16*log10(carrierFreqInGHz);
	    tempLinkInfo.shadowStdDev=8.0;     
	  }
	  else if(carrierFreqInGHz>=1.5 && carrierFreqInGHz <2.0)
	  {
	    tempLinkInfo.pathLoss=(44.9-6.55*log10(nodeLoc2.z)) *log10(distanceOfSeparation)+5.83*log10(nodeLoc2.z)+14.78+34.97*log10(carrierFreqInGHz);
	    tempLinkInfo.shadowStdDev=8.0;     
	    
	  }
	  else if(carrierFreqInGHz>=2.0 && carrierFreqInGHz <6.0)
	  {
	    tempLinkInfo.pathLoss=(44.9-6.55*log10(nodeLoc2.z)) *log10(distanceOfSeparation)+5.83*log10(nodeLoc2.z)+18.38+23.0*log10(carrierFreqInGHz);
	    tempLinkInfo.shadowStdDev=8.0;     
	    
	  }
	  else
	  {
	    cout<<"[both:] Warning... frequency is not supported for winner +  model... "<<endl;
	    tempLinkInfo.isLOS=0;
	    tempLinkInfo.pathLoss=10e6;
	    tempLinkInfo.shadowStdDev=0;
	    
	  }	  
	}
	
      }
      else
      {
	cout<<"[both:] Warning... Distance of separation greater than maximum distance in Urban-Macro Outdoor to Outdoor model... "<<endl;
	tempLinkInfo.isLOS=0;
	tempLinkInfo.pathLoss=10e6;
	tempLinkInfo.shadowStdDev=0;
	
      }
      
    }
    return(tempLinkInfo);
}


NodeLinkInfo_S PlusUMacroOutdoorToIndoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz,const Terrain * terr)
{
  if(terr->getRoomSize ().length==-1)
  {
	cout<<"[both:] Error... Room Size not set for Indoor Pathloss Computations...."<<endl;
	abort();
  }
  
  NodeLinkInfo_S tempLinkInfo,tempLinkInfoB1;
  double dIn;
  double PLb,PLtw,PLin;  
  
  double distanceOfSeparation,carrierFreqInGHz;
  
  distanceOfSeparation=findDistance(nodeLoc1,nodeLoc2);
  carrierFreqInGHz = carrierFreqInHz / 1e9;
  dIn=randu()*min((double)terr->getRoomSize ().length,distanceOfSeparation);//this is to make sure total distance is greater or equal to dIn
  
  
  tempLinkInfoB1=PlusUMacroOutdoorToOutdoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz);
  
  PLb=tempLinkInfoB1.pathLoss;
  PLtw=21.04+14.0*(1-1.8*log10(carrierFreqInGHz));
		  if(!tempLinkInfoB1.isLOS) PLtw-=0.8*nodeLoc1.z;
  PLin=0.5*dIn;
  
  tempLinkInfo.isLOS=0;//according to D112 plos approximated as zero
  tempLinkInfo.pathLoss=PLb+PLtw+PLin;
  tempLinkInfo.distance=tempLinkInfoB1.distance;
  tempLinkInfo.shadowStdDev=10;
  return(tempLinkInfo);
}

// NodeLinkInfo_S PlusIndoorToIndoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS,const Terrain * terr)
// {
//   NodeLinkInfo_S tempLinkInfo;
//   cout<<"[both:] Warning winner + dont have any IndoortoIndoor Pathloss Models defined please use Winner II models instead......"<<endl;  
//   {
//     tempLinkInfo.isLOS=1;
//     tempLinkInfo.pathLoss=10e6;
//     tempLinkInfo.shadowStdDev=0;
//   }
// 
// return(tempLinkInfo);
// }


NodeLinkInfo_S PlusSMacroOutdoorToOutdoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS)
{
  double distanceOfSeparation,carrierFreqInGHz;
  NodeLinkInfo_S tempLinkInfo;
  
  carrierFreqInGHz = carrierFreqInHz / 1e9;
  distanceOfSeparation = findDistance(nodeLoc1,nodeLoc2);
  tempLinkInfo.distance=distanceOfSeparation;
  
  double dBP = 4*(nodeLoc1.z)*(nodeLoc2.z)*carrierFreqInHz/lightSpeed;
  if(isLOS==-1)
  {
    double PLos=exp(-distanceOfSeparation/200.0);
    tempLinkInfo.isLOS=(randu()<PLos);    
  }
  else
  {
    tempLinkInfo.isLOS=isLOS;    
  }
  
  if( (tempLinkInfo.isLOS && distanceOfSeparation<30.0) || ( tempLinkInfo.isLOS==false && distanceOfSeparation<50.0) )
  {
 	cout<<"[both:] Warning... Distance of separation less than minimum distance in Suburban-Macro Outdoor to Outdoor model... "<<endl;
  	tempLinkInfo.isLOS=1;
  	tempLinkInfo.pathLoss=10e6;//putting a high pathloss
  	tempLinkInfo.shadowStdDev=0;
   }
   else 
   {
     if(distanceOfSeparation<5000)
     {
       if(tempLinkInfo.isLOS)
       {
	 if(distanceOfSeparation<dBP)
	 {
	   tempLinkInfo.pathLoss=23.8*log10(distanceOfSeparation)+27.2+20.0*log10(carrierFreqInGHz);
	   tempLinkInfo.shadowStdDev=4;
	   
	}
	else
	{
	  tempLinkInfo.pathLoss = 40.0*log10(distanceOfSeparation) + 9.0 - 16.2*log10(nodeLoc2.z)- 16.2*log10(nodeLoc1.z) + 3.8*log10(carrierFreqInGHz);
	  tempLinkInfo.shadowStdDev= 6;
	  
	}
	 
      }
      else
      {
	  if(carrierFreqInGHz>=.45 && carrierFreqInGHz <1.5)
	  {
	    tempLinkInfo.pathLoss=(44.9-6.55*log10(nodeLoc2.z)) *log10(distanceOfSeparation)+5.83*log10(nodeLoc2.z)+13.33+26.16*log10(carrierFreqInGHz);
	    tempLinkInfo.shadowStdDev=8.0;     
	  }
	  else if(carrierFreqInGHz>=1.5 && carrierFreqInGHz <2.0)
	  {
	    tempLinkInfo.pathLoss=(44.9-6.55*log10(nodeLoc2.z)) *log10(distanceOfSeparation)+5.83*log10(nodeLoc2.z)+11.78+34.97*log10(carrierFreqInGHz);
	    tempLinkInfo.shadowStdDev=8.0;     
	    
	  }
	  else if(carrierFreqInGHz>=2.0 && carrierFreqInGHz <6.0)
	  {
	    tempLinkInfo.pathLoss=(44.9-6.55*log10(nodeLoc2.z)) *log10(distanceOfSeparation)+5.83*log10(nodeLoc2.z)+15.38+23.0*log10(carrierFreqInGHz);
	    tempLinkInfo.shadowStdDev=8.0;     
	    
	  }
	  else
	  {
	    cout<<"[both:] Warning... frequency is not supported for winner +  model... "<<endl;
	    tempLinkInfo.isLOS=0;
	    tempLinkInfo.pathLoss=10e6;
	    tempLinkInfo.shadowStdDev=0;	    
	  }
	
      }
       
    }
    else
    {
	cout<<"[both:] Warning... Distance of separation greater than maximum distance in Urban-Macro Outdoor to Outdoor model... "<<endl;
	tempLinkInfo.isLOS=0;
	tempLinkInfo.pathLoss=10e6;
	tempLinkInfo.shadowStdDev=0;
      
    }
     
  }
    return(tempLinkInfo);
}
