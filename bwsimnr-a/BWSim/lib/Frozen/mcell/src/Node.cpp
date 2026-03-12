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

#include "../include/Node.h"
// #define _CHANN_CALIB_
// #define _PLOT_ANTENNA_PATTERN_
#ifdef _CHANN_CALIB_

typedef std::pair<int, int> pairIxI;
extern NodeLinkInfo_S getLinfoMap(pairIxI P);
extern void setLinkInfoMap(pairIxI P, NodeLinkInfo_S linkInfo);

#endif


Node::Node(){
  
  id=-1;
  hexagonalCellID=-1;
  idCell=-1;
  nAntenna=1;
  centralUnitID=-1;
  groupID.set_size(0);
  carrierFreq=-1;
  txPowerInDBm=-1;
  nodeCategory=_NODECATEGORY_UNINIT_;
  active=true;
  type=-1;
  corrDist=-1;
  shadowGrid.set_size(0,0);
  linkInfoUpdateStatus=false;
  
  aLinkInfo.set_size(0);
  aNeighbourCell.set_size(0);
  scmPropagationScenario=_SCM_UNINIT_;
  velocityInMetrePerSec=0;
  h_phase=0,v_phase=0;
  freqID = 0;
  satelliteID=-1;
}

void Node::selfConfig(string cfile){	
 
  string prefix;
  prefix="NODE"+to_str(id);
	
  vec nodeLoc;
  if(parse(cfile,prefix+".nodeLoc",nodeLoc))
  {
	cout<<"Loading loc of node "<<id<<" from config file... "<<endl;
	loc.x=nodeLoc(0);
	loc.y=nodeLoc(1);
	loc.z=nodeLoc(2);
  }
  
  if(parse(cfile,prefix+".isIndoor",loc.isIndoor))
  {
	if(loc.isIndoor)
	  cout<<"Making node"<< id <<" as inDoor  user as per the config file... "<<endl;
	else
	  cout<<"Making node"<< id <<" as outDoor  user as per the config file... "<<endl;
  }
	
  if(parse(cfile,prefix+".hexagonalCellID",hexagonalCellID))
	cout<<"Loading hexagonalCellID of node "<<id<<" from config file... "<<endl;
  
  if(parse(cfile,prefix+".carrierFreq",carrierFreq))
	cout<<"Loading carrierFreq of node "<<id<<" from config file... "<<endl;
  
  if(parse(cfile,prefix+".active",active))
	cout<<"Loading active/inactive status of node "<<id<<" from config file... "<<endl;
  
   
}

bool Node::isNodeReady(int isHexagonalLayout){
  
  bool isReady=true;
  
  if(isHexagonalLayout && hexagonalCellID==-1)
  {
    cout<<"Node Hexagonal Cell ID not set for node "<<id<<endl;
    isReady=false;
  }
  if(std::isnan(loc.x) || std::isnan(loc.y)) 
  {
    cout<<"Node Location not set for node "<<id<<endl;
    isReady=false;
  }
  if(std::isnan(loc.z))
  {
    cout<<"Node Height not set for node "<<id<<endl;
    isReady=false;
  }
//   if(carrierFreq==-1)
//   {
//     cout<<"Node Carrier Frequency not set for node "<<id<<endl;
//     isReady=false;
//   }
  if(nodeCategory==_NODECATEGORY_UNINIT_) 
  {
    cout<<"Node TxRx mode not set for node "<<id<<endl;
    isReady=false;
  }
  if(txPowerInDBm==-1) 
  {
    cout<<"Node Tx Power not set for node .. Loading 0dBm as default "<<id<<endl;
    txPowerInDBm=0;
  }
  if(txAntenna.isAntennaReady()==false)
  {
    cout<<"Tx Antenna not configured properly in node "<<id<<endl;
    isReady=false;
  }
  if(rxAntenna.isAntennaReady()==false)
  {
    if(txAntenna.isAntennaReady())
    {
      cout<<"Using Tx Antenna as Rx Antenna in node "<<id<<endl;
      txAntenna=rxAntenna;
    }
    else
    {
      cout<<"Rx Antenna not configured properly in node "<<id<<endl;
      isReady=false;
    }
  }
  return(isReady);
}


void Node::setHAPSAntenna(int M, int N, double dv, double dh,
                          double Ge_max, double Am, double SLAv,
                          double phi3dB, double theta3dB,
                          double feederLoss)
{
    txAntenna.setHAPSAntenna(M, N, dv, dh, Ge_max, Am, SLAv, phi3dB, theta3dB, feederLoss);
    rxAntenna.setHAPSAntenna(M, N, dv, dh, Ge_max, Am, SLAv, phi3dB, theta3dB, feederLoss);
}

void Node::setHAPSAntennaTilts(Location_S targetLoc)
{
    // Direction vector from HAPS (this node's loc) to target cell center
    double dx = targetLoc.x - loc.x;
    double dy = targetLoc.y - loc.y;
    double dz = targetLoc.z - loc.z; // negative value if target is below HAPS
    double dist = sqrt(dx*dx + dy*dy + dz*dz);
    
    // Unit vector in GCS direction
    double xt = dx/dist, yt = dy/dist, zt = dz/dist;
    
    // Euler angles to align LCS boresight (+Y_LCS) with the direction vector
    // Using convention: Boresight_GCS = Rz(alpha) * Ry(beta) * Rx(gamma) * [0, 1, 0]^T
    // Gamma (mechanical slant tilt around X): rotates boresight from +Y to -Z.
    // Gamma = arcsin(zt) in degrees. (zt=-1 for nadir -> gamma=-90 deg)
    double gamma = asin(zt) * 180.0/pi;
    
    // Alpha (bearing/azimuth around Z).
    // Boresight XY direction in GCS: [-sin(alpha), cos(alpha)]
    // tan(bearing_gcs) = dx/dy => alpha = atan2(-dx, dy)
    double alpha = atan2(-xt, yt) * 180.0/pi;
    double beta = 0.0;
    
    txAntenna.haps_alpha = alpha;
    txAntenna.haps_beta  = beta;
    txAntenna.haps_gamma = gamma;
    rxAntenna.haps_alpha = alpha;
    rxAntenna.haps_beta  = beta;
    rxAntenna.haps_gamma = gamma;
    
    txAntenna.setParameterUpdateStatus(true);
    rxAntenna.setParameterUpdateStatus(true);
}


void Node::setID(int nodeID,bool overwrite){
  
  if(overwrite || id==-1)
  {
	id=nodeID;
	loc.setLocId(id);
	wrapLoc.setLocId(id);
  }
  else
	cout<<"[both:] Warning... Tyring to overwrite the nodeID already set for node"<<id<<endl;
}

void Node::setIDCell(int idcell){
  
  idCell=idcell;
}

void Node::setCentralUnitID(int cuId){
  
  centralUnitID=cuId;
}

void Node::setNumerologies(ivec nus){
  
  numerologies=nus;
}


void Node::setVelocity(double velocity){
  
  velocityInMetrePerSec=velocity;
  if(velocity!=0)
  {
      h_phase = (2*randu()-1)*180;
      v_phase = 90; // assuming the node is moving in the X-Y plane only. 
  }
}
vec Node::getVelocityVector(bool isCartesian)
{
    if(isCartesian)
        return getCartesianVector(h_phase,v_phase,0,velocityInMetrePerSec).get_col(0);
    return getVectorWithNumbers(3,velocityInMetrePerSec,h_phase,v_phase);
}

void Node::setCoverageInfo(double beamRadius,Location_S centreLoc,int coverageID)
{
    ntnCoverageInfo.centre = centreLoc;
    ntnCoverageInfo.radius = beamRadius;
    ntnCoverageInfo.cellID = coverageID;
    
    setAntennaTilts(centreLoc);
}

void Node::setAntennaTilts(Location_S centreLoc,bool useWrapped)
{
    Location_S nodeLoc = (useWrapped) ? wrapLoc : loc;
    //cout << "useWrapped : "<< useWrapped << "wrapLoc and loc : " << wrapLoc <<","<<loc <<endl;
    //cout << "nodeLoc : " << nodeLoc <<endl; 
    //cout << "bsLoc tilts: " << centreLoc <<endl;
    vec angle = findAngle(nodeLoc,centreLoc);
    //cout << "angle : " << angle <<endl; 
    txAntenna.setTilt(angle(0),angle(1));
    rxAntenna.setTilt(angle(0),angle(1));
    
    txAntenna.setParameterUpdateStatus(true);
    rxAntenna.setParameterUpdateStatus(true);
}

void Node::setHexagonalCellID(int cellId){
  
  hexagonalCellID=cellId;
}

void Node::setNeighbourCell(Array<Neighbour_S> neighbourCellInfo){
  
  aNeighbourCell=neighbourCellInfo;
}

void Node::addToGroup(int groupId){
  
  append(groupID,groupId);
}

void Node::setAsIndoor(){
  
  if(loc.isIndoor==0) linkInfoUpdateStatus=false;
  loc.isIndoor=1;
  loc.isPedestrian=1;
}

void Node::setWithLowPenetrationLoss(){
  
  if(loc.isLowPenetrationLoss==0) linkInfoUpdateStatus=false;
  loc.isLowPenetrationLoss=1;
}

void Node::setAsOutdoor(){
  
  if(loc.isIndoor==1) linkInfoUpdateStatus=false;
  loc.isIndoor=0;
  loc.indoorDistance=0.0;
}

void Node::setAsPedestrian(){
  
  if(loc.isPedestrian==0) linkInfoUpdateStatus=false;
  loc.isPedestrian=1;
}

void Node::setIndoorDistance(double dist)
{
    if(loc.isIndoor)
    {
        loc.indoorDistance = dist;
    }
}

void Node::computePenetrationLoss(double carrierFreqInGHz,ChannelModelType channelType,SCMPropagationScenario scmScenario)
{
    if(loc.isIndoor)
    {
        PenetrationModel_E o2iModel = (loc.isLowPenetrationLoss) ? O_TO_I_PENETRATION_LOW : O_TO_I_PENETRATION_HIGH;
        loc.o2iLoss = computeO2IpenetrationLoss(channelType,o2iModel,carrierFreqInGHz,loc.indoorDistance,(scmScenario == _SCM5G_URBAN_MACRO_ || scmScenario == _SCM5G_URBAN_MICRO_));
        loc.carLoss = 0.0;
    }
    else if(!loc.isPedestrian)
    {
        loc.carLoss = 9.0 /*+ 5.0*randn()*/; //commented temporarily for testing - Dhiv
        loc.o2iLoss = 0.0;
    }
}

void Node::setAsInBuilding(){
  
  if(loc.isInBuilding==0) linkInfoUpdateStatus=false;
  loc.isIndoor=1;
  loc.isInBuilding=1;
}

void Node::setAsOutBuilding(){
  
  if(loc.isInBuilding==1) linkInfoUpdateStatus=false;
  loc.isInBuilding=0;
}

void Node::setActive(){
  
  if(active==false) linkInfoUpdateStatus=false;
  active=true;
  
}

void Node::setInactive(){
  
  if(active==true) linkInfoUpdateStatus=false;
  active=false;
}

void Node::setHeight(double nodeHeight, bool overwrite){
  
  if(overwrite || std::isnan(loc.z)) 
  { 
    loc.z=nodeHeight; 
    wrapLoc.z=nodeHeight; 
    txAntenna.setHeight(nodeHeight,true); 
    rxAntenna.setHeight(nodeHeight,true); 
    linkInfoUpdateStatus=false;
  }
  else cout<<"[both:] Warning .. Trying to overwrite Height of node "<<id<<endl;
}

void Node::setFloorIndex(int floorIndx,bool overwrite)
{
    if(overwrite) 
    { 
        loc.floorNumber=floorIndx; 
        wrapLoc.floorNumber=floorIndx; 
        txAntenna.setFloorNumber(floorIndx,true); 
        rxAntenna.setFloorNumber(floorIndx,true);  
        linkInfoUpdateStatus=false;
    }
    else cout<<"[both:] Warning .. Trying to overwrite Floor Indx of node "<<id<<endl;
}
// void Node::setCarrierFreq(double carrierFreqinHz){
//   
//   carrierFreq=carrierFreqinHz;
//   linkInfoUpdateStatus=false;
// }

void Node::setCarrierFreqInHz(double carrierFreqinHz){
  
  carrierFreq=carrierFreqinHz;
  linkInfoUpdateStatus=false;
}

void Node::setTxPowerInDBm(double txPowerIndBm){
  
  txPowerInDBm=txPowerIndBm;
  linkInfoUpdateStatus=false;
}

void Node::setCategory(NodeCategory_E category){
  
  nodeCategory=category;
  linkInfoUpdateStatus=false;
}
void Node::setType(int nodetype){
  
  type=nodetype;
}

void Node::setLoc(Location_S nodeLoc, bool overwrite){
  
  if(overwrite || std::isnan(loc.x))
  {
	loc=nodeLoc;
	linkInfoUpdateStatus=false;
// 	hexagonalCellID=-1;
	setWrapLoc(loc,true);
  }
  else
  {
    cout<<"[both:] Warning... Trying to overwrite location for node "<<id<<endl;
  }
}


void Node::setWrapLoc(Location_S wrapAroundLoc, bool overwrite){
  
  Location_S antLoc;
  if(overwrite || std::isnan(wrapLoc.x))
  {
    wrapLoc=wrapAroundLoc;
    antLoc=wrapLoc;
    txAntenna.setLoc(antLoc,true);
    rxAntenna.setLoc(antLoc,true);	
  }
  else
  {
    cout<<"[both:] Warning... Trying to overwrite wrap-location for node "<<id<<endl;
  }
}


void Node::setXYLoc(Location_S nodeLoc, bool overwrite){
  
  if(overwrite || std::isnan(loc.x))
  {
    if(loc.isInBuilding==true && nodeLoc.isInBuilding==false)
    {
      cout<<"[both:] Warning.. Trying to move an Inbuilding node to a non-InBuilding location.."<<endl;
      return;
    }
    else if(loc.isInBuilding==false && nodeLoc.isInBuilding==true)
    {
      cout<<"[both:] Warning.. Trying to move a non-Inbuilding node to a InBuilding location.."<<endl;
      return;
    }
    
    loc.x=nodeLoc.x;
    loc.y=nodeLoc.y;
    linkInfoUpdateStatus=false;
//     hexagonalCellID=-1;
    setWrapLoc(loc,true);
  }
  else
  {
      cout<<"[both:] Warning... Trying to overwrite XY-location for node "<<id<<endl;abort();
  }
}

void Node::setXYLoc(double xLoc, double yLoc, bool overwrite){
  
  if(overwrite || std::isnan(loc.x))
  {
	if(loc.isInBuilding==true)
	{
	  cout<<"[both:] Warning.. Setting new XY location for InBuilding node not allowed.."<<endl;
	  return;
	}
    
	loc.x=xLoc;
	loc.y=yLoc;
	linkInfoUpdateStatus=false;
// 	hexagonalCellID=-1;
	setWrapLoc(loc,true);
  }
  else
  {
      cout<<"[both:] Warning... Trying to overwrite XY-location for node "<<id<<endl;abort();
  }
}

void Node::initCorrelatedShdw(Correlatedshadow_S corrShdw){
  
  mat indpShdw,shdw;
  indpShdw = randn(corrShdw.lGridX,corrShdw.lGridY);
  shadowGrid = indpShdw*sqrt(1-corrShdw.siteToSiteCorr) + corrShdw.commnShdw;
  corrDist=corrShdw.corrDist;
  linkInfoUpdateStatus=false;
}

void Node::initGridsForSCMLargeScaleParameters(SCMInfo_S &scmInfo,SCMPropagationScenario scmScenario,InFScenario_E infScenario){
  
  /* Assumed hexagonal grid layout for generation of random values for Large scale parameters
   _ _ _ _
  |_|_|_|_|  0
  |_|_|_|_|  1
  |_|_|_|_|  2
  |_|_|_|_|  3
  0 1 2 3 4
  
  */
  scmPropagationScenario=scmScenario;
  if (scmScenario!=_SCMNTN_RURAL_)
  {
    if(scmScenario==_SCM2D_URBAN_MACRO_ || scmScenario==_SCM2D_URBAN_MICRO_ || scmScenario==_SCM2D_RURAL_MACRO_ || scmScenario==_SCM2D_INDOOR_HOTSPOT_)
    {
        scmGrids.set_length(1);
        scmGrids(0).set_length(5);
        int rowCount, colCount;
        if(scmPropagationScenario==_SCM2D_URBAN_MICRO_)
        {
        
        int size = 3; // LOS , NLOS , O2I
        for(int lsp = 0; lsp < 5 ; lsp++) // DS,ASD,ASA,SF,K
        {
        scmGrids(0)(lsp).set_length(size);
        for(int indx = 3;indx<6;indx++) // LOS, NLOS, O2I
        {
        rowCount = (ceil) ((scmInfo.coverageArea.yLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
        colCount = (ceil) ((scmInfo.coverageArea.xLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
        if(lsp == 4 && (indx == 4 || indx == 5)) // For K and (NLOS or O2I)
        {
            rowCount = 0; colCount = 0;
        }
        
        scmGrids(0)(lsp)(indx%3) = randn(rowCount,colCount);
        }
        }
        }else if(scmPropagationScenario==_SCM2D_URBAN_MACRO_)
        {
        int size = 3; // LOS , NLOS
        for(int lsp = 0; lsp < 5 ; lsp++) // DS,ASD,ASA,SF,K
        {
        scmGrids(0)(lsp).set_length(size);
        for(int indx = 6;indx<8;indx++) // LOS, NLOS, O2I
        {
        rowCount = (ceil) ((scmInfo.coverageArea.yLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
        colCount = (ceil) ((scmInfo.coverageArea.xLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
        if(lsp == 4 && indx == 7) // For K and NLOS
        {
            rowCount = 0; colCount = 0;
        }
        scmGrids(0)(lsp)(indx%3) = randn(rowCount,colCount);
        }
        }
        }else if(scmPropagationScenario==_SCM2D_RURAL_MACRO_)
        {
        int size = 2; // LOS , NLOS
        for(int lsp = 0; lsp < 5 ; lsp++) // DS,ASD,ASA,SF,K
        {
        scmGrids(0)(lsp).set_length(size);
        for(int indx = 9;indx<11;indx++) // LOS, NLOS, O2I
        {
        rowCount = (ceil) ((scmInfo.coverageArea.yLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
        colCount = (ceil) ((scmInfo.coverageArea.xLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
        if(lsp == 4 && indx == 10) // For K and NLOS
        {
            rowCount = 0; colCount = 0;
        }
        scmGrids(0)(lsp)(indx%3) = randn(rowCount,colCount);
        }
        }
        }
        else if(scmPropagationScenario==_SCM2D_INDOOR_HOTSPOT_)
        {
        int size = 2; // LOS , NLOS
        for(int lsp = 0; lsp < 5 ; lsp++) // DS,ASD,ASA,SF,K
        {
        scmGrids(0)(lsp).set_length(size);
        for(int indx = 0;indx<2;indx++) // LOS, NLOS
        {
        rowCount = (ceil) ((scmInfo.coverageArea.yLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
        colCount = (ceil) ((scmInfo.coverageArea.xLen/(5*scmInfo.correlationDistance(lsp,indx))) + 1);
        if(lsp == 4 && indx == 1) // For K and NLOS
        {
            rowCount = 0; colCount = 0;
        }
        scmGrids(0)(lsp)(indx) = randn(rowCount,colCount);
        }
        }
        }
    }
    else if(scmScenario==_SCM3D_URBAN_MICRO_ || scmScenario==_SCM3D_URBAN_MACRO_)
    {   
        int rowCount, colCount;
        int numberOfFloorsFor3D=8;
        scmGrids.set_length(numberOfFloorsFor3D);
        
        for(int floorIndx=0;floorIndx<numberOfFloorsFor3D;floorIndx++)//This is for to have uncorrelated lsps for each floor
        {
        scmGrids(floorIndx).set_length(7);// DS,ASD,ASA,SF,K,ZSA,ZSD	
        if(scmScenario==_SCM3D_URBAN_MICRO_)
        {
        
        int size = 3; // LOS , NLOS , O2I
        for(int lsp = 0; lsp < 7 ; lsp++) // DS,ASD,ASA,SF,K,ZSA,ZSD
        {
            scmGrids(floorIndx)(lsp).set_length(size);
            for(int indx = 0;indx<3;indx++) // LOS, NLOS, O2I
            {
            rowCount = (ceil) ((scmInfo.coverageArea.yLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
            colCount = (ceil) ((scmInfo.coverageArea.xLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
            if(lsp == 4 && (indx == 1 || indx == 2)) // For K --->(NLOS and O2I)
            {
            rowCount = 0; colCount = 0;
            }
            
            scmGrids(floorIndx)(lsp)(indx) = randn(rowCount,colCount);
            }
        }
        }else if(scmScenario==_SCM3D_URBAN_MACRO_)
        {
        int size = 3; // LOS , NLOS, O2I
        for(int lsp = 0; lsp < 7 ; lsp++) // DS,ASD,ASA,SF,K,ZSA,ZSD
        {
            scmGrids(floorIndx)(lsp).set_length(size);
            for(int indx = 3;indx<6;indx++) // LOS, NLOS, O2I
            {
            rowCount = (ceil) ((scmInfo.coverageArea.yLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
            colCount = (ceil) ((scmInfo.coverageArea.xLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
            if(lsp == 4 && (indx == 4 || indx == 5)) // For K --->(NLOS and O2I)
            {
            rowCount = 0; colCount = 0;
            }
            scmGrids(floorIndx)(lsp)(indx%3) = randn(rowCount,colCount);
            }
        }
        }
        }
        
    }
    else if(scmScenario==_SCM5G_URBAN_MICRO_ || scmScenario== _SCM5G_URBAN_MACRO_ ||scmScenario== _SCM5G_RURAL_MACRO_ ||scmScenario== _SCM5G_INDOOR_OFFICE_) //5G Channel Model
    {
        // FIXME! Using same grid as 3DSCM. If changes in 5G CM need to be changed
        bool useDiffSCMgrid = true;
        if(useDiffSCMgrid)
        {
            linkInfoUpdateStatus=false;
//             cout<<"Entring into grid place 1:"<<endl;
            return initGridsFor5GSCMLargeScaleParameters(scmGrids,&scmInfo,scmPropagationScenario);
        }
        int numberOfFloorsFor3D = 8;//as per TR 36.893
        scmGrids.set_length(numberOfFloorsFor3D);
        
        for(int floorIndx=0;floorIndx<numberOfFloorsFor3D;floorIndx++)//This is for to have uncorrelated lsps for each floor
        {
        scmGrids(floorIndx).set_length(7);// DS,ASD,ASA,SF,K,ZSA,ZSD
        int rowCount, colCount;
        if(scmScenario==_SCM5G_URBAN_MICRO_)
        {
        int size = 3; // LOS , NLOS , O2I
        for(int lsp = 0; lsp < 7 ; lsp++) // DS,ASD,ASA,SF,K,ZSA,ZSD
        {
        scmGrids(floorIndx)(lsp).set_length(size);
        for(int indx = 0;indx<3;indx++) // LOS, NLOS, O2I
        {
            rowCount = (ceil) ((scmInfo.coverageArea.yLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
            colCount = (ceil) ((scmInfo.coverageArea.xLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
            if(lsp == 4 && (indx == 1 || indx == 2)) // For K --->(NLOS and O2I)
            {
            rowCount = 0; colCount = 0;
            }
            scmGrids(floorIndx)(lsp)(indx) = randn(rowCount,colCount);
        }
        }
        }
        else if(scmScenario==_SCM5G_URBAN_MACRO_)
        {
        int size = 3; // LOS , NLOS, O2I
        for(int lsp = 0; lsp < 7 ; lsp++) // DS,ASD,ASA,SF,K,ZSA,ZSD
        {
        scmGrids(floorIndx)(lsp).set_length(size);
        for(int indx = 3;indx<6;indx++) // LOS, NLOS, O2I
        {
            rowCount = (ceil) ((scmInfo.coverageArea.yLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
            colCount = (ceil) ((scmInfo.coverageArea.xLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
            if(lsp == 4 && (indx == 4 || indx == 5)) // For K --->(NLOS and O2I)
            {
            rowCount = 0; colCount = 0;
            }
            scmGrids(floorIndx)(lsp)(indx%3) = randn(rowCount,colCount);
        }
        }
        }
        else if(scmScenario==_SCM5G_RURAL_MACRO_||scmScenario==_SCMNTN_RURAL_)
        {
        int size = 3; // LOS , NLOS, O2I
        for(int lsp = 0; lsp < 7 ; lsp++) // DS,ASD,ASA,SF,K,ZSA,ZSD
        {
        scmGrids(floorIndx)(lsp).set_length(size);
        for(int indx = 6;indx<9;indx++) // LOS, NLOS, O2I
        {
            rowCount = (ceil) ((scmInfo.coverageArea.yLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
            colCount = (ceil) ((scmInfo.coverageArea.xLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
            if(lsp == 4 && (indx == 7||(indx == 8))) // For K --->(NLOS)
            {
            rowCount = 0; colCount = 0;
            }
            scmGrids(floorIndx)(lsp)(indx%3) = randn(rowCount,colCount);
            cout<<"__scmGrids(floorIndx)(lsp)(indx%3)"<<scmGrids(floorIndx)(lsp)(indx%3)<<endl;

        }
        }
        }
        else if(scmScenario==_SCM5G_INDOOR_OFFICE_)
        {
        int size = 2; // LOS , NLOS, O2I
        for(int lsp = 0; lsp < 7 ; lsp++) // DS,ASD,ASA,SF,K,ZSA,ZSD
        {
        scmGrids(floorIndx)(lsp).set_length(size);
        for(int indx = 9;indx<11;indx++) // LOS, NLOS, O2I
        {
            rowCount = (ceil) ((scmInfo.coverageArea.yLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
            colCount = (ceil) ((scmInfo.coverageArea.xLen/(7*scmInfo.correlationDistance(lsp,indx))) + 1);
            if(lsp == 4 && (indx == 10)) // For K --->(NLOS)
            {
            rowCount = 0; colCount = 0;
            }
            scmGrids(floorIndx)(lsp)(indx%3) = randn(rowCount,colCount);
        }
        }
        }
        }//end for floorIndx
    }
    else if(scmScenario==_SCM5G_INDOOR_FACTORY_)
    {
        initGridsFor5GSCMLargeScaleParameters(scmGrids,&scmInfo,scmPropagationScenario,infScenario); 
    }
    else
    {
        cout<<"Node.cpp....Unsupported IMT Terrain for initGridsForSCMLargeScaleParameters()..."<<endl;abort();
    }
  }
  linkInfoUpdateStatus=false;
}

void Node::initGridsForSpatialConsistencyParameters(SCMInfo_S &scmInfo,SCMPropagationScenario scmScenario){
  
  /* Assumed hexagonal grid layout for generation of random values for spatialConsistency parameters
   *  _ _ _ _
   *  |_|_|_|  0 Cluster& ray spacific random variables
   *  |_|_|_|  1 LOS/NLOS state
   *  |_|_|_|  2 Indoor/outdoor state
   *   0 1 2  
   *  
   */
  
  //! Grid for Indoor/Ourdoor state not considered as we are controlling this from config file for now. 
  //! So spatialConsistency for this SCP is not followed - Dhiv
  
  
  
  if(scmScenario!=_SCMNTN_RURAL_)
  {
    dcorrForSPConsistency = scmInfo.dcorrForSPConsistency;
    if(scmScenario==_SCM5G_URBAN_MICRO_ || scmScenario== _SCM5G_URBAN_MACRO_ ||scmScenario== _SCM5G_RURAL_MACRO_ ||scmScenario== _SCM5G_INDOOR_OFFICE_ || scmScenario==_SCM5G_INDOOR_FACTORY_/*||scmScenario==_SCMNTN_RURAL_*/) //5G Channel Model
    {
        // FIXME! Using same grid as 3DSCM. If changes in 5G CM need to be changed
        scmPropagationScenario=scmScenario; 
        // Assumed 2D random process (in horizontal plane only) as per TR38.901 
        scmGridsForSpConsist.set_length(3);  // scmGridsForSpConsist(parameter)(indoo/outdoo.o2i)(grid)
        // scp =  spacial Consistency parameters
        int rowCount, colCount;
        if(scmScenario==_SCM5G_URBAN_MICRO_)
        {
            for(int scp = 0; scp < 3 ; scp++) // Cluster& ray spacific random variables,LOS/NLOS state,Indoor/outdoor state
            {
                if(scp == 0) // only for Cluster& ray spacific random variables
                    scmGridsForSpConsist(scp).set_length(3);// LOS, NLOS, O2I
                else
                    scmGridsForSpConsist(scp).set_length(1);
                    
                if(scp == 0) // only for Cluster& ray spacific random variables
                {
                    for(int indx = 0;indx<3;indx++) // LOS, NLOS, O2I
                    {
                        rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp/2,indx))) + 1);
                        colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp/2,indx))) + 1);
                        scmGridsForSpConsist(scp)(indx%3) = randi(rowCount,colCount,1,100000);	
                    }
                }
                else if(scp==1)
                {
                    rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp/2,0))) + 1);
                    colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp/2,0))) + 1);
                    
                    vec gridCornerYLoc = to_vec(getIntegers(0,rowCount))*(3*scmInfo.dcorrForSPConsistency(scp/2,0))-scmInfo.coverageArea.yLen/2.0;
                    vec gridCornerXLoc = to_vec(getIntegers(0,colCount))*(3*scmInfo.dcorrForSPConsistency(scp/2,0))-scmInfo.coverageArea.xLen/2.0;

                    scmGridsForSpConsist(scp)(0)=zeros_i(rowCount,colCount);
                    //LOS values assigned to grid corners assuming the serverNode is in the centre of the grid
                    for(int r=0;r<rowCount;r++)
                        for(int c=0;c<colCount;c++)
                        {
                            double distanceFromCentre  = findDistance(Location_S(gridCornerXLoc(c),gridCornerYLoc(r),0.0),Location_S(0.0,0.0,0.0));
                            scmGridsForSpConsist(scp)(0)(r,c) = isLOS_UMi(distanceFromCentre)==0 ? -1 : 1;
                        }

                    break;
                }
            }
            
        }
        else if(scmScenario==_SCM5G_URBAN_MACRO_)
        {
            for(int scp = 0; scp < 3 ; scp++) // Cluster& ray spacific random variables,LOS/NLOS state,Indoor/outdoor state
            {
                if(scp == 0) // only for Cluster& ray spacific random variables
                    scmGridsForSpConsist(scp).set_length(3);// LOS, NLOS, O2I
                else
                    scmGridsForSpConsist(scp).set_length(1);
                
                if(scp == 0) // only for Cluster& ray spacific random variables
                {
                    for(int indx = 3;indx<6;indx++) // LOS, NLOS, O2I
                    {
                        rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp/2,indx))) + 1);
                        colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp/2,indx))) + 1);
                        scmGridsForSpConsist(scp)(indx%3) = randi(rowCount,colCount,1,100000);	
                    }
                }
                else if(scp==1)
                {
                    rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp/2,3))) + 1);
                    colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp/2,3))) + 1);
                    
                    vec gridCornerYLoc = to_vec(getIntegers(0,rowCount))*(3*scmInfo.dcorrForSPConsistency(scp/2,3))-scmInfo.coverageArea.yLen/2.0;
                    vec gridCornerXLoc = to_vec(getIntegers(0,colCount))*(3*scmInfo.dcorrForSPConsistency(scp/2,3))-scmInfo.coverageArea.xLen/2.0;
                    
                    //LOS values assigned to grid corners assuming the serverNode is in the centre of the grid
                    scmGridsForSpConsist(scp)(0)=zeros_i(rowCount,colCount);
                    for(int r=0;r<rowCount;r++)
                        for(int c=0;c<colCount;c++)
                        {
                            double distanceFromCentre  = findDistance(Location_S(gridCornerXLoc(c),gridCornerYLoc(r),0.0),Location_S(0.0,0.0,0.0));
                            // Assuming UE height as zero to avoid 3D grid for LOS. Will be fine for Mobility simulations for now - Dhiv 
                            scmGridsForSpConsist(scp)(0)(r,c) = isLOS_UMa(distanceFromCentre,0.0)==0 ? -1 : 1; 
                        }	
                        
                    break;
                }
            }
        } 
        else if(scmScenario==_SCM5G_RURAL_MACRO_||scmScenario==_SCMNTN_RURAL_)
        {
            for(int scp = 0; scp < 3 ; scp++) // Cluster& ray spacific random variables,LOS/NLOS state,Indoor/outdoor state
            {
                if(scp == 0) // only for Cluster& ray spacific random variables
                    scmGridsForSpConsist(scp).set_length(3);// LOS, NLOS, O2I
                else
                    scmGridsForSpConsist(scp).set_length(1);
                
                if(scp == 0) // only for Cluster& ray spacific random variables
                {
                    for(int indx = 6;indx<9;indx++) // LOS, NLOS, O2I
                    {
                        rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp,indx))) + 1);
                        colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp,indx))) + 1);
                        scmGridsForSpConsist(scp)(indx%3) = randi(rowCount,colCount,1,100000);	
                    }
                }
                else if(scp==1)
                {
                    rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp,6))) + 1);
                    colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp,6))) + 1);
                    vec gridCornerYLoc = to_vec(getIntegers(0,rowCount))*(3*scmInfo.dcorrForSPConsistency(scp/2,6))-scmInfo.coverageArea.yLen/2.0;
                    vec gridCornerXLoc = to_vec(getIntegers(0,colCount))*(3*scmInfo.dcorrForSPConsistency(scp/2,6))-scmInfo.coverageArea.xLen/2.0;
                    
                    //LOS values assigned to grid corners assuming the serverNode is in the centre of the grid
                    scmGridsForSpConsist(scp)(0)=zeros_i(rowCount,colCount);
                    for(int r=0;r<rowCount;r++)
                        for(int c=0;c<colCount;c++)
                        {
                            double distanceFromCentre  = findDistance(Location_S(gridCornerXLoc(c),gridCornerYLoc(r),0.0),Location_S(0.0,0.0,0.0));
                            // Assuming UE height as zero to avoid 3D grid for LOS. Will be fine for Mobility simulations for now - Dhiv 
                            scmGridsForSpConsist(scp)(0)(r,c) = isLOS_RMa(distanceFromCentre)==0 ? -1 : 1; 
                        }	
                        
                    break;
                }    
            }
        }
        else if(scmScenario==_SCM5G_INDOOR_OFFICE_)
        {
            for(int scp = 0; scp < 2 ; scp++) // Cluster& ray spacific random variables,LOS/NLOS state
            {
                scmGridsForSpConsist(scp).set_length(1);
                
                rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp,9))) + 1);
                colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp,9))) + 1);
                scmGridsForSpConsist(scp)(0) = randi(rowCount,colCount,1,100000);
            }
        }
        else if(scmScenario==_SCM5G_INDOOR_FACTORY_)
        {
            for(int scp = 0; scp < 2 ; scp++) // Cluster& ray spacific random variables,LOS/NLOS state
            {
                scmGridsForSpConsist(scp).set_length(1);
                
                rowCount = (ceil) ((scmInfo.coverageArea.yLen/(3*scmInfo.dcorrForSPConsistency(scp,11))) + 1);
                colCount = (ceil) ((scmInfo.coverageArea.xLen/(3*scmInfo.dcorrForSPConsistency(scp,11))) + 1);
                scmGridsForSpConsist(scp)(0) = randi(rowCount,colCount,1,100000);
            }
        }
    }
    else
    {
        
        cout<<"Node.cpp....Unsupported Terrain for initGridsForSpatialConsistencyParameters()..."<<endl;abort();
    }
  }
  linkInfoUpdateStatus=false;
}

void Node::setShadowGrid(mat corrShdwGrid){
  
  shadowGrid=corrShdwGrid;
  if(corrDist!=-1) linkInfoUpdateStatus=false;
}

void Node::setNumOfAntenna(int antCnt){

  nAntenna=antCnt;
}

void Node::setAntenna(AntennaType_E antennaType,double antGain,double feederLoss,double radius,double temperature,double gtRatio)
{
    txAntenna.setPattern(_ANTENNA_TYPE_CIRCULAR_,-1,-1);
    txAntenna.setGain(antGain);
    txAntenna.setAperture(radius);
    txAntenna.setTemperature(temperature,gtRatio);
    txAntenna.setAntennaFeederLoss(feederLoss);
    rxAntenna = txAntenna;
    linkInfoUpdateStatus=false;
}

void Node::setAntenna(AntennaType_E antennaType,double antGain,double feederLoss, double horBW, double horTilt,double hMaxAtt,double verBW, double verTilt,double vMaxAtt)
{
    if(antennaType==_ANTENNA_TYPE_OMNI_)
        txAntenna.setPattern(antennaType,zeros(1),zeros(1));
    else
        txAntenna.setPattern(antennaType,horBW ,verBW);
    
    txAntenna.setGain(antGain);
    txAntenna.setMaxAttenuation(hMaxAtt,vMaxAtt);
    txAntenna.setTilt(horTilt,verTilt);
    txAntenna.setAntennaFeederLoss(feederLoss);
    
    rxAntenna=txAntenna;
    linkInfoUpdateStatus=false;
}

void Node::setAntenna(AntennaType_E antennaType,double antGain,double feederLoss, double hTilt, double vTilt, vec hPattern,vec vPattern)
{
    txAntenna.setPattern(antennaType,hPattern,vPattern);
    txAntenna.setGain(antGain);
    txAntenna.setTilt(hTilt,vTilt);
    txAntenna.setAntennaFeederLoss(feederLoss);
    rxAntenna=txAntenna;
    linkInfoUpdateStatus=false;
}  

void Node::setAntenna(Antenna newAntenna){
  
  txAntenna=newAntenna;
  rxAntenna=newAntenna;
  setWrapLoc(loc,true);
  txAntenna.setAntennaType(_ANTENNA_TYPE_CUSTOM_);
  rxAntenna.setAntennaType(_ANTENNA_TYPE_CUSTOM_);
  linkInfoUpdateStatus=false;
}
void Node::setTxAntenna(Antenna newAntenna){
  
  txAntenna=newAntenna;
  setWrapLoc(loc,true);
  txAntenna.setAntennaType(_ANTENNA_TYPE_CUSTOM_);
  linkInfoUpdateStatus=false;
}
void Node::setRxAntenna(Antenna newAntenna){
  
  rxAntenna=newAntenna;
  setWrapLoc(loc,true);
  rxAntenna.setAntennaType(_ANTENNA_TYPE_CUSTOM_);
  linkInfoUpdateStatus=false;
}
void Node::setLinkInfoUpdateStatus(bool status){
  
  linkInfoUpdateStatus=status;
}

void Node::overwriteLinkInfo(int txNodeID, string propertyName, double propertyValue) 
{
  for(int node_cnt=0;node_cnt<aLinkInfo.length();node_cnt++)
	if(aLinkInfo(node_cnt).id==txNodeID)
	{
	  if(propertyName=="pathLoss"){ aLinkInfo(node_cnt).pathLoss=propertyValue;return;}
	  if(propertyName=="isLOS"){ aLinkInfo(node_cnt).isLOS=propertyValue;return;}
	  if(propertyName=="shadowLoss"){ aLinkInfo(node_cnt).shadowLoss=propertyValue;return;}
	  if(propertyName=="txAntennaGain"){ aLinkInfo(node_cnt).txAntennaGain=propertyValue;return;}
	  if(propertyName=="rxAntennaGain"){ aLinkInfo(node_cnt).rxAntennaGain=propertyValue;return;}
	  cout<<"[both:] Unknown propertyName to overwriteLinkInfo()...";exit(0);
	}
	
	cout<<"[both:] Linkinfo for the given NodeID not available in the current node in overwriteLinkInfo()..."<<endl;exit(0);
}

void Node::setSCMGridsForSpConsist(Array< Array<imat> > gridsForSpConsist)
{
  scmGridsForSpConsist = gridsForSpConsist;
}

void Node::overwriteLinkInfo(int txNodeID, SCMLargeScaleParameters_S &scmLargeScaleParameters) 
{
  for(int node_cnt=0;node_cnt<aLinkInfo.length();node_cnt++)
    if(aLinkInfo(node_cnt).id==txNodeID)
    {
      aLinkInfo(node_cnt).scmLargeScaleParams=scmLargeScaleParameters;
      return;
    }

  cout<<"[both:] Linkinfo for the given NodeID not available in the current node in overwriteLinkInfo()..."<<endl;exit(0);
}

NodeCategory_E Node::getCategory(){
  
  return(nodeCategory);
}


void Node::clearShadowGrid(){
  
  shadowGrid.set_size(0,0);
}

int Node::getType(){
  
  return(type);
}

mat Node::getShadowGrid(){
  
  return(shadowGrid);
}

double Node::getCorrelatedDistance(){
  
  return(corrDist);
}

bool Node::isActive(){
  
  return(active);
}

bool Node::isIndoor(){
  
  return(loc.isIndoor);
}

bool Node::isPedestrian(){
  
  return(loc.isPedestrian);
}

bool Node :: isWithLowPenetrationLoss()
{
  return(loc.isLowPenetrationLoss);
}

bool Node :: isWithHighPenetrationLoss()
{
  return(!(loc.isLowPenetrationLoss));
}

bool Node::isOutdoor(){
  
  return(!loc.isIndoor);
}

bool Node::isLinked(int txNodeID){
  
  if(find(linkedTxNodes,txNodeID)!=-1)
    return(true);
  else
    return(false);
}

bool Node::isHavingSCMGrid(){
  
  if(scmGrids.length()!=0)
    return true;
  else
    return false;
}

int Node::getID(){
  
  return(id);
}

int Node::getIDCell(){
  
  return(idCell);
}

double Node::getCarrierFreqInHz(){
  
  return(carrierFreq);
}

int Node::getCellID(){
  
  return(hexagonalCellID);
}

int Node::getCentralUnitID(){
  
  return(centralUnitID);
}

ivec Node::getNumerologies(){
  
  return(numerologies); 
}



double Node::getVelocity(){
  
  return(velocityInMetrePerSec);
}

double Node::getTxPowerInDBm(){
  
  return(txPowerInDBm);
}

ivec Node::getGroupID(){
  
  return(groupID);
}

Location_S Node::getLoc(){
  
  return(loc);
}

double Node::getHeight(){
  
  return(loc.z);
}

Location_S Node::getWrapLoc(){
  
  return(wrapLoc);
}

Array<Neighbour_S> Node::getNeighbourInfo(){
  
  if(aNeighbourCell.length()==0)
  {
    cout<<"[both:] Trying to access Neighbour cells which are not generated in node "<<id<<endl;
    abort();
  }
  return(aNeighbourCell);
}

Location_S Node::getNeighbourCellLoc(int neighbourCellID){
  
  if(aNeighbourCell.length()==0)
  {
	cout<<"[both:] Trying to access Neighbour cells which are not generated in node "<<id<<endl;
	abort();
  }
  for(int neighbour_cnt=0;neighbour_cnt<aNeighbourCell.length();neighbour_cnt++)
  {
 	if(aNeighbourCell(neighbour_cnt).cellID==neighbourCellID)
	  return(aNeighbourCell(neighbour_cnt).cellXYLocation);
  }
  
  cout<<"[both:] Unknown neighbourCellID "<<neighbourCellID<<" in getNeighbourCellLoc()..."<<endl;
  exit(0);
}

int Node::getAntennaCount(){
  
  return(nAntenna);
}

void Node::setOrientation(double a,double b,double g)
{
    nodeOrient.alpha=a;nodeOrient.beta=b;nodeOrient.gamma=g;
}

double Node::getAntennaGain(Location_S newNodeLoc){
  
  
  return(txAntenna.getAntennaGain(newNodeLoc,carrierFreq));
}

double Node::getTxAntennaGain(Location_S newNodeLoc){
  
  //abort();
  return(txAntenna.getAntennaGain(newNodeLoc,carrierFreq));
}

double Node::getRxAntennaGain(Location_S newNodeLoc){
  
  return(rxAntenna.getAntennaGain(newNodeLoc,carrierFreq));
}

double Node::get3DTxAntennaGain(int /*newNodeType*/, Location_S newNodeLoc,double carrierFreq){
    //abort();
    return(txAntenna.get3DAntArrayGainForPort0(newNodeLoc,carrierFreq));
}

double Node::get3DRxAntennaGain(int /*newNodeType*/, Location_S newNodeLoc,double carrierFreq){
  
  
//   cout<<"newNodeLoc"<<newNodeLoc<<endl;
  
  return(rxAntenna.get3DAntArrayGainForPort0(newNodeLoc,carrierFreq));
}

Array< Array<imat> > Node::getSCMGridsForSpConsist()
{
  return(scmGridsForSpConsist);
}
int Node::getLinkCount(){
  
  return(aLinkInfo.length());
}

ivec Node::getLinkedTxNodes(){
  
  ivec linkedNodes(0);
  for(int node_cnt=0;node_cnt<aLinkInfo.length();node_cnt++)
    append(linkedNodes,aLinkInfo(node_cnt).id);
  return(linkedNodes);
}

void Node::updateLocation(Location_S& location)
{
    loc = location;
    wrapLoc.z = location.z;
    txAntenna.setLoc(location,true);
    rxAntenna.setLoc(location,true);
}

void Node::updateLinkInfo(NodeLinkInfo_S& linkInfo)
{
    int linkIndx = find(linkedTxNodes,linkInfo.id);
    if(linkIndx!=-1)
    {
        aLinkInfo(linkIndx).load(linkInfo);
        aLinkInfo(linkIndx).txAntennaGain = linkInfo.txAntennaGain;
        aLinkInfo(linkIndx).rxAntennaGain = linkInfo.rxAntennaGain;
        return;
    }
    cout<<"Unknown Link with ID: "<<linkInfo.id<<" in updateLinkInfo()"<<endl;abort();
}

NodeLinkInfo_S Node::getLinkInfo(int txNodeID){
  
  if(linkInfoUpdateStatus==false){ cout<<"[both:] Warning .... LinkInfo not up-to-date for node "<<id<<" while calling getLinkInfo()..."<<endl;abort();}
  int linkInfoIndx = find(linkedTxNodes,txNodeID);
  //cout<<"linkedTxNodes_txNodeID_check"<<linkedTxNodes<<","<<txNodeID<<endl;
  if(linkInfoIndx!=-1)
  {
      if(aLinkInfo(linkInfoIndx).id!=txNodeID)
      {
            cout<<"Improper mapping of nodeID and linkInfo..."<<endl;abort();
      }
      //cout<<"aLinkInfo(linkInfoIndx)"<<aLinkInfo(linkInfoIndx)<<endl;
      return aLinkInfo(linkInfoIndx);

  }
  cout<<"[both:] Given node "<<txNodeID <<" not linked with the current node in getLinkInfo()..."<<endl;
  abort();		
}

NodeLinkInfo_S Node::getLinkInfoByIndex(int index){

  if(linkInfoUpdateStatus==false){ cout<<"LinkInfo not up-to-date for node "<<id<<" while calling getLinkInfo()..."<<endl;abort();}
  if(index>=aLinkInfo.length())
	{cout<<"[both:] Link Index out of range in getLinkInfo()...";exit(0);}
  return(aLinkInfo(index));  
}

int Node::getLinkInfoIndx(int nodeID){
  
  return(find(linkedTxNodes,nodeID));
}

void Node::setLinkInfo(NodeLinkInfo_S& linkInfo)
{
    int linkIndx = find(linkedTxNodes,linkInfo.id);
    if(linkIndx==-1)
    {
        append(linkedTxNodes,linkInfo.id);
        append(aLinkInfo,linkInfo);
    }
    else
        updateLinkInfo(linkInfo);
}

double Node::getRxPower(int txNodeID){
  
  if(linkInfoUpdateStatus==false){ cout<<"LinkInfo not up-to-date for node "<<id<<" while calling getRxPower()..."<<endl;abort();}
  
  for(int node_cnt=0;node_cnt<aLinkInfo.length();node_cnt++)
	if(aLinkInfo(node_cnt).id==txNodeID)
	  return(aLinkInfo(node_cnt).txPowerInDBm+aLinkInfo(node_cnt).txAntennaGain+aLinkInfo(node_cnt).rxAntennaGain-aLinkInfo(node_cnt).pathLoss-aLinkInfo(node_cnt).shadowLoss);
  cout<<"[both:] Given node not linked with the current node in getLinkInfo()...";exit(0);	
}

double Node::getRxPowerByIndex(int index){
  
  if(linkInfoUpdateStatus==false){ cout<<"LinkInfo not up-to-date for node "<<id<<" while calling getLinkInfo()..."<<endl;abort();}
  if(index>=aLinkInfo.length())
  {cout<<"[both:] Link Index out of range in getLinkInfo()...";exit(0);}
  return(aLinkInfo(index).txPowerInDBm+aLinkInfo(index).txAntennaGain+aLinkInfo(index).rxAntennaGain-aLinkInfo(index).pathLoss-aLinkInfo(index).shadowLoss);
}

bool Node::getLinkInfoUpdateStatus(){
  
  return(linkInfoUpdateStatus);
}

void Node::clearLinkInfo(){
  
  aLinkInfo.set_size(0);
}

void Node::retainLinkInfo(ivec nodeIDs){
  
  if(nodeIDs.length())
  for(int link_cnt=0;link_cnt<aLinkInfo.length();link_cnt++)
  {
	if(find(nodeIDs,aLinkInfo(link_cnt).id)==-1)
	  clearLinkInfoByIndex(link_cnt);
  }
}

void Node::clearLinkInfo(int nodeID){
  
  int linkToRemove=-1;
  for(int link_cnt=0;link_cnt<aLinkInfo.length();link_cnt++)
  {
	if(aLinkInfo(link_cnt).id==nodeID)
	  linkToRemove=link_cnt;
  }
  if(linkToRemove!=-1)
	aLinkInfo=concat(aLinkInfo.left(linkToRemove),aLinkInfo.right(aLinkInfo.length()-linkToRemove-1));
  
  linkedTxNodes.del(linkToRemove);
}

void Node::clearLinkInfoByIndex(int index){
  
  aLinkInfo=concat(aLinkInfo.left(index),aLinkInfo.right(aLinkInfo.length()-index-1));
  linkedTxNodes.del(index);
}

Antenna Node::getTxAntennaInfo(){
  
  return(txAntenna);
}

Antenna Node::getRxAntennaInfo(){
  
  return(rxAntenna);
}

void Node::allocLinkInfo(ivec &txNodeIDs){
  
  
  if(linkedTxNodes.length())
  {
    ivec linkIndices=find(linkedTxNodes,txNodeIDs);
    ivec newLinks=find(linkIndices,-1,true);
    Array<NodeLinkInfo_S> newLinkInfos(newLinks.length());
    
    for(int cnt=0;cnt<newLinkInfos.length();cnt++)
      newLinkInfos(cnt).id=txNodeIDs(newLinks(cnt));
    
    append(aLinkInfo,newLinkInfos);
    append(linkedTxNodes,txNodeIDs(newLinks));
  }
  else
  {
    Array<NodeLinkInfo_S> newLinkInfos(txNodeIDs.length());
    for(int cnt=0;cnt<newLinkInfos.length();cnt++)
      newLinkInfos(cnt).id=txNodeIDs(cnt);
    
    append(aLinkInfo,newLinkInfos);
    append(linkedTxNodes,txNodeIDs);
  }
}

void Node::linkToTXNode(Node &txNode, Terrain &sysTerrain,ShadowType_E shdwType, bool isLinkAltered, bool retainUnAlteredLinks , bool retainOldLOS){
  
  int linkInfoIndx=getLinkInfoIndx(txNode.getID());
  
  if(linkInfoIndx!=-1)
    linkToTXNode(aLinkInfo(linkInfoIndx),txNode,sysTerrain,shdwType,isLinkAltered,retainUnAlteredLinks,retainOldLOS);
  else
  {
    NodeLinkInfo_S templinkInfo;
    linkToTXNode(templinkInfo,txNode,sysTerrain,shdwType,isLinkAltered,retainUnAlteredLinks,retainOldLOS);
    append(aLinkInfo,templinkInfo);
    append(linkedTxNodes,txNode.getID());
  }
}
/*!\fn linkToTXNode()
 * \brief--> It will create a link to the transmitter node specified.
 *\param[in,out] sysTerrain --> it will take the input parameter of terrain class and get modified and return it
 *\param[in,out]  tempLinkInfo--> It is a temporary link in which details of link are filled and returned.
 *\param[in,out] txNodeID --> It gives the transmitter node which is to be linked
 *\param[in] shdwType --> It gives the type of shadowing we are considering.
 *\param[in] isLinkAltered --> It is a flag which tells whether the link is changed or not.
 *\param[in] retainUnAlteredLinks --> it will retain all the links which are unaltered(for all the retainUnAlteredLinks ,isLinkAltered may not be true).
 *\param[in] retainOldLOS --> it is a boolean parameter which retains the OldLOS when function is called again.
 */

bool Node::getSpatiallyConsistentLOS(Area_S coverageArea, SCMPropagationScenario scmScenario, Location_S relativeServiceNodeLoc)
{
    dComplex normLocation;
    normLocation = dComplex(relativeServiceNodeLoc.x+coverageArea.xLen/2,relativeServiceNodeLoc.y+coverageArea.yLen/2); 
    dComplex  n[4] , ni ;
    double dcorr ; vec d(4), r(4) , n_val(4);
    
    int losGridIndx=-1;
    if(scmScenario==_SCM5G_URBAN_MICRO_) losGridIndx=0;
    else if(scmScenario==_SCM5G_URBAN_MACRO_) losGridIndx=3;
    else if(scmScenario==_SCM5G_RURAL_MACRO_) losGridIndx=6;
    else if(scmScenario==_SCM5G_INDOOR_OFFICE_) losGridIndx=9;
    else if(scmScenario==_SCM5G_INDOOR_FACTORY_) losGridIndx=11;
    
    dcorr = dcorrForSPConsistency(1,losGridIndx);
    
    int rowIndx = floor_i(imag(normLocation)/(3*dcorr)); // DS, NLOS
    int colIndx = floor_i(real(normLocation)/(3*dcorr));
    
    ni  = normLocation;
    n[0] = dComplex(colIndx * 3*dcorr,(rowIndx+1)*3*dcorr);
    n[1] = dComplex(colIndx * 3*dcorr,rowIndx*3*dcorr);
    n[2] = dComplex((colIndx+1) * 3*dcorr,rowIndx*3*dcorr);
    n[3] = dComplex((colIndx+1) * 3*dcorr,(rowIndx+1)*3*dcorr);
    
    int gridSubIndx=0;
    if(scmScenario!=_SCM5G_INDOOR_FACTORY_ && scmScenario!=_SCM5G_INDOOR_OFFICE_)    gridSubIndx = losGridIndx%3;
    else    gridSubIndx = 0;
    
    n_val[0] = scmGridsForSpConsist(1)(gridSubIndx)(rowIndx+1,colIndx);
    n_val[1] = scmGridsForSpConsist(1)(gridSubIndx)(rowIndx,colIndx);
    n_val[2] = scmGridsForSpConsist(1)(gridSubIndx)(rowIndx,colIndx+1);
    n_val[3] = scmGridsForSpConsist(1)(gridSubIndx)(rowIndx+1,colIndx+1);
    
    for(int ii = 0;ii<4;ii++)
    {
        d[ii] = abs(ni-n[ii]);
        r[ii] = exp(-d[ii]/dcorr);
    }
    
//     double sum_ri=0.0, ri_ni=0.0;
//     for(int ii=0;ii<4;ii++)
//     {
//         sum_ri+=r[ii]*r[ii];
//         ri_ni+=r[ii]*n_val[ii];
//     }
//     bool isLOS = (ri_ni/sum_ri)>0;

    int nearestVertex = min_index(d);
    bool isLOS = (n_val[nearestVertex])>0;

    
    return isLOS;
    
}

void Node::linkToTXNode(NodeLinkInfo_S &tempLinkInfo,Node &txNode, Terrain &sysTerrain,ShadowType_E shdwType, bool isLinkAltered, bool retainUnAlteredLinks , bool retainOldLOS){
  
  NodeCategory_E txNodeCategory=txNode.getCategory();
  bool validLink=true;
  double linkCarrierFrequency;  
  ChannelModelType channelModelType = sysTerrain.channelModelType;
  // check
  
  
  if(loc.getLocId() != getID())
  {
    cout<<"Loc Id :"<<loc.getLocId()<<"\t Node Id:"<<getID()<<endl;
  }
  if(txNode.loc.getLocId() != txNode.getID())
  {
    cout<<"Loc Id :"<<txNode.loc.getLocId() <<"\t Node Id:"<<txNode.getID()<<endl;
  }
  
  
  if(txNodeCategory!=nodeCategory)
  {
    if(txNodeCategory==_NODECATEGORY_SERVERNODE_)
      linkCarrierFrequency=txNode.getCarrierFreqInHz();
    else
      linkCarrierFrequency=carrierFreq;    
    
    if(linkCarrierFrequency==-1)
    {
      cout<<"[both:]Error. Uninitialized carrier frequency to linkToTXNode() for node : "<<txNode.getID()<<endl;
      abort();
    }
  }
  else //CrossLinks
  {
    if(carrierFreq!=-1 && txNode.getCarrierFreqInHz()!=-1)
    {
      if(carrierFreq!=txNode.getCarrierFreqInHz())
	validLink=false;
      else
	linkCarrierFrequency=carrierFreq;
    }
    else
    {
      validLink=false;
      linkCarrierFrequency=-1;
    }
    
  }

  if(validLink)
  {  
      int linkInfoIndx=getLinkInfoIndx(txNode.getID());
      
      
      int isLOSInit = -1;
      if(sysTerrain.isSpConsistEnable)
      {
          Location_S relativeServiceNodeLoc;
          if(txNodeCategory==_NODECATEGORY_SERVERNODE_)
          {
              relativeServiceNodeLoc = loc-txNode.getWrapLoc();
              isLOSInit = txNode.getSpatiallyConsistentLOS(sysTerrain.scmInfo.coverageArea,sysTerrain.scmScenario,relativeServiceNodeLoc);
          }
          else
          {
              relativeServiceNodeLoc = txNode.getWrapLoc()-loc;
              isLOSInit = getSpatiallyConsistentLOS(sysTerrain.scmInfo.coverageArea,sysTerrain.scmScenario,relativeServiceNodeLoc);
          }
      }
      if(txNode.getID()!=id) //! If there is no crosslinks//
      {
          if(retainUnAlteredLinks) //! checking for the links which are unaltered when we are trying to link again.
          { 
              if(linkInfoIndx!=-1)  //!  If link is generated before , normally it will be an integer because generate link info we are generating links for every node//
              {
                  if(isLinkAltered)   //! check whether link is altered or not//
                  {
                      
                      if(retainOldLOS)   //! If OldLOS is retained then get the linkInfo directly//
                          tempLinkInfo=sysTerrain.getPathLoss(loc,txNode.getWrapLoc(),linkCarrierFrequency,getLinkInfo(txNode.getID()).isLOS,txNode.getType(),type,nodeCategory,txNode.getCategory());
                      else      //! Else generate a newlink//
                          tempLinkInfo=sysTerrain.getPathLoss(loc,txNode.getWrapLoc(),linkCarrierFrequency,isLOSInit,txNode.getType(),type,nodeCategory,txNode.getCategory());
                      
                      // 		  clearLinkInfo(txNode.getID());
                  }
                  else
                      return;
              }
              else
                  tempLinkInfo=sysTerrain.getPathLoss(loc,txNode.getWrapLoc(),linkCarrierFrequency,isLOSInit,txNode.getType(),type,nodeCategory,txNode.getCategory());
          }
          else   //! If there is no  UnAlteredLinks//
          {
              if(linkInfoIndx!=-1)//! If the link is generated before,normally it will be an integer because generate link info we are generating links for every node//
              {
                  if(retainOldLOS) //! If OldLOS is retained then get the linkInfo directly//
                      tempLinkInfo=sysTerrain.getPathLoss(loc,txNode.getWrapLoc(),linkCarrierFrequency,getLinkInfo(txNode.getID()).isLOS,txNode.getType(),type,nodeCategory,txNode.getCategory());
                  else //! Else generate a newlink//
                      tempLinkInfo=sysTerrain.getPathLoss(loc,txNode.getWrapLoc(),linkCarrierFrequency,isLOSInit,txNode.getType(),type,nodeCategory,txNode.getCategory());
                  
                  // 	    clearLinkInfo(txNode.getID());
                  
              }
              else
                  tempLinkInfo=sysTerrain.getPathLoss(loc,txNode.getWrapLoc(),linkCarrierFrequency,isLOSInit,txNode.getType(),type,nodeCategory,txNode.getCategory());
          }
          
          if(((txNode.getWrapLoc()).x-(txNode.getLoc()).x)>1 || ((txNode.getWrapLoc()).y-(txNode.getLoc()).y)>1)
              tempLinkInfo.isWrapped=true;
          else
              tempLinkInfo.isWrapped=false;
          
          tempLinkInfo.txLocation=txNode.getWrapLoc();
          tempLinkInfo.rxLocation=loc;
          
          if(sysTerrain.getTerrain()!="_TERRAIN_NOLOSS_")
              if(loc.isInBuilding || txNode.getLoc().isInBuilding) // Pene
                  tempLinkInfo.pathLoss+=sysTerrain.getBuildingPenetrationLoss(loc,txNode.getWrapLoc(),tempLinkInfo.isWrapped,txNode.getLoc());
              
              tempLinkInfo.id=txNode.getID();
          tempLinkInfo.txPowerInDBm=txNode.getTxPowerInDBm();
          if(tempLinkInfo.distance==-1)
          {
              cout<<"[both:] Error ... Distance value not set in LinkInfo to compute pathDelay..."<<endl;abort();
          }
          else
              tempLinkInfo.pathDelay=tempLinkInfo.distance/lightSpeed;
          
          // Link carrier frequency in Hz
          tempLinkInfo.linkCarrierFrequencyInHz = linkCarrierFrequency;
          if(sysTerrain.isForSCMChannel)
          {
              
              if(sysTerrain.scmInfo.coverageArea.xLen==-1)
              {
                  cout<<"[both:] SCM info need to be initialized in the Terrain for generating SCM Large Scale Parameters in linkToTXNode()..."<<endl;
                  abort();
              }
              
              bool isOutdoorToIndoor;
              if((txNode.getLoc().isIndoor && !loc.isIndoor) || (!txNode.getLoc().isIndoor && loc.isIndoor))
                  isOutdoorToIndoor=true;
              else
                  isOutdoorToIndoor=false;
              
//               if(is5GSCMScenario(sysTerrain.scmScenario))
//               {
//                   tempLinkInfo.txLocation=txNode.getLoc();
//                   tempLinkInfo.rxLocation=loc;  
//                   
//                   Array<Array<Array<mat>>> gridsForSCM;
//                   if(!sysTerrain.scmInfo.useSOSForSpCorr)
//                   {
//                       gridsForSCM = (txNodeCategory==_NODECATEGORY_SERVERNODE_) ? txNode.scmGrids : (txNodeCategory==_NODECATEGORY_SERVICENODE_) ? scmGrids : sysTerrain.commonSCMGrids;
//                   }
//                   int cellID = (txNodeCategory==_NODECATEGORY_SERVERNODE_) ? txNode.getCellID() : getCellID(); //FIXME cellID must be set properly in case of SideLinks. - Goraknath.
//                   generate5GSCMLargeScaleParameters(tempLinkInfo,gridsForSCM,sysTerrain.scmInfo,sysTerrain.scmScenario,sysTerrain.infParams.scenario,channelModelType,isOutdoorToIndoor,(txNodeCategory==_NODECATEGORY_SERVERNODE_),cellID);
//               }
//               else
              
              {
                  if(!isNTNSCMScenario(sysTerrain.scmScenario))
                  {
                    if(scmGrids.size()!=0 && (txNode.isHavingSCMGrid()==false))//Uplink
                      generateSCMLargeScaleParameters(tempLinkInfo,scmGrids,scmPropagationScenario,sysTerrain.infParams.scenario,sysTerrain.scmInfo,txNode.getLoc(), channelModelType,isOutdoorToIndoor,0);
                    else if(scmGrids.size()==0 && (txNode.isHavingSCMGrid()==true))//Downlink
                    {
                        generateSCMLargeScaleParameters(tempLinkInfo,txNode.scmGrids,txNode.scmPropagationScenario,sysTerrain.infParams.scenario,sysTerrain.scmInfo,loc,channelModelType,isOutdoorToIndoor,1);
                    }
                    else
                    {
                        /// CrossLinks (SideLink)
                        if(scmGrids.size()!=0 && (txNode.isHavingSCMGrid()==true)) //Server-Server
                            generateSCMLargeScaleParameters(tempLinkInfo,txNode.scmGrids,txNode.scmPropagationScenario,sysTerrain.infParams.scenario,sysTerrain.scmInfo,loc,channelModelType,isOutdoorToIndoor);
                        else	//Service-Service
                            generateSCMLargeScaleParameters(tempLinkInfo,sysTerrain.commonSCMGrids,txNode.scmPropagationScenario,sysTerrain.infParams.scenario,sysTerrain.scmInfo,loc,channelModelType,isOutdoorToIndoor);
                    }

                  }
                  else
                  {
                    {
                        generateSCMLargeScaleParameters(tempLinkInfo,txNode.scmGrids,sysTerrain.scmScenario,sysTerrain.infParams.scenario,sysTerrain.scmInfo,loc,channelModelType,isOutdoorToIndoor,1);
                        //Santhosh
                    }

                  }
              }
          }
          else
          {   
              if(shdwType==_SHADOWTYPE_CORRELATED_)
              {
                  if(shadowGrid.size()!=0 && (txNode.getShadowGrid()).size()==0) 
                      tempLinkInfo.shadowLoss=getShadowLoss(txNode.getLoc(),shadowGrid,tempLinkInfo.shadowStdDev,corrDist);
                  else if(txNode.getShadowGrid().size()!=0) 
                      tempLinkInfo.shadowLoss=getShadowLoss(loc,txNode.getShadowGrid(),tempLinkInfo.shadowStdDev,txNode.getCorrelatedDistance());	
                  else // CrossLinks - 10010
                      tempLinkInfo.shadowLoss=tempLinkInfo.shadowStdDev*randn();
                  if(abs(tempLinkInfo.shadowLoss)>100){ cout <<tempLinkInfo.shadowLoss<<" SL fails ..."<<tempLinkInfo.shadowStdDev<<","<<txNode.getCorrelatedDistance()<<"\n"<<txNode.getShadowGrid();exit(0); }
                  
              }
              else
                  tempLinkInfo.shadowLoss=tempLinkInfo.shadowStdDev*randn();
//               cout << "[ShadowLoss Debug] Node " << id << " -> TxNode " << txNode.getID() << " ShadowLoss: " << tempLinkInfo.shadowLoss << " dB (StdDev: " << tempLinkInfo.shadowStdDev << ")" << endl;
          }
          
          
//           cout << "[ShadowLoss Debug] Node " << id << " -> TxNode " << txNode.getID() << " ShadowLoss: " << tempLinkInfo.shadowLoss << " dB (StdDev: " << tempLinkInfo.shadowStdDev << ")" << endl;

          txAntenna.setLoc(loc,true);	
          rxAntenna.setLoc(loc,true);
          
          computeAntennaGains(txNode,tempLinkInfo);    
    //abort();
      //cout << "computeAntennaGains: " << computeAntennaGains << "AOD :" << AOD << "ZOD : " << ZOD <<endl; 
          
      }
      else // Trying to link same node. Used only in crossLinks to avoid linkTable confusions - 10010
      {
          tempLinkInfo=getNoLossPathLoss(loc,txNode.getWrapLoc(),carrierFreq,-1,txNode.getType(),type);
          tempLinkInfo.isWrapped=false;
          tempLinkInfo.id=txNode.getID();;
          tempLinkInfo.txPowerInDBm=0;
          tempLinkInfo.pathDelay=0;
          tempLinkInfo.shadowLoss=0;
          tempLinkInfo.txAntennaGain=0;
          tempLinkInfo.rxAntennaGain=0;
          tempLinkInfo.txType=type;
          tempLinkInfo.rxType=type;
          tempLinkInfo.txLocation=txNode.getWrapLoc();
          tempLinkInfo.rxLocation=loc;
          
          
      }
  }
  else
  {
    //Equivalent of no link between these nodes
    tempLinkInfo.pathLoss=500;
    tempLinkInfo.shadowStdDev=0;
    tempLinkInfo.isLOS=0;
    tempLinkInfo.isWrapped=false;
    tempLinkInfo.id=txNode.getID();;
    tempLinkInfo.txPowerInDBm=0;
    tempLinkInfo.pathDelay=500;
    tempLinkInfo.shadowLoss=0;
    tempLinkInfo.txAntennaGain=0;
    tempLinkInfo.rxAntennaGain=0;
    tempLinkInfo.txType=type;
    tempLinkInfo.rxType=type;
    tempLinkInfo.txLocation=txNode.getWrapLoc();
    tempLinkInfo.rxLocation=loc;
    
    
  }

  
  
  
  
}
void Node::computeAntennaGains(Node& txNode,NodeLinkInfo_S& tempLinkInfo)
{
//     double linkCarrierFrequency = tempLinkInfo.linkCarrierFrequencyInHz;
//     if(txNode.txAntenna.enableAAS)
//     {
//         tempLinkInfo.txAntennaGain=txNode.get3DTxAntennaGain(type,loc,linkCarrierFrequency); 
//     }
//     else
        
    tempLinkInfo.txAntennaGain=txNode.getTxAntennaGain(loc); 
    
    #ifdef _PLOT_ANTENNA_PATTERN_
    // Antenna Pattern test TX
    ivec AOD = getIntegers(-180,179,1);
    ivec ZOD = getIntegers(0,179,1);
    mat TxGains = zeros(AOD.length(),ZOD.length());
    //abort();
    //cout<< "AOD.length(),ZOD.length()" << AOD.length() << ","<< ZOD.length()<<endl;
    for(int i =0; i<AOD.length();i++)
        for(int j =0;j<ZOD.length();j++)
        {
            TxGains(i,j) = txNode.txAntenna.get3DAntArrayGainForPort0(double(AOD(i)),double(ZOD(j)));
            //abort();
            // 	 TxGains(i,j) = txNode.txAntenna.get3DAntArrayGainForPort0(loc,txNode.carrierFreq);
        }
        std:ofstream fout;
    fout.open("./Results/antennaGain2.txt",std::ios::app);
    fout<<TxGains<<endl;
    fout.close();
    cout<<"Code is performing Antenna pattern testing. Disable the  macro the _PLOT_ANTENNA_PATTERN_ to go out of the test case... "<<endl;
    abort();
    #endif
    
    tempLinkInfo.txType=txNode.getType();
    tempLinkInfo.rxType=type; 
//     if(rxAntenna.enableAAS) 
//     {     
//         tempLinkInfo.rxAntennaGain=get3DRxAntennaGain(txNode.getType(),txNode.getWrapLoc(),linkCarrierFrequency); 
//     }
//     else
    
    tempLinkInfo.rxAntennaGain=getRxAntennaGain(txNode.getWrapLoc());
//     cout << "[AntennaGain Debug] RxNode " << getID() << " <- TxNode " << txNode.getID() << " TxGain: " << tempLinkInfo.txAntennaGain << " dBi, RxGain: " << tempLinkInfo.rxAntennaGain << " dBi" << endl;
    
    #ifdef _CHANN_CALIB_
    pairIxI P(getID(),txNode.getID());
    NodeLinkInfo_S calibLinkInfo;
    
    calibLinkInfo = getLinfoMap(P);
    calibLinkInfo.shadowLoss = (calibLinkInfo.shadowStdDev/tempLinkInfo.shadowStdDev)*tempLinkInfo.shadowLoss;
    calibLinkInfo.rxAntennaGain = tempLinkInfo.rxAntennaGain;
    calibLinkInfo.txAntennaGain = tempLinkInfo.txAntennaGain;
    setLinkInfoMap(P,calibLinkInfo);
    
    
    cout<<"RxNode.ID():"<<getID()<<"\tTXNode.getLocId():"<<txNode.getID()<<"calibLinkInfo.shadowStdDev:"<<calibLinkInfo.shadowStdDev;
    cout<<"\ttempLinkInfo.shadowStdDev:"<<tempLinkInfo.shadowStdDev<<endl;
    cout<<"calibLinkInfo.pathLoss:"<<calibLinkInfo.pathLoss<<endl;
    cout<<"tempLinkInfo.pathLoss:"<<tempLinkInfo.pathLoss<<endl;
    
    #endif
}
/*void Node::linkToTXNode(NodeLinkInfo_S &tempLinkInfo,Node &txNode, Terrain &sysTerrain,ShadowType_E shdwType, bool isLinkAltered, bool retainUnAlteredLinks , bool retainOldLOS){
  
  bool overwrite=false;
  int linkInfoIndx=getLinkInfoIndx(txNode.getID());
  if(txNode.getID()!=id) 
  {
    if(retainUnAlteredLinks)
 
    { 
      if(linkInfoIndx!=-1)  
      {
	if(isLinkAltered)   
	{
	  
	  if(retainOldLOS) 
	    tempLinkInfo=sysTerrain.getPathLoss(loc,txNode.getWrapLoc(),carrierFreq,getLinkInfo(txNode.getID()).isLOS,txNode.getType(),type,nodeCategory,txNode.getCategory());
	  else
	    tempLinkInfo=sysTerrain.getPathLoss(loc,txNode.getWrapLoc(),carrierFreq,-1,txNode.getType(),type,nodeCategory,txNode.getCategory());

	}
	else
	  return;
      }
      else
	tempLinkInfo=sysTerrain.getPathLoss(loc,txNode.getWrapLoc(),carrierFreq,-1,txNode.getType(),type,nodeCategory,txNode.getCategory());
    }
    else   
    {
      if(linkInfoIndx!=-1)
      {
	if(retainOldLOS) 
	  tempLinkInfo=sysTerrain.getPathLoss(loc,txNode.getWrapLoc(),carrierFreq,getLinkInfo(txNode.getID()).isLOS,txNode.getType(),type,nodeCategory,txNode.getCategory());
	else 
	  tempLinkInfo=sysTerrain.getPathLoss(loc,txNode.getWrapLoc(),carrierFreq,-1,txNode.getType(),type,nodeCategory,txNode.getCategory());
      }
      else
	tempLinkInfo=sysTerrain.getPathLoss(loc,txNode.getWrapLoc(),carrierFreq,-1,txNode.getType(),type,nodeCategory,txNode.getCategory());
    }
    
    if(((txNode.getWrapLoc()).x-(txNode.getLoc()).x)>1 || ((txNode.getWrapLoc()).y-(txNode.getLoc()).y)>1)
      tempLinkInfo.isWrapped=true;
    else
      tempLinkInfo.isWrapped=false;
   
    
    if(sysTerrain.getTerrain()!="_TERRAIN_NOLOSS_")
      if(loc.isInBuilding || txNode.getLoc().isInBuilding) 
	tempLinkInfo.pathLoss+=sysTerrain.getBuildingPenetrationLoss(loc,txNode.getWrapLoc(),tempLinkInfo.isWrapped,txNode.getLoc());
        
    tempLinkInfo.id=txNode.getID();
    tempLinkInfo.txPowerInDBm=txNode.getTxPowerInDBm();
    if(tempLinkInfo.distance==-1)
    {
      cout<<"[both:] Error ... Distance value not set in LinkInfo to compute pathDelay..."<<endl;abort();
    }
    else
      tempLinkInfo.pathDelay=tempLinkInfo.distance/lightSpeed;
    
    if(sysTerrain.isForSCMChannel)
    {
      if(sysTerrain.scmInfo.coverageArea.xLen==-1)
      {
	cout<<"[both:] SCM info need to be initialized in the Terrain for generating SCM Large Scale Parameters in linkToTXNode()..."<<endl;
	exit(0);
      }
      bool isOutdoorToIndoor;
      if((txNode.getLoc().isIndoor && !loc.isIndoor) || (!txNode.getLoc().isIndoor && loc.isIndoor))
	isOutdoorToIndoor=true;
      else
	isOutdoorToIndoor=false;
      if(scmGrids.size()!=0 && (txNode.isHavingSCMGrid()==false)) 
	generateSCMLargeScaleParameters(tempLinkInfo,scmGrids,scmPropagationScenario,sysTerrain.scmInfo,txNode.getLoc(),isOutdoorToIndoor);
      else if(scmGrids.size()==0 && (txNode.isHavingSCMGrid()==true)) 
	generateSCMLargeScaleParameters(tempLinkInfo,txNode.scmGrids,txNode.scmPropagationScenario,sysTerrain.scmInfo,loc,isOutdoorToIndoor);
      /// CrossLinks
	else if(scmGrids.size()!=0 && (txNode.isHavingSCMGrid()==true)) //Server-Server
	generateSCMLargeScaleParameters(tempLinkInfo,txNode.scmGrids,txNode.scmPropagationScenario,sysTerrain.scmInfo,loc,isOutdoorToIndoor);
	else	//Service-Service
	generateSCMLargeScaleParameters(tempLinkInfo,sysTerrain.commonSCMGrids,txNode.scmPropagationScenario,sysTerrain.scmInfo,loc,isOutdoorToIndoor);
	
    }
    else
    {   
      if(shdwType==_SHADOWTYPE_CORRELATED_)
      {
	if(shadowGrid.size()!=0 && (txNode.getShadowGrid()).size()==0) 
	  tempLinkInfo.shadowLoss=getShadowLoss(txNode.getLoc(),shadowGrid,tempLinkInfo.shadowStdDev,corrDist);
	else if(txNode.getShadowGrid().size()!=0) 
	  tempLinkInfo.shadowLoss=getShadowLoss(loc,txNode.getShadowGrid(),tempLinkInfo.shadowStdDev,txNode.getCorrelatedDistance());	
	else // CrossLinks - 10010
	      tempLinkInfo.shadowLoss=tempLinkInfo.shadowStdDev*randn();
	if(abs(tempLinkInfo.shadowLoss)>100){ cout <<tempLinkInfo.shadowLoss<<" SL fails ..."<<tempLinkInfo.shadowStdDev<<","<<txNode.getCorrelatedDistance()<<"\n"<<txNode.getShadowGrid();exit(0); }
	
      }
      else
	tempLinkInfo.shadowLoss=tempLinkInfo.shadowStdDev*randn();
    }
    tempLinkInfo.txAntennaGain=txNode.getTxAntennaGain(loc);
    txAntenna.setLoc(loc,true);	
    rxAntenna.setLoc(loc,true);	
    tempLinkInfo.txType=txNode.getType();
    tempLinkInfo.rxType=type;
    tempLinkInfo.rxAntennaGain=getRxAntennaGain(txNode.getWrapLoc());
    
  }
  else 
  {
      tempLinkInfo=getNoLossPathLoss();
      tempLinkInfo.isWrapped=false;
      tempLinkInfo.id=id;
      tempLinkInfo.txPowerInDBm=0;
      tempLinkInfo.pathDelay=0;
      tempLinkInfo.shadowLoss=0;
      tempLinkInfo.txAntennaGain=0;
      tempLinkInfo.rxAntennaGain=0;
      tempLinkInfo.txType=type;
      tempLinkInfo.rxType=type;
  }
  tempLinkInfo.txLocation=txNode.getWrapLoc();
  tempLinkInfo.rxLocation=loc;
  
 
}

*/
