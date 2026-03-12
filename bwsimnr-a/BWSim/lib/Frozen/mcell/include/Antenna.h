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

#ifndef _ANT_H_
#define _ANT_H_

#include "SupportingFunctions.h"
#include "BeamStructAndEnums.h"

/**
* @brief Antenna Class which manages the antenna pattern and computes antennaGain between two node Locations 
*/
class Antenna{
	
  protected:
	bool parametersUpdateStatus;
	double antennaWavelengthSpacing;
	double gain;
	vec hPattern,vPattern;
	double hTilt,vTilt; // This are sector tilts (60,180,300 for hTilt)
	double verAntmaxAttenuation;
	double horAntmaxAttenuation;
	Location_S loc; 
	AntennaType_E antType;
    cmat antCorrMatrix;
    double antennaFeederLossIndB;
	double hBeamwidth,vBeamwidth;
    int nPhysicalAntenna;
    bool isCrossPolarized;
    vec polarizationSlantAnglesInDegree;
	double apertureRadius;
    double GTRatio;
    double antennaTemperature;
    // HAPS antenna parameters (3GPP TR 38.901 element pattern + planar array)
    double haps_Ge_max;    // Element max gain (dBi)
    double haps_Am;        // Horiz max attenuation (dB)
    double haps_SLAv;      // Vert max attenuation (dB)
    double haps_phi3dB;    // Horiz 3dB beamwidth (deg)
    double haps_theta3dB;  // Vert 3dB beamwidth (deg)
    int    haps_M;         // Number of rows (vertical)
    int    haps_N;         // Number of columns (horizontal)
    double haps_dv;        // Vertical element spacing (wavelengths)
    double haps_dh;        // Horizontal element spacing (wavelengths)
	
  public:
        //3D Related
	//ivec nTXRUsForPort0;
	int antElementCount;
	ivec antStructure; //[Mg Ng M N P]
	vec antSpacing;    // [d_gv d_gh d_v d_h]
	int numTxRUs;      // Total Number ofTXRUs
	Array<vec> zenithBeamAngles;
	Array<vec> azimuthBeamAngles;
	ivec TXRUMap;  // [Mp Np]
    double coPolarisationSlantAngle;
    OrientationInfo_S orientationInfo;
    bool isForHexNode;
    // HAPS: Euler tilt angles (GCS-to-LCS rotation, public so Node.cpp can set per-beam)
    double haps_alpha;  // Bearing around Z (deg)
    double haps_beta;   // Downtilt around Y (deg), always 0
    double haps_gamma;  // Slant/mechanical tilt around X (deg)
    
	Antenna();
	bool isAntennaReady();	///< Returns true if the antenna parameters are set properly
	void setAntennaFeederLoss(double feedberLossIndB); ///< Sets Antenna Feeder loss
	void setAntennaType(AntennaType_E antennaType); ///< Sets Antenna Type
	void setLoc(Location_S antLoc, bool overwrite=false);	///< Sets Location of the Antenna
	void setHeight(double antennaHeight, bool overwrite=false);	///< Sets antenna Height
    void setFloorNumber(int floorIndx,bool overwrite=false);
	void setAntennaWavelengthSpacing(double antWavelengthSpacing, bool overwrite=false);	///< Sets antenna wavelength spacing (used only for printing in files)
	void setGain(double antGain);	///< Set the Gain of the antenna
    void setPattern(AntennaType_E antennaType,vec horPattern,vec verPattern="");
	void setPattern(AntennaType_E,double horBW ,double verBW=-1);	///< Set the pattern for the antenna
	void setPattern(vec horPattern,vec verPattern="");	///< Set the pattern for the antenna from the given vectors
	void setTilt(double horTilt,double verTilt=NAN);	///< Set horizontal and vertical fo the antenna
	void setCommonMaxAttenuation(double maxAtt);	///< Set maximum Attenuation of the antenna
	void setMaxAttenuation(double horPatternMaxAtt,double verPatternMaxAtt=NAN);	///< Set maximum Attenuation of the antenna
	void setParameterUpdateStatus(bool status);	///< Set ParameterUpdateStatus to true
    void setCrossPolarizationFlag(bool flag);
    void setCrossPolarizationAngle(vec angleInDegree);
    void setNumberOfPhysicalAntenna(int count);
    void setAperture(double radius);
    
	double getAntennaFeederLoss(); ///< Gets Antenna Feeder loss
	double getHorizontalTilt();	///< Returns horizontal tilt of antenna (set in case of directional antenna)
	double getVerticalTilt();	///< Returns vertical tilt of antenna (set in case of directional antenna with 3-D pattern)
	double getGain();		///< Returns gain of the antenna in dB
	double getMaxAttenuation();	///< Returns maximum attenuation of antenna pattern (used in case of parabolic antenna)
	double getAntennaWavelengthSpacing(); ///< Returns antenna spacing in multiples of wavelength
	double getHorizontalBeamWidth();	///< Returns Horizontal Beam width of the parabolic antenna 
	double getVerticalBeamWidth();	///< Returns Vertical Beam width of the parabolic antenna 
	double getParameterUpdateStatus(); ///< Returns parameter update status
	double getAntennaGain(Location_S otherNodeLoc,double carrierFreqInHz=-1);	///< Returns the antenna gain to the given location from the antenna
	double getAntennaGain(double hAngle, double vAngle,double carrierFreqInHz=-1);	///< Returns the antenna gain in the given angle from the antenna	
	double getAntennaGains(double hAngle, double vAngle,double& thetaAngle,double carrierFreqInHz=-1);
	double get3DAntArrayGainForPort0(Location_S otherNodeLoc,double carrierFreq);
	double get3DAntArrayGainForPort0(double hAngle, double vAngle,double carrierFreqInHz=-1);

	//test function
	//double get3DAntArrayGainForPort0Test(double AOD, double ZOD,double carrierFreq);
    
    bool isAntennaCrossPolarized();
    vec getCrossPolarizationAngle();
    int getNumberOfPhysicalAntenna();
	Location_S getLoc(){return loc;};
    void print()
    {
        cout<<"antType: "<<AntennaType_Str[antType]<<", gain: "<<gain<<", hBW: "<<hBeamwidth<<", vBW: "<<vBeamwidth<<", hTilt: "<<hTilt<<", vTilt: "<<vTilt<<endl;
        cout<<"hPattern: "<<hPattern<<", vPattern: "<<vPattern<<", hAtt: "<<horAntmaxAttenuation<<", vAtt: "<<verAntmaxAttenuation<<", FeederLoss: "<<antennaFeederLossIndB<<endl;
    }
    void printAntennaInfo(ostream &os,double lamda);
    void printAntennaStructure(ostream& os);
    void loadAASInfo(ivec AntStructure,ivec TxruMap,vec AntSpacing,Array<vec> ZenithAngles,Array<vec> AzimuthAngles);
    void setPolarisationAngles(vec crossPolarisationAngles,double coPolarisationAngle);
    void setOrientationInfo(Array<vec>& orientationPerPanel,Orientation_S nodeOrient);
    sTXRUParameters getTXRUParameters(double carrierFrequency);
    sBeamParameters getBeamParameters(double carrierFrequency);
    void setTemperature(double temperature,double gtRatio);
    double getTemperature(){return antennaTemperature;}
    vec getAntennaTilts(){return getVectorWithNumbers(2,hTilt,vTilt);}
    AntennaType_E getAntennaType(){return antType;}
    double getGTRatioInDB(){return GTRatio;}
// 	void getAntennaGainForTxRU(imat antArray,int TxRUIndx,double tiltAngle);
    
    void setAntenna(AntennaType_E antennaType,double antGain,double hAttMax,double vAttMax,double horzTilt,double verTilt,double hBW,double vBW,double feederLoss);
    void setHAPSAntenna(int M, int N, double dv, double dh,
                        double Ge_max, double Am, double SLAv,
                        double phi3dB, double theta3dB,
                        double feederLoss);
    void computeAntennaCorrMatrix(double alpha);
    cmat getAntennaCorrMatrix(){return antCorrMatrix;}
    
    
	
};
AntennaType_E getAntennaType(string antennaType);
#endif


