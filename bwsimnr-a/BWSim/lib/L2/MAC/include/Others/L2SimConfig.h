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
#ifndef L2SIMCONF_H
#define L2SIMCONF_H

#include "../Application/cTrafficClass.h"
#include "cPacket.h"
#include "constants.h"

// #include "../../../L1/common/include/OuterLoopLA.h"

#ifdef USING_RRC
#include "../../../RRC/include/infoElem/infoElem.h"
#endif

enum L2UlDlType
{
    UP_LINK_L2 = 0,
    DOWN_LINK_L2 = 1
};

///UL Transmit Power Control
enum TPC_Type 
{
  _ABSOLUTE_ = 0, 
  _ACCUMULATED_ = 1  
};
struct sXRTrafficGeneratorConfig
{
  int mFPS;
  int mAvgDataRate;

  double mMeanPktSize;
  double mStdPktSize;
  double mMinPktSize;
  double mMaxPktSize;

  double mMeanJitter;
  double mStdJitter;
  double mMinJitter;
  double mMaxJitter;

  bool isInitialised;
  sXRTrafficGeneratorConfig()
  {
      mFPS         = -1;
      mAvgDataRate = -1;

      mMeanPktSize = -1;
      mStdPktSize  = -1;
      mMinPktSize  = -1;
      mMaxPktSize  = -1;

      mMeanJitter  = -1;
      mStdJitter   = -1;
      mMinJitter   = -1;
      mMaxJitter   = -1;

      isInitialised = false;
  }
  void init(string l2ConfigFile)
  {
      parse(l2ConfigFile, "mAvgDataRate", mAvgDataRate);
      parse(l2ConfigFile, "mFPS", mFPS);
      parse(l2ConfigFile, "mMeanPktSize", mMeanPktSize);
      parse(l2ConfigFile, "mStdPktSize", mStdPktSize);
      parse(l2ConfigFile, "mMinPktSize", mMinPktSize);
      parse(l2ConfigFile, "mMaxPktSize", mMaxPktSize);
      parse(l2ConfigFile, "mMeanJitter", mMeanJitter);
      parse(l2ConfigFile, "mStdJitter", mStdJitter);
      parse(l2ConfigFile, "mMinJitter", mMinJitter);
      parse(l2ConfigFile, "mMaxJitter", mMaxJitter);
      isInitialised = true;
  }
};


enum eCQIType
{
  CQIType_UNINIT=-1, P10,P11,P20,P21, A12, A20, A22, A30, A31, A32 , ULCQI
};

enum eTransmissionScheme{ 
  
  _MODE_UNINIT_=-1,
  _MODE_NR_
};


struct sBwPart
{
  uint8_t  bwPartIndex; 				
  /*!<  The bandwidth part for which CQI is being reported. Value : 1..4*/
  uint8_t  sb; 					
  /*!<  The preferred subband in the current bandwidth part. Value :  0..11*/
  std::vector<uint8_t>  cqi; 					
  /*!<  The CQI conditioned Value : 0..15 */
};

struct sULCqiReport
{
    // this is the RB index
  int rb;
  int sb;
  /*!<  The preferred-M subbands reported by the UE.
  Value : 0..24*/

  uint8_t sb_pmi;
  /*!<  The PMI conditioned on the preferred -M subbands.
  Value : 0..15*/

  //vector<uint8_t> sb_cqi;
  /*!<  The CQI for up to two codewords conditioned on the preferred -M subbands. Value : 0..15 */
  
  vector<double> sb_SINR;
  // Reciprocity Precoder matrix
  vector< vector< std::complex<double> > >reciprocity_precoder;
  ivec reciprocity_DLCQI;
  int reciprocity_DLRank;
  Array<cmat> sb_channel;
 
  //reciprocity
    sULCqiReport()
  {
    reciprocity_precoder.resize(0);
    reciprocity_DLCQI.set_length(0);
    reciprocity_DLRank=-1;
    sb_channel.set_length(0);
  }
  
  
};



enum eCompSchemes{
  
  _eCOMP_SCHEME_UNINIT_=-1,
  _eCOMP_SCHEME_JOINT_TX_,
  _eCOMP_SCHEME_JOINT_RX_,
  _eCOMP_SCHEME_DPS_,
  _eCOMP_SCHEME_DB_
  
};



struct sUeSelected
{
  uint8_t sbList[MAX_UE_SELECTED_SB]; 	
  /*!<  The preferred-M subbands reported by the UE. 
  Value : 0..24*/
  uint8_t  sb_pmi; 					
  /*!<  The PMI conditioned on the preferred -M subbands. 
  Value : 0..15*/
  uint8_t  sb_cqi[MAX_TB_LIST]; 			
  /*!<  The CQI for up to two codewords conditioned on the preferred -M subbands. Value : 0..15 */
};



struct sHigherLayerSelected
{
  uint8_t  sb_pmi; 					
  /*!<  The PMI conditioned on the current subband. 
  Value : 0..63*/
  
  cmat sb_precoder;
  Array<cmat> sb_channel;
  
  uint8_t  sb_cqi[MAX_TB_LIST]; 			
  /*!<  The CQI for up to two codewords conditioned on the current subband. Value : 0..15*/
};


enum eNRCodeBookType {
  _eNR_CODEBOOK_TYPE_UNIT_ = -1,
  _eNR_CODEBOOK_TYPE_I_,
  _eNR_CODEBOOK_TYPE_I_MULTIPANEL_,
  _eNR_CODEBOOK_TYPE_II_
};


struct sMultiCqiList{
  
  uint16_t rnti; 					
  /*!<  The RNTI identifying the UE. 
   *   Valu*e : 1..65535*/
  vector<int> nodeGroup;// CoMP cooperating nodes (for configured compScheme) for which the UE reports CQI 
  
  eCompSchemes compScheme; // CoMP scheme like DPS, JT, DB
  
  //std::vector <uint8_t>  ri; 	
  uint8_t  ri; 	
  /*!<  The last received rank indication. 
   *   Valu*e : 1..4*/
  eCQIType cqiType; 					
  /*!<  The CQI format. 
   *   Valu*e : P10,P11,P20,P21, A12, A22, A20, A30, A31*/
  // std::vector< std::vector<uint8_t> >  wb_cqi;
  std::vector<uint8_t>  wb_cqi;
  
  /*!<  The reported wideband CQI value per codeword. 
   *   Valu*e : 0..15*/
  // std::vector <uint8_t>  wb_pmi; 
  uint8_t  wb_pmi;
  uint8_t  wb_i1; 
  uint8_t  wb_i1_1;
  uint8_t  wb_i1_2;
  uint8_t  wb_i1_3;
  uint8_t  wb_i1_4;
  
  /*!<  The reported wideband precoding matrix index. 
   *   Valu*e : 0..15*/
  //std::vector < Array<sBwPart> > bwPart;  
  Array<sBwPart> bwPart;
  
  eTransmissionScheme tx_scheme;
  
  //! Need to sort out later - 10010
  //     Array<sUeSelected> ueSelected;
  sUeSelected ueSelected; 	//! NOTE need to cross check -swathi
  /*!< The subband measurement results for aperiodic UE selected subbands.*/
  vector<sHigherLayerSelected> higherLyrSelected; 		
  /*!<  The subband measurement results for aperiodic higher-layer selected subbands.*/
  
  eNRCodeBookType nrCodeBookType;
  
  sMultiCqiList()
  {
    rnti = NOT_DEFINED; 					
    ri = NOT_DEFINED_8BIT; 
    cqiType=CQIType_UNINIT;
    wb_cqi.resize(2);
    wb_cqi[0] = NOT_DEFINED_8BIT;
    wb_cqi[1] = NOT_DEFINED_8BIT;
    wb_pmi = NOT_DEFINED_8BIT;
    wb_i1 = NOT_DEFINED_8BIT; 
    wb_i1_1 = NOT_DEFINED_8BIT;
    wb_i1_2 = NOT_DEFINED_8BIT;
    wb_i1_3 = NOT_DEFINED_8BIT;
    wb_i1_4 = NOT_DEFINED_8BIT;
    tx_scheme=_MODE_UNINIT_;
    higherLyrSelected.resize(MAX_HL_SB);
    nodeGroup.resize(0);
    compScheme = _eCOMP_SCHEME_UNINIT_;
    nrCodeBookType = _eNR_CODEBOOK_TYPE_UNIT_;
  }
  
};


//Need to find appropriate place
struct sCqiList{
  // In NR Tx mode these variable are not set. Only  vector<sMultiCqiList> multiCqiList structure is set .. Abhi
  
  uint16_t rnti; 					
  /*!<  The RNTI identifying the UE. 
   *   Value : 1..65535*/
  uint8_t  ri; 				
  /*!<  The last received rank indication. 
   *   Valu*e : 1..4*/
  eCQIType cqiType; 					
  /*!<  The CQI format. 
   *   Value : P10,P11,P20,P21, A12, A22, A20, A30, A31*/
  std::vector<uint8_t>  wb_cqi; 
  /*!<  The reported wideband CQI value per codeword. 
   *   Valu*e : 0..15*/
  uint8_t  wb_i1; 
  uint8_t  wb_i1_1;
  uint8_t  wb_i1_2;
  uint8_t  wb_i1_3;
  uint8_t  wb_i1_4;
  /*!<  The reported wideband i1. 
   *   Value : 0..15*/
  uint8_t  wb_pmi; 				
  /*!<  The reported wideband precoding matrix index. 
   *   Value : 0..15*/
  eTransmissionScheme tx_scheme;
  
  Array<sBwPart> bwPart;  					
  /*!<  The subband measurement results for periodic UE selected subbands. Indicates the current CQI for the best subband in the current bandwidth part.*/

  vector<sULCqiReport> ulCqiReportList; //added for uplink CQI report
  
  // Set in NR Tx mode 3_2 at present 
  vector<sMultiCqiList> multiCqiList; //added for Multiple CSI Process per UE
  
  //! Need to sort out later - 10010
//     Array<sUeSelected> ueSelected;
    sUeSelected ueSelected; 	//! NOTE need to cross check -swathi
    /*!< The subband measurement results for aperiodic UE selected subbands.*/
    vector<sHigherLayerSelected> higherLyrSelected; 		
    /*!<  The subband measurement results for aperiodic higher-layer selected subbands.*/
    
  
  sCqiList()
  {
    rnti = NOT_DEFINED; 					
    ri = NOT_DEFINED_8BIT; 
    cqiType=CQIType_UNINIT;
    wb_cqi.resize(2);
    wb_cqi[0] = NOT_DEFINED_8BIT;
    wb_cqi[1] = NOT_DEFINED_8BIT;
    wb_pmi = NOT_DEFINED_8BIT;
    wb_i1 = NOT_DEFINED_8BIT;
    wb_i1_1 = NOT_DEFINED_8BIT;
    wb_i1_2 = NOT_DEFINED_8BIT;
    wb_i1_3 = NOT_DEFINED_8BIT;
    wb_i1_4 = NOT_DEFINED_8BIT;
    
    tx_scheme=_MODE_UNINIT_;
    higherLyrSelected.resize(MAX_HL_SB);
    ulCqiReportList.resize(0);
  }
  
  
  
};

struct sCBRTrafficGeneratorConfig
{
  int mPacketSize;
  int mInterPacketDelay;
  bool isInitialised;
  sCBRTrafficGeneratorConfig()
  {
      mPacketSize = -1;mInterPacketDelay = -1;isInitialised = false;
  }
  void init(string l2ConfigFile)
  {
      parse(l2ConfigFile,"mPacketSize",mPacketSize);
      parse(l2ConfigFile,"mInterPacketDelay",mInterPacketDelay);
      isInitialised = true;
  }
};

struct sVideoTrafficGeneratorConfig
{
	int mVideoPayLoad;		// = 1500 - 57; 57 = 20 + 8 + 12 + 13 + 4 (IP + UDP + RTP + TS + PES) overhead

        int mGOPSize;                   // number of frames in a GOP
        vector<eFrameType> mGOP2;               // {I, P, B, P, B, P, B, P, B, P, B, P, B};
	std::string mBitRate;
	vector<double> mGammaParams_I;      // {3.5447e5, 7.6209e9, 16.487, 21499}
        vector<double> mGammaParams_P;      // {2.7134e5, 4.3131e9, 17.071, 15895}
        vector<double> mGammaParams_B;      // {2.2765e5, 3.3255e9, 15.584, 14608}
	int mVideoFramesPerSecond;	// Number of video frames per second - determined from the data rate of the stream (32kbps - 10fps)
	int mPktsPerVideoFrame;		// Number of packets in frame - its deterministic - 8 packet per frame.
	bool isInitialised;
	sVideoTrafficGeneratorConfig()
	{
	  mGOP2.resize(0);
	  mGammaParams_B.resize(0);
	  mGammaParams_I.resize(0);
	  mGammaParams_P.resize(0);
      isInitialised = false;
	}
	void setDefault()
    {
        mGOPSize = 13;mVideoPayLoad = 1443;
        mVideoFramesPerSecond = 8;
        mGOP2 = {I, P, B, P, B, P, B, P, B, P, B, P, B};
        if(mBitRate == "5.5Mbps")
        {
            mGammaParams_I = {0.0, 0.0, 16.487, 21499};
            mGammaParams_B = {0.0, 0.0, 15.584, 14608};
            mGammaParams_P = {0.0, 0.0, 17.071, 15895};
        }
        else if(mBitRate == "1.3Mbps") 
        {
            mGammaParams_I = {0.0, 0.0, 6.468, 22905};
            mGammaParams_B = {0.0, 0.0, 1.682 , 25572};
            mGammaParams_P = {0.0, 0.0, 2.549, 26287};
        }
        else if(mBitRate == "315Kbps") 
        {
            mGammaParams_I = {0.0, 0.0, 4.406, 12235 };
            mGammaParams_B = {0.0, 0.0, 0.813  , 7857};
            mGammaParams_P = {0.0, 0.0, 1.376, 11874};
        }
        else 
        {
            cout<<"Invalid mBitRate..."<<endl;abort();
        }
    }
	void init(string l2ConfigFile)
    {
        parse(l2ConfigFile,"mBitRate",mBitRate);
        setDefault();
        isInitialised = true;
    }
};

struct sFTPTrafficGeneratorConfig
{
  unsigned int mInitialCongestionWindow;/**< Initial congestion window for TCP **/

  double mLogNormalMean;		/**< File is lognormal distributed. this gives the mean of this distribution **/
  double mLogNormalStandardDeviation;	/**< this gives standard deviation of the lognormal distribution **/

  double mInterReadTimeLamba;		/**< lambda value of inter read time **/
  double mTCTimeLambda;			/**< lambda value of TC time **/
  double mArrTimeLambda;		/**> lambda for User Arrival Rate **/
  double mInterPktTimeLambda;  //lamda for inter-packet delay.

  /// file size can be either 'SMALL', 'MEDIUM' or 'LARGE'
  int mFTPFileSizeType;
  int mFTPModelType; ///Type can be 0, 1, 2, 3 - the last 3 correspond to 3GPP FTP models
  
  bool isTCPEnabled;
  unsigned int mWindowSize;
  bool isInitialised;
  int mMaxFileSize,mMinFileSize;
  
  sFTPTrafficGeneratorConfig()
  {
        isInitialised = false;
  }
  
  void loadTCPParams(string tcpFileName)
  {
      parse(tcpFileName,"isTCPEnabled",isTCPEnabled);
      parse(tcpFileName,"mTcpRxWindowsize",mWindowSize);
  }
  void init(string l2ConfigFile,string tcpConfigFile)
  {
      parse(l2ConfigFile,"mInitialCongestionWindow",mInitialCongestionWindow);
      parse(l2ConfigFile,"mInterReadTimeLamba",mInterReadTimeLamba);
      parse(l2ConfigFile,"mFTPModelType",mFTPModelType);
      parse(l2ConfigFile,"mArrTimeLambda",mArrTimeLambda);
      if(mFTPModelType==0)
      {
          loadTCPParams(tcpConfigFile);
          parse(l2ConfigFile,"mMaxFileSize",mMaxFileSize);
          parse(l2ConfigFile,"mMinFileSize",mMinFileSize);
          parse(l2ConfigFile,"mLogNormalMean",mLogNormalMean);
          parse(l2ConfigFile,"mLogNormalStandardDeviation",mLogNormalStandardDeviation);
          parse(l2ConfigFile,"mTCTimeLambda",mTCTimeLambda);
      }
      else
      {
          parse(l2ConfigFile,"mInterPktTimeLambda",mInterPktTimeLambda);
          parse(l2ConfigFile,"mFTPFileSizeType",mFTPFileSizeType);
      }
      isInitialised = true;
  }
};

struct sHTTPTrafficGeneratorConfig
{
	/**< Size of the main object is distributed as truncated lognormal distribution */
	double mMainObjMean;	/**<  Mean */
	double mMainObjSD;	/**<  Standard Deviation */
	unsigned long int mMainObjMax;
	unsigned long int mMainObjMin;

	/**<  Size of the Embedded objects are truncated lognormal distributed */
	double mEmbeddedObjMean;		/**<  Mean;*/
	double mEmbeddedObjSD;		/**<  Standard Deviation */
	unsigned long int mEmbeddedObjMax;
	unsigned long int mEmbeddedObjMin;

	/**<  Number of Embedded objects per page is distributed as truncated pareto */
	double mNumEmbeddedObjThreshold;
	double mNumEmbeddedObjIndex;
	unsigned int mNumEmbeddedObjMax;

	/**<  Reading time is distributed as Exponential distribution */
	double mReadingTimeLambda;

	/**<  Parsing time is distributed as Exponential distribution */
	double mParsingTimeLambda;

	/**<  TCTime is ditributed as Exponential distribution */
        double mTCTimeLambda;
    bool isInitialised;
    sHTTPTrafficGeneratorConfig()
    {
        isInitialised = false;
    }
        
    void init(string l2ConfigFile)
    {
      parse(l2ConfigFile,"mMainObjMean",mMainObjMean);
      parse(l2ConfigFile,"mMainObjSD",mMainObjSD);
      parse(l2ConfigFile,"mMainObjMax",mMainObjMax);
      parse(l2ConfigFile,"mMainObjMin",mMainObjMin);
      parse(l2ConfigFile,"mEmbeddedObjMean",mEmbeddedObjMean);
      parse(l2ConfigFile,"mEmbeddedObjSD",mEmbeddedObjSD);
      parse(l2ConfigFile,"mEmbeddedObjMax",mEmbeddedObjMax);
      parse(l2ConfigFile,"mEmbeddedObjMin",mEmbeddedObjMin);
      parse(l2ConfigFile,"mReadingTimeLambda",mReadingTimeLambda);
      parse(l2ConfigFile,"mParsingTimeLambda",mParsingTimeLambda);
      parse(l2ConfigFile,"mTCTimeLambda",mTCTimeLambda);
      isInitialised = true;
    }
};

struct sCarrierInfo {
  vec mCarrierFreq; ///List of carrier frequencies
  vec mCarrierBW; ///bandwidth of each carrier
  int mNumCarrier; ///Total number of carriers
  bvec isMacroCarrier; ///True if carrier is assigned to macro-layer
  bvec isPicoCarrier; ///True if carrier is assigned to small cell layer
  
  sCarrierInfo(){
    //mCarrierFreq.resize(0);
   // mCarrierBW.resize(0);
    //meNBId.resize(0);
    //mDLResourceBlocksPerCarrier.resize(0);
    //mULResourceBlocksPerCarrier.resize(0);
  }
};

struct sCarrierInfoPerCell{
 
   double mCarrierFreq; ///Carrier frequency
   double mCarrierBW; ///bandwidth of the carrier
   bool mIsPCell;
   bool mIsActive; ///Cell is active
   int mAssociatedeNBId; ///MeNB/SeNB with which this carrier is associated - relevant for UE only
   int mCarrierIndex;
   int N_CCE;
   int mFronthaulHARQDelay;
   
   sCarrierInfoPerCell(){
     mIsPCell = false;
     mIsActive = false;
     mAssociatedeNBId = -1;
     mCarrierIndex = -1;
     mFronthaulHARQDelay = 0;
   }
};

struct Location
{
  double x ;
  double y;
  int serverNodeID;
  Location()
  {
    x =0.0 ; 
    y =0.0 ;
    serverNodeID =-1;
  }
};

 
struct sHandoverContext{
  vector<sCarrierInfoPerCell> tCarrierInfo;
  int teNBId;
  //sCQIConfigInfo tCQIConfigInfo;
  //vector<sCQIConfigInfo> tCQIConfigInfoCoMP;
  bool isWithABS;
  bool isWithCoMP;
//   eCQIType cqiType;
  int tPCellIndex; ///Indicates the PCell carrier index;
  bool isModeAdaptation;
  uint8_t tNumHarqTxProcess;
  uint8_t tNumHarqRxProcess;
  uint8_t tMaxNumHarqRetx;
  uint8_t tFrontHaulDelay;
};


enum eSF_Value { DL, UL, S};

enum eTDD_Frame_Structure { TDD_Config0, TDD_Config1, TDD_Config2,TDD_Config3,TDD_Config4,TDD_Config5,TDD_Config6};

struct sTDD_InfoBlock{
  int mTDD_Subframe_Config[10];
  int mNum_DL_HARQ_process; 
  int mNum_UL_HARQ_Process;
  int mTDD_SubFrame_Delay[10];
  int mTDD_RTxSubframe[10];
  int mTDD_UL_Scheduling_delay[10];
};






/**
 * @brief
 * Structure passed between L1 and L2 Simulator which acts as a interface.
 * It also contains some configurable parameters for L2 Simulator.
*/
// struct sL2SimConfig{
//   L2UlDlType uldlMode;
//   Array <Location> mdropLocation;
//   Array < Array<Location> > mdropLocationUE;
//   int mNumberOfeNB; /**< Number of eNB */
//   vector<int> mNumberOfUEpereNB; /**< Number of UE per eNB */
//   vector<int> mNumberOfAntennapereNB; /**< Number of Physical Antenna per eNB */
//   Array <ivec> mNumberOfAntennaperUE; /**< Number of Physical Antenna per UE */
//   vector<bool> isMacroServerNodepereNB; /**< Is the enodeb a MACRO or PICO */
//   int mNumberOfRBPerUE; /**< Number of RB associated with UE */
//   int mNumberOfAppPerRB;  /**< Number of Application associated with Logical Channel */
//   //int mULResourceBlocks; /**< No. of UL Resource Blocks */
//   ivec mNumberOfAntennaPerUE;
//   vec mTxPowInDBmPerUE;
//   ivec mIDCellPereNB;
//   bool 	mIsNonIdealPDCCHEnabled; //Sayee 02 May 2014
//   int mPDCCHCount; /**< Number of PDCCH revervation symbols in a subframe (assumed to be a constant)*/
//   int mPeriodicPHR_Timer;
//   uint32_t mDiscardTimer;
//   int mPDCPSequenceSize;
//   
//   double mSimulationTime;   /**< In Seconds */
//   vector< vector<cTrafficClass::eTrafficType> > mTrafficType;
//   int mSequenceNumberSize; /**< It is configured by RRC to use either 5/10 bit SN */
//   int mMaxARQRetxThreshold;   /**< It is configured by RRC to use 1, 2, 3, 4, 6, 8, 16, 32 as values */
//   int mPollPDU;		   /**< It is configured by RRC to use 4, 8, 16, 32, 64, 128, 256, Infinity as values */
//   int mPollByte; 	   /**< It is configured by RRC to use (in Kilo-Bytes) 25, 50, 75, 100, 125, 250, 375, 500, 750, 1000, 1250, 1500, 2000,3000, infinity as values */
//   vector <int> mRLCMode; /**< It is configured by RRC to use the RLC Mode used i.e.
// 		     0 - TM_RLC_MODE
// 		     1 - UM_RLC_MODE
// 		     2 - AM_RLC_MODE
// 		*/
// 		
//   int mNumberOfLCPerUE ;
// 		
//   int mTReOrderingExpire_UM;
//   
//   int mTReOrderingExpire_AM;
//   
//   //Only for 10010
//   double mFrameDuration;
//   
//   int mNumOfDLHarqProcess;
//   int mNumOfULHarqProcess;
//   int mMaxNumOfHarqTransmissions;
//   
//   sFTPTrafficGeneratorConfig ftpConfig;
//   sHTTPTrafficGeneratorConfig httpConfig;
//   sVideoTrafficGeneratorConfig videoConfig;
//   sCBRTrafficGeneratorConfig cbrConfig;
//   
//   int nCRSPorts;
//   int nLayers;
//   int nCodeWords;
//   string mTransmissionMode;
//   string mULTransmissionMode;
//   string mDLTransmissionMode;
//   int nLayersUL;
//   int nCodeWordsUL;
//   
//   //Sim include from L2 support is not inclueded. so giving eroors
//   
//   enum AperiodicReportingMode_E aperiodicMode; 
//   enum PeriodicReportingMode_E periodicMode;
// 
//   int mFeedbackProcessingTime;
//   int mCQIReportDelay;
//   bool mIsAperiodicCQIEnabled;	
//   bool mIsSubbandCQIEnabled;
//   bool mIsDrxEnabled;
//   
//   bool mIsTDD;
//   sTDD_InfoBlock mTDD_InfoBlock;
//   int 	mTDD_ConfigIndex;
//   
//   bool mIsOLLAenabledForDL;
//   bool mIsOLLAenabledForUL;
//   
//   bool mIsModeAdaptationEnabled;
//   bool isSemiOpenloopScheme;
//   bool mIsMUMIMOEnabled;
//   int nUserPairing;
//   
//   bool mIsWithAPIForScheduler;
//   bool mIsWithAPIForMIMOModeAdaptation;
//   bool mIsWithAPIForABSeICIC;
//   
//   int mWindowSizeForDL;
//   int mWindowSizeForUL;
//   
//   double mTargetBLERForDL;
//   double mTargetBLERForUL;
//   
//   bool mIsTTIBundlingEnabled;
//   bool mIsNonAdaptiveHarqEnabled;
//   
//   bool isWithABS;
//   bvec  ABSBitPattern;
//   
// #ifdef USING_RRC
//   bool isWithRRC;
// #endif
//   bool isTCPEnabled;
//   int mtcpRxWindowsize;
//   bool isWithX2;
//   
//   // Added for New PF Scheduler (PF Scheduler V2)  
//   bool mIsWithPF;
//   bool mIsWithPFSchedulerV2;
//   
//   //Added for CoMP support 
//   bool isWithCoMP;
//   CompSchemes mCompScheme;
//   int maxCoMPUEsperMacro;
//   
//   bool mIsRAT2Enabled; // Added for Resource Allocation Type 2 support
//   
//   //Added for Multiple CSI support
//   bool isWithMultipleCSI;
//   int nCSIProcess;
//   
//   ivec centralUnits;
//   bool mIsCAEnabled;
//   sCarrierInfo mCarrierInfo;
//   int mFronthaulHARQDelay;
//   
//   ivec centralUnitIDsPerServerNode; 
//   Array<ivec> associatedServerNodesPerCentralUnit;
//   Array<ivec> associatedServiceNodesPerCentralUnit;
//   Array<ivec> associatedServiceNodesPerServerNode;
//   
//   enum TPC_Type mTPC;
//   
//   ivec arrTimePereNB;
//   ivec startTimePerUE;
//   bool mIsEPDCCHEnabled;
// 
//   ivec UElist;
// 
//   // UL specific variables
//   
//    sL2SimConfig(){
//      mPDCCHCount=0;
//      mIsNonIdealPDCCHEnabled = false;
//      mIsEPDCCHEnabled= false;
//      mDiscardTimer=0;
// //      mTReOrderingExpire_UM = 10;
// // //     mSimulationTime=1; 
// // //     mNumberOfeNB=1;
// // //     mNumberOfUEpereNB=10;
// // //     mNumberOfRBPerUE=1;
// // //     mNumberOfAppPerRB=1;
// // //     mTrafficType.resize(mNumberOfRBPerUE,vector<cTrafficClass::eTrafficType>(mNumberOfAppPerRB));
// // //     mTrafficType[0][0]=cTrafficClass::TRAFFIC_TYPE_FTP;
// // //    // mTrafficType[0][1]=cTrafficClass::TRAFFIC_TYPE_VOIP;
// // //  //   mTrafficType[1][0]=cTrafficClass::TRAFFIC_TYPE_FTP;
// // // //     mTrafficType[1][1]=cTrafficClass::TRAFFIC_TYPE_VOIP;
// // //     mBand=10;
// // //     mSequenceNumberSize =5;  
// // //     mMaxRetxThreshold =4;
// // //     mPollPDU = 4;
// // //     mPollByte = 25;
// // //     mRLCMode = 2;
//      mIsOLLAenabledForDL=false;
//      mIsOLLAenabledForUL=false;
//      mTargetBLERForDL=0.1;
//      mTargetBLERForUL=0.1;
//      mWindowSizeForDL=5;
//      mWindowSizeForUL=5;
//      mPeriodicPHR_Timer = 10;
//      mFronthaulHARQDelay = 0;
//      mIsAperiodicCQIEnabled=false;
//      mIsSubbandCQIEnabled=false;
//      nCRSPorts=-1;
//     
//      mIsModeAdaptationEnabled=false;
//      isSemiOpenloopScheme=false;
//      mIsCAEnabled = false;
//      arrTimePereNB.set_size(0);
//      startTimePerUE.set_size(0);
//      UElist.set_size(0);
//    }
// };


 class sL2SimConfig{
 public:
    string l2SupportConfigFile; 
    ivec mNumberOfAppPerRB;
    vector< vector<eTrafficType> > mTrafficType;
    sFTPTrafficGeneratorConfig ftpConfig;
    sHTTPTrafficGeneratorConfig httpConfig;
    sCBRTrafficGeneratorConfig cbrConfig;
    sXRTrafficGeneratorConfig xrConfig;
    sVideoTrafficGeneratorConfig videoConfig;
    int mSimulationTime;   /**< In Subframes */
    ivec startTimePerUE;
    ivec UElist;
    ivec arrTimePereNB;
    ivec interArrTimePerUE;
    double frameDuration;
    int nRadioBearers;
     
    sL2SimConfig()
    {
     
    }
    sL2SimConfig(string l2ConfigFile,string tcpConfigFile,ivec ueList,int nBS,int simTime,double slotDuration);
 };

struct sL2SimConst
{
  int mMacHeaderSize;
  int mPDCPHeaderSize;
  int mRLCHeaderSize;
  int mMaxPktSizeInBytes;
  int mCRCSize;
  sL2SimConst()
  {
    mMacHeaderSize=3;
    mPDCPHeaderSize=2;
    mRLCHeaderSize=2;
    mCRCSize =3;
    mMaxPktSizeInBytes=1500;
  }
};

struct sUELCPrioritizationReturn
{
    int rbId;
    int bufferSize;
    int dataAllocInCW1;
    int dataAllocInCW2;
    int dataAlloc;
    sUELCPrioritizationReturn()
    {
        rbId = -1;
        bufferSize = 0;
        dataAllocInCW1 = 0;
        dataAllocInCW2 = 0;
    }
   
};

struct sScheduleReturn
{
    enum CodeWordTrans
    {
        FRESH_TRANSMISSION = 0,
        RE_TRANSMISSION,
        NO_TRANSMISSION
    };
    vector< int > UEIndex;
    vector <CodeWordTrans> isRetx1;
    vector <CodeWordTrans> isRetx2;
    vector < vector<bool> > mResourceBlockMapping;
    Array < ivec > tsympos;
    vector < ivec > scheduledVRBs;
    
    vector< vector<int> > mSchInfo_1; // Outer vector dimension is over UEs x No.of Radio Bearers. Inner vector dimension is fixed to 3 (First Item: UE_ID, Second Item: TB Size, Third Item: RadioBearer_ID)
    vector< vector<int> > mSchInfo_2;
    
//     vector<int> slotsperuser;
    
    vector< int > mcs_1;
    vector< int > mcs_2;
    vector< int > tbSize_1;
    vector< int > tbSize_2;
    vector< int > pmi;
    vector < vector <cmat> > PrecoderPerRB;
    vector< int > transmissionMode;
    vector< int > nLayers;
    vector< bool > isPaired;
    int carrierIndex; ///Indicates the carrier frequency
    vector<bool> isSemiOpenloopScheme;

  sScheduleReturn()
  {
    scheduledVRBs.resize(0);
    PrecoderPerRB.resize(0);
    tsympos.set_length(0);
  }
  ~sScheduleReturn()
  {
          mSchInfo_1.clear();
	  mSchInfo_2.clear();
	  mcs_1.clear();
	  mcs_2.clear();
	  pmi.clear();
	  transmissionMode.clear();
	  nLayers.clear();
	  isRetx1.clear();
	  isRetx2.clear();
	 

        for(int i=0;i<(int)mResourceBlockMapping.size();i++)
            mResourceBlockMapping[i].clear();
	for(int i=0;i<(int)scheduledVRBs.size();i++)
         scheduledVRBs[i].set_length(0);
        
	mResourceBlockMapping.clear();
	scheduledVRBs.clear();
	isPaired.clear();
	isSemiOpenloopScheme.clear();
    }
    void fUpdateResourceMapping(int UEId, ivec RBGId)
    {
        //cout<<" UE Id : "<< UEId<< " RBGId : "<<RBGId<<endl;
        for(int i=0;i<(int)UEIndex.size();i++)
        {
            if(UEIndex[i] == UEId)
            {
                for(int j=0;j<RBGId.size();j++)
                {
                    mResourceBlockMapping[i][RBGId(j)] = true;
	}
      }
                    //cout<<"RBGId(j): " <<RBGId(j)<<endl;
    }
  }
  void fUpdateScheduledVRBs(int UEId, ivec vrbNumbers)
  {
    for(int i=0;i<UEIndex.size();i++)
    {
      if(UEIndex[i] == UEId)
      {
	append(scheduledVRBs[i],vrbNumbers);
      }
    }
  }
  
   void fUpdateSymbolPosition(int UEId, ivec tsymbols)
  {
    for(int i=0;i<UEIndex.size();i++)
    {
      if(UEIndex[i] == UEId)
      {
	tsympos(i)=tsymbols;
      }
    }
  }
  
  void fUpdateTxMode(int UEId, int txMode)
  {
    for(int i=0;i<UEIndex.size();i++)
    {
      if(UEIndex[i] == UEId)
      {
	transmissionMode[i]=txMode;
      }
    }
  }
  void fUpdateMCS(int UEId, ivec mcs)
  {
    for(int i=0;i<UEIndex.size();i++)
    {
      if(UEIndex[i] == UEId)
      {
	mcs_1[i]=mcs(0);
	if(mcs.length()==2)
	  mcs_2[i]=mcs(1);
      }
    }
  }
  void fUpdatePrecoderPerRB(int UEId, ivec RBGId,int nLayers, vector<cmat> tPrecoderPerRB)
  {
    for(int i=0;i<(int)UEIndex.size();i++)
    {
      if(UEIndex[i] == UEId)
      {
	for(int j=0;j<RBGId.size();j++)
	{
	  if(tPrecoderPerRB[j].cols() > nLayers)
	  {
	    
	    PrecoderPerRB[i][RBGId(j)] = tPrecoderPerRB[j].get_cols(0,nLayers-1);
	  }
	  else
	  {
	    PrecoderPerRB[i][RBGId(j)] = tPrecoderPerRB[j];
	  }
	}
      }
    }
  }
  
  void fPrintResourceMapping(int UEId, int index)
  {
    cout<<" UE Id : "<< UEId<< " Index : "<<index<<endl;
    
    for(int i=0;i<(int)mResourceBlockMapping[index].size();i++)
    {
      if(mResourceBlockMapping[index][i])
      {
	cout<<i<<" allocated"<<endl;
      }
      else cout<<i<<" not allocated"<<endl;
        }
    cout<<endl;
    }
};

struct sMACdata
{
  int mUEId;
  cPacketQueue mPktQueue;
  bool mError;
  sMACdata(int tUEId, cPacketQueue tPktQueue)
  {
    mError =false;
    mUEId=tUEId;
    mPktQueue=tPktQueue;
  }
   sMACdata()
  {
    mError =false;
    mUEId=0;
  }
  ~sMACdata()
  {
    mPktQueue.~cPacketQueue();
  }
  int fGetUEId(){
    return mUEId;
  }
};

struct sScheduler2Multiplexer
{
  int mUEId;
  int mRBId;
  int mAvailableBytes;
  bool reTx;
  sScheduler2Multiplexer(){
    mUEId=-1;
    mRBId=-1;
    mAvailableBytes=0;
    reTx = false;
  }
};

struct sUEBufferInfo
{
    int mUEId;
    int mDataInBuffer;

    sUEBufferInfo()
    {
        mUEId = -1;
        mDataInBuffer = 0;
    }
};

struct sUERBBufferInfo
{
  vector<int > mUEId;
  vector<int > mRBId;
  vector<int > mDataInBuffer;
  //vector<int > isSmallSizePktRB;
};

struct sUEswithData
{
  vector<int > mUEId;
  vector<int > mRBId;
  vector<int > mDataInBuffer;
};

struct sLCBufferSizeInfo
{
    int mRBId;
    int mLCId;
    int mDataInBuffer;
    double mRatio;
};

struct sLCPrioritizationInfo
{
    int mLCId;
    int mRBId;
    int mAllocationSize;
    int mMACSubHeaderSize;
    int mRLCMode;

    sLCPrioritizationInfo()
    {
        mLCId = -1;
        mRBId = -1;
        mAllocationSize = 0;
        mMACSubHeaderSize = 0;
	mRLCMode = -1;
    }
    
    void print()
    {
     cout<<"mLCId= "<<mLCId<<"\t"; 
     cout<<"mRBId= "<<mRBId<<"\t"; 
     cout<<"mAllocationSize= "<<mAllocationSize<<"\t"; 
     cout<<"mMACSubHeaderSize= "<<mMACSubHeaderSize<<"\t"; 
     cout<<"mRLCMode= "<<mRLCMode<<endl; 
    }
};

struct sLCConfigInfo
{
    int mLCID;
    int mLogicalChannelGroup;
    int mPriority;

    int mPrioritisedBitRate;
    int mBucketSizeDuration;

    int mQCI;
    int mQueueLength;
};

enum eMacControlElementLCIdType {LCID_INVALID = -1, PHR_LCID = 26, TRUNCATED_BSR_LCID = 28, SHORT_BSR_LCID = 29, LONG_BSR_LCID = 30};
enum eMacControlElementLength {SIZE_INVALID = -1, PHR_SIZE = 1, TRUNCATED_BSR_SIZE = 1, SHORT_BSR_SIZE = 1, LONG_BSR_SIZE = 3};

struct sMACControlElement
{
    eMacControlElementLCIdType mLCId;
    cByteSet mMacControlElementBytes;

	sMACControlElement()
	{
		mLCId = LCID_INVALID;
		mMacControlElementBytes.fReset();
	}
};

struct ULGrantInfoPerUE
{
    bool isULGrantAvail;
    int mcs_1;
    int mcs_2;
    vector<int> ULGrantRBs;
    int startRBIndex;
    int allocatedBytes;    /// Total bytes allocated
    int allocatedBytesCW1; ///Allocation for codeword 1
    int allocatedBytesCW2; ///Allocation for codeword 2
    int subframeNumber; // subframe number at which UL Grant is allocated
    int subframeNumberForULTransmit; // Actual subframe number where UE should perform UL Tx
    int numRBs;
    int nLayers;
    int txMode;
    int pmiIndex;
    int uplinkPowerControlCorrectionVal;
    int TTIBundleIndex; ///Indicates packet number within a TTI Bundle
};

struct HARQFeedbackSubframeWise
{
    bool feedback;
    int transmissionCount;
    int subframe;
    
    HARQFeedbackSubframeWise()
    {
      feedback = false;
      transmissionCount = -1;
      subframe	 = -1;
    }
};

struct HarqStatInfo
{
    int rnti;
    vector<HARQFeedbackSubframeWise> CW1HarqHistory;
    vector<HARQFeedbackSubframeWise> CW2HarqHistory;
    
    HarqStatInfo()
    {
      rnti = -1;
      CW1HarqHistory.resize(0);
      CW2HarqHistory.resize(0);
    }
    
    void fInit(int tRNTI)
    {
      rnti = tRNTI;
      CW1HarqHistory.resize(1);
      CW2HarqHistory.resize(1);
    }
};

// Moved to cRR.h
// struct scheduledUEInfo{

// struct scheduledUEInfoDL{
// 
//     ivec ueid;
//     Array< Array<ivec> > cqiIndices;
//     eCQIType cqiType;
//     TransmissionMode_E txMode;
//     vec mcsRate;
//     Array<ivec> mcsIndxPerCodeword;
//     Array<ivec> tbSizePerCodeword;
//     ivec pmi;
//     ivec nLayers;
//     ivec vrbs;
//     Array<imat> rbStructurePerRB;
//     bool isPaired;
//     int carrierIndex;
// 
//     scheduledUEInfoDL(){
// 
//       ueid.set_length(0);
//       cqiIndices.set_length(0);
//       cqiType=CQIType_UNINIT;
//       txMode=_TRANSMISSION_MODE_UNINIT_;
//       mcsRate.set_length(0);
//       nLayers.set_length(0);
//       vrbs.set_length(0);
//       rbStructurePerRB.set_length(0);
//       tbSizePerCodeword.set_size(0);
//       mcsIndxPerCodeword.set_size(0);
//       isPaired=false;
//   }
// };

struct sSchedulerInfo{
      int mSubframe;
      int mCarrierIndex;
      //sUEswithData mUEData;
//       vector<sCqiList> mCqiInfo;
      //vector<sHarqInfoToScheduler> &mHarqInfo;
      vector <HarqStatInfo> mHarqStatInfoList;
      vector <sUEBufferInfo> tUEData;
      //sScheduleReturn tSchedRet;
      
};


struct sABSRRCInfo
{
    bvec curABSSubframes;

    sABSRRCInfo()
    {
	curABSSubframes = zeros_b(40); // Periodicity for ABS pattern is set to 40 currently
    }

    void InitABSInfo(bool isMacro)
    {
        if (isMacro == false)
        {
	  curABSSubframes = "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0";
        }
        else
        {
	  curABSSubframes = "0 1 1 1 0 0 0 0 0 1 1 1 0 0 0 0 0 1 1 1 0 0 0 0 0 1 1 1 0 0 0 0 0 1 1 1 0 0 0 0";
        }
    }
    
    void InitABSInfo(bool isMacro,bvec ABSBitPattern)
    {
	if(isMacro == false)
	  curABSSubframes = zeros_b(40);
	else
	  curABSSubframes = ABSBitPattern;
    }
};

struct ServedUEStates
{
  //Any history info of an UE can be stored
  //I might contain cellIds in which UE served .
  //vector of cell ID
  int initialsubframe ;
  std::list <int > travelledcellIDs;
  std::list <int > durationofStay;
  ServedUEStates()
  {
    initialsubframe =0;
    travelledcellIDs.resize(0);
    durationofStay.resize(0);
  }
};


struct sCQIConfigInfo{

//     TransmissionMode_E mTxMode;
    int nCodewords;
    int rank;
    int nPorts;
//     eCQIType mCQIType;
    ivec nodeGroup;
//     CompSchemes compScheme;
    
    sCQIConfigInfo(){

//         mTxMode=_TRANSMISSION_MODE_UNINIT_;
        nCodewords=-1;
        nPorts=-1;
//         mCQIType=CQIType_UNINIT;
// 	compScheme=_COMP_SCHEME_UNINIT_;
    }
};

struct sRxInfo{
  
  int tSubFrame;
  int teNBID;
  int HARQId;
  vector<cByteSet> MACPDU;
  bvec IsSucess;
  //vector<cStatusPDUs> mStatusPDUs;
  ivec codeWordIndices;
  int cellIndx;
  
  sRxInfo(){
    HARQId = 255;
    MACPDU.resize(0);
    cellIndx = -1;
    //IsSucess.resize(0);
    //codeWordIndices.resize(0);
  }

};

struct sSCellConfig{
  sCarrierInfoPerCell mCarrierInfoPerCell;
  int mAssociatedeNB;
};

/*!
 * The following structure captures PDCCH statistics on a sub frame level
 */
struct	sPDCCHStatistics
{
    int		tSubFrame;				//!Sub frame number; goes beyond 9;
    int		mNumUEConsideredForPDCCHAllocation;
    uint32_t	mTotalCCEs;				//!Number of CCEs avaialble in a sub frame
    uint32_t	mCCEsAllocated;				//!Number of CCEs that are allocated in a sub frame
    uint32_t	mNumUEWithAggreLevelOne;		//!Captures the number of UEs that are assigned the aggregation level 1*
    uint32_t	mNumUEWithAggreLevelTwo;		//!Captures the number of UEs that are assigned the aggregation level 2*
    uint32_t	mNumUEWithAggreLevelFour;		//!Captures the number of UEs that are assigned the aggregation level 4*
    uint32_t	mNumUEWithAggreLevelEight;		//!Captures the number of UEs that are assigned the aggregation level 8*
    uint32_t	mNumBlockedUEWithSearchSpaceFailure;
    uint32_t	mNumUERejectedDueToLackOfCCE;
    uint32_t	mNumUEAllocatedWithCCE;
    double	mFracCCEsAllocated;			//!The fraction of available CCES allocated in a sub frame
    double	mFracUEBlockedDueToSearchSpaceFailure;

    sPDCCHStatistics()
    {
        mTotalCCEs = 0;
        mNumUEConsideredForPDCCHAllocation = 0;
        mCCEsAllocated = 0;
        mNumUEWithAggreLevelOne = 0;
        mNumUEWithAggreLevelTwo = 0;
        mNumUEWithAggreLevelFour = 0;
        mNumUEWithAggreLevelEight = 0;
        mFracCCEsAllocated = 0.0;
        mNumUEAllocatedWithCCE = 0;
        mNumBlockedUEWithSearchSpaceFailure = 0;
        mNumUERejectedDueToLackOfCCE = 0;
        mFracUEBlockedDueToSearchSpaceFailure = 0.0;
    }
};

struct sCQIListPerCell{
//   vector<sCqiList> mCqiList;
  int mCarrierIndex;
};

struct sTxProcInfo{
  sScheduleReturn  tSchedulerReturn;
  int tSubFrame;
//   cDLConfigRequest dlConfigRequest;
//   cTXRequest tTxRequest;
  sL2SimConfig sSimConfig;
  sCarrierInfo tCarrierInfo;
  
  sTxProcInfo(){
  }
};

uint16_t fComputeSFNSNFromTTICount(int subframeNumber);
int fGetTTICountFromSFNSF(uint16_t tSFNSF);
vector<cByte> fGetVecByteFromUNIT(uint8_t tVal);
vector<cByte> fGetVecByteFromUNIT(uint16_t tVal);
vector<cByte> fGetVecByteFromUNIT(uint32_t tVal);

uint32_t fGetUNIT32FromVecByte(vector<cByte> tByte);
uint16_t fGetUNIT16FromVecByte(vector<cByte> tByte);
uint8_t fGetUNIT8FromVecByte(vector<cByte> tByte);
uint8_t getCQIindexForCW1(unsigned int CQIIndexForCW0,unsigned int DifferentialCQI);//! get CQIindexForCW1 returns CQI index for second CW using the CQI index for the 1st CW and the quantized offset level of the 2nd CW from the 1st CW
int fFindDataAllocated(vector< std::vector< int > > arg1, int arg2);

eTrafficType getTrafficType(string name);

#endif
