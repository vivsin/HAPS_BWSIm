/// The following models are based on TR 36.828 or R1-112230

#include "../../include/Terrain.h"

NodeLinkInfo_S getCrossLinkPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int /*txNodeType*/, int /*rxNodeType*/,  const Terrain * terr,NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2)
{
  
  if (terr==0)
  {
    cout<<"[both:] please pass Terrain object as a member function to getCrossLinkPathloss (...,terr)"<<endl;
    exit(1);
  }
  string myTerrain=terr->getTerrain();
  
  if(nodeCategory1==_NODECATEGORY_SERVERNODE_ && nodeCategory2==_NODECATEGORY_SERVERNODE_) // BS to  BS
  {
    //! The models are yet to be finalised - Dhiv
    if(myTerrain=="_TERRAIN_IMT_URBAN_MACRO_" || myTerrain=="_TERRAIN_IMT_SUBURBAN_MACRO_" ||myTerrain=="_TERRAIN_IMT_RURAL_MACRO_" ||myTerrain=="_TERRAIN_3GPP_CASE1_" ||myTerrain=="_TERRAIN_3GPP_CASE3_" ||myTerrain=="_TERRAIN_WINNER_URBAN_MACRO_" ||myTerrain=="_TERRAIN_COMP_3GPP_CASE1_" ||myTerrain=="_TERRAIN_SMALL_CELLS_" ||myTerrain=="_TERRAIN_HATA_URBAN_" ||myTerrain=="_TERRAIN_HATA_SUBURBAN_" || myTerrain=="_TERRAIN_HATA_OPEN_")
      return(UMaServertoServerPathloss(nodeLoc1,nodeLoc2));  
    else if(myTerrain=="_TERRAIN_IMT_URBAN_MICRO_" || myTerrain=="_TERRAIN_WINNER_URBAN_MICRO_")
      return(imtUrbanMicro(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr)); //Need to be confirmed
      else
      {
	cout<<"[both:] Unsupported Terrain type to getCrossLinkPathloss()..."<<endl;
	exit(0);
      }
  }
  else if(nodeCategory1==_NODECATEGORY_SERVICENODE_ && nodeCategory2==_NODECATEGORY_SERVICENODE_) // UE to UE
    return(ServicetoServicePathloss(nodeLoc1,nodeLoc2));// Service node to Service node Path loss is same for all Terrains
    else
    {cout<<"[both:] Error .. CrossLinkPathloss model supported only for same node categories .. "<<endl;abort();}
    
}

NodeLinkInfo_S UMaServertoServerPathloss(Location_S nodeLoc1,Location_S nodeLoc2)
{   
  double distanceOfSeparation;
  NodeLinkInfo_S tempLinkInfo;
  
  distanceOfSeparation = findDistance(nodeLoc1,nodeLoc2);
  tempLinkInfo.isLOS = false;
  tempLinkInfo.distance=distanceOfSeparation;  
  double R=distanceOfSeparation/1000.0;   
  
  if(nodeLoc2.z==25 && nodeLoc1.z==25)//! Macro to Macro
  {
    tempLinkInfo.isLOS = true;
    tempLinkInfo.pathLoss = 98.45+20.0*log10(R);
    tempLinkInfo.shadowStdDev= 0.0;
  }
  else if(nodeLoc2.z==10 && nodeLoc1.z==10)//! pico to pico
  {
    tempLinkInfo.shadowStdDev= 6.0;
    tempLinkInfo.isLOS = randu()<0.5-min(0.5,5.0*exp(-0.156/R))+min(0.5,5.0*exp(-R/0.03));	       
    if(tempLinkInfo.isLOS)
    {
      if(R<2/3)
	tempLinkInfo.pathLoss=98.4+20.0*log10(R);
      else
	tempLinkInfo.pathLoss=101.9+40.0*log10(R);		 
    }
    else//! Non LOS
      tempLinkInfo.pathLoss=169.36+40.0*log10(R);  
    
  }
  else if((nodeLoc2.z==25 && nodeLoc1.z==10) || (nodeLoc2.z==10 && nodeLoc1.z==25))//! Macro to pico or pico to Macro
  {
    tempLinkInfo.shadowStdDev= 6.0;
    tempLinkInfo.isLOS = randu()<min(0.018/R,1.0)*(1-exp(-R/0.072))+exp(-R/0.072);       
    if(tempLinkInfo.isLOS)	       
      tempLinkInfo.pathLoss=100.7+23.5*log10(R);	       
    else//! Non LOS
      tempLinkInfo.pathLoss=125.2+36.3*log10(R);  
  }
  else //! if node heights are not matching, it will exit
  {
    
    cout<<endl<<endl<<endl;cout<<"Invalid Node Heights for Cross-Links,need changes in crossLinkPathLoss.cpp"<<endl;
    cout<<"Except Urban Micro, for other terrains,Node Heights are: Macro=25 & Pico=10,so Exiting..."<<endl;
    cout<<endl<<endl<<endl;exit(0);
    
  }
  return(tempLinkInfo);
}

NodeLinkInfo_S ServicetoServicePathloss(Location_S nodeLoc1,Location_S nodeLoc2)
{
  double distanceOfSeparation;
  NodeLinkInfo_S tempLinkInfo;
  
  distanceOfSeparation = findDistance(nodeLoc1,nodeLoc2);
  tempLinkInfo.isLOS = false;
  tempLinkInfo.distance=distanceOfSeparation;
  double R=distanceOfSeparation/1000.0;
  
  if(distanceOfSeparation<=50.0)
  {
    tempLinkInfo.pathLoss = 98.45 + 20.0*log10(R);
    tempLinkInfo.shadowStdDev= 12.0;
  }
  else
  {
    tempLinkInfo.pathLoss = 175.78 + 40.0*log10(R);
    tempLinkInfo.shadowStdDev= 12.0;
  }
  
  return(tempLinkInfo);
}

