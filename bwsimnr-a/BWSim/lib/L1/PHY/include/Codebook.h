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


#ifndef _CB_H
#define _CB_H

#include "StructsAndEnums.h"
#include "AWGNTables.h"

class AWGNTablesConvCoder;

extern string NRCodebookType_Str[8];
extern string NRCodebookMode_Str[2];

//NR codebook related
struct CBParam_S
{
//     int nLayers, N1, N2, O1, O2;
    int CBSizePerLayer, i11Size, i12Size, i11, i12, i2Size, i13Size,i14Size;
    ivec i2Sizes,i14Sizes;
    ivec i2,i14;
    ivec indicesSizes;
    dComplex phi_n, theta_p;
    cvec phi_p,a_p,b_n;
    CBParam_S()
    {
//         nLayers =-1;N1=-1;N2=-1;O1=-1; O2=-1;
        CBSizePerLayer=0;
        i11Size=-1;
        i12Size=-1;
        i11=0;
        i12=0;
        i2Size =-1;
        i13Size =-1;
        phi_n = dComplex(0,0);
        theta_p = dComplex(0,0);
    }
    void PrintIndices()
    {
        cout<<"i11: "<<i11<<" i12: "<<i12<<" i14: "<<i14<<" i2: "<<i2;
        cout<<" phi_n: "<<phi_n<<endl;
    }
    void PrintSizes()
    {
        cout<<" i11Size: "<<i11Size<<" i12Size: "<<i12Size<<" i13Size: "<<i13Size<<" i14Size: "<<i14Size<<" i2Size: "<<i2Size<<endl;
    }
};

struct CodeBookConfig_S{
  
  bool isNRCodeBook = true;
  int nCSIRSPorts;
  NRCodeBookType codeBookType = _NR_CODEBOOK_TYPE_UNIT_;
  NRCodeBookMode codeBookMode = _NR_CODEBOOK_MODE_UNIT_;
  bool isType2IdealCoeff = false;
  bool isType2IdealPrecoder = false;
  int LBeams = -1;
  int nPSK = -1;
  int nWideAmp = -1;
  int nSubAmp=-1;
  int N1 = -1;
  int N2 = -1;
  int P = -1;
  int O1 = -1;
  int O2 = -1;
  int Ng = -1;
  int M = -1;
  int d = -1;
  int N_Phase = -1;
  
  void setType1(int ng,int n1,int n2,int p,NRCodeBookMode CBmode)
  {
      Ng = ng,N1=n1,N2=n2,P=p,codeBookMode = CBmode;
      codeBookType = (Ng==1) ? _NR_CODEBOOK_TYPE_I_ : _NR_CODEBOOK_TYPE_I_MULTIPANEL_;
      O1=4;O2 = (N2==1) ? 1 : 4;
      nCSIRSPorts = Ng*N1*N2*P;
      isNRCodeBook = true;
  }
  
  void print(){
    
    cout<<"codeBookType : "<<NRCodebookType_Str[codeBookType]<<endl;
    cout<<"nCSIRSPorts : "<<nCSIRSPorts<<endl;
    cout<<"[Ng N1 N2 P]: ["<<Ng<<" "<<N1<<" "<<N2<<" "<<P<<"]\n"<<endl; 
    if(codeBookType!= _NR_CODEBOOK_TYPE_I_ && codeBookType!= _NR_CODEBOOK_TYPE_I_MULTIPANEL_)
    {
        cout<<"LBeams : "<<LBeams<<endl;
        cout<<"nPSK : "<<nPSK<<endl;
        cout<<"nWideAmp: "<<nWideAmp<<endl;
        cout<<"nSubAmp: "<<nSubAmp<<endl;
    }
    else
        cout<<"codeBookMode : "<<NRCodebookMode_Str[codeBookMode]<<endl;
  }
  void parseCodeBookConfig(string fileName1, string fileName2);
};

class Codebook{

public:
  CodeBookConfig_S dlCodeBookConfig;
private:  
  
  bool isAltCodeBookEnabledFor4TXEnabled;
  Array<Type1HalfBeamWithi1> type1halfBeamswithi1;

  Array< Array<cmat> > TwoPortCodebookForAllLayers;
  Array< Array<cmat> > FourPortCodebookForAllLayers;
  Array< Array<cmat> > EightPortCodebookForAllLayers;
  Array< Array<cmat> > TwelvePortCodebookForAllLayers;
  Array< Array<cmat> > SixteenPortCodebookForAllLayers;
  Array< Array<cmat> > TwentyfourPortCodebookForAllLayers;
  Array< Array<cmat> > ThirtytwoPortCodebookForAllLayers;
  
  Array< Array<cmat> > AlternativeCodeBookFor4Port;
  
  Array< Array<cmat> > SubSampledCodeBookForSubmode1;
  Array< Array<cmat> > CodeBookToSelecti1;
  Array< Array<cmat> > SubSampledCodeBookForsubmode2;
    
  Array< Array<cmat> > TwoPortCodebookForULAllLayers;
  Array< Array<cmat> > FourPortCodebookForULAllLayers;
  
public:
  Array<ivec> unique8portCodebookIndices;
  
  Array<ivec> CodeBookIndicesToSelecti1ForSubmode1;
  Array<ivec> CodeBookIndicesForSubMode2;
  Array<ivec> CodeBookIndicesToSelecti1;
  
  Codebook();
  void initializeDlCodebook();
  void initializeUlCodebook(bool isTransformPrecodingEnabled=0);
  CodeBookConfig_S getCodeBookConfig();
  
  // NR codebook related functions
  Array<cmat> getDLi2Codebook(i1index_S i1Index,int nLayers);
  cmat getPrecoderNR(int codeBookMode,int N1, int N2, int nLayers, type1PMI_S i1i2index);
  cmat getPrecoderNR(int nLayers, type1PMI_S i1i2index);
  Array<cmat> getPrecoderPerSubband(int nLayers,int nSubbands, type1PMI_S i1i2index,TypeIIPMIIndices pmiIndices,int coPhase=-1);
  Array<cmat> getNRPrecoderPerSubband(int nSubbands, TypeIIPMIIndices& pmiIndices);
  Array<cmat> getNRPrecoderPerSubband(int nLayers, type1PMI_S& pmiIndices);
  int getPrecoderIndxAbsNR(int nLayers, type1PMI_S i1i2index); // Gives absolute PMI index
  type1PMI_S getPMIIndices(int codeBookMode, int N1, int N2, int nLayers, int indx);
  type1PMI_S getPMIIndices(int nLayers, int indx);
  ivec geti2CmatIndicesIn_W(i1index_S i1, int codeBookMode, int N1, int N2, int nLayers);
  ivec geti2CmatIndicesIn_W(i1index_S i1 , int nLayers);
  ivec geti2CmatIndicesIn_W(type1PMI_S i1,int nLayers);
  Array< cmat > getType2PrecoderbyCombineLbeams(Array< cmat > LHalfbeams, Array< Array<vec> > amp, Array< Array< ivec > > phase, int nLayers, int sb_cnt, int nPSK);  //Halfbeams
  Array<cmat> getType2Precoder_OMP(Type2coefficients_N_OMP_S& type2Coefficients, int nSubbands, int nLayers);
  // NR codebook related functions end  
  
  Array<cmat> getCodebook(int nPorts,int nLayers,bool isDownlink);
  cmat getPrecoder(int nPorts,int nLayers,int pmi);
  cmat getRandomPrecoder(int nPorts,int nLayers);
  int getPmiIndex(int nPorts,int nLayers,cmat precoder); 
  void getUnique8portCodebookIndices();
  
  Array<cmat> getSubSampledCodeBookForSubmode1(int nLayers);
  Array<cmat> getCodeBookToSelecti1(int nLayers);
  Array<cmat> getSubSampledCodeBookForsubmode2(int nLayers);

  bool getAltCodeBookFlagFor4TX(){return isAltCodeBookEnabledFor4TXEnabled;};
  void setAltCodeBookFlagFor4TX(bool  flag){isAltCodeBookEnabledFor4TXEnabled = flag;};
  
  Array<Array<Array<cmat>>> genType1CodebookNR();
  Array<Array<Array<cmat>>> genType1MultipanelCodebookNR(); //Generates Multipanel Codebook
  Array<Array<cmat>> genType1CodebookNR(NRCodeBookMode CBmode);
  Array<cmat> generateType1CodebookNR(NRCodeBookMode CBmode,int nLayers);
  Array<Array<cmat>> genType1MultipanelCodebookNR(NRCodeBookMode CBmode); //Generates Multipanel Codebook
  Array<Array<cmat>> getCodeBookForPorts(int NrofPorts);
  CBParam_S getCodebookParam(int nLayers);
  Array<cmat> constructDLType1Codebook(int Ng,int N1,int N2,int P,int nLayers,NRCodeBookMode CBmode);
  
  Array<Type1HalfBeamWithi1> getType1HalfBeam();
  
  // Type OMP
  
  ivec quantizeCoefficients(const cvec actualCoefficients, vec& quantizedAmp, vec& quantizedPhases, cvec& quantizedCoeff,bool isWideband=false);
  int selectSetAndComputeCoeff(const Array<cmat> orthBeamsTotalSets, const cvec halfSingularVector, int Lbeams, vec& quantizedAmplitudes, vec& quatizedPhases, cvec& quantizedCoefficients);
  void computeCoeff(const cmat orthBeamsSet, const cvec halfSingularVector, int Lbeams, vec& quantizedAmplitudes, vec& quantizedPhases, cvec& quantizedCoefficients);
  int type1Layer1BeamIndx_OMP(const Array<cmat> orthBeamsTotalSets, cvec halfSingularVector, int& beamIndx);
  Type2coefficients_N_OMP_S getType2OMPCoefficients(Array<cmat>& entireRawChannel,int nSubbands,int subbandSize,int reCountPerRb, const int nLayers);
  void computeBeamSetForTypeII(Type2coefficients_N_OMP_S& type2Coefficients,Array<cmat>& entireRawChannel,int& Lbeams,int nLayers);
  void computeSubbandCoefficientsForIdealPrecoder(Type2coefficients_N_OMP_S& type2Coefficients,Array<cmat>& entireRawChannel,int& Lbeams,int nSubbands,int subbandSize,int reCountPerRb, const int nLayers);
  void compressCoefficientsAcrossSubbands(Type2coefficients_N_OMP_S& type2Coefficients,int& Lbeams,int nSubbands, const int nLayers);
  void computeBeamSetForTypeIIMultiTRP(Array<Type2coefficients_N_OMP_S>& type2Coefficients,Array<Array<cmat>>& entireRawChannel,int& Lbeams,int nLayers);
  void getType2OMPCoefficientsForMultiTRPCJT(Array<Type2coefficients_N_OMP_S>& type2CoefficientsPerTRP,Array<Array<cmat>>& entireRawChannel,int nSubbands,int subbandSize,int reCountPerRb, const int nLayers);
  void computeBeamSetForTypeIIPortSelection(Type2coefficients_N_OMP_S& type2Coefficients,Array<cmat>& entireRawChannel ,int& Lbeams,int nLayers);
  void compressCoefficientsAcrossSubbandsAndLayers(Type2coefficients_N_OMP_S& type2Coefficients,int& Lbeams,int nSubbands, const int nLayers);
  
  void computeSubbandCoefficientsForTypeII(Type2coefficients_N_OMP_S& type2Coefficients,Array<cmat>& entireRawChannel,int& Lbeams,int nSubbands,int subbandSize,int reCountPerRb, const int nLayers);
  void compressCoefficientsAcrossTRPs(Array<Type2coefficients_N_OMP_S>& type2Coefficients,int& Lbeams,int nSubbands, const int nLayers);
  cmat getType2SubbandPrecoder_OMP(const Subband_Type2coefficients_OMP_S& type2CoeffSubband, int nLayers, int nStrongBeams);
};

class CodebookCollection{
  
  ivec N1;
  ivec N2;
  ivec Ng;
  Array<Codebook> codebook;
  
public:
  CodebookCollection()
  {
    N1.set_length(0);
    N2.set_length(0);
    Ng.set_length(0);
    codebook.set_length(0);
  }
  int getCodebookIndx(int n1, int n2,int ng)
  {
      ivec N1Indices=find(N1,n1,true);
      int cbIndx=-1;
      if(N1Indices.length()){
          ivec N2Indices=find(N2(N1Indices),n2,true);  
            if(N2Indices.length())
                cbIndx = find(Ng(N2Indices),ng);
      }
      return cbIndx;
  }
  Codebook* getCodebookPointer(int n1, int n2,int ng)
  {
      int cbIndx=getCodebookIndx(n1,n2,ng);
      return &codebook(cbIndx);
  }
  void addCodebook(CodeBookConfig_S tDlCodebookConfig, bool altFlag=false)
  {
    int n1=tDlCodebookConfig.N1;
    int n2=tDlCodebookConfig.N2;
    int ng=tDlCodebookConfig.Ng;
    int cbIndx=getCodebookIndx(n1,n2,ng);
    if(cbIndx==-1)
    {
        if(currentTaskid==0)
            cout<<"Adding a new-codebook for Ng N1 N2 : "<<ng<<" , "<<n1<<" , "<<n2<<endl;
      Codebook newCodebook;
      newCodebook.dlCodeBookConfig = tDlCodebookConfig;
      newCodebook.initializeDlCodebook();
      newCodebook.setAltCodeBookFlagFor4TX(altFlag);
      newCodebook.initializeUlCodebook(tDlCodebookConfig.isNRCodeBook);
      
      append(codebook,newCodebook);
      append(N1,n1);
      append(N2,n2);
      append(Ng,ng);
    }
  }
  
};

//NR Codebook related

inline dComplex Phi(int n, int Npsk){   double theta = (2.0*pi*n)/(double)Npsk;   return expj(theta); }

inline dComplex Phi(int n){ double theta = (pi*n)/2.0; return expj(theta); }

inline dComplex Theta(int p){ double theta = (pi*p)/4.0; return expj(theta); }

inline dComplex ap(int p){return expj(pi/4)*Phi(p);}

inline dComplex bn(int n){return expj(-pi/4)*Phi(n);}

inline cvec Phi(ivec n)
{
    cvec p(n.size());
    for(int i=0;i<n.size();i++)
    {
        p(i)=Phi(n(i));
    }
    return p;
}
cmat getUm(int m, int N2, int O2);
cmat getVlm(cmat tUm, int l, int N1, int N2, int O1);
cmat getVlm_prime(cmat tUm, int l, int N1, int N2, int O1);
cmat genCmat(dComplex v1,dComplex v2,dComplex v3,dComplex v4);
CBParam_S getParam(NRCodeBookMode CBmode, int nLayers, int layerNum, int N1, int N2, int i2, int h, int v, int g); 
void getParamLayer1_2(int CBmode, int nLayers, int layerNum, int N1, int N2, int i2, int h, int v, int g, CBParam_S &CBparam);
Array<cmat> genTwoPortCodebook_DL_NR(int nLayers); 
CBParam_S getMultipanelParam(NRCodeBookMode CBmode, int nLayers, int layerNum, int N1, int N2, int Ng, int h, int v, int g, int p, int n); 
Array<Array<cmat>> genULCodeBookNR();

//NR Codebook related

ivec getVectorFromPMIIndices(type1PMI_S precIndex,NRCodeBookType CBtype);
type1PMI_S getPMIIndicesFromVector(ivec N);

ivec getK1K2(int nLayers,int N1,int N2,int g,int Ng=1);
cmat getVlm(int l,int m,int N1,int N2,int O1,int O2);
cmat getVlm_prime(int l,int m,int N1,int N2,int O1,int O2);
cmat constructMPPrecoder(int nLayers,type1PMI_S& precIndex,CodeBookConfig_S& dlCodeBookConfig,NRCodeBookMode CBmode=_NR_CODEBOOK_MODE_UNIT_);
cmat constructSPPrecoder(int nLayers,type1PMI_S& precIndex,CodeBookConfig_S& dlCodeBookConfig,NRCodeBookMode CBmode=_NR_CODEBOOK_MODE_UNIT_);
cmat constructTypeIPrecoder(int nLayers,type1PMI_S& precIndex,CodeBookConfig_S& dlCodeBookConfig,NRCodeBookMode CBmode=_NR_CODEBOOK_MODE_UNIT_);


Array<cmat> generateCodebookForDLTwoPorts(int nLayers);
Array<cmat> generateCodebookForULTwoPorts(int nLayers);
Array<cmat> generateCodebookForDLFourPorts(int nLayers);
Array<cmat> generateAlternativeCodebookForDLFourPorts(int nLayers);
Array<cmat> generateCodebookForULFourPorts(int nLayers);
Array<cmat> generateCodebookForDLEightPorts(int nLayers);

ivec getCodeBookIndicesForMode2_1(int nLayers,int indi1); //! refer table 7.2.2-1F in 36.213v11
Array<cmat> getSubSampledCodeBookForMode2_1(int nLayers,int indi1);
ivec getCodeBookIndicesToSelecti1(int nLayers);
int getShortTermPMIInd(int nLayers,int precInd);
Array<cmat> getCodeBookForSpecifiedi1(int nLayers,int Indi1);
ivec getCodeBookIndicesForSubMode2(int nLayers); //! refer Table 7.2.2-1D in 36.213v11
void generateLongTermAndShortTermInd(int nLayers,ivec& LongTermIndex,ivec& ShortTermIndex);
int getLongTermPMIInd(int nLayers,int precInd);
ivec getCodeBookIndicesToSelecti1ForSubmode1(int nLayers); //! refer 7.2.2-1E in 36.213v11
ivec getValidi1Indices(int nLayers);
Array<cmat> getCodeBookSubsetForEightPorts(int nLayers,ivec indices);
ivec getCodeBookIndicesForEightPorts(int nLayers,int Indi1,ivec Indi2=ivec("-1"));
ivec getCodeBookIndicesForEightPorts(int nLayers,ivec Indi1,ivec Indi2=ivec("-1"));
Array<cmat> uMatrixGeneration(int nPorts);
Array<ivec> getHouseHolderLocations(int nLayers);
cmat getW1(int i1, int i2);
cmat getW2(int i1, int i2);
cmat getW3(int i1, int i2);
cmat getW4(int i1, int i2);
cmat getW5(int i1);
cmat getW6(int i1);
cmat getW7(int i1);
cmat getW8();
cmat getAlt4TXW1(int i1, int i2);
cmat getAlt4TXW2(int i1, int i2);
cmat getPhi(int pIndex);
cmat getPhi_dash(int pIndex);
cmat getv(int vIndex);
cmat getv_dash(int v_dashIndex);

cmat generateDFTMatrix(int nPorts);
cmat generateDFTMatrix(int nPorts, int nLayers, int order);
bvec operator > (ivec a,ivec b);

vec getAmplitude(Array<cmat> halfBeams, Array<cmat> halfChannel);
double getAmplitude(cmat halfBeam, Array<cmat> halfChannel);
int selectSet(const Array<cmat> orthBeamsTotalSets, cmat halfSingularMatrix,int L,ivec& indices);
int selectSetWithFullEigenVector(const Array<cmat> orthBeamsTotalSets, cmat halfSingularMatrix,int L,ivec& indices);
int selectSetWithChannelCovariance(const Array<cmat> orthBeamsTotalSets, cmat avgChannelCovar,int L,ivec& indices);
int selectSetUsingOMP(const Array<cmat> orthBeamsTotalSets, cmat halfSingularMatrix,int L,ivec& indices);
int selectSet(const Array<cmat> orthBeamsTotalSets, const cvec halfSingularVector);
void computeBeamIndices(ivec& n1,ivec& n2,int i12,int N1,int N2,int L);
ivec getn3Indices(int i15,int i16,int N3,int M);
Array<cmat> getPortSelectionSets(int N1,int N2,int d,int Lbeams,Array<ivec>& indicesPerComb);
Array<cmat> type2OrthogonalBeamsSet(int N1, int N2, Array<ivec>& i11_IndicesPerSet, Array<ivec>& i12_IndicesPerSet);
cmat computeBeamMatrix(int N1,int N2);
int selectSet(Array<cmat>& orthBeamsTotalSets,Array<cmat>& entireRawChannel,int L,ivec& indices);
cvec coeffAndResidueUsingOMP(cvec halfSingularVector, const cmat orthBeamSet, double& residueNorm, int Lbeams);
#endif
