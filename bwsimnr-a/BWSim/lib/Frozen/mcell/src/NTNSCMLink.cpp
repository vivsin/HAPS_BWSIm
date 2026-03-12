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

#include "../include/SCMLink.h"
std::ofstream outFile("spreads.txt");
std::ofstream nlostapgain("nlostapgain.txt");
std::ofstream lostapgain("lostapgain.txt");
NTNSCMLink :: NTNSCMLink()
{
    propagationScenario = _SCM_UNINIT_;
    isReady = false;
    applyFaradayRotation = false;
    nTaps = 0;
    nClusters = 0;
    nRaysPerCluster = 0;
}

void NTNSCMLink::initAntennaParams(Antenna* txAntennaPtr,Antenna* rxAntennaPtr)
{
    double lamda = lightSpeed/carrierFrequencyInHz;

    txAntStructure = txAntennaPtr->antStructure;
    txTXRUMap = txAntennaPtr->TXRUMap;
    txNumOfTxRUs  = txAntennaPtr->numTxRUs;
    txAntSpacing = txAntennaPtr->antSpacing*lamda;
    
    sectorTilt = txAntennaPtr->getHorizontalTilt();
  
    rxAntStructure = rxAntennaPtr->antStructure;
    rxTXRUMap = rxAntennaPtr->TXRUMap;
    rxNumOfTxRUs = rxAntennaPtr->numTxRUs;
    rxAntSpacing = rxAntennaPtr->antSpacing*lamda;

    if(rxAntStructure(4)*txAntStructure(4)>4 || rxAntStructure(4)*txAntStructure(4)<1)
    {
        cout<<"Improper Number of Polarisations..."<<endl;abort();
    }
    
    calculateLocationVectorsAndPolAngles(txAntennaPtr,carrierFrequencyInHz,txLocationVectorPerAntennaElement,txPolarizationAngleInDegreePerAntennaElement);
    calculateLocationVectorsAndPolAngles(rxAntennaPtr,carrierFrequencyInHz,rxLocationVectorPerAntennaElement,rxPolarizationAngleInDegreePerAntennaElement);
    //cout<<"txAntStructure"<<txAntStructure<<endl;
    //cout<<"txTXRUMap"<< txTXRUMap<<endl;
    //cout<<"txNumOfTxRUs"<< txNumOfTxRUs<<endl;
    //cout<<"sectorTilt"<< sectorTilt<<endl;
    //cout<<"rxAntStructure"<<rxAntStructure <<endl;
    //cout<<"rxTXRUMap"<<rxTXRUMap <<endl;
    //cout<<"rxNumOfTxRUs"<< rxNumOfTxRUs<<endl;
    //cout<<"rxAntSpacing"<<rxAntSpacing <<endl;


}

void NTNSCMLink::initLocationParams(Location_S txLoc,Location_S rxLoc)
{
    linkStartLoc = rxLoc;
    linkEndLoc = txLoc;
    txHeight=txLoc.z;
    rxHeight=rxLoc.z;
    d3D =  find3Ddistance(txLoc,rxLoc);
    elevationAngle = getElevationAngle(txLoc,rxLoc);
    
    vec txAngles = find3DAngle(txLoc,rxLoc/*,txAntSpacing(2),txAntSpacing(3),txAntStructure(2)/txTXRUMap(0),txAntStructure(3)/txTXRUMap(1)*/); 
    theta_Tx = txAngles(0), phi_Tx = txAngles(1);
    
    vec rxAngles = find3DAngle(rxLoc,txLoc/*,rxAntSpacing(2),rxAntSpacing(3),rxAntStructure(2)/rxTXRUMap(0),rxAntStructure(3)/rxTXRUMap(1)*/); 
    theta_Rx = rxAngles(0), phi_Rx = rxAngles(1);
     //cout<<"linkStartLoc"<<linkStartLoc <<endl;
     //cout<<"linkEndLoc"<<linkEndLoc <<endl;
     //cout<<"txHeight"<< txHeight<<endl;
     //cout<<"rxHeight"<<rxHeight <<endl;
     //cout<<"d3D"<<d3D <<endl;
     //cout<<"elevationAngle"<<elevationAngle <<endl;
     //cout<<"txAngles"<<txAngles <<endl;
     //cout<<"rxAngles"<<rxAngles <<endl;

}

double NTNSCMLink :: initialize5GChannelFadingParams(int isOutdoortoIndoor, bool isLineOfSight, double dopplerSpread, double carrierFrequency, SCMPropagationScenario propScenario, SCMLargeScaleParameters_S& scmParams, Location_S nodeLocationTx, Location_S nodeLocationRx, Antenna* txAntennaPtr, Antenna* rxAntennaPtr)
{   double antenna_gain;
    isLOSChannel = isLineOfSight;
    carrierFrequencyInHz = carrierFrequency;
    isKaBandCarrier = isKaBand(carrierFrequencyInHz);
    propagationScenario = propScenario;
    //cout << "carrierFrequencyInHz and propagationScenario: "<< carrierFrequencyInHz << "," <<propagationScenario <<endl;
    initAntennaParams(txAntennaPtr,rxAntennaPtr);
    initLocationParams(nodeLocationTx,nodeLocationRx);
    scmLSPs = scmParams;    

    if(elevationAngle==0)
    {
        isReady = true;
        return 0;
    }
    
    nRaysPerCluster = 20;
    scmRowIndx = 2*int(propagationScenario-11) + int(!isLOSChannel);
    scmColIndx = round_i(elevationAngle/10)-1;
    nClusters = getClusterCount();
    scmLinkCondition = (isLOSChannel) ? _LOS_ : _NLOS_;
    
    double UE_sigma_AOD,UE_sigma_AOA,UE_sigma_DS,UE_sigma_ZOA,UE_sigma_ZOD,UE_sigma_SF;

    UE_sigma_DS = pow(10.0,scmParams.ds);
    UE_sigma_AOD = pow(10.0,scmParams.asd);
    UE_sigma_AOA = pow(10.0,scmParams.asa);
    UE_sigma_SF = scmParams.sf;
    UE_sigma_ZOA = pow(10.0,scmParams.zsa);
    UE_sigma_ZOD = pow(10.0,scmParams.zsd);

    K_rice = inv_dB(scmParams.k);
/*
    UE_sigma_DS = pow(10.0,-8.19);
    UE_sigma_AOD = pow(10.0,-1.78);
    UE_sigma_AOA = pow(10.0,-0.81);
    UE_sigma_SF = 0.72;
    UE_sigma_ZOA = pow(10.0,-0.46);
    UE_sigma_ZOD = pow(10.0,-2.65);
    K_rice = inv_dB(3.59);
   */
    //cout <<"scmParams.k  and K_rice: " << scmParams.k << "," << K_rice<<endl;
    muZSD = scmParams.muZSD;
    // START_DEBUG....
    //cout<<UE_sigma_DS<<" = pow(10.0,"<<scmParams.ds<<") "<<endl<<UE_sigma_AOD<<" = pow(10.0,"<<scmParams.asd<<") "<<endl<<UE_sigma_AOA<<" = pow(10.0,"<<scmParams.asa<<")"<<endl<<UE_sigma_SF<<" = "<<scmParams.sf<<endl<<UE_sigma_ZOA<<" = pow(10.0,"<<scmParams.zsa<<")"<<endl<<UE_sigma_ZOD<<" = pow(10.0,"<<scmParams.zsd<<")"<<endl<<K_rice<<" = inv_dB("<<scmParams.k<<")"<<endl<<muZSD<<" = "<<scmParams.muZSD<<endl<<endl;
    // END_DEBUG
    // Limiting the AOA and AOD to 104 degress & Limiting the ZOA and ZOD to 52 degress as per 3GPP TR 38.901 lsp GENERATION STEP 4
    UE_sigma_AOA = min(UE_sigma_AOA,104.0);
    UE_sigma_AOD = min(UE_sigma_AOD,104.0);
    UE_sigma_ZOA = min(UE_sigma_ZOA,52.0);
    UE_sigma_ZOD = min(UE_sigma_ZOD,52.0);
    
    generatePowerDelayProfiles(UE_sigma_DS);
    //cout<<UE_sigma_DS<<" = pow(10.0,"<<scmParams.ds<<") "<<endl<<UE_sigma_AOD<<" = pow(10.0,"<<scmParams.asd<<") "<<endl<<UE_sigma_AOA<<" = pow(10.0,"<<scmParams.asa<<")"<<endl<<UE_sigma_SF<<" = "<<scmParams.sf<<endl<<UE_sigma_ZOA<<" = pow(10.0,"<<scmParams.zsa<<")"<<endl<<UE_sigma_ZOD<<" = pow(10.0,"<<scmParams.zsd<<")"<<endl<<K_rice<<" = inv_dB("<<scmParams.k<<")"<<endl<<muZSD<<" = "<<scmParams.muZSD<<endl<<endl;

    //cout<<"UE_sigma_AOD,UE_sigma_AOA,UE_sigma_DS,UE_sigma_ZOA,UE_sigma_ZOD,UE_sigma_SF,K_rice,muZSD"<<UE_sigma_AOD<<","<<UE_sigma_AOA<<","<<UE_sigma_DS<<","<<UE_sigma_ZOA<<","<<UE_sigma_ZOD<<","<<UE_sigma_SF<<","<<K_rice<<","<<muZSD<<endl;
    clusterAOD = theta_Tx + generateAzimuthClusterAngles(0);
    clusterZOD = phi_Tx + generateZenithClusterAngles(0);
    clusterAOA = theta_Rx + generateAzimuthClusterAngles(UE_sigma_AOA);
    clusterZOA = ((isOutdoortoIndoor) ? 90 : phi_Rx) + generateZenithClusterAngles(UE_sigma_ZOA);

    //cout <<"clusterZOA <<clusterAOD << clusterZOA<< clusterAOA"<<clusterZOA  <<clusterAOD  << clusterZOA<<  clusterAOA<<endl;
    //cout<<"theta_Tx"<<theta_Tx<<endl;
    ///cout<<"theta_Rx"<<theta_Rx<<endl;
    //cout<<"phi_Tx"<<phi_Tx<<endl;
    //cout<<"phi_Rx"<<phi_Rx<<endl;

    
    txAntenna = *txAntennaPtr;
    rxAntenna = *rxAntennaPtr;
    
    generateCrossPolarizationRatios();
    generateSubpathPhases();
    generateChannelSubClusters();
    computeBoresight2subpath_5Gangles();
//     double thetaAngle;
    computeAntennaGains();
//     antenna_gain=computeAntennaGains(thetaAngle);
//     cout<< " antenna_gain : "<<antenna_gain<<endl;
    //cout<<" beam tilt angle (theta) in degree: "<<thetaAngle*180/pi<<endl;
    isReady = true;


    double m0;
    double delayspread=getRMSDelaySpread(m0);
    //cout<<"delayspread"<<delayspread<<endl;
    vec tapPow = getTapPowers();
    //cout<<"get tap pow: "<<tapPow<<endl;
    double m1,m2,m3,m4,s1,s2,s3,s4;
    s1 = getAngleSpread(tapPow,subpathAOD,m1);
    s2 = getAngleSpread(tapPow,subpathZOD,m2);
    s3 = getAngleSpread(tapPow,subpathAOA,m3);
    s4 = getAngleSpread(tapPow,subpathZOA,m4);
    //cout<<"s1 ,s2, s3, s4"<< dB(s1)<< " , "<<dB(s2)<<" , "<<dB(s3)<<" , "<<dB(s4)<<endl;
    //cout<<"s1 ,s2, s3, s4"<< s1<< " , "<<s2<<" , "<<s3<<" , "<<s4<<endl;


    if (!outFile) {
        std::cerr << "Error opening file for writing!" << std::endl;
        return 1;
    }

    outFile << delayspread << "   "<<m0<< "   "<<s1<< "   "<<m1<< "   "<<s2<< "   "<<m2<< "   "<<s3<< "   "<<m3<< "   "<<s4<<"   "<<m4<<endl;
//     cout<<"tapDelays : "<<tapDelays<<endl;
    return max(tapDelays);
}


int NTNSCMLink::getClusterCount()
{
    imat clusterCounts;
    if(!isKaBandCarrier)
    {
        clusterCounts = "3 3 3 3 3 3 3 3 3;"
                        "4 4 4 4 4 4 4 4 4;"
                        "4 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 2 2 2;"
                        "3 3 3 3 3 3 2 2 2;"
                        "4 4 4 4 4 3 3 3 3;"
                        "2 2 2 2 2 2 2 2 2;"
                        "3 3 2 2 2 2 2 2 2;";
    }
    else
    {
        clusterCounts = "3 3 3 3 3 3 3 3 3;"
                        "4 4 4 4 4 4 4 4 4;"
                        "4 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 2 2 2;"
                        "3 3 3 3 3 3 2 2 2;"
                        "4 4 4 4 4 3 3 3 3;"
                        "2 2 2 2 2 2 2 2 2;"
                        "3 3 2 2 2 2 2 2 2;";   //verified
    }
    return clusterCounts(scmRowIndx,scmColIndx);
}
double NTNSCMLink::getClusterShadow()
{
    mat clusterShadow;
    if(!isKaBandCarrier)
    {
        clusterShadow = "3 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 3 3 3;" //Santhosh
                        "3 3 3 3 3 3 3 3 3;";
    }
    else
    {
        clusterShadow = "3 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 3 3 3;"
                        "3 3 3 3 3 3 3 3 3;";          //verified
    }
    //cout << "scmRowIndx,scmColIndx :" << scmRowIndx << "," << scmColIndx << clusterShadow(scmRowIndx,scmColIndx) <<endl;
    //abort();
    return clusterShadow(scmRowIndx,scmColIndx);
}

double NTNSCMLink::getDelayScaling()
{
    mat delayScaling;
    if(!isKaBandCarrier)
    {
        delayScaling =  "2.5 2.5 2.5 2.5 2.5 2.5 2.5 2.5 2.5;"
                        "2.3 2.3 2.3 2.3 2.3 2.3 2.3 2.3 2.3;"
                        "2.5 2.5 2.5 2.5 2.5 2.5 2.5 2.5 2.5;"
                        "2.3 2.3 2.3 2.3 2.3 2.3 2.3 2.3 2.3;"
                        "2.20 3.36 3.50 2.81 2.39 2.73 2.07 2.04 2.04;"
                        "2.28 2.33 2.43 2.26 2.71 2.10 2.19 2.06 2.06;"
                        "3.8 3.8 3.8 3.8 3.8 3.8 3.8 3.8 3.8;"
                        "1.7 1.7 1.7 1.7 1.7 1.7 1.7 1.7 1.7;";
    }
    else
    {
        delayScaling =  "2.5 2.5 2.5 2.5 2.5 2.5 2.5 2.5 2.5;"
                        "2.3 2.3 2.3 2.3 2.3 2.3 2.3 2.3 2.3;"
                        "2.5 2.5 2.5 2.5 2.5 2.5 2.5 2.5 2.5;"
                        "2.3 2.3 2.3 2.3 2.3 2.3 2.3 2.3 2.3;"
                        "2.5 2.5 2.5 2.5 2.5 2.5 2.5 2.5 2.5;"
                        "2.3 2.3 2.3 2.3 2.3 2.3 2.3 2.3 2.3;"
                        "3.8 3.8 3.8 3.8 3.8 3.8 3.8 3.8 3.8;"
                        "1.7 1.7 1.7 1.7 1.7 1.7 1.7 1.7 1.7;";        //verified
    }
    return delayScaling(scmRowIndx,scmColIndx);
}

double NTNSCMLink::getRMSClusterDelaySpread()
{
    mat clusterDS;
    if(!isKaBandCarrier)
    {
        clusterDS = "3.9 3.9 3.9 3.9 3.9 3.9 3.9 3.9 3.9;"
                    "3.9 3.9 3.9 3.9 3.9 3.9 3.9 3.9 3.9;"
                    "3.9 3.9 3.9 3.9 3.9 3.9 3.9 3.9 3.9;"
                    "3.9 3.9 3.9 3.9 3.9 3.9 3.9 3.9 3.9;"
                    "1.6 1.6 1.6 1.6 1.6 1.6 1.6 1.6 1.6;"
                    "1.6 1.6 1.6 1.6 1.6 1.6 1.6 1.6 1.6;"
                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;";
    }
    else
    {
        clusterDS = "1.6 1.6 1.6 1.6 1.6 1.6 1.6 1.6 1.6;"
                    "3.9 3.9 3.9 3.9 3.9 3.9 3.9 3.9 3.9;"
                    "1.6 1.6 1.6 1.6 1.6 1.6 1.6 1.6 1.6;"
                    "1.6 1.6 1.6 1.6 1.6 1.6 1.6 1.6 1.6;"
                    "1.6 1.6 1.6 1.6 1.6 1.6 1.6 1.6 1.6;"
                    "1.6 1.6 1.6 1.6 1.6 1.6 1.6 1.6 1.6;"
                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;"
                    "-1 -1 -1 -1 -1 -1 -1 -1 -1;";            //verified
    }
    if(clusterDS(scmRowIndx,scmColIndx)==-1)
        return 3.91;
    return clusterDS(scmRowIndx,scmColIndx);
}

double NTNSCMLink::getRMSClusterAngleSpread(int isAzimuth_Zenith)
{
    double c_AS;
    mat cASD,cZSA,cASA;
    switch(isAzimuth_Zenith)
    {
        case 0: //ASD
            if(!isKaBandCarrier)
            {
                cASD =  "0 0 0 0 0 0 0 0 0;"
                        "0 0 0 0 0 0 0 0 0;"
                        "0.09 0.09 0.12 0.16 0.20 0.28 0.44 0.90 2.87;"
                        "0.08 0.1 0.14 0.23 0.33 0.53 1.0 1.4 6.63;"
                        "0 0 0 0 0 0 0 0 0;"
                        "0 0 0 0 0 0 0 0 0;"
                        "0.39 0.31 0.29 0.37 0.61 0.90 1.43 2.87 5.48;"
                        "0.03 0.05 0.07 1.0 0.15 0.22 0.5 1.04 2.11;";
            }
            else
            {
                cASD =  "0 0 0 0 0 0 0 0 0;"
                        "0 0 0 0 0 0 0 0 0;"
                        "0.09 0.09 0.11 0.15 0.18 0.27 0.42 0.86 2.55;" //santhosh
                        "0.08 0.1 0.14 0.22 0.31 0.49 0.97 1.52 5.36;"
                        "0 0 0 0 0 0 0 0 0;"
                        "0 0 0 0 0 0 0 0 0;"
                        "0.36 0.3 0.25 0.35 0.53 0.88 1.39 2.7 4.97;"
                        "0.03 0.05 0.07 0.09 0.16 0.22 0.51 0.89 1.68;";          //verified
            }
            c_AS = cASD(scmRowIndx,scmColIndx);
            break;
        case 1: //ASA
            if(!isKaBandCarrier)
            {
                cASA =  "11 11 11 11 11 11 11 11 11;"
                        "15 15 15 15 15 15 15 15 15;"
                        "12.55 12.76 14.36 16.42 17.13 19.01 19.31 22.39 27.8;"
                        "15.07 16.2 18.14 19.96 21.53 22.44 23.59 26.57 32.7;"
                        "11 11 11 11 11 11 11 11 11;"
                        "15 15 15 15 15 15 15 15 15;"
                        "10.81 8.09 13.7 20.05 24.51 26.35 31.84 36.62 36.77;" //santhosh
                        "18.16 26.82 21.99 22.86 25.93 27.79 28.5 37.53 29.23;";
            }
            else
            {
                cASA =  "11 11 11 11 11 11 11 11 11;"
                        "15 15 15 15 15 15 15 15 15;"
                        "11.78 11.6 13.05 14.56 15.35 16.97 17.96 20.68 25.08;"
                        "14.72 14.62 16.4 17.86 19.74 19.73 20.5 26.16 25.83;" 
                        "11 11 11 11 11 11 11 11 11;"
                        "15 15 15 15 15 15 15 15 15;"
                        "4.63 6.83 12.91 18.9 22.44 25.69 27.95 31.45 28.01;"
                        "18.21 24.08 22.06 21.4 24.26 24.15 25.99 36.07 24.51;";        //verified
                        
            }
            c_AS = cASA(scmRowIndx,scmColIndx);
            break;
        case 2: //ZSA
            if(!isKaBandCarrier)
            {
                cZSA =  "7 7 7 7 7 7 7 7 7;"
                        "7 7 7 7 7 7 7 7 7;"
                        "1.25 3.23 4.39 5.72 6.17 7.36 7.3 7.7 9.25;"
                        "1.66 4.71 7.33 9.82 11.52 11.75 10.93 12.19 16.68;"
                        "7 7 7 7 7 7 7 7 7;"
                        "7 7 7 7 7 7 7 7 7;"
                        "1.94 1.83 2.28 2.93 2.84 3.17 3.88 4.17 4.29;"
                        "2.32 7.34 8.28 8.76 9.68 9.94 8.9 13.74 12.16;";
            }
            else
            {
                cZSA =  "7 7 7 7 7 7 7 7 7;"
                        "7 7 7 7 7 7 7 7 7;"
                        "1.14 2.78 3.87 4.94 5.41 6.31 6.66 7.31 9.23;"
                        "1.57 4.3 6.64 9.21 10.32 10.3 10.2 12.27 12.75;"
                        "7 7 7 7 7 7 7 7 7;"
                        "7 7 7 7 7 7 7 7 7;"
                        "0.75 1.25 1.93 2.37 2.66 3.23 3.71 4.17 4.14;"
                        "2.13 6.52 7.72 8.45 8.92 8.76 9.0 13.6 10.56;";           //verified
            }
            c_AS = cZSA(scmRowIndx,scmColIndx);
            break;
        case 3: //ZSD
            c_AS = (3.0/8.0)*pow(10.0,muZSD);  
            break;
        default:
            cout<<"Invalid isAzimuth_Zenith.."<<isAzimuth_Zenith<<endl;abort();
            break;
    }
    return c_AS;
}

vec NTNSCMLink::generateSubpathAngleOffsets(int M,int  isAzimuth_Zenith) // isAzimuth_Zenith: 0=>AoD, 1=>AoA, 2=>ZoA, 3=>ZoD
{
  if (M != 20)
  {
    cout << "\n\n\ngenerate5GSubpathAngleOffsets: unsupported number of subpaths M -- exiting program" << endl;
    cout << "generate5GSubpathAngleOffsets: only M=20 is currently supported -- exiting program" << endl;
    exit(0);
  }
  if(isAzimuth_Zenith<0 || isAzimuth_Zenith>3)
  {
      cout<<"UnKnown isAzimuth_Zenith in getCluster5GRMSAzimuthSpread..."<<endl;exit(0);
  }
  
  vec subpathOffsets = "0.0447 -0.0447 0.1413 -0.1413 0.2492 -0.2492 0.3715 -0.3715 0.5129 -0.5129 0.6797 -0.6797 0.8844 -0.8844 1.1481 -1.1481 1.5195 -1.5195 2.1551 -2.1551";          //verified


  double rmsClusterAngleSpread = getRMSClusterAngleSpread(isAzimuth_Zenith);
  subpathOffsets = rmsClusterAngleSpread*subpathOffsets;
  //cout<<"rmsClusterAngleSpread"<<isAzimuth_Zenith<<"  : "<<rmsClusterAngleSpread<<endl;
  //cout << "subpathOffsets : " << subpathOffsets <<endl;
  return subpathOffsets;
  
  
}

vec NTNSCMLink::generateClusterAngles(vec& tapPow,double sigma,double C,bool isAzimuth,bool isLOS)
{
    vec clusterAngles(tapPow.length());
    double Pmax = max(tapPow);
    if(isAzimuth) //AOD,AOA
    {
        clusterAngles = (2.0*sigma/1.4)*sqrt(-1*log(tapPow/Pmax))/C;
        //cout<<"clusterAngles_check"<<clusterAngles<<endl;
        //Using inverse Wrapped Gaussian                 //verified
    }
    else    //ZOD,ZOA
    {
        clusterAngles = (-1*sigma)*log(tapPow/Pmax)/C;                  //Using inverse Laplacian 
    }
    
    vec Xn,Yn;
    

    Xn = 2*to_vec(randi(clusterAngles.length(),0,1))-1;
    Yn = (sigma/7.0)*randn(clusterAngles.length());
    
    
    clusterAngles = elem_mult(Xn,clusterAngles) + Yn;
    if(isLOS)
    {
        clusterAngles -= clusterAngles(0); //Refer Eq. 7.5-12 and 17 in 3GPP TR 38.901.
    }
    //cout << "clusterAngles : " << clusterAngles <<endl;
    return clusterAngles;
}

vec NTNSCMLink::generateAzimuthClusterAngles(double sigma_A)
{
    double C;

    switch (nClusters) // see 3GPP 3GPP TR 38.811 , Table 6.7.2-1aa
    {
        case 2:      C = 0.501;break;
        case 3:      C = 0.680;break;
        case 4:      C = 0.779;break;
        case 5:      C = 0.860;break;
        case 8:      C = 1.018;break;
        case 10:     C = 1.090;break;
        case 11:     C = 1.123;break;
        case 12:     C = 1.146;break;
        case 14:     C = 1.190;break;
        case 15:     C = 1.211;break;
        case 16:     C = 1.226;break;
        case 19:     C = 1.273;break;
        case 20:     C = 1.289;break;
        case 25:     C = 1.358;break;             //verified
        default :
        cout <<nClusters<< "\n\n\ngenerateClusterAngles: wrong number of clusters -- exiting program" << endl;
        abort();
        break;
    }
    //cout << "nClusters: " << nClusters <<" C :" << C <<endl;
    
    if(isLOSChannel) 
    {
        //cout << "isLOS v6: " << isLOS <<endl;
        double K_dB = 10*log10(K_rice);
        C = C*(1.1035 - 0.028*K_dB - 0.002*pow(K_dB,2) + 0.0001*pow(K_dB,3));          //verified
    }
    
    vec tapPow = getTapPowers();
    //cout << "tapPow: " << tapPow <<endl;
    return generateClusterAngles(tapPow,sigma_A,C,true,isLOSChannel); // Azimuth angle doesn't consider indoor or outdoor for LOS. 
}

vec NTNSCMLink::generateZenithClusterAngles(double sigma_Z)
{
    double C; 
    double K_dB = 10*log10(K_rice);
  
    switch (nClusters) // see 3GPP 3GPP TR 38.901, Table 7.5-4
    {
        case 2:     C = 0.430;break;
        case 3:     C = 0.594;break;
        case 4:     C = 0.697;break;
        case 8:     C = 0.889;break;
        case 10:     C = 0.957;break;
        case 11:     C = 1.031;break;
        case 12:     C = 1.104;break;
        case 15:     C = 1.1088;break;
        case 19:     C = 1.184;break;
        case 20:     C = 1.178;break;
        case 25:     C = 1.282;break;           //verified
        default :
            cout << "\n\n\ngenerateClusterAngles: wrong number of clusters -- exiting program" << endl;
            abort();
            break;
    }
    
    if(isLOSChannel)   
        C = C*(1.3086 + 0.0339*K_dB - 0.0077*pow(K_dB,2) + 0.0002*pow(K_dB,3));         //verified
  
    vec tapPow = getTapPowers();
    return generateClusterAngles(tapPow,sigma_Z,C,false,isLOSChannel); //only outdoor LOS is considered. 
}

vec NTNSCMLink::getXPRMuSigma()
{
    mat muMat,sigmaMat;
    if(!isKaBandCarrier)
    {
        muMat = "24.4 23.6 23.2 22.6 21.8 20.5 19.3 17.4 12.3;"
                "23.8 21.9 19.7 18.1 16.3 14.0 12.1 8.7 6.4;"
                "8 8 8 8 8 8 8 8 8;"
                "7 7 7 7 7 7 7 7 7;"
                "21.3 21.0 21.2 21.1 20.7 20.6 20.3 19.8 19.1;"
                "20.6 16.7 13.2 11.3 9.6 7.5 9.1 11.7 11.7;"
                "12 12 12 12 12 12 12 12 12;"
                "7 7 7 7 7 7 7 7 7;";
                
        sigmaMat =  "3.8 4.7 4.6 4.9 5.7 6.9 8.1 10.3 15.2;"
                    "4.4 6.3 8.1 9.3 11.5 13.3 14.9 17.0 12.3;"
                    "4 4 4 4 4 4 4 4 4;"
                    "3 3 3 3 3 3 3 3 3;"
                    "7.6 8.9 8.5 8.4 9.2 9.8 10.8 12.2 13.0;"
                    "8.5 12.0 12.8 13.8 12.5 11.2 10.1 13.1 13.1;"
                    "4 4 4 4 4 4 4 4 4;"
                    "3 3 3 3 3 3 3 3 3;";                                   //verified
    }
    else
    {
        muMat = "24.7 24.4 24.4 24.2 23.9 23.3 22.6 21.2 17.6;"
                "23.7 21.8 19.6 18.0 16.3 15.9 12.3 10.5 10.5;"
                "8 8 8 8 8 8 8 8 8;"
                "7 7 7 7 7 7 7 7 7;"
                "23.2 23.6 23.5 23.4 23.2 23.3 23.4 23.2 23.1;"
                "22.5 19.4 15.5 13.9 11.7 9.8 10.3 15.6 15.6;"
                "12 12 12 12 12 12 12 12 12;"
                "7 7 7 7 7 7 7 7 7;";
        
        sigmaMat =  "2.1 2.8 2.7 2.7 3.1 3.9 4.8 6.8 12.7;"
                    "4.5 6.3 8.2 9.4 11.5 12.4 15.0 15.7 15.7;"
                    "4 4 4 4 4 4 4 4 4;"
                    "3 3 3 3 3 3 3 3 3;"
                    "5.0 4.5 4.7 5.2 5.7 5.9 6.2 7.0 7.6;"
                    "5.0 8.5 10.0 10.6 10.0 9.1 9.1 9.1 9.1;"
                    "4 4 4 4 4 4 4 4 4;"
                    "3 3 3 3 3 3 3 3 3;";               //verified
    }
    return getVectorWithNumbers(2,muMat(scmRowIndx,scmColIndx),sigmaMat(scmRowIndx,scmColIndx));
}

void NTNSCMLink::generateSubpathPhases()
{
  int N = nTaps;
  subpathPhase_fixedpt.set_size(N);
  //cout<<"check randu"<<randu(100)<<endl;
  for (int i = 0; i< N; i++){
      //subpathPhase_fixedpt(i) = (2*randu(nRaysPerCluster)-1)*180;
      //subpathPhase_fixedpt(i) = (2*randu(1000)-1)*180;
      subpathPhase_fixedpt(i) = (2*randu(4*nRaysPerCluster)-1)*180;

      //cout<<"subpathPhase_fixedpt"<<":"<<subpathPhase_fixedpt<<endl;
      //cout<<"n:rays per cluster"<<nRaysPerCluster<<endl;

  }
  //cout << "N : " << N <<endl;
  if(applyFaradayRotation)
  {
      //abort();
      double si = 108.0/sqr(carrierFrequencyInHz/1e9);
      subpathFaradayPhase.set_size(N);
      for (int i = 0; i< N; i++){
        subpathFaradayPhase(i) = si;

      }
  }
}

void NTNSCMLink::generateCrossPolarizationRatios()
{
  vec xprMuSigma = getXPRMuSigma();  
  double mu = xprMuSigma(0),sigma = xprMuSigma(1);
  //cout<<"mu and sigma"<<mu<<","<<sigma<<endl;
  int N = nTaps;
  //cout << "nTaps : " << nTaps <<endl;
  XPR_lin = inv_dB(inv_dB(sigma)*randn(N,nRaysPerCluster)+inv_dB(mu));
  //XPR_lin = lognormal_rand(N,nRaysPerCluster,mu,sigma);
  //XPR_lin = inv_dB(sigma*randn(N,nRaysPerCluster)+mu);
  //cout <<" XPR_lin_value "<< XPR_lin<<endl;
  //cout <<"XPR_cdfplot"<<(inv_dB(sigma)*randn(1,5000)+inv_dB(mu))<<endl;
}

void NTNSCMLink::generatePowerDelayProfiles(double sigma_DS)
{
    vec Xn,tau,Pn_dash,Pn,Zn,tapdelays;
    double cutoff;
    double zeta,r_tau;
    ivec Pn_sort_index,skip_index;
    
    r_tau = getDelayScaling();
    //cout << "r_tau : "<< r_tau <<endl;
    zeta = getClusterShadow();
    // sigma_RND=pow(10.0,sigma_RND/10.0);// added for testing
    //cout << "zeta :" << zeta <<endl;
    //cout << "nClusters :" << nClusters<<endl;
    Xn = randu(nClusters);
    //cout << "Xn : "<< Xn <<endl;
    tau = -1*r_tau*sigma_DS*log(Xn);
    //tau = -1*r_tau*0.06*log(Xn);
    //cout << "sigma_DS : "<< sigma_DS <<endl;
    minTau = min(tau);
    tau = tau - minTau; //step 5
    sort(tau);
    
   zeta = pow(10, 0.1*zeta);
    Zn = zeta*randn(nClusters);
    Pn_dash = elem_mult(exp(-1*(r_tau-1)*tau/(r_tau*sigma_DS)),inv_dB(-1*Zn));
    //Pn_dash = elem_mult(exp(-1*(r_tau-1)*tau/(r_tau*0.06)),inv_dB(-1*Zn));
    Pn = Pn_dash/sum(Pn_dash);
    
    double K_dB = 10*log10(K_rice);
    double C_tau = (isLOSChannel) ? 0.7705 - 0.0433*K_dB + 0.0002*pow(K_dB,2) + 0.000017*pow(K_dB,3) : 1.0;            //verified
    
    ivec requiredIndices = getIntegers(0,Pn.length()-1);
    
    if(isLOSChannel)
    {
        //cout << "K_rice :" << K_rice <<endl;
        vec P_Los = Pn;
        //cout << "P_Los :" << P_Los <<endl;
        P_Los(0) += K_rice;
        //cout << "P_Los :" << P_Los <<endl;
        P_Los /= (K_rice+1);
        //cout << "P_Los :" << P_Los <<endl;
        //P_Los(0) /= Pn;
        cutoff = inv_dB(-25)*max(P_Los);
        requiredIndices = find(P_Los>=cutoff);
        //cout<<" requiredIndices"<< requiredIndices<<endl;
        //cout << "P_Los : " << P_Los<<endl;
    }
    else
    {
        cutoff = inv_dB(-25)*max(Pn);
        requiredIndices = find(Pn>=cutoff);
    }
    
    tapPow = Pn(requiredIndices);
    //cout << "Pn :" << Pn <<endl;
    //cout << "tapPow :" << tapPow <<endl;
    //cout << "requiredIndices : "<< requiredIndices <<endl;
    nTaps = tapPow.length();
    //cout << "nTap v1: "<< nTaps<<endl;
    tapDelays = tau(requiredIndices);
    //=======santhosh==============
    if(isLOSChannel)
    {
        tapDelays = tau(requiredIndices)/C_tau;
        //cout<<"tapDelays"<<tapDelays<<endl;
    }
    //=============================
    if(isFlatFadedChannel)
    {
        tapPow=ones(1);nTaps = 1;
        tapDelays.set_length(1,true);
    }
}

vec NTNSCMLink::getTapPowers()
{
    if(!tapPow.length())
    {
        cout<<"Please initialize 5G-Channel Link before calling get5GChannelTapPowers()..."<<endl;
        abort();
    }
    //cout<<"tapPow length"<<tapPow.length()<<endl;
    vec tapPowTemp = tapPow;
    //cout<<"tap powers"<<tapPowTemp<<endl;

    if(isLOSChannel)
    {
        tapPowTemp(0) += K_rice;
        tapPowTemp /= (K_rice+1);
    }
    //cout << "tapPowTemp : "<< tapPowTemp<<endl;
    return(tapPowTemp);
}

ivec NTNSCMLink::getSubPathIndicesofClusters(int setid)//see 3GPP TR 38.901 table 7.5-5
{
  ivec indices;
  switch (setid)
  {
    case 0:
      indices = "0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19"; 
      break;
    case 1:
      indices = "0 1 2 3 4 5 6 7 18 19";
      break;
    case 2:
      indices = "8 9 10 11 16 17";
      break;
    case 3:
      indices = "12 13 14 15";
      break;
    default :
      cout << "\n\n\nget5GSubPathIndicesofClusters: unknown cluster subpath set id --- exiting program" << endl;
      exit(0);
      break;
  }
  //cout << "indices : "<< indices<<endl;
  return indices;
}

void NTNSCMLink::generateChannelSubClusters() 
{
  double subdelay1,subdelay2;
  vec tmp1,allDelays,newtapPow5GChannel;
  ivec clusterSubPathtype,subPathIndices,angleMap,Pn_sort_index;
  int Q,n0,n1,s0,s1;
  int M = nRaysPerCluster;
  
  subPathIndices = getSubPathIndicesofClusters(0);
  //cout << "subPathIndices : " << subPathIndices <<endl;
  if (subPathIndices.length() != M)
  {
    cout << "generate5GChannelSubClusters: num of subpaths in original cluster should be same as set0 indicies" << endl;
    abort();
  }
  
  int nDelays = tapDelays.length();
  vec tempTapPow = tapPow;
  
  Pn_sort_index = sort_index(tempTapPow);
  //cout << "tempTapPow and Pn_sort_index " << tempTapPow << ","<< Pn_sort_index <<endl;
  ivec strong2;

    if(nDelays==1)
    {
        strong2 = ivec("0 0");
    }
    else
    {
        strong2 = Pn_sort_index.mid(Pn_sort_index.length()-2,2);
        // strong 
        //cout << "strong2 : " << strong2 << endl;
        if (tapDelays(strong2(0))>tapDelays(strong2(1)))
        {
            int tt = strong2(0);
            strong2(0) = strong2(1);
            strong2(1) = tt;
        }
    }
    //cout << "strong2 : " << strong2 << endl;
    n0 = strong2(0); // Strong 2 cluster indices 
    n1 = strong2(1);

    Q = 4;
    vec newDelays(Q);
    
    double c_DS = getRMSClusterDelaySpread(); //cluster delay spread see 3GPP TR 38.811
    //cout << "c_DS : " << c_DS << endl;
    subdelay1 = 1.28*c_DS*1e-9;//5e-9; 
    subdelay2 = 2.56*c_DS*1e-9;//10e-9;                           //verified
    
    newDelays(0) = tapDelays(n0)+subdelay1;
    newDelays(1) = tapDelays(n0)+subdelay2;
    newDelays(2) = tapDelays(n1)+subdelay1;
    newDelays(3) = tapDelays(n1)+subdelay2;
    //cout << "newDelays : " << newDelays << endl;
    /// Find the locations into which new delays falls in a sorted order
    ivec newIndex(newDelays.length());
    tmp1 = tapDelays;
    
    append(tmp1,newDelays);

    ivec newDelayIndex = sort_index(tmp1);    
    sort(tmp1);  
    newIndex =  find(newDelayIndex,getIntegers(newDelayIndex.length()-4,newDelayIndex.length()-1));
    
    allDelays = tmp1;
    //cout << "allDelays : " << allDelays << endl;
    newtapPow5GChannel =  tempTapPow;
    //cout << "newtapPow5GChannel : " << newtapPow5GChannel << endl;
    subPathIndices = getSubPathIndicesofClusters(1);
    //cout << "subPathIndices : " << subPathIndices << endl;
    newtapPow5GChannel(n0)=tempTapPow(n0)*(subPathIndices.length()/double(M));
    newtapPow5GChannel(n1)=tempTapPow(n1)*(subPathIndices.length()/double(M));
    //cout << "newtapPow5GChannel : " << newtapPow5GChannel << endl;
    clusterSubPathtype  = zeros_i(nDelays);
    clusterSubPathtype(n0) = clusterSubPathtype(n1) = 1;
    //cout << "clusterSubPathtype : " << clusterSubPathtype << endl;
    angleMap = getIntegers(0,nDelays-1);
    //cout << "angleMap : " << angleMap << endl;
    ivec newIndex1 = newIndex;
    //cout << "newIndex1 : " << newIndex1 << endl;
    sort(newIndex1);
    //cout << "newIndex1 : " << newIndex1 << endl;
    for(int indx =0;indx<Q;indx++)
    {
        newtapPow5GChannel.ins(newIndex1(indx),-1);
        clusterSubPathtype.ins(newIndex1(indx),-1);
        angleMap.ins(newIndex1(indx),-1);
    }
    //cout << "newtapPow5GChannel, clusterSubPathtype and angleMap : " << newtapPow5GChannel <<","<<clusterSubPathtype << "and"<<angleMap<< endl;
    ///find the index of each of the originial clusters in the new sorted order (of delays)
    ivec newClusterIndex = remove(newDelayIndex,newIndex);
    //cout << "newClusterIndex : " << newClusterIndex << endl;
    s0 = n0;
    s1 = n1;
    for (int i =0; i<newDelays.length();i++)
        if (tapDelays(n1)>newDelays(i))
        s1 = s1+1;

    subPathIndices = getSubPathIndicesofClusters(2); 
    clusterSubPathtype(newIndex(0)) = 2; 
    newtapPow5GChannel(newIndex(0)) = tempTapPow(n0)*subPathIndices.length()/M;
    angleMap(newIndex(0)) = n0;
    
    subPathIndices = getSubPathIndicesofClusters(3); 
    clusterSubPathtype(newIndex(3)) = 3;
    newtapPow5GChannel(newIndex(3)) = tempTapPow(n1)*subPathIndices.length()/M;
    angleMap(newIndex(3)) = n1;
    
    if (abs(allDelays(newIndex(1)) - (allDelays(s0) + subdelay2)) < abs(allDelays(newIndex(1)) - (allDelays(s1) + subdelay1))) // sub delay2 is 10 ns
    {
        subPathIndices = getSubPathIndicesofClusters(3);
        clusterSubPathtype(newIndex(1)) = 3;
        newtapPow5GChannel(newIndex(1)) = tempTapPow(n0)*subPathIndices.length()/M;
        angleMap(newIndex(1)) = n0;
        subPathIndices = getSubPathIndicesofClusters(2);
        clusterSubPathtype(newIndex(2)) = 2;
        newtapPow5GChannel(newIndex(2)) = tempTapPow(n1)*subPathIndices.length()/M;
        angleMap(newIndex(2)) = n1;
    }
    else
    {
        subPathIndices = getSubPathIndicesofClusters(2);
        clusterSubPathtype(newIndex(1)) = 2;
        newtapPow5GChannel(newIndex(1)) = tempTapPow(n1)*subPathIndices.length()/M;
        angleMap(newIndex(1)) = n1;
        subPathIndices = getSubPathIndicesofClusters(3);
        clusterSubPathtype(newIndex(2)) = 3;
        newtapPow5GChannel(newIndex(2)) = tempTapPow(n0)*subPathIndices.length()/M;
        angleMap(newIndex(2)) = n0;
    }
    
    // In case of only one tap remains, assuming n0=n1, repeated tap is deleted here.
    // if 

    if(tapDelays.length() == 1)
    {
        ivec detindx;
        detindx.set_length(2);
        detindx(0)= find(clusterSubPathtype,3);
        detindx(1)= find(clusterSubPathtype,2);
        detindx =-1*detindx;
        sort(detindx);
        detindx =-1*detindx;
        
        for(int i=0;i<detindx.length();i++)
        {
        newtapPow5GChannel.del(detindx(i));
        allDelays.del(detindx(i));
        angleMap.del(detindx(i));
        clusterSubPathtype.del(detindx(i));
        }
    }
    if(abs(sum(tempTapPow)-sum(newtapPow5GChannel))>0.0000001)
    {
        cout << "\n\n\ngenerate5GChannelSubClusters: Error in power calculation while adjusting for subclusters --- exiting program" << endl;
        exit(0);
    }

    tapPow = newtapPow5GChannel; 
    //cout<<"newtapPow5GChannel_power"<<  tapPow<<endl;
    nTaps = tapPow.length();
    //cout<<"tapPow.length()"<<tapPow.length()<<endl;
    tapDelays = allDelays;
    subPathtypeinCluster = clusterSubPathtype;
    //cout<<"clustersubpathtype_6or3"<<clusterSubPathtype<<endl;
    aMap = angleMap;
  
}

void NTNSCMLink::computeBoresight2subpath_5Gangles()
{
    if(!addPathSpecificAntennaGain) return;
    int nPaths = nRaysPerCluster;

    vec subpathAoDoffsets, subpathAoAoffsets,subpathZoAoffsets,subpathZoDoffsets;

    subpathAoDoffsets = generateSubpathAngleOffsets(nPaths,0);
    subpathAoAoffsets = generateSubpathAngleOffsets(nPaths,1);
    subpathZoAoffsets = generateSubpathAngleOffsets(nPaths,2);
    subpathZoDoffsets = generateSubpathAngleOffsets(nPaths,3);

    subpathAOD.set_length(nTaps);
    subpathZOD.set_length(nTaps);
    subpathAOA.set_length(nTaps);
    subpathZOA.set_length(nTaps);
    
    ivec idx(0);
    
    for (int tapCnt=0; tapCnt<nTaps; tapCnt++)
    {
        int clusterIndx = aMap(tapCnt);
        idx =  getSubPathIndicesofClusters(subPathtypeinCluster(tapCnt));
        nPaths = idx.length();
            
        subpathAOD(tapCnt).set_length(nPaths);
        subpathZOD(tapCnt).set_length(nPaths);
        subpathAOA(tapCnt).set_length(nPaths);
        subpathZOA(tapCnt).set_length(nPaths);
        
        ivec zodAssociation = randPick(idx,nPaths); //random Coupling AOA,ZOD with AOD within cluster or subcluster.
        ivec aoaAssociation = randPick(idx,nPaths);
        ivec zoaAssociation = randPick(zodAssociation,nPaths);//random Coupling ZOA with ZOD within cluster or subcluster.
        //cout<<"indices zod,aoa,zoa : "<<zodAssociation<<" | "<<aoaAssociation<<" | "<<zoaAssociation<<endl;
        
        for (int pathCnt = 0; pathCnt<nPaths; pathCnt++)
        {
            subpathAOD(tapCnt)(pathCnt) = clusterAOD(clusterIndx) + subpathAoDoffsets(idx(pathCnt));
            subpathZOD(tapCnt)(pathCnt) = clusterZOD(clusterIndx) + subpathZoDoffsets(zodAssociation(pathCnt));
            //cout<<"subpathAoDoffsets(idx(pathCnt) : "<<subpathAoDoffsets(idx(pathCnt))<<endl;
            //mapping ZOD to [0,180]
            subpathZOD(tapCnt)(pathCnt) = circmod(subpathZOD(tapCnt)(pathCnt),360.0);
            if(subpathZOD(tapCnt)(pathCnt)>180)
                subpathZOD(tapCnt)(pathCnt) = 360-subpathZOD(tapCnt)(pathCnt);
            // Map the angle to [-90, 90]
            //subpathZOD(tapCnt)(pathCnt) = (subpathZOD(tapCnt)(pathCnt) - 180.0) / 2.0;
            
            subpathAOA(tapCnt)(pathCnt) = clusterAOA(clusterIndx) + subpathAoAoffsets(aoaAssociation(pathCnt));
            subpathZOA(tapCnt)(pathCnt) = clusterZOA(clusterIndx) + subpathZoAoffsets(zoaAssociation(pathCnt));
            
            //mapping ZOA to [0,180]
            subpathZOA(tapCnt)(pathCnt) = circmod(subpathZOA(tapCnt)(pathCnt),360.0);
            if(subpathZOA(tapCnt)(pathCnt)>180)
                subpathZOA(tapCnt)(pathCnt) = 360-subpathZOA(tapCnt)(pathCnt);
            // Map the angle to [-90, 90]
            //subpathZOA(tapCnt)(pathCnt) = (subpathZOA(tapCnt)(pathCnt) - 180.0) / 2.0;

        }

    }
}

void NTNSCMLink::computeAntennaGains()
{

    txLosFieldAmplitude = compute5GLOSAntennaGains(&txAntenna,thetaTxLos,phiTxLos,theta_Tx,phi_Tx,carrierFrequencyInHz);
    rxLosFieldAmplitude = compute5GLOSAntennaGains(&rxAntenna,thetaRxLos,phiRxLos,theta_Rx,phi_Rx,carrierFrequencyInHz);

    //cout << "txLosFieldAmplitude and rxLosFieldAmplitude_test: " << sqr(txLosFieldAmplitude) << ","<< rxLosFieldAmplitude<<endl;
    int nTxOrients = txAntenna.orientationInfo.uniqueOrientations.length(),nRxOrients = rxAntenna.orientationInfo.uniqueOrientations.length();
    //cout << "addPathSpecificAntennaGain_test : " << addPathSpecificAntennaGain <<endl;
    if(addPathSpecificAntennaGain)
    {
        subpath_Boresight_AoD_angle = repeatAsArray(subpathAOD,nTxOrients);   //Effect of Orientation is added later.
        subpath_Boresight_ZoD_angle = repeatAsArray(subpathZOD,nTxOrients);
        
        subpath_Boresight_AoA_angle = repeatAsArray(subpathAOA,nRxOrients);   //Effect of Orientation is added later.
        subpath_Boresight_ZoA_angle = repeatAsArray(subpathZOA,nRxOrients);




        //cout<<"subpath_Boresight_AoD_angle_test"<<subpath_Boresight_AoD_angle <<endl<<"subpath_Boresight_ZoD_angle_test "<<subpath_Boresight_ZoD_angle <<endl<<"subpath_Boresight_AoA_angle_test "<<subpath_Boresight_AoA_angle <<endl<<"subpath_Boresight_ZoA_angle_test "<<subpath_Boresight_ZoA_angle <<endl;

        vec tapPow = getTapPowers();
        //abort();
        txAvgAntGain = compute5GPathSpecificAntennaGains(&txAntenna,subpathAoDZoDAntennaFieldAmplitude,subpath_Boresight_AoD_angle,subpath_Boresight_ZoD_angle,tapPow,carrierFrequencyInHz);
        //cout<<"subpathAoDZoDAntennaFieldAmplitude"<<subpathAoDZoDAntennaFieldAmplitude<<endl;
        rxAvgAntGain = compute5GPathSpecificAntennaGains(&rxAntenna,subpathAoAZoAAntennaFieldAmplitude,subpath_Boresight_AoA_angle,subpath_Boresight_ZoA_angle,tapPow,carrierFrequencyInHz);
    
        //cout << "txAvgAntGain_test and rxAvgAntGain_test : " << txAvgAntGain<< "," << rxAvgAntGain <<endl;
    }
}
double NTNSCMLink::computeAntennaGains(double& thetaAngle)
{

    txLosFieldAmplitude = compute5GLOSAntennaGain(&txAntenna,thetaTxLos,phiTxLos,theta_Tx,phi_Tx,thetaAngle,carrierFrequencyInHz);
    //cout<< "theta Angle value 01 : "<<thetaAngle<<endl;
    rxLosFieldAmplitude = compute5GLOSAntennaGain(&rxAntenna,thetaRxLos,phiRxLos,theta_Rx,phi_Rx,thetaAngle,carrierFrequencyInHz);
    //cout<< "theta Angle value 02 : "<<thetaAngle<<endl;
    //cout << "txLosFieldAmplitude and rxLosFieldAmplitude_test: " << sqr(txLosFieldAmplitude) << ","<< rxLosFieldAmplitude<<endl;
    int nTxOrients = txAntenna.orientationInfo.uniqueOrientations.length(),nRxOrients = rxAntenna.orientationInfo.uniqueOrientations.length();

    //cout << "addPathSpecificAntennaGain_test : " << addPathSpecificAntennaGain <<endl;
    if(addPathSpecificAntennaGain)
    {
        subpath_Boresight_AoD_angle = repeatAsArray(subpathAOD,nTxOrients);   //Effect of Orientation is added later.
        subpath_Boresight_ZoD_angle = repeatAsArray(subpathZOD,nTxOrients);

        subpath_Boresight_AoA_angle = repeatAsArray(subpathAOA,nRxOrients);   //Effect of Orientation is added later.
        subpath_Boresight_ZoA_angle = repeatAsArray(subpathZOA,nRxOrients);




        //cout<<"subpath_Boresight_AoD_angle_test"<<subpath_Boresight_AoD_angle <<endl<<"subpath_Boresight_ZoD_angle_test "<<subpath_Boresight_ZoD_angle <<endl<<"subpath_Boresight_AoA_angle_test "<<subpath_Boresight_AoA_angle <<endl<<"subpath_Boresight_ZoA_angle_test "<<subpath_Boresight_ZoA_angle <<endl;

        vec tapPow = getTapPowers();
        //abort();
        txAvgAntGain = compute5GPathSpecificAntennaGain(&txAntenna,subpathAoDZoDAntennaFieldAmplitude,subpath_Boresight_AoD_angle,subpath_Boresight_ZoD_angle,tapPow,thetaAngle,carrierFrequencyInHz);
        //cout<<"subpathAoDZoDAntennaFieldAmplitude"<<subpathAoDZoDAntennaFieldAmplitude<<endl;
        //cout<< "theta Angle value 03 : "<<thetaAngle<<endl;
        rxAvgAntGain = compute5GPathSpecificAntennaGain(&rxAntenna,subpathAoAZoAAntennaFieldAmplitude,subpath_Boresight_AoA_angle,subpath_Boresight_ZoA_angle,tapPow,thetaAngle,carrierFrequencyInHz);

        //cout << "txAvgAntGain_test and rxAvgAntGain_test : " << txAvgAntGain<< "," << rxAvgAntGain <<endl;
        //cout<< "theta Angle value 04 : "<<thetaAngle<<endl;
    }
    return max(dB(txAvgAntGain));
}


double NTNSCMLink::getDopplerPhase(mat r_tx,mat r_rx,double currentTimeInSeconds,bool isLOS)
{
    //NOTE: Its supports both dual mobility and single mobility.
    double lambda = lightSpeed/carrierFrequencyInHz,K = 2*pi/lambda;
     //Doppler frequency component Eqn 7.5-25 3GPP TR 38.901
    mat v_tx(3,1),v_rx(3,1);v_rx.set_col(0,rxVelocityVector),v_tx.set_col(0,txVelocityVector);
    double v_nm = (r_tx.T()*v_tx+r_rx.T()*v_rx)(0,0);
    if(!isLOS)
    {
        double p = 0.2,V_scatt = 0.0;
        double D = (2*randu()-1)*V_scatt,alpha = (randu()<p);
        v_nm += 2*D*alpha;
    }
    double c3 = K*v_nm*currentTimeInSeconds;
    //cout << "c3_print : "<< c3<< endl;
    return c3;
}

dComplex NTNSCMLink::
getPathGain(double txFieldAmplitude,double rxFieldAmplitude,cmat& Rot,mat& r_tx,mat& r_rx,int currentTxAntennaElement,int currentRxAntennaElement,double currentTimeInSeconds,bool isChannelGenForRSRP,bool isForLOSPath)
{
    double lamda=lightSpeed/carrierFrequencyInHz;
    mat txLocationVector = txLocationVectorPerAntennaElement(currentTxAntennaElement),rxLocationVector = rxLocationVectorPerAntennaElement(currentRxAntennaElement);
    double txPolAngle = txPolarizationAngleInDegreePerAntennaElement(currentTxAntennaElement),rxPolAngle = rxPolarizationAngleInDegreePerAntennaElement(currentRxAntennaElement);
    double K = 2*pi/lamda,deg2rad = pi/180; 
    double c1,c2,c3;
    cmat F_tx(2,1),F_rx(2,1);
    
    //Refer to Eqn 7.5-28 in 3GPP TR 38.901
    F_tx(0,0) = txFieldAmplitude*cos(deg2rad*txPolAngle);
    F_tx(1,0) = txFieldAmplitude*sin(deg2rad*txPolAngle);
    //cout << "txFieldAmplitude : "<< txFieldAmplitude<<endl;
    F_rx(0,0) = rxFieldAmplitude*cos(deg2rad*rxPolAngle);
    F_rx(1,0) = rxFieldAmplitude*sin(deg2rad*rxPolAngle);
    
    c1 = K*(r_rx.T()*rxLocationVector)(0,0);
    c2 = K*(r_tx.T()*txLocationVector)(0,0);
    //cout<<"check c1 and c2"<<expj(c1)<<","<<expj(c2)<<endl;
    //cout << "txPolAngle : "<< txPolAngle << " rxPolAngle: "<< rxPolAngle<<endl;
    //cout << "rxLocationVector : "<< rxLocationVector << " r_rx.T() : "<<r_rx.T()<<endl;
    //cout << "txLocationVector : "<< txLocationVector << " r_tx.T() : "<<r_tx.T()<<endl;
    //cout<<"TXFIELDAMPLITUDE"<<txFieldAmplitude<<endl;
    //cout<< "F_rx.T() :"<<F_rx.T()<<endl;
    //cout<<"F_tx :"<<(F_tx)<<endl;
    //cout<<"nlos_rot :"<<Rot<<endl;

    dComplex pathGain = (F_rx.T()*Rot*F_tx)(0,0)*expj(c1)*expj(c2);

    if(!isChannelGenForRSRP)
    {
        //abort();
//         cout<<"check_c3"<<endl;
        c3 = getDopplerPhase(r_tx,r_rx,currentTimeInSeconds,isForLOSPath);
        pathGain *= expj(c3);
    }
    //cout << "isLOS : "<< isLOS<<endl;
    if(isForLOSPath)
    {
        //abort();

        pathGain *= expj(-1*K*d3D);
        //cout << "expj(-1*K*d3D) : "<< expj(-1*K*d3D) <<endl;


    }
    if(isnan(abs(pathGain)))
    {
        cout<<"c1:"<<c1<<" c2:"<<c2<<" c3:"<<c3<<" gain:"<<(F_rx.T()*Rot*F_tx)(0,0)<<endl;
        abort();
    }
    //cout<<"c1:"<<c1<<" c2:"<<c2<<" c3:"<<c3<<" gain:"<<(F_rx.T()*Rot*F_tx)(0,0)<<endl;
    //cout<<"F_rx:"<<F_rx<<" F_tx:"<<F_tx<<endl;
    //cout << "pathGain (H) : " <<  pathGain <<endl;
     // if(!isLOS){
    //return F_tx(0,0);
     // }
    return pathGain;


}

cmat NTNSCMLink::generateRotationMatrix(int clusterIndx, int rayIndx)
{
  // Refer to 2nd Matrix of Eqn 7.5-28 in TR 38.901.
    double deg2rad = pi/180;
    
    cmat Rot;
    Rot.set_size(2,2);
    
    for(int i=0,p=0;i<2;i++)
        for(int j=0;j<2;j++,p++)
        {   //cout<<"p : "<<p<<endl;
            //cout<<"clusterIndx: "<<clusterIndx<<endl;
            //cout<<"rayIndx : "<<rayIndx<<endl;
            double phi =  deg2rad*subpathPhase_fixedpt(clusterIndx)(4*rayIndx+p);
            Rot(i,j) = expj(phi);
            //cout<<"phi:"<<phi<<endl;
            //cout<<"expj(phi)"<<expj(phi)<<endl;
        }
    //cout << "RotationMatrix :" << Rot <<endl;
    double Xpr = XPR_lin(clusterIndx,rayIndx);
    //cout<<"xpr"<<Xpr<<endl;
    
    Rot(0,1) *= sqrt(1/Xpr);
    Rot(1,0) *= sqrt(1/Xpr);
    
    if(applyFaradayRotation)
    {
        mat fRot(2,2);
        double phase = deg2rad*subpathFaradayPhase(clusterIndx)(rayIndx);
        fRot(0,0) = cos(phase); fRot(0,1) = -sin(phase);
        fRot(1,0) = sin(phase); fRot(1,1) = cos(phase);
        Rot = Rot*fRot;
        //cout<<"Rott"<<Rot<<endl;
    }
    //cout << "Rotation Matrix :" << Rot <<endl;
    return Rot;
}

dComplex NTNSCMLink::      generateTapGainPerClusterTxEtoRxE(int tapCnt,int currentTxAntennaElement,int currentRxAntennaElement, double currentTimeInSeconds, bool isChannelGenForRSRP,int txOrientationId, int rxOrientationId)
 {
     if(!addPathSpecificAntennaGain) //Added to have continuity in the code. - Goraknath.
     {
         return sqrt(tapPow(tapCnt))*generateTapGainForLOSClusterTxEtoRxE(tapCnt,currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds,isChannelGenForRSRP,txOrientationId,rxOrientationId);
     }
     cmat F_rx,F_tx,Rot;
     mat r_tx,r_rx;
     dComplex tapGain = dComplex(0,0);
     //cout<<" dComplex tapGain"<<tapGain<<endl;

     ivec idx = getSubPathIndicesofClusters(subPathtypeinCluster(tapCnt));
     //cout << "subPathtypeinCluster and tapCnt  : " << subPathtypeinCluster(tapCnt) << ", "<<tapCnt << endl;
     int M = idx.length(); // M is number of rays per sub cluster.
     //cout << "M : " << M << endl;
     int clusterIndx = aMap(tapCnt);
     //cout<<"clusterIndx"<<clusterIndx<<endl;
     vec txFieldAmplitude = subpathAoDZoDAntennaFieldAmplitude(txOrientationId)(tapCnt), rxFieldAmplitude = subpathAoAZoAAntennaFieldAmplitude(rxOrientationId)(tapCnt);
     //cout<<"subpathAoDZoDAntennaFieldAmplitude(txOrientationId)(tapCnt) : "<<subpathAoDZoDAntennaFieldAmplitude(txOrientationId)(tapCnt)<<endl;
     //cout<<"txOrientationId and rxOrientationId"<<txOrientationId<<","<<rxOrientationId<<endl;
     //cout << "txFieldAmplitude and rxFieldAmplitude :" << txFieldAmplitude << "," << rxFieldAmplitude<<endl;
     vec subpathAOAs = subpath_Boresight_AoA_angle(rxOrientationId)(tapCnt), subpathZOAs = subpath_Boresight_ZoA_angle(rxOrientationId)(tapCnt);
     vec subpathAODs = subpath_Boresight_AoD_angle(txOrientationId)(tapCnt), subpathZODs = subpath_Boresight_ZoD_angle(txOrientationId)(tapCnt);
     //cout << "subpathAOAs and subpathZOAs :" << subpathAOAs << "," << subpathZOAs<<endl;
     //cout << "subpathAODs and subpathZODs :" << subpathAODs << "," << subpathZODs<<endl;
     for(int pathCnt = 0; pathCnt<M; pathCnt++)
     {
         int rayIndx = idx(pathCnt);
         //cout<<"rayIndx"<<rayIndx<<endl;

         Rot = generateRotationMatrix(clusterIndx,rayIndx); // generates the 2nd Matrix of Eqn.


         r_rx = getCartesianVector(subpathAOAs(pathCnt),subpathZOAs(pathCnt));

         r_tx = getCartesianVector(subpathAODs(pathCnt),subpathZODs(pathCnt));


         dComplex pathGain = getPathGain(txFieldAmplitude(pathCnt),rxFieldAmplitude(pathCnt),Rot,r_tx,r_rx,currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds,isChannelGenForRSRP,0);
         //cout << "thetaTxNLos(txOrientationId) : " <<txFieldAmplitude(pathCnt) <<endl;
         //cout << "phiTxNLos(txOrientationId) : " <<rxFieldAmplitude(pathCnt)<<endl;
         //cout<<"Rot_Nlos"<<Rot;
         //cout<<"r_tx :Nlos "<<r_tx<<"r_rx : Nlos"<< r_rx<<endl;
         //cout<<"currentTxAntennaElement_NLOS "<<currentTxAntennaElement<<endl;
         //cout<<"currentRxAntennaElement_NLOS"<<currentRxAntennaElement<<endl;
         //cout<<"currentTimeInSeconds_NLOS"<<currentTimeInSeconds<<endl;
         //cout<<"isChannelGenForRSRP_NLOS"<<isChannelGenForRSRP<<endl;
         //cout<<"check path gain:"<<pathGain<<endl;
         tapGain += pathGain;
         //cout << "tapGain :" << tapGain <<endl;
    }
     //cout<<"tapGain value" <<tapGain<<endl;
     //cout << "tapGain and sqrt(Pn/m) :" << tapGain << "," << sqrt(tapPow(tapCnt)/M)<<endl;
     //cout<<"tap power: "<<tapPow<<endl;
     //cout<<"tap count : "<<tapCnt<<endl;

     return sqrt(tapPow(tapCnt)/M)*tapGain;

}

dComplex NTNSCMLink::generateTapGainForLOSClusterTxEtoRxE(int tapCnt, int currentTxAntennaElement, int currentRxAntennaElement, double currentTimeInSeconds, bool isChannelGenForRSRP,int txOrientationId , int rxOrientationId)
{
    cmat Rot;
    mat r_tx,r_rx;
    dComplex losTapGain = dComplex(0,0);
    
    r_tx.set_size(3,1);
    r_rx.set_size(3,1);
    
    //Refer Eqn 7.5-29 in 3GPP TR 38.901
    
    Rot = zeros_c(2,2);
    Rot(0,0) = 1.0;
    Rot(1,1) = -1.0;
    
    r_rx = getCartesianVector(thetaRxLos(rxOrientationId),phiRxLos(rxOrientationId));
    
    r_tx = getCartesianVector(thetaTxLos(txOrientationId),phiTxLos(txOrientationId));
    //cout << "thetaTxLos(txOrientationId),phiTxLos(txOrientationId) : " << thetaTxLos(txOrientationId) << "," << phiTxLos(txOrientationId) <<endl;

    losTapGain=getPathGain(txLosFieldAmplitude(txOrientationId),rxLosFieldAmplitude(rxOrientationId),Rot,r_tx,r_rx,currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds,isChannelGenForRSRP,1);

    //cout << "txLosFieldAmplitude and rxLosFieldAmplitude : " << txLosFieldAmplitude(txOrientationId) << "," << rxLosFieldAmplitude(rxOrientationId) <<endl;
    //cout<<"Rot_los"<<Rot;
    //cout<<"r_tx :los "<<r_tx;
    //cout<<"r_rx : los"<< r_rx;
    //cout<<"currentTxAntennaElement "<<currentTxAntennaElement<<endl;
    //cout<<"currentRxAntennaElement"<<currentRxAntennaElement<<endl;
    //cout<<"currentTimeInSeconds"<<currentTimeInSeconds<<endl;
    //cout<<"isChannelGenForRSRP"<<isChannelGenForRSRP<<endl;


    //cout << "losTapGain and abs : " << losTapGain << "," << abs(losTapGain)<<endl;
    //abort();
    return losTapGain;
}

cvec NTNSCMLink::generateTapGainsTxEtoRxE(int currentTxAntennaElement,int currentRxAntennaElement, double currentTimeInSeconds,bool isChannelGenForRSRP,int txOrientationId , int rxOrientationId)
{
    if(!isReady)
    {
        cout<<"[both:] 5GChannelLink not initialized ... Please call initializeFadingParams() before calling generateTapGains()..."<<endl;
        abort();
    }
    
    cvec tapGain = zeros_c(nTaps);
    //cout << "nTaps_check: " << nTaps <<endl;
    for(int i=0;i<nTaps;i++)
    {
       tapGain(i) = generateTapGainPerClusterTxEtoRxE(i,currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds,isChannelGenForRSRP,txOrientationId,rxOrientationId);
       //cout <<"tapgain v1:" << tapGain(i) << endl;
        //cout <<"i,currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds,isChannelGenForRSRP,txOrientationId,rxOrientationId"<<i<<","<<currentTxAntennaElement<<","<<currentRxAntennaElement<<","<<currentTimeInSeconds<<","<<isChannelGenForRSRP<<","<<txOrientationId<<","<<rxOrientationId<<endl;

    }

    //cout <<"tapgain v2:" << tapGain << endl;
    //cout<<"sum tpgainv2 "<<sum(tapGain)<<endl;
    nlostapgain<<cnt++<<" "<<sum(tapGain)<<endl;
    //cout << "generateTapGainsTxEtoRxE isLOS : "<< isLOS << endl;
    if(isLOSChannel)
    {
        //cout << "generateTapGainsTxEtoRxE isLOS :" << isLOS << endl;
        dComplex  losTapGain = generateTapGainForLOSClusterTxEtoRxE(0,currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds,isChannelGenForRSRP,txOrientationId,rxOrientationId);
        lostapgain<<cnt<<" "<<losTapGain<<endl;
        if(nTaps>1)
        {
            tapGain(0) += sqrt(K_rice)*losTapGain;
            tapGain = sqrt(1.0/(K_rice+1.0))*tapGain;
        }
        else
            tapGain(0) = losTapGain;
        
    }

    if(isnan(sum(abs(tapGain))))
    {
        cout<<"K_rice_5G: "<<K_rice<<"\ntapPow: "<<tapPow<<"\ntapDelays: "<<tapDelays<<endl;
        cout<<"TapGain cannot be NaN..."<<endl;abort();
    }
    //cout << "tapGain: " << tapGain <<endl;
    //abort();
    return tapGain;
}

void NTNSCMLink::set(NTNSCMLink* link)
{
    addPathSpecificAntennaGain = link->addPathSpecificAntennaGain;
    carrierFrequencyInHz = link->carrierFrequencyInHz;
//     dopplerSpreadInHz = link->dopplerSpreadInHz;
    linkStartLoc = link->linkStartLoc,linkEndLoc = link->linkEndLoc,d3D = link->d3D;
    nRaysPerCluster = link->nRaysPerCluster;
    scmRowIndx = link->scmRowIndx; scmColIndx = link->scmColIndx;
    txAntStructure = link->txAntStructure,rxAntStructure = link->rxAntStructure;
    tapPow = link->tapPow,tapDelays = link->tapDelays,nTaps = link->nTaps,minTau = link->minTau;
    theta_Tx = link->theta_Tx,phi_Tx = link->phi_Tx,theta_Rx = link->theta_Rx,phi_Rx = link->phi_Rx;
    clusterAOD = link->clusterAOD,clusterZOD = link->clusterZOD,clusterAOA = link->clusterAOA,clusterZOA = link->clusterZOA;
    subpathAOD = link->subpathAOD,subpathZOD = link->subpathZOD,subpathAOA = link->subpathAOA,subpathZOA = link->subpathZOA;
    txLocationVectorPerAntennaElement = link->txLocationVectorPerAntennaElement,rxLocationVectorPerAntennaElement = link->rxLocationVectorPerAntennaElement;
    txPolarizationAngleInDegreePerAntennaElement = link->txPolarizationAngleInDegreePerAntennaElement,rxPolarizationAngleInDegreePerAntennaElement = link->rxPolarizationAngleInDegreePerAntennaElement;
    XPR_lin = link->XPR_lin,K_rice = link->K_rice;
    aMap = link->aMap,subPathtypeinCluster = link->subPathtypeinCluster;
    isFlatFadedChannel = link->isFlatFadedChannel,addOxygenLoss = link->addOxygenLoss;
    applyFaradayRotation = link->applyFaradayRotation;
    isLOSChannel = link->isLOSChannel; // added --santhosh
    //cout << "isLOS on NTNSCMLink" << isLOS <<endl;
    //abort();
    generateSubpathPhases();
    isReady = true;
}

double NTNSCMLink::getRMSDelaySpread(double& mu)
{
    vec tapPow = getTapPowers();
    return getRMSSpread(tapDelays,tapPow,mu);
}


// string NTNSCMLink::getClusterSpreads()
// {
//     vec clusterPow = get5GChannelClusterPowers();
//     vec clusterDelays = get5GChannelTapDelays();
//     double muAOD,muZOD,muAOA,muZOA,muDS;
//     double sigmaDS = getRMSSpread(clusterDelays,get5GChannelTapPowers(),muDS);
//     double sigmaAOD = getRMSSpread(pathAoD5GChannel,clusterPow,muAOD);
//     double sigmaZOD = getRMSSpread(pathZoD5GChannel,clusterPow,muZOD);
//     double sigmaAOA = getRMSSpread(pathAoA5GChannel,clusterPow,muAOA);
//     double sigmaZOA = getRMSSpread(pathZoA5GChannel,clusterPow,muZOA);
//     stringstream ss;
//     ss<<muDS*1e9<<",\t"<<sigmaDS*1e9<<",\t"<<muAOD<<",\t"<<sigmaAOD<<",\t"<<muZOD<<",\t"<<sigmaZOD<<",\t"<<muAOA<<",\t"<<sigmaAOA<<",\t"<<muZOA<<",\t"<<sigmaZOA;
//     return ss.str();
// }

string NTNSCMLink::getAngleSpreads()
{
    vec tapPow = getTapPowers();
    double mu1,mu2,mu3,mu4,s1,s2,s3,s4;
    s1 = getAngleSpread(tapPow,subpathAOD,mu1);
    s2 = getAngleSpread(tapPow,subpathZOD,mu2);
    s3 = getAngleSpread(tapPow,subpathAOA,mu3);
    s4 = getAngleSpread(tapPow,subpathZOA,mu4);
    cout<<"s1 ,s2, s3, s4"<< s1<< " , "<<s2<<" , "<<s3<<" , "<<s4<<" , "<<endl;
    stringstream ss;
    ss<<mu1<<",\t"<<s1<<",\t"<<mu2<<",\t"<<s2<<",\t"<<mu3<<",\t"<<s3<<",\t"<<mu4<<",\t"<<s4;
    
    return ss.str();
}

vec NTNSCMLink::getTapDelays()
{
    if(!isReady)
    {
        cout<<"Please initialize 5G-Channel Link before calling getTapDelays()..."<<endl;
        return(vec(""));
    }
    return tapDelays;
}
