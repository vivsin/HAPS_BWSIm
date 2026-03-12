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

#ifndef _FRM_STRUCT_H_
#define _FRM_STRUCT_H_

#include "StructsAndEnums.h"


struct ResourceUnitInfo_S{
  
  Array<ivec> resourceUnitTones;
  ivec toneNumbers;
  Size_S resourceUnitSize;
  Array<string> toneTypes;
  Array<ivec> toneTypesMap;
  
  ivec getRow1stToneIndices(int toneNumber)
  {
    int toneIndx=find(toneNumbers,toneNumber);
    if(toneIndx!=-1)
    {
      return(resourceUnitTones(toneIndx));
    }
    else
      return(ivec(""));
  }
  ivec getRow1stToneIndices(string toneType)
  {
    ivec toneIndices(0);
    int indx=findInStringArray(toneTypes,toneType);
    if(indx!=-1)
    {
      ivec toneNumbersToExtract=toneTypesMap(indx);
   //   cout << "toneNumbersToExtract :" << toneNumbersToExtract << endl;
      for(int tone_num=0;tone_num<toneNumbersToExtract.length();tone_num++)
	append(toneIndices,getRow1stToneIndices(toneNumbersToExtract(tone_num)));
    }

    sort(toneIndices);
    return(toneIndices);
  }
  ivec getRow1stToneIndices(string toneType, ivec tsymbols)
  {
    ivec toneIndices(0);
    int indx=findInStringArray(toneTypes,toneType);
    if(indx!=-1)
    {
      ivec toneNumbersToExtract=toneTypesMap(indx);
      for(int tone_num=0;tone_num<toneNumbersToExtract.length();tone_num++)
	append(toneIndices,getRow1stToneIndices(toneNumbersToExtract(tone_num)));
    }

    ivec toneIndicesOverSymbols(0);
    for(int sym_cnt=0;sym_cnt<tsymbols.length();sym_cnt++)
      append(toneIndicesOverSymbols,getIntegers(0,11)+tsymbols(sym_cnt)*12);
    
    ivec temp=find(toneIndicesOverSymbols,toneIndices);
    temp=remove(temp,-1);
    toneIndices=toneIndicesOverSymbols(temp);
    sort(toneIndices);
    
    return(toneIndices);
  }
  ivec getToneNumbers(string toneType)
  {
    int indx=findInStringArray(toneTypes,toneType);
    if(indx!=-1)
      return(toneTypesMap(indx));
    else
      return(ivec(""));
  }
  ResourceUnitInfo_S() : resourceUnitTones(0),toneTypes(0),toneTypesMap(0)  {}
};
struct Offset_S{
  
  int subcarrierOffset;
  int symbolOffset;
  
  Offset_S(){
    
    subcarrierOffset=-1;
    symbolOffset=-1;
  }
  Offset_S(int symOffset, int subcOffset){
    
    subcarrierOffset=subcOffset;
    symbolOffset=symOffset;
  }
};
struct FrameInfo_S{
  
  Array< Array<string> > structure;
  ivec ruCountPerLevel;
  Size_S frameSize;
  ivec ruTypeOrder;
  Array<Offset_S> ruOffset;
  
  FrameInfo_S(){
    
    structure.set_size(0); 
    ruOffset.set_size(0);
    ruTypeOrder.set_size(0);
    ruCountPerLevel.set_size(0);
  }
  void updateRUType(int ruIndex, int ruTypeNumber)
  {
    if(ruIndex>=ruTypeOrder.length())
    {
      cout<<"[both:] Error... Invalid ruIndex "<<ruIndex<<" to updateRUType()..."<<endl;
    }
    ruTypeOrder(ruIndex)=ruTypeNumber;
  }
  
};
class FrameStructure{
  
  public:  
    
    int idCell;
    bool isFrameStructParsed;
    Array<string> resourceUnitType;
    Array<ResourceUnitInfo_S> resourceUnitInfo;
    Array<string> frameType;
    Array<FrameInfo_S> frameInfo;
    int currentFrameTypeIndx;
    imat currentFrameStructure;
    int isDownlink;
   
  public:
    
    FrameStructure();
    bool isReady();
    bool isFrameStructureParsed();
    bool isResourceUnitAvailable(string ruType);
    void parseFrameStructure(int frameIndx);
    Array<Offset_S> parseFrameStructure(Array< Array<string> > structure);
    void initFrameStructure(int idCell=0);
    void defineResourceUnit(string ruType, Size_S ruSize, Array<ivec> tones);
    void loadResourceUnit(string ruType, string fileName,int rowUpShift=0, int columnRightShift=0, ivec columnsToSwap="",ivec rowsToSwap="",bool rowsOperations1st=false,int nRepititions=1);
   
    void modifyResourceUnit(string baseRuType, string newRuType, string unalteredToneType,int toneNumberOffset, string toneTypeSuffix, int rowUpShift=0, int columnRightShift=0, ivec columnsToSwap="",ivec rowsToSwap="",bool rowsOperations1st=false);
    
    Size_S getResourceUnitSize(string ruType);
    imat getResourceUnitMatrix(string ruType);
    imat getResourceUnitMatrix(int ruIndex);
    string getResourceUnitType(int ruIndex);
    void loadFrameStructure(string frameTypeName, string filename, bool isDownlinkFrame);
    void mergeResourceUnits(string baseRuType, string newRuType,string resultantRuType ,string mergerToneType, ivec blockedSymbols="", Array<ivec> blockedSCPerSym="");
    void mergeResourceUnits(string baseRuType, Array<string> newRuTypes,Array<ivec> colPositions, ivec rowPositions,string resultantRuType ,string mergerToneType, ivec blockedSymbols="", Array<ivec> blockedSCPerSym="");
    imat getMergedResourceUnit(string baseRuType, Array<string> newRuTypes ,string mergerToneType);
    imat getMergedResourceUnit(string baseRuType, Array< string > newRuTypes, string mergerToneType, Array< ivec > colPositions,ivec rowPositions, ivec blockedSymbols="", Array< ivec > blockedSCPerSym="");
    ResourceUnitInfo_S getResourceUnitInfo(string ruType);
    ResourceUnitInfo_S getResourceUnitInfo(int ruIndex);
    Array<ivec> getResourceUnit(string ruType);
    int getAllRuCount(string frameTypeName="");
    int getRuCountPerLevel(string frameTypeName="");
    int getRuLevels(string frameTypeName="");
    void setCurrentFrameType(string frameTypeName);
    void updateRUType(ivec ruIndices, string ruType);
    void updateRUType(int ruIndex, string ruType);
    void updateRUType(int ruIndex, int ruTypeNumber);
    int getToneCountPerRU(int resourceUnitIndx, string toneType);
    int getToneCountPerRU(int resourceUnitIndx, string toneType, ivec symbols);
    ivec getTones(int resourceUnitIndx, string toneType);
    ivec getTones(int resourceUnitIndx, string toneType, ivec symbols);
    
    Offset_S getRUOffset(int resourceUnitIndx);
    
    Size_S getFrameSize(string frameTypeName="");
    void updateCurrentFrameStructure();
    imat* getCurrentFrameStructurePointer();
//     imat zero_i(int arg1, int arg2);
//     imat zero_i(int arg1, int arg2);
//     imat zero_i(int arg1, int arg2);
};




#endif
