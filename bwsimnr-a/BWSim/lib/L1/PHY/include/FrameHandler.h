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

#ifndef _FRM_HNDLR_H_
#define _FRM_HNDLR_H_

#include "FrameStructure.h"
#include "StructsAndEnums.h"

class Framehandler{
  
  
  public:
	Array<cmat> frame;
	
  private:

    FrameStructure *frameStructure;
    bool isFrameStructPointed;
    int antennaCount;
	
  public:
	Framehandler();
	void initCurrentFrame();
	void freeFrameMemory();
	bool isReady();
	
	bool isFrameStructurePointed();
	bool pointFrameStructure(FrameStructure *frameStructPointer);
	FrameStructure* getFrameStructurePointer();
	bool isResourceUnitAvailable(string ruType);
	int getAntennaCount();
// 	int getBlkCountInCurrentFrame();
	Size_S getResourceUnitSize(string ruType);
	
	ResourceUnitInfo_S getResourceUnitInfo(string ruType);
	ResourceUnitInfo_S getResourceUnitInfo(int ruIndex);
	Array<ivec> getResourceUnit(string ruType);
	imat getResourceUnitMatrix(string ruType);
	imat getResourceUnitMatrix(int ruIndex);
	imat getResourceUnitMatrix(string ruType,string toneType);
	Size_S getFrameSize(string frameTypeName="");
	int getAllRuCount(string frameTypeName="");
	int getRuCountPerLevel(string frameTypeName="");
	string getCurrentFrameType();
// 	Array<DataBlkInfo_S> getDataBlkInfo();
// 	DataBlkInfo_S getDataBlkInfo(int dataBlkNumber);
// 	DataGenInfo_S getDataGenInfo(int dataBlkNumber);
	ivec getOccupiedSymbols(ivec resourceUnitsIndices);
// 	ivec getOccupiedSymbols(int blkCount);
	ivec getToneLocationsInRU(string ruType,string toneType);
	Array<cmat> getTonesInRU(int resourceUnitIndx);
	cmat getTonesInRU(int resourceUnitIndx, int antenna);
	cvec getTonesInRU(int resourceUnitIndx, string toneType, int antenna);
	Array<cvec> getTonesInRU(int resourceUnitIndx, string toneType);
	ivec getRow1stToneIndicesInRU(int resourceUnitIndx,string toneType);
	ivec getRow1stToneIndicesInRUovereSymbols(int resourceUnitIndx, ivec symbolsLoc, string toneType);
// 	cvec getTonesInDataBlk(int dataBlkNumber, string toneType, int antenna);
// 	Array<cvec> getTonesInDataBlk(int dataBlkNumber, string toneType);
	Array<cmat> getTones(Array<imat> physicalSubcarriers,Array<imat> physicalSymbols);
	Array< Array<cvec> > getTones(Array< Array<ivec> > physicalSubcarriers,Array< Array<ivec> > physicalSymbols);
	int getToneCountPerRU(int resourceUnitIndx, string toneType);
// 	int getToneCountPerDataBlk(int dataBlkNumber, string toneType);
// 	ivec getRUsInDataBlk(int dataBlkNumber);
// 	Offset_S getDataBlkStartingOffset(int dataBlkNumber);
// 	Offset_S getRUOffset(int resourceUnitIndx);
// 	int getDataBlkNumber(int resourceUnitIndx);
// 	ivec getDataBlkNumbers(int resourceUnitIndx);
	Array<cmat> getFrame(int fromAntenna=-1, int toAntenna=-1, int fromSymbol=-1, int toSymbol=-1, int fromSubcarrier=-1, int toSubcarrier=-1);
	cmat getFrameFromSingleAntenna(int antenna, int fromSymbol=-1, int toSymbol=-1, int fromSubcarrier=-1, int toSubcarrier=-1);
	void defineResourceUnit(string ruType, Size_S ruSize, Array<ivec> tones);
	void loadResourceUnit(string ruType, string fileName,int rowUpShift=0, int columnRightShift=0, ivec CSIRSsymbolsToswap="");
	
	void updateRUType(ivec ruIndices, string ruType);
	void updateRUType(int ruIndex, string ruType);
	void updateRUType(int ruIndex, int ruTypeNumber);
// 	void setDataBlkInfo(Array<DataBlkInfo_S> &newDataBlkInfo, bool isAppend=false);
	void setCurrentFrameType(string frameTypeName);
	void setAntennaCount(int antCount, bool overwrite=false);
// 	void setDataGenInfo(Array<DataGenInfo_S> dataGenInfo,ivec dataBlkList, bool overwrite=false);
// 	void setDataGenInfo(int dataBlkNumber, DataGenInfo_S dataGenInfo, bool overwrite=false);
	void setTonesInRU(Array<cmat> data, int resourceUnitIndx,bool addOver=false);
	void setTonesInRU(cmat data, int resourceUnitIndx, int antenna,bool addOver=false);
	void setTonesInRU(Array<cvec> data,ivec symbolLoc, int resourceUnitIndx, string toneType,bool addOver=false);
	void setTonesInRU(Array<cvec> data, int resourceUnitIndx, string toneType,bool addOver=false);
	void setTonesInRU(cvec data, int resourceUnitIndx, string toneType, int antenna,bool addOver=false);
// 	void setTonesInDataBlk(Array<cvec> data, int dataBlkNumber, string toneType,bool addOver=false);
// 	void setTonesInDataBlk(cvec data, int dataBlkNumber, string toneType, int antenna,bool addOver=false);
	void setFrame(Array<cmat> data, int fromAntenna=-1, int toAntenna=-1, int fromSymbol=-1, int toSymbol=-1, int fromSubcarrier=-1, int toSubcarrier=-1,bool addOver=false);
	void setFrame(cmat data, int antenna, int fromSymbol=-1, int toSymbol=-1, int fromSubcarrier=-1, int toSubcarrier=-1,bool addOver=false);
	void setFrameColumn(Array<cvec> data, int columnIndx,bool addOver=false);
	void setFrameRow(Array<cvec> data, int rowIndx,bool addOver=false);
// 	void clearDataBlkInfo();
	void clearFrame(int fromAntenna=-1, int toAntenna=-1, int fromSymbol=-1, int toSymbol=-1, int fromSubcarrier=-1, int toSubcarrier=-1);
	void printAllTones(int rbNumber, string fileName);
 private:
	bool checkParams(int &fromAntenna, int &toAntenna, int &fromSymbol, int &toSymbol, int &fromSubcarrier, int &toSubcarrier);
	
};

bool crossCheckTones(string fileName, Framehandler &frameHandler, int rbNumber, Array<cmat> &crossCheckMatrix, bool neglectUnfilled=false);

#endif

