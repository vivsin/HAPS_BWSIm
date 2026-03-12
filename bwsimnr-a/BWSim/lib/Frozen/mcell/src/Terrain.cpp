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

#include "../include/Terrain.h"


Terrain::Terrain(){

    cellISD=-1;
    isForSCMChannel=false;
    maximumFloors=-1;
    wallSpacing=-1;
    terrainTypes.set_length(0);
    terrainType="_TERRAIN_UNINIT_";
    thickWallLossInDB=12;
    thinWallLossInDB=5;
    is3Ddistance=false;
    indoorOfficeScenario = _INDOOR_UNDEFINED_;
    
    isSpConsistEnable=false;
    

}

void Terrain::addTerrain(string terrainName, pathLossFunction terrainFunction, bool overwrite)
{
  if(findInStringArray(terrainTypes,terrainName)==-1)
  {
    terrainMap[terrainName]=terrainFunction;
  }
  else
	if(overwrite)
	  terrainMap[terrainName]=terrainFunction;
	else
	  cout<<"[both:] Warning.. Trying to overwrite pathLossFunction for terrain : "<<terrainName<<endl;
}

void Terrain::addTerrain(Array<string> terrainNames, pathLossFunction terrainFunction, bool overwrite)
{
  for(int terr_cnt=0;terr_cnt<terrainNames.length();terr_cnt++)
	addTerrain(terrainNames(terr_cnt),terrainFunction,overwrite);
}


bool Terrain::isReadyForIndoor(){

    bool ready=true;
    if(maximumFloors==-1)
    {
	cout<<"maximumFloors in Terrain is not set...."<<endl;
	ready=false;
    }
    if(wallSpacing==-1)
    {
	cout<<"wallSpacing in Terrain is not set...."<<endl;
	ready=false;
    }
    if(roomSize.length==-1)
    {
	cout<<"roomSize in Terrain is not set...."<<endl;
	ready=false;
    }
    return(ready);
}

void Terrain::setRoomSize(double length, double height){

    if(wallSpacing!=-1 && length<wallSpacing)
    {
	cout<<"[both:] Please specify room length greater than wall spacing..."<<endl;
	exit(0);
    }
    roomSize.length=length;
    roomSize.width=length;
    roomSize.height=height;
}

void Terrain::setRoomSize(double length,double width, double height){
  
  if(wallSpacing!=-1 && length<wallSpacing)
  {
    cout<<"[both:] Please specify room length greater than wall spacing..."<<endl;
    exit(0);
  }
  roomSize.length=length;
  roomSize.width=width;
  roomSize.height=height;
}
void Terrain::setBuildingSideLength(double buildingLength){
  
  buildingSideLength=buildingLength;
}

void Terrain::setWallPenetrationLoss(double thickWallPenetrationLossInDB, double thinWallPenetrationLossInDB){
  
  thickWallLossInDB=thickWallPenetrationLossInDB;
  thinWallLossInDB=thinWallPenetrationLossInDB;
}

void Terrain::setCellISD(double isd){

    cellISD=isd;
}

void Terrain::setTerrain(string terrainName){

    terrainType=terrainName;
    if(terrainName != "_TERRAIN_NOLOSS_")
    scmScenario = loadSCMScenario(terrainName,0);
}

void Terrain::setMaximumFloors(int maxFloors){

    maximumFloors=maxFloors;
}

void Terrain::setWallSpacing(double distanceInMeters){

    if(roomSize.length!=-1 && roomSize.length<distanceInMeters)
    {
	cout<<"[both:] Please specify wall spacing lesser than room length..."<<endl;
	exit(0);
    }
    wallSpacing=distanceInMeters;
}

double Terrain::getCellISD() const{

    return(cellISD);
}

string Terrain::getTerrain() const{

    return(terrainType);
}

int Terrain::getMaximumFloors() const{

    return(maximumFloors);
}

double Terrain::getWallSpacing() const{

    return(wallSpacing);
}

double Terrain::getWallPenetrationLoss(bool isThickWall) const{
  
  if(isThickWall)
    return(thickWallLossInDB);
  else
    return(thinWallLossInDB);
}

RoomSize_S Terrain::getRoomSize() const{

    return(roomSize);
}

double Terrain::getBuildingSideLength() const{
  
  return(buildingSideLength);
}

void Terrain::enable3DDistance()
{
  is3Ddistance=true;
}

void Terrain::disable3DDistance()
{
  is3Ddistance=false;
}

NodeLinkInfo_S Terrain::getPathLoss(Location_S nodeLoc1,Location_S nodeLoc2, double carrierFreqInHz, int isLOS, int txNodeType, int rxNodeType,NodeCategory_E nodeCategory1,NodeCategory_E nodeCategory2)
{
    if (terrainType=="_TERRAIN_UNINIT_")
    {
        cout<<" [both:] Pathloss model  is not set inside the Terrain object, Please set it, Now it is the default  "<<terrainType<<endl;
        exit(1);
    }
    if(nodeCategory1!=nodeCategory2 || terrainType=="_TERRAIN_NOLOSS_")
    {
      std::map<string,pathLossFunction>::iterator it=terrainMap.find(terrainType);
      if(it != terrainMap.end ())
      {
	  return (*terrainMap[terrainType])   (nodeLoc1,nodeLoc2,carrierFreqInHz,isLOS,txNodeType,rxNodeType,this,nodeCategory1,nodeCategory2);
      }
      else
      {
	  cout<<"[both:] \""<<terrainType<<"\" Pathloss model not implemented...."<<endl;
	  exit(1);
      }
    }
    else
    {
      return(getCrossLinkPathloss(nodeLoc1,nodeLoc2, carrierFreqInHz, isLOS, txNodeType, rxNodeType,this,nodeCategory1,nodeCategory2));
    }  
    
}

NodeLinkInfo_S getNoLossPathLoss(Location_S /*nodeLoc1*/,Location_S /*nodeLoc2*/, double /*carrierFreqInHz*/, int /*isLOS*/, int /*txNodeType*/, int /*rxNodeType*/, const Terrain * /*terr*/,NodeCategory_E /*nodeCategory1*/,NodeCategory_E /*nodeCategory2*/){
  
  NodeLinkInfo_S tempLinkInfo;
  tempLinkInfo.isLOS=0;
  tempLinkInfo.distance=0;
  tempLinkInfo.shadowStdDev=0;
  tempLinkInfo.pathLoss=0;
  return  (tempLinkInfo);
}
  
void loadNoLossTerrain(Terrain &terrain){
  
  Array<string> terrainNames="{ _TERRAIN_NOLOSS_ }";
  terrain.addTerrain(terrainNames,&getNoLossPathLoss);
}

void Terrain::setCoveragePlan(Array<mat> covrgPlan){
  
  coveragePlan = covrgPlan;
}

Array<mat> Terrain::getCoveragePlan() const{
  
  return(coveragePlan);
}

void Terrain :: setCoverageFootprint(imat &footPrint){
  
  coverageFootprint=footPrint;
}

imat Terrain :: getCoverageFootprint() const{
  
  return(coverageFootprint);
}

double Terrain :: getBuildingPenetrationLoss(Location_S rxNodeLoc, Location_S txNodeLoc, bool invalidErrorBreaker) const{
 
  double x1,y1,z1,x2,y2,z2;
  x1=rxNodeLoc.x;y1=rxNodeLoc.y;z1=rxNodeLoc.z;
  x2=txNodeLoc.x;y2=txNodeLoc.y;z2=txNodeLoc.z; 
  
  int xIntersect,yIntersect;
  double penetrationLoss=0;
  double zMin,zMax;
  if(z1>z2)
  {
    zMax=z1;
    zMin=z2;
  }
  else
  {
    zMax=z2;
    zMin=z1;
  }
  for(double z_cnt=zMin;z_cnt<=zMax;z_cnt++)
  {
    xIntersect = round_i((double)coveragePlan(round_i(z_cnt)).cols()/2.0+(((double)z_cnt-z1)/(z2-z1)*(x2-x1)+x1));
    yIntersect = round_i((double)coveragePlan(round_i(z_cnt)).rows()/2.0-(((double)z_cnt-z1)/(z2-z1)*(y2-y1)+y1));
    if((yIntersect>=coveragePlan(round_i(z_cnt)).rows() || xIntersect>=coveragePlan(round_i(z_cnt)).cols()) && invalidErrorBreaker==true)
    {
      continue;
    }
    else if((yIntersect>=coveragePlan(round_i(z_cnt)).rows() || xIntersect>=coveragePlan(round_i(z_cnt)).cols()))
    {
      cout<<"[both:]Error. Invalid intersects in getBuildingPenetrationLoss()."<<endl;
      abort();
    }
    penetrationLoss+=coveragePlan(round_i(z_cnt))(yIntersect,xIntersect);
  }
  return penetrationLoss;
}

double Terrain :: getBuildingPenetrationLoss(Location_S rxNodeLoc, Location_S txNodeLoc, bool isWrapped, Location_S txNodePhyLoc) const{
 
  
 if(isWrapped==false)
  return(getBuildingPenetrationLoss(rxNodeLoc,txNodeLoc));
 else
 {
   double penetrationLoss=0;
   penetrationLoss+=getBuildingPenetrationLoss(rxNodeLoc,txNodeLoc,true);// Finding penetrationLoss between rxNode and wrapped txNode upto boundary
   Location_S rxNodeWraploc=rxNodeLoc-(txNodeLoc-txNodePhyLoc);
   penetrationLoss+=getBuildingPenetrationLoss(rxNodeWraploc,txNodePhyLoc,true);// Finding penetrationLoss between txNode and wrapped rxNode from boundary
   return penetrationLoss;
 }
}

