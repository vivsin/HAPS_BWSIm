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

bool loadNTNTerrains(Terrain &terrain) 
{
    Array<string> terrainNames="{_TERRAIN_NTN_DENSE_URBAN_ _TERRAIN_NTN_DENSE_URBAN_LOS_ _TERRAIN_NTN_DENSE_URBAN_NLOS_ "
                                "_TERRAIN_NTN_URBAN_ _TERRAIN_NTN_URBAN_LOS_ _TERRAIN_NTN_URBAN_NLOS_ " 
                                "_TERRAIN_NTN_SUB_URBAN_ _TERRAIN_NTN_SUB_URBAN_LOS_ _TERRAIN_NTN_SUB_URBAN_NLOS_ "
                                "_TERRAIN_NTN_RURAL_ _TERRAIN_NTN_RURAL_LOS_ _TERRAIN_NTN_RURAL_NLOS_}";
    int index = findInStringArray(terrainNames,terrain.getTerrain());
    if(index!=-1)
    {
        terrain.addTerrain(terrain.getTerrain(),&getNTNChannelPathLoss);
        return true;
    }
    cout<<"Unsupported terrain "<<terrain.getTerrain()<<endl;
    return false;
}

NodeLinkInfo_S getNTNChannelPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr,NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2)
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
    
    if(myTerrain=="_TERRAIN_NTN_DENSE_URBAN_" || myTerrain=="_TERRAIN_NTN_DENSE_URBAN_LOS_" || myTerrain=="_TERRAIN_NTN_DENSE_URBAN_NLOS_")
    {
        if(myTerrain=="_TERRAIN_NTN_DENSE_URBAN_LOS_") isLOS = 1;
        else if(myTerrain=="_TERRAIN_NTN_DENSE_URBAN_NLOS_") isLOS = 0;
        
//         return pathLossNTNDenseUrban(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr,nodeCategory1,nodeCategory2,terr->channelModelType);
    }
    else if(myTerrain=="_TERRAIN_NTN_URBAN_" || myTerrain=="_TERRAIN_NTN_URBAN_LOS_" || myTerrain=="_TERRAIN_NTN_URBAN_NLOS_")
    {
        if(myTerrain=="_TERRAIN_NTN_URBAN_LOS_") isLOS = 1;
        else if(myTerrain=="_TERRAIN_NTN_URBAN_NLOS_") isLOS = 0;
        
//         return pathLossNTNUrban(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr,nodeCategory1,nodeCategory2,terr->channelModelType);
    }
    else if(myTerrain=="_TERRAIN_NTN_SUB_URBAN_" || myTerrain=="_TERRAIN_NTN_SUB_URBAN_LOS_" || myTerrain=="_TERRAIN_NTN_SUB_URBAN_NLOS_")
    {   
        if(myTerrain=="_TERRAIN_NTN_SUB_URBAN_LOS_") isLOS = 1;
        else if(myTerrain=="_TERRAIN_NTN_SUB_URBAN_NLOS_") isLOS = 0;
        
//         return pathLossNTNSubUrban(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr,nodeCategory1,nodeCategory2,terr->channelModelType);
    }
    else if(myTerrain=="_TERRAIN_NTN_RURAL_" || myTerrain=="_TERRAIN_NTN_RURAL_LOS_" || myTerrain=="_TERRAIN_NTN_RURAL_NLOS_")
    {   
        if(myTerrain=="_TERRAIN_NTN_RURAL_LOS_") isLOS = 1;
        else if(myTerrain=="_TERRAIN_NTN_RURAL_NLOS_") isLOS = 0;
        
//         return pathLossNTNRural(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr,nodeCategory1,nodeCategory2,terr->channelModelType);
    }
    else
    {
        cout<<"Unsupported Terrain for NTN..."<<endl;abort();
    }
    return computeNTNPathLoss(nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,terr,nodeCategory1,nodeCategory2,terr->channelModelType);
}

vec getBuildingPenetrationModelCoefficients(bool isTraditional)
{
    //Refer Table 6.6.3-1 in 3GPP TR 38.811
    vec val;
    if(isTraditional)
        val = "12.64 3.72 0.96 9.60 2.0 9.10 -3.0 4.5 -2.0";
    else 
        val = "28.19 -3.0 8.48 13.5 3.8 27.8 -2.9 9.4 -2.1";                //verified
    return val; 
}

double computeBuildingPenetrationLoss(double carrierFreqInGHz,bool isTraditional,double theta)
{
    vec coeff = getBuildingPenetrationModelCoefficients(isTraditional);
    double r,s,t,u,v,w,x,y,z;int indx=0;
    r=coeff(indx++);s=coeff(indx++);t=coeff(indx++);u=coeff(indx++);
    v=coeff(indx++);w=coeff(indx++);x=coeff(indx++);y=coeff(indx++);z=coeff(indx++);
    
    double P = randu(); //FIXME this is needed to be set properly.
    
    double logf = log10(carrierFreqInGHz);
    
    double Lh = r + s*logf + t*sqr(logf), Le = 0.212*abs(theta);
    
    double mu1 = Lh + Le, mu2 = w + x*logf, sigma1 = u + v*logf, sigma2 = y + z*logf;
    
    double mu=0,sigma = 1;
    double F_P = mu + sigma*sqrt(2)*erfinv(2*P-1);
    
    double A =  mu1 + F_P*sigma1, B = mu2 + F_P*sigma2, C = -3;
    
    double L_BEL = dB(inv_dB(A)+inv_dB(B)+inv_dB(C));
    return L_BEL;
}

double getZenithAtmosphereAttenuation(double carrierFreqInGHz)
{
    //Refer Fig 11 of ITU-R P.676-12
    #ifdef _USING_CALIBRATION_
    if(!isKaBand(carrierFreqInGHz*1e9)){
        return 0.025;
    }
    else
        return 0.25;
    #endif
    
    //NOTE: All these conditions are obtained through approximating as y = mx+c or y = ax^n from the plot given.
    if(carrierFreqInGHz<1)
        return 0;
    else if(carrierFreqInGHz<=4){// added newly
        return 0.05;
    }
    else if(carrierFreqInGHz<=10)
    {
        return 0.03 + (0.02/9)*(carrierFreqInGHz-1);
    }
    else if(carrierFreqInGHz<=23)
    {
        return 0.5*pow(carrierFreqInGHz/23,2.7645);
    }
    else if(carrierFreqInGHz<=30)
    {
        return 0.22*pow(carrierFreqInGHz/30,-3.09);
    }
    else if(carrierFreqInGHz<=50)
    {
        return 1.5*pow(carrierFreqInGHz/50,1.954);
    }
    else if(carrierFreqInGHz<=60)
    {
        return 1.5 + 15.85*(carrierFreqInGHz-50);
    }
    else if(carrierFreqInGHz<=70)
    {
        return 160 -15.85*(carrierFreqInGHz-60);
    }
    else if(carrierFreqInGHz<=100)
        return 0.75*pow(carrierFreqInGHz/100,-1.943);
    
    cout<<"carrierFreqInGHz > 100 is not supported currently..."<<endl;
    abort();
}

double computeAtmosphericLoss(double carrierFreqInGHz,double elevationAngle)
{
    //Refer Eqn 6.6-8 in 3GPP TR 38.811.
    double A_zenith = getZenithAtmosphereAttenuation(carrierFreqInGHz);
    
    double loss = A_zenith/(sin(deg_to_rad(elevationAngle)));
    return loss;
}

//NOTE: latitudes with North direction are considered as positive ie. North pole is +90 and south pole is -90. 
double getLatitude(Location_S loc)
{
    double theta = findAngle(loc,Location_S(0,0,0))(1);
    // cout<<"Error? latitude: "<<theta<<endl;
    return -(theta-90.0);
}

double computeScintillationLoss(double carrierFreqInGHz,Location_S loc,double elevationAngle)
{
    double scintillationLoss = 0.0;
    if(carrierFreqInGHz<=6) // Ionospheric Scintillation
    {
        //Refer to Section 6.6.6.1.4 of 3GPP TR 38.811.
        //if(abs(getLatitude(loc))<=20) changed
        if(elevationAngle<=50||abs(getLatitude(loc))<=30)
        {
            scintillationLoss = 1.1*pow(carrierFreqInGHz/4,-1.5)/sqrt(2);
        }
    }
    else    //Tropospheric Scintillation
    {
        // Refer to Section 6.6.6.2.1 of 3GPP TR 38.811
        vec attenuation = "1.08 0.48 0.30 0.22 0.17 0.13 0.12 0.12 0.12";  //verified
        scintillationLoss = (randu()<0.99) ? attenuation(round_i(elevationAngle/10)-1) : 0.0;
    }
    return scintillationLoss;
}

double getNTNLOSProbability(SCMPropagationScenario scmScenario,double elevationAngle)
{
    //Refer Table 6.6.1-1 in 3GPP TR 38.811. 
    vec losProbabilityPerElevation;
    switch(scmScenario)
    {
        case _SCMNTN_DENSE_URBAN_:
            losProbabilityPerElevation = "28.2 33.1 39.8 46.8 53.7 61.2 73.8 82.0 98.1";
            break;
        case _SCMNTN_URBAN_:
            losProbabilityPerElevation = "24.6 38.6 49.3 61.3 72.6 80.5 91.9 96.8 99.2";
            break;
        case _SCMNTN_SUB_URBAN_:
        case _SCMNTN_RURAL_:
            losProbabilityPerElevation = "78.2 86.9 91.9 92.9 93.5 94.0 94.9 95.2 99.8";//verified
            break;
        default:
            cout<<"Unknown NTN scenario.."<<endl;abort();
            break;
    }
    // cout<<"Error? losProbabilityPerElevation: "<<losProbabilityPerElevation(round_i(elevationAngle/10)-1)/100<<endl;
    return losProbabilityPerElevation(round_i(elevationAngle/10)-1)/100;
}

double getNTNClutterLossInDB(SCMPropagationScenario scmScenario,double elevationAngle,double carrierFreqInHz)
{
    //Refer Table 6.6.2-1,2,3 in 3GPP TR 38.811.
    vec clutterLossPerElevation;
    switch(scmScenario)
    {
        case _SCMNTN_DENSE_URBAN_:
        case _SCMNTN_URBAN_:
            if(!isKaBand(carrierFreqInHz))
                clutterLossPerElevation = "34.3 30.9 29.0 27.7 26.8 26.2 25.8 25.5 25.5";
            else
                clutterLossPerElevation = "44.3 39.9 37.5 35.8 34.6 33.8 33.3 33.0 32.9";
            break;
        case _SCMNTN_SUB_URBAN_:
        case _SCMNTN_RURAL_:
            if(!isKaBand(carrierFreqInHz))
                clutterLossPerElevation = "19.52 18.17 18.42 18.28 18.63 17.68 16.5 16.3 16.3";//Deepitha
            else
                clutterLossPerElevation = "29.5 24.6 21.9 20.0 18.7 17.8 17.2 16.9 16.8";         //verified
            break;
        default:
            cout<<"Unknown NTN scenario.."<<endl;abort();
            break;
    }
    // cout<<"clutterLossPerElevation: "<<clutterLossPerElevation<<endl;
    return clutterLossPerElevation(round_i(elevationAngle/10)-1);
}

double getNTNShadowStdDev(SCMPropagationScenario scmScenario,double elevationAngle,double carrierFreqInHz,bool isLOS)
{
    //Refer Table 6.6.2-1,2,3 in 3GPP TR 38.811.
    vec shadowStdDevPerElevation;
    switch(scmScenario)
    {
        case _SCMNTN_DENSE_URBAN_:
            if(!isKaBand(carrierFreqInHz))  //S-band
            {
                if(isLOS)
                    shadowStdDevPerElevation = "3.5 3.4 2.9 3.0 3.1 2.7 2.5 2.3 1.2";
                else
                    shadowStdDevPerElevation = "15.5 13.9 12.4 11.7 10.6 10.5 10.1 9.2 9.2";
            }
            else
            {
                if(isLOS)
                    shadowStdDevPerElevation = "2.9 2.4 2.7 2.4 2.4 2.7 2.6 2.8 0.6";
                else 
                    shadowStdDevPerElevation = "17.1 17.1 15.6 14.6 14.2 12.6 12.1 12.3 12.3";   //verified
            }
            break;
        case _SCMNTN_URBAN_:
            if(isLOS)
                shadowStdDevPerElevation = 4*ones(9);
            else
                shadowStdDevPerElevation = 6*ones(9);
            break;
        case _SCMNTN_SUB_URBAN_:
        case _SCMNTN_RURAL_:
            if(!isKaBand(carrierFreqInHz))  //S-band
            {                
                if(isLOS)
                    shadowStdDevPerElevation = "1.79 1.14 1.14 0.92 1.42 1.56 0.85 0.72 0.72"; //Santhosh
                else
                    shadowStdDevPerElevation = "8.93 9.08 8.78 10.25 10.56 10.74 10.17 11.52 11.52";
            
            }
            else
            {
                if(isLOS)
                    shadowStdDevPerElevation = "1.9 1.6 1.9 2.3 2.7 3.1 3.0 3.6 0.4";
                else
                    shadowStdDevPerElevation = "10.7 10.0 11.2 11.6 11.8 10.8 10.8 10.8 10.8";    //verified
            }
            break;
        default:
            cout<<"Unknown NTN scenario.."<<endl;abort();
            break;
    }
    //cout<<"shadowStdDevPerElevation: "<<shadowStdDevPerElevation(round_i(elevationAngle/10)-1)<<endl;
    //abort();
    return shadowStdDevPerElevation(round_i(elevationAngle/10)-1);
}

NodeLinkInfo_S computeNTNPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType)
{
    NodeLinkInfo_S tempLinkInfo; 
    
    double d = find3Ddistance(nodeLoc1,nodeLoc2);
    tempLinkInfo.distance = d;
    double alpha = 0; //elevationAngle
    double carrierFreqInGHz = carrierFreqInHz/1e9;
    bool isIndoor = false;
    bool isLowPenetrationLoss = false;
    
    if(nodeCategory1 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
    {
        alpha = getElevationAngle(nodeLoc1,nodeLoc2);
        isIndoor = nodeLoc2.isIndoor,isLowPenetrationLoss = nodeLoc2.isLowPenetrationLoss;
    }
    else if(nodeCategory2 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
    {
        alpha = getElevationAngle(nodeLoc2,nodeLoc1);
        isIndoor = nodeLoc1.isIndoor,isLowPenetrationLoss = nodeLoc1.isLowPenetrationLoss;
    }
    else
    {
        cout<<"One of the node should be a server node...Exiting"<<endl;
        abort();
    }
    if(isnan(alpha))
    {
        cout<<nodeLoc1<<" "<<nodeLoc2<<" "<<nodeLoc1.getDistFromOrigin()<<" "<<nodeLoc2.getDistFromOrigin()<<" "<<d<<endl;
        cout<<"elevation angle cannot be NAN....Check"<<endl;
        abort();
    }

    if(alpha>0)
    {
        if(isLOS==-1)
        {
            double PLos = getNTNLOSProbability(terr->scmScenario,alpha);
            tempLinkInfo.isLOS = randu()<PLos;
        }
        else 
            tempLinkInfo.isLOS = isLOS;
        //cout << "isLOS : " << isLOS <<endl;
        double FSPL = 32.45 + 20*log10(carrierFreqInGHz) + 20*log10(d); //Free Space Path Loss Eqn 6.6.2 in 3GPP TR 38.811
        //abort();
        //cout<<"distance in meters"<<d<<endl;
        double clutterLoss = (tempLinkInfo.isLOS) ? 0.0 : getNTNClutterLossInDB(terr->scmScenario,alpha,carrierFreqInHz);
        double buildingLoss = (isIndoor) ? computeBuildingPenetrationLoss(carrierFreqInGHz,isLowPenetrationLoss,alpha) : 0.0;//FIXME elevation angle of the path at the building façade - Should be properly set.
        double atmosphericLoss = computeAtmosphericLoss(carrierFreqInGHz,alpha);
        // double scintillationLoss = computeScintillationLoss(carrierFreqInGHz,(nodeCategory1==_NODECATEGORY_SERVERNODE_)?nodeLoc2 : nodeLoc1,alpha);
        double scintillationLoss=0;
        tempLinkInfo.pathLoss = FSPL + clutterLoss + buildingLoss + atmosphericLoss + scintillationLoss;
        tempLinkInfo.freeSpacePathLoss = FSPL + clutterLoss;
        tempLinkInfo.buildingLoss = buildingLoss;
        tempLinkInfo.atmosphericLoss = atmosphericLoss;
        tempLinkInfo.scintillationLoss = scintillationLoss;
        tempLinkInfo.shadowStdDev = getNTNShadowStdDev(terr->scmScenario,alpha,carrierFreqInHz,tempLinkInfo.isLOS);
        tempLinkInfo.elevationAngle = alpha;
        //cout<<"net pathloss"<<FSPL + clutterLoss + buildingLoss + atmosphericLoss + scintillationLoss<<endl;
        //cout<<"FSPL + clutterLoss + buildingLoss + atmosphericLoss + scintillationLoss" <<FSPL<<" , " <<clutterLoss<<" , " <<buildingLoss <<" , "<<atmosphericLoss <<" , "<<scintillationLoss <<","<<tempLinkInfo.shadowStdDev<<endl;
//         cout << "[PathLoss Debug] Dist: " << d << " m, Elev: " << alpha << " deg, LOS: " << tempLinkInfo.isLOS << endl;
//         cout << "[PathLoss Debug] FSPL: " << FSPL << " + Clutter: " << clutterLoss << " + Bldg: " << buildingLoss << " + Atmos: " << atmosphericLoss << " + Scint: " << scintillationLoss << " = Total PL: " << tempLinkInfo.pathLoss << endl;
    }
    else 
    {
        tempLinkInfo.pathLoss = 100000;
        tempLinkInfo.isLOS = 0;
        tempLinkInfo.shadowStdDev = 0;
        tempLinkInfo.shadowLoss=0;
    }
    // print statements
    // cout<<"alpha: "<<alpha<<endl;
    // cout<<"tempLinkInfo.distance: "<<tempLinkInfo.distance<<endl;
    // cout<<"isIndoor: "<<isIndoor<<endl;
    // cout<<"scmScenario: "<<terr->scmScenario<<endl;

    return tempLinkInfo;
}

// NodeLinkInfo_S pathLossNTNDenseUrban(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType)
// {
//     NodeLinkInfo_S tempLinkInfo; 
//     
//     double d = find3Ddistance(nodeLoc1,nodeLoc2);
//     tempLinkInfo.distance = d;
//     double alpha = 0; //elevationAngle
//     double carrierFreqInGHz = carrierFreqInHz/1e9;
//     bool isIndoor = false;
//     bool isLowPenetrationLoss = false;
//     
//     if(nodeCategory1 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
//     {
//         alpha = getElevationAngle(nodeLoc1,nodeLoc2);
//         isIndoor = nodeLoc2.isIndoor,isLowPenetrationLoss = nodeLoc2.isLowPenetrationLoss;
//     }
//     else if(nodeCategory2 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
//     {
//         alpha = getElevationAngle(nodeLoc2,nodeLoc1);
//         isIndoor = nodeLoc1.isIndoor,isLowPenetrationLoss = nodeLoc1.isLowPenetrationLoss;
//     }
//     else
//     {
//         cout<<"One of the node should be a server node...Exiting"<<endl;
//         abort();
//     }
//     if(isnan(alpha))
//     {
//         cout<<nodeLoc1<<" "<<nodeLoc2<<" "<<nodeLoc1.getDistFromOrigin()<<" "<<nodeLoc2.getDistFromOrigin()<<" "<<d<<" "<<nodeCategory1<<endl;abort();
//     }
//     if(alpha>0)
//     {        
//         int index = round_i(alpha/10)-1;
//         if(isLOS==-1)
//         {
//             vec losProbabilityPerElevation = "28.2 33.1 39.8 46.8 53.7 61.2 73.8 82.0 98.1";
//             double PLos = losProbabilityPerElevation(index)/100;
//             tempLinkInfo.isLOS = randu()<PLos;
//         }
//         else 
//             tempLinkInfo.isLOS = isLOS;
//         
//         double FSPL = 32.45 + 20*log10(carrierFreqInGHz) + 20*log10(d); //Free Space Path Loss Eqn 6.6.2 in 3GPP TR 38.811
//         vec clutterLossPerElevation,shadowDevPerElevation;
//         if(!isKaBand(carrierFreqInHz))  //S-band
//         {
//             clutterLossPerElevation = "34.3 30.9 29.0 27.7 26.8 26.2 25.8 25.5 25.5";
//             if(tempLinkInfo.isLOS)
//                 shadowDevPerElevation = "3.5 3.4 2.9 3.0 3.1 2.7 2.5 2.3 1.2";
//             else
//                 shadowDevPerElevation = "15.5 13.9 12.4 11.7 10.6 10.5 10.1 9.2 9.2";
//         }
//         else
//         {
//             clutterLossPerElevation = "44.3 39.9 37.5 35.8 34.6 33.8 33.3 33.0 32.9";
//             if(tempLinkInfo.isLOS)
//                 shadowDevPerElevation = "2.9 2.4 2.7 2.4 2.4 2.7 2.6 2.8 0.6";
//             else 
//                 shadowDevPerElevation = "17.1 17.1 15.6 14.6 14.2 12.6 12.1 12.3 12.3";
//         }
//         double CL = clutterLossPerElevation(index); //clutter Loss
//         tempLinkInfo.shadowStdDev = shadowDevPerElevation(index);
//         tempLinkInfo.pathLoss = (tempLinkInfo.isLOS) ? FSPL : FSPL + CL;
//         tempLinkInfo.elevationAngle = alpha;
//         
//         double buildingLoss = (isIndoor) ? computeBuildingPenetrationLoss(carrierFreqInGHz,isLowPenetrationLoss,alpha) : 0.0;//FIXME elevation angle of the path at the building façade - Should be properly set.
//         double atmosphericLoss = computeAtmosphericLoss(carrierFreqInGHz,alpha);
//         double scintillationLoss = computeScintillationLoss(carrierFreqInGHz,(nodeCategory1==_NODECATEGORY_SERVERNODE_)?nodeLoc2 : nodeLoc1,alpha);
//         tempLinkInfo.pathLoss += (buildingLoss + atmosphericLoss + scintillationLoss);
//     }
//     else 
//     {
//         tempLinkInfo.pathLoss = 100000;
//         tempLinkInfo.isLOS = 0;
//         tempLinkInfo.shadowStdDev = 0;
//         tempLinkInfo.shadowLoss=0;
//     }
//     return tempLinkInfo;
// }
// 
// NodeLinkInfo_S pathLossNTNUrban(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType)
// {
//     NodeLinkInfo_S tempLinkInfo; 
//     
//     double d = find3Ddistance(nodeLoc1,nodeLoc2);
//     tempLinkInfo.distance = d;
//     double alpha = 0; //elevationAngle
//     double carrierFreqInGHz = carrierFreqInHz/1e9;
//     bool isIndoor = false;
//     bool isLowPenetrationLoss = false;
//     
//     if(nodeCategory1 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
//     {
//         alpha = getElevationAngle(nodeLoc1,nodeLoc2);
//         isIndoor = nodeLoc2.isIndoor,isLowPenetrationLoss = nodeLoc2.isLowPenetrationLoss;
//     }
//     else if(nodeCategory2 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
//     {
//         alpha = getElevationAngle(nodeLoc2,nodeLoc1);
//         isIndoor = nodeLoc1.isIndoor,isLowPenetrationLoss = nodeLoc1.isLowPenetrationLoss;
//     }
//     else
//     {
//         cout<<"One of the node should be a server node...Exiting"<<endl;
//         abort();
//     }
//     if(isnan(alpha))
//     {
//         cout<<nodeLoc1<<" "<<nodeLoc2<<" "<<nodeLoc1.getDistFromOrigin()<<" "<<nodeLoc2.getDistFromOrigin()<<" "<<d<<endl;abort();
//     }
//     
//     if(alpha>0)
//     {
//         int index = round_i(alpha/10)-1;
//         if(isLOS==-1)
//         {
//             vec losProbabilityPerElevation = "24.6 38.6 49.3 61.3 72.6 80.5 91.9 96.8 99.2";
//             double PLos = losProbabilityPerElevation(index)/100;
//             tempLinkInfo.isLOS = randu()<PLos;
//         }
//         else 
//             tempLinkInfo.isLOS = isLOS;
//         
//         double FSPL = 32.45 + 20*log10(carrierFreqInGHz) + 20*log10(d); //Free Space Path Loss Eqn 6.6.2 in 3GPP TR 38.811
//         vec clutterLossPerElevation,shadowDevPerElevation;
//         if(!isKaBand(carrierFreqInHz))  //S-band
//         {
//             clutterLossPerElevation = "34.3 30.9 29.0 27.7 26.8 26.2 25.8 25.5 25.5";
//             if(tempLinkInfo.isLOS)
//                 shadowDevPerElevation = 4*ones(9);
//             else
//                 shadowDevPerElevation = 6*ones(9);
//         }
//         else
//         {
//             clutterLossPerElevation = "44.3 39.9 37.5 35.8 34.6 33.8 33.3 33.0 32.9";
//             if(tempLinkInfo.isLOS)
//                 shadowDevPerElevation = 4*ones(9);
//             else
//                 shadowDevPerElevation = 6*ones(9);
//         }
//         double CL = clutterLossPerElevation(index); //clutter Loss
//         tempLinkInfo.shadowStdDev = shadowDevPerElevation(index);
//         tempLinkInfo.pathLoss = (tempLinkInfo.isLOS) ? FSPL : FSPL + CL;
//         tempLinkInfo.elevationAngle = alpha;
//         
//         double buildingLoss = (isIndoor) ? computeBuildingPenetrationLoss(carrierFreqInGHz,isLowPenetrationLoss,alpha) : 0.0;//FIXME elevation angle of the path at the building façade - Should be properly set.
//         double atmosphericLoss = computeAtmosphericLoss(carrierFreqInGHz,alpha);
//         double scintillationLoss = computeScintillationLoss(carrierFreqInGHz,(nodeCategory1==_NODECATEGORY_SERVERNODE_)?nodeLoc2 : nodeLoc1,alpha);
//         tempLinkInfo.pathLoss += (buildingLoss + atmosphericLoss + scintillationLoss);
//     }
//     else 
//     {
//         tempLinkInfo.pathLoss = 100000;
//         tempLinkInfo.isLOS = 0;
//         tempLinkInfo.shadowStdDev = 0;
//         tempLinkInfo.shadowLoss=0;
//     }
//     return tempLinkInfo;
// }
// 
// NodeLinkInfo_S pathLossNTNSubUrban(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType)
// {
//     NodeLinkInfo_S tempLinkInfo; 
//     
//     double d = find3Ddistance(nodeLoc1,nodeLoc2);
//     tempLinkInfo.distance = d;
//     double alpha = 0; //elevationAngle
//     double carrierFreqInGHz = carrierFreqInHz/1e9;
//     bool isIndoor = false;
//     bool isLowPenetrationLoss = false;
//     
//     if(nodeCategory1 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
//     {
//         alpha = getElevationAngle(nodeLoc1,nodeLoc2);
//         isIndoor = nodeLoc2.isIndoor,isLowPenetrationLoss = nodeLoc2.isLowPenetrationLoss;
//     }
//     else if(nodeCategory2 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
//     {
//         alpha = getElevationAngle(nodeLoc2,nodeLoc1);
//         isIndoor = nodeLoc1.isIndoor,isLowPenetrationLoss = nodeLoc1.isLowPenetrationLoss;
//     }
//     else
//     {
//         cout<<"One of the node should be a server node...Exiting"<<endl;
//         abort();
//     }
//     if(isnan(alpha))
//     {
//         cout<<nodeLoc1<<" "<<nodeLoc2<<" "<<nodeLoc1.getDistFromOrigin()<<" "<<nodeLoc2.getDistFromOrigin()<<" "<<d<<endl;abort();
//     }
//     
//     if(alpha>0)
//     {
//         int index = round_i(alpha/10)-1;
//         if(isLOS==-1)
//         {
//             vec losProbabilityPerElevation = "78.2 86.9 91.9 92.9 93.5 94.0 94.9 95.2 99.8";
//             double PLos = losProbabilityPerElevation(index);
//             tempLinkInfo.isLOS = randu()<PLos;
//         }
//         else 
//             tempLinkInfo.isLOS = isLOS;
//         
//         double FSPL = 32.45 + 20*log10(carrierFreqInGHz) + 20*log10(d); //Free Space Path Loss Eqn 6.6.2 in 3GPP TR 38.811
//         vec clutterLossPerElevation,shadowDevPerElevation;
//         if(!isKaBand(carrierFreqInHz))  //S-band
//         {
//             clutterLossPerElevation = "19.52 18.17 18.42 18.28 18.63 17.68 16.5 16.3 16.5";
//             if(tempLinkInfo.isLOS)
//                 shadowDevPerElevation = "1.79 1.14 1.14 0.92 1.42 1.56 0.85 0.72 0.72";
//             else
//                 shadowDevPerElevation = "8.93 9.08 8.78 10.25 10.56 10.74 10.17 11.52 11.52";
//         }
//         else
//         {
//             clutterLossPerElevation = "29.5 24.6 21.9 20.0 18.7 17.8 17.2 16.9 16.8";
//             if(tempLinkInfo.isLOS)
//                 shadowDevPerElevation = "1.9 1.6 1.9 2.3 2.7 3.1 3.0 3.6 0.4";
//             else
//                 shadowDevPerElevation = "10.7 10.0 11.2 11.6 11.8 10.8 10.8 10.8 10.8";
//         }
//         double CL = clutterLossPerElevation(index); //clutter Loss
//         tempLinkInfo.shadowStdDev = shadowDevPerElevation(index);
//         tempLinkInfo.pathLoss = (tempLinkInfo.isLOS) ? FSPL : FSPL + CL;
//         tempLinkInfo.elevationAngle = alpha;
//         
//         double buildingLoss = (isIndoor) ? computeBuildingPenetrationLoss(carrierFreqInGHz,isLowPenetrationLoss,alpha) : 0.0;//FIXME elevation angle of the path at the building façade - Should be properly set.
//         double atmosphericLoss = computeAtmosphericLoss(carrierFreqInGHz,alpha);
//         double scintillationLoss = computeScintillationLoss(carrierFreqInGHz,(nodeCategory1==_NODECATEGORY_SERVERNODE_)?nodeLoc2 : nodeLoc1,alpha);
//         tempLinkInfo.pathLoss += (buildingLoss + atmosphericLoss + scintillationLoss);
//     }
//     else 
//     {
//         tempLinkInfo.pathLoss = 100000;
//         tempLinkInfo.isLOS = 0;
//         tempLinkInfo.shadowStdDev = 0;
//         tempLinkInfo.shadowLoss=0;
//     }
//     return tempLinkInfo;
// }
// 
// NodeLinkInfo_S pathLossNTNRural(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType)
// {
//     NodeLinkInfo_S tempLinkInfo; 
//     
//     double d = find3Ddistance(nodeLoc1,nodeLoc2);
//     tempLinkInfo.distance = d;
//     double alpha = 0; //elevationAngle
//     double carrierFreqInGHz = carrierFreqInHz/1e9;
//     bool isIndoor = false;
//     bool isLowPenetrationLoss = false;
//     
//     if(nodeCategory1 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
//     {
//         alpha = getElevationAngle(nodeLoc1,nodeLoc2);
//         isIndoor = nodeLoc2.isIndoor,isLowPenetrationLoss = nodeLoc2.isLowPenetrationLoss;
//     }
//     else if(nodeCategory2 == _NODECATEGORY_SERVERNODE_)//nodeCategory1 corresponds to nodeLoc1 & nodeCategory2 corresponds to nodeLoc2
//     {
//         alpha = getElevationAngle(nodeLoc2,nodeLoc1);
//         isIndoor = nodeLoc1.isIndoor,isLowPenetrationLoss = nodeLoc1.isLowPenetrationLoss;
//     }
//     else
//     {
//         cout<<"One of the node should be a server node...Exiting"<<endl;
//         abort();
//     }
//     if(isnan(alpha))
//     {
//         cout<<nodeLoc1<<" "<<nodeLoc2<<" "<<nodeLoc1.getDistFromOrigin()<<" "<<nodeLoc2.getDistFromOrigin()<<" "<<d<<endl;abort();
//     }
//     
//     if(alpha>0)
//     {
//         int index = round_i(alpha/10)-1;
//         if(isLOS==-1)
//         {
//             vec losProbabilityPerElevation = "78.2 86.9 91.9 92.9 93.5 94.0 94.9 95.2 99.8";
//             double PLos = losProbabilityPerElevation(index);
//             tempLinkInfo.isLOS = randu()<PLos;
//         }
//         else 
//             tempLinkInfo.isLOS = isLOS;
//         
//         double FSPL = 32.45 + 20*log10(carrierFreqInGHz) + 20*log10(d); //Free Space Path Loss Eqn 6.6.2 in 3GPP TR 38.811
//         vec clutterLossPerElevation,shadowDevPerElevation;
//         if(!isKaBand(carrierFreqInHz))  //S-band
//         {
//             clutterLossPerElevation = "19.52 18.17 18.42 18.28 18.63 17.68 16.5 16.3 16.5";
//             if(tempLinkInfo.isLOS)
//                 shadowDevPerElevation = "1.79 1.14 1.14 0.92 1.42 1.56 0.85 0.72 0.72";
//             else
//                 shadowDevPerElevation = "8.93 9.08 8.78 10.25 10.56 10.74 10.17 11.52 11.52";
//         }
//         else
//         {
//             clutterLossPerElevation = "29.5 24.6 21.9 20.0 18.7 17.8 17.2 16.9 16.8";
//             if(tempLinkInfo.isLOS)
//                 shadowDevPerElevation = "1.9 1.6 1.9 2.3 2.7 3.1 3.0 3.6 0.4";
//             else
//                 shadowDevPerElevation = "10.7 10.0 11.2 11.6 11.8 10.8 10.8 10.8 10.8";
//         }
//         double CL = clutterLossPerElevation(index); //clutter Loss
//         tempLinkInfo.shadowStdDev = shadowDevPerElevation(index);
//         tempLinkInfo.pathLoss = (tempLinkInfo.isLOS) ? FSPL : FSPL + CL;
//         tempLinkInfo.elevationAngle = alpha;
//         
//         double buildingLoss = (isIndoor) ? computeBuildingPenetrationLoss(carrierFreqInGHz,isLowPenetrationLoss,alpha) : 0.0;//FIXME elevation angle of the path at the building façade - Should be properly set.
//         double atmosphericLoss = computeAtmosphericLoss(carrierFreqInGHz,alpha);
//         double scintillationLoss = computeScintillationLoss(carrierFreqInGHz,(nodeCategory1==_NODECATEGORY_SERVERNODE_)?nodeLoc2 : nodeLoc1,alpha);
//         tempLinkInfo.pathLoss += (buildingLoss + atmosphericLoss + scintillationLoss);
//     }
//     else 
//     {
//         tempLinkInfo.pathLoss = 100000;
//         tempLinkInfo.isLOS = 0;
//         tempLinkInfo.shadowStdDev = 0;
//         tempLinkInfo.shadowLoss=0;
//     }
//     return tempLinkInfo;
// }


