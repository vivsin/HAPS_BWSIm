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
// #define _CHANN_CALIB_

bool load5GTerrains(Terrain &terrain) {
  
  bool successFlag = false;
  Array<string> terrainNames="{_TERRAIN_5G_RURAL_MACRO_ _TERRAIN_5G_RURAL_MACRO_LOS_ _TERRAIN_5G_RURAL_MACRO_NLOS_ " 
                              "_TERRAIN_5G_URBAN_MACRO_ _TERRAIN_5G_URBAN_MACRO_LOS_ _TERRAIN_5G_URBAN_MACRO_NLOS_ "
                              "_TERRAIN_5G_URBAN_MICRO_ _TERRAIN_5G_URBAN_MICRO_LOS_ _TERRAIN_5G_URBAN_MICRO_NLOS_ " 
                              "_TERRAIN_5G_INDOOR_ _TERRAIN_5G_INDOOR_LOS_ _TERRAIN_5G_INDOOR_NLOS_ "
                              "_TERRAIN_5G_INDOOR_FACTORY_ _TERRAIN_5G_INDOOR_FACTORY_LOS_ _TERRAIN_5G_INDOOR_FACTORY_NLOS_}";
//   terrain.addTerrain(terrainNames,&get5GChannelPathLoss);
//   successFlag = true;
//   return successFlag;
  
  int index = findInStringArray(terrainNames,terrain.getTerrain());
  if(index!=-1)
  {
      terrain.addTerrain(terrain.getTerrain(),&get5GChannelPathLoss);
      return true;
  }
  return loadNTNTerrains(terrain);
}
std::map<int,double>indoorDist;
typedef std::pair<int, int> pairIxI;
std::map<pairIxI,NodeLinkInfo_S> linkInfoMap;

NodeLinkInfo_S getLinfoMap(pairIxI P)
{
  std::map<pairIxI,NodeLinkInfo_S> ::iterator it=linkInfoMap.find(P);
  if(it != linkInfoMap.end())
  {
    return it->second;
  }
  else
  {
    NodeLinkInfo_S x;
    return x;
  }
}

void setLinkInfoMap(pairIxI P, NodeLinkInfo_S linkInfo)
{
  linkInfoMap[P] = linkInfo;
}
double getIndoorDistance5G(int LocId)
{
  std::map<int,double>::iterator it=indoorDist.find(LocId);
  if(it!= indoorDist.end())
  {
    return it->second;
  }
  else{
  double  d2Din_1=25.0*randu();
   double d2Din_2=25.0*randu();
     
  double  d2Din = min(d2Din_1,d2Din_2);
  indoorDist[LocId]=d2Din;
  return d2Din;
  }
}
NodeLinkInfo_S get5GChannelPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr,NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2)
{
  int ueID,bsID;
  if(nodeCategory1 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
  {
    bsID=nodeLoc1.getLocId();
    ueID = nodeLoc2.getLocId();
  }
  else if(nodeCategory2 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
  {
    bsID=nodeLoc2.getLocId();
    ueID = nodeLoc1.getLocId();
  }
  else
  {
    cout<<"One of the node should be a server node...Exiting"<<endl;
    abort();
  }
  string myTerrain=terr->getTerrain();
  ChannelModelType channelModelType = terr->channelModelType;
  bool isLMLC = terr->isLMLC;
  if (myTerrain=="_TERRAIN_5G_RURAL_MACRO_" || myTerrain=="_TERRAIN_5G_RURAL_MACRO_LOS_" || myTerrain=="_TERRAIN_5G_RURAL_MACRO_NLOS_")
  {
    #ifdef _CHANN_CALIB_
    NodeLinkInfo_S linkInfoCalib;
    linkInfoCalib = pathLoss5GRuralMacro(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,nodeCategory1,nodeCategory2);
    pairIxI P(ueID,bsID);
    setLinkInfoMap(P,linkInfoCalib);

    isLOS  =1;
    #endif
    
    if(myTerrain=="_TERRAIN_5G_RURAL_MACRO_LOS_") 
        isLOS = 1;
    else if(myTerrain=="_TERRAIN_5G_RURAL_MACRO_NLOS_")
        isLOS = 0;
    
    return(pathLoss5GRuralMacro(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,nodeCategory1,nodeCategory2,channelModelType,isLMLC));
  }
  else if (myTerrain=="_TERRAIN_5G_URBAN_MACRO_" || myTerrain=="_TERRAIN_5G_URBAN_MACRO_LOS_" || myTerrain=="_TERRAIN_5G_URBAN_MACRO_NLOS_")
  {
    #ifdef _CHANN_CALIB_
    NodeLinkInfo_S linkInfoCalib;
    linkInfoCalib = pathLoss5GUrbanMacro(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,nodeCategory1,nodeCategory2);
    pairIxI P(nodeLoc1.getLocId(),nodeLoc2.getLocId());
//     pairIxI P(bsID,ueID);
    setLinkInfoMap(P,linkInfoCalib);
//     pairIxI Q(bsID,ueID);
//     setLinkInfoMap(Q,linkInfoCalib);
    isLOS  =-1;
    
    cout<<"nodeLoc1.getLocId():"<<nodeLoc1.getLocId()<<"\tnodeLoc2.getLocId():"<<nodeLoc2.getLocId()<<"\t linkInfoCalib.shadowStdDev:"<<linkInfoCalib.shadowStdDev<<endl;
    #endif
    
    if(myTerrain=="_TERRAIN_5G_URBAN_MACRO_LOS_") 
        isLOS = 1;
    else if(myTerrain=="_TERRAIN_5G_URBAN_MACRO_NLOS_")
        isLOS = 0;
    
    return(pathLoss5GUrbanMacro(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr,nodeCategory1,nodeCategory2,channelModelType));
  }
  else if (myTerrain=="_TERRAIN_5G_URBAN_MICRO_" || myTerrain=="_TERRAIN_5G_URBAN_MICRO_LOS_" || myTerrain=="_TERRAIN_5G_URBAN_MICRO_NLOS_")
  {
    #ifdef _CHANN_CALIB_
    NodeLinkInfo_S linkInfoCalib;
    linkInfoCalib = pathLoss5GUrbanMicro(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr,nodeCategory1,nodeCategory2);
    pairIxI P(ueID,bsID);
    setLinkInfoMap(P,linkInfoCalib);

    isLOS = 1;

    #endif
    
    if(myTerrain=="_TERRAIN_5G_URBAN_MICRO_LOS_") 
        isLOS = 1;
    else if(myTerrain=="_TERRAIN_5G_URBAN_MICRO_NLOS_")
        isLOS = 0;
    
    return(pathLoss5GUrbanMicro(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr,nodeCategory1,nodeCategory2,channelModelType));
  }
  else if (myTerrain=="_TERRAIN_5G_INDOOR_" || myTerrain=="_TERRAIN_5G_INDOOR_LOS_" || myTerrain=="_TERRAIN_5G_INDOOR_NLOS_")
  {
    #ifdef _CHANN_CALIB_
    NodeLinkInfo_S linkInfoCalib;
    linkInfoCalib = pathLoss5GIndoor(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr,nodeCategory1,nodeCategory2,channelModelType);
    pairIxI P(ueID,bsID);
    setLinkInfoMap(P,linkInfoCalib);
    isLOS  =1;
    #endif
    
    if(myTerrain=="_TERRAIN_5G_INDOOR_LOS_") 
        isLOS = 1;
    else if(myTerrain=="_TERRAIN_5G_INDOOR_NLOS_")
        isLOS = 0;
    
    return(pathLoss5GIndoor(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr,nodeCategory1,nodeCategory2,channelModelType));
  }
  else if(myTerrain=="_TERRAIN_5G_INDOOR_FACTORY_" ||  myTerrain=="_TERRAIN_5G_INDOOR_FACTORY_LOS_" || myTerrain=="_TERRAIN_5G_INDOOR_FACTORY_NLOS_")
  {
      if(myTerrain=="_TERRAIN_5G_INDOOR_FACTORY_LOS_") 
          isLOS = 1;
      else if(myTerrain=="_TERRAIN_5G_INDOOR_FACTORY_NLOS_")
          isLOS = 0;
      
      return(pathLoss5GIndoorFactory(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr,nodeCategory1,nodeCategory2,channelModelType));
  }
  else
  {
    cout<<"[both:] Unknown terrain in get5GChannelPathLoss.. aborting"<<endl;abort();
  }
}

double computeO2IpenetrationLoss(ChannelModelType channelModelType,PenetrationModel_E OtoIPenetrationModel,double carrierFreqInGigaHz,double d2Din,bool isUrban)
{
    double PL_tw,PL_in,sigma_p;
    if(channelModelType == _MODEL_A_ && carrierFreqInGigaHz<=6.0)
    {
        PL_tw = (isUrban) ? 20.0 : 10.0;
        sigma_p = 0.0;
    }
    else
    {
        if(OtoIPenetrationModel == O_TO_I_PENETRATION_LOW)
        {
            double L_glass = 2.0 + 0.2*carrierFreqInGigaHz;
            double L_concrete = 5.0 + 4.0*carrierFreqInGigaHz;
            PL_tw = 5.0 - dB(0.3*inv_dB(-L_glass) + 0.7*inv_dB(-L_concrete));	
            sigma_p = 4.4;
        }
        else if(OtoIPenetrationModel == O_TO_I_PENETRATION_HIGH)
        {
            double L_IRR_glass = 23.0 + 0.3*carrierFreqInGigaHz;
            double L_concrete = 5.0 + 4.0*carrierFreqInGigaHz;
            PL_tw = 5.0 - dB(0.7*inv_dB(-L_IRR_glass) + 0.3*inv_dB(-L_concrete));
            sigma_p = 6.5;
        }
        else
        {
            double L_glass = 2.0 + 0.2*carrierFreqInGigaHz;
            double L_concrete = 5.0 + 4.0*carrierFreqInGigaHz;
            PL_tw = 5.0 - dB(0.3*inv_dB(-L_glass) + 0.7*inv_dB(-L_concrete));	
            sigma_p = 4.4;
            cout<<"No specific O to I penetration model specified .. Selecting O_TO_I_PENETRATION_LOW as default"<<endl;
        }
    }
    PL_in = 0.5*d2Din;
    double penetrationLoss = PL_tw + PL_in + gaussian(0,sqr(sigma_p));
    return penetrationLoss;
}


bool isLOS_RMa(double d2Dout)
{
    double PLos = 0.0;
    
    if(d2Dout <= 10.0)
        PLos = 1.0; 
    else
        PLos = exp((-1.0*(d2Dout - 10.0))/1000.0);
    
    return (bool)(randu()<PLos);
}

NodeLinkInfo_S pathLoss5GRuralMacro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType, bool isLMLC)
{

    NodeLinkInfo_S tempLinkInfo;  
    int isUEIndoor=-1;
    int isWithLowLoss  =-1;
    double d2Dout = 0.0;
    double d2Din = 0.0;
    double d2D = 0.0;
    double carrierFreqInGigaHz;
    
    double ueHeight;
    double bsHeight;	
    double temp_LOS_PathLoss=0.0;
    double temp_NLOS_PathLoss=0.0;
    double shadowStdDev = 0.0;
    tempLinkInfo.pathLoss=0.0;
    double dBP;
    double d3D;
    double h = 5; // Avg building height
    double W = 20; // Avg Street width
    double indoorDistance = 0.0;
    
    carrierFreqInGigaHz=carrierFreqInHz/pow(10.0,9); 	
    bool isPedestrian;
    double carLoss=0.0,o2iLoss=0.0;
    
    if(nodeCategory1 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
    {
        bsHeight=nodeLoc1.z;
        ueHeight=nodeLoc2.z;
        isUEIndoor = nodeLoc2.isIndoor;
        isPedestrian = nodeLoc2.isPedestrian;
        isWithLowLoss = nodeLoc2.isLowPenetrationLoss;
        indoorDistance = nodeLoc2.indoorDistance;
        carLoss = nodeLoc2.carLoss;
        o2iLoss = nodeLoc2.o2iLoss;
    }
    else if(nodeCategory2 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
    {
        bsHeight=nodeLoc2.z;
        ueHeight=nodeLoc1.z;
        isUEIndoor = nodeLoc1.isIndoor;
        isPedestrian = nodeLoc1.isPedestrian;
        isWithLowLoss = nodeLoc1.isLowPenetrationLoss;
        indoorDistance = nodeLoc1.indoorDistance;
        carLoss = nodeLoc1.carLoss;
        o2iLoss = nodeLoc1.o2iLoss;
    }
    else
    {
        cout<<"One of the node should be a server node...Exiting"<<endl;
        abort();
    }
    
    d2D = findDistance(nodeLoc1,nodeLoc2);
    d3D = find3Ddistance(nodeLoc1,nodeLoc2);
    tempLinkInfo.distance=d3D;
    
    if(isUEIndoor)
    {
        d2Din = indoorDistance;
        
        if(d2Din > d2D)
        {
            d2Din = d2D;
            d2Dout = 0.0;
        }
        else
            d2Dout = d2D - d2Din;
    }
    else    //OutDoor -> in car 
    {
        d2Dout = d2D;
        d2Din = 0;
    }
    
    dBP = 2.0*pi*(bsHeight)*(ueHeight)*carrierFreqInHz/lightSpeed;
    
    //-------------------LOS probability calculation----------------------------
    if(isLOS==-1)
    {
        tempLinkInfo.isLOS=isLOS_RMa(d2Dout);
    }
    else
        tempLinkInfo.isLOS=isLOS;

    
    //----------------------------------------------------------------------------
    
    double PL1 = 20*log10(40*pi*d3D*carrierFreqInGigaHz/3) + min(0.03*pow(h,1.72),10.0)*log10(d3D) - min(0.044*pow(h,1.72),14.77) + 0.002*log10(h)*d3D;
    double PL2 = PL1 + 40*log10(d3D/dBP);
    if(d2D>=10 && d2D<=dBP)
    {
        temp_LOS_PathLoss = PL1;
        shadowStdDev = 4.0;
    }
    else if(d2D>dBP && d2D<=21000)
    {
        temp_LOS_PathLoss = PL2;
        shadowStdDev = 6.0;
    }
    else 
    {
        temp_LOS_PathLoss = 5000;
    }
    
    if(tempLinkInfo.isLOS)
    {
        tempLinkInfo.pathLoss = temp_LOS_PathLoss;
        tempLinkInfo.shadowStdDev = shadowStdDev;
    }
    else
    {
        temp_NLOS_PathLoss = 161.04 - 7.1*log10(W)+7.5*log10(h)-(24.37 - 3.7*pow((h/bsHeight),2))*log10(bsHeight)+(43.42-3.1*log10(bsHeight))*(log10(d3D)-3.0)+20.0*log10(carrierFreqInGigaHz)-(3.2*pow(log10(11.75*ueHeight),2)-4.97);
        tempLinkInfo.pathLoss = max(temp_LOS_PathLoss,temp_NLOS_PathLoss - int(isLMLC)*12.0);
        tempLinkInfo.shadowStdDev = 8.0;
    }
    tempLinkInfo.pathLoss += o2iLoss + carLoss;
//     cout<<"pathloss , o2iLoss , carLoss :"<<tempLinkInfo.pathLoss <<","<<o2iLoss<<","<<carLoss<<endl;
    return tempLinkInfo;
}

double getC(double d2D,double h_UT)
{
    double g = (d2D>18.0) ? 1.25e-6*pow(d2D,3.0)*exp(-d2D/150.0) : 0;
    double C = (h_UT>13.0) ?  C = pow((h_UT-13.0)/10,1.5)*g : 0;
    return C;
}

bool isLOS_UMa(double d2Dout, double ueHeight)
{
    double PLos = 0;
    #ifdef _USING_CALIBRATION_
    PLos = min(18.0/d2Dout,1.0)*(1-exp(-d2Dout/63.0)) + exp(-d2Dout/36.0)*(1+getC(d2Dout,ueHeight));
    #else
    double C_dash = (ueHeight<13.0) ? 0.0 : pow(((ueHeight-13.0)/10.0),1.5);
    if(d2Dout<=18)
        PLos = 1;
    else	
        PLos = ((18.0/d2Dout)+ exp((-1.0*d2Dout)/63.0)*(1.0-(18.0/d2Dout)))*(1.0+(C_dash*1.25*pow((d2Dout/100),3)*exp(-d2Dout/150))) ;
    #endif
    return (bool)(randu()<PLos);
}


NodeLinkInfo_S pathLoss5GUrbanMacro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType)
{
    NodeLinkInfo_S tempLinkInfo;
    int isUEIndoor=-1;
    int isWithLowLoss = -1;
    bool isPedestrian =0;
    double d2Dout = 0.0;
    double d2Din = 0.0;
    double d2D = 0.0;
    double carrierFreqInGigaHz;
    double indoorDistance = 0.0;
    
    //double PL;
    double ueHeight;
    double bsHeight;	
    double temp_LOS_PathLoss=0.0;
    double temp_NLOS_PathLoss=0.0;
    tempLinkInfo.pathLoss=0.0;
    double dBP;
    double d3D;
    double carLoss=0.0,o2iLoss=0.0;
    carrierFreqInGigaHz=carrierFreqInHz/pow(10.0,9); 	
    if(nodeCategory1 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
    {
        bsHeight=nodeLoc1.z;
        ueHeight=nodeLoc2.z;
        isUEIndoor = nodeLoc2.isIndoor;
        isWithLowLoss = nodeLoc2.isLowPenetrationLoss;
        isPedestrian = nodeLoc2.isPedestrian;
        indoorDistance = nodeLoc2.indoorDistance;
        carLoss = nodeLoc2.carLoss;
        o2iLoss = nodeLoc2.o2iLoss;
    }
    else if(nodeCategory2 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
    {
        bsHeight=nodeLoc2.z;
        ueHeight=nodeLoc1.z;
        isUEIndoor = nodeLoc1.isIndoor;
        isWithLowLoss = nodeLoc1.isLowPenetrationLoss;
        isPedestrian = nodeLoc1.isPedestrian;
        indoorDistance = nodeLoc1.indoorDistance;
        carLoss = nodeLoc1.carLoss;
        o2iLoss = nodeLoc1.o2iLoss;
    }
    else
    {
        cout<<"One of the node should be a server node...Exiting"<<endl;
        abort();
    }
//     if(bsHeight!=25.0)
//     {
//         cout<<"UMa PathLoss Equations Assume BS Height to be 25m..."<<endl;abort();
//     }
    d2D = findDistance(nodeLoc1,nodeLoc2);
    if(isUEIndoor)
    {
        d2Din = indoorDistance;
        
        if(d2Din > d2D)
        {
            d2Din = d2D;
            d2Dout = 0.0;
        }
        else
        {
            d2Dout=d2D - d2Din;
        }
    }
    else//OutDoor
    {
        d2Dout = d2D;
        d2Din = 0;
    }
    
    d3D = sqrt(pow(d2D,2.0) + pow((bsHeight-ueHeight),2.0));
    tempLinkInfo.distance=d3D;
    
    if(ueHeight > 22.5 || ueHeight < 1.5)
    {
        cout<<"ueheight should not be more than 22.5 or less than 1.5. Assumption max building floor 8... aborting.."<<endl;
        abort();
    }
    //-------------------LOS probability calculation----------------------------
    
    if(isLOS==-1)
    {
        tempLinkInfo.isLOS=isLOS_UMa(d2Dout,ueHeight);
    }
    else
        tempLinkInfo.isLOS=isLOS;
    //--------------------------------------------------------------------------
    
    
    double effectiveHeight;
    #ifdef _USING_CALIBRATION_
    if((randu() <= 1.0/(1.0 + getC(d2D,ueHeight)) && tempLinkInfo.isLOS) || ueHeight<12)
        effectiveHeight=1.0;
    else
    {
        vec temp_Vec=getVector(12,ueHeight-1.5,3);
        effectiveHeight=temp_Vec(randi(0,temp_Vec.length()-1));
    }
    #else
    if(randu() <= 1.0/(1.0 + getC(d2D,ueHeight)))
        effectiveHeight=1.0;
    else
    {
        vec temp_Vec=getVector(12,ueHeight-1.5,3);
        effectiveHeight=temp_Vec(randi(0,temp_Vec.length()-1));
    }
    #endif
    dBP = 4.0*(bsHeight-effectiveHeight)*(ueHeight-effectiveHeight)*carrierFreqInGigaHz*pow(10.0,9)/lightSpeed;  
    
    if(dBP<=0.0)
    {
        cout<<dBP<<" "<<ueHeight<<" "<<effectiveHeight<<endl;
        cout<<"dBP cannot be negative..."<<endl;abort();
    }
    
    if((d2D>=10.0)&&(d2D<dBP))
    {
        temp_LOS_PathLoss = 28.0 + 22*log10(d3D) + 20.0*log10(carrierFreqInGigaHz);
    }
    else if((d2D>=dBP)&&(d2D<=5000.0))
    {
        temp_LOS_PathLoss = 28.0 + 40.0*log10(d3D) + 20.0*log10(carrierFreqInGigaHz) - 9.0*log10(pow(dBP,2) + pow((bsHeight-ueHeight),2));
    }
    else if(d2D>5000.0)
    {
        temp_LOS_PathLoss = 5000;
    }
    if(tempLinkInfo.isLOS)
    {
        tempLinkInfo.pathLoss = temp_LOS_PathLoss;
        tempLinkInfo.shadowStdDev = 4.0;
    }
    else
    {
        bool optionalPathLossEqEnabled = terr->useOptionalPathLoss;
        if(!optionalPathLossEqEnabled)
        {
            if(channelModelType == _MODEL_B_ || (carrierFreqInGigaHz>6 && channelModelType == _MODEL_A_))
            {
                temp_NLOS_PathLoss = 13.54 + 39.08*log10(d3D) + 20*log10(carrierFreqInGigaHz) - 0.6*(ueHeight - 1.5);
            }
            else if(channelModelType == _MODEL_A_)
            {
                double h = 20.0;
                double W = 20.0;
                temp_NLOS_PathLoss = 161.04 - 7.1*log10(W) +7.5*log10(h)- (24.37-3.7*sqr(h/bsHeight))*log10(bsHeight)+(43.42-3.1*log10(bsHeight))*(log10(d3D)-3)+ 20*log10(carrierFreqInGigaHz) -(3.2*sqr(log10(17.625))-4.97)- 0.6*(ueHeight - 1.5);
            }
            else
            {
                cout<<"Undefined Channel Model Type..."<<endl;abort();
            }
            tempLinkInfo.pathLoss = max(temp_LOS_PathLoss,temp_NLOS_PathLoss);
            tempLinkInfo.shadowStdDev = 6.0;
        }
        else
        {
            tempLinkInfo.pathLoss = 32.4 + 20*log10(d3D) + 30.0*log10(carrierFreqInGigaHz);
            tempLinkInfo.shadowStdDev=7.8;
        }
    }
    tempLinkInfo.pathLoss += o2iLoss + carLoss;
    return (tempLinkInfo);
}


bool isLOS_UMi(double d2Dout)
{
    double PLos = 0;
    #ifdef _USING_CALIBRATION_
    PLos = min(18.0/d2Dout,1.0)*(1-exp(-d2Dout/36.0)) + exp(-d2Dout/36.0);
    #else
    if(d2Dout<=18)
        PLos = 1;
    else	
        PLos = (18.0/d2Dout)+ exp((-1.0*d2Dout)/36.0)*(1.0-(18.0/d2Dout)) ;
    #endif
    return (bool)(randu()<PLos); 
}

NodeLinkInfo_S pathLoss5GUrbanMicro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType)
{    
    NodeLinkInfo_S tempLinkInfo;
    int isUEIndoor=-1;  
    int isWithLowLoss  =-1;
    bool isPedestrian =0;
    double d2Dout;
    double d2Din;
    double d2D;
    double dBP;
    double d3D;
    double ueHeight;
    double bsHeight;  
    double temp_LOS_PathLoss=0.0;
    double temp_NLOS_PathLoss=0.0;
    double shadowStdDev =0.0;
    tempLinkInfo.pathLoss=0.0;
    double carrierFreqInGigaHz=carrierFreqInHz/pow(10.0,9); 
    double indoorDistance = 0.0;
    double carLoss=0.0,o2iLoss=0.0;
    
    if(nodeCategory1 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
    {
        bsHeight=nodeLoc1.z;
        ueHeight=nodeLoc2.z;
        isUEIndoor = nodeLoc2.isIndoor;
        isWithLowLoss = nodeLoc2.isLowPenetrationLoss;
        isPedestrian = nodeLoc2.isPedestrian;
        indoorDistance = nodeLoc2.indoorDistance;
        carLoss = nodeLoc2.carLoss;
        o2iLoss = nodeLoc2.o2iLoss;
    }
    else if(nodeCategory2 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
    {
        bsHeight=nodeLoc2.z;
        ueHeight=nodeLoc1.z;
        isUEIndoor = nodeLoc1.isIndoor;
        isWithLowLoss = nodeLoc1.isLowPenetrationLoss;
        isPedestrian = nodeLoc1.isPedestrian;
        indoorDistance = nodeLoc1.indoorDistance;
        carLoss = nodeLoc1.carLoss;
        o2iLoss = nodeLoc1.o2iLoss;
    }
    else
    {
        cout<<"One of the node should be a server node...Exiting"<<endl;
        abort();
    }
//     if(bsHeight!=10.0)
//     {
//         cout<<"UMi pathLoss Equations Assume BS Height to be 10m..."<<endl;abort();
//     }
    d2D = findDistance(nodeLoc1,nodeLoc2);	
    if(isUEIndoor)
    {
        d2Din = indoorDistance;
        
        if(d2Din > d2D)
        {
            d2Din = d2D;
            d2Dout = 0.0;
        }
        else
        {
            d2Dout=d2D - d2Din;
        }
    }
    else//OutDoor
    {
        d2Dout = d2D;
        d2Din = 0;
    }
    
    dBP = 4.0*(bsHeight-1)*(ueHeight-1)*carrierFreqInGigaHz*pow(10.0,9)/lightSpeed;  
    
    d3D = find3Ddistance(nodeLoc1,nodeLoc2);
    tempLinkInfo.distance=d3D;
    
    //-------------------LOS probability calculation----------------------------
    if(isLOS==-1)
    {
        tempLinkInfo.isLOS=isLOS_UMi(d2Dout);
    }
    else
        tempLinkInfo.isLOS=isLOS;
    //--------------------------------------------------------------------------
    
    //LOS pathLoss Computation
    if(channelModelType==_MODEL_B_ || (channelModelType==_MODEL_A_ && carrierFreqInGigaHz>6))
    {
        if((d2D>=10)&&(d2D<=dBP))
        {
            temp_LOS_PathLoss = 32.40 + 21.0*log10(d3D) + 20.0*log10(carrierFreqInGigaHz);
            shadowStdDev = 4.0;
        }
        else if((d2D>dBP)&&(d2D<=5000.0))
        {
            temp_LOS_PathLoss = 32.40 + 40.0*log10(d3D) + 20.0*log10(carrierFreqInGigaHz) - 9.50*log10(pow(dBP,2) + pow((bsHeight-ueHeight),2));
            shadowStdDev = 4.0;
        }
        else
        {
            temp_LOS_PathLoss =5000;
            /*cout<<"Error.... LOS Path Loss model for 5GUMi is not defined for d2D is greater than 5000.0m"<<endl;
            abort();*/		    
        }
    }
    else if(channelModelType==_MODEL_A_)
    {
        if((d2D>=10)&&(d2D<=dBP))
        {
            temp_LOS_PathLoss = 22.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz);
            shadowStdDev = 3.0;
        }
        else if((d2D>dBP)&&(d2D<5000.0))
        {
            temp_LOS_PathLoss = 40.0*log10(d3D) + 28.0 + 20.0*log10(carrierFreqInGigaHz) - 9.0*log10(pow(dBP,2) + pow((bsHeight-ueHeight),2));
            shadowStdDev = 3.0;
        }
        else
        {
            temp_LOS_PathLoss =5000;
            
            /*cout<<"Error.... LOS Path Loss model for 5GUMi is not defined for d2D is greater than 5000.0m"<<endl;
            abort();*/		    
        }
    }
    else
    {
        cout<<"Undefined Channel Model Type..."<<endl;abort();
    }
    
    if(tempLinkInfo.isLOS) // With LOS
    {
        tempLinkInfo.pathLoss = temp_LOS_PathLoss;
        tempLinkInfo.shadowStdDev = shadowStdDev;
    }
    else
    {
        bool optionalPathLossEqEnabled = terr->useOptionalPathLoss;
        if(!optionalPathLossEqEnabled)
        {
            if(channelModelType==_MODEL_B_ || (channelModelType==_MODEL_A_ && carrierFreqInGigaHz>6))
            {
                if(d2D>=10 && d2D<=5000)
                {
                    temp_NLOS_PathLoss = 35.3*log10(d3D) + 22.4 + 21.3*log10(carrierFreqInGigaHz) - 0.3*(ueHeight-1.5);
                    shadowStdDev = 7.82;
                }
                else
                {
                    temp_NLOS_PathLoss =5000;
            
//                     cout<<"Error.... NLOS Path Loss model for 5GUMi (model B) is not defined for d2D is greater than 5000.0m"<<endl;
//                     abort();
                }
            }
            else if(channelModelType==_MODEL_A_)
            {
                if(d2D>=10 && d2D<=2000)
                {
                    temp_NLOS_PathLoss = 36.7*log10(d3D) + 22.7 + 26.0*log10(carrierFreqInGigaHz) - 0.3*(ueHeight-1.5);
                    shadowStdDev = 4.0;
                }
                else
                {
                    temp_NLOS_PathLoss = 5000;
//                     cout<<"Error.... NLOS Path Loss model for 5GUMi (model A) is not defined for d2D is greater than 2000.0m"<<endl;
//                     abort();
                }
            }
            tempLinkInfo.pathLoss = max(temp_LOS_PathLoss,temp_NLOS_PathLoss);
            tempLinkInfo.shadowStdDev = shadowStdDev;
        }
        else
        {
            tempLinkInfo.pathLoss = 32.4 + 31.9*log10(d3D) + 20*log10(carrierFreqInGigaHz);
            tempLinkInfo.shadowStdDev = 8.2;
        }
    }
    tempLinkInfo.pathLoss += o2iLoss + carLoss;
    return (tempLinkInfo);
}

NodeLinkInfo_S pathLoss5GIndoor(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2, ChannelModelType channelModelType)
{
    NodeLinkInfo_S tempLinkInfo;  
    int isUEIndoor=-1;
    double d2D = 0.0;
    double ueHeight;
    double bsHeight;	
    tempLinkInfo.pathLoss=0.0;
    double d3D;
    double temp_LOS_PathLoss=0.0;
    double temp_NLOS_PathLoss=0.0;
    double shadowStdDev = 0.0 ;
    
    double carrierFreqInGigaHz=carrierFreqInHz/pow(10.0,9); 	
    if(nodeCategory1 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
    {
        bsHeight=nodeLoc1.z;
        ueHeight=nodeLoc2.z;
        isUEIndoor = nodeLoc2.isIndoor;     
    }
    else if(nodeCategory2 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
    {
        bsHeight=nodeLoc2.z;
        ueHeight=nodeLoc1.z;
        isUEIndoor = nodeLoc1.isIndoor;
    }
    else
    {
        cout<<"One of the node should be a server node...Exiting"<<endl;
        abort();
    }
    d2D = findDistance(nodeLoc1,nodeLoc2);
    if(!isUEIndoor)
    {
        cout<<"All UEs should be indoor in Indoor scenario.. Check Node initialization.."<<endl;
        abort();
    }
    
    d3D = sqrt(pow(d2D,2.0) + pow((bsHeight-ueHeight),2.0));
    tempLinkInfo.distance=d3D;
    IndoorScenario_E indoorScenario = terr->indoorOfficeScenario;
    
    //-------------------LOS probability calculation----------------------------
    if(isLOS==-1)
    {
        double PLos = 0.0;
        if(indoorScenario == _INDOOR_OPEN_OFFICE_)// _INDOOR_OPEN_OFFICE_ scenario
        {     
            if(d2D <= 5)
                PLos = 1.0; 
            else if(d2D>5 && d2D <=49)
                PLos = exp((-1.0*(d2D - 5.0))/70.8);
            else
                PLos = 0.54*exp((-1.0*(d2D - 49.0))/211.7);
        }
        else if(indoorScenario == _INDOOR_MIXED_OFFICE_)
        {
            if(d2D <= 1.2)
                PLos = 1.0; 
            else if(d2D>1.2 && d2D <=6.5)
                PLos = exp((-1.0*(d2D - 1.2))/4.7);
            else
                PLos = 0.32*exp((-1.0*(d2D - 6.5))/32.6);
        }
        else
        {
            cout<<"Undefined Indoor scenario..."<<endl;
            abort();
        }
        tempLinkInfo.isLOS=(randu()<PLos);
    }
    else
        tempLinkInfo.isLOS=isLOS;
    //----------------------------------------------------------------------------
    
    if((carrierFreqInGigaHz>6 && channelModelType == _MODEL_A_) || channelModelType == _MODEL_B_)
    {
        #ifdef _USING_CALIBRATION_
        if(d3D>=1 && d3D<=100)
        {
            temp_LOS_PathLoss =  32.4 + 17.3*log10(d3D) + 20.0*log10(carrierFreqInGigaHz);
        }
        else
        {
            temp_LOS_PathLoss = 1e6;
        }
        #else
        if(d3D>=1 && d3D<= 150)
        {
            temp_LOS_PathLoss =  32.4 + 17.3*log10(d3D) + 20.0*log10(carrierFreqInGigaHz);
        }
        else
        {
            cout<<"minimum d3D distance should less than 150 and greaterthan 1 m.."<<endl;
            abort();
        }
        #endif
        shadowStdDev = 3.0;
    }
    else if(channelModelType == _MODEL_A_)
    {
        if(d2D>=0 && d2D<= 150)
        {
            temp_LOS_PathLoss = 32.8 + 16.9*log10(d3D) + 20.0*log10(carrierFreqInGigaHz);
            shadowStdDev = 3.02;
        }
        else
        {
            cout<<"minimum d2D distance should less than 150 and greaterthan 0 m for LOS.."<<endl;
            abort();
        }
    }
    else
    {
        cout<<"Channel Model Undefined..."<<endl;abort();
    }
    
    if(tempLinkInfo.isLOS)
    {
        tempLinkInfo.pathLoss = temp_LOS_PathLoss;
        tempLinkInfo.shadowStdDev = shadowStdDev;
    }
    else
    {
        bool optionalPathLossEqEnabled = terr->useOptionalPathLoss; // Hard coded now later should be changed
        if(!optionalPathLossEqEnabled)
        {
            if((carrierFreqInGigaHz>6 && channelModelType == _MODEL_A_) || channelModelType == _MODEL_B_)
            {
                #ifdef _USING_CALIBRATION_
                if(d3D>=1.0 && d3D<=86.0)
                {
                    temp_NLOS_PathLoss = 38.3*log10(d3D) + 17.3 + 24.9*log10(carrierFreqInGigaHz);
                }
                else 
                {
                    temp_NLOS_PathLoss = 1e6;
                }
                #else 
                temp_NLOS_PathLoss = 38.3*log10(d3D) + 17.3 + 24.9*log10(carrierFreqInGigaHz);
                #endif
                shadowStdDev = 8.03;
            }
            else
            {
                temp_NLOS_PathLoss = 43.3*log10(d3D) + 11.5 + 20.0*log10(carrierFreqInGigaHz);
                shadowStdDev = 4.0;
            }
            tempLinkInfo.pathLoss = max(temp_LOS_PathLoss,temp_NLOS_PathLoss);
            tempLinkInfo.shadowStdDev = shadowStdDev;
        }
        else
        {
            tempLinkInfo.pathLoss = 32.4+31.9*log10(d3D) +20.0*log10(carrierFreqInGigaHz);
            tempLinkInfo.shadowStdDev=8.29;
        }
    }
    return tempLinkInfo;
}

NodeLinkInfo_S pathLoss5GIndoorFactory(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2, ChannelModelType channelModelType)
{
    NodeLinkInfo_S tempLinkInfo;  
    int isUEIndoor=-1;
    double d2D = 0.0;
    double ueHeight;
    double bsHeight;	
    tempLinkInfo.pathLoss=0.0;
    double d3D;
    double temp_LOS_PathLoss=0.0;
    double temp_NLOS_PathLoss=0.0;
    double shadowStdDev = 0.0 ;
    double o2iLoss=0.0;
    
    double carrierFreqInGigaHz=carrierFreqInHz/pow(10.0,9); 	
    if(nodeCategory1 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
    {
        bsHeight=nodeLoc1.z;
        ueHeight=nodeLoc2.z;
        isUEIndoor = nodeLoc2.isIndoor; 
        o2iLoss = nodeLoc2.o2iLoss;
    }
    else if(nodeCategory2 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
    {
        bsHeight=nodeLoc2.z;
        ueHeight=nodeLoc1.z;
        isUEIndoor = nodeLoc1.isIndoor;
        o2iLoss = nodeLoc1.o2iLoss;
    }
    else
    {
        cout<<"One of the node should be a server node...Exiting"<<endl;
        abort();
    }
    if(isUEIndoor)
    {
        d2D = findDistance(nodeLoc1,nodeLoc2);	    
    }
    else
    {
        cout<<"All UEs should be indoor in Indoor scenario.. Check Node initialization.."<<endl;
        abort();
    }
    
    d3D = sqrt(pow(d2D,2.0) + pow((bsHeight-ueHeight),2.0));
    tempLinkInfo.distance=d3D;
    if(d3D>600 || d3D<1)
    {
        cout<<"d3D must be in [1,600] for Indoor Factory."<<endl;abort();
    }
    if(channelModelType == _MODEL_A_)
    {
        cout<<"Indoor Factory doesn't support channel Model-A....Aborting..."<<endl;abort();
    }
    
    double r = terr->infParams.clutterDensity/100.0;
    double hc = terr->infParams.clutterHeight;
    double dc = terr->infParams.clutterSize;
    
    InFScenario_E infScenario = terr->infParams.scenario;
    if(ueHeight>=hc && infScenario != _INF_HH_)
    {
        cout<<"ueHeight must be less that clutterHeight..."<<endl;abort();
    }
    //-------------------LOS probability calculation----------------------------
    if(isLOS==-1)
    {
        double PLos=0.0,k_subsec=0.0;
        if(infScenario == _INF_SL_ || infScenario == _INF_DL_)
        {
            if(bsHeight>hc)
            {
                cout<<"bsHeight must be less than clutterHeight for Low BS scenario..."<<endl;abort();
            }
            k_subsec = -dc/log(1-r);
        }
        else if(infScenario == _INF_SH_ || infScenario == _INF_DH_ || infScenario ==_INF_HH_)
        {
            if(bsHeight<hc)
            {
                cout<<"bsHeight must be greater than clutterHeight for High BS scenario..."<<endl;abort();
            }
            k_subsec = (-dc/log(1-r))*((bsHeight-ueHeight)/(hc-ueHeight));
        }
        else
        {
            cout<<"Undefined Indoor Factory scenario..."<<endl;
            abort();
        }
        PLos = (infScenario ==_INF_HH_) ? 1.0 : exp(-d2D/k_subsec);
        tempLinkInfo.isLOS=randu()<PLos;
    }
    else
        tempLinkInfo.isLOS=isLOS;
    //----------------------------------------------------------------------------
    
    temp_LOS_PathLoss = 31.84 + 21.5*log10(d3D) + 19*log10(carrierFreqInGigaHz);
    if(tempLinkInfo.isLOS)
    {
        tempLinkInfo.pathLoss = temp_LOS_PathLoss;
        tempLinkInfo.shadowStdDev = 4.32;
    }
    else //NLOS
    {
        double SL_pathLoss;
        switch(infScenario)
        {
            case _INF_SL_: 
                temp_NLOS_PathLoss = 33.0 + 25.5*log10(d3D) + 20*log10(carrierFreqInGigaHz);
                break;
                
            case _INF_DL_:
                temp_NLOS_PathLoss = 18.6 + 35.7*log10(d3D) + 20*log10(carrierFreqInGigaHz);
                #ifndef _USING_CALIBRATION_
                SL_pathLoss = 33.0 + 25.5*log10(d3D) + 20*log10(carrierFreqInGigaHz);
                temp_NLOS_PathLoss = max(temp_NLOS_PathLoss,SL_pathLoss);
                #endif
                break;
                
            case _INF_SH_:
                #ifdef _USING_CALIBRATION_
                temp_NLOS_PathLoss = 50.5 + 12.8*log10(d3D) + 20*log10(carrierFreqInGigaHz);
                #else
                temp_NLOS_PathLoss = 32.4 + 23.0*log10(d3D) + 20*log10(carrierFreqInGigaHz);
                #endif
                break;
                
            case _INF_DH_:
                #ifdef _USING_CALIBRATION_
                temp_NLOS_PathLoss = 28.60 + 25.5*log10(d3D) + 20*log10(carrierFreqInGigaHz);
                #else
                temp_NLOS_PathLoss = 33.63 + 21.9*log10(d3D) + 20*log10(carrierFreqInGigaHz);
                #endif
                break;
                
            default: 
                cout<<"Undefined InF Scenario..."<<endl;abort();
                break;
                
        }
        tempLinkInfo.pathLoss = max(temp_LOS_PathLoss,temp_NLOS_PathLoss);
        tempLinkInfo.shadowStdDev = getSigmaSFForInF(infScenario);
    }
    //tempLinkInfo.pathLoss += o2iLoss; //Since medium changes as BS is outside clutter and UE is inside clutter. 
    return tempLinkInfo;
}

void Terrain::loadIndoorFactoryParams(string mySysConfigFile)
{
    string infScenario;
    parse(mySysConfigFile,"InFScenario",infScenario);
    if(infScenario == "_INF_SL_")
    {
        infParams.scenario =_INF_SL_;
    }
    else if(infScenario == "_INF_DL_")
    {
        infParams.scenario =_INF_DL_;
    }
    else if(infScenario == "_INF_SH_")
    {
        infParams.scenario =_INF_SH_;
    }
    else if(infScenario == "_INF_DH_")
    {
        infParams.scenario =_INF_DH_;
    }
    else if(infScenario == "_INF_HH_")
    {
        infParams.scenario =_INF_HH_;
    }
    else 
    {
        cout<<"Unknown Indoor Factory Scenario.."<<endl;abort();
    }
    parse(mySysConfigFile,"clutterDensity",infParams.clutterDensity);
    parse(mySysConfigFile,"clutterSize",infParams.clutterSize);
    parse(mySysConfigFile,"clutterHeight",infParams.clutterHeight);
    if(infParams.clutterDensity<40 && (infParams.scenario==_INF_DH_ || infParams.scenario==_INF_DL_))
    {
        cout<<"clutterDensity should not be less than 40 for "<<infScenario<<endl;abort();
    }
    if(infParams.clutterDensity>40 && (infParams.scenario==_INF_SH_ || infParams.scenario==_INF_SL_))
    {
        cout<<"clutterDensity should not be greater than 40 for "<<infScenario<<endl;abort();
    }
}

 
NodeLinkInfo_S pathLossFreeSpace(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz){
  
  NodeLinkInfo_S tempLinkInfo;
  tempLinkInfo.isLOS=0;
  tempLinkInfo.shadowStdDev=0;
  
  double d2D = findDistance(nodeLoc1,nodeLoc2);	    
  double d3D = sqrt(pow(d2D,2.0) + pow((nodeLoc1.z-nodeLoc2.z),2.0));
 
  tempLinkInfo.pathLoss=20*log10(4*pi*d3D*carrierFreqInHz/lightSpeed);
  
  tempLinkInfo.distance=d3D;
  
  return  (tempLinkInfo);
} 
