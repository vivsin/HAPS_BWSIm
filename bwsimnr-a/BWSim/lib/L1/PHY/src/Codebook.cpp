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

#include "../include/Codebook.h"
#include "../include/CSIEstimator.h"

string NRCodebookType_Str[]={
    "_NR_CODEBOOK_TYPE_I_",
    "_NR_CODEBOOK_TYPE_I_MULTIPANEL_",
    "_NR_CODEBOOK_TYPE_II_",
    "_NR_CODEBOOK_TYPE_II_ENHANCED_",
    "_NR_CODEBOOK_TYPE_II_ENHANCED_CJT_",
    "_NR_CODEBOOK_PORT_SELECTION_",
    "_NR_CODEBOOK_PORT_SELECTION_ENHANCED_",
    "_NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_"
};
string NRCodebookMode_Str[]={
    "_NR_CODEBOOK_MODE_1_",
    "_NR_CODEBOOK_MODE_2_"
};

//Generates a vector of integers as i11,i12,i13,i14,i2 sequence
ivec getVectorFromPMIIndices(type1PMI_S precIndex,NRCodeBookType CBtype)
{
    ivec N(4);
    if(CBtype == _NR_CODEBOOK_TYPE_I_MULTIPANEL_){N.set_length(5);}
    N(0) = precIndex.i11;
    N(1) = precIndex.i12;
    N(2) = precIndex.i13;
    if(CBtype == _NR_CODEBOOK_TYPE_I_MULTIPANEL_)
    {
        N(3) = precIndex.i14;N(4) = precIndex.i2;
    }
    else
    {
        N(3) = precIndex.i2;
    }
    return N;
}
//conversion of vector PMI indices
type1PMI_S getPMIIndicesFromVector(ivec N)
{
    type1PMI_S precIndex;
    int L = N.length();
    if(!(L==5 || L==4)){cout<<"Invalid Length..."<<endl;abort();}
    precIndex.i11 = N(0);
    precIndex.i12 = N(1);
    precIndex.i13 = N(2);
    precIndex.i14 = (L==5) ? N(3) : -1;
    precIndex.i2 = N(L-1);
    return precIndex;
}

type1PMI_S getPMI(i1index_S i1,int i2=0)
{
    return type1PMI_S(i1.i11,i1.i12,i1.i13,i1.i14,i2);
}

void CodeBookConfig_S:: parseCodeBookConfig(string fileName1, string fileName2)
{
    isNRCodeBook = true;
    string codeBookmode,codebookType;
    
    if(parse(fileName1,"NRCodebookType",codebookType))
    {
        if(codebookType == "_NR_CODEBOOK_TYPE_I_")
            codeBookType = _NR_CODEBOOK_TYPE_I_;
        else if(codebookType == "_NR_CODEBOOK_TYPE_II_")
            codeBookType =  _NR_CODEBOOK_TYPE_II_;
        else if(codebookType == "_NR_CODEBOOK_TYPE_II_ENHANCED_")
            codeBookType =  _NR_CODEBOOK_TYPE_II_ENHANCED_;
        else if(codebookType == "_NR_CODEBOOK_TYPE_II_ENHANCED_CJT_")
            codeBookType =  _NR_CODEBOOK_TYPE_II_ENHANCED_CJT_;
        else if(codebookType == "_NR_CODEBOOK_PORT_SELECTION_")
            codeBookType =  _NR_CODEBOOK_PORT_SELECTION_;
        else if(codebookType == "_NR_CODEBOOK_PORT_SELECTION_ENHANCED_")
            codeBookType =  _NR_CODEBOOK_PORT_SELECTION_ENHANCED_;
        else if(codebookType == "_NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_")
            codeBookType =  _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_;
        else if(codebookType == "_NR_CODEBOOK_TYPE_I_MULTIPANEL_")
            codeBookType = _NR_CODEBOOK_TYPE_I_MULTIPANEL_;
        else
        {
            cout<<"Invalid Codebook Type"<<endl;abort();
            codeBookType = _NR_CODEBOOK_TYPE_UNIT_;
        }
    }
    else
      codeBookType = _NR_CODEBOOK_TYPE_UNIT_;
    
    if(codeBookType == _NR_CODEBOOK_TYPE_I_ || codeBookType == _NR_CODEBOOK_TYPE_I_MULTIPANEL_)
    {
        if(parse(fileName1,"NRCodebookMode",codeBookmode))
        {
            if(codeBookmode == "_NR_CODEBOOK_MODE_1_")
                codeBookMode = _NR_CODEBOOK_MODE_1_;
            else if(codeBookmode == "_NR_CODEBOOK_MODE_2_")
                codeBookMode = _NR_CODEBOOK_MODE_2_;
            else
                codeBookMode = _NR_CODEBOOK_MODE_UNIT_;
        }
        else
        codeBookMode = _NR_CODEBOOK_MODE_UNIT_;
    }
    else 
        codeBookMode = _NR_CODEBOOK_MODE_UNIT_;
    
    Array<ivec> AntStructure, TXRUMap;
    ivec NgN1N2P(4);

    if(parse(fileName2,"AntStructure",AntStructure) && (AntStructure.length() == 4) && parse(fileName2,"TXRUMap",TXRUMap) && (TXRUMap.length() == 4))
    {
        NgN1N2P(0) = AntStructure(0)(0)*AntStructure(0)(1);
        NgN1N2P(3) = AntStructure(0)(0)*AntStructure(0)(4);
        NgN1N2P(1) = TXRUMap(0)(0);
        NgN1N2P(2) = TXRUMap(0)(1);
        Ng=NgN1N2P(0),N1=NgN1N2P(1),N2=NgN1N2P(2),P=NgN1N2P(3),nCSIRSPorts = prod(NgN1N2P);

        if(codeBookType == _NR_CODEBOOK_TYPE_I_MULTIPANEL_)
        {
            if(Ng!=2 && Ng!=4)
            {
                cout<<"[both:] Ng can be {2,4} for Type-I Multipanel Codebook"<<endl;abort();
            }
            else if(Ng!=2 && codeBookmode == "_NR_CODEBOOK_MODE_2_")
            {
                cout<<"[both:] Ng cannot be "<<NgN1N2P(0)<<" for Codebook mode 2"<<endl;abort();
            }
            if(N2>2)
            {
                cout<<"[both:] N2 cannot be greater than 2"<<endl;abort();
            }
        }
        else if(Ng>1)
        {
            cout<<"Ng cannot be greater than 1 for Single Panel Codebook..."<<endl;abort();
        }
    }
    else
    {
        cout<<"[both:]Configuration Error in ActiveAntennaSystemConfig.txt"<<endl;
      abort();
    }

    if(Ng*N1*N2==1 && codeBookType!=_NR_CODEBOOK_TYPE_I_)
    {
        cout<<codeBookType<<" doesnot support single port CSIRS...setting _NR_CODEBOOK_TYPE_I_ by default.."<<endl;
        codeBookType = _NR_CODEBOOK_TYPE_I_;
    }
    
    if(codeBookType !=_NR_CODEBOOK_TYPE_I_ && codeBookType!=_NR_CODEBOOK_TYPE_I_MULTIPANEL_)
    {
        parse(fileName1,"Lbeams",LBeams);
        parse(fileName1,"nPSK",nPSK);
        if((codeBookType ==_NR_CODEBOOK_TYPE_II_) && ((nPSK != 4) && (nPSK != 8))){
            cout<<endl<<"ABORT::Invalid configuration of nPSK for Rel-15 Type-II Codebook. Possible values of nPSK are 4, 8"<<endl;abort();
        }
        if((codeBookType ==_NR_CODEBOOK_TYPE_II_ENHANCED_)&&(nPSK != 16)){
            cout<<endl<<"ABORT::Invalid configuration of nPSK for Rel-16 eType-II Codebook. Possible value of nPSK is 16"<<endl;abort();
        }
        parse(fileName1,"nWideAmp",nWideAmp);
        if((codeBookType ==_NR_CODEBOOK_TYPE_II_)&&(nWideAmp != 8)){
            cout<<endl<<"ABORT::Invalid configuration of nWideAmp for Rel-15 Type-II Codebook. Possible value of nWideAmp is 8"<<endl;abort();
        }
        if((codeBookType ==_NR_CODEBOOK_TYPE_II_ENHANCED_)&&(nWideAmp != 16)){
            cout<<endl<<"ABORT::Invalid configuration of nWideAmp for Rel-16 eType-II Codebook. Possible value of nWideAmp is 16"<<endl;abort();
        }
        parse(fileName1,"nSubAmp",nSubAmp);
        if((codeBookType ==_NR_CODEBOOK_TYPE_II_)&&(nSubAmp != 2)){
            cout<<endl<<"ABORT::Invalid configuration of nSubAmp for Rel-15 Type-II Codebook. Possible value of nSubAmp is 2"<<endl;abort();
        }
        if((codeBookType ==_NR_CODEBOOK_TYPE_II_ENHANCED_)&&(nSubAmp != 8)){
            cout<<endl<<"ABORT::Invalid configuration of nSubAmp for Rel-16 eType-II Codebook. Possible value of nSubAmp is 8"<<endl;abort();
        }
        parse(fileName1,"isType2IdealCoeff",isType2IdealCoeff);
        parse(fileName1,"isType2IdealPrecoder",isType2IdealPrecoder);
        
        switch(codeBookType)
        {
            case _NR_CODEBOOK_PORT_SELECTION_:
            case _NR_CODEBOOK_PORT_SELECTION_ENHANCED_:
                parse(fileName1,"portSelectionSamplingSize",d);
                if(d>min(nCSIRSPorts/2,LBeams))
                {
                    cout<<"portSelectionSamplingSize must be less than or equal to min(nPorts/2, L)"<<endl;
                    abort();
                }
                break;
            case _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_:
                {
                    double alpha;
                    parse(fileName1,"alpha",alpha);
                    LBeams = alpha*nCSIRSPorts/2;
                    M = 2;
                }
                break;
            case _NR_CODEBOOK_TYPE_II_ENHANCED_CJT_:
                parse(fileName1,"NPhase",N_Phase);
                break;
            default:
                break;
        }
        LBeams = min(LBeams,nCSIRSPorts/2);
    }
}


Codebook::Codebook()
{
  isAltCodeBookEnabledFor4TXEnabled = false;
  type1halfBeamswithi1.set_size(0);
}

void Codebook::initializeDlCodebook()
{ 
  dlCodeBookConfig.nCSIRSPorts = dlCodeBookConfig.P*dlCodeBookConfig.N1*dlCodeBookConfig.N2*dlCodeBookConfig.Ng;
  
  if(dlCodeBookConfig.N1 == -1 || dlCodeBookConfig.N2==-1 || dlCodeBookConfig.P==-1 || dlCodeBookConfig.Ng==-1)
  {
    cout<<"[both:] Ng, N1, N2 and P should be configured"<<endl;
    abort();
  }
  
  dlCodeBookConfig.O1 = 4;
  dlCodeBookConfig.O2 = (dlCodeBookConfig.N2 == 1) ? 1 : 4;
  
  //****************************NR codebook related***********************************//
  if(dlCodeBookConfig.isNRCodeBook) 
  {
    /************ Downlink Codebooks ***********/
    if(dlCodeBookConfig.nCSIRSPorts==1)
        return;
    
    // For two ports
    if(dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_I_)
    {
        switch(dlCodeBookConfig.nCSIRSPorts)
        {
            case 2:
                TwoPortCodebookForAllLayers.set_size(2);
                for(int numLayers =0; numLayers<2;numLayers++)
                    TwoPortCodebookForAllLayers(numLayers)=genTwoPortCodebook_DL_NR(numLayers+1);
                break;
            case 4:
                FourPortCodebookForAllLayers = getCodeBookForPorts(dlCodeBookConfig.nCSIRSPorts);
                break;
            case 8:
                EightPortCodebookForAllLayers = getCodeBookForPorts(dlCodeBookConfig.nCSIRSPorts);
                break;
            case 12:
                TwelvePortCodebookForAllLayers = getCodeBookForPorts(dlCodeBookConfig.nCSIRSPorts);
                break;
            case 16:
                SixteenPortCodebookForAllLayers = getCodeBookForPorts(dlCodeBookConfig.nCSIRSPorts);
                break;
            case 24:
                TwentyfourPortCodebookForAllLayers = getCodeBookForPorts(dlCodeBookConfig.nCSIRSPorts);
                break;
            case 32:
                ThirtytwoPortCodebookForAllLayers = getCodeBookForPorts(dlCodeBookConfig.nCSIRSPorts);
                break;
            case 48:
            case 64:
            case 96:
            case 128:
                break;
            default:
                cout<<"Unsupported nCSIRSPorts = "<<dlCodeBookConfig.nCSIRSPorts<<" aborting in initializeDlCodebook()"<<endl;
                abort();
                break;
        }
    }
    else if(dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_I_MULTIPANEL_)
    {
        switch(dlCodeBookConfig.nCSIRSPorts)
        {
            case 8:
                EightPortCodebookForAllLayers = getCodeBookForPorts(dlCodeBookConfig.nCSIRSPorts);
                break;
            case 16:
                SixteenPortCodebookForAllLayers = getCodeBookForPorts(dlCodeBookConfig.nCSIRSPorts);
                break;
            case 32:
                ThirtytwoPortCodebookForAllLayers = getCodeBookForPorts(dlCodeBookConfig.nCSIRSPorts);
                break;
            default:
                cout<<"Unsupported nCSIRSPorts = "<<dlCodeBookConfig.nCSIRSPorts<<" aborting in initializeDlCodebook()"<<endl;
                abort();
                break;
        } 
    }
  /************ Downlink Codebooks End***********/
      
  //****************************NR codebook related End ***********************************//
  }
  else
  { // LTE code book
    /// For two ports
    cout<<"[detl LTE Code book is selected ..."<<endl;
    TwoPortCodebookForAllLayers.set_size(2);
    for(int numLayers =0; numLayers<2;numLayers++)
    {
      TwoPortCodebookForAllLayers(numLayers)=generateCodebookForDLTwoPorts(numLayers+1);
    }
    
    if(!getAltCodeBookFlagFor4TX())
    {
      FourPortCodebookForAllLayers.set_size(4);
      for(int numLayers =0; numLayers<4;numLayers++)
      {
        FourPortCodebookForAllLayers(numLayers)=generateCodebookForDLFourPorts(numLayers+1);
      }
    }
    else
    {
      /// For Four ports Alternate code book Rel-12
      FourPortCodebookForAllLayers.set_size(4);
      for(int numLayers =0; numLayers<4;numLayers++)
      {
	FourPortCodebookForAllLayers(numLayers)=generateAlternativeCodebookForDLFourPorts(numLayers+1);
      }
    }
    /// For Eight ports
    
    CodeBookIndicesToSelecti1ForSubmode1.set_size(8);
    CodeBookIndicesForSubMode2.set_size(8);
    CodeBookIndicesToSelecti1.set_size(8);
    
    EightPortCodebookForAllLayers.set_size(8);
    
    SubSampledCodeBookForSubmode1.set_size(8);
    SubSampledCodeBookForsubmode2.set_size(8);
    CodeBookToSelecti1.set_size(8);
    
    for(int numLayers =0; numLayers<8;numLayers++)
    {
      EightPortCodebookForAllLayers(numLayers)=generateCodebookForDLEightPorts(numLayers+1);
      
      CodeBookIndicesToSelecti1ForSubmode1(numLayers)=getCodeBookIndicesToSelecti1ForSubmode1(numLayers+1);
      CodeBookIndicesForSubMode2(numLayers)=getCodeBookIndicesForSubMode2(numLayers+1);
      CodeBookIndicesToSelecti1(numLayers)=getCodeBookIndicesToSelecti1(numLayers+1);
      
      for(int cnt=0;cnt<CodeBookIndicesToSelecti1ForSubmode1(numLayers).length();cnt++)
	append(SubSampledCodeBookForSubmode1(numLayers),EightPortCodebookForAllLayers(numLayers)(CodeBookIndicesToSelecti1ForSubmode1(numLayers)(cnt)));

      for(int cnt=0;cnt<CodeBookIndicesForSubMode2(numLayers).length();cnt++)
	append(SubSampledCodeBookForsubmode2(numLayers),EightPortCodebookForAllLayers(numLayers)(CodeBookIndicesForSubMode2(numLayers)(cnt)));
      
      for(int cnt=0;cnt<CodeBookIndicesToSelecti1(numLayers).length();cnt++)
	append(CodeBookToSelecti1(numLayers),EightPortCodebookForAllLayers(numLayers)(CodeBookIndicesToSelecti1(numLayers)(cnt)));
    }
    
    getUnique8portCodebookIndices();
  }
}

void Codebook::initializeUlCodebook(bool isTransformPrecodingEnabled)
{ 
  
  //****************************NR codebook related***********************************//
  if(dlCodeBookConfig.isNRCodeBook) 
  {
       
    /************ Uplink Codebooks ***********/
    
    //Codebook type                                   Corresponding W_UL index
    // Single Layer - 2 ports                                             0
    // Single Layer - 4 ports - Transform Precoding Enabled               1
    // Single Layer - 4 ports - Transform Precoding Disabled              2
    // Two Layer - 2 ports - Transform Precoding Disabled                 3
    // Two Layer - 4 ports - Transform Precoding Disabled                 4
    // Three Layer - 4 ports - Transform Precoding Disabled               5
    // Four Layer - 4 ports - Transform Precoding Disabled                6 
    
    
    /// For two ports
    Array<Array<cmat>> W_UL;
    W_UL = genULCodeBookNR();
    TwoPortCodebookForULAllLayers.set_size(2);
    TwoPortCodebookForULAllLayers(0)=W_UL(0);  // Single Layer Two ports
    TwoPortCodebookForULAllLayers(1)=W_UL(3);  // Two Layer  Two ports
    
    /// For Four ports
    FourPortCodebookForULAllLayers.set_size(4);
    if(isTransformPrecodingEnabled)
      FourPortCodebookForULAllLayers(0) = W_UL(1);
    else
      FourPortCodebookForULAllLayers(0) = W_UL(2);
    
    FourPortCodebookForULAllLayers(1) = W_UL(4);
    FourPortCodebookForULAllLayers(2) = W_UL(5);
    FourPortCodebookForULAllLayers(3) = W_UL(6);
    
    
    /************ Uplink Codebooks End ***********/
    
    
    //****************************NR codebook related End ***********************************//
  }
  else
  { // LTE code book
    /// For two ports
    TwoPortCodebookForULAllLayers.set_size(2);
    for(int numLayers =0; numLayers<2;numLayers++)
    {
      TwoPortCodebookForULAllLayers(numLayers)=generateCodebookForULTwoPorts(numLayers+1);
    }
    /// For Four ports
    FourPortCodebookForULAllLayers.set_size(4);
    for(int numLayers =0; numLayers<4;numLayers++)
    {
      FourPortCodebookForULAllLayers(numLayers)=generateCodebookForULFourPorts(numLayers+1);
    }
    
  }
}

CodeBookConfig_S Codebook::getCodeBookConfig(){
  
  return dlCodeBookConfig;
}

//****************************NR codebook related***********************************//
Array<Array<cmat>> Codebook::getCodeBookForPorts(int NrofPorts)
{
    if (dlCodeBookConfig.P*dlCodeBookConfig.N1*dlCodeBookConfig.N2*dlCodeBookConfig.Ng != NrofPorts)
    {
        cout << "Number of Ports and (N1, N2) mismatch... Nr of ports = " << NrofPorts << "  2*Ng*N1*N2 = " << 2*dlCodeBookConfig.N1*dlCodeBookConfig.N2*dlCodeBookConfig.Ng <<  endl<< "..... aborting..." <<endl;
        abort();
    }
    if(!(dlCodeBookConfig.codeBookMode == _NR_CODEBOOK_MODE_1_ || dlCodeBookConfig.codeBookMode == _NR_CODEBOOK_MODE_2_))
    {
        cout << "Invalid codebook mode chosen... aborting" << endl;
        abort();
    }
    if(dlCodeBookConfig.codeBookType!=_NR_CODEBOOK_TYPE_I_MULTIPANEL_ && dlCodeBookConfig.Ng>1)
    {
        cout<<"Ng cannot be greater than 1 for single panel..."<<endl;abort();
    }
    
    return (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_I_MULTIPANEL_) ? genType1MultipanelCodebookNR(dlCodeBookConfig.codeBookMode) : genType1CodebookNR(dlCodeBookConfig.codeBookMode);
}

//Finding the codebook index for the given PMI indices
int Codebook::getPrecoderIndxAbsNR(int nLayers, type1PMI_S i1i2index)
{
    int layerNum =-1 , i2=0, h=0, v=0, g=0, indx,p=0;
    
    int nPorts = dlCodeBookConfig.nCSIRSPorts;
    int maxLayers = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_I_MULTIPANEL_) ? 4:8;
    CBParam_S CBparam;
    
    if(nLayers > maxLayers || nLayers<1)
    {
        cout << "nLayers must be in [1,"<<maxLayers<<"]... aborting ..."<< endl; abort();
    }
    
    if(dlCodeBookConfig.codeBookType != _NR_CODEBOOK_TYPE_I_MULTIPANEL_)
    {
        if(nPorts>2)
        {
            CBparam = getParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, i2, h, v, g);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
        }
        else
        {
            return i1i2index.i2; // Assuming for port 2 i2 will be set
        }
    }
    else
    {
        CBparam = getMultipanelParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, dlCodeBookConfig.Ng, h, v, g,p,i2);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
    }
    
    ivec pmiIndices = getVectorFromPMIIndices(i1i2index,dlCodeBookConfig.codeBookType);
    indx = decode(CBparam.indicesSizes,pmiIndices);
    if(pmiIndices != encode(CBparam.indicesSizes,indx))
    {
        cout<<"Decoding Mismatch..."<<endl;abort();
    }
    
    if(indx>=CBparam.CBSizePerLayer){cout<<"Index cannot be greater than CBSize"<<endl;abort();}
    return indx;
}

//Finding the PMI indices for given codebook index
type1PMI_S Codebook::getPMIIndices(int codeBookMode, int N1, int N2, int nLayers, int indx)
{
    type1PMI_S PMIindices;
    int layerNum =-1 , i2=0, h=0, v=0, g=0,p=0;
    int maxLayers = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_I_MULTIPANEL_) ? 4:8;
    
    if(nLayers > maxLayers|| nLayers<1)
    {
        cout << "nLayers must be in [1,"<<maxLayers<<"]... aborting ..."<< endl; abort();
    }
    
    CBParam_S CBparam;
    if(dlCodeBookConfig.codeBookType != _NR_CODEBOOK_TYPE_I_MULTIPANEL_)
    {
        CBparam = getParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, N1, N2, i2, h, v, g);
    }
    else
    {
        CBparam = getMultipanelParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, dlCodeBookConfig.Ng, h, v, g,p,i2);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
    }
    
    PMIindices = getPMIIndicesFromVector(encode(CBparam.indicesSizes,indx));
    return PMIindices;
}

//Finding the PMI indices for given codebook index
type1PMI_S Codebook::getPMIIndices(int nLayers, int indx) 
{  
    type1PMI_S PMIindices;
    int layerNum =-1 , i2=0, h=0, v=0, g=0,p=0;
    int maxLayers = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_I_MULTIPANEL_) ? 4:8;
    
    if(nLayers > maxLayers || nLayers<1)
    {
        cout << "nLayers must be in [1,"<<maxLayers<<"]... aborting ..."<< endl; abort();
    }
    
    CBParam_S CBparam;
    if(dlCodeBookConfig.codeBookType != _NR_CODEBOOK_TYPE_I_MULTIPANEL_)
    {
        CBparam = getParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, i2, h, v, g);
    }
    else
    {
        CBparam = getMultipanelParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, dlCodeBookConfig.Ng, h, v, g,p,i2);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
    }
    PMIindices = getPMIIndicesFromVector(encode(CBparam.indicesSizes,indx));
    if(indx!=decode(CBparam.indicesSizes,getVectorFromPMIIndices(PMIindices,dlCodeBookConfig.codeBookType)))
    {
        cout<<"Encoding Mismatch.."<<endl;abort();
    }
    return PMIindices;
}

// Fetch the precoder matrix for given PMI Indices
cmat Codebook::getPrecoderNR(int codeBookMode, int N1, int N2, int nLayers, type1PMI_S i1i2index)
{
    cmat tP;
    Array<cmat> W;
    int layerNum =-1 , i2=0, h=0, v=0, g=0, indx,p=0;
    int nPorts = dlCodeBookConfig.nCSIRSPorts;
    W = getCodebook(nPorts,nLayers,true);
    
    int maxLayers = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_I_MULTIPANEL_) ? 4:8;
    CBParam_S CBparam;
    
    if(nLayers > maxLayers|| nLayers<1)
    {
        cout << "nLayers must be in [1,"<<maxLayers<<"]... aborting ..."<< endl; abort();
    }
    
    if(dlCodeBookConfig.codeBookType != _NR_CODEBOOK_TYPE_I_MULTIPANEL_)
    {
        if(nPorts==2)
        {
            return W(i1i2index.i11); //Assuming i11 is set
        }
        else
        {
            CBparam = getParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, N1, N2, i2, h, v, g);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
        }
    }
    else 
    {
        CBparam = getMultipanelParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, dlCodeBookConfig.Ng, h, v, g,p,i2);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
    }
    indx = decode(CBparam.indicesSizes,getVectorFromPMIIndices(i1i2index,dlCodeBookConfig.codeBookType));
    
    if(indx>=CBparam.CBSizePerLayer || indx<0 || W.length()!=CBparam.CBSizePerLayer)
    {
        cout<<"nLayers: "<<nLayers<<" W Size: "<<W.length()<<endl;
        cout<<"Index: "<<indx<<" CBSize: "<< CBparam.CBSizePerLayer<<endl;
        i1i2index.printValues();
        CBparam.PrintSizes();
        abort();
    }
    
    tP = W(indx);
    
    return tP;
}

cmat Codebook::getPrecoderNR(int nLayers, type1PMI_S i1i2index)
{ 
    cmat tP;
    
    int maxLayers = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_I_MULTIPANEL_) ? 4 :8;
    
    if(nLayers > maxLayers || nLayers<1)
    {
        cout << "nLayers must be in [1,"<<maxLayers<<"]... aborting ..."<< endl; abort();
    }
    
    if(dlCodeBookConfig.nCSIRSPorts==1)
    {
        return ones_c(1,1);
    }
    else if(dlCodeBookConfig.nCSIRSPorts == 2)
    {
        Array<cmat> W = getCodebook(dlCodeBookConfig.nCSIRSPorts,nLayers,true);
        return W(i1i2index.i2); //Assuming i2 is within the range of the Codebook indices
    }
    
    tP = constructTypeIPrecoder(nLayers,i1i2index,dlCodeBookConfig);
    
    return tP;
}

Array<cmat> Codebook::getPrecoderPerSubband(int nLayers,int nSubbands, type1PMI_S i1i2index,TypeIIPMIIndices pmiIndices,int coPhase)
{
    Array<cmat> tP;
    switch(dlCodeBookConfig.codeBookType)
    {
        case _NR_CODEBOOK_TYPE_I_:
        case _NR_CODEBOOK_TYPE_I_MULTIPANEL_:
            {
                int maxLayers = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_I_MULTIPANEL_) ? 4 :8;
                
                if(nLayers > maxLayers || nLayers<1)
                {
                    cout << "nLayers must be in [1,"<<maxLayers<<"]... aborting ..."<< endl; abort();
                }
                
                if(dlCodeBookConfig.nCSIRSPorts==1)
                {
                    cmat P = ones_c(1,1);
                    return repeatAsArray(P,i1i2index.i2Indices.length());
                }
                else if(dlCodeBookConfig.nCSIRSPorts == 2)
                {
                    Array<cmat> W = getCodebook(dlCodeBookConfig.nCSIRSPorts,nLayers,true);
                    return getFromArray(W,i1i2index.i2Indices); //Assuming i2 is within the range of the Codebook indices
                }
                tP.set_length(i1i2index.i2Indices.length());
                for(int sb_cnt=0;sb_cnt<i1i2index.i2Indices.length();sb_cnt++)
                {
                    type1PMI_S pmi = i1i2index;
                    pmi.i2 = i1i2index.i2Indices(sb_cnt);
                    tP(sb_cnt) = constructTypeIPrecoder(nLayers,pmi,dlCodeBookConfig);
                }
            }
            break;
        case _NR_CODEBOOK_TYPE_II_:
        case _NR_CODEBOOK_PORT_SELECTION_:
        case _NR_CODEBOOK_TYPE_II_ENHANCED_:
        case _NR_CODEBOOK_PORT_SELECTION_ENHANCED_:
        case _NR_CODEBOOK_PORT_SELECTION_FURTHER_ENHANCED_:
        case _NR_CODEBOOK_TYPE_II_ENHANCED_CJT_:
            {
                int maxLayers = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_II_ || dlCodeBookConfig.codeBookType == _NR_CODEBOOK_PORT_SELECTION_) ? 2 : 4;
                
                if(nLayers > maxLayers || nLayers<1)
                {
                    cout << "nLayers must be in [1,"<<maxLayers<<"]... aborting ..."<< endl; abort();
                }
                tP = getNRPrecoderPerSubband(nSubbands,pmiIndices);
                
                if(coPhase!=0)
                for(int sb_cnt=0;sb_cnt<tP.length();sb_cnt++)
                {
                    tP(sb_cnt) *= expj(2*pi*coPhase/(double)dlCodeBookConfig.N_Phase);
                }
            }
            break;
        default:
            cout<<"Unsupported Codebook Type"<<endl;
            abort();
    }
    
    return tP;
}

Array<cmat> Codebook::getNRPrecoderPerSubband(int nLayers, type1PMI_S& pmiIndices)
{
    int nSubbands = pmiIndices.i2Indices.length(); 
    Array<cmat> tP(nSubbands);
    
    for(int sb_cnt=0;sb_cnt<nSubbands;sb_cnt++)
    {
        pmiIndices.i2 = pmiIndices.i2Indices(sb_cnt);
        tP = constructTypeIPrecoder(nLayers,pmiIndices,dlCodeBookConfig);
    }
    return tP;
}

Array<cmat> Codebook::constructDLType1Codebook(int Ng,int N1,int N2,int P,int nLayers,NRCodeBookMode CBmode)
{
    CodeBookConfig_S cbConfig;
    cbConfig.setType1(Ng,N1,N2,P,CBmode);
    
    CBParam_S CBparam = getCodebookParam(nLayers);
    
    Array<cmat> codebook;codebook.set_length(0);
    for(int cnt=0;cnt<CBparam.CBSizePerLayer;cnt++)
    {
        type1PMI_S precIndex = getPMIIndicesFromVector(encode(CBparam.indicesSizes,cnt));
        append(codebook,constructTypeIPrecoder(nLayers,precIndex,cbConfig));
    }
    return codebook;
}

Array<cmat> Codebook::getDLi2Codebook(i1index_S i1Index,int nLayers)
{
    CBParam_S CBparam = getCodebookParam(nLayers);
    Array<cmat> i2Codebook; i2Codebook.set_size(0);
    for(int i2=0;i2<CBparam.i2Size;i2++)
    {
        append(i2Codebook,getPrecoderNR(nLayers,type1PMI_S(i1Index.i11,i1Index.i12,i1Index.i13,i1Index.i14,i2)));
    }
    return i2Codebook;
}

//Finding all possible i2 for given i1 indices
ivec Codebook::geti2CmatIndicesIn_W(i1index_S i1, int codeBookMode, int N1, int N2, int nLayers)
{
    ivec i2indices;
    int startIndx=0;
    int layerNum =-1 , i2=0, h=0, v=0, g=0, indx,p=0;   // layernum = -1, it is used to get sizes of indices. Should not be confused.
    int maxLayers = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_I_MULTIPANEL_) ? 4:8;
    
    if(nLayers > maxLayers|| nLayers<1)
    {
        cout << "nLayers must be in [1,"<<maxLayers<<"]... aborting ..."<< endl; abort();
    }
    
    CBParam_S CBparam;
    if(dlCodeBookConfig.codeBookType != _NR_CODEBOOK_TYPE_I_MULTIPANEL_)
    {
        CBparam = getParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, N1, N2, i2, h, v, g); 
    }
    else 
    {
        CBparam = getMultipanelParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, dlCodeBookConfig.Ng, h, v, g,p,i2);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
    }
    
    startIndx = decode(CBparam.indicesSizes,getVectorFromPMIIndices(getPMI(i1),dlCodeBookConfig.codeBookType));
    if(startIndx+CBparam.i2Size-1 >= CBparam.CBSizePerLayer)
    {
        cout<<"Invalid startIndx.."<<endl;abort();
    }
    i2indices = startIndx + getIntegers(0,CBparam.i2Size-1);
    return i2indices;
}

//Finding all possible i2 for given i1 indices
ivec Codebook:: geti2CmatIndicesIn_W(i1index_S i1 , int nLayers)
{
    ivec i2indices;
    int startIndx=0;
    int layerNum =-1 , i2=0, h=0, v=0, g=0,p=0;   // layernum = -1, it is used to get sizes of indices. Should not be confused.
    CBParam_S CBparam;
    int maxLayers = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_I_MULTIPANEL_) ? 4 :8;
    
    if(nLayers > maxLayers|| nLayers<1)
    {
        cout << "nLayers must be in [1,"<<maxLayers<<"]... aborting ..."<< endl; abort();
    }
    
    if(dlCodeBookConfig.codeBookType != _NR_CODEBOOK_TYPE_I_MULTIPANEL_)
    {
        CBparam = getParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, i2, h, v, g); 
        if(nLayers < 2 || nLayers > 4 )
        {
            if(i1.i13 != 0)
            {
                cout<<"i13 must be zero.. check... "<<endl;
                abort();
            }
        }
    }
    else
    {
        if(nLayers<2 && i1.i13!=0)
        {
            cout<<"i13 must be zero.. check... "<<endl;
            abort();
        }
        CBparam = getMultipanelParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, dlCodeBookConfig.Ng, h, v, g,p,i2);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
    }
    startIndx = decode(CBparam.indicesSizes,getVectorFromPMIIndices(getPMI(i1),dlCodeBookConfig.codeBookType));
    if(startIndx+CBparam.i2Size-1 >= CBparam.CBSizePerLayer)
    {
        cout<<"Invalid startIndx.."<<endl;abort();
    }
    i2indices = startIndx + getIntegers(0,CBparam.i2Size-1);
    return i2indices;
}

//Finding all possible i2 for given i1 indices
ivec Codebook:: geti2CmatIndicesIn_W(type1PMI_S i1 , int nLayers)
{
  ivec i2indices;
  int startIndx=0;
  int layerNum =-1 , i2=0, h=0, v=0, g=0,p=0;   // layernum = -1, it is used to get sizes of indices. Should not be confused.
  CBParam_S CBparam;
  int maxLayers = (dlCodeBookConfig.codeBookType == _NR_CODEBOOK_TYPE_I_MULTIPANEL_) ? 4 :8;
  
  if(nLayers > maxLayers|| nLayers<1)
  {
      cout << "nLayers must be in [1,"<<maxLayers<<"]... aborting ..."<< endl; abort();
  }
  
  if(dlCodeBookConfig.codeBookType != _NR_CODEBOOK_TYPE_I_MULTIPANEL_)
  {
      if(dlCodeBookConfig.nCSIRSPorts==1)
          return to_ivec(0);
    CBparam = getParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, i2, h, v, g); 
    if(nLayers < 2 || nLayers > 4 )
    {
        if(i1.i13 != 0)
        {
        cout<<"i13 must be zero.. check... "<<endl;
        abort();
        }
    }
  }
  else
  {
      if(nLayers<2 && i1.i13!=0)
      {
          cout<<"i13 must be zero.. check... "<<endl;
          abort();
      }
      CBparam = getMultipanelParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, dlCodeBookConfig.Ng, h, v, g,p,i2);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
  }
  i1.i2=0;
  startIndx = decode(CBparam.indicesSizes,getVectorFromPMIIndices(i1,dlCodeBookConfig.codeBookType));
  if(startIndx+CBparam.i2Size-1 >= CBparam.CBSizePerLayer)
  {
      cout<<startIndx<<endl;
      i1.printValues();
      cout<<CBparam.indicesSizes<<endl;
      cout<<"Invalid startIndx.."<<endl;abort();
  }
  i2indices = startIndx + getIntegers(0,CBparam.i2Size-1);
  return i2indices;
}

cmat getVlm(int l,int m,int N1,int N2,int O1,int O2)
{
    cmat tUm(N2,1),tVl(N1,1);
    double thetaL = 2.0*pi*(double)l / (double)(N1*O1),thetaM = 2.0*pi*(double)m /(double) (N2*O2);
    tUm.set_col(0,expj(thetaM*getIntegers(0,N2-1)));
    tVl.set_col(0,expj(thetaL*getIntegers(0,N1-1)));
    return kron(tVl,tUm);
}

cmat getVlm_prime(int l, int m, int N1, int N2, int O1, int O2)
{
    cmat tUm(N2,1),tVl(N1/2,1);
    double thetaL = 4.0*pi*(double)l / (double)(N1*O1),thetaM = 2.0*pi*(double)m /(double) (N2*O2);
    tUm.set_col(0,expj(thetaM*getIntegers(0,N2-1)));
    tVl.set_col(0,expj(thetaL*getIntegers(0,N1/2-1)));
    return kron(tVl,tUm);
}

cmat getUm(int m, int N2, int O2)      // 38.214 5.2.2.2.1 Vector Um 
{
    cmat tUm(N2,1);
    if(N2==1)
        tUm(0,0) = std::complex<double> (1,0);
    else
    {
        for(auto k=0;k<N2;k++)
	  tUm(k,0) = expj(2.0*pi*(double)(m*k)/(double)(O2*N2));
    }
    return tUm;
}

cmat getVlm(cmat tUm, int l, int N1, int N2, int O1)
{
    dComplex ph;
    cmat tVlm(N1*N2,1);
    tVlm = tUm;
    
    for(auto k=1;k<N1;k++)
    {
      ph = expj(2*pi*(double)(l*k)/(double)(O1*N1));
      tVlm = concat_vertical(tVlm,ph*tUm);
    }
    return tVlm;
}

cmat getVlm_prime(cmat tUm, int l, int N1, int N2, int O1)
{
    dComplex ph;
    cmat tVlm(N1*N2/2,1);
    tVlm = tUm;
    for(auto k=1;k<(N1/2);k++)
    {
      ph = expj(4*pi*(double)(l*k)/(double)(O1*N1));
        tVlm = concat_vertical(tVlm,ph*tUm);
    }
    return tVlm;
}

cmat genCmat(dComplex v1, dComplex v2, dComplex v3, dComplex v4)
{
   cmat M(4,1);
   M(0,0)=v1;M(1,0)=v2;M(2,0)=v3;M(3,0)=v4;
   return M;
}

Array<cmat> genTwoPortCodebook_DL_NR(int nLayers)
{
  Array<cmat> W;
  const dComplex R = dComplex(1.0,0.0);
  const dComplex I = dComplex(0.0,1.0);
    switch(nLayers)
    {
        case 1:
        {
            W.set_size(4);
            
            for(int i=0; i<4; i++)
                W(i).set_size(2,nLayers);
            
               W(0)(0) = R/sqrt(2); W(0)(1) = R/sqrt(2);
               W(1)(0) = R/sqrt(2); W(1)(1) = I/sqrt(2);
               W(2)(0) = R/sqrt(2); W(2)(1) = -R/sqrt(2);  
               W(3)(0) = R/sqrt(2); W(3)(1) = -I/sqrt(2);
            
        }
        break;
        
        case 2:
        {
            W.set_size(2);
            for(int i=0; i<2; i++)
                W(i).set_size(2,nLayers);
            
            W(1) = W(0) = ("0.5 0.5; 0.5 -0.5");
            W(1).set_row(1,W(1).get_row(1)*I);   // Multiplying second row with j
        
        }
        break;
        default: cout<<"[both:] Undefined #Layers .... exiting in generateCodebookForTwoPorts();"<<endl;abort();
    }
  return(W);
}

bool Check(cmat P)
{
    cmat W = P.hermitian_transpose()*P;
    W = W/W(0,0);
    for(int i=0;i<W.rows();i++)
    {
        for(int j=0;j<W.cols();j++)
        {
            for(int k=0;k<=1;k++)
            {
                if(abs(W(i,j)-dComplex(k,0))<=1e-6)
                    W(i,j) = dComplex(k,0);
            }
        }
    }
    if(W!=eye_c(W.rows())){return true;}
    return false;
}

void PrintMatrices(int CBmode,int nLayers,int nPorts,string s,cmat P,cmat tVlm,int z=-1)
{
    stringstream ss;
    ss<<"./PrecoderMatrices/CBmode:"<<CBmode<<"-"<<nPorts<<"x"<<nLayers<<".txt";
    ofstream op(ss.str().c_str(),std::ios::app);
    op<<s<<endl;
    if(tVlm.cols()==1)
        op<<"Vlm:"<<endl;
    else 
        op<<"[Vlm Vlm']: "<<endl;
    op<<tVlm<<"\t";
    op<<"\n\nP:\n\n"<<P<<endl;
    op.close();
}

void PrintMatrices(Array<Array<Array<cmat>>> tW,int N1,int N2,int Ng,int z=-1)
{
    int O1=4,O2=(N2==1) ? 1:4;
    stringstream ss;
    ss<<"./Matrices/CBmode:"<<1<<"-"<<2*N1*N2*Ng<<"x"<<1<<".txt";
    int i =system("rm -r PrecoderMatrices");
    i = system("mkdir PrecoderMatrices");
    std::ofstream op("./PrecoderMatrices/codebookSizes.txt",std::ios::out);
    int h=0,v=0,g=0,size_cnt=0,p=0;
    for(int nLayers=1;nLayers<=tW.length();nLayers++){
        for(int CBmode=0;CBmode<tW(nLayers-1).length();CBmode++){
            CBParam_S CBparam = (Ng==1) ? getParam(NRCodeBookMode(CBmode), nLayers,-1,N1,N2, 0, h,v,g) : getMultipanelParam(NRCodeBookMode(CBmode), nLayers,-1,N1,N2,Ng, h, v, g,p,0);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
            op<<"nLayers: "<<nLayers<<" CBmode: "<<CBmode+1<<" CBsize: "<<tW(nLayers-1)(CBmode).length()<<"\t"<<CBparam.indicesSizes<<endl;
            for (h=0,size_cnt=0;h<CBparam.i11Size;h++){
                for(v=0;v<CBparam.i12Size;v++){
                    for(g=0;g<CBparam.i13Size;g++){
                        ivec k1k2 = getK1K2(nLayers,N1,N2,g,Ng);
                        int k1 = k1k2(0),k2=k1k2(1);
                        for(p=0;p<CBparam.i14Size;p++){
                            for(int n=0;n<CBparam.i2Size;n++,size_cnt++){
                                stringstream op;
                                cmat tVlm,Vl,Um;cvec phi_n(0),phi_p(0);
                                for(int layerNum=1;layerNum<=nLayers;layerNum++){
                                    CBParam_S tempCBparam = (Ng==1) ? getParam(NRCodeBookMode(CBmode), nLayers,layerNum,N1,N2, n, h,v,g) : getMultipanelParam(NRCodeBookMode(CBmode), nLayers,layerNum,N1,N2,Ng, h, v, g,p,n);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
                                    tVlm = concat_horizontal(tVlm,getVlm(tempCBparam.i11,tempCBparam.i12,N1,N2,O1,O2));
                                    Vl=concat_horizontal(Vl,getUm(tempCBparam.i11,N1,O1));
                                    Um=concat_horizontal(Um,getUm(tempCBparam.i11,N2,O2));
                                    if(Ng==1)
                                        append(phi_n,tempCBparam.phi_n);
                                    else 
                                    {
                                        append(phi_n,Phi(tempCBparam.i2));
                                        append(phi_p,Phi(tempCBparam.i14));
                                    }
                                }
                                op<<"\ni11: "<<h<<" i12: "<<v<<" i13: "<<g<<" [k1 k2]: ["<<k1<<" "<<k2<<"]";
                                if(Ng!=1)
                                    op<<" i14: "<<p<<" i2: "<<n;
                                else
                                    op<<" i2: "<<n;
                                op<<" CBmode: "<<CBmode+1<<" CBSize: "<<CBparam.CBSizePerLayer<<" "<<CBparam.indicesSizes<<" CodebookIndex: "<<size_cnt<<endl;
                                op<<"Um:\n"<<Um<<"\n\nVl:\n"<<Vl<<endl;
                                if(Ng!=1)
                                    op<<"\nphi_p:"<<phi_p<<" phi_n: "<<phi_n<<endl;
                                else 
                                    op<<"\nphi_n: "<<phi_n<<endl;
                                PrintMatrices(CBmode+1,nLayers,2*N1*N2*Ng,op.str(),tW(nLayers-1)(CBmode)(size_cnt),tVlm,z);                               
                            }}}}}}}    
                            op.close();
}

ivec getK1K2(int nLayers,int N1,int N2,int g,int Ng)
{
    ivec k1k2=zeros_i(2);
    Array<imat> i13k1k2Table;int i13index=-1;
    if((Ng==1 && 2*N1*N2>=16 && nLayers>2) || nLayers ==1){return k1k2;}
    if(nLayers==2)
    {
        i13k1k2Table.set_length(4);
        i13k1k2Table(0) = ("0 0; 4 0; 0 4; 8 0"); i13k1k2Table(1) = ("0 0; 4 0; 0 4; 4 4");
        i13k1k2Table(2) = ("0 0; 4 0"); i13k1k2Table(3) = ("0 0; 4 0; 8 0; 12 0");
        i13index = (N1>N2 && N2>1) ? 0 : ((N1 == N2) ? 1: ((N1>2 && N2==1) ? 3 : 2));
    }
    else if(nLayers==3 || nLayers==4)
    {
        i13k1k2Table.set_length(5);
        i13k1k2Table(0) = ("4 0"); i13k1k2Table(1) = ("4 0;8 0;12 0"); i13k1k2Table(2) = ("4 0;8 0;12 0;16 0"); 
        i13k1k2Table(3) = ("4 0; 0 4; 4 4"); i13k1k2Table(4) = ("4 0; 0 4; 4 4;8 0");
        if(Ng==1)
            i13index = (N2==1) ? (N1/2)-1 : N1+1;
        else
            i13index = (N2==1) ? log2(N1)-1 : 2 + log2(N1);
    }
    if(i13index!=-1)
        k1k2 = i13k1k2Table(i13index).get_row(g);
    
    return k1k2;
}

//construct precoder layer by layer using Table 5.2.2.2.2-3 to 6 in TS 38.214. 
cmat constructMPPrecoder(int nLayers,type1PMI_S& precIndex,CodeBookConfig_S& dlCodeBookConfig,NRCodeBookMode CBmode)
{
    if(CBmode==_NR_CODEBOOK_MODE_UNIT_)
        CBmode = dlCodeBookConfig.codeBookMode;
    cmat P;
    for (int layerNum = 1; layerNum <= nLayers; layerNum++)
    {
        CBParam_S CBparam = getMultipanelParam(CBmode, nLayers, layerNum,dlCodeBookConfig.N1, dlCodeBookConfig.N2, dlCodeBookConfig.Ng, precIndex.i11, precIndex.i12, precIndex.i13,precIndex.i14,precIndex.i2);    
        cmat Vlm = getVlm(CBparam.i11,CBparam.i12,dlCodeBookConfig.N1,dlCodeBookConfig.N2,dlCodeBookConfig.O1,dlCodeBookConfig.O2);
        cmat phi;
        if(!CBmode)
        {
            cmat phi_n,phi_p;
            phi_n = zeros_c(2,1);phi_n(0,0) = dComplex(1,0);phi_n(1,0) = CBparam.phi_n;
            phi_p = concat(dComplex(1,0),CBparam.phi_p);
            phi = kron(phi_p,phi_n);
        }
        else
        {
            phi = zeros_c(4,1);
            phi(0,0) = dComplex(1,0); phi(1,0)=CBparam.phi_n;
            cvec ap_bn = elem_mult(CBparam.a_p,CBparam.b_n);
            phi(2,0) = ap_bn(0);phi(3,0)=ap_bn(1);
        }
        cmat W = kron(phi,Vlm);
        P = concat_horizontal(P,W); 
    }
    P = P / sqrt(nLayers * dlCodeBookConfig.nCSIRSPorts);
    return P;
}

//construct precoder layer by layer using Table 5.2.2.2.1-5 to 12 in TS 38.214. 
cmat constructSPPrecoder(int nLayers,type1PMI_S& precIndex,CodeBookConfig_S& dlCodeBookConfig,NRCodeBookMode CBmode)
{
    if(CBmode==_NR_CODEBOOK_MODE_UNIT_)
        CBmode = dlCodeBookConfig.codeBookMode;
    cmat P;
    for(int layerNum=1;layerNum<=nLayers;layerNum++)
    {
        cmat Vlm;
        CBParam_S CBparam = getParam(CBmode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, precIndex.i2, precIndex.i11, precIndex.i12, precIndex.i13);    
        
        if(dlCodeBookConfig.nCSIRSPorts>=16 && (nLayers==3 || nLayers==4))
        {
            cmat vlm_ = getVlm_prime(precIndex.i11,precIndex.i12,dlCodeBookConfig.N1,dlCodeBookConfig.N2,dlCodeBookConfig.O1,dlCodeBookConfig.O2);
            cmat theta(2,1); theta(0,0) = dComplex(1,0);theta(1,0) = CBparam.theta_p;
            Vlm = kron(theta,vlm_);
        }
        else
        {
            Vlm = getVlm(CBparam.i11,CBparam.i12,dlCodeBookConfig.N1,dlCodeBookConfig.N2,dlCodeBookConfig.O1,dlCodeBookConfig.O2);
        }
        cmat phi(2,1);phi(0,0)=dComplex(1,0);phi(1,0)=CBparam.phi_n;
        cmat W = kron(phi,Vlm);
        P = concat_horizontal(P,W);
    }
    P = P / sqrt(nLayers * dlCodeBookConfig.nCSIRSPorts);
    return P;
}

cmat constructTypeIPrecoder(int nLayers,type1PMI_S& precIndex,CodeBookConfig_S& dlCodeBookConfig,NRCodeBookMode CBmode)
{
    if(dlCodeBookConfig.Ng==1)
        return constructSPPrecoder(nLayers,precIndex,dlCodeBookConfig,CBmode);
    else
        return constructMPPrecoder(nLayers,precIndex,dlCodeBookConfig,CBmode);
}

Array<Array<Array<cmat>>> Codebook::genType1CodebookNR()
{
    Array<Array<Array<cmat>>> tW; 
    type1PMI_S i1i2index;
    CBParam_S CBparam, tempCBParam;
    //ofstream myfile;
    cmat tUm,tVlm, tVlm_prime, tempLayer, tL1, tL2;
    Mat<int> signOfPhi = (" 1 1 -1 -1 ");
    int h=0, v=0, g=0, i2=0, nLayers = -1, nLayersMax, layerNum, size_cnt=0, mapIndx=0;
    nLayersMax = (dlCodeBookConfig.nCSIRSPorts == 4) ? 4 : 8; // Max layers for 4 ports are 4. Supported ports: 4,8,12,16,24 and 32
    tW.set_size(nLayersMax);  // codebook for each layer
    
    int halfbeamIndx;
    for(nLayers=1;nLayers<=nLayersMax;nLayers++)  // maximum number of supported layers = 8 . This for loop is for having separate codebook for each layer. nLayers is starting from 1, son wherever accesing array of mat subtract nLayers by 1.
    {
        tW(nLayers-1).set_size(2);    // For each layer two codebook modes. Although, one codebook mode for nLayers >=2, this gives provision for future enhancements      
        for(int CBmode =0; CBmode<2;CBmode++)   // only two codebook modes 
        {
            layerNum = -1;h=0, v=0, g=0, i2=0; // for preliminary paramaters
            CBparam = getParam(NRCodeBookMode(CBmode), nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, i2, h, v, g);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
            tW(nLayers-1)(CBmode).set_size(CBparam.CBSizePerLayer);   // This sets the size of codebook for each layer and for each CBmode
            
            if(nLayers==1 && CBmode==0)
            {
                type1halfBeamswithi1.set_size(CBparam.i11Size*CBparam.i12Size);  //HalfBeams
            } 
            for (auto k=0;k<CBparam.CBSizePerLayer;k++)
                tW(nLayers-1)(CBmode)(k).set_size(dlCodeBookConfig.nCSIRSPorts,nLayers);      // Each Matrix size = No.of Ports X No. of layers
                size_cnt =0;
            
            for (h=0,halfbeamIndx=0;h<CBparam.i11Size;h++)
                for(v=0;v<CBparam.i12Size;v++,halfbeamIndx++)
                {
                    if(nLayers==1 && CBmode==0)
                    {
                        layerNum = 1;
                        tempCBParam = getParam(NRCodeBookMode(CBmode), nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, i2, h, v, g);
                        tVlm = getVlm(tempCBParam.i11,tempCBParam.i12,dlCodeBookConfig.N1,dlCodeBookConfig.N2,dlCodeBookConfig.O1,dlCodeBookConfig.O2);
                        type1halfBeamswithi1(halfbeamIndx).i11=h;
                        type1halfBeamswithi1(halfbeamIndx).i12=v;
                        type1halfBeamswithi1(halfbeamIndx).i2Size=tempCBParam.i2Size;
                        type1halfBeamswithi1(halfbeamIndx).Halfbeam=tVlm;
                    }
                    for(g=0;g<CBparam.i13Size;g++)
                        for(int k=0;k<CBparam.i2Size;k++)
                        {
                            type1PMI_S precIndex(h,v,g,-1,k);
                            cmat W = constructSPPrecoder(nLayers,precIndex,dlCodeBookConfig,NRCodeBookMode(CBmode));
                            tW(nLayers-1)(CBmode)(size_cnt) = W;size_cnt++;
                            if(Check(W)){cout<<"Invalid Precoder..\n\n"<<endl;CBparam.PrintIndices();cout<<h<<"\t"<<v<<"\t"<<g<<"\t"<<getK1K2(nLayers,dlCodeBookConfig.N1,dlCodeBookConfig.N2,g)<<endl;abort();}
                        }
                }
        }
    }    
    
    bool printMatrices=false;
    if(printMatrices)
        PrintMatrices(tW,dlCodeBookConfig.N1,dlCodeBookConfig.N2,dlCodeBookConfig.Ng,int(printMatrices));
    
    return tW;
}

Array<Array<Array<cmat>>> Codebook::genType1MultipanelCodebookNR()
{
    Array<Array<Array<cmat>>> tW;
    CBParam_S CBparam,tempCBparam;
    cmat tUm,tVlm;
    int h=0,v=0,g=0,p=0,n=0,nLayers=-1,nLayersMax=4,layerNum,size_cnt;
    tW.set_size(nLayersMax);
    
    int halfbeamIndx;
    
    for(nLayers=1;nLayers<=nLayersMax;nLayers++)  
    {
        tW(nLayers-1).set_size(2);    // For each layer two codebook modes. Although, one codebook mode for nLayers >=2, this gives provision for future enhancements      
        for(int CBmode =0; CBmode<2;CBmode++)   // only two codebook modes 
        {
            layerNum = -1;
            CBparam = getMultipanelParam(NRCodeBookMode(CBmode), nLayers, layerNum,dlCodeBookConfig.N1, dlCodeBookConfig.N2, dlCodeBookConfig.Ng, h, v, g,p,n);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
            tW(nLayers-1)(CBmode).set_size(CBparam.CBSizePerLayer);   // This sets the size of codebook for each layer and for each CBmode
            
            if(nLayers==1 && CBmode==0)
            {
                type1halfBeamswithi1.set_size(CBparam.i11Size*CBparam.i12Size);  //HalfBeams
            } 
            for (auto num=0;num<CBparam.CBSizePerLayer;num++)
                tW(nLayers-1)(CBmode)(num).set_size(dlCodeBookConfig.nCSIRSPorts,nLayers);      // Each Matrix size = No.of Ports X No. of layers
                size_cnt =0;
            
            for (h=0,halfbeamIndx=0;h<CBparam.i11Size;h++)
                for(v=0;v<CBparam.i12Size;v++,halfbeamIndx++)
                {
                    if(nLayers==1 && CBmode==0)
                    {
                        layerNum = 1;
                        tVlm = getVlm(h,v,dlCodeBookConfig.N1,dlCodeBookConfig.N2,dlCodeBookConfig.O1,dlCodeBookConfig.O2);
                        type1halfBeamswithi1(halfbeamIndx).i11=h;
                        type1halfBeamswithi1(halfbeamIndx).i12=v;
                        type1halfBeamswithi1(halfbeamIndx).i2Size=CBparam.i2Size;
                        type1halfBeamswithi1(halfbeamIndx).Halfbeam=tVlm;
                    }
                    for(g=0;g<CBparam.i13Size;g++)
                        for(p=0;p<CBparam.i14Size;p++) 
                            for(n=0;n<CBparam.i2Size;n++)
                            {
                                type1PMI_S precIndex(h,v,g,p,n);
                                cmat W = constructMPPrecoder(nLayers,precIndex,dlCodeBookConfig,NRCodeBookMode(CBmode));
                                tW(nLayers-1)(CBmode)(size_cnt) =W;
                                if(Check(W)){cout<<"Invalid Precoder..\n\n"<<endl;tempCBparam.PrintIndices();cout<<h<<"\t"<<v<<"\t"<<g<<"\t"<<getK1K2(nLayers,dlCodeBookConfig.N1,dlCodeBookConfig.N2,g,dlCodeBookConfig.Ng)<<endl;cout<<W<<"\n\n"<<endl;abort();}
                                size_cnt++;
                            }
                }
        }
    }
    bool printMatrices=false;   //  0 not to print Matrices, 1 prints matrices
    if(printMatrices)
        PrintMatrices(tW,dlCodeBookConfig.N1,dlCodeBookConfig.N2,dlCodeBookConfig.Ng,int(printMatrices));
    return tW;
}

Array<Array<cmat>> Codebook::genType1CodebookNR(NRCodeBookMode CBmode)
{
    Array<Array<cmat>> tW; 
    type1PMI_S i1i2index;
    CBParam_S CBparam, tempCBParam;
    //ofstream myfile;
    cmat tUm,tVlm, tVlm_prime, tempLayer, tL1, tL2;
    Mat<int> signOfPhi = (" 1 1 -1 -1 ");
    int h=0, v=0, g=0, i2=0, nLayers = -1, nLayersMax, layerNum, size_cnt=0, mapIndx=0;
    nLayersMax = (dlCodeBookConfig.nCSIRSPorts == 4) ? 4 : 8; // Max layers for 4 ports are 4. Supported ports: 4,8,12,16,24 and 32
    tW.set_size(nLayersMax);  // codebook for each layer
    
    int halfbeamIndx;
    for(nLayers=1;nLayers<=nLayersMax;nLayers++)  // maximum number of supported layers = 8 . This for loop is for having separate codebook for each layer. nLayers is starting from 1, son wherever accesing array of mat subtract nLayers by 1.
    {
        layerNum = -1;h=0, v=0, g=0, i2=0; // for preliminary paramaters
        CBparam = getParam(CBmode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, i2, h, v, g);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
        tW(nLayers-1).set_size(CBparam.CBSizePerLayer);   // This sets the size of codebook for each layer and for each CBmode
        
        if(nLayers==1)
        {
            type1halfBeamswithi1.set_size(CBparam.i11Size*CBparam.i12Size);  //HalfBeams
        } 
        for (auto k=0;k<CBparam.CBSizePerLayer;k++)
            tW(nLayers-1)(k).set_size(dlCodeBookConfig.nCSIRSPorts,nLayers);      // Each Matrix size = No.of Ports X No. of layers
            size_cnt =0;
        
        for (h=0,halfbeamIndx=0;h<CBparam.i11Size;h++)
            for(v=0;v<CBparam.i12Size;v++,halfbeamIndx++)
            {
                if(nLayers==1)
                {
                    layerNum = 1;
                    tempCBParam = getParam(CBmode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, i2, h, v, g);
                    tVlm = getVlm(tempCBParam.i11,tempCBParam.i12,dlCodeBookConfig.N1,dlCodeBookConfig.N2,dlCodeBookConfig.O1,dlCodeBookConfig.O2);
                    type1halfBeamswithi1(halfbeamIndx).i11=h;
                    type1halfBeamswithi1(halfbeamIndx).i12=v;
                    type1halfBeamswithi1(halfbeamIndx).i2Size=tempCBParam.i2Size;
                    type1halfBeamswithi1(halfbeamIndx).Halfbeam=tVlm;
                }
                for(g=0;g<CBparam.i13Size;g++)
                    for(int k=0;k<CBparam.i2Size;k++)
                    {
                        type1PMI_S precIndex(h,v,g,-1,k);
                        cmat W = constructSPPrecoder(nLayers,precIndex,dlCodeBookConfig,CBmode);
                        tW(nLayers-1)(size_cnt) = W;size_cnt++;
//                         if(Check(W)){cout<<"Invalid Precoder..\n\n"<<endl;CBparam.PrintIndices();cout<<h<<"\t"<<v<<"\t"<<g<<"\t"<<getK1K2(nLayers,dlCodeBookConfig.N1,dlCodeBookConfig.N2,g)<<endl;abort();}
                    }
            }
    }    
    return tW;
}

Array<cmat> Codebook::generateType1CodebookNR(NRCodeBookMode CBmode,int nLayers)
{
    int layerNum = -1,h=0, v=0, g=0, i2=0,size_cnt; // for preliminary paramaters
    CBParam_S CBparam = getParam(CBmode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, i2, h, v, g);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
    Array<cmat> tW(CBparam.CBSizePerLayer);   // This sets the size of codebook for each layer and for each CBmode
    
    for(h=0,size_cnt=0;h<CBparam.i11Size;h++)
        for(v=0;v<CBparam.i12Size;v++)
            for(g=0;g<CBparam.i13Size;g++)
                for(int k=0;k<CBparam.i2Size;k++)
                {
                    type1PMI_S precIndex(h,v,g,-1,k);
                    cmat W = constructSPPrecoder(nLayers,precIndex,dlCodeBookConfig,CBmode);
                    tW(size_cnt) = W;size_cnt++;
                }
    return tW;
}

Array<Array<cmat>> Codebook::genType1MultipanelCodebookNR(NRCodeBookMode CBmode)
{
    Array<Array<cmat>> tW;
    CBParam_S CBparam,tempCBparam;
    cmat tUm,tVlm;
    int h=0,v=0,g=0,p=0,n=0,nLayers=-1,nLayersMax=4,layerNum,size_cnt;
    tW.set_size(nLayersMax);
    
    int halfbeamIndx;
    
    for(nLayers=1;nLayers<=nLayersMax;nLayers++)  
    {
        layerNum = -1;
        CBparam = getMultipanelParam(CBmode, nLayers, layerNum,dlCodeBookConfig.N1, dlCodeBookConfig.N2, dlCodeBookConfig.Ng, h, v, g,p,n);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
        tW(nLayers-1).set_size(CBparam.CBSizePerLayer);   // This sets the size of codebook for each layer and for each CBmode
        
        if(nLayers==1)
        {
            type1halfBeamswithi1.set_size(CBparam.i11Size*CBparam.i12Size);  //HalfBeams
        }
        for (auto num=0;num<CBparam.CBSizePerLayer;num++)
            tW(nLayers-1)(num).set_size(dlCodeBookConfig.nCSIRSPorts,nLayers);      // Each Matrix size = No.of Ports X No. of layers
            size_cnt =0;
        
        for (h=0,halfbeamIndx=0;h<CBparam.i11Size;h++)
            for(v=0;v<CBparam.i12Size;v++,halfbeamIndx++)
            {
                if(nLayers==1)
                {
                    layerNum = 1;
                    tVlm = getVlm(h,v,dlCodeBookConfig.N1,dlCodeBookConfig.N2,dlCodeBookConfig.O1,dlCodeBookConfig.O2);
                    type1halfBeamswithi1(halfbeamIndx).i11=h;
                    type1halfBeamswithi1(halfbeamIndx).i12=v;
                    type1halfBeamswithi1(halfbeamIndx).i2Size=CBparam.i2Size;
                    type1halfBeamswithi1(halfbeamIndx).Halfbeam=tVlm;
                }
                for(g=0;g<CBparam.i13Size;g++)
                    for(p=0;p<CBparam.i14Size;p++) 
                        for(n=0;n<CBparam.i2Size;n++)
                        {
                            type1PMI_S precIndex(h,v,g,p,n);
                            cmat W = constructMPPrecoder(nLayers,precIndex,dlCodeBookConfig,CBmode);
                            tW(nLayers-1)(size_cnt) =W;
                            if(Check(W)){cout<<"Invalid Precoder..\n\n"<<endl;tempCBparam.PrintIndices();cout<<h<<"\t"<<v<<"\t"<<g<<"\t"<<getK1K2(nLayers,dlCodeBookConfig.N1,dlCodeBookConfig.N2,g,dlCodeBookConfig.Ng)<<endl;cout<<W<<"\n\n"<<endl;abort();}
                            size_cnt++;
                        }
            }
    }
    return tW;
}

CBParam_S getParam(NRCodeBookMode CBmode, int nLayers, int layerNum,int N1, int N2, int i2, int h, int v, int g)
{
    CBParam_S CBparam;
    int O1=4,O2=(N2==1) ? 1:4;int k1=0,k2=0;
    CBparam.i11Size=N1*O1;CBparam.i12Size=N2*O2;CBparam.i2Size=(nLayers==1) ? 4 : 2;CBparam.i13Size=1;CBparam.i14Size=1;
    CBparam.i11=h;CBparam.i12=v;CBparam.i2 =to_ivec(i2%CBparam.i2Size);
    int signOfPhi=1;CBparam.phi_n = Phi(CBparam.i2(0));
    if(nLayers<=2)
    {
        CBparam.i13Size = (nLayers==1) ? 1 : (N2==1 && N1==2) ? 2 : 4;
        if(CBmode==_NR_CODEBOOK_MODE_2_)
        {
            int groupSize = CBparam.i2Size;
            CBparam.i11Size = N1*O1/2;CBparam.i12Size=(N2==1) ? 1 : N2*O2/2;CBparam.i2Size =groupSize*4;
            
            if(N2==1)
            {
                CBparam.i11 = 2*h + (int) i2/groupSize;
                CBparam.i12 = 0;
            }
            else
            {
                CBparam.i11 = 2*h + (int(i2/groupSize))%2;
                CBparam.i12 = 2*v + int(i2/(2*groupSize));
            }
        }
        if(layerNum==2)
        {
            signOfPhi=-1;
            ivec k1k2 = getK1K2(nLayers,N1,N2,g);
            CBparam.i11 += k1k2(0);CBparam.i12 += k1k2(1);
        }
    }
    else if (nLayers==3 || nLayers==4)
    {
        int nPorts = 2*N1*N2;
        if(nPorts>=16)
        {
            CBparam.i13Size=4;CBparam.i11Size= N1*O1/2;
            CBparam.theta_p = dComplex(2*(layerNum%2)-1,0)*Theta(g);
        }
        else 
        {
            CBparam.i13Size = (N2==1) ? (N1/2) + int(N1!=2): N1+1;
            if(layerNum%2==0)
            {
                ivec k1k2 = getK1K2(nLayers,N1,N2,g);
                CBparam.i11 +=k1k2(0);CBparam.i12 += k1k2(1);
            }
        }
        if(layerNum>2)
            signOfPhi =-1; 
    }
    else if(nLayers<=8)
    {
        int layersWithPhi = (nLayers==5) ? 2 : (nLayers==7) ? 3 : 4;
        if(layerNum>layersWithPhi){CBparam.phi_n=1;}
        signOfPhi = (nLayers!=7 || (nLayers==7 && layerNum<=3)) ? 2*(layerNum%2)-1 : 1-2*(layerNum%2);
        if(nLayers<=6)
        {
            if(N2>1)
            {
                CBparam.i11 += (layerNum<=2) ? 0 : O1;
                CBparam.i12 += (layerNum<=4) ? 0 : O2;
            }
            else
            {
                CBparam.i11 += ((layerNum-1)/2)*O1;
                CBparam.i12 = 0;
            }
        }
        else
        {
            if(N2==1 && N1==4){CBparam.i11Size=N1*O1/2;}
            if(N1>2  && N2==2){CBparam.i12Size=N2*O2/2;}
            int partNum = (nLayers==7 && layerNum>=4) ? 3 + (layerNum-4)/2 : (layerNum+1)/2;
            CBparam.i11 +=  ((N2==1) ? (partNum-1) : ((partNum-1)%2))*O1;
            CBparam.i12 += (N2==1 || partNum<=2) ? 0 : O2; 
        }
    }
    else
    {
        cout<<"Invalid nLayers..."<<endl;abort();
    }
    
    CBparam.phi_n *= signOfPhi;
    
    CBparam.indicesSizes=getVectorWithIntegers(4,CBparam.i11Size,CBparam.i12Size,CBparam.i13Size,CBparam.i2Size);
    CBparam.CBSizePerLayer = prod(CBparam.indicesSizes);
    
    return CBparam;
}

CBParam_S getMultipanelParam(NRCodeBookMode CBmode, int nLayers, int layerNum,int N1, int N2,int Ng, int h, int v, int g,int p,int n)
{
    CBParam_S CBparam;
    if(N1<2 && N2<1){ cout<<"Wrong N1 N2 configuration selected... aborting..."<<endl; abort(); }
    if(nLayers>4){cout<<"Layers more than 4 are not supported..."<<endl;abort();}
    int O1=4, O2=(N2 == 1) ? 1 : 4;
    
    CBparam.i11Size =N1*O1; CBparam.i12Size=N2*O2;  //i11, i12 Sizes
    
    //i13 Size
    CBparam.i13Size = (nLayers==1 || (N1==2 && N2==1)) ? 1 + int(nLayers==2) : (nLayers==2) ? 4 : log2(N1) + N2;
    
    CBparam.i2Sizes = (nLayers==1) ? to_ivec(4) :to_ivec(2);
    
    if(CBmode == _NR_CODEBOOK_MODE_2_)
        append(CBparam.i2Sizes,2*ones_i(2));
    
    //i14 and i2 Sizes
    CBparam.i14Sizes = (CBmode == _NR_CODEBOOK_MODE_1_) ? 4 * ones_i(Ng-1) : 4*ones_i(2);
    CBparam.i2Size = prod(CBparam.i2Sizes);
    CBparam.i14Size = prod(CBparam.i14Sizes);
    
    CBparam.indicesSizes = getVectorWithIntegers(5,CBparam.i11Size,CBparam.i12Size,CBparam.i13Size,CBparam.i14Size,CBparam.i2Size);
    
    CBparam.CBSizePerLayer = prod(CBparam.indicesSizes);
    if(layerNum==-1){return CBparam;}
    int signOfPhi = ((layerNum>2 || (layerNum==2 && nLayers==2))) ? -1: 1;
    
    ivec k1k2 = getK1K2(nLayers,N1,N2,g,Ng);
    (layerNum%2) ? (CBparam.i11=h,CBparam.i12=v) : (CBparam.i11=h+k1k2(0),CBparam.i12=v+k1k2(1));
    CBparam.i14 = encode(CBparam.i14Sizes,p);
    CBparam.i2 = encode(CBparam.i2Sizes,n);
    CBparam.phi_n = dComplex(signOfPhi,0)*Phi(CBparam.i2(0));
    if(CBmode == _NR_CODEBOOK_MODE_1_)
        CBparam.phi_p = Phi(CBparam.i14);
    else
    {
        CBparam.a_p = expj(pi/4)*Phi(CBparam.i14);
        CBparam.b_n = expj(-pi/4)*Phi(CBparam.i2(1,2));
        if(signOfPhi==-1)
            CBparam.b_n(1) *=signOfPhi;
    }
    
    return CBparam;
}

Array< Array< cmat > > genULCodeBookNR()
{
  Array<Array<cmat>> tW_UL;
  tW_UL.set_length(7);
  const std::complex<double> j(0, 1);
  Array<cmat> c;
  c.set_length(28);

  c(0)=genCmat(1,0,0,0);   c(8) =genCmat(0,1,0,1);  c(16)=genCmat(1,j,1,j);  c(24)=genCmat(1,-j,1,-j);
  c(1)=genCmat(0,1,0,0);   c(9) =genCmat(0,1,0,-1); c(17)=genCmat(1,j,j,1);  c(25)=genCmat(1,-j,j,-1);
  c(2)=genCmat(0,0,1,0);   c(10)=genCmat(0,1,0,j);  c(18)=genCmat(1,j,-1,-j);c(26)=genCmat(1,-j,-1,j);
  c(3)=genCmat(0,0,0,1);   c(11)=genCmat(0,1,0,-j); c(19)=genCmat(1,j,-j,-1);c(27)=genCmat(1,-j,-j,1);
  c(4)=genCmat(1,0,1,0);   c(12)=genCmat(1,1,1,-1); c(20)=genCmat(1,-1,1,1);
  c(5)=genCmat(1,0,-1,0);  c(13)=genCmat(1,1,j,j);  c(21)=genCmat(1,-1,j,-j);
  c(6)=genCmat(1,0,j,0);   c(14)=genCmat(1,1,-1,1); c(22)=genCmat(1,-1,-1,-1);
  c(7)=genCmat(1,0,-j,0);  c(15)=genCmat(1,1,-j,-j);c(23)=genCmat(1,-1,-j,j);
  
  tW_UL(0).set_length(6);        // Single Layer - 2 ports
  tW_UL(1).set_length(28);       // Single Layer - 4 ports - Transform Precoding Enabled
  tW_UL(2).set_length(28);       // Single Layer - 4 ports - Transform Precoding Disabled
  tW_UL(3).set_length(3);        // Two Layer - 2 ports - Transform Precoding Disabled
  tW_UL(4).set_length(22);       // Two Layer - 4 ports - Transform Precoding Disabled
  tW_UL(5).set_length(7);        // Three Layer - 4 ports - Transform Precoding Disabled
  tW_UL(6).set_length(5);        // Four Layer - 4 ports - Transform Precoding Disabled
  
  // *********** Single Layer - 2 ports ****************  //
  for(int i=0; i<tW_UL(0).size(); i++)
    tW_UL(0)(i).set_size(2,1);
  
  tW_UL(0)(0)(0) =  1/sqrt(2); tW_UL(0)(0)(1) =  0;
  tW_UL(0)(1)(0) =  0;         tW_UL(0)(1)(1) =  1/sqrt(2);
  tW_UL(0)(2)(0) =  1/sqrt(2); tW_UL(0)(2)(1) =  1/sqrt(2);
  tW_UL(0)(3)(0) =  1/sqrt(2); tW_UL(0)(3)(1) =  -1/sqrt(2);
  tW_UL(0)(4)(0) =  1/sqrt(2); tW_UL(0)(4)(1) =  j/sqrt(2);
  tW_UL(0)(5)(0) =  1/sqrt(2); tW_UL(0)(5)(1) =  -j/sqrt(2);
  
  // ******************************************************  //
  
  // Single Layer - 4 ports - Transform Precoding Enabled  //
  
  tW_UL(1) = c;
  for(int i=0; i<tW_UL(1).size(); i++)
  {
    tW_UL(1)(i) = 0.5*tW_UL(1)(i);
  }
  
  // ******************************************************  //
  
  
  // Single Layer - 4 ports - Transform Precoding Disabled  //
  
  tW_UL(2) = tW_UL(1);
  // Following are the diffrences
  tW_UL(2)(12)(3) = 0.5;  tW_UL(2)(14)(3) = -0.5; tW_UL(2)(17)(3) = -0.5; tW_UL(2)(19)(3) = 0.5;
  tW_UL(2)(20)(3) = -0.5;  tW_UL(2)(22)(3) = 0.5; tW_UL(2)(25)(3) = 0.5; tW_UL(2)(27)(3) = -0.5;
  
  
  // ******************************************************  //
  
  
  // Two Layer - 2 ports - Transform Precoding Disabled   //
  
    for(int i=0; i<tW_UL(3).size(); i++)
      tW_UL(3)(i).set_size(2,1);
    tW_UL(3)(0) = ("1 0; 0 1"); tW_UL(3)(0) = tW_UL(3)(0)/sqrt(2);
    tW_UL(3)(1) = ("0.5 0.5; 0.5 -0.5");
    tW_UL(3)(2) = tW_UL(3)(1); tW_UL(3)(2)(1,0) = 0.5*j; tW_UL(3)(2)(1,1) = -0.5*j;
    
    // ******************************************************  //
  
  
    // Two Layer - 4 ports - Transform Precoding Disabled
    
    for(int i=0; i<tW_UL(4).size(); i++)
      tW_UL(4)(i).set_size(4,2);
    
    tW_UL(4)(0) = 0.5*concat_horizontal(c(0),c(1));  tW_UL(4)(12) = 0.5*concat_horizontal(c(6),c(8)); 
    tW_UL(4)(1) = 0.5*concat_horizontal(c(0),c(2));  tW_UL(4)(13) = 0.5*concat_horizontal(c(6),c(9));
    tW_UL(4)(2) = 0.5*concat_horizontal(c(0),c(3));  tW_UL(4)(14) = (0.5/sqrt(2))*concat_horizontal(genCmat(1,1,1,1),c(14));
    tW_UL(4)(3) = 0.5*concat_horizontal(c(1),c(2));  tW_UL(4)(15) = (0.5/sqrt(2))*concat_horizontal(c(13),c(15));
    tW_UL(4)(4) = 0.5*concat_horizontal(c(1),c(3));  tW_UL(4)(16) = (0.5/sqrt(2))*concat_horizontal(c(16),c(18));
    tW_UL(4)(5) = 0.5*concat_horizontal(c(2),c(3));  tW_UL(4)(17) = (0.5/sqrt(2))*concat_horizontal(c(17),c(19));
    tW_UL(4)(6) = 0.5*concat_horizontal(c(4),c(11)); tW_UL(4)(18) = (0.5/sqrt(2))*concat_horizontal(c(20),c(22));
    tW_UL(4)(7) = 0.5*concat_horizontal(c(4),c(10)); tW_UL(4)(19) = (0.5/sqrt(2))*concat_horizontal(c(21),c(23));
    tW_UL(4)(8) = 0.5*concat_horizontal(c(7),c(8));  tW_UL(4)(20) = (0.5/sqrt(2))*concat_horizontal(c(24),c(26));
    tW_UL(4)(9) = 0.5*concat_horizontal(c(7),c(9));  tW_UL(4)(21) = (0.5/sqrt(2))*concat_horizontal(c(25),c(27));
    tW_UL(4)(10) = 0.5*concat_horizontal(c(5),c(11)); 
    tW_UL(4)(11) = 0.5*concat_horizontal(c(5),c(10)); 
    
    // ******************************************************  //
  
    // Three Layer - 4 ports - Transform Precoding Disabled    //
    for(int i=0; i<tW_UL(5).size(); i++)
      tW_UL(5)(i).set_size(4,3);
    
    tW_UL(5)(0) = concat_horizontal(tW_UL(4)(0), tW_UL(1)(2));  
    tW_UL(5)(1) = concat_horizontal(tW_UL(1)(4), tW_UL(1)(1));  tW_UL(5)(1) = concat_horizontal(tW_UL(5)(1), tW_UL(1)(3));
    tW_UL(5)(2) = tW_UL(5)(1); tW_UL(5)(2)(2,0)=-0.5;
    tW_UL(5)(3) = concat_horizontal(tW_UL(2)(12), tW_UL(2)(20));tW_UL(5)(3) = (1/sqrt(3))*concat_horizontal(tW_UL(5)(3), tW_UL(2)(14));
    tW_UL(5)(4) = concat_horizontal(tW_UL(2)(13), tW_UL(2)(21));tW_UL(5)(4) = (1/sqrt(3))*concat_horizontal(tW_UL(5)(4), tW_UL(2)(15));
    tW_UL(5)(5).set_col(0,tW_UL(5)(3).get_col(1));tW_UL(5)(5).set_col(1,tW_UL(5)(3).get_col(0)); tW_UL(5)(5).set_col(2,(1/sqrt(3))*tW_UL(2)(22).get_col(0));
    tW_UL(5)(6) = concat_horizontal(tW_UL(2)(21), tW_UL(2)(13));tW_UL(5)(6) = (1/sqrt(3))*concat_horizontal(tW_UL(5)(6), tW_UL(2)(23));
    // ******************************************************  //
    
	
    // Four Layer - 4 ports - Transform Precoding Disabled  //
	
    for(int i=0; i<tW_UL(6).size(); i++)
      tW_UL(6)(i).set_size(4,4);
    cmat temp;
    temp.set_size(4,2);
    tW_UL(6)(0) = concat_horizontal(tW_UL(5)(0), tW_UL(1)(3)); 
    tW_UL(6)(1).set_col(0,c(4).get_col(0));tW_UL(6)(1).set_col(1,c(5).get_col(0));tW_UL(6)(1).set_col(2,c(8).get_col(0));tW_UL(6)(1).set_col(3,c(9).get_col(0)); tW_UL(6)(1) = (0.5/sqrt(2))*tW_UL(6)(1);
    tW_UL(6)(2).set_col(0,c(6).get_col(0));tW_UL(6)(2).set_col(1,c(7).get_col(0));tW_UL(6)(2).set_col(2,c(10).get_col(0));tW_UL(6)(2).set_col(3,c(11).get_col(0)); tW_UL(6)(2) = (0.5/sqrt(2))*tW_UL(6)(2);
    tW_UL(6)(3).set_col(0,tW_UL(2)(12).get_col(0));tW_UL(6)(3).set_col(1,tW_UL(2)(20).get_col(0));tW_UL(6)(3).set_col(2,tW_UL(2)(14).get_col(0));tW_UL(6)(3).set_col(3,tW_UL(2)(22).get_col(0)); tW_UL(6)(3) = (0.5)*tW_UL(6)(3);
    tW_UL(6)(4).set_col(0,c(13).get_col(0));tW_UL(6)(4).set_col(1,c(21).get_col(0));tW_UL(6)(4).set_col(2,c(15).get_col(0));tW_UL(6)(4).set_col(3,c(23).get_col(0)); tW_UL(6)(4) = (0.25)*tW_UL(6)(4);
    
    // ******************************************************  //
    
    return tW_UL;
}

//****************************End of NR codebook related***********************************//


Array<cmat> Codebook::getCodebook(int nPorts,int nLayers,bool isDownlink)
{
  Array<cmat > codeBook;
  
  if(isDownlink)
  {
    switch(nPorts)
    {
      // written to avoid error with 1x1 NR Transmission
      case 1: 
        codeBook.set_size(1);
        codeBook(0)=cmat("1");
        break;
      case 2: 
        codeBook = TwoPortCodebookForAllLayers(nLayers-1);
        break;
      case 4: 
        codeBook = FourPortCodebookForAllLayers(nLayers-1);
        break;
      case 8: 
        codeBook = EightPortCodebookForAllLayers(nLayers-1);
        break;
      case 12: 
        codeBook = TwelvePortCodebookForAllLayers(nLayers-1);
        break;
      case 16: 
        codeBook = SixteenPortCodebookForAllLayers(nLayers-1);
        break;
      case 24: 
        codeBook = TwentyfourPortCodebookForAllLayers(nLayers-1);
        break;
      case 32:
          cout<<endl<<"For 32 Ports"<<endl;
        codeBook = ThirtytwoPortCodebookForAllLayers(nLayers-1);
        break;
      default:
        cout<<"[both:] Undefined #antenna ports in getCodebook(int nPorts,int nLayers,bool isDownlink) ";
        abort();
    }
  }
  else
  {
    switch(nPorts)
    {
      // written to avoid error with 1x1 NR Transmission
      case 1: 
        codeBook.set_size(1);
        codeBook(0)=cmat("1");
        break;
      case 2:
        codeBook = TwoPortCodebookForULAllLayers(nLayers-1);	
        break;
      case 4: 
        codeBook = FourPortCodebookForULAllLayers(nLayers-1);
        break;
//       case 8: 
//         codeBook = EightPortCodebookForAllLayers(nLayers-1);
//         break;
      default: 
        cout<<"[both:] Undefined #antenna ports in getCodebook(int nPorts,int nLayers,bool isDownlink);"<<endl;
        abort();
    }
  }
  return codeBook;
}
cmat Codebook::getRandomPrecoder(int nPorts,int nLayers)
{
  int pmi;  
  switch(nPorts)
  {
      case 1: return "1";
    case 2:
        pmi=randi(0,TwoPortCodebookForAllLayers(nLayers-1).length()-1);
        return(TwoPortCodebookForAllLayers(nLayers-1)(pmi));
    break;
    case 4:
//       if(isAltCodeBookEnabledFor4TXEnabled)
// 	return(AlternativeCodeBookFor4Port(nLayers-1)(pmi));
//       else
        pmi=randi(0,FourPortCodebookForAllLayers(nLayers-1).length()-1);
        return(FourPortCodebookForAllLayers(nLayers-1)(pmi));
        break;
    case 8:
        pmi=randi(0,EightPortCodebookForAllLayers(nLayers-1).length()-1);
        return(EightPortCodebookForAllLayers(nLayers-1)(pmi));
        break;
    case 12:
        pmi=randi(0,TwelvePortCodebookForAllLayers(nLayers-1).length()-1);
        return(TwelvePortCodebookForAllLayers(nLayers-1)(pmi));
        break; 
    case 16:
        pmi=randi(0,SixteenPortCodebookForAllLayers(nLayers-1).length()-1);
        return(SixteenPortCodebookForAllLayers(nLayers-1)(pmi));
        break;
    case 24:
        pmi=randi(0,TwentyfourPortCodebookForAllLayers(nLayers-1).length()-1);
        return(TwentyfourPortCodebookForAllLayers(nLayers-1)(pmi));
        break;
    case 32:
        pmi=randi(0,ThirtytwoPortCodebookForAllLayers(nLayers-1).length()-1);
        return(ThirtytwoPortCodebookForAllLayers(nLayers-1)(pmi));
        break;
    default:
      cout<<"[both:] error in getCodebook(int nPorts,int nLayers,int pmi) ";
      abort();
  }
};
cmat Codebook::getPrecoder(int nPorts,int nLayers,int pmi)
{
  switch(nPorts)
  {
      case 1: return "1";
    case 2:return(TwoPortCodebookForAllLayers(nLayers-1)(pmi));
    break;
    case 4:
//       if(isAltCodeBookEnabledFor4TXEnabled)
// 	return(AlternativeCodeBookFor4Port(nLayers-1)(pmi));
//       else
	return(FourPortCodebookForAllLayers(nLayers-1)(pmi));
    break;
    case 8:return(EightPortCodebookForAllLayers(nLayers-1)(pmi));
    break;
    case 12:return(TwelvePortCodebookForAllLayers(nLayers-1)(pmi));
    break; 
    case 16:return(SixteenPortCodebookForAllLayers(nLayers-1)(pmi));
    break;
    case 24:return(TwentyfourPortCodebookForAllLayers(nLayers-1)(pmi));
    break;
    case 32:return(ThirtytwoPortCodebookForAllLayers(nLayers-1)(pmi));
    break;
    default:
      cout<<"[both:] error in getCodebook(int nPorts,int nLayers,int pmi) ";
      abort();
  }
};
int Codebook::getPmiIndex(int nPorts,int nLayers,cmat precoder) 
{
  int toReturn = -1;
  switch(nPorts)
  {
    case 2:
      for(int pmi=0;pmi<TwoPortCodebookForAllLayers(nLayers-1).length();pmi++)
      {
	if(TwoPortCodebookForAllLayers(nLayers-1)(pmi)==precoder)
	  toReturn = pmi;
      }
	break;
    case 4:
      for(int pmi=0;pmi<FourPortCodebookForAllLayers(nLayers-1).length();pmi++)
	if(FourPortCodebookForAllLayers(nLayers-1)(pmi)==precoder)
	  toReturn = pmi;
	break;
    case 8:
      for(int pmi=0;pmi<EightPortCodebookForAllLayers(nLayers-1).length();pmi++)
	if(EightPortCodebookForAllLayers(nLayers-1)(pmi)==precoder)
	  toReturn = pmi;
	break;
    default:
      cout<<"[both:] error in getCodebook(int nPorts,int nLayers,int pmi) ";
      abort();
  }

  if(toReturn == -1)
  {
    cout<<"[both:] Precoder is not available in the codebook"<<endl;
    abort();
  }

  return toReturn;
};

void Codebook::getUnique8portCodebookIndices()
{
  unique8portCodebookIndices.set_length(8);
  for(int layer_cnt=0;layer_cnt<8;layer_cnt++)
  {
    ivec Indices;Indices.set_length(0);
    ivec num=getIntegers(0,EightPortCodebookForAllLayers(layer_cnt).length()-1);
    for(int i1=0;i1<(EightPortCodebookForAllLayers(layer_cnt).length());i1++)
    {
      if(find(num,i1)!=-1)
      {
	append(Indices,i1);
	for(int i2=0;i2<(EightPortCodebookForAllLayers(layer_cnt).length());i2++)
	{
	  if(i1!=i2)
	  {
	    cmat check = EightPortCodebookForAllLayers(layer_cnt)(i1).H()*EightPortCodebookForAllLayers(layer_cnt)(i2);
	    if(abs(trace(check))>0.99)
	    {
// 	      cout << "prec1 :" << EightPortCodebookForAllLayers(layer_cnt)(i1)<< endl;
// 	      cout << "prec2 :" << EightPortCodebookForAllLayers(layer_cnt)(i2)<< endl;
	      //cout << "diff :" << EightPortCodebookForAllLayers(layer_cnt)(i1)-EightPortCodebookForAllLayers(layer_cnt)(i2) << endl;
	      num=remove(num,i2);
	    }
	  }
	}
      }
    }
    unique8portCodebookIndices(layer_cnt)=Indices;
    //unique8portCodebookIndices(layer_cnt)=num;
  }
}

Array<cmat> Codebook::getSubSampledCodeBookForSubmode1(int nLayers)
{
  return(SubSampledCodeBookForSubmode1(nLayers-1));
}

Array<cmat> Codebook::getCodeBookToSelecti1(int nLayers)
{
  return(CodeBookToSelecti1(nLayers-1));
}

Array<cmat> Codebook::getSubSampledCodeBookForsubmode2(int nLayers)
{
  return(SubSampledCodeBookForsubmode2(nLayers-1));
}

CBParam_S Codebook::getCodebookParam(int nLayers)
{
  int layerNum =-1 , i2=0, h=0, v=0, g=0,p=0,n=0;
  if(dlCodeBookConfig.codeBookType != _NR_CODEBOOK_TYPE_I_MULTIPANEL_){
      return getParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, i2, h, v, g);
  }
  return getMultipanelParam(dlCodeBookConfig.codeBookMode, nLayers, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, dlCodeBookConfig.Ng, h, v, g,p,n);
}

Array<Type1HalfBeamWithi1> Codebook::getType1HalfBeam()
{
    if(!type1halfBeamswithi1.length())
    {
        NRCodeBookMode CBmode = _NR_CODEBOOK_MODE_1_;
        int layerNum = -1,h=0, v=0, g=0, i2=0,halfbeamIndx; // for preliminary paramaters
        CBParam_S CBparam = getParam(CBmode, 1, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, i2, h, v, g);    // This is to get only CodebookSizeperLayer,i11Size, i12size: so layerNum is kept 0 intentionally (its a work around)
    
        type1halfBeamswithi1.set_size(CBparam.i11Size*CBparam.i12Size);
        for(h=0,halfbeamIndx=0;h<CBparam.i11Size;h++)
            for(v=0;v<CBparam.i12Size;v++,halfbeamIndx++)
            {
                CBParam_S tempCBParam = getParam(CBmode, 1, layerNum, dlCodeBookConfig.N1, dlCodeBookConfig.N2, i2, h, v, g);
                cmat tVlm = getVlm(tempCBParam.i11,tempCBParam.i12,dlCodeBookConfig.N1,dlCodeBookConfig.N2,dlCodeBookConfig.O1,dlCodeBookConfig.O2);
                type1halfBeamswithi1(halfbeamIndx).i11=h;
                type1halfBeamswithi1(halfbeamIndx).i12=v;
                type1halfBeamswithi1(halfbeamIndx).i2Size=CBparam.i2Size;
                type1halfBeamswithi1(halfbeamIndx).Halfbeam=tVlm;
            }
    }
    return type1halfBeamswithi1;
}

Array<cmat> generateCodebookForDLTwoPorts(int nLayers)
{
  Array<cmat> W;
  const dComplex R = dComplex(1.0,0.0);
  const dComplex I = dComplex(0.0,1.0);
  #ifdef USING_CALIBRATION 
  if(traceValuePerTraceName["RITesting"]=="true")
  {
    switch(nLayers)
    {
      case 1:
      {
	W.set_size(2);
	for(int i=0; i<2; i++)
	  W(i).set_size(2,nLayers);
	
	W(0)(0,0)=R/SQRT_2;	W(0)(1,0)=R/SQRT_2;
	W(1)(0,0)=R/SQRT_2;	W(1)(1,0)=-R/SQRT_2;
      }
      break;
      
      case 2:
      {
	W.set_size(2);
	for(int i=0; i<2; i++)
	  W(i).set_size(2,nLayers);

	W(0)(0,0)=R/SQRT_2; W(0)(0,1)=0;   W(0)(1,0)=0;   W(0)(1,1)=R/SQRT_2;
	W(1)(0,0)=R/2.0;      W(1)(0,1)=R/2.0; W(1)(1,0)=R/2.0; W(1)(1,1)=-R/2.0;

      }break;
      default: cout<<"[both:] Undefined #Layers .... exiting in generateCodebookForTwoPorts();"<<endl;abort();
    }
  }
  else
  {
#endif
    switch(nLayers)
    {
      case 1:
      {
	W.set_size(4);
	for(int i=0; i<4; i++)
	  W(i).set_size(2,nLayers);
	
	W(0)(0,0)=R/SQRT_2;	W(0)(1,0)=R/SQRT_2;
	W(1)(0,0)=R/SQRT_2;	W(1)(1,0)=-R/SQRT_2;
	W(2)(0,0)=R/SQRT_2;	W(2)(1,0)=I/SQRT_2;
	W(3)(0,0)=R/SQRT_2;	W(3)(1,0)=-I/SQRT_2;
      }
      break;
      
      case 2:
      {
	W.set_size(3);
	for(int i=0; i<3; i++)
	  W(i).set_size(2,nLayers);
	
	W(0)(0,0)=R/SQRT_2; W(0)(0,1)=0;   W(0)(1,0)=0;   W(0)(1,1)=R/SQRT_2;
	W(1)(0,0)=R/2.0;      W(1)(0,1)=R/2.0; W(1)(1,0)=R/2.0; W(1)(1,1)=-R/2.0;
	W(2)(0,0)=R/2.0;      W(2)(0,1)=R/2.0; W(2)(1,0)=I/2.0; W(2)(1,1)=-I/2.0;
	
      }break;
      default: cout<<"[both:] Undefined #Layers .... exiting in generateCodebookForTwoPorts();"<<endl;abort();
    }
#ifdef USING_CALIBRATION 
  }
#endif
  return(W);
}

Array<cmat> generateCodebookForDLFourPorts(int nLayers)
{
  Array<cmat> W(16);
  if(nLayers>0 && nLayers<5)
  {
    for(int i=0; i<16; i++)
      W(i).set_size(4,nLayers);
    cvec tempCvec;
    ivec tempIvec;
    Array<cmat> uMatrix;
    Array<ivec> pickLocation;
    cmat tempMatrix,innerProduct;
    
    uMatrix = uMatrixGeneration(4);
    
    pickLocation = getHouseHolderLocations(nLayers);
    for (int iIndex = 0;iIndex < 16;iIndex ++)
    {
      tempIvec = pickLocation(iIndex);
      innerProduct = (hermitian_transpose(uMatrix(iIndex)) * uMatrix(iIndex));
      W(iIndex) = zeros_c(4,nLayers);
      tempMatrix = eye_c(4) - 2.0 * ((uMatrix(iIndex) * hermitian_transpose(uMatrix(iIndex))) / innerProduct(0,0));
      for (int iCol = 0;iCol < nLayers;iCol ++)
      {
	tempCvec = tempMatrix.get_col(tempIvec(iCol));
	W(iIndex).set_col(iCol,tempCvec);
      }

      W(iIndex) = (1.0/sqrt(nLayers))*W(iIndex);
    }
  }
  else
  {
    cout<<"[both:] Undefined #Layers .... exiting in generateCodebookForFourPorts();"<<endl;abort();
  }
  
  return(W);
}

//Function to generate alternate codebook for DL Four Port, as per release 12

Array<cmat> generateAlternativeCodebookForDLFourPorts(int nLayers)
{
  int CBsize;
  Array<cmat> W;
  
  switch (nLayers)
  {
    case 1 :
      CBsize = 16*16;
      W.set_size(CBsize);
      for(int i=0; i<CBsize; i++)
	W(i).set_size(4,nLayers);
      for (int i1=0, i=0;i1<16;i1++)
	for (int i2=0;(i2<16 && i<CBsize);i2++,i++)
	  W(i) = getAlt4TXW1(i1, i2);
	break;
      
    case 2 :
      CBsize = 16*16;
      W.set_size(CBsize);
      for(int i=0; i<CBsize; i++)
	W(i).set_size(4,nLayers);
      for (int i1=0, i=0;i1<16;i1++)
	for (int i2=0;(i2<16 && i<CBsize);i2++,i++)
	  W(i) = getAlt4TXW2(i1, i2);
	
      break;
      
    case 3 :
    case 4 :
      CBsize = 16*1;
      W.set_size(CBsize);
      W = generateCodebookForDLFourPorts(nLayers);
      break;
    default :
      cout<<"[both:] nLayers is incorrect ... exiting in generateAlternativeCodebookForDLFourPorts()"<<endl;abort();
      break;
      
  }
  return(W);
}


Array<cmat> generateCodebookForDLEightPorts(int nLayers)
{
  int CBsize;
  Array<cmat> W;
  
  switch (nLayers)
  {
    case 1 :
      CBsize = 16*16;
      W.set_size(CBsize);
      for(int i=0; i<CBsize; i++)
	W(i).set_size(8,nLayers);
      for (int i1=0, i=0;i1<16;i1++)
	for (int i2=0;(i2<16 && i<CBsize);i2++,i++)
	  W(i) = getW1(i1, i2);break;
	
    case 2 :
      CBsize = 16*16;
      W.set_size(CBsize);
      for(int i=0; i<CBsize; i++)
	W(i).set_size(8,nLayers);
      for (int i1=0, i=0;i1<16;i1++)
	for (int i2=0;(i2<16 && i<CBsize);i2++,i++)
	  W(i) = getW2(i1, i2);break;
	
    case 3 :
      CBsize = 16*4;
      W.set_size(CBsize);
      for(int i=0; i<CBsize; i++)
	W(i).set_size(8,nLayers);
      for (int i1=0, i=0;i1<4;i1++)
	for (int i2=0;(i2<16 && i<CBsize);i2++,i++)
	  W(i) = getW3(i1, i2);break;
	
    case 4 :
      CBsize = 8*4;
      W.set_size(CBsize);
      for(int i=0; i<CBsize; i++)
	W(i).set_size(8,nLayers);
      for (int i1=0, i=0;i1<4;i1++)
	for (int i2=0;(i2<8 && i<CBsize);i2++,i++)
	  W(i) = getW4(i1, i2);break;
	
    case 5 :
      CBsize = 4;
      W.set_size(CBsize);
      for(int i=0; i<CBsize; i++)
	W(i).set_size(8,nLayers);
      for (int i1=0;i1<CBsize;i1++)
	W(i1) = getW5(i1);break;
      
    case 6 :
      CBsize = 4;
      W.set_size(CBsize);
      for(int i=0; i<CBsize; i++)
	W(i).set_size(8,nLayers);
      for (int i1=0;i1<CBsize;i1++)
	W(i1) = getW6(i1);break;
      
    case 7 :
      CBsize = 4;
      W.set_size(CBsize);
      for(int i=0; i<CBsize; i++)
	W(i).set_size(8,nLayers);
      for (int i1=0;i1<CBsize;i1++)
	W(i1) = getW7(i1);break;
      
    case 8 :
      CBsize = 1;
      W.set_size(CBsize);
      for(int i=0; i<CBsize; i++)
	W(i).set_size(8,nLayers);
      for (int i1=0;i1<CBsize;i1++)
	W(i1) = getW8();break;
      
    default :
      cout<<"[both:] nLayers is incorrect ... exiting in generateCodebookForEightPorts()"<<endl;abort();
      
  }
  return(W);
  
}


ivec getCodeBookIndicesForMode2_1(int nLayers,int indi1) //! refer table 7.2.2-1F in 36.213v11
{
  ivec indi2;
  switch(nLayers)
  {
    case 1:indi2=getIntegers(0,15);break;
    case 2:indi2=getIntegers(0,6,2);break;
    case 3:indi2=ivec("2 3 10 11");break;
    case 4:indi2=getIntegers(0,6,2);break;
    case 5:
    case 6:
    case 7:
    case 8:indi2=ivec("0");break;
    default:{cout<<"[both:] Invalid nLayers in getSubSampledCodeBookForMode2_1"<<endl;abort();}      
  }
  return(getCodeBookIndicesForEightPorts(nLayers,indi1,indi2));
}
Array<cmat> getSubSampledCodeBookForMode2_1(int nLayers,int indi1)
{
  ivec ind=getCodeBookIndicesForMode2_1(nLayers,indi1);
  return(getCodeBookSubsetForEightPorts(nLayers,ind));
}
ivec getCodeBookIndicesToSelecti1(int nLayers)  //! few repeated precoders are excluded
{
  ivec validi1,validi2;
  ivec precInd(0);

  switch(nLayers)
  {
    case 1:validi1=getIntegers(0,14,2);validi2=getIntegers(0,15,1);break;
    case 2:validi1=getIntegers(0,15,1);validi2=ivec("0 1 2 3 8 9 10 11 12 13 14 15");break;
    case 3:validi1=getIntegers(0,3,1);validi2=getIntegers(0,15,1);break;
    case 4:validi1=getIntegers(0,3,1);validi2=getIntegers(0,7);break;
    case 5:
    case 6:
    case 7:validi1=getIntegers(0,3),validi2=ivec("0");break;
    case 8:validi1=ivec("0");validi2=ivec("0");break;
    default:{cout<<"[both:] Invalid nLayers in getCodeBookIndicesForSubMode2"<<endl;abort();}
  }
  return(getCodeBookIndicesForEightPorts(nLayers,validi1,validi2));
}

int getShortTermPMIInd(int nLayers,int precInd)
{
  ivec LongTermIndex;
  ivec ShortTermIndex;
  generateLongTermAndShortTermInd(nLayers,LongTermIndex,ShortTermIndex);
  return(ShortTermIndex(precInd));
}
Array<cmat> getCodeBookForSpecifiedi1(int nLayers,int Indi1)
{
  ivec Ind=getCodeBookIndicesForEightPorts(nLayers,to_ivec(Indi1));
  return(getCodeBookSubsetForEightPorts(nLayers,Ind));
}
ivec getCodeBookIndicesForSubMode2(int nLayers) //! refer Table 7.2.2-1D in 36.213v11
{
  ivec validi1,validi2;
  ivec precInd(0);

  switch(nLayers)
  {
    case 1:validi1=getIntegers(0,14,2);validi2=ivec("0 2");break;
    case 2:validi1=getIntegers(0,14,2);validi2=ivec("0 1");break;
    case 3:validi1=ivec("0 2");validi2=ivec("0 1 2 3 8 9 10 11");break;
    case 4:validi1=ivec("0 1");validi2=getIntegers(0,7);break;
    case 5:
    case 6:
    case 7:validi1=getIntegers(0,3),validi2=ivec("0");break;
    case 8:validi1=ivec("0");validi2=ivec("0");break;
    default:{cout<<"[both:] Invalid nLayers in getCodeBookIndicesForSubMode2"<<endl;abort();}
  }
  return(getCodeBookIndicesForEightPorts(nLayers,validi1,validi2));
}

void generateLongTermAndShortTermInd(int nLayers,ivec& LongTermIndex,ivec& ShortTermIndex)
{
  int CBsize;
  switch (nLayers)
  {
    case 1 :
      CBsize = 16*16;
      LongTermIndex.set_length(CBsize);
      ShortTermIndex.set_length(CBsize);
      for(int i1=0, i=0;i1<16;i1++)
	for (int i2=0;(i2<16 && i<CBsize);i2++,i++)
	{
	  LongTermIndex(i)=i1;
	  ShortTermIndex(i)=i2;	  
	}
	break;
      
    case 2 :
      CBsize = 16*16;
      LongTermIndex.set_length(CBsize);
      ShortTermIndex.set_length(CBsize);
      for (int i1=0, i=0;i1<16;i1++)
	for (int i2=0;(i2<16 && i<CBsize);i2++,i++)
	{
	  LongTermIndex(i)=i1;
	  ShortTermIndex(i)=i2;	  
	}
	break;
    case 3 :
      CBsize = 16*4;
      LongTermIndex.set_length(CBsize);
      ShortTermIndex.set_length(CBsize);
      for (int i1=0, i=0;i1<4;i1++)
	for (int i2=0;(i2<16 && i<CBsize);i2++,i++)
	{
	  LongTermIndex(i)=i1;
	  ShortTermIndex(i)=i2;	  
	}
	break;
      
    case 4 :
      CBsize = 8*4;
      LongTermIndex.set_length(CBsize);
      ShortTermIndex.set_length(CBsize);
      for (int i1=0, i=0;i1<4;i1++)
	for (int i2=0;(i2<8 && i<CBsize);i2++,i++)
	{
	  LongTermIndex(i)=i1;
	  ShortTermIndex(i)=i2;	  
	}
	break;
      
    case 5 :
      CBsize = 4;
      LongTermIndex.set_length(CBsize);
      ShortTermIndex.set_length(CBsize);
      for (int i1=0;i1<CBsize;i1++)
      {
	LongTermIndex(i1)=i1;
	ShortTermIndex(i1)=0;	
      }
      break;
      
    case 6 :
      CBsize = 4;
      LongTermIndex.set_length(CBsize);
      ShortTermIndex.set_length(CBsize);
      for (int i1=0;i1<CBsize;i1++)
      {
	LongTermIndex(i1)=i1;
	ShortTermIndex(i1)=0;	  
      }
      break;
      
    case 7 :
      CBsize = 4;
      LongTermIndex.set_length(CBsize);
      ShortTermIndex.set_length(CBsize);
      for (int i1=0;i1<CBsize;i1++)
      {
	LongTermIndex(i1)=i1;
	ShortTermIndex(i1)=0;
      }
      break;
      
    case 8 :
      CBsize = 1;
      LongTermIndex.set_length(CBsize);
      ShortTermIndex.set_length(CBsize);
      for (int i1=0;i1<CBsize;i1++)
      {
	LongTermIndex(i1)=i1;
	ShortTermIndex(i1)=0;
      }
      break;
      
    default :
      cout<<"[both:] nLayers is incorrect ... exiting in generateCodebookForDLEightPorts()"<<endl;abort();
      
  }  
}

int getLongTermPMIInd(int nLayers,int precInd)
{
  ivec LongTermIndex;
  ivec ShortTermIndex;
  generateLongTermAndShortTermInd(nLayers,LongTermIndex,ShortTermIndex);
  return(LongTermIndex(precInd));
}
ivec getCodeBookIndicesForEightPorts(int nLayers,ivec Indi1,ivec Indi2)
{
  ivec precInd(0);
  int maxLengthi2=0;
 
  if(Indi2(0)==-1)
  {
    
    if((1<=nLayers) && (nLayers<=3))
      Indi2=getIntegers(0,15);
    else if(nLayers==4)
      Indi2=getIntegers(0,7);
    else if((5<=nLayers) && (nLayers<=8))
      Indi2=ivec("0");
    else
    {cout<<"[both:] Invalid nlayers in getCodeBookIndices"<<endl;abort();}
  }
  if((1<=nLayers) && (nLayers<=3))
      maxLengthi2=16;
    else if(nLayers==4)
      maxLengthi2=8;   
    else if((5<=nLayers) && (nLayers<=8))
      maxLengthi2=1;   
    else
    {cout<<"[both:] Invalid nlayers in getCodeBookIndices"<<endl;abort();}
  
  for(int cnt1=0;cnt1<Indi1.length();cnt1++)
    for(int cnt2=0;cnt2<Indi2.length();cnt2++)
      append(precInd,(Indi1(cnt1)*maxLengthi2)+Indi2(cnt2));

    return(precInd);
}
ivec getCodeBookIndicesForEightPorts(int nLayers,int Indi1,ivec Indi2)
{
  ivec precInd(0);
  int maxLengthi2=0;
   if(Indi2(0)==-1)
  {
    
    if((1<=nLayers) && (nLayers<=3))
      Indi2=getIntegers(0,15);
    else if(nLayers==4)
      Indi2=getIntegers(0,7);
    else if((5<=nLayers) && (nLayers<=8))
      Indi2=ivec("0");
    else
    {cout<<"[both:] Invalid nlayers in getCodeBookIndices"<<endl;abort();}
  }
  if((1<=nLayers) && (nLayers<=3))
    maxLengthi2=16;
  else if(nLayers==4)
    maxLengthi2=8;   
  else if((5<=nLayers) && (nLayers<=8))
    maxLengthi2=1;   
  else
  {cout<<"[both:] Invalid nlayers in getCodeBookIndices"<<endl;abort();}
  
  for(int cnt2=0;cnt2<Indi2.length();cnt2++)
      append(precInd,(Indi1*maxLengthi2)+Indi2(cnt2));
   
  return(precInd);
}

Array<cmat> getCodeBookSubsetForEightPorts(int nLayers,ivec indices)
{
  Array<cmat> codeBook = generateCodebookForDLEightPorts(nLayers);
  Array<cmat> subSampledCodeBook(0);
  for(int cnt=0;cnt<indices.length();cnt++)
    append(subSampledCodeBook,codeBook(indices(cnt)));
  return(subSampledCodeBook);
  
}
ivec getCodeBookIndicesToSelecti1ForSubmode1(int nLayers) //! refer 7.2.2-1E in 36.213v11
{
  ivec validi1,validi2;
  ivec precInd(0);
  
  switch(nLayers)
  {
    case 1:validi1=getIntegers(0,14,2);validi2=getIntegers(0,15,1);break;
    case 2:validi1=getIntegers(0,14,2);validi2=getIntegers(0,15,1);break;
    case 3:validi1=getIntegers(0,2,2);validi2=getIntegers(0,15,1);break;
    case 4:validi1=getIntegers(0,2,2);validi2=getIntegers(0,7);break;
    case 5:
    case 6:
    case 7:validi1=getIntegers(0,2,2);validi2=ivec("0");break;
    case 8:validi1=ivec("0");validi2=ivec("0");break;
    default:{cout<<"[both:] Invalid nLayers in getCodeBookIndicesForSubMode2"<<endl;abort();}
  }
  return(getCodeBookIndicesForEightPorts(nLayers,validi1,validi2));
}

ivec getValidi1Indices(int nLayers)
{
  ivec validi1;
  
  switch(nLayers)
  {
    case 1:validi1=getIntegers(0,14,2);break;
    case 2:validi1=getIntegers(0,14,2);break;
    case 3:validi1=getIntegers(0,2,2);break;
    case 4:validi1=getIntegers(0,2,2);break;
    case 5:
    case 6:
    case 7:validi1=getIntegers(0,2,2);break;
    case 8:validi1=ivec("0");break;
    default:{cout<<"[both:] Invalid nLayers in getValidi1Indices"<<endl;abort();}
  }
  return validi1;
}

Array<cmat> uMatrixGeneration(int nPorts)
{
  Array<cmat> uMatrix(16);
  for(int i=0; i<16; i++)
    uMatrix(i).set_size(nPorts,1);
  
  
  const dComplex R = dComplex(1.0,0.0);
  const dComplex I = dComplex(0.0,1.0);
  
  
  uMatrix(0)(0,0) = R;uMatrix(0)(1,0) = -R;uMatrix(0)(2,0) = -R;uMatrix(0)(3,0) = -R;
  uMatrix(1)(0,0) = R;uMatrix(1)(1,0) = -I;uMatrix(1)(2,0) = R;uMatrix(1)(3,0) = I;
  uMatrix(2)(0,0) = R;uMatrix(2)(1,0) = R;uMatrix(2)(2,0) = -R;uMatrix(2)(3,0) = R;
  uMatrix(3)(0,0) = R;uMatrix(3)(1,0) = I;uMatrix(3)(2,0) = R;uMatrix(3)(3,0) = -I;
  uMatrix(4)(0,0) = R;uMatrix(4)(1,0) = (-R - I) / SQRT_2;uMatrix(4)(2,0) = -I;uMatrix(4)(3,0) = (R - I) / SQRT_2;
  uMatrix(5)(0,0) = R;uMatrix(5)(1,0) = (R - I) / SQRT_2;uMatrix(5)(2,0) = I;uMatrix(5)(3,0) = (-R - I) / SQRT_2;
  uMatrix(6)(0,0) = R;uMatrix(6)(1,0) = (R + I) / SQRT_2;uMatrix(6)(2,0) = -I;uMatrix(6)(3,0) = (-R + I) / SQRT_2;
  uMatrix(7)(0,0) = R;uMatrix(7)(1,0) = (-R + I) / SQRT_2;uMatrix(7)(2,0) = I;uMatrix(7)(3,0) = (R + I) / SQRT_2;
  uMatrix(8)(0,0) = R;uMatrix(8)(1,0) = -R;uMatrix(8)(2,0) = R;uMatrix(8)(3,0) = R;
  uMatrix(9)(0,0) = R;uMatrix(9)(1,0) = -I;uMatrix(9)(2,0) = -R;uMatrix(9)(3,0) = -I;
  uMatrix(10)(0,0) = R;uMatrix(10)(1,0) = R;uMatrix(10)(2,0) = R;uMatrix(10)(3,0) = -R;
  uMatrix(11)(0,0) = R;uMatrix(11)(1,0) = I;uMatrix(11)(2,0) = -R;uMatrix(11)(3,0) = I;
  uMatrix(12)(0,0) = R;uMatrix(12)(1,0) = -R;uMatrix(12)(2,0) = -R;uMatrix(12)(3,0) = R;
  uMatrix(13)(0,0) = R;uMatrix(13)(1,0) = -R;uMatrix(13)(2,0) = R;uMatrix(13)(3,0) = -R;
  uMatrix(14)(0,0) = R;uMatrix(14)(1,0) = R;uMatrix(14)(2,0) = -R;uMatrix(14)(3,0) = -R;
  uMatrix(15)(0,0) = R;uMatrix(15)(1,0) = R;uMatrix(15)(2,0) = R;uMatrix(15)(3,0) = R;
  
  return(uMatrix);
}


Array<ivec> getHouseHolderLocations(int nLayers)
{
  Array<ivec> locationPick;
  locationPick.set_size(16);
  
  switch (nLayers)
  {
    case 1 :
      
      for (int iIndex = 0;iIndex < locationPick.size();iIndex ++)
      {
	locationPick(iIndex) = "0";
      }
      
      break;
      
    case 2 :
      
      locationPick(0) = "0 3";
      locationPick(1) = "0 1";
      locationPick(2) = "0 1";
      locationPick(3) = "0 1";
      locationPick(4) = "0 3";
      locationPick(5) = "0 3";
      locationPick(6) = "0 2";
      locationPick(7) = "0 2";
      locationPick(8) = "0 1";
      locationPick(9) = "0 3";
      locationPick(10) = "0 2";
      locationPick(11) = "0 2";
      locationPick(12) = "0 1";
      locationPick(13) = "0 2";
      locationPick(14) = "0 2";
      locationPick(15) = "0 1";
      break;
      
    case 3 :
      
      locationPick(0) = "0 1 3";
      locationPick(1) = "0 1 2";
      locationPick(2) = "0 1 2";
      locationPick(3) = "0 1 2";
      locationPick(4) = "0 1 3";
      locationPick(5) = "0 1 3";
      locationPick(6) = "0 2 3";
      locationPick(7) = "0 2 3";
      locationPick(8) = "0 1 3";
      locationPick(9) = "0 2 3";
      locationPick(10) = "0 1 2";
      locationPick(11) = "0 2 3";
      locationPick(12) = "0 1 2";
      locationPick(13) = "0 1 2";
      locationPick(14) = "0 1 2";
      locationPick(15) = "0 1 2";
      break;
      
    case 4 :
      
      locationPick(0) = "0 1 2 3";
      locationPick(1) = "0 1 2 3";
      locationPick(2) = "2 1 0 3";
      locationPick(3) = "2 1 0 3";
      locationPick(4) = "0 1 2 3";
      locationPick(5) = "0 1 2 3";
      locationPick(6) = "0 2 1 3";
      locationPick(7) = "0 2 1 3";
      locationPick(8) = "0 1 2 3";
      locationPick(9) = "0 1 2 3";
      locationPick(10) = "0 2 1 3";
      locationPick(11) = "0 2 1 3";
      locationPick(12) = "0 1 2 3";
      locationPick(13) = "0 2 1 3";
      locationPick(14) = "2 1 0 3";
      locationPick(15) = "0 1 2 3";
      break;
    default :
      cout<<"[both:] Undefined #layers ..... exiting in houseHolderLocationGeneration()"<<endl;abort();
  }
  
  return locationPick;
}

cmat getW1(int i1, int i2)
{
  cmat phi(1,1);
  cmat v(4,1), W(8,1);
  phi = getPhi(i2 % 4);
  v = getv(2*i1+floor(i2/4.0));
  W = concat_vertical(v, v*phi);
  W =W/sqrt(8);
  return(W);
}

cmat getAlt4TXW1(int i1, int i2)
{
  cmat phi_dash(1,1);
  cmat v_dash(2,1), W(4,1);
  
  phi_dash = getPhi_dash(floor_i(i2/4)*2 + 8*(i2 % 4));
  v_dash = getv_dash(i1+floor_i(i2/4)*8);
  W = concat_vertical(v_dash, v_dash*phi_dash);
  W =W/pow(sqrt(abs(det(W.hermitian_transpose()*W))), 1.0/W.cols());
  
  return(W);
}

cmat getW2(int i1, int i2)
{
  int m, mp, n;
  cmat phi(1,1);
  cmat v1(4,1), v2(4,1), W1(8,1), W2(8,1), W(8,2);
  if (i2<8)
  {
    m = 2*i1+(floor(i2/2.0)); 
    mp=2*i1+(floor(i2/2.0));    
  }
  else
  {
    m = 2*i1+((int)(floor(i2/2.0))%2); 
    if (i2>=8 && i2<=11)
      mp=m+1;  
    else //if (i2>=12 && i2<=15) 
      mp=2*i1+3;
  }
  n=(i2%2);
  
  phi = getPhi(n);
  v1 = getv(m);
  v2 = getv(mp);
  W1 = concat_vertical(v1, v1*phi);
  W2 = concat_vertical(v2, -v2*phi);
  W = concat_horizontal(W1, W2);
  W = W/4;

  return(W);
}

cmat getAlt4TXW2(int i1, int i2)
{
  int m, mp;
  cmat phi(1,1);
  cmat v_dashM(2,1),v_dashMP(2,1);
  cmat v1(2,1), v2(2,1), W1(4,1), W2(4,1), W(4,2);
  
  if(i2<8)
  {
    m = mp = i1+floor_i(i2/2)*8;
  }
  else if(i2 < 12)
  {
    m = i1+floor_i((i2%8)/2)*8;
    mp = i1+floor_i((i2%8)/2)*8+8;
  }
  else
  {
    if(i2>11 && i2 < 14)
    {
      m = i1;
      mp = i1+24;
    }
    else
    {
      m = i1+8;
      mp = i1+24;
    }
  }
  
  phi = getPhi(i2 % 2);
  v_dashM = getv_dash(m);
  v_dashMP = getv_dash(mp);
  W1 = concat_vertical(v_dashM, v_dashM*phi);
  W2 = concat_vertical(v_dashMP, -v_dashMP*phi);
  W = concat_horizontal(W1, W2);
  W =W/pow(sqrt(abs(det(W.hermitian_transpose()*W))), 1.0/W.cols());
  
  return(W);
}


cmat getW3(int i1, int i2)
{
  int m, mp, mpp, sign;
  cmat v1(4,1), v2(4,1), v3(4,1), W1(8,1), W2(8,1), W3(8,1), W(8,3);
  
  switch(((int)(floor(i2/2.0))%2))
  {
    case 0:
      m=8*i1+((int)(i2/2.0))+8*(i2%2);
      mp=8*i1+((int)(i2/2.0));
      mpp=8*i1+((int)(i2/2.0))+8;
      sign=-1;break;
      
    default://case 1:
      m=8*i1+((int)((i2-2)/2.0))+8*(i2%2);
      mp=8*i1+((int)((i2-2)/2.0))+8*((i2+1)%2);
      mpp=8*i1+((int)((i2-2)/2.0))+8*((i2+1)%2);
      sign=1;break;
  }
  
  v1 = getv(m);
  v2 = getv(mp);
  v3 = getv(mpp);
  W1 = concat_vertical(v1, v1);
  W2 = concat_vertical(v2, sign*v2);
  W3 = concat_vertical(v3, -v3);
  W = concat_horizontal(W1, concat_horizontal(W2, W3));
  W =W/sqrt(24);
  
  return(W);
}

cmat getW4(int i1, int i2)
{
  int m, mp, n;
  cmat phi(1,1), v1(4,1), v2(4,1), W1(8,1), W2(8,1), W3(8,1), W4(8,1), W(8,4);
  
  m=8*i1+2*floor(i2/2.0);
  mp=m+8;
  n=(i2%2);
  
  v1 = getv(m);
  v2 = getv(mp);
  phi = getPhi(n);
  W1 = concat_vertical(v1, v1*phi);
  W2 = concat_vertical(v2, v2*phi);
  W3 = concat_vertical(v1, -v1*phi);
  W4 = concat_vertical(v2, -v2*phi);
  W = concat_horizontal(concat_horizontal(W1, W2), concat_horizontal(W3, W4));
  W =W/sqrt(32);
  
  return(W);
}

cmat getW5(int i1)
{
  int m1, m2, m3;
  cmat v1(4,1), v2(4,1), v3(4,1), W1(8,1), W2(8,1), W3(8,1), W4(8,1), W5(8,1), W(8,5);
  
  m1=2*i1;
  m2=2*i1+8;
  m3=2*i1+16;
  
  v1 = getv(m1);
  v2 = getv(m2);
  v3 = getv(m3);
  W1 = concat_vertical(v1, v1);
  W2 = concat_vertical(v1, -v1);
  W3 = concat_vertical(v2, v2);
  W4 = concat_vertical(v2, -v2);
  W5 = concat_vertical(v3, v3);
  W = concat_horizontal(concat_horizontal(concat_horizontal(W1, W2), concat_horizontal(W3, W4)), W5);
  W =W/sqrt(40);
  
  return(W);
}

cmat getW6(int i1)
{
  int m1, m2, m3;
  cmat v1(4,1), v2(4,1), v3(4,1), W1(8,1), W2(8,1), W3(8,1), W4(8,1), W5(8,1), W6(8,1), W(8,6);
  
  m1=2*i1;
  m2=2*i1+8;
  m3=2*i1+16;
  
  v1 = getv(m1);
  v2 = getv(m2);
  v3 = getv(m3);
  W1 = concat_vertical(v1, v1);
  W2 = concat_vertical(v1, -v1);
  W3 = concat_vertical(v2, v2);
  W4 = concat_vertical(v2, -v2);
  W5 = concat_vertical(v3, v3);
  W6 = concat_vertical(v3, -v3);
  W = concat_horizontal(concat_horizontal(concat_horizontal(concat_horizontal(W1, W2), concat_horizontal(W3, W4)), W5), W6);
  W =W/sqrt(48);
  
  return(W);
}

cmat getW7(int i1)
{
  int m1, m2, m3, m4;
  cmat v1(4,1), v2(4,1), v3(4,1), v4(4,1), W1(8,1), W2(8,1), W3(8,1), W4(8,1), W5(8,1), W6(8,1), W7(8,1), W(8,7);
  
  m1=2*i1;
  m2=2*i1+8;
  m3=2*i1+16;
  m4=2*i1+24;
  
  v1 = getv(m1);
  v2 = getv(m2);
  v3 = getv(m3);
  v4 = getv(m4);
  W1 = concat_vertical(v1, v1);
  W2 = concat_vertical(v1, -v1);
  W3 = concat_vertical(v2, v2);
  W4 = concat_vertical(v2, -v2);
  W5 = concat_vertical(v3, v3);
  W6 = concat_vertical(v3, -v3);
  W7 = concat_vertical(v4, v4);
  W = concat_horizontal(concat_horizontal(concat_horizontal(concat_horizontal(concat_horizontal(W1, W2), concat_horizontal(W3, W4)), W5), W6), W7);
  W =W/sqrt(56);
  
  return(W);
}

cmat getW8()
{
  int m1, m2, m3, m4;
  cmat v1(4,1), v2(4,1), v3(4,1), v4(4,1), W1(8,1), W2(8,1), W3(8,1), W4(8,1), W5(8,1), W6(8,1), W7(8,1), W8(8,1), W(8,8);
  
  m1=0;
  m2=8;
  m3=16;
  m4=24;
  
  v1 = getv(m1);
  v2 = getv(m2);
  v3 = getv(m3);
  v4 = getv(m4);
  W1 = concat_vertical(v1, v1);
  W2 = concat_vertical(v1, -v1);
  W3 = concat_vertical(v2, v2);
  W4 = concat_vertical(v2, -v2);
  W5 = concat_vertical(v3, v3);
  W6 = concat_vertical(v3, -v3);
  W7 = concat_vertical(v4, v4);
  W8 = concat_vertical(v4, -v4);
  W = concat_horizontal(concat_horizontal(concat_horizontal(concat_horizontal(concat_horizontal(concat_horizontal(W1, W2), concat_horizontal(W3, W4)), W5), W6), W7), W8);
  W =W/8;

  return(W);
}

cmat getPhi(int pIndex)
{
  cmat phi(1,1);
  phi = complex< double >(cos(pi*pIndex/2.0),sin(pi*pIndex/2.0));
  
  return(phi);
}

cmat getPhi_dash(int pIndex)
{
  cmat phi_dash(1,1);
  phi_dash = complex< double >(cos(2.0*pi*pIndex/32.0),sin(2.0*pi*pIndex/32.0));
  
  return(phi_dash);
}

cmat getv(int vIndex)
{
  cmat v(4,1);
  v(0,0)=complex< double >(1,0);
  v(1,0)=complex< double >(cos(2*pi*vIndex/32.0),sin(2*pi*vIndex/32.0));
  v(2,0)=complex< double >(cos(4*pi*vIndex/32.0),sin(4*pi*vIndex/32.0));
  v(3,0)=complex< double >(cos(6*pi*vIndex/32.0),sin(6*pi*vIndex/32.0));
  
  return(v);
}


cmat getv_dash(int v_dashIndex)
{
  cmat v(2,1);
  v(0,0)=complex< double >(1,0);
  v(1,0)=complex< double >(cos(2*pi*v_dashIndex/32.0),sin(2*pi*v_dashIndex/32.0));
  
  return(v);
}


Array<cmat> generateCodebookForULTwoPorts(int nLayers)
{
  Array<cmat> W;
  const dComplex R = dComplex(1.0,0.0);
  const dComplex I = dComplex(0.0,1.0);
  
  switch(nLayers)
  {
    case 1:
    {
      W.set_size(6);
      for(int i=0; i<6; i++)
	W(i).set_size(2,nLayers);
      
      W(0)(0,0)=R/SQRT_2;	W(0)(1,0)=R/SQRT_2;
      W(1)(0,0)=R/SQRT_2;	W(1)(1,0)=-R/SQRT_2;
      W(2)(0,0)=R/SQRT_2;	W(2)(1,0)=I/SQRT_2;
      W(3)(0,0)=R/SQRT_2;	W(3)(1,0)=-I/SQRT_2;
      W(4)(0,0)=R/SQRT_2;	W(4)(1,0)=0;
      W(5)(0,0)=0;	        W(5)(1,0)=R/SQRT_2;
    }
    break;
    
    case 2:
    {
      W.set_size(1);
      W(0).set_size(2,nLayers);
      W(0)(0,0)=R/SQRT_2; W(0)(0,1)=0;   W(0)(1,0)=0;   W(0)(1,1)=R/SQRT_2;    
    }
    break;
    default: cout<<"[both:] Undefined #Layers .... exiting in generateCodebookForTwoPorts();"<<endl;abort();
  }
  return(W);
}

Array<cmat> generateCodebookForULFourPorts(int nLayers)
{
  Array<cmat> W;
  const dComplex R = dComplex(1.0,0.0);
  const dComplex I = dComplex(0.0,1.0);
  switch(nLayers)
  {
    case 1:
    {
      W.set_size(24);
      for(int i=0; i<24; i++)
	W(i).set_size(4,nLayers);
      
      W(0)(0,0)=R;	W(0)(1,0)=R;    W(0)(2,0)=R;     W(0)(3,0)=-R;
      W(1)(0,0)=R;	W(1)(1,0)=R;    W(1)(2,0)=I;     W(1)(3,0)=I;
      W(2)(0,0)=R;	W(2)(1,0)=R;    W(2)(2,0)=-R;    W(2)(3,0)=R;
      W(3)(0,0)=R;	W(3)(1,0)=R;    W(3)(2,0)=-I;    W(3)(3,0)=-I;
      W(4)(0,0)=R;	W(4)(1,0)=I;    W(4)(2,0)=R;     W(4)(3,0)=I;
      W(5)(0,0)=R;	W(5)(1,0)=I;    W(5)(2,0)=I;     W(5)(3,0)=R;
      W(6)(0,0)=R;	W(6)(1,0)=I;    W(6)(2,0)=-R;    W(6)(3,0)=-I;
      W(7)(0,0)=R;	W(7)(1,0)=I;    W(7)(2,0)=-I;    W(7)(3,0)=-R;
      
      W(8)(0,0)=R;	W(8)(1,0)=-R;    W(8)(2,0)=R;    W(8)(3,0)=R;
      W(9)(0,0)=R;	W(9)(1,0)=-R;    W(9)(2,0)=I;    W(9)(3,0)=-I;
      W(10)(0,0)=R;	W(10)(1,0)=-R;   W(10)(2,0)=-R;  W(10)(3,0)=-R;
      W(11)(0,0)=R;	W(11)(1,0)=-R;   W(11)(2,0)=-I;  W(11)(3,0)=I;
      W(12)(0,0)=R;	W(12)(1,0)=-I;   W(12)(2,0)=R;   W(12)(3,0)=-I;
      W(13)(0,0)=R;	W(13)(1,0)=-I;   W(13)(2,0)=I;   W(13)(3,0)=-R;
      W(14)(0,0)=R;	W(14)(1,0)=-I;   W(14)(2,0)=-R;  W(14)(3,0)=I;
      W(15)(0,0)=R;	W(15)(1,0)=-I;   W(15)(2,0)=-I;  W(15)(3,0)=R;
      
      W(16)(0,0)=R;	W(16)(1,0)=0;   W(16)(2,0)=R;    W(16)(3,0)=0;
      W(17)(0,0)=R;	W(17)(1,0)=0;   W(17)(2,0)=-R;   W(17)(3,0)=0;
      W(18)(0,0)=R;	W(18)(1,0)=0;   W(18)(2,0)=I;    W(18)(3,0)=0;
      W(19)(0,0)=R;	W(19)(1,0)=0;   W(19)(2,0)=-I;   W(19)(3,0)=0;
      W(20)(0,0)=0;	W(20)(1,0)=R;   W(20)(2,0)=0;    W(20)(3,0)=R;
      W(21)(0,0)=0;	W(21)(1,0)=R;   W(21)(2,0)=0;    W(21)(3,0)=-R;
      W(22)(0,0)=0;	W(22)(1,0)=R;   W(22)(2,0)=0;    W(22)(3,0)=I;
      W(23)(0,0)=0;	W(23)(1,0)=R;   W(23)(2,0)=0;    W(23)(3,0)=-I;
    }
    break;
    case 2:
    {
      W.set_size(16); 
      for(int i=0; i<16; i++)
	W(i).set_size(4,nLayers);
      
      W(0)(0,0)=R;   W(0)(0,1)=0;   W(0)(1,0)=R;   W(0)(1,1)=0;   W(0)(2,0)=0;   W(0)(2,1)=R;   W(0)(3,0)=0;   W(0)(3,1)=-I;
      W(1)(0,0)=R;   W(1)(0,1)=0;   W(1)(1,0)=R;   W(1)(1,1)=0;   W(1)(2,0)=0;   W(1)(2,1)=R;   W(1)(3,0)=0;   W(1)(3,1)=I;
      W(2)(0,0)=R;   W(2)(0,1)=0;   W(2)(1,0)=-I;  W(2)(1,1)=0;   W(2)(2,0)=0;   W(2)(2,1)=R;   W(2)(3,0)=0;   W(2)(3,1)=R;
      W(3)(0,0)=R;   W(3)(0,1)=0;   W(3)(1,0)=-I;  W(3)(1,1)=0;   W(3)(2,0)=0;   W(3)(2,1)=R;   W(3)(3,0)=0;   W(3)(3,1)=-R;
      
      W(4)(0,0)=R;   W(4)(0,1)=0;   W(4)(1,0)=-R;   W(4)(1,1)=0;   W(4)(2,0)=0;   W(4)(2,1)=R;   W(4)(3,0)=0;   W(4)(3,1)=-I;
      W(5)(0,0)=R;   W(5)(0,1)=0;   W(5)(1,0)=-R;   W(5)(1,1)=0;   W(5)(2,0)=0;   W(5)(2,1)=R;   W(5)(3,0)=0;   W(5)(3,1)=I;
      W(6)(0,0)=R;   W(6)(0,1)=0;   W(6)(1,0)=I;    W(6)(1,1)=0;   W(6)(2,0)=0;   W(6)(2,1)=R;   W(6)(3,0)=0;   W(6)(3,1)=R;
      W(7)(0,0)=R;   W(7)(0,1)=0;   W(7)(1,0)=I;    W(7)(1,1)=0;   W(7)(2,0)=0;   W(7)(2,1)=R;   W(7)(3,0)=0;   W(7)(3,1)=-R;
      
      W(8)(0,0)=R;   W(8)(0,1)=0;   W(8)(1,0)=0;    W(8)(1,1)=R;   W(8)(2,0)=R;   W(8)(2,1)=0;   W(8)(3,0)=0;   W(8)(3,1)=R;
      W(9)(0,0)=R;   W(9)(0,1)=0;   W(9)(1,0)=0;    W(9)(1,1)=R;   W(9)(2,0)=R;   W(9)(2,1)=0;   W(9)(3,0)=0;   W(9)(3,1)=-R;
      W(10)(0,0)=R;  W(10)(0,1)=0;  W(10)(1,0)=0;   W(10)(1,1)=R;  W(10)(2,0)=-R; W(10)(2,1)=0;  W(10)(3,0)=0;  W(10)(3,1)=R;
      W(11)(0,0)=R;  W(11)(0,1)=0;  W(11)(1,0)=0;   W(11)(1,1)=R;  W(11)(2,0)=-R; W(11)(2,1)=0;  W(11)(3,0)=0;  W(11)(3,1)=-R;
      
      W(12)(0,0)=R;  W(12)(0,1)=0;  W(12)(1,0)=0;   W(12)(1,1)=R;  W(12)(2,0)=0;  W(12)(2,1)=R;  W(12)(3,0)=R;  W(12)(3,1)=0;
      W(13)(0,0)=R;  W(13)(0,1)=0;  W(13)(1,0)=0;   W(13)(1,1)=R;  W(13)(2,0)=0;  W(13)(2,1)=-R; W(13)(3,0)=R;  W(13)(3,1)=0;
      W(14)(0,0)=R;  W(14)(0,1)=0;  W(14)(1,0)=0;   W(14)(1,1)=R;  W(14)(2,0)=0;  W(14)(2,1)=R;  W(14)(3,0)=-R;  W(14)(3,1)=0;
      W(15)(0,0)=R;  W(15)(0,1)=0;  W(15)(1,0)=0;   W(15)(1,1)=R;  W(15)(2,0)=0;  W(15)(2,1)=-R; W(15)(3,0)=-R;  W(15)(3,1)=0;
    }
    break;
    case 3:
    {
      W.set_size(12); 
      for(int i=0; i<12; i++)
	W(i).set_size(4,nLayers);
      
      
      W(0)(0,0)=R;   W(0)(0,1)=0;   W(0)(0,2)=0;   W(0)(1,0)=R;   W(0)(1,1)=0;   W(0)(1,2)=0;   W(0)(2,0)=0;   W(0)(2,1)=R;   W(0)(2,2)=0;   W(0)(3,0)=0;   W(0)(3,1)=0;   W(0)(3,2)=R;
      W(1)(0,0)=R;   W(1)(0,1)=0;   W(1)(0,2)=0;   W(1)(1,0)=-R;  W(1)(1,1)=0;   W(1)(1,2)=0;   W(1)(2,0)=0;   W(1)(2,1)=R;   W(1)(2,2)=0;   W(1)(3,0)=0;   W(1)(3,1)=0;   W(1)(3,2)=R;
      W(2)(0,0)=R;   W(2)(0,1)=0;   W(2)(0,2)=0;   W(2)(2,0)=0;   W(2)(2,1)=R;   W(2)(2,2)=0;   W(2)(1,0)=R;   W(2)(1,1)=0;   W(2)(1,2)=0;   W(2)(3,0)=0;   W(2)(3,1)=0;   W(2)(3,2)=R;
      W(3)(0,0)=R;   W(3)(0,1)=0;   W(3)(0,2)=0;   W(3)(2,0)=0;   W(3)(2,1)=R;   W(3)(2,2)=0;   W(3)(1,0)=-R;  W(3)(1,1)=0;   W(3)(1,2)=0;   W(3)(3,0)=0;   W(3)(3,1)=0;   W(3)(3,2)=R;
      
      W(4)(0,0)=R;   W(4)(0,1)=0;   W(4)(0,2)=0;   W(4)(1,0)=0;   W(4)(1,1)=R;   W(4)(1,2)=0;   W(4)(2,0)=0;   W(4)(2,1)=0;   W(4)(2,2)=R;   W(4)(3,0)=R;   W(4)(3,1)=0;   W(4)(3,2)=0;
      W(5)(0,0)=R;   W(5)(0,1)=0;   W(5)(0,2)=0;   W(5)(1,0)=0;   W(5)(1,1)=R;   W(5)(1,2)=0;   W(5)(2,0)=0;   W(5)(2,1)=0;   W(5)(2,2)=R;   W(5)(3,0)=-R;  W(5)(3,1)=0;   W(5)(3,2)=0;
      W(6)(0,0)=0;   W(6)(0,1)=R;   W(6)(0,2)=0;   W(6)(1,0)=R;   W(6)(1,1)=0;   W(6)(1,2)=0;   W(6)(2,0)=R;   W(6)(2,1)=0;   W(6)(2,2)=0;   W(6)(3,0)=0;   W(6)(3,1)=0;   W(6)(3,2)=R;
      W(7)(0,0)=0;   W(7)(0,1)=R;   W(7)(0,2)=0;   W(7)(1,0)=R;   W(7)(1,1)=0;   W(7)(1,2)=0;   W(7)(2,0)=-R;  W(7)(2,1)=0;   W(7)(2,2)=0;   W(7)(3,0)=0;   W(7)(3,1)=0;   W(7)(3,2)=R;
      
      W(8)(0,0)=0;   W(8)(0,1)=R;   W(8)(0,2)=0;   W(8)(1,0)=R;   W(8)(1,1)=0;   W(8)(1,2)=0;   W(8)(2,0)=0;   W(8)(2,1)=0;   W(8)(2,2)=R;   W(8)(3,0)=R;   W(8)(3,1)=0;   W(8)(3,2)=0;
      W(9)(0,0)=0;   W(9)(0,1)=R;   W(9)(0,2)=0;   W(9)(1,0)=R;   W(9)(1,1)=0;   W(9)(1,2)=0;   W(9)(2,0)=0;   W(9)(2,1)=0;   W(9)(2,2)=R;   W(9)(3,0)=-R;  W(9)(3,1)=0;   W(9)(3,2)=0;
      W(10)(0,0)=0;  W(10)(0,1)=R;  W(10)(0,2)=0;  W(10)(1,0)=0;  W(10)(1,1)=0;  W(10)(1,2)=R;  W(10)(2,0)=R;  W(10)(2,1)=0;  W(10)(2,2)=0;  W(10)(3,0)=R;  W(10)(3,1)=0;  W(10)(3,2)=0;
      W(11)(0,0)=0;  W(11)(0,1)=R;  W(11)(0,2)=0;  W(11)(1,0)=0;  W(11)(1,1)=0;  W(11)(1,2)=R;  W(11)(2,0)=R;  W(11)(2,1)=0;  W(11)(2,2)=0;  W(11)(3,0)=-R; W(11)(3,1)=0;  W(11)(3,2)=0;
    }
    break;
    case 4:
    {
      W.set_size(1);
      W(0)=eye_c(4);
    }
    break;
    
    default: cout<<"[both:] Undefined #Layers .... exiting in generateCodebookForFourPorts();"<<endl;abort();
  }
  for (int k=0;k<W.length();k++)
    W(k)=W(k)/2.0;
  return(W);
}

cmat generateDFTMatrix(int nPorts){

//   cmat DFTmatrix = ones_c(nPorts,nPorts);
//   const dComplex R = dComplex(1.0,0.0);
//   const dComplex I = dComplex(0.0,1.0);
//   const dComplex w = dComplex(1.0/SQRT_2,-1.0/SQRT_2);

//   switch(nPorts)
//   {
//     case 2:
//       DFTmatrix(1,1)=-R;
//       DFTmatrix = (1.0/SQRT_2) * DFTmatrix;
//       break;
//     case 4:
//       DFTmatrix(1,1)=-I; DFTmatrix(1,2)=-R; DFTmatrix(1,3)=I;
//       DFTmatrix(2,1)=-R; DFTmatrix(2,3)=-R;
//       DFTmatrix(3,1)=I; DFTmatrix(3,2)=-R; DFTmatrix(3,3)=-I;
//       DFTmatrix = (1.0/2.0) * DFTmatrix;
//       break;
//     case 8:
//       DFTmatrix(1,1) = pow(w,1); DFTmatrix(1,2) = pow(w,2); DFTmatrix(1,3) = pow(w,3); DFTmatrix(1,4) = pow(w,4); DFTmatrix(1,5) = pow(w,5); DFTmatrix(1,6) = pow(w,6); DFTmatrix(1,7) = pow(w,7);
//       DFTmatrix(2,1) = pow(w,2); DFTmatrix(2,2) = pow(w,4); DFTmatrix(2,3) = pow(w,6); DFTmatrix(2,4) = pow(w,8); DFTmatrix(2,5) = pow(w,10); DFTmatrix(2,6) = pow(w,12); DFTmatrix(2,7) = pow(w,14);
//       DFTmatrix(3,1) = pow(w,3); DFTmatrix(3,2) = pow(w,6); DFTmatrix(3,3) = pow(w,9); DFTmatrix(3,4) = pow(w,12); DFTmatrix(3,5) = pow(w,15); DFTmatrix(3,6) = pow(w,18); DFTmatrix(3,7) = pow(w,21);
//       DFTmatrix(4,1) = pow(w,4); DFTmatrix(4,2) = pow(w,8); DFTmatrix(4,3) = pow(w,12); DFTmatrix(4,4) = pow(w,16); DFTmatrix(4,5) = pow(w,20); DFTmatrix(4,6) = pow(w,24); DFTmatrix(4,7) = pow(w,28);
//       DFTmatrix(5,1) = pow(w,5); DFTmatrix(5,2) = pow(w,10); DFTmatrix(5,3) = pow(w,15); DFTmatrix(5,4) = pow(w,20); DFTmatrix(5,5) = pow(w,25); DFTmatrix(5,6) = pow(w,30); DFTmatrix(5,7) = pow(w,35);
//       DFTmatrix(6,1) = pow(w,6); DFTmatrix(6,2) = pow(w,12); DFTmatrix(6,3) = pow(w,18); DFTmatrix(6,4) = pow(w,24); DFTmatrix(6,5) = pow(w,30); DFTmatrix(6,6) = pow(w,36); DFTmatrix(6,7) = pow(w,42);
//       DFTmatrix(7,1) = pow(w,7); DFTmatrix(7,2) = pow(w,14); DFTmatrix(7,3) = pow(w,21); DFTmatrix(7,4) = pow(w,28); DFTmatrix(7,5) = pow(w,35); DFTmatrix(7,6) = pow(w,42); DFTmatrix(7,7) = pow(w,49);
//       DFTmatrix = (1.0/sqrt(8.0)) * DFTmatrix;
//       break;
//     default:
//       cout<<"[both:] Not supported nPorts .... exiting in dftmtx();"<<endl;abort();
//   }
    
    mat phase(nPorts,nPorts);
    for(int k=0;k<nPorts;k++)
        for(int n=0;n<nPorts;n++)
            phase(k,n) = -2.0*pi*k*n/nPorts;

  return expj(phase);

}

cmat generateDFTMatrix(int nPorts, int nLayers, int order){
  
  cmat precoder = generateDFTMatrix(nPorts).get_cols(order , order);
  
  //cout<<" "<<order;
  for(int k = 1; k < nLayers; k++)
  {
    //cout<<" "<<((order+k)%nPorts);
    precoder = concat_horizontal(precoder,generateDFTMatrix(nPorts).get_cols( ((order+k)%nPorts) , ((order+k)%nPorts) ));
  }
  return (1.0/sqrt(nLayers))*precoder;
}

vec getAmplitude(Array<cmat> halfBeams, Array<cmat> halfChannel)
{
    vec Amp(halfBeams.length());
    for(int i=0;i<halfBeams.length();i++)
        Amp(i) = getAmplitude(halfBeams(i),halfChannel);
    return Amp;
}

double getAmplitude(cmat halfBeam, Array<cmat> halfChannel)
{
  double amplitude = 0.0;
  int nTones = halfChannel.length();

  double sigPower = 0.0;
  for(int tone_cnt = 0; tone_cnt < nTones; tone_cnt++)
  {  
    cmat HP = halfChannel(tone_cnt)*halfBeam;
    sigPower += abs(HP.hermitian_transpose()*HP)(0);
  }
  amplitude = sigPower/nTones;
  return amplitude;
}

cmat getOverSampledDFTMatrix(int N, int O)
{
    mat phase = zeros(N,N*O);
    for(int r=0;r<N;r++)
      for(int c=0;c<N*O;c++)
        phase(r,c) = (double) 2*pi*r*c/(N*O);
    return expj(phase);
}

cmat computeBeamMatrix(int N1,int N2)
{
  int O1 = 4,O2= (N2==1) ? 1: 4;
  cmat Vl = getOverSampledDFTMatrix(N1,O1);
  cmat Um = getOverSampledDFTMatrix(N2,O2);
  return kron(Vl,Um);
}
cmat Codebook::getType2SubbandPrecoder_OMP(const Subband_Type2coefficients_OMP_S& type2CoeffSubband, int nLayers, int nStrongBeams)
{
    int numOfPolzns=2;
    int nTx = 2*type2CoeffSubband.orthBeams.rows();
    cmat subbandType2Precoder = zeros_c(nTx, nLayers);
    int Lbeams = type2CoeffSubband.orthBeams.cols();
    if(nStrongBeams>Lbeams)
    {
        cout<<"Number of stronger beams can't be more than Lbeams, check nStrongBeams, and Lbeams. Aborting.."<<endl;
        abort();
    }
    Array<cvec> linearCombOfbeamsOverPolzn(numOfPolzns);
    cvec tmpLayer(nTx);
    //     vec tmpNormFactor = zeros(numOfPolzns);  // Used in the spec normfactor calculation
    double normfactor=1.0;
    for(int layerCnt = 0; layerCnt<nLayers; layerCnt++)
    {
        for(int polCnt=0; polCnt<numOfPolzns; polCnt++)
        {
            linearCombOfbeamsOverPolzn(polCnt)=zeros_c(type2CoeffSubband.orthBeams.rows());
            for(int beamCnt =0; beamCnt<nStrongBeams; beamCnt++)
            {
                linearCombOfbeamsOverPolzn(polCnt) += type2CoeffSubband.coefficients(layerCnt)(polCnt)(beamCnt)*type2CoeffSubband.sortedBeams(layerCnt)(polCnt).get_col(beamCnt);
                //                 tmpNormFactor(polCnt) +=  pow(type2CoeffSubband.amplitudes(layerCnt)(polCnt)(beamCnt),2);
            }
        }
        append(linearCombOfbeamsOverPolzn(0),linearCombOfbeamsOverPolzn(1));
        // spec normfactor
        //         normfactor = sqrt(dlCodeBookConfig.N1*dlCodeBookConfig.N2*(tmpNormFactor(0)+tmpNormFactor(1)));
        
        tmpLayer = linearCombOfbeamsOverPolzn(0);
        normfactor = norm(tmpLayer);
        tmpLayer = (1/normfactor)*tmpLayer;
        subbandType2Precoder.set_col(layerCnt, tmpLayer);
    }
    if(nLayers > 1)
        subbandType2Precoder = (1/sqrt(nLayers))*subbandType2Precoder;
    
    return subbandType2Precoder;
}


















