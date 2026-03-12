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

SCM5GLink::SCM5GLink(){
  
  propagationScenario5GChannel = _SCM_UNINIT_;
  channelModelType = _MODEL_UNINIT_;
  spConsistPtr =NULL;
  isReadyFor5GChannel=false;
  isLOS5GChannel=-1;
//   isOutdoorToIndoor5GChannel=-1;
  scmLinkCondition = _UNINIT_SCMLC_;
  scmGridIndx=-1;
  nClusters5GChannel=-1;
  isFlatFadedChannel=0;
  addOxygenLoss=0;
  carrierFrequencyInHz5GChannel = 0.0;
  dopplerSpreadInHz5GChannel = 0.0;
  mobileVelocity5GChannel = 0.0;
  tapPow5GChannel.set_length(0);
  tapDelays5GChannel.set_length(0);
  pathAoD5GChannel.set_length(0);
  pathAoA5GChannel.set_length(0);
  pathZoD5GChannel.set_length(0);
  pathZoA5GChannel.set_length(0);
  subpathPhase_5Gfixedpt.set_length(0);
  subpath_Boresight_5GAoD_angle.set_length(0);
  subpath_Boresight_5GAoA_angle.set_length(0);
  subpath_Boresight_5GZoD_angle.set_length(0);
  subpath_Boresight_5GZoA_angle.set_length(0);
  subpath5GAoDZoDAntennaFieldAmplitude.set_length(0),subpath5GAoAZoAAntennaFieldAmplitude.set_length(0);
  SubPathtypeinCluster5G.set_length(0);
  aMap5G.set_length(0);
  XPR_lin_5G.set_size(0,0);
  theta_Tx_5G=-1;
  theta_Rx_5G=-1;
  phi_Tx_5G=-1;
  phi_Rx_5G=-1;
  K_rice_5G=-1;
  txLos5GFieldAmplitude.set_length(0),rxLos5GFieldAmplitude.set_length(0);
  txAvg5GAntGain.set_length(0),rxAvg5GAntGain.set_length(0);
  thetaTxLos5G.set_length(0),thetaRxLos5G.set_length(0),phiTxLos5G.set_length(0),phiRxLos5G.set_length(0);
  
  txCrossPolarizationFlag5G=false,rxCrossPolarizationFlag5G=false;
  txAntStructure.set_length(0),rxAntStructure.set_length(0); //[Mg Ng M N P]
  txTXRUMap.set_length(0),rxTXRUMap.set_length(0);
  txAntSpacing.set_length(0),rxAntSpacing.set_length(0);
  txHeight=-1;
  rxHeight=-1;
//   sectorTilt = 0;
//SP module
  linkStartLoc.x = -1;
  linkStartLoc.y = -1;
  
  linkEndLoc.x = -1;
  linkEndLoc.y = -1;

  muOffSetZOD=-1;
  muZSD=-1;
//   scmGridsForSpConsist.set_length(0);
//   dcorrForSPConsistency.set_size(0,0);
  txLocationVectorPerAntennaElement.set_length(0);
  rxLocationVectorPerAntennaElement.set_length(0);
  txPolarizationAngleInDegreePerAntennaElement.set_length(0);
  rxPolarizationAngleInDegreePerAntennaElement.set_length(0);
  useGroundReflection = false;
  groundReflectedTap = -1;
}

bool SCM5GLink::is5GChannelInitialized(){
  
  return(isReadyFor5GChannel);
}


double SCM5GLink::initialize5GChannelFadingParams(int  isOutdoortoIndoor,bool isLineOfSight,double dopplerSpread,double carrierFrequency,SCMPropagationScenario propScenario,SCMLargeScaleParameters_S &channel5GParameters,Location_S nodeLocationTx, Location_S nodeLocationRx,Antenna* txAntennaPtr, Antenna* rxAntennaPtr)
{
//   isOutdoorToIndoor5GChannel=isOutdoortoIndoor;
  isLOS5GChannel = isLineOfSight;
    
    
  propagationScenario5GChannel = propScenario;
  
  if(!is5GSCMScenario(propagationScenario5GChannel))
  {
      cout<<"Improper 5G Propagation scenario.."<<propagationScenario5GChannel<<endl;abort();
  }
  if((propagationScenario5GChannel==_SCM5G_INDOOR_FACTORY_ || propagationScenario5GChannel == _SCM5G_INDOOR_OFFICE_) && isOutdoortoIndoor == 1)
  {
      cout<<"O2I is not supported for Indoor Propagation scenario..."<<endl;abort();
  }
  
  scmLinkCondition = (isOutdoortoIndoor) ? _O2I_ : (isLineOfSight) ? _LOS_ : _NLOS_;
  useGroundReflection = useGroundReflection && scmLinkCondition == _LOS_ && !isFlatFadedChannel;
  scmGridIndx = get5GSCMGridIndx(propagationScenario5GChannel,scmLinkCondition);
 
  carrierFrequencyInHz5GChannel = carrierFrequency;
  double lamda = (double) lightSpeed/carrierFrequencyInHz5GChannel;
  dopplerSpreadInHz5GChannel = dopplerSpread;
  mobileVelocity5GChannel = dopplerSpreadInHz5GChannel*lamda;
  
  txHeight=nodeLocationTx.z;
  rxHeight=nodeLocationRx.z;
  
  //SP module
  linkStartLoc = nodeLocationRx;
  linkEndLoc = nodeLocationTx;
  
  d3D =  find3Ddistance(linkStartLoc,linkEndLoc);
//   sectorTilt = txAntennaPtr->getHorizontalTilt();
  
  //Loading AAS Info
  txAntStructure = txAntennaPtr->antStructure;
  txTXRUMap = txAntennaPtr->TXRUMap;
  txNumOfTxRUs  = txAntennaPtr->numTxRUs;
  txCrossPolarizationFlag5G = txAntennaPtr->isAntennaCrossPolarized();
  txAntSpacing = txAntennaPtr->antSpacing*lamda;
  
  rxAntStructure = rxAntennaPtr->antStructure;
  rxTXRUMap = rxAntennaPtr->TXRUMap;
  rxNumOfTxRUs = rxAntennaPtr->numTxRUs;
  rxCrossPolarizationFlag5G = rxAntennaPtr->isAntennaCrossPolarized();
  rxAntSpacing = rxAntennaPtr->antSpacing*lamda;
  
  if(rxAntStructure(4)*txAntStructure(4)>4 || rxAntStructure(4)*txAntStructure(4)<1)
  {
        cout<<"Improper Number of Polarisations..."<<endl;abort();
  }
  
  initialize5GChannelModel(channel5GParameters,nodeLocationTx,nodeLocationRx,txAntennaPtr,rxAntennaPtr);
  
  return(max(tapDelays5GChannel));
}

void SCM5GLink::initialize5GChannelModel(SCMLargeScaleParameters_S& channel5GParameters,Location_S nodeLocationTx, Location_S nodeLocationRx,Antenna* txAntennaPtr, Antenna* rxAntennaPtr)
{
  
  isSpConsistEnable = true;
    
  nRaysPerCluster = 20;
  double UE_sigma_AOD,UE_sigma_AOA,UE_sigma_DS,UE_sigma_ZOA,UE_sigma_ZOD,UE_sigma_SF;
  
  UE_sigma_DS = pow(10.0,channel5GParameters.ds); 
  UE_sigma_AOD = pow(10.0,channel5GParameters.asd); 
  UE_sigma_AOA = pow(10.0,channel5GParameters.asa); 
  UE_sigma_SF = channel5GParameters.sf; 
  UE_sigma_ZOA = pow(10.0,channel5GParameters.zsa); 
  UE_sigma_ZOD = pow(10.0,channel5GParameters.zsd);
  
  muOffSetZOD = channel5GParameters.muOffSetZOD;
  muZSD = channel5GParameters.muZSD;
  
  if(propagationScenario5GChannel==_SCM5G_INDOOR_FACTORY_) delTau = pow(10.0,channel5GParameters.deltaTau);
  
  K_rice_5G = inv_dB(channel5GParameters.k);
  
  nClusters5GChannel = get5GChannelClusterCount(scmGridIndx);

//   ue_phase_5G = (2.0*randu()-1.0)*180.0;   //FIXME CHECK_VALIDITY
  
  // Limiting the AOA and AOD to 104 degress & Limiting the ZOA and ZOD to 52 degress as per 3GPP TR 38.900 lsp GENERATION STEP 4
  UE_sigma_AOA = min(UE_sigma_AOA,104.0);
  UE_sigma_AOD = min(UE_sigma_AOD,104.0);
  UE_sigma_ZOA = min(UE_sigma_ZOA,52.0);
  UE_sigma_ZOD = min(UE_sigma_ZOD,52.0);
  
  static bool printValues = true;
  if(currentTaskid==0 && printValues)
  {
      std::ofstream op("5GChannelParams.txt",std::ios::app);
      op<<"\n%linkCondition,\tscmGridIndx,\tnClusters,\trTau,\tzeta,\txprMu,\txprSigma,\tc_DS,\tc_ASD,\tc_ASA,\tc_ZSA"<<endl;
      int nLinkConditions = (propagationScenario5GChannel==_SCM5G_INDOOR_OFFICE_ || propagationScenario5GChannel == _SCM5G_INDOOR_FACTORY_) ? 2 : 3;
      int tempGridIndx = get5GSCMGridIndx(propagationScenario5GChannel,SCMLinkCondition(0));
      for(int i=0;i<nLinkConditions;i++,tempGridIndx++)
      {
        op<<getString(SCMLinkCondition(i))<<",\t"<<tempGridIndx<<",\t"<<get5GChannelClusterCount(tempGridIndx)<<",\t"<<get5GChannelDelayScaling(tempGridIndx)<<",\t"<<get5GChannelClusterShadow(tempGridIndx)<<",\t";
        vec xprMuSigma = getXPRMuSigma(tempGridIndx);
        op<<xprMuSigma(0)<<",\t"<<xprMuSigma(1)<<",\t";
        op<<get5GRMSClusterDelaySpread(carrierFrequencyInHz5GChannel/1e9,propagationScenario5GChannel,channelModelType,SCMLinkCondition(i))<<",\t";
        for(int j=0;j<3;j++)
            op<<get5GRMSClusterAngleSpread(j,tempGridIndx,muZSD,carrierFrequencyInHz5GChannel/1e9)<<",\t";
        op<<endl;
      }
      printValues = false;
  }
  
  linkStartLoc = nodeLocationRx,linkEndLoc = nodeLocationTx;
  
  getBoresight_LOS_3Dangle(nodeLocationTx,nodeLocationRx); //step 1c.
  
  mobiltyModeling.t0RxLoc=nodeLocationRx;
  mobiltyModeling.t0TxLoc=nodeLocationTx;
    
  // Refer Steps 5-11 in Section 7.5 of TR 38.901
  generate5GChannelPowerDelayProfiles(UE_sigma_DS); // Step 5 & 6
  
  //Refer Eq. 7.5-11 & 16 in TR 38.901.
  pathAoD5GChannel = theta_Tx_5G + generateAzimuth5GChannelClusterAngles(UE_sigma_AOD,1); // Step 7
  pathZoD5GChannel = (phi_Tx_5G + muOffSetZOD) + generateZenith5GChannelClusterAngles(UE_sigma_ZOD,2);
  pathAoA5GChannel = theta_Rx_5G + generateAzimuth5GChannelClusterAngles(UE_sigma_AOA,3);
  pathZoA5GChannel = ((scmLinkCondition == _O2I_) ? 90.0 : phi_Rx_5G) + generateZenith5GChannelClusterAngles(UE_sigma_ZOA,4);
  
  
//   cout<<"Info3 : "<<pathAoD5GChannel<<"\n"<<pathZoD5GChannel<<"\n"<<pathAoA5GChannel<<"\n"<<pathZoA5GChannel<<endl;
  
  txAntenna = *txAntennaPtr;
  rxAntenna = *rxAntennaPtr;
    
  initialize5GChannelModelFromStep9();
  
  isReadyFor5GChannel = true;
}

void SCM5GLink::initialize5GChannelModelFromStep9()
{  
  
  generate5GCrossPolarizationRatios();  // Step 9
  generate5GSubpathPhases(); // Step 10
  generate5GChannelSubClusters();// FIXME CHECK_VALIDITY Verify the logic and confirm with 38.900
  
  computeBoresight2subpath_5Gangles(); //Step 7 with step 8.
  
  //Generating Requirements for step 11
  compute5GAntennaGains();//calculates path specific and LOS fieldAmplitudes
  calculateLocationVectorsAndPolAngles(&txAntenna,carrierFrequencyInHz5GChannel,txLocationVectorPerAntennaElement,txPolarizationAngleInDegreePerAntennaElement);
  calculateLocationVectorsAndPolAngles(&rxAntenna,carrierFrequencyInHz5GChannel,rxLocationVectorPerAntennaElement,rxPolarizationAngleInDegreePerAntennaElement);
  
    if(useGroundReflection)
    {

        double losDelay = d3D/lightSpeed;
        double d2D = findDistance(linkStartLoc,linkEndLoc);
        double dGR = sqrt(sqr(d2D) + sqr(txHeight+rxHeight));
        double groundDelay = dGR/lightSpeed;
        double groundTapDelay = groundDelay - losDelay;
        double groundTapPower = K_rice_5G*sqr(d3D/dGR)/(K_rice_5G+1.0);
        if(propagationScenario5GChannel == _SCM5G_INDOOR_FACTORY_)
        {
            groundTapDelay = groundDelay + delTau;
        }
    
        if(tapDelays5GChannel(nTaps5GChannel-1)<groundTapDelay)
        {
            append(tapDelays5GChannel,groundTapDelay);
            append(tapPow5GChannel,groundTapPower);
            groundReflectedTap = nTaps5GChannel-1;
        }
        else
        {
            for(int tap_cnt=1;tap_cnt<nTaps5GChannel;tap_cnt++)
            {
                if(tapDelays5GChannel(tap_cnt)>=groundTapDelay && tapDelays5GChannel(tap_cnt-1)<groundTapDelay)
                {
                    tapDelays5GChannel.ins(tap_cnt,groundTapDelay);
                    tapPow5GChannel.ins(tap_cnt,groundTapPower);
                    groundReflectedTap = tap_cnt;break;
                }
            }
        }
        
    }
    
    
    /*
        * Abhijeet:
        * Additional Channel modelling for Absolute time of arrival
        * As per section 7.6.9 from 38.901
        * To support simulations in which absolute time of arrival is important, the propagation time delay due to the total path length is considered 
    */
    bool isAbsTimeArriveFlag = false;
    if(isAbsTimeArriveFlag)
    {
        tapDelays5GChannel = tapDelays5GChannel + d3D/lightSpeed; 
    }
    nTaps5GChannel = tapPow5GChannel.length();

}

void SCM5GLink::getSpatiallyConsistentPhase(int nRaysPerCluster, vec &subpathPhase , double &losPhase)
{
    
    
    #ifndef USING_ITPP
    cout<<"Spatial Consistency model works only with ITPP.. "<<endl;
    abort();
    #endif
    
    int losGridIndx = 0; 
    int scp =0;// this is index for spatial Consistency parameter in table 7. ... cluster delay / cluster shadow is 1st SCP so index = 0 
    losGridIndx = scmGridIndx;
    
    
    // Test Ground
    
    //     cout<<"dcorrForSPConsistency:"<<dcorrForSPConsistency<<endl;
    //     cout<<"NosOfClusters:"<<NosOfClusters<<endl;
    //     cout<<"scp:"<<scp<<endl;
    //     cout<<"losGridIndx:"<<losGridIndx<<endl;
    //     cout<<"coverageArea.xLen:"<<coverageArea.xLen<<endl;
    //     cout<<"coverageArea.yLen:"<<coverageArea.yLen<<endl;
    //     cout<<"scmGridsForSpConsist"<<scmGridsForSpConsist(scp)(losGridIndx%3)<<endl;
    
    
    dComplex normLocation;
    //     normLocation.real() = linkStartLoc.x+coverageArea.xLen/2;
    //     normLocation.imag() = linkStartLoc.y+coverageArea.yLen/2;
    Area_S coverageArea = spConsistPtr->coverageArea;
    normLocation = dComplex(linkStartLoc.x+coverageArea.xLen/2,linkStartLoc.y+coverageArea.yLen/2); 
    dComplex  n[4] , ni ;
    double dcorr ; vec d(4), r(4) , n_val(4);
    
    dcorr = spConsistPtr->dcorrForSPConsistency(scp,losGridIndx);
     
    
    int rowIndx = floor_i(imag(normLocation)/(3*dcorr)); // DS, NLOS
    int colIndx = floor_i(real(normLocation)/(3*dcorr));
     
     
    ni  = normLocation;
    n[0] = dComplex(colIndx * 3*dcorr,(rowIndx+1)*3*dcorr);
    n[1] = dComplex(colIndx * 3*dcorr,rowIndx*3*dcorr);
    n[2] = dComplex((colIndx+1) * 3*dcorr,rowIndx*3*dcorr);
    n[3] = dComplex((colIndx+1) * 3*dcorr,(rowIndx+1)*3*dcorr);
    
    int gridSubIndx=0;
    if(propagationScenario5GChannel!=_SCM5G_INDOOR_FACTORY_ && propagationScenario5GChannel!=_SCM5G_INDOOR_OFFICE_)    gridSubIndx = losGridIndx%3;
    else    gridSubIndx = 0;
    
    n_val[0] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx+1,colIndx);
    n_val[1] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx,colIndx);
    n_val[2] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx,colIndx+1);
    n_val[3] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx+1,colIndx+1);
    
    Array<vec> subpathPhasePerCorner(4);
    vec losPhasePerCorner(4);
    
    ivec currentState;
    RNG_get_state(currentState);
    
    for(int cr_cnt=0;cr_cnt<4;cr_cnt++)
    {
        setSeed(n_val[cr_cnt]);
        losPhasePerCorner(cr_cnt) = randn();
        subpathPhasePerCorner(cr_cnt) = randn(4*nRaysPerCluster);
    }
    
    RNG_set_state(currentState);   
    
    for(int ii = 0;ii<4;ii++)
    {
        d[ii] = abs(ni-n[ii]);
        r[ii] = exp(-d[ii]/dcorr);
    }
    
    vec subpathPhaseInterp=zeros(4*nRaysPerCluster);
    double losPhaseInterp = 0.0;
    
    vec rNorm = r/norm(r);
    
    for(int ii=0;ii<4;ii++)
    {
        subpathPhaseInterp+=rNorm[ii]*subpathPhasePerCorner(ii);
        losPhaseInterp+=rNorm[ii]*losPhasePerCorner(ii);
    }
    
    
    //Converting from gaussian to uniform
    losPhase = 1.0/2.0*(1.0+erf(losPhaseInterp/sqrt(2.0)));
    subpathPhase = 1.0/2.0*(1.0+erf(subpathPhaseInterp/sqrt(2.0)));
    
    
    return;
}

void SCM5GLink::getSpatiallyConsistentXPR(int N, int nRaysPerCluster, mat &XPR)
{
    
    #ifndef USING_ITPP
    cout<<"Spatial Consistency model works only with ITPP.. "<<endl;
    abort();
    #endif
    
    int losGridIndx = 0; 
    int scp =0;// this is index for spatial Consistency parameter in table 7. ... cluster delay / cluster shadow is 1st SCP so index = 0 
    losGridIndx = scmGridIndx;
    
    
    // Test Ground
    
    //     cout<<"dcorrForSPConsistency:"<<dcorrForSPConsistency<<endl;
    //     cout<<"NosOfClusters:"<<NosOfClusters<<endl;
    //     cout<<"scp:"<<scp<<endl;
    //     cout<<"losGridIndx:"<<losGridIndx<<endl;
    //     cout<<"coverageArea.xLen:"<<coverageArea.xLen<<endl;
    //     cout<<"coverageArea.yLen:"<<coverageArea.yLen<<endl;
    //     cout<<"scmGridsForSpConsist"<<scmGridsForSpConsist(scp)(losGridIndx%3)<<endl;
    
    
    dComplex normLocation;
    //     normLocation.real() = linkStartLoc.x+coverageArea.xLen/2;
    //     normLocation.imag() = linkStartLoc.y+coverageArea.yLen/2;
    Area_S coverageArea = spConsistPtr->coverageArea;
    normLocation = dComplex(linkStartLoc.x+coverageArea.xLen/2,linkStartLoc.y+coverageArea.yLen/2); 
    dComplex  n[4] , ni ;
    double dcorr ; vec d(4), r(4) , n_val(4);
    
    dcorr = spConsistPtr->dcorrForSPConsistency(scp,losGridIndx);
     
    
    int rowIndx = floor_i(imag(normLocation)/(3*dcorr)); // DS, NLOS
    int colIndx = floor_i(real(normLocation)/(3*dcorr));
    
     
     
    ni  = normLocation;
    n[0] = dComplex(colIndx * 3*dcorr,(rowIndx+1)*3*dcorr);
    n[1] = dComplex(colIndx * 3*dcorr,rowIndx*3*dcorr);
    n[2] = dComplex((colIndx+1) * 3*dcorr,rowIndx*3*dcorr);
    n[3] = dComplex((colIndx+1) * 3*dcorr,(rowIndx+1)*3*dcorr);
    
    int gridSubIndx=0;
    if(propagationScenario5GChannel!=_SCM5G_INDOOR_FACTORY_ && propagationScenario5GChannel!=_SCM5G_INDOOR_OFFICE_)    gridSubIndx = losGridIndx%3;
    else    gridSubIndx = 0;
    
    n_val[0] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx+1,colIndx);
    n_val[1] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx,colIndx);
    n_val[2] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx,colIndx+1);
    n_val[3] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx+1,colIndx+1);
    
    Array<mat> XPRPerCorner(4);
    
    ivec currentState;
    RNG_get_state(currentState);
    
    for(int cr_cnt=0;cr_cnt<4;cr_cnt++)
    {
        setSeed(n_val[cr_cnt]);
        XPRPerCorner(cr_cnt) = randn(N,nRaysPerCluster);
    }
    
    RNG_set_state(currentState);   
    
    for(int ii = 0;ii<4;ii++)
    {
        d[ii] = abs(ni-n[ii]);
        r[ii] = exp(-d[ii]/dcorr);
    }
    
    mat XPRInterp=zeros(N,nRaysPerCluster);
    vec rNorm = r/norm(r);
    
    for(int ii=0;ii<4;ii++)
    {
        XPRInterp+=rNorm[ii]*XPRPerCorner(ii);
    }
    
    XPR = XPRInterp;    
    
    return;
}

void SCM5GLink::getSpatiallyConsistentClusterDelayAndPower(vec &Xn, vec &Zn)
{
    
#ifndef USING_ITPP
    cout<<"Spatial Consistency model works only with ITPP.. "<<endl;
    abort();
#endif
    
    int losGridIndx = 0; 
    int scp =0;// this is index for spatial Consistency parameter in table 7. ... cluster delay / cluster shadow is 1st SCP so index = 0 
    losGridIndx = scmGridIndx;
    
    
    // Test Ground
    
    //     cout<<"dcorrForSPConsistency:"<<dcorrForSPConsistency<<endl;
    //     cout<<"NosOfClusters:"<<NosOfClusters<<endl;
    //     cout<<"scp:"<<scp<<endl;
    //     cout<<"losGridIndx:"<<losGridIndx<<endl;
    //     cout<<"coverageArea.xLen:"<<coverageArea.xLen<<endl;
    //     cout<<"coverageArea.yLen:"<<coverageArea.yLen<<endl;
    //     cout<<"scmGridsForSpConsist"<<scmGridsForSpConsist(scp)(losGridIndx%3)<<endl;
    
    
    dComplex normLocation;
    //     normLocation.real() = linkStartLoc.x+coverageArea.xLen/2;
    //     normLocation.imag() = linkStartLoc.y+coverageArea.yLen/2;
    Area_S coverageArea = spConsistPtr->coverageArea;
    normLocation = dComplex(linkStartLoc.x+coverageArea.xLen/2,linkStartLoc.y+coverageArea.yLen/2); 
    
    dComplex  n[4] , ni ;
    double dcorr ; vec d(4), r(4) , n_val(4);
    
    dcorr = spConsistPtr->dcorrForSPConsistency(scp,losGridIndx);
    
    int rowIndx = floor_i(imag(normLocation)/(3*dcorr)); // DS, NLOS
    int colIndx = floor_i(real(normLocation)/(3*dcorr));
    
     
     
    ni  = normLocation;
    n[0] = dComplex(colIndx * 3*dcorr,(rowIndx+1)*3*dcorr);
    n[1] = dComplex(colIndx * 3*dcorr,rowIndx*3*dcorr);
    n[2] = dComplex((colIndx+1) * 3*dcorr,rowIndx*3*dcorr);
    n[3] = dComplex((colIndx+1) * 3*dcorr,(rowIndx+1)*3*dcorr);
    
    int gridSubIndx=0;
    if(propagationScenario5GChannel!=_SCM5G_INDOOR_FACTORY_ && propagationScenario5GChannel!=_SCM5G_INDOOR_OFFICE_)    gridSubIndx = losGridIndx%3;
    else    gridSubIndx = 0;
            
    n_val[0] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx+1,colIndx);
    n_val[1] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx,colIndx);
    n_val[2] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx,colIndx+1);
    n_val[3] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx+1,colIndx+1);
    
    
    Array<vec> XnPerCorner(4),ZnPerCorner(4),Xnu(4);
    
    ivec currentState;
    RNG_get_state(currentState);
   
    for(int cr_cnt=0;cr_cnt<4;cr_cnt++)
    {
        setSeed(n_val[cr_cnt]);
        XnPerCorner(cr_cnt) = randn(nClusters5GChannel); 
        ZnPerCorner(cr_cnt) = randn(nClusters5GChannel);
    }
    
    RNG_set_state(currentState);   
    
    for(int ii = 0;ii<4;ii++)
    {
        d[ii] = abs(ni-n[ii]);
        r[ii] = exp(-d[ii]/dcorr);
    }
    
    vec rNorm = r/norm(r);
    
    vec XnInterp=zeros(nClusters5GChannel), ZnInterp=zeros(nClusters5GChannel);
    for(int ii=0;ii<4;ii++)
    {
        XnInterp+=rNorm[ii]*XnPerCorner(ii);
        ZnInterp+=rNorm[ii]*ZnPerCorner(ii);
    }
    
    
    //Converting from gaussian to uniform
    Xn = 1.0/2.0*(1.0+erf(XnInterp/sqrt(2.0)));
    
    Zn = ZnInterp;
    
    
    return;
}

void SCM5GLink::getSpatiallyConsistentClusterSignAndOffset(int length,vec &Xn, vec &Yn, unsigned int seedOffsetForSpatialConsistency)
{
    
    #ifndef USING_ITPP
    cout<<"Spatial Consistency model works only with ITPP.. "<<endl;
    abort();
    #endif
    
    int losGridIndx = 0; 
    int scp =0;// this is index for spatial Consistency parameter in table 7. ... cluster delay / cluster shadow is 1st SCP so index = 0 
    losGridIndx = scmGridIndx;
    
    
    // Test Ground
    
    //     cout<<"dcorrForSPConsistency:"<<dcorrForSPConsistency<<endl;
    //     cout<<"NosOfClusters:"<<NosOfClusters<<endl;
    //     cout<<"scp:"<<scp<<endl;
    //     cout<<"losGridIndx:"<<losGridIndx<<endl;
    //     cout<<"coverageArea.xLen:"<<coverageArea.xLen<<endl;
    //     cout<<"coverageArea.yLen:"<<coverageArea.yLen<<endl;
    //     cout<<"scmGridsForSpConsist"<<scmGridsForSpConsist(scp)(losGridIndx%3)<<endl;
    
    
    dComplex normLocation;
    //     normLocation.real() = linkStartLoc.x+coverageArea.xLen/2;
    //     normLocation.imag() = linkStartLoc.y+coverageArea.yLen/2;
    Area_S coverageArea = spConsistPtr->coverageArea;
    normLocation = dComplex(linkStartLoc.x+coverageArea.xLen/2,linkStartLoc.y+coverageArea.yLen/2); 
    dComplex  n[4] , ni ;
    double dcorr ; vec d(4), r(4) , n_val(4);
    
    dcorr = spConsistPtr->dcorrForSPConsistency(scp,losGridIndx);
     
    
    int rowIndx = floor_i(imag(normLocation)/(3*dcorr)); // DS, NLOS
    int colIndx = floor_i(real(normLocation)/(3*dcorr));
    
     
     
    ni  = normLocation;
    n[0] = dComplex(colIndx * 3*dcorr,(rowIndx+1)*3*dcorr);
    n[1] = dComplex(colIndx * 3*dcorr,rowIndx*3*dcorr);
    n[2] = dComplex((colIndx+1) * 3*dcorr,rowIndx*3*dcorr);
    n[3] = dComplex((colIndx+1) * 3*dcorr,(rowIndx+1)*3*dcorr);
    
    int gridSubIndx=0;
    if(propagationScenario5GChannel!=_SCM5G_INDOOR_FACTORY_ && propagationScenario5GChannel!=_SCM5G_INDOOR_OFFICE_)    gridSubIndx = losGridIndx%3;
    else    gridSubIndx = 0;
    
    n_val[0] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx+1,colIndx);
    n_val[1] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx,colIndx);
    n_val[2] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx,colIndx+1);
    n_val[3] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx+1,colIndx+1);
    
    Array<vec> XnPerCorner(4),YnPerCorner(4);
    
    ivec currentState;
    RNG_get_state(currentState);
    
    for(int cr_cnt=0;cr_cnt<4;cr_cnt++)
    {
        setSeed(n_val[cr_cnt]+seedOffsetForSpatialConsistency); // seed offset is used to create different seed based values for AOD/AOA/ZOD/ZOA
        XnPerCorner(cr_cnt) = randn(length);
        YnPerCorner(cr_cnt) = randn(length); 
    }
    
    
    RNG_set_state(currentState);   
    
    
    for(int ii = 0;ii<4;ii++)
    {
        d[ii] = abs(ni-n[ii]);
        r[ii] = exp(-d[ii]/dcorr);
    }
    
    vec rNorm = r/norm(r);
    
    vec XnInterp=zeros(length), YnInterp=zeros(length);
    for(int ii=0;ii<4;ii++)
    {
        XnInterp+=rNorm[ii]*XnPerCorner(ii);
        YnInterp+=rNorm[ii]*YnPerCorner(ii);
    }
    
    //Converting Xn from gaussian to uniform
    Xn = 1.0/2.0*(1.0+erf(XnInterp/sqrt(2.0)));
    Xn = 2.0*Xn-1.0;
    
    Yn = YnInterp;
    
    return;
}

void SCM5GLink::getSpatiallyConsistentRayCoupling(ivec idx , ivec &zodAssociation, ivec &aoaAssociation, ivec &zoaAssociation)
{
    
    //Since spatially consistentRay coupling is complicated to generate, we use the ray coupling corresponding to the closest corner in the grid - Dhiv
    
    int nPaths = idx.length();
    
    #ifndef USING_ITPP
    cout<<"Spatial Consistency model works only with ITPP.. "<<endl;
    abort();
    #endif
    
    int losGridIndx = 0; 
    int scp =0;// this is index for spatial Consistency parameter in table 7. ... cluster delay / cluster shadow is 1st SCP so index = 0 
    losGridIndx = scmGridIndx;
    
    
    // Test Ground
    
    //     cout<<"dcorrForSPConsistency:"<<dcorrForSPConsistency<<endl;
    //     cout<<"NosOfClusters:"<<NosOfClusters<<endl;
    //     cout<<"scp:"<<scp<<endl;
    //     cout<<"losGridIndx:"<<losGridIndx<<endl;
    //     cout<<"coverageArea.xLen:"<<coverageArea.xLen<<endl;
    //     cout<<"coverageArea.yLen:"<<coverageArea.yLen<<endl;
    //     cout<<"scmGridsForSpConsist"<<scmGridsForSpConsist(scp)(losGridIndx%3)<<endl;
    
    
    dComplex normLocation;
    //     normLocation.real() = linkStartLoc.x+coverageArea.xLen/2;
    //     normLocation.imag() = linkStartLoc.y+coverageArea.yLen/2;
    Area_S coverageArea = spConsistPtr->coverageArea;
    normLocation = dComplex(linkStartLoc.x+coverageArea.xLen/2,linkStartLoc.y+coverageArea.yLen/2); 
    dComplex  n[4] , ni ;
    double dcorr ; vec d(4), r(4) , n_val(4);
    
    
    
    dcorr = spConsistPtr->dcorrForSPConsistency(scp,losGridIndx);
     
    
    int rowIndx = floor_i(imag(normLocation)/(3*dcorr)); // DS, NLOS
    int colIndx = floor_i(real(normLocation)/(3*dcorr));
    
     
     
    ni  = normLocation;
    n[0] = dComplex(colIndx * 3*dcorr,(rowIndx+1)*3*dcorr);
    n[1] = dComplex(colIndx * 3*dcorr,rowIndx*3*dcorr);
    n[2] = dComplex((colIndx+1) * 3*dcorr,rowIndx*3*dcorr);
    n[3] = dComplex((colIndx+1) * 3*dcorr,(rowIndx+1)*3*dcorr);
    
    int gridSubIndx=0;
    if(propagationScenario5GChannel!=_SCM5G_INDOOR_FACTORY_ && propagationScenario5GChannel!=_SCM5G_INDOOR_OFFICE_)    gridSubIndx = losGridIndx%3;
    else    gridSubIndx = 0;
    
    n_val[0] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx+1,colIndx);
    n_val[1] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx,colIndx);
    n_val[2] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx,colIndx+1);
    n_val[3] = spConsistPtr->scmGridsForSpConsist(scp)(gridSubIndx)(rowIndx+1,colIndx+1);
    
    Array<ivec> zodAssociationPerCorner(4),aoaAssociationPerCorner(4), zoaAssociationPerCorner(4);
    
    ivec currentState;
    RNG_get_state(currentState);
    
    for(int cr_cnt=0;cr_cnt<4;cr_cnt++)
    {
        setSeed(n_val[cr_cnt]);
        
        zodAssociationPerCorner(cr_cnt) = randPick(idx,nPaths); 
        aoaAssociationPerCorner(cr_cnt) = randPick(idx,nPaths);
        zoaAssociationPerCorner(cr_cnt) = randPick(idx,nPaths);
    }
    
    RNG_set_state(currentState);   
    
    
    for(int ii = 0;ii<4;ii++)
    {
        d[ii] = abs(ni-n[ii]);
    }
    
    zodAssociation = zodAssociationPerCorner(min_index(d));
    aoaAssociation = aoaAssociationPerCorner(min_index(d));
    zoaAssociation = zoaAssociationPerCorner(min_index(d));
    
    return;
}


int get5GChannelClusterCount(int scmGridIndx)
{
    ivec clusterCounts;
    #ifdef _USING_CALIBRATION_
    clusterCounts = "12 19 12 "/*UMi*/"12 20 12 "/*UMa*/"11 10 10 "/*RMa*/"8 10 "/*InH*/"25 25"/*InF*/;
    #else
    clusterCounts = "12 19 12 "/*UMi*/"12 20 12 "/*UMa*/"11 10 10 "/*RMa*/"15 19 "/*InH*/"25 25"/*InF*/;
    #endif
    
    return clusterCounts(scmGridIndx);
}

double get5GChannelDelayScaling(int scmGridIndx)
{
    vec rtauMat;
    #ifdef _USING_CALIBRATION_
    rtauMat = "3.0 2.1 2.2 "/*UMi*/"2.5 2.3 2.2 "/*UMa*/"3.8 1.7 1.7 "/*RMa*/"2.15 1.84 "/*InH*/"2.7 3.0"/*InF*/;
    #else
    rtauMat = "3.0 2.1 2.2 "/*UMi*/"2.5 2.3 2.2 "/*UMa*/"3.8 1.7 1.7 "/*RMa*/"3.6 3.0 "/*InH*/"2.7 3.0"/*InF*/;
    #endif
    return rtauMat(scmGridIndx);
}

double get5GChannelClusterShadow(int scmGridIndx)
{
    vec zetaMat = "3.0 3.0 4.0 "/*UMi*/"3.0 3.0 4.0 "/*UMa*/"3.0 3.0 3.0 "/*RMa*/"6.0 3.0 "/*InH*/"4.0 3.0"/*InF*/;        
    
    return zetaMat(scmGridIndx);
}

vec getXPRMuSigma(int scmGridIndx)
{
    mat XPRMuSigmaMat;
    #ifdef _USING_CALIBRATION_
    XPRMuSigmaMat = "9.0 8.0 9.0 "/*UMi*/"8.0 7.0 9.0 "/*UMa*/"12.0 7.0 7.0 "/*RMa*/"15.0 12.0 "/*InH*/"12.0 11.0;"/*InF*/ //mu
                    "3.0 3.0 5.0 "/*UMi*/"4.0 3.0 5.0 "/*UMa*/"4.0 3.0 3.0 "/*RMa*/"3.0 7.0 "/*InH*/"6.0 6.0"/*InF*/;   //sigma
    #else
    XPRMuSigmaMat = "9.0 8.0 9.0 "/*UMi*/"8.0 7.0 9.0 "/*UMa*/"12.0 7.0 7.0 "/*RMa*/"11.0 10.0 "/*InH*/"12.0 11.0;"/*InF*/ //mu
                    "3.0 3.0 5.0 "/*UMi*/"4.0 3.0 5.0 "/*UMa*/"4.0 3.0 3.0 "/*RMa*/"4.0 4.0 "/*InH*/"6.0 6.0"/*InF*/;   //sigma
    #endif
    return XPRMuSigmaMat.get_col(scmGridIndx);
}

void SCM5GLink::generate5GChannelPowerDelayProfiles(double sigma_DS)
{
    vec Xn,tau,Pn_dash,Pn,Zn,tapdelays;
    double cutoff;
    double zeta,r_tau;
    ivec Pn_sort_index,skip_index;    
    r_tau = get5GChannelDelayScaling(scmGridIndx);
    zeta = get5GChannelClusterShadow(scmGridIndx);
    // sigma_RND=pow(10.0,sigma_RND/10.0);// added for testing
    
    //Below code will work only with ITPP as we are using RNG_get_state() and RNG_set_state() functions - Dhiv
    ivec currentState;
    
    if(isSpConsistEnable)
    {
        getSpatiallyConsistentClusterDelayAndPower(Xn,Zn);
        Zn = zeta*Zn;
    }
    else
    {
        Xn = randu(nClusters5GChannel); 
        Zn = zeta*randn(nClusters5GChannel); 
    }
    
    tau = -1*r_tau*sigma_DS*log(Xn);
    minTau = min(tau);
    
    tau = tau - minTau; //step 5
    sort(tau);
    
    Pn_dash = elem_mult(exp(-1*(r_tau-1)*tau/(r_tau*sigma_DS)),inv_dB(-1*Zn));
    Pn = Pn_dash/sum(Pn_dash);
    
    double K_dB = 10*log10(K_rice_5G);
    double C_tau = (scmLinkCondition==_LOS_) ? 0.7705 - 0.0433*K_dB + 0.0002*pow(K_dB,2) + 0.000017*pow(K_dB,3) : 1.0;
    
    ivec requiredIndices = getIntegers(0,Pn.length()-1);
    
    if(scmLinkCondition == _LOS_ && nClusters5GChannel!=25)
    {
        vec P_Los = Pn;
        P_Los(0) += K_rice_5G;
        P_Los /= (K_rice_5G+1);
        
        cutoff = inv_dB(-25)*max(P_Los);
        requiredIndices = find(P_Los>=cutoff);
    }
    else
    {
        cutoff = inv_dB(-25)*max(Pn);
        requiredIndices = find(Pn>=cutoff);
    }
    
    tapPow5GChannel = Pn(requiredIndices); 
    tapDelays5GChannel = tau(requiredIndices)/C_tau;
  
    mobiltyModeling.tau_n0 = tau(requiredIndices);
    
    if(isFlatFadedChannel)
    {
        tapPow5GChannel=ones(1);
        nTaps5GChannel = 1;
        tapDelays5GChannel.set_length(1,true);
        mobiltyModeling.tau_n0.set_length(1,true);
    }
    
    if(sum(tapDelays5GChannel<0))
    {
        cout<<tapDelays5GChannel<<" "<<C_tau<<" "<<dB(K_rice_5G)<<" "<<sigma_DS<<endl;
        cout<<"Tap Delays cannot be negative...Please Check.."<<endl;abort();
    }
    if(addOxygenLoss)
    {
        computeOxygenLoss(tapDelays5GChannel);
        tapPow5GChannel = elem_mult(inv_dB(-1*oxygenLossPerClusterIndB),tapPow5GChannel);
    }
    
    if(propagationScenario5GChannel==_SCM5G_INDOOR_FACTORY_ && scmLinkCondition != _LOS_)
    {
        tapDelays5GChannel += delTau;
    }
    
    nTaps5GChannel = tapPow5GChannel.length();
    
}

vec SCM5GLink::generateClusterAngles(vec& tapPow,double sigma,double C,bool isAzimuth,bool isLOS, unsigned int seedOffsetForSpatialConsistency)
{
    vec clusterAngles(tapPow.length());
    double Pmax = max(tapPow);
    if(isAzimuth) //AOD,AOA
    {
        clusterAngles = (2.0*sigma/1.4)*sqrt(-1*log(tapPow/Pmax))/C;
//         cout<<"clusterAngles_check"<<clusterAngles<<endl;
        //Using inverse Wrapped Gaussian                 //verified
    }
    else    //ZOD,ZOA
    {
        clusterAngles = (-1*sigma)*log(tapPow/Pmax)/C;                  //Using inverse Laplacian 
    }
    
    vec Xn,Yn;
    if(isSpConsistEnable)
    {
        getSpatiallyConsistentClusterSignAndOffset(clusterAngles.length(),Xn, Yn,seedOffsetForSpatialConsistency);
        Yn = (sigma/7.0)*Yn;
    }
    else
    {
        Xn = 2*to_vec(randi(clusterAngles.length(),0,1))-1;
        Yn = (sigma/7.0)*randn(clusterAngles.length());
    }
    
    
    
    clusterAngles = elem_mult(Xn,clusterAngles) + Yn;
    if(isLOS)
    {
        clusterAngles -= clusterAngles(0); //Refer Eq. 7.5-12 and 17 in 3GPP TR 38.901.
    }
//     cout << "clusterAngles : " << clusterAngles <<endl;
    return clusterAngles;
}

vec SCM5GLink::generateAzimuth5GChannelClusterAngles(double sigma_A, unsigned int seedOffsetForSpatialConsistency)
{
  double C;
  
  switch (nClusters5GChannel) // see 3GPP 3GPP TR 38.901 , Table 7.5-2
  {
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
    case 25:     C = 1.358;break;                                       //verified
    default :
      cout <<nClusters5GChannel<< "\n\n\ngenerateClusterAngles: wrong number of clusters -- exiting program" << endl;
      abort();
      break;
  }
  
  if(scmLinkCondition == _LOS_) 
  {
      double K_dB = 10*log10(K_rice_5G);
      C = C*(1.1035 - 0.028*K_dB - 0.002*pow(K_dB,2) + 0.0001*pow(K_dB,3));              //verified
  }
  
  vec tapPow = get5GChannelTapPowers();
  return generateClusterAngles(tapPow,sigma_A,C,true,scmLinkCondition == _LOS_,seedOffsetForSpatialConsistency); // Azimuth angle doesn't consider indoor or outdoor for LOS. 
}

vec SCM5GLink::generateZenith5GChannelClusterAngles(double sigma_Z, unsigned int seedOffsetForSpatialConsistency)
{
  double C; 
  double K_dB = 10*log10(K_rice_5G); 
  
  if(channelModelType == _MODEL_A_)
  {
    switch (nClusters5GChannel) // see 3GPP 3GPP TR 38.900, Table 7.5-4
    {
      case 10:     C = 0.9854;break;
      case 11:     C = 1.013;break;
      case 12:     C = 1.04;break;
      case 15:     C = 1.1088;break;
      case 19:     C = 1.1764;break;
      case 20:     C = 1.1918;break;
      default :
        cout << "\n\n\ngenerateClusterAngles: wrong number of clusters -- exiting program" << endl;
        abort();
        break;
    }
   
   if(scmLinkCondition == _LOS_)   
        C = C*(1.35 + 0.0202*K_dB - 0.0077*pow(K_dB,2) + 0.0002*pow(K_dB,3));
  }
  else if(channelModelType == _MODEL_B_)
  {
    switch (nClusters5GChannel) // see 3GPP 3GPP TR 38.900, Table 7.5-4
    {
      case 8 :     C = 0.889;break;
      case 10:     C = 0.957;break;
      case 11:     C = 1.031;break;
      case 12:     C = 1.104;break;
      case 15:     C = 1.1088;break;
      case 19:     C = 1.184;break;
      case 20:     C = 1.178;break;
      case 25:     C = 1.282;break;                                                  //verified
      default :
        cout << "\n\n\ngenerateClusterAngles: wrong number of clusters -- exiting program" << endl;
        abort();
        break;
    }
    
    if(scmLinkCondition == _LOS_)   
        C = C*(1.3086 + 0.0339*K_dB - 0.0077*pow(K_dB,2) + 0.0002*pow(K_dB,3));            //verified
  }
  else
  {
    cout << "\n\n\n channelModelType is not initialized" << endl;
    abort();
  }
  
  vec tapPow = get5GChannelTapPowers();
  return generateClusterAngles(tapPow,sigma_Z,C,false,scmLinkCondition == _LOS_,seedOffsetForSpatialConsistency); //only outdoor LOS is considered. 
}

void SCM5GLink::getBoresight_LOS_3Dangle(Location_S nodeLocationTx, Location_S nodeLocationRx)
{
    vec txAngles = find3DAngle(nodeLocationTx,nodeLocationRx/*,txAntSpacing(2),txAntSpacing(3),txAntStructure(2)/txTXRUMap(0),txAntStructure(3)/txTXRUMap(1)*/); 
    theta_Tx_5G = txAngles(0), phi_Tx_5G = txAngles(1);
    
    vec rxAngles = find3DAngle(nodeLocationRx,nodeLocationTx/*,rxAntSpacing(2),rxAntSpacing(3),rxAntStructure(2)/rxTXRUMap(0),rxAntStructure(3)/rxTXRUMap(1)*/); 
    theta_Rx_5G = rxAngles(0), phi_Rx_5G = rxAngles(1);
    
    d3D =  find3Ddistance(nodeLocationTx,nodeLocationRx);
//     cout<<"Info1 : "<<theta_Tx_5G<<" , "<<phi_Tx_5G<<" , "<<theta_Rx_5G<<" , "<<phi_Rx_5G<<endl;
    
}

void SCM5GLink::computeBoresight2subpath_5Gangles()
{
    int nTaps = nTaps5GChannel,nPaths = nRaysPerCluster;

    vec subpathAoDoffsets, subpathAoAoffsets,subpathZoAoffsets,subpathZoDoffsets;
    
    subpathAoDoffsets = generate5GSubpathAngleOffsets(nPaths,0); 
    subpathAoAoffsets = generate5GSubpathAngleOffsets(nPaths,1);
    subpathZoAoffsets = generate5GSubpathAngleOffsets(nPaths,2);
    subpathZoDoffsets = generate5GSubpathAngleOffsets(nPaths,3);
    
    subpathAOD.set_length(nTaps);
    subpathZOD.set_length(nTaps);
    subpathAOA.set_length(nTaps);
    subpathZOA.set_length(nTaps);
    
    ivec idx(0);
    
    for (int tapCnt=0; tapCnt<nTaps; tapCnt++)
    {
        int clusterIndx = aMap5G(tapCnt);
        idx =  get5GSubPathIndicesofClusters(SubPathtypeinCluster5G(tapCnt));
        nPaths = idx.length();
            
        subpathAOD(tapCnt).set_length(nPaths);
        subpathZOD(tapCnt).set_length(nPaths);
        subpathAOA(tapCnt).set_length(nPaths);
        subpathZOA(tapCnt).set_length(nPaths);
        
        // Below if condition commented as holding random coupling while the PDP is revised leads to error. Need to check how mandatory this is - Dhiv
        
//         if(!isReadyFor5GChannel) // Performed only once especially during reinit SCM in mobility simulations - Dhiv
        {
            if(isSpConsistEnable)
            {
                getSpatiallyConsistentRayCoupling(idx,zodAssociation,aoaAssociation,zoaAssociation);
            }
            else
            {
                zodAssociation = randPick(idx,nPaths); //random Coupling AOA,ZOD,ZOA with AOD within cluster or subcluster.
                aoaAssociation = randPick(idx,nPaths);
                zoaAssociation = randPick(idx,nPaths);
            }
        }        
        for (int pathCnt = 0; pathCnt<nPaths; pathCnt++)
        {
            subpathAOD(tapCnt)(pathCnt) = pathAoD5GChannel(clusterIndx) + subpathAoDoffsets(idx(pathCnt));
            subpathZOD(tapCnt)(pathCnt) = pathZoD5GChannel(clusterIndx) + subpathZoDoffsets(zodAssociation(pathCnt));
            
            //mapping ZOD to [0,180]
            subpathZOD(tapCnt)(pathCnt) = circmod(subpathZOD(tapCnt)(pathCnt),360.0);
            if(subpathZOD(tapCnt)(pathCnt)>180)
                subpathZOD(tapCnt)(pathCnt) = 360-subpathZOD(tapCnt)(pathCnt);
            
            subpathAOA(tapCnt)(pathCnt) = pathAoA5GChannel(clusterIndx) + subpathAoAoffsets(aoaAssociation(pathCnt));
            subpathZOA(tapCnt)(pathCnt) = pathZoA5GChannel(clusterIndx) + subpathZoAoffsets(zoaAssociation(pathCnt));
            
            //mapping ZOA to [0,180]
            subpathZOA(tapCnt)(pathCnt) = circmod(subpathZOA(tapCnt)(pathCnt),360.0);
            if(subpathZOA(tapCnt)(pathCnt)>180)
                subpathZOA(tapCnt)(pathCnt) = 360-subpathZOA(tapCnt)(pathCnt);
        }
    }
}

double getAntennaFieldAmplitude(Antenna* antennaPtr,Orientation_S& orientation,double& zenith,double& azimuth,double carrierFrequencyInHz)
{  
//     convertAngleFromGCStoLCS(zenith,azimuth,orientation.alpha,orientation.beta,orientation.gamma);

    //cout <<"zenith and azimuth : "<< zenith << "," << azimuth << ","<< orientation.alpha <<orientation.beta << orientation.gamma<<endl;
    //cout <<"getAntennaGain : "<< antennaPtr->getAntennaGain(azimuth,zenith,carrierFrequencyInHz) <<endl;
    //cout <<"sqrt of getAntennaGain in dB_test : "<< sqrt(inv_dB(antennaPtr->getAntennaGain(azimuth,zenith,carrierFrequencyInHz))) <<endl;
    //abort();
    return sqrt(inv_dB(antennaPtr->getAntennaGain(azimuth,zenith,carrierFrequencyInHz)));
}

//Computes LOS Antenna Gains across orientations and stores the rotated theta and phi for r_rx and r_tx computations.
vec compute5GLOSAntennaGains(Antenna* antennaPtr,vec& losTheta,vec& losPhi,double theta,double phi,double carrierFrequencyInHz)
{
    int nOrients = antennaPtr->orientationInfo.uniqueOrientations.length();
    vec losAmp(nOrients);
    losTheta = theta*ones(nOrients);
    losPhi = phi*ones(nOrients);
    //cout << "losTheta :" << losTheta << "losPhi :" << losPhi <<endl;
    //cout << "nOrients : "<< nOrients <<endl;
    for(int orientCnt=0;orientCnt<nOrients;orientCnt++)
    {
        Orientation_S orientation = antennaPtr->orientationInfo.uniqueOrientations(orientCnt);
        losAmp(orientCnt) = getAntennaFieldAmplitude(antennaPtr,orientation,losPhi(orientCnt),losTheta(orientCnt),carrierFrequencyInHz);
        //cout << "orientation : " << orientation <<endl;
    }

    //cout << "losAmp : "<< losAmp <<endl;
    //abort(); // remove abort santhosh
    return losAmp;
}

//computes Field Amplitudes and Average Antenna Gains. 
vec compute5GPathSpecificAntennaGains(Antenna* antennaPtr,Array<Array<vec>>& fieldAmplitudes,Array<Array<vec>>& boreSightAzimuthAngles,Array<Array<vec>>& boreSightZenithAngles,vec& tapPow,double carrierFrequencyInHz)
{
    int nOrients = antennaPtr->orientationInfo.uniqueOrientations.length();
    //cout<<"nOrients"<<nOrients<<endl;
    vec avgGains = zeros(nOrients);
    fieldAmplitudes.set_length(nOrients);
    for(int orientCnt=0;orientCnt<nOrients;orientCnt++)
    {
        Orientation_S orientation = antennaPtr->orientationInfo.uniqueOrientations(orientCnt);
        //cout<<"orientation nlos"<<orientation<<endl;
        fieldAmplitudes(orientCnt).set_length(boreSightAzimuthAngles(orientCnt).length());
        for(int tapCnt=0;tapCnt<boreSightAzimuthAngles(orientCnt).length();tapCnt++)
        {
            fieldAmplitudes(orientCnt)(tapCnt).set_length(boreSightAzimuthAngles(orientCnt)(tapCnt).length());
            //cout<<"field amplitude check: "<<fieldAmplitudes(orientCnt)(tapCnt)<<endl;
            for(int subpathCnt=0;subpathCnt<boreSightAzimuthAngles(orientCnt)(tapCnt).length();subpathCnt++)
            {   //cout<<"field ampli check"<<fieldAmplitudes(orientCnt)(tapCnt)(subpathCnt)<<endl;
                fieldAmplitudes(orientCnt)(tapCnt)(subpathCnt) = getAntennaFieldAmplitude(antennaPtr,orientation,boreSightZenithAngles(orientCnt)(tapCnt)(subpathCnt),boreSightAzimuthAngles(orientCnt)(tapCnt)(subpathCnt),carrierFrequencyInHz);
                //cout<<"field amplitude 5g"<<fieldAmplitudes(orientCnt)(tapCnt)(subpathCnt)<<endl;
            }
            avgGains(orientCnt) += tapPow(tapCnt)*mean(sqr(fieldAmplitudes(orientCnt)(tapCnt)));
            //cout<<"avgGain_"<<avgGains(orientCnt)<<endl;
            //cout<<"field amplitude:"<<fieldAmplitudes<<endl;
             //cout<<"field amplitude(orientCnt): "<<fieldAmplitudes(orientCnt)<<endl;
        }
    }
    //cout<<"avg Gain"<<avgGains/sum(tapPow)<<endl;
    return avgGains/sum(tapPow);

}
double getAntennaFieldAmplitude(Antenna* antennaPtr,Orientation_S& orientation,double& zenith,double& azimuth,double& thetaAngle,double carrierFrequencyInHz)
{
    convertAngleFromGCStoLCS(zenith,azimuth,orientation.alpha,orientation.beta,orientation.gamma);

    //cout <<"zenith and azimuth : "<< zenith << "," << azimuth << ","<< orientation.alpha <<orientation.beta << orientation.gamma<<endl;
    //cout <<"getAntennaGain : "<< antennaPtr->getAntennaGain(azimuth,zenith,carrierFrequencyInHz) <<endl;
    //cout <<"sqrt of getAntennaGain in dB_test : "<< sqrt(inv_dB(antennaPtr->getAntennaGains(azimuth,zenith,thetaAngle,carrierFrequencyInHz))) <<endl;
    //abort();
    //cout<<" theta angle value :" <<thetaAngle<<endl;
    return sqrt(inv_dB(antennaPtr->getAntennaGains(azimuth,zenith,thetaAngle,carrierFrequencyInHz)));
}

vec compute5GLOSAntennaGain(Antenna* antennaPtr,vec& losTheta,vec& losPhi,double theta,double phi,double& thetaAngle,double carrierFrequencyInHz)
{
    int nOrients = antennaPtr->orientationInfo.uniqueOrientations.length();
    vec losAmp(nOrients);
    losTheta = theta*ones(nOrients);
    losPhi = phi*ones(nOrients);
    //cout << "losTheta :" << losTheta << "losPhi :" << losPhi <<endl;
    //cout << "  "<< nOrients <<endl;
    for(int orientCnt=0;orientCnt<nOrients;orientCnt++)
    {
        Orientation_S orientation = antennaPtr->orientationInfo.uniqueOrientations(orientCnt);
        losAmp(orientCnt) = getAntennaFieldAmplitude(antennaPtr,orientation,losPhi(orientCnt),losTheta(orientCnt),thetaAngle,carrierFrequencyInHz);
        //cout << "orientation : " << orientation <<endl;
    }
    //cout<< "theta Angle value for LOS: "<<thetaAngle<<endl;
    //cout << "losAmp : "<< losAmp <<endl;
    //abort(); // remove abort santhosh
    return losAmp;
}

//computes Field Amplitudes and Average Antenna Gains.
vec compute5GPathSpecificAntennaGain(Antenna* antennaPtr,Array<Array<vec>>& fieldAmplitudes,Array<Array<vec>>& boreSightAzimuthAngles,Array<Array<vec>>& boreSightZenithAngles,vec& tapPow,double& thetaAngle,double carrierFrequencyInHz)
{
    int nOrients = antennaPtr->orientationInfo.uniqueOrientations.length();
    //cout<<"nOrients"<<nOrients<<endl;
    vec avgGains = zeros(nOrients);
    fieldAmplitudes.set_length(nOrients);
    for(int orientCnt=0;orientCnt<nOrients;orientCnt++)
    {
        Orientation_S orientation = antennaPtr->orientationInfo.uniqueOrientations(orientCnt);
        //cout<<"orientation nlos"<<orientation<<endl;
        fieldAmplitudes(orientCnt).set_length(boreSightAzimuthAngles(orientCnt).length());
        for(int tapCnt=0;tapCnt<boreSightAzimuthAngles(orientCnt).length();tapCnt++)
        {
            fieldAmplitudes(orientCnt)(tapCnt).set_length(boreSightAzimuthAngles(orientCnt)(tapCnt).length());
            //cout<<"field amplitude check: "<<fieldAmplitudes(orientCnt)(tapCnt)<<endl;
            for(int subpathCnt=0;subpathCnt<boreSightAzimuthAngles(orientCnt)(tapCnt).length();subpathCnt++)
            {   //cout<<"field ampli check"<<fieldAmplitudes(orientCnt)(tapCnt)(subpathCnt)<<endl;
                fieldAmplitudes(orientCnt)(tapCnt)(subpathCnt) = getAntennaFieldAmplitude(antennaPtr,orientation,boreSightZenithAngles(orientCnt)(tapCnt)(subpathCnt),boreSightAzimuthAngles(orientCnt)(tapCnt)(subpathCnt),thetaAngle,carrierFrequencyInHz);
                //cout<<"field amplitude 5g"<<fieldAmplitudes(orientCnt)(tapCnt)(subpathCnt)<<endl;
                //cout<< "theta Angle value for path specific 01 : "<<thetaAngle<<endl;
            }
            avgGains(orientCnt) += tapPow(tapCnt)*mean(sqr(fieldAmplitudes(orientCnt)(tapCnt)));
            //cout<<"avgGain_"<<avgGains(orientCnt)<<endl;
            //cout<<"field amplitude:"<<fieldAmplitudes<<endl;
             //cout<<"field amplitude(orientCnt): "<<fieldAmplitudes(orientCnt)<<endl;
             //cout<< "theta Angle value for path specific 02 : "<<thetaAngle<<endl;
        }
        //cout<< "theta Angle value for path specific 03 : "<<thetaAngle<<endl;
    }
    //cout<< "theta Angle value for path specific 04 : "<<thetaAngle<<endl;
    //cout<<"avg Gain"<<avgGains/sum(tapPow)<<endl;
    return avgGains/sum(tapPow);

}

void SCM5GLink::compute5GAntennaGains()

{
    txLos5GFieldAmplitude = compute5GLOSAntennaGains(&txAntenna,thetaTxLos5G,phiTxLos5G,theta_Tx_5G,phi_Tx_5G);
    rxLos5GFieldAmplitude = compute5GLOSAntennaGains(&rxAntenna,thetaRxLos5G,phiRxLos5G,theta_Rx_5G,phi_Rx_5G);

    int nTxOrients = txAntenna.orientationInfo.uniqueOrientations.length(),nRxOrients = rxAntenna.orientationInfo.uniqueOrientations.length();
    
    subpath_Boresight_5GAoD_angle = repeatAsArray(subpathAOD,nTxOrients);   //Effect of Orientation is added later.
    subpath_Boresight_5GZoD_angle = repeatAsArray(subpathZOD,nTxOrients);
    
    subpath_Boresight_5GAoA_angle = repeatAsArray(subpathAOA,nRxOrients);   //Effect of Orientation is added later.
    subpath_Boresight_5GZoA_angle = repeatAsArray(subpathZOA,nRxOrients);
    
    vec tapPow = get5GChannelTapPowers();
    
    txAvg5GAntGain = compute5GPathSpecificAntennaGains(&txAntenna,subpath5GAoDZoDAntennaFieldAmplitude,subpath_Boresight_5GAoD_angle,subpath_Boresight_5GZoD_angle,tapPow);
    rxAvg5GAntGain = compute5GPathSpecificAntennaGains(&rxAntenna,subpath5GAoAZoAAntennaFieldAmplitude,subpath_Boresight_5GAoA_angle,subpath_Boresight_5GZoA_angle,tapPow);
    
    if(useGroundReflection)
    {
        double d2D = findDistance(linkStartLoc,linkEndLoc);
        double ZOD = 180.0 - rad_to_deg(atan(d2D/(txHeight+rxHeight)));
        double AOD = theta_Tx_5G,AOA = AOD+180.0,ZOA = ZOD;
        
        txGround5GFieldAmplitude = compute5GLOSAntennaGains(&txAntenna,thetaTxGround5G,phiTxGround5G,AOD,ZOD);
        rxGround5GFieldAmplitude = compute5GLOSAntennaGains(&rxAntenna,thetaRxGround5G,phiRxGround5G,AOA,ZOA);
    }
}

void SCM5GLink::generate5GChannelSubClusters() 
{
  double subdelay1,subdelay2;
  vec tmp1,allDelays,newtapPow5GChannel;
  ivec clusterSubPathtype,subPathIndices,angleMap,Pn_sort_index;
  int Q,n0,n1,s0,s1;
  int M = nRaysPerCluster;
  
  subPathIndices = get5GSubPathIndicesofClusters(0);
  if (subPathIndices.length() != M)
  {
    cout << "generate5GChannelSubClusters: num of subpaths in original cluster should be same as set0 indicies" << endl;
    abort();
  }
  
  int nDelays = tapDelays5GChannel.length();
  vec tapPow = get5GChannelTapPowers();
  Pn_sort_index = sort_index(tapPow);
  
  if(nDelays==1)
  {
      strong2 = ivec("0 0");
  }
  else
  {
      strong2 = Pn_sort_index.mid(Pn_sort_index.length()-2,2);
      // strong 
      if (tapDelays5GChannel(strong2(0))>tapDelays5GChannel(strong2(1)))
      {
          int tt = strong2(0);
          strong2(0) = strong2(1);
          strong2(1) = tt;
      }
  }
  
  n0 = strong2(0); // Strong 2 cluster indices 
  n1 = strong2(1);

  Q = 4;
  vec newDelays(Q);
  
  double c_DS = get5GRMSClusterDelaySpread(carrierFrequencyInHz5GChannel/1e9,propagationScenario5GChannel,channelModelType,scmLinkCondition); //cluster delay spread see 3GPP TR 38.901  Eq 7.5-26
  subdelay1 = 1.28*c_DS*1e-9;//5e-9; 
  subdelay2 = 2.56*c_DS*1e-9;//10e-9;
  
  newDelays(0) = tapDelays5GChannel(n0)+subdelay1;
  newDelays(1) = tapDelays5GChannel(n0)+subdelay2;
  newDelays(2) = tapDelays5GChannel(n1)+subdelay1;
  newDelays(3) = tapDelays5GChannel(n1)+subdelay2;
  
  /// Find the locations into which new delays falls in a sorted order
  ivec newIndex(newDelays.length());
  tmp1 = tapDelays5GChannel;
  
  append(tmp1,newDelays);

  ivec newDelayIndex = sort_index(tmp1);    
  sort(tmp1);  
  newIndex =  find(newDelayIndex,getIntegers(newDelayIndex.length()-4,newDelayIndex.length()-1));
  
  allDelays = tmp1;
  newtapPow5GChannel =  tapPow5GChannel;
  subPathIndices = get5GSubPathIndicesofClusters(1);
  newtapPow5GChannel(n0)=tapPow5GChannel(n0)*(subPathIndices.length()/double(M));
  newtapPow5GChannel(n1)=tapPow5GChannel(n1)*(subPathIndices.length()/double(M));
  
  clusterSubPathtype  = zeros_i(nDelays);
  clusterSubPathtype(n0) = clusterSubPathtype(n1) = 1;
  
  angleMap = getIntegers(0,nDelays-1);
  
  ivec newIndex1 = newIndex;
  sort(newIndex1);
  for(int indx =0;indx<Q;indx++)
  {
    newtapPow5GChannel.ins(newIndex1(indx),-1);
    clusterSubPathtype.ins(newIndex1(indx),-1);
    angleMap.ins(newIndex1(indx),-1);
  }
  
  ///find the index of each of the originial clusters in the new sorted order (of delays)
  ivec newClusterIndex = remove(newDelayIndex,newIndex);
  s0 = n0;
  s1 = n1;
  for (int i =0; i<newDelays.length();i++)
    if (tapDelays5GChannel(n1)>newDelays(i))
      s1 = s1+1;

  subPathIndices = get5GSubPathIndicesofClusters(2); 
  clusterSubPathtype(newIndex(0)) = 2; 
  newtapPow5GChannel(newIndex(0)) = tapPow5GChannel(n0)*subPathIndices.length()/M;
  angleMap(newIndex(0)) = n0;
  
  subPathIndices = get5GSubPathIndicesofClusters(3); 
  clusterSubPathtype(newIndex(3)) = 3;
  newtapPow5GChannel(newIndex(3)) = tapPow5GChannel(n1)*subPathIndices.length()/M;
  angleMap(newIndex(3)) = n1;
  
  if (abs(allDelays(newIndex(1)) - (allDelays(s0) + subdelay2)) < abs(allDelays(newIndex(1)) - (allDelays(s1) + subdelay1))) // sub delay2 is 10 ns
  {
    subPathIndices = get5GSubPathIndicesofClusters(3);
    clusterSubPathtype(newIndex(1)) = 3;
    newtapPow5GChannel(newIndex(1)) = tapPow5GChannel(n0)*subPathIndices.length()/M;
    angleMap(newIndex(1)) = n0;
    subPathIndices = get5GSubPathIndicesofClusters(2);
    clusterSubPathtype(newIndex(2)) = 2;
    newtapPow5GChannel(newIndex(2)) = tapPow5GChannel(n1)*subPathIndices.length()/M;
    angleMap(newIndex(2)) = n1;
  }
  else
  {
    subPathIndices = get5GSubPathIndicesofClusters(2);
    clusterSubPathtype(newIndex(1)) = 2;
    newtapPow5GChannel(newIndex(1)) = tapPow5GChannel(n1)*subPathIndices.length()/M;
    angleMap(newIndex(1)) = n1;
    subPathIndices = get5GSubPathIndicesofClusters(3);
    clusterSubPathtype(newIndex(2)) = 3;
    newtapPow5GChannel(newIndex(2)) = tapPow5GChannel(n0)*subPathIndices.length()/M;
    angleMap(newIndex(2)) = n0;
  }
  
  // In case of only one tap remains, assuming n0=n1, repeated tap is deleted here.
  // if 
  if(tapDelays5GChannel.length() == 1)
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

  if(abs(sum(tapPow5GChannel)-sum(newtapPow5GChannel))>0.0000001)
  {
    cout << "\n\n\ngenerate5GChannelSubClusters: Error in power calculation while adjusting for subclusters --- exiting program" << endl;
    exit(0);
  }

  tapPow5GChannel = newtapPow5GChannel; 
  nTaps5GChannel = tapPow5GChannel.length();
  tapDelays5GChannel = allDelays;
  SubPathtypeinCluster5G = clusterSubPathtype;
  aMap5G = angleMap;
}

ivec SCM5GLink::get5GSubPathIndicesofClusters(int setid)
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
  
  return indices;
}

void SCM5GLink::generate5GSubpathPhases()
{
  int N = getNumberOfClusters();  
  subpathPhase_5Gfixedpt.set_size(N);
  
  vec subpathPhase;
  double losPhase;

  if(isSpConsistEnable)
  {
      getSpatiallyConsistentPhase(nRaysPerCluster , subpathPhase , losPhase); 
  }
  else
  {
      subpathPhase = randu(4*nRaysPerCluster);
      losPhase = randu();
  }
  
  
  for (int i = 0; i< N; i++)
      subpathPhase_5Gfixedpt(i) = (2*subpathPhase-1)*180;
  losPhase5G = (2*losPhase-1)*180.0;
}

int SCM5GLink::getNumberOfClusters()
{
//     if(aMap5G.length()) return max(aMap5G)+1;
    return nTaps5GChannel;
}

void SCM5GLink::generate5GCrossPolarizationRatios()
{
  vec xprMuSigma = getXPRMuSigma(scmGridIndx);  
  double mu = xprMuSigma(0),sigma = xprMuSigma(1);
   
  int N = getNumberOfClusters();
  
  mat XPR;
  if(isSpConsistEnable)
  {
      getSpatiallyConsistentXPR(N, nRaysPerCluster, XPR);
  }
  else
  {
      XPR = randn(N,nRaysPerCluster);
  }
  
  
  XPR_lin_5G = inv_dB(sigma*XPR+mu);
}

vec SCM5GLink::generate5GSubpathAngleOffsets(int M,int  isAzimuth_Zenith) // isAzimuth_Zenith: 0=>AoD, 1=>AoA, 2=>ZoA, 3=>ZoD
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
  
  vec subpathOffsets = "0.0447 -0.0447 0.1413 -0.1413 0.2492 -0.2492 0.3715 -0.3715 0.5129 -0.5129 0.6797 -0.6797 0.8844 -0.8844 1.1481 -1.1481 1.5195 -1.5195 2.1551 -2.1551";

  double rmsClusterAngleSpread = get5GRMSClusterAngleSpread(isAzimuth_Zenith,scmGridIndx,muZSD,carrierFrequencyInHz5GChannel/1e9);
  subpathOffsets = rmsClusterAngleSpread*subpathOffsets;
  
  return subpathOffsets;
}

double SCM5GLink::get5GRMSClusterAngleSpread(int isAzimuth_Zenith,int scmGridIndx,double muZSD,double fc)
{
    double AS;
    #ifdef _USING_CALIBRATION_
    if(isAzimuth_Zenith<3)
    {
        if(scmGridIndx == 9 || scmGridIndx == 10)
        {
            double lfc = log10(1+fc);
            if(isAzimuth_Zenith==0)  //c_ASD
            {
                AS = (scmGridIndx==9) ? 7 : 3;
            }
            else if(isAzimuth_Zenith == 1)  //c_ASA
            {
                AS = (scmGridIndx==9) ? -6.2*lfc + 16.72 : -13*lfc+30.53;
            }
            else    //c_ZSA
            {
                AS = (scmGridIndx==9) ? -3.85*lfc + 10.28 : -3.72*lfc+10.25;
            }
        }
        else
        {
            mat c_AS = "3 10 5 "/*UMi*/"5 2 5 "/*UMa*/"2 2 2 "/*RMa*/"5 5 "/*InH*/"5 5;"/*InF*/    //c_ASD
                       "17 22 20 "/*UMi*/"11 15 20 "/*UMa*/"3 3 3 "/*RMa*/"8 11 "/*InH*/"8 8;"/*InF*/   //c_ASA
                       "7 7 6 "/*UMi*/"7 7 6 "/*UMa*/"3 3 3 "/*RMa*/"9 9 "/*InH*/"9 9"/*InF*/;  //c_ZSA
            AS = c_AS(isAzimuth_Zenith,scmGridIndx);
        }
    }
    else 
    {
        AS = (3.0/8.0)*pow(10.0,muZSD); 
    }
    #else
    //0=>AoD 1=>AoA 2=>ZoA 3=>ZoD
    if(isAzimuth_Zenith>=0 && isAzimuth_Zenith<3)
    {
        mat c_AS = "3 10 5 "/*UMi*/"5 2 5 "/*UMa*/"2 2 2 "/*RMa*/"5 5 "/*InH*/"5 5;"/*InF*/    //c_ASD
                   "17 22 8 "/*UMi*/"11 15 8 "/*UMa*/"3 3 3 "/*RMa*/"8 11 "/*InH*/"8 8;"/*InF*/   //c_ASA
                   "7 7 3 "/*UMi*/"7 7 3 "/*UMa*/"3 3 3 "/*RMa*/"9 9 "/*InH*/"9 9"/*InF*/;  //c_ZSA
        AS = c_AS(isAzimuth_Zenith,scmGridIndx);
    }
    else if (isAzimuth_Zenith == 3 ) // ZOD
    {
        AS = (3.0/8.0)*pow(10.0,muZSD); // added to have continuity in the code.
    }
    else
    {
        cout<<"UnKnown isAzimuth_Zenith in getCluster5GRMSAzimuthSpread..."<<endl;exit(0);
    }
    #endif
    return AS;
}

double get5GRMSClusterDelaySpread(double fc,SCMPropagationScenario propagationScenario,ChannelModelType channelModelType,SCMLinkCondition scmLinkCondition)
{
    double c_DS=-1;
    if((fc>6 && channelModelType == _MODEL_A_) || channelModelType == _MODEL_B_)
    {
        if(propagationScenario==_SCM5G_URBAN_MICRO_) /*UMi*/
        {
            c_DS = (scmLinkCondition==_LOS_) ? 5.0 : 11.0;
        }
        else if(propagationScenario==_SCM5G_URBAN_MACRO_) /*UMa*/
        {
            if(fc<6) fc=6;
            c_DS = (scmLinkCondition==_O2I_) ? 11.0 : max(0.25,-3.4084*log10(fc)+6.5622);
        }
    }
    if(c_DS == -1)
    {
        c_DS = 3.91;
    }
    return c_DS;
}

int SCM5GLink::getNumberOfTaps()
{
    return nTaps5GChannel;
}


cvec SCM5GLink::generate5GChannelTapGainsTxEtoRxE(int currentTxAntennaElement,int currentRxAntennaElement, double currentTimeInSeconds,bool isChannelGenForRSRP,int txOrientationId , int rxOrientationId)
{
    if(!isReadyFor5GChannel)
    {
        cout<<"[both:] 5GChannelLink not initialized ... Please call initializeFadingParams() before calling generateTapGains()..."<<endl;
        abort();
    }
    
    cvec tapGain = zeros_c(nTaps5GChannel);
    
    for(int i=0;i<nTaps5GChannel;i++)
        tapGain(i) = generate5GChannelTapGainPerClusterTxEtoRxE(i,currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds,isChannelGenForRSRP,txOrientationId,rxOrientationId);
   
    if(scmLinkCondition == _LOS_)
    {
        dComplex  losTapGain = generate5GChannelTapGainForLOSClusterTxEtoRxE(0,currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds,isChannelGenForRSRP,txOrientationId,rxOrientationId);  
        tapGain(0) += sqrt(K_rice_5G)*losTapGain;
        
        if(useGroundReflection && !isFlatFadedChannel)
        {
            tapGain(groundReflectedTap) = sqrt(K_rice_5G)*computeChannelTapGainForGroundReflection(currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds,isChannelGenForRSRP,txOrientationId,rxOrientationId);
        }
        
        tapGain = sqrt(1.0/(K_rice_5G+1.0))*tapGain;
    }
    
    if(isnan(sum(abs(tapGain))))
    {
        cout<<"K_rice_5G: "<<K_rice_5G<<"\ntapPow: "<<tapPow5GChannel<<"\ntapDelays: "<<tapDelays5GChannel<<endl;
        cout<<"TapGain cannot be NaN..."<<endl;abort();
    }
    
    return tapGain;
}

dComplex SCM5GLink::generate5GChannelTapGainPerClusterTxEtoRxE(int tapCnt,int currentTxAntennaElement,int currentRxAntennaElement, double currentTimeInSeconds, bool isChannelGenForRSRP,int txOrientationId, int rxOrientationId)
{   
    if(useGroundReflection)
    {
        if(tapCnt==groundReflectedTap)
        {
            return dComplex(0,0);
        }
        else if(tapCnt>groundReflectedTap)
        {
            tapCnt--;
        }
    }
    cmat F_rx,F_tx,Rot;
    mat r_tx,r_rx;
    dComplex tapGain = dComplex(0,0);
    
    ivec idx = get5GSubPathIndicesofClusters(SubPathtypeinCluster5G(tapCnt));
    
    int M = idx.length();
    int clusterIndx = aMap5G(tapCnt);
    vec txFieldAmplitude = subpath5GAoDZoDAntennaFieldAmplitude(txOrientationId)(tapCnt), rxFieldAmplitude = subpath5GAoAZoAAntennaFieldAmplitude(rxOrientationId)(tapCnt);
    
    vec subpathAOAs = subpath_Boresight_5GAoA_angle(rxOrientationId)(tapCnt), subpathZOAs = subpath_Boresight_5GZoA_angle(rxOrientationId)(tapCnt);
    vec subpathAODs = subpath_Boresight_5GAoD_angle(txOrientationId)(tapCnt), subpathZODs = subpath_Boresight_5GZoD_angle(txOrientationId)(tapCnt);    

    for(int pathCnt = 0; pathCnt<M; pathCnt++)
    {
        int rayIndx = idx(pathCnt);
        
        Rot = generateRotationMatrix(clusterIndx,rayIndx); // generates the 2nd Matrix of Eqn.
        r_rx = getCartesianVector(subpathAOAs(pathCnt),subpathZOAs(pathCnt)); 
        
        r_tx = getCartesianVector(subpathAODs(pathCnt),subpathZODs(pathCnt)); 
        
        dComplex pathGain = getPathGain(txFieldAmplitude(pathCnt),rxFieldAmplitude(pathCnt),Rot,r_tx,r_rx,currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds,isChannelGenForRSRP);
        
        tapGain += pathGain;
    }
    return sqrt(tapPow5GChannel(tapCnt)/M)*tapGain;
}

dComplex SCM5GLink::generate5GChannelTapGainForLOSClusterTxEtoRxE(int tapCnt, int currentTxAntennaElement, int currentRxAntennaElement, double currentTimeInSeconds, bool isChannelGenForRSRP,int txOrientationId , int rxOrientationId)
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
    
    r_rx = getCartesianVector(thetaRxLos5G(rxOrientationId),phiRxLos5G(rxOrientationId));
    
    r_tx = getCartesianVector(thetaTxLos5G(txOrientationId),phiTxLos5G(txOrientationId));
    
    losTapGain = getPathGain(txLos5GFieldAmplitude(txOrientationId),rxLos5GFieldAmplitude(rxOrientationId),Rot,r_tx,r_rx,currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds,isChannelGenForRSRP,1);
    
    return losTapGain;
}

dComplex SCM5GLink::computeChannelTapGainForGroundReflection(int currentTxAntennaElement, int currentRxAntennaElement, double currentTimeInSeconds, bool isChannelGenForRSRP,int txOrientationId , int rxOrientationId)
{
    double d2D = findDistance(linkStartLoc,linkEndLoc);
    double dGR = sqrt(sqr(d2D) + sqr(txHeight+rxHeight));
    double deg2rad = pi/180.0;
    
    mat K = "5.31	0	0.0326	0.8095;"
            "3.75	0	0.038	0;"
            "2.94	0	0.0116	0.7076;"
            "1.99	0	0.0047	1.0718;"
            "3.66	0	0.0044	1.3515;"
            "1	    0	107	       0;"
            "3	    0	0.00015	2.52;"
            "15	  −0.1	0.035	1.63;"
            "30	  −0.4	0.15	1.3";
    
    int materialIndex = 6;
    double a = K(materialIndex,0),b = K(materialIndex,1),c = K(materialIndex,2), d = K(materialIndex,3);
    double fc = carrierFrequencyInHz5GChannel*1e-9;
    
    double epsilon_r = a*pow(fc,b), sigma = c*pow(fc,d);
    double epsilon0 = 8.854187817e-12;
    dComplex epsilon = dComplex(epsilon_r,-sigma/(2*pi*carrierFrequencyInHz5GChannel*epsilon0));
    
    double ZOD = phiTxGround5G(txOrientationId),ZOA = phiRxGround5G(rxOrientationId);
    double AOD = thetaTxGround5G(txOrientationId),AOA = thetaRxGround5G(rxOrientationId);
    double sinZOD = sin(deg2rad*ZOD), cosZOD = cos(deg2rad*ZOD);
    
    dComplex R1 = (epsilon*cosZOD + sqrt(epsilon-sqr(sinZOD))) / (epsilon*cosZOD - sqrt(epsilon-sqr(sinZOD))); 
    dComplex R2 = (cosZOD + sqrt(epsilon-sqr(sinZOD))) / (cosZOD - sqrt(epsilon-sqr(sinZOD))); 
    
    cmat Rot;
    mat r_tx,r_rx;
    dComplex groundTapGain = dComplex(0,0);
    
    r_tx.set_size(3,1);
    r_rx.set_size(3,1);
    
    //Refer Eqn 7.5-29 in 3GPP TR 38.901
    
    Rot = zeros_c(2,2);
    Rot(0,0) = R1;
    Rot(1,1) = -R2;
    
    r_rx = getCartesianVector(AOA,ZOA);
    r_tx = getCartesianVector(AOD,ZOD);

    groundTapGain = (d3D/dGR)*getPathGain(txGround5GFieldAmplitude(txOrientationId),rxGround5GFieldAmplitude(rxOrientationId),Rot,r_tx,r_rx,currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds,isChannelGenForRSRP,0);
    
    return groundTapGain;
}


double SCM5GLink::getDopplerSpreadInHz()
{
    vec relVelocity = rxVelocityVector - txVelocityVector;
    double relSpeed = sqrt(relVelocity*relVelocity);
    return relSpeed*carrierFrequencyInHz5GChannel/lightSpeed;
}

double SCM5GLink::getDopplerPhase(mat r_tx,mat r_rx,double currentTimeInSeconds,bool isLOS)
{
    //NOTE: Its supports both dual mobility and single mobility.
    double lambda = lightSpeed/carrierFrequencyInHz5GChannel,K = 2*pi/lambda;
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
    return c3;
}

dComplex SCM5GLink::getPathGain(double txFieldAmplitude,double rxFieldAmplitude,cmat& Rot,mat& r_tx,mat& r_rx,int currentTxAntennaElement,int currentRxAntennaElement,double currentTimeInSeconds,bool isChannelGenForRSRP,bool isForLOSPath)
{
    double lamda=lightSpeed/carrierFrequencyInHz5GChannel;
    mat txLocationVector = txLocationVectorPerAntennaElement(currentTxAntennaElement),rxLocationVector = rxLocationVectorPerAntennaElement(currentRxAntennaElement);
    double txPolAngle = txPolarizationAngleInDegreePerAntennaElement(currentTxAntennaElement),rxPolAngle = rxPolarizationAngleInDegreePerAntennaElement(currentRxAntennaElement);
    double K = 2*pi/lamda,deg2rad = pi/180; 
    double c1,c2,c3=1;
    cmat F_tx(2,1),F_rx(2,1);
    
    //Refer to Eqn 7.5-28 in 3GPP TR 38.901
    
    F_tx(0,0) = txFieldAmplitude*cos(deg2rad*txPolAngle);
    F_tx(1,0) = txFieldAmplitude*sin(deg2rad*txPolAngle);
    
    F_rx(0,0) = rxFieldAmplitude*cos(deg2rad*rxPolAngle);
    F_rx(1,0) = rxFieldAmplitude*sin(deg2rad*rxPolAngle);
    
    c1 = K*(r_rx.T()*rxLocationVector)(0,0);
    c2 = K*(r_tx.T()*txLocationVector)(0,0);
    
    dComplex pathGain = (F_rx.T()*Rot*F_tx)(0,0)*expj(c1)*expj(c2);
    if(!isChannelGenForRSRP)
    {
        c3 = getDopplerPhase(r_tx,r_rx,currentTimeInSeconds,isForLOSPath);
        pathGain *= expj(c3);
    }
    if(isForLOSPath)
    {
        #ifdef _USING_CALIBRATION_
        pathGain *= expj(deg2rad*losPhase5G);
        #else 
        pathGain *= expj(-1*K*d3D);
        #endif
    }
    if(isnan(abs(pathGain)))
    {
        cout<<"c1:"<<c1<<" c2:"<<c2<<" c3:"<<c3<<" gain:"<<(F_rx.T()*Rot*F_tx)(0,0)<<endl;
        abort();
    }
    return pathGain;
}

cmat SCM5GLink::generateRotationMatrix(int clusterIndx, int rayIndx)
{
  // Refer to 2nd Matrix of Eqn 7.5-28 in TR 38.901.
    double deg2rad = pi/180;
    
    cmat Rot;
    Rot.set_size(2,2);
    for(int i=0,p=0;i<2;i++)
        for(int j=0;j<2;j++,p++)
        {
            double phi =  deg2rad*subpathPhase_5Gfixedpt(clusterIndx)(4*rayIndx+p);
            Rot(i,j) = expj(phi);
        }
    
    double Xpr = XPR_lin_5G(clusterIndx,rayIndx);
    
    Rot(0,1) *= sqrt(1/Xpr);
    Rot(1,0) *= sqrt(1/Xpr);
    return Rot;
}

string SCM5GLink::getClusterSpreads()
{
    vec clusterPow = get5GChannelClusterPowers();
    vec clusterDelays = get5GChannelTapDelays();
    double muAOD,muZOD,muAOA,muZOA,muDS;
    double sigmaDS = getRMSSpread(clusterDelays,get5GChannelTapPowers(),muDS);
    double sigmaAOD = getRMSSpread(pathAoD5GChannel,clusterPow,muAOD);
    double sigmaZOD = getRMSSpread(pathZoD5GChannel,clusterPow,muZOD);
    double sigmaAOA = getRMSSpread(pathAoA5GChannel,clusterPow,muAOA);
    double sigmaZOA = getRMSSpread(pathZoA5GChannel,clusterPow,muZOA);
    stringstream ss;
    ss<<muDS*1e9<<",\t"<<sigmaDS*1e9<<",\t"<<muAOD<<",\t"<<sigmaAOD<<",\t"<<muZOD<<",\t"<<sigmaZOD<<",\t"<<muAOA<<",\t"<<sigmaAOA<<",\t"<<muZOA<<",\t"<<sigmaZOA;
    return ss.str();
}   
    
string SCM5GLink::getAngleSpreads()
{
    vec tapPow = get5GChannelTapPowers();
    double mu1,mu2,mu3,mu4,s1,s2,s3,s4;
    if(propagationScenario5GChannel==_SCM5G_INDOOR_FACTORY_)
    {
        s1 = getAngleSpreadForInF(tapPow,subpathAOD,mu1);
        s2 = getAngleSpreadForInF(tapPow,subpathZOD,mu2);
        s3 = getAngleSpreadForInF(tapPow,subpathAOA,mu3);
        s4 = getAngleSpreadForInF(tapPow,subpathZOA,mu4);
    }
    else
    {
        s1 = getAngleSpread(tapPow,subpathAOD,mu1);
        s2 = getAngleSpread(tapPow,subpathZOD,mu2);
        s3 = getAngleSpread(tapPow,subpathAOA,mu3);
        s4 = getAngleSpread(tapPow,subpathZOA,mu4);
    }
    stringstream ss;
    ss<<mu1<<",\t"<<s1<<",\t"<<mu2<<",\t"<<s2<<",\t"<<mu3<<",\t"<<s3<<",\t"<<mu4<<",\t"<<s4;
    
    return ss.str();
}

vec SCM5GLink::get5GChannelTapDelays()
{
  if(!isReadyFor5GChannel)
  {
    cout<<"Please initialize 5G-Channel Link before calling getTapDelays()..."<<endl;
    return(vec(""));
  }
  return(tapDelays5GChannel);
}

vec SCM5GLink::get5GChannelClusterPowers()
{
    vec tapPow = get5GChannelTapPowers();
    vec clusterPowers;
    if(aMap5G.length())
    {
        clusterPowers = zeros(max(aMap5G)+1);
        for(int tap_cnt=0;tap_cnt<tapPow.length();tap_cnt++)
        {
            clusterPowers(aMap5G(tap_cnt)) += tapPow(tap_cnt);
        }
    }
    else
        clusterPowers = tapPow;
    return clusterPowers;
}

vec SCM5GLink::get5GChannelTapPowers()
{
    if(!tapPow5GChannel.length())
    {
        cout<<"Please initialize 5G-Channel Link before calling get5GChannelTapPowers()..."<<endl;
        abort();
    }
    vec tapPow = tapPow5GChannel;
    if(scmLinkCondition == _LOS_)
    {
        tapPow /= (K_rice_5G+1);
        tapPow(0) *= K_rice_5G;
    }
    return(tapPow);
}

vec SCM5GLink::get5GChannelClusterDelays()
{
    vec clusterDelays;
    if(aMap5G.length())
    {
        clusterDelays.set_length(max(aMap5G)+1);
        for(int tap_cnt=0;tap_cnt<tapDelays5GChannel.length();tap_cnt++)
        {
            if(SubPathtypeinCluster5G(tap_cnt)<=1) // 0 means nonSubCluster and 1 means 1st SubCluster
            {
                clusterDelays(aMap5G(tap_cnt)) = tapDelays5GChannel(tap_cnt);
            }
        }
    }
    else
        clusterDelays = tapDelays5GChannel;
    
    return clusterDelays;
}

double SCM5GLink::getRMSDelaySpread(double& mu)
{
    vec tapPow = get5GChannelTapPowers();
    return getRMSSpread(tapDelays5GChannel,tapPow,mu);
}

void SCM5GLink::computeOxygenLoss(vec tapDelays)
{
    oxygenLossPerClusterIndB = zeros(tapDelays.length());
    double carrFreqInGHz = carrierFrequencyInHz5GChannel/1e9;
    if(carrFreqInGHz<=52 || carrFreqInGHz>=68){return;}
    
    int lfc = floor_i(carrFreqInGHz),hfc = ceil_i(carrFreqInGHz);
    vec alphaPerCarrier = "0.0 1.0 2.2 4.0 6.6 9.7 12.6 14.6 15.0 14.6 14.3 10.5 6.8 3.9 1.9 1.0 0.0";
    
    double alpha;
    if(lfc==hfc)
    {
        alpha = alphaPerCarrier(lfc-52);
    }
    else 
    {
        double l_alpha = alphaPerCarrier(lfc-52),h_alpha = alphaPerCarrier(hfc-52);
        double slope = (h_alpha - l_alpha)/(hfc-lfc);
        alpha = l_alpha + slope*(carrFreqInGHz-lfc); //Linear interpolation
    }
    double deltaTau = (scmLinkCondition==_LOS_) ? 0.0 : minTau;
    oxygenLossPerClusterIndB = (alpha/1000)*(d3D + lightSpeed*(deltaTau+tapDelays));
}

string SCM5GLink::getLOSAnglesAndLinkLocations()
{
    stringstream ss;
    ss<<theta_Tx_5G<<",\t"<<phi_Tx_5G<<",\t"<<theta_Rx_5G<<",\t"<<phi_Rx_5G<<",\t"<<linkStartLoc<<",\t"<<linkEndLoc;
    return ss.str();
}

void SCM5GLink::set(SCM5GLink* link)
{
    carrierFrequencyInHz5GChannel = link->carrierFrequencyInHz5GChannel;
//     dopplerSpreadInHz5GChannel = link->dopplerSpreadInHz5GChannel;
    linkStartLoc = link->linkStartLoc,linkEndLoc = link->linkEndLoc,d3D = link->d3D;
    txHeight = link->txHeight,rxHeight = link->rxHeight;
    nRaysPerCluster = link->nRaysPerCluster,scmLinkCondition = link->scmLinkCondition,scmGridIndx = link->scmGridIndx;
    txAntStructure = link->txAntStructure,rxAntStructure = link->rxAntStructure;
    tapPow5GChannel = link->tapPow5GChannel,tapDelays5GChannel = link->tapDelays5GChannel,nTaps5GChannel = link->nTaps5GChannel,minTau = link->minTau;
    theta_Tx_5G = link->theta_Tx_5G,phi_Tx_5G = link->phi_Tx_5G,theta_Rx_5G = link->theta_Rx_5G,phi_Rx_5G = link->phi_Rx_5G;
    pathAoD5GChannel = link->pathAoD5GChannel,pathZoD5GChannel = link->pathZoD5GChannel,pathAoA5GChannel = link->pathAoA5GChannel,pathZoA5GChannel = link->pathZoA5GChannel;
    subpathAOD = link->subpathAOD,subpathZOD = link->subpathZOD,subpathAOA = link->subpathAOA,subpathZOA = link->subpathZOA;
    txLocationVectorPerAntennaElement = link->txLocationVectorPerAntennaElement,rxLocationVectorPerAntennaElement = link->rxLocationVectorPerAntennaElement;
    txPolarizationAngleInDegreePerAntennaElement = link->txPolarizationAngleInDegreePerAntennaElement,rxPolarizationAngleInDegreePerAntennaElement = link->rxPolarizationAngleInDegreePerAntennaElement;
    XPR_lin_5G = link->XPR_lin_5G,K_rice_5G = link->K_rice_5G;
    aMap5G = link->aMap5G,SubPathtypeinCluster5G = link->SubPathtypeinCluster5G;
    oxygenLossPerClusterIndB = link->oxygenLossPerClusterIndB;
    isFlatFadedChannel = link->isFlatFadedChannel,addOxygenLoss = link->addOxygenLoss;
    useGroundReflection = link->useGroundReflection, groundReflectedTap = link->groundReflectedTap;
    generate5GSubpathPhases();
    isReadyFor5GChannel = true;
}

void SCM5GLink::reInitChannelForFDDReciprocity(double carrierFrequency)
{
    
    carrierFrequencyInHz5GChannel = carrierFrequency;
    generate5GSubpathPhases(); // Step 10
    calculateLocationVectorsAndPolAngles(&txAntenna,carrierFrequency,txLocationVectorPerAntennaElement,txPolarizationAngleInDegreePerAntennaElement);
    calculateLocationVectorsAndPolAngles(&rxAntenna,carrierFrequency,rxLocationVectorPerAntennaElement,rxPolarizationAngleInDegreePerAntennaElement);
    compute5GAntennaGains();
}

double getAngleSpread(vec tapPow,Array<vec> angle,double& mu)
{
    int nTaps = tapPow.length();
    if(angle.length()!=nTaps)
    {
        cout<<angle.length()<<" "<<nTaps<<endl;
        cout<<"Lengths mismatch in getAngleSpread().."<<endl;abort();
    }
    double AS = 0.0,totalTapPow = sum(tapPow);
    mu = 0.0;
    for(int tap_cnt=0;tap_cnt<nTaps;tap_cnt++)
    {
        mu += mean(angle(tap_cnt))*tapPow(tap_cnt);
    }
    mu = mu/totalTapPow;
    
    for(int tap_cnt=0;tap_cnt<nTaps;tap_cnt++)
    {
        int M = angle(tap_cnt).length();
        for(int ray_cnt=0;ray_cnt<M;ray_cnt++)
        {
            double theta = circmod(angle(tap_cnt)(ray_cnt)-mu+180.0,360.0)-180.0; //Refer Eqn A-2 in TR 25.996.
            AS += sqr(theta)*tapPow(tap_cnt)/M; //Refer Eqn A-1 in TR 25.996.
        }
    }
    AS = sqrt(AS/totalTapPow);
    return AS;
}

double getAngleSpreadForInF(vec tapPow,Array<vec>& angle,double& mu)
{
    int nTaps = tapPow.length();
    if(angle.length()!=nTaps)
    {
        cout<<"Lengths mismatch..."<<endl;abort();
    }
    //Refer to Eqn A-1 in TR 38.901
    double deg2rad = pi/180.0;
    dComplex temp(0,0);
    for(int tap_cnt=0;tap_cnt<nTaps;tap_cnt++)
    {
        for(int ray_cnt=0;ray_cnt<angle(tap_cnt).length();ray_cnt++)
        {
            temp += expj(angle(tap_cnt)(ray_cnt)*deg2rad)*(tapPow(tap_cnt)/angle(tap_cnt).length());//Refer Eqn A-1 and A-2 in TR 38.901
        }  
    }
    mu = arg(temp/sum(tapPow))/deg2rad;
    double sigma = sqrt(-2*log(abs(temp/sum(tapPow))));
    return sigma/deg2rad;
}

double getRMSSpread(vec param,vec tapPow,double &mu)
{
    if(param.length()!=tapPow.length())
    {
        cout<<"Lengths mismatch in getRMSSpread()..."<<endl;abort();
    }
    double totalPow = sum(tapPow);
    mu = sum(elem_mult(tapPow,param))/totalPow;
    vec s2 = sqr(param-mu);
    double sigma = sqrt(sum(elem_mult(s2,tapPow))/totalPow);
    //cout<<"RMS_spread_sigma"<<sigma<<endl;
    return sigma;
}

mat getPanelCentre(ivec antStructure,vec antSpacing)
{
    //with reference to bottom element of first column.
    mat centre = zeros(3,1);
    centre(1,0) = (antStructure(3)-1)*antSpacing(3)/2; // Y 
    centre(2,0) = (antStructure(2)-1)*antSpacing(2)/2; // Z
    return centre;
}

mat getStructureCentre(ivec antStructure,vec antSpacing)
{
    //with reference to bottom element of first column.
    mat centre = zeros(3,1);
    centre(1,0) = ((antStructure(3)-1)*antSpacing(3)+(antStructure(1)-1)*antSpacing(1))/2; // Y 
    centre(2,0) = ((antStructure(2)-1)*antSpacing(2)+(antStructure(0)-1)*antSpacing(0))/2; // Z
    return centre;
}


Array<vec> getHorizontalElementLocationsFor3Sectors(int sectorIndx, int nHorizontalElements, double horizontalSpacing)
{
    Array<vec> pts(nHorizontalElements);
    
    if(nHorizontalElements>1)
    {
        double a = (nHorizontalElements-1)*horizontalSpacing;
        double R = a / sqrt(3.0);
        
        // vertices (fixed orientation)
        mat V(3,2);
        
        V(0,0) =  R;        V(0,1) = 0;
        V(1,0) = -R/2.0;    V(1,1) =  sqrt(3.0)*R/2.0;
        V(2,0) = -R/2.0;    V(2,1) = -sqrt(3.0)*R/2.0;
        
        int v1 = sectorIndx;
        int v2 = (sectorIndx+1)%3;
        
        vec start = V.get_row(v1);
        vec end   = V.get_row(v2);
        
        for(int i=0;i<nHorizontalElements;i++)
        {
            double t = (double)i/(nHorizontalElements-1);
            pts(i).set_length(2);
            pts(i)(0) = start(0) + t*(end(0)-start(0));
            pts(i)(1) = start(1) + t*(end(1)-start(1));
        }
    }
    else
        pts(0)=zeros(2);
    
    return pts;
}


void calculateLocationVectorsAndPolAngles(Antenna* antennaPtr,double carrierFrequencyInHz5GChannel,Array<mat>& locationVectorPerAntennaElement, vec& polarizationAngleInDegreePerAntennaElement)
{
    /* the antenna elements are indexed in the following form
     * Example given for (2,2,2,2,2) 32 elements
     * 
     * -----panel 1-----      -----panel 3-----
     * | (10,11) (14,15) |    | (26,27) (30,31) | 
     * | (8,9)   (12,13) |    | (24,25) (28,29) |
     *  ----------------       -----------------
     * 
     * -----panel 0-----      ----panel 2------
     * | (2,3)     (6,7) |    | (18,19) (22,23) |
     * | (0,1)     (4,5) |    | (16,17) (20,21) |
     * -----------------     -----------------
     */
    
    vec crossPolarizationAngles = antennaPtr->getCrossPolarizationAngle();
    
    sTXRUParameters txruParameters = antennaPtr->getTXRUParameters(carrierFrequencyInHz5GChannel);
    
    locationVectorPerAntennaElement.set_length(antennaPtr->antElementCount);
    polarizationAngleInDegreePerAntennaElement.set_length(antennaPtr->antElementCount);
    
    bool isCrossPolarizationEnabled = antennaPtr->isAntennaCrossPolarized();
    
    if(txruParameters.Ng!=1 || txruParameters.Mg!=1)
    {
        cout<<"Multipanel location vectors are not yet defined."<<endl;abort();
    }
    
    if(antennaPtr->isForHexNode)
    {
        int sectorIndx = floor_i(antennaPtr->orientationInfo.orientationPerPanel(0).alpha/120.0);
        Array<vec> horizontalLocations=getHorizontalElementLocationsFor3Sectors(sectorIndx, txruParameters.N, txruParameters.dh);
//         cout<<"Computing location vector for sector : "<<sectorIndx<<" as "<<horizontalLocations<<endl;
        for(int col_cnt = 0, elementCnt = 0; col_cnt < txruParameters.N; col_cnt++)
            for(int row_cnt = 0; row_cnt < txruParameters.M; row_cnt++)
                for(int pol_cnt = 0; pol_cnt < txruParameters.P; pol_cnt++, elementCnt++)
                {
                    locationVectorPerAntennaElement(elementCnt) = zeros(3,1);                //finding local coordinates for each element
                    locationVectorPerAntennaElement(elementCnt)(0,0) = horizontalLocations(col_cnt)(0);
                    locationVectorPerAntennaElement(elementCnt)(1,0) = horizontalLocations(col_cnt)(1);
                    locationVectorPerAntennaElement(elementCnt)(2,0) = row_cnt*txruParameters.dv; 
                    
                    polarizationAngleInDegreePerAntennaElement(elementCnt) = (isCrossPolarizationEnabled) ? crossPolarizationAngles(pol_cnt) : antennaPtr->coPolarisationSlantAngle;
                }
    }
    else
    {
    
        for(int panel_col_cnt = 0, elementCnt = 0; panel_col_cnt < txruParameters.Ng; panel_col_cnt++)
            for(int panel_row_cnt = 0; panel_row_cnt < txruParameters.Mg; panel_row_cnt++)
            {
                for(int col_cnt = 0; col_cnt < txruParameters.N; col_cnt++)
                    for(int row_cnt = 0; row_cnt < txruParameters.M; row_cnt++)
                        for(int pol_cnt = 0; pol_cnt < txruParameters.P; pol_cnt++, elementCnt++)
                        {
                            locationVectorPerAntennaElement(elementCnt) = zeros(3,1);                //finding local coordinates for each element
                            locationVectorPerAntennaElement(elementCnt)(1,0) = col_cnt*txruParameters.dh + panel_col_cnt*txruParameters.dph;
                            locationVectorPerAntennaElement(elementCnt)(2,0) = row_cnt*txruParameters.dv + panel_row_cnt*txruParameters.dpv; 
                            
                            polarizationAngleInDegreePerAntennaElement(elementCnt) = (isCrossPolarizationEnabled) ? crossPolarizationAngles(pol_cnt) : antennaPtr->coPolarisationSlantAngle;
                        }
            }
    }


    
}


