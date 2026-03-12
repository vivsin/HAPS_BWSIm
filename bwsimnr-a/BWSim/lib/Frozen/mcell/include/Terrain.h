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

#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include<map>
#include "SupportingFunctions.h"

class Terrain;
typedef NodeLinkInfo_S (*pathLossFunction)(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType,const Terrain * terr,NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2);

/**
* @brief Terrain Class which manages pathloss and shadowLoss models and responsible for generating NodeLinkInfo_S for each Tx-Rx link 
*/
enum PenetrationModel_E {_UNDEFINED_, O_TO_I_PENETRATION_LOW,O_TO_I_PENETRATION_HIGH};
enum IndoorScenario_E {_INDOOR_UNDEFINED_,_INDOOR_MIXED_OFFICE_ ,_INDOOR_OPEN_OFFICE_ };

struct InFParam_S
{
    InFScenario_E scenario;
    double clutterDensity;
    double clutterHeight;
    double clutterSize;
    InFParam_S()
    {
        scenario = _INF_UNDEF_;
        clutterDensity=-1;
        clutterHeight=-1;
        clutterSize=-1;
    }
};

class Terrain {

  public:
    bool isForSCMChannel;
    bool isSpConsistEnable;
    
    SCMInfo_S scmInfo;
    Array< Array< Array<mat> > > commonSCMGrids;
//     Array< Array<mat> > commonSCMGridsForSpConsist;
    imat coverageFootprint;
    Array<mat> coveragePlan;
    bool is3Ddistance;
    ChannelModelType channelModelType;
    IndoorScenario_E indoorOfficeScenario;
    SCMPropagationScenario scmScenario;
    bool isLMLC;
    bool useOptionalPathLoss;
    InFParam_S infParams; 
private :

    Array<string> terrainTypes;
    string terrainType;
    double cellISD;
    double wallSpacing;
    int maximumFloors;
    RoomSize_S roomSize;
    double thickWallLossInDB;
    double thinWallLossInDB;
    double buildingSideLength;
    map<string,pathLossFunction> terrainMap;
    
public :

    Terrain();
    bool isReadyForIndoor();	///< Check if all parameters for Indoor Simulation is set properly
    void setCellISD(double isd);	///< Set Inter-site-Distance Information in case of Hexagonal Layout
    void setTerrain(string terrainName);		///< Set Simulation Terrain
    void setMaximumFloors(int maxFloors);	///< Set maximum number of floors in the terrain for Indoor Simulation
    void setWallSpacing(double distanceInMeters);	///< Set distance between adjacent walls(light walls) inside building in the terrain for Indoor Simulation
    void setRoomSize(double length, double height);	///< Set Roomsize in the terrain for Indoor Simulation 
    void setRoomSize(double length,double width, double height);
    void setBuildingSideLength(double buildingLength);	///< Set Size of the building in the terrain for Indoor Simulation 
    void setWallPenetrationLoss(double thickWallPenetrationLossInDB, double thinWallPenetrationLossInDB);	///< Set wall penetration loss in the terrain for Indoor Simulation 
    void addTerrain(string terrainName,pathLossFunction terrainFunction, bool overwrite=false);	///< Add a new terrain model and corresponding Pathloss function to the current Terrain class
    void addTerrain(Array<string> terrainNames, pathLossFunction terrainFunction, bool overwrite=false);///< Add new terrain models and corresponding Pathloss function  to the current Terrain class
    double getCellISD() const; ///< Returns the cell ISD (set only in Hexagonal Layout simulation)
    string getTerrain() const;	///< Returns the cell ISD (set only in Hexagonal Layout simulation)
    int getMaximumFloors() const;	///< Returns the Maximum Floors set in the Terrain (set only in simulations with Indoor Nodes)
    double getWallSpacing() const;	///< Returns the Indoor wallSpacing set in the Terrain (set only in simulations with Indoor Nodes)
    double getWallPenetrationLoss(bool isThickWall=true) const;	///< Returns the Wall PenetrationLoss in dB set in the Terrain (set only in simulations with Indoor Nodes)
    RoomSize_S getRoomSize() const;	///< Returns the Roomsize set in the Terrain (set only in simulations with Indoor Nodes)
    double getBuildingSideLength() const;///< Returns the building size set in the Terrain (set only in simulations with Indoor Nodes)
    NodeLinkInfo_S getPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1,int txNodeType=-1, int rxNodeType=-1,NodeCategory_E nodeCategory1=_NODECATEGORY_UNINIT_,NodeCategory_E nodeCategory2=_NODECATEGORY_UNINIT_);///< Returns the Pathloss and related Info between the two node Locations in the Terrain

    /// Building Related
    void setCoveragePlan(Array <mat> CovPlan);
    Array<mat> getCoveragePlan() const;
    void setCoverageFootprint(imat &footPrint);
    imat getCoverageFootprint() const;
    double getBuildingPenetrationLoss(Location_S rxNodeLoc, Location_S txNodeLoc, bool isWrapped, Location_S txNodeWrapLoc) const; 
    void enable3DDistance();// to compute 3D distance for pathloss
    void disable3DDistance();// to compute 3D distance for pathloss
    void loadIndoorFactoryParams(string mySysConfigFile);

    
private:
  double getBuildingPenetrationLoss(Location_S rxNodeLoc, Location_S txNodeLoc, bool invalidErrorBreaker=false) const;
  
    
};

// No-Loss Terrain (0dB Pathloss & 0dB shadowLoss) // Only used for Link-Level simulation
NodeLinkInfo_S getNoLossPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1, int txNodeType=-1, int rxNodeType=-1, const Terrain * terr=0,NodeCategory_E nodeCategory1=_NODECATEGORY_UNINIT_,NodeCategory_E nodeCategory2=_NODECATEGORY_UNINIT_); ///<Returns 0dB pathloss and 0dB Shadow Stansard Deviation
void loadNoLossTerrain(Terrain &terrain);	 ///< Loads NoLoss-Terrain Model

//3gppModel
// _TERRAIN_3GPP_CASE1_ _TERRAIN_3GPP_CASE3_ 
NodeLinkInfo_S getHetNetPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1, int txNodeType=-1, int rxNodeType=-1, const Terrain * terr=0,NodeCategory_E nodeCategory1=_NODECATEGORY_UNINIT_,NodeCategory_E nodeCategory2=_NODECATEGORY_UNINIT_);
NodeLinkInfo_S het3gppPicoUeModel1(Location_S nodeLoc1, Location_S nodeLoc2, double carrierFreqInHz);
NodeLinkInfo_S het3gppMacroUeModel1(Location_S nodeLoc1, Location_S nodeLoc2, double carrierFreqInHz);
void load3gppTerrain(Terrain &terrain);

//imtModel
// _TERRAIN_IMT_URBAN_MICRO_ _TERRAIN_IMT_URBAN_MACRO_ _TERRAIN_IMT_SUBURBAN_MACRO_ _TERRAIN_IMT_RURAL_MACRO_ 
NodeLinkInfo_S getIMTPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1, int txNodeType=-1, int rxNodeType=-1, const Terrain * terr=0,NodeCategory_E nodeCategory1=_NODECATEGORY_UNINIT_,NodeCategory_E nodeCategory2=_NODECATEGORY_UNINIT_);
NodeLinkInfo_S imtUrbanMicro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1, const Terrain * terr=0);
NodeLinkInfo_S imtUrbanMacro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1, const Terrain * terr=0);
NodeLinkInfo_S imtSubUrbanMacro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1);
NodeLinkInfo_S imtRuralMacro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int txNodeType, int rxNodeType, int isLOS=-1);
void loadIMTTerrains(Terrain &terrain);

//winnerModel
// _TERRAIN_WINNER_URBAN_MICRO_ _TERRAIN_WINNER_URBAN_MACRO_ 
NodeLinkInfo_S getWinnerPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1, int txNodeType=-1, int rxNodeType=-1,  const Terrain * terr=0,NodeCategory_E nodeCategory1=_NODECATEGORY_UNINIT_,NodeCategory_E nodeCategory2=_NODECATEGORY_UNINIT_);
NodeLinkInfo_S UMicroIndoorToOutdoorPathloss(Location_S nodeLoc1, Location_S nodeLoc2, double carrierFreqInHz, const Terrain * terr=0);
NodeLinkInfo_S UMicroOutdoorToOutdoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1);
NodeLinkInfo_S UMacroOutdoorToOutdoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1,const Terrain * terr=0);
NodeLinkInfo_S UMacroOutdoorToIndoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz,const Terrain * terr=0);
NodeLinkInfo_S IndoorToIndoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1,const Terrain * terr=0);
void loadWinnerTerrains(Terrain &terrain);


NodeLinkInfo_S getWinnerPlusPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1, int txNodeType=-1, int rxNodeType=-1,  const Terrain * terr=0,NodeCategory_E nodeCategory1=_NODECATEGORY_UNINIT_,NodeCategory_E nodeCategory2=_NODECATEGORY_UNINIT_);
NodeLinkInfo_S PlusUMicroIndoorToOutdoorPathloss(Location_S nodeLoc1, Location_S nodeLoc2, double carrierFreqInHz, const Terrain * terr=0);
NodeLinkInfo_S PlusUMicroOutdoorToOutdoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1);
NodeLinkInfo_S PlusUMacroOutdoorToOutdoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1);
NodeLinkInfo_S PlusSMacroOutdoorToOutdoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1);
NodeLinkInfo_S PlusUMacroOutdoorToIndoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz,const Terrain * terr=0);
// NodeLinkInfo_S PlusIndoorToIndoorPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1,const Terrain * terr=0);
void loadWinnerPlusTerrains(Terrain &terrain);


// compModel
//_TERRAIN_COMP_3GPP_CASE1_
NodeLinkInfo_S getCompPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1, int txNodeType=-1, int rxNodeType=-1, const Terrain * terr=0,NodeCategory_E nodeCategory1=_NODECATEGORY_UNINIT_,NodeCategory_E nodeCategory2=_NODECATEGORY_UNINIT_);
void loadCompTerrains(Terrain &terrain);

//Okamura Hata model for 700MHz(150-1500MHz)
//_TERRAIN_HATA_URBAN_ _TERRAIN_HATA_SUBURBAN_ _TERRAIN_HATA_OPEN_
NodeLinkInfo_S getHataPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr,NodeCategory_E nodeCategory1=_NODECATEGORY_UNINIT_,NodeCategory_E nodeCategory2=_NODECATEGORY_UNINIT_);

NodeLinkInfo_S getHataPathLoss_S(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr,NodeCategory_E nodeCategory1=_NODECATEGORY_UNINIT_,NodeCategory_E nodeCategory2=_NODECATEGORY_UNINIT_);

NodeLinkInfo_S getHataPathLoss_O(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr,NodeCategory_E nodeCategory1=_NODECATEGORY_UNINIT_,NodeCategory_E nodeCategory2=_NODECATEGORY_UNINIT_);
void loadHataTerrains(Terrain &terrain);

//CoExistence Terrains
NodeLinkInfo_S getCrossLinkPathloss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType,  const Terrain * terr,NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2);
NodeLinkInfo_S UMaServertoServerPathloss(Location_S nodeLoc1,Location_S nodeLoc2);
NodeLinkInfo_S ServicetoServicePathloss(Location_S nodeLoc1,Location_S nodeLoc2);


// smallCellModel
//_TERRAIN_SMALL_CELLS_
NodeLinkInfo_S getSmallCellPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr,NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2);
void loadSmallCellTerrains(Terrain &terrain);



//3D Channel

NodeLinkInfo_S get3DChannelPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1, int txNodeType=-1, int rxNodeType=-1,  const Terrain * terr=0,NodeCategory_E nodeCategory1=_NODECATEGORY_UNINIT_,NodeCategory_E nodeCategory2=_NODECATEGORY_UNINIT_);
NodeLinkInfo_S channel3DUrbanMicro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1, int txNodeType=-1, int rxNodeType=-1, const Terrain * terr=0, NodeCategory_E nodeCategory1=_NODECATEGORY_UNINIT_,NodeCategory_E nodeCategory2=_NODECATEGORY_UNINIT_);
NodeLinkInfo_S channel3DUrbanMacro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS=-1, int txNodeType=-1, int rxNodeType=-1, const Terrain * terr=0, NodeCategory_E nodeCategory1=_NODECATEGORY_UNINIT_,NodeCategory_E nodeCategory2=_NODECATEGORY_UNINIT_);
void load3DChannelTerrains(Terrain &terrain);


//5G Channel

bool load5GTerrains(Terrain &terrain);

bool isLOS_RMa(double d2Dout);
bool isLOS_UMa(double d2Dout, double ueHeight);
bool isLOS_UMi(double d2Dout);

NodeLinkInfo_S get5GChannelPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr,NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2);
NodeLinkInfo_S pathLoss5GRuralMacro(Location_S nodeLoc1, Location_S nodeLoc2, double carrierFreqInHz, int isLOS, NodeCategory_E nodeCategory1, NodeCategory_E nodeCategory2, ChannelModelType channelModelType, bool isLMLC = false);
NodeLinkInfo_S pathLoss5GUrbanMacro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType);
NodeLinkInfo_S pathLoss5GUrbanMicro(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType);
NodeLinkInfo_S pathLoss5GIndoor(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2, ChannelModelType channelModelType);
NodeLinkInfo_S pathLoss5GIndoorFactory(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2, ChannelModelType channelModelType);
NodeLinkInfo_S pathLossFreeSpace(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz);
double computeO2IpenetrationLoss(ChannelModelType channelModelType,PenetrationModel_E OtoIPenetrationModel,double carrierFreqInGigaHz,double d2Din,bool isUrban);

//NTN Channel

bool loadNTNTerrains(Terrain &terrain); 
NodeLinkInfo_S getNTNChannelPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType, const Terrain * terr,NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2);
// NodeLinkInfo_S pathLossNTNDenseUrban(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType);
// NodeLinkInfo_S pathLossNTNUrban(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType);
// NodeLinkInfo_S pathLossNTNSubUrban(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType);
// NodeLinkInfo_S pathLossNTNRural(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType);
double computeBuildingPenetrationLoss(double carrierFreqInGHz,bool isTraditional,double theta=0);
double computeAtmosphericLoss(double carrierFreqInGHz,double elevationAngle);
double computeScintillationLoss(double carrierFreqInGHz,Location_S loc,double elevationAngle);
NodeLinkInfo_S computeNTNPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, const Terrain * terr, NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2,ChannelModelType channelModelType);
double getNTNLOSProbability(SCMPropagationScenario scmScenario,double elevationAngle);
double getNTNClutterLossInDB(SCMPropagationScenario scmScenario,double elevationAngle,double carrierFreqInHz);
double getNTNShadowStdDev(SCMPropagationScenario scmScenario,double elevationAngle,double carrierFreqInHz,bool isLOS);
#endif
