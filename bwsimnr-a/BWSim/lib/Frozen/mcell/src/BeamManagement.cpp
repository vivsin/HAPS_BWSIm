/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2014 CEWiT, India
 *
 * \ NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */


#include "../include/BeamStructAndEnums.h"

sBeamParameters::sBeamParameters(){
  
  isInitialized = false;
  numberOfAzimuthAngles = 0;
  numberOfZenithAngles = 0;
  nTotalBeams = 0;
  electricalTilts.set_length(0);
  weightsPerTilts.set_length(0);
    txRUPerBeamForAnalogBeamSweeping.set_length(0);
  
}

void sBeamParameters::configureBeamParameters(ivec& antStructure,ivec& txruMap, vec& antSpacing, Array<vec>& azimuthAngles, Array<vec>& zenithAngles, double carrierFrequencyInHz,OrientationInfo_S orientationInfo)
{
    TXRUparameters.initTXRUParameters(carrierFrequencyInHz,antStructure,txruMap,antSpacing);
    TXRUparameters.formTXRUmodel();
    TXRUparameters.setOrientations(orientationInfo);
    setBeamTiltsAndWeights(azimuthAngles, zenithAngles);
    setBeamParameters(azimuthAngles,zenithAngles);
}

void sBeamParameters::setBeamParameters(Array<vec> azimuthAngles, Array<vec> zenithAngles){
  
    
    txRUPerBeamForAnalogBeamSweeping=zeros_i(nTotalBeams);
    txRUsPerBeam.set_size(nTotalBeams);
    
    if(azimuthAngles.length()!=1) // dedicated beams for each panel
    {
        int beamIndx = 0;
        for(int panel_cnt=0;panel_cnt<azimuthAngles.length();panel_cnt++)
        {
            int defaultTxRU = TXRUparameters.TXRUsPerPanel(panel_cnt,0); // first txRU of each Panel is considered as defaultTxRU for a dedicated beam   - Dhiv
            ivec txRUIndices = TXRUparameters.TXRUsPerPanel.get_row(panel_cnt);
            int dedicatedBeamsInThePanel = azimuthAngles(panel_cnt).length()*zenithAngles(panel_cnt).length();
            ivec beamIndicesToSet = getIntegers(0,dedicatedBeamsInThePanel-1)+beamIndx;
            
            setInVec(txRUPerBeamForAnalogBeamSweeping,beamIndicesToSet,defaultTxRU);
            setInArray(txRUsPerBeam,beamIndicesToSet,txRUIndices);
            
            beamIndx+=dedicatedBeamsInThePanel;
        }
    }
    else
    {
        ivec txRUIndices = getIntegers(0,TXRUparameters.numberOfTXRU-1);
        for(int beam_cnt=0;beam_cnt<nTotalBeams;beam_cnt++)
            txRUsPerBeam(beam_cnt) = txRUIndices ; 
    }

    isInitialized = true;
}



void sBeamParameters::setBeamTiltsAndWeights(Array<vec> &azimuthAngles, Array<vec> &zenithAngles){
    
    
    if(azimuthAngles.length()!=1 || zenithAngles.length()!=1)
    {
        if(azimuthAngles.length()==TXRUparameters.nPanels && zenithAngles.length()==1)
        {
            vec zenAngels = zenithAngles(0);
            for(int cnt=1;cnt<(azimuthAngles.length());cnt++)
                append(zenithAngles,zenAngels);
        }
        
        if(zenithAngles.length()==TXRUparameters.nPanels && azimuthAngles.length()==1)
        {
            
            vec azAngels = azimuthAngles(0);
            for(int cnt=1;cnt<(zenithAngles.length()-1);cnt++)
                append(azimuthAngles,azAngels);
        }
    }
    
    
    sBeamParameters beamParametersTemp=*this;
    for(int panel_cnt=0; panel_cnt<azimuthAngles.length(); panel_cnt++)
    {
        beamParametersTemp.setBeamTiltsAndWeights(azimuthAngles(panel_cnt),zenithAngles(panel_cnt));
        numberOfAzimuthAngles += beamParametersTemp.numberOfAzimuthAngles;
        numberOfZenithAngles += beamParametersTemp.numberOfZenithAngles;
        nTotalBeams += beamParametersTemp.nTotalBeams;
        append(electricalTilts,beamParametersTemp.electricalTilts);
        append(weightsPerTilts,beamParametersTemp.weightsPerTilts);
    }
}

void sBeamParameters::setBeamTiltsAndWeights(vec azimuthAngles, vec zenithAngles){
  
    numberOfAzimuthAngles = azimuthAngles.length();
    numberOfZenithAngles = zenithAngles.length();
    nTotalBeams = numberOfAzimuthAngles * numberOfZenithAngles;
    electricalTilts.set_length(nTotalBeams);
    weightsPerTilts.set_length(nTotalBeams);
    
    double deg2rad = pi/180.0; 
    double lamda = TXRUparameters.lamda;
    if(numberOfAzimuthAngles>1 && TXRUparameters.numberOfColsPerTXRU==1)
    {
        cout<<"Number of Horizontal Elements per TXRU must be greater than 1 for horizontal beamforming"<<endl;abort();
    }
    if(numberOfZenithAngles>1 && TXRUparameters.numberOfRowsPerTXRU==1)
    {
        cout<<"Number of Vertical Elements per TXRU must be greater than 1 for vertical beamforming"<<endl;abort();
    }
    
    //Refer section 5.4.4.1.3 in 3GPP TR 37.840  
    
    for(int hCnt = 0,cnt=0; hCnt < numberOfAzimuthAngles; hCnt++)
    {
        for(int vCnt = 0; vCnt < numberOfZenithAngles; vCnt++,cnt++)
        {
            electricalTilts(cnt).horizontalTilt = azimuthAngles(hCnt);
            electricalTilts(cnt).verticalTilt = zenithAngles(vCnt);
            
            weightsPerTilts(cnt) = expj(-1*getPhaseShiftPerElement(&TXRUparameters,electricalTilts(cnt).verticalTilt,electricalTilts(cnt).horizontalTilt));
        
            weightsPerTilts(cnt) *= (1.0/sqrt(TXRUparameters.numberOfElementsPerTXRU));
        }
    }
}


cvec sBeamParameters::getBeamformingVector(int beamID){
  
  if(beamID == -1 || weightsPerTilts.size() <= beamID )
  {
    cout<<"Unsupported beamID in getBeamformingVector()"<<beamID<<endl;abort();
  }
  
  return weightsPerTilts(beamID);
}

void sBeamParameters::printOrientations(ostream& os)
{
    if(TXRUparameters.nOrientations>1)
    {
        os<<"\norientationIDperPanel:"<<endl;
        for(int i=0;i<TXRUparameters.orientationIDPerPanel.length();i++)
        {
            os<<"{"<<i<<","<<TXRUparameters.orientationIDPerPanel(i)<<"}\t";
        }
        os<<endl;
    }
}

sTXRUParameters::sTXRUParameters(double carrierFrequencyInHz, ivec antennaStructure, ivec txruMap, vec antennaSpacing,OrientationInfo_S orientationInfo)
{
    initTXRUParameters(carrierFrequencyInHz,antennaStructure,txruMap,antennaSpacing);
    formTXRUmodel();
    setOrientations(orientationInfo);
}


sTXRUParameters::sTXRUParameters(){
  
  numberOfTXRU = -1;
  numberOfRowsPerTXRU = -1;
  numberOfColsPerTXRU = -1;
  numberOfElementsPerTXRU = -1;
  
  Mg = -1;
  Ng = -1;
  M = -1;
  N = -1;
  P = -1;
  
  Mp = -1;
  Np = -1;
  
  dpv = -1.0;
  dph = -1.0;
  dv = -1.0;
  dh = -1.0;
  
  lamda = -1.0;
  nPanels=-1;
  
  orientationIDPerPanel.set_length(0);
  
}

void sTXRUParameters::setOrientations(OrientationInfo_S orientationInfo)
{
    nOrientations=orientationInfo.uniqueOrientations.length();
    if(nOrientations>Mg*Ng)
    {
        cout<<"nOrientations cannot be greater than nPanels..."<<endl;abort();
    }
    orientationIDPerPanel=zeros_i(nPanels);
    orientationIDperTXRU=zeros_i(numberOfTXRU);
    for(int or_cnt=0;or_cnt<orientationInfo.uniqueOrientations.length();or_cnt++)
    {
        for(int cnt=0;cnt<orientationInfo.panelsPerUniqueOrientations(or_cnt).length();cnt++)
        {
            orientationIDPerPanel(orientationInfo.panelsPerUniqueOrientations(or_cnt)(cnt)) = or_cnt;
            ivec txRUs = TXRUsPerPanel.get_row(orientationInfo.panelsPerUniqueOrientations(or_cnt)(cnt));
            setInVec(orientationIDperTXRU,txRUs,or_cnt);
        }
    }
}

void sTXRUParameters::printTXRUStructure(ostream& os)
{
    int F=1;
    ivec Sizes(5),params(5);
    Sizes(0)=Ng;Sizes(1)=Mg;Sizes(2)=P;Sizes(3)=Np;Sizes(4)=Mp;
    for(int mg=Mg-1;mg>=0;mg--)
    {
        params(1)=mg;
        for(int p=P-1;p>=0;p--)
        {
            params(2)=p;
            for(int m=Mp-1;m>=0;m--)
            {
                params(4)=m;
                for(int ng=0;ng<Ng;ng++)
                {
                    params(0)=ng;
                    for(int n=0;n<Np;n++)
                    {
                        params(3)=n;
                        int txruCnt = decode(Sizes,params);
                        os<<"{"<<txruCnt+getIntegers(0,F-1)*(numberOfTXRU/F)<<","<<antennaElementsPerTXRU(txruCnt)<<"}"<<setw(4);
                    }
                    os<<"\t\t\t";
                }
                os<<"\n";
            }
            if(!(mg==0 && p==0))
            os<<"\n";
        }
        if(mg!=0)
        os<<"\n\n";
    }
}

void sTXRUParameters::initTXRUParameters(double carrierFrequencyInHz, ivec antennaStructure, ivec txruMap, vec antennaSpacing){
  
  lamda = lightSpeed/carrierFrequencyInHz;
  
  if(antennaStructure.length() != 5 || antennaSpacing.length() != 4 )
  {
    cout<<"[both:]Incorrect size of either antennaStructure or antennaSpacing"<<endl;abort();
  }
  
  Mg = antennaStructure(0);
  Ng = antennaStructure(1);
  M = antennaStructure(2);
  N = antennaStructure(3);
  P = antennaStructure(4);
  
  nTotalElements = Mg * Ng * M * N * P;
  nPanels = Mg*Ng;
  
  Mp = txruMap(0);
  Np = txruMap(1);
  
  dpv = antennaSpacing(0)*lamda;
  dph = antennaSpacing(1)*lamda;
  dv = antennaSpacing(2)*lamda;
  dh = antennaSpacing(3)*lamda;
  
  panelN1=Np*Ng;
  panelN2=Mp*Mg;
  
  numberOfRowsPerTXRU = M/Mp;
  numberOfColsPerTXRU = N/Np;
  numberOfElementsPerTXRU = numberOfRowsPerTXRU * numberOfColsPerTXRU;
  numberOfTXRU = (nTotalElements) / numberOfElementsPerTXRU;
  antennaElementsPerTXRU.set_length(0);
}

void sTXRUParameters::formTXRUmodel()
{
  antennaElementsPerTXRU.set_length(numberOfTXRU);
  
  // assigning TxRU elements
  
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
  
  int nTXRUsPerPanel = numberOfTXRU/nPanels;
  ivec TXRUs = getIntegers(0,numberOfTXRU-1);
  TXRUsPerPanel = reshape(TXRUs,nPanels,nTXRUsPerPanel);
  
  cmat panelMap = zeros_c(M*Mg,N*Ng);
  
  for(int p_col_cnt =0,cnt=0;p_col_cnt<Ng;p_col_cnt++)
      for(int p_row_cnt =0;p_row_cnt<Mg;p_row_cnt++)
        for(int col_cnt = 0; col_cnt < N; col_cnt++)
            for(int row_cnt = 0; row_cnt < M; row_cnt++)
            {
                panelMap(p_row_cnt*M+row_cnt,p_col_cnt*N+col_cnt) = complex<double> (cnt,cnt+P-1.0);
                cnt = cnt + P;
            }
  
          
    for(int p_col_cnt =0, txru_cnt = 0;p_col_cnt<Ng;p_col_cnt++)
        for(int p_row_cnt =0;p_row_cnt<Mg;p_row_cnt++)
            for(int p = 0; p < P; p++)
                for(int col_cnt = 0; col_cnt < N; col_cnt=col_cnt + numberOfColsPerTXRU)
                    for(int row_cnt = 0; row_cnt < M; row_cnt = row_cnt + numberOfRowsPerTXRU, txru_cnt++)
                    {
                        int row = p_row_cnt*M + row_cnt;
                        int col = p_col_cnt*N + col_cnt;
                        if(p==0)
                            antennaElementsPerTXRU(txru_cnt)   = to_ivec(cvectorize(real(panelMap.get(row, row + numberOfRowsPerTXRU - 1, col, col + numberOfColsPerTXRU - 1))));
                        else
                            antennaElementsPerTXRU(txru_cnt)   = to_ivec(cvectorize(imag(panelMap.get(row, row + numberOfRowsPerTXRU - 1, col, col + numberOfColsPerTXRU - 1))));
                    }

}

void sTXRUParameters::setTXRUParameters(int numOfTXRU, double tVerAntElementSpacing, double tHorzAntElementSpacing, int numRowsPerTXRU, int numElemPerTXRU){
  
  numberOfTXRU = numOfTXRU;
  dv = tVerAntElementSpacing;
  dh = tHorzAntElementSpacing;
  numberOfRowsPerTXRU = numRowsPerTXRU;
  numberOfElementsPerTXRU = numElemPerTXRU;
  numberOfColsPerTXRU = floor_i(numElemPerTXRU/numRowsPerTXRU);
}

ivec sTXRUParameters::getTXRUAntennaElements(int TXRUid){
  
  return antennaElementsPerTXRU(TXRUid);
}

void sTXRUParameters::modifyTXRUs(imat TXRUsPerPanel)
{
    Array<ivec> nu;
    parse("./configFiles/mySysConfig.txt","nodeNumerologies",nu);
    if(nu(0).length()>1){return;} //multiple nodeNumerologies case...
    Array<string> nuFolders;
    ivec nuIDs;
    parse("./configFiles/myNUConfig.txt","configFileFolder",nuFolders);
    parse("./configFiles/myNUConfig.txt","nuIDs",nuIDs);
    int nuIndx = find(nuIDs,nu(0)(0));
    ivec NgN1N2P;
    string dlTxConfigFile = "./configFiles/"+nuFolders(nuIndx)+"/myDLTxRxConfig.txt";
    parse(dlTxConfigFile,"NgN1N2P",NgN1N2P);
    if(NgN1N2P(0)==Mg*Ng || Mg*Ng==1){return;}
    
    ivec txRUs(0);
    for(int p=0;p<P;p++)
        for(int ng=0;ng<Ng;ng++)
            for(int np=0;np<Np;np++)
                for(int mg=0;mg<Mg;mg++)
                    for(int mp=0;mp<Mp;mp++)
                    {
                        append(txRUs,TXRUsPerPanel(mg+ng*Mg,p*Mp*Np+mp+np*Mp));
                    }
    Array<ivec> antennaElements = antennaElementsPerTXRU;
    for(int txru_cnt=0;txru_cnt<txRUs.length();txru_cnt++)
    {
        antennaElementsPerTXRU(txru_cnt) = antennaElements(txRUs(txru_cnt));
    }
}

vec getPhaseShiftPerElement(sTXRUParameters* txruParameters,double ZOD,double AOD,bool isInRadian)
{
    vec phasePerElement(txruParameters->numberOfElementsPerTXRU);
    
    if(!isInRadian)
    {
        double deg2rad = pi/180.0;
        ZOD *= deg2rad;AOD *= deg2rad;
    }
    //Refer section 5.4.4.1.3 in 3GPP TR 37.840  
    double vTiltRad = 2*pi*(txruParameters->dv/txruParameters->lamda)*cos(ZOD);
    double hTiltRad = 2*pi*(txruParameters->dh/txruParameters->lamda)*sin(ZOD)*sin(AOD);
    
    for(int colIndx=0,antCnt=0;colIndx<txruParameters->numberOfColsPerTXRU;colIndx++)
        for(int rowIndx=0;rowIndx<txruParameters->numberOfRowsPerTXRU;rowIndx++,antCnt++)
        { 
            phasePerElement(antCnt) = vTiltRad*rowIndx+hTiltRad*colIndx;
        }
    return phasePerElement;
}


