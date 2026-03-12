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

#include "../include/FrameHandler.h"

Framehandler::Framehandler(){
  
//   dataBlkInfo.set_size(0);
  frameStructure=NULL;
  isFrameStructPointed=false;
  frame.set_size(0); 
  antennaCount=-1;
}

bool Framehandler::isReady(){
  
  if(isFrameStructPointed==false)
  {
    cout<<"Please point a frameStructure in the Frame Handler..."<<endl;
    return(false);
  }
  if(frameStructure->isReady()==false || frameStructure->isFrameStructureParsed()==false){
    
   cout<<"Loaded framestructure either not fully configured or not parsed in frameHandler..."<<endl;
   return(false);
  }
  return(true);
}

bool Framehandler::isFrameStructurePointed(){
  
  return(isFrameStructPointed);
}

bool Framehandler::pointFrameStructure(FrameStructure *frameStructPointer){
  
  if(frameStructPointer->isFrameStructureParsed()==false){
    
    cout<<"Please initialize the frameStructure fully before loading it in frameHandler..."<<endl;
    return(false);
  }
  frameStructure=frameStructPointer;
  isFrameStructPointed=true;
  return true;
}

FrameStructure* Framehandler::getFrameStructurePointer(){
  
  return(frameStructure);
}
void Framehandler::initCurrentFrame(){
  
  if(antennaCount==-1)
  { cout<<"[both:] Antenna Count not set in FrameHandler for initCurrentFrame() ..."; abort();}
  
  if(frame.length()!=antennaCount)
    frame.set_size(antennaCount);
  for(int ant_cnt=0;ant_cnt<antennaCount;ant_cnt++)
  {
    if(frame(ant_cnt).rows()!=frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSubcarriers || frame(ant_cnt).cols()!=frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSymbols)
      frame(ant_cnt).set_size(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSubcarriers,frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSymbols);
    frame(ant_cnt).clear();
  }
}

void Framehandler::freeFrameMemory(){
  
  frame.set_size(0);
}


bool Framehandler::isResourceUnitAvailable(string ruType){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling isResourceUnitAvailable()..."<<endl;
    abort();
  }
  return(frameStructure->isResourceUnitAvailable(ruType));
  
}

int Framehandler::getAntennaCount(){
  
  return(antennaCount);
}

// int Framehandler::getBlkCountInCurrentFrame(){
//   
//   return(dataBlkInfo.length());
// }

Size_S Framehandler::getResourceUnitSize(string ruType){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getResourceUnitSize()..."<<endl;
    abort();
  }
  return(frameStructure->getResourceUnitSize(ruType));
}

ResourceUnitInfo_S Framehandler::getResourceUnitInfo(string ruType){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getResourceUnitInfo()..."<<endl;
    abort();
  }
  return(frameStructure->getResourceUnitInfo(ruType));
}

ResourceUnitInfo_S Framehandler::getResourceUnitInfo(int ruIndex){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getResourceUnitInfo()..."<<endl;
    abort();
  }
  return(frameStructure->getResourceUnitInfo(ruIndex));
}

Array<ivec> Framehandler::getResourceUnit(string ruType){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getResourceUnit()..."<<endl;
    abort();
  }
  int ruIndx=findInStringArray(frameStructure->resourceUnitType,ruType);
  if(ruIndx==-1)
  {	
    cout<<"[both:] Error ... Given ruType "<<ruType<<" not available in the resourceUnitType list..."<<endl;
    abort();
  }
  
  return(frameStructure->getResourceUnit(ruType));
}

imat Framehandler::getResourceUnitMatrix(string ruType){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getResourceUnitMatrix()..."<<endl;
    abort();
  }
  return(frameStructure->getResourceUnitMatrix(ruType));
}

imat Framehandler::getResourceUnitMatrix(int ruIndex){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getResourceUnitMatrix()..."<<endl;
    abort();
  }
  return(frameStructure->getResourceUnitMatrix(ruIndex));
}

imat Framehandler::getResourceUnitMatrix(string ruType,string toneType){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getResourceUnitMatrix()..."<<endl;
    abort();
  }
  int ruIndx=findInStringArray(frameStructure->resourceUnitType,ruType);
  if(ruIndx==-1)
  {	
    cout<<"[both:] Error ... Given resource Unit Type not available in the list..."<<endl;
    abort();
  }
  imat tempRU;
  tempRU.set_size(frameStructure->resourceUnitInfo(ruIndx).resourceUnitSize.numberOfSubcarriers,frameStructure->resourceUnitInfo(ruIndx).resourceUnitSize.numberOfSymbols);
  tempRU.zeros();  
  ivec toneIndices=frameStructure->resourceUnitInfo(ruIndx).getRow1stToneIndices(toneType);
  setInMatrix(tempRU,toneIndices,1);
  return(tempRU);
}

int Framehandler::getAllRuCount(string frameTypeName){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getAllRuCount()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getAllRuCount()..."<<endl;
    abort();
  }
  if(frameTypeName=="")
  {
    return(sum(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruCountPerLevel));
  }
  else
  {
    int frmIndx=findInStringArray(frameStructure->frameType,frameTypeName);
    if(frmIndx==-1)
    {
      cout<<"[both:] Error .. Undefined frameType to getAllRuCount()..."<<endl;
      abort();
    }
    return(sum(frameStructure->frameInfo(frmIndx).ruCountPerLevel));
  }
}

int Framehandler::getRuCountPerLevel(string frameTypeName){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getAllRuCount()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getAllRuCount()..."<<endl;
    abort();
  }
  if(frameTypeName=="")
  {
    return(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruCountPerLevel(0));
  }
  else
  {
    int frmIndx=findInStringArray(frameStructure->frameType,frameTypeName);
    if(frmIndx==-1)
    {
      cout<<"[both:] Error .. Undefined frameType to getAllRuCount()..."<<endl;
      abort();
    }
    return(frameStructure->frameInfo(frmIndx).ruCountPerLevel(0));
  }
}


Size_S Framehandler::getFrameSize(string frameTypeName){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getFrameSize()..."<<endl;
    abort();
  }
  return(frameStructure->getFrameSize(frameTypeName));
}

string Framehandler::getCurrentFrameType(){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getCurrentFrameType()..."<<endl;
    abort();
  }
  return(frameStructure->frameType(frameStructure->currentFrameTypeIndx));
}
/*
Array<DataBlkInfo_S> Framehandler::getDataBlkInfo(){
  
  return(dataBlkInfo);
}*/
/*
DataBlkInfo_S Framehandler::getDataBlkInfo(int dataBlkNumber){
  
  return(dataBlkInfo(dataBlkNumber));
}

DataGenInfo_S Framehandler::getDataGenInfo(int dataBlkNumber){
  
  return(dataBlkInfo(dataBlkNumber).dataGenInfo);
}*/

ivec Framehandler::getOccupiedSymbols(ivec resourceUnitsIndices){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getOccupiedSymbols()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getOccupiedSymbols()..."<<endl;
    abort();
  }
  ivec occupiedSymbols=getIntegers(0,frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSymbols-1);
  bvec isOccupied(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSymbols);
  isOccupied.clear();
  Offset_S ruOffset;
  Size_S ruSize;
  for(int ru_cnt=0;ru_cnt<resourceUnitsIndices.length();ru_cnt++)
  {
    ruOffset=frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruOffset(resourceUnitsIndices(ru_cnt));
    ruSize=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitsIndices(ru_cnt))).resourceUnitSize;
    for(int sym_cnt=0;sym_cnt<ruSize.numberOfSymbols;sym_cnt++)
      isOccupied(ruOffset.symbolOffset+sym_cnt)=1;
    
  }
  return(occupiedSymbols(find(isOccupied)));
}
/*
ivec Framehandler::getOccupiedSymbols(int blkCount){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getOccupiedSymbols()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getOccupiedSymbols()..."<<endl;
    abort();
  }
  if(blkCount>=dataBlkInfo.length())
  {
    cout<<"[both:] Error... Invalid blkCount to getOccupiedSymbols()..."<<endl;
    abort();
  }
  ivec resourceUnitsIndices=dataBlkInfo(blkCount).resourceBlocks;
  ivec occupiedSymbols=getIntegers(0,frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSymbols-1);
  bvec isOccupied(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSymbols);
  isOccupied.clear();
  Offset_S ruOffset;
  Size_S ruSize;
  for(int ru_cnt=0;ru_cnt<resourceUnitsIndices.length();ru_cnt++)
  {
    ruOffset=frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruOffset(resourceUnitsIndices(ru_cnt));
    ruSize=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitsIndices(ru_cnt))).resourceUnitSize;
    for(int sym_cnt=0;sym_cnt<ruSize.numberOfSymbols;sym_cnt++)
      isOccupied(ruOffset.symbolOffset+sym_cnt)=1;
    
  }
  return(occupiedSymbols(find(isOccupied)));
}*/

Array<cmat> Framehandler::getTonesInRU(int resourceUnitIndx){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getTonesInRU()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getTonesInRU()..."<<endl;
    abort();
  }
  Offset_S ruOffset=frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruOffset(resourceUnitIndx);
  Size_S ruSize=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).resourceUnitSize;
  return(getFrame(0,antennaCount-1,ruOffset.symbolOffset,ruOffset.symbolOffset+ruSize.numberOfSymbols-1,
		  ruOffset.subcarrierOffset,ruOffset.subcarrierOffset+ruSize.numberOfSubcarriers-1));
}

cmat Framehandler::getTonesInRU(int resourceUnitIndx, int antenna){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getTonesInRU()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getTonesInRU()..."<<endl;
    abort();
  }
  Offset_S ruOffset=frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruOffset(resourceUnitIndx);
  Size_S ruSize=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).resourceUnitSize;
  return(getFrameFromSingleAntenna(antenna,ruOffset.symbolOffset,ruOffset.symbolOffset+ruSize.numberOfSymbols-1,
				   ruOffset.subcarrierOffset,ruOffset.subcarrierOffset+ruSize.numberOfSubcarriers-1));
}

cvec Framehandler::getTonesInRU(int resourceUnitIndx, string toneType, int antenna){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getTonesInRU()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getTonesInRU()..."<<endl;
    abort();
  }
  Array<cmat> tones=getTonesInRU(resourceUnitIndx);
  ivec myTonesIndx=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).getRow1stToneIndices(toneType);
  if(myTonesIndx.length()!=0)
  {
    Size_S ruSize=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).resourceUnitSize;
    cvec myTones(tones.length());
    myTones=zeros_c(myTonesIndx.length());
    for(int tone_cnt=0;tone_cnt<myTonesIndx.length();tone_cnt++)
      (myTones)(tone_cnt)=(tones(antenna))(myTonesIndx(tone_cnt)%ruSize.numberOfSubcarriers,floor_i(myTonesIndx(tone_cnt)/ruSize.numberOfSubcarriers));
    return(myTones);  
  }
  else
  {
    cout<<"[both:] Error... undefined Tone Type to getTones()..."<<endl;
    abort();
  }
}

Array<cvec> Framehandler::getTonesInRU(int resourceUnitIndx, string toneType){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getTonesInRU()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getTonesInRU()..."<<endl;
    abort();
  }
  Array<cmat> tones=getTonesInRU(resourceUnitIndx);
  ivec myTonesIndx=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).getRow1stToneIndices(toneType);
 
  if(myTonesIndx.length()!=0)
  {
    Size_S ruSize=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).resourceUnitSize;
    Array<cvec> myTones(tones.length());
    for(int ant_cnt=0;ant_cnt<tones.length();ant_cnt++)
    {  
      myTones(ant_cnt)=zeros_c(myTonesIndx.length());
      for(int tone_cnt=0;tone_cnt<myTonesIndx.length();tone_cnt++)
	(myTones(ant_cnt))(tone_cnt)=(tones(ant_cnt))(myTonesIndx(tone_cnt)%ruSize.numberOfSubcarriers,floor_i(myTonesIndx(tone_cnt)/ruSize.numberOfSubcarriers));
    }
    return(myTones);  
  }
  else
  {
    cout<<"[both:] Error... undefined Tone Type to getTones()..."<<endl;
    abort();
  }
  
}

ivec Framehandler::getRow1stToneIndicesInRU(int resourceUnitIndx, string toneType){

  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getRow1stToneIndicesInRU()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getRow1stToneIndicesInRU()..."<<endl;
    abort();
  }
  //cout << ".............." << toneType << endl;
  return(frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).getRow1stToneIndices(toneType));
}

ivec Framehandler::getRow1stToneIndicesInRUovereSymbols(int resourceUnitIndx,ivec symbolsLoc, string toneType){

  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getRow1stToneIndicesInRU()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getRow1stToneIndicesInRU()..."<<endl;
    abort();
  }
  //cout << ".............." << toneType << endl;
  ivec tonedata=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).getRow1stToneIndices(toneType,symbolsLoc);
 
  return tonedata;
}

/*
cvec Framehandler::getTonesInDataBlk(int dataBlkNumber, string toneType, int antenna){
  
  int nRU=dataBlkInfo(dataBlkNumber).resourceBlocks.length();
  Array<cvec> tonesInRU(nRU);
  cvec tonesInDataBlk(0);
  for(int ru_cnt=0;ru_cnt<nRU;ru_cnt++)
    tonesInRU(ru_cnt)=getTonesInRU(dataBlkInfo(dataBlkNumber).resourceBlocks(ru_cnt),toneType,antenna);
  
  for(int ru_cnt=0;ru_cnt<nRU;ru_cnt++)
    append(tonesInDataBlk,tonesInRU(ru_cnt));
  return(tonesInDataBlk);
}

Array<cvec> Framehandler::getTonesInDataBlk(int dataBlkNumber, string toneType){
  
  int nRU=dataBlkInfo(dataBlkNumber).resourceBlocks.length();
  Array< Array<cvec> > tonesInRU(nRU);
  Array<cvec> tonesInDataBlk;
  for(int ru_cnt=0;ru_cnt<nRU;ru_cnt++)
    tonesInRU(ru_cnt)=getTonesInRU(dataBlkInfo(dataBlkNumber).resourceBlocks(ru_cnt),toneType);
  
  tonesInDataBlk.set_size(tonesInRU(0).length());
  for(int ary_cnt=0;ary_cnt<tonesInDataBlk.length();ary_cnt++)
    tonesInDataBlk(ary_cnt).set_size(0);
  
  for(int ru_cnt=0;ru_cnt<nRU;ru_cnt++)
    for(int ary_cnt=0;ary_cnt<tonesInRU(ru_cnt).length();ary_cnt++)
      append(tonesInDataBlk(ary_cnt),tonesInRU(ru_cnt)(ary_cnt));
    
    return(tonesInDataBlk);
  
}*/


Array<cmat> Framehandler::getTones(Array<imat> physicalSubcarriers,Array<imat> physicalSymbols){
  
  int rows,cols;
  Array<cmat> toneValues(physicalSubcarriers.length());
  for(int ant_cnt=0;ant_cnt<physicalSubcarriers.length();ant_cnt++)
  {
    rows=physicalSubcarriers(ant_cnt).rows();
    cols=physicalSubcarriers(ant_cnt).cols();
    toneValues(ant_cnt).set_size(rows,cols);
    for(int row_cnt=0;row_cnt<rows;row_cnt++)
      for(int col_cnt=0;col_cnt<cols;col_cnt++)
	toneValues(ant_cnt)(row_cnt,col_cnt)=frame(ant_cnt)(physicalSubcarriers(ant_cnt)(row_cnt,col_cnt),physicalSymbols(ant_cnt)(row_cnt,col_cnt));
  }
  return(toneValues);
  
}

Array< Array<cvec> > Framehandler::getTones(Array< Array<ivec> > physicalSubcarriers,Array< Array<ivec> > physicalSymbols){
  
  Array< Array<cvec> > toneValues(physicalSubcarriers.length());
  for(int ant_cnt=0;ant_cnt<physicalSubcarriers.length();ant_cnt++)
  {
    toneValues(ant_cnt).set_size(physicalSubcarriers(ant_cnt).length());
    for(int col_cnt=0;col_cnt<physicalSubcarriers(ant_cnt).length();col_cnt++)
      for(int row_cnt=0;row_cnt<physicalSubcarriers(ant_cnt)(col_cnt).length();row_cnt++)
	append(toneValues(ant_cnt)(col_cnt),frame(ant_cnt)(physicalSubcarriers(ant_cnt)(col_cnt)(row_cnt),physicalSymbols(ant_cnt)(col_cnt)(row_cnt)));
  }
  return(toneValues);
  
}

int Framehandler::getToneCountPerRU(int resourceUnitIndx, string toneType){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getToneCountPerRU()..."<<endl;
    abort();
  }
  return(frameStructure->getToneCountPerRU(resourceUnitIndx,toneType));
}
/*
int Framehandler::getToneCountPerDataBlk(int dataBlkNumber, string toneType){
  
  
  int nRU=dataBlkInfo(dataBlkNumber).resourceBlocks.length();
  int totalTones=0;
  for(int ru_cnt=0;ru_cnt<nRU;ru_cnt++)
    totalTones+=getToneCountPerRU(dataBlkInfo(dataBlkNumber).resourceBlocks(ru_cnt),toneType);
  return(totalTones);
  
}

ivec Framehandler::getRUsInDataBlk(int dataBlkNumber){
  
  return(dataBlkInfo(dataBlkNumber).resourceBlocks);
}

Offset_S Framehandler::getDataBlkStartingOffset(int dataBlkNumber){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getDataBlkStartingOffset()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getDataBlkStartingOffset()..."<<endl;
    abort();
  }
  return(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruOffset(dataBlkInfo(dataBlkNumber).resourceBlocks(0)));
}

Offset_S Framehandler::getRUOffset(int resourceUnitIndx){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling getDataBlkStartingOffset()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getDataBlkStartingOffset()..."<<endl;
    abort();
  }
  return(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruOffset(resourceUnitIndx));
}

int Framehandler::getDataBlkNumber(int resourceUnitIndx){
  
  for(int blk_cnt=0;blk_cnt<dataBlkInfo.length();blk_cnt++)
    if(find(dataBlkInfo(blk_cnt).resourceBlocks, resourceUnitIndx)!=-1)
      return(blk_cnt);
    
    return(-1);
}


ivec Framehandler::getDataBlkNumbers(int resourceUnitIndx){
  
  ivec toReturn(0);
  for(int blk_cnt=0;blk_cnt<dataBlkInfo.length();blk_cnt++)
    if(find(dataBlkInfo(blk_cnt).resourceBlocks, resourceUnitIndx)!=-1)
      append(toReturn,blk_cnt);
    
    return(toReturn);
}*/


Array<cmat> Framehandler::getFrame(int fromAntenna, int toAntenna, int fromSymbol, int toSymbol, int fromSubcarrier, int toSubcarrier){
  
  if(checkParams(fromAntenna, toAntenna, fromSymbol, toSymbol, fromSubcarrier, toSubcarrier)==false)
  {
    
    cout<<"[both:] Error in the parameters passed to getFrame()..."<<endl;
    abort();
  }
   Array<cmat> tempFrame;
  tempFrame.set_size(toAntenna-fromAntenna+1);
    for(int antenna_cnt=0;antenna_cnt<=(toAntenna-fromAntenna);antenna_cnt++)
    tempFrame(antenna_cnt)=frame(fromAntenna+antenna_cnt).get(fromSubcarrier,toSubcarrier,fromSymbol,toSymbol);
  
    return(tempFrame);
}

cmat Framehandler::getFrameFromSingleAntenna(int antenna, int fromSymbol, int toSymbol, int fromSubcarrier, int toSubcarrier){
  
  if(checkParams(antenna, antenna, fromSymbol, toSymbol, fromSubcarrier, toSubcarrier)==false)
  {
    
    cout<<"[both:] Error in the parameters passed to getFrame()..."<<endl;
    abort();
  }
  
  cmat tempFrame=frame(antenna).get(fromSubcarrier,toSubcarrier,fromSymbol,toSymbol);
  return(tempFrame);
}

void Framehandler::updateRUType(ivec ruIndices, string ruType){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling updateRUType()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling updateRUType()..."<<endl;
    abort();
  }
  int ruTypeNumber=findInStringArray(frameStructure->resourceUnitType,ruType);
  if(ruTypeNumber==-1)
  {
    cout<<"[both:] Error... Given 'ruType' does not exist in updateRUType()..."<<endl;
    abort();
  }
  for(int ru_cnt=0;ru_cnt<ruIndices.length();ru_cnt++)
    updateRUType(ruIndices(ru_cnt),ruTypeNumber);
}

void Framehandler::updateRUType(int ruIndex, string ruType){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling updateRUType()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling updateRUType()..."<<endl;
    abort();
  }
  int ruTypeNumber=findInStringArray(frameStructure->resourceUnitType,ruType);
  if(ruTypeNumber==-1)
  {
    cout<<"[both:] Error... Given ruType "<<ruType<<" does not exist in updateRUType()..."<<endl;
    abort();
  }
  updateRUType(ruIndex,ruTypeNumber);
}

void Framehandler::updateRUType(int ruIndex, int ruTypeNumber){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling updateRUType()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling updateRUType()..."<<endl;
    abort();
  }
  
  frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).updateRUType(ruIndex,ruTypeNumber);
}
/*
void Framehandler::setDataBlkInfo(Array<DataBlkInfo_S> &newDataBlkInfo, bool isAppend){
  
  if(dataBlkInfo.length()==0 || isAppend==false)
    dataBlkInfo=newDataBlkInfo;
  else
    append(dataBlkInfo,newDataBlkInfo);
}*/


void Framehandler::setCurrentFrameType(string frameTypeName){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling setCurrentFrameType()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling setCurrentFrameType()..."<<endl;
    abort();
  }
  int frmIndx=findInStringArray(frameStructure->frameType,frameTypeName);
  if(frmIndx==-1)
  {
    cout<<"[both:] Error .. Undefined frameType to setCurrentFrameType()..."<<endl;
    abort();
  }
  
  if(frameStructure->currentFrameTypeIndx!=frmIndx)
  {
    frameStructure->currentFrameTypeIndx=frmIndx;
    if(frameStructure->frameInfo(frmIndx).frameSize.numberOfSubcarriers==-1)
    {
      cout<<"[both:] Error... Set Frame Size of Frame Type "<<frameTypeName<<" before setting it as current Frame Type.."<<endl;
      abort();
    }
    initCurrentFrame();
  }
}

void Framehandler::setAntennaCount(int antCount, bool overwrite){
  
  if(overwrite || antennaCount==-1)
    antennaCount=antCount;
  else
    cout<<"[both:] Warning... Trying to overwrite antennaCount..."<<endl;
}
/*
void Framehandler::setDataGenInfo(Array<DataGenInfo_S> dataGenInfo,ivec dataBlkList, bool overwrite){
  
  if(dataBlkList.length()==0 && dataGenInfo.length()==dataBlkInfo.length())
    for(int dataBlk_cnt=0;dataBlk_cnt<dataBlkInfo.length();dataBlk_cnt++)  
      if(overwrite || dataBlkInfo(dataBlk_cnt).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_UNINIT_)
	dataBlkInfo(dataBlk_cnt).dataGenInfo=dataGenInfo(dataBlk_cnt);
    else
	cout<<"[both:] Warning .. Trying to overwrite DataBlk Info ... "<<endl;
  else
    if(dataGenInfo.length()==dataBlkList.length())  
    {
      for(int dataGenInfo_cnt=0;dataGenInfo_cnt<dataBlkList.length();dataGenInfo_cnt++)
	if(dataBlkList(dataGenInfo_cnt)<dataBlkInfo.length())
	  if(overwrite || dataBlkInfo(dataBlkList(dataGenInfo_cnt)).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_UNINIT_)
	    dataBlkInfo(dataBlkList(dataGenInfo_cnt)).dataGenInfo=dataGenInfo(dataGenInfo_cnt);
	  else
	    cout<<"[both:] Warning .. Trying to overwrite DataBlk Info at "<<dataBlkList(dataGenInfo_cnt)<<" ... "<<endl;
	else
	  cout<<"[both:] Warning ... Invalid dataBlk number "<< dataBlkList(dataGenInfo_cnt) <<" specified in setDataGenInfo()"<<endl;
    } 
    else
    {
      cout<<"[both:] Error ... Length mismatch between dataGenInfo and dataBlkList passed to setDataGenInfo()..."<<endl;
      abort();
    }
}

void Framehandler::setDataGenInfo(int dataBlkNumber, DataGenInfo_S dataGenInfo, bool overwrite){
  
  if(dataBlkNumber>=dataBlkInfo.length())
  {
    cout<<"[both:] Error... Invalid dataBlkNumber to setDataGenInfo()..."<<endl;
    abort();
  }
  if(overwrite || dataBlkInfo(dataBlkNumber).dataGenInfo.transmissionScheme==_TRANSMISSION_SCHEME_UNINIT_)
    dataBlkInfo(dataBlkNumber).dataGenInfo=dataGenInfo;
  else
    cout<<"[both:] Warning .. Trying to overwrite DataBlk Info at "<<dataBlkNumber<<" ... "<<endl;
  
}*/

void Framehandler::setTonesInRU(Array<cmat> data, int resourceUnitIndx, bool addOver){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling setTonesInRU()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling setTonesInRU()..."<<endl;
    abort();
  }
  if(data.length()!=antennaCount)
  {
    cout<<"[both:] Data need to be passed for "<<antennaCount<<" antennas in setTonesInRU..."<<endl;
    abort();
  }
  
  
  Offset_S ruOffset=frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruOffset(resourceUnitIndx);
  Size_S ruSize=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).resourceUnitSize;
  setFrame(data,0,antennaCount-1,ruOffset.symbolOffset,ruOffset.symbolOffset+ruSize.numberOfSymbols-1,
	   ruOffset.subcarrierOffset,ruOffset.subcarrierOffset+ruSize.numberOfSubcarriers-1,addOver);
	  
}

void Framehandler::setTonesInRU(cmat data, int resourceUnitIndx, int antenna, bool addOver){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling setTonesInRU()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling setTonesInRU()..."<<endl;
    abort();
  }
  Offset_S ruOffset=frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruOffset(resourceUnitIndx);
  Size_S ruSize=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).resourceUnitSize;
  setFrame(data,antenna,ruOffset.symbolOffset,ruOffset.symbolOffset+ruSize.numberOfSymbols-1,
	   ruOffset.subcarrierOffset,ruOffset.subcarrierOffset+ruSize.numberOfSubcarriers-1, addOver);
}

void Framehandler::setTonesInRU(Array<cvec> data, int resourceUnitIndx, string toneType, bool addOver){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling setTonesInRU()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling setTonesInRU()..."<<endl;
    abort();
  }
  if(data.length()!=antennaCount)
  {
    cout<<"[both:] Data need to be passed for "<<antennaCount<<" antennas in setTonesInRU..."<<endl;
    abort();
  }
  
  ivec myTonesIndx=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).getRow1stToneIndices(toneType);
  if(myTonesIndx.length()!=0)
  {
    Array<cmat> tones=getTonesInRU(resourceUnitIndx);
    if(data(0).length()<myTonesIndx.length())
    {
      cout<<"[both:] Insufficient data for setTonesInRU()..."<<endl;
      abort();
    }
    Size_S ruSize=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).resourceUnitSize;
    for(int ant_cnt=0;ant_cnt<tones.length();ant_cnt++)
    {  
      if(addOver)
	for(int tone_cnt=0;tone_cnt<myTonesIndx.length();tone_cnt++)
	  tones(ant_cnt)(myTonesIndx(tone_cnt)%ruSize.numberOfSubcarriers,floor_i(myTonesIndx(tone_cnt)/ruSize.numberOfSubcarriers))+=data(ant_cnt)(tone_cnt);
      else
	for(int tone_cnt=0;tone_cnt<myTonesIndx.length();tone_cnt++)
	  tones(ant_cnt)(myTonesIndx(tone_cnt)%ruSize.numberOfSubcarriers,floor_i(myTonesIndx(tone_cnt)/ruSize.numberOfSubcarriers))=data(ant_cnt)(tone_cnt);
    }
    setTonesInRU(tones, resourceUnitIndx); 
  }
  else
  {
    cout<<"[both:] Error... undefined Tone Type to setTonesInRU()..."<<endl;
    abort();
  }
}

void Framehandler::setTonesInRU(Array<cvec> data,ivec symbolLoc, int resourceUnitIndx, string toneType,bool addOver)
{
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling setTonesInRU()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling setTonesInRU()..."<<endl;
    abort();
  }
  if(data.length()!=antennaCount)
  {
    cout<<"[both:] Data need to be passed for "<<antennaCount<<" antennas in setTonesInRU..."<<endl;
    abort();
  }
  
  ivec myTonesIndx=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).getRow1stToneIndices(toneType,symbolLoc);
 	
  if(myTonesIndx.length()!=0)
  {
    Array<cmat> tones=getTonesInRU(resourceUnitIndx);
    if(data(0).length()<myTonesIndx.length())
    {
      cout<<"[both:] Insufficient data for setTonesInRU()..."<<endl;
      abort();
    }
    Size_S ruSize=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).resourceUnitSize;
    for(int ant_cnt=0;ant_cnt<tones.length();ant_cnt++)
    {  
      if(addOver)
      {
	for(int tone_cnt=0;tone_cnt<myTonesIndx.length();tone_cnt++)
	{
	  tones(ant_cnt)(myTonesIndx(tone_cnt)%ruSize.numberOfSubcarriers,floor_i(myTonesIndx(tone_cnt)/ruSize.numberOfSubcarriers))+=data(ant_cnt)(tone_cnt);
	  }
      }
      else
      {
	for(int tone_cnt=0;tone_cnt<myTonesIndx.length();tone_cnt++)
	{
	  tones(ant_cnt)(myTonesIndx(tone_cnt)%ruSize.numberOfSubcarriers,floor_i(myTonesIndx(tone_cnt)/ruSize.numberOfSubcarriers))=data(ant_cnt)(tone_cnt);
	}
      }
    }
    setTonesInRU(tones, resourceUnitIndx); 
  }
  else
  {
    cout<<"[both:] Error... undefined Tone Type to setTonesInRU()..."<<endl;
    abort();
  }
}

void Framehandler::setTonesInRU(cvec data, int resourceUnitIndx, string toneType, int antenna, bool addOver){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling setTonesInRU()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling setTonesInRU()..."<<endl;
    abort();
  }
  ivec myTonesIndx=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).getRow1stToneIndices(toneType);
  if(myTonesIndx.length()!=0)
  {
    cmat tones=getTonesInRU(resourceUnitIndx,antenna);
    if(data.length()<myTonesIndx.length())
    {
      cout<<"[both:] Insufficient data for setTonesInRU()..."<<endl;
      abort();
    }
    Size_S ruSize=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).resourceUnitSize;
    if(addOver)
      for(int tone_cnt=0;tone_cnt<myTonesIndx.length();tone_cnt++)
	tones(myTonesIndx(tone_cnt)%ruSize.numberOfSubcarriers,floor_i(myTonesIndx(tone_cnt)/ruSize.numberOfSubcarriers))+=data(tone_cnt);
    else
      for(int tone_cnt=0;tone_cnt<myTonesIndx.length();tone_cnt++)
	tones(myTonesIndx(tone_cnt)%ruSize.numberOfSubcarriers,floor_i(myTonesIndx(tone_cnt)/ruSize.numberOfSubcarriers))=data(tone_cnt);
	
    setTonesInRU(tones, resourceUnitIndx,antenna);
  }
  else
  {
    cout<<"[both:] Error... undefined Tone Type to setTonesInRU()..."<<endl;
    abort();
  }
}
/*
void Framehandler::setTonesInDataBlk(Array<cvec> data, int dataBlkNumber, string toneType, bool addOver){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling setTonesInDataBlk()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling setTonesInDataBlk()..."<<endl;
    abort();
  }
  if(data.length()!=antennaCount)
  {
    cout<<"[both:] Data need to be passed for "<<antennaCount<<" antennas in setTonesInDataBlk..."<<endl;
    abort();
  }
  
  Array<cvec> tempData(data.length());
  int nRU=dataBlkInfo(dataBlkNumber).resourceBlocks.length();
  ivec totalTones(nRU);
  totalTones.clear();
  for(int ru_cnt=0;ru_cnt<nRU;ru_cnt++)
    totalTones(ru_cnt)=frameStructure->resourceUnitInfo(frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).ruTypeOrder(dataBlkInfo(dataBlkNumber).resourceBlocks(ru_cnt))).getRow1stToneIndices(toneType).length();
  
  if(sum(totalTones)==0)
  {
    cout<<"[both:] Error... undefined Tone Type to setTonesInDataBlk()..."<<endl;
    abort();
  }
  
  if(data(0).length()<sum(totalTones))
  {
    cout<<"[both:] Insufficient data for setTonesInDataBlk..."<<endl;
    abort();
  }
  int data_offset=0;
  for(int ru_cnt=0;ru_cnt<dataBlkInfo(dataBlkNumber).resourceBlocks.length();ru_cnt++)
  {
    for(int data_cnt=0;data_cnt<data.length();data_cnt++)
      tempData(data_cnt)=data(data_cnt).get(data_offset,data_offset+totalTones(ru_cnt)-1);
    data_offset+=totalTones(ru_cnt);
    setTonesInRU(tempData,dataBlkInfo(dataBlkNumber).resourceBlocks(ru_cnt),toneType, addOver);
  }
  
}*/

void Framehandler::setFrame(Array<cmat> data, int fromAntenna, int toAntenna, int fromSymbol, int toSymbol, int fromSubcarrier, int toSubcarrier, bool addOver){
  
  if(checkParams(fromAntenna, toAntenna, fromSymbol, toSymbol, fromSubcarrier, toSubcarrier)==false)
  {
    cout<<"[both:] Error in the parameters passed to setFrame()..."<<endl;
    abort();
  }
  if(addOver)
    for(int antenna_cnt=0;antenna_cnt<=(toAntenna-fromAntenna);antenna_cnt++)
      frame(fromAntenna+antenna_cnt).set_submatrix(fromSubcarrier,fromSymbol,frame(fromAntenna+antenna_cnt).get(fromSubcarrier,fromSubcarrier+data(antenna_cnt).rows()-1,fromSymbol,fromSymbol+data(antenna_cnt).cols()-1)+data(antenna_cnt));
  else
    for(int antenna_cnt=0;antenna_cnt<=(toAntenna-fromAntenna);antenna_cnt++)
      frame(fromAntenna+antenna_cnt).set_submatrix(fromSubcarrier,fromSymbol,data(antenna_cnt));
}

void Framehandler::setFrame(cmat data, int antenna, int fromSymbol, int toSymbol, int fromSubcarrier, int toSubcarrier, bool addOver){
  
  if(checkParams(antenna, antenna, fromSymbol, toSymbol, fromSubcarrier, toSubcarrier)==false)
  {
    cout<<"[both:] Error in the parameters passed to setFrame()..."<<endl;
    abort();
  }
  if(addOver)
    frame(antenna).set_submatrix(fromSubcarrier,fromSymbol,frame(antenna).get(fromSubcarrier,fromSubcarrier+data.rows()-1,fromSymbol,fromSymbol+data.cols()-1)+data);
  else
    frame(antenna).set_submatrix(fromSubcarrier,fromSymbol,data);
}

void Framehandler::setFrameColumn(Array<cvec> data, int columnIndx, bool addOver){
  
  if(columnIndx<frame(0).cols())
  {
    if(addOver)
      for(int antenna_cnt=0;antenna_cnt<frame.length();antenna_cnt++)
	frame(antenna_cnt).set_col(columnIndx,frame(antenna_cnt).get_col(columnIndx)+data(antenna_cnt));
    else
      for(int antenna_cnt=0;antenna_cnt<frame.length();antenna_cnt++)
	frame(antenna_cnt).set_col(columnIndx,data(antenna_cnt));
  }
  else
    cout<<"[both:] Error ... Column Index exceeding frame Size...Skipping setFrameColumn()..."<<endl;
}
void Framehandler::setFrameRow(Array<cvec> data, int rowIndx, bool addOver){
  
  if(rowIndx<frame(0).rows())
  {
    if(addOver)
      for(int antenna_cnt=0;antenna_cnt<frame.length();antenna_cnt++)
	frame(antenna_cnt).set_row(rowIndx,frame(antenna_cnt).get_row(rowIndx)+data(antenna_cnt));
    else
      for(int antenna_cnt=0;antenna_cnt<frame.length();antenna_cnt++)
	frame(antenna_cnt).set_row(rowIndx,data(antenna_cnt));
  }
  else
    cout<<"[both:] Error ... Row Index exceeding frame Size...Skipping setFrameRow()..."<<endl;
}



// void Framehandler::clearDataBlkInfo(){
//   
//   dataBlkInfo.set_length(0);
// }


void Framehandler::clearFrame(int fromAntenna, int toAntenna, int fromSymbol, int toSymbol, int fromSubcarrier, int toSubcarrier){
  
  if(checkParams(fromAntenna, toAntenna, fromSymbol, toSymbol, fromSubcarrier, toSubcarrier)==false)
  {
    cout<<"[both:] Error in the parameters passed to clearFrame()..."<<endl;
    abort();
  }
  
  for(int antenna_cnt=0;antenna_cnt<=(toAntenna-fromAntenna);antenna_cnt++)
    frame(fromAntenna+antenna_cnt).set_submatrix(fromSubcarrier,fromSymbol,zeros_c(toSubcarrier-fromSubcarrier+1,toSymbol-fromSymbol+1));
}


bool Framehandler::checkParams(int &fromAntenna, int &toAntenna, int &fromSymbol, int &toSymbol, int &fromSubcarrier, int &toSubcarrier){
  
  if(isFrameStructPointed==false)
  {
    cout<<"[both:] Please load the frameStructure before calling mergeResourceUnits()..."<<endl;
    abort();
  }
  if(frameStructure->isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling mergeResourceUnits()..."<<endl;
    abort();
  }
  
  if(frameStructure->currentFrameTypeIndx==-1)
  {
    cout<<"[both:] Error... currentFrameTypeIndx need to be initialized before handling the frame..."<<endl;
    abort();
  }
  
  if(fromAntenna!=-1 && toAntenna==-1)
    toAntenna=fromAntenna;
  else if(fromAntenna==-1 && toAntenna!=-1)
  {cout<<"Please Specify a valid fromAntenna .."<<endl;return(false);}
  else if(fromAntenna==-1 && toAntenna==-1)
  {
    fromAntenna=0;
    toAntenna=antennaCount-1;
  }
  else
  {
    if(fromAntenna>toAntenna)
    {cout<<"fromAntenna can't be greater than toAntenna .."<<endl;return(false);}
    if(toAntenna>=antennaCount)
    {cout<<"toAntenna can't be greater than "<<antennaCount<<" for the current frame.."<<endl;return(false);}
    if(fromAntenna>=antennaCount)
    {cout<<"fromAntenna can't be greater than "<<antennaCount<<" for the current frame.."<<endl;return(false);}
  }
  
  if(fromSymbol!=-1 && toSymbol==-1)
    toSymbol=fromSymbol;
  else if(fromSymbol==-1 && toSymbol!=-1)
  {cout<<"Please Specify a valid fromSymbol .."<<endl;return(false);}
  else if(fromSymbol==-1 && toSymbol==-1)
  {
    fromSymbol=0;
    toSymbol=frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSymbols-1;
  }
  else
  {
    if(fromSymbol>toSymbol)
    {cout<<"fromSymbol can't be greater than toSymbol .."<<endl;return(false);}
    if(toSymbol>=frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSymbols)
    {cout<<"toSymbol can't be greater than "<<frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSymbols<<" for the current frame.."<<endl;return(false);}
    if(fromSymbol>=frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSymbols)
    {cout<<"fromSymbol can't be greater than "<<frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSymbols<<" for the current frame.."<<endl;return(false);}
    
  }
  
  if(fromSubcarrier!=-1 && toSubcarrier==-1)
    toSubcarrier=fromSubcarrier;
  else if(fromSubcarrier==-1 && toSubcarrier!=-1)
  {cout<<"Please Specify a valid fromSubcarrier .."<<endl;return(false);}
  else if(fromSubcarrier==-1 && toSubcarrier==-1)
  {
    fromSubcarrier=0;
    toSubcarrier=frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSubcarriers-1;
  }
  else
  {
    if(fromSubcarrier>toSubcarrier)
    {cout<<"fromSubcarrier can't be greater than toSubcarrier .."<<endl;return(false);}
    if(toSubcarrier>=frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSubcarriers)
    {cout<<"toSubcarrier can't be greater than "<<frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSubcarriers<<" for the current frame.."<<endl;return(false);}
    if(fromSubcarrier>=frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSubcarriers)
    {cout<<"fromSubcarrier can't be greater than "<<frameStructure->frameInfo(frameStructure->currentFrameTypeIndx).frameSize.numberOfSubcarriers<<" for the current frame.."<<endl;return(false);}
  }
  
  return(true);
}


void Framehandler::printAllTones(int rbNumber, string fileName){
  
  ResourceUnitInfo_S ruInfo=getResourceUnitInfo(rbNumber);
  std::ofstream fout(fileName.c_str(), std::ios::out);
  ivec filledTones(0);
  for(int toneType_cnt=0;toneType_cnt<ruInfo.toneTypes.length();toneType_cnt++)
  {
    Array<cvec> tonesAcrossAntenna=getTonesInRU(rbNumber,ruInfo.toneTypes(toneType_cnt));
    cvec toPrint=zeros_c(tonesAcrossAntenna(0).length());
    
    for(int ant_cnt=0;ant_cnt<tonesAcrossAntenna.length();ant_cnt++)
      toPrint+=tonesAcrossAntenna(ant_cnt);
    
    fout<<ruInfo.toneTypes(toneType_cnt)<<"="<<toPrint<<";"<<endl;
    append(filledTones,ruInfo.getRow1stToneIndices(ruInfo.toneTypes(toneType_cnt)));
    
  } 
  
  ivec allTones=getIntegers(0,ruInfo.resourceUnitSize.numberOfSubcarriers*ruInfo.resourceUnitSize.numberOfSymbols-1);
  ivec unfilledTones=findNot(allTones,filledTones);
  if(unfilledTones.length())
    fout<<"UnfilledToneIndices="<<unfilledTones<<";"<<endl;
  
  fout.close();
}


bool crossCheckTones(string fileName, Framehandler &frameHandler, int rbNumber, Array<cmat> &crossCheckMatrix, bool neglectUnfilled){
  
  bool toReturn=true;
  ResourceUnitInfo_S ruInfo=frameHandler.getResourceUnitInfo(rbNumber);
  cvec dataFromFile;
  Array<cvec> dataFromMat;
  for(int toneType_cnt=0;toneType_cnt<ruInfo.toneTypes.length();toneType_cnt++)
  {
    if(!parse(fileName,ruInfo.toneTypes(toneType_cnt),dataFromFile))
      {cout<<"Tonetype "<<ruInfo.toneTypes(toneType_cnt)<<" missing in the file in crossCheckTones()...skipping check for that tone..."<<endl;toReturn=false; break;}
    
    dataFromMat=getFromArrayOfMatrix(crossCheckMatrix,ruInfo.getRow1stToneIndices(ruInfo.toneTypes(toneType_cnt)));
    for(int cnt=0;cnt<dataFromMat.length();cnt++)
      if(abs(sum(dataFromFile-dataFromMat(cnt)))>1e-5) toReturn=false;
    
    if(toReturn)
      cout<<"Tonetype \""<<ruInfo.toneTypes(toneType_cnt)<<"\" passed the testing..."<<endl;
    else
      {	cout<<"Tonetype \""<<ruInfo.toneTypes(toneType_cnt)<<"\" failed the testing..."<<endl; break;}
      
  }
  
  if(!neglectUnfilled)
  {  
    ivec unfilledTones;
    parse(fileName,"UnfilledToneIndices",unfilledTones);
    if(unfilledTones.length())
    {
      dataFromMat=getFromArrayOfMatrix(crossCheckMatrix,unfilledTones);
      for(int cnt=0;cnt<dataFromMat.length();cnt++)
	if(abs(sum(dataFromMat(cnt)))>1e-5) 
	  {	cout<<"Unfilled tones are not matching to zeros ... If filled outside of frameHandler in the transmitter this error can be neglected ..."<<endl; toReturn=false;}
    }
    
  }
  return(toReturn);
}

