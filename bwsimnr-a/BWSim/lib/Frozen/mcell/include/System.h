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

#ifndef _MSYS_H_
#define _MSYS_H_

#include "Node.h"
#include "Channel.h"

/**
* @brief McellSystem Class which manages Nodes and other global parameters and can control the dropping of nodes 
*/

using std::string;
struct CarrierAssociationInfo_S{
  
  ivec serviceNodes; //Need not be all available serviceNodes in the McellSystem
  ivec associatedServerNodesForEachServiceNode;
};


class McellSystem{
  
  //variables
  public :   
    Array<Node> aNodes;
    Area_S coverageArea;
    Terrain sysTerrain;
    vec carrierFreqInMHzPerServerNodeType;
    ChannelCloud channelCloud;
    bool isCrossLinksEnabled;
    Array<ivec> serviceNodesPerTask;
    imat rndSeedForPL;
        
  protected :
    
    ivec nodeIDs;
    bvec configReady;
    CellLayout_E cellLayout;
    double cellISD;
    double minDist;
    ShadowType_E shadowType;
    ivec nNodes;
    vec nodeTypeHeights;
    double hexDropCommonVerticalTilt;
    vec antennaFeederLossIndB;
    Array<string> aNodeTypes;
    Array<NodeCategory_E> nodeCategoryPerNodeType;
    vec noiseFigurePerNodeType;
    Array<NodeGroup_S> aNodeGroups;
    ivec nodeDropMethods;
    ivec indoorNodePercentage;
    ivec pedestrianNodePercentage;
    ivec OtoIPntrModlL_H_Percentage;
    Correlatedshadow_S corrShdw;
    bool addPathSpecificSCMAntennaGain;
    MobilityInfo_S mobilityInfo;
    BuildingInfo_S buildingInfo;
    bool cellWiseDropping;
    ivec networkIDPerNodeType;
    CarrierAssociationInfo_S carrierAssociationInfo;
    int nSectorsPerSite;
    int nBeamsPerSatellite;
    double satelliteBeamRadius;
    int frequencyReUseFactor;
    double beamElevationAngle;
    //Functions  
  public :
    
    /// System Related	
    McellSystem();
    void initSystem();
    void initSystem(string mySysConfigFile, string AASConfigFile,bvec nodeTypeSelector="");	///< Mandatory Function to be called to initialize the System Properties .. 
    void initNodes(string mySysConfigFile, string AASConfigFile);	///< Function to be called to initialize the Node Properties from the given file.. 
    void dropNodes();	///< Drop all nodes in the system using their respective drop methods ...
    void dropNodes(ivec nodeTypes,bool overWrite=false,bool setIndoorNodes=true);	///< Drop given node types in the system using their respective drop methods ...
    void initSCMParameters(ivec gridHolderNodeTypes,SCMPropagationScenario scmScenario);
    void init2DSCMParameters(ivec gridHolderNodeTypes,SCMPropagationScenario scmScenario);//< 2D SCM Channel Model
    void init3DChannelParameters(ivec gridHolderNodeTypes,SCMPropagationScenario scmScenario);//< 3D Channel Model
    void loadAASConfig(string AASConfigFile,SCMPropagationScenario scmScenario,bool isToLoadAAS=false);
    void loadAntennaInfo(string AASConfigFile,string mySysConfigFile,bool isToLoadAAS=false);
    //5GChan
    void init5GChannelParameters(ivec gridHolderNodeTypes,SCMPropagationScenario scmScenario);
    mat lspMuSigmaCal(double carrierFreqInGHz, ChannelModelType channelModelType);
    mat correlationDistCal(double carrierFreqInGHz);
    //SP module
    mat dcorrForSPConsistencyCal();
    //
    //Pallavi : 
    bool isInsideHexLayout(Location_S nodeLoc);
    void initCorrelatedShdw(ivec shdwNodeTypes, double corrDistInMeters=50, double siteToSiteCorr=0.5);	///< Initialize Correlated Shadow for the nodeTypes specified with the user specified parameters.. Must be called when Shadow mode is selected as correlated
    void setCoverageArea(double xLength, double yLength);	///< Set the coverage area of the network for random cell Layout
    void overwriteHexagonalCoverageArea(double xLength, double yLength);	///< Overwrite the coverage area of the network for hexagonal cell Layout
    void setInterSiteDistanceInMeters(double interSiteDistance);	///< Set Inter Site Distance for Hexagonal cell Layout
    void setMinDistanceFromCellCenter(double minDistance);	/// Set the minimum distance of other nodes from the Hexagonal Nodes ...
    void setCommonVerticalTilt(int commonVerticalTilt);
    void setTerrain(string terrainName);		///< Set Simulation Terrain	
    void setCellLayout(CellLayout_E cellLayout);	///< Set the system Layout for the current simualtion
    void setShadowType(ShadowType_E shadwType);	///< Set System Shadow Type
    //void setCarrierFreqInHz(double carrierFreqinHz);	///< Set Carrier Frequency
    void setNodeCount(string nodeType, int nodeCnt);	///< Set node count for the given node Type	
    void setNodeCount(ivec nodeCnts);	///< Set node count for all node Types
    void nameNodeType(int nodeTypeCnt, string nodeType);	///< Name the nodeType specified with user defined name...
    void nameNodeType(Array<string> nodeTypes);	///< Name the nodeTypes with user defined names...
    void setNodeDropMethod(string nodeType, DropMethod_E dropMethod);	///< Set the node drop method for the given Node Type as hexagonal or random
    void enablePathSpecificAntennaGainInSCM();
    void disablePathSpecificAntennaGainInSCM();
    bool isPathSpecificAntennaGainInSCMEnabled();
    Area_S getCoverageArea();	/// Get the coverage area of the current simulation
    double getMinDistanceFromCellCenter();	/// Returns the minimum distance of other nodes from the Hexagonal Nodes ...
    string getTerrain();		///< Get Simulation Terrain	
    CellLayout_E getCellLayout();	///< Get the current cell Layout model
    ShadowType_E getShadowType();	///< Get System Shadow Type
    double getCarrierFreqInHz();	///< Get Carrier Frequency
    int getCommonVerticalTilt();	///< Get the common vertical tilt of the Sectorized antenna in Hexagonal Nodes
    ivec getNodeCount();	///< Get node count for all node Types	
    int getNodeCount(string nodeType);	///< Get node count for the given node Type	
    int getNodeCount(int nodeTypeCnt);	///< Get node count for the given node Type	
    Location_S getNodeLocation(int nodeID);///< Get node Location of the given nodeID
    Array<Location_S> getNodeLocation(ivec nodeIDs);///< Get node Location of the given nodeIDs
    Array<string> getNodeTypes();	///< Get node Types in the system...
    string getHexagonalDropNodetype(); /// Get the nodeType with hexagonal drop method
    string getInHDropNodetype();
    DropMethod_E getNodeDropMethod(string nodeType);	///< Set the node drop method
    void printConfig(string configFile);	/// Print the current system configuration into the given file
    void printConfig();	/// Print the current system configuration on the screen
   void convertAngleFromLCStoGCS(double& elevationAngle);
    bool isValidLink(int txNodeID, int rxNodeID); /// Return if the link is valid based on the carrier frequency used.    
    void enableCrossLinks();
    void disableCrossLinks();
    void generateCrossLinkInfo(bool retailUnAlteredLinks=false, bool retainOldLOS=false);	/// Generate Cross Link Info of all active nodes
    void generateCrossLinkInfo(Terrain &myTerrain, bool retailUnAlteredLinks=false, bool retainOldLOS=false);	/// Generate Cross Link Info of all active nodes
    void generateCrossLinkInfo(int newNode,bool retailUnAlteredLinks=false, bool retainOldLOS=false);	///< Generate Cross-Link Info from the newNode to all other active nodes
    void generateCrossLinkInfo(int newNode,Terrain &myTerrain, bool retailUnAlteredLinks=false, bool retainOldLOS=false);	///< Generate Link Info from the newNode to all other active nodes
    void loadCarrierAssociationInfo(ivec srvcNodes, ivec associatedServerNodes);
    void updateCarrierAssociationInfo(int srvcNode, int associatedServerNode);
    ivec getServerNodeTypes();///< Get server node Types in the system...
    ivec getServiceNodeTypes();///< Get service node Types in the system...
    
    bool isInCoverage(Location_S location);
    
    void generateLinkInfo(bool retailUnAlteredLinks, bool retainOldLOS);	/// Generate Link Info of all active nodes
//     void generateLinkInfo(Terrain &myTerrain, bool retailUnAlteredLinks, bool retainOldLOS);	/// Generate Link Info of all active nodes
    void generateLinkInfo(int newNode,bool retailUnAlteredLinks, bool retainOldLOS);	///< Generate Link Info from the newNode to all other active nodes
    void generateLinkInfo(int newNode,Terrain &myTerrain, bool retailUnAlteredLinks, bool retainOldLOS);	///< Generate Link Info from the newNode to all other active nodes
    void generateLinkInfo(int newNode,int otherNode,Terrain &myTerrain, bool retailUnAlteredLinks, bool retainOldLOS);	///< Generate Link Info from the newNode to the other node
    void removeLinkInfo(ivec nodes, NodeCategory_E nodeCategory);	///< Remove Link Info of the given nodes from all linked nodes
    
    void printLinkInfo(string filename,ivec serverNodes="",ivec serviceNodes="");	///< Print the Link Info into the given file
    void printLinkGainTable(string filename,ivec serverNodes="",ivec serviceNodes="", bool disableFullLinkGainPrinting = false);	///< Print the Link Budget into the given file
    void printDownlinkPowerMatrix(string filename,ivec serverNodes="",ivec serviceNodes=""); ///< Prints the Downlink Power Matrix into the given file
    void printShadowLossMatrix(string filename,ivec serverNodes="",ivec serviceNodes=""); ///< Prints the Shadow Matrix into the given file
    void printDetailedLinkInfo(string filename,ivec serverNodes="",ivec serviceNodes="");	///< Print the Link Info in detail(separate values of Pathloss, ShadowLoss, AntGain) into the given file
    void printNodeInfo(string filename,ivec nodeList="");	///< Print node info into the given file
    void printNodeLocations(string filename, ivec nodeIDs="");	///< Print node locations into the given file
    void printCrossLinkGainTable(string filename,ivec serverNodes="",ivec serviceNodes="", bool appendInFile=false);	///< Print the Cross Link Table into the given file
    void printCarrierFrequencyInfo(string filename,ivec serverNodes="",ivec serviceNodes="", bool appendInFile=false);
    
    ///Mobility Related
    void initMobilityInfo(string configFile);
    void reinitMobilityDirection(int nodeID=-1);
    void changeMobilityDirection(int servicenodeID, int serverNodeID, double angleWrtToServingRadius);
    MobilityInfo_S getMobilityInfo();
    ChannelGainInfo_S moveServiceNodeForMobility(int nodeID, double currentTimeInSec); ///< Move the node as per the mobilityInfo set and returns the updated channelGainInDB
    ChannelGainInfo_S moveServiceNode(int nodeID, double currentTimeInSec, double distance);
    void moveServiceNodeAlone(int nodeID, double distance);
    Location_S dropInRandomLocation();
    double getMovableDistance(int nodeID, double currentTimeInSec);
    double getDeltaT(int serviceNodeID, double currentTimeInSec);
        
    bool isSysReady();	/// Returns true if all necessary parameters of the system and all modules in it are set
    bool isSysReady(Terrain &myTerrain);	/// Returns true if all necessary parameters of the system and all modules in it are set
    bool isLinkInfoUpToDate(); 	/// Checks whether all link Info in the system is up-to-date
    
    /// Node Related	
    void setNodeHeightByIndex(ivec nodeList, double nodeHeight);	///< Set node height for the given node list	
    void setNodeHeight(string nodeType, double nodeHeight);	///< Set node height for the given node type
    void setNodeHeight(int nodeTypeCnt, double nodeHeight);	///< Set node height for the given node type count
    void setNodeHeightByIndex(ivec nodeList, vec heightList);	///< Set node height for the given node list	
    void setNodeAntennaCount(string nodeType, int antCnt);	///< Set node antenna count for the given node Type	
    void setNodeAntennaCount(int nodeTypeCnt, int antCnt);	///< Set node antenna count for the given node Type count
    void setNodeCarrierFrequency(string nodeType, double carrierFreqinHz);	///< Set node carrier frequency for the given node Type	
    void setNodeCarrierFrequencyInHz(int nodeTypeCnt, double carrierFreqinHz);	///< Set node carrier frequency for the given node Type count
    void setNodeCarrierFrequencyForAllNodes(double carrierFreqInHz, bool& regenLinkInfo);	//< Set node carrier frequency for all nodes
    void setNodeCarrierFrequencyByIndex(ivec nodeList, double carrierFreqinHz);
    void setNodeNoiseFigure(string nodeType, double noiseFigureindB);	///< Set node noiseFigure for the given node Type	
    void setNodeNoiseFigure(int nodeTypeCnt, double noiseFigureindB);	///< Set node noiseFigure for the given node Type count
    void setNodeAntennaCountByIndex(ivec nodeList,  int antCnt);///< Set node antenna count for the given node list	
    void setNodeAntennaSpacing(string nodeType, double wavelengthSpacing);	///< Set node antenna Spacing for the given node Type	
    void setNodeAntennaSpacing(int nodeTypeCnt, double wavelengthSpacing);	///< Set node antenna Spacing for the given node Type count
    void setNodeAntennaSpacingByIndex(ivec nodeList,  double wavelengthSpacing);///< Set node antenna Spacing for the given node list	
    void setNodeNumerologies(string nodeType, ivec nus);
    void setNodeNumerologies(int nodeTypeCnt, ivec nus);
    void setNodeNumerologiesByIndex(ivec nodeList, ivec nus);
    void setNodeIDCell(string nodeType, ivec IDCells);	///< Set node IDcell for the given node type
    void setNodeIDCell(int nodeTypeCnt, ivec IDCells);	///< Set node IDcell for the given node type count
    void setNodeIDCellByIndex(ivec nodeList, ivec IDCells);	///< Set node IDcell for the given node list	
    void setNodeVelocity(string nodeType, vec velocityInMetrePerSec);	///< Set node velocity for the given node type
    void setNodeVelocity(int nodeTypeCnt, vec velocityInMetrePerSec);	///< Set node velocity for the given node type count
    void setNodeVelocityByIndex(ivec nodeList, vec velocityInMetrePerSec);	///< Set node velocity for the given node list	
    void setNodeCentralUnitID(string nodeType, ivec centralUnitIDs);	///< Set node CentralUnitID for the given node type
    void setNodeCentralUnitID(int nodeTypeCnt, ivec centralUnitIDs);	///< Set node CentralUnitID for the given node type count
    void setNodeCentralUnitIDByIndex(ivec nodeList, ivec centralUnitIDs);	///< Set node CentralUnitID for the given node list	
    void setNodeHeight(string nodeType, vec heightList);	///< Set node height of the given node type from the vector of heights given
    void setNodeHeightByIndex(ivec nodeList, double hMin, double hMax);	///< Set node height for the given node list uniformly between hMin and hMax
    void setNodeHeight(string nodeType, double hMin, double hMax);	///< Set node height for the given node list uniformly between hMin and hMax
    void setNodeCategory(string nodeType, NodeCategory_E category);	///< Set the category (server/service node) for the given Node Type	
    void setNodeCategory(int nodeTypeCnt,NodeCategory_E category);///< Set the category (server/service node) for the given Node Type Count	
    void setNodeTypeAsPedestrian(string nodeType, double percentage=100.0);	
    void setNodeListAsPedestrian(ivec nodeList,double percentage=100.0);	
    void setNodeTypeAsIndoor(string nodeType, double percentage=100.0);	///< Set all nodes of the nodeType as Indoor
    void setNodeTypeAsOutdoor(string nodeType,double percentage=100.0);	///< Set all nodes of the nodeType as Outdoor
    void setNodeListAsIndoor(ivec nodeList,double percentage=100.0, bool addFloorHeight=false);	///< Set the nodes in the list as Indoor
    void setNodeTxPowerInDBm(string nodeType,double txPower);	///< Set transmit power of all nodes of the nodeType to the given value
    void setNodeTxPowerInDBm(int nodeTypeCnt,double txPower);	///< Set transmit power of all nodes of the nodeTypeCount to the given value
    void setNodeTxPowerInDBmByIndex(ivec nodeList,double txPower);	///< Set transmit power of all nodes in the list to the given value	
    void activateNodes();	///< Activate all the nodes for generating link info
    void activateNodes(ivec nodeList);	///<  Activate all the nodes from the list for generating link info                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
    void deactivateNodes(ivec nodeList);	///< Deactive all the nodes from the list 
    void moveNode(int nodeID, double xLoc, double yLoc);	///<Move the node to the given XY location
    void moveNode(int nodeID, Location_S newLoc);	///<Move the node to the given location
    void moveNode(ivec nodeList,Array<Location_S> newLocs); 	///<Move the given nodes to the given XY locations
    void shiftNode(int nodeID, double xOffset, double yOffset);	///<Move the node to the given offset
    void shiftNode(int nodeID, Location_S offset);	///<Move the node to the given offset
    void shiftNode(ivec nodeList,Location_S offsets); 	///<Move the given nodes to the given offset
    void shiftNode(ivec nodeList,Array<Location_S> offsets); 	///<Move the given nodes to the given offsets
    void updateNodeLocations(string filename,bool updateLinkInfo=false);
    void moveNodesToProximity(int proximityNodeType,bool updateLinkInfo=false);
    ivec addNode(string nodeType,int numNodes);	///< Adds new Nodes and assigns the Node Type and returns the new Node IDs..
    int addNode(string nodeType ,double xLoc,double yLoc,double nodeHeight);	///< Adds a new Node of given type into the system at the given location
    int addNode(string nodeType,Location_S nodeLoc);	///< Adds a new Node of given type into the system at the given location
    ivec addNode(string nodeType ,Array<Location_S> nodeLocs);	///< Adds new Nodes of given type into the system at the given locations
    int addNode(int hexCellID,string nodeType);	///< Adds a new Node of given type into the system within the given hexagonal cell 
    int addNode(Node newNode);	///< Add the given node object into the system
    /*int duplicateNode(int nodeID,string nodeType="");	///< Create a duplicate node as the given nodeID with a new nodeID and of the given nodeType
    ivec duplicateNodes(ivec nodeIDs,string nodeType="");///< Create duplicate nodes as the given nodeIDs with new nodeIDs and of the given nodeType
   */ 
    int duplicateNode(int nodeID,string nodeType,bool newNodeType=false);	///< Create a duplicate node as the given nodeID with a new nodeID and of the given nodeType
    ivec duplicateNodes(ivec nodeIDs,string nodeType,bool newNodeType=false);///< Create duplicate nodes as the given nodeIDs with new nodeIDs and of the given nodeType
    void createNodeType(string newNodeType); ///Create a new node type with no nodes. All related parameters will be uninitialized
    void setNetworkIDPerNodeType(int newNodesToCreate); //! Create Network-IDs for Second-Network
    void setNodeCategoryForSecondNetwork(string nodeType);//! setting NodeCategory for Second-Network nodes
    //5G
    void setNodeListWithLowPenetrationLoss(ivec nodeList, double percentage);
    //end
    
    int getNearestNode(int nodeID, ivec nodeList);	///< Get the nearest node from the list to the given nodeID 
    int getNearestNode(Location_S loc, ivec nodeList); ///< Get the nearest node from the list to the given location 
        
    ivec getNodes(NodeCategory_E nodeCategory);	///< Get the nodes of the given category
    ivec getNodes(DropMethod_E nodeDropMethod, bool all=false);	///< Get the nodes of the given category
    ivec getNodes(string nodeType);	///< Get the nodes of the given node Type
    ivec getNodes(int nodeTypeCnt);	///< Get the nodes of the given node Type Count
    ivec getNodes(int nodeID, double radius);	///< Get all the nodes within given radius from the given nodeID location
    ivec getNodes(double xLoc, double yLoc, double radius);	///< Get all the nodes within given radius from the given location
    ivec getActiveNodes();	///< Get all the active nodes in the system
    ivec getIndoorNodes();	///< Get all the Indoor nodes in the system
    ivec getOutdoorNodes();	///< Get all the Outdoor nodes in the system
    ivec getActiveNodes(string nodeType);	///< Get all the active nodes of the given type in the system
    ivec getIndoorNodes(string nodeType);	///< Get all the Indoor nodes of the given type in the system
    ivec getOutdoorNodes(string nodeType);	///< Get all the Outdoor nodes of the given type in the system
    ivec getActiveNodesFromList(ivec nodeList);		///< Get all the active nodes from the given list
    ivec getServerNodesFromList(ivec nodeList);		///< Get all the Server nodes from the given list
    ivec getServiceNodesFromList(ivec nodeList);	///< Get all the Service nodes from the given list
    vec getNodeTxPowerInDBm(string nodeType);	///< Get transmitpower of all nodes of the given nodeType
    vec getNodeTxPowerInDBmFromList(ivec nodeList);	///< Get transmitpower of all nodes in the list
    NodeLinkInfo_S getLinkInfo(int txNodeID, int rxNodeID);	///< Get the link info between the given two nodes
    int findHexagonalCellID(int nodeID);	///< Finds the cell ID for the given node 
    int findHexagonalCellID(Location_S loc);///< Finds the cell ID for the given location 
    NodeCategory_E getNodeCategory(string nodeType);	///< Returns the NodeCategory_E of the nodeType if they all follow the same type . Else returns _NODECATEGORY_UNINIT_ with a warning.
    Array<NodeCategory_E> getNodeCategory();
    ivec getNodeIDCell(string nodeType); ///< Returns the ID cell of the specific nodes
    ivec getNodeIDCell(int nodeTypeCnt); ///< Returns the ID cell of the specific nodes
    ivec getNodeIDCellFromList(ivec nodeList); ///< Returns the ID cell of the specific nodes
    vec getNodeVelocity(string nodeType); ///< Returns velocity of the specific nodes
    vec getNodeVelocity(int nodeTypeCnt); ///< Returns velocity of the specific nodes
    vec getNodeVelocityFromList(ivec nodeList); ///< Returns velocity of the specific nodes
    ivec getNodeAntennaCount(string nodeType); ///< Returns the AntennaCount of the specific nodes
    ivec getNodeAntennaCount(int nodeTypeCnt); ///< Returns the AntennaCount of the specific nodes
    double getNodeNoiseFigure(string nodeType); ///< Returns the noiseFigure of the specific nodeType
    double getNodeNoiseFigure(int nodeTypeCnt); ///< Returns the noiseFigure of the specific nodeType
    ivec getNodeAntennaCountFromList(ivec nodeList); ///< Returns the AntennaCount of the specific nodes
    vec getNodeCarrierFreuencyInHz(string nodeType); ///< Returns the carrier frequency of the specific nodes
    vec getNodeCarrierFreuencyInHz(int nodeTypeCnt); ///< Returns the carrier frequency of the specific nodes
    vec getNodeCarrierFreuencyInHzFromList(ivec nodeList); ///< Returns the AntennaCount of the specific nodes
    ivec getNodeCentralUnitID(string nodeType); ///< Returns the CentralUnitID of the specific nodes
    ivec getNodeCentralUnitID(int nodeTypeCnt); ///< Returns the CentralUnitID of the specific nodes
    ivec getNodeCentralUnitIDFromList(ivec nodeList); ///< Returns the CentralUnitID of the specific nodes
    Array<ivec> getNodeNumerologies(string nodeType);
    Array<ivec> getNodeNumerologies(int nodeTypeCnt);
    Array<ivec> getNodeNumerologiesFromList(ivec nodeList);
    ivec getNodeTypeFromList(ivec nodeList); ///< Returns the NodeType of the specific nodes
    
    
    /// Group Related	
    int createNodeGroup(string groupName, ivec nodeList);	///< Group the Nodes under the unique Group Name given	
    void setGroupCarrierFreq(string groupName,double carrierFreqinHz);	///< Set the carrier frequency of all the nodes in the given group
    void setGroupCarrierFreq(int groupID,double carrierFreqinHz);	///< Set the carrier frequency of all the nodes in the given group
    void setGroupCategory(string groupName,NodeCategory_E category);	///< Set the Category of all the nodes in the given group
    void setGroupCategory(int groupID,NodeCategory_E category);		///< Set the Category of all the nodes in the given group
    void setGroupLoc(string groupName, Location_S loc);		///< Set the location of all the nodes in the given group	
    void setGroupLoc(int groupID, Location_S loc);	///< Set the location of all the nodes in the given group	
    void activateNodeGroup(string groupName);	///< Activate the node group
    void activateNodeGroup(int groupID);	///< Activate the node group
    void deactivateNodeGroup(string groupName);	///< Deactivate the node group
    void deactivateNodeGroup(int groupID);	///< Deactivate the node group 
    void setGroupShadowGrid(string groupName, mat shadowGrid);		///< Set the correlated shadow grid of all the nodes in the given group	
    void setGroupShadowGrid(int groupID, mat shadowGrid);	///< Set the correlated shadow grid of all the nodes in the given group	
    void setGroupAntenna(string groupName, Antenna antenna);	///< Set the antenna pattern of all nodes in the group
    void setGroupAntenna(int groupID, Antenna antenna);	///< Set the antenna pattern of all nodes in the group
    void setGroupTxAntenna(string groupName, Antenna antenna);	///< Set the Tx antenna pattern of all nodes in the group
    void setGroupTxAntenna(int groupID, Antenna antenna);	///< Set the Tx antenna pattern of all nodes in the group
    void setGroupRxAntenna(string groupName, Antenna antenna);	///< Set the Rx antenna pattern of all nodes in the group
    void setGroupRxAntenna(int groupID, Antenna antenna);	///< Set the Rx antenna pattern of all nodes in the group
    ivec findGroup(string groupNameToFind, bool partialMatch=false);	///< Finds all the node groups in the system with the given group name
    ivec findGroup(ivec groupIDs, string groupNameToFind,bool partialMatch=false);	///< Finds all the node groups in the list with the given group name
    ivec getNodesInTheGroup(int groupID); ///< Returns all member nodes of the given groupID
    NodeGroup_S getGroupInfo(int groupID);	///< Returns groupInfo of the given groupID
    
    
    /// Channel Related Functions : 
    void transmitData(int txNodeID, int txBeamID,int datasize, Array< cvec >& dataFromAllAntennas, double currentTimeInSec, int overwriteOrAdd=0); //overwriteOrAdd - 0->No Action; 1-> Overwrite; 2->Add
    void initBeamParameters(ivec nodeIDs);
    void initSCMLink(ivec rxNodeIDs, ivec txNodeIDs, bool maintainReciprocity, bool reinitialize = false);
    void initSCMLink(int rxNodeID,int txNodeID, bool maintainReciprocity, bool reinitialize = false);
    void updateSCMLinkForMobility(int rxNodeID, int txNodeID, double deltaT);
    void updateSCMLinkForMobility(int rxNodeID, ivec txNodeIDs, double deltaT);
    void preAllocateSCMLinks(ivec rxNodeIDs, Array<ivec> &txNodeIDs);
    ChannelOutput_S receiveData(int rxNodeID, int rxBeamID, int txNodeID, int ofdmSymbolIndx, double currentTimeInSeconds, double endTimeInSeconds = -1, bool printChannel = false, bool computeSumPower = true, cmat txAntennaCorrelationMatrix = "", cmat rxAntennaCorrelationMatrix = ""); ///< Calls ReceiveData of ChannelCloud along with taking care of SCM initialization
    ChannelOutput_S receiveData(int rxNodeID, int rxBeamID, AntennaCorrelationType_E antCorrelationType, int ofdmSymbolIndx, double currentTimeInSeconds, double endTimeInSeconds = -1, bool printChannel = false, ivec txNodeList = "", ivec txNodesToSumPowerFrom = "" );	///< Calls ReceiveData of ChannelCloud along with taking care of SCM initialization
    ChannelOutput_S generateChannel(int rxNodeID, int rxBeamID, AntennaCorrelationType_E antCorrelationType, double currentTimeInSeconds, int fftSize, ivec txNodeList = "");	///< Calls generateChannel of ChannelCloud along with taking care of SCM initialization
    ChannelMatrix_S generateChannel(int rxNodeID, int rxBeamID, AntennaCorrelationType_E antCorrelationType, vec currentTimeInSeconds, int fftSize, ivec txNodeList = "");	///< Calls generateChannel of ChannelCloud along with taking care of SCM initialization
    
    ChannelMatrix_S generateChannelForAnalogBeams(int rxNodeID, ivec txNodeList, ivec& nTxBeamsPerTxNode, int& nRxBeams, double currentTimeInSec=0.0, bool isChannelGenForRSRP=true);	///< Calls generateChannelForAnalogBeams of ChannelCloud along with taking care of SCM initialization
    ChannelMatrix_S generateChannelForAnalogBeams(int rxNodeID, ivec txNodeList,ivec& nTxPanelsPerTxNode, ivec& nTxBeamsPerTxNode,int& nRxPanels, int& nRxBeams, double currentTimeInSec=0.0, bool isChannelGenForRSRP=true);
    
    ChannelMatrix_S generateChannel(int rxNodeID, int rxBeamID, int txNodeID, vec currentTimeInSeconds, int fftSize, cmat txAntennaCorrelationMatrix = "", cmat rxAntennaCorrelationMatrix = "");	///< Calls generateChannel of ChannelCloud along with taking care of SCM initialization
    ChannelOutput_S generateChannel(int rxNodeID, int rxBeamID, int txNodeID, double currentTimeInSeconds,int fftSize, cmat txAntennaCorrelationMatrix = "", cmat rxAntennaCorrelationMatrix = "");
    Location_S getWrapAroundLocation(int baseNodeID, int nodeID); ///<Gives the wrapAround location of the nodeID w.r.t the baseNodeID
    Location_S getWrapAroundLocation(int baseNodeID, Location_S &location ,int cellIDOfLocation); ///<Gives the wrapAround location w.r.t the baseNodeID

    /// Building Related
    void initIndoorInfo(string configFile, bool createbuildings);
    void dropBuildings();
    void dropInBuildingNodes(string nodeType="");
    void setNonInBuildingIndoorNodes();
    void printBuildingInfo(string filename);	
    int getBuildingCount();
    void generateCoveragePlan();
    ivec findIndoorNodes(ivec &buildingIndx,string nodeType="");
    void printSCMparams(ivec serviceNodes,ivec serverNodes,string filename);
    
//     bool updateLinkInfo(string linkInfoFileName,string lspFileName);// Used to overWrite the node locations and link Info from the file. 
    void setLinkInfo(NodeLinkInfo_S& linkInfo,int rxNodeID);
    void updateNodeLocations(ivec nodeIDs,Array<Location_S> &newLoc,Array<Orientation_S> &newOrient,Array<vec>& newVelocity);
    void clearSCMGrids();
    void copySCMGridsForIntrasiteCorrelation();
    bool updateLocations(string locFileName);
//     void copyLinkInfo(int rxNode,int txNode,NodeLinkInfo_S tempLinkInfo);
    void allocateLinkInfo();
    ivec getNodeCounts(){return nNodes;}
    void computePenetrationLossesForAllServiceNodes();
//     void computeLSPsForLink(int srvrNode,int srvcNode,int isDownlink);
    int getLinkCount();
    void retainLinkInfo(ivec serviceNodes,Array<ivec> serverNodesPerServiceNode);
    void clearRedundantMemory(ivec serviceNodes,Array<ivec> serverNodesPerServiceNode);
    
    vec getAntennaTemperature(ivec nodeIDs);
    vec getNoiseFigureInDB(ivec nodeIDs);
    void setCarrierFrequencyForAllNodes(double carrierFreqInHz,double bandWidthInHz);
    vec getGTRatioInDB(ivec nodeIDs);
    int getFrequencyReuseFactor(){return frequencyReUseFactor;}
    ivec getNodesWithFrequencyID(int satID,int freqID,NodeCategory_E nodeCategory);  

  private :  
    
    
    void loadSystemDefault(); ///< Load all possible default values for uninitialized parameters in the McellSystem
    void loadNodeDefault();	///< Load all possible default values for uninitialized parameters in the Nodes
    Correlatedshadow_S getCorrShdwInfo();	///< Returns Correlated Shadowing Info in the McellSystem
    int getNodeTypeCnt(string nodeType);	///< Returns nodeType count of the given nodeType
    void updateHexagonalCoverageArea();		///< Compute and Update coverageArea for Hexagonal Layout based on ISD
    
    
};

void testFuncMoveNodeLocation(McellSystem& mySys);
void printAntennaGain(double antennaGain,Location_S loc);
double getIndoorDistance(SCMPropagationScenario scmScenario,ChannelModelType channelType);
#endif
