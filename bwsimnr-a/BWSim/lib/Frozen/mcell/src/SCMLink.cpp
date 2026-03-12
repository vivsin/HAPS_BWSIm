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

SCMLink::SCMLink()
{
    #ifdef USING_SECURITY 
    BWSIM_Password_function();
    #endif
    spConsist=NULL;
    scm5GLink=NULL;/*scm2DLink=NULL;scm3DLink=NULL;*/ntnSCMLink=NULL;scmType = _SCMTYPE_UNINIT_;
    fixedTapGainsForInitialBeam.set_length(0);
//     txElementGainInDB=0.0,rxElementGainInDB=0.0;
    propagationScenario = _SCM_UNINIT_;
    addPathSpecificAntennaGain = false;
    enableFlatFading=false;
    addOxygenLoss=false;
    applyFaradayRotation = false;
    useGroundReflection = false;
}
double SCMLink::initializeFadingParams(int  isOutdoortoIndoor,bool isLineOfSight,double dopplerSpread,double carrierFrequency,SCMPropagationScenario propScenario,SCMLargeScaleParameters_S &scmParameters,Location_S nodeLocationTx, Location_S nodeLocationRx,Antenna *txAntenna, Antenna *rxAntenna ,double txAntSpacing,double rxAntSpacing, ChannelModelType tChannelModelType){
  
  double maxDelay = 0.0;
  //isLOS = isLineOfSight; // santhosh
  propagationScenario = propScenario;
  txLoc = nodeLocationTx, rxLoc = nodeLocationRx;
  scmType = getSCMType(propagationScenario);
  switch(scmType)
    {
    case _SCMTYPE_5G_:
    {
        scm5GLink = new SCM5GLink();  
        scm5GLink->channelModelType = tChannelModelType;
        scm5GLink->isFlatFadedChannel = enableFlatFading;
        scm5GLink->addOxygenLoss = addOxygenLoss;
        scm5GLink->useGroundReflection = useGroundReflection;
        scm5GLink->spConsistPtr = spConsist;
        maxDelay = scm5GLink->initialize5GChannelFadingParams(isOutdoortoIndoor,isLineOfSight,dopplerSpread,carrierFrequency,propScenario,scmParameters,nodeLocationTx,nodeLocationRx,txAntenna,rxAntenna);
        break;
}
    case _SCMTYPE_NTN_:
    {
        ntnSCMLink = new NTNSCMLink();  
        ntnSCMLink->isFlatFadedChannel = enableFlatFading;
        ntnSCMLink->addPathSpecificAntennaGain = addPathSpecificAntennaGain;
        ntnSCMLink->applyFaradayRotation = applyFaradayRotation;
        maxDelay = ntnSCMLink->initialize5GChannelFadingParams(isOutdoortoIndoor,isLineOfSight,dopplerSpread,carrierFrequency,propScenario,scmParameters,nodeLocationTx,nodeLocationRx,txAntenna,rxAntenna);
        break;
    }
    default:
        cout<<"[both:]Error...Unknown propScenario. Aborting ...."<<endl;abort();
        break;  

    }
    return maxDelay;
}

bool SCMLink :: isSCMChannelInitialized()
{
    switch(scmType)
    {
        case _SCMTYPE_5G_:
            return scm5GLink->is5GChannelInitialized();
            break;
        case _SCMTYPE_NTN_:            
            return ntnSCMLink->isInitialized();
            break;
        default:
            return false;
    }
    return false;
}

void SCMLink :: acquireSCMGridsForSpConsist(Array< Array<imat> > GridsForSpConsist, mat dcorrMat, Area_S coverageAreaLoc)
{
//   cout<<"GridsForSpConsist:"<<GridsForSpConsist;/   cout<<"dcorrMat"<<dcorrMat<<endl;
//   cout<<"coverageAreaLoc"<<coverageAreaLoc.xLen<<endl<<coverageAreaLoc.xLen<<endl;
    spConsist = new SCMSpatialConsistency();
    spConsist->scmGridsForSpConsist = GridsForSpConsist;
    spConsist->dcorrForSPConsistency = dcorrMat;
    spConsist->coverageArea  = coverageAreaLoc;
//   cout<<"scmGridsForSpConsist:"<<scmGridsForSpConsist;
//   cout<<"dcorrForSPConsistency"<<dcorrForSPConsistency<<endl;
//   cout<<"coverageArea"<<coverageArea.xLen<<endl<<coverageArea.xLen<<endl;
}

cvec SCMLink::generateTapGains(int currentTxAntenna, int currentRxAntenna,double currentTimeInSeconds)
{
  cvec tapWeights;
  
    switch(scmType)
    {
        case _SCMTYPE_5G_:
        case _SCMTYPE_NTN_:
            cout<<"[both:] 5G SCM channel generation should have called in the previous function "<<endl;abort();
            break;
        default:
            cout<<"SCM channel not initialised..."<<endl;abort();
            break;
    }
    return tapWeights;
}

vec SCMLink::getTapDelays(){ 
  
    vec tapDelays;
    switch(scmType)
    {
        case _SCMTYPE_5G_:
            tapDelays = scm5GLink->get5GChannelTapDelays(); 
            break;
        case _SCMTYPE_NTN_:
            tapDelays = ntnSCMLink->getTapDelays();            
            break;
        default:
            cout<<"SCM channel not initialised..."<<endl;abort();
            break;
    }
    return(tapDelays);
}

double SCMLink::getLOSAntennaGainInDB(int txOrientationID,int rxOrientationID){
  
  double LOSGain=-1;
  
    switch(scmType)
    {
        case _SCMTYPE_5G_:
            LOSGain = dB(sqr(scm5GLink->txLos5GFieldAmplitude(txOrientationID)*scm5GLink->rxLos5GFieldAmplitude(rxOrientationID))); 
            break;
        case _SCMTYPE_NTN_:
            LOSGain = dB(sqr(ntnSCMLink->txLosFieldAmplitude(txOrientationID)*ntnSCMLink->rxLosFieldAmplitude(rxOrientationID)));
            //abort();
            break;
        default:
            cout<<"SCM channel not initialised..."<<endl;abort();
            break;


    }
    //cout<<"LOSGAIN"<<LOSGain<<endl;
    return(LOSGain);
}

//NOTE: For SCM3D & SCM5G, this Function has to be modified correctly
double SCMLink::getAvgAntennaGainInDB(int txOrientationID,int rxOrientationID){
  
    switch(scmType)
    {
        case _SCMTYPE_5G_:
            return (dB(scm5GLink->txAvg5GAntGain(txOrientationID)*scm5GLink->rxAvg5GAntGain(rxOrientationID)));
            break;
        case _SCMTYPE_NTN_:
            {
                if(!getElevationAngle(txLoc,rxLoc)){return 0;}
                return (dB(ntnSCMLink->txAvgAntGain(txOrientationID)*ntnSCMLink->rxAvgAntGain(rxOrientationID)));
                //abort();
            }
            break;
        default:
            cout<<"SCM channel not initialised..."<<endl;abort();
            break;
    }
    return 0;
}

void SCMLink::printAntennaGains(ostream &os)
{
    
    switch(scmType)
    {
        /*case _SCMTYPE_2D_:
            os<<2*dB(scm2DLink->txLosAntGain)<<",\t "<<dB(scm2DLink->txAvgAntGain)<<",\t "<<2*dB(scm2DLink->rxLosAntGain)<<",\t "<<dB(scm2DLink->rxAvgAntGain);
            break;
        case _SCMTYPE_3D_:
            os<<2*dB(scm3DLink->txLos3DAntGain)<<",\t "<<dB(scm3DLink->txAvg3DAntGain)<<",\t "<<2*dB(scm3DLink->rxLos3DAntGain)<<",\t "<<dB(scm3DLink->rxAvg3DAntGain);
            break;
        */case _SCMTYPE_5G_:
            os<<2*dB(scm5GLink->txLos5GFieldAmplitude)<<",\t "<<dB(scm5GLink->txAvg5GAntGain)<<",\t "<<2*dB(scm5GLink->rxLos5GFieldAmplitude)<<",\t "<<dB(scm5GLink->rxAvg5GAntGain);
            break;
        case _SCMTYPE_NTN_:
            os<<2*dB(ntnSCMLink->txLosFieldAmplitude)<<",\t "<<dB(ntnSCMLink->txAvgAntGain)<<",\t "<<2*dB(ntnSCMLink->rxLosFieldAmplitude)<<",\t "<<dB(ntnSCMLink->rxAvgAntGain);
            break;
        default:
            cout<<"SCM channel not initialised..."<<endl;abort();
            break;
    }
}

cvec SCMLink::generate5GChannelTapGainsForInitialBeams(int currentTxAntennaElement,int currentRxAntennaElement, double currentTimeInSeconds,bool isChannelGenForRSRP,int txOrientationId , int rxOrientationId)
{
    switch(scmType)
    {
        case _SCMTYPE_5G_: 
            return scm5GLink->generate5GChannelTapGainsTxEtoRxE(currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds,isChannelGenForRSRP,txOrientationId,rxOrientationId);
            break;
        case _SCMTYPE_NTN_:
            //abort();
            return ntnSCMLink->generateTapGainsTxEtoRxE(currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds,isChannelGenForRSRP,txOrientationId,rxOrientationId);
            break;
        default:
            cout<<"Invalid propagationScenario in generate5GChannelTapGains()"<<endl;abort();
            break;
    }
}


void SCMLink::generate5GChannelTapGainsForInitialBeams (sTXRUParameters* txTXRUParameters,sTXRUParameters* rxTXRUParameters)
{
    if(scmType != _SCMTYPE_5G_ && scmType != _SCMTYPE_NTN_)
    {
        cout<<"5G SCM channel should be initialised before generating tap Gains..."<<endl;abort();
    }
    
    int rxNumberOfTxRUs = rxTXRUParameters->numberOfTXRU;
    int nRxPanels = rxTXRUParameters->nPanels;
    int nRxTxRUsPerPanel = rxNumberOfTxRUs/nRxPanels;
    int nTxOrients = txTXRUParameters->nOrientations;

    //cout<<"rxNumberOfTxRUs"<<rxNumberOfTxRUs<<endl;
    //cout<<"nRxPanels"<<nRxPanels<<endl;
    //cout<<"nRxTxRUsPerPanel "<<nRxTxRUsPerPanel <<endl;
    //cout<<"nTxOrients"<<nTxOrients<<endl;

    fixedTapGainsForInitialBeam.set_length(rxNumberOfTxRUs);
    for(int panel_cnt=0,rxTxRUCnt=0;panel_cnt<nRxPanels;panel_cnt++)
            for(int k=0;k<nRxTxRUsPerPanel;k++,rxTxRUCnt++)
            {
                fixedTapGainsForInitialBeam (rxTxRUCnt).set_length(nTxOrients);
                ivec physicalRxAntennaElemNums = rxTXRUParameters->getTXRUAntennaElements(rxTxRUCnt);
                int rxElementsPerTXRU = physicalRxAntennaElemNums.length();
                //cout<<"rxElementsPerTXRU :"<<rxElementsPerTXRU<<endl;
                for(int txOrient=0;txOrient<nTxOrients;txOrient++)
                {
                    fixedTapGainsForInitialBeam (rxTxRUCnt)(txOrient).set_length(rxElementsPerTXRU);
                    for(int j=0;j<rxElementsPerTXRU;j++)
                    {
                        int txTxRUID = find(txTXRUParameters->orientationIDperTXRU,txOrient); // Considering one TXRU per Orientation. 
                        ivec physicalTxAntennaElemNums = txTXRUParameters->getTXRUAntennaElements(txTxRUID);
                        int txElementsPerTXRU = physicalTxAntennaElemNums.length();
                        //cout<<"txElementsPerTXRU :"<<txElementsPerTXRU<<endl;
                        fixedTapGainsForInitialBeam (rxTxRUCnt)(txOrient)(j).set_length(txElementsPerTXRU);
                        
                        for(int i=0;i<txElementsPerTXRU;i++)
                        {
                            //cout << "generate5GChannelTapGainsForInitialBeams isLOS :" << tempLinkInfo.isLOS << endl; 
                            fixedTapGainsForInitialBeam(rxTxRUCnt)(txOrient)(j)(i) = generate5GChannelTapGainsForInitialBeams(physicalTxAntennaElemNums(i),physicalRxAntennaElemNums(j),0.0,true,txOrient,rxTXRUParameters->orientationIDPerPanel(panel_cnt));
                            //cout<<"physicalTxAntennaElemNums(i):"<<physicalTxAntennaElemNums(i)<<endl;
                            //cout<<"physicalRxAntennaElemNums(j): "<<physicalRxAntennaElemNums(j)<<endl;

                            //cout<<" fixedTapGainsForInitialBeam(rxTxRUCnt)(txOrient)(j)(i) :"<< fixedTapGainsForInitialBeam(rxTxRUCnt)(txOrient)(j)(i) <<","<<rxTxRUCnt<<","<<txOrient<<","<<j<<","<<i<<endl;
                        }
                    }
                }
            }
}

// void SCMLink::set(SCMLink* scmLink)
// {
//     propagationScenario = scmLink->propagationScenario;
//     addPathSpecificAntennaGain = scmLink->addPathSpecificAntennaGain;
//     enableFlatFading = scmLink->enableFlatFading;
//     useGroundReflection = scmLink->useGroundReflection;
//     txLoc = scmLink->txLoc,rxLoc = scmLink->rxLoc;
//     
//     /*if(is2DSCMScenario(propagationScenario))
//     {
//         scm2DLink = new SCM2DLink();
//         *scm2DLink = *scmLink->scm2DLink;
//     }
//     else if(is3DSCMScenario(propagationScenario))
//     {
//         scm3DLink = new SCM3DLink();
//         *scm3DLink = *scmLink->scm3DLink;
//     }
//     else */if(is5GSCMScenario(propagationScenario))
//     {
//         scm5GLink = new SCM5GLink();
//         *scm5GLink = *scmLink->scm5GLink;
// //         scm5GLink->set(scmLink->scm5GLink);
//     }
// }

void SCMLink::setDoppler(vec rxVelocityVector,vec txVelocityVector)
{
    double mobileVelocity = sqrt(rxVelocityVector*rxVelocityVector);
    double hPhase,vPhase;
    getAngles(rxVelocityVector/mobileVelocity,hPhase,vPhase,0);
    switch(scmType)
    {
        case _SCMTYPE_5G_:
            scm5GLink->mobileVelocity5GChannel = mobileVelocity;
            scm5GLink->dopplerSpreadInHz5GChannel = mobileVelocity*scm5GLink->carrierFrequencyInHz5GChannel/lightSpeed;
            scm5GLink->txVelocityVector = txVelocityVector,scm5GLink->rxVelocityVector = rxVelocityVector;
            break;
        case _SCMTYPE_NTN_:
            ntnSCMLink->txVelocityVector = txVelocityVector,ntnSCMLink->rxVelocityVector = rxVelocityVector;
            break;
        default:
            cout<<"SCM channel should be initialised before setting doppler..."<<endl;abort();
            break;
    }
}

void SCMLink::computeAntennaGains()
{
    switch(scmType)
    {
        case _SCMTYPE_5G_:
            //abort(); //remove abort santhosh
        scm5GLink->compute5GAntennaGains();
            break;
        case _SCMTYPE_NTN_:
            ntnSCMLink->computeAntennaGains();
            //abort();
            break;
        default:
            cout<<"SCM channel not initialised..."<<endl;abort();
            break;
    }
}

string SCMLink::getAngleAndDelaySpread()
{
    stringstream ss;
    double mu1,mu2,mu3,mu4,s1,s2,s3,s4,muDS,sigmaDS;
    
    switch(scmType)
    {
        /*case _SCMTYPE_2D_:
            sigmaDS = getRMSSpread(scm2DLink->tapDelays,scm2DLink->tapPow,muDS);
            s1 = getAngleSpread(scm2DLink->tapPow,scm2DLink->subpath_Boresight_AoD_angle,mu1);
            s2 = getAngleSpread(scm2DLink->tapPow,scm2DLink->subpath_Boresight_AoD_angle,mu2);
            ss<<muDS*1e9<<",\t"<<sigmaDS*1e9<<",\t"<<mu1<<",\t"<<s1<<",\t"<<mu2<<",\t"<<s2;
            break;
        case _SCMTYPE_3D_:
            sigmaDS = getRMSSpread(scm3DLink->tapDelays3DChannel,scm3DLink->tapPow3DChannel,muDS);
            s1 = getAngleSpread(scm3DLink->tapPow3DChannel,scm3DLink->subpath_Boresight_3DAoD_angle,mu1);
            s2 = getAngleSpread(scm3DLink->tapPow3DChannel,scm3DLink->subpath_Boresight_3DZoD_angle,mu2);
            s3 = getAngleSpread(scm3DLink->tapPow3DChannel,scm3DLink->subpath_Boresight_3DAoA_angle,mu3);
            s4 = getAngleSpread(scm3DLink->tapPow3DChannel,scm3DLink->subpath_Boresight_3DZoA_angle,mu4);
            ss<<muDS*1e9<<",\t"<<sigmaDS*1e9<<",\t"<<mu1<<",\t"<<s1<<",\t"<<mu2<<",\t"<<s2<<",\t"<<mu3<<",\t"<<s3<<",\t"<<mu4<<",\t"<<s4;
            break;
        */case _SCMTYPE_5G_:
            sigmaDS = scm5GLink->getRMSDelaySpread(muDS);
            ss<<muDS*1e9<<",\t"<<sigmaDS*1e9<<",\t"<<scm5GLink->getAngleSpreads();
            break;
        case _SCMTYPE_NTN_:
            sigmaDS = ntnSCMLink->getRMSDelaySpread(muDS);
            ss<<muDS*1e9<<",\t"<<sigmaDS*1e9<<",\t"<<ntnSCMLink->getAngleSpreads();
            break;
        default:
            cout<<"SCM channel not initialised..."<<endl;abort();
            break;
    }
    return ss.str();
}

string SCMLink::getClusterSpread()
{
    if(scmType == _SCMTYPE_5G_)
        return scm5GLink->getClusterSpreads();
    else
        return "";
}

SCMLinkCondition SCMLink::getSCMLinkCondition()
{
    if(scmType == _SCMTYPE_5G_)
        return scm5GLink->scmLinkCondition;
    else if(scmType == _SCMTYPE_NTN_)
        return ntnSCMLink->scmLinkCondition;
    else
        return _UNINIT_SCMLC_;
}
SCMType getSCMType(SCMPropagationScenario scmScenario)
{
    switch(scmScenario)
    {
        case _SCM2D_URBAN_MACRO_:
        case _SCM2D_URBAN_MICRO_:
        case _SCM2D_RURAL_MACRO_:
        case _SCM2D_INDOOR_HOTSPOT_:
            return _SCMTYPE_2D_;
            break;
            
        case _SCM3D_URBAN_MICRO_:
        case _SCM3D_URBAN_MACRO_:
            return _SCMTYPE_3D_;
            break;
            
        case _SCM5G_URBAN_MICRO_:
        case _SCM5G_URBAN_MACRO_:
        case _SCM5G_RURAL_MACRO_:
        case _SCM5G_INDOOR_OFFICE_:
        case _SCM5G_INDOOR_FACTORY_:
            return _SCMTYPE_5G_;
            break;
          
        case _SCMNTN_DENSE_URBAN_:
        case _SCMNTN_URBAN_:
        case _SCMNTN_SUB_URBAN_:
        case _SCMNTN_RURAL_:
            return _SCMTYPE_NTN_;
            break;
            
        default:
            return _SCMTYPE_UNINIT_;
            break;
    }
}
// SCM2DLink::SCM2DLink(){
//   
//   isReady=false;
//   isLOS=-1;
//   isOutdoorToIndoor=-1;
//   nClusters=-1;
//   tapPow.set_length(0);
//   tapDelays.set_length(0);
//   pathAoD.set_length(0);
//   pathAoA.set_length(0);
//   subpathAoA_association_Index.set_length(0);
//   subpathPhase_fixedpt.set_length(0);
//   subpath_Boresight_AoD_angle.set_length(0);
//   subpath_Boresight_AoA_angle.set_length(0);
//   SubPathtypeinCluster.set_length(0);
//   aMap.set_length(0);
//   XPR_lin=-1;
//   theta_Tx=-1;
//   theta_Rx=-1;
//   phi_Tx=-1;
//   phi_Rx=-1;
//   K_rice=-1;
//   v_phase=-1;
//   propagationScenario2DChannel=_SCM_UNINIT_;
//   txLosAntGain=0;
//   rxLosAntGain=0;
//   txCrossPolarizationFlag=false;
//   txPolarizationAngleInDegree=90;
//   rxCrossPolarizationFlag=false;
//   rxPolarizationAngleInDegree=90;
// }

// SCM2DLink::SCM2DLink(int dummy){
//   
//   isReady=false;
//   isLOS=-1;
//   isOutdoorToIndoor=-1;
//   nClusters=-1;
//   tapPow.set_length(0);
//   tapDelays.set_length(0);
//   pathAoD.set_length(0);
//   pathAoA.set_length(0);
//   subpathAoA_association_Index.set_length(0);
//   subpathPhase_fixedpt.set_length(0);
//   subpath_Boresight_AoD_angle.set_length(0);
//   subpath_Boresight_AoA_angle.set_length(0);
//   SubPathtypeinCluster.set_length(0);
//   aMap.set_length(0);
//   XPR_lin=-1;
//   theta_Tx=-1;
//   theta_Rx=-1;
//   phi_Tx=-1;
//   phi_Rx=-1;
//   K_rice=-1;
//   v_phase=-1;
//   propagationScenario=_SCM_UNINIT_;
//   addPathSpecificAntennaGain=true;
//   
//   txCrossPolarizationFlag=false;
//   rxCrossPolarizationFlag=false;
//   
//   txPolarizationAngleInDegree.set_size(0);
//   rxPolarizationAngleInDegree.set_size(0);
// }

// bool SCM2DLink::isInitialized(){
//   
//   return(isReady);
// }
// 
// void SCM2DLink::diablePathSpecificAntennaGain(){
//   
//   addPathSpecificAntennaGain2D=false;
// }
// 
// void SCM2DLink::enablePathSpecificAntennaGain(){
//   
//   addPathSpecificAntennaGain2D=true;
// }
// 
// vec SCM2DLink::get2DSCMTapDelays(){
//   
//   if(!isReady)
//   {
//     cout<<"Please initialize 2D-SCM Link before calling getTapDelays()..."<<endl;
//     return(vec(""));
//   }
//   return(tapDelays);
// }
// 
// double SCM2DLink::get2DSCMLOSAntennaGainInDB(){
//   
//    return(dB(sqr(txLosAntGain*rxLosAntGain))); 
// }
// 
// 
// 
// double SCM2DLink::initialize2DFadingParams(int  isOutdoortoIndoor,bool isLineOfSight,double dopplerSpread,double carrierFrequency,SCMPropagationScenario propScenario,SCMLargeScaleParameters_S &scmParameters,Location_S nodeLocationTx, Location_S nodeLocationRx,Antenna &txAntenna, Antenna &rxAntenna ,double txAntSpacing,double rxAntSpacing)
// {
//   if( isLineOfSight==1 && isOutdoortoIndoor==1 )
//     isLineOfSight = 0;
//   
//   isLOS = isLineOfSight;
//   isOutdoorToIndoor=isOutdoortoIndoor;
//   
//   propagationScenario2DChannel=propScenario;
//   dopplerSpreadInHz=dopplerSpread;
//   carrierFrequencyInHz=carrierFrequency;
//   mobileVelocity=dopplerSpreadInHz*lightSpeed/carrierFrequencyInHz;
//   vec v2_LSP;
//   K_rice = scmParameters.k;
//   
//   if(isLOS == true )
//   {
//     
//     v2_LSP.set_size(5);
//     v2_LSP(0) = scmParameters.ds;
//     v2_LSP(1) = scmParameters.asd;
//     v2_LSP(2) = scmParameters.asa;
//     v2_LSP(3) = scmParameters.sf;
//     v2_LSP(4) = scmParameters.k;
//     K_rice = pow(10.0,v2_LSP(4)/10.0);
//       
//   }
//   else
//   {
//     v2_LSP.set_size(4);
//     v2_LSP(0) = scmParameters.ds;
//     v2_LSP(1) = scmParameters.asd;
//     v2_LSP(2) = scmParameters.asa;
//     v2_LSP(3) = scmParameters.sf;
//   }
//   
//   initializeChannelModel(v2_LSP,nodeLocationTx,nodeLocationRx,txAntenna,rxAntenna,txAntSpacing,rxAntSpacing);
//   return(max(tapDelays));
// }
//   
// void SCM2DLink::initializeChannelModel(vec v2_LSP,Location_S nodeLocationTx, Location_S nodeLocationRx,Antenna &txAntenna, Antenna &rxAntenna,double txAntSpacing, double rxAntSpacing)
// {
//   
//   int NosOfClusters , NosOfRays;
//   NosOfRays = 20;
//   double UE_sigma_AOD,UE_sigma_AOA,UE_sigma_DS;
//   UE_sigma_AOD = pow(10.0,v2_LSP(1)); 
//   UE_sigma_AOA = pow(10.0,v2_LSP(2)); 
//   UE_sigma_DS = pow(10.0,v2_LSP(0)); 
// 
//   if( isLOS==1 && isOutdoorToIndoor==1 )
//     isLOS = 0;
//   
//   if (isLOS ==1)
//     K_rice = pow(10.0,v2_LSP(4)/10.0); 
//     
//   NosOfClusters = getClusterCount();
//   nClusters = NosOfClusters;
//   
//   v_phase = (2*randu()-1)*180;
//   
//   // Limiting the AOA and AOD to 104 degress (ITU-R M.2135 , pg no: 34)
//   if (UE_sigma_AOA >104.0)
//     UE_sigma_AOA = 104.0;
//   if (UE_sigma_AOD >104.0)
//     UE_sigma_AOD = 104.0;
//   
//   generatePowerDelayProfiles(NosOfClusters,UE_sigma_DS,tapPow,tapDelays);
//   pathAoD=generatePathAODs(tapPow,UE_sigma_AOD);
//   pathAoA=generatePathAOAs(tapPow,UE_sigma_AOA);
//   subpathAoA_association_Index=associateBSMSpaths(tapPow.length(),NosOfRays);
//   getBoresight_LOS_angle(theta_Tx,theta_Rx,phi_Tx,phi_Rx,nodeLocationTx,nodeLocationRx);
//   generateSubClusters(NosOfRays);
//   subpathPhase_fixedpt = generateSubpathPhases(tapPow.length());
//   XPR_lin = getCrossPolarizationRatio();
//   computeBoresight2subpath_angles(tapPow.length(),NosOfRays);
//   computeAntennaGains(txAntenna,rxAntenna);
// 
//   int txAntennaCount=txAntenna.getNumberOfPhysicalAntenna();
//   txCrossPolarizationFlag=txAntenna.isAntennaCrossPolarized();
//   txPolarizationAngleInDegree.set_size(txAntennaCount);
//   txAntennaSpacing.set_size(txAntennaCount);
//   if(txCrossPolarizationFlag==false)
//   {
//     txPolarizationAngleInDegree=txAntenna.getCrossPolarizationAngle();
//     for(int tx_ant=0;tx_ant<txAntennaCount;tx_ant++)
//       txAntennaSpacing(tx_ant)=txAntSpacing*(double)tx_ant;
//   }
//   else
//   {
//     if(txAntennaCount%2==1)
//     {
//       cout<<"[detl:] Error. Cross Polarization can't be enabled with odd number of tx antennas."<<endl;
//       abort();
//     }
//     for(int tx_ant=0;tx_ant<txAntennaCount;tx_ant++)
//     {
//       txPolarizationAngleInDegree(tx_ant)=45*pow(-1,tx_ant);
//       txAntennaSpacing(tx_ant)=txAntSpacing*floor(tx_ant/2.0);
//     }
//   }
// 
//   int rxAntennaCount=rxAntenna.getNumberOfPhysicalAntenna();
//   rxCrossPolarizationFlag=rxAntenna.isAntennaCrossPolarized();
//   rxPolarizationAngleInDegree.set_size(rxAntennaCount);
//   rxAntennaSpacing.set_size(rxAntennaCount);
// 
//   if(rxCrossPolarizationFlag==false)
//   {
//     rxPolarizationAngleInDegree=rxAntenna.getCrossPolarizationAngle();
//     for(int rx_ant=0;rx_ant<rxAntennaCount;rx_ant++)
//       rxAntennaSpacing(rx_ant)=rxAntSpacing*(double)rx_ant;
//   }
//   else
//   {
//     if(rxAntennaCount%2==1)
//     {
//       cout<<"[detl:] Error. Cross Polarization can't be enabled with odd number of rx antennas."<<endl;
//       abort();
//     }
//     for(int rx_ant=0;rx_ant<rxAntennaCount;rx_ant++)
//     {
//       rxPolarizationAngleInDegree(rx_ant)=45*pow(-1,rx_ant);
//       rxAntennaSpacing(rx_ant)=rxAntSpacing*floor(rx_ant/2.0);
//     }
//   }
//   isReady=true;
// }

vec getAvgAntennaGains(Array<vec>& subpathAoDZoDAntennaGain,Array<vec>& subpathAoAZoAAntennaGain,vec tapPow,int isLOS,double K_rice)
{ cout<<"tap pow avg ant gain:"<<tapPow<<endl;
  cout<<"is line of sight"<<isLOS<<endl;

    if (isLOS == 1)
    {
        tapPow = (1/(K_rice+1))*tapPow;
        tapPow(0) = tapPow(0) + K_rice/(K_rice+1);
    }

    double avgTxAntennaGain=0;
    for (int i=0; i<subpathAoDZoDAntennaGain.length(); i++)
        avgTxAntennaGain+=tapPow(i)*mean(sqr(subpathAoDZoDAntennaGain(i)));
    
    double avgRxAntennaGain=0;
    for (int i=0; i<subpathAoAZoAAntennaGain.length(); i++)
        avgRxAntennaGain+=tapPow(i)*mean(sqr(subpathAoAZoAAntennaGain(i)));

    vec avgGain = concat(avgTxAntennaGain,to_vec(avgRxAntennaGain));
    cout<<"avgGain : "<<avgGain<<endl;
    return avgGain;
}

/*void SCM2DLink::computeAntennaGains(Antenna &txAntenna, Antenna &rxAntenna){

  txLosAntGain =sqrt(inv_dB(txAntenna.getAntennaGain(theta_Tx, phi_Tx)));
  rxLosAntGain =sqrt(inv_dB(rxAntenna.getAntennaGain(theta_Rx, phi_Rx)));
  
  if(addPathSpecificAntennaGain2D)
  {  
    subpathAoDAntennaGain.set_length(subpath_Boresight_AoD_angle.length());
    for(int cnt1=0;cnt1<subpath_Boresight_AoD_angle.length();cnt1++)
    {
      subpathAoDAntennaGain(cnt1).set_length(subpath_Boresight_AoD_angle(cnt1).length());
      for(int cnt2=0;cnt2<subpath_Boresight_AoD_angle(cnt1).length();cnt2++)
	subpathAoDAntennaGain(cnt1)(cnt2)=sqrt(inv_dB(txAntenna.getAntennaGain(subpath_Boresight_AoD_angle(cnt1)(cnt2),phi_Tx)));
    }   
    
    subpathAoAAntennaGain.set_length(subpath_Boresight_AoA_angle.length());
    for(int cnt1=0;cnt1<subpath_Boresight_AoA_angle.length();cnt1++)
    {
      subpathAoAAntennaGain(cnt1).set_length(subpath_Boresight_AoA_angle(cnt1).length());
      for(int cnt2=0;cnt2<subpath_Boresight_AoA_angle(cnt1).length();cnt2++)
	subpathAoAAntennaGain(cnt1)(cnt2)=sqrt(inv_dB(rxAntenna.getAntennaGain(subpath_Boresight_AoA_angle(cnt1)(cnt2),phi_Rx)));
    }
    
    vec avgGains = getAvgAntennaGains(subpathAoDAntennaGain,subpathAoAAntennaGain,tapPow,isLOS,K_rice);
    
    txAvgAntGain = avgGains(0),rxAvgAntGain = avgGains(1);
  }
 
}

cvec SCM2DLink::generate2DSCMTapGains(int currentTxAntenna, int currentRxAntenna,double currentTimeInSeconds)
{
  
  if(!isReady)
  {
    cout<<"[both:] SCM2DLink not initialized ... Please call initializeFadingParams() before calling generateTapGains()..."<<endl;
    abort();
  }
  
  double GTx_hpol, GTx_vpol, GRx_hpol,GRx_vpol,K,d2rad;
  int M;
  cmat F_rx,F_tx,Rot,tmp;
  cvec tapGain;
  ivec idx;
  double c1,c2,c3,pvv,phv,pvh,phh;
  double lamda=lightSpeed/carrierFrequencyInHz;
  double mobileVelocity=dopplerSpreadInHz*lamda;
  
  F_rx.set_size(1,2);
  F_tx.set_size(2,1);
  Rot.set_size(2,2);
  
  
  K = 2*pi/lamda;
  d2rad = pi/180; 
  tapGain.set_length(tapPow.length());
  tapGain.zeros();
  
  for (int i=0; i<tapPow.length(); i++)
  {
    idx = getSubPathIndicesofClusters(SubPathtypeinCluster(i));
    M = idx.length();
    for (int j = 0; j<M; j++)
    {
      if(addPathSpecificAntennaGain2D)
      {
          GTx_vpol = subpathAoDAntennaGain(i)(j)*sin(d2rad*txPolarizationAngleInDegree(currentTxAntenna));
          GTx_hpol = subpathAoDAntennaGain(i)(j)*cos(d2rad*txPolarizationAngleInDegree(currentTxAntenna));

          GRx_vpol = subpathAoAAntennaGain(i)(j)*sin(d2rad*rxPolarizationAngleInDegree(currentRxAntenna));
          GRx_hpol = subpathAoAAntennaGain(i)(j)*cos(d2rad*rxPolarizationAngleInDegree(currentRxAntenna));
      }
      else
      {
          GTx_vpol = txLosAntGain*sin(d2rad*txPolarizationAngleInDegree(currentTxAntenna));
          GTx_hpol = txLosAntGain*cos(d2rad*txPolarizationAngleInDegree(currentTxAntenna));

          GRx_vpol = rxLosAntGain*sin(d2rad*rxPolarizationAngleInDegree(currentRxAntenna));
          GRx_hpol = rxLosAntGain*cos(d2rad*rxPolarizationAngleInDegree(currentRxAntenna));
      }
      c1 = K*(txAntennaSpacing(currentTxAntenna))*sin(d2rad*subpath_Boresight_AoD_angle(i)(j));
      c2 = K*(rxAntennaSpacing(currentRxAntenna))*sin(d2rad*subpath_Boresight_AoA_angle(i)(j));
      c3 = K*mobileVelocity*cos(d2rad*(subpath_Boresight_AoA_angle(i)(j) - v_phase))*currentTimeInSeconds;
      pvv = d2rad*subpathPhase_fixedpt(i)(4*j)/10; 
      pvh = d2rad*subpathPhase_fixedpt(i)(4*j+1)/10;
      phv = d2rad*subpathPhase_fixedpt(i)(4*j+2)/10;
      phh = d2rad*subpathPhase_fixedpt(i)(4*j+3)/10;
      
      F_rx(0,0) = GRx_vpol;
      F_rx(0,1) = GRx_hpol;
      F_tx(0,0) = GTx_vpol;
      F_tx(1,0) = GTx_hpol;
      
      Rot(0,0) = complex< double >(cos(pvv),sin(pvv));      
      Rot(0,1) = sqrt(1/XPR_lin)*complex< double >(cos(pvh),sin(pvh));      
      Rot(1,0) = sqrt(1/XPR_lin)*complex< double >(cos(phv),sin(phv));      
      Rot(1,1) = complex< double >(cos(phh),sin(phh));
      
      tmp = F_rx*Rot*F_tx; 
      tapGain(i) += sqrt(tapPow(i)/M)*tmp(0,0)*complex< double >(cos(c1),sin(c1))*complex< double >(cos(c2),sin(c2))*complex< double >(cos(c3),sin(c3));
    }
  }
  
  
  if (isLOS == 1)
  {
    GTx_vpol = txLosAntGain*sin(d2rad*txPolarizationAngleInDegree(currentTxAntenna));
    GTx_hpol = txLosAntGain*cos(d2rad*txPolarizationAngleInDegree(currentTxAntenna));

    GRx_vpol = rxLosAntGain*sin(d2rad*rxPolarizationAngleInDegree(currentRxAntenna));
    GRx_hpol = rxLosAntGain*cos(d2rad*rxPolarizationAngleInDegree(currentRxAntenna));
      
    c1 = K*(txAntennaSpacing(currentTxAntenna))*sin(d2rad*theta_Tx);
    c2 = K*(rxAntennaSpacing(currentRxAntenna))*sin(d2rad*theta_Rx);
    c3 = K*mobileVelocity*cos(d2rad*(theta_Rx - v_phase))*currentTimeInSeconds;
    
    M = subpathPhase_fixedpt.length();
    pvv = d2rad*subpathPhase_fixedpt(0)(M-2)/10;
    phh = d2rad*subpathPhase_fixedpt(0)(M-1)/10;
    
    F_rx(0,0) = GRx_vpol;
    F_rx(0,1) = GRx_hpol;
    F_tx(0,0) = GTx_vpol;
    F_tx(1,0) = GTx_hpol;
    
    Rot.zeros();
    Rot(0,0) = complex< double >(cos(pvv),sin(pvv));
    Rot(1,1) = complex< double >(cos(phh),sin(phh));
    
    tmp = F_rx*Rot*F_tx; 
    tapGain = sqrt(1/(K_rice+1.0))*tapGain;
    tapGain(0) += sqrt(K_rice/(K_rice+1.0))*tmp(0,0)*complex< double >(cos(c1),sin(c1))*complex< double >(cos(c2),sin(c2))*complex< double >(cos(c3),sin(c3));
    
  }
  
  return tapGain;
}      


void SCM2DLink::generatePowerDelayProfiles(int N,double sigma_DS,vec& tapPow,vec& tapdelays)
{
  vec z,tau,tau_Los,tauc,Pn_dash,Pnc,Pn,Pn_sort,zeta;
  double tmp,cutoff;
  double sigma_RND,D,K1,r_DS;
  ivec Pn_sort_index,skip_index;
  int M;
  
  r_DS = getDelayScaling();
  sigma_RND = getClusterShadow();
  
  z = randu(N);
  tau = -1*r_DS*sigma_DS*log(z);
  sort(tau);
  tmp = tau(0);
  tau = tau - tmp;
  tau(0)=0;

  if (isLOS == 1)
  {
    zeta = sigma_RND*randn(N); 
    Pn_dash = elem_mult( exp( (1-r_DS)*tau/(r_DS*sigma_DS) ) , pow10(-1*zeta/10) );
    Pn = Pn_dash/sum(Pn_dash);
    
    Pn_sort = -1*Pn;
    sort(Pn_sort);
    Pn_sort = Pn_sort*-1; 
    Pn_sort_index = sort_index(-1*Pn);
    
    skip_index.set_length(0);
    cutoff = 0.0032*max(Pn); // -25dB
    for (int i = 0; i < N; i++)
    {
      if (Pn_sort(i) < cutoff)
      {
	M = skip_index.length();
	skip_index.set_length(M+1,true);
	skip_index(M) = Pn_sort_index(i);
      }
    }
    
    sort(skip_index);
    for (int i = skip_index.length()-1; i>=0; i--)
    {
      tau.del(skip_index(i));
      Pn.del(skip_index(i));
      
    } 
    
    tauc = tau;
    K1 = 10*log10(K_rice); 
    D = 0.7705 - 0.0433*K1 + 0.0002*pow(K1,2) + 0.000017*pow(K1,3);
    tau_Los = tauc/D;
    Pnc = Pn;
    tapdelays = tau_Los;
    
  }
  else
  {
    zeta = sigma_RND*randn(N); 
    Pn_dash = elem_mult( exp( (1-r_DS)*tau/(r_DS*sigma_DS) ) , pow10(-1*zeta/10) );
    Pn = Pn_dash/sum(Pn_dash);
    
    Pn_sort = -1*Pn; 
    sort(Pn_sort);
    Pn_sort = Pn_sort*-1; 
    Pn_sort_index = sort_index(-1*Pn);
    
    skip_index.set_length(0);
    cutoff = 0.0032*max(Pn); // -25dB
    for (int i = 0; i < N; i++)
    {
      if (Pn_sort(i) < cutoff)
      {
	M = skip_index.length();
	skip_index.set_length(M+1,true);
	skip_index(M) = Pn_sort_index(i);
      }
    }
    
    sort(skip_index);
    for (int i = skip_index.length()-1; i>=0; i--)
    {
      tau.del(skip_index(i));
      Pn.del(skip_index(i));
      
    } 
    
    tauc = tau;
    Pnc = Pn;
    tapdelays = tauc;
  }
  Pnc = Pnc/sum(Pnc); 
  tapPow = Pnc; 
  
  
//   double taubar=sum(elem_mult(tapPow,tapdelays))/sum(tapPow);
//   double tauSqbar=sum(elem_mult(tapPow,sqr(tapdelays)))/sum(tapPow);
//   double rmsDelaySpread=sqrt(tauSqbar-sqr(taubar));
//   
//   
//   cout<<"tapdelays : "<<tapdelays<<endl;
//   cout<<"tapPow : "<<tapPow<<endl;
//   
//   
//   cout<<"taubar : "<<taubar<<endl;
//   cout<<"tauSqbar : "<<tauSqbar<<endl;
//   std::ofstream fout("rms.txt",std::ios::app);
//   
//   fout<<rmsDelaySpread<<endl;
  
}

double SCM2DLink::getClusterShadow()
{
  double zeta;
  
  switch (propagationScenario2DChannel) // see 3GPP TR 36.814 v 9.0.0, Table B.1.2.2.1-4
  {
    case _SCM2D_URBAN_MACRO_:
      if (isLOS == 1)	zeta = 3.0;
      else		zeta = 3.0;
      break;
      
    case _SCM2D_URBAN_MICRO_:
      if (isLOS==1)      zeta = 3.0;
      else
	if (isOutdoorToIndoor == 1) zeta = 4.0;
	else		 zeta = 3.0;
      break;
      
    case _SCM2D_RURAL_MACRO_:
      if (isLOS == 1)	zeta = 3.0;
      else		zeta = 3.0;
      break;
      
    case _SCM2D_INDOOR_HOTSPOT_:
      if (isLOS == 1)	zeta = 6.0;
      else		zeta = 3.0;
      break;
    default :
      cout << "\n\n\ngetClusterShadow: unknown Propagation scenario -- exiting program" << endl;
      abort();
      break;
  }
  return zeta;
  
}

double SCM2DLink::getDelayScaling()
{
  double rtau;
  switch (propagationScenario2DChannel) // see 3GPP TR 36.814 v 9.0.0, Table B.1.2.2.1-4
  {
    case _SCM2D_URBAN_MACRO_:
      if (isLOS == 1)	rtau = 2.5;
      else		rtau = 2.3;
      break;
      
    case _SCM2D_URBAN_MICRO_:
      if (isLOS==1)	rtau = 3.2;
      else	
	if (isOutdoorToIndoor == 1)rtau = 2.2;
	else	  	rtau = 3.0;
      break;
      
    case _SCM2D_RURAL_MACRO_:
      if (isLOS == 1)	rtau = 3.8;
      else		rtau = 1.7;
      break;
      
    case _SCM2D_INDOOR_HOTSPOT_:
      if (isLOS == 1)	rtau = 3.6;
      else	        rtau = 3.0;
      break;
    default :
      cout << "\n\n\ngetDelayScaling: unknown Propagation scenario -- exiting program" << endl;
      abort();
      break;
  }
  
  return rtau;
  
}


vec SCM2DLink::generatePathAODs(vec tpow,double sigma_AOD)
{
  vec x,y;
  ivec p;
  vec pathAoD;
  
  switch (propagationScenario2DChannel)
  {
    case _SCM2D_URBAN_MACRO_:
      pathAoD = generateClusterAngles(tpow,sigma_AOD);
      break;
    case _SCM2D_URBAN_MICRO_:
      pathAoD = generateClusterAngles(tpow,sigma_AOD);
      break;
    case _SCM2D_RURAL_MACRO_:
      pathAoD = generateClusterAngles(tpow,sigma_AOD);
      break;
    case _SCM2D_INDOOR_HOTSPOT_:
      pathAoD = generateClusterAngles(tpow,sigma_AOD);
      break;
    default :
      cout << "\n\n\ngeneratePathAODs: unknown Propagation scenario -- exiting program" << endl;
      abort();
      break;
  }
  return(pathAoD);
}

vec SCM2DLink::generatePathAOAs(vec tpow,double sigma_AOA)
{
  vec AoA;
  vec sigma_AoA,tmp;
  AoA.set_length(tpow.length());
  
  
  switch (propagationScenario2DChannel)
  {
    case _SCM2D_URBAN_MACRO_:
      AoA = generateClusterAngles(tpow,sigma_AOA);
      break;
    case _SCM2D_URBAN_MICRO_:
      AoA = generateClusterAngles(tpow,sigma_AOA);
      break;
    case _SCM2D_RURAL_MACRO_:
      AoA = generateClusterAngles(tpow,sigma_AOA);
      break;
    case _SCM2D_INDOOR_HOTSPOT_:
      AoA = generateClusterAngles(tpow,sigma_AOA);
      break;
    default :
      cout << "\n\n\ngeneratePathAOAs: unknown Propagation scenario -- exiting program" << endl;
      abort();
      break;
  }
  return(AoA);
}

vec SCM2DLink::generateClusterAngles(vec tpow, double sigma_A)
{
  vec pathAngles,tmp,SignScale;
  double C;
  double Pmax;
  int N;
  
  pathAngles.set_length(tpow.length());
  N = getClusterCount();
  
  switch (N) // see 3GPP TR 36.814 v 9.0.0, Table B.1.2.2.1-1
  {
    case 4:      C = 0.779;break;
    case 5:      C = 0.860;break;
    case 8:      C = 1.018;break;
    case 10:     C = 1.090;break;
    case 11:     C = 1.123;break;
    case 12:     C = 1.146;break;
    case 14:     C = 1.190;break;
    case 15:     
      C = 1.211;
      if (propagationScenario2DChannel == _SCM2D_INDOOR_HOTSPOT_)	C = 1.434;
      break;
    case 16:     C = 1.226;break;
    case 19:     
      C = 1.273;
      if (propagationScenario2DChannel == _SCM2D_INDOOR_HOTSPOT_)   C = 1.501;
      break;
    case 20:     C = 1.289;break;
    default :
      cout << "\n\n\ngenerateClusterAngles: wrong number of clusters -- exiting program" << endl;
      abort();
      break;
  }
  
  double K = 10*log10(K_rice);
  if (isLOS == 1)
  {
    if (propagationScenario2DChannel == _SCM2D_INDOOR_HOTSPOT_)
      C = C*(0.9275 + 0.0439*K - 0.0071*pow(K,2) + 0.0002*pow(K,3));
    else
      C = C*(1.1035 - 0.028*K - 0.002*pow(K,2) + 0.0001*pow(K,3));
  }

  K = pow(10.0,K/10);
  if (isLOS == 1)
  {
    tpow = (1/(K+1))*tpow;
    tpow(0) = tpow(0) + K/(K+1);
  }

  Pmax = max(tpow);
  int PmaxIndx=max_index(tpow);
  for (int i=0; i < pathAngles.length(); i++)
    if(i==PmaxIndx)
      pathAngles(i) = 0;
    else
    {
      /// sigma_A here is the "sigma_phi" in  eqn 13,14 of 3GPP TR 36.814 v 9.0.0, page 111
      if (propagationScenario2DChannel == _SCM2D_INDOOR_HOTSPOT_)
	pathAngles(i) = -1.0*sigma_A*log( tpow(i)/Pmax ) / C;
      else
	pathAngles(i) = (2.0*sigma_A/1.4)*sqrt( -1*log( tpow(i)/Pmax ) ) / C; 
    }

  tmp = randu(pathAngles.length()) - 0.5;
  SignScale = sign(tmp);
  tmp = (sigma_A/7)*randn(pathAngles.length()); 
  pathAngles = elem_mult(pathAngles,SignScale) + tmp; 
  if (isLOS == 1)
    pathAngles = pathAngles - pathAngles(0);
  
  return pathAngles;
}

int SCM2DLink::getClusterCount()
{
  int N;
  switch (propagationScenario2DChannel) // see 3GPP TR 36.814 v 9.0.0, Table B.1.2.2.1-4
  {
    case _SCM2D_URBAN_MACRO_:
      if (isLOS == 1)	N = 12;
      else		N = 20;
      break;
      
    case _SCM2D_URBAN_MICRO_:
      if (isLOS==1)    N = 12;
      else
	if (isOutdoorToIndoor == 1)N = 12;
	else		N = 19;
      break;
      
    case _SCM2D_RURAL_MACRO_:
      if (isLOS == 1)	N = 11;
      else		N = 10;
      break;
      
    case _SCM2D_INDOOR_HOTSPOT_:
      if (isLOS == 1)	N = 15;
      else		N = 19;
      break;
    default :
      cout << "\n\n\ngetClusterCount: unknown Propagation scenario -- exiting program" << endl;
      abort();
      break;
  }
  
  return N;
}

Array<ivec> SCM2DLink::associateBSMSpaths(int N,int M)
{
  Array<ivec> AoA_associated_Index;
  AoA_associated_Index.set_size(N);
  int m,n,tmp1;
  ivec idx;
  
  if (M != 20)
  {
    cout << "\n\n\nassociateBSMSpaths: Size mismatch  should be same as subpath angle offsets size, presently assumed to be 20 -- exiting program" << endl;
    abort();
  }
  
  idx.set_length(M);
  for (int i = 0; i<M; i++)
    idx(i) = i;
  
  for (int i = 0; i<N; i++)
  {
    AoA_associated_Index(i) = idx;
    
    for (int j = 0; j<M*10; j++)
    {
      m = randi(0,M-1);
      n = randi(0,M-1);
      
      tmp1 = AoA_associated_Index(i)(m);
      AoA_associated_Index(i)(m) = AoA_associated_Index(i)(n);
      AoA_associated_Index(i)(n) = tmp1;
    }
  }
  return(AoA_associated_Index);
}
      
void SCM2DLink::getBoresight_LOS_angle(double& thetaTx, double& thetaRx, double& phiTx, double& phiRx, Location_S nodeLocationTx, Location_S nodeLocationRx)
{
  vec angleInDegree=findAngle(nodeLocationTx,nodeLocationRx);
  thetaTx = angleInDegree(0);
  phiTx = angleInDegree(1); 
  
  angleInDegree=findAngle(nodeLocationRx,nodeLocationTx);
  thetaRx = angleInDegree(0);
  phiRx = angleInDegree(1); 
}

void SCM2DLink::generateSubClusters(int M)
{
  double subdelay1,subdelay2;
  vec newDelays,tmp1,allDelays,newtapPow,Pn_sort;
  ivec newIndex,SnewIndex,newClusterIndex,clusterSubPathtype,subPathIndices,angleMap,Pn_sort_index,strong2,unsplitindx;
  int Q, x=-1,tt,n0,n1,s0,s1,cnt;
  
  Pn_sort = tapPow;
  Pn_sort_index = sort_index(tapPow);
  strong2 = Pn_sort_index.mid(Pn_sort_index.length()-2,2);
  if (tapDelays(strong2(0))>tapDelays(strong2(1)))
  {
    tt = strong2(0);
    strong2(0) = strong2(1);
    strong2(1) = tt;
  }
  n0 = strong2(0); 
  n1 = strong2(1);
  
  unsplitindx.set_length(tapPow.length());
  for (int i = 0; i<unsplitindx.length(); i++)
  {
    unsplitindx(i) = i;
  }
  unsplitindx.del(n1);
  unsplitindx.del(n0);
  
  Q = 4;
  newDelays.set_length(Q);
  subdelay1 = 5e-9;
  subdelay2 = 10e-9;
  newDelays(0) = tapDelays(n0)+subdelay1;
  newDelays(1) = tapDelays(n0)+subdelay2;
  newDelays(2) = tapDelays(n1)+subdelay1;
  newDelays(3) = tapDelays(n1)+subdelay2;
  
  /// Find the locations into which new delays falls in a sorted order
  newIndex.set_length(newDelays.length());
  tmp1 = tapDelays;
  for (int m = 0; m<newDelays.length(); m++)
  {
    for (int i = 1; i < tmp1.length(); i++) 
    {
      if (newDelays(m) < tmp1(i))
      {
	x = i;
	break;
      }
      else
 	x = i+1;
    }
    tmp1.ins(x,newDelays(m));
    newIndex(m) = x;
    for (int n = 0; n<m; n++)
      if(newIndex(m)<=newIndex(n))
	newIndex(n) = newIndex(n)+1;
  }
  ///find the index of each of the originial clusters in the new sorted order (of delays)
    newClusterIndex.set_length(tapPow.length());
    for (int i =0; i<newClusterIndex.length(); i++) 
     newClusterIndex(i) = i;
    
    SnewIndex = newIndex;
    sort(SnewIndex);
    for (int i = 0; i<SnewIndex.length(); i++)
    {
      for (int j = 0; j<newClusterIndex.length(); j++)
      {
	if (newClusterIndex(j) >= SnewIndex(i)) 
	  newClusterIndex(j) = newClusterIndex(j)+1;
      }
    }
    
    allDelays = tmp1; 
    clusterSubPathtype.set_length(tapPow.length()+Q);
    newtapPow.set_length(tapPow.length()+Q);
    angleMap.set_length(tapPow.length()+Q);
    
    
    subPathIndices = getSubPathIndicesofClusters(0);
    if (subPathIndices.length() != M)
      cout << "generateSubClusters: num of subpaths in original cluster should be same as set0 indicies" << endl;
    
    s0 = n0;
    s1 = n1;
    for (int i =0; i<newDelays.length();i++)
      if (tapDelays(n1)>newDelays(i))
	s1 = s1+1;

    cnt = 0;
    
    for (int i = 0; i < newClusterIndex.length(); i++)
    {
      if (!(newClusterIndex(i) == s0 || newClusterIndex(i) == s1))
      {
	clusterSubPathtype(newClusterIndex(i)) = 0; 
	newtapPow(newClusterIndex(i)) = tapPow(unsplitindx(cnt));
	angleMap(newClusterIndex((i))) = unsplitindx(cnt);
	cnt++;
      }
    }
    
    subPathIndices = getSubPathIndicesofClusters(1);
    clusterSubPathtype((s0)) = 1; 
    clusterSubPathtype((s1)) = 1; 
    newtapPow((s0)) = tapPow(n0)*subPathIndices.length()/M;
    newtapPow((s1)) = tapPow(n1)*subPathIndices.length()/M;
    angleMap((s0)) = n0;
    angleMap((s1)) = n1;
    
    subPathIndices = getSubPathIndicesofClusters(2); 
    clusterSubPathtype(newIndex(0)) = 2; 
    newtapPow(newIndex(0)) = tapPow(n0)*subPathIndices.length()/M;
    angleMap(newIndex(0)) = n0;
    
    subPathIndices = getSubPathIndicesofClusters(3); 
    clusterSubPathtype(newIndex(3)) = 3;
    newtapPow(newIndex(3)) = tapPow(n1)*subPathIndices.length()/M;
    angleMap(newIndex(3)) = n1;
    if ( abs(allDelays(newIndex(1)) - (allDelays(s0) + subdelay2)) < abs(allDelays(newIndex(1)) - (allDelays(s1) + subdelay1))) // sub delay2 is 10 ns
    {
      subPathIndices = getSubPathIndicesofClusters(3);
      clusterSubPathtype(newIndex(1)) = 3;
      newtapPow(newIndex(1)) = tapPow(n0)*subPathIndices.length()/M;
      angleMap(newIndex(1)) = n0;
      subPathIndices = getSubPathIndicesofClusters(2);
      clusterSubPathtype(newIndex(2)) = 2;
      newtapPow(newIndex(2)) = tapPow(n1)*subPathIndices.length()/M;
      angleMap(newIndex(2)) = n1;
    }
    else
    {
      subPathIndices = getSubPathIndicesofClusters(2);
      clusterSubPathtype(newIndex(1)) = 2;
      newtapPow(newIndex(1)) = tapPow(n1)*subPathIndices.length()/M;
      angleMap(newIndex(1)) = n1;
      subPathIndices = getSubPathIndicesofClusters(3);
      clusterSubPathtype(newIndex(2)) = 3;
      newtapPow(newIndex(2)) = tapPow(n0)*subPathIndices.length()/M;
      angleMap(newIndex(2)) = n0;
    }
    
    if (abs(sum(tapPow) - sum(newtapPow)) > 0.0000001 )
    {
      cout << "\n\n\ngenerateSubClusters: Error in power calculation while adjusting for subclusters --- exiting program" << endl;
      abort();
    }
    
    tapPow = newtapPow;
    tapDelays = allDelays;
    SubPathtypeinCluster = clusterSubPathtype;
    aMap = angleMap;
}
      
ivec SCM2DLink::getSubPathIndicesofClusters(int setid)
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
      cout << "\n\n\ngetSubPathIndicesofClusters: unknown cluster subpath set id --- exiting program" << endl;
      abort();
      break;
  }
  
  return indices;
}

Array<ivec> SCM2DLink::generateSubpathPhases(int N)
{
  int cnt;
  ivec idx;
  Array<ivec> phases;
  phases.set_size(N);
  
  for (int i = 0; i< N; i++)
  {
    idx = getSubPathIndicesofClusters(SubPathtypeinCluster(i));
    cnt = 4*idx.length();
    if (isLOS ==1 && i == 0)
      cnt = 4*idx.length()+2;
    phases(i) = floor_i(10*(randu(cnt)*360-180.0)+0.5); 
  }
  
  return phases;
}
      
double SCM2DLink::getCrossPolarizationRatio()
{
  double XPR_dB;
  
  switch (propagationScenario2DChannel) // see 3GPP TR 36.814 v 9.0.0, Table B.1.2.2.1-4
  {
    case _SCM2D_URBAN_MACRO_:
      if (isLOS == 1) 	XPR_dB = 8.0;
      else	 	XPR_dB = 7.0;
      break;
      
    case _SCM2D_URBAN_MICRO_:
      if (isLOS==1) 	XPR_dB = 9.0;
      else
	if (isOutdoorToIndoor == 1)XPR_dB = 9.0;
      else		XPR_dB = 8.0;
      break;
      
    case _SCM2D_RURAL_MACRO_:
      if (isLOS == 1)   XPR_dB = 12.0;
      else	        XPR_dB = 7.0;
      break;
      
    case _SCM2D_INDOOR_HOTSPOT_:
      if (isLOS == 1)	XPR_dB = 11.0;
      else	        XPR_dB = 10.0;
      break;
    default :
      cout << "\n\n\ngetCrossPolarizationRatio: unknown Propagation scenario -- exiting program" << endl;
      abort();
      break;
  }
	
  return pow(10.0,XPR_dB/10);
  
}
      
void SCM2DLink::computeBoresight2subpath_angles(int nTaps, int nRays)
{
  vec subpathAoDoffsets, subpathAoAoffsets,tmpD,tmpA;
  ivec idx;
  
  subpath_Boresight_AoD_angle.set_size(nTaps);
  subpath_Boresight_AoA_angle.set_size(nTaps);  
  
  
  subpathAoDoffsets = generateSubpathAngleOffsets(nRays,0); 
  subpathAoAoffsets = generateSubpathAngleOffsets(nRays,1);
  for (int i=0; i<nTaps; i++)
  {
    idx = getSubPathIndicesofClusters(SubPathtypeinCluster(i));
    tmpD.set_length(idx.length());
    tmpA.set_length(idx.length());
    for (int j = 0; j<idx.length(); j++)
    {
      //for calibration
      tmpD(j) = (double) theta_Tx + pathAoD(aMap(i)) + subpathAoDoffsets(idx(j));
      tmpA(j) = (double) theta_Rx + pathAoA(aMap(i)) + subpathAoAoffsets(subpathAoA_association_Index(aMap(i))(idx(j)));
    }
    subpath_Boresight_AoD_angle(i) = tmpD;
    subpath_Boresight_AoA_angle(i) = tmpA;
  }
}
      
vec SCM2DLink::generateSubpathAngleOffsets(int M,int  isAoA) 
{
  vec offsets;
  double rmsclusterAzimuthSpread;
  
  if (M != 20)
  {
    cout << "\n\n\ngenerateSubpathAngleOffsets: unsupported number of subpaths M -- exiting program" << endl;
    cout << "generateSubpathAngleOffsets: only M=20 is currently supported -- exiting program" << endl;
    abort();
  }
  
  offsets.set_length(M);
  rmsclusterAzimuthSpread = getClusterRMSAzimuthSpread(isAoA);
  
  offsets(0) = 0.0447; //see 3GPP TR 36.814 v 9.0.0, Table B.1.2.2.1-2
  offsets(1) = -0.0447;
  offsets(2) = 0.1413;
  offsets(3) = -0.1413;
  offsets(4) = 0.2492;
  offsets(5) = -0.2492;
  offsets(6) = 0.3715;
  offsets(7) = -0.3715;
  offsets(8) = 0.5129;
  offsets(9) = -0.5129;
  offsets(10) = 0.6797;
  offsets(11) = -0.6797;
  offsets(12) = 0.8844;
  offsets(13) = -0.8844;
  offsets(14) = 1.1481;
  offsets(15) = -1.1481;
  offsets(16) = 1.5195;
  offsets(17) = -1.5195;
  offsets(18) = 2.1551;
  offsets(19) = -2.1551;
  
  offsets = rmsclusterAzimuthSpread*offsets;
  
  return offsets;
}

double SCM2DLink::getClusterRMSAzimuthSpread(int isAoA)
{
  double AS;
  
  if (isAoA == 1)
  {
    switch (propagationScenario2DChannel) //cluster ASA see 3GPP TR 36.814 v 9.0.0, Table B.1.2.2.1-4
    {
      case _SCM2D_URBAN_MACRO_:
	if (isLOS == 1)	  AS = 11.0;
	else		  AS = 15.0;
	break;
	
      case _SCM2D_URBAN_MICRO_:
	if (isLOS==1)	  AS = 17.0;
	else
	  if (isOutdoorToIndoor == 1)AS = 8.0;
	  else	          AS = 22.0;
	break;
	
      case _SCM2D_RURAL_MACRO_:
	if (isLOS == 1)	  AS = 3.0;
	else		  AS = 3.0;
	break;
	
      case _SCM2D_INDOOR_HOTSPOT_:
	if (isLOS == 1)	  AS = 8.0;
	else		  AS = 11.0;
	break;
      default :
	cout << "\n\n\ngetClusterRMSAzimuthSpread: unknown Propagation scenario -- exiting program" << endl;
	abort();
	break;
    }
  }
  else
  {
    switch (propagationScenario2DChannel) //cluster ASD see 3GPP TR 36.814 v 9.0.0, Table B.1.2.2.1-4
    {
      case _SCM2D_URBAN_MACRO_:
	if (isLOS == 1)	  AS = 5.0;
	else		  AS = 2.0;
	break;
	
      case _SCM2D_URBAN_MICRO_:
	if (isLOS==1)	  AS = 3.0;
	else
	  if (isOutdoorToIndoor == 1)AS = 5.0;
	  else	          AS = 10.0;
	break;
	
      case _SCM2D_RURAL_MACRO_:
	if (isLOS == 1)	  AS = 2.0;
	else		  AS = 2.0;
	break;
	
      case _SCM2D_INDOOR_HOTSPOT_:
	if (isLOS == 1)	  AS = 5.0;
	else		  AS = 5.0;
	break;
      default :
	cout << "\n\n\ngetClusterRMSAzimuthSpread: unknown Propagation scenario -- exiting program" << endl;
	abort();
	break;
    }
  }
  
  return AS;
  
}
      
// Calibrating Function

double SCM2DLink::measureDelaySpread(vec p, vec t, double K, int isLOS)
{
  double t_mean,sd_t;
  
  if (isLOS == 1)
  {
    p = p*(1/(K+1));
    p(0) = p(0) + K/(K+1);
  }
  
  t_mean = sum(elem_mult(p,t))/sum(p);
  
  sd_t = sqrt( sum(elem_mult(sqr(t-t_mean),p))/sum(p));
  
  return sd_t;
}

double SCM2DLink::measureCircularAngularSpread(Array<vec> a,vec p,ivec SubPathtypeinCluster,double K,int isLOS,double thetaLOS)
{
  double aSpread,mu,delta,pi,d2rad,x,xtot,ptot;
  int M,R;
  ivec idx;
  vec s;
  
  pi = 3.141592653590;
  d2rad = pi/180; 
  
  if (isLOS == 1)
    p = p*(1/(K+1));
  
  R = 48; 
  s.set_length(R);
  for (int g =0; g<R; g++)
  {
    delta = -180.0*d2rad +360.0*g*d2rad/R;
    
    mu = 0.0;
    xtot = 0.0;
    ptot = 0.0;
    /// calc mu_delta
    for (int i=0; i<p.length(); i++)
    {
      idx = getSubPathIndicesofClusters(SubPathtypeinCluster(i));
      M = idx.length();
      for (int j=0; j<M; j++)
      {
	x = d2rad*a(i)(j) + delta;
	xtot = xtot + x*p(i)/M;
	ptot = ptot + p(i)/M;
      }
    }
    if (isLOS == 1)
    {
      xtot = xtot + (d2rad*thetaLOS + delta)*(K/(K+1));
      ptot = ptot + K/(K+1);
    }
    
    mu = xtot/ptot;
    
    if (mu < -1*pi)
      mu = mu+2*pi;
    if (mu > pi)
      mu = mu-2*pi;
    
    xtot = 0.0;
    ptot = 0.0;
    ///calc sigma AS
    for (int i=0; i<p.length(); i++)
    {
      idx = getSubPathIndicesofClusters(SubPathtypeinCluster(i));
      M = idx.length();
      for (int j=0; j<M; j++)
      {
	x = d2rad*a(i)(j) + delta - mu;
	
	if (x < -1*pi)
	  x = x + 2*pi;
	if (x > pi)
	  x = x - 2*pi;
	
	xtot = xtot + sqr(x)*p(i)/M;
	ptot = ptot + p(i)/M;
      }
    }
    
    if (isLOS == 1)
    {
      x = d2rad*thetaLOS + delta - mu;
      
      if (x < -1*pi)
	x = x + 2*pi;
      if (x > pi)
	x = x - 2*pi;
      
      xtot = xtot + sqr(x)*(K/(K+1));
      ptot = ptot + K/(K+1);
    }
    
    s(g) = sqrt(xtot/ptot);
  }
  
  aSpread = min(s)/d2rad; 
  
  return aSpread;
  
}
     */ 
