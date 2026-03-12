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


#include "../include/Antenna.h"

Antenna::Antenna(){
    
#ifdef USING_SECURITY 
  BWSIM_Password_function();
#endif
  
  parametersUpdateStatus=false;
  antennaWavelengthSpacing=-1;
  gain=0;
  hPattern=zeros(1);
  vPattern=zeros(1);
  antType=_ANTENNA_TYPE_OMNI_;
  hTilt=0;
  vTilt=0;
  verAntmaxAttenuation=-1;
  horAntmaxAttenuation=-1;
  loc.x=NAN;loc.y=NAN;loc.z=NAN;
  hBeamwidth=-1;
  vBeamwidth=-1;
  antennaFeederLossIndB=0;
  isCrossPolarized=false;
  polarizationSlantAnglesInDegree="";
  zenithBeamAngles.set_length(0);
  azimuthBeamAngles.set_length(0);
  antSpacing = -1.0;
  apertureRadius = -1;
  isForHexNode=false;
  // HAPS defaults
  haps_Ge_max=8.0; haps_Am=30.0; haps_SLAv=30.0;
  haps_phi3dB=65.0; haps_theta3dB=65.0;
  haps_M=2; haps_N=2; haps_dv=0.5; haps_dh=0.5;
  haps_alpha=0.0; haps_beta=0.0; haps_gamma=-90.0;  
}

bool Antenna::isAntennaReady(){
  
  bool isReady=true;
  if(std::isnan((double)(loc.x)) || std::isnan((double)(loc.y)) || std::isnan((double)(loc.z)))
  {
	cout<<"Location not set for Antenna "<<endl;
	isReady=false;
  }
  return(isReady);
}

sTXRUParameters Antenna::getTXRUParameters(double carrierFrequency)
{
    sTXRUParameters txruParameters;
    txruParameters.initTXRUParameters(carrierFrequency,antStructure,TXRUMap,antSpacing);
    return txruParameters;
}
sBeamParameters Antenna::getBeamParameters(double carrierFrequency)
{
    sBeamParameters beamParameters;
    beamParameters.configureBeamParameters(antStructure,TXRUMap,antSpacing,azimuthBeamAngles,zenithBeamAngles,carrierFrequency,orientationInfo);
    return beamParameters;
}

void Antenna::setAntennaType(AntennaType_E antennaType){
  
  parametersUpdateStatus=false;
  antType=antennaType;
}

void Antenna::setLoc(Location_S antLoc, bool overwrite){
  
  if(overwrite)
	loc=antLoc;
  else
  {
	if(std::isnan((double)(loc.x))) loc.x=antLoc.x; 	else cout<<"[both:] Warning... Trying to overwrite location for antenna "<<endl;
	if(std::isnan((double)(loc.y))) loc.y=antLoc.y;
	if(std::isnan((double)(loc.z))) loc.z=antLoc.z;	else cout<<"[both:] Warning... Trying to overwrite height for antenna "<<endl;
  }  

}

void Antenna::setAntenna(AntennaType_E antennaType,double antGain,double hAttMax,double vAttMax,double horzTilt,double verTilt,double hBW,double vBW,double feederLoss)
{
    if(antennaType==_ANTENNA_TYPE_PARABOLIC_)
    {
        antType = _ANTENNA_TYPE_PARABOLIC_;
        gain = antGain;
        horAntmaxAttenuation = hAttMax,verAntmaxAttenuation = vAttMax;
        hTilt = horzTilt,vTilt = verTilt;
        hBeamwidth = hBW, vBeamwidth = vBW;
    }
    else if(antennaType==_ANTENNA_TYPE_OMNI_)
    {
        antType = _ANTENNA_TYPE_OMNI_;
        hPattern =  "0",vPattern = "0";
        gain = 0.0;
    }
    else 
    {
        cout<<"Unsupported antenna type.."<<endl;abort();
    }
    antennaFeederLossIndB = feederLoss;
}

void Antenna::setHAPSAntenna(int M, int N, double dv, double dh,
                              double Ge_max, double Am, double SLAv,
                              double phi3dB, double theta3dB,
                              double feederLoss)
{
    antType       = _ANTENNA_TYPE_HAPS_;
    haps_M        = M;      haps_N        = N;
    haps_dv       = dv;     haps_dh       = dh;
    haps_Ge_max   = Ge_max; haps_Am       = Am;
    haps_SLAv     = SLAv;   haps_phi3dB   = phi3dB;
    haps_theta3dB = theta3dB;
    // Tilt angles (alpha, gamma) are set later by Node::setHAPSAntennaTilts()
    haps_alpha = 0.0; haps_beta = 0.0; haps_gamma = -90.0;
    antennaFeederLossIndB = feederLoss;
    parametersUpdateStatus = false;
    gain = 0.0; // element gain is embedded in Ge_max, not gain
}

void Antenna::computeAntennaCorrMatrix(double alpha)
{
    if(numTxRUs==1)
    {
        antCorrMatrix = eye_c(1);
        return;
    }
    vec weights = 1.0*getIntegers(1,numTxRUs-1);
    weights = pow(alpha,sqr(weights))/sqr(numTxRUs-1);
    cmat oneSideMatrix = zeros_c(numTxRUs,numTxRUs);
    for(int cnt=0;cnt<numTxRUs-1;cnt++)
    {
        cvec tempRow = concat(zeros_c(cnt+1),to_cvec(weights(0,numTxRUs-cnt-2)));
        oneSideMatrix.set_row(cnt,tempRow);
    }
    antCorrMatrix = oneSideMatrix.H() + eye_c(numTxRUs) + oneSideMatrix;
}

void Antenna::setHeight(double antennaHeight, bool overwrite){
  
  if(overwrite || std::isnan(loc.z)) {loc.z=antennaHeight;parametersUpdateStatus=false;}
  else cout<<"[both:] Warning... Trying to overwrite height for antenna "<<endl;
}

void Antenna::setFloorNumber(int floorIndx,bool overwrite)
{
    if(overwrite || std::isnan(loc.z)) {loc.floorNumber=floorIndx;parametersUpdateStatus=false;}
    else cout<<"[both:] Warning... Trying to overwrite height for antenna "<<endl;
}
void Antenna::setAntennaWavelengthSpacing(double antWavelengthSpacing, bool overwrite){
  
  if(overwrite || antennaWavelengthSpacing==-1) {antennaWavelengthSpacing=antWavelengthSpacing;}
  else cout<<"[both:] Warning... Trying to overwrite antennaSpacing for antenna "<<endl;
}

void Antenna::setGain(double antGain){
  
  parametersUpdateStatus=false;
  gain=antGain;
}

void Antenna::setAperture(double radius)
{
  parametersUpdateStatus=false;
  apertureRadius = radius;
}

void Antenna::setTemperature(double temperature,double gtRatio)
{
    parametersUpdateStatus = false;
    antennaTemperature = temperature;
    GTRatio = gtRatio;
}

void Antenna::setAntennaFeederLoss(double feedberLossIndB){
  
  parametersUpdateStatus=false;
  antennaFeederLossIndB=feedberLossIndB;
}

void Antenna::setPattern(vec horPattern,vec verPattern){
  
  parametersUpdateStatus=false;
  antType=_ANTENNA_TYPE_CUSTOM_;
  hPattern=horPattern;
  vPattern=verPattern;
}

void Antenna::setPattern(AntennaType_E antennaType,vec horPattern,vec verPattern)
{
    parametersUpdateStatus=false;
    antType=antennaType;
    hPattern=horPattern;
    vPattern=verPattern;
}

void Antenna::setPattern(AntennaType_E antennaType,double horBW, double verBW){
  
  parametersUpdateStatus=false;
  antType=antennaType;
  hBeamwidth=horBW;
  vBeamwidth=verBW;
  horAntmaxAttenuation=20.0;
  verAntmaxAttenuation=20.0;
}

void Antenna::setTilt(double horTilt,double verTilt){
  
  parametersUpdateStatus=false;
  hTilt=horTilt;
  vTilt=verTilt;
}

void Antenna::setCommonMaxAttenuation(double maxAtt){
  
  parametersUpdateStatus=false;
  verAntmaxAttenuation=maxAtt;
  horAntmaxAttenuation=maxAtt;
}

void Antenna::setMaxAttenuation(double horPatternMaxAtt,double verPatternMaxAtt){
  
  parametersUpdateStatus=false;
  verAntmaxAttenuation=verPatternMaxAtt;
  horAntmaxAttenuation=horPatternMaxAtt;
}

void Antenna::setParameterUpdateStatus(bool status){
  
  parametersUpdateStatus=status;
}

void Antenna::setCrossPolarizationFlag(bool flag){

  isCrossPolarized=flag;
}

void Antenna::loadAASInfo(ivec AntStructure,ivec TxruMap,vec AntSpacing,Array<vec> ZenithAngles,Array<vec> AzimuthAngles)
{
    antStructure = AntStructure;TXRUMap=TxruMap;antSpacing = AntSpacing;
    zenithBeamAngles = ZenithAngles;azimuthBeamAngles = AzimuthAngles;
    numTxRUs = prod(TxruMap)*antStructure(0)*antStructure(1)*antStructure(4); // Mg*Ng*Mp*Np*P*F
    antElementCount = prod(antStructure); //Mg*Ng*M*N*P
}

void Antenna::setPolarisationAngles(vec crossPolarisationAngles,double coPolarisationAngle)
{
    if(antStructure.length()!=5)
    {
        cout<<"Antenna Structure not set"<<endl;abort();
    }
    if(antStructure(4)==1)
    {
        isCrossPolarized = false;
        coPolarisationSlantAngle = coPolarisationAngle;
        if(coPolarisationSlantAngle!=0)
        {
            cout<<"[Warning]: coPolarisationAngle must be zero for Single Polarisation...Setting it to zero..."<<endl;
            coPolarisationSlantAngle = 0;
        }
    }
    else
    {
        isCrossPolarized = true;
        polarizationSlantAnglesInDegree = crossPolarisationAngles;
    }
}

void Antenna::setOrientationInfo(Array<vec>& orientationPerPanel,Orientation_S nodeOrient)
{
    int nPanels = orientationPerPanel.length();
    orientationInfo.orientationPerPanel.set_length(nPanels);
    Array<vec> uniqueOrientations = findUniqueElements(orientationPerPanel);
    cout<<"orientation per pannel: "<<orientationPerPanel<<endl;
    for(int panel_cnt=0;panel_cnt<nPanels;panel_cnt++)
        orientationInfo.orientationPerPanel(panel_cnt) = Orientation_S(orientationPerPanel(panel_cnt)) + nodeOrient;
    int nOrients = uniqueOrientations.length();
    orientationInfo.uniqueOrientations.set_length(nOrients);
    orientationInfo.panelsPerUniqueOrientations.set_length(nOrients);
    for(int or_cnt=0;or_cnt<nOrients;or_cnt++)
    {
        orientationInfo.panelsPerUniqueOrientations(or_cnt)=find(orientationPerPanel,uniqueOrientations(or_cnt),true);
        orientationInfo.uniqueOrientations(or_cnt) = Orientation_S(uniqueOrientations(or_cnt)) + nodeOrient;
    }
}

void Antenna::setCrossPolarizationAngle(vec angleInDegree){

    polarizationSlantAnglesInDegree=angleInDegree;
}

void Antenna::setNumberOfPhysicalAntenna(int count){

  nPhysicalAntenna=count;
}

double Antenna::getHorizontalTilt(){
  
  return(hTilt);
}

bool Antenna::isAntennaCrossPolarized(){

  return(isCrossPolarized);
}

vec Antenna::getCrossPolarizationAngle(){

    return(polarizationSlantAnglesInDegree);
}

int Antenna::getNumberOfPhysicalAntenna(){

//   if(enableAAS)
    return(numTxRUs); // AAS
//   else
//     return(nPhysicalAntenna);
}

double Antenna::getVerticalTilt(){
  
  return(vTilt);
}


double Antenna::getAntennaFeederLoss(){
  
  return(antennaFeederLossIndB);
}

double Antenna::getGain(){
  
  return(gain);
}

double Antenna::getMaxAttenuation(){
  
  return(horAntmaxAttenuation);
}

double Antenna::getAntennaWavelengthSpacing(){
  
  return(antennaWavelengthSpacing);
}

double Antenna::getHorizontalBeamWidth(){
  
  return(hBeamwidth);
}

double Antenna::getVerticalBeamWidth(){
  
  return(vBeamwidth);
}
  
double Antenna::getParameterUpdateStatus(){
  
  return(parametersUpdateStatus);
}

double Antenna::getAntennaGain(Location_S otherNodeLoc,double carrierFreqInHz){
  
  if(loc==otherNodeLoc) 
  {
    return(0);
  }
  else
  {
    vec angle=findAngle(loc,otherNodeLoc);

    double finalGain=getAntennaGain(angle(0),angle(1),carrierFreqInHz);    
    
    return(finalGain);
  }
}

double Antenna::getAntennaGain(double hAngle, double vAngle,double carrierFreqInHz){

    int hIndx,vIndx;
    double hRes,vRes,hGain=NAN,vGain=NAN;
    
    if(antType==_ANTENNA_TYPE_CUSTOM_)
    {
        hRes=360.0/length(hPattern);
        hAngle-=hTilt;
        hIndx=round_i(rem((hAngle+360),360)/hRes)%hPattern.length();
        hGain=hPattern(hIndx);
        
        if(vPattern.length())
        {
            vRes=180.0/length(vPattern);
            vAngle-=vTilt;
            vIndx=round_i(rem(((vAngle+90)+180),180)/vRes)%vPattern.length();
            vGain=vPattern(vIndx);
        }
    }
    else if(antType==_ANTENNA_TYPE_PARABOLIC_)
    {
        if(hBeamwidth!=-1)
        {
            hAngle-=hTilt;
            if(hAngle>179) hAngle-=360; else if(hAngle<-180) hAngle+=360;
            hGain=-std::min((double)(12.0*sqr(hAngle/hBeamwidth)),horAntmaxAttenuation);
        }
        
        if(vBeamwidth!=-1)
        {
            vAngle-=vTilt;
            if(vAngle>90) vAngle-=180; else if(vAngle<-90) vAngle+=180;
            vGain=-std::min((double)(12.0*sqr(vAngle/vBeamwidth)),verAntmaxAttenuation);
        }
    }
    else if(antType==_ANTENNA_TYPE_OMNI_)
    {
        
        hGain=hPattern(0);
        vGain=vPattern(0);
    }
    else if(antType==_ANTENNA_TYPE_QUASI_ISOTROPIC_)
    {
        if(vBeamwidth!=-1)
        {
            vAngle = circmod(vAngle,360.0); if(vAngle>180.0) vAngle = 360.0 - vAngle;  //mapped to [0,180]
            vGain=-std::min((double)(12.0*sqr((vAngle-vTilt)/vBeamwidth)),verAntmaxAttenuation);
        }
    }
    
    else if(antType== _ANTENNA_TYPE_CIRCULAR_)
    {
        if(carrierFreqInHz==-1){cout<<"carrierFrequency not set for _ANTENNA_TYPE_SATELLITE_.."<<endl;abort();}
        double lamda = (double)lightSpeed/carrierFreqInHz;
        
        #ifdef _PLOT_ANTENNA_PATTERN_
        ofstream op("antennaPattern.txt",std::ios::out);
        for(double a = -90;a<90;a = a+ 0.1)
        {
            double x = 2*pi*(apertureRadius/lamda)*sin(deg_to_rad(a));
            op<<a<<",\t"<<dB(4*sqr(abs(besselj(1,x))/x)) + gain<<endl;
        }
        op.close();
        op.open("bessel.txt",std::ios::out);
        for(double x=-200*pi;x<200*pi;x = x+0.1)
        {
            op<<x<<",\t"<<besselj(1,x)<<endl;
        }
        op.close();
        cout<<"Code is performing Antenna pattern testing. Disable the  macro the _PLOT_ANTENNA_PATTERN_ to go out of the test case... "<<endl;
        // abort();
        #endif
        //cout << "hTilt and vTilt : "  << hTilt << "," << vTilt <<endl;
        //cout << "hangle and vangle : "  << hAngle << "," << vAngle <<endl;
        
        mat u1 = getCartesianVector(hTilt,vTilt,0);
        mat u2 = getCartesianVector(hAngle,vAngle,0);
        double theta = getAngle(u1,u2);
        //cout << "theta (hangle=0):" << rad_to_deg(theta) <<endl;
        // cout << "theta_hangle=0:" << rad_to_deg(theta) <<endl;
        if(theta==0.000)
            vGain = 0.0;
        else
        {
            double x = 2*pi*(apertureRadius/lamda)*sin(theta);
            vGain = dB(sqr(abs(2*besselj(1.0,x)/x)));
            
        }
    }
    else if(antType == _ANTENNA_TYPE_HAPS_)
    {
        // --- HAPS: 3GPP TR 38.901 Table 7.3-1 element pattern in LCS ---
        // GCS convention: hAngle=azimuth (deg), vAngle=zenith-based angle (deg, 90=horizon, 0=up)
        
        // ZOD_lcs is LCS theta in degrees (zenith), AOD_lcs is LCS phi in degrees (azimuth)
        double ZOD_lcs = vAngle;
        double AOD_lcs = hAngle;
        convertAngleFromGCStoLCS(ZOD_lcs, AOD_lcs, haps_alpha, haps_beta, haps_gamma);

        double theta_prime = ZOD_lcs;  // [0, 180] deg
        double phi_prime   = AOD_lcs;  // [-180, 180] deg

        // 3GPP TR 38.901 Table 7.3-1 element pattern
        // A_H: horizontal pattern
        double A_H = -std::min(12.0 * sqr(phi_prime / haps_phi3dB), haps_Am);
        // A_V: vertical pattern (theta measured from zenith; boresight at theta=90 deg)
        double A_V = -std::min(12.0 * sqr((theta_prime - 90.0) / haps_theta3dB), haps_SLAv);
        // Combined element gain
        double A_elem_dB = haps_Ge_max - std::min(-(A_H + A_V), haps_Am);

        vGain = A_elem_dB - haps_Ge_max; // subtract Ge_max here because it's added back via gain=haps_Ge_max below
        hGain = NAN;
        gain  = haps_Ge_max; // will be added by the existing finalGain logic
        horAntmaxAttenuation = -1; // disable combined max-att clipping for this type
    }
    double finalGain;
    parametersUpdateStatus=true;
    //If condition is being generated -DD
    if(!std::isnan(vGain))
    {
        if(horAntmaxAttenuation==-1)
        {
            if(!std::isnan(hGain))
                finalGain=hGain+vGain+gain;	
            else
                finalGain=vGain+gain;	
            
        }
        else
            //else condition is taking place-DD
        {
            if(!std::isnan(hGain))
                finalGain=-std::min(-(hGain+vGain),horAntmaxAttenuation)+gain;
            
            else//else condition is taking place-DD
                finalGain=vGain+gain;	
            
            
        }
        
    }  
    else
    {
        if(!std::isnan(hGain))
            finalGain=hGain+gain;	
        else
            finalGain=gain;	
        
    }
    
    finalGain-=antennaFeederLossIndB;
    //cout<<"final_gain"<<finalGain<<endl;

    return(finalGain);

}
double Antenna::getAntennaGains(double hAngle, double vAngle, double& thetaAngle, double carrierFreqInHz)
{

  int hIndx,vIndx;
  double hRes,vRes,hGain=NAN,vGain=NAN;
  if(antType==_ANTENNA_TYPE_CUSTOM_)
  {
    hRes=360.0/length(hPattern);
    hAngle-=hTilt;
    hIndx=round_i(rem((hAngle+360),360)/hRes)%hPattern.length();
    hGain=hPattern(hIndx);
    if(vPattern.length())
    {
      vRes=180.0/length(vPattern);
      vAngle-=vTilt;
      vIndx=round_i(rem(((vAngle+90)+180),180)/vRes)%vPattern.length();
      vGain=vPattern(vIndx);
    }
  }
  else if(antType==_ANTENNA_TYPE_PARABOLIC_)
  {
    if(hBeamwidth!=-1)
    {
      hAngle = circmod(hAngle,360.0); if(hAngle>180.0) hAngle = hAngle - 360.0;  //mapped to [-180 ,180]
      hGain=-std::min((double)(12.0*sqr(hAngle/hBeamwidth)),horAntmaxAttenuation);
    }
    if(vBeamwidth!=-1)
    {
      vAngle = circmod(vAngle,360.0); if(vAngle>180.0) vAngle = 360.0 - vAngle;  //mapped to [0,180]
      vGain=-std::min((double)(12.0*sqr((vAngle-vTilt)/vBeamwidth)),verAntmaxAttenuation);
    }
  }
  else if(antType==_ANTENNA_TYPE_OMNI_)
  {
    hGain=hPattern(0);
    vGain=vPattern(0);
  }
  else if(antType==_ANTENNA_TYPE_QUASI_ISOTROPIC_)
  {
            if(vBeamwidth!=-1)
            {
                vAngle = circmod(vAngle,360.0); if(vAngle>180.0) vAngle = 360.0 - vAngle;  //mapped to [0,180]
                vGain=-std::min((double)(12.0*sqr((vAngle-vTilt)/vBeamwidth)),verAntmaxAttenuation);
            }
  }

      else if(antType== _ANTENNA_TYPE_CIRCULAR_)
            {
                if(carrierFreqInHz==-1){cout<<"carrierFrequency not set for _ANTENNA_TYPE_SATELLITE_.."<<endl;abort();}
                double lamda = (double)lightSpeed/carrierFreqInHz;

                   #ifdef _PLOT_ANTENNA_PATTERN_
                   ofstream op("antennaPattern.txt",std::ios::out);
                   for(double a = -90;a<90;a = a+ 0.1)
                   {
                       double x = 2*pi*(apertureRadius/lamda)*sin(deg_to_rad(a));
                       op<<a<<",\t"<<dB(4*sqr(abs(besselj(1,x))/x)) + gain<<endl;
                   }
                   op.close();
                   op.open("bessel.txt",std::ios::out);
                   for(double x=-200*pi;x<200*pi;x = x+0.1)
                   {
                       op<<x<<",\t"<<besselj(1,x)<<endl;
                   }
                   op.close();
                   cout<<"Code is performing Antenna pattern testing. Disable the  macro the _PLOT_ANTENNA_PATTERN_ to go out of the test case... "<<endl;
                  // abort();
                   #endif
                //cout << "hTilt and vTilt : "  << hTilt << "," << vTilt <<endl;
                //cout << "hangle and vangle : "  << hAngle << "," << vAngle <<endl;

                mat u1 = getCartesianVector(hTilt,vTilt,0);
                mat u2 = getCartesianVector(hAngle,vAngle,0);
                thetaAngle = getAngle(u1,u2);
                //cout << "theta :" << thetaAngle <<endl;
                if(thetaAngle==0.000)
                    vGain = 0.0;
                else
                {
                    double x = 2*pi*(apertureRadius/lamda)*sin(thetaAngle);
                    vGain = dB(sqr(abs(2*besselj(1.0,x)/x)));

                }
            }
  else
  {
        cout<<"Unknown Antenna Type...Aborting..."<<endl;abort();
  }
  double finalGain;
  parametersUpdateStatus=true;
//   cout<<"elemental Gain :"<<gain<<endl;
//   cout<<"V Gain :"<<vGain+gain<<endl;
//   cout<<"H Gain :"<<hGain+gain<<endl;
//   cout<<"antType : "<<antType<<endl;

  //If condition is being generated -DD
  if(!std::isnan(vGain))
  {
      if(horAntmaxAttenuation==-1)
      {
          if(!std::isnan(hGain))
              finalGain=hGain+vGain+gain;
          else
              finalGain=vGain+gain;

      }
      else
        //else condition is taking place-DD
      {
          if(!std::isnan(hGain))
              finalGain=-std::min(-(hGain+vGain),horAntmaxAttenuation)+gain;

          else//else condition is taking place-DD
              finalGain=vGain+gain;


      }

  }
  else
  {
      if(!std::isnan(hGain))
          finalGain=hGain+gain;
      else
          finalGain=gain;

  }

  finalGain-=antennaFeederLossIndB;
  //cout<<"final_gain"<<finalGain<<endl;

  return(finalGain);

}

// double Antenna::get3DAntArrayGain(Location_S otherNodeLoc,double carrierFreq)
// { 
//   if(enableAAS)
//   {
//     double totalGain=0;
//     for(int k=0;k<nElementsForPort0.length();k++)
//     {
//       if(isAASEnabled)
// 	tiltAngle=TxRUtiltAngle(nElementsForPort0(k));      
//       ivec TxRUElements=TxRUToAntMapping.get_row(nElementsForPort0(k));
//       if(verAntSpacing < 0){ cout<<endl<<"Error...verAntSpacing can not be -ve pls check Antenna.cpp & configFiles."<<endl<<endl;abort();}
//       if(carrierFreq < 0){ cout<<endl<<"Error...carrierFreq can not be -ve pls check Antenna.cpp & configFiles."<<endl<<endl;abort();}
//       double vertAntElementSpacing=verAntSpacing*lightSpeed/carrierFreq;
//       vec angle=find3DAngle(loc,otherNodeLoc,vertAntElementSpacing,TxRUElements.length());
//       double AOD=angle(0);
//       double ZOD=angle(1);
//       complex< double > virtualGain=0.0; 
//       double d2rad = pi/180;
//       double combinedGain;
//       if(TxRUElements.length() > 0)
// 	for(int antElementIndx=0;antElementIndx<TxRUElements.length();antElementIndx++)
// 	{
// 	  double tempZOD = 2.0*pi*antElementIndx*verAntSpacing*sin(d2rad*ZOD);
//           double tempTilt = 2.0*pi*antElementIndx*verAntSpacing*sin(d2rad*tiltAngle);
// 	  virtualGain = virtualGain + complex< double >(cos(tempZOD),sin(tempZOD))*complex< double >(cos(tempTilt),sin(-tempTilt));	    
// 	}
//       else
//         { cout<<endl<<"Error...TxRUElements length cannot be < 0 pls check Antenna.cpp"<<endl<<endl;abort();}
//       double effectiveArraygain = 20*log10(abs(sqrt(1.0/(double)TxRUElements.length())*virtualGain)); 
//       combinedGain=getAntennaGain(AOD,ZOD) + effectiveArraygain;
//       totalGain+=combinedGain;
// 
//     }
//     
//     return totalGain;
//   }
//   else
//   {
//     cout<<"Error. 3D antenna not configured while calling get3DAntArrayGain(). "<<endl;
//     abort();
//   }
// }

double Antenna::get3DAntArrayGainForPort0(Location_S otherNodeLoc,double carrierFreq)
{
//     if(enableAAS)
    {
        double d_v = antSpacing(2),d_h = antSpacing(3);
        double vAntElemSpacingInMeters = d_v*lightSpeed/carrierFreq;
        double hAntElemSpacingInMeters = d_h*lightSpeed/carrierFreq;
        int elemPerColPerTXRU = antStructure(2)/TXRUMap(0);
        int elemPerTXRU = antElementCount/numTxRUs;  
        int elemPerRowPerTXRU = round_i(double(elemPerTXRU)/elemPerColPerTXRU);   
        vec angle=find3DAngle(loc,otherNodeLoc/*,vAntElemSpacingInMeters,hAntElemSpacingInMeters,elemPerColPerTXRU, elemPerRowPerTXRU*/);
        cout<<"loc and other nod loc : "<<loc<<"  "<<otherNodeLoc<<endl;
        //abort();
        return get3DAntArrayGainForPort0(angle(0),angle(1),carrierFreq);
        
    }
//     else 
//     {
//         cout<<"Error. 3D antenna not configured while calling get3DAntArrayGain(). "<<endl;
//         abort();
//     }
}

double Antenna::get3DAntArrayGainForPort0(double hAngle, double vAngle,double carrierFreqInHz)
{ 
    // HAPS: use per-node Euler angles stored in the antenna object itself
    if(antType == _ANTENNA_TYPE_HAPS_)
    {
        double ZOD_lcs = vAngle;
        double AOD_lcs = hAngle;
        convertAngleFromGCStoLCS(ZOD_lcs, AOD_lcs, haps_alpha, haps_beta, haps_gamma);

        double theta_lcs_rad = deg_to_rad(ZOD_lcs);
        double phi_lcs_rad   = deg_to_rad(AOD_lcs);

        // Planar array factor for M (vertical) x N (horizontal) element array
        // Phase reference: element (0,0)
        dComplex virtualGain(0.0, 0.0);
        for(int m = 0; m < haps_M; m++)
        {
            for(int n = 0; n < haps_N; n++)
            {
                // 3GPP TR 38.901 Eq. (7.3-11) projected phase
                double phase = 2.0 * pi * (m * haps_dv * cos(theta_lcs_rad)
                              + n * haps_dh * sin(theta_lcs_rad) * sin(phi_lcs_rad));
                virtualGain += expj(phase);
            }
        }
        int nElements = haps_M * haps_N;
        double effectiveArrayGain = 20.0 * log10(abs(sqrt(1.0 / double(nElements)) * virtualGain));
        double elemGain = getAntennaGain(hAngle, vAngle, carrierFreqInHz); // element gain in LCS
        double combinedGain = elemGain + effectiveArrayGain;
        cout << "[HAPS] alpha=" << haps_alpha << " gamma=" << haps_gamma
             << " arrayGain=" << effectiveArrayGain << " elemGain=" << elemGain
             << " combined=" << combinedGain << endl;
        return combinedGain;
    }

  int Mg, Ng, M, N, Mp, elemPerTXRU;
  double tiltAngle_V, tiltAngle_H,d_gv,d_gh, d_v, d_h;

//   if(enableAAS)
  {
    Mg = antStructure(0);
    Ng = antStructure(1);
    M = antStructure(2);
    N = antStructure(3);
    
    Mp = TXRUMap(0);
    elemPerTXRU = antElementCount/numTxRUs;    
    
    ivec antElemIndex = elemPerTXRU + getIntegers(0,elemPerTXRU-1,1);
    
    d_gv = antSpacing(0);
    d_gh = antSpacing(1);
    d_v = antSpacing(2);
    d_h = antSpacing(3);
    
    tiltAngle_V = zenithBeamAngles(0)(0);  // Consideing only first beam of first panel, which is wrong , but this gain is unused for SCM  - Dhiv
    
    tiltAngle_H = azimuthBeamAngles(0)(0);
    
//     if(tiltAngle_H>179) tiltAngle_H-=360; else if(tiltAngle_H<-180) tiltAngle_H+=360;
 
    int elemPerColPerTXRU = M/Mp;
    int elemPerRowPerTXRU = round_i(double(elemPerTXRU)/elemPerColPerTXRU);   
    
    double AOD=hAngle;//node angle(0)
    double ZOD=vAngle;//node angle (1)
    
   // Finding Local Co-ordinate System (LCS) location based on sector tilt
    
    dComplex virtualGain(0.0,0.0); 
    double d2rad = pi/180;
    double alpha = (antType == _ANTENNA_TYPE_CIRCULAR_) ? 0 : hTilt; // hTilt bearing Angle (sector tilt)
    double beta = 0; // downtilt
    double gamma = 0; // Slant angle
    
    convertAngleFromGCStoLCS(ZOD,AOD,alpha,beta,gamma);

    double combinedGain;
    
    sTXRUParameters txruParameters = getTXRUParameters(carrierFreqInHz);//NOTE: carrierFrequency of 6 GHz is used as the phase shifts are independent of carrierFrequency.
    vec tempAngles = getPhaseShiftPerElement(&txruParameters,ZOD,AOD);
    vec tempTilts = getPhaseShiftPerElement(&txruParameters,tiltAngle_V,tiltAngle_H);
    virtualGain = sum(expj(tempAngles-tempTilts));
    
//     for(int antElementIndxInCol=0;antElementIndxInCol<elemPerColPerTXRU;antElementIndxInCol++)
//       for (int antElementIndexInRow = 0;antElementIndexInRow<elemPerRowPerTXRU;antElementIndexInRow++)
//       {
//         double tempAng = 2.0*pi*(antElementIndxInCol*d_v*cos(d2rad*ZOD) + antElementIndexInRow*d_h*sin(d2rad*ZOD)*sin(d2rad*AOD)) ;
//         double tempTilt = 2.0*pi*(antElementIndxInCol*d_v*sin(d2rad*tiltAngle_V) - antElementIndexInRow*d_h*cos(d2rad*tiltAngle_V)*sin(d2rad*tiltAngle_H));
//         virtualGain = virtualGain + expj(tempAng+tempTilt);
//       }
    
    double effectiveArraygain =  20*log10(abs(sqrt(1.0/double(elemPerTXRU))*virtualGain));
    combinedGain = getAntennaGain(AOD,ZOD,carrierFreqInHz) + effectiveArraygain;
    cout << "effectiveArraygain: " << effectiveArraygain <<endl;
    cout << "combinedGain: " << combinedGain << "AOD :" << AOD << "ZOD : " << ZOD <<endl; 
    //abort();
    //cout << "hAngle, double vAngle,double carrierFreqInHz: " << hAngle << ","<< vAngle<< ","<< carrierFreqInHz<<endl; 
    return combinedGain;
  }
//   else
//   {
//     cout<<"Error. 3D antenna not configured while calling get3DAntArrayGain(). "<<endl;
//     abort();
//   }
}

// double Antenna::get3DAntArrayGainForTilt(Location_S otherNodeLoc,double carrierFreq, eTilt electricalTilt)
// { 
//   int Mg, Ng, M, N, Mp, elemPerTXRU;
//   double tiltAngle_V, tiltAngle_H,d_gv,d_gh, d_v, d_h;
//   
//   if(enableAAS)
//   {
//     Mg = antStructure(0);
//     Ng = antStructure(1);
//     M = antStructure(2);
//     N = antStructure(3);
//     Mp =TXRUMap(2);
// 
//     elemPerTXRU = antElementCount/numTxRUs;   
//       
//     ivec antElemIndex = elemPerTXRU + getIntegers(0,elemPerTXRU-1,1);
//     
//     d_gv = antSpacing(0);
//     d_gh = antSpacing(1);
//     d_v = antSpacing(2);
//     d_h = antSpacing(3);
//     
//     tiltAngle_V = electricalTilt.verticalTilt;  
//     tiltAngle_H = electricalTilt.horizontalTilt;  
// //     if(tiltAngle_V>90) tiltAngle_V-=90; else if(tiltAngle_V<90) tiltAngle_V-=90;
//     if(tiltAngle_H>179) tiltAngle_H-=360; else if(tiltAngle_H<-180) tiltAngle_H+=360;
// 
// 
//     double vAntElemSpacingInMeters = d_v*lightSpeed/carrierFreq;
//     double hAntElemSpacingInMeters = d_h*lightSpeed/carrierFreq;
//     
//     int elemPerColPerTXRU = M/Mp;
//     int elemPerRowPerTXRU = round_i(double(elemPerTXRU)/elemPerColPerTXRU);
// 
//     vec angle=find3DAngle(loc,otherNodeLoc,vAntElemSpacingInMeters,hAntElemSpacingInMeters,elemPerColPerTXRU, elemPerRowPerTXRU,hTilt);
//     double AOD=angle(0);
//     double ZOD=angle(1);
// 
//     
//     complex< double > virtualGain=0.0; 
//     double d2rad = pi/180;
//     double combinedGain;      
//     
//     for(int antElementIndxInCol=0;antElementIndxInCol<elemPerColPerTXRU;antElementIndxInCol++)
//     {
//       for (int antElementIndexInRow = 0;antElementIndexInRow<elemPerRowPerTXRU;antElementIndexInRow++)
//       {
//         double tempAng = -2.0*pi*(antElementIndxInCol*d_v*cos(d2rad*ZOD) + antElementIndexInRow*d_h*sin(d2rad*ZOD)*sin(d2rad*AOD)) ;
//         double tempTilt = 2.0*pi*(antElementIndxInCol*d_v*sin(d2rad*tiltAngle_V) + antElementIndexInRow*d_h*cos(d2rad*tiltAngle_V)*sin(d2rad*tiltAngle_H));
//         virtualGain = virtualGain + complex< double >(cos(tempAng),sin(tempAng))*complex< double >(cos(tempTilt),sin(tempTilt));	    
//       }
//     }
//     double effectiveArraygain = 20*log10(abs(sqrt(1.0/double(elemPerTXRU))*virtualGain)); 
//     combinedGain = getAntennaGain(AOD,ZOD) + effectiveArraygain;
//     
//     return combinedGain;
//   }
//   else
//   {
//     cout<<"Error. 3D antenna not configured while calling get3DAntArrayGain(). "<<endl;
//     abort();
//   }
// }

// double Antenna::get3DAntArrayGainForPort0Test(double AOD, double ZOD,double carrierFreq)
// {
//   int Mg, Ng, M,N,Mp,TXRUsPerPanal, elemPerTXRU;
//   double tiltAngle_V, tiltAngle_H,d_gv,d_gh, d_v, d_h;
//   if(enableAAS)
//   {
// 
//       Mg = antStructure(0);
//       Ng = antStructure(1);
//       M = antStructure(2);
//       N = antStructure(3);
//       
//       Mp = TXRUMap(0);
// 
//       TXRUsPerPanal = numTxRUs/(Mg*Ng);      
//       elemPerTXRU = antElementCount/numTxRUs;  
// 
//       ivec antElemIndex = getIntegers(0,elemPerTXRU-1,1);
//       
//       d_gv = antSpacing(0);
//       d_gh = antSpacing(1);
//       d_v = antSpacing(2);
//       d_h = antSpacing(3);
//       
//       tiltAngle_V = zenithBeamAngles(0);  
//       tiltAngle_H = azimuthBeamAngles(0);        
//       if(tiltAngle_V>90) tiltAngle_V-=90; else if(tiltAngle_V<90) tiltAngle_V-=90;
//       if(tiltAngle_H>179) tiltAngle_H-=360; else if(tiltAngle_H<-180) tiltAngle_H+=360;
//       
//       double vAntElemSpacingInMeters = d_v*lightSpeed/carrierFreq;
//       double hAntElemSpacingInMeters = d_h*lightSpeed/carrierFreq;
//       double vAntPanalSpacingInMeters = d_gv*lightSpeed/carrierFreq;
//       double hAntPanalSpacingInMeters = d_gh*lightSpeed/carrierFreq;
//       int elemPerColPerTXRU = M/Mp;
//       int elemPerRowPerTXRU = round_i(double(elemPerTXRU)/elemPerColPerTXRU);      
// 
//       complex< double > virtualGain=0.0; 
//       double d2rad = pi/180;
//       double combinedGain;
// 
//       for(int antElementIndxInCol=0;antElementIndxInCol<elemPerColPerTXRU;antElementIndxInCol++)
//       {
//         for (int antElementIndexInRow = 0;antElementIndexInRow<elemPerRowPerTXRU;antElementIndexInRow++)
//         {
//           double tempAng = 2.0*pi*(antElementIndxInCol*d_v*sin(d2rad*ZOD) + antElementIndexInRow*d_h*cos(d2rad*ZOD)*sin(d2rad*AOD)) ;
//           double tempTilt = 2.0*pi*(antElementIndxInCol*d_v*sin(d2rad*tiltAngle_V) - antElementIndexInRow*d_h*cos(d2rad*tiltAngle_V)*sin(d2rad*tiltAngle_H));
//           virtualGain = virtualGain + (complex< double >(cos(tempAng),sin(tempAng))*complex< double >(cos(tempTilt),-sin(tempTilt)));	    
//         }
//       }
// 
//       double effectiveArraygain = 20*log10(abs(sqrt(1.0/double(elemPerTXRU))*virtualGain)); 
//       combinedGain = getAntennaGain(AOD,ZOD) + effectiveArraygain;
//     
//       return combinedGain;
//   }
//   else
//   {
//     cout<<"Error. 3D antenna not configured while calling get3DAntArrayGain(). "<<endl;
//     abort();
//   }
// }

void Antenna::printAntennaInfo(ostream &os,double lamda)
{
    os<<"carrierFrequency: "<<lightSpeed*1e-9/lamda<<" GHz"<<endl;
    os<<"[Mg,Ng,M,N,P]: "<<antStructure<<" \t [Mp,Np,F]: "<<TXRUMap<<" \t [dpv,dph,dv,dh]: "<<antSpacing*lamda<<endl;
    os<<"\nantType: "<<AntennaType_Str[antType]<<", elementGainIndB: "<<gain<<", hBW: "<<hBeamwidth<<", vBW: "<<vBeamwidth<<", hTilt: "<<hTilt<<", vTilt: "<<vTilt;
    os<<"\thPattern: "<<hPattern<<", vPattern: "<<vPattern<<", hAtt: "<<horAntmaxAttenuation<<", vAtt: "<<verAntmaxAttenuation<<", FeederLoss: "<<antennaFeederLossIndB<<endl;
}
void Antenna::printAntennaStructure(ostream& os)
{
    int Mg = antStructure(0),Ng = antStructure(1), M = antStructure(2), N = antStructure(3), P = antStructure(4);
    
    ivec Sizes(5),params(5);
    Sizes(0)=Ng;Sizes(1)=Mg;Sizes(2)=N;Sizes(3)=M;Sizes(4)=P;
    for(int mg=Mg-1;mg>=0;mg--)
    {
        params(1)=mg;
        for(int m=M-1;m>=0;m--)
        {
            params(3)=m;
            for(int ng=0;ng<Ng;ng++)
            {
                params(0)=ng;
                for(int n=0;n<N;n++)
                {
                    params(2)=n;params(4)=0;
                    int elementCnt = decode(Sizes,params);
                    os<<"("<<elementCnt;
                    if(P==2)
                    {
                        os<<","<<elementCnt+1;
                    }
                    os<<")  "; 
                }
                os<<"\t\t";
            }
            os<<"\n";
        }
        if(mg!=0)
            os<<"\n\n\n";
    }
}
AntennaType_E getAntennaType(string antennaType)
{
    for(int i=0;i<AntennaType_Str->size();i++)
    {
        if(antennaType==AntennaType_Str[i])
            return AntennaType_E(i);
    }
    cout<<"Invalid antennaType "<<antennaType<<endl;
    abort();
}
