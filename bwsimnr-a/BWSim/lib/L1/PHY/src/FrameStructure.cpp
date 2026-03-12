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


#include "../include/FrameStructure.h"

FrameStructure::FrameStructure(){

  resourceUnitType.set_length(0);
  resourceUnitInfo.set_length(0);
  frameType.set_length(0);
  frameInfo.set_length(0);
  currentFrameTypeIndx=-1;
  idCell=-1;
  isDownlink=-1;
  isFrameStructParsed=false;
}

bool FrameStructure::isReady(){

  if(frameType.length()==0)
  {
    cout<<"Please load a frame format before initializing the frameStructure..."<<endl;
    return(false);
  }
  if(resourceUnitType.length()==0)
  {
    cout<<"Please load a resource unit type before initializing the frameStructure..."<<endl;
    return(false);
  }


  for(int frm_Indx=0;frm_Indx<frameInfo.length();frm_Indx++)
  {
    if(frameInfo(frm_Indx).structure.size()==0)
    {
      cout<<"[both:] Error... Please load a frame structure before initializing the frameStructure..."<<endl;
      return(false);
    }
    for(int level_1=0;level_1<frameInfo(frm_Indx).structure.length();level_1++)
      for(int level_2=0;level_2<frameInfo(frm_Indx).structure(level_1).length();level_2++)
      {
	if(atof((frameInfo(frm_Indx).structure(level_1))(level_2).c_str())==0.0)
	  if(findInStringArray(resourceUnitType,(frameInfo(frm_Indx).structure(level_1))(level_2))==-1)
	  {

	    cout<<"Undefined Resource Unit type at "<<level_1<<" , "<<level_2<<" for frame Type "<< frameType(frm_Indx)<<endl;
	    return(false);
	  }
      }
  }

  return(true);
}


bool FrameStructure::isFrameStructureParsed(){

  return(isFrameStructParsed);
}


bool FrameStructure::isResourceUnitAvailable(string ruType){

  if(findInStringArray(resourceUnitType,ruType)==-1)
    return(false);
  else
    return(true);

}

void FrameStructure::parseFrameStructure(int frameIndx){


  int symCount=0,subcCount=0;

  for(int level_1=0;level_1<frameInfo(frameIndx).structure.length();level_1++)
    if(atof((frameInfo(frameIndx).structure(level_1))(0).c_str())==0.0)
      subcCount+=getResourceUnitSize((frameInfo(frameIndx).structure(level_1))(0)).numberOfSubcarriers;
    else
      subcCount+=floor_i(atof((frameInfo(frameIndx).structure(level_1))(0).c_str()));

    for(int level_2=0;level_2<frameInfo(frameIndx).structure(0).length();level_2++)
    {
      if(atof((frameInfo(frameIndx).structure(0))(level_2).c_str())==0.0)
	symCount+=getResourceUnitSize((frameInfo(frameIndx).structure(0))(level_2)).numberOfSymbols;
      else
      {
	int dotLocation=(frameInfo(frameIndx).structure(0))(level_2).find(".");
	symCount+=atoi(((frameInfo(frameIndx).structure(0))(level_2).substr(dotLocation+1,(frameInfo(frameIndx).structure(0))(level_2).length()-dotLocation)).c_str());
      }
    }

    frameInfo(frameIndx).frameSize.numberOfSymbols=symCount;
    frameInfo(frameIndx).frameSize.numberOfSubcarriers=subcCount;

    ivec boundary;
    int symOffset,subcOffset;
    Size_S ruSize;
    imat ruMatrix;
    imat myFrame;
    Offset_S tempOffset;
    myFrame=zeros_i(subcCount,symCount)-1;
    boundary=zeros_i(symCount);

    Array< Array<Offset_S> > ruArrayOffset(frameInfo(frameIndx).structure(0).length());
    Array<ivec> ruArrayTypeOrder(frameInfo(frameIndx).structure(0).length());
    for(int level_2=0;level_2<frameInfo(frameIndx).structure(0).length();level_2++)
    {
      ruArrayOffset(level_2).set_size(0);
      ruArrayTypeOrder(level_2).set_size(0);
    }
    ivec ruCountPerLevel(frameInfo(frameIndx).structure(0).length());
    ruCountPerLevel.clear();
    for(int level_1=0;level_1<frameInfo(frameIndx).structure.length();level_1++)
      for(int level_2=0;level_2<frameInfo(frameIndx).structure(level_1).length();level_2++)
      {
	symOffset=min_index(boundary);
	subcOffset=min(boundary);
	if(atof((frameInfo(frameIndx).structure(level_1))(level_2).c_str())==0.0)
	{

	  ruSize=getResourceUnitSize((frameInfo(frameIndx).structure(level_1))(level_2));
	  if((symOffset+ruSize.numberOfSymbols)>frameInfo(frameIndx).frameSize.numberOfSymbols || (subcOffset+ruSize.numberOfSubcarriers)>frameInfo(frameIndx).frameSize.numberOfSubcarriers)
	  {
	    cout<<"[both:] Unable to pack the RU at "<<level_1<<","<<level_2<<" because of the frameSize determined from the first row and column..."<<endl;
	    abort();
	  }
	  else
	  {
	    if((find(boundary(symOffset,symOffset+ruSize.numberOfSymbols-1)>subcOffset)).length()>0)
	    {
	      cout<<"[both:] Unable to pack the RU at "<<level_1<<","<<level_2<<" because of shape mismatch or missing to specify blank regions.. Please recheck the frame structure specified.. "<<endl;
	      abort();
	    }
	    tempOffset.symbolOffset=symOffset;
	    tempOffset.subcarrierOffset=subcOffset;
	    append(ruArrayTypeOrder(level_2),findInStringArray(resourceUnitType,(frameInfo(frameIndx).structure(level_1))(level_2)));
	    append(ruArrayOffset(level_2),tempOffset);
	    ruCountPerLevel(level_2)=ruCountPerLevel(level_2)+1;
	    ruMatrix=getResourceUnitMatrix((frameInfo(frameIndx).structure(level_1))(level_2));
	    myFrame.set_submatrix(subcOffset,symOffset,ruMatrix);
	    for(int bound_cnt=0;bound_cnt<ruSize.numberOfSymbols;bound_cnt++)
	      boundary(symOffset+bound_cnt)+=ruSize.numberOfSubcarriers;
	  }
	}
	else
	{
	  ruSize.numberOfSubcarriers=floor_i(atof((frameInfo(frameIndx).structure(level_1))(level_2).c_str()));
	  int dotLocation=(frameInfo(frameIndx).structure(level_1))(level_2).find(".");
	  ruSize.numberOfSymbols=atoi(((frameInfo(frameIndx).structure(level_1))(level_2).substr(dotLocation+1,(frameInfo(frameIndx).structure(level_1))(level_2).length()-dotLocation)).c_str());
	  myFrame.set_submatrix(subcOffset,symOffset,zeros_i(ruSize.numberOfSubcarriers,ruSize.numberOfSymbols)-2);
	  for(int bound_cnt=0;bound_cnt<ruSize.numberOfSymbols;bound_cnt++)
	    boundary(symOffset+bound_cnt)+=ruSize.numberOfSubcarriers;
	}

      }
      for(int rc_cnt=0;rc_cnt<ruCountPerLevel.length();rc_cnt++)
	if(ruCountPerLevel(rc_cnt)!=0)
	  append(frameInfo(frameIndx).ruCountPerLevel,ruCountPerLevel(rc_cnt));

	if(find(myFrame,-1)!=-1)
	  cout<<"Some portion of the frame is still unfilled... These will be left blank and not be considered in the tone types available..."<<endl;

	for(int level_2=0;level_2<frameInfo(frameIndx).structure(0).length();level_2++)
	{
	  append(frameInfo(frameIndx).ruTypeOrder,ruArrayTypeOrder(level_2));
	  append(frameInfo(frameIndx).ruOffset,ruArrayOffset(level_2));
	}
	isFrameStructParsed=true;
};

Array<Offset_S> FrameStructure::parseFrameStructure(Array< Array<string> > structure){

  int symCount=0,subcCount=0;
  Size_S frameSize;
  Array<Offset_S> ruOffset;
  Array< Array<Offset_S> > ruArrayOffset(structure(0).length());

  for(int level_2=0;level_2<structure(0).length();level_2++)
    ruArrayOffset(level_2).set_size(0);


  for(int level_1=0;level_1<structure.length();level_1++)
    if(atof((structure(level_1))(0).c_str())==0.0)
      subcCount+=getResourceUnitSize((structure(level_1))(0)).numberOfSubcarriers;
    else
      subcCount+=floor_i(atof((structure(level_1))(0).c_str()));

    for(int level_2=0;level_2<structure(0).length();level_2++)
      if(atof((structure(0))(level_2).c_str())==0.0)
	symCount+=getResourceUnitSize((structure(0))(level_2)).numberOfSymbols;
      else
      {
	int dotLocation=(structure(0))(level_2).find(".");
	symCount+=atoi(((structure(0))(level_2).substr(dotLocation+1,(structure(0))(level_2).length()-dotLocation)).c_str());
      }

      frameSize.numberOfSymbols=symCount;
    frameSize.numberOfSubcarriers=subcCount;


    int totalRUCount=0;
    ivec bounadry;
    int symOffset=0,subcOffset;
    Size_S ruSize;
    bounadry=zeros_i(symCount);
    for(int level_1=0;level_1<structure.length();level_1++)
    {
      for(int level_2=0;level_2<structure(level_1).length();level_2++)
	if(atof((structure(level_1))(level_2).c_str())==0.0)
	{
	  ruSize=getResourceUnitSize((structure(level_1))(level_2));
	  symOffset=min_index(bounadry);
	  subcOffset=min(bounadry);

	  if((symOffset+ruSize.numberOfSymbols)>frameSize.numberOfSymbols || (subcOffset+ruSize.numberOfSubcarriers)>frameSize.numberOfSubcarriers)
	  {
	    cout<<"[both:] Unable to pack the RU at "<<level_1<<","<<level_2<<" because of the frameSize determined from the first row and column..."<<endl;
	    abort();
	  }
	  else
	  {
	    if((find(bounadry(symOffset,symOffset+ruSize.numberOfSymbols)>=subcOffset)).length()>0)
	    {
	      cout<<"[both:] Unable to pack the RU at "<<level_1<<","<<level_2<<" because of shape mismatch or missing to specify blank regions.. Please recheck the frame structure specified.. "<<endl;
	      abort();
	    }
	    append(ruArrayOffset(level_2),Offset_S(symOffset,subcOffset));
	    bounadry(symOffset,symOffset+ruSize.numberOfSymbols)=bounadry(symOffset,symOffset+ruSize.numberOfSymbols)+ruSize.numberOfSubcarriers;
	    totalRUCount++;
	  }
	}
	else
	{
	  ruSize.numberOfSubcarriers=floor_i(atof((structure(level_1))(level_2).c_str()));
	  int dotLocation=(structure(0))(level_2).find(".");
	  ruSize.numberOfSymbols=atoi(((structure(0))(level_2).substr(dotLocation+1,(structure(0))(level_2).length()-dotLocation)).c_str());
	  bounadry(symOffset,symOffset+ruSize.numberOfSymbols)=bounadry(symOffset,symOffset+ruSize.numberOfSymbols)+ruSize.numberOfSubcarriers;
	}
    }

    for(int level_2=0;level_2<ruArrayOffset.length();level_2++)
      append(ruOffset,ruArrayOffset(level_2));

    return(ruOffset);

}


void FrameStructure::initFrameStructure(int idcell){

  idCell=idcell;
  if(isReady()==false)
  {
    cout<<"[both:] FrameHandler not fully configured ... "<<endl;
    abort();
  }
  for(int frm_Indx=0;frm_Indx<frameInfo.length();frm_Indx++)
    parseFrameStructure(frm_Indx);

}

void FrameStructure::defineResourceUnit(string ruType, Size_S ruSize, Array<ivec> tones){

  if(findInStringArray(resourceUnitType,ruType)!=-1)
  {
    cout<<"[both:] Error... Given 'ruType' already exists.. Try different name if u wan to add the resource unit structure..."<<endl;
    abort();
  }
  int totalTones=0;
  ivec tempRU=zeros_i(ruSize.numberOfSubcarriers*ruSize.numberOfSymbols);
  for(int tonesType_cnt=0;tonesType_cnt<tones.length();tonesType_cnt++)
  {
    if((find(tones(tonesType_cnt)>(ruSize.numberOfSubcarriers*ruSize.numberOfSymbols-1))).length()>0)
    {
      cout<<"[both:] Error .. Invalid tone locations for Tone Type "<< tonesType_cnt <<"..."<<endl;
      abort();
    }
    sort(tones(tonesType_cnt));
    tempRU(tones(tonesType_cnt))+=1;
    totalTones+=tones(tonesType_cnt).length();
  }

  if(totalTones>ruSize.numberOfSubcarriers*ruSize.numberOfSymbols)
  {
    cout<<"[both:] Error ... Invalid input to defineResourceUnit()..."<<endl;
    abort();
  }
  else if(totalTones<ruSize.numberOfSubcarriers*ruSize.numberOfSymbols)
  {
    ivec extraTones=find(tempRU,0,true);
    cout<<"There are "<<extraTones.length()<<" extra tones available in the RU "<<ruType<<" which will be considered as of type"<<tones.length()<<endl;
    tempRU(extraTones)+=1;
    append(tones,extraTones);
  }
  if(find(tempRU!=1).length()>0)
  {
    cout<<"[both:] Error...Tones overlapping in defineResourceUnit()...."<<endl;
    abort();
  }

  ResourceUnitInfo_S ruInfo;
  ruInfo.resourceUnitTones=tones;
  ruInfo.resourceUnitSize=ruSize;

  append(resourceUnitType,ruType);
  append(resourceUnitInfo,ruInfo);

}

void FrameStructure::loadResourceUnit(string ruType, string fileName,int rowUpShift, int columnRightShift, ivec columnsToSwap,ivec rowsToSwap,bool rowsOperations1st,int nRepititions){

  if(findInStringArray(resourceUnitType,ruType)!=-1)
  {
    cout<<"[both:] Resource unit with name "<<ruType<<" already available..."<<endl;
    abort();
  }
  append(resourceUnitType,ruType);
  imat ruMat(0,0);
  Array<ivec> ruNew;
  ivec toneNumbers, tmp;
  if(!parse(fileName,"rbStructure",ruMat)){

    cout<<"[both:] Unable to parse ruStructure from file : "<<fileName<<endl;abort();
  }
  ResourceUnitInfo_S ruInfo;

  if(rowsOperations1st)
  {
    if(rowUpShift)
      ruMat=rotateRow(ruMat,rowUpShift);

    if(rowsToSwap.length())
    {
      if(sum(rowsToSwap>=ruMat.rows())) {cout<<"[both:] Error... invalid row index for swapping in  loadResourceUnit()..."<<endl;abort();}
	else
	{
	  for(int cnt=0;cnt<rowsToSwap.length()/2;cnt++)
	  {
	    tmp=ruMat.get_row(rowsToSwap(2*cnt));
	    ruMat.set_row(rowsToSwap(2*cnt), ruMat.get_row(rowsToSwap(2*cnt+1)));
	    ruMat.set_row(rowsToSwap(2*cnt+1), tmp);
	  }
	}
    }

    if(columnRightShift)
      ruMat=rotateColumn(ruMat,columnRightShift);

    if(columnsToSwap.length())
    {
      if(sum(columnsToSwap>=ruMat.cols())) {cout<<"[both:] Error... invalid column index for swapping in  loadResourceUnit()..."<<endl;abort();}
	else
	{
	  for(int cnt=0;cnt<columnsToSwap.length()/2;cnt++)
	  {
	    tmp=ruMat.get_col(columnsToSwap(2*cnt));
	    ruMat.set_col(columnsToSwap(2*cnt), ruMat.get_col(columnsToSwap(2*cnt+1)));
	    ruMat.set_col(columnsToSwap(2*cnt+1), tmp);
	  }
	}
    }
  }
  else
  {
    if(columnRightShift)
      ruMat=rotateColumn(ruMat,columnRightShift);

    if(columnsToSwap.length())
    {
      if(sum(columnsToSwap>=ruMat.cols())) {cout<<"[both:] Error... invalid column index for swapping in  loadResourceUnit()..."<<endl;abort();}
      else
      {
	for(int cnt=0;cnt<columnsToSwap.length()/2;cnt++)
	{
	  tmp=ruMat.get_col(columnsToSwap(2*cnt));
	  ruMat.set_col(columnsToSwap(2*cnt), ruMat.get_col(columnsToSwap(2*cnt+1)));
	  ruMat.set_col(columnsToSwap(2*cnt+1), tmp);
	}
      }
    }

    if(rowUpShift)
      ruMat=rotateRow(ruMat,rowUpShift);
    if(rowsToSwap.length())
    {
      if(sum(rowsToSwap>=ruMat.rows())) {cout<<"[both:] Error... invalid row index for swapping in  loadResourceUnit()..."<<endl;abort();}
      else
      {
	for(int cnt=0;cnt<rowsToSwap.length()/2;cnt++)
	{
	  tmp=ruMat.get_row(rowsToSwap(2*cnt));
	  ruMat.set_row(rowsToSwap(2*cnt), ruMat.get_row(rowsToSwap(2*cnt+1)));
	  ruMat.set_row(rowsToSwap(2*cnt+1), tmp);
	}
      }
    }


  }
  imat temp_ruMat=ruMat;
  for(int rep_cnt=1;rep_cnt<nRepititions;rep_cnt++){
    appendHorizontal(ruMat,temp_ruMat);
  }
  ruInfo.resourceUnitSize.numberOfSubcarriers=ruMat.rows();
  ruInfo.resourceUnitSize.numberOfSymbols=ruMat.cols();
  ivec ruVec=cvectorize(ruMat);
  ruInfo.toneNumbers=findUniqueNumbers(ruVec);
  ruInfo.resourceUnitTones.set_size(ruInfo.toneNumbers.length());
  for(int toneNum_cnt=0;toneNum_cnt<ruInfo.toneNumbers.length();toneNum_cnt++)
  ruInfo.resourceUnitTones(toneNum_cnt)=find(ruVec,ruInfo.toneNumbers(toneNum_cnt),true);
   Array<string> toneTypes;
  parse(fileName,"toneTypes",toneTypes);
  ivec temp;
  for(int toneType_cnt=0;toneType_cnt<toneTypes.length();toneType_cnt++)
  {
    if(parse(fileName,toneTypes(toneType_cnt),temp))
    {
      append(ruInfo.toneTypes,toneTypes(toneType_cnt));
      append(ruInfo.toneTypesMap,temp);
    }
    else
    {
      cout<<"[both:] Error ... Unable to parse toneType "<<toneTypes(toneType_cnt)<<" from the file in loadResourceUnit()..."<<endl;
      abort();
    }
  }
  append(resourceUnitInfo,ruInfo);
}
void FrameStructure::modifyResourceUnit(string baseRuType, string newRuType, string unalteredToneType, int toneNumberOffset, string toneTypeSuffix, int rowUpShift, int columnRightShift, ivec columnsToSwap,ivec rowsToSwap,bool rowsOperations1st){
  
  if(findInStringArray(resourceUnitType,baseRuType)==-1)
  {
    cout<<"[both:] Unknown baseRuType "<<baseRuType<<" to modifyResourceUnit()..."<<endl;
    abort();
  }
  if(findInStringArray(resourceUnitType,newRuType)!=-1)
  {
    cout<<"[both:] Resource unit with name "<<newRuType<<" already available..."<<endl;
    abort();
  }
  append(resourceUnitType,newRuType);
  Array<ivec> ruNew;
  ivec toneNumbers, tmp;
  imat ruMat=getResourceUnitMatrix(baseRuType);
  ResourceUnitInfo_S baseRUInfo=getResourceUnitInfo(baseRuType);
  ResourceUnitInfo_S ruInfo;
  if(rowsOperations1st)
  {
    if(rowUpShift)
      ruMat=rotateRow(ruMat,rowUpShift);
    
    if(rowsToSwap.length())
    {
      if(sum(rowsToSwap>=ruMat.rows())) {cout<<"[both:] Error... invalid row index for swapping in  loadResourceUnit()..."<<endl;abort();}
      else
      {
	for(int cnt=0;cnt<rowsToSwap.length()/2;cnt++)
	{
	  tmp=ruMat.get_row(rowsToSwap(2*cnt));
	  ruMat.set_row(rowsToSwap(2*cnt), ruMat.get_row(rowsToSwap(2*cnt+1)));
	  ruMat.set_row(rowsToSwap(2*cnt+1), tmp);
	}
      }
    }
    
    if(columnRightShift)
      ruMat=rotateColumn(ruMat,columnRightShift);
    
    if(columnsToSwap.length())
    {
      if(sum(columnsToSwap>=ruMat.cols())) {cout<<"[both:] Error... invalid column index for swapping in  loadResourceUnit()..."<<endl;abort();}
      else
      {
	for(int cnt=0;cnt<columnsToSwap.length()/2;cnt++)
	{
	  tmp=ruMat.get_col(columnsToSwap(2*cnt));
	  ruMat.set_col(columnsToSwap(2*cnt), ruMat.get_col(columnsToSwap(2*cnt+1)));
	  ruMat.set_col(columnsToSwap(2*cnt+1), tmp);
	}
      }
    }
  }
  else
  {
    if(columnRightShift)
      ruMat=rotateColumn(ruMat,columnRightShift);
    
    if(columnsToSwap.length())
    {
      if(sum(columnsToSwap>=ruMat.cols())) {cout<<"[both:] Error... invalid column index for swapping in  loadResourceUnit()..."<<endl;abort();}
      else
      {
	for(int cnt=0;cnt<columnsToSwap.length()/2;cnt++)
	{
	  tmp=ruMat.get_col(columnsToSwap(2*cnt));
	  ruMat.set_col(columnsToSwap(2*cnt), ruMat.get_col(columnsToSwap(2*cnt+1)));
	  ruMat.set_col(columnsToSwap(2*cnt+1), tmp);
	}
      }
    }
    
    if(rowUpShift)
      ruMat=rotateRow(ruMat,rowUpShift);
    
    if(rowsToSwap.length())
    {
      if(sum(rowsToSwap>=ruMat.rows())) {cout<<"[both:] Error... invalid row index for swapping in  loadResourceUnit()..."<<endl;abort();}
      else
      {
	for(int cnt=0;cnt<rowsToSwap.length()/2;cnt++)
	{
	  tmp=ruMat.get_row(rowsToSwap(2*cnt));
	  ruMat.set_row(rowsToSwap(2*cnt), ruMat.get_row(rowsToSwap(2*cnt+1)));
	  ruMat.set_row(rowsToSwap(2*cnt+1), tmp);
	}
      }
    }
    
    
  }
  ruInfo.resourceUnitSize.numberOfSubcarriers=ruMat.rows();
  ruInfo.resourceUnitSize.numberOfSymbols=ruMat.cols();
  
  int unalteredTone=baseRUInfo.getToneNumbers(unalteredToneType)(0);
  ivec ruVec=cvectorize(ruMat);
  ivec tonesToAlter=findNot(ruVec,unalteredTone);
  setInVec(ruVec,tonesToAlter,ruVec(tonesToAlter)+toneNumberOffset);
  ruInfo.toneNumbers=findUniqueNumbers(ruVec);
  ruInfo.resourceUnitTones.set_size(ruInfo.toneNumbers.length());
  for(int toneNum_cnt=0;toneNum_cnt<ruInfo.toneNumbers.length();toneNum_cnt++){
    ruInfo.resourceUnitTones(toneNum_cnt)=find(ruVec,ruInfo.toneNumbers(toneNum_cnt),true);
    
  }
  
  int unalteredIndx=findInStringArray(baseRUInfo.toneTypes,unalteredToneType);
  append(ruInfo.toneTypes,baseRUInfo.toneTypes(unalteredIndx));
  append(ruInfo.toneTypesMap,baseRUInfo.toneTypesMap(unalteredIndx));
  ivec temp;
  for(int toneType_cnt=0;toneType_cnt<baseRUInfo.toneTypes.length();toneType_cnt++)
    if(toneType_cnt!=unalteredIndx)
    { append(ruInfo.toneTypes,baseRUInfo.toneTypes(toneType_cnt)+toneTypeSuffix);
      append(ruInfo.toneTypesMap,baseRUInfo.toneTypesMap(toneType_cnt)+toneNumberOffset);
     }
    append(resourceUnitInfo,ruInfo);
}


Size_S FrameStructure::getResourceUnitSize(string ruType){

  int ruIndx=findInStringArray(resourceUnitType,ruType);
  if(ruIndx==-1)
  {
    cout<<"[both:] Error ... Given ruType "<<ruType<<" not available in the resourceUnitType list..."<<endl;
    abort();
  }

  return(resourceUnitInfo(ruIndx).resourceUnitSize);
}


imat FrameStructure::getResourceUnitMatrix(string ruType){

  int ruIndx=findInStringArray(resourceUnitType,ruType);
  if(ruIndx==-1)
  {
    cout<<"[both:] Error ... Given resource Unit Type not available in the list..."<<endl;
    abort();
  }

  imat tempRU;
  tempRU.set_size(resourceUnitInfo(ruIndx).resourceUnitSize.numberOfSubcarriers,resourceUnitInfo(ruIndx).resourceUnitSize.numberOfSymbols);
  tempRU.zeros();
  for(int toneType_cnt=0;toneType_cnt<resourceUnitInfo(ruIndx).toneNumbers.length();toneType_cnt++){
    for(int tone_cnt=0;tone_cnt<resourceUnitInfo(ruIndx).resourceUnitTones(toneType_cnt).length();tone_cnt++){
      tempRU((resourceUnitInfo(ruIndx).resourceUnitTones(toneType_cnt))(tone_cnt)%resourceUnitInfo(ruIndx).resourceUnitSize.numberOfSubcarriers,
	     floor_i((resourceUnitInfo(ruIndx).resourceUnitTones(toneType_cnt))(tone_cnt)/resourceUnitInfo(ruIndx).resourceUnitSize.numberOfSubcarriers))=resourceUnitInfo(ruIndx).toneNumbers(toneType_cnt);
    }
  }
	     return(tempRU);
	     
}

string FrameStructure::getResourceUnitType(int ruIndex){


  return(resourceUnitType(frameInfo(currentFrameTypeIndx).ruTypeOrder(ruIndex)));
}


imat FrameStructure::getResourceUnitMatrix(int ruIndex){


  return(getResourceUnitMatrix(resourceUnitType(frameInfo(currentFrameTypeIndx).ruTypeOrder(ruIndex))));
}

void FrameStructure::loadFrameStructure(string frameTypeName, string filename, bool isDownlinkFrame){
  int frmIndx=findInStringArray(frameType,frameTypeName);
  if(frmIndx==-1)
  {
    frmIndx=frameType.length();
    append(frameType,frameTypeName);
    FrameInfo_S newFrameInfo;
    append(frameInfo,newFrameInfo);
  }
  isDownlink=isDownlinkFrame;
  parse(filename,"frameStructure",frameInfo(frmIndx).structure);

}



void FrameStructure::mergeResourceUnits(string baseRuType, string newRuType,string resultantRuType ,string mergerToneType, ivec blockedSymbols, Array<ivec> blockedSCPerSym){
  
  if(findInStringArray(resourceUnitType,baseRuType)==-1)
  {
    cout<<"[both:] Unknown baseRuType "<<baseRuType<<" to mergeResourceUnits()..."<<endl;
    abort();
  }
  if(findInStringArray(resourceUnitType,newRuType)==-1)
  {
    cout<<"[both:] Unknown newRuType "<<newRuType<<" to mergeResourceUnits()..."<<endl;
    abort();
  }
  if(findInStringArray(resourceUnitType,resultantRuType)!=-1)
  {
    cout<<"[both:] Resource unit with name "<<resultantRuType<<" already available in mergeResourceUnits()..."<<endl;
    abort();
  }
 
  ResourceUnitInfo_S newRUInfo,resultantRUInfo;
  newRUInfo=getResourceUnitInfo(newRuType);
  resultantRUInfo=getResourceUnitInfo(baseRuType);
  imat newRuMatrix=getResourceUnitMatrix(newRuType);
  imat resultantRuMatrix=getResourceUnitMatrix(baseRuType);
  bmat mergeFlags=zeros_b(resultantRuMatrix.rows(),resultantRuMatrix.cols());
  ivec blockedTones(0);
  if(blockedSymbols.length())
  {
    for(int sym_cnt=0;sym_cnt<blockedSymbols.length();sym_cnt++)
    {      
        //mergeFlags.set_col(blockedSymbols(sym_cnt),ones_b(resultantRuMatrix.rows()));
         if(blockedSCPerSym(sym_cnt).length()==0)  
        mergeFlags.set_col(blockedSymbols(sym_cnt),ones_b(resultantRuMatrix.rows()));
      else
      {
         bvec temp=mergeFlags.get_col(blockedSymbols(sym_cnt));
         temp.set_subvector(blockedSCPerSym(sym_cnt)(0),blockedSCPerSym(sym_cnt)(1),ones_b(blockedSCPerSym(sym_cnt)(1)-blockedSCPerSym(sym_cnt)(0)+1));
         mergeFlags.set_col(blockedSymbols(sym_cnt),temp);
      }
    }
    blockedTones=find(cvectorize(mergeFlags));
    setInMatrix(resultantRuMatrix,blockedTones,zeros_i(blockedTones.length())-2); // -2 is used to represent blocked tones
  }
  ivec mergeTones=resultantRUInfo.getRow1stToneIndices(mergerToneType);
  append(mergeTones,resultantRUInfo.getRow1stToneIndices("dontcare"));
  setInMatrix(mergeFlags,mergeTones,ones_b(mergeTones.length()));
  if(blockedTones.length())
    setInMatrix(mergeFlags,blockedTones,zeros_b(blockedTones.length()));
  
  merge(resultantRuMatrix,newRuMatrix,mergeFlags);
  resultantRUInfo.toneNumbers=findUniqueNumbers(resultantRuMatrix);
  resultantRUInfo.resourceUnitTones.set_length(resultantRUInfo.toneNumbers.length());
  for(int tone_cnt=0;tone_cnt<resultantRUInfo.toneNumbers.length();tone_cnt++)
    resultantRUInfo.resourceUnitTones(tone_cnt)=find(resultantRuMatrix,resultantRUInfo.toneNumbers(tone_cnt),true);
  for(int tone_cnt=0;tone_cnt<newRUInfo.toneTypes.length();tone_cnt++)
  {
    int indx=findInStringArray(resultantRUInfo.toneTypes,newRUInfo.toneTypes(tone_cnt));
    if(indx!=-1)
    {
      ivec toneTypesMap=concat(resultantRUInfo.toneTypesMap(indx),newRUInfo.toneTypesMap(tone_cnt));
      resultantRUInfo.toneTypesMap(indx)=findUniqueNumbers(toneTypesMap);
    }
    else
    {
      append(resultantRUInfo.toneTypes,newRUInfo.toneTypes(tone_cnt));
      append(resultantRUInfo.toneTypesMap,newRUInfo.toneTypesMap(tone_cnt));
    }
  }
  append(resourceUnitType,resultantRuType);
  append(resourceUnitInfo,resultantRUInfo);
}


void FrameStructure::mergeResourceUnits(string baseRuType, Array<string> newRuTypes,Array<ivec> colPositions, ivec rowPositions,string resultantRuType ,string mergerToneType, ivec blockedSymbols, Array<ivec> blockedSCPerSym){

  
 
  if(findInStringArray(resourceUnitType,baseRuType)==-1)
  {
    cout<<"[both:] Unknown baseRuType "<<baseRuType<<" to mergeResourceUnits()..."<<endl;
    abort();
  }
  for(int ru_cnt=0;ru_cnt<newRuTypes.length();ru_cnt++)
  {
    if(findInStringArray(resourceUnitType,newRuTypes(ru_cnt))==-1)
    {
      cout<<"[both:] Unknown newRuType "<<newRuTypes(ru_cnt)<<" to mergeResourceUnits()..."<<endl;
      abort();
    }
  }
  if(findInStringArray(resourceUnitType,resultantRuType)!=-1)
  {
    cout<<"[both:] Resource unit with name "<<resultantRuType<<" already available in mergeResourceUnits()..."<<endl;
    abort();
  }
  ResourceUnitInfo_S newRUInfo,resultantRUInfo;
  resultantRUInfo=getResourceUnitInfo(baseRuType);
  imat resultantRuMatrix=getResourceUnitMatrix(baseRuType);
  ivec mergeTones;
  imat newRuMatrix;
  imat newRuMatrix2;
  bmat mergeFlags=zeros_b(resultantRuMatrix.rows(),resultantRuMatrix.cols());
  ivec blockedTones(0);
  int NRuTypes=newRuTypes.length();
  if(blockedSymbols.length())
  { 
    for(int sym_cnt=0;sym_cnt<blockedSymbols.length();sym_cnt++)
    {
      if(blockedSCPerSym(sym_cnt).length()==0)  
        mergeFlags.set_col(blockedSymbols(sym_cnt),ones_b(resultantRuMatrix.rows()));
      else
      {
         bvec temp=mergeFlags.get_col(blockedSymbols(sym_cnt));
         temp.set_subvector(blockedSCPerSym(sym_cnt)(0),blockedSCPerSym(sym_cnt)(1),ones_b(blockedSCPerSym(sym_cnt)(1)-blockedSCPerSym(sym_cnt)(0)+1));
         mergeFlags.set_col(blockedSymbols(sym_cnt),temp);
      }
    }
    blockedTones=find(cvectorize(mergeFlags));
    setInMatrix(resultantRuMatrix,blockedTones,zeros_i(blockedTones.length())-2); // -2 is used to represent blocked tones   
    NRuTypes++;
  }
 if(NRuTypes!=0 && newRuTypes.length()==0)  //to make sure that ruInfo is properly updated incase of no newRuTypes and number of blockedsymbols are not zero
 {  
    mergeTones=resultantRUInfo.getRow1stToneIndices(mergerToneType);
    mergeFlags=zeros_b(resultantRuMatrix.rows(),resultantRuMatrix.cols());
    setInMatrix(mergeFlags,mergeTones,ones_b(mergeTones.length()));
    if(blockedTones.length())
      setInMatrix(mergeFlags,blockedTones,zeros_b(blockedTones.length()));
        merge(resultantRuMatrix,resultantRuMatrix,mergeFlags);
    resultantRUInfo.toneNumbers=findUniqueNumbers(resultantRuMatrix);
    resultantRUInfo.resourceUnitTones.set_length(resultantRUInfo.toneNumbers.length());
    for(int tone_cnt=0;tone_cnt<resultantRUInfo.toneNumbers.length();tone_cnt++)
      resultantRUInfo.resourceUnitTones(tone_cnt)=find(resultantRuMatrix,resultantRUInfo.toneNumbers(tone_cnt),true);
 }
 for(int ru_cnt=0;ru_cnt<newRuTypes.length();ru_cnt++)
 { 
   newRuMatrix=zeros_i(resultantRuMatrix.rows(),resultantRuMatrix.cols());
   if(colPositions(ru_cnt).length()) 
   {
     newRUInfo=getResourceUnitInfo(newRuTypes(ru_cnt));
     for(int loop=0;loop<colPositions(ru_cnt).length();loop++)
     { 
        newRuMatrix2=getResourceUnitMatrix(newRuTypes(ru_cnt));
        if(rowPositions(ru_cnt)>0)
        newRuMatrix2=rotateRow(newRuMatrix2,-(rowPositions(ru_cnt)%12));
       newRuMatrix.set_submatrix(0,colPositions(ru_cnt)(loop),newRuMatrix2);
     }
   }
   else
   {
      newRUInfo=getResourceUnitInfo(newRuTypes(ru_cnt));
     newRuMatrix=getResourceUnitMatrix(newRuTypes(ru_cnt));
   }
   //
   mergeTones=resultantRUInfo.getRow1stToneIndices(mergerToneType);
   append(mergeTones,resultantRUInfo.getRow1stToneIndices("dontcare"));
   mergeFlags=zeros_b(resultantRuMatrix.rows(),resultantRuMatrix.cols());
   setInMatrix(mergeFlags,mergeTones,ones_b(mergeTones.length()));
   if(blockedTones.length())
     setInMatrix(mergeFlags,blockedTones,zeros_b(blockedTones.length()));
  
     merge(resultantRuMatrix,newRuMatrix,mergeFlags);
    resultantRUInfo.toneNumbers=findUniqueNumbers(resultantRuMatrix);
   resultantRUInfo.resourceUnitTones.set_length(resultantRUInfo.toneNumbers.length());
   for(int tone_cnt=0;tone_cnt<resultantRUInfo.toneNumbers.length();tone_cnt++)
     resultantRUInfo.resourceUnitTones(tone_cnt)=find(resultantRuMatrix,resultantRUInfo.toneNumbers(tone_cnt),true);
   
   for(int tone_cnt=0;tone_cnt<newRUInfo.toneTypes.length();tone_cnt++)
   {
     int indx=findInStringArray(resultantRUInfo.toneTypes,newRUInfo.toneTypes(tone_cnt));
     if(indx!=-1)
     {
       ivec toneTypesMap=concat(resultantRUInfo.toneTypesMap(indx),newRUInfo.toneTypesMap(tone_cnt));
       resultantRUInfo.toneTypesMap(indx)=findUniqueNumbers(toneTypesMap);
     }
     else
     {
       append(resultantRUInfo.toneTypes,newRUInfo.toneTypes(tone_cnt));
       append(resultantRUInfo.toneTypesMap,newRUInfo.toneTypesMap(tone_cnt));
     }
   }
 }
 append(resourceUnitType,resultantRuType);
 append(resourceUnitInfo,resultantRUInfo);
}

imat FrameStructure::getMergedResourceUnit(string baseRuType, Array<string> newRuTypes ,string mergerToneType){

  if(findInStringArray(resourceUnitType,baseRuType)==-1)
  {
    cout<<"[both:] Unknown baseRuType "<<baseRuType<<" to mergeResourceUnits()..."<<endl;
    abort();
  }
  for(int ru_cnt=0;ru_cnt<newRuTypes.length();ru_cnt++)
  {
    if(findInStringArray(resourceUnitType,newRuTypes(ru_cnt))==-1)
    {
      cout<<"[both:] Unknown newRuType "<<newRuTypes(ru_cnt)<<" to mergeResourceUnits()..."<<endl;
      abort();
    }
  }
  ResourceUnitInfo_S newRUInfo,baseRUInfo;
  baseRUInfo=getResourceUnitInfo(baseRuType);
  imat resultantRuMatrix=getResourceUnitMatrix(baseRuType);
  ivec mergeTones;
  imat newRuMatrix;
  bmat mergeFlags;
  for(int ru_cnt=0;ru_cnt<newRuTypes.length();ru_cnt++)
  {
    newRUInfo=getResourceUnitInfo(newRuTypes(ru_cnt));
    newRuMatrix=getResourceUnitMatrix(newRuTypes(ru_cnt));
    int toneRefnumber=baseRUInfo.getToneNumbers(mergerToneType)(0);
    mergeTones=find(resultantRuMatrix,toneRefnumber,true);
    mergeFlags=zeros_b(resultantRuMatrix.rows(),resultantRuMatrix.cols());
    setInMatrix(mergeFlags,mergeTones,ones_b(mergeTones.length()));
    merge(resultantRuMatrix,newRuMatrix,mergeFlags);
  }
  
  return(resultantRuMatrix);
}

imat FrameStructure::getMergedResourceUnit(string baseRuType, Array<string> newRuTypes ,string mergerToneType, Array<ivec> colPositions,ivec rowPositions, ivec blockedSymbols, Array<ivec> blockedSCPerSym){
  if(findInStringArray(resourceUnitType,baseRuType)==-1)
  {
    cout<<"[both:] Unknown baseRuType "<<baseRuType<<" to mergeResourceUnits()..."<<endl;
    abort();
  }
  for(int ru_cnt=0;ru_cnt<newRuTypes.length();ru_cnt++)
  {
    if(findInStringArray(resourceUnitType,newRuTypes(ru_cnt))==-1)
    {
      cout<<"[both:] Unknown newRuType "<<newRuTypes(ru_cnt)<<" to mergeResourceUnits()..."<<endl;
      abort();
    }
  }

  ResourceUnitInfo_S newRUInfo,baseRUInfo;
  baseRUInfo=getResourceUnitInfo(baseRuType);
  imat resultantRuMatrix=getResourceUnitMatrix(baseRuType);
  ivec mergeTones;
  imat newRuMatrix;
  imat newRuMatrix2;
  bmat mergeFlags;
  int loop; 
  newRuMatrix=zeros_i(resultantRuMatrix.rows(),resultantRuMatrix.cols());
  for(int ru_cnt=0;ru_cnt<newRuTypes.length();ru_cnt++)
  {
    if(colPositions(ru_cnt).length())
    {	
      newRUInfo=getResourceUnitInfo(newRuTypes(ru_cnt));
      for(loop=0;loop<colPositions(ru_cnt).length();loop++)
      {  newRuMatrix2=getResourceUnitMatrix(newRuTypes(ru_cnt));
	if(rowPositions(ru_cnt)>0)
	{
	  newRuMatrix2=rotateRow(newRuMatrix2,-(rowPositions(ru_cnt)%12));
	 }
	  newRuMatrix.set_submatrix(0,colPositions(ru_cnt)(loop),newRuMatrix2);
      }
  
    }
    else
    { 
      newRUInfo=getResourceUnitInfo(newRuTypes(ru_cnt));
      newRuMatrix=getResourceUnitMatrix(newRuTypes(ru_cnt));
    }
    int toneRefnumber=baseRUInfo.getToneNumbers(mergerToneType)(0);
    mergeTones=find(resultantRuMatrix,toneRefnumber,true);
    mergeFlags=zeros_b(resultantRuMatrix.rows(),resultantRuMatrix.cols());
    setInMatrix(mergeFlags,mergeTones,ones_b(mergeTones.length()));
    merge(resultantRuMatrix,newRuMatrix,mergeFlags);
  }
  
  if(blockedSymbols.length())
  { 
    mergeFlags=zeros_b(resultantRuMatrix.rows(),resultantRuMatrix.cols());
    for(int sym_cnt=0;sym_cnt<blockedSymbols.length();sym_cnt++)
    {
      if(blockedSCPerSym(sym_cnt).length()==0)  
	mergeFlags.set_col(blockedSymbols(sym_cnt),ones_b(resultantRuMatrix.rows()));
      else
      {
	bvec temp=mergeFlags.get_col(blockedSymbols(sym_cnt));
	temp.set_subvector(blockedSCPerSym(sym_cnt)(0),blockedSCPerSym(sym_cnt)(1),ones_b(blockedSCPerSym(sym_cnt)(1)-blockedSCPerSym(sym_cnt)(0)+1));
	mergeFlags.set_col(blockedSymbols(sym_cnt),temp);
      }
    }
    ivec blockedTones=find(cvectorize(mergeFlags));
    setInMatrix(resultantRuMatrix,blockedTones,zeros_i(blockedTones.length())-2); // -2 is used to represent blocked tones   
  }
  return(resultantRuMatrix);
}


ResourceUnitInfo_S FrameStructure::getResourceUnitInfo(string ruType){
  int ruIndx=findInStringArray(resourceUnitType,ruType);
  if(ruIndx==-1)
  {
    cout<<"[both:] Error ... Given ruType "<<ruType<<" not available in the resourceUnitType list..."<<endl;
    abort();
  }
  
//   for(int i=0; i<resourceUnitInfo(ruIndx).resourceUnitTones.length(); i++)
//   {cout<<"resource unit tones"<<resourceUnitInfo(ruIndx).resourceUnitTones(i);
//   }
    return(resourceUnitInfo(ruIndx));
}

ResourceUnitInfo_S FrameStructure::getResourceUnitInfo(int ruIndex){

  return(getResourceUnitInfo(resourceUnitType(frameInfo(currentFrameTypeIndx).ruTypeOrder(ruIndex))));
}

Array<ivec> FrameStructure::getResourceUnit(string ruType){

  int ruIndx=findInStringArray(resourceUnitType,ruType);
  if(ruIndx==-1)
  {
    cout<<"[both:] Error ... Given ruType "<<ruType<<" not available in the resourceUnitType list..."<<endl;
    abort();
  }
  return(resourceUnitInfo(ruIndx).resourceUnitTones);
  }


int FrameStructure::getAllRuCount(string frameTypeName){

  if(frameTypeName=="")
  {
    return(sum(frameInfo(currentFrameTypeIndx).ruCountPerLevel));
  }
  else
  {
    int frmIndx=findInStringArray(frameType,frameTypeName);
    if(frmIndx==-1)
    {
      cout<<"[both:] Error .. Undefined frameType to getAllRuCount()..."<<endl;
      abort();
    }
    return(sum(frameInfo(frmIndx).ruCountPerLevel));
  }
}

int FrameStructure::getRuCountPerLevel(string frameTypeName){
  
  if(frameTypeName=="")
  {
    return(frameInfo(currentFrameTypeIndx).ruCountPerLevel(0));
  }
  else
  {
    int frmIndx=findInStringArray(frameType,frameTypeName);
    if(frmIndx==-1)
    {
      cout<<"[both:] Error .. Undefined frameType to getRuCountPerLevel()..."<<endl;
      abort();
    }
    return(frameInfo(frmIndx).ruCountPerLevel(0));
  }
}


int FrameStructure::getRuLevels(string frameTypeName){
  
  if(frameTypeName=="")
  {
    return(frameInfo(currentFrameTypeIndx).ruCountPerLevel.length());
  }
  else
  {
    int frmIndx=findInStringArray(frameType,frameTypeName);
    if(frmIndx==-1)
    {
      cout<<"[both:] Error .. Undefined frameType to getRuLevels()..."<<endl;
      abort();
    }
    return(frameInfo(frmIndx).ruCountPerLevel.length());
  }
}

void FrameStructure::setCurrentFrameType(string frameTypeName){

  int frmIndx=findInStringArray(frameType,frameTypeName);
  if(frmIndx==-1)
  {
    cout<<"[both:] Error .. Undefined frameType to setCurrentFrameType()..."<<endl;
    abort();
  }

  if(currentFrameTypeIndx!=frmIndx)
    currentFrameTypeIndx=frmIndx;
}

void FrameStructure::updateRUType(ivec ruIndices, string ruType){

  if(isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling updateRUType()..."<<endl;
    abort();
  }
  int ruTypeNumber=findInStringArray(resourceUnitType,ruType);
  if(ruTypeNumber==-1)
  {
    cout<<"[both:] Error... Given ruType "<<ruType<<" does not exist in updateRUType()..."<<endl;
    abort();
  }
  for(int ru_cnt=0;ru_cnt<ruIndices.length();ru_cnt++)
    updateRUType(ruIndices(ru_cnt),ruTypeNumber);
}

void FrameStructure::updateRUType(int ruIndex, string ruType){

  if(isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling updateRUType()..."<<endl;
    abort();
  }
  int ruTypeNumber=findInStringArray(resourceUnitType,ruType); 
  if(ruTypeNumber==-1)
  {
    cout<<"[both:] Error... Given 'ruType' does not exist in updateRUType()..."<<endl;
    abort();
  }
  updateRUType(ruIndex,ruTypeNumber);
}

void FrameStructure::updateRUType(int ruIndex, int ruTypeNumber){

  if(isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling updateRUType()..."<<endl;
    abort();
  }
  
  frameInfo(currentFrameTypeIndx).updateRUType(ruIndex,ruTypeNumber);
}

int FrameStructure::getToneCountPerRU(int resourceUnitIndx, string toneType){

  if(isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getToneCountPerRU()..."<<endl;
    abort();
  }
  
  ivec myTonesIndx=resourceUnitInfo(frameInfo(currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).getRow1stToneIndices(toneType);
  if(myTonesIndx.length()!=0)
    return(myTonesIndx.length());
  else
  {
    cout<<"[both:] Error... undefined Tone Type to getTones()..."<<endl;
    abort();
  }
}

int FrameStructure::getToneCountPerRU(int resourceUnitIndx, string toneType, ivec symbols){

  if(isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getToneCountPerRU()..."<<endl;
    abort();
  }
  
  ivec myTonesIndx=resourceUnitInfo(frameInfo(currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).getRow1stToneIndices(toneType,symbols);
  if(myTonesIndx.length()!=0)
    return(myTonesIndx.length());
  else
  {
    cout<<"symbols : "<<symbols<<endl;
    cout<<"[both:] Error... undefined Tone Type to getTones()..."<<endl;
    abort();
  }
}

ivec FrameStructure::getTones(int resourceUnitIndx, string toneType){
  
  if(isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getTones()..."<<endl;
    abort();
  }
  ivec myTonesIndx=resourceUnitInfo(frameInfo(currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).getRow1stToneIndices(toneType);
  if(myTonesIndx.length()!=0)
    return(myTonesIndx);
  else
  {
    cout<<"[both:] Error... undefined Tone Type to getTones()..."<<endl;
    abort();
  }
}

ivec FrameStructure::getTones(int resourceUnitIndx, string toneType, ivec symbols){
  
  if(isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getTones()..."<<endl;
    abort();
  }
  
  ivec myTonesIndx=resourceUnitInfo(frameInfo(currentFrameTypeIndx).ruTypeOrder(resourceUnitIndx)).getRow1stToneIndices(toneType,symbols);
  if(myTonesIndx.length()!=0)
    return(myTonesIndx);
  else
  {
    cout<<"[both:] Error... undefined Tone Type to getTones()..."<<endl;
    abort();
  }
}

Offset_S FrameStructure::getRUOffset(int resourceUnitIndx){
  
  if(isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getRUOffset()..."<<endl;
    abort();
  }
  return(frameInfo(currentFrameTypeIndx).ruOffset(resourceUnitIndx));
}

Size_S FrameStructure::getFrameSize(string frameTypeName){

  if(isFrameStructureParsed()==false)
  {
    cout<<"[both:] Please initialize the frameStructure before calling getFrameSize()..."<<endl;
    abort();
  }
  if(frameTypeName=="")
  {
    return(frameInfo(currentFrameTypeIndx).frameSize);
  }
  else
  {
    int frmIndx=findInStringArray(frameType,frameTypeName);
    if(frmIndx==-1)
    {
      cout<<"[both:] Error .. Undefined frameType to getFrameSize()..."<<endl;
      abort();
    }
    return(frameInfo(frmIndx).frameSize);
  }
}

void FrameStructure::updateCurrentFrameStructure(){

  currentFrameStructure.set_size(0,0);
  for(int ru_cnt=0;ru_cnt<frameInfo(currentFrameTypeIndx).ruTypeOrder.length();ru_cnt++)
    appendVertical(currentFrameStructure,getResourceUnitMatrix(resourceUnitType(frameInfo(currentFrameTypeIndx).ruTypeOrder(ru_cnt))));

}

imat* FrameStructure::getCurrentFrameStructurePointer(){


  return(&currentFrameStructure);
}
