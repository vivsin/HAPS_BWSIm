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

void loadCompTerrains(Terrain &terrain)
{
  terrain.addTerrain((string)"_TERRAIN_COMP_3GPP_CASE1_",getCompPathLoss);
}

NodeLinkInfo_S getCompPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int /*txNodeType*/, int /*rxNodeType*/, const Terrain * terr,NodeCategory_E /*nodeCategory1*/,NodeCategory_E /*nodeCategory2*/)
{
  /*!
  3GPP comp calibration differ from IMT advanced in 2 significant way
  1.UMA is used for eNodeB-UE link, with indoor support
  2. In case of Indoor Users LOS links are used for Pathloss, LOS probability  is calculated using the distance from eNodeB/Pico/LPN node to wall distance (dout).
  
  Ref: R1-111937 "Way forward on extension of channel modeling for Phase 2 evaluations" 
  and set of email discussions with subject "Re: [65-54] Submission of calibration results for CoMP evaluation."
  
  The following code modifies IMT advanced such that Pathloss of outdoor UE is calculated using IMT model  with the modified LOS probability  supplied. Indoor pathloss and Penetration loss added from this file
  The whole session of IMT PLOS and indoor Pathloss in the case of UMI is bypassed.
  
  
  */
  
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
    if(nodeLoc2.z==25){
      nodeLoc1m=nodeLoc1;
      nodeLoc2m=nodeLoc2;
      isIndoor=nodeLoc1m.isIndoor;
    }
    else if(nodeLoc1.z==25){
      nodeLoc1m=nodeLoc2;
      nodeLoc2m=nodeLoc1;
      isIndoor=nodeLoc1m.isIndoor;
    }
    int isLOSLink=-1;
    double distance=0,din=0,dout=0;double losThreshold=0;
    if(nodeLoc1m.isIndoor)
    {
      distance=findDistance(nodeLoc1,nodeLoc2);
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
      isLOSLink = isLOS;
    
    #ifdef DEBUG_COMP_CALIB_SUPPORT
    if(distance<500) cout<<"COMP DEBUG macro distance="<< distance<<", din="<<din<<",dout="<<dout<<",losThreshold="<<losThreshold<<",isLos="<<isLOS<<",indoor ="<<isIndoor<<", modIndoor= "<<nodeLoc1m.isIndoor<<", actual_los_prob="<<min(18.0 / distance, 1.0) * (1.0 - exp(-distance / 63.0)) + exp(-distance / 63.0)<<endl;
    #endif
    NodeLinkInfo_S nodeInfo=imtUrbanMacro(nodeLoc1m,  nodeLoc2m, carrierFreqInHz,  isLOSLink,terr);
        
    //since the imtUrbanMacro adds a 9 dB incar pathloss to remove it the following code
    nodeInfo.pathLoss-=9;
    // comp Calibration indoor case have 80% / 50% UE indoor in outdoor-indoor case, that is different from   imtUrbanMacro   so the penetration loss will be calculated here
    if(isIndoor==true)
    {
      
      
      nodeInfo.pathLoss += 20.0 + (0.5*din);
      nodeInfo.shadowStdDev=6.0;
    }
    
    return nodeInfo;
  }
     if(nodeLoc2.z==10||nodeLoc1.z==10)
    {
      Location_S nodeLoc1m;
      Location_S nodeLoc2m;
      bool isIndoor;
      if(nodeLoc2.z==10){
	nodeLoc1m=nodeLoc1;
	nodeLoc2m=nodeLoc2;
	isIndoor=nodeLoc1m.isIndoor;
      }
      else if(nodeLoc1.z==10){
	nodeLoc1m=nodeLoc2;
	nodeLoc2m=nodeLoc1;
	isIndoor=nodeLoc1m.isIndoor;
      }
      int isLOSLink=-1;
      double distance=0,din=0,dout=0;double losThreshold=0;
      if(nodeLoc1m.isIndoor)
      {
	distance=findDistance(nodeLoc1,nodeLoc2);
	din=randu()*min(25.0,distance);
	dout=distance;
	losThreshold = min(18.0 / dout , 1.0) * (1.0 - exp(-dout / 36.0)) + exp(-dout/ 36.0);
	isLOSLink=false;
	if (randu() < losThreshold )
	{
	  isLOSLink = true;
	}
	nodeLoc1m.isIndoor=false;
      }
      if(isLOS!=-1)
	isLOSLink = isLOS;
      NodeLinkInfo_S nodeInfo= imtUrbanMicro(nodeLoc1m,  nodeLoc2m, carrierFreqInHz,  isLOSLink,terr);
      
      //since the imtUrbanMacro adds a 9 dB incar pathloss to match with it the following 9 dB can be added to UMI pathloss---well this is very unlikely scenario
      //nodeInfo.pathLoss+=9;
      // comp Calibration indoor case have 80% / 50% UE indoor in outdoor-indoor case, that is different from   imtUrbanMacro   so the penetration loss will be calculated here
      #ifdef DEBUG_COMP_CALIB_SUPPORT
      if(distance<500) cout<<"COMP DEBUG micro distance="<< distance<<", din="<<din<<",dout="<<dout<<",losThreshold="<<losThreshold<<",isLos="<<isLOS<<",indoor ="<<isIndoor<<", modIndoor "<<nodeLoc1m.isIndoor<<",actual_los_prob="<<min(18.0 / distance, 1.0) * (1.0 - exp(-distance / 36.0)) + exp(-distance / 36.0)<<endl;
      #endif
      if(isIndoor==true)
      {
	
	
	nodeInfo.pathLoss += 20.0 + (0.5*din);
	nodeInfo.shadowStdDev=7.0;
      }
      return nodeInfo;
    }
    else{
      cout<<"[both:] Error: Scenario for mix of macro/ low power nodes are defined for two heights 25/10 m please check the input loc" <<nodeLoc2<<endl;
      exit(2);
    }
    
  }
