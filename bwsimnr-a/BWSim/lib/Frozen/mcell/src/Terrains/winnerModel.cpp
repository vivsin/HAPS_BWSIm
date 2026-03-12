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

void loadWinnerTerrains(Terrain &terrain){
  
  Array<string> terrainNames="{ _TERRAIN_WINNER_URBAN_MICRO_ _TERRAIN_WINNER_URBAN_MACRO_ }";
  terrain.addTerrain(terrainNames,&getWinnerPathloss);
}

NodeLinkInfo_S getWinnerPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int /*txNodeType*/, int /*rxNodeType*/,  const Terrain * terr,NodeCategory_E /*nodeCategory1*/,NodeCategory_E /*nodeCategory2*/)
{
  int linkType=nodeLoc2.isIndoor*10+nodeLoc1.isIndoor;
  if (terr==0)
  {
	cout<<"[both:] please pass Terrain object as a member function to getPathLoss_Basic (...,terr)"<<endl;
	exit(1);
  }
  string myTerrain=terr->getTerrain();
  
  switch(linkType)
  {
	case 0:
	  if(myTerrain=="_TERRAIN_WINNER_URBAN_MICRO_")
		return(UMicroOutdoorToOutdoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS));
	  else if(myTerrain=="_TERRAIN_WINNER_URBAN_MACRO_")
		return(UMacroOutdoorToOutdoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr));
	  else
	  {
		cout<<"[both:] Unsupported Terrain type for pathloss calculation..."<<endl;
		exit(0);
	  }
	case 1:
	  if(myTerrain=="_TERRAIN_WINNER_URBAN_MICRO_")
		return(UMicroIndoorToOutdoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,terr));
	  else if(myTerrain=="_TERRAIN_WINNER_URBAN_MACRO_")
		return(UMacroOutdoorToIndoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,terr));
	  else
	  {
		cout<<"[both:] Unsupported Terrain type for pathloss calculation..."<<endl;
		exit(0);
	  }
	case 10:
	  if(myTerrain=="_TERRAIN_WINNER_URBAN_MICRO_")
		return(UMicroIndoorToOutdoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,terr));
	  else if(myTerrain=="_TERRAIN_WINNER_URBAN_MACRO_")
		return(UMacroOutdoorToIndoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,terr));
	  else
	  {
		cout<<"[both:] Unsupported Terrain type for pathloss calculation..."<<endl;
		exit(0);
	  }
	case 11:
	  return(IndoorToIndoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr));
  }
  
  cout<<" [both:] The code should not reach this point getPathLoss_Basic () exiting....."<<endl;
  exit(1);
}

NodeLinkInfo_S UMicroIndoorToOutdoorPathloss(Location_S nodeLoc1, Location_S nodeLoc2, double carrierFreqInHz, const Terrain * terr)
{
  if(terr->getRoomSize ().length==-1)
  {
	cout<<"[both:] Error... Room Size not set for Indoor Pathloss Computations...."<<endl;
	exit(0);
  }
  
  NodeLinkInfo_S tempLinkInfo,tempLinkInfoB1;
  double dIn;
  double PLb,PLtw,PLin,theta;
  dIn=randu()*(terr->getRoomSize ().length);
  tempLinkInfoB1=UMicroOutdoorToOutdoorPathloss(nodeLoc1,nodeLoc2,carrierFreqInHz);
  PLb=tempLinkInfoB1.pathLoss;
  theta=randi(0,90)*pi/180;
  PLtw=14+15*sqr((1-cos(theta)));
  PLin=0.5*dIn;
  tempLinkInfo.isLOS=0;
  tempLinkInfo.pathLoss=PLb+PLtw+PLin;
  tempLinkInfo.distance=tempLinkInfoB1.distance;
  tempLinkInfo.shadowStdDev=7;
  return(tempLinkInfo);
}

NodeLinkInfo_S UMicroOutdoorToOutdoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS)
{
  double distanceOfSeparation,carrierFreqInGHz;
  NodeLinkInfo_S tempLinkInfo;
  
  carrierFreqInGHz = carrierFreqInHz / 1e9;
  distanceOfSeparation = findDistance(nodeLoc1,nodeLoc2);
  tempLinkInfo.isLOS = false;
  double d1=nodeLoc1.x-nodeLoc2.x;
  if(d1<0) d1=d1*-1;
  double d2=nodeLoc1.y-nodeLoc2.y;
  if(d2<0) d2=d2*-1;
  double dBP = 4*(nodeLoc1.z-1)*(nodeLoc2.z-1)*carrierFreqInHz/lightSpeed;
  tempLinkInfo.distance=distanceOfSeparation;
		//if(d1<10)
		//   {
		  // 	cout<<"[both:] Warning... Distance of separation less than minimum distance in Urban-Micro Outdoor to Outdoor model... "<<endl;
		  // 	tempLinkInfo.isLOS=1;
		  // 	tempLinkInfo.pathLoss=0;
		  // 	tempLinkInfo.shadowStdDev=0;
		  //   }
		  //   else if(d1>10)
	  {
		if(d2<10 || d1<10)
		  tempLinkInfo.isLOS=1;
		else
		{
		  if(isLOS==-1)
		  {
			double PLos=std::min(18/d2,(double)1.0)*(1-exp(-d2/36))+exp(-d2/36);
			tempLinkInfo.isLOS=(randu()<PLos);
		  }
		  else
			tempLinkInfo.isLOS=isLOS;
		}
		if(d1<5000 && d2<2000)
		{
		  if(tempLinkInfo.isLOS)
		  {
			double d_new=std::max(d1,d2);
			if(d_new<dBP)
			{
			  tempLinkInfo.pathLoss=22.7*log10(d_new)+41.0+20.0*log10(carrierFreqInGHz/5.0);
			  tempLinkInfo.shadowStdDev=3;
			}
			else
			{
			  tempLinkInfo.pathLoss = 40.0*log10(d_new) + 9.45 - 17.3*log10(nodeLoc1.z-1)- 17.3*log10(nodeLoc2.z-1) + 2.7*log10(carrierFreqInGHz/5.0);
			  tempLinkInfo.shadowStdDev= 3;
			}
		  }
		  else
		  {
			
			double PLos1,PLos2,PL1,PL2;
			if(d1<dBP)	PLos1=22.7*log10(d1)+41.0+20.0*log10(carrierFreqInGHz/5.0);
			else	PLos1 = 40.0*log10(d1) + 9.45 - 17.3*log10(nodeLoc1.z-1)- 17.3*log10(nodeLoc2.z-1) + 2.7*log10(carrierFreqInGHz/5.0);
			if(d2<dBP)	PLos2=22.7*log10(d2)+41.0+20.0*log10(carrierFreqInGHz/5.0);
			else	PLos2 = 40.0*log10(d2) + 9.45 - 17.3*log10(nodeLoc1.z-1)- 17.3*log10(nodeLoc2.z-1) + 2.7*log10(carrierFreqInGHz/5.0);
			
			PL1=PLos1 + 20 - 12.5*(std::max(2.8 - 0.0024*d1 , 1.84)) + 10*(std::max(2.8 - 0.0024*d1 , 1.84))*log10(d2) + 3*log10(carrierFreqInGHz/5.0);
			PL2=PLos2 + 20 - 12.5*(std::max(2.8 - 0.0024*d2 , 1.84)) + 10*(std::max(2.8 - 0.0024*d2 , 1.84))*log10(d1) + 3*log10(carrierFreqInGHz/5.0);
			
			tempLinkInfo.pathLoss = std::min(PL1,PL2);
			tempLinkInfo.shadowStdDev=4;
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

NodeLinkInfo_S UMacroOutdoorToOutdoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS,const Terrain * terr)
{
  double distanceOfSeparation,carrierFreqInGHz;
  NodeLinkInfo_S tempLinkInfo;
  
  carrierFreqInGHz = carrierFreqInHz / 1e9;
  distanceOfSeparation = findDistance(nodeLoc1,nodeLoc2);
  tempLinkInfo.isLOS = false;
  tempLinkInfo.distance=distanceOfSeparation;
  
  double dBP = 4*(nodeLoc1.z-1)*(nodeLoc2.z-1)*carrierFreqInHz/lightSpeed;
  //   if(distanceOfSeparation<10)
		//   {
		  // 	cout<<"[both:] Warning... Distance of separation less than minimum distance in Urban-Macro Outdoor to Outdoor model... "<<endl;
		  // 	tempLinkInfo.isLOS=1;
		  // 	tempLinkInfo.pathLoss=0;
		  // 	tempLinkInfo.shadowStdDev=0;
		  //   }
		  //   else if(distanceOfSeparation>10)
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
		if(distanceOfSeparation>.8*(terr->getCellISD()))
		{
		  tempLinkInfo.isLOS=0;
		}
		
		if(distanceOfSeparation<5000)
		{
		  if(tempLinkInfo.isLOS)
		  {
			if(distanceOfSeparation<dBP)
			{
			  tempLinkInfo.pathLoss=26.0*log10(distanceOfSeparation)+39.0+20.0*log10(carrierFreqInGHz/5.0);
			  tempLinkInfo.shadowStdDev=4;
			}
			else
			{
			  tempLinkInfo.pathLoss = 40.0*log10(distanceOfSeparation) + 13.47 - 14.0*log10(nodeLoc1.z-1)- 14.0*log10(nodeLoc2.z-1) + 6.0*log10(carrierFreqInGHz/5.0);
			  tempLinkInfo.shadowStdDev= 6;
			}
		  }
		  else
		  {
			tempLinkInfo.pathLoss = (44.9 - 6.55*log10(std::max(nodeLoc1.z,nodeLoc2.z)))*log10(distanceOfSeparation) + 34.46 + 5.83*log10(std::max(nodeLoc1.z,nodeLoc2.z)) + 23*log10(carrierFreqInGHz/5.0);
			tempLinkInfo.shadowStdDev= 8;
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


NodeLinkInfo_S UMacroOutdoorToIndoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz,const Terrain * terr)
{
  if(terr->getRoomSize ().length==-1)
  {
	cout<<"[both:] Error... Room Size not set for Indoor Pathloss Computations...."<<endl;
	exit(0);
  }
  double dIn=randu()*(terr->getRoomSize ().length);
  NodeLinkInfo_S tempLinkInfoC2=UMacroOutdoorToOutdoorPathloss(nodeLoc1,nodeLoc2, carrierFreqInHz,-1,terr);
  NodeLinkInfo_S tempLinkInfo;
  double hMS;
  if(nodeLoc1.isIndoor) hMS=nodeLoc1.z;
  else	hMS=nodeLoc2.z;
  tempLinkInfo.isLOS=0;
  tempLinkInfo.distance=tempLinkInfoC2.distance;
  tempLinkInfo.pathLoss=tempLinkInfoC2.pathLoss + 17.4 + 0.5*dIn - 0.8*hMS;
  tempLinkInfo.shadowStdDev=10;
  return(tempLinkInfo);
}

NodeLinkInfo_S IndoorToIndoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS,const Terrain * terr)
{
  
  if(terr->getRoomSize ().length==-1)
  {
	cout<<"[both:] Error... Room Size not set for Indoor Pathloss Computations...."<<endl;
	exit(1);
  }
  if(terr->getWallSpacing()==-1)
  {
	cout<<"[both:] Error...Wall Spacing not set for Indoor-Indoor pathloss Computations..."<<endl;
	exit(1);
  }
  if(terr->getMaximumFloors ()==-1)
  {
	cout<<"[both:] Error...Maximum Floors not set for Indoor-Indoor pathloss Computations..."<<endl;
	exit(1);
  }
  
  
  
  NodeLinkInfo_S tempLinkInfo;
  double A=0,B=0,C=0,X=0,FL=0;
  double distanceOfSeparation = findDistance(nodeLoc1,nodeLoc2);
  double carrierFreqInGHz=carrierFreqInHz/1e9;
  tempLinkInfo.distance=distanceOfSeparation;
  /*  if(distanceOfSeparation<3)
  {
	// 	cout<<"[both:] Warning... Distance of separation less than minimum distance in Indoor to Indoor model.. "<<endl;
	tempLinkInfo.isLOS=1;
	tempLinkInfo.pathLoss=0;
	tempLinkInfo.shadowStdDev=0;
}
else */if(distanceOfSeparation<100)
{
  if(distanceOfSeparation> terr->getRoomSize ().length)
	tempLinkInfo.isLOS=0;
  else
  {
	if(isLOS==-1)
	{
	  double PLos=1-0.9*pow(1-pow(0.61*log10(distanceOfSeparation),3),1/3);
	  tempLinkInfo.isLOS=(randu()<PLos);
	}
	else
	  tempLinkInfo.isLOS=isLOS;
	/// If the distance of separation is less than 3, make it LOS -Note: Very strictly speaking, pathloss model is not valid
	if(distanceOfSeparation<3)
	  tempLinkInfo.isLOS = 1;
  }
  if(tempLinkInfo.isLOS)
  {
	A=18.7;B=46.8;C=20;
	tempLinkInfo.shadowStdDev=3;
  }
  else
  {
	// Considering only Room to Room communication, light walls...
	A=20;B=46.4;C=20;
	int nLightWalls=floor_i(distanceOfSeparation/terr->getWallSpacing ());
	int nThickWalls=floor_i(distanceOfSeparation/terr->getRoomSize ().length);
	if(nLightWalls>0)
	  X+=terr->getWallPenetrationLoss(false)*(nLightWalls-nThickWalls);
	if(nThickWalls>0)
	  X+=terr->getWallPenetrationLoss()*(nThickWalls);
	
	if(nThickWalls)
	  tempLinkInfo.shadowStdDev=8;
	else
	  tempLinkInfo.shadowStdDev=6;
  }
  int n1Floor,n2Floor;
  n1Floor=nodeLoc1.z/terr->getRoomSize ().height;
  n2Floor=nodeLoc2.z/terr->getRoomSize ().height;
  if(n1Floor!=n2Floor)
	FL=17+4*(std::abs(n1Floor-n2Floor)-1);
  tempLinkInfo.pathLoss=A*log10(distanceOfSeparation) + B + C*log10(carrierFreqInGHz/5.0) + X + FL;
  
}
else
{
  // 	cout<<"[both:] Warning... Distance of separation greater than maximum distance in Indoor to Indoor model... "<<endl;
  tempLinkInfo.isLOS=0;
  tempLinkInfo.pathLoss=20e3;
  tempLinkInfo.shadowStdDev=0;
}
if(distanceOfSeparation==0)
{
  tempLinkInfo.isLOS=1;
  tempLinkInfo.pathLoss=10e3;
  tempLinkInfo.shadowStdDev=0;
}

return(tempLinkInfo);
}
