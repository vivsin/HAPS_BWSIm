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

void loadSmallCellTerrains(Terrain &terrain)
{
  terrain.enable3DDistance();
  terrain.addTerrain((string)"_TERRAIN_SMALL_CELLS_",getSmallCellPathLoss);
}

NodeLinkInfo_S getSmallCellPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int /*txNodeType*/, int /*rxNodeType*/, const Terrain * terr,NodeCategory_E /*nodeCategory1*/,NodeCategory_E /*nodeCategory2*/)
{  
  if(nodeLoc1.isIndoor&&nodeLoc2.isIndoor==1)
  {
    cout<<" [both:] Error! ...... Indoor to Indoor Model is supported now, Please add a Terrain, for the scenario both eNodeB and UE is indoor, exiting"<<endl;
    exit(2);
  }  
  if(nodeLoc2.z==25||nodeLoc1.z==25)
  {
    Location_S nodeLoc1m;
    Location_S nodeLoc2m;
    bool isIndoor;
    if(nodeLoc2.z==25)
    {
      nodeLoc1m=nodeLoc1;
      nodeLoc2m=nodeLoc2;
      isIndoor=nodeLoc1m.isIndoor;
    }
    else if(nodeLoc1.z==25)
    {
      nodeLoc1m=nodeLoc2;
      nodeLoc2m=nodeLoc1;
      isIndoor=nodeLoc1m.isIndoor;
    }
    int isLOSLink=-1;
    double distance=0,din=0,dout=0;double losThreshold=0;
    if(nodeLoc1m.isIndoor)
    {
      distance=find3Ddistance(nodeLoc1,nodeLoc2);
      din=randu()*min(25.0,distance);
      dout=distance;
      losThreshold = min(18.0 / dout, 1.0) * (1.0 - exp(-dout / 63.0)) + exp(-dout / 63.0);
      isLOSLink=false;
      if (randu() < losThreshold )
      {
	isLOSLink = true;
      }
      nodeLoc1m.isIndoor=false;
    }  
    if(isLOS!=-1)
      isLOSLink=isLOS;
      
    NodeLinkInfo_S nodeInfo=imtUrbanMacro(nodeLoc1m,  nodeLoc2m, carrierFreqInHz,  isLOSLink,terr);
    
    //since the imtUrbanMacro adds a 9 dB incar pathloss to remove it the following code
    nodeInfo.pathLoss-=9;
    if(isIndoor==true)
    {    
      nodeInfo.pathLoss += 20.0 + (0.5*din);
      nodeInfo.shadowStdDev=6.0;
    }   
    return nodeInfo;
  }
  else if(nodeLoc2.z==10||nodeLoc1.z==10)
  {
    Location_S nodeLoc1m;
    Location_S nodeLoc2m;
    bool isIndoor;
    if(nodeLoc2.z==10)
    {
      nodeLoc1m=nodeLoc1;
      nodeLoc2m=nodeLoc2;
      isIndoor=nodeLoc1m.isIndoor;
    }
    else if(nodeLoc1.z==10)
    {
      nodeLoc1m=nodeLoc2;
      nodeLoc2m=nodeLoc1;
      isIndoor=nodeLoc1m.isIndoor;
    }
    int isLOS=-1;
    double distance=0,din=0,dout=0;double losThreshold=0;
    if(nodeLoc1m.isIndoor)
    {
      distance=find3Ddistance(nodeLoc1,nodeLoc2);
      din=randu()*min(25.0,distance);
      dout=distance;
      losThreshold = min(18.0 / dout , 1.0) * (1.0 - exp(-dout / 36.0)) + exp(-dout/ 36.0);
      isLOS=false;
      if (randu() < losThreshold )
      {
	isLOS = true;
      }
      nodeLoc1m.isIndoor=false;
    }    
    NodeLinkInfo_S nodeInfo= imtUrbanMicro(nodeLoc1m,  nodeLoc2m, carrierFreqInHz,  isLOS,terr);
    if(isIndoor==true)
    {   
      nodeInfo.pathLoss += 20.0 + (0.5*din);
      if(3.5e9==carrierFreqInHz)
	nodeInfo.pathLoss +=3;//Additional penetration loss at high frequency.
      nodeInfo.shadowStdDev=7.0;
    }
    return nodeInfo;
  }
  else
  {
    cout<<"[both:] Error: Scenario for mix of macro/ low power nodes are defined for two heights 25/10 m please check the input loc" <<nodeLoc2<<endl;
    exit(2);
  }  
}
