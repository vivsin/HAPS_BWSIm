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


void loadIMTTerrains(Terrain &terrain) {

    Array<string> terrainNames="{ _TERRAIN_IMT_URBAN_MICRO_ _TERRAIN_IMT_URBAN_MACRO_ _TERRAIN_IMT_SUBURBAN_MACRO_ _TERRAIN_IMT_RURAL_MACRO_ }";
    terrain.addTerrain(terrainNames,&getIMTPathLoss);
}


NodeLinkInfo_S getIMTPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr,NodeCategory_E /*nodeCategory1*/,NodeCategory_E /*nodeCategory2*/)
{
    string myTerrain=terr->getTerrain();
    if (myTerrain=="_TERRAIN_IMT_URBAN_MICRO_")
        return(imtUrbanMicro(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr));
    else if (myTerrain=="_TERRAIN_IMT_URBAN_MACRO_")
        return(imtUrbanMacro(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr));
    else if (myTerrain=="_TERRAIN_IMT_SUBURBAN_MACRO_")
        return(imtSubUrbanMacro(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS));
    else if (myTerrain=="_TERRAIN_IMT_RURAL_MACRO_")
      return(imtRuralMacro(nodeLoc1,nodeLoc2,carrierFreqInHz,txNodeType,rxNodeType,isLOS));
    else
    {
        cout<<"[both:] Undefined Terrain for IMT Pathloss model...."<<endl;
        exit(1);
    }
}


NodeLinkInfo_S imtUrbanMicro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr)
{
    

    Location_S nodeLoc1m;
    Location_S nodeLoc2m;

    if(nodeLoc2.z>nodeLoc1.z) {
        nodeLoc1m=nodeLoc1;
        nodeLoc2m=nodeLoc2;

    }
    else {
        nodeLoc1m=nodeLoc2;
        nodeLoc2m=nodeLoc1;

    }
    
    
    if (nodeLoc1m.isIndoor==true && nodeLoc2m.isIndoor==true)
    {
        cout<<"[both:] Error ... Indoor to Indoor model not currently supported in IMT UMi Pathloss ..."<<endl;
        exit(1);
    }

    NodeLinkInfo_S tempLinkInfo;

    tempLinkInfo.isLOS = false;
    if(terr->is3Ddistance)
      tempLinkInfo.distance=find3Ddistance(nodeLoc1m,nodeLoc2m);
    else
      tempLinkInfo.distance=findDistance(nodeLoc1m,nodeLoc2m);


    double losThreshold,breakDistance,carrierFrequency;
    carrierFrequency = carrierFreqInHz / 1e9;
    breakDistance = 4.0 * (nodeLoc2m.z - 1.0) * (nodeLoc1m.z - 1.0) * carrierFreqInHz / lightSpeed;
    
    //! Below loop commented to match the SINR in 36.814
    /*if (nodeLoc1m.isIndoor || nodeLoc2m.isIndoor) // Indoor users are always NLOS users
    {
      tempLinkInfo.isLOS=false;
    }
    else
    */
    {
     if (isLOS==-1)
      {
	  losThreshold = min(18.0 / tempLinkInfo.distance , 1.0) * (1.0 - exp(-tempLinkInfo.distance / 36.0)) + exp(-tempLinkInfo.distance/ 36.0);
	  if (randu() < losThreshold )
	    tempLinkInfo.isLOS = true;
      }
      else
	tempLinkInfo.isLOS = isLOS;
    }

    if (tempLinkInfo.isLOS)
    {
	tempLinkInfo.shadowStdDev = 3.0;
        if ((tempLinkInfo.distance >= 10) && (tempLinkInfo.distance <= breakDistance))
            tempLinkInfo.pathLoss = 22.0 * log10(tempLinkInfo.distance) + 28.0 + 20.0 * log10(carrierFrequency);
        else if ((tempLinkInfo.distance > breakDistance) && (tempLinkInfo.distance < 5000.0))
            tempLinkInfo.pathLoss = 40.0 * log10(tempLinkInfo.distance) + 7.8 - 18.0 * log10((nodeLoc2m.z - 1.0)) - 18.0 * log10((nodeLoc1m.z - 1.0)) + 2.0 * log10(carrierFrequency);
        else
            tempLinkInfo.pathLoss = 1.0e6;
    }
    else
    {
        tempLinkInfo.shadowStdDev=4.0;
        if ((tempLinkInfo.distance >= 10.0) && (tempLinkInfo.distance < 5000.0))
            tempLinkInfo.pathLoss = 36.7 * log10(tempLinkInfo.distance) + 22.7 + 26.0 * log10(carrierFrequency);
        else
            tempLinkInfo.pathLoss = 1.0e6;
    }
    
    if (nodeLoc1m.isIndoor || nodeLoc2m.isIndoor)
    {
        if (tempLinkInfo.distance>25)
            tempLinkInfo.pathLoss += 20.0 + (0.5*25*randu());
        else
            tempLinkInfo.pathLoss += 20.0 + (0.5*tempLinkInfo.distance*randu());//assuming a randomdistance of the total distance as din instead of 25m
        tempLinkInfo.shadowStdDev=7.0;
	tempLinkInfo.isLOS=false; // Irrespective of tossing Indoor user is always NLOS
    }
    
    return tempLinkInfo;

}

NodeLinkInfo_S imtUrbanMacro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr)
{

    Location_S nodeLoc1m;
    Location_S nodeLoc2m;

    if(nodeLoc2.z>nodeLoc1.z) {
        nodeLoc1m=nodeLoc1;
        nodeLoc2m=nodeLoc2;

    }
    else {
        nodeLoc1m=nodeLoc2;
        nodeLoc2m=nodeLoc1;

    }
  
    if (nodeLoc1m.isIndoor==true || nodeLoc2m.isIndoor==true)
    {
        cout<<"[both:] Error ... Indoor model not currently supported in IMT UMa Pathloss ..."<<endl;
        abort();
    }
   if (nodeLoc1m.isIndoor==true || nodeLoc2m.isIndoor==true)
   {
     isLOS = 0;
  }
    double losThreshold,breakDistance,carrierFrequency,avgW,avgH;

    NodeLinkInfo_S tempLinkInfo;
    tempLinkInfo.isLOS = false;
    if(terr->is3Ddistance)
      tempLinkInfo.distance=find3Ddistance(nodeLoc1m,nodeLoc2m);
    else
      tempLinkInfo.distance=findDistance(nodeLoc1m,nodeLoc2m);

    carrierFrequency = carrierFreqInHz / 1e9;
    breakDistance = 4.0 * (nodeLoc2m.z - 1.0) * (nodeLoc1m.z - 1.0) * carrierFreqInHz / lightSpeed;

    if (isLOS==-1)
    {
        losThreshold = min(18.0 / tempLinkInfo.distance , 1.0) * (1.0 - exp(-tempLinkInfo.distance / 63.0)) + exp(-tempLinkInfo.distance / 63.0);
        if (randu() < losThreshold)
            tempLinkInfo.isLOS = true;
    }
    else
        tempLinkInfo.isLOS = isLOS;

    if (tempLinkInfo.isLOS)
    {

        tempLinkInfo.shadowStdDev = 4.0;
        if ((tempLinkInfo.distance > 10.0) && (tempLinkInfo.distance <= breakDistance))
        {
            tempLinkInfo.pathLoss = 22.0 * log10(tempLinkInfo.distance) + 28.0 + 20.0 * log10(carrierFrequency);
        }
        else if ((tempLinkInfo.distance > breakDistance) && (tempLinkInfo.distance < 5000.0))
        {
            tempLinkInfo.pathLoss = 40.0 * log10(tempLinkInfo.distance) + 7.8 - 18.0 * log10((nodeLoc2m.z - 1.0)) - 18.0 * log10((nodeLoc1m.z - 1.0)) + 2.0 * log10(carrierFrequency);
        }
        else
        {
            tempLinkInfo.pathLoss = 1.0e6;
        }
    }
    else
    {
        avgH = 20;
        avgW = 20;

        tempLinkInfo.shadowStdDev = 6.0;
        if ((tempLinkInfo.distance >= 10.0) && (tempLinkInfo.distance < 5000.0))
        {
            tempLinkInfo.pathLoss = 161.04 - 7.1 * log10(avgW) + 7.5 * log10(avgH) - (24.37 - 3.7 * sqr((avgH / nodeLoc2m.z))) * log10(nodeLoc2m.z) 	+ (43.42 - 3.1 * log10(nodeLoc2m.z)) * (log10(tempLinkInfo.distance) - 3.0) + 20.0 * log10(carrierFrequency) - (3.2 * sqr(log10(11.75 * nodeLoc1m.z)) - 4.97);
        }
        else
        {
            tempLinkInfo.pathLoss = 1.0e6;
        }
    }
    //Outdoor to in-car penetration loss
    tempLinkInfo.pathLoss += 9.0;
    return tempLinkInfo;
}

NodeLinkInfo_S imtSubUrbanMacro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS)
{
  
    Location_S nodeLoc1m;
    Location_S nodeLoc2m;

    if(nodeLoc2.z>nodeLoc1.z) {
        nodeLoc1m=nodeLoc1;
        nodeLoc2m=nodeLoc2;

    }
    else {
        nodeLoc1m=nodeLoc2;
        nodeLoc2m=nodeLoc1;

    }

    if (nodeLoc1m.isIndoor==true || nodeLoc2m.isIndoor==true)
    {
        cout<<"[both:] Error ... Indoor model not currently supported in IMT Sub-UMa Pathloss ..."<<endl;
        exit(1);
    }

    //this is same as rural in first glance except for building height h=10m instead of 5m

    double losThreshold,breakDistance,carrierFrequency,avgW,avgH;

    NodeLinkInfo_S tempLinkInfo;
    tempLinkInfo.isLOS = false;
    tempLinkInfo.distance=findDistance(nodeLoc1m,nodeLoc2m);

    carrierFrequency = carrierFreqInHz / 1e9;
    if (isLOS==-1)
    {
        if (tempLinkInfo.distance < 10.0)
        {
            tempLinkInfo.isLOS=true;
        }
        else
        {
            losThreshold = exp(-(tempLinkInfo.distance - 10.0) / 1e3);
            if (randu() < losThreshold)
            {
                tempLinkInfo.isLOS=true;
            }
        }
    }
    else
        tempLinkInfo.isLOS=isLOS;

    avgH = 10;
    avgW = 20;
    breakDistance = 2.0 * pi* nodeLoc2m.z * nodeLoc1m.z * carrierFreqInHz / lightSpeed;

    if (tempLinkInfo.isLOS)
    {

        if ((tempLinkInfo.distance >= 10.0) && (tempLinkInfo.distance <= breakDistance))
        {
            tempLinkInfo.shadowStdDev= 4.0;
            tempLinkInfo.pathLoss = 20.0 * log10(40.0 * pi* tempLinkInfo.distance * carrierFrequency / 3.0) + min(0.03 * pow(avgH,1.72),10.0) * log10(tempLinkInfo.distance) - min(0.044 * pow(avgH,1.72),14.77) + 0.002 * log10(avgH) * tempLinkInfo.distance;
        }
        else if ((tempLinkInfo.distance > breakDistance) && (tempLinkInfo.distance < 10000.0))
        {
            tempLinkInfo.shadowStdDev= 6.0;
            tempLinkInfo.pathLoss = 20.0 * log10(40.0 *pi* breakDistance * carrierFrequency / 3.0) + min(0.03 * pow(avgH,1.72),10.0) * log10(breakDistance) - min(0.044 * pow(avgH,1.72),14.77) + 0.002 * log10(avgH) * breakDistance;
            tempLinkInfo.pathLoss += 40.0 * log10(tempLinkInfo.distance / breakDistance);
        }
        else
        {
            tempLinkInfo.shadowStdDev= 0.0;
            tempLinkInfo.pathLoss = 1.0e6;
        }
    }
    else
    {

        if ((tempLinkInfo.distance > 10.0) && (tempLinkInfo.distance < 5000.0))
        {
            tempLinkInfo.shadowStdDev= 8.0;
            tempLinkInfo.pathLoss = 161.04 - 7.1 * log10(avgW) + 7.5 * log10(avgH) - (24.37 - 3.7 * sqr((avgH / nodeLoc2m.z))) * log10(nodeLoc2m.z) + (43.42 - 3.1 * log10(nodeLoc2m.z)) * (log10(tempLinkInfo.distance) - 3.0) + 20.0 * log10(carrierFrequency) - (3.2 * sqr(log10(11.75 * nodeLoc1m.z)) - 4.97);
        }
        else
        {
            tempLinkInfo.shadowStdDev= 0.0;
            tempLinkInfo.pathLoss = 1.0e6;
        }
    }
    //Outdoor to in-car penetration loss
    tempLinkInfo.pathLoss += 9.0;

    return tempLinkInfo;
}

NodeLinkInfo_S imtRuralMacro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int txNodeType, int rxNodeType, int isLOS)
{
  cout<<"*****************************evaluating path loss in imtRuralRuralMacro function*******************"<<endl;
    Location_S nodeLoc1m;
    Location_S nodeLoc2m;

    if(nodeLoc2.z>nodeLoc1.z) {
        nodeLoc1m=nodeLoc1;
        nodeLoc2m=nodeLoc2;

    }
    else {
        nodeLoc1m=nodeLoc2;
        nodeLoc2m=nodeLoc1;

    }
    if (nodeLoc1m.isIndoor==true || nodeLoc2m.isIndoor==true)
    {
        cout<<"[both:] Error ... Indoor model not currently supported in IMT RMa Pathloss ..."<<endl;
        exit(1);
    }

    double losThreshold,breakDistance,carrierFrequency,avgW,avgH;

    NodeLinkInfo_S tempLinkInfo;
    tempLinkInfo.isLOS = false;
    tempLinkInfo.distance=findDistance(nodeLoc1m,nodeLoc2m);


    carrierFrequency = carrierFreqInHz / 1e9;
    if (isLOS==-1)
    {
        if (tempLinkInfo.distance < 10.0)
        {
            tempLinkInfo.isLOS=true;
        }
        else
        {
            losThreshold = exp(-(tempLinkInfo.distance - 10.0) / 1e3);
            if (randu() < losThreshold)
            {
                tempLinkInfo.isLOS=true;
            }
        }
    }
    else
        tempLinkInfo.isLOS=isLOS;

    avgH = 5;
    avgW = 20;
    breakDistance = 2.0 * pi* nodeLoc2m.z * nodeLoc1m.z * carrierFreqInHz / lightSpeed;
    cout<<" break distance :"<<breakDistance<<endl;
    if (tempLinkInfo.isLOS)
    {

        if ((tempLinkInfo.distance >= 10.0) && (tempLinkInfo.distance <= breakDistance))
        {
            tempLinkInfo.shadowStdDev= 4.0;
            tempLinkInfo.pathLoss = 20.0 * log10(40.0 *pi* tempLinkInfo.distance * carrierFrequency / 3.0) + min(0.03 * pow(avgH,1.72),10.0) * log10(tempLinkInfo.distance) - min(0.044 * pow(avgH,1.72),14.77) + 0.002 * log10(avgH) * tempLinkInfo.distance;
        }
        else if ((tempLinkInfo.distance > breakDistance) && (tempLinkInfo.distance < 10000.0))
        {
            tempLinkInfo.shadowStdDev= 6.0;
            tempLinkInfo.pathLoss = 20.0 * log10(40.0 *pi* breakDistance * carrierFrequency / 3.0) + min(0.03 * pow(avgH,1.72),10.0) * log10(breakDistance) - min(0.044 * pow(avgH,1.72),14.77) + 0.002 * log10(avgH) * breakDistance;
            tempLinkInfo.pathLoss += 40.0 * log10(tempLinkInfo.distance / breakDistance);
        }
        else
        {
            tempLinkInfo.shadowStdDev= 0.0;
            tempLinkInfo.pathLoss = 1.0e6;
        }
    }
    else
    {
        // LMLC changes
        double PL_LOS, PL_NLOS;
        if ((tempLinkInfo.distance >= 10.0) && (tempLinkInfo.distance <= breakDistance))
        {
          tempLinkInfo.shadowStdDev= 8.0;
          PL_LOS = 20.0 * log10(40.0 *pi* tempLinkInfo.distance * carrierFrequency / 3.0) + min(0.03 * pow(avgH,1.72),10.0) * log10(tempLinkInfo.distance) - min(0.044 * pow(avgH,1.72),14.77) + 0.002 * log10(avgH) * tempLinkInfo.distance;
          
          PL_NLOS = -12 + 161.04 - 7.1 * log10(avgW) + 7.5 * log10(avgH) - (24.37 - 3.7 * sqr((avgH / nodeLoc2m.z))) * log10(nodeLoc2m.z) + (43.42 - 3.1 * log10(nodeLoc2m.z)) * (log10(tempLinkInfo.distance) - 3.0) + 20.0 * log10(carrierFrequency) - (3.2 * sqr(log10(11.75 * nodeLoc1m.z)) - 4.97);
          
          tempLinkInfo.pathLoss = max(PL_LOS,PL_NLOS);
        }
        else if ((tempLinkInfo.distance > breakDistance) && (tempLinkInfo.distance < 100000.0))
        {
          tempLinkInfo.shadowStdDev= 8.0;
          PL_LOS = 20.0 * log10(40.0 *pi* breakDistance * carrierFrequency / 3.0) + min(0.03 * pow(avgH,1.72),10.0) * log10(breakDistance) - min(0.044 * pow(avgH,1.72),14.77) + 0.002 * log10(avgH) * breakDistance;
          PL_LOS += 40.0 * log10(tempLinkInfo.distance / breakDistance);
          
          PL_NLOS = -12 + 161.04 - 7.1 * log10(avgW) + 7.5 * log10(avgH) - (24.37 - 3.7 * sqr((avgH / nodeLoc2m.z))) * log10(nodeLoc2m.z) + (43.42 - 3.1 * log10(nodeLoc2m.z)) * (log10(tempLinkInfo.distance) - 3.0) + 20.0 * log10(carrierFrequency) - (3.2 * sqr(log10(11.75 * nodeLoc1m.z)) - 4.97);
          
          tempLinkInfo.pathLoss = max(PL_LOS,PL_NLOS);
          
        }
        else
        {
            tempLinkInfo.shadowStdDev= 0.0;
            tempLinkInfo.pathLoss = 1.0e6;
        }
    }
    
    if (nodeLoc1m.isIndoor || nodeLoc2m.isIndoor)
    {
      if (tempLinkInfo.distance>25)
        tempLinkInfo.pathLoss += 20.0 + (0.5*25*randu());
      else
        tempLinkInfo.pathLoss += 20.0 + (0.5*tempLinkInfo.distance*randu());//assuming a randomdistance of the total distance as din instead of 25m
      tempLinkInfo.shadowStdDev=8.0;
      tempLinkInfo.isLOS=false; // Irrespective of tossing Indoor user is always NLOS
    }
    
    //Outdoor to in-car penetration loss
    if(txNodeType == 7 || rxNodeType ==7)
      tempLinkInfo.pathLoss += 9.0;
    
    return tempLinkInfo;
}




