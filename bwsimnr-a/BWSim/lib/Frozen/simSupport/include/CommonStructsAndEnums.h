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

#ifndef _COMM_STRUCT_H_
#define _COMM_STRUCT_H_

#include "simSupport.h"


/*!Enums ...!*/


#define lightSpeed	2.99792458e8

enum TransmissionMode_E{
  
  _TRANSMISSION_MODE_UNINIT_=-1,
  _TRANSMISSION_MODE_1_,
  _TRANSMISSION_MODE_2_,
  _TRANSMISSION_MODE_3_,
  _TRANSMISSION_MODE_4_,
  _TRANSMISSION_MODE_5_,
  _TRANSMISSION_MODE_6_,
  _TRANSMISSION_MODE_7_,
  _TRANSMISSION_MODE_8_,
  _TRANSMISSION_MODE_9_,
  _TRANSMISSION_MODE_10_,
  _TRANSMISSION_MODE_11_,
  _TRANSMISSION_MODE_NR_
  };

enum TransmissionScheme_E{ 
  
  _TRANSMISSION_SCHEME_UNINIT_=-1,
  _TRANSMISSION_SCHEME_SINGLE_PORT_CRS_, /// Single Layer and Single port
  _TRANSMISSION_SCHEME_TRANSMIT_DIVERSITY_, /// SFBC(2 ports), SFBC-FSTD(4 ports)
  _TRANSMISSION_SCHEME_LARGE_DELAY_CDD_, /// OL-SM
  _TRANSMISSION_SCHEME_CL_SM_,
  _TRANSMISSION_SCHEME_CL_MU_MIMO_,
  _TRANSMISSION_SCHEME_CL_BF_,
  _TRANSMISSION_SCHEME_SINGLE_PORT_DMRS_,
  _TRANSMISSION_SCHEME_CL_2LAYERS_DMRS_,
  _TRANSMISSION_SCHEME_CL_MAX8LAYERS_DMRS_,
  _TRANSMISSION_SCHEME_COMP_,
  _TRANSMISSION_SCHEME_OLMUMIMO_,
  _TRANSMISSION_SCHEME_NR_CL_
  };


enum MOD_ALPHABET_SIZE
{
  PiBy2BPSK=1, QAM_4 = 2, QAM_16 = 4, QAM_64 = 6, QAM_256 = 8
};

  
  
enum CompSchemes{
  
  _COMP_SCHEME_UNINIT_=-1,
  _COMP_SCHEME_JOINT_TX_,
  _COMP_SCHEME_JOINT_RX_,
  _COMP_SCHEME_DPS_,
  _COMP_SCHEME_DB_

};


enum Waveform_E{
  
  _Waveform_UNINIT_ = -1,
  _Waveform_OFDMA_,
  _Waveform_SCFDMA_
  
};


enum _UE_CATEGORY_
{
    CATEGORY_1 = 250368,
    CATEGORY_2 = 1237248,
    CATEGORY_3 = 1237248,
    CATEGORY_4 = 1827072,
    CATEGORY_5 = 3667200,
    CATEGORY_6 = 3654144,
    CATEGORY_7 = 3654144,
    CATEGORY_8 = 35982720,
    CATEGORY_9 = 5481216,
    CATEGORY_10 = 5481216
};


enum TransmissionType{
    
    _TRANSMISSION_TYPE_UNINIT_ = -1,
    _TRANSMISSION_TYPE_DL_,
    _TRANSMISSION_TYPE_UL_,
    _TRANSMISSION_TYPE_SL_
    
};


/*!Structs ...!*/

enum InitialAssociationMetric{

  _UNINIT_=-1,
  _RSRP_=0,
  _RSRQ_=1,
  _PATHLOSS_=2
};

struct sBeamPairLink
{
  int srvrBeamID=0;
  int srvcBeamID=0;
  double rspIndBm=0.0;
  double rsrpIndBm=0.0;
  double rsrqIndB=0.0;
};

struct AssociationChannelInfo_S
{
    mat dlCouplingGain;
    mat ulCouplingGain;
    mat dlRSRP;
    mat ulRSRP;
    vec serviceNodeNoiseVariance;
    vec serverNodeNoiseVariance;
    
    AssociationChannelInfo_S()
    {
        mat k(0,0);
        dlCouplingGain = ulCouplingGain = dlRSRP = ulRSRP = k;
        serviceNodeNoiseVariance.set_length(0);
        serverNodeNoiseVariance.set_length(0);
    }
};

class AssociationInfo_S
{
public:  
  ivec serverNodes;
  ivec serviceNodes;
  Array<ivec> associatedServiceNodesPerServerNode;
  ivec associatedServerNodesForEachServiceNode;
  Array <sBeamPairLink> associatedBeamPairLinkforEachServiceNode;
  Array<ivec> serviceNodesPerTask;  //Used for Mcell
  vec geometrySINRindBperServiceNode;
  Array<ivec> coordinatingServerNodesForEachServiceNode;
  
  
  AssociationInfo_S()
  { 
    geometrySINRindBperServiceNode.set_length(0);
    serviceNodesPerTask.set_length(0);
  }

  void reassociate(int serviceNode, int serverNode){
    
    int indx=find(serviceNodes,serviceNode);
    if(indx!=-1)
    {
      int oldSrvrNode, newSrvrNode=serverNode;
      oldSrvrNode=associatedServerNodesForEachServiceNode(indx);
      
      if(oldSrvrNode!=newSrvrNode)
      {
	associatedServerNodesForEachServiceNode(indx)=newSrvrNode;
	int oldIndx=find(serverNodes,oldSrvrNode);
	int newIndx=find(serverNodes,newSrvrNode);
	associatedServiceNodesPerServerNode(oldIndx).del(find(associatedServiceNodesPerServerNode(oldIndx),serviceNode));
	append(associatedServiceNodesPerServerNode(newIndx),serviceNode);
	
	
      }
    }
    else
      cout<<"Unknown serviceNode to reassociate()..."<<endl;
  }
  
  bool isMyServerNode(int serviceNode, int serverNode){
    
    int indx=find(serviceNodes,serviceNode);
    if(indx!=-1)
      return((associatedServerNodesForEachServiceNode(indx)==serverNode));
    else
    {
      cout<<"Unknown serviceNode to isMyServerNode()..."<<endl;
      return(false);
    }
    
  }
  
  int getMyAssociatedServerNode(int serviceNode){
    
    int indx=find(serviceNodes,serviceNode);
    if(indx!=-1)
      return(associatedServerNodesForEachServiceNode(indx));
    else
    {
      cout<<"Unknown serviceNode to getMyAssociatedServerNode()..."<<endl;
      abort();
    }
  }
  
  ivec getMyAssociatedServiceNodes(int serverNode){
    
    int indx=find(serverNodes,serverNode);
    if(indx!=-1)
      return(associatedServiceNodesPerServerNode(indx));
    else
    {
      cout<<"Unknown serviceNode to getMyAssociatedServiceNodes()..."<<endl;
      abort();
    }
  }
    void update()
    {
        associatedServiceNodesPerServerNode.set_size(serverNodes.length());
        for(int srvr_cnt=0;srvr_cnt<serverNodes.length();srvr_cnt++)
            associatedServiceNodesPerServerNode(srvr_cnt).set_length(0);
        for(int srvc_cnt=0;srvc_cnt<serviceNodes.length();srvc_cnt++)
        {
            int srvrNodeIndx = find(serverNodes,associatedServerNodesForEachServiceNode(srvc_cnt));
            append(associatedServiceNodesPerServerNode(srvrNodeIndx),serviceNodes(srvc_cnt));
        }
    }
    void load(string linkTableFileName)
    {
        ivec serverNodeType,serverNodeCentralUnitID,serviceNodeCentralUnitID,serverIDsPerServiceNode,serverNodeNetworkIDs,serviceNodeNetworkIDs;
        if(parse(linkTableFileName,"serverNodeList",serverNodes)==false)  {	cout<<"[both:]Error... Unable to load serverNodes from the file in associateNodes()..."<<endl;	abort();  }
        if(parse(linkTableFileName,"serviceNodeList",serviceNodes)==false)  {	cout<<"[both:]Error... Unable to load serviceNodes from the file in associateNodes()..."<<endl;	abort();  }
        if(parse(linkTableFileName,"serverNodeCentralUnitID",serverNodeCentralUnitID)==false)  {	cout<<"[both:]Error... Unable to load serverNodeCentralUnitID from the file in associateNodes()..."<<endl;	abort();  }
        if(parse(linkTableFileName,"serviceNodeCentralUnitID",serviceNodeCentralUnitID)==false)  {	cout<<"[both:]Error... Unable to load serviceNodeCentralUnitID from the file in associateNodes()..."<<endl;	abort();  }

        associatedServiceNodesPerServerNode.set_size(serverNodes.length());
        for(int serverNode_cnt=0;serverNode_cnt<associatedServiceNodesPerServerNode.length();serverNode_cnt++)
            associatedServiceNodesPerServerNode(serverNode_cnt).set_size(0);
        associatedServerNodesForEachServiceNode = zeros_i(serviceNodes.length());
        associatedBeamPairLinkforEachServiceNode.set_length(serviceNodes.length());
        coordinatingServerNodesForEachServiceNode.set_length(serviceNodes.length());
        
    }
    void printDetailedAssociationInfo(string filename)
    {
        ofstream fout(filename.c_str(),std::ios::out);
        fout<<"%ServerNode,\t txPanelID,\t rxPanelID,\t ServerBeam,\t ServiceBeam,\t associatedServiceNodes,\t nAssociatedNodes"<<endl;
        int nTxNodes = serverNodes.length();
        Array<sBeamPairLink> beamPairLink = associatedBeamPairLinkforEachServiceNode;
        Array<Array<Array<ivec>>> associatedNodesPerBeamPerSrvrNode(nTxNodes);
    
        for(int srvr_cnt=0; srvr_cnt<nTxNodes; srvr_cnt++)
        {
            ivec srvcIndices=find(serviceNodes,associatedServiceNodesPerServerNode( srvr_cnt ));
            for(int srvc_cnt=0;srvc_cnt<srvcIndices.length();srvc_cnt++)
            {
                
                int srvrBeamID = beamPairLink(srvcIndices(srvc_cnt)).srvrBeamID;
                if(srvrBeamID>=associatedNodesPerBeamPerSrvrNode( srvr_cnt ).length())
                {
                    associatedNodesPerBeamPerSrvrNode( srvr_cnt ).set_length(srvrBeamID+1,true);
                }
                int srvcBeamID = beamPairLink(srvcIndices(srvc_cnt)).srvcBeamID;
                if(srvcBeamID>=associatedNodesPerBeamPerSrvrNode( srvr_cnt )(srvrBeamID).length())
                {
                    associatedNodesPerBeamPerSrvrNode( srvr_cnt )(srvrBeamID).set_length(srvcBeamID+1,true);
                }
                append(associatedNodesPerBeamPerSrvrNode( srvr_cnt )(srvrBeamID)(srvcBeamID),serviceNodes(srvcIndices(srvc_cnt)));
            }
        }
        for(int tx_cnt=0;tx_cnt<nTxNodes;tx_cnt++)
                   for(int tx_beam=0;tx_beam<associatedNodesPerBeamPerSrvrNode(tx_cnt).length();tx_beam++)
                        for(int rx_beam=0;rx_beam<associatedNodesPerBeamPerSrvrNode(tx_cnt)(tx_beam).length();rx_beam++)
                            if(associatedNodesPerBeamPerSrvrNode(tx_cnt)(tx_beam)(rx_beam).length())
                                fout<<serverNodes(tx_cnt)<<",\t "<<tx_beam<<",\t "<<rx_beam<<",\t "<<associatedNodesPerBeamPerSrvrNode(tx_cnt)(tx_beam)(rx_beam)<<"\t "<<associatedNodesPerBeamPerSrvrNode(tx_cnt)(tx_beam)(rx_beam).length()<<endl;
        
        fout.close();
    }
    void printAssociationInfo(string associationInfoFileName,string beamAssociationFileName)
    {
        if(currentTaskid==0) 
        {
            bool isMultibeam = false;
//             if(traceValuePerTraceName["printAssociationInfo"]=="true")
            {
                
//                 #ifdef ASSOCIATION_DEBUG
                cout<<"[detl:]Printing Association Info into "+associationInfoFileName+" file."<<endl;
//                 #endif
                std::ofstream fout1(associationInfoFileName.c_str(), std::ios::out);
                
                fout1<<"ServerNodes = "<<serverNodes<<"\n\nnServerNodes = "<<serverNodes.length()<<"\n\nServiceNodes = "<<serviceNodes<<"\n\nnServiceNodes = "<<serviceNodes.length()<<"\n\nAssociated ServerNode For Each ServiceNode = "<<associatedServerNodesForEachServiceNode<<"\n\nAssociated ServiceNodes to  ServerNode\n\n"<<"ServerNode\t"<<"ServiceNodes\t"<<"#ServiceNodes\n"<<endl;
                
                for(int tx_cnt=0;tx_cnt<serverNodes.length();tx_cnt++)
                    fout1<<serverNodes(tx_cnt)<<" = "<<associatedServiceNodesPerServerNode(tx_cnt)<<"  "<<associatedServiceNodesPerServerNode(tx_cnt).length()<<endl;
                
                fout1<<"\nBeamAssociation : \n "<<endl;
                for(int tx_cnt=0;tx_cnt<serverNodes.length();tx_cnt++)
                {
                    ivec srvcIndices=find(serviceNodes,associatedServiceNodesPerServerNode(tx_cnt));
                    fout1<<serverNodes(tx_cnt)<<" = [";
                    for(int srvc_cnt=0;srvc_cnt<srvcIndices.length();srvc_cnt++)
                    {
                        if(associatedBeamPairLinkforEachServiceNode(srvcIndices(srvc_cnt)).srvrBeamID || associatedBeamPairLinkforEachServiceNode(srvcIndices(srvc_cnt)).srvcBeamID){isMultibeam=true;}  
                        fout1<<associatedBeamPairLinkforEachServiceNode(srvcIndices(srvc_cnt)).srvrBeamID<<",";
                    }
                    fout1<<"]"<<endl;
                }
                fout1.close();
                if(beamAssociationFileName!=""){
                    if(isMultibeam)
                    {
                        #ifdef ASSOCIATION_DEBUG
                        cout<<"[detl:]Printing Beam Association Info into "+beamAssociationFileName+" file."<<endl<<endl;
                        #endif
                        printDetailedAssociationInfo(beamAssociationFileName);
                    }
                    
                    for(int k=0;k<serviceNodes.length();k++)
                    {
                        cout<<"Node "<<serviceNodes(k)<<" is associated to node "<<associatedServerNodesForEachServiceNode(k);
                        if(isMultibeam)
                            cout<<" with Txbeam "<<associatedBeamPairLinkforEachServiceNode(k).srvrBeamID<<" and RxBeam "<<associatedBeamPairLinkforEachServiceNode(k).srvcBeamID;
                        cout<<endl;	
                    }
                    cout<<endl;}
                
            }
        }

    }
    void computeGeometrySINR(mat downlinkPowerMatrixInDBm,vec serviceNodeNoiseVariance)
    {
        double SINR=0.0;
        geometrySINRindBperServiceNode.set_length(serviceNodes.length());

        for(int iServiceNode=0;iServiceNode<downlinkPowerMatrixInDBm.rows(); iServiceNode++)
        {
            int srvcNodeIndx = (!serviceNodesPerTask.length())? iServiceNode : find(serviceNodes,serviceNodesPerTask(currentTaskid)(iServiceNode));
            int associatedServerNode=associatedServerNodesForEachServiceNode(srvcNodeIndx);
            int associatedServerNodeIdx=find(serverNodes,associatedServerNode);
            SINR=inv_dB(downlinkPowerMatrixInDBm(iServiceNode,associatedServerNodeIdx))/(sum(inv_dB(downlinkPowerMatrixInDBm.get_row(iServiceNode)))-inv_dB(downlinkPowerMatrixInDBm(iServiceNode,associatedServerNodeIdx)) + serviceNodeNoiseVariance(iServiceNode));
            
            geometrySINRindBperServiceNode(srvcNodeIndx) = dB(SINR);
        }
    }
};

struct NUConfig_S{
  
  int nuID;
  double subcarrierSpacingInHz;
  double ttiDurationInSec;
  double dlUsableBW;
  double ulUsableBW;
//   vec CPDurationInSec; //Length should match the columns in RBStructure files.
  string configFileFolder;
  int slotsPerTTI;
  double slotDuraton;
  double CellConfigPeriodicityInSec;
  int CellConfigPeriodicityInSlots;
  NUConfig_S()
  {
    nuID=1;
    subcarrierSpacingInHz=15000;
    ttiDurationInSec=1e-3;
    slotDuraton=0.0;
//     CPDurationInSec="5.208e-6 4.688e-6 4.688e-6 4.688e-6 4.688e-6 4.688e-6 4.688e-6 5.208e-6 4.688e-6 4.688e-6 4.688e-6 4.688e-6 4.688e-6 4.688e-6";
    dlUsableBW=0.9;
    ulUsableBW=0.9;
    configFileFolder="NU1";
    CellConfigPeriodicityInSec=1e-2;
  }
  void init()
  {
  
    slotDuraton=1e-3*15000.0/subcarrierSpacingInHz;
    
//   for(int i=0; i < CPDurationInSec.size(); i++)
//   {
//     symbolduration = (double)CPDurationInSec(i)+(1/subcarrierSpacingInHz);
// //     cout<<"symbol duration ::"<<symbolduration<<endl;
//     slotDuraton=slotDuraton+ symbolduration;
//   }
//   cout<<"slot duration ::"<<slotDuraton<<endl;
  slotsPerTTI= ceil_i(ttiDurationInSec/slotDuraton);
//   cout<<"slotsPerTTI"<<ceil(slotsPerTTI)<<endl;
  CellConfigPeriodicityInSlots=ceil_i(CellConfigPeriodicityInSec/slotDuraton);
  
  }
};


struct NUParams_S{
  
  int nuID;
  double subcarrierSpacingInHz;
  double ttiDurationInSec;
  
  double carrierFrequencyInMHz;
  double samplingFrequencyInHz;
  int subCarrierCountPerRB;
  
  //check for continous bandwidth parts for a particular Numerlogy.
  
  double bandwidthInHz;// So fill this bandwidth
  bool applyFilter;
  
  Array < ivec > BSBWPLocations; // over multiple locations of BWP.
  
  //Get the bitmap for all RBG in the common indexing, 
 
  int fftSize;
  int dlNRBs;
  int ulNRBs;
//   ivec CPLengths; 
  int slotsPerTTI;
  
  NUParams_S()
  {
    nuID=-1;
    subcarrierSpacingInHz=-1;
    samplingFrequencyInHz=-1;
    ttiDurationInSec=-1;
    carrierFrequencyInMHz=-1;
//     CPLengths.set_size(0);
    
    fftSize=-1;
    dlNRBs=-1;
    ulNRBs=-1;
    BSBWPLocations.set_length(0);
    applyFilter=false;
    slotsPerTTI=-1;
    subCarrierCountPerRB=-1;
  }
  void check()
  {
      //refer section 5.3.2 of TS 38.101-1&2 - Goraknath.
      double SCSinKHz =  subcarrierSpacingInHz/1000, BWinMHz = bandwidthInHz/1e6;
      vec BWs;int SCSIndx,bwIndx;double uLimit,lLimit;Array<ivec> nRBsPerSCSperBW;
      
      vec possibleSCS = 15*pow2(to_vec(getIntegers(0,3)));
      SCSIndx = find(possibleSCS,SCSinKHz);
      
      if(SCSIndx==-1)
      {
          cout<<"Invalid subcarrierSpacing.."<<endl;abort();
      }
      
      if(carrierFrequencyInMHz<=7125 && SCSIndx<3)
      {
          BWs = concat(5.0*getIntegers(1,6),30.0+10.0*getIntegers(1,7));
          uLimit = (SCSinKHz==15) ? 50 : 100, lLimit = (SCSinKHz==60) ? 10 : 5;
          nRBsPerSCSperBW = "{[25 52 79 106 133 160 216 270][11 24 38 51 65 78 106 133 162 189 217 245 273][0 11 18 24 31 38 51 65 79 93 107 121 135]}";
          SCSIndx = log2(SCSinKHz/15);
      }
      else if(carrierFrequencyInMHz>=24250 && (SCSIndx>1))
      {
          BWs = 50*pow2(to_vec(getIntegers(0,3)));
          uLimit = (SCSinKHz==60) ? 200 : 400,lLimit=50;
          SCSIndx -= 2;
          nRBsPerSCSperBW = "{[66 132 264][32 66 132 264]}";
      }
      else
      {
          cout<<"Invalid configuration of CarrierFrequency "<<carrierFrequencyInMHz/1000<<" GHz with subcarrierSpacing "<<SCSinKHz<<" kHz.."<<endl;abort();
      }
      bwIndx = find(BWs,BWinMHz);
      if(BWinMHz<lLimit || BWinMHz>uLimit || bwIndx==-1)
      {
          cout<<"subcarrierSpacing "<<SCSinKHz<<" kHz doesnot support bandwidth "<<BWinMHz<<" MHz..."<<endl;abort();
      }
      if(dlNRBs>nRBsPerSCSperBW(SCSIndx)(bwIndx) || ulNRBs>nRBsPerSCSperBW(SCSIndx)(bwIndx))
      {
          cout<<"dlRBs/ulRBs must not exceed "<<nRBsPerSCSperBW(SCSIndx)(bwIndx)<<" for SCS "<<SCSinKHz<<" kHz and bandwidth "<<BWinMHz<<" MHz"<<endl;abort();
      }
      if(samplingFrequencyInHz<bandwidthInHz)
      {
          cout<<"Sampling Frequency cannot be less than bandwidth..."<<endl;abort();
      }
  }
  void init(int nuid,NUConfig_S &nuConfig, int rbSubcarrierCount, double carrierFrequencyinMHz, double BWInHz, double samplingFreqInHz,bool toCheck)
  {
    nuID=nuid; // this nuid is different from nuConfig.nuID
    subcarrierSpacingInHz=nuConfig.subcarrierSpacingInHz;
    ttiDurationInSec=nuConfig.ttiDurationInSec;
    carrierFrequencyInMHz=carrierFrequencyinMHz;
    samplingFrequencyInHz=samplingFreqInHz;
    bandwidthInHz=BWInHz;
    slotsPerTTI=nuConfig.slotsPerTTI;
    subCarrierCountPerRB = rbSubcarrierCount;
    
    fftSize=(int)(samplingFrequencyInHz/subcarrierSpacingInHz);
    dlNRBs=((int)(nuConfig.dlUsableBW*bandwidthInHz/subcarrierSpacingInHz))/rbSubcarrierCount;
    ulNRBs=((int)(nuConfig.ulUsableBW*bandwidthInHz/subcarrierSpacingInHz))/rbSubcarrierCount;
//     vec temp=nuConfig.CPDurationInSec*samplingFrequencyInHz;
//     CPLengths=round_i(temp);
//    if(toCheck) check();
  }
  
};

inline int getNUID(int nuCaID){ return nuCaID%100; }
inline int getCarrierFreqInMHz(int nuCaID){ return nuCaID/100; }
inline ivec getNUCaIDs(ivec nuIDs, ivec carriersInMHz)
{
  ivec nuCaID(carriersInMHz.length()*nuIDs.length());
  for(int carrier_cnt=0,tot_cnt=0;carrier_cnt<carriersInMHz.length();carrier_cnt++)
    for(int nu_cnt=0;nu_cnt<nuIDs.length();nu_cnt++,tot_cnt++)
      nuCaID(tot_cnt)=carriersInMHz(carrier_cnt)*100+nuIDs(nu_cnt);
    
    return nuCaID;
}

inline int getNUCaID(int nuID,int carrierFreqinMHz)
{
    return carrierFreqinMHz*100+nuID;
}

inline dComplex expj(double theta,bool isInRadian=true)
{ 
    if(!isInRadian)
        theta *= pi/180.0;
     return dComplex(cos(theta),sin(theta)); 
}

inline cvec expj(vec thetas,bool isInRadian=true)
{
    cvec temp(thetas.length());
    for(int i=0;i<thetas.length();i++)
        temp(i) = expj(thetas(i),isInRadian);
    return temp;
}

inline cmat expj(mat thetas,bool isInRadian=true)
{
    cmat temp(thetas.rows(),thetas.cols());
    for(int i=0;i<temp.rows();i++)
        for(int j=0;j<temp.cols();j++)
            temp(i,j) = expj(thetas(i,j),isInRadian);
        return temp;
}

struct sBWPartInfo{
  
  ivec bwPartPerRB;
  ivec subbandIndxPerRB;
  ivec subbandIndxPerRB_higherLayerConfigured; // Added for Aperiodic modes 3_0 and 3_1
  
  sBWPartInfo()
  {
    bwPartPerRB.set_length(0);
    subbandIndxPerRB.set_length(0);
    subbandIndxPerRB_higherLayerConfigured.set_length(0);
  }
  
};

struct BWPartInfoPerCarrier{
  
  Array<sBWPartInfo> bwPartInfo;
  vec carrierFrequenciesinMHz; 
  
  sBWPartInfo get(double carrierInMHz)
  {
    int indx=find(carrierFrequenciesinMHz,carrierInMHz);
    if(indx==-1)
    {
      cout<<"Unsupported carrier in BWPartInfoPerCarrier.get().."<<endl;
      abort();
    }
    return(bwPartInfo(indx));
  }
};



struct Orientation_S 
{
    double alpha;
    double beta;
    double gamma;
    Orientation_S(){
        
        alpha=0.0;
        beta=0.0;
        gamma=0.0;
        
    }
    //Orientation from config file is parsed as ivec. Hence the constructor - Dhiv
    Orientation_S(vec abg){
        
        alpha=(double)abg(0);
        beta=(double)abg(1);
        gamma=(double)abg(2);
        
    }
    Orientation_S(double a, double b, double g){
        
        alpha=a;
        beta=b;
        gamma=g;
        
    }
    friend ostream& operator<<(ostream& os , const Orientation_S A)
    {
        os<<"["<<A.alpha<<" "<<A.beta<<" "<<A.gamma<<"]";
        return os;
    }
    Orientation_S operator+(const Orientation_S &A)
    {
        Orientation_S B;
        B.alpha = alpha + A.alpha;
        B.beta = beta + A.beta;
        B.gamma = gamma + A.gamma;
        return B;
    }
    vec getAsVector()
    {
        return getVectorWithNumbers(3,alpha,beta,gamma);
    }
};

struct OrientationInfo_S{
    
    Array<Orientation_S> orientationPerPanel;
    Array<Orientation_S> uniqueOrientations;
    Array<ivec> panelsPerUniqueOrientations;
};


struct InfoElementFileNames{
    string RadioResourceConfigDedicatedFile;
    string PDCPConfigFile;
    string RLCConfigFile;
    string MACMainConfigFile;
    string LogicalChannelConfigFile;
    string SRBFile;
    string DRBFile;
    
    InfoElementFileNames()
    {
      
    }
    
    int loadAllInfoElements(bool isForSetup)
    {
      if(isForSetup)
      {  
      RadioResourceConfigDedicatedFile = "../configFiles/infoElem/radioResourceConfig.txt";
      PDCPConfigFile = "../configFiles/infoElem/pdcp_config.txt";
      RLCConfigFile = "../configFiles/infoElem/rlc_config.txt";
      MACMainConfigFile = "../configFiles/infoElem/macMain_config.txt";
      LogicalChannelConfigFile = "../configFiles/infoElem/logicalChannel_config.txt";
      DRBFile = "../configFiles/infoElem/drb.txt";
      SRBFile = "../configFiles/infoElem/srb.txt";
      }
      else
      {
      RadioResourceConfigDedicatedFile = "../configFiles/infoElem/rrcReconfig.txt";
      PDCPConfigFile = "../configFiles/infoElem/rrcReconfig.txt";
      RLCConfigFile = "../configFiles/infoElem/rrcReconfig.txt";
      MACMainConfigFile = "../configFiles/infoElem/rrcReconfig.txt";
      LogicalChannelConfigFile = "../configFiles/infoElem/rrcReconfig.txt";
      DRBFile = "../configFiles/infoElem/rrcReconfig.txt";
      SRBFile = "../configFiles/infoElem/rrcReconfig.txt";
      }  
  
      return 0;   
    }
};


struct TransmissionStatus_S{
    
    int serviceNodeID;
    int harqProcessID;
    Array<bvec> txInfoBitsPerCodeWord;      //! Contains the infoBits transmitted.
    Array<bvec> rxInfoBitsPerCodeWord;      //! Contains the infoBits received.
    ivec infoBitsBlockSizePerCodeword;		//! Total number of received bits per CodeWord (ie purely infoBits without CRC)
    bvec isSuccess;                               //! Success status per codeWord.
    Array<bvec> isSuccessPerCB;                   //!Success status per codeBlock in codeWord
    ivec nBitsInError;	                        //! Actual number of bit in error per CodeWord
    int usedResourceElements;
    ivec codeWordIndicesToConsider;
    vec abstractionBLER;
    bvec isNewTransmission;
    vec mcsRatePerCW;
    
    TransmissionStatus_S(){
        
        serviceNodeID=-1;
        harqProcessID=-1;
        txInfoBitsPerCodeWord.set_length(0);
        rxInfoBitsPerCodeWord.set_length(0);
        infoBitsBlockSizePerCodeword.set_length(0);
        isSuccess.set_length(0);
        isSuccessPerCB.set_length(0);
        nBitsInError.set_length(0);
        usedResourceElements=0;
        abstractionBLER.set_length(0);
        isNewTransmission.set_length(0);
        codeWordIndicesToConsider.set_length(0);
    }
};


struct NUConfigFiles_S{
  
  string ulTxRxConfigFileName;
  string dlTxRxConfigFileName;
  string rbPatternPathForDownlink;
  string rbPatternPathForUplink;
  string cchPatternPath;
  string turboInterleaverTableFileName;
  string transportBlockMCSMappingFileName;
  string layerMappingTBSTranslationFileName;
  string cqiToMaxMCSRateFileName;
  string L2SupportFileName;
  string TCPSupportFilesName;
  string NRrbPatternPath;
//   string NRrbPatternPathUL;
};


struct ResultFileNames_S{

  string directory;
  string nodeLocationsFileName;
  string associationInfoFileName;
  string beamAssociationInfoFileName;
  string beamManagementInfoFileName;
  string linkGainFileName;
  string beamRSRPfileName;
  string associationLogsFileName;
  string antennaInfoFileName;
  string antennaGainsFileName;
  string linkTableFileName;
  Array<string> dlWindowBLERstatsFileName;
  Array<string> ulWindowBLERstatsFileName;
  Array<string> dlLinkPerformanceInfoFileName;
  Array<string> dlUEPerformanceInfoFileName;
  Array<string> dlSystemPerformanceInfoFileName;
  Array<string> ulLinkPerformanceInfoFileName;
  Array<string> ulUEPerformanceInfoFileName;
  Array<string> ulSystemPerformanceInfoFileName;
  Array<string> slUEPerformanceInfoFileName;
  Array<string> slSystemPerformanceInfoFileName;
  Array<string> WideBandCQIlogsFileName;
  Array<string> SubbandCQIlogsFileName;
  Array<string> AperiodicCQIlogsFileName;
  Array<string> MCSlogsFileName;
  Array<string> RIlogsFileName;
  Array<string> SchInfoFileName;
  Array<string> MUMIMOpairingFileName;
  Array<string> dlCumulativePerformanceFileName;
  Array<string> ulCumulativePerformanceFileName;
  void loadResultFileNames(string dirPrefix, ivec tNUIDs);
  
 void fSetSize(int tsize)
 {
   dlWindowBLERstatsFileName.set_length(tsize);
   ulWindowBLERstatsFileName.set_length(tsize);
   dlLinkPerformanceInfoFileName.set_length(tsize);
   dlUEPerformanceInfoFileName.set_length(tsize);
   dlSystemPerformanceInfoFileName.set_length(tsize);
   ulLinkPerformanceInfoFileName.set_length(tsize);
   ulUEPerformanceInfoFileName.set_length(tsize);
   ulSystemPerformanceInfoFileName.set_length(tsize);
   slUEPerformanceInfoFileName.set_length(tsize);
   slSystemPerformanceInfoFileName.set_length(tsize);
   WideBandCQIlogsFileName.set_length(tsize);
   SubbandCQIlogsFileName.set_length(tsize);
   AperiodicCQIlogsFileName.set_length(tsize);
   MCSlogsFileName.set_length(tsize);
   RIlogsFileName.set_length(tsize);
   SchInfoFileName.set_length(tsize);
   MUMIMOpairingFileName.set_length(tsize);
   dlCumulativePerformanceFileName.set_length(tsize);
   ulCumulativePerformanceFileName.set_length(tsize);
}

};


struct ConfigFileNames_S{

  string directory;  
  string sysConfigFileName;
  string AASConfigFileName;
  string advancedSysConfigFileName;
  string simulationConfigFileName;
 // string linkTableFileName;
  string nuConfigFileName;
  string mobilitySupportFileName;
  string buildingInfoFileName;
//   string carrierAggregationFileName;
  string admissionControlFileName;
  string portToPhysicalAntennaMapperFileName;
  string tracesFileName;
  string ssbConfigFileName;
  string ssbPatternPath;
  ivec nuIDs;
  Array<NUConfigFiles_S> nuConfigFiles;
  void loadConfigFileNames(string dirPrefix);
  
};


struct PerformanceInfo_S
{
    vec avgThroughputPerServerNodeType;
    vec avgSEPerServerNodeType;
    vec avgThroughputPerServiceNodeUnderServerNodeType;
    vec avgSEPerServiceNodeUnderServerNodeType;
    ivec totalScheduledRBsPerServerNodeType;
    ivec totalScheduledRBsForSystemPerServerNodeType;
    ivec numberofServerNodes;
    PerformanceInfo_S(int length)
    {
        avgThroughputPerServerNodeType = zeros(length);
        avgSEPerServerNodeType = zeros(length);
        avgThroughputPerServiceNodeUnderServerNodeType = zeros(length);
        avgSEPerServiceNodeUnderServerNodeType = zeros(length);
        totalScheduledRBsPerServerNodeType = zeros_i(length);
        totalScheduledRBsForSystemPerServerNodeType = zeros_i(length);
        numberofServerNodes = zeros_i(length);
    }
};


#endif

