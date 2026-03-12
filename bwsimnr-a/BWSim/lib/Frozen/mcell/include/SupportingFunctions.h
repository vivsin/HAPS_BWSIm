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

#ifndef _SF_MC_H_
#define _SF_MC_H_

#include "StructsAndEnums.h"

extern string DropMethod_Str[4];
extern string TxRxMode_Str[2];
extern string AntennaType_Str[6];
extern string CellLayout_Str[3];
extern string ShadowType_Str[3];

bool isInsideHexCell(double x, double y, Location_S hexCellLoc, double cellISD, double minDistFromCenter=0);
Array <Location_S>  generateHexCellCenterLocation(int nSec,double cellRadius);
Array <Location_S>  generateUniformDropLocationInHexCell(Location_S cellloc,double cellRadius,double minDistFromCenter,int locationCount=1);
Array <Location_S>  generateUniformDropLocationInHexCell(Location_S cellloc,int sectorsPerCell,int secid,double cellRadius,double minDistFromCenter,int locationCount=1);
Array <Location_S>  generateUniformDropLocation(Area_S cvrgArea,int locationCount=1);
Array <Location_S> generateEquiDistantLocationInRect(Area_S cvrgArea, int locationCount, double xminDist, double yminDist=-1);
Array <Location_S> generateUniformDropLocationForInHCell(Area_S cvrgArea, int locationCount=1);
Array <Location_S>  generateUniformDropLocationInCircle(Location_S circleCenter,double circleRadius,double minDistFromCenter,int locationCount=1);
Array <Location_S>  generateEquiDistantLocationOnCircle(Location_S circleCenter,double circleRadius,double phaseOffsetInDegree,int locationCount=1);
Array <Location_S>   generateIndoorHotspotLoc(Array <Location_S>  inHotspotTypeCellLoc,ivec inHNodes);
Array <Location_S> generateHexCellCenterLocationForDenseUrban(int nSec,double cellRadius);
Array < Location_S> generateUniformDropLocationInInHotspot(Location_S cellloc, int locationCount=1);
Array <Location_S>   generateIndoorLoc(Array<Location_S>& cellLoc,Area_S coverageArea,double minDist,int locationCount=1);
Array<Location_S> generate3DSatelliteDropLocation(double height,int locationCount=1);
Array<Location_S> generateHexCellCenterLocationIn3D(int nCell,double cellRadius,Location_S centreLoc);
Array <Location_S>  generateUniformDropLocationInHexCell3D(Location_S cellloc,double cellRadius,double minDistFromCenter,double height=0,int locationCount=1);
Array<Neighbour_S> getWrapAroundNeighbours3D(int cellCnt, int cellID, Location_S cellLocation, double cellRadius);
Location_S WrapMyLocation3D(Location_S wrapTxCellLoc, Location_S txCellLoc, Location_S txLoc);
void generateRadiusAndAngleForHexCellCenterLocation(vec& radius,vec& angle,int nCell,double cellRadius);

double findDistance(Location_S N1Loc, Location_S N2Loc);
double find3Ddistance(Location_S N1Loc, Location_S N2Loc);
vec findAngle(Location_S N1Loc, Location_S N2Loc);
vec find3DAngle(Location_S N1Loc, Location_S N2Loc,double verAntElementSpacing,int antElementsPerCol);// for 3D Channel Model
//AAS
vec find3DAngle(Location_S N1Loc, Location_S N2Loc/*,double verAntElementSpacing,double horAntElementSpacing, int antElementsPerCol,int antElementsPerRow*//*,double sectorTilt*/);
double getShadowLoss(Location_S nodeLoc,mat shdwGrid,double shadwStdDev,double corrDist);
Array<Neighbour_S> getWrapAroundNeighbours(int cellCnt, int cellID, Location_S cellLocation, int cellISD);
void initGridsForSCMLargeScaleParameters(Array< Array< Array<mat> > > &scmGrids,SCMInfo_S &scmInfo,SCMPropagationScenario scmScenario,InFScenario_E infScenario=_INF_UNDEF_);
void initGridsForSpatialConsistencyParameters(Array< Array<mat> > &scmGridsForSpConsist,SCMInfo_S &scmInfo,SCMPropagationScenario scmScenario);
Location_S WrapMyLocation(Location_S wrapTxCellLoc, Location_S txCellLoc, Location_S txLoc);
int findInStringArray(Array<string> stringArray, string stringToFind);
mat IMT_computeSiteCorrelationMatrices (int Scenario,int isLOS, int isOutdoorToIndoor);
mat compute3DChannelSiteCorrelationMatrices (int Scenario,int isLOS, int isOutdoorToIndoor);
mat compute5GChannelSiteCorrelationMatrices (int Scenario,bool isLOS, bool isOtoI);
mat computeNTNCorrelationMatrices(SCMPropagationScenario scmScenario,double elevationAngle,bool isLOS,bool isKaBand);
void generateSCMLargeScaleParameters(NodeLinkInfo_S &linkInfo,Array< Array< Array<mat> > > &scmGrids,SCMPropagationScenario scmScenario,InFScenario_E infScenario, SCMInfo_S &scmInfo ,Location_S nodeLocation, ChannelModelType channelModelType, bool isOutdoorToIndoor,int isDownlink=-1);
vec getZSDparams(NodeLinkInfo_S& linkInfo,SCMPropagationScenario scmScenario,ChannelModelType channelModelType,double fc,bool isDownlink,SCMLinkCondition linkCondition);
void generateNTNSCMLargeScaleParameters(NodeLinkInfo_S& linkInfo,SCMPropagationScenario scmScenario,InFScenario_E infScenario,ChannelModelType channelModelType,int isDownlink);
double gaussian(double mu, double var);
complex<double> complexGaussian(double mu, double var);
cvec complexGaussian(double mu, double var,int lengthOfVector);
SCMPropagationScenario loadSCMScenario(string fileName,bool isFileName=true);
Array<NodeCategory_E> parseNodeCategory(string configFile,string variableName);
FadingChannelModel_E parseChannelModel(string chnModel);
int findClosestNeighbourCellIndx(Array<Neighbour_S> &neighbourInfo, Location_S referenceLocation);
Array<Location_S> getRotatedLocations(Array<Location_S> &locations, double rotationInDegree);
Array<Location_S> getShiftedLocations(Array<Location_S> &locations, Location_S xyShift);
ChannelGenerationMethod_E parseChannelGenerationMethod(string fileName);
ChannelModelType parseChannelModelType(string fileName);
mat Rz(double aplhaInRadian);
mat Ry(double betaInRadian);
mat PhiCap(double phiInRadian);
mat ThetaCap(double thetaInRadian, double phiInRadian);
void convertAngleFromGCStoLCS(double &ZOD,double &AOD,double alpha,double beta,double gamma,bool isInRadian=false);
void convertAngleFromLCStoGCS(double &ZOD,double &AOD,double alpha,double beta,double gamma,bool isInRadian=false);
mat getRotationMatrix(double alpha,double beta,double gamma,bool isInRadian=false);
mat getCartesianVector(double theta,double phi,bool isInRadian=false,double rho=1.0);
void getAngles(mat unitVector,double& AOD,double& ZOD,bool returnInRadian=false);

void initGridsFor5GSCMLargeScaleParameters(Array< Array< Array<mat> > >& scmGrids,SCMInfo_S *scmInfo,SCMPropagationScenario scmScenario,InFScenario_E infScenario=_INF_UNDEF_);
double rho(double norm_d);
vec rho(vec norm_d);
mat getlspMuSigmaForInF(Area_S coverageArea,double fc,InFScenario_E infScenario);
string getString(SCMLinkCondition linkCondition);
bool is5GSCMScenario(SCMPropagationScenario scmScenario);
bool is2DSCMScenario(SCMPropagationScenario scmScenario);
bool is3DSCMScenario(SCMPropagationScenario scmScenario);
bool isNTNSCMScenario(SCMPropagationScenario scmScenario);
// void generate5GSCMLargeScaleParameters(NodeLinkInfo_S& linkInfo,Array< Array< Array<mat> > > &scmGrids,SCMInfo_S& scmInfo,SCMPropagationScenario scmScenario,InFScenario_E infScenario,ChannelModelType channelModelType,bool isOutdoorToIndoor,int isDownlink,int cellID);
vec generateSCRVsUsingSOS(Location_S txLocation,Location_S rxLocation,mat dcorr,SCMPropagationScenario scmScenario,InFScenario_E infScenario,SCMLinkCondition linkCondition,int cellID,ACF_type acfType);
void computeLSPs(NodeLinkInfo_S& linkInfo,vec lsp,vec lspMuSigma,SCMPropagationScenario scmScenario,ChannelModelType channelModelType,SCMLinkCondition linkCondition,int isDownlink);
vec generateSpatialCorrelatedRandomVariables(Location_S nodeLocation,vec corrDist,Array< Array< Array<mat> > > &scmGrids,Area_S coverageArea,InFScenario_E infScenario,SCMLinkCondition linkCondition,vec normGridDist);
vec generateSpatialCorrelatedRandomVariablesForNTN(Location_S nodeLocation,vec corrDist,Array< Array< Array<mat> > > &scmGrids,Area_S coverageArea,InFScenario_E infScenario,SCMLinkCondition linkCondition,vec normGridDist);
vec generateUncorrelatedRandomVariablesForNTN(InFScenario_E infScenario,SCMLinkCondition linkCondition);
void print5GParams(SCMInfo_S* scmInfo,NodeLinkInfo_S* linkInfo,SCMPropagationScenario scmScenario,ChannelModelType channelModelType,InFScenario_E infScenario,double fc,int isDownlink);
int get5GSCMGridIndx(SCMPropagationScenario scmScenario,SCMLinkCondition linkCondition);
double getSigmaSFForInF(InFScenario_E infScenario);
double getElevationAngle(double bsHeight, double d);
double getElevationAngle(Location_S bsLoc,Location_S ueLoc);
bool isKaBand(double carrierFrequencyInHz);
vec getlspMuSigmaForNTN(SCMPropagationScenario scmScenario,double elevationAngle,bool isLOS,bool isKaBand);
vec getCorrelationDistancesForNTN(SCMPropagationScenario scmScenario,double elevationAngle,bool isLOS,bool isKaBand);
Array<ivec> getNodeIndicesForFrequencyReuse(int reuseFactor,int nCells);
ivec getFrequencyIDPerCell(int reuseFactor,int nCells);
ivec getRefCellID(int reuseFactor);
double getAngle(mat u1,mat u2);
#endif



