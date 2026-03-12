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

#ifndef _NODE_H_
#define _NODE_H_

#include "Antenna.h"
#include "Terrain.h"

struct NTNCoverageInfo_S
{
    int cellID;
    Location_S centre; // Centre of coverage area
    double radius; //radius of coverage
    NTNCoverageInfo_S()
    {
        radius=NAN;cellID=-1;
    }
};

class Node{
  
  //variables
  public :   
    Antenna txAntenna;
    Antenna rxAntenna;
    Array< Array< Array<mat> > > scmGrids;
    Array< Array<imat> > scmGridsForSpConsist; // TR 38.901 for spatial Consistency
    mat dcorrForSPConsistency;
    Array<NodeLinkInfo_S> aLinkInfo; 
    ivec linkedTxNodes;
    SCMPropagationScenario scmPropagationScenario;
    Orientation_S nodeOrient;
    NTNCoverageInfo_S ntnCoverageInfo;
  protected :
    int id;
    int idCell;
    int nAntenna;
    int centralUnitID;
    int hexagonalCellID;
    ivec groupID;
    double carrierFreq;
    double txPowerInDBm;
    NodeCategory_E nodeCategory;
    Location_S loc;
    Location_S wrapLoc;
    bool active;
    int type;
    mat shadowGrid;
    double corrDist;
    bool linkInfoUpdateStatus;
    double velocityInMetrePerSec;
    double h_phase,v_phase; //velocity phase factors(theta,phi)
    Array<Neighbour_S>	aNeighbourCell;
    ivec numerologies;
    int satelliteID; //Refers to satellite to which current node is covered by. 
    int freqID; //Refers to the frequency which is assigned to current node among that of FRF. 
    //Functions  
  public :
    Node();
    void selfConfig(string filename);	///< Load node configurations for the current node from the given file
    bool isNodeReady(int isHexagonalLayout);	///< Returns true if all necessary parameters of the node is set
    void setID(int nodeID,bool overwrite=false);	///< Set the ID of the node
    void setIDCell(int idcell);	///< Set the ID cell for the node (used in system simulations)
    void setCentralUnitID(int cuID);	///< Set the CentralUnitID for the node (used in system simulations)
    void setNumerologies(ivec nus);	///< Set the CentralUnitID for the node (used in system simulations)
    void setVelocity(double velocityInMetrePerSec);	///< Set velocity for the node (used in system simulations)
    void setHexagonalCellID(int cellId);	///< Set the Hexagonal cell ID for the node
    void setNeighbourCell(Array<Neighbour_S>);	///< Set neighbour cell info
    void setAsIndoor();	///< Make the node Indoor
    void setWithLowPenetrationLoss(); //Seting low penetration loss
    void setOrientation(double a,double b,double g);
    
    void setAsOutdoor();	///< Make the node Outdoor
    void setAsPedestrian();
    void setIndoorDistance(double dist);
    void computePenetrationLoss(double carrierFreqInGHz,ChannelModelType channelType,SCMPropagationScenario scmScenario);
    void setAsInBuilding();	///< Make the node InBuidling (will set both InBuilding and Indoor flags)
    void setAsOutBuilding();	///< Make the node OutBuilding (will set only InBuilding flag)	
    void setActive();	///< Make the node Active
    void setInactive();	///< Make the node Inactive
    void setHeight(double nodeHeight, bool overwrite=false); ///< Set Height for the node
    void setCarrierFreqInHz(double carrierFreqinHz);	///< Set Carrier Frequency for the node
    //void setCarrierFreq(double carrierFreqinHz);	///< Set Carrier Frequency for the node
    void setTxPowerInDBm(double txPowerIndBm);	///< Set Node Tx power
    void setCategory(NodeCategory_E category);	///< Set the Node category for the node (serverNode/serviceNode)
    void setType(int nodetype);	///< Set the node to the given type
    void setLoc(Location_S nodeLoc, bool overwrite=false);	///< Set Location for the node
    void setWrapLoc(Location_S wrapAroundXYLoc, bool overwrite=false);	///< Set the Wrap Around Location of the node
    void setXYLoc(Location_S nodeLoc, bool overwrite=false);	///< Set the X,Y Location of the node retaining the height if any set already
    void setXYLoc(double xLoc, double yLoc, bool overwrite=false);	///< Set the X,Y Location of the node
    void setShadowGrid(mat corrShdwGrid);	///< Set the shadow grid for the node in case of Correlated shadowing
    void setNumOfAntenna(int antCnt);	///< Set the number of antenna in the node (both Tx and Rx)
    void setAntenna(AntennaType_E antennaType,double antGain,double feederLoss, double horBW, double horTilt,double hMaxAtt,double verBW, double verTilt,double vMaxAtt);	///< Set the antenna Pattern for the  node
    void setAntenna(AntennaType_E antennaType,double antGain,double feederLoss, double hTilt, double vTilt, vec hPattern,vec vPattern);	///< Set the antenna Pattern for the  node
    void setAntenna(Antenna newAntenna);	///< Set the antenna Pattern for the  node
    void setTxAntenna(Antenna newAntenna);	///< Set the Tx antenna Pattern for the  node
    void setRxAntenna(Antenna newAntenna);	///< Set the Rx antenna Pattern for the  node
    void setAntenna(AntennaType_E antennaType,double antGain,double feederLoss,double radius,double temperature,double gtRatio);
    void setHAPSAntenna(int M, int N, double dv, double dh,
                        double Ge_max, double Am, double SLAv,
                        double phi3dB, double theta3dB,
                        double feederLoss);
    void setHAPSAntennaTilts(Location_S targetLoc);
    void setLinkInfoUpdateStatus(bool status);	///< Set the linkInfo Update Status of the current node
    void addToGroup(int groupID);	///< Adds the node to the given group
    void setSCMGridsForSpConsist(Array< Array<imat> > gridsForSpConsist);
    void setFloorIndex(int floorIndx,bool overwrite=false);
    
    NodeCategory_E getCategory();	///< Returns the current category of the node	
    int getType();	///< Returns the type of the node
    mat getShadowGrid();	///< Returns ther shadow grid of the node in case of Correlated Shadowing
    double getCorrelatedDistance(); ///<Returns the Correlated distance for Correlated Shadowing
    void clearShadowGrid();	///< Clears the Shadow Grid of the node
    bool isActive();	///< Reutrns true if the node is active
    bool isIndoor();	///< Reutrns true if the node is Indoor
    bool isPedestrian();	///< Reutrns true if the node is Pedestrian
    bool isWithLowPenetrationLoss();
    bool isWithHighPenetrationLoss();
    bool isOutdoor();	///< Reutrns true if the node is Outdoor
    bool isLinked(int txNodeID);	///< Returns true if the give tx node is linked to the Node
    bool isHavingSCMGrid();///< Returns true if the node is having SCM Grids 
    int getID();	///< Returns the ID of the node
    int getIDCell();	///< Returns the ID cell of the node (used in system simulations)
    int getCentralUnitID();	///< Returns the CentralUnit ID of the node (used in system simulations)
    ivec getNumerologies();	
    double getVelocity();	///< Returns velocity of the node (used in system simulations)
    double getCarrierFreqInHz();///< Returns the carrier frequency of the node
    int getCellID();	///< Returns the cell ID of the node
    double getTxPowerInDBm(); ///< Returns the node's Tx power
    ivec getGroupID();	///< Returns the group IDs ,the node belong to
    Location_S getLoc();	///< Returns the Location of the node
    double getHeight();	///< Returns the Height of the node
    Location_S getWrapLoc();	///< Returns the Wrap Around Location of the node
    Array<Neighbour_S> getNeighbourInfo(); ///< Returns the neighbour cell Info
    Location_S getNeighbourCellLoc(int neighbourCellID);	///< Returns the neighbour cell location with respect to this node in case of Hexagonal Layout
    int getAntennaCount();
    double getAntennaGain(Location_S newNodeLoc);	///< Returns the antenna Gain for the given Location from this node
    double getTxAntennaGain(Location_S newNodeLoc);	///< Returns the antenna Gain for the given Location from this node
    double getRxAntennaGain(Location_S newNodeLoc);	///< Returns the antenna Gain for the given Location from this node
    
    double get3DTxAntennaGain(int newNodeType, Location_S newNodeLoc,double carrierFreq);
    
    double get3DRxAntennaGain(int newNodeType, Location_S newNodeLoc,double carrierFreq);
    Array< Array<imat> > getSCMGridsForSpConsist();
    
    int getLinkCount();	///< Returns the number of link Info available in the node
    ivec getLinkedTxNodes(); ///< Returns the Tx node IDs linked to the current Rx node
    NodeLinkInfo_S getLinkInfo(int nodeID);	///< Returns the link info of the given nodeID from this node
    void updateLinkInfo(NodeLinkInfo_S& linkInfo);
    void updateLocation(Location_S& loc);
    NodeLinkInfo_S getLinkInfoByIndex(int index);	///< Returns the index-th link info available in the node
    double getRxPower(int nodeID);	///< Returns receive power from the given nodeID
    double getRxPowerByIndex(int index); ///< Returns receive power from the given linkinfo index
    bool getLinkInfoUpdateStatus();		///<Checks whether all node Link Info are up-to-date
    void clearLinkInfo();	///< Clears the linkInfo of all nodes	
    void retainLinkInfo(ivec nodeIDs=""); ///< Clears the LinkInfo of all nodes other than the given nodesIDs
    void clearLinkInfo(int nodeID);	///< Clears the linkInfo of the given nodeID from the current node
    void clearLinkInfoByIndex(int index);	///< Clears the linkInfo at the given index from the current node
    Antenna getTxAntennaInfo();	///< Returns the Tx antenna object in the current node
    Antenna getRxAntennaInfo();	///< Returns the Rx antenna object in the current nodebsLoc
    void allocLinkInfo(ivec &txNodeIDs);///< Used for reserving space for the linkInfos (used to improve speed)
    void linkToTXNode(Node &txNode, Terrain &sysTerrain,ShadowType_E shdwType, bool isLinkAltered ,bool retainUnAlteredLinks ,bool retainOldLOS=false);	///< Link the node to the given Tx node
    
    void initCorrelatedShdw(Correlatedshadow_S corrShdw);	///< Initialize Correlated Shadowing related parameters
    void initGridsForSCMLargeScaleParameters(SCMInfo_S &scmInfo,SCMPropagationScenario scmScenario,InFScenario_E infScenario=_INF_UNDEF_);	///< Initialize Grids for SCM Large Scale Paramter Generation
    void initGridsForSpatialConsistencyParameters(SCMInfo_S &scmInfo,SCMPropagationScenario scmScenario);
    void overwriteLinkInfo(int txNodeID, string propertyName, double propertyValue); ///< Overwrites the link info generated by generateLinkInfo() manually . Take Extra Care while using this function
    void overwriteLinkInfo(int txNodeID, SCMLargeScaleParameters_S &scmLargeScaleParameters);
    vec getVelocityVector(bool isCartesian=true);
    void computeAntennaGains(Node& txNode,NodeLinkInfo_S& tempLinkInfo);
    void setVelocity(vec vel)
    {
        velocityInMetrePerSec = vel(0);h_phase = vel(1);v_phase = vel(2);
    }
    int getLinkInfoIndx(int nodeID);
    void setLinkInfo(NodeLinkInfo_S& linkInfo);
    void setCoverageInfo(double beamRadius,Location_S centreLoc,int coverageID);
    void setAntennaTilts(Location_S centreLoc,bool useWrapped=false);
    void setSatelliteID(int satID){satelliteID = satID;}
    int getSatelliteID(){return satelliteID;}
    int getCoverageCellID(){return ntnCoverageInfo.cellID;}
    void setFrequencyID(int id){freqID = id;}
    int getFrequencyID(){return freqID;}
    private:
    
      bool getSpatiallyConsistentLOS(Area_S coverageArea, SCMPropagationScenario scmScenario, Location_S relativeServiceNodeLoc);
      void linkToTXNode(NodeLinkInfo_S &linkInfo,Node &txNode, Terrain &sysTerrain,ShadowType_E shdwType, bool isLinkAltered ,bool retainUnAlteredLinks ,bool retainOldLOS=false);	///< Link the node to the given Tx node
    
};
#endif






