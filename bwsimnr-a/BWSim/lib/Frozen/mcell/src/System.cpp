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


#include "../include/System.h"

McellSystem::McellSystem(){     
  
  coverageArea.xLen=-1;  
  coverageArea.yLen=-1;  
  cellLayout=_CELLLAYOUT_UNINIT_; 
  cellISD=-1;             
  hexDropCommonVerticalTilt=-1;    
//   carrierFreqPerServerNodeType.set_length(0);
  //carrierFreq=-1;
  minDist=-1;
  shadowType=_SHADOWTYPE_UNINIT_;  
  aNodeTypes.set_size(0);
  nNodes.set_size(0);
  nodeDropMethods.set_size(0);
  addPathSpecificSCMAntennaGain=true;
  isCrossLinksEnabled=false;
  antennaFeederLossIndB=zeros(1);
  cellWiseDropping=false;
  frequencyReUseFactor = 1;
}

/*! \fn loadSystenDefault(...)
* \brief creating a default mcell system which will take the default values,unless and untill  there is specification of system then taking the load default values of 21 BS,210 UES and thier types,otherwise specify thier specifications
*/


void McellSystem::loadSystemDefault(){  
  if(sum(nNodes)==0) 
  {
    if(aNodeTypes.length()==0)  
    {
      cout<<"[detl:]Loading default nodes as 21 BaseStations and 210 UEs with node Type Names as 'BS' and 'UE' respectively."<<endl;
      nNodes="21 210";
      aNodeTypes="{BS UE";
      cout<<"[detl:]Loading Drop methods as 'Hexagonal' and 'Random' for BS and UE respectively."<<endl;
      nodeDropMethods.set_size(2);
      nodeDropMethods(0)=_DROPMETHOD_HEXAGONAL_;
      nodeDropMethods(1)=_DROPMETHOD_RANDOM_;
    }
    else
    {
      cout<<"[both:]Please specify node counts for the Node Types specified. Default Values not possible."<<endl;abort();
    }
  }

  if(sum(nNodes)!=0 && (find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_)==-1 && find(nodeDropMethods,_DROPMETHOD_RANDOM_)==-1))//if there is a node and note type is not specified//
  {
    cout<<"[detl:]Loading Random drop for all the node types as default.";
    for(int type_cnt=0;type_cnt<nNodes.length();type_cnt++)
      if(nNodes(type_cnt)!=0)
	nodeDropMethods(type_cnt)=0;

  }
  
  if(cellLayout==_CELLLAYOUT_UNINIT_)  
  {
	if(find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_)!=-1) 
	{
#ifdef USING_MPI
if(currentTaskid==0) {
#endif
	  cout<<"[detl:]Loading hexagonal Layout as per the settings."<<endl;
#ifdef USING_MPI
}
#endif
	  cellLayout=_CELLLAYOUT_HEXAGONAL_;                
	}
	else if(find(nodeDropMethods,_DROPMETHOD_SATELLITE_)!=-1)
    {
        #ifdef USING_MPI
        if(currentTaskid==0) {
            #endif
            cout<<"[detl:]Loading NTN Layout as per the settings."<<endl;
            #ifdef USING_MPI
        }
        #endif
        cellLayout=_CELLLAYOUT_NTN_;        
    }
	else                                                
	{
#ifdef USING_MPI
if(currentTaskid==0) {
#endif         
	  cout<<"[detl:]Loading Random Layout as per the settings."<<endl;
#ifdef USING_MPI
}                                                           
#endif
	  cellLayout=_CELLLAYOUT_RANDOM_;                     
	}
  }
 
// Loading Configured Terrain
// Note : function loadOtherTerrain(..) is containing rest of ther other terrain model that are supported not mentione in check
// Added to simplify the changes for any new terrain if added in future


  if(sysTerrain.getTerrain()=="_TERRAIN_UNINIT_")            
  {
    #ifdef USING_MPI
    if(currentTaskid==0)
    {
    #endif
	    cout<<"[detl:]Loading Urban Micro from Winner Model as default Terrain."<<endl;
    #ifdef USING_MPI
    }
    #endif
    sysTerrain.setTerrain("_TERRAIN_WINNER_URBAN_MICRO_");
    loadWinnerTerrains(sysTerrain);
  }
  else                                                     
  {
    string terrain=sysTerrain.getTerrain();                 

    if(terrain=="_TERRAIN_3GPP_CASE1_" || terrain=="_TERRAIN_3GPP_CASE3_")
    {
        load3gppTerrain(sysTerrain);
    }
    else if(terrain=="_TERRAIN_NOLOSS_")
    {
        loadNoLossTerrain(sysTerrain);
    }
    else if(terrain=="_TERRAIN_IMT_URBAN_MICRO_" || terrain=="_TERRAIN_IMT_URBAN_MACRO_" || terrain=="_TERRAIN_IMT_SUBURBAN_MACRO_" || terrain=="_TERRAIN_IMT_RURAL_MACRO_")
    {
        loadIMTTerrains(sysTerrain);
    }
    else if(terrain=="_TERRAIN_WINNER_URBAN_MICRO_" || terrain=="_TERRAIN_WINNER_URBAN_MACRO_")
    {
        loadWinnerTerrains(sysTerrain);
    }
    else if ("_TERRAIN_WINNER_PLUS_SUBURBAN_MACRO_"==terrain ||"_TERRAIN_WINNER_PLUS_URBAN_MACRO_"==terrain ||"_TERRAIN_WINNER_PLUS_URBAN_MICRO_"==terrain )
    {
        loadWinnerPlusTerrains(sysTerrain);
    }
    else if(terrain=="_TERRAIN_SCM3D_URBAN_MICRO_" || terrain=="_TERRAIN_SCM3D_URBAN_MACRO_")
    {
        load3DChannelTerrains(sysTerrain);
    }
    else if(terrain=="_TERRAIN_COMP_3GPP_CASE1_")
    {
        loadCompTerrains(sysTerrain);
    }
    else if(terrain=="_TERRAIN_SMALL_CELLS_")
    {
        loadSmallCellTerrains(sysTerrain);
    }
    else // 5GChan
    {
        bool flag_terrSupported;
        flag_terrSupported = load5GTerrains(sysTerrain);
        if(!flag_terrSupported )
        {
            cout<<"[detl:] Default loading not possible."<<endl;
        }
    }
    // end
  }
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_ && cellISD==-1)
  {
	if(sysTerrain.getTerrain()=="_TERRAIN_WINNER_URBAN_MICRO_")
	  {cout<<"[detl:]Loading 200m as default Inter Site Distance."<<endl; cellISD=200;}
	else if(sysTerrain.getTerrain()=="_TERRAIN_WINNER_URBAN_MACRO_")
	  {cout<<"[detl:]Loading 500m as default Inter Site Distance."<<endl; cellISD=500;}
	else
	  {cout<<"[both:]Error. Unable to load default for Inter Site Distance for the given terrain."<<endl; exit(1);}
  }

  if(cellLayout==_CELLLAYOUT_HEXAGONAL_ && minDist==-1)
  {
	if(sysTerrain.getTerrain()=="_TERRAIN_WINNER_URBAN_MICRO_")
	{cout<<"[detl:]Loading 10m as Minimum Distance from cell center in Hexagonal Drop."<<endl; minDist=10;}
	else if(sysTerrain.getTerrain()=="_TERRAIN_WINNER_URBAN_MACRO_")
	{cout<<"[detl:]Loading 10m as Minimum Distance from cell center in Hexagonal Drop."<<endl; minDist=25;}
	else
	{cout<<"[both:]Error. Unable to load default for Minimum Distance from cell center in Hexagonal Drop for the given terrain."<<endl; exit(1);}
  }  
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_ && hexDropCommonVerticalTilt==-1)
  {
	if(sysTerrain.getTerrain()=="_TERRAIN_WINNER_URBAN_MICRO_" || sysTerrain.getTerrain()=="_TERRAIN_WINNER_URBAN_MACRO_")
	{cout<<"[detl:]Loading 12 degree as vertical tilt for sector antennas in Hexagonal Drop."<<endl; hexDropCommonVerticalTilt=12;}
	else
	{cout<<"[both:]Error. Unable to load default for vertical tilt for sector antennas in Hexagonal Drop for the given terrain."<<endl; exit(1);}
  }
  
  //if(carrierFreq==-1) {cout<<"Loading 2.5GHz as default System Carrier Frequency ..."<<endl; carrierFreq=2.5e9;}
//   if(carrierFreqPerServerNodeType.length()==0) {cout<<"Loading 2.5GHz as default System Carrier Frequency ..."<<endl; carrierFreqPerServerNodeType=ones(getServerNodeTypes().length())*2.5e9;}
  if(sum(nNodes)!=0 && aNodeTypes.length()==0)
  {
    	cout<<"[detl:]Loading defaults Type0 , Type1, Type2 as default names for different Node Types respectively."<<endl;
	Array<string> typeNames;
	typeNames="{Type1 Type2 Type3 Type4 Type5 Type6 Type7 Type8 Type9 Type10}";
	for(int type_cnt=0;type_cnt<length(nNodes);type_cnt++)
	  if(nNodes(type_cnt)!=0)
		aNodeTypes(type_cnt)=typeNames(type_cnt);
  }

  if(sum(nNodes)!=0 && aNodeTypes.length()!=0 && cellLayout==_CELLLAYOUT_HEXAGONAL_ && find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_)==-1)
	{cout<<"[both:]Please specify proper Drop methods for the Hexagonal Layout specified. Default values not possible."<<endl;abort();}

  if(sum(nNodes)!=0 && aNodeTypes.length()!=0 && cellLayout==_CELLLAYOUT_RANDOM_ && (coverageArea.xLen==-1 || coverageArea.yLen==-1))
  {
    cout<<"[detl:]Loading default coverage area as 1000 x 1000 for Random Layout."<<endl;
    coverageArea.xLen=450000;
    coverageArea.yLen=450000;
  }

  if(cellLayout==_CELLLAYOUT_HEXAGONAL_ || cellLayout==_CELLLAYOUT_NTN_ )
    updateHexagonalCoverageArea();

  sysTerrain.setCellISD(cellISD);
}

/*! \fn updateHexagonalCoverageArea()
 * \brief--> this function will update hexagonal coverage area depends upon number of cell present
 */
void McellSystem::updateHexagonalCoverageArea(){                       

#ifdef USING_MPI
if(currentTaskid==0) {
#endif
  cout<<"[detl:]Computing Coverage Area for Hexagonal Drop."<<endl;    
#ifdef USING_MPI
}
#endif
  int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
  int satDropNodeType=find(nodeDropMethods,_DROPMETHOD_SATELLITE_);
  
  if(hexDropNodeType!=-1)                                               
  {
    switch(nNodes(hexDropNodeType))                                    
    {
      case 1:                                                          
      case 3:
      case 6:
            coverageArea.xLen = sqrt(3);
            coverageArea.yLen = 2;
            break;

      case 7:                                                           
      case 21:
      case 42:
            coverageArea.xLen = 3*sqrt(3);                                        
            coverageArea.yLen = 5;
            break;

      case 19:                                                          
      case 57:
      case 114:
            coverageArea.xLen = 5*sqrt(3);
            coverageArea.yLen = 8;
            break;
      default:                                                         
      {
	cout<<"[both:]Unknown hexagonal node count for updateHexagonalCoverageArea()."<<endl;
	abort();
      }
    }
    coverageArea.xLen*=cellISD/sqrt(3);                                
    coverageArea.yLen*=cellISD/sqrt(3);
  }
  else if(satDropNodeType!=-1)
  {
      switch(nNodes(satDropNodeType))                                    
      {
          case 1:                                                          
          case 3:
          case 6:
              coverageArea.xLen = sqrt(3);
              coverageArea.yLen = 2;
              break;
              
          case 7:                                                           
          case 21:
          case 42:
              coverageArea.xLen = 3*sqrt(3);                                        
              coverageArea.yLen = 5;
              break;
              
          case 19:                                                          
          case 57:
          case 114:
              coverageArea.xLen = 5*sqrt(3);
              coverageArea.yLen = 8;
              break;
          case 61:                                                          
              coverageArea.xLen = 7*sqrt(3);
              coverageArea.yLen = 11;
              break;
          default:                                                         
          {
              cout<<"[both:]Unknown hexagonal node count for updateHexagonalCoverageArea()."<<endl;
              abort();
          }
      }
      coverageArea.xLen*=cellISD/sqrt(3);                                
      coverageArea.yLen*=cellISD/sqrt(3);  
  }
  else
  {
    int singleCellNodeType=find(nodeDropMethods,_DROPMETHOD_SINGLE_CELL_);
    if(singleCellNodeType!=-1)                                             
    {
      coverageArea.xLen = sqrt(3);
      coverageArea.yLen = 2;
      coverageArea.xLen*=cellISD/sqrt(3);                                
      coverageArea.yLen*=cellISD/sqrt(3);
    }
  }
  cout<<"coverage x axis"<<coverageArea.xLen<<endl;
  cout<<"coverage y axis"<<coverageArea.yLen<<endl;
}


void McellSystem::loadNodeDefault(){                                

#ifdef USING_MPI
if(currentTaskid==0) {                    
#endif
  cout<<"[detl:]Loading default Tx power as 0dBm for all nodes if not already assigned. Will be overwritten on any further changes."<<endl;
#ifdef USING_MPI
}
#endif
  for(int node_cnt=0;node_cnt<sum(nNodes);node_cnt++)             
  {
	if(aNodes(node_cnt).getTxPowerInDBm()==-1) aNodes(node_cnt).setTxPowerInDBm(0);
	
	aNodes(node_cnt).txAntenna.setAntennaFeederLoss(antennaFeederLossIndB(0));
  } 
  
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_)                           
  {
   	string terrainType=getTerrain();               
	int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_); 
	
	if(terrainType=="_TERRAIN_WINNER_URBAN_MACRO_")     
	{
	  setNodeHeight(aNodeTypes(hexDropNodeType),25);
	}
	else if(terrainType=="_TERRAIN_WINNER_URBAN_MICRO_")       
	{
	  setNodeHeight(aNodeTypes(hexDropNodeType),10);
	}

      
	//! If the node drop is not hexagonal which means random another default node height specified //
	for(int type_cnt=0;type_cnt<aNodeTypes.length();type_cnt++)  
	if(type_cnt!=hexDropNodeType)                               
	{
	  setNodeHeight(aNodeTypes(type_cnt),1.5);                   
	}

	ivec hexNodes=getNodes(aNodeTypes(hexDropNodeType));     

	if(hexDropCommonVerticalTilt==-1)                       
	{
	  cout<<"[both:]Error. hexDropCommonVerticalTilt not set in the system. Default value not possible."<<endl;
	  abort();
	}

	if(length(hexNodes)==3 || length(hexNodes)==21 || length(hexNodes)==57 )  //!if there are 3 sectors per cell //
	{
	  ivec hTilts="60 180 300"; // This is equivalent to 30 150 and 270 for the Hex locations considered in the simulator - Dhiv
// 	  ivec hTilts="0 0 0";
// 	  ivec hTilts="30 150 270";
	  
#ifdef USING_MPI
if(currentTaskid==0) {
#endif
	  cout<<"[detl:]Loading default Antenna for "<<aNodeTypes(hexDropNodeType)<<" as "<<" a parabolic sectorized antenna with \n HorBW = 70deg, VerBW = 15deg, VerTilt="<<hexDropCommonVerticalTilt<<" AntGain = 8dBi"<<endl;
	  cout<<"[detl:]With Sector antennas facing "<<hTilts<<endl;
#ifdef USING_MPI
}
#endif
	  for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
          aNodes(hexNodes(hexNode_cnt)).setAntenna(_ANTENNA_TYPE_PARABOLIC_,8,antennaFeederLossIndB(hexDropNodeType),70,hTilts(hexNode_cnt%3),20,15,hexDropCommonVerticalTilt,20);
	  
	}
	else if(length(hexNodes)==42 || length(hexNodes)==114 )     //!if there are 6 sectors per cell
	{
        ivec hTilts="60 120 180 240 300 360"; // This is equivalent to 30 150 and 270 for the Hex locations considered in the simulator - Dhiv                                   
// 	  ivec hTilts="30 90 150 210 270 330";          
#ifdef USING_MPI
if(currentTaskid==0) {
#endif
	  cout<<"[detl:]Loading default Antenna for "<<aNodeTypes(hexDropNodeType)<<" as "<<" a parabolic sectorized antenna with \n HorBW = 35deg, VerBW = 15deg, VerTilt="<<hexDropCommonVerticalTilt<<" AntGain = 17dBi"<<endl;
	  cout<<"[detl:]With Sector antennas facing "<<hTilts<<endl;
#ifdef USING_MPI
}
#endif
	  for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
          aNodes(hexNodes(hexNode_cnt)).setAntenna(_ANTENNA_TYPE_PARABOLIC_,8,antennaFeederLossIndB(hexDropNodeType),35,hTilts(hexNode_cnt%6),20,15,hexDropCommonVerticalTilt,20);
	  
	}
  }
  else
  {
    int singleCellNodeType=find(nodeDropMethods,_DROPMETHOD_SINGLE_CELL_); 
    if(singleCellNodeType!=-1)
    {
      #ifdef USING_MPI
      if(currentTaskid==0) {
	#endif
	cout<<"[detl:]Loading default Antenna for "<<aNodeTypes(singleCellNodeType)<<" as "<<" a parabolic sectorized antenna with \n HorBW = 35deg, VerBW = 15deg, VerTilt="<<hexDropCommonVerticalTilt<<" AntGain = 17dBi"<<endl;
	cout<<"[detl:]With Sector antenna facing 90 degree"<<endl;
	#ifdef USING_MPI
      }
      #endif
      ivec singleCellNode=getNodes(aNodeTypes(singleCellNodeType));
      aNodes(singleCellNode(0)).setAntenna(_ANTENNA_TYPE_PARABOLIC_,8,antennaFeederLossIndB(singleCellNodeType),70,90,20,15,hexDropCommonVerticalTilt,20);
    }
    
  }

}
/*!\fn initSystem()
 * \brief --> initializing the system like type,ID,category,carrier frequencies of the nodes
 */
 
void McellSystem::initSystem(){     

  loadSystemDefault();

  aNodes.set_size(sum(nNodes));
//   cout << "mcell ddb: " << nNodes << " " << sum(nNodes) << " " << aNodes.size() << " " << aNodeTypes.size() << endl;

  nodeCategoryPerNodeType.set_length(nNodes.length());   
  int totalNode_cnt=0; 
  networkIDPerNodeType=ones_i(aNodeTypes.length());//! Creating first network-ID, i.e.,1, if Second-Network is created,network-ID, i.e.,2 will be added   
  for(int type_cnt=0;type_cnt<aNodeTypes.length();type_cnt++) 
  {
    for(int node_cnt=0;node_cnt<nNodes(type_cnt);node_cnt++,totalNode_cnt++) 
    {
      aNodes(totalNode_cnt).setType(type_cnt);                        
      aNodes(totalNode_cnt).setID(totalNode_cnt);         
      //aNodes(totalNode_cnt).setCarrierFreq(carrierFreq);  
    }
    nodeCategoryPerNodeType(type_cnt)=_NODECATEGORY_UNINIT_; 
  }
  nodeIDs=getIntegers(0,totalNode_cnt-1);                 
  loadNodeDefault();    
}

void McellSystem::loadAASConfig(string AASConfigFile,SCMPropagationScenario scmScenario,bool isToLoadAAS)
{
    bool isCrossPolarized=false;
    ivec nodeAntennaElementCount;
    ivec numOfTxRUs;
    Array<ivec > TXRUMap, beamMap;
    Array<vec> crossPolarizationAngles; // NOTE : crossPolarizationAngles is parsed as ivec for simplicity
    Array< Array<vec> > orientationPerPanel; // NOTE : orientationPerPanel is parsed as ivec for simplicity
    Array <vec > AntSpacing;
    Array < Array <vec> > zenithBeamAngles;
    Array < Array <vec> > azimuthBeamAngles;
    Array <ivec > AntStructure;
    vec coPolarizationSlantAngle;
    
    //parse(AASConfigFile,"nodeAntennaElementCount",nodeAntennaElementCount);
//     parse(AASConfigFile,"numTxRUs",numOfTxRUs);
    parse(AASConfigFile,"TXRUMap",TXRUMap);
    parse(AASConfigFile,"beamMap",beamMap);
    if(!parse(AASConfigFile,"orientationPerPanel",orientationPerPanel))
    {
        cout<<"Unable to parse orientationPerPanel.. Assigning [0 0 0] for all nodes.."<<endl;
        orientationPerPanel.set_length(aNodeTypes.length());
        for(int type_cnt=0;type_cnt<aNodeTypes.length();type_cnt++)  
            orientationPerPanel(type_cnt).set_length(0); // Default orientations loaded separately.
    }
    if(!parse(AASConfigFile,"crossPolarizationAngles",crossPolarizationAngles))
    {
        cout<<"Unable to parse crossPolarizationAngles.. Assigning [45 -45] for all nodes.."<<endl;
        crossPolarizationAngles.set_length(aNodeTypes.length());
        for(int type_cnt=0;type_cnt<aNodeTypes.length();type_cnt++)  
            crossPolarizationAngles(type_cnt).set_length(0); // Default crossPolarizationAngles loaded separately.
    }
    
    parse(AASConfigFile,"AntStructure",AntStructure);
    parse(AASConfigFile,"AntSpacing",AntSpacing);
    parse(AASConfigFile,"zenithBeamAngles",zenithBeamAngles);
    parse(AASConfigFile,"azimuthBeamAngles",azimuthBeamAngles);
    
    numOfTxRUs.set_length(TXRUMap.length());
    for(int nodeTypeCnt = 0;nodeTypeCnt < TXRUMap.length();nodeTypeCnt++){
        numOfTxRUs(nodeTypeCnt) = prod(TXRUMap(nodeTypeCnt))*AntStructure(nodeTypeCnt)(4);
    }

    cout<<"zenithBeamAngles : "<<zenithBeamAngles<<endl;
    
    cout<<"azimuthBeamAngles : "<<azimuthBeamAngles<<endl;
    
    parse(AASConfigFile,"coPolarizationSlantAngle",coPolarizationSlantAngle);
    nodeAntennaElementCount.set_length(AntStructure.length());
    
    for(int i=0;i<nNodes.length();i++)
    {
        if(nNodes(i)!=0)
        {   
            if(prod(TXRUMap(i))*AntStructure(i)(0)*AntStructure(i)(1)*AntStructure(i)(4)!=numOfTxRUs(i))
            {
                cout<<"[both:] Incorrect numTxRUs for nodeType "<<i<<".Aborting in initSystem()..."<<endl;abort();
            }
            nodeAntennaElementCount(i)  = prod(AntStructure(i));
            if(nodeAntennaElementCount(i)%(numOfTxRUs(i))!=0)
            {
                cout<<"[both:] Incorrect parameters 'nodeAntennaElementCount' and 'numTxRUs'. NodeAntennaElementCount should be integral multiple of numTxRUs. Check 'AASConfig'. Aborting in initSystem()..."<<endl;
                abort();
            }
        }
    }   
    
    int totalNode_cnt=0;
    
    for(int type_cnt=0;type_cnt<aNodeTypes.length();type_cnt++)  
    {
        int nPanels=AntStructure(type_cnt)(0)*AntStructure(type_cnt)(1);
        
        vec zeroOrient="0 0 0";
        int defaultOrients=nPanels-orientationPerPanel(type_cnt).length();
        for(int i=0;i<defaultOrients;i++)    
            append(orientationPerPanel(type_cnt),zeroOrient);
        
        if(crossPolarizationAngles(type_cnt).length()==0)
            crossPolarizationAngles(type_cnt)="45 -45";
        
        Array<vec> uniqueOrientations=findUniqueElements(orientationPerPanel(type_cnt));
        Array<ivec> panelsPerUniqueOrientations(uniqueOrientations.length());
        for(int or_cnt=0;or_cnt<uniqueOrientations.length();or_cnt++)
            panelsPerUniqueOrientations(or_cnt)=find(orientationPerPanel(type_cnt),uniqueOrientations(or_cnt),true);
        
        if(AntStructure(type_cnt)(4) == 2) 
            isCrossPolarized = true;
        else
            isCrossPolarized = false;
        
        SCMType scmType = getSCMType(scmScenario);
        
        for(int node_cnt=0;node_cnt<nNodes(type_cnt);node_cnt++,totalNode_cnt++)
        {
            aNodes(totalNode_cnt).txAntenna.setAntennaWavelengthSpacing(AntStructure(type_cnt)(3));
            aNodes(totalNode_cnt).rxAntenna.setAntennaWavelengthSpacing(AntStructure(type_cnt)(3));
            
            if(scmType != _SCMTYPE_2D_) // 3D Channel parameters are parsed here
            {
                Orientation_S nodeOrient = aNodes(totalNode_cnt).nodeOrient;
                // AAS - Modified to all nodes
                aNodes(totalNode_cnt).txAntenna.loadAASInfo(AntStructure(type_cnt),TXRUMap(type_cnt),AntSpacing(type_cnt),zenithBeamAngles(type_cnt),azimuthBeamAngles(type_cnt));
                aNodes(totalNode_cnt).txAntenna.setPolarisationAngles(crossPolarizationAngles(type_cnt),coPolarizationSlantAngle(type_cnt));
                
                aNodes(totalNode_cnt).txAntenna.orientationInfo.panelsPerUniqueOrientations=panelsPerUniqueOrientations;
                aNodes(totalNode_cnt).txAntenna.orientationInfo.orientationPerPanel.set_length(nPanels);
                for(int panel_cnt=0;panel_cnt<nPanels;panel_cnt++)
                    aNodes(totalNode_cnt).txAntenna.orientationInfo.orientationPerPanel(panel_cnt) = Orientation_S(orientationPerPanel(type_cnt)(panel_cnt)) + nodeOrient;
                aNodes(totalNode_cnt).txAntenna.orientationInfo.uniqueOrientations.set_length(uniqueOrientations.length());
                for(int or_cnt=0; or_cnt<uniqueOrientations.length(); or_cnt++)
                    aNodes(totalNode_cnt).txAntenna.orientationInfo.uniqueOrientations(or_cnt) = Orientation_S(uniqueOrientations(or_cnt)) + nodeOrient;
                
                aNodes(totalNode_cnt).rxAntenna = aNodes(totalNode_cnt).txAntenna;
            }
        }
    }
}

void loadCustomAntennaPatterns(string mySysConfigFile,vec& hPattern,vec& vPattern,string nodeType)
{
    string fileName;
    parse(mySysConfigFile,"customAntennaPatternFile",fileName);
    fileName = getDirectory(mySysConfigFile) + fileName;
    if(!parse(fileName,nodeType+"_hPattern",hPattern))
    {
        cout<<"Unable to parse "<<nodeType+"_hPattern"<<" from file "<<fileName<<endl;abort();
    }
    if(!parse(fileName,nodeType+"_vPattern",vPattern))
    {
        cout<<"Unable to parse "<<nodeType+"_vPattern"<<" from file "<<fileName<<endl;abort();
    }
}

// void McellSystem::loadAntennaInfo(string AASConfigFile,string mySysConfigFile,bool isToLoadAAS)
// {
//     string terrain=sysTerrain.getTerrain();
//     cout<<"terrain : "<<terrain<<endl;
//     
//     SCMType scmType = getSCMType(sysTerrain.scmScenario);
//     
//     if(scmType != _SCMTYPE_2D_) // 3D Channel parameters are parsed here
//     {
//         if(hexDropCommonVerticalTilt!=90)
//             cout<<"Warning.. SCM model supports only mechanical vertical tilt as 90 degree w.r.t panel. Hence overwriting commonVerticalTilt internally..."<<endl;
//         
//         hexDropCommonVerticalTilt=90;
//     }
//     int totalNode_cnt=0;
//     for(int type_cnt=0;type_cnt<aNodeTypes.length();type_cnt++)  
//         for(int node_cnt=0;node_cnt<nNodes(type_cnt);node_cnt++,totalNode_cnt++)
//         {
//             aNodes(totalNode_cnt).setType(type_cnt);
//             aNodes(totalNode_cnt).setID(totalNode_cnt);
//             aNodes(totalNode_cnt).setActive();
//         }
//         
//     vec hBW,vBW,hMaxAtt,vMaxAtt,macroAntGainIndB;
//     Array<string> antennaType;
//     parse(mySysConfigFile,"nodeAntennaType",antennaType); 
//     parse(mySysConfigFile,"nodeAntennaHorizontalBeamWidth",hBW);               
//     parse(mySysConfigFile,"nodeAntennaVerticalBeamWidth",vBW);
//     if(!parse(mySysConfigFile,"nodeAntGainIndB",macroAntGainIndB))
//     {
//         cout<<"[detl:]Incorrect macroAntGainIndB value in mySysConfig... Aborting... "<<endl;
//         abort();
//     }
//     parse(mySysConfigFile,"nodeAntennaHorizontalMaxAttenuation",hMaxAtt);
//     parse(mySysConfigFile,"nodeAntennaVerticalMaxAttenuation",vMaxAtt);
//     parse(mySysConfigFile,"antennaFeederLossIndB",antennaFeederLossIndB);
//     
//     vec antennaApertureRadius,GTRatio,antennaTemperature;
//     parse(mySysConfigFile,"antennaApertureRadius",antennaApertureRadius);
//     parse(mySysConfigFile,"GTRatio",GTRatio);
//     if(!parse(mySysConfigFile,"antennaTemperature",antennaTemperature))
//     {
//         cout<<"[both:]Unable to parse antennaTemperature from mySysConfigFile. Loading 298K as default"<<endl;
//         antennaTemperature = 298*ones(aNodeTypes.length());
//     }    
// 
//     int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_); 
//     int inH3SecNodeType = find(nodeDropMethods,_DROPMETHOD_INBUILDING3SEC_); 
//     int inHNodeType = find(nodeDropMethods,_DROPMETHOD_INBUILDING_); 
//     int singleCellNodeType=find(nodeDropMethods,_DROPMETHOD_SINGLE_CELL_);
//     
//     int bsNodeType=-1;
//     
//     if(hexDropNodeType!=-1) bsNodeType = hexDropNodeType;
//     else if(inH3SecNodeType!=-1) bsNodeType = inH3SecNodeType;
//     else if(inHNodeType!=-1) bsNodeType = inHNodeType;
//     else if(singleCellNodeType!=-1) bsNodeType = singleCellNodeType;
//     
//     if(bsNodeType!=-1)
//     {
//         AntennaType_E antType = getAntennaType(antennaType(bsNodeType));
//         vec hPattern,vPattern;
//         if(antType==_ANTENNA_TYPE_CUSTOM_)
//         {
//             loadCustomAntennaPatterns(mySysConfigFile,hPattern,vPattern,aNodeTypes(bsNodeType));
//         }
//         ivec bsNodes = getNodes(aNodeTypes(bsNodeType));
//         double sectorAngle = 360.0/nSectorsPerSite;
//         vec hTilts = ((singleCellNodeType!=-1) ? 90.0 : (inHNodeType!=-1) ? 0.0 : 60.0) + getVector(0,(nSectorsPerSite-1)*sectorAngle,sectorAngle);//(nSectorsPerSite==3) ? "60 180 300" : "60 120 180 240 300 360";
//         #ifdef USING_MPI
//             if(currentTaskid==0) {                                                       
//                 #endif
//                 cout<<"[detl:]Loading Antenna for "<<aNodeTypes(bsNodeType)<<" as "<<" a parabolic sectorized antenna with \n HorBW = "<<hBW(bsNodeType)<<"deg, VerBW = "<<vBW(bsNodeType)<<"deg, VerTilt="<<hexDropCommonVerticalTilt<<" AntGain = "<<macroAntGainIndB(bsNodeType)<<" dBi"<<endl;
//                 cout<<"[detl:]With Sector antennas facing "<<hTilts<<endl;
//                 #ifdef USING_MPI
//             }
//         #endif
//         
//         for(int bsNode_cnt=0;bsNode_cnt<bsNodes.length();bsNode_cnt++)
//         {
//             double sectorTilt = hTilts(bsNode_cnt%nSectorsPerSite);
//             aNodes(bsNodes(bsNode_cnt)).setOrientation(sectorTilt,0,0);
//             switch(antType)
//             {
//                 case _ANTENNA_TYPE_OMNI_:
//                         aNodes(bsNodes(bsNode_cnt)).setAntenna(_ANTENNA_TYPE_OMNI_,0,0,-1,sectorTilt,-1,-1,hexDropCommonVerticalTilt,-1); // Loading default values for Omni Directional Antenna.
//                         break;
//                 case _ANTENNA_TYPE_CUSTOM_:
//                         aNodes(bsNodes(bsNode_cnt)).setAntenna(_ANTENNA_TYPE_CUSTOM_,macroAntGainIndB(bsNodeType),antennaFeederLossIndB,sectorTilt,hexDropCommonVerticalTilt,hPattern,vPattern);
//                         break;
//                 case _ANTENNA_TYPE_PARABOLIC_:
//                         aNodes(bsNodes(bsNode_cnt)).setAntenna(_ANTENNA_TYPE_PARABOLIC_,macroAntGainIndB(bsNodeType),antennaFeederLossIndB,hBW(bsNodeType),sectorTilt,hMaxAtt(bsNodeType),vBW(bsNodeType),hexDropCommonVerticalTilt,vMaxAtt(bsNodeType));
//                         break;
//                 default:
//                         cout<<"Unknown Antenna Type..."<<endl;abort();
//                         break;
//             }
//         }
//     }
//     
//     ivec randomDropType = find(nodeDropMethods,_DROPMETHOD_RANDOM_,true);
//     for(int type_cnt=0;type_cnt<randomDropType.length();type_cnt++)
//     {
//         int dropType = randomDropType(type_cnt);
//         ivec nodes = getNodes(aNodeTypes(dropType));
//         if(nodes.length())
//         {
//             AntennaType_E antType = getAntennaType(antennaType(dropType));
//             vec hPattern,vPattern;
//             if(antType == _ANTENNA_TYPE_CUSTOM_)
//                 loadCustomAntennaPatterns(mySysConfigFile,hPattern,vPattern,aNodeTypes(dropType));
//             for(int node_cnt=0;node_cnt<length(nodes);node_cnt++)
//             {
//                 double hTilt = randu()*360.0,vTilt = 90.0;
//                 aNodes(nodes(node_cnt)).setOrientation(hTilt,0,0);
//                 switch(antType)
//                 {
//                     case _ANTENNA_TYPE_OMNI_:
//                         aNodes(nodes(node_cnt)).setAntenna(antType,0,0,-1,hTilt,-1,-1,vTilt,-1); // Loading default values for Omni Directional Antenna.
//                         break;
//                     case _ANTENNA_TYPE_CUSTOM_:
//                         aNodes(nodes(node_cnt)).setAntenna(antType,macroAntGainIndB(dropType),antennaFeederLossIndB,hTilt,vTilt,hPattern,vPattern); // Loading default values for Omni Directional Antenna.
//                         break;
//                     case _ANTENNA_TYPE_PARABOLIC_:
//                         aNodes(nodes(node_cnt)).setAntenna(antType,macroAntGainIndB(dropType),antennaFeederLossIndB,hBW(dropType),hTilt,hMaxAtt(dropType),vBW(dropType),vTilt,vMaxAtt(dropType));
//                         break;
//                     default:
//                         cout<<"Unknown Antenna Type.."<<endl;abort();
//                         break;
//                 }
//             }
//         }
//     }
//     cout<<"Loading AAS config for SCM CHannel."<<endl;
//     
//     loadAASConfig(AASConfigFile,sysTerrain.scmScenario,isToLoadAAS);
// }
void McellSystem::loadAntennaInfo(string AASConfigFile,string mySysConfigFile,bool isToLoadAAS)
{
    string terrain=sysTerrain.getTerrain();
    cout<<"terrain : "<<terrain<<endl;
    SCMType scmType = getSCMType(sysTerrain.scmScenario);
    
    if(scmType != _SCMTYPE_2D_) // 3D Channel parameters are parsed here
    {
        if(hexDropCommonVerticalTilt!=90)
            cout<<"Warning.. SCM model supports only mechanical vertical tilt as 90 degree w.r.t panel. Hence overwriting commonVerticalTilt internally..."<<endl;
        
        hexDropCommonVerticalTilt=90;
    }
    int totalNode_cnt=0;
    //cout << "aNodeTypes" << nNodes << endl;
    for(int type_cnt=0;type_cnt<aNodeTypes.length();type_cnt++) 
        
        for(int node_cnt=0;node_cnt<nNodes(type_cnt);node_cnt++,totalNode_cnt++)
        {
            aNodes(totalNode_cnt).setType(type_cnt);
            aNodes(totalNode_cnt).setID(totalNode_cnt);
            aNodes(totalNode_cnt).setActive();
        }
        
    vec hBW,vBW,hMaxAtt,vMaxAtt,macroAntGainIndB;
    Array<string> antennaType;
    parse(mySysConfigFile,"nodeAntennaType",antennaType); 
    parse(mySysConfigFile,"nodeAntennaHorizontalBeamWidth",hBW);               
    parse(mySysConfigFile,"nodeAntennaVerticalBeamWidth",vBW);
    if(!parse(mySysConfigFile,"nodeAntGainIndB",macroAntGainIndB))
    {
        cout<<"[detl:]Incorrect macroAntGainIndB value in mySysConfig... Aborting... "<<endl;
        abort();
    }
    parse(mySysConfigFile,"nodeAntennaHorizontalMaxAttenuation",hMaxAtt);
    parse(mySysConfigFile,"nodeAntennaVerticalMaxAttenuation",vMaxAtt);
    parse(mySysConfigFile,"antennaFeederLossIndB",antennaFeederLossIndB);
    
    vec antennaApertureRadius,GTRatio,antennaTemperature;
    parse(mySysConfigFile,"antennaApertureRadius",antennaApertureRadius);
    parse(mySysConfigFile,"GTRatio",GTRatio);
    if(!parse(mySysConfigFile,"antennaTemperature",antennaTemperature))
    {
        cout<<"[both:]Unable to parse antennaTemperature from mySysConfigFile. Loading 298K as default"<<endl;
        antennaTemperature = 298*ones(aNodeTypes.length());
    }
    int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_); 
    if(hexDropNodeType==-1)
        hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_RANDOMHEX_); 
    int inH3SecNodeType = find(nodeDropMethods,_DROPMETHOD_INBUILDING3SEC_); 
    int inHNodeType = find(nodeDropMethods,_DROPMETHOD_INBUILDING_); 
    int singleCellNodeType=find(nodeDropMethods,_DROPMETHOD_SINGLE_CELL_);
    
    int bsNodeType=-1;
    
    if(hexDropNodeType!=-1) bsNodeType = hexDropNodeType;
    else if(inH3SecNodeType!=-1) bsNodeType = inH3SecNodeType;
    else if(inHNodeType!=-1) bsNodeType = inHNodeType;
    else if(singleCellNodeType!=-1) bsNodeType = singleCellNodeType;

    int satNodeType = find(nodeDropMethods,_DROPMETHOD_SATELLITE_);
    if(satNodeType!=-1) bsNodeType = satNodeType;
    
    
    if(bsNodeType!=-1)
    {
        AntennaType_E antType = getAntennaType(antennaType(bsNodeType));
        vec hPattern,vPattern;
        if(antType==_ANTENNA_TYPE_CUSTOM_)
        {
            loadCustomAntennaPatterns(mySysConfigFile,hPattern,vPattern,aNodeTypes(bsNodeType));
        }
        ivec bsNodes = getNodes(aNodeTypes(bsNodeType));
        double sectorAngle = 360.0/nSectorsPerSite;
        vec hTilts = ((singleCellNodeType!=-1) ? 90.0 : (inHNodeType!=-1) ? 0.0 : 60.0) + getVector(0,(nSectorsPerSite-1)*sectorAngle,sectorAngle);//(nSectorsPerSite==3) ? "60 180 300" : "60 120 180 240 300 360";
        
        for(int bsNode_cnt=0;bsNode_cnt<bsNodes.length();bsNode_cnt++)
        {
            double sectorTilt = hTilts(bsNode_cnt%nSectorsPerSite);
            switch(antType)
            {
                case _ANTENNA_TYPE_OMNI_:
                {
                    if(bsNode_cnt==0) cout<<"[detl:]Loading Antenna for "<<aNodeTypes(bsNodeType)<<" as "<<" a omni-directional with AntGain = "<<macroAntGainIndB(bsNodeType)<<" dBi"<<endl;
                    aNodes(bsNodes(bsNode_cnt)).setAntenna(_ANTENNA_TYPE_OMNI_,0,0,-1,sectorTilt,-1,-1,hexDropCommonVerticalTilt,-1); // Loading default values for Omni Directional Antenna.
                        break;
                }
                case _ANTENNA_TYPE_CUSTOM_:
                    aNodes(bsNodes(bsNode_cnt)).setOrientation(sectorTilt,0,0);
                    aNodes(bsNodes(bsNode_cnt)).setAntenna(_ANTENNA_TYPE_CUSTOM_,macroAntGainIndB(bsNodeType),antennaFeederLossIndB(bsNodeType),sectorTilt,hexDropCommonVerticalTilt,hPattern,vPattern);
                        break;
                case _ANTENNA_TYPE_PARABOLIC_:
                {
                    if(bsNode_cnt==0) 
                    {
                        cout<<"[detl:]Loading Antenna for "<<aNodeTypes(bsNodeType)<<" as "<<" a parabolic sectorized antenna with \n HorBW = "<<hBW(bsNodeType)<<"deg, VerBW = "<<vBW(bsNodeType)<<"deg, VerTilt="<<hexDropCommonVerticalTilt<<" AntGain = "<<macroAntGainIndB(bsNodeType)<<" dBi"<<endl;
                        cout<<"[detl:]With Sector antennas facing "<<hTilts<<endl;
                    }
                    
                    aNodes(bsNodes(bsNode_cnt)).setOrientation(sectorTilt,0,0);
                    aNodes(bsNodes(bsNode_cnt)).setAntenna(_ANTENNA_TYPE_PARABOLIC_,macroAntGainIndB(bsNodeType),antennaFeederLossIndB(bsNodeType),hBW(bsNodeType),sectorTilt,hMaxAtt(bsNodeType),vBW(bsNodeType),hexDropCommonVerticalTilt,vMaxAtt(bsNodeType));
                    aNodes(bsNodes(bsNode_cnt)).txAntenna.isForHexNode=true;
                    aNodes(bsNodes(bsNode_cnt)).rxAntenna.isForHexNode=true;
                    break;
               
                }
                case _ANTENNA_TYPE_CIRCULAR_:
                    aNodes(bsNodes(bsNode_cnt)).setAntenna(_ANTENNA_TYPE_CIRCULAR_,macroAntGainIndB(bsNodeType),antennaFeederLossIndB(bsNodeType),antennaApertureRadius(bsNodeType),antennaTemperature(bsNodeType),GTRatio(bsNodeType));
                    break;
                case _ANTENNA_TYPE_HAPS_:
                {
                    if(bsNode_cnt==0) cout<<"[detl:]Loading Antenna for "<<aNodeTypes(bsNodeType)<<" as HAPS with 7 cells"<<endl;
                    
                    int haps_M_centre, haps_M_neighbour, haps_N;
                    double haps_dv, haps_dh, haps_Ge_max, haps_Am, haps_SLAv;
                    double haps_phi3dB, haps_theta3dB, haps_cellRadius;

                    parse(mySysConfigFile, "haps_M_centre", haps_M_centre);
                    parse(mySysConfigFile, "haps_M_neighbour", haps_M_neighbour);
                    parse(mySysConfigFile, "haps_N", haps_N);
                    parse(mySysConfigFile, "haps_dv", haps_dv);
                    parse(mySysConfigFile, "haps_dh", haps_dh);
                    parse(mySysConfigFile, "haps_Ge_max", haps_Ge_max);
                    parse(mySysConfigFile, "haps_Am", haps_Am);
                    parse(mySysConfigFile, "haps_SLAv", haps_SLAv);
                    parse(mySysConfigFile, "haps_phi3dB", haps_phi3dB);
                    parse(mySysConfigFile, "haps_theta3dB", haps_theta3dB);
                    parse(mySysConfigFile, "haps_cellRadius", haps_cellRadius);

                    // Generate cell center locations for 7 cells
                    Array<Location_S> cellCentres = generateHexCellCenterLocation(7, haps_cellRadius);
                    
                    // For HAPS, we assume 1 gNB node with 7 beams (modelled as 7 server nodes here)
                    // The center beam is node 0, neighbours are 1-6.
                    int beamIdx = bsNode_cnt % 7;
                    int M = (beamIdx == 0) ? haps_M_centre : haps_M_neighbour;
                    
                    Location_S centreLoc = cellCentres(beamIdx);
                    centreLoc.z = 0.0; // Pointing to ground level
                    
                    aNodes(bsNodes(bsNode_cnt)).setHAPSAntenna(M, haps_N, haps_dv, haps_dh, 
                                                                haps_Ge_max, haps_Am, haps_SLAv, 
                                                                haps_phi3dB, haps_theta3dB, 
                                                                antennaFeederLossIndB(bsNodeType));
                    aNodes(bsNodes(bsNode_cnt)).setHAPSAntennaTilts(centreLoc);
                    break;
                }
                default:
                        cout<<"Unknown Antenna Type..."<<endl;abort();
                        break;
            }
        }
    }
    
    ivec randomDropType = find(nodeDropMethods,_DROPMETHOD_RANDOM_,true);
    for(int type_cnt=0;type_cnt<randomDropType.length();type_cnt++)
    {
        int dropType = randomDropType(type_cnt);
        ivec nodes = getNodes(aNodeTypes(dropType));
        if(nodes.length())
        {
            AntennaType_E antType = getAntennaType(antennaType(dropType));
            vec hPattern,vPattern;
            if(antType == _ANTENNA_TYPE_CUSTOM_)
                loadCustomAntennaPatterns(mySysConfigFile,hPattern,vPattern,aNodeTypes(dropType));
            for(int node_cnt=0;node_cnt<length(nodes);node_cnt++)
            {
                double hTilt = randu()*360.0,vTilt = 90.0;
                aNodes(nodes(node_cnt)).setOrientation(hTilt,0,0);
                switch(antType)
                {
                    case _ANTENNA_TYPE_OMNI_:
                        // Fix: Use dropType instead of type_cnt
                        aNodes(nodes(node_cnt)).setAntenna(_ANTENNA_TYPE_OMNI_,macroAntGainIndB(dropType),antennaFeederLossIndB(dropType),-1,0,-1,-1,hexDropCommonVerticalTilt,-1); // Loading default values for Omni Directional Antenna.
                        break;
                    case _ANTENNA_TYPE_CUSTOM_:
                        aNodes(nodes(node_cnt)).setAntenna(antType,macroAntGainIndB(dropType),antennaFeederLossIndB(dropType),hTilt,vTilt,hPattern,vPattern); 
                        break;
                    case _ANTENNA_TYPE_PARABOLIC_:
                        aNodes(nodes(node_cnt)).setAntenna(antType,macroAntGainIndB(dropType),antennaFeederLossIndB(dropType),hBW(dropType),hTilt,hMaxAtt(dropType),vBW(dropType),vTilt,vMaxAtt(dropType));
                        break;
//                     case _ANTENNA_TYPE_QUASI_ISOTROPIC_:
//                         aNodes(nodes(node_cnt)).setAntenna(antType,macroAntGainIndB(type_cnt),antennaFeederLossIndB(type_cnt),-1,-1,-1,vBW(type_cnt),vTilt,vMaxAtt(type_cnt));
//                         break;
//                     case _ANTENNA_TYPE_CIRCULAR_:
// 
//                         //double diff_elevattion[]={0.7717,2.9291,4.5934,5.0813,4.8799,5.2574,5.2804,9.3638,8.5696,10.3269,7.0583,7.7331,8.9551,6.8821,7.6976,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//                       //double diff_elevattion[]={0,4.5162,4.5162,4.5162,4.5162,4.5162,4.5162,8.9839,7.7942,8.9839,7.7942,8.9839,7.7942,8.9839,7.7942,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//                       //double elevattion[]={90,85.4838,85.4838,85.4838,85.4838,85.4838,85.4838,81.0161,82.2058,81.0161,82.2058,81.0161,82.2058,81.0161,82.2058,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//                         aNodes(nodes(node_cnt)).setOrientation(0,0,0);
//                         aNodes(nodes(node_cnt)).setAntenna(antType,macroAntGainIndB(type_cnt),antennaFeederLossIndB(type_cnt),antennaApertureRadius(type_cnt),antennaTemperature(type_cnt),GTRatio(type_cnt));
//                         break;
                }
            }
        }
    }
    cout<<"Loading AAS config for SCM CHannel."<<endl;
    
    loadAASConfig(AASConfigFile,sysTerrain.scmScenario, isToLoadAAS);
}
 /*!\fn initSystem()
  * \brief--> initializing the system like type,ID,category,carrier frequencies of the nodes when nodetype and config files are specified 
    *\param[in] nodetypeselector --> it gives the which type of nodes are to given to the system
    *\param[in] configFile --> the config file which is already made is given as input file
   */
 
 void McellSystem::initSystem(string mySysConfigFile, string AASConfigFile,bvec nodeTypeSelector){      

  string sysTerr;                                          
  if(parse(mySysConfigFile,"sysTerrain",sysTerr))            
	sysTerrain.setTerrain(sysTerr);

  sysTerrain.channelModelType = parseChannelModelType(mySysConfigFile);
  
  if(sysTerrain.scmScenario == _SCM5G_INDOOR_OFFICE_)
  {
      string indoorType;
      parse(mySysConfigFile,"indoorScenario",indoorType);
      if(indoorType == "_INDOOR_MIXED_OFFICE_")
          sysTerrain.indoorOfficeScenario = _INDOOR_MIXED_OFFICE_;
      else
          sysTerrain.indoorOfficeScenario = _INDOOR_OPEN_OFFICE_;
  }
    
  sysTerrain.loadIndoorFactoryParams(mySysConfigFile);
  
  
  
  
  bool optionalPL = false;
  parse(mySysConfigFile,"useOptionalPathLoss",optionalPL);
  sysTerrain.useOptionalPathLoss = optionalPL;
   
   if(!parse(mySysConfigFile,"isLMLC",sysTerrain.isLMLC))
   {
     cout<<"isLMLC is not decleared in mySysConfig.. Aborting..."<<endl;
    abort();
   }
  
  parse(mySysConfigFile,"cellISD",cellISD);                     
  if(parse(mySysConfigFile,"carrierFreqInMHzPerServerNodeType",carrierFreqInMHzPerServerNodeType)==false)
  {
    cout<<"[both:] Warning... unable to read carrierFreqInMHzPerServerNodeType. "<<endl;
    abort();
  }
    
  parse(mySysConfigFile,"minDist",minDist);
  
  vec nodeHeights;
  parse(mySysConfigFile,"nodeTypeHeights",nodeHeights);
  
  parse(mySysConfigFile,"commonVerticalTilt",hexDropCommonVerticalTilt);
  parse(mySysConfigFile,"nodeCounts",nNodes);

  cout << "nodeTypeSelector: " << nodeTypeSelector << endl;
  if(nodeTypeSelector.length())                            
  {
   if(nodeTypeSelector.length()!=nNodes.length())         
    {
      cout<<"[both:]Error. nodeTypeSelector for initSys() must be of equal length with the number of nodes in the config file."<<endl;abort();
      }
    nNodes=elem_mult(nNodes,to_ivec(nodeTypeSelector));
  }
  parse(mySysConfigFile,"nodeTypeNames",aNodeTypes);
  cout<<"check the nodeCounts ::"<<nNodes<<endl;
  
  cout<<"check the node types ::"<<aNodeTypes<<endl;
  parse(mySysConfigFile,"indoorNodePercentage",indoorNodePercentage);
  
  parse(mySysConfigFile,"pedestrianNodePercentage",pedestrianNodePercentage);
  
// vec serviceNodeVelocity;
//   parse(mySysConfigFile,"serviceNodeVelocity",serviceNodeVelocity);
//   
//   for(int i=1;i<aNodeTypes.length();i++)
//       if(nNodes(i)>0)
//       {
//         if(serviceNodeVelocity(i-1)<2.5 && pedestrianNodePercentage(i)==0 && indoorNodePercentage(i)!=100)
//         {
//             cout<<"[Warning:] UE in car cannot move less than 9 Kmph..."<<endl;abort();
//         }
//         else if(serviceNodeVelocity(i-1)>2.5 && pedestrianNodePercentage(i)!=0)
//         {
//             cout<<"[Warning:] Pedestrians cannnot move more than 9 Kmph.."<<endl;abort();
//         }
//       }
  
  parse(mySysConfigFile,"OtoIPenetrationModelLow_HighPercentage",OtoIPntrModlL_H_Percentage);
  cout<<"[detl] OtoIPntrModlL_H_Percentage:"<<OtoIPntrModlL_H_Percentage<<endl;
 
  bool createBuildings=false;
  parse(mySysConfigFile,"createBuildings",createBuildings);
  if(sum(indoorNodePercentage)>0 && createBuildings)
  {
    cout<<"[both:]Error. indoorNodePercentage can't be set along with building models."<<endl;abort();
  }
  
   
  if(!parse(mySysConfigFile,"isCrossLinksEnabled",isCrossLinksEnabled)){cout<<"[detl:]Disabling crosslinks by default."<<endl;isCrossLinksEnabled=false;};
  networkIDPerNodeType=ones_i(aNodeTypes.length());//! Creating first network-ID, i.e.,1, if Second-Network is created,network-ID, i.e.,2 will be added   
  nodeDropMethods=zeros_i(nNodes.length()); 
  Array<string> dropMethods;    
  
  if(parse(mySysConfigFile,"nodeDropMethods",dropMethods))      
  {
	for(int string_cnt=0;string_cnt<dropMethods.length();string_cnt++)
	  if((dropMethods(string_cnt))=="_DROPMETHOD_RANDOM_") nodeDropMethods(string_cnt)=_DROPMETHOD_RANDOM_;
      else if((dropMethods(string_cnt))=="_DROPMETHOD_HEXAGONAL_")     
      {
          if(nNodes(string_cnt)==3 || nNodes(string_cnt)==21 || nNodes(string_cnt)==57 || nNodes(string_cnt)==42 || nNodes(string_cnt)==114) //this count gives number of basestations(hexagonaldrop ) for 1,7.6 cell system//
              nodeDropMethods(string_cnt)=_DROPMETHOD_HEXAGONAL_;
          else
          {
              #ifdef USING_MPI
              if(currentTaskid==0) {          
                  #endif
                  if(nNodes(string_cnt)==1)
                  {
                      cout<<"[detl:]Overriding drop method for node type \""<<aNodeTypes(string_cnt)<<"\" to Single Cell as the count is not supported for Hexagonal Layout."<<endl;
                      nodeDropMethods(string_cnt)=_DROPMETHOD_SINGLE_CELL_; 
                  }
                  else
                  {
                      cout<<"[detl:]Overriding drop method for node type \""<<aNodeTypes(string_cnt)<<"\" to Random as the count is not supported for Hexagonal Layout."<<endl;
                      nodeDropMethods(string_cnt)=_DROPMETHOD_RANDOM_; 
                  }
                  #ifdef USING_MPI
              }
              #endif
          }
      }
      else if((dropMethods(string_cnt))=="_DROPMETHOD_RANDOMHEX_")     
      {
          if(nNodes(string_cnt)%3==0)
              nodeDropMethods(string_cnt)=_DROPMETHOD_RANDOMHEX_;
          else
          {
              #ifdef USING_MPI
              if(currentTaskid==0) {          
                  #endif
                  if(nNodes(string_cnt)==1)
                  {
                      cout<<"[detl:]Overriding drop method for node type \""<<aNodeTypes(string_cnt)<<"\" to Single Cell as the count is not supported for Hexagonal Layout."<<endl;
                      nodeDropMethods(string_cnt)=_DROPMETHOD_SINGLE_CELL_; 
                  }
                  else
                  {
                      cout<<"[detl:]Overriding drop method for node type \""<<aNodeTypes(string_cnt)<<"\" to Random as the count is not supported for Hexagonal Layout."<<endl;
                      nodeDropMethods(string_cnt)=_DROPMETHOD_RANDOM_; 
                  }
                  #ifdef USING_MPI
              }
              #endif
          }
      }
      else if((dropMethods(string_cnt))=="_DROPMETHOD_INBUILDING_") nodeDropMethods(string_cnt)=_DROPMETHOD_INBUILDING_;
      else if((dropMethods(string_cnt))=="_DROPMETHOD_INBUILDING3SEC_")
      {
          if(nNodes(string_cnt)%3)
          {
              cout<<"Node count of NodeType with _DROPMETHOD_INBUILDING3SEC_ as drop method should be a multiple of 3. "<<endl;abort();
          }
          nodeDropMethods(string_cnt)=_DROPMETHOD_INBUILDING3SEC_;
      }
      else if(dropMethods(string_cnt)=="_DROPMETHOD_SATELLITE_")
          nodeDropMethods(string_cnt) = _DROPMETHOD_SATELLITE_;
     else
	  {
#ifdef USING_MPI
if(currentTaskid==0) {
#endif
	    cout<<"[detl:]Unknown Drop Method "<<dropMethods(string_cnt)<<" in the config File for initSys(). Loading Random drop"<<endl;
#ifdef USING_MPI
}
#endif
	    nodeDropMethods(string_cnt)=_DROPMETHOD_RANDOM_; 
	  }
  }
  //abort();
  if(find(nodeDropMethods,_DROPMETHOD_INBUILDING3SEC_)!=-1)
  {
        nSectorsPerSite = 3;
  }
   else if(find(nodeDropMethods,_DROPMETHOD_SATELLITE_)!=-1)
  {
        parse(mySysConfigFile,"nBeamsPerSatellite",nBeamsPerSatellite);
        parse(mySysConfigFile,"frequencyReUseFactor",frequencyReUseFactor);

        // Check if HAPS antenna is used to avoid forced 19/61 beam tiers
        Array<string> nodeAntennaTypes;
        parse(mySysConfigFile, "nodeAntennaType", nodeAntennaTypes);
        bool isHAPS = false;
        for(int i=0; i<nodeAntennaTypes.length(); i++) {
            if(nodeAntennaTypes(i) == "_ANTENNA_TYPE_HAPS_") {
                isHAPS = true;
                break;
            }
        }

        if(isHAPS)
        {
            nSectorsPerSite = nBeamsPerSatellite;
        }
        else if(frequencyReUseFactor==1)
        {
            if(nBeamsPerSatellite==7) nSectorsPerSite = 19;
            else if(nBeamsPerSatellite==19) nSectorsPerSite = 61;
            else nSectorsPerSite = nBeamsPerSatellite; // fallback
        }
        else if(frequencyReUseFactor==3 || frequencyReUseFactor==2)
        {
            if(nBeamsPerSatellite==7) nSectorsPerSite = 37;
            else if(nBeamsPerSatellite==19){nSectorsPerSite = 127;}
            else nSectorsPerSite = nBeamsPerSatellite; // fallback
        }
        else
        {
            cout<<"Unsupported value of frequency reuse..."<<endl;abort();
        }
        nNodes(find(nodeDropMethods,_DROPMETHOD_SATELLITE_)) *=nSectorsPerSite;
  }
 else if(find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_)!=-1)
  {
      int nHexNodes = nNodes(find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_));
      if(nHexNodes%7==0)
          nSectorsPerSite = nHexNodes/7;
      else if(nHexNodes%19==0)
          nSectorsPerSite = nHexNodes/19;
      else 
          nSectorsPerSite = nHexNodes;
  }
  
  loadSystemDefault();
  
//   string cellLayt;                                           
//   if(parse(mySysConfigFile,"cellLayout",cellLayt))                
//   {
//     if(cellLayt=="_CELLLAYOUT_HEXAGONAL_")                   
//     {
//       if(findInStringArray(dropMethods,"_DROPMETHOD_HEXAGONAL_")==-1) 
//       {
// 	cout<<"[both:]Drop method must be Hexagonal for atleast one node type for Hexagonal Layout."<<endl;
// 	abort();
//       }
//       cellLayout=_CELLLAYOUT_HEXAGONAL_;                      
//     }
//     else if(cellLayt=="_CELLLAYOUT_RANDOM_") 
//       cellLayout=_CELLLAYOUT_RANDOM_;                       
//     else {cout<<"[both:]Unknown Cell Layout in the config File for initSys().";abort();} 
//   }
  
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_)                        
  {
    if(!parse(mySysConfigFile,"cellWiseDropping",cellWiseDropping))
    {
      cout<<"[detl:]Loading Sectorwise node dropping for all Random-Drop nodes by default."<<endl;
      cellWiseDropping=false;
    }
  }
  else  
  { 
    if(isNTNSCMScenario(sysTerrain.scmScenario))
    {
        sysTerrain.isSpConsistEnable = false;
        
        if(!parse(mySysConfigFile,"satelliteBeamRadius",satelliteBeamRadius))
        {
            cout<<"[both:]Error. Unable to parse coverageRadius from config file. Default value not possible."<<endl;abort();
        }
        if(!parse(mySysConfigFile,"beamElevationAngle",beamElevationAngle))
        {
            cout<<"[both:]Error. Unable to parse coverageRadius from config file. Default value not possible."<<endl;abort();
        }
    }
    else
    {    
    if(!parse(mySysConfigFile,"coverageLength",coverageArea.xLen))  
    {
      cout<<"[both:]Error. Unable to parse coverageLength from config file. Default value not possible."<<endl;abort();
    }
    if(!parse(mySysConfigFile,"coverageWidth",coverageArea.yLen))
    {
      cout<<"[both:]Error. Unable to parse coverageWidth from config file. Default value not possible."<<endl;abort();
    }
    if(sysTerrain.scmScenario == _SCM5G_INDOOR_FACTORY_)
    {
        if(!parse(mySysConfigFile,"roomHeight",coverageArea.zLen))
        {
            cout<<"[both:]Error. Unable to parse roomHeight from config file. Default value not possible."<<endl;abort();
        }
        int inhNodeType = find(nodeDropMethods,_DROPMETHOD_INBUILDING_);
        if(inhNodeType==-1)
            inhNodeType = find(nodeDropMethods,_DROPMETHOD_INBUILDING3SEC_);
        if(inhNodeType==-1)
        {
            cout<<"Invalid Drop Method for Indoor Factory.."<<endl;abort();
        }
        if(nodeHeights(inhNodeType)>coverageArea.zLen)
        {
            cout<<"BS height cannot be greater than roomHeight for Indoor Factory..."<<endl;abort();
        }
    }
    }
  }
  
  string shdwType;                         
  if(parse(mySysConfigFile,"shdwType",shdwType)) 
  {
    if(shdwType=="_SHADOWTYPE_CORRELATED_") shadowType=_SHADOWTYPE_CORRELATED_;
    else if(shdwType=="_SHADOWTYPE_UNCORRELATED_") shadowType=_SHADOWTYPE_UNCORRELATED_;
    else {cout<<"[detl:]Unknown Shadow Type in the config File (). Loading _SHADOWTYPE_UNCORRELATED_ as default.";shadowType=_SHADOWTYPE_UNCORRELATED_;}
  }

  int nNodesSize = sum(nNodes);
  aNodes.set_size(nNodesSize);
  nodeCategoryPerNodeType=parseNodeCategory(mySysConfigFile,"nodeCategory");

  int totalNode_cnt=sum(nNodes(0,aNodeTypes.length()-1));
  nodeIDs=getIntegers(0,totalNode_cnt-1);
  
  ivec nodeList;
  nodeList.set_size(0);
  parse(mySysConfigFile,"NODESELFCONFIG",nodeList);
  for(int index=0;index<nodeList.size();index++){
	aNodes(nodeList(index)).selfConfig(mySysConfigFile);
  }

  if(find(nodeDropMethods,_DROPMETHOD_INBUILDING3SEC_)!=-1 ||  find(nodeDropMethods,_DROPMETHOD_RANDOMHEX_)!=-1)
  {
        nSectorsPerSite = 3;
  }
  else if(find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_)!=-1)
  {
      int nHexNodes = nNodes(find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_));
      if(nHexNodes%7==0)
          nSectorsPerSite = nHexNodes/7;
      else if(nHexNodes%19==0)
          nSectorsPerSite = nHexNodes/19;
      else 
          nSectorsPerSite = nHexNodes;
  }
  else if(find(nodeDropMethods,_DROPMETHOD_SINGLE_CELL_)!=-1||  find(nodeDropMethods,_DROPMETHOD_INBUILDING_)!=-1)
  {
    nSectorsPerSite=1;
  }
  
    loadAntennaInfo(AASConfigFile,mySysConfigFile);
    for(int node_cnt=0;node_cnt<aNodes.length();node_cnt++)
    {
        aNodes(node_cnt).rxAntenna=aNodes(node_cnt).txAntenna;
    }
}

/*! \fn initNodes()
 * \brief--> initiating the system nodes if the config type file is given as the input like server nodes ,service nodes,node heights
 * \brief --> defining sadowing type seperatly for SCM channel
 * \param [in] configFile ---> The systems configfile which is already defined is given as input
 */
void McellSystem::initNodes(string mySysConfigFile, string AASConfigFile){

  ivec srvrNodeTypes(0);
  nodeCategoryPerNodeType=parseNodeCategory(mySysConfigFile,"nodeCategory");
  for(int nodeCat_cnt=0;nodeCat_cnt<aNodeTypes.length();nodeCat_cnt++)
  if(nNodes(nodeCat_cnt)!=0)
  {
    setNodeCategory(nodeCat_cnt,nodeCategoryPerNodeType(nodeCat_cnt));
    if(nodeCategoryPerNodeType(nodeCat_cnt)==_NODECATEGORY_SERVERNODE_)
      append(srvrNodeTypes,nodeCat_cnt);
  }
  
  if(srvrNodeTypes.length()!=carrierFreqInMHzPerServerNodeType.length())
  {
      cout<<"Error. Carriers not set for all server nodes.."<<endl;
      abort();
  }
  
  for(int srvr_cnt=0;srvr_cnt<srvrNodeTypes.length();srvr_cnt++)
        setNodeCarrierFrequencyInHz(srvrNodeTypes(srvr_cnt),carrierFreqInMHzPerServerNodeType(srvr_cnt)*1e6);

  string channelGenMethod="";
  parse(mySysConfigFile,"channelGenerationMethod",channelGenMethod);
  bool isWithScmShadow=false;
  parse(mySysConfigFile,"isWithScmShadow",isWithScmShadow);
  if(channelGenMethod=="_CHANNEL_GENERATION_METHOD_SCM_")
    isWithScmShadow=true;

  if(!isWithScmShadow)
  {
     if(shadowType==_SHADOWTYPE_CORRELATED_)
     {
       double corrDistance, siteTositeCorr;
       if(parse(mySysConfigFile,"corrDistance",corrDistance) && parse(mySysConfigFile,"siteTositeCorr",siteTositeCorr))
	initCorrelatedShdw(srvrNodeTypes,corrDistance,siteTositeCorr);
       else
	initCorrelatedShdw(srvrNodeTypes);
     }
  }
  else
  {
    SCMPropagationScenario scmScenario=sysTerrain.scmScenario;
//     if(is5GSCMScenario(scmScenario) || isNTNSCMScenario(scmScenario))
//     {
//         if(!parse(mySysConfigFile,"useSOSForSpCorr",sysTerrain.scmInfo.useSOSForSpCorr))
//         {
//             sysTerrain.scmInfo.useSOSForSpCorr = false;
//         }
//         if(sysTerrain.scmInfo.useSOSForSpCorr)
//         {
//             string acfType;int nCoeff;
//             parse(mySysConfigFile,"acfType",acfType);
//             parse(mySysConfigFile,"nCoefficients",nCoeff);
//             sysTerrain.scmInfo.acfType = getACF(acfType,nCoeff);
//         }
//     }
      initSCMParameters(srvrNodeTypes,scmScenario);
  }

  if(parse(mySysConfigFile,"nodeTypeHeights",nodeTypeHeights))
  {
    for(int type_cnt=0;type_cnt<nodeTypeHeights.length();type_cnt++)
      setNodeHeight(type_cnt,nodeTypeHeights(type_cnt));
  }
  
  bool isTimeDomain=false;
  parse(mySysConfigFile,"timeDomain",isTimeDomain);
  
  Array<ivec> nodeNumerologies;
  if(parse(mySysConfigFile,"nodeNumerologies",nodeNumerologies))
  {
    for(int type_cnt=0;type_cnt<nodeNumerologies.length();type_cnt++)
    {
      if(nodeNumerologies(type_cnt).length()>1 && !isTimeDomain)
      {
	cout<<"Multiple Numerolgoies supported only with timeDomain simulation.."<<endl;abort();
      }
      setNodeNumerologies(type_cnt,nodeNumerologies(type_cnt));
    }
  }

  vec nodeTransmitPowerInDBm;
  if(parse(mySysConfigFile,"nodeTransmitPowerInDBm",nodeTransmitPowerInDBm))
  {
	for(int type_cnt=0;type_cnt<nodeTransmitPowerInDBm.length();type_cnt++)
	  setNodeTxPowerInDBm(type_cnt,nodeTransmitPowerInDBm(type_cnt));
  }
  ivec nodeAntennaCount;      
//   if(parse(mySysConfigFile,"nodeAntennaCount",nodeAntennaCount))
  Array<ivec > TXRUMapTemp;Array<ivec> AntStructureTemp;
  if(parse(AASConfigFile,"TXRUMap",TXRUMapTemp)&&parse(AASConfigFile,"AntStructure",AntStructureTemp)){
      nodeAntennaCount.set_length(TXRUMapTemp.length());
      for(int nodeTypeCnt = 0;nodeTypeCnt < TXRUMapTemp.length();nodeTypeCnt++){
          nodeAntennaCount(nodeTypeCnt) = prod(TXRUMapTemp(nodeTypeCnt))*AntStructureTemp(nodeTypeCnt)(4);
          setNodeAntennaCount(nodeTypeCnt,nodeAntennaCount(nodeTypeCnt));
      }
  }else{cout<<"Unable to parse TXRUMap and AntStructure from ActiveAntennaSystemConfig.txt .. aborting"<<endl; abort();}
  
//   if(parse(AASConfigFile,"numTxRUs",nodeAntennaCount)) // antenna count = TxRUs assumed
//   {
//     for(int type_cnt=0;type_cnt<nodeAntennaCount.length();type_cnt++)
//       setNodeAntennaCount(type_cnt,nodeAntennaCount(type_cnt));
//   }else{cout<<"Unable to parse numTxRUs from mySysConfig.txt .. aborting"<<endl; abort();}

  if(!parse(mySysConfigFile,"nodeReceiveNoiseFigureInDB",noiseFigurePerNodeType))
  {
    cout<<"[both:]Error. Unable to parse nodeReceiveNoiseFigureInDB from mySysConfigFile."<<endl;abort();
  }

}

// function initSCMParameters() selects either 2DSCM or 3D Channel Model
void McellSystem::initSCMParameters(ivec gridHolderNodeTypes,SCMPropagationScenario scmScenario){
  
    SCMType scmType = getSCMType(scmScenario);
    switch(scmType)
    {
        case _SCMTYPE_2D_:
            init2DSCMParameters(gridHolderNodeTypes,scmScenario);
            break;
        case _SCMTYPE_3D_:
            init3DChannelParameters(gridHolderNodeTypes,scmScenario);
            break;
        case _SCMTYPE_5G_:
        case _SCMTYPE_NTN_:
            init5GChannelParameters(gridHolderNodeTypes,scmScenario);
            break;
        default:
            cout<<"Unknown SCM propagationScenario. Aborting ...."<<endl;abort();
            break;
    }
}


void McellSystem::init2DSCMParameters(ivec gridHolderNodeTypes,SCMPropagationScenario scmScenario){

  if(coverageArea.xLen==-1)
  {
    cout<<"[both:]Error. Please call initSystem before calling initSCMParameters()."<<endl;
    abort();
  }
  sysTerrain.isForSCMChannel=true;
  sysTerrain.scmInfo.coverageArea=coverageArea;
  sysTerrain.scmInfo.lspMuSigma="-7.70 -7.41 -1 -7.19 -6.89 -6.62 -7.03 -6.44 -1 -7.49 -7.43 -1;"
  "0.18 0.14 -1 0.40 0.54 0.32 0.66 0.39 -1 0.55 0.48 -1;"
  "1.60 1.62 -1 1.20 1.41 1.25 1.15 1.41 -1 0.90  0.95 -1;"
  "0.18  0.25 -1  0.43  0.17  0.42  0.28  0.28 -1  0.38  0.45 -1;"
  "1.62  1.77 -1  1.75  1.84  1.76  1.81  1.87 -1  1.52  1.52 -1;"
  "0.22  0.16 -1  0.19  0.15  0.16  0.20  0.11 -1  0.24  0.13 -1;"
  "-1  -1  -1  -1   -1   -1   -1   -1  -1  -1   -1  -1;"
  "3 4 -1  3  4 7 4 6 -1 4 8 -1;"
  "7 -1  -1 9 -1 -1 9 -1 -1 7 -1 -1;"
  "4 -1  -1 5 -1 -1 3.5 -1 -1 4 -1 -1;";

  sysTerrain.scmInfo.correlationDistance = "8 5 -1 7 10 10 30 40 -1 50 36 -1;"
  "7 3 -1 8 10 11 18 50 -1 25 30 -1;"
  "5 3 -1 8 9 17 15 50 -1 35 40 -1;"
  "10 6 -1 10 13 7 37 50 -1 37 120 -1;"
  "4 -1 -1 15 -1 -1 12 -1 -1 40 -1 -1;";
 
  ivec tempNodes;
  for(int type_cnt=0;type_cnt<length(gridHolderNodeTypes);type_cnt++)
  {
    tempNodes=getNodes(gridHolderNodeTypes(type_cnt));
    for(int node_cnt=0;node_cnt<length(tempNodes);node_cnt++)
      aNodes(tempNodes(node_cnt)).initGridsForSCMLargeScaleParameters(sysTerrain.scmInfo,scmScenario);
  }
  initGridsForSCMLargeScaleParameters(sysTerrain.commonSCMGrids,sysTerrain.scmInfo,scmScenario);
}

void McellSystem::init3DChannelParameters(ivec gridHolderNodeTypes,SCMPropagationScenario scmScenario){

  if(coverageArea.xLen==-1)
  {
    cout<<"[both:]Error. Please call initSystem before calling initSCMParameters()."<<endl;
    exit(0);
  }
  
  /*    
   *   Table 7.3-6  3GPP TR 36.873 V2.0.0 (2014-03)
   * This is Matrix form for sysTerrain.channel3DInfo.lspMuSigma
   * 
   *           3D-UMi                    3D-UMa
   *        ---------------         ----------------
   *        LOS   NLOS O2I        LOS   NLOS  O2I
   * DS 
   * 
   * ASD
   * 
   * ASA  
   *    
   * SF
   * 
   * K
   * 
   * ZSA   
   */
  
  sysTerrain.isForSCMChannel=true;
  sysTerrain.scmInfo.coverageArea=coverageArea;
  sysTerrain.scmInfo.lspMuSigma="-7.19 -6.89 -6.62 -7.03 -6.44 -6.62;"
                                      " 0.40  0.54  0.32  0.66  0.39  0.32;"
				      " 1.20  1.41  1.25  1.15  1.41  1.25;"
				      " 0.43  0.17  0.42  0.28  0.28  0.42;"
				      " 1.75  1.84  1.76  1.81  1.87  1.76;"
				      " 0.19  0.15  0.16  0.20  0.11  0.16;"				      
				      "  -1    -1    -1    -1    -1    -1;"
				      "  3      4     7     4     6     7;"
				      "  9     -1    -1     9    -1    -1;"
				      "  5     -1    -1    3.5   -1    -1;"
				      " 0.60  0.88  1.01  0.95  1.26  1.01;"
				      " 0.16  0.16  0.43  0.16  0.16  0.43;";
   

  sysTerrain.scmInfo.correlationDistance ="7 10 10 30 40 10;"
                                                "8 10 11 18 50 11;"
						"8  9 17 15 50 17;"
						"10 13 7  37 50 7;"
						"15 -1 -1 12 -1 -1;"
						"12  10 25 15 50 25;"
						"12 10 25 15 50 25";
    
                                                 
                                         
  ivec tempNodes;
  for(int type_cnt=0;type_cnt<length(gridHolderNodeTypes);type_cnt++)
  {
    tempNodes=getNodes(gridHolderNodeTypes(type_cnt));
    for(int node_cnt=0;node_cnt<length(tempNodes);node_cnt++)
      aNodes(tempNodes(node_cnt)).initGridsForSCMLargeScaleParameters(sysTerrain.scmInfo,scmScenario);
  }
  initGridsForSCMLargeScaleParameters(sysTerrain.commonSCMGrids,sysTerrain.scmInfo,scmScenario);
}


void McellSystem :: init5GChannelParameters(ivec gridHolderNodeTypes,SCMPropagationScenario scmScenario)
{
    //bool isInitParameters = 0;
    if(coverageArea.xLen==-1)
    {
        cout<<"[both:]Error. Please call initSystem before calling initSCMParameters()."<<endl;
        exit(0);
    }
    /*    
     *   Table 7.3-6  3GPP TR 38.901-(2017)
     * This is Matrix form for sysTerrain.channel3DInfo.lspMuSigma
     * 
     *           5G-UMi                    5G-UMa                  5G-RMa              5G-Indoor
     *        ---------------         ----------------       ------------------      ----------------
     *        LOS   NLOS O2I           LOS   NLOS  O2I           LOS   NLOS           LOS   NLOS  
     * DS 
     * 
     * ASD
     * 
     * ASA  
     *    
     * SF
     * 
     * K
     * 
     * ZSA   
     */
    
    
    sysTerrain.isForSCMChannel=true;
    sysTerrain.scmInfo.coverageArea=coverageArea;
    //bool isMmWaveCarrier = 0;
    double mmWaveCarrierFreqInHz = 0 ;
    ivec tempNodes;
    
    mmWaveCarrierFreqInHz = carrierFreqInMHzPerServerNodeType(0)*1e6; // Need to sort out - Dhiv
    
    // for frq greater than 6GHz as per TR 38.901 table 7.5-6 Part I part II
    {
        //cout<<"[both:] 5G Channel parameter are configured for carrier frequency "<< double(mmWaveCarrierF/1e9) <<"GHz. Make sure same is used for all server node types.."<<endl;
        sysTerrain.scmInfo.lspMuSigma = lspMuSigmaCal(double(mmWaveCarrierFreqInHz/1e9),sysTerrain.channelModelType);
        sysTerrain.scmInfo.correlationDistance = correlationDistCal(double(mmWaveCarrierFreqInHz/1e9));
        sysTerrain.scmInfo.dcorrForSPConsistency = dcorrForSPConsistencyCal();
//         if(!sysTerrain.scmInfo.useSOSForSpCorr)
        if(!isNTNSCMScenario(scmScenario))
        {
            for(int type_cnt=0;type_cnt<length(gridHolderNodeTypes);type_cnt++)
            {
                tempNodes=getNodes(gridHolderNodeTypes(type_cnt));
                for(int node_cnt=0;node_cnt<length(tempNodes);node_cnt++)
                {
                    aNodes(tempNodes(node_cnt)).initGridsForSCMLargeScaleParameters(sysTerrain.scmInfo,scmScenario,sysTerrain.infParams.scenario);
                    aNodes(tempNodes(node_cnt)).initGridsForSpatialConsistencyParameters(sysTerrain.scmInfo,scmScenario);
                }
            }
            initGridsForSCMLargeScaleParameters(sysTerrain.commonSCMGrids,sysTerrain.scmInfo,scmScenario,sysTerrain.infParams.scenario);  
            // initGridsForSpatialConsistencyParameters(sysTerrain.commonSCMGridsForSpConsist,sysTerrain.scmInfo,scmScenario);
        }
    }
}

mat getlspMuSigmaForCalibration(double fc)
{
    mat lms(12,11);
    int r = 0,c = 0;
    double lfc = log10(1+fc);
    
      // Delay spread (lgDS) lgDS=log10(DS/1s)
      lms(r,c++) = -0.24*lfc-7.14;      lms(r,c++) = -0.24*lfc-6.83;        lms(r,c++) = -6.62;/*UMi*/  lms(r,c++) = -6.955-0.0963*log10(fc);       lms(r,c++) = -6.28-0.204*log10(fc);         lms(r,c++) = -6.62;/*UMa*/     lms(r,c++) = -7.49;     lms(r,c++) = -7.43;     lms(r,c++) = -7.47/*RUma O2I*/;     lms(r,c++) = -0.01*lfc-7.79;        lms(r,c++) = -0.28*lfc-7.29;    r++;c = 0;
      lms(r,c++) = 0.38;                lms(r,c++) = 0.16*lfc+0.28;         lms(r,c++) = 0.32;          lms(r,c++) = 0.66;                          lms(r,c++) = 0.39;                          lms(r,c++) = 0.32;             lms(r,c++)  = 0.55;     lms(r,c++) = 0.48;      lms(r,c++) = 0.24/*RUma O2I*/;      lms(r,c++) = -0.16*lfc+0.5;         lms(r,c++) = 0.10*lfc+0.11;     r++;c = 0;
      
      // AoD spread (gASD) gASD=log10(ASD/1degree)  
      lms(r,c++) = -0.05*lfc+1.21;      lms(r,c++) = -0.23*lfc+1.53;        lms(r,c++) = 1.25;/*UMi*/   lms(r,c++) = 1.06+0.1114*log10(fc);         lms(r,c++) = 1.5-0.1144*log10(fc);          lms(r,c++) = 1.25;/*UMa*/      lms(r,c++) = 0.90;      lms(r,c++) = 0.95;      lms(r,c++) = 0.67/*RUma O2I*/;      lms(r,c++) = 1.60;                  lms(r,c++) = 1.49;              r++;c = 0;
      lms(r,c++) = 0.41;                lms(r,c++) = 0.11*lfc+0.33;         lms(r,c++) = 0.42;          lms(r,c++) = 0.28;                          lms(r,c++) = 0.28;                          lms(r,c++) = 0.42;             lms(r,c++)  = 0.38;     lms(r,c++) = 0.45;      lms(r,c++) = 0.18/*RUma O2I*/;      lms(r,c++) = 0.18;                  lms(r,c++) = 0.17;              r++;c = 0;
      
      // AoA spread (lgASA) lgASA=log10(ASA/1degree)  
      lms(r,c++) = -0.08*lfc+1.73;      lms(r,c++) = -0.08*lfc+1.81;        lms(r,c++) = 1.76;/*UMi*/   lms(r,c++) = 1.81;                          lms(r,c++) = 2.08-0.27*log10(fc);           lms(r,c++) = 1.76;/*UMa*/      lms(r,c++)  = 1.52;     lms(r,c++) = 1.52;      lms(r,c++) = 1.66/*RUma O2I*/;      lms(r,c++) = -0.19*lfc+1.86;        lms(r,c++) = -0.11*lfc+1.80;    r++;c = 0;
      lms(r,c++) = 0.014*lfc+0.28;      lms(r,c++) = 0.05*lfc+0.3;          lms(r,c++) = 0.16;          lms(r,c++) = 0.20;                          lms(r,c++) = 0.11;                          lms(r,c++) = 0.16;             lms(r,c++)  = 0.24;     lms(r,c++) = 0.13;      lms(r,c++) = 0.21/*RUma O2I*/;      lms(r,c++) = 0.12*lfc;              lms(r,c++) = 0.12*lfc;          r++;c = 0;
      
      // Shadow fading (SF) [dB]
      lms(r,c++) = -1;                  lms(r,c++) = -1;                    lms(r,c++) =-1;   /*UMi*/   lms(r,c++) =-1;                             lms(r,c++) = -1;                            lms(r,c++) = -1;  /*UMa*/      lms(r,c++)  = -1;       lms(r,c++) = -1;        lms(r,c++) = -1/*RUma O2I*/;        lms(r,c++) =-1;                     lms(r,c++) = -1;                r++;c = 0;
      lms(r,c++) = 4.0;                 lms(r,c++) = 7.82;                  lms(r,c++) = 7.0;           lms(r,c++) = 4.0;                           lms(r,c++) = 6.0;                           lms(r,c++) = 7.0;              lms(r,c++)  = 4.0;      lms(r,c++) = 8.0;       lms(r,c++) = 8.0/*RUma O2I*/;       lms(r,c++) = 3.0;                   lms(r,c++) = 8.03;              r++;c = 0;
      
      //K-factor (K) [dB]
      lms(r,c++) = 9;                   lms(r,c++) = -1;                    lms(r,c++) =-1;   /*UMi*/   lms(r,c++) = 9;                             lms(r,c++) = -1;                            lms(r,c++) = -1;  /*UMa*/      lms(r,c++)  = 7;        lms(r,c++) = -1;        lms(r,c++) = -1/*RUma O2I*/;        lms(r,c++) = 0.84*lfc+2.12;         lms(r,c++) = -1;                r++;c = 0;
      lms(r,c++) = 5;                   lms(r,c++) = -1;                    lms(r,c++) = -1;            lms(r,c++) = 3.5;                           lms(r,c++) = -1;                            lms(r,c++) = -1;               lms(r,c++)  = 4;        lms(r,c++) = -1;        lms(r,c++) = -1/*RUma O2I*/;        lms(r,c++) = -0.58*lfc+6.19;        lms(r,c++) = -1;                r++;c = 0;
      
      //ZoA spread (lgZSA) lgZSA=log10(ZSA/1degree)  
      lms(r,c++) = -0.1*lfc+0.73;       lms(r,c++) = -0.04*lfc+0.92;        lms(r,c++) = 1.01;/*UMi*/   lms(r,c++) = 0.95;                          lms(r,c++) = -0.3236*log10(fc)+1.512;       lms(r,c++) = 1.01;/*UMa*/      lms(r,c++)  = 0.47;     lms(r,c++) = 0.58;      lms(r,c++) = 0.93/*RUma O2I*/;      lms(r,c++) = -0.26*lfc+1.21;        lms(r,c++) = -0.15*lfc+1.04;    r++;c = 0;
      lms(r,c++) = -0.04*lfc+0.34;      lms(r,c++) = -0.07*lfc+0.41;        lms(r,c++) = 0.43;          lms(r,c++) = 0.16;                          lms(r,c++) = 0.16;                          lms(r,c++) = 0.43;             lms(r,c++)  = 0.40;     lms(r,c++) = 0.37;      lms(r,c++) = 0.22/*RUma O2I*/;      lms(r,c++) = -0.04*lfc+0.17;        lms(r,c++) = -0.09*lfc+0.24;    r++;c = 0;
      
      return lms;
}

mat McellSystem::lspMuSigmaCal(double carrierFreqInGHz,ChannelModelType channelModelType)
{
  double fc =carrierFreqInGHz;
  #ifdef _USING_CALIBRATION_
  if(channelModelType == _MODEL_B_ || fc>6)
      return getlspMuSigmaForCalibration(fc);
  #endif
  if((sysTerrain.scmScenario == _SCM5G_URBAN_MACRO_ || sysTerrain.scmScenario == _SCM5G_INDOOR_OFFICE_) && fc<6)
  {
    fc = 6;  // TR 38.901 foot node after Table 7.5-6 Part-1: Channel model parameters UMa and indoor
  }
  else if(sysTerrain.scmScenario == _SCM5G_URBAN_MICRO_ && fc<2)
  {
    fc = 2;  // TR 38.901 foot node after Table 7.5-6 Part-1: Channel model parameters UMa and indoor
  }
  mat lms(12,13);
  int r = 0,c = 0;
  double lfc = log10(1+fc);
  
  double V = coverageArea.getVolume();
  double S = coverageArea.getSurfaceArea(1);
  double V_S_ratio = (V/S);
  
  
  // Delay spread (lgDS) lgDS=log10(DS/1s)

    if(channelModelType == _MODEL_A_ && (fc >= 0.5 && fc<=6))
    {
      if(sysTerrain.scmScenario == _SCM5G_INDOOR_FACTORY_)
      {
          cout<<"Mode A incomplete for InF"<<endl;
          abort();
      }
      // Delay spread (lgDS) lgDS=log10(DS/1s)
      lms(r,c++) = -7.19;  lms(r,c++) = -6.89;  lms(r,c++) = -6.62;  lms(r,c++) = -7.03;  lms(r,c++) = -6.44;  lms(r,c++) = -6.62;  lms(r,c++) = -7.49;  lms(r,c++) = -7.43; lms(r,c++) = -7.47/*RUma O2I*/; lms(r,c++) = -7.70;  lms(r,c++) = -7.41; r++;c = 0;
      lms(r,c++) = 0.40;  lms(r,c++) = 0.54;  lms(r,c++) = 0.32;  lms(r,c++) = 0.66;  lms(r,c++) = 0.39;  lms(r,c++) = 0.32;  lms(r,c++)  = 0.55;  lms(r,c++) = 0.48; lms(r,c++) = 0.24/*RUma O2I*/; lms(r,c++) = 0.18;  lms(r,c++) = 0.14;r++;c = 0;
      
      // AoD spread (gASD) gASD=log10(ASD/1degree)  
      lms(r,c++) = 1.20;  lms(r,c++) = 1.41;  lms(r,c++) = 1.25;  lms(r,c++) = 1.15; lms(r,c++) = 1.41;  lms(r,c++) = 1.25; lms(r,c++) = 0.90;  lms(r,c++) = 	0.95; lms(r,c++) = 0.67/*RUma O2I*/; lms(r,c++) =1.60;  lms(r,c++) = 1.62;r++;c = 0;
      lms(r,c++) = 0.43;  lms(r,c++) = 0.17;  lms(r,c++) = 0.42;  lms(r,c++) = 0.28;  lms(r,c++) = 0.28;  lms(r,c++) = 0.42;  lms(r,c++)  = 0.38;  lms(r,c++) = 0.45; lms(r,c++) = 0.18/*RUma O2I*/; lms(r,c++) = 0.18;  lms(r,c++) = 0.25;r++;c = 0;
      
      // AoA spread (lgASA) lgASA=log10(ASA/1degree)  
      lms(r,c++) = 1.75;  lms(r,c++) = 1.84;  lms(r,c++) = 1.76;  lms(r,c++) = 1.81; lms(r,c++) = 1.87;  lms(r,c++) = 1.76;  lms(r,c++)  = 1.52;  lms(r,c++) = 1.52; lms(r,c++) = 1.66/*RUma O2I*/; lms(r,c++) =1.62;  lms(r,c++) = 1.77;r++;c = 0;
      lms(r,c++) = 0.19;  lms(r,c++) = 0.15;  lms(r,c++) = 0.16;  lms(r,c++) = 0.20;  lms(r,c++) = 0.11;  lms(r,c++) = 0.16;  lms(r,c++)  = 0.24;  lms(r,c++) = 0.13; lms(r,c++) =0.21/*RUma O2I*/; lms(r,c++) =0.22;  lms(r,c++) = 0.16;r++;c = 0;
      
      // Shadow fading (SF) [dB]
      lms(r,c++) = -1;  lms(r,c++) = -1;  lms(r,c++) =-1;  lms(r,c++) =-1; lms(r,c++) = -1;  lms(r,c++) = -1;  lms(r,c++)  = -1;  lms(r,c++) = -1; lms(r,c++) = -1/*RUma O2I*/; lms(r,c++) =-1;  lms(r,c++) = -1;r++;c = 0;
      lms(r,c++) = 3.0;  lms(r,c++) = 4.0;  lms(r,c++) = 7.0;  lms(r,c++) = 4;  lms(r,c++) = 6;  lms(r,c++) = 7.0;  lms(r,c++)  = 4.0;  lms(r,c++) = 8.0;  lms(r,c++) = 8.0/*RUma O2I*/;  lms(r,c++) =3.0;lms(r,c++) = 4.0;r++;c = 0;
      
      //K-factor (K) [dB]
      lms(r,c++) = 9;  lms(r,c++) = -1;  lms(r,c++) =-1;  lms(r,c++) =9; lms(r,c++) = -1;  lms(r,c++) = -1;  lms(r,c++)  = 7;  lms(r,c++) = -1; lms(r,c++) = -1/*RUma O2I*/; lms(r,c++) = 7.0;  lms(r,c++) =-1; r++;c = 0;
      lms(r,c++) = 5;  lms(r,c++) = -1;  lms(r,c++) = -1;  lms(r,c++) = 3.5;  lms(r,c++) = -1;  lms(r,c++) = -1;  lms(r,c++)  = 4;  lms(r,c++) = -1;lms(r,c++) = -1/*RUma O2I*/;lms(r,c++) = 4.0;  lms(r,c++) = -1; r++;c = 0;
      
      //ZoA spread (lgZSA) lgZSA=log10(ZSA/1degree)  
      lms(r,c++) = 0.60;  lms(r,c++) = 0.88;  lms(r,c++) = 1.01;  lms(r,c++) = 0.95; lms(r,c++) = 1.26;  lms(r,c++) = 1.01;  lms(r,c++)  = 0.47;  lms(r,c++) = 0.58; lms(r,c++) = 0.93/*RUma O2I*/;lms(r,c++) =1.22;  lms(r,c++) = 1.26; r++;c = 0;
      lms(r,c++) =0.16;  lms(r,c++) =0.16;  lms(r,c++) = 0.43;  lms(r,c++) = 0.16;  lms(r,c++) = 0.16;  lms(r,c++) = 0.43;  lms(r,c++)  = 0.40;  lms(r,c++) = 0.37;lms(r,c++) = 0.22/*RUma O2I*/; lms(r,c++) = 0.23;  lms(r,c++) = 0.67; r++;c = 0;
    }
    else  // if(fc >6  && fc<=100 || channelModelType == _MODEL_B_) or 38.901 3GPP
    {
        
        lms(r,c++) = -0.24*lfc-7.14;      lms(r,c++) = -0.24*lfc-6.83;        lms(r,c++) = -6.62;         lms(r,c++) = -6.955-0.0963*log10(fc);       lms(r,c++) = -6.280-0.204*log10(fc);        lms(r,c++) = -6.62;     lms(r,c++) = -7.49;     lms(r,c++) = -7.43;     lms(r,c++) = -7.47/*RUma O2I*/;     lms(r,c++) = -0.01*lfc-7.692;       lms(r,c++) = -0.280*lfc-7.173; lms(r,c++) = log10(26*V_S_ratio+14)-9.35; lms(r,c++) = log10(30*V_S_ratio+32)-9.44;  r++;c = 0;
        lms(r,c++) = 0.38;                lms(r,c++) = 0.16*lfc+0.28;         lms(r,c++) = 0.32;          lms(r,c++) = 0.66;                          lms(r,c++) = 0.39;                          lms(r,c++) = 0.32;      lms(r,c++)  = 0.55;     lms(r,c++) = 0.48;      lms(r,c++) = 0.24/*RUma O2I*/;      lms(r,c++) = 0.18;                  lms(r,c++) = 0.10*lfc+0.055;lms(r,c++) =0.15; lms(r,c++) =0.19;     r++;c = 0;
      
      // AoD spread (gASD) gASD=log10(ASD/1degree)  
        lms(r,c++) = -0.05*lfc+1.21;      lms(r,c++) = -0.23*lfc+1.53;        lms(r,c++) = 1.25;          lms(r,c++) = 1.06+0.1114*log10(fc);         lms(r,c++) = 1.5-0.1144*log10(fc);          lms(r,c++) = 1.25;      lms(r,c++) = 0.90;      lms(r,c++) = 0.95;      lms(r,c++) = 0.67/*RUma O2I*/;      lms(r,c++) =1.60;                   lms(r,c++) = 1.62;lms(r,c++) =1.56; lms(r,c++) =1.57;               r++;c = 0;
        lms(r,c++) = 0.41;                lms(r,c++) = 0.11*lfc+0.33;         lms(r,c++) = 0.42;          lms(r,c++) = 0.28;                          lms(r,c++) = 0.28;                          lms(r,c++) = 0.42;      lms(r,c++)  = 0.38;     lms(r,c++) = 0.45;      lms(r,c++) = 0.18/*RUma O2I*/;      lms(r,c++) = 0.18;                  lms(r,c++) = 0.25;   lms(r,c++) =0.25; lms(r,c++) =0.2 ;           r++;c = 0;
      
      // AoA spread (lgASA) lgASA=log10(ASA/1degree)  
        lms(r,c++) = -0.08*lfc+1.73;      lms(r,c++) = -0.08*lfc+1.81;        lms(r,c++) = 1.76;          lms(r,c++) = 1.81;                          lms(r,c++) = 2.08-0.27*log10(fc);           lms(r,c++) = 1.76;      lms(r,c++)  = 1.52;     lms(r,c++) = 1.52;      lms(r,c++) = 1.66/*RUma O2I*/;      lms(r,c++) =-0.19*lfc+1.781;        lms(r,c++) = -0.11*lfc+1.863; lms(r,c++) =-0.18*lfc+1.78;  lms(r,c++) =1.72 ;  r++;c = 0;
        lms(r,c++) = 0.014*lfc+0.28;      lms(r,c++) = 0.05*lfc+0.3;          lms(r,c++) = 0.16;          lms(r,c++) = 0.20;                          lms(r,c++) = 0.11;                          lms(r,c++) = 0.16;      lms(r,c++)  = 0.24;     lms(r,c++) = 0.13;      lms(r,c++) = 0.21/*RUma O2I*/;      lms(r,c++) =0.12*lfc+0.119;         lms(r,c++) = 0.12*lfc+0.059;  lms(r,c++) = 0.12*lfc+0.2; lms(r,c++) =0.3;   r++;c = 0;
      
      // Shadow fading (SF) [dB]
        lms(r,c++) = -1;                  lms(r,c++) = -1;                    lms(r,c++) =-1;             lms(r,c++) =-1;                             lms(r,c++) = -1;                            lms(r,c++) = -1;        lms(r,c++)  = -1;       lms(r,c++) = -1;        lms(r,c++) = -1/*RUma O2I*/;        lms(r,c++) =-1;                     lms(r,c++) = -1;       lms(r,c++) =0.0; lms(r,c++) =0.0  ;        r++;c = 0;
        lms(r,c++) = 4.0;                 lms(r,c++) = 7.82;                  lms(r,c++) = 7.0;           lms(r,c++) = 4.0;                           lms(r,c++) = 6.0;                           lms(r,c++) = 7.0;       lms(r,c++)  = 4.0;      lms(r,c++) = 8.0;       lms(r,c++) = 8.0/*RUma O2I*/;       lms(r,c++) = 3.0;                   lms(r,c++) = 8.03;      lms(r,c++) =4.32; lms(r,c++) =getSigmaSFForInF(sysTerrain.infParams.scenario);         r++;c = 0;
      
      //K-factor (K) [dB]
        lms(r,c++) = 9;                   lms(r,c++) = -1;                    lms(r,c++) =-1;             lms(r,c++) = 9;                             lms(r,c++) = -1;                            lms(r,c++) = -1;        lms(r,c++)  = 7;        lms(r,c++) = -1;        lms(r,c++) = -1/*RUma O2I*/;        lms(r,c++) = 7.0;                   lms(r,c++) =-1;      lms(r,c++) =7.0; lms(r,c++) =-1;            r++;c = 0;
        lms(r,c++) = 5;                   lms(r,c++) = -1;                    lms(r,c++) = -1;            lms(r,c++) = 3.5;                           lms(r,c++) = -1;                            lms(r,c++) = -1;        lms(r,c++)  = 4;        lms(r,c++) = -1;        lms(r,c++) = -1/*RUma O2I*/;        lms(r,c++) = 4.0;                   lms(r,c++) = -1;     lms(r,c++) =8.0; lms(r,c++) =-1;            r++;c = 0;
      
      //ZoA spread (lgZSA) lgZSA=log10(ZSA/1degree)  
        lms(r,c++) = -0.1*lfc+0.73;       lms(r,c++) = -0.04*lfc+0.92;        lms(r,c++) = 1.01;          lms(r,c++) = 0.95;                          lms(r,c++) = -0.3236*log10(fc)+1.512;       lms(r,c++) = 1.01;      lms(r,c++)  = 0.47;     lms(r,c++) = 0.58;      lms(r,c++) = 0.93/*RUma O2I*/;      lms(r,c++) = -0.26*lfc+1.44;        lms(r,c++) = -0.15*lfc+1.387; lms(r,c++) =-0.2*lfc+1.5; lms(r,c++) =-0.13*lfc+1.45;   r++;c = 0;
        lms(r,c++) = -0.04*lfc+0.34;      lms(r,c++) = -0.07*lfc+0.41;        lms(r,c++) = 0.43;          lms(r,c++) = 0.16;                          lms(r,c++) = 0.16;                          lms(r,c++) = 0.43;      lms(r,c++)  = 0.40;     lms(r,c++) = 0.37;      lms(r,c++) = 0.22/*RUma O2I*/;      lms(r,c++) = -0.04*lfc+0.264;       lms(r,c++) = -0.09*lfc+0.746;  lms(r,c++) =0.35; lms(r,c++) =0.45;  r++;c = 0;
      
    }
   
  return lms;
}

mat getCorrelationDistanceForCalibration(double carrierFreqInGHz)
{
    mat corrDist(7,13);
    corrDist =  "7 10 10 30 40 10 50 36 36 8 5 50 52;" //DS
                "8 10 11 18 50 11 25 30 30 7 3 10 13;"  //ASD
                "8  9 17 15 50 17 35 40 40 5 3 10 13;"  //ASA
                "10 13 7 37 50 7 37 120 120 10 6 15 30;" //SF
                "15 -1 -1 12 -1 -1 40 -1 -1 4 -1 32 -1;"    //K
                "12 10 25 15 50 25 15 50 50 3 3 10 20;" //ZSA
                "12 10 25 15 50 25 15 50 50 3 3 10 20"; //ZSD
    corrDist(5,0) = -4.95*log10(1+carrierFreqInGHz) + 12.65;
    corrDist(6,0) = -3.76*log10(1+carrierFreqInGHz) + 11.92;
    return corrDist;
}

mat  McellSystem::correlationDistCal(double carrierFreqInGHz)
{
    #ifdef _USING_CALIBRATION_
    return getCorrelationDistanceForCalibration(carrierFreqInGHz);
    #endif
  //double fc =carrierFreqInGHz;
  mat corrDist(7,13);
  //int rowIndx = 0,colIndx = 0;
  corrDist = "7	10 10 30 40 10 50 36 36 8 5 10 10;" //DS
  "8 10 11 18 50 11 25 30 30 7 3 10 10;"  //ASD
  "8  9 17 15 50 17 35 40 40 5 3 10 10;"  //ASA
  "10 13 7 37 50 7 37 120 120 10 6 10 10;" //SF
  "15 -1 -1 12 -1 -1 40 -1 -1 4 -1 10 -1;"    //K
  "12 10 25 15 50 25 15 50 50 4 4 10 10;" //ZSA
  "12 10 25 15 50 25 15 50 50 4 4 10 10"; //ZSD
  return corrDist;
}

mat  McellSystem::dcorrForSPConsistencyCal()
{
  int dClutter=sysTerrain.infParams.clutterSize; // Allowed values : 10,2  
    
  mat corrDist(3,10);
  //int rowIndx = 0,colIndx = 0;
  // TR38.901 Table 7.6.3.1-2 correlation dist for spatial Consistency
  if(dClutter==10)
  {
    corrDist =  "12 15 15 40 50 15 50 60 15 10 -1 10 -1;"  // Cluster delay/shadowing
                "50 -1 -1 50 -1 -1 60 -1 -1 10 -1 5 -1;"  // LOS/NLOS
                "50 -1 -1 50 -1 -1 50 -1 -1 -1 -1 -1";  // Indoor/outdoor
  }
  else
  {
    corrDist =  "12 15 15 40 50 15 50 60 15 10 -1 10 -1;"  // Cluster delay/shadowing
                "50 -1 -1 50 -1 -1 60 -1 -1 10 -1 10 -1;"  // LOS/NLOS
                "50 -1 -1 50 -1 -1 50 -1 -1 -1 -1 -1";  // Indoor/outdoor
  }
  return corrDist;
}
void McellSystem::initCorrelatedShdw(ivec shdwNodeTypes,double corrDistInMeters, double siteToSiteCorr){

#ifdef USING_MPI
if(currentTaskid==0) {
#endif  
  cout<<"[detl:]Initializing Correlated shadowing with correlation distance "<<corrDistInMeters<<"m and site-To-site Correlation of "<<siteToSiteCorr<<endl;
#ifdef USING_MPI
}
#endif  
  shadowType=_SHADOWTYPE_CORRELATED_;
  corrShdw.corrDist=corrDistInMeters;
  corrShdw.siteToSiteCorr=siteToSiteCorr;
  corrShdw.lGridY = (int) ceil(coverageArea.xLen/corrShdw.corrDist)+1;
  corrShdw.lGridX = (int) ceil(coverageArea.yLen/corrShdw.corrDist)+1;
  corrShdw.commnShdw=randn(corrShdw.lGridX,corrShdw.lGridY)*sqrt(corrShdw.siteToSiteCorr);

  ivec tempNodes;
  for(int type_cnt=0;type_cnt<length(shdwNodeTypes);type_cnt++)
  {
	tempNodes=getNodes(shdwNodeTypes(type_cnt));
	for(int node_cnt=0;node_cnt<length(tempNodes);node_cnt++)
	  aNodes(tempNodes(node_cnt)).initCorrelatedShdw(corrShdw);
  }
}

/*! \fn nameNodeType() 
 * \brief -->  This function add  the nodetypes  to the existing nodetypes present in the system or overwriting 
 *\param[in] type--> it gives number of types of nodes present in a system
 *\param[in] nodeType--> it gives the nodetype(BS,US  etc) present in the system
 */
void McellSystem::nameNodeType(int type, string nodeType){

  if(type<nNodes.length())
  {
	if(type<aNodeTypes.length()) {cout<<"[detl:]Warning. NodeType name being overwritten.;"; aNodeTypes(type)=nodeType;}
	else if(type==aNodeTypes.length())
	  append(aNodeTypes,nodeType);
	else
	{
	  cout<<"[both:]Un-named Node Types available before the given node type in nameNodeType().";
	  abort();
	}
  }
  else
	{cout<<"[both:]Type count exceeding index.";abort();}
	
}

void McellSystem::nameNodeType(Array<string> nodeTypes){

  if(nodeTypes.length()==nNodes.length())
	for(int type_cnt=0;type_cnt<nodeTypes.length();type_cnt++)
	  nameNodeType(type_cnt, nodeTypes(type_cnt));
  else
	{cout<<"[both:]Node names exceeding node Type available in nameNodeType().";abort();}

}

void McellSystem::setCoverageArea(double lenX ,double lenY){     

  coverageArea.xLen=lenX;         
  coverageArea.yLen=lenY;
  cellLayout=_CELLLAYOUT_RANDOM_;

}

void McellSystem::overwriteHexagonalCoverageArea(double lenX ,double lenY){

  coverageArea.xLen=lenX;
  coverageArea.yLen=lenY;

}

void McellSystem::setInterSiteDistanceInMeters(double isd){       

  cellISD=isd;
  cellLayout=_CELLLAYOUT_HEXAGONAL_;
  updateHexagonalCoverageArea();
  sysTerrain.setCellISD(cellISD);
}

void McellSystem::setMinDistanceFromCellCenter(double minDistance){   

  minDist=minDistance;
}

void McellSystem::setCommonVerticalTilt(int commonVerticalTilt){

  hexDropCommonVerticalTilt=commonVerticalTilt;
}

void McellSystem::setTerrain(string systemTerrain){

  sysTerrain.setTerrain(systemTerrain);
}

void McellSystem::setCellLayout(CellLayout_E cellLayt){

  cellLayout=cellLayt;
}

void McellSystem::setShadowType(ShadowType_E shadwType){

  shadowType=shadwType;
}

// void McellSystem::setCarrierFreqInHz(double carrierFreqinHz){
// 
//   carrierFreq=carrierFreqinHz;
// }

// void McellSystem::setCarrierFreqInHzPerServerNodeType(vec carrierFreqinHz){  
//   
//   if(carrierFreqinHz.length()!=getServerNodeTypes().length())
//     cout<<"[both:]Error. Insufficient length for carrierFreqinHz in setCarrierFreqInHzPerServerNodeType()."<<endl;
//   carrierFreqPerServerNodeType=carrierFreqinHz;
// }

void McellSystem::setNodeCount(string nodeType, int nodeCnt){

  int nodeTypeIndx=findInStringArray(aNodeTypes,nodeType);
  if(nodeTypeIndx!=-1) nNodes(nodeTypeIndx)=nodeCnt;
  else {cout<<"[both:]Undefined Node Name to setNodeCount().";abort();}
}

void McellSystem::setNodeCount(ivec nodeCnts){

  nNodes=nodeCnts;
}

MobilityInfo_S McellSystem::getMobilityInfo(){          

  return(mobilityInfo);
}

Area_S McellSystem::getCoverageArea(){

  return(coverageArea);
}

double McellSystem::getMinDistanceFromCellCenter(){

  return(minDist);
}

string McellSystem::getTerrain(){

  return(sysTerrain.getTerrain());
}

CellLayout_E McellSystem::getCellLayout(){

  return(cellLayout);
}

ShadowType_E McellSystem::getShadowType(){

  return(shadowType);
}

// double McellSystem::getCarrierFreqInHz(){
// 
//   return(carrierFreq);
// }

double McellSystem::getCarrierFreqInHz(){

  return(carrierFreqInMHzPerServerNodeType(0)*1e6);
}

int McellSystem::getCommonVerticalTilt(){

  return(hexDropCommonVerticalTilt);
}

ivec McellSystem::getNodeCount(){

  return(nNodes);
}

int McellSystem::getNodeCount(string nodeType){

  return(nNodes(getNodeTypeCnt(nodeType)));
}

int McellSystem::getNodeCount(int nodeTypeCnt){
  
  return(nNodes(nodeTypeCnt));
}

Location_S McellSystem::getNodeLocation(int nodeID){

  return(aNodes(nodeID).getLoc());
}

Array<Location_S> McellSystem::getNodeLocation(ivec nodeIDs){

  Array<Location_S> toReturn(nodeIDs.length());
  for(int node_cnt=0;node_cnt<nodeIDs.length();node_cnt++)
    toReturn(node_cnt)=aNodes(nodeIDs(node_cnt)).getLoc();
  return(toReturn);
}

Array<string> McellSystem::getNodeTypes(){

  return(aNodeTypes);
}

ivec McellSystem::getServerNodeTypes(){  
  
  ivec serverNodeTypes(0);
  for(int nodeType_cnt=0;nodeType_cnt<nodeCategoryPerNodeType.length();nodeType_cnt++)
    if(nodeCategoryPerNodeType(nodeType_cnt)==_NODECATEGORY_SERVERNODE_)
      append(serverNodeTypes,nodeType_cnt);

  return(serverNodeTypes);
}

ivec McellSystem::getServiceNodeTypes(){

  ivec serviceNodeTypes(0);
  for(int nodeType_cnt=0;nodeType_cnt<nodeCategoryPerNodeType.length();nodeType_cnt++)
    if(nodeCategoryPerNodeType(nodeType_cnt)==_NODECATEGORY_SERVICENODE_)
      append(serviceNodeTypes,nodeType_cnt);

  return(serviceNodeTypes);
}

string McellSystem::getHexagonalDropNodetype(){

  if(find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_)!=-1)
	return(aNodeTypes(find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_)));
  else
  {
      if(currentTaskid==0)  
        cout<<"[both:]No nodeType follows Hexagonal Drop method."<<endl;
	return("");
  }
}

string McellSystem::getInHDropNodetype(){
  
  if(find(nodeDropMethods,_DROPMETHOD_INBUILDING_)!=-1)
    return(aNodeTypes(find(nodeDropMethods,_DROPMETHOD_INBUILDING_)));
  else
  {
    if(currentTaskid==0)  
        cout<<"[both:]No nodeType follows _DROPMETHOD_INBUILDING_ Drop method."<<endl;
    return("");
  }
}

DropMethod_E McellSystem::getNodeDropMethod(string nodeType){

  return(DropMethod_E(nodeDropMethods(getNodeTypeCnt(nodeType))));
}

/*! \fn printConfig( ) 
 * \brief -->  This function prints the current system configuration into the given file
 *\param[in] filename --> the file name in which configurations are to be present .
 */

void McellSystem::printConfig(string filename){               

  std::ofstream fout(filename.c_str(), std::ios::out);
  vec cvrgArea(2);cvrgArea(0)=coverageArea.xLen;cvrgArea(1)=coverageArea.yLen;
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_)                      
  {                                                           
	fout<<"cellLayout="<<CellLayout_Str[cellLayout]<<endl;
	fout<<"cellISD="<<cellISD<<endl;

  }
  else
	fout<<"coverageArea = "<< cvrgArea << endl;            
  fout<<"sysTerrain="<<sysTerrain.getTerrain()<<endl;
  //fout<<"carrierFreq="<<carrierFreq<<endl;
  fout<<"carrierFreqInMHzPerServerNodeType="<<carrierFreqInMHzPerServerNodeType<<endl;
  fout<<"minDist="<<minDist<<endl;
  fout<<"shdwType="<<ShadowType_Str[shadowType]<<endl;
  fout<<"nodeCounts="<<nNodes<<endl;
  fout<<"nodeTypeNames=\"{";
  for(int string_cnt=0;string_cnt<aNodeTypes.length();string_cnt++)
	fout<<aNodeTypes(string_cnt)<<" ";
  fout<<"\""<<endl;
  fout<<"nodeDropMethods=\"{";
  for(int string_cnt=0;string_cnt<nodeDropMethods.length();string_cnt++)
	fout<<DropMethod_Str[nodeDropMethods(string_cnt)]<<" ";
  fout<<"\""<<endl;
}
/*! \fn printConfig( ) 
 * \brief -->  This function prints the current system configuration on the screen
 */

void McellSystem::printConfig(){    

  vec cvrgArea(2);cvrgArea(0)=coverageArea.xLen;cvrgArea(1)=coverageArea.yLen;
  cout<<"[detl:]coverageArea = "<< cvrgArea << endl;
  cout<<"[detl:]cellLayout="<<CellLayout_Str[cellLayout]<<endl;
  cout<<"[detl:]sysTerrain="<<sysTerrain.getTerrain()<<endl;
  cout<<"[detl:]cellISD="<<cellISD<<endl;
  //cout<<"[detl:]carrierFreq="<<carrierFreq<<endl;
  cout<<"carrierFreqInMHzPerServerNodeType="<<carrierFreqInMHzPerServerNodeType<<endl;
  cout<<"[detl:]minDist="<<minDist<<endl;
  cout<<"[detl:]shdwType="<<ShadowType_Str[shadowType]<<endl;
  cout<<"[detl:]nodeCounts="<<nNodes<<endl;
  cout<<"[detl:]nodeTypeNames="<<aNodeTypes<<endl;
  cout<<"[detl:]nodeDropMethods=\"{";
  for(int string_cnt=0;string_cnt<nodeDropMethods.length();string_cnt++)
	cout<<DropMethod_Str[nodeDropMethods(string_cnt)]<<" ";
  cout<<"\""<<endl;
}

bool McellSystem::isValidLink(int txNodeID, int rxNodeID){
  
  if(aNodes(txNodeID).getCarrierFreqInHz()==aNodes(rxNodeID).getCarrierFreqInHz())
    return(true);
  else
    return(false);
}

void McellSystem::enableCrossLinks(){  
  
  isCrossLinksEnabled=true;
}

void McellSystem::disableCrossLinks(){
  
  isCrossLinksEnabled=false;
}

void McellSystem::setNodeHeightByIndex(ivec nodeList,double nodeHeight){

  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
	aNodes(nodeList(node_cnt)).setHeight(nodeHeight,true);
}

void McellSystem::setNodeHeight(string nodeType,double nodeHeight){

  ivec nodeList=getNodes(nodeType);
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
	aNodes(nodeList(node_cnt)).setHeight(nodeHeight,true);
}

void McellSystem::setNodeHeight(int nodeTypeCnt, double nodeHeight){

  ivec nodeList=getNodes(nodeTypeCnt);
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
	aNodes(nodeList(node_cnt)).setHeight(nodeHeight,true);
}

void McellSystem::setNodeHeightByIndex(ivec nodeList,vec heightList){

  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
	aNodes(nodeList(node_cnt)).setHeight(heightList(node_cnt),true);
}

void McellSystem::setNodeAntennaCount(string nodeType,int antCnt){

  ivec nodeList=getNodes(nodeType);
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
  {
    aNodes(nodeList(node_cnt)).setNumOfAntenna(antCnt);
    aNodes(nodeList(node_cnt)).txAntenna.setNumberOfPhysicalAntenna(antCnt);
    aNodes(nodeList(node_cnt)).rxAntenna.setNumberOfPhysicalAntenna(antCnt);
  }
}

void McellSystem::setNodeAntennaCount(int nodeTypeCnt, int antCnt){

  ivec nodeList=getNodes(nodeTypeCnt);
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
  {
    aNodes(nodeList(node_cnt)).setNumOfAntenna(antCnt);
    aNodes(nodeList(node_cnt)).txAntenna.setNumberOfPhysicalAntenna(antCnt);
    aNodes(nodeList(node_cnt)).rxAntenna.setNumberOfPhysicalAntenna(antCnt);
  }
}

void McellSystem::setNodeCarrierFrequency(string nodeType,double carrierFreq){
  
  ivec nodeList=getNodes(nodeType);
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
    aNodes(nodeList(node_cnt)).setCarrierFreqInHz(carrierFreq);
}

void McellSystem::setNodeCarrierFrequencyInHz(int nodeTypeCnt,double carrierFreq){
  
  ivec nodeList=getNodes(nodeTypeCnt);
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
    aNodes(nodeList(node_cnt)).setCarrierFreqInHz(carrierFreq);
}

void McellSystem::setNodeCarrierFrequencyByIndex(ivec nodeList,double carrierFreq){
  
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
  {
    aNodes(nodeList(node_cnt)).setCarrierFreqInHz(carrierFreq);
  }
}

void McellSystem::setNodeCarrierFrequencyForAllNodes(double carrierFreqInHz, bool& regenLinkInfo){
  
  regenLinkInfo = false;
  for(int node_cnt = 0; node_cnt<aNodes.length(); node_cnt++)
  {
    if(aNodes(node_cnt).getCarrierFreqInHz() != carrierFreqInHz)
    {
      aNodes(node_cnt).setCarrierFreqInHz(carrierFreqInHz);
      regenLinkInfo = true;
    }
  }
}

void McellSystem::setNodeNoiseFigure(string nodeType, double noiseFigureindB){

  int nodeTypeCnt=findInStringArray(aNodeTypes,nodeType);
  setNodeNoiseFigure(nodeTypeCnt,noiseFigureindB);

}

void McellSystem::setNodeNoiseFigure(int nodeTypeCnt, double noiseFigureindB){

  if(noiseFigurePerNodeType.length()==0)
    noiseFigurePerNodeType=zeros(nNodes.length())-1;
  noiseFigurePerNodeType(nodeTypeCnt)=noiseFigureindB;
}


void McellSystem::setNodeAntennaCountByIndex(ivec nodeList,int antCnt){

    for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
    {
      aNodes(nodeList(node_cnt)).setNumOfAntenna(antCnt);
      aNodes(nodeList(node_cnt)).txAntenna.setNumberOfPhysicalAntenna(antCnt);
      aNodes(nodeList(node_cnt)).rxAntenna.setNumberOfPhysicalAntenna(antCnt);
    }
}

void McellSystem::setNodeAntennaSpacing(string nodeType, double wavelengthSpacing){

  ivec nodeList=getNodes(nodeType);
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
  {
    aNodes(nodeList(node_cnt)).txAntenna.setAntennaWavelengthSpacing(wavelengthSpacing);
    aNodes(nodeList(node_cnt)).rxAntenna.setAntennaWavelengthSpacing(wavelengthSpacing);
  }
}

void McellSystem::setNodeAntennaSpacing(int nodeTypeCnt, double wavelengthSpacing){

  ivec nodeList=getNodes(nodeTypeCnt);
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
  {
    aNodes(nodeList(node_cnt)).txAntenna.setAntennaWavelengthSpacing(wavelengthSpacing);
    aNodes(nodeList(node_cnt)).rxAntenna.setAntennaWavelengthSpacing(wavelengthSpacing);
  }
}

void McellSystem::setNodeAntennaSpacingByIndex(ivec nodeList,  double wavelengthSpacing){

  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
  {
    aNodes(nodeList(node_cnt)).txAntenna.setAntennaWavelengthSpacing(wavelengthSpacing);
    aNodes(nodeList(node_cnt)).rxAntenna.setAntennaWavelengthSpacing(wavelengthSpacing);
  }
}


void McellSystem::setNodeIDCell(string nodeType, ivec IDCells){

  ivec nodeList=getNodes(nodeType);
  setNodeIDCellByIndex(nodeList,IDCells);
}

void McellSystem::setNodeIDCell(int nodeTypeCnt, ivec IDCells){

  ivec nodeList=getNodes(nodeTypeCnt);
  setNodeIDCellByIndex(nodeList,IDCells);
}

void McellSystem::setNodeVelocity(string nodeType, vec velocityInMetrePerSec){

  ivec nodeList=getNodes(nodeType);
  setNodeVelocityByIndex(nodeList,velocityInMetrePerSec);
}

void McellSystem::setNodeVelocity(int nodeTypeCnt, vec velocityInMetrePerSec){

  ivec nodeList=getNodes(nodeTypeCnt);
  setNodeVelocityByIndex(nodeList,velocityInMetrePerSec);
}

void McellSystem::setNodeCentralUnitID(string nodeType, ivec centralUnitIDs){

  ivec nodeList=getNodes(nodeType);
  setNodeCentralUnitIDByIndex(nodeList,centralUnitIDs);
}

void McellSystem::setNodeCentralUnitID(int nodeTypeCnt, ivec centralUnitIDs){

  ivec nodeList=getNodes(nodeTypeCnt);
  setNodeCentralUnitIDByIndex(nodeList,centralUnitIDs);
}

void McellSystem::setNodeCentralUnitIDByIndex(ivec nodeList, ivec centralUnitIDs){

  if(nodeList.length()!=centralUnitIDs.length())
  {
    cout<<"[both:]Length mismatch in setNodeIDCellByIndex()."<<endl;
    abort();
  }
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
    aNodes(nodeList(node_cnt)).setCentralUnitID(centralUnitIDs(node_cnt));
}

void McellSystem::setNodeNumerologies(string nodeType, ivec nus){
  
  ivec nodeList=getNodes(nodeType);
  setNodeNumerologiesByIndex(nodeList,nus);
}

void McellSystem::setNodeNumerologies(int nodeTypeCnt, ivec nus){
  
  ivec nodeList=getNodes(nodeTypeCnt);
  setNodeNumerologiesByIndex(nodeList,nus);
}

void McellSystem::setNodeNumerologiesByIndex(ivec nodeList, ivec nus){

  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
    aNodes(nodeList(node_cnt)).setNumerologies(nus);
}

void McellSystem::setNodeIDCellByIndex(ivec nodeList, ivec IDCells){

  if(nodeList.length()!=IDCells.length())
  {
	cout<<"[both:]Length mismatch in setNodeIDCellByIndex()."<<endl;
	abort();
  }
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
	aNodes(nodeList(node_cnt)).setIDCell(IDCells(node_cnt));
}

void McellSystem::setNodeVelocityByIndex(ivec nodeList, vec velocityInMetrePerSec){

  if(nodeList.length()!=velocityInMetrePerSec.length())
  {
    cout<<"[both:]Length mismatch in setNodeVelocityByIndex()."<<endl;
    abort();
  }

  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
    aNodes(nodeList(node_cnt)).setVelocity(velocityInMetrePerSec(node_cnt));
}


void McellSystem::setNodeHeight(string nodeType,vec heightList){

  ivec nodeList=getNodes(nodeType);
  if(length(nodeList)>length(heightList)) {cout<<"[both:]Error. Insufficient heights provided for setNodeHeight().";abort();}
  if(length(nodeList)<length(heightList)) {cout<<"[detl:]Warning. Excess heights provided for setNodeHeight(). Assigning only upto the required count."<<endl;}
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
	aNodes(nodeList(node_cnt)).setHeight(heightList(node_cnt),true);
}

void McellSystem::setNodeHeightByIndex(ivec nodeList,double hMin,double hMax){

  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
	aNodes(nodeList(node_cnt)).setHeight((randu()*(hMax-hMin))+hMin,true);
}

void McellSystem::setNodeHeight(string nodeType,double hMin,double hMax){

  ivec nodeList=getNodes(nodeType);
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
	aNodes(nodeList(node_cnt)).setHeight((randu()*(hMax-hMin))+hMin,true);
}

void McellSystem::setNodeCategory(string nodeType,NodeCategory_E category){

  ivec nodeList=getNodes(nodeType);
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
    aNodes(nodeList(node_cnt)).setCategory(category);

  nodeCategoryPerNodeType(findInStringArray(aNodeTypes,nodeType))=category;
}

void McellSystem::setNodeCategory(int nodeTypeCnt,NodeCategory_E category){

  ivec nodeList=getNodes(nodeTypeCnt);
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
    aNodes(nodeList(node_cnt)).setCategory(category);

  nodeCategoryPerNodeType(nodeTypeCnt)=category;

}

void McellSystem::setNodeTypeAsIndoor(string nodeType, double percentage){
  ivec allNodes=getNodes(nodeType);
  if(!buildingInfo.isBuildingsAvailable)
    setNodeListAsIndoor(allNodes,percentage, false);
  else
    setNodeListAsIndoor(allNodes,percentage, true);
}

void McellSystem::setNodeTypeAsOutdoor(string nodeType, double percentage){

  ivec allNodes=getNodes(nodeType);
  setNodeListAsIndoor(allNodes,100.0-percentage,true);
}

void McellSystem::setNodeTypeAsPedestrian(string nodeType, double percentage){
  
  ivec allNodes=getNodes(nodeType);
  setNodeListAsPedestrian(allNodes,percentage);
}

void McellSystem::setNodeListAsPedestrian(ivec nodeList, double percentage)
{
    int nPedestrianUsers = round_i(double(percentage/100.0)*nodeList.length());
    ivec pedestrianUserIndices = (nPedestrianUsers<=0) ? zeros_i(0) : randPick(getIntegers(0,nodeList.length()-1),nPedestrianUsers); 
    sort(pedestrianUserIndices);
    
    for(int node_cnt=0;node_cnt<pedestrianUserIndices.length();node_cnt++)
    {
        if(aNodes(nodeList(pedestrianUserIndices(node_cnt))).isIndoor())
        {
            cout<<"[detl:]Warning. Node "<<nodeList(pedestrianUserIndices(node_cnt))<<" is already indoor."<<endl;
            continue;
        }
        aNodes(nodeList(pedestrianUserIndices(node_cnt))).setAsPedestrian();
    }
    if(currentTaskid==0)
    {
        cout<<"[detl:]Outdoor Pedestrian Nodes : "<<nodeList(pedestrianUserIndices)<<endl;
        cout<<"[detl:]Outdoor In-Car Nodes: "<<nodeList(findNot(getIntegers(0,nodeList.length()-1),pedestrianUserIndices))<<endl;
    }
}

double getIndoorDistance(SCMPropagationScenario scmScenario,ChannelModelType channelType)
{
    double indoorDistance = 0.0;
    if(scmScenario == _SCM5G_URBAN_MACRO_ || scmScenario == _SCM5G_URBAN_MICRO_)
    {
        indoorDistance = (channelType == _MODEL_A_) ? randu()*25.0 : min(randu()*25.0,randu()*25.0);
    }
    else if(scmScenario == _SCM5G_RURAL_MACRO_)
    {
        indoorDistance = (channelType == _MODEL_A_) ? randu()*10.0 : min(randu()*10.0,randu()*10.0);
    }
    return indoorDistance;
}

/*!\fn setNodeListAsIndoor()
 * \brief --> This function will gives the list of nodes to be indoor
 * \brief --> Node height should be less than the room size height
 *\param[in] nodelist --> it gives the list of total nodes present in the system
 *\param[in] addFloorHeight --> it suggests whether floor height to be added to the node height
 */
void McellSystem::setNodeListAsIndoor(ivec nodeList, double percentage, bool addFloorHeight){
  
  if(!sysTerrain.isReadyForIndoor())
  {
	cout<<"[both:]Terrain not fully configured to support Indoor Nodes."<<endl;
	abort();
  }
  ivec indoorUserIndices(0),outdoorUserindices(0);
  for(int indx = 0; indx < nodeList.length(); indx++)
  {
    if(tossACoin(percentage/100.0))
      append(indoorUserIndices,indx);
    else
      append(outdoorUserindices,indx);
  }

//   ivec indoorUserIndices=randPick(getIntegers(0,nodeList.length()-1),round_i((percentage/100.0)*nodeList.length()));
//   ivec outdoorUserindices=findNot(getIntegers(0,nodeList.length()-1),indoorUserIndices);
  int nFloor;
//   if(currentTaskid==0)
//     cout<<"[detl:]Indoor Nodes : "<<nodeList(indoorUserIndices)<<endl;
  for(int node_cnt=0;node_cnt<indoorUserIndices.length();node_cnt++)
  {
	if(aNodes(nodeList(indoorUserIndices(node_cnt))).isIndoor())
	{
	  cout<<"[detl:]Warning. Node "<<nodeList(indoorUserIndices(node_cnt))<<" is already indoor."<<endl;
	  continue;
	}
	if(addFloorHeight)
	{
	  if(sysTerrain.scmScenario>3 && sysTerrain.scmScenario<10)//Refer 36.873 Table 6-1
	  {
// 	     if(sysTerrain.getMaximumFloors()!=8) { cout<<"Error...Maximum Floors for 3D UMa & 3D UMi is 8, please check buildingInfo.txt in configFiles."<<endl;abort();}
	     if(sysTerrain.getRoomSize().height!=3) { cout<<"Error...Maximum Room Size for 3D UMa & 3D UMi is 3, please check buildingInfo.txt in configFiles."<<endl;abort();}
	     int N_fl = randi(4,sysTerrain.getMaximumFloors());
	     nFloor=randi(0,N_fl);
	  }
	  else
	    nFloor=randi(0,sysTerrain.getMaximumFloors());
	 	  
	  if(aNodes(nodeList(indoorUserIndices(node_cnt))).getHeight()>sysTerrain.getRoomSize().height)
	  {cout<<"[both:]Error. Node "<< nodeList(indoorUserIndices(node_cnt)) <<"'s Height is greater than the room height."<<endl;abort();}
	  
	  double nodeHeight = rem(aNodes(nodeList(indoorUserIndices(node_cnt))).getHeight(),sysTerrain.getRoomSize().height)+(nFloor-1)*sysTerrain.getRoomSize().height;
	  aNodes(nodeList(indoorUserIndices(node_cnt))).setHeight(nodeHeight,true);
      aNodes(nodeList(indoorUserIndices(node_cnt))).setFloorIndex(nFloor,true);
	}
	else if(sysTerrain.scmScenario == _SCM3D_URBAN_MACRO_ || sysTerrain.scmScenario == _SCM3D_URBAN_MICRO_ || sysTerrain.scmScenario == _SCM5G_URBAN_MACRO_ || sysTerrain.scmScenario == _SCM5G_URBAN_MICRO_ || sysTerrain.scmScenario == _SCM5G_RURAL_MACRO_)//Refer 36.873 Table 6-1
	{
// 	  if(sysTerrain.getMaximumFloors()!=8) { cout<<"Error...Maximum Floors for 3D UMa & 3D UMi is 8, please check buildingInfo.txt in configFiles."<<endl;abort();}
	  if(sysTerrain.getRoomSize().height!=3) { cout<<"Error...Maximum Room Size for 3D UMa & 3D UMi is 3, please check buildingInfo.txt in configFiles."<<endl;abort();}
	  int N_fl = randi(4,sysTerrain.getMaximumFloors());
      nFloor=randi(1,N_fl);
	  double nodeHeight = rem(aNodes(nodeList(indoorUserIndices(node_cnt))).getHeight(),sysTerrain.getRoomSize().height)+(nFloor-1)*sysTerrain.getRoomSize().height;
	  aNodes(nodeList(indoorUserIndices(node_cnt))).setHeight(nodeHeight,true);
      aNodes(nodeList(indoorUserIndices(node_cnt))).setFloorIndex(nFloor-1,true);
	}
	aNodes(nodeList(indoorUserIndices(node_cnt))).setAsIndoor();
    aNodes(nodeList(indoorUserIndices(node_cnt))).setIndoorDistance(getIndoorDistance(sysTerrain.scmScenario,sysTerrain.channelModelType));
  }
  if(OtoIPntrModlL_H_Percentage(0) > 0 && (cellLayout == _CELLLAYOUT_HEXAGONAL_ || isNTNSCMScenario(sysTerrain.scmScenario)))
  { 
      int nLowLossUsers = round_i((double(OtoIPntrModlL_H_Percentage(0))/100.0)*indoorUserIndices.length());
      ivec nodeWithOtoIPntrModlLowIndices = (nLowLossUsers<=0) ? zeros_i(0) : randPick(indoorUserIndices,nLowLossUsers);
      sort(nodeWithOtoIPntrModlLowIndices);
      for(int node_cnt=0;node_cnt<nLowLossUsers;node_cnt++)
      {  	
          aNodes(nodeList(nodeWithOtoIPntrModlLowIndices(node_cnt))).setWithLowPenetrationLoss();
      }
      if(currentTaskid==0 && nLowLossUsers>0)
      {
          cout<<"[detl:]Low Loss Indoor Nodes: "<<nodeList(nodeWithOtoIPntrModlLowIndices)<<endl;
          cout<<"[detl:]High Loss Indoor Nodes: "<<nodeList(findNot(indoorUserIndices,nodeWithOtoIPntrModlLowIndices))<<endl;
      }
  }
  if(currentTaskid==0)
  {
        cout<<"[detl:]Outdoor Nodes: "<<nodeList(outdoorUserindices)<<endl;
  }
  for(int node_cnt=0;node_cnt<length(outdoorUserindices);node_cnt++)
	aNodes(nodeList(outdoorUserindices(node_cnt))).setAsOutdoor();
}

void McellSystem::computePenetrationLossesForAllServiceNodes()
{
    double fc = aNodes(getNodes(_NODECATEGORY_SERVERNODE_)(0)).getCarrierFreqInHz()/1e9;
    ivec serviceNodes = getNodes(_NODECATEGORY_SERVICENODE_);
    for(int node_cnt=0;node_cnt<serviceNodes.length();node_cnt++)
    {
        aNodes(serviceNodes(node_cnt)).computePenetrationLoss(fc,sysTerrain.channelModelType,sysTerrain.scmScenario);
    }
}

void McellSystem::setNodeDropMethod(string nodeType,DropMethod_E dropMethod){

  if(nodeDropMethods.length()!=aNodeTypes.length())
	nodeDropMethods=zeros_i(aNodeTypes.length());

  int nodeTypeIndx=findInStringArray(aNodeTypes,nodeType);
  if(nodeTypeIndx!=-1) nodeDropMethods(nodeTypeIndx)=dropMethod;
  else {cout<<"[both:]Undefined Node Name to setNodeDropMethod().";abort();}
}

void McellSystem::enablePathSpecificAntennaGainInSCM(){

  addPathSpecificSCMAntennaGain=true;
}

void McellSystem::disablePathSpecificAntennaGainInSCM(){
  
  addPathSpecificSCMAntennaGain=false;
}

bool McellSystem::isPathSpecificAntennaGainInSCMEnabled(){

  return(addPathSpecificSCMAntennaGain);
}

void McellSystem::initMobilityInfo(string configFile){

    string mobilityType;
    if(!parse(configFile,"mobilityType",mobilityType))
    {
        cout<<"[detl:]Loading _MOBILITY_TYPE_RND_DIR_ as default value for mobilityType."<<endl;
        mobilityInfo.mobilityType=_MOBILITY_TYPE_RND_DIR_;
    }
    else
    {
        if(mobilityType=="_MOBILITY_TYPE_RND_DIR_") mobilityInfo.mobilityType=_MOBILITY_TYPE_RND_DIR_;
        else if(mobilityType=="_MOBILITY_TYPE_RND_WALK_") mobilityInfo.mobilityType=_MOBILITY_TYPE_RND_WALK_;
        else
        {cout<<"[both:]Unknown mobilityType in config file."<<endl;abort();}
    }      
    
    parse(configFile,"refreshDistanceForRandomWalk",mobilityInfo.refreshDistanceForRandomWalk);
    
    mobilityInfo.serviceNodes=getNodes(_NODECATEGORY_SERVICENODE_);
    mobilityInfo.pastTimePerServiceNode=zeros(mobilityInfo.serviceNodes.length());
//     mobilityInfo.directionPerServiceNode=90;
    mobilityInfo.directionPerServiceNode=randu(mobilityInfo.serviceNodes.length())*360;
    if(mobilityInfo.mobilityType==_MOBILITY_TYPE_RND_WALK_)
    {
        mobilityInfo.distanceMovedPerServiceNode=zeros(mobilityInfo.serviceNodes.length());
    }
    else
    {
        cout<<"[both:]Unknown mobilityType to setMobilityInfo()."<<endl;abort();
    }
}
void McellSystem::reinitMobilityDirection(int nodeID){

  if(nodeID!=-1)
  {
    int indx=find(mobilityInfo.serviceNodes,nodeID);
    if(indx!=-1)
      mobilityInfo.directionPerServiceNode(indx)=mobilityInfo.directionPerServiceNode(indx) + randu()*90-45;
    else
    {cout<<"[both:]Unknown nodeID to reinitMobilityDirection(). Skipping reinitialization."<<endl;return;}
  
    mobilityInfo.directionPerServiceNode(indx)=rem(mobilityInfo.directionPerServiceNode(indx),360);
    mobilityInfo.distanceMovedPerServiceNode(indx) = 0;
  }
  else
  {
    mobilityInfo.directionPerServiceNode=randu(mobilityInfo.directionPerServiceNode.length())*360;
    mobilityInfo.distanceMovedPerServiceNode=zeros(mobilityInfo.directionPerServiceNode.length());
      
  }
   
}

/*!\fn createNodeGroup--> This function group the Nodes under the unique group Name given	
 *\param[in] groupname --> it gives the group name to be searched
 *\param[in] nodelist --> it gives the total number of nodes that should be presented under the particular groupname
 *\param[out] aNodeGroup.lenght--> it sets the lenght of the anodegroup after searching for particular group 
 */

void McellSystem::changeMobilityDirection(int servicenodeID, int serverNodeID, double maxAngleWrtToServingRadius)
{
  Location_S loc=getNodeLocation(serverNodeID);  
  Location_S loc1=getNodeLocation(servicenodeID);
  double angle=findAngle(loc,loc1)(0);
  
  if(randu()<0.5)
   angle=angle-randu()*maxAngleWrtToServingRadius;
  else
   angle=angle+randu()*maxAngleWrtToServingRadius;  
 
  int indx=find(mobilityInfo.serviceNodes,servicenodeID);
  if(indx!=-1){
    mobilityInfo.directionPerServiceNode(indx)=angle;
  }
  else
    {cout<<"[both:]Unknown nodeID to changeMobilityDirection(). Skipping reinitialization."<<endl;abort();}
}


int McellSystem::createNodeGroup(string groupName, ivec nodeList){

  for(int group_cnt=0;group_cnt<aNodeGroups.length();group_cnt++)
	if(aNodeGroups(group_cnt).groupName==groupName)
	{
	  cout<<"[both:]Error. Group Name already used for group."<<group_cnt<<endl;
	  abort();
	}

  sort(nodeList);
  NodeGroup_S newGroup;
  newGroup.groupName=groupName;
  newGroup.nodeIDs=nodeList;

  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
	aNodes(nodeList(node_cnt)).addToGroup(aNodeGroups.length());

  append(aNodeGroups,newGroup);
  return(aNodeGroups.length()-1);
}


Location_S getCentreLoc(Location_S satLoc,double elevationAngle)
{
    vec centreLoc = satLoc.getAsVector(3)/(satLoc.getDistFromOrigin()); 
    //cout<<"satLoc.getAsVector(3) : "<<satLoc.getAsVector(3)<<endl;
    //cout<<"satLoc.getDistFromOrigin() : "<<satLoc.getDistFromOrigin()<<endl;
    Location_S nadirLoc(centreLoc*(double)earthRadius);//This represents the projection of satellite boresight on the earth. 
    //cout<<"center location : "<<centreLoc<<endl;
    //cout<<"nadir location : "<<nadirLoc<<endl;
    //cout<<"elevation angle value : "<<elevationAngle<<endl;
    if(elevationAngle==90) return nadirLoc;

    double Re = (double) earthRadius;
    mat loc;Location_S reqLoc;
    do
    {
        loc = getCartesianVector(randu()*360.0,randu()*180.0);
        //loc = getCartesianVector(0,0);
        reqLoc = Location_S(loc.get_col(0)*Re);
        cout<<"elevation angle!=90 then loc is :"<<reqLoc<<endl;
        
    }while(abs(getElevationAngle(satLoc,reqLoc)-elevationAngle)>1e-3);
    cout<<"required loc :"<<reqLoc<<endl;
    return reqLoc;
}

mat getUnitVector(Location_S l1,Location_S l2)
{
    mat t = l2.getAsMatrix(3)-l1.getAsMatrix(3);
    t = t/sqrt((t.T()*t)(0,0));
    return t;
}

void McellSystem::setCarrierFrequencyForAllNodes(double carrierFreqInHz,double bandWidthInHz)
{
    ivec serverNodes = getNodes(_NODECATEGORY_SERVERNODE_);
    ivec serviceNodes = getNodes(_NODECATEGORY_SERVICENODE_);
    
    switch(frequencyReUseFactor)
    {
        case 1: 
            setNodeCarrierFrequencyByIndex(concat(serverNodes,serviceNodes),carrierFreqInHz);
            break;
        case 2:
            {
                vec frequency(2);
                frequency(0) = carrierFreqInHz - (bandWidthInHz/2);
                frequency(1) = carrierFreqInHz + (bandWidthInHz/2);
                if(aNodes(serviceNodes(0)).txAntenna.getAntennaType()!=_ANTENNA_TYPE_CIRCULAR_)
                {
                    cout<<"frequencyReUseFactor of 2 is not supported for non-circular type antenna.."<<endl;
                    abort();
                }
                int nCells = (find(nodeDropMethods,_DROPMETHOD_SATELLITE_)!=-1) ? nSectorsPerSite : serverNodes.length()/nSectorsPerSite;
                
                ivec freqIDPerCell = getFrequencyIDPerCell(frequencyReUseFactor,nCells);
                for(int srvr_cnt=0;srvr_cnt<serverNodes.length();srvr_cnt++)
                {
                    int freqID = freqIDPerCell(aNodes(serverNodes(srvr_cnt)).getCellID());
                    aNodes(serverNodes(srvr_cnt)).setCarrierFreqInHz(frequency(freqID%2));
                    aNodes(serverNodes(srvr_cnt)).setFrequencyID(freqID);
                }
                
                for(int srvc_cnt=0;srvc_cnt<serviceNodes.length();srvc_cnt++)
                {
                    int cellID = aNodes(serviceNodes(srvc_cnt)).getCellID();
                    aNodes(serviceNodes(srvc_cnt)).setCarrierFreqInHz(aNodes(cellID).getCarrierFreqInHz());
                    aNodes(serviceNodes(srvc_cnt)).setFrequencyID(aNodes(cellID).getFrequencyID());
                }
            }
            break;
        case 3:
            {
                vec frequency(frequencyReUseFactor);
                frequency(0) = carrierFreqInHz - (bandWidthInHz/3);
                frequency(1) = carrierFreqInHz;
                frequency(2) = carrierFreqInHz + (bandWidthInHz/3);
                
                int nCells = (find(nodeDropMethods,_DROPMETHOD_SATELLITE_)!=-1) ? nSectorsPerSite : serverNodes.length()/nSectorsPerSite;
                
                ivec freqIDPerCell = getFrequencyIDPerCell(frequencyReUseFactor,nCells);
                for(int srvr_cnt=0;srvr_cnt<serverNodes.length();srvr_cnt++)
                {
                    int freqID = freqIDPerCell(aNodes(serverNodes(srvr_cnt)).getCellID());
                    aNodes(serverNodes(srvr_cnt)).setCarrierFreqInHz(frequency(freqID));
                    aNodes(serverNodes(srvr_cnt)).setFrequencyID(freqID);
                }
                
                for(int srvc_cnt=0;srvc_cnt<serviceNodes.length();srvc_cnt++)
                {
                    int cellID = aNodes(serviceNodes(srvc_cnt)).getCellID();
                    aNodes(serviceNodes(srvc_cnt)).setCarrierFreqInHz(aNodes(cellID).getCarrierFreqInHz());
                    aNodes(serviceNodes(srvc_cnt)).setFrequencyID(aNodes(cellID).getFrequencyID());
                }
            }
            break;
        default:
            cout<<"Unsupported Frequency Reuse Factor"<<endl;abort();
            break;
    }
}
 
/*!\fn dropnodes()
 * \brief--> this function tells how to drop nodes of differnt types in a the system (BS,US etc)
 * \brief --> if we want to drop the node firstly drop hexagonal nodes and go for remaining
*/
void McellSystem::dropNodes(){     
  cout<<"Inside dropNodes function"<<endl;
  double sqrtThree=1.732;
  int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);  
  int satNodeType=find(nodeDropMethods,_DROPMETHOD_SATELLITE_);
  int rndHexDropNodeType=find(nodeDropMethods,_DROPMETHOD_RANDOMHEX_);  
  int singleCellNodeType=find(nodeDropMethods,_DROPMETHOD_SINGLE_CELL_);
  int inhNodeType=find(nodeDropMethods,_DROPMETHOD_INBUILDING_);
  if(inhNodeType==-1)
      inhNodeType=find(nodeDropMethods,_DROPMETHOD_INBUILDING3SEC_);
  cout<<"\t"<<"hexDropNodeType"<<hexDropNodeType<<endl;
  cout<<"\t"<<"rndHexDropNodeType"<<rndHexDropNodeType<<endl;
  cout<<"\t"<<"singleCellNodeType"<<singleCellNodeType<<endl;
  cout<<"\t"<<"inhNodeType"<<inhNodeType<<endl;
  if(hexDropNodeType!=-1)          
  {
	Array <Location_S> hexLoc=generateHexCellCenterLocation(nNodes(hexDropNodeType),cellISD/sqrtThree); 
	ivec hexNodes=getNodes(aNodeTypes(hexDropNodeType)); 


	for(int hexNode_cnt=0;hexNode_cnt<nNodes(hexDropNodeType);hexNode_cnt++) 
	{
	  aNodes(hexNodes(hexNode_cnt)).setXYLoc(hexLoc(floor(hexNode_cnt/nSectorsPerSite)));
      aNodes(hexNodes(hexNode_cnt)).setHexagonalCellID(floor(hexNode_cnt/nSectorsPerSite));
      aNodes(hexNodes(hexNode_cnt)).setNeighbourCell(getWrapAroundNeighbours(nNodes(hexDropNodeType)/nSectorsPerSite, floor(hexNode_cnt/nSectorsPerSite), hexLoc(floor(hexNode_cnt/nSectorsPerSite)), cellISD));//setting neighbouring cell//
	}
	int newGroup;    
	
	//! If more than one node have the same cell location they are grouped normally depends upon the sectors more than one basestation have same center location/////
	
    for(int node_cnt=0;node_cnt<(nNodes(hexDropNodeType)/nSectorsPerSite);node_cnt++)
	{
        newGroup=createNodeGroup("SameLoc"+to_str(node_cnt),hexNodes.get(node_cnt*nSectorsPerSite,(node_cnt+1)*nSectorsPerSite-1));
	      aNodeGroups(newGroup).property.set_size(0); //! Setting size of group ex: 3 for cell with 3 sectors//
	      aNodeGroups(newGroup).propertyName="SourceNode";
	}

	ivec tempNode;  
	for(int type_cnt=0;type_cnt<aNodeTypes.length();type_cnt++)
	{
	  if(type_cnt!=hexDropNodeType)
	  {
		if(nodeDropMethods(type_cnt)==_DROPMETHOD_HEXAGONAL_)
		{
		  cout<<"[both:]Error. More than one Node Type can't have Hexagonal Drop in a system."; //! in a cell only hexagonal drop should be once//
		  abort();
		}
		else
		{
		  if(nNodes(type_cnt)>0)                             
		  {
			tempNode=getNodes(aNodeTypes(type_cnt));      
			int nodesPerCell=floor_i((double)nNodes(type_cnt)/(double)hexLoc.length());
			int totalNodeCount=0;                         //
			for(int node_cnt=0;node_cnt<nodesPerCell;node_cnt++) 
			  for(int cell_cnt=0;cell_cnt<hexLoc.length();cell_cnt++)
			  {
			    if(!cellWiseDropping)   
                    aNodes(tempNode(totalNodeCount)).setXYLoc((generateUniformDropLocationInHexCell(hexLoc(cell_cnt),nSectorsPerSite,node_cnt%nSectorsPerSite,cellISD/sqrt(3),minDist))(0));//the drop is given by generateUniformDropLocationInHexCell of which ue should be dropped//
			    else
			      aNodes(tempNode(totalNodeCount)).setXYLoc((generateUniformDropLocationInHexCell(hexLoc(cell_cnt),cellISD/sqrt(3),minDist))(0));//cell wise dropping is done which is given by same func//
			    aNodes(tempNode(totalNodeCount)).setHexagonalCellID(cell_cnt);
			    totalNodeCount++;       
			  }
			int rndCell; 
			for(int node_cnt=0;node_cnt<(nNodes(type_cnt)-(hexLoc.length()*nodesPerCell));node_cnt++)
			{
                rndCell=randi(0,1000)%(nNodes(hexDropNodeType)/nSectorsPerSite); //creating a random location in the cell//
			  aNodes(tempNode(totalNodeCount)).setXYLoc((generateUniformDropLocationInHexCell(hexLoc(rndCell),cellISD/sqrt(3),minDist))(0));
			  aNodes(tempNode(totalNodeCount)).setHexagonalCellID(rndCell);  
			  totalNodeCount++;
			}
		  }
		}
	  }
	}
  }
  else if(rndHexDropNodeType!=-1)          
  {
    ivec rndhexNodes=getNodes(_DROPMETHOD_RANDOMHEX_,true); 
    Array <Location_S> rndLoc= generateUniformDropLocation(coverageArea,rndhexNodes.length()/nSectorsPerSite); 
	int nodeCount = rndhexNodes.length();
    
	for(int rndhex_cnt=0;rndhex_cnt<nodeCount;rndhex_cnt++) 
	{
	  aNodes(rndhexNodes(rndhex_cnt)).setXYLoc(rndLoc(floor(rndhex_cnt/nSectorsPerSite)));
      aNodes(rndhexNodes(rndhex_cnt)).setHexagonalCellID(floor(rndhex_cnt/nSectorsPerSite));
	}
	int newGroup;    
	
	//! If more than one node have the same cell location they are grouped normally depends upon the sectors more than one basestation have same center location/////
	
    for(int node_cnt=0;node_cnt<(nodeCount/nSectorsPerSite);node_cnt++)
	{
        newGroup=createNodeGroup("SameLoc"+to_str(node_cnt),rndhexNodes.get(node_cnt*nSectorsPerSite,(node_cnt+1)*nSectorsPerSite-1));
        aNodeGroups(newGroup).property.set_size(0); //! Setting size of group ex: 3 for cell with 3 sectors//
        aNodeGroups(newGroup).propertyName="SourceNode";
	}
    
	
	ivec tempNode;  
    ivec rndHexTypes=find(nodeDropMethods,(int)_DROPMETHOD_RANDOMHEX_,true);
	for(int type_cnt=0;type_cnt<aNodeTypes.length();type_cnt++)
	{
	  if(find(rndHexTypes,type_cnt)==-1)
	  {
        if(nNodes(type_cnt)>0)                             
        {
        tempNode=getNodes(aNodeTypes(type_cnt));     
        Array<Location_S> nodeLoc = generateUniformDropLocation(coverageArea,length(tempNode));
        for(int node_cnt=0;node_cnt<length(tempNode);node_cnt++)
            aNodes(tempNode(node_cnt)).setXYLoc(nodeLoc(node_cnt));
        }
	  }
	}
  }
  else    
  {
    if(singleCellNodeType!=-1)
    {
      Location_S hexLoc(0.0,-cellISD/sqrt(3.0),0.0); 
      ivec singleCellNode=getNodes(aNodeTypes(singleCellNodeType));  // will be of length 1 always
      
      
      aNodes(singleCellNode(0)).setXYLoc(hexLoc);
      aNodes(singleCellNode(0)).setHexagonalCellID(0);
      ivec tempNode;  
      for(int type_cnt=0;type_cnt<aNodeTypes.length();type_cnt++)
      {
	if(type_cnt!=singleCellNodeType)
	{
	    if(nNodes(type_cnt)>0)                             
	    {
	      tempNode=getNodes(aNodeTypes(type_cnt));      
	      int nodesPerCell=nNodes(type_cnt);
	      int totalNodeCount=0;                         //
	      for(int node_cnt=0;node_cnt<nodesPerCell;node_cnt++) 
	      {
		Location_S tempLoc;
		do
		{
		  
		  tempLoc=(generateUniformDropLocationInHexCell(Location_S(0,0,0),cellISD/sqrt(3),0.0))(0);
		  
		}while(findDistance(hexLoc,tempLoc)<minDist);
		  
		aNodes(tempNode(totalNodeCount)).setXYLoc(tempLoc);//cell wise dropping is done which is given by same func//
		aNodes(tempNode(totalNodeCount)).setHexagonalCellID(0);
		totalNodeCount++;       
		
	      }
	  }
	}
      }
    }
    else if(inhNodeType!=-1)
    {
        Array <Location_S> equLoc=generateEquiDistantLocationInRect(coverageArea,nNodes(inhNodeType)/nSectorsPerSite,cellISD); 
        ivec equNodes=getNodes(aNodeTypes(inhNodeType)); 
        
        for(int equNode_cnt=0;equNode_cnt<equNodes.length();equNode_cnt++)
        {
            int cellID = floor_i(equNode_cnt/nSectorsPerSite);
            aNodes(equNodes(equNode_cnt)).setXYLoc(equLoc(cellID));
            aNodes(equNodes(equNode_cnt)).setHexagonalCellID(cellID);
        }

        int newGroup;    
        //! If more than one node have the same cell location they are grouped normally depends upon the sectors more than one basestation have same center location/////
        for(int node_cnt=0;node_cnt<(nNodes(inhNodeType)/nSectorsPerSite);node_cnt++)
        {
            newGroup=createNodeGroup("SameLoc"+to_str(node_cnt),equNodes.get(node_cnt*nSectorsPerSite,(node_cnt+1)*nSectorsPerSite-1));
            aNodeGroups(newGroup).property.set_size(0); //! Setting size of group ex: 3 for cell with 3 sectors//
            aNodeGroups(newGroup).propertyName="SourceNode";
        }
        
        ivec tempNode;  
        for(int type_cnt=0;type_cnt<aNodeTypes.length();type_cnt++)
        {
            if(type_cnt!=inhNodeType && nNodes(type_cnt)>0)
            {
                if(nodeDropMethods(type_cnt)==_DROPMETHOD_INBUILDING_ || nodeDropMethods(type_cnt)==_DROPMETHOD_INBUILDING3SEC_)
                {
                    cout<<"[both:]Error. More than one Node Type can't have InH Drop in a system."; //! in a cell only hexagonal drop should be once//
                    abort();
                }
                else
                {
                    Array<Location_S> nodeLoc = generateIndoorLoc(equLoc,coverageArea,minDist,nNodes(type_cnt));
                    tempNode=getNodes(aNodeTypes(type_cnt));  
                    for(int node_cnt=0;node_cnt<nNodes(type_cnt);node_cnt++)
                        aNodes(tempNode(node_cnt)).setXYLoc(nodeLoc(node_cnt));
                }
            }
        }
    }
    else if(satNodeType!=-1)
            {
                
                Array<Location_S> satLoc = generate3DSatelliteDropLocation(nodeTypeHeights(satNodeType),nNodes(satNodeType)/nSectorsPerSite);
                //cout<<"Info  :"<<nSectorsPerSite<<" , "<<nNodes(satNodeType)<<" , "<<satLoc << endl;
                ivec satNodes=getNodes(aNodeTypes(satNodeType));
                //cout << "satNodes : " <<satNodes <<endl;
                //abort();
                bool printAllLocations = true;
                for(int sat_cnt=0,node_cnt=0;sat_cnt<nNodes(satNodeType)/nSectorsPerSite;sat_cnt++)
                {
                    Location_S centreLoc = getCentreLoc(satLoc(sat_cnt),beamElevationAngle);
                    //cout << "centreLoc : " <<centreLoc << beamElevationAngle << endl;
                    Array<Location_S> hexLoc = generateHexCellCenterLocationIn3D(nSectorsPerSite,satelliteBeamRadius,centreLoc);
                    //cout<<"HexCentres : "<<hexLoc<<endl;
                    for(int beam_cnt=0;beam_cnt<nSectorsPerSite;beam_cnt++,node_cnt++)
                    {
                        aNodes(satNodes(node_cnt)).setXYLoc(satLoc(sat_cnt),1);
                        //cout<< "node_cnt"<<node_cnt<<endl;
                        aNodes(satNodes(node_cnt)).setHeight(satLoc(sat_cnt).z,1);
                        aNodes(satNodes(node_cnt)).setHexagonalCellID(beam_cnt);
                        aNodes(satNodes(node_cnt)).setSatelliteID(sat_cnt);
                        
                        Location_S coverageCentre = hexLoc(beam_cnt);
                        //cout<<"satellite coverage area"<< satelliteBeamRadius<<"|"<<coverageCentre<<"|"<<node_cnt<<endl;
                        aNodes(satNodes(node_cnt)).setCoverageInfo(satelliteBeamRadius,coverageCentre,node_cnt);
                        aNodes(satNodes(node_cnt)).setAntennaTilts(aNodes(satNodes(node_cnt)).ntnCoverageInfo.centre);

//                         aNodes(satNodes(node_cnt)).setNeighbourCell(getWrapAroundNeighbours3D(nSectorsPerSite,beam_cnt,coverageCentre,satelliteBeamRadius));
                        
                        if(printAllLocations)
                        {
                            std::ofstream op("centreLoc.txt",std::ios::app);
                            op<<satNodes(node_cnt)<<","<<coverageCentre.x<<","<<coverageCentre.y<<","<<coverageCentre.z<<";"<<endl;
                            op.close();
                        }
                    }
                    if(printAllLocations)
                    {
                        std::ofstream op("loc.txt",std::ios::app);
                        op<<sat_cnt<<","<<satLoc(sat_cnt).x<<","<<satLoc(sat_cnt).y<<","<<satLoc(sat_cnt).z<<";"<<endl;
                    }
                }
                
                int newGroup;    
                
                //! If more than one node have the same cell location they are grouped normally depends upon the sectors more than one basestation have same center location/////
                
                for(int node_cnt=0;node_cnt<(nNodes(satNodeType)/nSectorsPerSite);node_cnt++)
                {
                    newGroup=createNodeGroup("SameLoc"+to_str(node_cnt),satNodes.get(node_cnt*nSectorsPerSite,(node_cnt+1)*nSectorsPerSite-1));
                    aNodeGroups(newGroup).property.set_size(0); //! Setting size of group ex: 3 for cell with 3 sectors//
                    aNodeGroups(newGroup).propertyName="SourceNode";
                }
                
                ivec tempNode;  
                for(int type_cnt=0;type_cnt<aNodeTypes.length();type_cnt++)
                {
                    if(type_cnt!=satNodeType && nNodes(type_cnt)>0)
                    {
                        
                        if(nodeDropMethods(type_cnt)==_DROPMETHOD_SATELLITE_)
                        {
                            cout<<"[both:]Error. More than one Node Type can't have Satellite Drop in a system."; 
                            abort();
                        }
                        else if(nodeDropMethods(type_cnt)!=_DROPMETHOD_RANDOM_)
                        {
                            cout<<"Drop Method other than random is not supported for non-Satellite nodes"<<endl;
                            abort();
                        }
                        switch(nodeDropMethods(type_cnt))
                        {
                            case _DROPMETHOD_SATELLITE_:
                                cout<<"[both:]Error. More than one Node Type can't have Satellite Drop in a system."; 
                                abort();
                                break;
                            case _DROPMETHOD_RANDOM_:
                                {
                                    
                                    //NOTE: nBeamsPerSatellite has the number of cells in which UEs are dropped and
                                    //nSectorsPerSite has the number of cells to be considered for proper interference profile. 
                                    tempNode=getNodes(aNodeTypes(type_cnt));
                                    
                                    for(int node_cnt=0;node_cnt<nNodes(type_cnt);node_cnt++)
                                    {
                                        int nSatellites = nNodes(satNodeType) / nSectorsPerSite;
                                        //cout <<"satNodeType : " << satNodeType << " nSatellites : " << nSatellites<<endl;
                                        
                                        int nNodesPerSatellite = nNodes(type_cnt)/nSatellites;
                                        //cout <<"nNodesPerSatellite : " << nNodesPerSatellite << " type_cnt : " << type_cnt<<endl;
                                        int satID = floor_i(node_cnt/nNodesPerSatellite);
                                        int bsNode = node_cnt%nBeamsPerSatellite + satID*nSectorsPerSite;
                                        //int bsNode = node_cnt%1 + satID*nSectorsPerSite;
                                        //cout << "bsNode :" << bsNode<<endl; 
                                        NTNCoverageInfo_S coverageInfo = aNodes(bsNode).ntnCoverageInfo;
                                        
                                        Location_S nodeLoc = generateUniformDropLocationInHexCell3D(coverageInfo.centre,coverageInfo.radius,0,nodeTypeHeights(type_cnt),1)(0);
                                        //coverageInfo.radius, to drop at center cell radius given as 0-DD
                                        //Location_S nodeLoc = {-5.81174e+06,	-2.39042e+06,	-1.04846e+06};
                                        //cout << "nodeLoc : " << nodeLoc <<endl;
                                        aNodes(tempNode(node_cnt)).setXYLoc(nodeLoc,1);
                                        aNodes(tempNode(node_cnt)).setHeight(nodeLoc.z,1);
                                        aNodes(tempNode(node_cnt)).setAntennaTilts(aNodes(bsNode).getLoc());
                                        //cout<<"find the nod lo to get tilts : "<<aNodes(bsNode).getLoc()<<endl;
                                        aNodes(tempNode(node_cnt)).setHexagonalCellID(aNodes(bsNode).getCellID());
                                        aNodes(tempNode(node_cnt)).setSatelliteID(aNodes(bsNode).getSatelliteID());
                                        //cout << "nodeLoc : " <<  aNodes(tempNode(node_cnt)).setXYLoc(nodeLoc,1) <<endl;
                                        if(printAllLocations)
                                        {
                                            std::ofstream op("loc.txt",std::ios::app);
                                            op<<tempNode(node_cnt)<<","<<nodeLoc.x<<","<<nodeLoc.y<<","<<nodeLoc.z<<";"<<endl;
                                            op.close();
                                        }
                                    }
                                }
                                //abort();
                                break;
                           
                            default:
                                cout<<"Unsupported Drop Method.."<<endl;
                                abort();
                        }
                    }
                }
            }
            
        else
            {
                Array<Location_S> nodeLoc = generateUniformDropLocation(coverageArea,sum(nNodes));
                for(int node_cnt=0;node_cnt<sum(nNodes);node_cnt++)
                    aNodes(node_cnt).setXYLoc(nodeLoc(node_cnt));
            }
            
    }
    
    if(sum(indoorNodePercentage))
    {
        for(int cnt=0;cnt<nNodes.length();cnt++) 
        {
            if(nNodes(cnt))       
            {
        if((cnt==hexDropNodeType || cnt==rndHexDropNodeType || cnt==singleCellNodeType) && indoorNodePercentage(cnt)!=0) 
        {
            cout<<"[detl:]Warning. Hexagonal Drop Nodes can't be indoor. Skipping indoor flag for node type."<<aNodeTypes(cnt)<<"'"<<endl;
        }
        else
        {
            if((cnt==inhNodeType) && (indoorNodePercentage(cnt)==0))
            {
                cout<<"[detl:]Warning. Inbuilding Drop Nodes can't be Outdoor. putting indoor flag for node type."<<aNodeTypes(cnt)<<"'"<<endl;
                indoorNodePercentage(cnt)=100;
            }
            setNodeTypeAsIndoor(aNodeTypes(cnt),indoorNodePercentage(cnt));
        }
      }
    }
  }

  if(sum(pedestrianNodePercentage))
  {
    for(int cnt=0;cnt<nNodes.length();cnt++) 
      if(nNodes(cnt) && pedestrianNodePercentage(cnt)!=0 && indoorNodePercentage(cnt)==0)       
	setNodeTypeAsPedestrian(aNodeTypes(cnt),pedestrianNodePercentage(cnt)); 
  }
}

/*!\fn dropnodes()
 * \brief--> this function tells how to drop nodes of differnt types in a the system (BS,US etc) if nodeTypes are specified
  *\param[in] nodesTypes --> gives the nodeTypes for which node to be dropped
*/

void McellSystem::dropNodes(ivec nodeTypes,bool overWrite,bool setIndoorNodes){              

 if(cellLayout==_CELLLAYOUT_HEXAGONAL_) 
  {
    int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);  
	ivec hexNodes=getNodes(aNodeTypes(hexDropNodeType));//
	
	//! If there no hexDropNodeType in the list given then check whether hex node had already been dropped ////
	//! unless and other hex nodes are dropped no other nodes shouldnt be dropped/////
     	if(find(nodeTypes,hexDropNodeType)==-1)                          
	{
	  if(std::isnan((aNodes(hexNodes(0)).getLoc()).x))                   
	  {                                                         
		cout<<"[both:]Error. Please Drop Hexagonal Nodes before dropping other node types."<<endl;
		abort();
	  }
           //! If hexagonal nodes are dropped before  or the there is hexagonal node in the given list of nodes then remaining nodes are dropped //
	  else  
	  {
		ivec tempNode;                                     
		int nNodesPerCell=1;  
		//! If the there are 3 and 6 nodes(BS ) per cell ,there locations are genereated and then grouped////
		if(nNodes(hexDropNodeType)==3 || nNodes(hexDropNodeType)==21 || nNodes(hexDropNodeType)==57) nNodesPerCell=3;
		else if(nNodes(hexDropNodeType)==42 || nNodes(hexDropNodeType)==114) nNodesPerCell=6;

		Array <Location_S> hexLoc=generateHexCellCenterLocation(nNodes(hexDropNodeType),cellISD/sqrt(3.0));//generating hexagonalcell center location//
		//! Making sure that one only one nodetype have hexagonal drop in a system////
		for(int type_cnt=0;type_cnt<nodeTypes.length();type_cnt++) 
		if(nodeTypes(type_cnt)!=hexDropNodeType)
		{
		  if(nodeDropMethods(nodeTypes(type_cnt))==_DROPMETHOD_HEXAGONAL_)
		  {
			cout<<"[both:]Error. More than one Node Type can't have Hexagonal Drop in a system.";//morethan one nodetype cant have hexagonal drop// 
			abort();
		  }
		    //! If there is no hexagonal dropnode types then see how many nodes per cell and how the nodes are to be distributed in the cell// 
		  else
		  {
		    //! For these types of nodes check whether cellwise droping or sectorwise droping should be done ///
		    //! If it is complete random node, random drop should be done but in particular cell/// 
			if(nNodes(nodeTypes(type_cnt))>0) 
			{
			  tempNode=getNodes(aNodeTypes(nodeTypes(type_cnt)));
			  int nodesPerCell=floor_i((double)nNodes(nodeTypes(type_cnt))/(double)hexLoc.length());
			  int totalNodeCount=0;
			    for(int node_cnt=0;node_cnt<nodesPerCell;node_cnt++)   
			      for(int cell_cnt=0;cell_cnt<hexLoc.length();cell_cnt++)
			      {
			      if(!cellWiseDropping)  //! If itis not cell wisedroping it should be sector wise//
				aNodes(tempNode(totalNodeCount)).setXYLoc((generateUniformDropLocationInHexCell(hexLoc(cell_cnt),nNodesPerCell,node_cnt%nNodesPerCell,cellISD/sqrt(3.0),minDist))(0), overWrite);//setting node location by the function generateUniformDropLocationInHexCell//
			      else      //! If there is cell wise droping//
				aNodes(tempNode(totalNodeCount)).setXYLoc((generateUniformDropLocationInHexCell(hexLoc(cell_cnt),cellISD/sqrt(3.0),minDist))(0), overWrite);//setting XY locations from the function//
			      aNodes(tempNode(totalNodeCount)).setHexagonalCellID(cell_cnt);
			      totalNodeCount++;
			    }
			    int rndCell; //! If it is node which is completely random then generate a completely random location in a particular cell and drop there///
			  for(int node_cnt=0;node_cnt<(nNodes(nodeTypes(type_cnt))-(hexLoc.length()*nodesPerCell));node_cnt++)
			  {
			    rndCell=randi(0,1000)%(nNodes(hexDropNodeType)/nNodesPerCell);
			    aNodes(tempNode(totalNodeCount)).setXYLoc((generateUniformDropLocationInHexCell(hexLoc(rndCell),cellISD/sqrt(3.0),minDist))(0), overWrite);//creating a random location in particular hex cell//
			    aNodes(tempNode(totalNodeCount)).setHexagonalCellID(rndCell);
			    totalNodeCount++;
			  }
			}
		  }
		}
	  }

	}
	else //! If there are hexnodestypes in the list given, check for them and set their locations,cell ID's,neighbouring cells//// ////
	{
	  int nNodesPerCell=1;   
	  //! Depends upon the number of BS per cell cell locations and grouping are done////
	  //! For those nodes dropping should be done depend on ISD  and thier cell ID's and neighbouring cells should be specified////
	  if(nNodes(hexDropNodeType)==3 || nNodes(hexDropNodeType)==21 || nNodes(hexDropNodeType)==57) nNodesPerCell=3;
	  else if(nNodes(hexDropNodeType)==42 || nNodes(hexDropNodeType)==114) nNodesPerCell=6;

	  Array <Location_S> hexLoc=generateHexCellCenterLocation(nNodes(hexDropNodeType),cellISD/sqrt(3.0)); //! Generating hexcell center location//

	  for(int hexNode_cnt=0;hexNode_cnt<nNodes(hexDropNodeType);hexNode_cnt++)
	  {
		aNodes(hexNodes(hexNode_cnt)).setXYLoc(hexLoc(floor(hexNode_cnt/nNodesPerCell)),overWrite); 
		aNodes(hexNodes(hexNode_cnt)).setHexagonalCellID(floor(hexNode_cnt/nNodesPerCell));
		aNodes(hexNodes(hexNode_cnt)).setNeighbourCell(getWrapAroundNeighbours(nNodes(hexDropNodeType)/nNodesPerCell, floor(hexNode_cnt/nNodesPerCell), hexLoc(floor(hexNode_cnt/nNodesPerCell)), cellISD));
	  }
	  int newGroup;  //! Grouping of all the hexnodes having the sameLoc as name and naming it as source node//
	  	for(int node_cnt=0;node_cnt<(nNodes(hexDropNodeType)/nNodesPerCell);node_cnt++)
		{
		  newGroup=createNodeGroup("SameLoc"+to_str(node_cnt),hexNodes.get(node_cnt*nNodesPerCell,(node_cnt+1)*nNodesPerCell-1));//group into newgroup//
		  aNodeGroups(newGroup).property.set_size(0);
		  aNodeGroups(newGroup).propertyName="SourceNode";
		}

	  ivec tempNode; 
	  //! Only one node can have hexagonal drop in a system to make sure that check for dropmethod hexagonal//
	  
	  for(int type_cnt=0;type_cnt<nodeTypes.length();type_cnt++)
		if(nodeTypes(type_cnt)!=hexDropNodeType)  
		{
		  if(nodeDropMethods(nodeTypes(type_cnt))==_DROPMETHOD_HEXAGONAL_) 
		  {
			cout<<"[both:]Error. More than one Node Type can't have Hexagonal Drop in a system.";
			abort();
		  }
		  else   //! Else if the  dropmethod is not hexagonal which implies random droping should be there///
                        //! Get how many  nodes per cell should be present//    
		  {
			if(nNodes(nodeTypes(type_cnt))>0)  
			{
			  tempNode=getNodes(aNodeTypes(nodeTypes(type_cnt))); 
			  int nodesPerCell=floor_i((double)nNodes(nodeTypes(type_cnt))/(double)hexLoc.length());
			  int totalNodeCount=0;
			  for(int node_cnt=0;node_cnt<nodesPerCell;node_cnt++)
			    for(int cell_cnt=0;cell_cnt<hexLoc.length();cell_cnt++)
			    {
			      aNodes(tempNode(totalNodeCount)).setXYLoc((generateUniformDropLocationInHexCell(hexLoc(cell_cnt),node_cnt%nNodesPerCell,cellISD/sqrt(3.0),minDist))(0),overWrite); //! Setting XY location by the function generateUniformDropLocationInHexCell //
			      aNodes(tempNode(totalNodeCount)).setHexagonalCellID(cell_cnt);
			      totalNodeCount++;
			    }
			  int rndCell; //! For the node which is totally random then drop those nodes in a particular cell randomly and get their locations ////
			  for(int node_cnt=0;node_cnt<(nNodes(nodeTypes(type_cnt))-(hexLoc.length()*nodesPerCell));node_cnt++)
			  {
			    rndCell=randi(0,1000)%(nNodes(hexDropNodeType)/nNodesPerCell);
			    aNodes(tempNode(totalNodeCount)).setXYLoc((generateUniformDropLocationInHexCell(hexLoc(rndCell),cellISD/sqrt(3.0),minDist))(0),overWrite);//!setting a location randomly in cell//
			    aNodes(tempNode(totalNodeCount)).setHexagonalCellID(rndCell);//! Setting hexagonal cell id for that random cell//
			    totalNodeCount++;
			  }
			}
		  }
	        }
	}
	if(setIndoorNodes)
	{
	  if(sum(indoorNodePercentage))//! If indoor node percentage is considered//
	  {    
	    for(int cnt=0;cnt<nodeTypes.length();cnt++) 
	      if(nNodes(nodeTypes(cnt))) 
	      {
		if(nodeTypes(cnt)==hexDropNodeType && indoorNodePercentage(nodeTypes(cnt))!=0) //! If there is a hexagonal node type in indoor node which is not possible//
		{                                             
		  cout<<"[detl:]Warning. Hexagonal Drop Nodes can't be indoor. Skipping indoor flag for node type."<<aNodeTypes(nodeTypes(cnt))<<"'"<<endl;
		}
		else
		  setNodeTypeAsIndoor(aNodeTypes(nodeTypes(cnt)),indoorNodePercentage(nodeTypes(cnt)));//! Setting node type as indoor//
	      }
	  }
	}
  }
  else //! If cell layout is not hexagonal layout get the random layout nodes and drop them randomly and check for the indoor percentage of those nodes//
  {
    int singleCellNodeType=find(nodeDropMethods,_DROPMETHOD_SINGLE_CELL_);  
    Location_S hexLoc=Location_S(0.0,-cellISD/sqrt(3.0),0);
    ivec nodesToDrop; //! Get the nodes to drop//
    for(int type_cnt=0;type_cnt<nodeTypes.length();type_cnt++) 
    for(int node_cnt=0;node_cnt<nNodes(nodeTypes(type_cnt));node_cnt++)
    {
      nodesToDrop=getNodes(nodeTypes(type_cnt));
      if(singleCellNodeType==-1)
	aNodes(nodesToDrop(node_cnt)).setXYLoc((generateUniformDropLocation(coverageArea))(0),overWrite);//! Setting XY location by the function genrateuniformDropLocation//
      else
      {
	Location_S tempLoc;
	do
	{
	  tempLoc=(generateUniformDropLocationInHexCell(Location_S(0,0,0),cellISD/sqrt(3.0),0.0))(0);
	  
	}while(findDistance(hexLoc,tempLoc)<minDist);
	
	aNodes(nodesToDrop(node_cnt)).setXYLoc(tempLoc,overWrite);
	aNodes(nodesToDrop(node_cnt)).setHexagonalCellID(0);
      }
    }
    if(setIndoorNodes)
    if(sum(indoorNodePercentage))
    {
      for(int cnt=0;cnt<nodeTypes.length();cnt++)
	if(nNodes(nodeTypes(cnt)))
	  setNodeTypeAsIndoor(aNodeTypes(nodeTypes(cnt)),indoorNodePercentage(nodeTypes(cnt)));
    }
    
  }

}
vec extractVector(string word)
{
    int start = word.find("[");
    int end = word.find("]",start+1);
    return vec(word.substr(start+1,end-(start+1)));
}

void getLocations(string filename,ivec& nodeIDs,Array<Location_S>& newLoc,Array<Orientation_S> &newOrient,Array<vec>& newVelocity)
{
    //NodeType	NodeID	X_Location	Y_Location	Z_Location	isIndoor	isWithLowPenetraionLoss
    newLoc.set_length(0);
    nodeIDs.set_length(0);
    ifstream infile;
    infile.open(filename.c_str());
    string nodeLocationInfo;
    getline(infile,nodeLocationInfo); //To Remove the first line(Header)
    while(getline(infile,nodeLocationInfo))
    {
        stringstream ss(nodeLocationInfo);
        string word;
        vector<string> words;
        int nodeID;
        double x,y,z;
        while(getline(ss, word,'\t')) 
        { 
            words.push_back(word); 
        }
        nodeID = stoi(words[1]);
        x = stod(words[2]);y = stod(words[3]);z = stod(words[4]);
        Location_S loc(x,y,z);
        loc.isIndoor = stoi(words[5]);
        loc.isLowPenetrationLoss = stoi(words[6]);
        if(words.size()>7)
        {
            loc.floorNumber = stoi(words[7]);
            loc.isPedestrian = stoi(words[8]);
            append(newOrient,Orientation_S(extractVector(words[9])));
            append(newVelocity,extractVector(words[10]));
        }
        append(newLoc,loc);
        append(nodeIDs,nodeID);
    }
    infile.close();
}

void getNodeLocations(string filename,ivec& nodeIDs,Array<Location_S>& newLocations)
{
    ifstream infile;
    infile.open (filename.c_str());
    string nodeLocationInfo;
    string toFind1="_";
    string toFind2="[",toFind3="]";
    string nodeIDStr="";
    size_t start,end;
    while(getline(infile,nodeLocationInfo))
        if(nodeLocationInfo.substr(0,6)=="NODEID") 
        {
            //Extracting nodeIDs
            start = nodeLocationInfo.find(toFind1);
            end = nodeLocationInfo.find(toFind1,start+1);
            nodeIDStr=nodeIDStr+nodeLocationInfo.substr(start+1,end-start-1)+" ";
            
            //Extracting locations
            start = nodeLocationInfo.find(toFind2);
            end = nodeLocationInfo.find(toFind3,start+1);
            append(newLocations,Location_S(extractVector(nodeLocationInfo)));
        }
        nodeIDs=ivec(nodeIDStr);
}

void McellSystem::updateNodeLocations(ivec NodeIDs,Array<Location_S> &newLoc,Array<Orientation_S> &newOrient,Array<vec>& newVelocity)
{
    for(int node_cnt=0;node_cnt<NodeIDs.length();node_cnt++)
    {
        if(isInCoverage(newLoc(node_cnt)))
        {
            int nodeIndx = find(nodeIDs,NodeIDs(node_cnt));
            aNodes(nodeIndx).updateLocation(newLoc(node_cnt));
            if(newVelocity.length())
            {
                aNodes(nodeIndx).setVelocity(newVelocity(node_cnt));
                aNodes(nodeIndx).nodeOrient = newOrient(node_cnt);
            }
        }
        else
        {
            cout<<"Location outside coverage.."<<endl;abort();
        }
    }
}
void McellSystem::clearSCMGrids()
{
    for(int node_cnt=0;node_cnt<aNodes.length();node_cnt++)
    {
        aNodes(node_cnt).scmGrids.set_length(0);
        aNodes(node_cnt).scmGridsForSpConsist.set_length(0);
    }
}


void McellSystem::copySCMGridsForIntrasiteCorrelation()
{
    
    ivec gridHolderNodeTypes(0);
    Array<NodeCategory_E> nodeCategoryPerNodeType=getNodeCategory();
    for(int nodeCat_cnt=0;nodeCat_cnt<aNodeTypes.length();nodeCat_cnt++)
        if(nNodes(nodeCat_cnt)!=0)
        {
            if(nodeCategoryPerNodeType(nodeCat_cnt)==_NODECATEGORY_SERVERNODE_)
                append(gridHolderNodeTypes,nodeCat_cnt);
        }
        
    for(int type_cnt=0;type_cnt<length(gridHolderNodeTypes);type_cnt++)
    {
        ivec tempNodes=getNodes(gridHolderNodeTypes(type_cnt));
        for(int node_cnt=0;node_cnt<length(tempNodes);node_cnt++)
        {//Code to maintain site-to-site correlation (overwriteLinkInfo() will become redundant because of this) - Dhiv
            ivec txGroupID=findGroup(aNodes(tempNodes(node_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernodes with the same loc(name) and getting thier IDs//
            if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0) // Defining the first node as reference node
            {
                aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
                aNodeGroups(txGroupID(0)).nodeIDs(0)=tempNodes(node_cnt);
            }
            else  //Copy grids from reference node to maintain site-to-site correlation of 1 - Dhiv
            {
                aNodes(tempNodes(node_cnt)).scmGrids = aNodes(aNodeGroups(txGroupID(0)).nodeIDs(0)).scmGrids;
                aNodes(tempNodes(node_cnt)).scmGridsForSpConsist = aNodes(aNodeGroups(txGroupID(0)).nodeIDs(0)).scmGridsForSpConsist;
            }
        } 
    }
}

void McellSystem::setLinkInfo(NodeLinkInfo_S& linkInfo,int rxNodeID)
{
    int nodeIndx = find(nodeIDs,rxNodeID);
    if(nodeIndx==-1)
    {
        cout<<"Unknown nodeID "<<rxNodeID<<" in setLinkInfo()"<<endl;abort();
    }
    aNodes(nodeIndx).updateLinkInfo(linkInfo);
}

void getSCMparams(string fileName,Array<vec>& scmParams)
{
    ifstream infile;
    infile.open(fileName.c_str());
    if(!infile.good())
    {
        cout<<fileName<<" file doesn't exist...Skipping the updation of locations.."<<endl;return;
    }
    scmParams.set_length(0);
    int i=0;
    string nodeLinkInfo;
    getline(infile,nodeLinkInfo); //To Remove the first line(Header)
    while(getline(infile,nodeLinkInfo))
    {
        stringstream ss(nodeLinkInfo);
        string word;
        vector<string> words;
        while(getline(ss, word,',')) 
        { 
            words.push_back(word);
        }
        int nLSPs=9; //LSPs and muOffsetZoD,muZSD
        vec lsp(nLSPs);
        for(int i=0;i<nLSPs;i++)
        {
            lsp(i) = stod(words[i]);
        }
        append(scmParams,lsp);
    }
    infile.close();
}

bool McellSystem::updateLocations(string locFileName)
{
    if(!IsFileExists(locFileName))
    {
        cout<<"locationInfo File not exist...Skipping Updation of linkInfo..."<<endl;return false;
    }
    if((getLineCountFromFile(locFileName)-1)!=nodeIDs.length())
    {
        cout<<"Number of nodes mismatch....Skipping Updation of linkInfo..."<<endl;return false;
    }
    
    ivec NodeIDs;
    Array<Location_S> newLocations(0);
    Array<Orientation_S> orients;Array<vec> vel;
    getLocations(locFileName,NodeIDs,newLocations,orients,vel);
    if(cellLayout==_CELLLAYOUT_HEXAGONAL_) 
    {
        int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);  
        ivec hexNodes=getNodes(aNodeTypes(hexDropNodeType));
        
        ivec hexNodeEntries(0);
        for(int cnt=0;cnt<hexNodes.length();cnt++)
            append(hexNodeEntries,find(NodeIDs,hexNodes(cnt),true));
        
        if(hexNodeEntries.length())
        {
            cout<<"[both:]Warning Hexagonal Nodes can't be moved. Skipping the entries."<<endl;
            deleteInVec(NodeIDs,hexNodeEntries);
            deleteInArray(newLocations,hexNodeEntries);
            deleteInArray(orients,hexNodeEntries);
            deleteInArray(vel,hexNodeEntries);
        }
    }
    if(NodeIDs.length() && currentTaskid==0)
        cout<<"[detl:]Updating Locations of Nodes : "<<NodeIDs<<endl;
    updateNodeLocations(NodeIDs,newLocations,orients,vel);
    return true;
}

// bool McellSystem::updateLinkInfo(string linkInfoFileName,string lspFileName)
// {
//     if(!is5GSCMScenario(sysTerrain.scmScenario)){cout<<"LinkInfo updation is cannot be done for non-5G scenarios"<<endl;return false;}
//     if(!IsFileExists(linkInfoFileName))
//     {
//         cout<<"LinkInfo file doesn't exist...skipping linkInfo updation..."<<endl;return false;
//     }
//     allocateLinkInfo();
//     
//     ivec serviceNodes = getNodes(_NODECATEGORY_SERVICENODE_),serverNodes = getNodes(_NODECATEGORY_SERVERNODE_);
//     int nLinks= serviceNodes.length()*serverNodes.length();
//     
//     //nodeID	serverNodeID	distance	srvrNodeTxAntennaGainInDB	srvcNodeRxAntennaGainInDB	isLOS	PathlossInDB	ShadowlossInDB	srvcNodeTxAntennaGainInDB	srvrNodeRxAntennaGainInDB	isLOS	PathlossInDB	ShadowlossInDB	isIndoor
//     ifstream linkInfoFile;
//     linkInfoFile.open(linkInfoFileName.c_str(),std::ios::in);
//     
//     if((getLineCountFromFile(linkInfoFileName)-1)!=nLinks)
//     {
//         cout<<"Number of links mismatch...skipping updation of linkInfo..."<<endl;
//         return false;
//     }
//     string nodeLinkInfo;
//     getline(linkInfoFile,nodeLinkInfo); //To Remove the first line(Header)
//     while(getline(linkInfoFile,nodeLinkInfo))
//     {
//         stringstream ss(nodeLinkInfo);
//         string word;
//         vector<string> words;
//         vec num(0);
//         int srvcID,srvrID;
//         while(getline(ss, word,',')) 
//         { 
//             words.push_back(word);
//         }
//         srvcID = stoi(words[0]);
//         srvrID = stoi(words[1]);
//         double dist = stod(words[2]);
//         int srvcIndx = find(nodeIDs,srvcID),srvrIndx = find(nodeIDs,srvrID);
//         
//         aNodes(srvrIndx).setWrapLoc(getWrapAroundLocation(srvcID,srvrID),1);
//         aNodes(srvcIndx).setWrapLoc(getWrapAroundLocation(srvrID,srvcID),1);
//         
//         NodeLinkInfo_S downlinkInfo = aNodes(srvcIndx).getLinkInfo(srvrID);
//         downlinkInfo.distance = dist;
//         downlinkInfo.txAntennaGain = stod(words[3]);
//         downlinkInfo.rxAntennaGain = stod(words[4]);
//         downlinkInfo.isLOS = stoi(words[5]);
//         downlinkInfo.pathLoss = stod(words[6]);
//         downlinkInfo.shadowLoss = stod(words[7]);
//         downlinkInfo.txLocation = aNodes(srvrIndx).getWrapLoc();
//         downlinkInfo.rxLocation = aNodes(srvcIndx).getLoc();
//         aNodes(srvcIndx).updateLinkInfo(downlinkInfo);
//         
//         NodeLinkInfo_S uplinkInfo = aNodes(srvrIndx).getLinkInfo(srvcID);
//         uplinkInfo.distance = dist;
//         uplinkInfo.txAntennaGain = stod(words[8]);
//         uplinkInfo.rxAntennaGain = stod(words[9]);
//         uplinkInfo.isLOS = stoi(words[10]);
//         uplinkInfo.pathLoss = stod(words[11]);
//         uplinkInfo.shadowLoss = stod(words[12]);
//         uplinkInfo.txLocation = aNodes(srvcIndx).getWrapLoc();
//         uplinkInfo.rxLocation = aNodes(srvrIndx).getLoc();
//         aNodes(srvrIndx).updateLinkInfo(uplinkInfo);
//     }
//     
//     linkInfoFile.close();
//     if(currentTaskid==0)
//         cout<<"[detl:]LinkInfo Updated from file."<<endl;
//     
//     if(IsFileExists(lspFileName) && (getLineCountFromFile(lspFileName)-1)==nLinks)
//     {
//         ifstream lspFile;
//         lspFile.open(lspFileName.c_str(),std::ios::in);
//         string lspInfo;
//         getline(lspFile,lspInfo); //To Remove the first line(Header)
//         while(getline(lspFile,lspInfo))
//         {
//             stringstream ss(lspInfo);
//             string word;
//             vector<string> words;
//             while(getline(ss, word,',')) 
//             { 
//                 words.push_back(word);
//             }
//             int txNode = stoi(words[0]),rxNode = stoi(words[1]);
//             
//             vec lsp = extractVector(words[2]);
//             
//             NodeLinkInfo_S dlLinkInfo = aNodes(find(nodeIDs,txNode)).getLinkInfo(rxNode);
//             dlLinkInfo.scmLargeScaleParams.load(lsp);
//             aNodes(find(nodeIDs,txNode)).updateLinkInfo(dlLinkInfo);
//             
//             NodeLinkInfo_S ulLinkInfo = aNodes(find(nodeIDs,rxNode)).getLinkInfo(txNode);
//             ulLinkInfo.scmLargeScaleParams.load(lsp);
//             aNodes(find(nodeIDs,rxNode)).updateLinkInfo(ulLinkInfo);
//         }
//         lspFile.close();
//         if(currentTaskid==0)
//             cout<<"[detl:]LSPs updated from the File."<<endl;
//     }
//     else
//     {
//         ivec serviceNodes = getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));
//         ivec serverNodes = getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));
//         
//         for(int serviceNode_cnt=0;serviceNode_cnt<length(serviceNodes);serviceNode_cnt++)//! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
//         {
//             for(int serverNode_cnt=0;serverNode_cnt<length(serverNodes);serverNode_cnt++)
//             {
//                 computeLSPsForLink(serverNodes(serverNode_cnt),serviceNodes(serviceNode_cnt),1);
//                 computeLSPsForLink(serverNodes(serverNode_cnt),serviceNodes(serviceNode_cnt),0);
//             }
//         }
//     }
//     
//     return true;
// }

int McellSystem::getLinkCount()
{
    return getNodes(_NODECATEGORY_SERVERNODE_).length()*getNodes(_NODECATEGORY_SERVICENODE_).length()*2;
}

// void McellSystem::computeLSPsForLink(int srvrNode,int srvcNode,int isDownlink)
// {
//     int refBSid = nSectorsPerSite*aNodes(srvrNode).getCellID();
//     int isInHNodeType = find(nodeDropMethods,_DROPMETHOD_INBUILDING_);
//     if(isInHNodeType==-1)
//         isInHNodeType = find(nodeDropMethods,_DROPMETHOD_INBUILDING3SEC_);
//     
//     Location_S txCellLoc,wrapTxLoc;
//     if(srvrNode == refBSid)
//     {
//         if(cellLayout == _CELLLAYOUT_HEXAGONAL_)
//         {
//             ivec hexNodes = getNodes(_DROPMETHOD_HEXAGONAL_);
//             for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)//! Get the location of hexagonal cell since it(servernode) is taken as transmitter from which we get transmitter cell ID//
//                 if(isDownlink==1)
//                 {
//                     if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(srvrNode).getCellID())//! If the hexnode cell ID is server node cell ID //
//                         txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();
//                 }
//                 else
//                 {
//                     if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(srvcNode).getCellID())//! If the hexnode cell ID is server node cell ID //
//                         txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();
//                 }
//                 
//                 if(aNodes(srvcNode).getCellID()!=aNodes(srvrNode).getCellID())//! If server node cell ID is not equal to service node cellID implies both are not in the same cell//
//                 {
//                     for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)//! For all hexnodes if some hexnode cellID which is equal to servicenode cellID there will be a wraparound of tx with respect to neighboring cells////
//                     {
//                         if(isDownlink==1)
//                         {
//                             if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(srvcNode).getCellID())
//                                 wrapTxLoc=WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(srvrNode).getCellID()),txCellLoc,aNodes(srvrNode).getLoc());
//                             aNodes(srvrNode).setWrapLoc(wrapTxLoc,true);//! Setting the wrap around location//
//                         }
//                         else if(isDownlink==0)
//                         {
//                             if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(srvrNode).getCellID())
//                                 wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(srvcNode).getCellID()),txCellLoc,aNodes(srvcNode).getLoc());
//                             aNodes(srvcNode).setWrapLoc(wrapTxLoc,true);
//                         }
//                     }
//                 }
//                 else   //! Else if server and service node are in the same cell there is no wraparound with respect to that cell wraparound will give the samecell so get the location of same cell //
//                 {
//                     if(isDownlink==1)
//                         aNodes(srvrNode).setWrapLoc(aNodes(srvrNode).getLoc(),true);
//                     else if(isDownlink==0)
//                         aNodes(srvcNode).setWrapLoc(aNodes(srvcNode).getLoc(),true);
//                 }
//         }
//         else 
//         {
//             if(isDownlink==1)
//             {
//                 txCellLoc=aNodes(srvrNode).getLoc();    
//                 aNodes(srvrNode).setWrapLoc(aNodes(srvrNode).getLoc(),true);
//             }
//             else if(isDownlink==0)
//             {
//                 txCellLoc=aNodes(srvcNode).getLoc();
//                 aNodes(srvcNode).setWrapLoc(aNodes(srvcNode).getLoc(),true);
//             }
//         }
//         
//         bool isOutdoorToIndoor;
//         if((aNodes(srvrNode).getLoc().isIndoor && !aNodes(srvcNode).getLoc().isIndoor) || (!aNodes(srvrNode).getLoc().isIndoor && aNodes(srvcNode).getLoc().isIndoor))
//             isOutdoorToIndoor=true;
//         else
//             isOutdoorToIndoor=false;
//         
//         NodeLinkInfo_S tempLinkInfo;
//         
//         Node *txNodePtr,*rxNodePtr;
//         
//         if(isDownlink==0)
//         {
//             txNodePtr = &aNodes(srvcNode),rxNodePtr = &aNodes(srvrNode);
//             tempLinkInfo = getLinkInfo(srvcNode,srvrNode);
//             tempLinkInfo.linkCarrierFrequencyInHz = rxNodePtr->getCarrierFreqInHz();
//         }
//         else
//         {
//             txNodePtr = &aNodes(srvrNode),rxNodePtr = &aNodes(srvcNode);
//             tempLinkInfo = getLinkInfo(srvrNode,srvcNode);
//             tempLinkInfo.linkCarrierFrequencyInHz = txNodePtr->getCarrierFreqInHz();
//         }
//         
//         if(is5GSCMScenario(sysTerrain.scmScenario) || isNTNSCMScenario(sysTerrain.scmScenario))
//         {
//             tempLinkInfo.txLocation = txNodePtr->getWrapLoc();
//             tempLinkInfo.rxLocation = rxNodePtr->getLoc();
//             Array<Array<Array<mat>>> gridsForSCM;
// 
// //             if(!sysTerrain.scmInfo.useSOSForSpCorr)
//             {
//                 gridsForSCM = (isDownlink==1) ? txNodePtr->scmGrids : (isDownlink==0) ? rxNodePtr->scmGrids : sysTerrain.commonSCMGrids;
//             }
//             int cellID = (isDownlink==1) ? txNodePtr->getCellID() : rxNodePtr->getCellID(); //FIXME cellID must be set properly in case of SideLinks. - Goraknath.
//             generate5GSCMLargeScaleParameters(tempLinkInfo,gridsForSCM,sysTerrain.scmInfo,sysTerrain.scmScenario,sysTerrain.infParams.scenario,sysTerrain.channelModelType,isOutdoorToIndoor,isDownlink,cellID);
//         }
//         else
//         {
//             if(rxNodePtr->scmGrids.size()!=0 && (txNodePtr->isHavingSCMGrid()==false))//Uplink 
//                 generateSCMLargeScaleParameters(tempLinkInfo,txNodePtr->scmGrids,rxNodePtr->scmPropagationScenario,sysTerrain.infParams.scenario,sysTerrain.scmInfo,txNodePtr->getLoc(), sysTerrain.channelModelType,isOutdoorToIndoor,0);
//             else if(rxNodePtr->scmGrids.size()==0 && (txNodePtr->isHavingSCMGrid()==true))//Downlink  
//             {
//                 generateSCMLargeScaleParameters(tempLinkInfo,txNodePtr->scmGrids,txNodePtr->scmPropagationScenario,sysTerrain.infParams.scenario,sysTerrain.scmInfo,rxNodePtr->getLoc(),sysTerrain.channelModelType,isOutdoorToIndoor,1);
//             }
//             /// CrossLinks (SideLink)
//             else if(rxNodePtr->scmGrids.size()!=0 && (txNodePtr->isHavingSCMGrid()==true)) //Server-Server
//                 generateSCMLargeScaleParameters(tempLinkInfo,txNodePtr->scmGrids,txNodePtr->scmPropagationScenario,sysTerrain.infParams.scenario,sysTerrain.scmInfo,rxNodePtr->getLoc(),sysTerrain.channelModelType,isOutdoorToIndoor);
//             else	//Service-Service
//                 generateSCMLargeScaleParameters(tempLinkInfo,sysTerrain.commonSCMGrids,txNodePtr->scmPropagationScenario,sysTerrain.infParams.scenario,sysTerrain.scmInfo,rxNodePtr->getLoc(),sysTerrain.channelModelType,isOutdoorToIndoor);
//         }
//         rxNodePtr->updateLinkInfo(tempLinkInfo);
//     }
//     else
//     {
//         if(isDownlink==1)
//             copyLinkInfo(srvcNode,srvrNode,aNodes(srvcNode).getLinkInfo(refBSid));
//         else 
//             copyLinkInfo(srvrNode,srvcNode,aNodes(refBSid).getLinkInfo(srvcNode));
//     }
// }

void McellSystem::updateNodeLocations(string filename,bool updateLinkInfo)
{
    ivec nodeIDs;
    Array<Location_S> newLocations(0);
    getNodeLocations(filename,nodeIDs,newLocations);
    
    if(cellLayout==_CELLLAYOUT_HEXAGONAL_) 
    {
      int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);  
      ivec hexNodes=getNodes(aNodeTypes(hexDropNodeType));
      
      ivec hexNodeEntries(0);
      for(int cnt=0;cnt<hexNodes.length();cnt++)
	append(hexNodeEntries,find(nodeIDs,hexNodes(cnt),true));
      
      if(hexNodeEntries.length())
      {
	cout<<"[both:]Warning Hexagonal Nodes can't be moved. Skipping the entries."<<endl;
	deleteInVec(nodeIDs,hexNodeEntries);
	deleteInArray(newLocations,hexNodeEntries);
      }
    }
    if(nodeIDs.length())
      cout<<"[detl:]Updating Locations of Nodes : "<<nodeIDs<<endl;
    if(updateLinkInfo)
      for(int node_cnt=0;node_cnt<nodeIDs.length();node_cnt++)
      {
        moveNode(nodeIDs(node_cnt),newLocations(node_cnt));
        generateLinkInfo(nodeIDs(node_cnt),false,false);
      }
   else
     moveNode(nodeIDs,newLocations);
}

void McellSystem::moveNodesToProximity(int proximityNodeType,bool updateLinkInfo)
{
    ivec baseNodes=getNodes(_DROPMETHOD_RANDOMHEX_,true);
    Array<Location_S> centreLocs = getNodeLocation(baseNodes);
    ivec proximityNodes=getNodes(proximityNodeType);
    
    if(proximityNodes.length())
      cout<<"[detl:]Updating Locations of Nodes : "<<proximityNodes<<endl;

    for(int node_cnt=0;node_cnt<proximityNodes.length();node_cnt++)
    {
        
    int randIndx = randPick(getIntegers(0,baseNodes.length()-1),1)(0);  
    Location_S newLoc = generateUniformDropLocationInCircle(centreLocs(randIndx),100,0)(0);
    vec rndHeights =to_vec(getIntegers(0,sysTerrain.getMaximumFloors()-1)*sysTerrain.getRoomSize().height)+1.5;
    newLoc.z=randPick(rndHeights,1)(0);
    moveNode(proximityNodes(node_cnt),newLoc);
    if(updateLinkInfo)
        generateLinkInfo(proximityNodes(node_cnt),false,false);
    }
}



/*!\fn addNode() 
 * \brief --addition of newnodes depends upon ther type into the system when nodetype and thier number are given
 * \param[in]--> nodetype of newnode is given
 * \param[in]--> number of nodes of particular type is given
 * \return --> the newnode ID which is added to the system is returned
 */
ivec McellSystem::addNode(string nodeType,int numNodes){

  ivec newNodes;
  newNodes.set_size(0);
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
	if(nodeDropMethods(getNodeTypeCnt(nodeType))==_DROPMETHOD_HEXAGONAL_)
	{
	  cout<<"[both:]Error. Cannot add a new Hegonal Node into the Hexagonal Cell Layout.";
	  abort();
	}
	else
	{
	  int cellCnt;

	  if(nNodes(getNodeTypeCnt(nodeType))==7 || nNodes(getNodeTypeCnt(nodeType))==21) cellCnt=7;
	  else if(nNodes(getNodeTypeCnt(nodeType))==19 || nNodes(getNodeTypeCnt(nodeType))==57) cellCnt=19;
	  else
		cellCnt=1;
	  for(int node_cnt=0;node_cnt<numNodes;node_cnt++)
	    append(newNodes, addNode(randi(0,cellCnt),nodeType) );
	}
  else
    for(int node_cnt=0;node_cnt<numNodes;node_cnt++)
      append(newNodes, addNode(nodeType,(generateUniformDropLocation(coverageArea)))(0));

  return(newNodes);

}

int McellSystem::addNode(string nodeType ,double xLoc,double yLoc, double nodeHeight){
/*!\fn addNode --> addition of newnodes depends upon ther type into the system when the node location and height are given
 * \param[in] nodeType--> nodetype of newnode is given
 * \param[in] xLoc,yLoc--> x andy coordinate of the newnode is given
 * \param[in] nodeHeight -->nodeheight is given as the input
 * \return  addNode --> it returns the newnodes ID added to the system 
 */

  Location_S loc;
  loc.x=xLoc;
  loc.y=yLoc;
  loc.z=nodeHeight;
  return(addNode(nodeType,loc));
}

int McellSystem::addNode(string nodeType,Location_S nodeLoc){

  if(nodeDropMethods(getNodeTypeCnt(nodeType))==_DROPMETHOD_HEXAGONAL_)
  {
    cout<<"[both:]Error. Cannot add a new Hegonal Node into the Hexagonal Cell Layout.";
    abort();
  }
  int nodeTypeCnt=findInStringArray(aNodeTypes,nodeType);
  if(nodeTypeCnt!=-1)
  {
    Node newNode;
	newNode.setLoc(nodeLoc);
	newNode.setID(aNodes.length());
	newNode.setType(nodeTypeCnt);
	
    if(nodeCategoryPerNodeType(nodeTypeCnt)==_NODECATEGORY_SERVERNODE_)
    {
        ivec srvrNodeTypes = find(nodeCategoryPerNodeType,_NODECATEGORY_SERVERNODE_,true);
        int srvrNodeIndx = find(srvrNodeTypes,nodeTypeCnt);
        newNode.setCarrierFreqInHz(carrierFreqInMHzPerServerNodeType(srvrNodeIndx)*1e6);
    }
	nNodes(nodeTypeCnt)++;
	append(aNodes,newNode);
	if(cellLayout==_CELLLAYOUT_HEXAGONAL_) {aNodes(newNode.getID()).setHexagonalCellID(findHexagonalCellID(newNode.getID()));}
	return(newNode.getID());
  }
  else {cout<<"[both:]Undefined Node Name to addNode().";abort();}
}

ivec McellSystem::addNode(string nodeType ,Array<Location_S> nodeLocs){

  ivec nodeIDs;
  for(int node_cnt=0;node_cnt<nodeLocs.length();node_cnt++)
    append(nodeIDs,addNode(nodeType,nodeLocs(node_cnt)));

  return(nodeIDs);

}

int McellSystem::addNode(int hexCellID,string nodeType){

  return(addNode(nodeType,(generateUniformDropLocationInHexCell(aNodes(hexCellID).getLoc(),cellISD/sqrt(3),minDist)))(0));
}

int McellSystem::addNode(Node newNode){

  newNode.setID(aNodes.length(),true);
  nNodes(newNode.getType())++;
  append(aNodes,newNode);
  aNodes(newNode.getID()).setHexagonalCellID(findHexagonalCellID(newNode.getID()));
  return(newNode.getID());
}

void McellSystem::createNodeType(string newNodeType){

  int nodeTypecnt=findInStringArray(aNodeTypes,newNodeType);
  if(nodeTypecnt!=-1)
  {
    cout<<"[detl:]Given nodeType to createEmptyNodeType() already available in the McellSystem."<<endl;
    return;
  }
  else
  {
    append(aNodeTypes,newNodeType);
    append(nNodes,0);
    append(nodeTypeHeights,-1.0);
   // append(nodeCategoryPerNodeType,_NODECATEGORY_UNINIT_);
    append(noiseFigurePerNodeType,0.0);
    append(indoorNodePercentage,0);
    append(nodeDropMethods,-1);
  }
}

void McellSystem::setNodeCategoryForSecondNetwork(string nodeType){

    int indxForNodeCategory=findInStringArray(aNodeTypes,nodeType);
    if(nodeCategoryPerNodeType(indxForNodeCategory)==_NODECATEGORY_SERVERNODE_)
      append(nodeCategoryPerNodeType,_NODECATEGORY_SERVERNODE_);
    else
      append(nodeCategoryPerNodeType,_NODECATEGORY_SERVICENODE_);
}

void McellSystem::setNetworkIDPerNodeType(int newNodesToCreate){

  append(networkIDPerNodeType,ones_i(newNodesToCreate)*2);//! getting second network-ID, i.e.,2
}

/*
int McellSystem::duplicateNode(int nodeID,string nodeType){

  Node newNode=aNodes(nodeID);
  if(nodeType!="")
  {
    int nodeTypecnt=findInStringArray(aNodeTypes,nodeType);
   if(nodeTypecnt!=-1)
    newNode.setType(nodeTypecnt);
   else
   {
     cout<<"[detl:]Given nodeType to duplicateNode() not available in the McellSystem. Skipping duplication of node."<<endl;
     return(-1);
   }
  }
  return(addNode(newNode));
}

ivec McellSystem::duplicateNodes(ivec nodeIDs,string nodeType){

  ivec toReturn(nodeIDs.length());
  for(int node_cnt=0;node_cnt<nodeIDs.length();node_cnt++)
    toReturn(node_cnt)=duplicateNode(nodeIDs(node_cnt),nodeType);
  return(toReturn);
}
*/
int McellSystem::duplicateNode(int nodeID,string nodeType,bool newNodeType){

  Node newNode=aNodes(nodeID);
  if(nodeType!="")
  {
    int nodeTypecnt=findInStringArray(aNodeTypes,nodeType);
    if(nodeTypecnt!=-1)
      newNode.setType(nodeTypecnt);
    else
    {
      if(newNodeType)
	createNodeType(nodeType);
      else
      {
	cout<<"[detl:]Given nodeType to duplicateNode() not available in the McellSystem. Skipping duplication of node."<<endl;
	return(-1);
      }
    }
  }
  return(addNode(newNode));
}


ivec McellSystem::duplicateNodes(ivec nodeIDs,string nodeType,bool newNodeType){

  ivec toReturn(nodeIDs.length());
  for(int node_cnt=0;node_cnt<nodeIDs.length();node_cnt++)
    toReturn(node_cnt)=duplicateNode(nodeIDs(node_cnt),nodeType,newNodeType);
  return(toReturn);
}

void McellSystem::activateNodes(){

  for(int node_cnt=0;node_cnt<sum(nNodes);node_cnt++)
	aNodes(node_cnt).setActive();
}

void McellSystem::activateNodes(ivec nodeList){

  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
    if(nodeList(node_cnt)<sum(nNodes))
	aNodes(nodeList(node_cnt)).setActive();
}

void McellSystem::deactivateNodes(ivec nodeList){

  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
    if(nodeList(node_cnt)<sum(nNodes))
  	aNodes(nodeList(node_cnt)).setInactive();
}

void McellSystem::moveNode(int nodeID, double xLoc, double yLoc){

    if(abs(xLoc)<=coverageArea.xLen/2 && abs(yLoc)<=coverageArea.yLen/2)  
    {
        aNodes(nodeID).setXYLoc(xLoc,yLoc,true);
        if(find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_)!=-1) // 1=> Hexagonal Drop
            aNodes(nodeID).setHexagonalCellID(findHexagonalCellID(nodeID));
    }
    else
    {
        cout<<"Location out of coverage ... "<<endl;abort();
    }
}

void McellSystem::moveNode(int nodeID, Location_S newLoc){

  if(isInCoverage(newLoc))
  {
    aNodes(nodeID).setXYLoc(newLoc.x,newLoc.y,true);
    aNodes(nodeID).setHeight(newLoc.z,true);
    if(find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_)!=-1) // 1=> Hexagonal Drop
        aNodes(nodeID).setHexagonalCellID(findHexagonalCellID(nodeID));
  }
  else
  {
      cout<<"Location out of coverage ... "<<endl;abort();
  }
}

void McellSystem::moveNode(ivec nodeList,Array<Location_S> newLocs){

  if(newLocs.length()<nodeList.length())
  {
	cout<<"[both:]Error. Insufficient location count for moving nodes in moveNode()."<<endl;
	abort();
  }
  else if(newLocs.length()>nodeList.length()) cout<<"[detl:]Warning excess location count for moveNode(). Omitting extras."<<endl;

  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
	  moveNode(nodeList(node_cnt),newLocs(node_cnt));
}

void McellSystem::shiftNode(int nodeID, double xOffset, double yOffset){

  Location_S newLoc=aNodes(nodeID).getLoc();
  newLoc.x+=xOffset;newLoc.y+=yOffset;
  moveNode(nodeID,newLoc);
}

void McellSystem::shiftNode(int nodeID, Location_S offset){

  Location_S newLoc=aNodes(nodeID).getLoc();
  newLoc.x+=offset.x;newLoc.y+=offset.y;
  moveNode(nodeID,newLoc);

}

void McellSystem::shiftNode(ivec nodeList,Location_S offset){

  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
    shiftNode(nodeList(node_cnt),offset);
}

void McellSystem::shiftNode(ivec nodeList,Array<Location_S> offsets){

  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
    shiftNode(nodeList(node_cnt),offsets(node_cnt));
}

void McellSystem::moveServiceNodeAlone(int nodeID, double distance)
{
 Location_S newLoc;
  int indx=find(mobilityInfo.serviceNodes,nodeID);
  newLoc=aNodes(nodeID).getLoc();
  newLoc.x+=distance*cos(2*pi*mobilityInfo.directionPerServiceNode(indx));
  newLoc.y+=distance*sin(2*pi*mobilityInfo.directionPerServiceNode(indx));
  
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
  {
    ivec hexNodes;
    int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
    hexNodes=getNodes(aNodeTypes(hexDropNodeType));
    double nativeDistance=-1.0;
    int nativeIndx=-1;
    vec distances(hexNodes.length());
    Array<Location_S> wrapLocations(hexNodes.length());

    for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
      if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(nodeID).getCellID())
      {
	nativeDistance=findDistance(newLoc,aNodes(hexNodes(hexNode_cnt)).getLoc());
	nativeIndx=hexNode_cnt;
	break;
      }

      for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
	if(aNodes(hexNodes(hexNode_cnt)).getCellID()!=aNodes(nodeID).getCellID())
	  distances(hexNode_cnt)=findDistance(newLoc,aNodes(nativeIndx).getNeighbourCellLoc(aNodes(hexNodes(hexNode_cnt)).getCellID()));
	else
	  distances(hexNode_cnt)=nativeDistance;

      int minIndx=min_index(distances);

      if(minIndx!=nativeIndx) 
      {
	Location_S nearestCellPhyLoc=aNodes(hexNodes(minIndx)).getLoc();
	Location_S nearestCellWrapLoc=aNodes(nativeIndx).getNeighbourCellLoc(aNodes(hexNodes(minIndx)).getCellID());
	newLoc.x=nearestCellPhyLoc.x + (newLoc.x-nearestCellWrapLoc.x);
	newLoc.y=nearestCellPhyLoc.y + (newLoc.y-nearestCellWrapLoc.y);
      }
    
  }
  else
  {
    if(newLoc.x>=0){ if(newLoc.x>coverageArea.xLen/2.0) newLoc.x-=coverageArea.xLen;}
    else	{if(newLoc.x<(-coverageArea.xLen/2.0)) newLoc.x+=coverageArea.xLen;}

    if(newLoc.y>=0) {if(newLoc.y>coverageArea.yLen/2.0) newLoc.y-=coverageArea.yLen;}
    else	{if(newLoc.y<(-coverageArea.yLen/2.0)) newLoc.y+=coverageArea.yLen;}
  }
  moveNode(nodeID,newLoc); 
}

Location_S McellSystem::dropInRandomLocation()
{
 Location_S loc; 
 if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
 {
  ivec hexNodes;
    int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
    hexNodes=getNodes(aNodeTypes(hexDropNodeType));
    int hexNodeCount=hexNodes.length();
    int rndNodeIndx=randi(0,hexNodeCount-1);
    int cellId=aNodes(hexNodes(rndNodeIndx)).getCellID();
    int secid=randi(0,nSectorsPerSite-1);
 
    loc=generateUniformDropLocationInHexCell(aNodes(cellId).getLoc(),nSectorsPerSite, secid, cellISD/sqrt(3),minDist)(0); 
 }
 else
 {
   
 }
 return loc;
}

ChannelGainInfo_S McellSystem::moveServiceNode(int nodeID, double currentTimeInSec, double distance)
{
  Location_S newLoc;
  int indx=find(mobilityInfo.serviceNodes,nodeID);
  mobilityInfo.pastTimePerServiceNode(indx)=currentTimeInSec;
  newLoc=aNodes(nodeID).getLoc();
  if(distance==-1)
  {
   newLoc=dropInRandomLocation();
   mobilityInfo.directionPerServiceNode(indx)=randu()*360;
  }
  else
  {    
   newLoc.x+=distance*cos(2*pi*mobilityInfo.directionPerServiceNode(indx));
   newLoc.y+=distance*sin(2*pi*mobilityInfo.directionPerServiceNode(indx));
  }
  
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
  {
      ivec hexNodes;
      int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
      hexNodes=getNodes(aNodeTypes(hexDropNodeType));
      double nativeDistance=-1.0;
      int nativeIndx=-1;
      vec distances(hexNodes.length());
      Array<Location_S> wrapLocations(hexNodes.length());
      
      for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
          if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(nodeID).getCellID())
          {
              nativeDistance=findDistance(newLoc,aNodes(hexNodes(hexNode_cnt)).getLoc());
              nativeIndx=hexNode_cnt;
              break;
          }
          
          for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
              if(aNodes(hexNodes(hexNode_cnt)).getCellID()!=aNodes(nodeID).getCellID())
                  distances(hexNode_cnt)=findDistance(newLoc,aNodes(nativeIndx).getNeighbourCellLoc(aNodes(hexNodes(hexNode_cnt)).getCellID()));
              else
                  distances(hexNode_cnt)=nativeDistance;
              
          int minIndx=min_index(distances);
          
          if(minIndx!=nativeIndx) 
          {
              Location_S nearestCellPhyLoc=aNodes(hexNodes(minIndx)).getLoc();
              Location_S nearestCellWrapLoc=aNodes(nativeIndx).getNeighbourCellLoc(aNodes(hexNodes(minIndx)).getCellID());
              newLoc.x=nearestCellPhyLoc.x + (newLoc.x-nearestCellWrapLoc.x);
              newLoc.y=nearestCellPhyLoc.y + (newLoc.y-nearestCellWrapLoc.y);
          }
    
  }
  else
  {
    if(newLoc.x>=0){if(newLoc.x>coverageArea.xLen/2.0) newLoc.x-=coverageArea.xLen;}
    else	{if(newLoc.x<(-coverageArea.xLen/2.0)) newLoc.x+=coverageArea.xLen;}

    if(newLoc.y>=0) {if(newLoc.y>coverageArea.yLen/2.0) newLoc.y-=coverageArea.yLen;}
    else	{if(newLoc.y<(-coverageArea.yLen/2.0)) newLoc.y+=coverageArea.yLen;}
  }
  moveNode(nodeID,newLoc);
  generateLinkInfo(nodeID,false,false);
  ChannelGainInfo_S updatedChannelGainsInfo;
  if(aNodes(nodeID).getCategory()==_NODECATEGORY_SERVICENODE_)
  {
    for(int link_cnt=0;link_cnt<aNodes(nodeID).getLinkCount();link_cnt++)
    {
      NodeLinkInfo_S linkInfoDl=aNodes(nodeID).getLinkInfoByIndex(link_cnt);
      append(updatedChannelGainsInfo.linkedNodeIDs,linkInfoDl.id);
      append(updatedChannelGainsInfo.downlinkGainsInDB,(double)(linkInfoDl.txAntennaGain+linkInfoDl.rxAntennaGain-linkInfoDl.pathLoss-linkInfoDl.shadowLoss));
      NodeLinkInfo_S linkInfoUl=aNodes(linkInfoDl.id).getLinkInfo(nodeID);
      append(updatedChannelGainsInfo.uplinkGainsInDB,(double)(linkInfoUl.txAntennaGain+linkInfoUl.rxAntennaGain-linkInfoUl.pathLoss-linkInfoUl.shadowLoss));
    }
  }
  else
  {
    cout<<"[both:] ServerNode can't be moved as of now in mcell .. "<<endl;abort();
  }
  return updatedChannelGainsInfo;
}
//Random Walk Model
ChannelGainInfo_S McellSystem::moveServiceNodeForMobility(int nodeID, double currentTimeInSec){

  Location_S newLoc;
  if(mobilityInfo.mobilityType ==_MOBILITY_TYPE_RND_DIR_){

    int indx=find(mobilityInfo.serviceNodes,nodeID);
    if(indx!=-1)
    {
      double distance=aNodes(nodeID).getVelocity()*(currentTimeInSec-mobilityInfo.pastTimePerServiceNode(indx));
      mobilityInfo.pastTimePerServiceNode(indx)=currentTimeInSec;
      newLoc=aNodes(nodeID).getLoc();
      newLoc.x+=distance*cos(2*pi*mobilityInfo.directionPerServiceNode(indx));
      newLoc.y+=distance*sin(2*pi*mobilityInfo.directionPerServiceNode(indx));
    }
    else
    {
      cout<<"[both:]Unknown nodeID to walkNode()."<<endl;abort();
    }
  }
  else if(mobilityInfo.mobilityType==_MOBILITY_TYPE_RND_WALK_){

    int indx=find(mobilityInfo.serviceNodes,nodeID);
    if(indx!=-1)
    {
        double distance=aNodes(nodeID).getVelocity()*(currentTimeInSec-mobilityInfo.pastTimePerServiceNode(indx));
        
        do
        {
          newLoc=aNodes(nodeID).getLoc();
        
          newLoc.x+=distance*cos(2*pi*mobilityInfo.directionPerServiceNode(indx));
          newLoc.y+=distance*sin(2*pi*mobilityInfo.directionPerServiceNode(indx));
      
          if(findHexagonalCellID(newLoc)!=aNodes(nodeID).getCellID())
              reinitMobilityDirection(nodeID);
          
        }while(findHexagonalCellID(newLoc)!=aNodes(nodeID).getCellID());
      
        mobilityInfo.distanceMovedPerServiceNode(indx) = mobilityInfo.distanceMovedPerServiceNode(indx) + distance;
        if(mobilityInfo.distanceMovedPerServiceNode(indx) > mobilityInfo.refreshDistanceForRandomWalk)
            reinitMobilityDirection(nodeID);
      
      mobilityInfo.pastTimePerServiceNode(indx)=currentTimeInSec;
      
    }
    else
    {
      cout<<"[both:]Unknown nodeID to walkNode()."<<endl; abort();
    }
  }

  if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
  {
      ivec hexNodes;
      int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
      hexNodes=getNodes(aNodeTypes(hexDropNodeType));
      double nativeDistance = -1.0;
      int nativeIndx = -1;
      vec distances(hexNodes.length());
      Array<Location_S> wrapLocations(hexNodes.length());
      
      if(length(hexNodes)>1)
      {
          for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
              if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(nodeID).getCellID())
              {
                  nativeDistance=findDistance(newLoc,aNodes(hexNodes(hexNode_cnt)).getLoc());
                  nativeIndx=hexNode_cnt;
                  break;
              }
              
              for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
                  if(aNodes(hexNodes(hexNode_cnt)).getCellID()!=aNodes(nodeID).getCellID())
                      distances(hexNode_cnt)=findDistance(newLoc,aNodes(nativeIndx).getNeighbourCellLoc(aNodes(hexNodes(hexNode_cnt)).getCellID()));
                  else
                      distances(hexNode_cnt)=nativeDistance;
                  
              int minIndx=min_index(distances);
              
              if(minIndx!=nativeIndx) 
              {
                  Location_S nearestCellPhyLoc=aNodes(hexNodes(minIndx)).getLoc();
                  Location_S nearestCellWrapLoc=aNodes(nativeIndx).getNeighbourCellLoc(aNodes(hexNodes(minIndx)).getCellID());
                  newLoc.x=nearestCellPhyLoc.x + (newLoc.x-nearestCellWrapLoc.x);
                  newLoc.y=nearestCellPhyLoc.y + (newLoc.y-nearestCellWrapLoc.y);
              }
      }
      else
      {
          if(!isInsideHexCell(newLoc.x,newLoc.y,aNodes(hexNodes(0)).getLoc(),cellISD))
          {
              if(abs(newLoc.x)>cellISD/2.0) // exceeded along x axis
              {
                  if(newLoc.x>cellISD/2.0) 
                      newLoc.x=newLoc.x-cellISD;
                  else 
                  {
                      if(newLoc.x<(-cellISD/2.0)) 
                          newLoc.x=newLoc.x+cellISD;
                  }
              }
              
              if(abs(newLoc.y)>(cellISD-abs(newLoc.x))/sqrt(3)) // exceeded along y axis
              {
                  if(newLoc.y>(cellISD-abs(newLoc.x))/sqrt(3)) newLoc.y=-newLoc.y+2.0*(abs(newLoc.y)-(cellISD-abs(newLoc.x))/sqrt(3));
                  else
                  { 
                      if(newLoc.y<-(cellISD-abs(newLoc.x))/sqrt(3)) newLoc.y=-newLoc.y-2.0*(abs(newLoc.y)-(cellISD-abs(newLoc.x))/sqrt(3));
                  }
              }
          }
      }
  }
  else
  {
      if(newLoc.x>=0) {if(newLoc.x>coverageArea.xLen/2.0) newLoc.x-=coverageArea.xLen;}
      else	{if(newLoc.x<(-coverageArea.xLen/2.0)) newLoc.x+=coverageArea.xLen;}
      
      if(newLoc.y>=0) {if(newLoc.y>coverageArea.yLen/2.0) newLoc.y-=coverageArea.yLen;}
      else	{if(newLoc.y<(-coverageArea.yLen/2.0)) newLoc.y+=coverageArea.yLen;}
  }
  cout<<"\n\nMoving node to new loc : "<<newLoc<<endl;
  moveNode(nodeID,newLoc);
  generateLinkInfo(nodeID,false,false);
  ChannelGainInfo_S updatedChannelGainsInfo;
  if(aNodes(nodeID).getCategory()==_NODECATEGORY_SERVICENODE_)
  {
    for(int link_cnt=0;link_cnt<aNodes(nodeID).getLinkCount();link_cnt++)
    {
      NodeLinkInfo_S linkInfoDl=aNodes(nodeID).getLinkInfoByIndex(link_cnt);
      append(updatedChannelGainsInfo.linkedNodeIDs,linkInfoDl.id);
      append(updatedChannelGainsInfo.isLOS,linkInfoDl.isLOS);
      append(updatedChannelGainsInfo.downlinkGainsInDB,(double)(linkInfoDl.txAntennaGain+linkInfoDl.rxAntennaGain-linkInfoDl.pathLoss-linkInfoDl.shadowLoss));
      NodeLinkInfo_S linkInfoUl=aNodes(linkInfoDl.id).getLinkInfo(nodeID);
      append(updatedChannelGainsInfo.uplinkGainsInDB,(double)(linkInfoUl.txAntennaGain+linkInfoUl.rxAntennaGain-linkInfoUl.pathLoss-linkInfoUl.shadowLoss));
    }
    Array<vec> linkGains(2);
    linkGains(0)=updatedChannelGainsInfo.downlinkGainsInDB;
    linkGains(1)=updatedChannelGainsInfo.uplinkGainsInDB;
    channelCloud.updateLinkGainInfo(nodeID,updatedChannelGainsInfo.linkedNodeIDs,linkGains);  
  }
  else
  {
    cout<<"[both:] ServerNode can't be moved as of now in mcell .. "<<endl;abort();
  }
  return updatedChannelGainsInfo;
}

double McellSystem::getMovableDistance(int serviceNodeID, double currentTimeInSec){

  int indx=find(mobilityInfo.serviceNodes,serviceNodeID);
  if(indx!=-1)
  {
    return(aNodes(serviceNodeID).getVelocity()*(currentTimeInSec-mobilityInfo.pastTimePerServiceNode(indx)));
  }
  else
  {
    cout<<"[detl:]Unknown serviceNodeID to getMovableDistance()."<<endl;
    return(-1);
  }
}
double McellSystem::getDeltaT(int serviceNodeID, double currentTimeInSec){
    
    int indx=find(mobilityInfo.serviceNodes,serviceNodeID);
    if(indx!=-1)
    {
        return((currentTimeInSec-mobilityInfo.pastTimePerServiceNode(indx)));
    }
    else
    {
        cout<<"[detl:]Unknown serviceNodeID to getMovableDistance()."<<endl;
        return(-1);
    }
}
ivec McellSystem::getNodes(NodeCategory_E nodeCategory){

  ivec output(0),temp;
  for(int type_cnt=0;type_cnt<nNodes.length();type_cnt++)
    if(nNodes(type_cnt)!=0)
    {
      temp=getNodes(type_cnt);
      if(aNodes(temp(0)).getCategory()==nodeCategory)
	append(output,temp);
    }
  return(output);
}

ivec McellSystem::getNodes(DropMethod_E nodeDropMethod, bool all){

  if(all)
  {
    ivec dropIndices =find(nodeDropMethods,nodeDropMethod,true);
    ivec nodesToReturn(0);
    for(int cnt=0;cnt<dropIndices.length();cnt++)
        append(nodesToReturn,getNodes(aNodeTypes(dropIndices(cnt))));
    
    if(nodesToReturn.length()==0)
        cout<<"[both:]No node with drop method '"<<nodeDropMethod<<"' is available to getNodes().";
    
    return(nodesToReturn);
  }
  else
  {
    int dropIndx =find(nodeDropMethods,nodeDropMethod);
    if(dropIndx != -1)
        return(getNodes(aNodeTypes(dropIndx)));
    else {cout<<"[both:]No node with drop method '"<<nodeDropMethod<<"' is available to getNodes().";return(ivec(""));}
  }
}
ivec McellSystem::getNodes(string nodeType){
  
    int nodeTypeCnt=findInStringArray(aNodeTypes,nodeType);
    if(nodeTypeCnt!=-1)
        return(getNodes(nodeTypeCnt));
    else {cout<<"[both:]Undefined Node Name '"<<nodeType<<"' to getNodes().";abort();}
  
}

ivec McellSystem::getNodes(int nodeTypeCnt){

  if(nodeTypeCnt<aNodeTypes.length())
  {
	ivec nodeList;
	nodeList.set_size(0);

	if(aNodes.length()==0) {cout<<"[both:]Initialize nodes by calling initSys() before trying to access them()."<<endl;abort();}

	for(int node_cnt=0;node_cnt<sum(nNodes);node_cnt++)
	  if((aNodes(node_cnt).getType())==nodeTypeCnt)
	    append(nodeList,node_cnt);
	return(nodeList);
  }
  else{
	cout<<"[detl:]Warning. Invalid nodeTypeCnt for getNodes()."<<endl;abort();}

}

ivec McellSystem::getNodes(int nodeID, double radius){

  ivec nodeList;
  nodeList.set_size(0);
  for(int node_cnt=0;node_cnt<sum(nNodes);node_cnt++)
	if(findDistance(aNodes(nodeID).getLoc(),aNodes(node_cnt).getLoc())<=radius)
	  if(nodeID!=node_cnt)
	    append(nodeList,node_cnt);
  if(length(nodeList)==0) {cout<<"[detl:]Unable to find any node in the given radius around the given Node ID in getNodes()."<<endl;}
  return(nodeList);
}

ivec McellSystem::getNodes(double xLoc, double yLoc, double radius){

  Location_S loc;
  loc.x=xLoc;
  loc.y=yLoc;
  loc.z=0;
  ivec nodeList;
  double distance;
  nodeList.set_size(0);
  for(int node_cnt=0;node_cnt<sum(nNodes);node_cnt++)
  {
	distance=findDistance(loc,aNodes(node_cnt).getLoc());
	if(distance<=radius && distance>0)
	  append(nodeList,node_cnt);
  }
  if(length(nodeList)==0) {cout<<"[detl:]Unable to find any node in the given radius around the given location in getNodes()."<<endl;}
  return(nodeList);
}

ivec McellSystem::getActiveNodes(){

  ivec nodeList;
  nodeList.set_size(0);
  for(int node_cnt=0;node_cnt<sum(nNodes);node_cnt++)
	if(aNodes(node_cnt).isActive())
	  append(nodeList,node_cnt);
  return(nodeList);
}

ivec McellSystem::getIndoorNodes(){

  ivec nodeList;
  nodeList.set_size(0);
  for(int node_cnt=0;node_cnt<sum(nNodes);node_cnt++)
	if(aNodes(node_cnt).isIndoor())
	  append(nodeList,node_cnt);
	if(length(nodeList)==0) {cout<<"[detl:]Warning. Unable to find any Indoor node in getIndoorNodes()."<<endl;}
  return(nodeList);
}

ivec McellSystem::getOutdoorNodes(){

  ivec nodeList;
  nodeList.set_size(0);
  for(int node_cnt=0;node_cnt<sum(nNodes);node_cnt++)
	if(aNodes(node_cnt).isOutdoor())
	  append(nodeList,node_cnt);
	if(length(nodeList)==0) {cout<<"[detl:]Warning. Unable to find any Outdoor node in getOutdoorNodes()."<<endl;}
  return(nodeList);
}

ivec McellSystem::getActiveNodes(string nodeType){

  ivec nodeList;
  nodeList.set_size(0);
  ivec tempNodes=getNodes(nodeType);
  for(int node_cnt=0;node_cnt<length(tempNodes);node_cnt++)
	if(aNodes(tempNodes(node_cnt)).isActive())
	  append(nodeList,tempNodes(node_cnt));
  return(nodeList);

}

ivec McellSystem::getIndoorNodes(string nodeType){

  ivec nodeList;
  nodeList.set_size(0);
  ivec tempNodes=getNodes(nodeType);
  for(int node_cnt=0;node_cnt<length(tempNodes);node_cnt++)
	if(aNodes(tempNodes(node_cnt)).isIndoor())
	  append(nodeList,node_cnt);
	if(length(nodeList)==0) {cout<<"[detl:]Warning. Unable to find any Indoor node in node Type."<<nodeType<<endl;}
	return(nodeList);
}

ivec McellSystem::getOutdoorNodes(string nodeType){

  ivec nodeList;
  nodeList.set_size(0);
  ivec tempNodes=getNodes(nodeType);
  for(int node_cnt=0;node_cnt<length(tempNodes);node_cnt++)
	if(aNodes(tempNodes(node_cnt)).isOutdoor())
	  append(nodeList,node_cnt);
	if(length(nodeList)==0) {cout<<"[detl:]Warning. Unable to find any Indoor node in node Type."<<nodeType<<endl;}
	return(nodeList);
}


ivec McellSystem::getActiveNodesFromList(ivec tempNodes){

  ivec nodeList;
  nodeList.set_size(0);
  for(int node_cnt=0;node_cnt<length(tempNodes);node_cnt++)
	if(aNodes(tempNodes(node_cnt)).isActive())
	  append(nodeList,tempNodes(node_cnt));
	if(length(nodeList)==0) {cout<<"[detl:]Unable to find any active node in getActiveNodes()."<<endl;}
	return(nodeList);
}

ivec McellSystem::getServerNodesFromList(ivec tempNodes){

  ivec nodeList;
  nodeList.set_size(0);
  for(int node_cnt=0;node_cnt<length(tempNodes);node_cnt++)
    if(aNodes(tempNodes(node_cnt)).getCategory()==_NODECATEGORY_SERVERNODE_)
      append(nodeList,tempNodes(node_cnt));
    if(length(nodeList)==0) {cout<<"[detl:]Unable to find any server node in getServerNodesFromList()."<<endl;}
    return(nodeList);
}

ivec McellSystem::getServiceNodesFromList(ivec tempNodes){

  ivec nodeList;
  nodeList.set_size(0);
  for(int node_cnt=0;node_cnt<length(tempNodes);node_cnt++)
    if(aNodes(tempNodes(node_cnt)).getCategory()==_NODECATEGORY_SERVICENODE_)
      append(nodeList,tempNodes(node_cnt));
    if(length(nodeList)==0) {cout<<"[detl:]Unable to find any service node in getServiceNodesFromList()."<<endl;}
    return(nodeList);
}

int McellSystem::getNearestNode(int nodeID, ivec nodeList){

  int nearestNode=nodeList(0);
  double nearestNodeDistance=findDistance(aNodes(nodeID).getLoc(),aNodes(nearestNode).getLoc());
  double newNodeDistance;
  for(int node_cnt=1;node_cnt<length(nodeList);node_cnt++)
  {
    newNodeDistance=findDistance(aNodes(nodeID).getLoc(),aNodes(nodeList(node_cnt)).getLoc());
    if(nearestNodeDistance>newNodeDistance)
    {
      nearestNode=nodeList(node_cnt);
      nearestNodeDistance=newNodeDistance;
    }
  }
  return(nearestNode);
}
int McellSystem::getNearestNode(Location_S loc, ivec nodeList){
    
    int nearestNode=nodeList(0);
    double nearestNodeDistance=findDistance(loc,aNodes(nearestNode).getLoc());
    double newNodeDistance;
    for(int node_cnt=1;node_cnt<length(nodeList);node_cnt++)
    {
        newNodeDistance=findDistance(loc,aNodes(nodeList(node_cnt)).getLoc());
        if(nearestNodeDistance>newNodeDistance)
        {
            nearestNode=nodeList(node_cnt);
            nearestNodeDistance=newNodeDistance;
        }
    }
    return(nearestNode);
}
int McellSystem::getNodeTypeCnt(string nodeType){

  int nodeTypeCnt=findInStringArray(aNodeTypes,nodeType);
  if(nodeTypeCnt!=-1)
	return(nodeTypeCnt);
  else {cout<<"[both:]Undefined Node Name to getNodeType().";abort();}
}

vec McellSystem::getNodeTxPowerInDBm(string nodeType){

  ivec nodeList=getNodes(nodeType);
  return(getNodeTxPowerInDBmFromList(nodeList));
}

vec McellSystem::getNodeTxPowerInDBmFromList(ivec nodeList){

  vec txPowInDBm(nodeList.length());
  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
	txPowInDBm.set(node_cnt,aNodes(nodeList(node_cnt)).getTxPowerInDBm());
  return(txPowInDBm);
}

NodeLinkInfo_S McellSystem::getLinkInfo(int txNodeID, int rxNodeID){

  if(!aNodes(txNodeID).isActive())
  {
	cout<<"[both:]Node "<<txNodeID<<" not active in getLinkInfo()."<<endl;
	abort();
  }
  if(!aNodes(rxNodeID).isActive())
  {
	cout<<"[both:]Node "<<rxNodeID<<" not active in getLinkInfo()."<<endl;
	abort();
  }

  if(!isLinkInfoUpToDate()) cout<<"[detl:]Please call generateLinkInfo() for the respective nodes or the entire system."<<endl;

  if(aNodes(rxNodeID).isLinked(txNodeID)){
    //cout<<"txNodeID value"<<txNodeID<<endl;
    return(aNodes(rxNodeID).getLinkInfo(txNodeID));}
  else
  {
	cout<<"[both:]Given TxNode and RxNode are not linked."<<endl;
	abort();
  }
}

/*!\fn findGroup
 * \brief --> this function finds the groups having the same group name and group IDs 
 *\param[in] groupNameToFind--> it will give the name of the group that should be found
 *\param[in] partialMatch --> it wil tell the partial match of the name
 *\return myGroupIDs-->returns the groupIDs of the group that found
 */
ivec McellSystem::findGroup(string groupNameToFind, bool partialMatch){

  ivec myGroupIDs;
  myGroupIDs.set_size(0);
  if(partialMatch)
  {
	for(int group_cnt=0;group_cnt<aNodeGroups.length();group_cnt++)
	  if(aNodeGroups(group_cnt).groupName.find(groupNameToFind)!=string::npos)
		append(myGroupIDs,group_cnt);
  }
  else
  {
	for(int group_cnt=0;group_cnt<aNodeGroups.length();group_cnt++)
	  if(aNodeGroups(group_cnt).groupName==groupNameToFind)
		append(myGroupIDs,group_cnt);
  }
  if(myGroupIDs.length()==0) myGroupIDs="-1";
	return(myGroupIDs);

}
/*!\fn findGroup()
 * \brief --> this function finds the groups having the same group name and group IDs
 * \param[in] groupIDs--> given the vector of groupIDs 
 *\param[in] groupNameToFind--> it will give the name of the group that should be found
 *\return myGroupIDs-->returns the groupIDs of the group that found
 */
ivec McellSystem::findGroup(ivec groupIDs, string groupNameToFind,bool partialMatch){

  ivec myGroupIDs;
  myGroupIDs.set_size(0);
  if(partialMatch)
  {
	for(int group_cnt=0;group_cnt<length(groupIDs);group_cnt++)
	  if(aNodeGroups(groupIDs(group_cnt)).groupName.find(groupNameToFind)!=string::npos)
		append(myGroupIDs,groupIDs(group_cnt));
  }
  else
  {
	for(int group_cnt=0;group_cnt<length(groupIDs);group_cnt++)
	  if(aNodeGroups(groupIDs(group_cnt)).groupName==groupNameToFind)
		append(myGroupIDs,groupIDs(group_cnt));
  }
  if(myGroupIDs.length()==0) myGroupIDs="-1";
  return(myGroupIDs);

}

void McellSystem::setGroupCarrierFreq(string groupName,double carrierFreqinHz){

  ivec groupID=findGroup(groupName);
  setGroupCarrierFreq(groupID(0),carrierFreqinHz);
}

void McellSystem::setGroupCarrierFreq(int groupID,double carrierFreqinHz){

  if(groupID >= aNodeGroups.length()){cout<<"[both:]Invalid Group ID for setGroupCarrierFreq().";abort();}
  for(int node_cnt=0;node_cnt<aNodeGroups(groupID).nodeIDs.length();node_cnt++)
	aNodes(aNodeGroups(groupID).nodeIDs(node_cnt)).setCarrierFreqInHz(carrierFreqinHz);
}

void McellSystem::setGroupCategory(string groupName,NodeCategory_E category){

   ivec groupID=findGroup(groupName);
   setGroupCategory(groupID(0),category);
}

void McellSystem::setGroupCategory(int groupID,NodeCategory_E category){

  if(groupID >= aNodeGroups.length()){cout<<"[both:]Invalid Group ID for setGroupMode().";abort();}
  for(int node_cnt=0;node_cnt<aNodeGroups(groupID).nodeIDs.length();node_cnt++)
	aNodes(aNodeGroups(groupID).nodeIDs(node_cnt)).setCategory(category);
}

void McellSystem::setGroupLoc(string groupName, Location_S loc){

   ivec groupID=findGroup(groupName);
   setGroupLoc(groupID(0), loc);
}

void McellSystem::setGroupLoc(int groupID, Location_S loc){

  if(groupID >= aNodeGroups.length()){cout<<"[both:]Invalid Group ID for setGroupLoc().";abort();}
  for(int node_cnt=0;node_cnt<aNodeGroups(groupID).nodeIDs.length();node_cnt++)
	aNodes(aNodeGroups(groupID).nodeIDs(node_cnt)).setLoc(loc, true);
}

void McellSystem::activateNodeGroup(string groupName){

   ivec groupID=findGroup(groupName);
   activateNodeGroup(groupID(0));
}

void McellSystem::activateNodeGroup(int groupID){

  if(groupID >= aNodeGroups.length()){cout<<"[both:]Invalid Group ID for activateNodeGroup().";abort();}
  for(int node_cnt=0;node_cnt<aNodeGroups(groupID).nodeIDs.length();node_cnt++)
	aNodes(aNodeGroups(groupID).nodeIDs(node_cnt)).setActive();
}

void McellSystem::deactivateNodeGroup(string groupName){

  ivec groupID=findGroup(groupName);
  deactivateNodeGroup(groupID(0));
}

void McellSystem::deactivateNodeGroup(int groupID){

  if(groupID >= aNodeGroups.length()){cout<<"[both:]Invalid Group ID for deactivateNodeGroup().";abort();}
  for(int node_cnt=0;node_cnt<aNodeGroups(groupID).nodeIDs.length();node_cnt++)
  aNodes(aNodeGroups(groupID).nodeIDs(node_cnt)).setInactive();
}

void McellSystem::setGroupShadowGrid(string groupName, mat shadowGrid){

   ivec groupID=findGroup(groupName);
   setGroupShadowGrid(groupID(0), shadowGrid);
}

void McellSystem::setGroupShadowGrid(int groupID, mat shadowGrid){

  if(groupID >= aNodeGroups.length()){cout<<"[both:] Invalid Group ID for setGroupShadowGrid()..";abort();}
  if(shadowType!=_SHADOWTYPE_CORRELATED_) {cout<<"[detl:]Warning. Shadow Grid is available in nodes only when shadow type is correlated."<<endl;}

  for(int node_cnt=0;node_cnt<aNodeGroups(groupID).nodeIDs.length();node_cnt++)
	aNodes(aNodeGroups(groupID).nodeIDs(node_cnt)).setShadowGrid(shadowGrid);
}

void McellSystem::setGroupAntenna(string groupName, Antenna antenna){

   ivec groupID=findGroup(groupName);
   setGroupAntenna(groupID(0), antenna);
}

void McellSystem::setGroupAntenna(int groupID, Antenna antenna){

  if(groupID >= aNodeGroups.length()){cout<<"[both:]Invalid Group ID for setGroupAntenna().";abort();}
  for(int node_cnt=0;node_cnt<aNodeGroups(groupID).nodeIDs.length();node_cnt++)
	aNodes(aNodeGroups(groupID).nodeIDs(node_cnt)).setAntenna(antenna);
}
void McellSystem::setGroupTxAntenna(string groupName, Antenna antenna){

  ivec groupID=findGroup(groupName);
  setGroupTxAntenna(groupID(0), antenna);
}

void McellSystem::setGroupTxAntenna(int groupID, Antenna antenna){

  if(groupID >= aNodeGroups.length()){cout<<"[both:]Invalid Group ID for setGroupTxAntenna().";abort();}
  for(int node_cnt=0;node_cnt<aNodeGroups(groupID).nodeIDs.length();node_cnt++)
    aNodes(aNodeGroups(groupID).nodeIDs(node_cnt)).setTxAntenna(antenna);
}
void McellSystem::setGroupRxAntenna(string groupName, Antenna antenna){

  ivec groupID=findGroup(groupName);
  setGroupRxAntenna(groupID(0), antenna);
}

void McellSystem::setGroupRxAntenna(int groupID, Antenna antenna){

  if(groupID >= aNodeGroups.length()){cout<<"[both:]Invalid Group ID for setGroupRxAntenna().";abort();}
  for(int node_cnt=0;node_cnt<aNodeGroups(groupID).nodeIDs.length();node_cnt++)
    aNodes(aNodeGroups(groupID).nodeIDs(node_cnt)).setRxAntenna(antenna);
}
ivec McellSystem::getNodesInTheGroup(int groupID){

  if(groupID >= aNodeGroups.length()){cout<<"[both:]Invalid Group ID for getNodesInTheGroup().";abort();}
  return(aNodeGroups(groupID).nodeIDs);
}

NodeGroup_S McellSystem::getGroupInfo(int groupID){

  if(groupID >= aNodeGroups.length()){cout<<"[both:]Invalid Group ID for getGroupInfo().";abort();}
  return(aNodeGroups(groupID));
}



int McellSystem::findHexagonalCellID(int nodeID){

  if(cellLayout==_CELLLAYOUT_RANDOM_)
  {
    cout<<"[detl:]Warning. Cannot call function findHexagonalCellID() for non Hexagonal Layout.";
	return(-1);
  }
  int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
  ivec hexNodes=getNodes(aNodeTypes(hexDropNodeType));
  int nearestNode=getNearestNode(nodeID,hexNodes);
  return(aNodes(nearestNode).getCellID());
}

int McellSystem::findHexagonalCellID(Location_S loc){
    
    if(cellLayout!=_CELLLAYOUT_HEXAGONAL_)
    {
        cout<<"[detl:]Warning. Cannot call function findHexagonalCellID() for non Hexagonal Layout.";
        return(-1);
    }
    int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
    ivec hexNodes=getNodes(aNodeTypes(hexDropNodeType));
    int nearestNode=getNearestNode(loc,hexNodes);
    return(aNodes(nearestNode).getCellID());
}


NodeCategory_E McellSystem::getNodeCategory(string nodeType){

  ivec nodeList=getNodes(nodeType);

  if(aNodes(nodeList(0)).getCategory()==_NODECATEGORY_SERVERNODE_)
    return(_NODECATEGORY_SERVERNODE_);
  else if(aNodes(nodeList(0)).getCategory()==_NODECATEGORY_SERVICENODE_)
    return(_NODECATEGORY_SERVICENODE_);
  else
  {
    cout<<"[detl:]Warning. Category of all Nodes are not initialized in getNodeCategory()."<<endl;
    return(_NODECATEGORY_UNINIT_);
  }

}

Array<NodeCategory_E> McellSystem::getNodeCategory()
{
    Array<NodeCategory_E> nodeCategories(aNodeTypes.length());
    for(int nodeType_cnt=0;nodeType_cnt<aNodeTypes.length();nodeType_cnt++)
        if(nNodes(nodeType_cnt)>0)
            nodeCategories(nodeType_cnt)= getNodeCategory(aNodeTypes(nodeType_cnt));
        else
            nodeCategories(nodeType_cnt)=_NODECATEGORY_UNINIT_;
    return nodeCategories;
}

ivec McellSystem::getNodeIDCell(string nodeType){

  ivec nodeList=getNodes(nodeType);
  return(getNodeIDCellFromList(nodeList));
}

ivec McellSystem::getNodeIDCell(int nodeTypeCnt){

  ivec nodeList=getNodes(nodeTypeCnt);
  return(getNodeIDCellFromList(nodeList));
}

ivec McellSystem::getNodeIDCellFromList(ivec nodeList){

  ivec idcells(nodeList.length());
  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
    idcells(node_cnt)=aNodes(nodeList(node_cnt)).getIDCell();
  return(idcells);
}

vec McellSystem::getNodeVelocity(string nodeType){

  ivec nodeList=getNodes(nodeType);
  return(getNodeVelocityFromList(nodeList));
}

vec McellSystem::getNodeVelocity(int nodeTypeCnt){

  ivec nodeList=getNodes(nodeTypeCnt);
  return(getNodeVelocityFromList(nodeList));
}

vec McellSystem::getNodeVelocityFromList(ivec nodeList){

  vec velocity(nodeList.length());
  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
    velocity(node_cnt)=aNodes(nodeList(node_cnt)).getVelocity();
  return(velocity);
}

ivec McellSystem::getNodeAntennaCount(string nodeType){

  ivec nodeList=getNodes(nodeType);
  return(getNodeAntennaCountFromList(nodeList));
}

ivec McellSystem::getNodeAntennaCount(int nodeTypeCnt){

  ivec nodeList=getNodes(nodeTypeCnt);
  return(getNodeAntennaCountFromList(nodeList));
}

double McellSystem::getNodeNoiseFigure(string nodeType){

  int nodeTypeCnt=findInStringArray(aNodeTypes,nodeType);
  return(getNodeNoiseFigure(nodeTypeCnt));
}

double McellSystem::getNodeNoiseFigure(int nodeTypeCnt){

  return(noiseFigurePerNodeType(nodeTypeCnt));
}

ivec McellSystem::getNodeAntennaCountFromList(ivec nodeList){

  ivec antCount(nodeList.length());
  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
    antCount(node_cnt)=aNodes(nodeList(node_cnt)).getAntennaCount();
  return(antCount);
}

vec McellSystem::getNodeCarrierFreuencyInHz(string nodeType){

  ivec nodeList=getNodes(nodeType);
  return(getNodeCarrierFreuencyInHzFromList(nodeList));
}

vec McellSystem::getNodeCarrierFreuencyInHz(int nodeTypeCnt){

  ivec nodeList=getNodes(nodeTypeCnt);
  return(getNodeCarrierFreuencyInHzFromList(nodeList));
}

vec McellSystem::getNodeCarrierFreuencyInHzFromList(ivec nodeList){

  vec carrierFreq(nodeList.length());
  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
    carrierFreq(node_cnt)=aNodes(nodeList(node_cnt)).getCarrierFreqInHz();
  return(carrierFreq);
}

ivec McellSystem::getNodeCentralUnitID(string nodeType){

  ivec nodeList=getNodes(nodeType);
  return(getNodeCentralUnitIDFromList(nodeList));
}

ivec McellSystem::getNodeCentralUnitID(int nodeTypeCnt){

  ivec nodeList=getNodes(nodeTypeCnt);
  return(getNodeCentralUnitIDFromList(nodeList));
}

ivec McellSystem::getNodeCentralUnitIDFromList(ivec nodeList){

  ivec centralUnitIDs(nodeList.length());
  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
    centralUnitIDs(node_cnt)=aNodes(nodeList(node_cnt)).getCentralUnitID();
  return(centralUnitIDs);
}

Array<ivec> McellSystem::getNodeNumerologies(string nodeType){
  
  ivec nodeList=getNodes(nodeType);
  return(getNodeNumerologiesFromList(nodeList));
}

Array<ivec> McellSystem::getNodeNumerologies(int nodeTypeCnt){
  
  ivec nodeList=getNodes(nodeTypeCnt);
  return(getNodeNumerologiesFromList(nodeList));
}

Array<ivec> McellSystem::getNodeNumerologiesFromList(ivec nodeList){
  
  Array<ivec> nus(nodeList.length());
  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
    nus(node_cnt)=aNodes(nodeList(node_cnt)).getNumerologies();
  return(nus);
}

ivec McellSystem::getNodeTypeFromList(ivec nodeList){

  ivec nodeType(nodeList.length());
  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
    nodeType(node_cnt)=aNodes(nodeList(node_cnt)).getType();
  return(nodeType);

}

void McellSystem::setNodeTxPowerInDBm(string nodeType,double txPower){

  int nodeTypeCnt=findInStringArray(aNodeTypes,nodeType);
  if(nodeTypeCnt!=-1)
	setNodeTxPowerInDBm(nodeTypeCnt,txPower);
  else {cout<<"[both:]Undefined Node Name to setNodeTxPowerInDBm().";abort();}
}

void McellSystem::setNodeTxPowerInDBm(int nodeTypeCnt,double txPower){

 ivec nodeList=getNodes(nodeTypeCnt);
	setNodeTxPowerInDBmByIndex(nodeList,txPower);
}

void McellSystem::setNodeTxPowerInDBmByIndex(ivec nodeList,double txPower){

  for(int node_cnt=0;node_cnt<length(nodeList);node_cnt++)
	  aNodes(nodeList(node_cnt)).setTxPowerInDBm(txPower);
}

// void McellSystem::copyLinkInfo(int rxNode,int txNode,NodeLinkInfo_S tempLinkInfo)
// {
//     tempLinkInfo.id = txNode;
//     aNodes(rxNode).computeAntennaGains(aNodes(txNode),tempLinkInfo);
//     int linkInfoIndx = aNodes(rxNode).getLinkInfoIndx(txNode);
//     if(linkInfoIndx==-1)
//     {
//         append(aNodes(rxNode).linkedTxNodes,txNode);
//         append(aNodes(rxNode).aLinkInfo,tempLinkInfo);
//     }
//     else
//     {
//         aNodes(rxNode).updateLinkInfo(tempLinkInfo);
//     }
// }

void McellSystem::allocateLinkInfo()
{
    if(!isSysReady())         
    {
        cout<<"[both:]System Not Fully Configured.";
        abort();
    }
    
    ivec activeNodes=getActiveNodes();  
    if(length(activeNodes)==0)           
    {
        cout<<"[detl:]Making all nodes active as no Node in the system is activated."<<endl; 
        activateNodes();
    }
    ivec serverNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));
    ivec serviceNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));
    append(activeNodes,serverNodes);
    bvec serverNodeLinkInfoStatus(serverNodes.length()); 
    bvec serviceNodeLinkInfoStatus(serviceNodes.length());
    for(int serviceNode_cnt=0;serviceNode_cnt<length(serviceNodes);serviceNode_cnt++)
        serviceNodeLinkInfoStatus(serviceNode_cnt)=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfoUpdateStatus();
    for(int serverNode_cnt=0;serverNode_cnt<length(serverNodes);serverNode_cnt++)  
        serverNodeLinkInfoStatus(serverNode_cnt)=aNodes(serverNodes(serverNode_cnt)).getLinkInfoUpdateStatus();
    for(int activeNode_cnt=0;activeNode_cnt<activeNodes.length();activeNode_cnt++)
    {
        aNodes(activeNodes(activeNode_cnt)).setLinkInfoUpdateStatus(true);
        aNodes(activeNodes(activeNode_cnt)).txAntenna.setParameterUpdateStatus(true);
        aNodes(activeNodes(activeNode_cnt)).rxAntenna.setParameterUpdateStatus(true);
    }
    
    if(isCrossLinksEnabled)
    {
        ivec nodeList=concat(serverNodes,serviceNodes);
        for(int serverNode_cnt=0;serverNode_cnt<length(serverNodes);serverNode_cnt++)
            aNodes(serverNodes(serverNode_cnt)).allocLinkInfo(nodeList);
        for(int serviceNode_cnt=0;serviceNode_cnt<length(serviceNodes);serviceNode_cnt++)
            aNodes(serviceNodes(serviceNode_cnt)).allocLinkInfo(nodeList);
    }
    else  //! If crosslink is not present allocating the memory for service and server nodes seperatly //   
    {
        for(int serverNode_cnt=0;serverNode_cnt<length(serverNodes);serverNode_cnt++)
            aNodes(serverNodes(serverNode_cnt)).allocLinkInfo(serviceNodes);
        for(int serviceNode_cnt=0;serviceNode_cnt<length(serviceNodes);serviceNode_cnt++)
            aNodes(serviceNodes(serviceNode_cnt)).allocLinkInfo(serverNodes);
    }
}

void McellSystem::generateLinkInfo(bool retailUnAlteredLinks, bool retainOldLOS)
{
    if(!isSysReady())         
    {
        cout<<"[both:]System Not Fully Configured.";
        abort();
    }
    
    ivec activeNodes=getActiveNodes();  
    if(activeNodes.length()==0)           
    {
        cout<<"[detl:]Making all nodes active as no Node in the system is activated."<<endl; 
        activateNodes();
    }
    
    ivec serverNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));
    ivec serviceNodes=(serviceNodesPerTask.length()) ? (serviceNodesPerTask.length()>currentTaskid) ? serviceNodesPerTask(currentTaskid): zeros_i(0) : getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));
    //   ivec serviceNodes = serviceNodesPerTask(currentTaskid);
    
    append(activeNodes,serverNodes);
    bvec serverNodeLinkInfoStatus(serverNodes.length()); 
    bvec serviceNodeLinkInfoStatus(serviceNodes.length());
    for(int serviceNode_cnt=0;serviceNode_cnt<length(serviceNodes);serviceNode_cnt++)
        serviceNodeLinkInfoStatus(serviceNode_cnt)=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfoUpdateStatus();
    for(int serverNode_cnt=0;serverNode_cnt<length(serverNodes);serverNode_cnt++)  
        serverNodeLinkInfoStatus(serverNode_cnt)=aNodes(serverNodes(serverNode_cnt)).getLinkInfoUpdateStatus();
    for(int activeNode_cnt=0;activeNode_cnt<activeNodes.length();activeNode_cnt++)
    {
        aNodes(activeNodes(activeNode_cnt)).setLinkInfoUpdateStatus(true);
        aNodes(activeNodes(activeNode_cnt)).txAntenna.setParameterUpdateStatus(true);
        aNodes(activeNodes(activeNode_cnt)).rxAntenna.setParameterUpdateStatus(true);
    }
    
    computePenetrationLossesForAllServiceNodes();
    
    ivec hexNodes(0);
    ivec rndhexNodes(0);
    ivec inHNodes(0);
    bool isInHNodeType =false;
    bool isrndhexNodeType =false;
    if(cellLayout==_CELLLAYOUT_HEXAGONAL_) 
    {
        int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
        hexNodes=getNodes(aNodeTypes(hexDropNodeType));
    }
    
    if(cellLayout == _CELLLAYOUT_RANDOM_)
    {
        int inHnodeType =find(nodeDropMethods,_DROPMETHOD_INBUILDING3SEC_);
        if(inHnodeType ==-1)
            inHnodeType =find(nodeDropMethods,_DROPMETHOD_INBUILDING_);
        
        if(inHnodeType !=-1)
        {
            inHNodes=getNodes(aNodeTypes(inHnodeType));
            isInHNodeType =true;
        }
        
        int rndhexnodeType =find(nodeDropMethods,_DROPMETHOD_RANDOMHEX_);
        if(rndhexnodeType !=-1)
        {
            isrndhexNodeType=true;
            rndhexNodes=getNodes(aNodeTypes(rndhexnodeType));
        }
    }
    
    ivec rxGroupID,txGroupID; 
    NodeLinkInfo_S tempLinkInfo;
    Location_S wrapTxLoc,txCellLoc;
    bool isLinkAltered;
    
    if(isCrossLinksEnabled)
    {
        ivec nodeList=concat(serverNodes,serviceNodes);
        for(int serverNode_cnt=0;serverNode_cnt<length(serverNodes);serverNode_cnt++)
            aNodes(serverNodes(serverNode_cnt)).allocLinkInfo(nodeList);
        for(int serviceNode_cnt=0;serviceNode_cnt<length(serviceNodes);serviceNode_cnt++)
            aNodes(serviceNodes(serviceNode_cnt)).allocLinkInfo(nodeList);
    }
    else  //! If crosslink is not present allocating the memory for service and server nodes seperatly //   
    {
        for(int serverNode_cnt=0;serverNode_cnt<length(serverNodes);serverNode_cnt++)
            aNodes(serverNodes(serverNode_cnt)).allocLinkInfo(serviceNodes);
        for(int serviceNode_cnt=0;serviceNode_cnt<length(serviceNodes);serviceNode_cnt++)
            aNodes(serviceNodes(serviceNode_cnt)).allocLinkInfo(serverNodes);
    }
    
    //! Actual linking begins//
    int isDownLink=-1;
    for(int serviceNode_cnt=0;serviceNode_cnt<length(serviceNodes);serviceNode_cnt++)//! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
    {
        
        //     if(aNodes(serviceNodes(serviceNode_cnt)).getLinkCount()!=0)
        //       aNodes(serviceNodes(serviceNode_cnt)).retainLinkInfo();
             
        for(int serverNode_cnt=0;serverNode_cnt<length(serverNodes);serverNode_cnt++)
        {
            //! Assuming serverNodes as Transmitters
            isDownLink=1;
            {
                if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
                {
                  
                    for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)//! Get the location of hexagonal cell since it(servernode) is taken as transmitter from which we get transmitter cell ID//
                    if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(serverNode_cnt)).getCellID())//! If the hexnode cell ID is server node cell ID //
                        txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();
                    
                    if(aNodes(serviceNodes(serviceNode_cnt)).getCellID()!=aNodes(serverNodes(serverNode_cnt)).getCellID())//! If server node cell ID is not equal to service node cellID implies both are not in the same cell//
                    {
                        for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)//! For all hexnodes if some hexnode cellID which is equal to servicenode cellID there will be a wraparound of tx with respect to neighboring cells////
                            if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(serviceNode_cnt)).getCellID())
                                wrapTxLoc=WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serverNodes(serverNode_cnt)).getCellID()),txCellLoc,aNodes(serverNodes(serverNode_cnt)).getLoc());
                            aNodes(serverNodes(serverNode_cnt)).setWrapLoc(wrapTxLoc,true);//! Setting the wrap around location//
                    }
                    else   //! Else if server and service node are in the same cell there is no wraparound with respect to that cell wraparound will give the samecell so get the location of same cell //
                        aNodes(serverNodes(serverNode_cnt)).setWrapLoc(aNodes(serverNodes(serverNode_cnt)).getLoc(),true);
                    
                    isLinkAltered=!(serviceNodeLinkInfoStatus(serviceNode_cnt)*serverNodeLinkInfoStatus(serverNode_cnt));
                    aNodes(serviceNodes(serviceNode_cnt)).linkToTXNode(aNodes(serverNodes(serverNode_cnt)),sysTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//! This gives the link details w.r.t the transmitter//
                    
                    rxGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenodes with the same loc(name) and getting thier IDs//
                    txGroupID=findGroup(aNodes(serverNodes(serverNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernodes with the same loc(name) and getting thier IDs//
                    
                    if(rxGroupID(0)!=-1)//! Receiver grouping is done ,if it is the first node in the group ,set the properties as defined by linkto tx node//
                    {
                        if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0) 
                        {
                            aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
                            aNodeGroups(rxGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt);
                        }
                        else   //! Else if there exits a node before then set the properties of new node to the properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
                        {
                            tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serverNodes(serverNode_cnt));  
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"isLOS",tempLinkInfo.isLOS);
                            if(sysTerrain.isForSCMChannel)
                                aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),tempLinkInfo.scmLargeScaleParams);
                        }
                    }
                    if(txGroupID(0)!=-1)//! Transmitter grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
                    {	
                        if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
                        {
                            aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
                            aNodeGroups(txGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt);
                        }
                        else    //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
                        {
                            tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"isLOS",tempLinkInfo.isLOS);
                            if(sysTerrain.isForSCMChannel) //! If there is scm channel//
                                aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),tempLinkInfo.scmLargeScaleParams);
                        }
                    }
                }
                else if(cellLayout==_CELLLAYOUT_RANDOM_ && (isInHNodeType|| isrndhexNodeType))
                {
                    txCellLoc=aNodes(serverNodes(serverNode_cnt)).getLoc();
                    aNodes(serverNodes(serverNode_cnt)).setWrapLoc(aNodes(serverNodes(serverNode_cnt)).getLoc(),true);
                    isLinkAltered=!(serviceNodeLinkInfoStatus(serviceNode_cnt)*serverNodeLinkInfoStatus(serverNode_cnt));
                    aNodes(serviceNodes(serviceNode_cnt)).linkToTXNode(aNodes(serverNodes(serverNode_cnt)),sysTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//! This gives the link details w.r.t the transmitter//
                    
                    rxGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenodes with the same loc(name) and getting thier IDs//
                    txGroupID=findGroup(aNodes(serverNodes(serverNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernodes with the same loc(name) and getting thier IDs//
                    
                    if(rxGroupID(0)!=-1)//! Receiver grouping is done ,if it is the first node in the group ,set the properties as defined by linkto tx node//
                    {
                        if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0) 
                        {
                            aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
                            aNodeGroups(rxGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt);
                        }
                        else   //! Else if there exits a node before then set the properties of new node to the properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
                        {
                            tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serverNodes(serverNode_cnt));  
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"isLOS",tempLinkInfo.isLOS);
                            if(sysTerrain.isForSCMChannel)
                                aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),tempLinkInfo.scmLargeScaleParams);
                        }
                    }
                    if(txGroupID(0)!=-1)//! Transmitter grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
                    {	
                        if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
                        {
                            aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
                            aNodeGroups(txGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt);
                        }
                        else    //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
                        {
                            tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"isLOS",tempLinkInfo.isLOS);
                            if(sysTerrain.isForSCMChannel) //! If there is scm channel//
                                aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),tempLinkInfo.scmLargeScaleParams);
                        }
                    }
                }
                else if(cellLayout==_CELLLAYOUT_NTN_)
                {
                    isLinkAltered=!(serviceNodeLinkInfoStatus(serviceNode_cnt)*serverNodeLinkInfoStatus(serverNode_cnt));
                    int srvrSatID = aNodes(serverNodes(serverNode_cnt)).getSatelliteID();
                    int srvcSatID = aNodes(serviceNodes(serviceNode_cnt)).getSatelliteID();
                    int srvrFreqID = aNodes(serverNodes(serverNode_cnt)).getFrequencyID();
                    int srvcFreqID = aNodes(serviceNodes(serviceNode_cnt)).getFrequencyID();
                    
                    //cout<<"SatID : "<<srvrSatID<<" , "<<srvcSatID<<endl;
                    //cout<<"FreqID : "<<srvrFreqID<<" , "<<srvcFreqID<<endl;
                    
                    
                    if(srvcSatID!=srvrSatID || srvrFreqID!=srvcFreqID)
                    {
                        //cout<<"Generating linkinfo condition1 : "<<serverNodes(serverNode_cnt)<<endl;
                        aNodes(serviceNodes(serviceNode_cnt)).linkToTXNode(aNodes(serverNodes(serverNode_cnt)),sysTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//get the linkinfo parameters directly by using linkToTXNode//
                    }
                    else
                    {
                        int refBSid = nSectorsPerSite*aNodes(serverNodes(serverNode_cnt)).getSatelliteID();
                        int srvrCellID = refBSid + aNodes(serverNodes(serverNode_cnt)).getCellID();
                        int refCellID = getRefCellID(frequencyReUseFactor)(srvrFreqID);
                        
                        //cout<<"refBSid : "<<refBSid<<" , "<<refCellID<<" , "<<srvrCellID<<endl;
                        
                        if(refBSid + refCellID == srvrCellID)
                        {
                            //cout<<"Generating linkinfo condition2 : "<<serverNodes(serverNode_cnt)<<endl;
                            aNodes(serviceNodes(serviceNode_cnt)).linkToTXNode(aNodes(serverNodes(serverNode_cnt)),sysTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//get the linkinfo parameters directly by using linkToTXNode//
                        }
                        else
                        {
                            //cout<<"Copying link info from : "<<refBSid+refCellID<<" to "<<serverNodes(serverNode_cnt)<<endl;
                            aNodes(serviceNodes(serviceNode_cnt)).linkToTXNode(aNodes(serverNodes(serverNode_cnt)),sysTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//get the linkinfo parameters directly by using linkToTXNode//
                    
                            tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(refBSid+refCellID);  
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
                            aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"isLOS",tempLinkInfo.isLOS);
                            if(sysTerrain.isForSCMChannel)
                                aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),tempLinkInfo.scmLargeScaleParams);
                        }
                    }
                }	
                else  //! If the cell layout is random //
                {
                  aNodes(serverNodes(serverNode_cnt)).setWrapLoc(aNodes(serverNodes(serverNode_cnt)).getLoc(),true);
                    aNodes(serviceNodes(serviceNode_cnt)).linkToTXNode(aNodes(serverNodes(serverNode_cnt)),sysTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//get the linkinfo parameters directly by using linkToTXNode//
                }
            }
            //! Assuming serverNodes as Receivers and service nodes as transmitters//
            isDownLink=0;
            {
                //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
                if(cellLayout==_CELLLAYOUT_HEXAGONAL_)   //! Setting the service node location as transmitter location for all hexagonal nodes//
                {
                    for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)//
                            if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(serviceNode_cnt)).getCellID())
                                txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();
                            
                            //! If the server node and service node are in  different cells then finding the wraparound location of service nodes with respect to servernodes //// 
                            
                            if(aNodes(serverNodes(serverNode_cnt)).getCellID()!=aNodes(serviceNodes(serviceNode_cnt)).getCellID())
                            {
                                for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++) 
                                    if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(serverNode_cnt)).getCellID())
                                        wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serviceNodes(serviceNode_cnt)).getCellID()),txCellLoc,aNodes(serviceNodes(serviceNode_cnt)).getLoc());
                                    aNodes(serviceNodes(serviceNode_cnt)).setWrapLoc(wrapTxLoc,true);
                            }
                            else //! Else if server and service nodes are in same cell implies the wrap around location will be the same cell location////
                                aNodes(serviceNodes(serviceNode_cnt)).setWrapLoc(aNodes(serviceNodes(serviceNode_cnt)).getLoc(),true);
                            isLinkAltered=!(serverNodeLinkInfoStatus(serverNode_cnt)*serviceNodeLinkInfoStatus(serviceNode_cnt));
                            aNodes(serverNodes(serverNode_cnt)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt)),sysTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//! Getting the parameters of link info status of to the transmitter by this function//
                    
                    // Overwriting PL,SL and isLOS to make the forward and reverse links identical ...//since it isuplink the properties of a link will be same as that of uplink// 
                    tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(serverNodes(serverNode_cnt));//! Get the linkinfo report of the servicenode in downlink to feed in linkinfo report of server node in uplink//
                    aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
                    aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
                    aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
                    if(sysTerrain.isForSCMChannel) //! In the presence of system terrain of SCM channel//
                        aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);//! Overwrite scmlargeScaleParams in the uplink for servicenode//
                        
                        rxGroupID=findGroup(aNodes(serverNodes(serverNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernodes with the same loc(name) and getting thier IDs
                        txGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenode with the same loc(name) and getting thier IDs//
                        
                        if(rxGroupID(0)!=-1) //! Receiver grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
                        {
                            if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
                            {
                                aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
                                aNodeGroups(rxGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt);
                            }
                            else //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
                            {
                                tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serviceNodes(serviceNode_cnt));
                                aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
                                aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
                                aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
                                if(sysTerrain.isForSCMChannel) 
                                    aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);
                            }
                        }
                        if(txGroupID(0)!=-1) //! Transmitter grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
                        {
                            if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0) 
                            {
                                aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
                                aNodeGroups(txGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt);
                            }
                            else  //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
                            {
                                tempLinkInfo=aNodes(serverNodes(serverNode_cnt)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0)); //! Getting the linkinfo of the first node//
                                aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
                                aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
                                aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
                                if(sysTerrain.isForSCMChannel)
                                    aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);
                            }
                        }
                }
                else if(cellLayout==_CELLLAYOUT_RANDOM_ && (isInHNodeType|| isrndhexNodeType))   //! Setting the service node location as transmitter location for all hexagonal nodes//
                {
                    
                    txCellLoc=aNodes(serviceNodes(serviceNode_cnt)).getLoc();
                    aNodes(serviceNodes(serviceNode_cnt)).setWrapLoc(aNodes(serviceNodes(serviceNode_cnt)).getLoc(),true);
                    isLinkAltered=!(serverNodeLinkInfoStatus(serverNode_cnt)*serviceNodeLinkInfoStatus(serviceNode_cnt));
                    aNodes(serverNodes(serverNode_cnt)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt)),sysTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//! Getting the parameters of link info status of to the transmitter by this function//
                    
                    // Overwriting PL,SL and isLOS to make the forward and reverse links identical ...//since it isuplink the properties of a link will be same as that of uplink// 
                    tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(serverNodes(serverNode_cnt));//! Get the linkinfo report of the servicenode in downlink to feed in linkinfo report of server node in uplink//
                    aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
                    aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
                    aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
                    
                    if(sysTerrain.isForSCMChannel) //! In the presence of system terrain of SCM channel//
                        aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);//! Overwrite scmlargeScaleParams in the uplink for servicenode//
                        
                    rxGroupID=findGroup(aNodes(serverNodes(serverNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernodes with the same loc(name) and getting thier IDs
                    txGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenode with the same loc(name) and getting thier IDs//
                        
                    if(rxGroupID(0)!=-1) //! Receiver grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
                    {
                        if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
                        {
                            aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
                            aNodeGroups(rxGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt);
                        }
                        else //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
                        {
                            tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serviceNodes(serviceNode_cnt));
                            aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
                            aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
                            aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
                            if(sysTerrain.isForSCMChannel) 
                                aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);
                        }
                    }
                    if(txGroupID(0)!=-1) //! Transmitter grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
                    {
                        if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0) 
                        {
                            aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
                            aNodeGroups(txGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt);
                        }
                        else  //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
                        {
                            tempLinkInfo=aNodes(serverNodes(serverNode_cnt)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0)); //! Getting the linkinfo of the first node//
                            aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
                            aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
                            aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
                            if(sysTerrain.isForSCMChannel)
                                aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);
                        }
                    }
                }
                else if(cellLayout==_CELLLAYOUT_NTN_)
                {
                    isLinkAltered=!(serviceNodeLinkInfoStatus(serviceNode_cnt)*serverNodeLinkInfoStatus(serverNode_cnt));
                    int srvrSatID = aNodes(serverNodes(serverNode_cnt)).getSatelliteID();
                    int srvcSatID = aNodes(serviceNodes(serviceNode_cnt)).getSatelliteID();
                    int srvrFreqID = aNodes(serverNodes(serverNode_cnt)).getFrequencyID();
                    int srvcFreqID = aNodes(serviceNodes(serviceNode_cnt)).getFrequencyID();
                    
                    //cout<<"SatID : "<<srvrSatID<<" , "<<srvcSatID<<endl;
                    //cout<<"FreqID : "<<srvrFreqID<<" , "<<srvcFreqID<<endl;
                    
                    
                    if(srvcSatID!=srvrSatID || srvrFreqID!=srvcFreqID)
                    {
                        //cout<<"Generating linkinfo condition1 : "<<serverNodes(serverNode_cnt)<<endl;
                        aNodes(serviceNodes(serviceNode_cnt)).linkToTXNode(aNodes(serverNodes(serverNode_cnt)),sysTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//get the linkinfo parameters directly by using linkToTXNode//
                    }
                    else
                    {
                        int refBSid = nSectorsPerSite*aNodes(serverNodes(serverNode_cnt)).getSatelliteID();
                        int srvrCellID = refBSid + aNodes(serverNodes(serverNode_cnt)).getCellID();
                        int refCellID = getRefCellID(frequencyReUseFactor)(srvrFreqID);
                        
                        //cout<<"refBSid : "<<refBSid<<" , "<<refCellID<<" , "<<srvrCellID<<endl;
                        
                        if(refBSid + refCellID == srvrCellID)
                        {
                            //cout<<"Generating linkinfo condition2 : "<<serverNodes(serverNode_cnt)<<endl;
                            aNodes(serverNodes(serverNode_cnt)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt)),sysTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//get the linkinfo parameters directly by using linkToTXNode//
                          
                            //Overwriting PL,SL and isLOS to make the forward and reverse links identical ...
                            tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(serverNodes(serverNode_cnt));
                            aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
                            aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
                            aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
                            if(sysTerrain.isForSCMChannel)
                                aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);
                            
                        }
                        else
                        {
                            //cout<<"Copying link info from : "<<refBSid+refCellID<<" to "<<serverNodes(serverNode_cnt)<<endl;
                            aNodes(serverNodes(serverNode_cnt)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt)),sysTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//get the linkinfo parameters directly by using linkToTXNode//
                            
                            tempLinkInfo=aNodes(refBSid+refCellID).getLinkInfo(serviceNodes(serviceNode_cnt));  
                            aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
                            aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
                            aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
                            if(sysTerrain.isForSCMChannel)
                                aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);
                        }
                    }
                }	
                else //! If the cell layout is random //
                {
                    aNodes(serverNodes(serverNode_cnt)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt)),sysTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);
                    
                    //Overwriting PL,SL and isLOS to make the forward and reverse links identical ...
                    tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(serverNodes(serverNode_cnt));
                    aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
                    aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
                    aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
                    if(sysTerrain.isForSCMChannel)
                        aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);
                }
            }
        }
    }
}


/*!\fn generateLinkInfo()
 * \brief--> It will generate the links between the nodes of different combinations server and service nodes and get the properties of those links
 *\param[in,out] myTerrain --> it will take the input parameter of terrain class and get modified and return it
 *\param[in] retailUnAlteredLinks --> it is boolean parameter which retains the unaltered links when function is called again
 *\param[in] retainOldLOS --> it is a boolean parameter which retains the OldLOS when function is called again
 */
// void McellSystem::generateLinkInfo(Terrain &myTerrain, bool retailUnAlteredLinks, bool retainOldLOS){ 
// 
//   if(!isSysReady())         
//   {
// 	cout<<"[both:]System Not Fully Configured.";
// 	abort();
//   }
// 
//   ivec activeNodes=getActiveNodes();  
//   if(activeNodes.length()==0)           
//   {
//     cout<<"[detl:]Making all nodes active as no Node in the system is activated."<<endl; 
//     activateNodes();
//   }
//   
//   ivec serverNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));
//   ivec serviceNodes=(serviceNodesPerTask.length()) ? (serviceNodesPerTask.length()>currentTaskid) ? serviceNodesPerTask(currentTaskid): zeros_i(0) : getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));
// //   ivec serviceNodes = serviceNodesPerTask(currentTaskid);
//   
//   append(activeNodes,serverNodes);
//   bvec serverNodeLinkInfoStatus(serverNodes.length()); 
//   bvec serviceNodeLinkInfoStatus(serviceNodes.length());
//   for(int serviceNode_cnt=0;serviceNode_cnt<length(serviceNodes);serviceNode_cnt++)
//     serviceNodeLinkInfoStatus(serviceNode_cnt)=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfoUpdateStatus();
//   for(int serverNode_cnt=0;serverNode_cnt<length(serverNodes);serverNode_cnt++)  
//     serverNodeLinkInfoStatus(serverNode_cnt)=aNodes(serverNodes(serverNode_cnt)).getLinkInfoUpdateStatus();
//   for(int activeNode_cnt=0;activeNode_cnt<activeNodes.length();activeNode_cnt++)
//   {
//     aNodes(activeNodes(activeNode_cnt)).setLinkInfoUpdateStatus(true);
//     aNodes(activeNodes(activeNode_cnt)).txAntenna.setParameterUpdateStatus(true);
//     aNodes(activeNodes(activeNode_cnt)).rxAntenna.setParameterUpdateStatus(true);
//   }
//   
//   computePenetrationLossesForAllServiceNodes();
// 
//   ivec hexNodes(0);
//   ivec inHNodes(0);
//   bool isInHNodeType =false;
//   if(cellLayout==_CELLLAYOUT_HEXAGONAL_) 
//   {
// 	int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
// 	hexNodes=getNodes(aNodeTypes(hexDropNodeType));
//   }
//   
//   if(cellLayout == _CELLLAYOUT_RANDOM_)
//   {
//     int inHnodeType =find(nodeDropMethods,_DROPMETHOD_INBUILDING3SEC_);
//     if(inHnodeType ==-1)
//         inHnodeType =find(nodeDropMethods,_DROPMETHOD_INBUILDING_);
//     
//     if(inHnodeType !=-1)
//     {
//       inHNodes=getNodes(aNodeTypes(inHnodeType));
//       isInHNodeType =true;
//     }
//   }
// 
//   ivec rxGroupID,txGroupID; 
//   NodeLinkInfo_S tempLinkInfo;
//   Location_S wrapTxLoc,txCellLoc;
//   bool isLinkAltered;
//   
//   if(isCrossLinksEnabled)
//   {
//     ivec nodeList=concat(serverNodes,serviceNodes);
//     for(int serverNode_cnt=0;serverNode_cnt<length(serverNodes);serverNode_cnt++)
//       aNodes(serverNodes(serverNode_cnt)).allocLinkInfo(nodeList);
//     for(int serviceNode_cnt=0;serviceNode_cnt<length(serviceNodes);serviceNode_cnt++)
//       aNodes(serviceNodes(serviceNode_cnt)).allocLinkInfo(nodeList);
//   }
//   else  //! If crosslink is not present allocating the memory for service and server nodes seperatly //   
//   {
//     for(int serverNode_cnt=0;serverNode_cnt<length(serverNodes);serverNode_cnt++)
//       aNodes(serverNodes(serverNode_cnt)).allocLinkInfo(serviceNodes);
//     for(int serviceNode_cnt=0;serviceNode_cnt<length(serviceNodes);serviceNode_cnt++)
//       aNodes(serviceNodes(serviceNode_cnt)).allocLinkInfo(serverNodes);
//   }
//   
//   //! Actual linking begins//
//   int isDownLink=-1;
//   for(int serviceNode_cnt=0;serviceNode_cnt<length(serviceNodes);serviceNode_cnt++)//! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
//   {
//     
// //     if(aNodes(serviceNodes(serviceNode_cnt)).getLinkCount()!=0)
// //       aNodes(serviceNodes(serviceNode_cnt)).retainLinkInfo();
// //     
//       for(int serverNode_cnt=0;serverNode_cnt<length(serverNodes);serverNode_cnt++)
//       {
//           //! Assuming serverNodes as Transmitters
//           isDownLink=1;
//           {
//               if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
//               {
//                   int refBSid = nSectorsPerSite*aNodes(serverNodes(serverNode_cnt)).getCellID();
//                   if(serverNodes(serverNode_cnt) == refBSid)
//                   {
//                     for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)//! Get the location of hexagonal cell since it(servernode) is taken as transmitter from which we get transmitter cell ID//
//                       if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(serverNode_cnt)).getCellID())//! If the hexnode cell ID is server node cell ID //
//                           txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();
//                       
//                       if(aNodes(serviceNodes(serviceNode_cnt)).getCellID()!=aNodes(serverNodes(serverNode_cnt)).getCellID())//! If server node cell ID is not equal to service node cellID implies both are not in the same cell//
//                       {
//                           for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)//! For all hexnodes if some hexnode cellID which is equal to servicenode cellID there will be a wraparound of tx with respect to neighboring cells////
//                               if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(serviceNode_cnt)).getCellID())
//                                   wrapTxLoc=WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serverNodes(serverNode_cnt)).getCellID()),txCellLoc,aNodes(serverNodes(serverNode_cnt)).getLoc());
//                               aNodes(serverNodes(serverNode_cnt)).setWrapLoc(wrapTxLoc,true);//! Setting the wrap around location//
//                       }
//                       else   //! Else if server and service node are in the same cell there is no wraparound with respect to that cell wraparound will give the samecell so get the location of same cell //
//                           aNodes(serverNodes(serverNode_cnt)).setWrapLoc(aNodes(serverNodes(serverNode_cnt)).getLoc(),true);
//                       
//                       isLinkAltered=!(serviceNodeLinkInfoStatus(serviceNode_cnt)*serverNodeLinkInfoStatus(serverNode_cnt));
//                       aNodes(serviceNodes(serviceNode_cnt)).linkToTXNode(aNodes(serverNodes(serverNode_cnt)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//! This gives the link details w.r.t the transmitter//
//                   }
//                   else
//                   {
//                       copyLinkInfo(serviceNodes(serviceNode_cnt),serverNodes(serverNode_cnt),aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(refBSid));
//                   }
//                       rxGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenodes with the same loc(name) and getting thier IDs//
//                       txGroupID=findGroup(aNodes(serverNodes(serverNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernodes with the same loc(name) and getting thier IDs//
//                       
//                       if(rxGroupID(0)!=-1)//! Receiver grouping is done ,if it is the first node in the group ,set the properties as defined by linkto tx node//
//                       {
//                           if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0) 
//                           {
//                               aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
//                               aNodeGroups(rxGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt);
//                           }
//                           else   //! Else if there exits a node before then set the properties of new node to the properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
//                           {
//                               tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serverNodes(serverNode_cnt));  
//                               aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
//                               aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
//                               aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"isLOS",tempLinkInfo.isLOS);
//                               if(myTerrain.isForSCMChannel)
//                                   aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),tempLinkInfo.scmLargeScaleParams);
//                           }
//                       }
//                       if(txGroupID(0)!=-1)//! Transmitter grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
//                       {	
//                           if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
//                           {
//                               aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
//                               aNodeGroups(txGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt);
//                           }
//                           else    //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
//                           {
//                               tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
//                               aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
//                               aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
//                               aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"isLOS",tempLinkInfo.isLOS);
//                               if(myTerrain.isForSCMChannel) //! If there is scm channel//
//                                   aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),tempLinkInfo.scmLargeScaleParams);
//                           }
//                       }
//               }
//               else if(cellLayout==_CELLLAYOUT_RANDOM_ && isInHNodeType)
//               {
//                   int refBSid = nSectorsPerSite*aNodes(serverNodes(serverNode_cnt)).getCellID();
//                   if(serverNodes(serverNode_cnt) == refBSid)
//                   {
//                         txCellLoc=aNodes(serverNodes(serverNode_cnt)).getLoc();
//                   /*if(aNodes(serviceNodes(serviceNode_cnt)).getCellID()!=aNodes(serverNodes(serverNode_cnt)).getCellID())//! If server node cell ID is not equal to service node cellID implies both are not in the same cell//
//                    *	  {
//                    *	    for(int inHNode_cnt=0;inHNode_cnt<length(inHNodes);inHNode_cnt++)//! For all hexnodes if some hexnode cellID which is equal to servicenode cellID there will be a wraparound of tx with respect to neighboring cells////
//                    *	      if(aNodes(inHNodes(inHNode_cnt)).getCellID()==aNodes(serviceNodes(serviceNode_cnt)).getCellID())
//                    *		wrapTxLoc=WrapMyLocation(aNodes(inHNodes(inHNode_cnt)).getNeighbourCellLoc(aNodes(serverNodes(serverNode_cnt)).getCellID()),txCellLoc,aNodes(serverNodes(serverNode_cnt)).getLoc());
//                    *	      aNodes(serverNodes(serverNode_cnt)).setWrapLoc(wrapTxLoc,true);//! Setting the wrap around location//
//               }
//               else */  //! Else if server and service node are in the same cell there is no wraparound with respect to that cell wraparound will give the samecell so get the location of same cell //
//                         aNodes(serverNodes(serverNode_cnt)).setWrapLoc(aNodes(serverNodes(serverNode_cnt)).getLoc(),true);
//                         isLinkAltered=!(serviceNodeLinkInfoStatus(serviceNode_cnt)*serverNodeLinkInfoStatus(serverNode_cnt));
//                         aNodes(serviceNodes(serviceNode_cnt)).linkToTXNode(aNodes(serverNodes(serverNode_cnt)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//! This gives the link details w.r.t the transmitter//
//                   }
//                   else
//                   {
//                       copyLinkInfo(serviceNodes(serviceNode_cnt),serverNodes(serverNode_cnt),aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(refBSid));
//                   }
//               rxGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenodes with the same loc(name) and getting thier IDs//
//               txGroupID=findGroup(aNodes(serverNodes(serverNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernodes with the same loc(name) and getting thier IDs//
//               
//               if(rxGroupID(0)!=-1)//! Receiver grouping is done ,if it is the first node in the group ,set the properties as defined by linkto tx node//
//               {
//                   if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0) 
//                   {
//                       aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
//                       aNodeGroups(rxGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt);
//                   }
//                   else   //! Else if there exits a node before then set the properties of new node to the properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
//                   {
//                       tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serverNodes(serverNode_cnt));  
//                       aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
//                       aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
//                       aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"isLOS",tempLinkInfo.isLOS);
//                       if(myTerrain.isForSCMChannel)
//                           aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),tempLinkInfo.scmLargeScaleParams);
//                   }
//               }
//               if(txGroupID(0)!=-1)//! Transmitter grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
//               {	
//                   if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
//                   {
//                       aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
//                       aNodeGroups(txGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt);
//                   }
//                   else    //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
//                   {
//                       tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
//                       aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
//                       aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
//                       aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),"isLOS",tempLinkInfo.isLOS);
//                       if(myTerrain.isForSCMChannel) //! If there is scm channel//
//                           aNodes(serviceNodes(serviceNode_cnt)).overwriteLinkInfo(serverNodes(serverNode_cnt),tempLinkInfo.scmLargeScaleParams);
//                   }
//               }
//               }	
//               else  //! If the cell layout is random //
//               {
//                   aNodes(serviceNodes(serviceNode_cnt)).linkToTXNode(aNodes(serverNodes(serverNode_cnt)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//get the linkinfo parameters directly by using linkToTXNode//
//               }
//           }
//           //! Assuming serverNodes as Receivers and service nodes as transmitters//
//           isDownLink=0;
//           {
//               //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
//               if(cellLayout==_CELLLAYOUT_HEXAGONAL_)   //! Setting the service node location as transmitter location for all hexagonal nodes//
//               {
//                   int refBSid = nSectorsPerSite*aNodes(serverNodes(serverNode_cnt)).getCellID();
//                   if(serverNodes(serverNode_cnt)==refBSid)
//                   {
//                   for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)//
//                       if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(serviceNode_cnt)).getCellID())
//                           txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();
//                       
//                       //! If the server node and service node are in  different cells then finding the wraparound location of service nodes with respect to servernodes //// 
//                       
//                       if(aNodes(serverNodes(serverNode_cnt)).getCellID()!=aNodes(serviceNodes(serviceNode_cnt)).getCellID())
//                       {
//                           for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++) 
//                               if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(serverNode_cnt)).getCellID())
//                                   wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serviceNodes(serviceNode_cnt)).getCellID()),txCellLoc,aNodes(serviceNodes(serviceNode_cnt)).getLoc());
//                               aNodes(serviceNodes(serviceNode_cnt)).setWrapLoc(wrapTxLoc,true);
//                       }
//                       else //! Else if server and service nodes are in same cell implies the wrap around location will be the same cell location////
//                           aNodes(serviceNodes(serviceNode_cnt)).setWrapLoc(aNodes(serviceNodes(serviceNode_cnt)).getLoc(),true);
//                       isLinkAltered=!(serverNodeLinkInfoStatus(serverNode_cnt)*serviceNodeLinkInfoStatus(serviceNode_cnt));
//                       aNodes(serverNodes(serverNode_cnt)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//! Getting the parameters of link info status of to the transmitter by this function//
//                   }
//                   else
//                   {
//                       copyLinkInfo(serverNodes(serverNode_cnt),serviceNodes(serviceNode_cnt),aNodes(refBSid).getLinkInfo(serviceNodes(serviceNode_cnt)));
//                   }
//                       // Overwriting PL,SL and isLOS to make the forward and reverse links identical ...//since it isuplink the properties of a link will be same as that of uplink// 
//                       tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(serverNodes(serverNode_cnt));//! Get the linkinfo report of the servicenode in downlink to feed in linkinfo report of server node in uplink//
//                       aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
//                       aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
//                       aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
//                       if(myTerrain.isForSCMChannel) //! In the presence of system terrain of SCM channel//
//                           aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);//! Overwrite scmlargeScaleParams in the uplink for servicenode//
//                           
//                           rxGroupID=findGroup(aNodes(serverNodes(serverNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernodes with the same loc(name) and getting thier IDs
//                           txGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenode with the same loc(name) and getting thier IDs//
//                           
//                           if(rxGroupID(0)!=-1) //! Receiver grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
//                           {
//                               if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
//                               {
//                                   aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
//                                   aNodeGroups(rxGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt);
//                               }
//                               else //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
//                               {
//                                   tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serviceNodes(serviceNode_cnt));
//                                   aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
//                                   aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
//                                   aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
//                                   if(myTerrain.isForSCMChannel) 
//                                       aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);
//                               }
//                           }
//                           if(txGroupID(0)!=-1) //! Transmitter grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
//                           {
//                               if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0) 
//                               {
//                                   aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
//                                   aNodeGroups(txGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt);
//                               }
//                               else  //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
//                               {
//                                   tempLinkInfo=aNodes(serverNodes(serverNode_cnt)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0)); //! Getting the linkinfo of the first node//
//                                   aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
//                                   aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
//                                   aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
//                                   if(myTerrain.isForSCMChannel)
//                                       aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);
//                               }
//                           }
//               }
//               else if(cellLayout==_CELLLAYOUT_RANDOM_ && isInHNodeType)   //! Setting the service node location as transmitter location for all hexagonal nodes//
//               {
//                   int refBSid = nSectorsPerSite*aNodes(serverNodes(serverNode_cnt)).getCellID();
//                   if(serverNodes(serverNode_cnt)==refBSid)
//                   {
//                   // 	  for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)//
//                   // 	    if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(serviceNode_cnt)).getCellID())
//                         txCellLoc=aNodes(serviceNodes(serviceNode_cnt)).getLoc();
//                   
//                   //! If the server node and service node are in  different cells then finding the wraparound location of service nodes with respect to servernodes //// 
//                   /*if(aNodes(serverNodes(serverNode_cnt)).getCellID()!=aNodes(serviceNodes(serviceNode_cnt)).getCellID())
//                    *	  {
//                    *	    for(int inHNode_cnt=0;inHNode_cnt<length(inHNodes);inHNode_cnt++) 
//                    *	      if(aNodes(inHNodes(inHNode_cnt)).getCellID()==aNodes(serverNodes(serverNode_cnt)).getCellID())
//                    *		wrapTxLoc = WrapMyLocation(aNodes(inHNodes(inHNode_cnt)).getNeighbourCellLoc(aNodes(serviceNodes(serviceNode_cnt)).getCellID()),txCellLoc,aNodes(serviceNodes(serviceNode_cnt)).getLoc());
//                    *	      aNodes(serviceNodes(serviceNode_cnt)).setWrapLoc(wrapTxLoc,true);
//               }
//               else //! Else if server and service nodes are in same cell implies the wrap around location will be the same cell location////
//                   */  
//                   
//                         aNodes(serviceNodes(serviceNode_cnt)).setWrapLoc(aNodes(serviceNodes(serviceNode_cnt)).getLoc(),true);
//                         isLinkAltered=!(serverNodeLinkInfoStatus(serverNode_cnt)*serviceNodeLinkInfoStatus(serviceNode_cnt));
//                         aNodes(serverNodes(serverNode_cnt)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//! Getting the parameters of link info status of to the transmitter by this function//
//                   }
//                   else
//                   {
//                       copyLinkInfo(serverNodes(serverNode_cnt),serviceNodes(serviceNode_cnt),aNodes(refBSid).getLinkInfo(serviceNodes(serviceNode_cnt)));
//                   }
//               // Overwriting PL,SL and isLOS to make the forward and reverse links identical ...//since it isuplink the properties of a link will be same as that of uplink// 
//               tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(serverNodes(serverNode_cnt));//! Get the linkinfo report of the servicenode in downlink to feed in linkinfo report of server node in uplink//
//               aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
//               aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
//               aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
//               if(myTerrain.isForSCMChannel) //! In the presence of system terrain of SCM channel//
//                   aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);//! Overwrite scmlargeScaleParams in the uplink for servicenode//
//                   
//                   rxGroupID=findGroup(aNodes(serverNodes(serverNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernodes with the same loc(name) and getting thier IDs
//                   txGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenode with the same loc(name) and getting thier IDs//
//                   
//                   if(rxGroupID(0)!=-1) //! Receiver grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
//                   {
//                       if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
//                       {
//                           aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
//                           aNodeGroups(rxGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt);
//                       }
//                       else //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
//                       {
//                           tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serviceNodes(serviceNode_cnt));
//                           aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
//                           aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
//                           aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
//                           if(myTerrain.isForSCMChannel) 
//                               aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);
//                       }
//                   }
//                   if(txGroupID(0)!=-1) //! Transmitter grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
//                   {
//                       if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0) 
//                       {
//                           aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
//                           aNodeGroups(txGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt);
//                       }
//                       else  //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
//                       {
//                           tempLinkInfo=aNodes(serverNodes(serverNode_cnt)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0)); //! Getting the linkinfo of the first node//
//                           aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
//                           aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
//                           aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
//                           if(myTerrain.isForSCMChannel)
//                               aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);
//                       }
//                   }
//               }
//               else //! If the cell layout is random //
//               {
//                   aNodes(serverNodes(serverNode_cnt)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);
//                   
//                   //Overwriting PL,SL and isLOS to make the forward and reverse links identical ...
//                   tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(serverNodes(serverNode_cnt));
//                   aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
//                   aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
//                   aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),"isLOS",tempLinkInfo.isLOS);
//                   if(myTerrain.isForSCMChannel)
//                       aNodes(serverNodes(serverNode_cnt)).overwriteLinkInfo(serviceNodes(serviceNode_cnt),tempLinkInfo.scmLargeScaleParams);
//               }
//           }
//       }
//   }
// 
// 
// //  ///CrossLinks - Used for CoExistence Study / TDD interference analysis
// //  if(isCrossLinksEnabled)//! If crosslink is enabled there is a combination of service -service node,server-server node links//
// //  {
// //    cout<<"[detl:]Creating CrossLinks."<<endl;
// //    //! ServiceNode-ServiceNode Links//
// //    //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
// //    for(int serviceNode_cnt1=0;serviceNode_cnt1<length(serviceNodes);serviceNode_cnt1++)
// //    {
// //      for(int serviceNode_cnt2=serviceNode_cnt1;serviceNode_cnt2<length(serviceNodes);serviceNode_cnt2++)
// //      {
// //	//! Assuming  serviceNode_cnt1 as Transmitters  and serviceNode_cnt2 as receivers
// //	{
// //	  if(cellLayout==_CELLLAYOUT_HEXAGONAL_) //! If cell layout is hexagonal setting the serviceNode_cnt1 location as transmitter location//
// //	  {
// //	    for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
// //	      if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(serviceNode_cnt1)).getCellID())
// //		txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();
//                
// //	      //! If the serviceNode_cnt1 node and serviceNode_cnt2 are in different cells then getting  the wrap around location of serviceNode_cnt1 w.r.t serviceNode_cnt2//
// //	      if(aNodes(serviceNodes(serviceNode_cnt1)).getCellID()!=aNodes(serviceNodes(serviceNode_cnt2)).getCellID())//! If service node cnt_1 cell ID is not equal to service node_cnt2 cellID implies both are not in the same cell//
// //	      {
// //		for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
// //		  if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(serviceNode_cnt2)).getCellID())
// //		    wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serviceNodes(serviceNode_cnt1)).getCellID()),txCellLoc,aNodes(serviceNodes(serviceNode_cnt1)).getLoc()); //there will be a wraparound of rx with respect to neighboring cells//
// //		  aNodes(serviceNodes(serviceNode_cnt1)).setWrapLoc(wrapTxLoc,true);
// //	      }
// //	      else //! Else if serviceNode_cnt1 and serviceNode_2 are in the same cell there is no wraparound with respect to that cell and location gives the same cell location//
// //		aNodes(serviceNodes(serviceNode_cnt1)).setWrapLoc(aNodes(serviceNodes(serviceNode_cnt1)).getLoc(),true);
// //	      isLinkAltered=!(serviceNodeLinkInfoStatus(serviceNode_cnt2)*serviceNodeLinkInfoStatus(serviceNode_cnt1));
// //	      aNodes(serviceNodes(serviceNode_cnt2)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt1)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//this gives the link details w.r.t the receiver servicenode cnt2//
// 	      
// //	      rxGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt2)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenode_cnt2 with the same loc(name) and getting thier IDs//
// //	      txGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt1)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenode_cnt1 with the same loc(name) and getting thier IDs//
// 
// //	      if(rxGroupID(0)!=-1)//! Receiver grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
// //	      {
// //		if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
// //		{
// //		  aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
// //		  aNodeGroups(rxGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt2);
// //		}
// //		else //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
// //		{
// //		  tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serviceNodes(serviceNode_cnt1));
// 		  
// //		  aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"pathLoss",tempLinkInfo.pathLoss);
// //		  aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"shadowLoss",tempLinkInfo.shadowLoss);
// //		  aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"isLOS",tempLinkInfo.isLOS);
// //		  if(myTerrain.isForSCMChannel)
// //		    aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),tempLinkInfo.scmLargeScaleParams);
// 		  
// //		}
// //	      }
// //	      if(txGroupID(0)!=-1)//! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
// //	      {
// //		if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
// //		{
// //		  aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
// //		  aNodeGroups(txGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt1);
// //		}
// //		else  //! Else if there exits a node before then set the properties of new node to the properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
// //		{
// 
// //		  tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt2)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
// 		  
// //		  aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"pathLoss",tempLinkInfo.pathLoss);
// //		  aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"shadowLoss",tempLinkInfo.shadowLoss);
// //		  aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"isLOS",tempLinkInfo.isLOS);
// //		  if(myTerrain.isForSCMChannel)//! If there is SCMChannel//
// //		    aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),tempLinkInfo.scmLargeScaleParams);//! Setting the scmLargescale parameters from the first node to second//
// 		    
// //		}
// //	      }
// //	  }
// //	  else//! If cell layout is random//
// //	    aNodes(serviceNodes(serviceNode_cnt2)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt1)),myTerrain,shadowType,retailUnAlteredLinks,retainOldLOS);//! Getting the link to txnode info for that node////
// //	}
// 
//       
// //	//! Assuming  serviceNode_cnt1 as Transmitters  and serviceNode_cnt2 as receivers
// //	{
// //	  if(cellLayout==_CELLLAYOUT_HEXAGONAL_) //! If cell layout is hexagonal setting the serviceNode_cnt2 location as transmitter location//
// //	    {
// //	      for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
// //		if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(serviceNode_cnt2)).getCellID())
// //		  txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();
// 		
// //		//! If the serviceNode_cnt2 node and serviceNode_cnt1 are in different cells then getting  the wrap around location of serviceNode_cnt2 w.r.t serviceNode_cnt1//
// //		  if(aNodes(serviceNodes(serviceNode_cnt2)).getCellID()!=aNodes(serviceNodes(serviceNode_cnt1)).getCellID())//! If service node cnt_1 cell ID is not equal to service node_cnt2 cellID implies both are not in the same cell//
// //		{
// //		  for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
// //		    if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(serviceNode_cnt1)).getCellID())
// //		      wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serviceNodes(serviceNode_cnt2)).getCellID()),txCellLoc,aNodes(serviceNodes(serviceNode_cnt2)).getLoc()); //there will be a wraparound of rx with respect to neighboring cells//
// //		      aNodes(serviceNodes(serviceNode_cnt2)).setWrapLoc(wrapTxLoc,true);
// //		}
// //		else //! Else if serviceNode_cnt2 and serviceNode_2 are in the same cell there is no wraparound with respect to that cell and location gives the same cell location//
// //		  aNodes(serviceNodes(serviceNode_cnt2)).setWrapLoc(aNodes(serviceNodes(serviceNode_cnt2)).getLoc(),true);
// //		isLinkAltered=!(serviceNodeLinkInfoStatus(serviceNode_cnt1)*serviceNodeLinkInfoStatus(serviceNode_cnt2));
// //		aNodes(serviceNodes(serviceNode_cnt1)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt2)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//this gives the link details w.r.t the receiver servicenode cnt2//
// 		
// //		// Overwriting PL,SL and isLOS to make the forward and reverse links identical ...//since it isuplink the properties of a link will be same as that of uplink//
// //		tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt2)).getLinkInfo(serviceNodes(serviceNode_cnt1));//! Get the linkinfo report of the servicenode in downlink to feed in linkinfo report of server node in uplink//
// //		aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"pathLoss",tempLinkInfo.pathLoss);
// //		aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"shadowLoss",tempLinkInfo.shadowLoss);
// //		aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"isLOS",tempLinkInfo.isLOS);
// //		if(myTerrain.isForSCMChannel) //! In the presence of system terrain of SCM channel//
// //		aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),tempLinkInfo.scmLargeScaleParams);//! Overwrite scmlargeScaleParams in the uplink for servicenode//
// 		
// //		rxGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt1)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenode_cnt2 with the same loc(name) and getting thier IDs//
// //		txGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt2)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenode_cnt1 with the same loc(name) and getting thier IDs//
// 		
// //		if(rxGroupID(0)!=-1)//! Receiver grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
// //		{
// //		  if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
// //		  {
// //		    aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
// //		    aNodeGroups(rxGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt1);
// //		  }
// //		  else //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
// //		  {
// //		    tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serviceNodes(serviceNode_cnt2));
// 		    
// //		    aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"pathLoss",tempLinkInfo.pathLoss);
// //		    aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"shadowLoss",tempLinkInfo.shadowLoss);
// //		    aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"isLOS",tempLinkInfo.isLOS);
// //		    if(myTerrain.isForSCMChannel)
// //		      aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),tempLinkInfo.scmLargeScaleParams);
// //		  }
// //		}
// //		if(txGroupID(0)!=-1)//! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
// //		{
// //		  if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
// //		  {
// //		    aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
// //		    aNodeGroups(txGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt2);
// //		  }
// //		  else  //! Else if there exits a node before then set the properties of new node to the properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
// //		  {
// 		    
// //		    tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt1)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
// 		    
// //		    aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"pathLoss",tempLinkInfo.pathLoss);
// //		    aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"shadowLoss",tempLinkInfo.shadowLoss);
// //		    aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"isLOS",tempLinkInfo.isLOS);
// //		    if(myTerrain.isForSCMChannel)//! If there is SCMChannel//
// //		      aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),tempLinkInfo.scmLargeScaleParams);//! Setting the scmLargescale parameters from the first node to second//
// 		      
// //		  }
// //		}
// //	    }
// //	    else//! If cell layout is random//
// //	      aNodes(serviceNodes(serviceNode_cnt1)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt2)),myTerrain,shadowType,retailUnAlteredLinks,retainOldLOS);//! Getting the link to txnode info for that node////
// //	}
//       
// //      }
// 
// //    }
// 
// //    //! ServerNode-ServerNode Links //
//    
// //  //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
// //    for(int serverNode_cnt1=0;serverNode_cnt1<length(serverNodes);serverNode_cnt1++)
// //    {
// //      for(int serverNode_cnt2=serverNode_cnt1;serverNode_cnt2<length(serverNodes);serverNode_cnt2++)
// //      {
// //	//! Assuming  serverNode_cnt1 as Transmitters and serverNode_cnt2 as receivers
// //	{
// //	  if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
// //	  {
// //	    for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)//! For all hexagonal nodes setting the serverNode_cnt1 location as transmitter location//
// //	      if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(serverNode_cnt1)).getCellID())
// //		txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();
//              
// //	      //!  If the serverNode_cnt1 node and serverNode_cnt2 are in different cells then getting  the wrap around location of serverNode_cnt1 w.r.t serverNode_cnt2//
// //	      if(aNodes(serverNodes(serverNode_cnt1)).getCellID()!=aNodes(serverNodes(serverNode_cnt2)).getCellID())
// //	      {
// //		for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
// //		  if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(serverNode_cnt2)).getCellID())
// //		    wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serverNodes(serverNode_cnt1)).getCellID()),txCellLoc,aNodes(serverNodes(serverNode_cnt1)).getLoc());
// //		  aNodes(serverNodes(serverNode_cnt1)).setWrapLoc(wrapTxLoc,true);
// //	      }
// //	      else//! If both server nodes are in the same cell there is no wraparound with respect to that cell and wrap around location will be the samecell location////
// //		aNodes(serverNodes(serverNode_cnt1)).setWrapLoc(aNodes(serverNodes(serverNode_cnt1)).getLoc(),true);
// //	      isLinkAltered=!(serverNodeLinkInfoStatus(serverNode_cnt2)*serverNodeLinkInfoStatus(serverNode_cnt1));
// //	      aNodes(serverNodes(serverNode_cnt2)).linkToTXNode(aNodes(serverNodes(serverNode_cnt1)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//! This gives the link details w.r.t the receiver//
// 	      
// //	      rxGroupID=findGroup(aNodes(serverNodes(serverNode_cnt2)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernode_cnt2 with the same loc(name) and getting thier IDs//
// //	      txGroupID=findGroup(aNodes(serverNodes(serverNode_cnt1)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernode_cnt1 with the same loc(name) and getting thier IDs//
// 
// //	      if(rxGroupID(0)!=-1)//! Receiver grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
// //	      {
// //		if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
// //		{
// //		  aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
// //		  aNodeGroups(rxGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt2);
// //		}
// //		else //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
// //		{
// //		  tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serverNodes(serverNode_cnt1));
// 		 
// //		  aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),"pathLoss",tempLinkInfo.pathLoss);
// //		  aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),"shadowLoss",tempLinkInfo.shadowLoss);
// //		  aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),"isLOS",tempLinkInfo.isLOS);
// 
// //		  if(myTerrain.isForSCMChannel)
// //		    aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),tempLinkInfo.scmLargeScaleParams);//! Setting the scmLargescale parameters from the first node to remainingnodes//
// 		    
// //		}
// //	      }
// //	      if(txGroupID(0)!=-1)//! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
// //	      {
// //		if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
// //		{
// //		  aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
// //		  aNodeGroups(txGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt1);
// //		}
// //		else  //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
// //		{
// 
// //		  tempLinkInfo=aNodes(serverNodes(serverNode_cnt2)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
// 		  
// //		  aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),"pathLoss",tempLinkInfo.pathLoss);
// //		  aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),"shadowLoss",tempLinkInfo.shadowLoss);
// //		  aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),"isLOS",tempLinkInfo.isLOS);
// //		  if(myTerrain.isForSCMChannel)
// //		    aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),tempLinkInfo.scmLargeScaleParams);
// 		  
// //		}
// //	      }
// 	      
// //	  }
// //	  else //! If cell layout is random//
// //	    aNodes(serverNodes(serverNode_cnt2)).linkToTXNode(aNodes(serverNodes(serverNode_cnt1)),myTerrain,shadowType,retailUnAlteredLinks,retainOldLOS);//getting the link to txnode info for that node////
// //	}
// 
//       
// //      //! Assuming  serverNode_cnt2 as Transmitters and serverNode_cnt1 as receivers
// //      {
// //	if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
// //	{
// //	  for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)//! For all hexagonal nodes setting the serverNode_cnt2 location as transmitter location//
// //	      if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(serverNode_cnt2)).getCellID())
// //		txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();
// 	      
// //	      //!  If the serverNode_cnt2 node and serverNode_cnt1 are in different cells then getting  the wrap around location of serverNode_cnt2 w.r.t serverNode_cnt1//
// //		if(aNodes(serverNodes(serverNode_cnt2)).getCellID()!=aNodes(serverNodes(serverNode_cnt1)).getCellID())
// //		{
// //		  for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
// //		    if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(serverNode_cnt1)).getCellID())
// //		      wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serverNodes(serverNode_cnt2)).getCellID()),txCellLoc,aNodes(serverNodes(serverNode_cnt2)).getLoc());
// //		    aNodes(serverNodes(serverNode_cnt2)).setWrapLoc(wrapTxLoc,true);
// //		}
// //		else//! If both server nodes are in the same cell there is no wraparound with respect to that cell and wrap around location will be the samecell location////
// //		aNodes(serverNodes(serverNode_cnt2)).setWrapLoc(aNodes(serverNodes(serverNode_cnt2)).getLoc(),true);
// //		isLinkAltered=!(serverNodeLinkInfoStatus(serverNode_cnt1)*serverNodeLinkInfoStatus(serverNode_cnt2));
// //		aNodes(serverNodes(serverNode_cnt1)).linkToTXNode(aNodes(serverNodes(serverNode_cnt2)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//! This gives the link details w.r.t the receiver//
// 		
// //		// Overwriting PL,SL and isLOS to make the forward and reverse links identical ...
// //		tempLinkInfo=aNodes(serverNodes(serverNode_cnt2)).getLinkInfo(serverNodes(serverNode_cnt1));
// //		aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"pathLoss",tempLinkInfo.pathLoss);
// //		aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"shadowLoss",tempLinkInfo.shadowLoss);
// //		aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"isLOS",tempLinkInfo.isLOS);
// 		
// //		if(myTerrain.isForSCMChannel)
// //		  aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),tempLinkInfo.scmLargeScaleParams);//! Setting the scmLargescale parameters from the first node to remainingnodes//
// 		  
// //		rxGroupID=findGroup(aNodes(serverNodes(serverNode_cnt1)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernode_cnt2 with the same loc(name) and getting thier IDs//
// //		txGroupID=findGroup(aNodes(serverNodes(serverNode_cnt2)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernode_cnt1 with the same loc(name) and getting thier IDs//
// 		
// //		if(rxGroupID(0)!=-1)//! Receiver grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
// //	      {
// //		if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
// //		{
// //		  aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
// //		  aNodeGroups(rxGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt1);
// //		}
// //		else //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
// //		{
// //		  tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serverNodes(serverNode_cnt2));
// 		  
// //		  aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"pathLoss",tempLinkInfo.pathLoss);
// //		  aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"shadowLoss",tempLinkInfo.shadowLoss);
// //		  aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"isLOS",tempLinkInfo.isLOS);
// 		  
// //		  if(myTerrain.isForSCMChannel)
// //		    aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),tempLinkInfo.scmLargeScaleParams);//! Setting the scmLargescale parameters from the first node to remainingnodes//
// 		    
// //		}
// //	      }
// //	      if(txGroupID(0)!=-1)//! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
// //	      {
// //		if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
// //		{
// //		  aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
// //		  aNodeGroups(txGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt2);
// //		}
// //		else  //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
// //		{
// 		  
// //		  tempLinkInfo=aNodes(serverNodes(serverNode_cnt1)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
// 		  
// //		  aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"pathLoss",tempLinkInfo.pathLoss);
// //		  aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"shadowLoss",tempLinkInfo.shadowLoss);
// //		  aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"isLOS",tempLinkInfo.isLOS);
// //		  if(myTerrain.isForSCMChannel)
// //		    aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),tempLinkInfo.scmLargeScaleParams);
// 		  
// //		}
// //	      }
// 	      
// //	}
// //	else //! If cell layout is random//
// //	    aNodes(serverNodes(serverNode_cnt1)).linkToTXNode(aNodes(serverNodes(serverNode_cnt2)),myTerrain,shadowType,retailUnAlteredLinks,retainOldLOS);//getting the link to txnode info for that node////
// //      }
//       
// //      }
// 
// //    }
// //  }
//   
// #ifdef USING_MPI
// if(currentTaskid==0) {
// #endif
//   cout<<"[detl:]LinkInfo generated."<<endl;
// #ifdef USING_MPI
// }
// #endif
// }

void McellSystem::generateCrossLinkInfo(bool retailUnAlteredLinks, bool retainOldLOS){

  generateCrossLinkInfo(sysTerrain,retailUnAlteredLinks,retainOldLOS);
}
/*!\fn generateCrossLinkInfo()
 * \brief--> It will generate the links between the nodes of different combinations server and service nodes and get the properties of those links
 *\param[in,out] myTerrain --> it will take the input parameter of terrain class and get modified and return it
 *\param[in] retailUnAlteredLinks --> it is boolean parameter which retains the unaltered links when function is called again
 *\param[in] retainOldLOS --> it is a boolean parameter which retains the OldLOS when function is called again
 */
void McellSystem::generateCrossLinkInfo(Terrain &myTerrain, bool retailUnAlteredLinks, bool retainOldLOS){

  if(!isSysReady())          //! checking whether system is ready or not//
  {
    cout<<"[both:]System Not Fully Configured.";
    abort();
  }

  ivec activeNodes=getActiveNodes();   //! Getting the nodes which are active//
  if(length(activeNodes)==0)
  {
    cout<<"[detl:]Making all nodes active as no Node in the system is activated."<<endl; //! Making all the nodes active//
    activateNodes();
  }
  ivec serverNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));
  ivec serviceNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));
  append(activeNodes,serverNodes);
  for(int activeNode_cnt=0;activeNode_cnt<activeNodes.length();activeNode_cnt++)
  {
    aNodes(activeNodes(activeNode_cnt)).setLinkInfoUpdateStatus(true);
    aNodes(activeNodes(activeNode_cnt)).txAntenna.setParameterUpdateStatus(true);
    aNodes(activeNodes(activeNode_cnt)).rxAntenna.setParameterUpdateStatus(true);
  }
  ivec hexNodes(0);
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
  {
    int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
    hexNodes=getNodes(aNodeTypes(hexDropNodeType));
  }
  int rndhexDropNodeType=find(nodeDropMethods,_DROPMETHOD_RANDOMHEX_);
  
  ivec rxGroupID,txGroupID;
  NodeLinkInfo_S tempLinkInfo;
  Location_S wrapTxLoc,txCellLoc;
  bool isLinkAltered=true;

  ///CrossLinks - Used for CoExistence Study / TDD interference analysis
  if(isCrossLinksEnabled)//! If crosslink is enabled there is a combination of service -service node,server-server node links//
  {
    cout<<"[detl:]Creating CrossLinks."<<endl;
    //! ServiceNode-ServiceNode Links//
    //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
    for(int serviceNode_cnt1=0;serviceNode_cnt1<length(serviceNodes);serviceNode_cnt1++)
    {
      for(int serviceNode_cnt2=serviceNode_cnt1;serviceNode_cnt2<length(serviceNodes);serviceNode_cnt2++)
      {
    //! Assuming  serviceNode_cnt1 as Transmitters  and serviceNode_cnt2 as receivers
    {
      if(cellLayout==_CELLLAYOUT_HEXAGONAL_) //! If cell layout is hexagonal setting the serviceNode_cnt1 location as transmitter location//
      {
        for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
          if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(serviceNode_cnt1)).getCellID())
        txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();

          //! If the serviceNode_cnt1 node and serviceNode_cnt2 are in different cells then getting  the wrap around location of serviceNode_cnt1 w.r.t serviceNode_cnt2//
        if(aNodes(serviceNodes(serviceNode_cnt1)).getCellID()!=aNodes(serviceNodes(serviceNode_cnt2)).getCellID())//! If service node cnt_1 cell ID is not equal to service node_cnt2 cellID implies both are not in the same cell//
          {
        for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
          if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(serviceNode_cnt2)).getCellID())
            wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serviceNodes(serviceNode_cnt1)).getCellID()),txCellLoc,aNodes(serviceNodes(serviceNode_cnt1)).getLoc()); //there will be a wraparound of rx with respect to neighboring cells//
            aNodes(serviceNodes(serviceNode_cnt1)).setWrapLoc(wrapTxLoc,true);
          }
          else //! Else if serviceNode_cnt1 and serviceNode_2 are in the same cell there is no wraparound with respect to that cell and location gives the same cell location//
        aNodes(serviceNodes(serviceNode_cnt1)).setWrapLoc(aNodes(serviceNodes(serviceNode_cnt1)).getLoc(),true);
          aNodes(serviceNodes(serviceNode_cnt2)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt1)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//this gives the link details w.r.t the receiver servicenode cnt2//

          rxGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt2)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenode_cnt2 with the same loc(name) and getting thier IDs//
          txGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt1)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenode_cnt1 with the same loc(name) and getting thier IDs//

          if(rxGroupID(0)!=-1)//! Receiver grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
          {
        if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
        {
          aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
          aNodeGroups(rxGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt2);
        }
        else //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
        {
          tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serviceNodes(serviceNode_cnt1));

          aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"pathLoss",tempLinkInfo.pathLoss);
          aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"shadowLoss",tempLinkInfo.shadowLoss);
          aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"isLOS",tempLinkInfo.isLOS);
          if(myTerrain.isForSCMChannel)
            aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),tempLinkInfo.scmLargeScaleParams);

        }
          }
          if(txGroupID(0)!=-1)//! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
          {
        if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
        {
          aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
          aNodeGroups(txGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt1);
        }
        else  //! Else if there exits a node before then set the properties of new node to the properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
        {

          tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt2)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));

          aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"pathLoss",tempLinkInfo.pathLoss);
          aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"shadowLoss",tempLinkInfo.shadowLoss);
          aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"isLOS",tempLinkInfo.isLOS);
          if(myTerrain.isForSCMChannel)//! If there is SCMChannel//
            aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),tempLinkInfo.scmLargeScaleParams);//! Setting the scmLargescale parameters from the first node to second//

        }
          }
      }
      else if(cellLayout==_CELLLAYOUT_RANDOM_ && rndhexDropNodeType!=-1) //! If cell layout is hexagonal setting the serviceNode_cnt1 location as transmitter location//
      {
        ivec rndhexNodes = getNodes(aNodeTypes(rndhexDropNodeType));
    
         aNodes(serviceNodes(serviceNode_cnt1)).setWrapLoc(aNodes(serviceNodes(serviceNode_cnt1)).getLoc(),true);
          aNodes(serviceNodes(serviceNode_cnt2)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt1)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//this gives the link details w.r.t the receiver servicenode cnt2//

          rxGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt2)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenode_cnt2 with the same loc(name) and getting thier IDs//
          txGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt1)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenode_cnt1 with the same loc(name) and getting thier IDs//

          if(rxGroupID(0)!=-1)//! Receiver grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
          {
        if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
        {
          aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
          aNodeGroups(rxGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt2);
        }
        else //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
        {
          tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serviceNodes(serviceNode_cnt1));

          aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"pathLoss",tempLinkInfo.pathLoss);
          aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"shadowLoss",tempLinkInfo.shadowLoss);
          aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"isLOS",tempLinkInfo.isLOS);
          if(myTerrain.isForSCMChannel)
            aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),tempLinkInfo.scmLargeScaleParams);

        }
          }
          if(txGroupID(0)!=-1)//! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
          {
        if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
        {
          aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
          aNodeGroups(txGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt1);
        }
        else  //! Else if there exits a node before then set the properties of new node to the properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
        {

          tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt2)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));

          aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"pathLoss",tempLinkInfo.pathLoss);
          aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"shadowLoss",tempLinkInfo.shadowLoss);
          aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),"isLOS",tempLinkInfo.isLOS);
          if(myTerrain.isForSCMChannel)//! If there is SCMChannel//
            aNodes(serviceNodes(serviceNode_cnt2)).overwriteLinkInfo(serviceNodes(serviceNode_cnt1),tempLinkInfo.scmLargeScaleParams);//! Setting the scmLargescale parameters from the first node to second//

        }
          }
      }
      else//! If cell layout is random//
        aNodes(serviceNodes(serviceNode_cnt2)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt1)),myTerrain,shadowType,retailUnAlteredLinks,retainOldLOS);//! Getting the link to txnode info for that node////
    }


    //! Assuming  serviceNode_cnt1 as Transmitters  and serviceNode_cnt2 as receivers
    {
      if(cellLayout==_CELLLAYOUT_HEXAGONAL_) //! If cell layout is hexagonal setting the serviceNode_cnt2 location as transmitter location//
        {
          for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
        if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(serviceNode_cnt2)).getCellID())
          txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();

        //! If the serviceNode_cnt2 node and serviceNode_cnt1 are in different cells then getting  the wrap around location of serviceNode_cnt2 w.r.t serviceNode_cnt1//
          if(aNodes(serviceNodes(serviceNode_cnt2)).getCellID()!=aNodes(serviceNodes(serviceNode_cnt1)).getCellID())//! If service node cnt_1 cell ID is not equal to service node_cnt2 cellID implies both are not in the same cell//
        {
          for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
            if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(serviceNode_cnt1)).getCellID())
              wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serviceNodes(serviceNode_cnt2)).getCellID()),txCellLoc,aNodes(serviceNodes(serviceNode_cnt2)).getLoc()); //there will be a wraparound of rx with respect to neighboring cells//
              aNodes(serviceNodes(serviceNode_cnt2)).setWrapLoc(wrapTxLoc,true);
        }
        else //! Else if serviceNode_cnt2 and serviceNode_2 are in the same cell there is no wraparound with respect to that cell and location gives the same cell location//
          aNodes(serviceNodes(serviceNode_cnt2)).setWrapLoc(aNodes(serviceNodes(serviceNode_cnt2)).getLoc(),true);
        aNodes(serviceNodes(serviceNode_cnt1)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt2)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//this gives the link details w.r.t the receiver servicenode cnt2//

        // Overwriting PL,SL and isLOS to make the forward and reverse links identical ...//since it isuplink the properties of a link will be same as that of uplink//
        tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt2)).getLinkInfo(serviceNodes(serviceNode_cnt1));//! Get the linkinfo report of the servicenode in downlink to feed in linkinfo report of server node in uplink//
        aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"pathLoss",tempLinkInfo.pathLoss);
        aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"shadowLoss",tempLinkInfo.shadowLoss);
        aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"isLOS",tempLinkInfo.isLOS);
        if(myTerrain.isForSCMChannel) //! In the presence of system terrain of SCM channel//
        aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),tempLinkInfo.scmLargeScaleParams);//! Overwrite scmlargeScaleParams in the uplink for servicenode//

        rxGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt1)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenode_cnt2 with the same loc(name) and getting thier IDs//
        txGroupID=findGroup(aNodes(serviceNodes(serviceNode_cnt2)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenode_cnt1 with the same loc(name) and getting thier IDs//

        if(rxGroupID(0)!=-1)//! Receiver grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
        {
          if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
          {
            aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
            aNodeGroups(rxGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt1);
          }
          else //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
          {
            tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serviceNodes(serviceNode_cnt2));

            aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"pathLoss",tempLinkInfo.pathLoss);
            aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"shadowLoss",tempLinkInfo.shadowLoss);
            aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"isLOS",tempLinkInfo.isLOS);
            if(myTerrain.isForSCMChannel)
              aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),tempLinkInfo.scmLargeScaleParams);
          }
        }
        if(txGroupID(0)!=-1)//! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
        {
          if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
          {
            aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
            aNodeGroups(txGroupID(0)).nodeIDs(0)=serviceNodes(serviceNode_cnt2);
          }
          else  //! Else if there exits a node before then set the properties of new node to the properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
          {

            tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt1)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));

            aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"pathLoss",tempLinkInfo.pathLoss);
            aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"shadowLoss",tempLinkInfo.shadowLoss);
            aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),"isLOS",tempLinkInfo.isLOS);
            if(myTerrain.isForSCMChannel)//! If there is SCMChannel//
              aNodes(serviceNodes(serviceNode_cnt1)).overwriteLinkInfo(serviceNodes(serviceNode_cnt2),tempLinkInfo.scmLargeScaleParams);//! Setting the scmLargescale parameters from the first node to second//

          }
        }
        }
        else//! If cell layout is random//
          aNodes(serviceNodes(serviceNode_cnt1)).linkToTXNode(aNodes(serviceNodes(serviceNode_cnt2)),myTerrain,shadowType,retailUnAlteredLinks,retainOldLOS);//! Getting the link to txnode info for that node////
    }

      }

    }

    //! ServerNode-ServerNode Links //

    //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
    for(int serverNode_cnt1=0;serverNode_cnt1<length(serverNodes);serverNode_cnt1++)
    {
      for(int serverNode_cnt2=serverNode_cnt1;serverNode_cnt2<length(serverNodes);serverNode_cnt2++)
      {
    //! Assuming  serverNode_cnt1 as Transmitters and serverNode_cnt2 as receivers
    {
      if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
      {
        for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)//! For all hexagonal nodes setting the serverNode_cnt1 location as transmitter location//
          if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(serverNode_cnt1)).getCellID())
        txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();

          //!  If the serverNode_cnt1 node and serverNode_cnt2 are in different cells then getting  the wrap around location of serverNode_cnt1 w.r.t serverNode_cnt2//
        if(aNodes(serverNodes(serverNode_cnt1)).getCellID()!=aNodes(serverNodes(serverNode_cnt2)).getCellID())
        {
          for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
            if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(serverNode_cnt2)).getCellID())
              wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serverNodes(serverNode_cnt1)).getCellID()),txCellLoc,aNodes(serverNodes(serverNode_cnt1)).getLoc());
            aNodes(serverNodes(serverNode_cnt1)).setWrapLoc(wrapTxLoc,true);
        }
        else//! If both server nodes are in the same cell there is no wraparound with respect to that cell and wrap around location will be the samecell location////
        aNodes(serverNodes(serverNode_cnt1)).setWrapLoc(aNodes(serverNodes(serverNode_cnt1)).getLoc(),true);
        aNodes(serverNodes(serverNode_cnt2)).linkToTXNode(aNodes(serverNodes(serverNode_cnt1)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//! This gives the link details w.r.t the receiver//

        rxGroupID=findGroup(aNodes(serverNodes(serverNode_cnt2)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernode_cnt2 with the same loc(name) and getting thier IDs//
        txGroupID=findGroup(aNodes(serverNodes(serverNode_cnt1)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernode_cnt1 with the same loc(name) and getting thier IDs//

        if(rxGroupID(0)!=-1)//! Receiver grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
          {
        if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
        {
          aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
          aNodeGroups(rxGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt2);
        }
        else //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
        {
          tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serverNodes(serverNode_cnt1));

          aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),"pathLoss",tempLinkInfo.pathLoss);
          aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),"shadowLoss",tempLinkInfo.shadowLoss);
          aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),"isLOS",tempLinkInfo.isLOS);

          if(myTerrain.isForSCMChannel)
            aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),tempLinkInfo.scmLargeScaleParams);//! Setting the scmLargescale parameters from the first node to remainingnodes//

        }
          }
          if(txGroupID(0)!=-1)//! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
          {
        if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
        {
          aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
          aNodeGroups(txGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt1);
        }
        else  //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
        {

          tempLinkInfo=aNodes(serverNodes(serverNode_cnt2)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));

          aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),"pathLoss",tempLinkInfo.pathLoss);
          aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),"shadowLoss",tempLinkInfo.shadowLoss);
          aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),"isLOS",tempLinkInfo.isLOS);
          if(myTerrain.isForSCMChannel)
            aNodes(serverNodes(serverNode_cnt2)).overwriteLinkInfo(serverNodes(serverNode_cnt1),tempLinkInfo.scmLargeScaleParams);

        }
          }

      }
      else //! If cell layout is random//
        aNodes(serverNodes(serverNode_cnt2)).linkToTXNode(aNodes(serverNodes(serverNode_cnt1)),myTerrain,shadowType,retailUnAlteredLinks,retainOldLOS);//getting the link to txnode info for that node////
    }


    //! Assuming  serverNode_cnt2 as Transmitters and serverNode_cnt1 as receivers
    {
      if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
      {
        for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)//! For all hexagonal nodes setting the serverNode_cnt2 location as transmitter location//
          if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(serverNode_cnt2)).getCellID())
        txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();

          //!  If the serverNode_cnt2 node and serverNode_cnt1 are in different cells then getting  the wrap around location of serverNode_cnt2 w.r.t serverNode_cnt1//
        if(aNodes(serverNodes(serverNode_cnt2)).getCellID()!=aNodes(serverNodes(serverNode_cnt1)).getCellID())
        {
          for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
            if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(serverNode_cnt1)).getCellID())
              wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serverNodes(serverNode_cnt2)).getCellID()),txCellLoc,aNodes(serverNodes(serverNode_cnt2)).getLoc());
            aNodes(serverNodes(serverNode_cnt2)).setWrapLoc(wrapTxLoc,true);
        }
        else//! If both server nodes are in the same cell there is no wraparound with respect to that cell and wrap around location will be the samecell location////
        aNodes(serverNodes(serverNode_cnt2)).setWrapLoc(aNodes(serverNodes(serverNode_cnt2)).getLoc(),true);
        aNodes(serverNodes(serverNode_cnt1)).linkToTXNode(aNodes(serverNodes(serverNode_cnt2)),myTerrain,shadowType,isLinkAltered,retailUnAlteredLinks,retainOldLOS);//! This gives the link details w.r.t the receiver//

        // Overwriting PL,SL and isLOS to make the forward and reverse links identical ...
        tempLinkInfo=aNodes(serverNodes(serverNode_cnt2)).getLinkInfo(serverNodes(serverNode_cnt1));
        aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"pathLoss",tempLinkInfo.pathLoss);
        aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"shadowLoss",tempLinkInfo.shadowLoss);
        aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"isLOS",tempLinkInfo.isLOS);

        if(myTerrain.isForSCMChannel)
          aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),tempLinkInfo.scmLargeScaleParams);//! Setting the scmLargescale parameters from the first node to remainingnodes//

          rxGroupID=findGroup(aNodes(serverNodes(serverNode_cnt1)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernode_cnt2 with the same loc(name) and getting thier IDs//
          txGroupID=findGroup(aNodes(serverNodes(serverNode_cnt2)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernode_cnt1 with the same loc(name) and getting thier IDs//

          if(rxGroupID(0)!=-1)//! Receiver grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
          {
        if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
        {
          aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
          aNodeGroups(rxGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt1);
        }
        else //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
        {
          tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serverNodes(serverNode_cnt2));

          aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"pathLoss",tempLinkInfo.pathLoss);
          aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"shadowLoss",tempLinkInfo.shadowLoss);
          aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"isLOS",tempLinkInfo.isLOS);

          if(myTerrain.isForSCMChannel)
            aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),tempLinkInfo.scmLargeScaleParams);//! Setting the scmLargescale parameters from the first node to remainingnodes//

        }
          }
          if(txGroupID(0)!=-1)//! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
          {
        if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
        {
          aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
          aNodeGroups(txGroupID(0)).nodeIDs(0)=serverNodes(serverNode_cnt2);
        }
        else  //! Else if there exits a node before then set the properties of new node to th properties of the node(like pathloss,shadowloss,isLOS and SCM channel) that already present//
        {

          tempLinkInfo=aNodes(serverNodes(serverNode_cnt1)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));

          aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"pathLoss",tempLinkInfo.pathLoss);
          aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"shadowLoss",tempLinkInfo.shadowLoss);
          aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),"isLOS",tempLinkInfo.isLOS);
          if(myTerrain.isForSCMChannel)
            aNodes(serverNodes(serverNode_cnt1)).overwriteLinkInfo(serverNodes(serverNode_cnt2),tempLinkInfo.scmLargeScaleParams);

        }
          }

      }
      else //! If cell layout is random//
        aNodes(serverNodes(serverNode_cnt1)).linkToTXNode(aNodes(serverNodes(serverNode_cnt2)),myTerrain,shadowType,retailUnAlteredLinks,retainOldLOS);//getting the link to txnode info for that node////
    }

      }

    }
    #ifdef USING_MPI
    if(currentTaskid==0) {
      #endif
      cout<<"[detl:]CrossLinkInfo generated."<<endl;
      #ifdef USING_MPI
    }
    #endif
  }
}

void McellSystem::generateLinkInfo(int newNode,bool retailUnAlteredLinks, bool retainOldLOS){

  generateLinkInfo(newNode,sysTerrain,retailUnAlteredLinks,retainOldLOS);
}

/*!\fn generateLinkInfo()
 * \brief--> generating linkinfo when new node is introduced which may be service or server node
 *\param[in,out] myTerrain --> it will take the input parameter of terrain class and get modified and return it
 *\param[in] retailUnAlteredLinks --> it is boolean parameter which retains the unaltered links when function is called again
 *\param[in] retainOldLOS --> it is a boolean parameter which retains the OldLOS when function is called again
 */

void McellSystem::generateLinkInfo(int newNode,Terrain &myTerrain, bool retailUnAlteredLinks, bool retainOldLOS){ 

  if(!isSysReady()) //! If system is not ready//
  {
	cout<<"[both:]System Not Fully Configured.";
	abort();
  }
  if(!aNodes(newNode).isActive()) //! If the newnode is not active set it active before generating linkinfo//
  {
    aNodes(newNode).setActive();
    cout<<"[detl:]New node activated automatically before trying to generate LinkInfo.";
  }

  if(aNodes(newNode).getCategory()==_NODECATEGORY_UNINIT_) //! If node category is not initialized//
  { 
    cout<<"[both:]New node category not initialized in generateLinkInfo().";
    abort();
  }

  ivec rxGroupID,txGroupID; 
  NodeLinkInfo_S tempLinkInfo;

  ivec hexNodes(0);
  Location_S wrapTxLoc,txCellLoc; 
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_)//! For the cellLayout hexagonal find the nodes which have hexagonal dropmethod//
  {
    int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
    hexNodes=getNodes(aNodeTypes(hexDropNodeType));
  }
   
    int rndhexDropNodeType=find(nodeDropMethods,_DROPMETHOD_RANDOMHEX_);
   

  bool linkStatus=aNodes(newNode).getLinkInfoUpdateStatus(); //! Getting the linkinfo status of the newnode//
  aNodes(newNode).setLinkInfoUpdateStatus(true); 
  aNodes(newNode).txAntenna.setParameterUpdateStatus(true);
  aNodes(newNode).rxAntenna.setParameterUpdateStatus(true);
 
  //! If the new node is servernode(base station ) which is a transmitter and all the service nodes as receivers//
  if(aNodes(newNode).getCategory()==_NODECATEGORY_SERVERNODE_)
  {
    ivec serviceNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));

   
    {
      //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
	  for(int rxNode_cnt=0;rxNode_cnt<length(serviceNodes);rxNode_cnt++) 
	  {
	    
	    if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
	    {
              //! If the newnode cellID is not equal servicenode cellID which implies both the nodes are not in the samecell//
		if(aNodes(newNode).getCellID()!=aNodes(serviceNodes(rxNode_cnt)).getCellID()) 
		{  //! Getting the newnode cell ID and setting it to transmitter location//
		  for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
		    if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(newNode).getCellID())
		      txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();
              //! Since both the nodes are in the different cells get the wrap around location of tranmitter nodes with respect to receiver//
		  for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++) 
		    if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(rxNode_cnt)).getCellID())
		      wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(newNode).getCellID()),txCellLoc,aNodes(newNode).getLoc());
		    aNodes(newNode).setWrapLoc(wrapTxLoc,true); 
		}
		else //! If newnode and service nodes are in the same cell implies wrap around location is the same cell location///
		  aNodes(newNode).setWrapLoc(aNodes(newNode).getLoc(),true); 
	    }
	    aNodes(serviceNodes(rxNode_cnt)).linkToTXNode(aNodes(newNode),myTerrain,shadowType,!(aNodes(serviceNodes(rxNode_cnt)).getLinkInfoUpdateStatus()*linkStatus),retailUnAlteredLinks,retainOldLOS);

	    if(cellLayout==_CELLLAYOUT_HEXAGONAL_ || rndhexDropNodeType!=-1)
	    {
		  rxGroupID=findGroup(aNodes(serviceNodes(rxNode_cnt)).getGroupID(),"SameLoc",true); //! Finding the group that having the servicenode with the same loc(name) and getting thier IDs//
		  txGroupID=findGroup(aNodes(newNode).getGroupID(),"SameLoc",true);//! Finding the group that having the newNode with the same loc(name) and getting thier IDs//

		  if(rxGroupID(0)!=-1) //! Reciever grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
		  {
		    if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
		    {
		      aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
		      aNodeGroups(rxGroupID(0)).nodeIDs(0)=serviceNodes(rxNode_cnt);
		    }
		    else//! Else if there exits a node before in that group that links to the newnode then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present//
		      if(aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).isLinked(newNode))
		    {
		      tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(newNode);
			  aNodes(serviceNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"pathLoss",tempLinkInfo.pathLoss);
			  aNodes(serviceNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"shadowLoss",tempLinkInfo.shadowLoss);
			  aNodes(serviceNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"isLOS",tempLinkInfo.isLOS);
			  if(myTerrain.isForSCMChannel)
			    aNodes(serviceNodes(rxNode_cnt)).overwriteLinkInfo(newNode,tempLinkInfo.scmLargeScaleParams);
		    }
		  }
		  if(txGroupID(0)!=-1) //! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
		  {
		    if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
		    {
		      aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
		      aNodeGroups(txGroupID(0)).nodeIDs(0)=newNode;
		    }
		    else  //! Else if there exists a newnode already exists in the group then set properties of the newnode(like pathloss,shadowloss,isLOS and SCM channel)  to that already present//
		      if(aNodes(serviceNodes(rxNode_cnt)).isLinked(aNodeGroups(txGroupID(0)).nodeIDs(0)))
		    {
		      tempLinkInfo=aNodes(serviceNodes(rxNode_cnt)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
			  aNodes(serviceNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"pathLoss",tempLinkInfo.pathLoss);
			  aNodes(serviceNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"shadowLoss",tempLinkInfo.shadowLoss);
			  aNodes(serviceNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"isLOS",tempLinkInfo.isLOS);
			  if(myTerrain.isForSCMChannel)
			    aNodes(serviceNodes(rxNode_cnt)).overwriteLinkInfo(newNode,tempLinkInfo.scmLargeScaleParams);
		    }
		  }

	    }
	  }
    }
    //! Assuming newNode is a Receiver and all serviceNodes as Transmitters//
    {

// 	  aNodes(newNode).retainLinkInfo(serviceNodes);
	  for(int txNode_cnt=0;txNode_cnt<length(serviceNodes);txNode_cnt++)
	  {
	    if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
	    {
	      //! If the newnode(receiver ) is not in the same cell of the transmitter then get the sevicenode location as transmitter location and getting the wrap around location of receiver(newnode) with respect to transmitter//
              //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//                
		  if(aNodes(newNode).getCellID()!=aNodes(serviceNodes(txNode_cnt)).getCellID())
		  {
		    for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
		      if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serviceNodes(txNode_cnt)).getCellID())
			txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();

		    for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
		    if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(newNode).getCellID())
			    wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serviceNodes(txNode_cnt)).getCellID()),txCellLoc,aNodes(serviceNodes(txNode_cnt)).getLoc());
		    aNodes(serviceNodes(txNode_cnt)).setWrapLoc(wrapTxLoc,true);
		  }
		  else //! If the newnode(receiver) is in the same location of servicenode(transmitter) then the wrap around location will be the same cell location//  
		    aNodes(serviceNodes(txNode_cnt)).setWrapLoc(aNodes(serviceNodes(txNode_cnt)).getLoc(),true);
	    }

	    aNodes(newNode).linkToTXNode(aNodes(serviceNodes(txNode_cnt)),myTerrain,shadowType, !(linkStatus*aNodes(serviceNodes(txNode_cnt)).getLinkInfoUpdateStatus()),retailUnAlteredLinks,retainOldLOS);

	    //Overwriting PL,SL and isLOS to make the forward and reverse links are identical ...
	    tempLinkInfo=aNodes(serviceNodes(txNode_cnt)).getLinkInfo(newNode);
	    aNodes(newNode).overwriteLinkInfo(serviceNodes(txNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
	    aNodes(newNode).overwriteLinkInfo(serviceNodes(txNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
	    aNodes(newNode).overwriteLinkInfo(serviceNodes(txNode_cnt),"isLOS",tempLinkInfo.isLOS);

	    if(cellLayout==_CELLLAYOUT_HEXAGONAL_ || rndhexDropNodeType)
	    {
		  rxGroupID=findGroup(aNodes(newNode).getGroupID(),"SameLoc",true); //! Finding the group that having the newnode with the same loc(name) and getting thier IDs//
		  txGroupID=findGroup(aNodes(serviceNodes(txNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenodes with the same loc(name) and getting thier IDs//
		 
		  if(rxGroupID(0)!=-1) //! Reciever grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
		  
		  {
		    if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
		    {
		      aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
		      aNodeGroups(rxGroupID(0)).nodeIDs(0)=newNode;
		    }
		    else //! Else if there exits a node before in that group that links to the service node then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present//
		      if(aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).isLinked(serviceNodes(txNode_cnt)))
		    {
		      tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serviceNodes(txNode_cnt));
			  aNodes(newNode).overwriteLinkInfo(serviceNodes(txNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
			  aNodes(newNode).overwriteLinkInfo(serviceNodes(txNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
			  aNodes(newNode).overwriteLinkInfo(serviceNodes(txNode_cnt),"isLOS",tempLinkInfo.isLOS);
			  if(myTerrain.isForSCMChannel)
			    aNodes(newNode).overwriteLinkInfo(serviceNodes(txNode_cnt),tempLinkInfo.scmLargeScaleParams);
		    }
		  }
		  if(txGroupID(0)!=-1)//! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
		  {
		    if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
		    {
		      aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
		      aNodeGroups(txGroupID(0)).nodeIDs(0)=serviceNodes(txNode_cnt);
		    }
		    else//! Else if there exists a node already exists in the group linking to the newnode then set properties of the remaining node(like pathloss,shadowloss,isLOS and SCM channel)  to that already present//
		      if(aNodes(newNode).isLinked(aNodeGroups(txGroupID(0)).nodeIDs(0)))
		    {
		      tempLinkInfo=aNodes(newNode).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
			  aNodes(newNode).overwriteLinkInfo(serviceNodes(txNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
			  aNodes(newNode).overwriteLinkInfo(serviceNodes(txNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
			  aNodes(newNode).overwriteLinkInfo(serviceNodes(txNode_cnt),"isLOS",tempLinkInfo.isLOS);
			  if(myTerrain.isForSCMChannel)
			    aNodes(newNode).overwriteLinkInfo(serviceNodes(txNode_cnt),tempLinkInfo.scmLargeScaleParams);
		    }
		  }

	    }
	  }

    }

  }
  
  //! Newnode as server node is completed with all possibilities//
  else //! If the new node is service node-UE
  {
    ivec serverNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));//! Create a vector of serverNodes which contains the node of _NODECATEGORY_SERVERNODE_
    
    //! Assuming newNode(servicenode) is a Receiver and all serverNodes as Transmitters which implies downlink//
    {
      ivec hexNodes(0);
      Location_S wrapTxLoc,txCellLoc;
      if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
      {
	int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
	hexNodes=getNodes(aNodeTypes(hexDropNodeType)); 
      }
      //       aNodes(newNode).retainLinkInfo(serverNodes);
      for(int txNode_cnt=0;txNode_cnt<length(serverNodes);txNode_cnt++)
      {
	if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
	{   
	  //! Set the transmitter cell location that is servernodes location///
	  //! If newnode and server nodes are in different cells then get the wrap around location of servernodes with respect to servicenodes//
	  //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
	  for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
	    if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(txNode_cnt)).getCellID())
	      txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();

	    if(aNodes(newNode).getCellID()!=aNodes(serverNodes(txNode_cnt)).getCellID())
	    {
	      for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
		if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(newNode).getCellID())
		  wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serverNodes(txNode_cnt)).getCellID()),txCellLoc,aNodes(serverNodes(txNode_cnt)).getLoc());
		aNodes(serverNodes(txNode_cnt)).setWrapLoc(wrapTxLoc,true);
	    }
	    else //! If the new node and server nodes are in same cell then wrap around location will be the same cell location//
	      aNodes(serverNodes(txNode_cnt)).setWrapLoc(aNodes(serverNodes(txNode_cnt)).getLoc(),true);
	}
	//! Get the linkinfo for new node w.r.t all the servernodes//
	aNodes(newNode).linkToTXNode(aNodes(serverNodes(txNode_cnt)),myTerrain,shadowType, !(linkStatus*aNodes(serverNodes(txNode_cnt)).getLinkInfoUpdateStatus()),retailUnAlteredLinks,retainOldLOS);
	if(cellLayout==_CELLLAYOUT_HEXAGONAL_ || rndhexDropNodeType)
	{
	  rxGroupID=findGroup(aNodes(newNode).getGroupID(),"SameLoc",true); //! Finding the group that having the newnodes with the same loc(name) and getting thier IDs//
	  txGroupID=findGroup(aNodes(serverNodes(txNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernode with the same loc(name) and getting thier IDs//
	  
	  if(rxGroupID(0)!=-1) //! Receiver grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node// 
	  {
	    if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
	    {
	      aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
	      aNodeGroups(rxGroupID(0)).nodeIDs(0)=newNode;
	    }
	    else //! Else if there exits a newnode before in that group that links to the servernode  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
	      if(aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).isLinked(serverNodes(txNode_cnt)))
	      {
		tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serverNodes(txNode_cnt));
		aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
		aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
		aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"isLOS",tempLinkInfo.isLOS);
		if(myTerrain.isForSCMChannel)
		  aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),tempLinkInfo.scmLargeScaleParams);
	      }
	  }
	  if(txGroupID(0)!=-1)//! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node// 
	  {
	    if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
	    {
	      aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
	      aNodeGroups(txGroupID(0)).nodeIDs(0)=serverNodes(txNode_cnt);
	    }
	    else//! Else if there exits a node before in that group that links to newnode  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
	      if(aNodes(newNode).isLinked(aNodeGroups(txGroupID(0)).nodeIDs(0)))
	      {
		tempLinkInfo=aNodes(newNode).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
		aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
		aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
		aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"isLOS",tempLinkInfo.isLOS);
		if(myTerrain.isForSCMChannel)
		  aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),tempLinkInfo.scmLargeScaleParams);
	      }
	  }
      
    }
    NodeLinkInfo_S linkinfo = aNodes(newNode).getLinkInfo(serverNodes(txNode_cnt));
//     cout<<"linkinfo : "<<aNodes(newNode).getCellID()<<" , "<<linkinfo.isLOS<<" , "<<linkinfo.pathLoss<<" , "<<linkinfo.shadowLoss<<" , "<<linkinfo.txAntennaGain<<" , "<<linkinfo.txLocation<<" , "<<linkinfo.rxLocation<<endl;
    }
      
    }
    
    
    
    //! Assuming newNode(service node) is a Transmitter and all serverNodes as Receivers
    {
      for(int rxNode_cnt=0;rxNode_cnt<length(serverNodes);rxNode_cnt++)//! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
      {
	if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
	{
             //! Set the transmitter cell location that is newnode location///
	  //! If newnode and server nodes are in different cells then get the wrap around location of servernodes with respect to servicenodes//
	  //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
	  if(aNodes(newNode).getCellID()!=aNodes(serverNodes(rxNode_cnt)).getCellID())
	  {
	    for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
	      if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(newNode).getCellID())
		txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();

	      for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
		if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(rxNode_cnt)).getCellID())
		  wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(newNode).getCellID()),txCellLoc,aNodes(newNode).getLoc());
		aNodes(newNode).setWrapLoc(wrapTxLoc,true);
	  }
	  else //! If the new node and server nodes are in same cell then wrap around location will be the same cell location//
	    aNodes(newNode).setWrapLoc(aNodes(newNode).getLoc(),true);
	}
	//! Get the linkinfo details//
	aNodes(serverNodes(rxNode_cnt)).linkToTXNode(aNodes(newNode),myTerrain,shadowType,!(aNodes(serverNodes(rxNode_cnt)).getLinkInfoUpdateStatus()*linkStatus),retailUnAlteredLinks,retainOldLOS);
	//Overwriting PL,SL and isLOS to make the forward and reverse links are identical ...
	tempLinkInfo=aNodes(newNode).getLinkInfo(serverNodes(rxNode_cnt));
	aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"pathLoss",tempLinkInfo.pathLoss);
	aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"shadowLoss",tempLinkInfo.shadowLoss);
	aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"isLOS",tempLinkInfo.isLOS);
	if(myTerrain.isForSCMChannel)
	  aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,tempLinkInfo.scmLargeScaleParams);
	
	if(cellLayout==_CELLLAYOUT_HEXAGONAL_ || rndhexDropNodeType)
	{
	  rxGroupID=findGroup(aNodes(serverNodes(rxNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernodes with the same loc(name) and getting thier IDs//
	  txGroupID=findGroup(aNodes(newNode).getGroupID(),"SameLoc",true);//! Finding the group that having the newnode with the same loc(name) and getting thier IDs//

	  if(rxGroupID(0)!=-1) //! Receiver grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node// 
	  {
	    if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
	    {
	      aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
	      aNodeGroups(rxGroupID(0)).nodeIDs(0)=serverNodes(rxNode_cnt);
	    }
	    else//! Else if there exits a servernode before in that group that links to the newnode  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
	      if(aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).isLinked(newNode))
	    {
	      tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(newNode);
	      aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"pathLoss",tempLinkInfo.pathLoss);
	      aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"shadowLoss",tempLinkInfo.shadowLoss);
	      aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"isLOS",tempLinkInfo.isLOS);
	      if(myTerrain.isForSCMChannel)
		aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,tempLinkInfo.scmLargeScaleParams);
	    }
	  }
	  if(txGroupID(0)!=-1)//! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node// 
	  {
	    if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
	    {
	      aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
	      aNodeGroups(txGroupID(0)).nodeIDs(0)=newNode;
	    }
	    else//! Else if there exits a newnode before in that group that links to the servernode(group)  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
	      if(aNodes(serverNodes(rxNode_cnt)).isLinked(aNodeGroups(txGroupID(0)).nodeIDs(0)))
	    {
	      tempLinkInfo=aNodes(serverNodes(rxNode_cnt)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
	      aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"pathLoss",tempLinkInfo.pathLoss);
	      aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"shadowLoss",tempLinkInfo.shadowLoss);
	      aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"isLOS",tempLinkInfo.isLOS);
	      if(myTerrain.isForSCMChannel)
		aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,tempLinkInfo.scmLargeScaleParams);
	    }
	  }

	}
      }
    }
    

  }


}
//! All the combinations of genration of lininfo are completed//

void McellSystem::generateCrossLinkInfo(int newNode,bool retailUnAlteredLinks, bool retainOldLOS){

  generateCrossLinkInfo(newNode,sysTerrain,retailUnAlteredLinks,retainOldLOS);
}

/*!\fn generateCrossLinkInfo()
 * \brief--> generating crosslinkinfo when new node is introduced which may be service or server node
 *\param[in,out] myTerrain --> it will take the input parameter of terrain class and get modified and return it
 *\param[in] retailUnAlteredLinks --> it is boolean parameter which retains the unaltered links when function is called again
 *\param[in] retainOldLOS --> it is a boolean parameter which retains the OldLOS when function is called again
 */

void McellSystem::generateCrossLinkInfo(int newNode,Terrain &myTerrain, bool retailUnAlteredLinks, bool retainOldLOS){

  if(!isSysReady()) //! If system is not ready//
  {
    cout<<"[both:]System Not Fully Configured.";
    abort();
  }
  if(!aNodes(newNode).isActive()) //! If the newnode is not active set it active before generating linkinfo//
  {
    aNodes(newNode).setActive();
    cout<<"[detl:]New node activated automatically before trying to generate LinkInfo.";
  }

  if(aNodes(newNode).getCategory()==_NODECATEGORY_UNINIT_) //! If node category is not initialized//
  {
    cout<<"[both:]New node category not initialized in generateLinkInfo().";
    abort();
  }

  ivec rxGroupID,txGroupID;
  NodeLinkInfo_S tempLinkInfo;

  ivec hexNodes(0);
  Location_S wrapTxLoc,txCellLoc;
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_)//! For the cellLayout hexagonal find the nodes which have hexagonal dropmethod//
  {
    int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
    hexNodes=getNodes(aNodeTypes(hexDropNodeType));
  }
    int rndhexDropNodeType=find(nodeDropMethods,_DROPMETHOD_RANDOMHEX_);


  //! If the new node is servernode(base station ) which is a transmitter and all the service nodes as receivers//
  if(aNodes(newNode).getCategory()==_NODECATEGORY_SERVERNODE_)
  {
    ivec serviceNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));


    ///CrossLinks - 10010
    if(isCrossLinksEnabled) //! If the crosslink is enabled//
    {
      ivec serverNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));//! Get the active node list which are of category server node and the new node is of category servernode//

      //! Assuming newNode is a Transmitter but it is a server node and all serverNodes as Receivers  so there are links from newnode(server) to remaining srevernode//
      {
    for(int rxNode_cnt=0;rxNode_cnt<length(serverNodes);rxNode_cnt++)
    {
      if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
      {
        //! If the newnode is not in the same cell of remaining server nodes then set the transmitter cell location as newnode cell location
        //! And getting the wrap around location of newnodes with respect to all other servernodes//
        //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
        if(aNodes(newNode).getCellID()!=aNodes(serverNodes(rxNode_cnt)).getCellID())
        {
          for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
        if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(newNode).getCellID())
          txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();

        for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)//! checking hexagonalNodes//
                  if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(rxNode_cnt)).getCellID())//! For all server nodes//
                    wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(newNode).getCellID()),txCellLoc,aNodes(newNode).getLoc());//finding the wrap around location of servernodes w.r.t newnode which is server node but transmiter//
                aNodes(newNode).setWrapLoc(wrapTxLoc,true);
        }
        else //! If newnode is in same cell of servernode then the wraparound location will be the same cell location//
                  aNodes(newNode).setWrapLoc(aNodes(newNode).getLoc(),true);
      }
      //! Get the link properties for newnode for all server nodes//
      //! FIXME isLinkAltered variable hardcoded .. This will change the link across multiple call of the function
      aNodes(serverNodes(rxNode_cnt)).linkToTXNode(aNodes(newNode),myTerrain,shadowType,true,retailUnAlteredLinks,retainOldLOS);//after getting the wrap around location get the link To TX Node parameters from the function//

      if(cellLayout==_CELLLAYOUT_HEXAGONAL_ || rndhexDropNodeType)//! If the cellLayout is hexagonal//
      {
        rxGroupID=findGroup(aNodes(serverNodes(rxNode_cnt)).getGroupID(),"SameLoc",true); //! Finding the group that having the servernodes with the same loc(name) and getting thier IDs//
        txGroupID=findGroup(aNodes(newNode).getGroupID(),"SameLoc",true);//! Finding the group that having the newnodes with the same loc(name) and getting thier IDs//

        if(rxGroupID(0)!=-1)//! Reciever grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
        {
          if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
          {
        aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
        aNodeGroups(rxGroupID(0)).nodeIDs(0)=serverNodes(rxNode_cnt);
          }
          else //! Else if there exits a node before in that group that links to the newnode  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
        if(aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).isLinked(newNode))
        {


          tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(newNode);
          aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"pathLoss",tempLinkInfo.pathLoss);
          aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"shadowLoss",tempLinkInfo.shadowLoss);
          aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"isLOS",tempLinkInfo.isLOS);
          if(myTerrain.isForSCMChannel)
            aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,tempLinkInfo.scmLargeScaleParams);
        }
        }
        if(txGroupID(0)!=-1) //! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
        {
          if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
          {
        aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
        aNodeGroups(txGroupID(0)).nodeIDs(0)=newNode;
          }
          else//! Else if there exits a node before in that group that links to the servernode  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
          if(aNodes(serverNodes(rxNode_cnt)).isLinked(aNodeGroups(txGroupID(0)).nodeIDs(0)))//! If there already exixted a node in that group//
          {
        tempLinkInfo=aNodes(serverNodes(rxNode_cnt)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
        aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"pathLoss",tempLinkInfo.pathLoss);
        aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"shadowLoss",tempLinkInfo.shadowLoss);
        aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"isLOS",tempLinkInfo.isLOS);
        if(myTerrain.isForSCMChannel)
          aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,tempLinkInfo.scmLargeScaleParams);
          }
        }

      }
    }
      }
      //! Assuming newNode is a Receiver(servernode) and all remaining  serverNodes as Transmitters
      {

    //       aNodes(newNode).retainLinkInfo(serverNodes);
    for(int txNode_cnt=0;txNode_cnt<length(serverNodes);txNode_cnt++)//! For all the transmitter nodes//
    {
      if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
      {
        //! If the newnode is not in the same cell of remaining server nodes then set the transmitter cell location as servernodes location
        //! And getting the wrap around location of servernodes nodes with respect to all other newnodes//
        //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
        if(aNodes(newNode).getCellID()!=aNodes(serverNodes(txNode_cnt)).getCellID())
        {
          for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
        if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(txNode_cnt)).getCellID())
          txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();//! set the loc as transmitter location

          for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
            if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(newNode).getCellID())
              wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serverNodes(txNode_cnt)).getCellID()),txCellLoc,aNodes(serverNodes(txNode_cnt)).getLoc());
            aNodes(serverNodes(txNode_cnt)).setWrapLoc(wrapTxLoc,true);
        }
        else   //! If newnode is in same cell of servernode then the wraparound location will be the same cell location//
                              aNodes(serverNodes(txNode_cnt)).setWrapLoc(aNodes(serverNodes(txNode_cnt)).getLoc(),true);
      }
      //! For all the newnodes getting the link info details//
      aNodes(newNode).linkToTXNode(aNodes(serverNodes(txNode_cnt)),myTerrain,shadowType, true,retailUnAlteredLinks,retainOldLOS);//get the link to tx properties from the function for newnode//

      //! Overwriting PL,SL and isLOS to make the forward and reverse links are identical ...
      tempLinkInfo=aNodes(serverNodes(txNode_cnt)).getLinkInfo(newNode);//! Getting the linkinfo status from the newnode
      aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
      aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
      aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"isLOS",tempLinkInfo.isLOS);

      if(cellLayout==_CELLLAYOUT_HEXAGONAL_ || rndhexDropNodeType)
      {
        rxGroupID=findGroup(aNodes(newNode).getGroupID(),"SameLoc",true);//!Finding the group that having the newnode with the same loc(name) and getting thier IDs//
        txGroupID=findGroup(aNodes(serverNodes(txNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernodes with the same loc(name) and getting thier IDs//

        if(rxGroupID(0)!=-1)//! Reciever grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
        {
          if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
          {
        aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
        aNodeGroups(rxGroupID(0)).nodeIDs(0)=newNode;
          }
          else //! Else if there exits a newnode before in that group that links to the servernode(servernode group)  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
            if(aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).isLinked(serverNodes(txNode_cnt)))
            {
              tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serverNodes(txNode_cnt));
              aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
              aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
              aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"isLOS",tempLinkInfo.isLOS);
              if(myTerrain.isForSCMChannel)
                aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),tempLinkInfo.scmLargeScaleParams);
            }
        }
        if(txGroupID(0)!=-1)//! Tranmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
        {
          if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
          {
        aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
        aNodeGroups(txGroupID(0)).nodeIDs(0)=serverNodes(txNode_cnt);
          }
          else //! Else if there exits a servernode  before in that group that links to the newnode  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
          if(aNodes(newNode).isLinked(aNodeGroups(txGroupID(0)).nodeIDs(0)))
          {
        tempLinkInfo=aNodes(newNode).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
        aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
        aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
        aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"isLOS",tempLinkInfo.isLOS);
        if(myTerrain.isForSCMChannel)
          aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),tempLinkInfo.scmLargeScaleParams);
          }
        }

      }
    }

      }
    }

  }

  //! Newnode as server node is completed with all possibilities//
  else //! If the new node is service node-UE
  {
    ivec serverNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));//! Create a vector of serverNodes which contains the node of _NODECATEGORY_SERVERNODE_

    //! Assuming newNode(servicenode) is a Receiver and all serverNodes as Transmitters which implies downlink//
    {
      ivec hexNodes(0);
      Location_S wrapTxLoc,txCellLoc;
      if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
      {
    int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
    hexNodes=getNodes(aNodeTypes(hexDropNodeType));
      }
      //       aNodes(newNode).retainLinkInfo(serverNodes);
      for(int txNode_cnt=0;txNode_cnt<length(serverNodes);txNode_cnt++)
      {
    if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
    {
      //! Set the transmitter cell location that is servernodes location///
      //! If newnode and server nodes are in different cells then get the wrap around location of servernodes with respect to servicenodes//
      //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
      for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
        if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(txNode_cnt)).getCellID())
          txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();
        if(aNodes(newNode).getCellID()!=aNodes(serverNodes(txNode_cnt)).getCellID())
        {
          for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
        if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(newNode).getCellID())
          wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(serverNodes(txNode_cnt)).getCellID()),txCellLoc,aNodes(serverNodes(txNode_cnt)).getLoc());
        aNodes(serverNodes(txNode_cnt)).setWrapLoc(wrapTxLoc,true);
        }
        else //! If the new node and server nodes are in same cell then wrap around location will be the same cell location//
          aNodes(serverNodes(txNode_cnt)).setWrapLoc(aNodes(serverNodes(txNode_cnt)).getLoc(),true);
    }
    //! Get the linkinfo for new node w.r.t all the servernodes//
    aNodes(newNode).linkToTXNode(aNodes(serverNodes(txNode_cnt)),myTerrain,shadowType, true,retailUnAlteredLinks,retainOldLOS);
    if(cellLayout==_CELLLAYOUT_HEXAGONAL_ || rndhexDropNodeType!=-1)
    {
      rxGroupID=findGroup(aNodes(newNode).getGroupID(),"SameLoc",true); //! Finding the group that having the newnodes with the same loc(name) and getting thier IDs//
      txGroupID=findGroup(aNodes(serverNodes(txNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernode with the same loc(name) and getting thier IDs//

      if(rxGroupID(0)!=-1) //! Receiver grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
      {
        if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
        {
          aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
          aNodeGroups(rxGroupID(0)).nodeIDs(0)=newNode;
        }
        else //! Else if there exits a newnode before in that group that links to the servernode  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
          if(aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).isLinked(serverNodes(txNode_cnt)))
          {
        tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(serverNodes(txNode_cnt));
        aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
        aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
        aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"isLOS",tempLinkInfo.isLOS);
        if(myTerrain.isForSCMChannel)
          aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),tempLinkInfo.scmLargeScaleParams);
          }
      }
      if(txGroupID(0)!=-1)//! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
      {
        if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
        {
          aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
          aNodeGroups(txGroupID(0)).nodeIDs(0)=serverNodes(txNode_cnt);
        }
        else//! Else if there exits a node before in that group that links to newnode  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
          if(aNodes(newNode).isLinked(aNodeGroups(txGroupID(0)).nodeIDs(0)))
          {
        tempLinkInfo=aNodes(newNode).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
        aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
        aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
        aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),"isLOS",tempLinkInfo.isLOS);
        if(myTerrain.isForSCMChannel)
          aNodes(newNode).overwriteLinkInfo(serverNodes(txNode_cnt),tempLinkInfo.scmLargeScaleParams);
          }
      }

    }
      }

    }



    //! Assuming newNode(service node) is a Transmitter and all serverNodes as Receivers
    {
      for(int rxNode_cnt=0;rxNode_cnt<length(serverNodes);rxNode_cnt++)//! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
      {
    if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
    {
      //! Set the transmitter cell location that is newnode location///
      //! If newnode and server nodes are in different cells then get the wrap around location of servernodes with respect to servicenodes//
      //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
      if(aNodes(newNode).getCellID()!=aNodes(serverNodes(rxNode_cnt)).getCellID())
      {
        for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
          if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(newNode).getCellID())
        txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();

          for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
        if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(serverNodes(rxNode_cnt)).getCellID())
          wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(newNode).getCellID()),txCellLoc,aNodes(newNode).getLoc());
        aNodes(newNode).setWrapLoc(wrapTxLoc,true);
      }
      else //! If the new node and server nodes are in same cell then wrap around location will be the same cell location//
        aNodes(newNode).setWrapLoc(aNodes(newNode).getLoc(),true);
    }
    //! Get the linkinfo details//
    aNodes(serverNodes(rxNode_cnt)).linkToTXNode(aNodes(newNode),myTerrain,shadowType,true,retailUnAlteredLinks,retainOldLOS);
    //Overwriting PL,SL and isLOS to make the forward and reverse links are identical ...
    tempLinkInfo=aNodes(newNode).getLinkInfo(serverNodes(rxNode_cnt));
    aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"pathLoss",tempLinkInfo.pathLoss);
    aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"shadowLoss",tempLinkInfo.shadowLoss);
    aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"isLOS",tempLinkInfo.isLOS);
    if(myTerrain.isForSCMChannel)
      aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,tempLinkInfo.scmLargeScaleParams);

    if(cellLayout==_CELLLAYOUT_HEXAGONAL_ || rndhexDropNodeType!=-1)
    {
      rxGroupID=findGroup(aNodes(serverNodes(rxNode_cnt)).getGroupID(),"SameLoc",true);//! Finding the group that having the servernodes with the same loc(name) and getting thier IDs//
      txGroupID=findGroup(aNodes(newNode).getGroupID(),"SameLoc",true);//! Finding the group that having the newnode with the same loc(name) and getting thier IDs//

      if(rxGroupID(0)!=-1) //! Receiver grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
      {
        if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
        {
          aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
          aNodeGroups(rxGroupID(0)).nodeIDs(0)=serverNodes(rxNode_cnt);
        }
        else//! Else if there exits a servernode before in that group that links to the newnode  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
          if(aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).isLinked(newNode))
          {
        tempLinkInfo=aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(0)).getLinkInfo(newNode);
        aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"pathLoss",tempLinkInfo.pathLoss);
        aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"shadowLoss",tempLinkInfo.shadowLoss);
        aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"isLOS",tempLinkInfo.isLOS);
        if(myTerrain.isForSCMChannel)
          aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,tempLinkInfo.scmLargeScaleParams);
          }
      }
      if(txGroupID(0)!=-1)//! Transmitter grouping is done, if it is the first node in the group ,set the properties as defined by linkto tx node//
      {
        if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
        {
          aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
          aNodeGroups(txGroupID(0)).nodeIDs(0)=newNode;
        }
        else//! Else if there exits a newnode before in that group that links to the servernode(group)  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
          if(aNodes(serverNodes(rxNode_cnt)).isLinked(aNodeGroups(txGroupID(0)).nodeIDs(0)))
          {
        tempLinkInfo=aNodes(serverNodes(rxNode_cnt)).getLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(0));
        aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"pathLoss",tempLinkInfo.pathLoss);
        aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"shadowLoss",tempLinkInfo.shadowLoss);
        aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,"isLOS",tempLinkInfo.isLOS);
        if(myTerrain.isForSCMChannel)
          aNodes(serverNodes(rxNode_cnt)).overwriteLinkInfo(newNode,tempLinkInfo.scmLargeScaleParams);
          }
      }

    }
      }
    }

  }


}
//! All the combinations of genration of lininfo are completed//


//! new node and othernode are taken at the same time//

/*!\fn generateLinkInfo()
 * \brief--> generating linkinfo when new node and othernode are introduced which can be transmitter or reciever.
 * \param[in] newNode --> It will give the number of newnodes introduced
 * \param[in] otherNode -->It will give the number of othernodes introduced
 *\param[in,out] myTerrain --> it will take the input parameter of terrain class and get modified and return it
 *\param[in] retailUnAlteredLinks --> it is boolean parameter which retains the unaltered links when function is called again
 *\param[in] retainOldLOS --> it is a boolean parameter which retains the OldLOS when function is called again
 */


void McellSystem::generateLinkInfo(int newNode,int otherNode,Terrain &myTerrain, bool retailUnAlteredLinks, bool retainOldLOS){

  if(!isSysReady())   //! Checking whether system is ready or not//
  {
    cout<<"[both:]System Not Fully Configured.";
    abort();
  }
  if(!aNodes(newNode).isActive()) //! If the node is not active make them active//
  {
    aNodes(newNode).setActive();
    cout<<"[detl:]New node activated automatically before trying to generate LinkInfo.";
  }

  if(aNodes(newNode).getCategory()==_NODECATEGORY_UNINIT_)
  {
    cout<<"[both:]New node category not initialized in generateLinkInfo().";
    abort();
  }

  ivec rxGroupID,txGroupID;
  NodeLinkInfo_S tempLinkInfo;

  ivec hexNodes(0);
  Location_S wrapTxLoc,txCellLoc;
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
  {
    int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
    hexNodes=getNodes(aNodeTypes(hexDropNodeType));
  }
    int rndhexDropNodeType=find(nodeDropMethods,_DROPMETHOD_RANDOMHEX_);

//! Getting the linkinfo status of the new node//
  bool linkStatus=aNodes(newNode).getLinkInfoUpdateStatus();
  aNodes(newNode).setLinkInfoUpdateStatus(true);
  aNodes(newNode).txAntenna.setParameterUpdateStatus(true);
  aNodes(newNode).rxAntenna.setParameterUpdateStatus(true);
  if(aNodes(newNode).getCategory()==_NODECATEGORY_SERVERNODE_)//! Check for the newnode category if it is servernode(BS ) it can be acted as transmitter or receiver so two combinations are taken into account//
  {
    //! Assuming newNode is a Transmitter and otherNode as Receiver
    {
	if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
	{
          //! If the new node and other node are in different cell then set the transmitter location as the newnode locaton and
          //! Get the wraparound location of newnode with respect to  othernode//
          //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
	  if(aNodes(newNode).getCellID()!=aNodes(otherNode).getCellID())
	  {
	    for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
	      if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(newNode).getCellID())
		txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();

	      for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
		if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(otherNode).getCellID())
		  wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(newNode).getCellID()),txCellLoc,aNodes(newNode).getLoc());
		aNodes(newNode).setWrapLoc(wrapTxLoc,true);
	  }
	  else //! Else if newnode and other node are in same cell then wrap around location is the samecell location//
               //! And get the link info details of all the nodes//
	    aNodes(newNode).setWrapLoc(aNodes(newNode).getLoc(),true);
	}
	aNodes(otherNode).linkToTXNode(aNodes(newNode),myTerrain,shadowType,!(aNodes(otherNode).getLinkInfoUpdateStatus()*linkStatus),retailUnAlteredLinks,retainOldLOS);

	if(cellLayout==_CELLLAYOUT_HEXAGONAL_ || rndhexDropNodeType!=-1)
	{
	  rxGroupID=findGroup(aNodes(otherNode).getGroupID(),"SameLoc",true);//! Finding the group that having the othernodes with the same loc(name) and getting thier IDs//
	  txGroupID=findGroup(aNodes(newNode).getGroupID(),"SameLoc",true);//! Finding the group that having the newnodes with the same loc(name) and getting thier IDs//

	  if(rxGroupID(0)!=-1)//! Once receiver(othernode) grouping is done ,if it is the first node in the group ,set the properties as defined by linkto tx node// 
	  {
	    if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
	    {
	      aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
	      aNodeGroups(rxGroupID(0)).nodeIDs(0)=otherNode;
	    }
	    else //! Else if there exits a othernode before in that group that links to the newnode(group)  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
	    {

	      tempLinkInfo=aNodes(otherNode).getLinkInfo(newNode);
	      for(int groupNode_cnt=0;groupNode_cnt<aNodeGroups(rxGroupID(0)).nodeIDs.length();groupNode_cnt++)
		if(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)!=otherNode)
		  if(aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).isLinked(newNode))
		  {
		    aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(newNode,"pathLoss",tempLinkInfo.pathLoss);
		    aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(newNode,"shadowLoss",tempLinkInfo.shadowLoss);
		    aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(newNode,"isLOS",tempLinkInfo.isLOS);
		    if(myTerrain.isForSCMChannel)
		      aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(newNode,tempLinkInfo.scmLargeScaleParams);
		  }

	    }
	  }
	  if(txGroupID(0)!=-1) //! Once transmitter(newnode) grouping is done ,if it is the first node in the group ,set the properties as defined by linkto tx node// 
	  {
	    if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
	    {
	      aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
	      aNodeGroups(txGroupID(0)).nodeIDs(0)=newNode;
	    }
	    else //! Else if there exits a newnode before in that group that links to the othernode(group)  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
	    {
	      tempLinkInfo=aNodes(otherNode).getLinkInfo(newNode);
	      for(int groupNode_cnt=0;groupNode_cnt<aNodeGroups(txGroupID(0)).nodeIDs.length();groupNode_cnt++)
		if(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt)!=newNode)
		  if(aNodes(otherNode).isLinked(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt)))
		  {
		    aNodes(otherNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
		    aNodes(otherNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
		    aNodes(otherNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),"isLOS",tempLinkInfo.isLOS);
		    if(myTerrain.isForSCMChannel)
		      aNodes(otherNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),tempLinkInfo.scmLargeScaleParams);
		  }
	    }
	  }

	}
     }
    //! Assuming newNode is a Receiver and othernode as Transmitter
    {

      aNodes(newNode).retainLinkInfo(to_ivec(otherNode));
	if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
	{

	  if(aNodes(newNode).getCellID()!=aNodes(otherNode).getCellID())
	  {
	    //! If the new node and other node are in different cell then set the transmitter location as the  locaton and
          //! Get the wraparound location of othernode with respect to newnode//
	    
	    for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
	      if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(otherNode).getCellID())
		txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();

	      for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
		if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(newNode).getCellID())
		  wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(otherNode).getCellID()),txCellLoc,aNodes(otherNode).getLoc());
		aNodes(otherNode).setWrapLoc(wrapTxLoc,true);
	  }
	  else//! Else if newnode and other node are in same cell then wrap around location is the samecell location//
               //! And get the link info details of all the nodes//
	
	    aNodes(otherNode).setWrapLoc(aNodes(otherNode).getLoc(),true);
	}

	aNodes(newNode).linkToTXNode(aNodes(otherNode),myTerrain,shadowType, !(linkStatus*aNodes(otherNode).getLinkInfoUpdateStatus()),retailUnAlteredLinks,retainOldLOS);

	//Overwriting PL,SL and isLOS to make the forward and reverse links are identical ...
	tempLinkInfo=aNodes(otherNode).getLinkInfo(newNode);
	aNodes(newNode).overwriteLinkInfo(otherNode,"pathLoss",tempLinkInfo.pathLoss);
	aNodes(newNode).overwriteLinkInfo(otherNode,"shadowLoss",tempLinkInfo.shadowLoss);
	aNodes(newNode).overwriteLinkInfo(otherNode,"isLOS",tempLinkInfo.isLOS);

	if(cellLayout==_CELLLAYOUT_HEXAGONAL_ || rndhexDropNodeType!=-1)
	{
	  rxGroupID=findGroup(aNodes(newNode).getGroupID(),"SameLoc",true);//! Finding the group that having the newnode with the same loc(name) and getting thier IDs//
	  txGroupID=findGroup(aNodes(otherNode).getGroupID(),"SameLoc",true);//! Finding the group that having the othernode with the same loc(name) and getting thier IDs//
	 
	  if(rxGroupID(0)!=-1)//! Once receiver(newnode) grouping is done ,if it is the first node in the group ,set the properties as defined by linkto tx node// 
	  {
	    if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
	    {
	      aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
	      aNodeGroups(rxGroupID(0)).nodeIDs(0)=newNode;
	    }
	    else //! Else if there exits a newnode before in that group that links to the othernode(group)  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
	    {
	      tempLinkInfo=aNodes(newNode).getLinkInfo(otherNode);
	      for(int groupNode_cnt=0;groupNode_cnt<aNodeGroups(rxGroupID(0)).nodeIDs.length();groupNode_cnt++)
		if(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)!=newNode)
		  if(aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).isLinked(otherNode))
		  {
		    aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(otherNode,"pathLoss",tempLinkInfo.pathLoss);
		    aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(otherNode,"shadowLoss",tempLinkInfo.shadowLoss);
		    aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(otherNode,"isLOS",tempLinkInfo.isLOS);
		    if(myTerrain.isForSCMChannel)
		      aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(otherNode,tempLinkInfo.scmLargeScaleParams);
		  }
	    }
	  }
	  if(txGroupID(0)!=-1)//! Once transmitter(othernode) grouping is done ,if it is the first node in the group ,set the properties as defined by linkto tx node// 
	  {
	    if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
	    {
	      aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
	      aNodeGroups(txGroupID(0)).nodeIDs(0)=otherNode;
	    }
	    else  //! Else if there exits a othernode before in that group that links to the newnode(group)  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
	    {
	      tempLinkInfo=aNodes(newNode).getLinkInfo(otherNode);
	      for(int groupNode_cnt=0;groupNode_cnt<aNodeGroups(txGroupID(0)).nodeIDs.length();groupNode_cnt++)
		if(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt)!=otherNode)
		  if(aNodes(newNode).isLinked(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt)))
		  {
		    aNodes(newNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
		    aNodes(newNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
		    aNodes(newNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),"isLOS",tempLinkInfo.isLOS);
		    if(myTerrain.isForSCMChannel)
		      aNodes(newNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),tempLinkInfo.scmLargeScaleParams);
		  }

	    }
	  }

	}


    }
  }
  //! New node and other can be server node or service node so there ther is no need for specification of server nodes and service nodes ///
  else//! Else if the nodecategory is serivenode(UE) then again two combinations are taken in to account//
  {
    //! Assuming newNode is a Receiver and otherNode as Transmitter
    {
      ivec hexNodes(0);
      Location_S wrapTxLoc,txCellLoc;
      if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
      {
	int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
	hexNodes=getNodes(aNodeTypes(hexDropNodeType));
      }
      aNodes(newNode).retainLinkInfo(to_ivec(otherNode));
      if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
      {
	//! If the new node and other node are in different cell then set the transmitter location as the othernode locaton and
          //! Get the wraparound location of othernode with respect to newnode//
          //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
	
	for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
	  if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(otherNode).getCellID())
	    txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();
	  if(aNodes(newNode).getCellID()!=aNodes(otherNode).getCellID())
	  {
	    for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
	      if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(newNode).getCellID())
		wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(otherNode).getCellID()),txCellLoc,aNodes(otherNode).getLoc());
	      aNodes(otherNode).setWrapLoc(wrapTxLoc,true);
	  }
	  else//! Elseif both the nodes are in same location then wraparound location will be the same cell location//
	    aNodes(otherNode).setWrapLoc(aNodes(otherNode).getLoc(),true);
      }
      //! Getting the linkinfo of the reciver that is newnode//
      aNodes(newNode).linkToTXNode(aNodes(otherNode),myTerrain,shadowType, !(linkStatus*aNodes(otherNode).getLinkInfoUpdateStatus()),retailUnAlteredLinks,retainOldLOS);
      
      if(myTerrain.isForSCMChannel)
	aNodes(newNode).overwriteLinkInfo(otherNode,tempLinkInfo.scmLargeScaleParams);
      
      if(cellLayout==_CELLLAYOUT_HEXAGONAL_ || rndhexDropNodeType!=-1)
      {
	rxGroupID=findGroup(aNodes(newNode).getGroupID(),"SameLoc",true); //! Finding the group that having the newnode with the same loc(name) and getting thier IDs// 
	txGroupID=findGroup(aNodes(otherNode).getGroupID(),"SameLoc",true); //! Finding the group that having the othernode with the same loc(name) and getting thier IDs//
	
	if(rxGroupID(0)!=-1)  //! Once receiver(newnode) grouping is done ,if it is the first node in the group ,set the properties as defined by linkto tx node// 
	{
	  if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
	  {
	    aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
	    aNodeGroups(rxGroupID(0)).nodeIDs(0)=newNode;
	  }
	  else //! Else if there exits a newnode before in that group that links to the othernode(group)  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
	  {
	    tempLinkInfo=aNodes(newNode).getLinkInfo(otherNode);
	    for(int groupNode_cnt=0;groupNode_cnt<aNodeGroups(rxGroupID(0)).nodeIDs.length();groupNode_cnt++)
	      if(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)!=newNode)
		if(aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).isLinked(otherNode))
		{
		  aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(otherNode,"pathLoss",tempLinkInfo.pathLoss);
		  aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(otherNode,"shadowLoss",tempLinkInfo.shadowLoss);
		  aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(otherNode,"isLOS",tempLinkInfo.isLOS);
		  if(myTerrain.isForSCMChannel)
		    aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(otherNode,tempLinkInfo.scmLargeScaleParams);
		}
	  }
	}
	if(txGroupID(0)!=-1) //! Once transmitter(othernode) grouping is done ,if it is the first node in the group ,set the properties as defined by linkto tx node// 
	{
	  if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
	  {
	    aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
	    aNodeGroups(txGroupID(0)).nodeIDs(0)=otherNode;
	  }
	  else  //! Else if there exits a othernode before in that group that links to the newnode(group)  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
	  {
	    tempLinkInfo=aNodes(newNode).getLinkInfo(otherNode);
	    for(int groupNode_cnt=0;groupNode_cnt<aNodeGroups(txGroupID(0)).nodeIDs.length();groupNode_cnt++)
	      if(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt)!=otherNode)
		if(aNodes(newNode).isLinked(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt)))
		{
		  aNodes(newNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
		  aNodes(newNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
		  aNodes(newNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),"isLOS",tempLinkInfo.isLOS);
		  if(myTerrain.isForSCMChannel)
		    aNodes(newNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),tempLinkInfo.scmLargeScaleParams);
		}
	  }
	}
	
      }
      
    }
    //! Assuming newNode is a Transmitter and otherNode as Receiver
    {
	if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
	{

	  if(aNodes(newNode).getCellID()!=aNodes(otherNode).getCellID())
	  {
	    //! If the new node and other node are in different cell then set the transmitter location as the newnode locaton and
          //! Get the wraparound location of newnode with respect to othernode//
          //! Finding the wraparound locations of the receiver nodes and getting linkinfo details//
	    for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
	      if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(newNode).getCellID())
		txCellLoc=aNodes(hexNodes(hexNode_cnt)).getLoc();

	      for(int hexNode_cnt=0;hexNode_cnt<length(hexNodes);hexNode_cnt++)
		if(aNodes(hexNodes(hexNode_cnt)).getCellID()==aNodes(otherNode).getCellID())
		  wrapTxLoc	=	WrapMyLocation(aNodes(hexNodes(hexNode_cnt)).getNeighbourCellLoc(aNodes(newNode).getCellID()),txCellLoc,aNodes(newNode).getLoc());
		aNodes(newNode).setWrapLoc(wrapTxLoc,true);
	  }
	  else//! If both the othernode and new node are in the same cell then wrap around location is the same cell location//
               //! And get the linkinfo of the othernode//
	    aNodes(newNode).setWrapLoc(aNodes(newNode).getLoc(),true);
	}
	aNodes(otherNode).linkToTXNode(aNodes(newNode),myTerrain,shadowType,!(aNodes(otherNode).getLinkInfoUpdateStatus()*linkStatus),retailUnAlteredLinks,retainOldLOS);
	
	//Overwriting PL,SL and isLOS to make the forward and reverse links are identical ...
	tempLinkInfo=aNodes(newNode).getLinkInfo(otherNode);
	aNodes(otherNode).overwriteLinkInfo(newNode,"pathLoss",tempLinkInfo.pathLoss);
	aNodes(otherNode).overwriteLinkInfo(newNode,"shadowLoss",tempLinkInfo.shadowLoss);
	aNodes(otherNode).overwriteLinkInfo(newNode,"isLOS",tempLinkInfo.isLOS);
	
	//! Once we got the linkinfos of all the nodes group them respectively//
	if(cellLayout==_CELLLAYOUT_HEXAGONAL_ || rndhexDropNodeType!=-1)
	{
	  rxGroupID=findGroup(aNodes(otherNode).getGroupID(),"SameLoc",true);//! Finding the group that having the othernode with the same loc(name) and getting thier IDs// 
	  txGroupID=findGroup(aNodes(newNode).getGroupID(),"SameLoc",true);//! Finding the group that having the newnode with the same loc(name) and getting thier IDs// 

	  if(rxGroupID(0)!=-1)//! Once receivers(othernode) grouping is done ,if it is the first node in the group ,set the properties as defined by linkto tx node// 
	  {
	    if(aNodeGroups(rxGroupID(0)).nodeIDs.length()==0)
	    {
	      aNodeGroups(rxGroupID(0)).nodeIDs.set_size(1);
	      aNodeGroups(rxGroupID(0)).nodeIDs(0)=otherNode;
	    }
	    else //! Else if there exits a othernode before in that group that links to the newnode(group)  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
	    {
	      tempLinkInfo=aNodes(otherNode).getLinkInfo(newNode);
	      for(int groupNode_cnt=0;groupNode_cnt<aNodeGroups(rxGroupID(0)).nodeIDs.length();groupNode_cnt++)
		if(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)!=otherNode)
		  if(aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).isLinked(newNode))
		  {
		    aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(newNode,"pathLoss",tempLinkInfo.pathLoss);
		    aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(newNode,"shadowLoss",tempLinkInfo.shadowLoss);
		    aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(newNode,"isLOS",tempLinkInfo.isLOS);
		    if(myTerrain.isForSCMChannel)
		      aNodes(aNodeGroups(rxGroupID(0)).nodeIDs(groupNode_cnt)).overwriteLinkInfo(newNode,tempLinkInfo.scmLargeScaleParams);
		  }
	    }
	  }
	  if(txGroupID(0)!=-1)//! Once transmitter(newnode) grouping is done ,if it is the first node in the group ,set the properties as defined by linkto tx node// 
	  {
	    if(aNodeGroups(txGroupID(0)).nodeIDs.length()==0)
	    {
	      aNodeGroups(txGroupID(0)).nodeIDs.set_size(1);
	      aNodeGroups(txGroupID(0)).nodeIDs(0)=newNode;
	    }
	    else//! Else if there exits a newnode before in that group that links to the othernode(group)  then set the properties of  that link (like pathloss,shadowloss,isLOS and SCM channel) that already present to remaining nodes//
	    {
	      tempLinkInfo=aNodes(otherNode).getLinkInfo(newNode);
	      for(int groupNode_cnt=0;groupNode_cnt<aNodeGroups(txGroupID(0)).nodeIDs.length();groupNode_cnt++)
		if(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt)!=newNode)
		  if(aNodes(otherNode).isLinked(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt)))
		  {
		    aNodes(otherNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),"pathLoss",tempLinkInfo.pathLoss);
		    aNodes(otherNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),"shadowLoss",tempLinkInfo.shadowLoss);
		    aNodes(otherNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),"isLOS",tempLinkInfo.isLOS);
		    if(myTerrain.isForSCMChannel)
		      aNodes(otherNode).overwriteLinkInfo(aNodeGroups(txGroupID(0)).nodeIDs(groupNode_cnt),tempLinkInfo.scmLargeScaleParams);
		  }
	    }
	  }

	}

    }
  }


}
/*!\fn removeLinkInfo()--> it removes the info of particular link between the server and service nodes
 *\param[in] nodes--> gives the number of nodes present
 *\param[in] nodeCategory--> gives the types of nodes present
 */
void McellSystem::removeLinkInfo(ivec nodes, NodeCategory_E nodeCategory){
  //! Depends upon the node category get the active node list and clear linkinfo details of those nodes//

  ivec linkedNodes;
  if(nodeCategory==_NODECATEGORY_SERVERNODE_)
    linkedNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));
  else
    linkedNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));

  for(int node_cnt=0;node_cnt<length(nodes);node_cnt++)
    for(int linkedNode_cnt=0;linkedNode_cnt<length(linkedNodes);linkedNode_cnt++)
    {
      aNodes(linkedNodes(linkedNode_cnt)).clearLinkInfo(nodes(linkedNode_cnt));
      aNodes(nodes(linkedNode_cnt)).clearLinkInfo(linkedNodes(linkedNode_cnt));
    }
}

void McellSystem::retainLinkInfo(ivec serviceNodes,Array<ivec> serverNodesPerServiceNode)
{
    ivec serverNodes = getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));
    for(int srvc_cnt=0;srvc_cnt<serviceNodes.length();srvc_cnt++)
    {
        ivec otherNodeIndices = findNot(serverNodes,serverNodesPerServiceNode(srvc_cnt));
        for(int srvr_cnt=0;srvr_cnt<otherNodeIndices.length();srvr_cnt++)
        {
            aNodes(serviceNodes(srvc_cnt)).clearLinkInfo(otherNodeIndices(srvr_cnt));
            aNodes(otherNodeIndices(srvr_cnt)).clearLinkInfo(serviceNodes(srvc_cnt));
        }
    }
}

void McellSystem::clearRedundantMemory(ivec currentSrvcNodes,Array<ivec> serverNodesPerServiceNode)
{
    channelCloud.clearSCMFixedTapGains();
    retainLinkInfo(currentSrvcNodes,serverNodesPerServiceNode);
    ivec currentSrvrNodes(0);
    for(int cnt=0;cnt<currentSrvcNodes.length();cnt++)
        append(currentSrvrNodes,serverNodesPerServiceNode(cnt));
    currentSrvrNodes = findUniqueNumbers(currentSrvrNodes);
    
    ivec serviceNodes = getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));
    ivec serverNodes = getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));
    
    ivec redundantSrvrNodes = serverNodes(findNot(serverNodes,currentSrvrNodes));
    ivec redundantSrvcNodes = serviceNodes(findNot(serviceNodes,currentSrvcNodes));
    
//     channelCloud.nodesInfo.resetBeamParameters(redundantSrvcNodes);
//     channelCloud.nodesInfo.resetBeamParameters(redundantSrvrNodes);
    
    ivec nonCurrentNodes = concat(redundantSrvrNodes,redundantSrvcNodes);
    for(int cnt=0;cnt<nonCurrentNodes.length();cnt++)
        aNodes(nonCurrentNodes(cnt)).scmGrids.set_length(0);
}


/*!\fn printLinkInfo()--> prints the link info by taking all the combinations of the nodes directly from the file
 *\param[in] servernode--> gives the list of server nodes present in the system
 *\param[in] servicenodes--> gives the list of serice nodes present in the system
 *\param[out] filename --> It is the filename in which linkinfo should be printed
 */

void McellSystem::printLinkInfo(string filename,ivec serverNodes,ivec serviceNodes){
  //! The generation of linkinfo is followed by printing of linkinfo //

  if(!isLinkInfoUpToDate()) cout<<"[detl:]Please call generateLinkInfo() for the respective nodes or the entire system."<<endl;

  NodeLinkInfo_S tempLinkInfo;
  int nodeID;
  std::ofstream fout(filename.c_str(), std::ios::out);
//! all the types of server and service  nodes which are active are taken into account// 
  if(serverNodes.length()==0)
    serverNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));
  else
    serverNodes=getActiveNodesFromList(serverNodes);

  if(serviceNodes.length()==0)
    serviceNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));
  else
    serviceNodes=getActiveNodesFromList(serviceNodes);

  fout<<"nodeID\t\tserverNodeID\t\tSrvrNodeToSrvcNodeGainInDB\t\tSrvcNodeToSrvrNodeGainInDB\n";

  //for all the service node and server node counts get thier node ids ,transmitter power ,transmitter antenna gain,rx antenna gain,path losses and shawdow losses//
  for(int srvcNode_cnt=0;srvcNode_cnt<length(serviceNodes);srvcNode_cnt++)
  {
    nodeID=aNodes(serviceNodes(srvcNode_cnt)).getID();
    fout<<nodeID<<"\t\t";
    for(int serverNodes_cnt=0;serverNodes_cnt<length(serverNodes);serverNodes_cnt++)
    {
      tempLinkInfo=aNodes(serviceNodes(srvcNode_cnt)).getLinkInfo(serverNodes(serverNodes_cnt));
      fout<<tempLinkInfo.id<<"\t\t";
      fout<<tempLinkInfo.txPowerInDBm+tempLinkInfo.txAntennaGain+tempLinkInfo.rxAntennaGain-(tempLinkInfo.pathLoss+tempLinkInfo.shadowLoss)<<"\t\t";

      tempLinkInfo=aNodes(serverNodes(serverNodes_cnt)).getLinkInfo(serviceNodes(srvcNode_cnt));
      fout<<tempLinkInfo.txPowerInDBm+tempLinkInfo.txAntennaGain+tempLinkInfo.rxAntennaGain-(tempLinkInfo.pathLoss+tempLinkInfo.shadowLoss)<<"\t\t";
    }
    fout<<endl;
  }

  fout.close();
  cout<<"[detl:]LinkInfo printed into the file."<<endl;
}
/*!\fn printLinkGainTable()--> prints the link table by taking all the combinations of the nodes directly from the file
 *\param[in] servernode--> gives the list of server nodes present in the system
 *\param[in] servicenodes--> gives the list of serice nodes present in the system
 *\param[out] filename --> It is the filename in which linkinfo should be printed
 */ 
void McellSystem::printLinkGainTable(string filename,ivec serverNodes,ivec serviceNodes, bool disableFullLinkGainPrinting){
 // printing the link gain table
  if(!isLinkInfoUpToDate()) cout<<"[detl:]Please call generateLinkInfo() for the respective nodes or the entire system."<<endl;

  NodeLinkInfo_S tempLinkInfo;
  //int rxID;
  std::ofstream fout(filename.c_str(), std::ios::out);

  ivec activeServiceNodes;
  if(serviceNodes.length()==0)
    activeServiceNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));
  else
    activeServiceNodes=getActiveNodesFromList(serviceNodes);


  ivec activeServerNodes;
  if(serverNodes.length()==0)
    activeServerNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));
  else
    activeServerNodes=getActiveNodesFromList(serverNodes);

  ivec serverNodeTypes(activeServerNodes.length());
  vec serverNodeCarrFreq(activeServerNodes.length());  
  for(int node_cnt=0;node_cnt<length(activeServerNodes);node_cnt++)
  {
    serverNodeTypes(node_cnt)=aNodes(activeServerNodes(node_cnt)).getType();
    serverNodeCarrFreq(node_cnt)=aNodes(activeServerNodes(node_cnt)).getCarrierFreqInHz();    
  }
  
  //! for Second BS Network    
  ivec serviceNodeTypes(activeServiceNodes.length());
  for(int node_cnt=0;node_cnt<length(activeServiceNodes);node_cnt++)
    serviceNodeTypes(node_cnt)=aNodes(activeServiceNodes(node_cnt)).getType();
  //! End for Second BS Network

  fout<<"serverNodeList="<<activeServerNodes<<endl;
  fout<<"serviceNodeList="<<activeServiceNodes<<endl;
  fout<<"serverNodeType="<<serverNodeTypes<<endl;
  fout<<"serviceNodeTypes="<<serviceNodeTypes<<endl; //! for Second BS Network
  fout<<"serverNodeIDCell="<<getNodeIDCellFromList(activeServerNodes)<<endl;
  fout<<"serviceNodeIDCell="<<getNodeIDCellFromList(activeServiceNodes)<<endl;
  fout<<"serverNodeCentralUnitID="<<getNodeCentralUnitIDFromList(activeServerNodes)<<endl;
  fout<<"serviceNodeCentralUnitID="<<getNodeCentralUnitIDFromList(activeServiceNodes)<<endl;
  fout<<"serverNodeNumerologies="<<getNodeNumerologiesFromList(activeServerNodes)<<endl;
  fout<<"serviceNodeNumerologies="<<getNodeNumerologiesFromList(activeServiceNodes)<<endl;
  //fout<<"serviceNodeDopplerInHz="<<getNodeVelocityFromList(activeServiceNodes)*carrierFreq/lightSpeed<<endl;// Assuming carrierFreq is same for all nodes
  fout<<"serviceNodeVelocity="<<getNodeVelocityFromList(activeServiceNodes)<<endl;
  fout<<"serverNodeAntennaCount="<<getNodeAntennaCountFromList(activeServerNodes)<<endl;
  fout<<"serviceNodeAntennaCount="<<getNodeAntennaCountFromList(activeServiceNodes)<<endl;
  fout<<"serverNodeTransmitPowerInDBm="<<getNodeTxPowerInDBmFromList(activeServerNodes)<<endl;
  fout<<"serviceNodeTransmitPowerInDBm="<<getNodeTxPowerInDBmFromList(activeServiceNodes)<<endl;
  fout<<"serverNodeCarrierFreqInHz="<<serverNodeCarrFreq<<endl; 
  
  if(noiseFigurePerNodeType.length())
  {
    vec serverNodeNoiseFigures(0),serviceNodeNoiseFigures(0);
    for(int nodeType_cnt=0;nodeType_cnt<aNodeTypes.length();nodeType_cnt++)
    {
      ivec activeNodes = getActiveNodes(aNodeTypes(nodeType_cnt));
      int selectedNodeCount ; 
      if(nodeCategoryPerNodeType(nodeType_cnt)==_NODECATEGORY_SERVERNODE_)
      {
	selectedNodeCount = findNot(find(activeServerNodes,activeNodes),-1).length();
	append(serverNodeNoiseFigures,ones(selectedNodeCount)*noiseFigurePerNodeType(nodeType_cnt));
      }
      else
      {
	selectedNodeCount = findNot(find(activeServiceNodes,activeNodes),-1).length();
	append(serviceNodeNoiseFigures,ones(selectedNodeCount)*noiseFigurePerNodeType(nodeType_cnt));
      }
    }
    fout<<"serverNodeReceiveNoiseFigureInDB="<<serverNodeNoiseFigures<<endl;
    fout<<"serviceNodeReceiveNoiseFigureInDB="<<serviceNodeNoiseFigures<<endl;
  }
  
  //! printing serverNodeNetworkIDs & serviceNodeNetworkIDs to file for association   
      if(networkIDPerNodeType.length())
	  {
	    ivec serverNodeNetworkIDs(0),serviceNodeNetworkIDs(0);
	    for(int nodeType_cnt=0;nodeType_cnt<aNodeTypes.length();nodeType_cnt++)
	    {
	      ivec activeNodes = getActiveNodes(aNodeTypes(nodeType_cnt));
	      int selectedNodeCount ;
	      if(nodeCategoryPerNodeType(nodeType_cnt)==_NODECATEGORY_SERVERNODE_)
	      {
		selectedNodeCount = findNot(find(activeServerNodes,activeNodes),-1).length();
		append(serverNodeNetworkIDs,ones_i(selectedNodeCount)*networkIDPerNodeType(nodeType_cnt));
	      }
	      else
	      {
		selectedNodeCount = findNot(find(activeServiceNodes,activeNodes),-1).length();
		append(serviceNodeNetworkIDs,ones_i(selectedNodeCount)*networkIDPerNodeType(nodeType_cnt));
	      }
	    }
	    fout<<"serverNodeNetworkIDs="<<serverNodeNetworkIDs<<endl;
	    fout<<"serviceNodeNetworkIDs="<<serviceNodeNetworkIDs<<endl;
	  }  
  //! end for printing
  
  
  double unlinkedGain=-500;  //! If there is no link between the nodes set the gain values as -500 (default)//
  
  fout<<"serverToServiceNodeLinkGainInDB=\"";
  //! For all the receiver nodes(service) which are active(UE) get the linkinfo to all server nodes //
  //! And calculate the server to serrvice node linkgain by using  the formulae//
  //! If there is no link set the default value//
  for(int rxNode_cnt=0;rxNode_cnt<length(activeServiceNodes);rxNode_cnt++)
  {
    //rxID=aNodes(activeServiceNodes(rxNode_cnt)).getID();
    for(int txActiveNode_cnt=0;txActiveNode_cnt<length(activeServerNodes);txActiveNode_cnt++)
      if(aNodes(activeServiceNodes(rxNode_cnt)).isLinked(activeServerNodes(txActiveNode_cnt)))
      {
	tempLinkInfo=aNodes(activeServiceNodes(rxNode_cnt)).getLinkInfo(activeServerNodes(txActiveNode_cnt));
	fout<<tempLinkInfo.txAntennaGain+tempLinkInfo.rxAntennaGain-(tempLinkInfo.pathLoss+tempLinkInfo.shadowLoss)<<" ";
	
    // cout<<tempLinkInfo.txAntennaGain<<'\t'<<'\t'<<tempLinkInfo.rxAntennaGain<<'\t'<<'\t'<<tempLinkInfo.pathLoss<<'\t'<<'\t'<<tempLinkInfo.shadowLoss<<endl;
		
      }
      else 
	fout<<unlinkedGain<<" ";
    fout<<";";
  } 
  fout<<"\""<<endl;
  //AAM
  //isLOS
  fout<<"isLOS =\"";
  for(int rxNode_cnt=0;rxNode_cnt<length(activeServiceNodes);rxNode_cnt++)
  {
    //rxID=aNodes(activeServiceNodes(rxNode_cnt)).getID();
    for(int txActiveNode_cnt=0;txActiveNode_cnt<length(activeServerNodes);txActiveNode_cnt++)
      if(aNodes(activeServiceNodes(rxNode_cnt)).isLinked(activeServerNodes(txActiveNode_cnt)))
      {
	tempLinkInfo=aNodes(activeServiceNodes(rxNode_cnt)).getLinkInfo(activeServerNodes(txActiveNode_cnt));
	fout<<tempLinkInfo.isLOS<<" ";
      }
      else 
	fout<<"-100.123"<<" ";
      fout<<";";
  }
  fout<<"\""<<endl;
  
  fout<<"serverToServiceNodePathLossInDB=\"";
  for(int rxNode_cnt=0;rxNode_cnt<length(activeServiceNodes);rxNode_cnt++)
  {
    //rxID=aNodes(activeServiceNodes(rxNode_cnt)).getID();
    for(int txActiveNode_cnt=0;txActiveNode_cnt<length(activeServerNodes);txActiveNode_cnt++)
      if(aNodes(activeServiceNodes(rxNode_cnt)).isLinked(activeServerNodes(txActiveNode_cnt)))
      {
	tempLinkInfo=aNodes(activeServiceNodes(rxNode_cnt)).getLinkInfo(activeServerNodes(txActiveNode_cnt));
	fout<<tempLinkInfo.pathLoss<<" ";
      }
      else 
	fout<<"-100.123"<<" ";
      fout<<";";
  }
  fout<<"\""<<endl;
  
  fout<<"serverToServiceNodeShadowLossInDB=\"";
  for(int rxNode_cnt=0;rxNode_cnt<length(activeServiceNodes);rxNode_cnt++)
  {
    //rxID=aNodes(activeServiceNodes(rxNode_cnt)).getID();
    for(int txActiveNode_cnt=0;txActiveNode_cnt<length(activeServerNodes);txActiveNode_cnt++)
      if(aNodes(activeServiceNodes(rxNode_cnt)).isLinked(activeServerNodes(txActiveNode_cnt)))
      {
	tempLinkInfo=aNodes(activeServiceNodes(rxNode_cnt)).getLinkInfo(activeServerNodes(txActiveNode_cnt));
	fout<<tempLinkInfo.shadowLoss<<" ";
      }
      else 
	fout<<"-123"<<" ";
      fout<<";";
  }
  fout<<"\""<<endl;
  
  
  fout<<"serverToServiceNodeTxAntennaGainInDB=\"";
  for(int rxNode_cnt=0;rxNode_cnt<length(activeServiceNodes);rxNode_cnt++)
  {
    //rxID=aNodes(activeServiceNodes(rxNode_cnt)).getID();
    for(int txActiveNode_cnt=0;txActiveNode_cnt<length(activeServerNodes);txActiveNode_cnt++)
      if(aNodes(activeServiceNodes(rxNode_cnt)).isLinked(activeServerNodes(txActiveNode_cnt)))
      {
	tempLinkInfo=aNodes(activeServiceNodes(rxNode_cnt)).getLinkInfo(activeServerNodes(txActiveNode_cnt));
	fout<<tempLinkInfo.txAntennaGain<<" ";
      }
      else 
	fout<<"-80"<<" ";
      fout<<";";
  }
  fout<<"\""<<endl;
  
  fout<<"serverToServiceNodeRxAntennaGainInDB=\"";
  for(int rxNode_cnt=0;rxNode_cnt<length(activeServiceNodes);rxNode_cnt++)
  {
    //rxID=aNodes(activeServiceNodes(rxNode_cnt)).getID();
    for(int txActiveNode_cnt=0;txActiveNode_cnt<length(activeServerNodes);txActiveNode_cnt++)
      if(aNodes(activeServiceNodes(rxNode_cnt)).isLinked(activeServerNodes(txActiveNode_cnt)))
      {
	tempLinkInfo=aNodes(activeServiceNodes(rxNode_cnt)).getLinkInfo(activeServerNodes(txActiveNode_cnt));
	fout<<tempLinkInfo.rxAntennaGain<<" ";
      }
      else 
	fout<<"-80"<<" ";
      fout<<";";
  }
  fout<<"\""<<endl;
  // AAM end
  
  fout<<"serviceToServerNode=\"";
  for(int rxNode_cnt=0;rxNode_cnt<length(activeServerNodes);rxNode_cnt++)
  {
    //! For all the transmitter nodes(service) which are active(UE) get the linkinfo to all server nodes //
  //! And calculate the service to server node linkgain by using  the formulae//
  //! If there is no link set the default value//
    //rxID=aNodes(activeServerNodes(rxNode_cnt)).getID();
    for(int txActiveNode_cnt=0;txActiveNode_cnt<length(activeServiceNodes);txActiveNode_cnt++)
      if(aNodes(activeServerNodes(rxNode_cnt)).isLinked(activeServiceNodes(txActiveNode_cnt)))  
      {
	tempLinkInfo=aNodes(activeServerNodes(rxNode_cnt)).getLinkInfo(activeServiceNodes(txActiveNode_cnt));
	fout<<tempLinkInfo.txAntennaGain+tempLinkInfo.rxAntennaGain-(tempLinkInfo.pathLoss+tempLinkInfo.shadowLoss)<<" ";
      }
      else
	fout<<unlinkedGain<<" ";
    fout<<";";
  }
  fout<<"\""<<endl;

  if(disableFullLinkGainPrinting==false)
  {
    ivec nodeList=concat(activeServerNodes,activeServiceNodes); // getting the nodelist //
    fout<<"nodeList="<<nodeList<<endl;

    mat pathDelays=zeros(nodeList.length(),nodeList.length());

    
    fout<<"linkGainInDB=\"";
    for(int rxNode_cnt=0;rxNode_cnt<length(nodeList);rxNode_cnt++)
    {
      //rxID=aNodes(nodeList(rxNode_cnt)).getID();
      for(int txActiveNode_cnt=0;txActiveNode_cnt<length(nodeList);txActiveNode_cnt++)
	if(aNodes(nodeList(rxNode_cnt)).isLinked(nodeList(txActiveNode_cnt))) // If isCrossLinksEnabled is disabled, this condition will fail for all the crosslinks
	{
	  tempLinkInfo=aNodes(nodeList(rxNode_cnt)).getLinkInfo(nodeList(txActiveNode_cnt));
	  pathDelays(rxNode_cnt,txActiveNode_cnt)=tempLinkInfo.pathDelay;
	  fout<<tempLinkInfo.txAntennaGain+tempLinkInfo.rxAntennaGain-(tempLinkInfo.pathLoss+tempLinkInfo.shadowLoss)<<" ";
     // fout<<-(tempLinkInfo.pathLoss+tempLinkInfo.shadowLoss)<<" ";
    }
	else
	{
	  pathDelays(rxNode_cnt,txActiveNode_cnt)=100;//Loading an unrealistic value..
	  fout<<unlinkedGain<<" ";
	}
      fout<<";";
    }
    fout<<"\""<<endl;

    fout<<"propagationDelayInSec=\"";
    for(int row_cnt=0;row_cnt<pathDelays.rows();row_cnt++)
    {
      for(int col_cnt=0;col_cnt<pathDelays.cols();col_cnt++)
	fout<<pathDelays(row_cnt,col_cnt)<<" ";
      fout<<";";
    }
    fout<<"\""<<endl;
    fout.close();
  }
  

  if(currentTaskid==0)
    cout<<"[detl:]LinkBudget printed into the file."<<endl;

}
/*!\fn printCrossLinkGainTable()--> prints the cross-link table by taking all the combinations of the nodes directly from the file
 * \param[in] servernode--> gives the list of server nodes present in the system
 *\param[in] servicenodes--> gives the list of serice nodes present in the system
 *\param[out] filename --> It is the filename in which linkinfo should be printed
 */
void McellSystem::printCrossLinkGainTable(string filename,ivec serverNodes,ivec serviceNodes, bool appendInFile){

  if(!isLinkInfoUpToDate()) cout<<"[detl:]Please call generateLinkInfo() for the respective nodes or the entire system."<<endl;

  NodeLinkInfo_S tempLinkInfo;
  //int rxID;
  std::ofstream fout;
  if(appendInFile)
    fout.open(filename.c_str(), std::ios::app);
  else
    fout.open(filename.c_str(), std::ios::out);

  ivec activeServiceNodes;
  if(serviceNodes.length()==0)
    activeServiceNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));
  else
    activeServiceNodes=getActiveNodesFromList(serviceNodes);


  ivec activeServerNodes;
  if(serverNodes.length()==0)
    activeServerNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));
  else
    activeServerNodes=getActiveNodesFromList(serverNodes);

  ivec serverNodeTypes(activeServerNodes.length());
  for(int node_cnt=0;node_cnt<length(activeServerNodes);node_cnt++)
    serverNodeTypes(node_cnt)=aNodes(activeServerNodes(node_cnt)).getType();

  double unlinkedGain=-500;  //! If there is no link between the nodes set the gain values as -500 (default)//

  ivec nodeList=concat(activeServerNodes,activeServiceNodes); // getting the nodelist //
  fout<<"nodeList="<<nodeList<<endl;
  fout<<"nodeVelocity="<<getNodeVelocityFromList(nodeList)<<endl;

  mat pathDelays=zeros(nodeList.length(),nodeList.length());


  fout<<"linkGainInDB=\"";
  for(int rxNode_cnt=0;rxNode_cnt<length(nodeList);rxNode_cnt++)
  {
    //rxID=aNodes(nodeList(rxNode_cnt)).getID();
    for(int txActiveNode_cnt=0;txActiveNode_cnt<length(nodeList);txActiveNode_cnt++)
      if(aNodes(nodeList(rxNode_cnt)).isLinked(nodeList(txActiveNode_cnt))) // If isCrossLinksEnabled is disabled, this condition will fail for all the crosslinks
      {
    tempLinkInfo=aNodes(nodeList(rxNode_cnt)).getLinkInfo(nodeList(txActiveNode_cnt));
    pathDelays(rxNode_cnt,txActiveNode_cnt)=tempLinkInfo.pathDelay;
    fout<<tempLinkInfo.txAntennaGain+tempLinkInfo.rxAntennaGain-(tempLinkInfo.pathLoss+tempLinkInfo.shadowLoss)<<" ";
//    fout<<-(tempLinkInfo.pathLoss+tempLinkInfo.shadowLoss)<<" ";
      }
      else
      {
    pathDelays(rxNode_cnt,txActiveNode_cnt)=100;//Loading an unrealistic value..
    fout<<unlinkedGain<<" ";
      }
      fout<<";";
  }
  fout<<"\""<<endl;

  fout<<"propagationDelayInSec=\"";
  for(int row_cnt=0;row_cnt<pathDelays.rows();row_cnt++)
  {
    for(int col_cnt=0;col_cnt<pathDelays.cols();col_cnt++)
      fout<<pathDelays(row_cnt,col_cnt)<<" ";
    fout<<";";
  }
  fout<<"\""<<endl;


  fout.close();
  #ifdef USING_MPI
  if(currentTaskid==0) {
    #endif
    cout<<"[detl:]Cross-Link Info printed into the file."<<endl;
    #ifdef USING_MPI
  }
  #endif

}

void McellSystem::printCarrierFrequencyInfo(string filename,ivec serverNodes,ivec serviceNodes, bool appendInFile){
  
  std::ofstream fout;
  if(appendInFile)
    fout.open(filename.c_str(), std::ios::app);
  else
    fout.open(filename.c_str(), std::ios::out);
  ivec activeServiceNodes;
  if(serviceNodes.length()==0)
    activeServiceNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));
  else
    activeServiceNodes=getActiveNodesFromList(serviceNodes);
  
  
  ivec activeServerNodes;
  if(serverNodes.length()==0)
    activeServerNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));
  else
    activeServerNodes=getActiveNodesFromList(serverNodes);
  ivec nodeList=concat(activeServerNodes,activeServiceNodes);
  
  fout<<"carrierFrequencyInHz="<<getNodeCarrierFreuencyInHzFromList(nodeList)<<endl;
  fout.close();
}

void McellSystem::printDownlinkPowerMatrix(string filename,ivec serverNodes,ivec serviceNodes){
  
  if(!isLinkInfoUpToDate()) cout<<"[detl:]Please call generateLinkInfo() for the respective nodes or the entire system."<<endl;
  
  NodeLinkInfo_S tempLinkInfo;
  std::ofstream fout(filename.c_str(), std::ios::out);
  
  ivec activeServiceNodes;
  if(serviceNodes.length()==0)
    activeServiceNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));
  else
    activeServiceNodes=getActiveNodesFromList(serviceNodes);
  
  
  ivec activeServerNodes;
  if(serverNodes.length()==0)
    activeServerNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));
  else
    activeServerNodes=getActiveNodesFromList(serverNodes);
  
  ivec serverNodeTypes(activeServerNodes.length());
  for(int node_cnt=0;node_cnt<length(activeServerNodes);node_cnt++)
    serverNodeTypes(node_cnt)=aNodes(activeServerNodes(node_cnt)).getType();
  
  vec dlTxPow=getNodeTxPowerInDBmFromList(activeServerNodes);
  
  double unlinkedGain=-500;
  
  fout<<"-1 ";
  for(int txActiveNode_cnt=0;txActiveNode_cnt<length(activeServerNodes);txActiveNode_cnt++)
    fout<<activeServerNodes(txActiveNode_cnt)<<" ";
  fout<<endl;  
  for(int rxNode_cnt=0;rxNode_cnt<length(activeServiceNodes);rxNode_cnt++)
  {
    fout<<activeServiceNodes(rxNode_cnt)<<" ";
    for(int txActiveNode_cnt=0;txActiveNode_cnt<length(activeServerNodes);txActiveNode_cnt++)
      if(aNodes(activeServiceNodes(rxNode_cnt)).isLinked(activeServerNodes(txActiveNode_cnt)))
      {
	tempLinkInfo=aNodes(activeServiceNodes(rxNode_cnt)).getLinkInfo(activeServerNodes(txActiveNode_cnt));
	fout<<dlTxPow(txActiveNode_cnt)+tempLinkInfo.txAntennaGain+tempLinkInfo.rxAntennaGain-(tempLinkInfo.pathLoss+tempLinkInfo.shadowLoss)<<" ";
      }
      else
	fout<<unlinkedGain<<" ";
      fout<<endl;
  }
  
  fout.close();
  #ifdef USING_MPI
  if(currentTaskid==0) {
    #endif
    cout<<"[detl:]PowerMatrix printed into the file."<<endl;
    #ifdef USING_MPI
  }
  #endif
}

void McellSystem::printShadowLossMatrix(string filename,ivec serverNodes,ivec serviceNodes){
  
  if(!isLinkInfoUpToDate()) cout<<"[detl:]Please call generateLinkInfo() for the respective nodes or the entire system."<<endl;
  
  NodeLinkInfo_S tempLinkInfo;
  std::ofstream fout(filename.c_str(), std::ios::out);
  
  ivec activeServiceNodes;
  if(serviceNodes.length()==0)
    activeServiceNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));
  else
    activeServiceNodes=getActiveNodesFromList(serviceNodes);
  
  
  ivec activeServerNodes;
  if(serverNodes.length()==0)
    activeServerNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));
  else
    activeServerNodes=getActiveNodesFromList(serverNodes);
  
  ivec serverNodeTypes(activeServerNodes.length());
  for(int node_cnt=0;node_cnt<length(activeServerNodes);node_cnt++)
    serverNodeTypes(node_cnt)=aNodes(activeServerNodes(node_cnt)).getType();
  
  vec dlTxPow=getNodeTxPowerInDBmFromList(activeServerNodes);
  
  double unlinkedGain=-500;
  
  fout<<"-1 ";
  for(int txActiveNode_cnt=0;txActiveNode_cnt<length(activeServerNodes);txActiveNode_cnt++)
    fout<<activeServerNodes(txActiveNode_cnt)<<" ";
  fout<<endl;  
  for(int rxNode_cnt=0;rxNode_cnt<length(activeServiceNodes);rxNode_cnt++)
  {
    fout<<activeServiceNodes(rxNode_cnt)<<" ";
    for(int txActiveNode_cnt=0;txActiveNode_cnt<length(activeServerNodes);txActiveNode_cnt++)
      if(aNodes(activeServiceNodes(rxNode_cnt)).isLinked(activeServerNodes(txActiveNode_cnt)))
      {
	tempLinkInfo=aNodes(activeServiceNodes(rxNode_cnt)).getLinkInfo(activeServerNodes(txActiveNode_cnt));
	fout<<tempLinkInfo.shadowLoss<<" ";
      }
      else
	fout<<unlinkedGain<<" ";
      fout<<endl;
  }
  
  fout.close();
  #ifdef USING_MPI
  if(currentTaskid==0) {
    #endif
    cout<<"[detl:]ShadowLossMatrix printed into the file."<<endl;
    #ifdef USING_MPI
  }
  #endif
}

/*! \fn printDetailedLinkInfo()
 * \brief --> it prints the detailed link info of all the nodes like nodeID,srvrNodeTxAntennaGainInDB,srvcNodeRxAntennaGainInDB
 *\param[in] servernode--> this function takes the input of all servernode
 *\param[in] servicenode--> this function takes the input of all service nodes
 *\param[out] filename --> it gives the filename in which detailed linkInfo should be printed
 */
void McellSystem::printDetailedLinkInfo(string filename,ivec serverNodes,ivec serviceNodes){


  if(!isLinkInfoUpToDate()) cout<<"[detl:]Please call generateLinkInfo() for the respective nodes or the entire system."<<endl;

  NodeLinkInfo_S tempLinkInfo;
  int nodeID;
  std::ofstream fout(filename.c_str(), std::ios::out);

  if(serverNodes.length()==0)
    serverNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVERNODE_));
  else 
    serverNodes=getActiveNodesFromList(serverNodes);

  if(serviceNodes.length()==0)
    serviceNodes=getActiveNodesFromList(getNodes(_NODECATEGORY_SERVICENODE_));
  else
    serviceNodes=getActiveNodesFromList(serviceNodes);

  fout<<"%nodeID,serverNodeID,distance,tx_xLoc,tx_yLoc,rx_xLoc,rx_yLoc,srvrNodeTxAntennaGainInDB,srvcNodeRxAntennaGainInDB,isLOS,PathlossInDB,ShadowlossInDB,srvcNodeTxAntennaGainInDB,srvrNodeRxAntennaGainInDB,isLOS,PathlossInDB,ShadowlossInDB,isIndoor\n";

  for(int serviceNode_cnt=0;serviceNode_cnt<length(serviceNodes);serviceNode_cnt++)
  {
    
    nodeID=aNodes(serviceNodes(serviceNode_cnt)).getID();
    for(int serverNodes_cnt=0;serverNodes_cnt<length(serverNodes);serverNodes_cnt++)
    {
      fout<<nodeID<<",";
      tempLinkInfo=aNodes(serviceNodes(serviceNode_cnt)).getLinkInfo(serverNodes(serverNodes_cnt));
      fout<<tempLinkInfo.id<<","<<tempLinkInfo.distance<<","<<tempLinkInfo.txLocation.x<<","<<tempLinkInfo.txLocation.y<<","<<tempLinkInfo.rxLocation.x<<","<<tempLinkInfo.rxLocation.y<<",";
      fout<<tempLinkInfo.txAntennaGain<<","<<tempLinkInfo.rxAntennaGain<<","<<tempLinkInfo.isLOS<<","<<tempLinkInfo.pathLoss<<","<<tempLinkInfo.shadowLoss<<",";

      tempLinkInfo=aNodes(serverNodes(serverNodes_cnt)).getLinkInfo(serviceNodes(serviceNode_cnt));
      fout<<tempLinkInfo.txAntennaGain<<","<<tempLinkInfo.rxAntennaGain<<","<<tempLinkInfo.isLOS<<","<<tempLinkInfo.pathLoss<<","<<tempLinkInfo.shadowLoss<<","<<aNodes(serviceNodes(serviceNode_cnt)).isIndoor()<<",";
      fout<<endl;
    }
  }

  fout.close();
#ifdef USING_MPI
if(currentTaskid==0) {
#endif
  cout<<"[detl:]LinkInfo printed into the file."<<endl;
#ifdef USING_MPI
}
#endif

}

/*! \fn printNodeInfo() --> prints the information regarding the nodes into a seperate file
 * \param nodelist[in]-->gives the number of nodes present
 * \param[in] filename --> it gives the filename in which Nodeinfo should be printed
 * */

void McellSystem::printNodeInfo(string filename,ivec nodeList){

  if(!isLinkInfoUpToDate()) cout<<"[detl:]Please call generateLinkInfo() for the respective nodes or the entire system."<<endl;

  Location_S nodeLoc;
  std::ofstream fout(filename.c_str(), std::ios::out);

  if(nodeList.length()==0)
	for(int node_cnt=0;node_cnt<sum(nNodes);node_cnt++)
	{
	  nodeLoc=aNodes(node_cnt).getLoc();
	  fout<<aNodes(node_cnt).getID()<<"\t"<<nodeLoc.x<<"\t"<<nodeLoc.y<<"\t"<<nodeLoc.z<<"\t"<<aNodes(node_cnt).getType()<<endl;
	}
  else
	for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
	{
	  nodeLoc=aNodes(nodeList(node_cnt)).getLoc();
	  fout<<aNodes(nodeList(node_cnt)).getID()<<"\t"<<nodeLoc.x<<"\t"<<nodeLoc.y<<"\t"<<nodeLoc.z<<"\t"<<aNodes(nodeList(node_cnt)).getType()<<endl;
	}

  fout.close();
  cout<<"[detl:]NodeInfo printed into the file."<<endl;
}

/*!\fn printNodeLocation
 *\brief -->  It prints the nodelocations of all the nodes when node IDs are given into particular file
 * \param nodeIDs[in]-->it gives the node id of all the nodes
 * \param filename[in]-->it give the filename to which node ID's should be printed
 */
void McellSystem::printNodeLocations(string filename, ivec nodeIDs){

  std::ofstream fout(filename.c_str(), std::ios::out);
  fout<<"%\"NodeType\",\t\"NodeID\",\t\"X_Location\",\t\"Y_Location\",\t\"Z_Location\",\t\"isIndoor\",\t\"isWithLowPenetraionLoss\",\t\"floorNumber\",\t\"isPedestrian\",\t\"Orientation\",\t\"nodeVelocity\""<<endl;

  //! if node IDs are specified it will print location by using those IDs else it will print directly//
  if(nodeIDs.length()==0)
  {
    for(int nodeType_cnt=0;nodeType_cnt<nNodes.length();nodeType_cnt++)
      if(nNodes(nodeType_cnt))
    {
      ivec temp=getActiveNodesFromList(getNodes(nodeType_cnt));
      for(int node_cnt=0;node_cnt<temp.length();node_cnt++)
      {
	Location_S tempLoc=aNodes(temp(node_cnt)).getLoc();
    fout<<"\""<<aNodeTypes(nodeType_cnt)<<"\"\t"<<temp(node_cnt)<<"\t"<<tempLoc.x<<"\t"<<tempLoc.y<<"\t"<<tempLoc.z<<"\t"<<tempLoc.isIndoor<<"\t"<<tempLoc.isLowPenetrationLoss<<"\t"<<tempLoc.floorNumber<<"\t"<<setw(10);
    fout<<tempLoc.isPedestrian<<"\t"<<aNodes(temp(node_cnt)).nodeOrient<<"\t"<<aNodes(temp(node_cnt)).getVelocityVector(false)<<";"<<endl;
      }
    }
  }
  else
  {

    ivec nodeTypes=getNodeTypeFromList(nodeIDs);
    for(int nodeType_cnt=0;nodeType_cnt<nNodes.length();nodeType_cnt++)
      if(nNodes(nodeType_cnt))
      {
	ivec temp=getActiveNodesFromList(nodeIDs(find(nodeTypes,nodeType_cnt,true)));
	for(int node_cnt=0;node_cnt<temp.length();node_cnt++)
	{
	  Location_S tempLoc=aNodes(temp(node_cnt)).getLoc();
	  fout<<aNodeTypes(nodeType_cnt)<<"\t"<<temp(node_cnt)<<"\t"<<tempLoc.x<<"\t"<<tempLoc.y<<"\t"<<tempLoc.z<<endl;
	}
      }
  }

  fout.close();
}

// void System::printSortedTxInfo(string filename,ivec rxNodeList, int numberOfTxToPrintExplicitly, string txNodeType){
//   int nodeType;
//   if(txNodeType!="")
// 	nodeType=findInStringArray(aNodeTypes,txNodeType);
//   else
// 	nodeType=-2;
//
//   if(nodeType==-1)  {cout<<"[both:] Undefined Node Name to printSortedTxInfo()... ";abort();}
//
//   if(!isLinkInfoUpToDate()) cout<<"Please call generateLinkInfo() for the respective nodes or the entire system..."<<endl;
//
//   std::ofstream fout(filename.c_str(), std::ios::out);
//   ivec rxActiveNodes;
//   if(rxNodeList.length()==0)
// 	rxActiveNodes=getRxNodesFromList(getActiveNodes());
//   else
// 	rxActiveNodes=getActiveNodesFromList(rxNodeList);
//
//   ivec txActiveNodes=getTxNodesFromList(getActiveNodes());
//
//
//   Array<vec> rxPower(rxActiveNodes.length());
//   Array<ivec> txType(rxActiveNodes.length());
//   Array<ivec> txID(rxActiveNodes.length());
//   ivec sortIndex;
//   NodeLinkInfo_S tempLinkInfo;
//   if(numberOfTxToPrintExplicitly==-1) numberOfTxToPrintExplicitly=0;
//   fout<<"rxNodes="<<rxActiveNodes<<endl;
//   fout<<"txNodes="<<txActiveNodes<<endl;
//   fout<<"txPowersInDBm="<<getNodeTxPowerInDBmFromList(txActiveNodes)<<endl;
//   string outputString;
//   for(int rxNode_cnt=0;rxNode_cnt<length(rxActiveNodes);rxNode_cnt++)
//   {
// 	txType(rxNode_cnt).set_size(0);
// 	rxPower(rxNode_cnt).set_size(0);
// 	txID(rxNode_cnt).set_size(0);
// 	for(int link_cnt=0;link_cnt<aNodes(rxActiveNodes(rxNode_cnt)).getLinkCount();link_cnt++)
// 	{
// 	  tempLinkInfo=aNodes(rxActiveNodes(rxNode_cnt)).getLinkInfoByIndex(link_cnt);
// 	  if(nodeType==-2 || tempLinkInfo.txType==nodeType)
// 	  {
// 		rxPower(rxNode_cnt)=concat(rxPower(rxNode_cnt),(double)aNodes(rxActiveNodes(rxNode_cnt)).getRxPowerByIndex(link_cnt));
// 		txType(rxNode_cnt)=concat(txType(rxNode_cnt),tempLinkInfo.txType);
// 		txID(rxNode_cnt)=concat(txID(rxNode_cnt),tempLinkInfo.id);
// 	  }
// 	}
// 	sortIndex=sort_index(rxPower(rxNode_cnt));
// 	sortIndex=reverse(sortIndex);
// 	txID(rxNode_cnt)=txID(rxNode_cnt)(sortIndex);
// 	txType(rxNode_cnt)=txType(rxNode_cnt)(sortIndex);
// 	sort(rxPower(rxNode_cnt));
// 	rxPower(rxNode_cnt)=reverse(rxPower(rxNode_cnt));
// 	outputString="txID"+toString(rxActiveNodes(rxNode_cnt));
// 	fout<<outputString<<"="<<txID(rxNode_cnt)(0,numberOfTxToPrintExplicitly-1)<<endl;
// 	outputString="txType"+toString(rxActiveNodes(rxNode_cnt));
// 	fout<<outputString<<"="<<txType(rxNode_cnt)(0,numberOfTxToPrintExplicitly-1)<<endl;
// 	outputString="rxPower"+toString(rxActiveNodes(rxNode_cnt));
// 	fout<<outputString<<"="<<rxPower(rxNode_cnt)(0,numberOfTxToPrintExplicitly-1)<<endl;
//   }
// }
//
// void System::printSortedRxPower(string filename,ivec rxNodeList, int numberOfTxToPrintExplicitly, bool normalised, bool calculateSIR, double minSir, double maxSir){
//
//   if(!isLinkInfoUpToDate()) cout<<"Please call generateLinkInfo() for the respective nodes or the entire system before calling printSortedRxPower()..."<<endl;
//
//   std::ofstream fout(filename.c_str(), std::ios::out);
//   ivec rxActiveNodes;
//   if(rxNodeList.length()==0)
// 	rxActiveNodes=getRxNodesFromList(getActiveNodes());
//   else
// 	rxActiveNodes=getActiveNodesFromList(rxNodeList);
//
//   Array<vec> rxPower(rxActiveNodes.length());
//   vec sir;
//   if(calculateSIR)
// 	sir.set_size(rxActiveNodes.length());
//   bool printInfo=false;
//   for(int rxNode_cnt=0;rxNode_cnt<length(rxActiveNodes);rxNode_cnt++)
//   {
// 	rxPower(rxNode_cnt).set_size(aNodes(rxActiveNodes(rxNode_cnt)).getLinkCount());
// 	for(int link_cnt=0;link_cnt<aNodes(rxActiveNodes(rxNode_cnt)).getLinkCount();link_cnt++)
// 	  rxPower(rxNode_cnt)(link_cnt)=aNodes(rxActiveNodes(rxNode_cnt)).getRxPowerByIndex(link_cnt);
// 	sort(rxPower(rxNode_cnt));
// 	rxPower(rxNode_cnt)=reverse(rxPower(rxNode_cnt));
// 	if(normalised)
// 	  rxPower(rxNode_cnt)-=rxPower(rxNode_cnt)(0);
//
// 	if(!calculateSIR)
// 	{
// 	  if(numberOfTxToPrintExplicitly!=0)
// 	  if(numberOfTxToPrintExplicitly!=-1)
// 	  {
// 		fout<<rxPower(rxNode_cnt)(0,numberOfTxToPrintExplicitly-1)<<"\t";
// 		fout<<dB(sum(inv_dB(rxPower(rxNode_cnt).get(numberOfTxToPrintExplicitly,-1))))<<"\n";
// 	  }
// 	  else
// 		fout<<rxPower(rxNode_cnt)<<"\n";
// 	}
// 	else
// 	{
// 	  sir(rxNode_cnt)=dB(inv_dB(rxPower(rxNode_cnt).get(0))/sum(inv_dB(rxPower(rxNode_cnt).get(1,-1))));
// 	  if(!std::isnan(minSir))
// 		{if(sir(rxNode_cnt)>=minSir) printInfo=true; else printInfo=false;}
// 	  else
// 		printInfo=true;
// 	  if(!std::isnan(maxSir))
// 		{if(sir(rxNode_cnt)<=maxSir) printInfo=true; else printInfo=false;}
// 	  else
// 		printInfo=true;
//
//
// 	  if(printInfo)
// 	  {
// 		if(numberOfTxToPrintExplicitly!=0)
// 		if(numberOfTxToPrintExplicitly!=-1)
// 		{
// 		  fout<<rxPower(rxNode_cnt)(0,numberOfTxToPrintExplicitly-1)<<"\t";
// 		  fout<<dB(sum(inv_dB(rxPower(rxNode_cnt).get(numberOfTxToPrintExplicitly,-1))))<<"\t";
// 		}
// 		else
// 		  fout<<rxPower(rxNode_cnt)<<"\t";
// 		fout<<sir(rxNode_cnt)<<endl;
// 	  }
// 	}
//
//   }
//   fout.close();
//   cout<<"Rx powers printed into file..."<<endl;
// }
//
// void System::printSCMLargeScaleParameters(string filename,ivec rxNodeList, ivec txNodeList){
//
//   if(!isLinkInfoUpToDate()) cout<<"Please call generateLinkInfo() for the respective nodes or the entire system before calling printSCMLargeScaleParameters()..."<<endl;
//
//   NodeLinkInfo_S tempLinkInfo;
//   int rxID;
//   std::ofstream fout(filename.c_str(), std::ios::out);
//
//   ivec rxActiveNodes;
//   if(rxNodeList.length()==0)
//     rxActiveNodes=getRxNodesFromList(getActiveNodes());
//   else
//     rxActiveNodes=getActiveNodesFromList(rxNodeList);
//
//
//   ivec txActiveNodes;
//   if(txNodeList.length()!=0)
//     txActiveNodes=getActiveNodesFromList(txNodeList);
//
//   fout<<"rxNodes="<<rxActiveNodes<<endl;
//   fout<<"txNodes="<<txActiveNodes<<endl;
//
//   for(int rxNode_cnt=0;rxNode_cnt<length(rxActiveNodes);rxNode_cnt++)
//   {
//     rxID=aNodes(rxActiveNodes(rxNode_cnt)).getID();
//     if(txNodeList.length()==0)
//       for(int link_cnt=0;link_cnt<aNodes(rxActiveNodes(rxNode_cnt)).getLinkCount();link_cnt++)
//       {
// 	tempLinkInfo=aNodes(rxActiveNodes(rxNode_cnt)).getLinkInfoByIndex(link_cnt);
// 	fout<<"\nFrom"+toString(rxID)+"to"+toString(tempLinkInfo.id)+"="<<tempLinkInfo.scmLargeScaleParams.getAsVector();
//       }
//       else
//       {
// 	for(int txActiveNode_cnt=0;txActiveNode_cnt<length(txActiveNodes);txActiveNode_cnt++)
// 	{
// 	  tempLinkInfo=aNodes(rxActiveNodes(rxNode_cnt)).getLinkInfo(txActiveNodes(txActiveNode_cnt));
// 	  fout<<"\nFrom"+toString(rxID)+"to"+toString(tempLinkInfo.id)+"="<<tempLinkInfo.scmLargeScaleParams.getAsVector();
// 	}
//       }
//       fout<<endl;
//   }
//   fout.close();
//   cout<<"SCM LargeScale Parameters printed into the file.."<<endl;
// }


bool McellSystem::isSysReady(){

  return(isSysReady(sysTerrain));
}

/*!\fn isSysReady--> this function checks for the all possibilities of readiness of the system
 */
bool McellSystem::isSysReady(Terrain &myTerrain){

  bool isReady=true;

  if(coverageArea.xLen==-1)
  {
	cout<<"[detl:]Warning! Coverage Area not set in the System for Random Drop. Try using setCoverageArea().";
	isReady=false;
  }
  if(myTerrain.getTerrain()=="_TERRAIN_UNINIT_")
  {
	cout<<"[detl:]Warning! Terrain not set in the System. Try using setTerrain().";
	isReady=false;
  }
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_ && cellISD==-1)
  {
	cout<<"[detl:]Warning! Inter Site Distance not set in the System. Try using setInterSiteDistanceInMeters().";
	isReady=false;
  }
//   if(carrierFreq==-1)
//   {
// 	cout<<"[detl:]Warning! carrierFreq not set in the System. Try using setCarrierFreq().";
// 	isReady=false;
//   }
//   if(carrierFreqPerServerNodeType.length()==0)
//   {
// 	cout<<"[detl:]Warning! carrierFreq not set in the System. Try using setCarrierFreq().";
// 	isReady=false;
//   }
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_ && minDist==-1)
  {
	cout<<"[detl:]Warning! Minimum Distance from Cell Center not set in the System. Try using setMinDistanceFromCellCenter().";
	isReady=false;
  }
  if(shadowType==_SHADOWTYPE_UNINIT_)
  {
	cout<<"[detl:]Warning! Shadow Type not set in the System. Try using setShadowType().";
	isReady=false;
  }
  if(aNodeTypes.length()==0)
  {
	cout<<"[detl:]Warning! Node Types are not set in the System. Try using nameNodeType().";
	isReady=false;
  }
  if(sum(nNodes)==0)
  {
	cout<<"[detl:]Warning! Node Counts are not set in the System. Try using setNodeCount().";
	isReady=false;
  }
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_ && sum(nodeDropMethods)==0)
  {
	cout<<"[detl:]Warning!  Drop method not set properly for Hexagonal Drop Node in the System. Try using setNodeDropMethod().";
	isReady=false;
  }
  for(int node_cnt=0;node_cnt<sum(nNodes);node_cnt++){
    
	if(!aNodes(node_cnt).isNodeReady((cellLayout==_CELLLAYOUT_HEXAGONAL_)))
	{
	  cout<<"[both:]Node "<<node_cnt<<" not fully configured."<<endl;
	  isReady=false;
	  break;
	}
  }
  return(isReady);
}
/*!\fn isLinkInfoUpToDate--> this function make sures that linkinfo is up to the date or not it checks for for the status of linkinfo and it update it
 * \param[out] linkinfostatus--> it returns the linkinfo status throught which the update will be done
 */
bool McellSystem::isLinkInfoUpToDate(){

  bool linkStatus=true;
  for(int node_cnt=0;node_cnt<sum(nNodes);node_cnt++)
  {
	if(aNodes(node_cnt).isActive() && aNodes(node_cnt).getLinkInfoUpdateStatus()==false)
	{
	  cout<<"[detl:]Warning. Link Info for Node "<<node_cnt<<" not up-to-date because of some modification in the node."<<endl;
	  linkStatus=false;
	}
	if(aNodes(node_cnt).isActive() && (aNodes(node_cnt).txAntenna.getParameterUpdateStatus()==false || aNodes(node_cnt).rxAntenna.getParameterUpdateStatus()==false))
	{
	  cout<<"[detl:]Warning. Link Info for Node "<<node_cnt<<" not up-to-date because of some modification in the node's Antenna."<<endl;
	  linkStatus=false;
	}
  }
  return(linkStatus);
}

void McellSystem::transmitData(int txNodeID,int txBeamID,int datasize, Array<cvec> &dataFromAllAntennas,double currentTimeInSec, int overwriteOrAdd){

  channelCloud.transmitData(txNodeID,txBeamID,datasize,dataFromAllAntennas,currentTimeInSec,overwriteOrAdd);
}

void McellSystem::initBeamParameters(ivec nodeIDs){
  
  if(currentTaskid==0)  
        cout<<"Initializing Beam Parameters."<<endl;
  for(int node_cnt=0;node_cnt<nodeIDs.length();node_cnt++)
  {
    int nodeIndx=nodeIDs(node_cnt);
    channelCloud.nodesInfo.initBeamParameters(nodeIDs(node_cnt),aNodes(nodeIndx).getCarrierFreqInHz(),aNodes(nodeIndx).txAntenna.antStructure,aNodes(nodeIndx).txAntenna.TXRUMap,aNodes(nodeIndx).txAntenna.antSpacing,aNodes(nodeIndx).txAntenna.azimuthBeamAngles,aNodes(nodeIndx).txAntenna.zenithBeamAngles,aNodes(nodeIndx).txAntenna.orientationInfo,aNodes(nodeIndx).getCategory());
  }
}

/*!\fn initSCMLink--> it make sures that scm link is initialized or not , if it is not initialized it will make it do
 *\param[in] txNodeID --> it will give the node Ids of all transmitters
 *\param[in] rxNodeID--> it gives the node Ids of all receivers
 */
void McellSystem::initSCMLink(ivec rxNodeIDs, ivec txNodeIDs, bool maintainReciprocity, bool reinitialize){
  
    //cout << "rxNodeIDs" << txNodeIDs << endl;
  if(channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_)
  {
    for(int rx_cnt=0;rx_cnt<rxNodeIDs.length();rx_cnt++)
    {
      for(int tx_cnt=0;tx_cnt<txNodeIDs.length();tx_cnt++)
      {
        #ifdef USING_MPI
        cout<<"Initializing SCM Links for nodePair: ("<<rxNodeIDs(rx_cnt)<<","<<txNodeIDs(tx_cnt)<<")"<<endl; 
        #endif
        
        if(!channelCloud.isSCMLinkInitialized(txNodeIDs(tx_cnt),rxNodeIDs(rx_cnt)) || reinitialize)
        {
            int scmLinkIndx = channelCloud.getScmLinkIndx(rxNodeIDs(rx_cnt),txNodeIDs(tx_cnt));
            if(scmLinkIndx==-1)
            {
                cout<<"Pre-Allocation of SCMLink is not done.."<<endl;abort();
            }
            
            int rxIndx = find(nodeIDs,rxNodeIDs(rx_cnt)),txIndx = find(nodeIDs,txNodeIDs(tx_cnt));
            channelCloud.nodesInfo.initBeamParameters(rxNodeIDs(rx_cnt),aNodes(rxIndx).getCarrierFreqInHz(),aNodes(rxIndx).rxAntenna.antStructure,aNodes(rxIndx).rxAntenna.TXRUMap,aNodes(rxIndx).rxAntenna.antSpacing,aNodes(rxIndx).rxAntenna.azimuthBeamAngles,aNodes(rxIndx).rxAntenna.zenithBeamAngles,aNodes(rxIndx).rxAntenna.orientationInfo,aNodes(rxIndx).getCategory());
            channelCloud.nodesInfo.initBeamParameters(txNodeIDs(tx_cnt),aNodes(txIndx).getCarrierFreqInHz(),aNodes(txIndx).txAntenna.antStructure,aNodes(txIndx).txAntenna.TXRUMap,aNodes(txIndx).txAntenna.antSpacing,aNodes(txIndx).txAntenna.azimuthBeamAngles,aNodes(txIndx).txAntenna.zenithBeamAngles,aNodes(txIndx).txAntenna.orientationInfo,aNodes(txIndx).getCategory());
            
            initSCMLink(rxNodeIDs(rx_cnt),txNodeIDs(tx_cnt),maintainReciprocity,reinitialize);
          
            SCMPropagationScenario propagationScenario = channelCloud.scmLinks(scmLinkIndx).propagationScenario;
            
            //cout << "propagationScenario v1 : " << propagationScenario << endl;
            if(is5GSCMScenario(propagationScenario)|| isNTNSCMScenario(propagationScenario))
            {
                sTXRUParameters rxTXRUParameters = channelCloud.nodesInfo.getBeamParameters(rxNodeIDs(rx_cnt)).TXRUparameters;
                sTXRUParameters txTXRUParameters = channelCloud.nodesInfo.getBeamParameters(txNodeIDs(tx_cnt)).TXRUparameters;  
                channelCloud.scmLinks(scmLinkIndx).generate5GChannelTapGainsForInitialBeams (&txTXRUParameters,&rxTXRUParameters);
            }
        }
      }
    }
    //abort(); // remove abort santhosh
  }
}

void McellSystem::initSCMLink(int rxNodeID, int txNodeID, bool maintainReciprocity, bool reinitialize)
{
    // Getting scmGridsForSpConsist from node and assigning in respective SCMLink
    int scmLinkIndx = channelCloud.getScmLinkIndx(rxNodeID,txNodeID);
    if(scmLinkIndx==-1)
    {
        channelCloud.preAllocateSCMLink(rxNodeID,txNodeID,maintainReciprocity);
        scmLinkIndx = channelCloud.getScmLinkIndx(rxNodeID,txNodeID);
    }
    
    int rxIndx = find(nodeIDs,rxNodeID);
    int txIndx = find(nodeIDs,txNodeID);
    
    if(!channelCloud.nodesInfo.isBeamParametersInitialised(rxNodeID))
            channelCloud.nodesInfo.initBeamParameters(rxNodeID,aNodes(rxIndx).getCarrierFreqInHz(),aNodes(rxIndx).rxAntenna.antStructure,aNodes(rxIndx).rxAntenna.TXRUMap,aNodes(rxIndx).rxAntenna.antSpacing,aNodes(rxIndx).rxAntenna.azimuthBeamAngles,aNodes(rxIndx).rxAntenna.zenithBeamAngles,aNodes(rxIndx).rxAntenna.orientationInfo,aNodes(rxIndx).getCategory());
    if(!channelCloud.nodesInfo.isBeamParametersInitialised(txNodeID))
            channelCloud.nodesInfo.initBeamParameters(txNodeID,aNodes(txIndx).getCarrierFreqInHz(),aNodes(txIndx).txAntenna.antStructure,aNodes(txIndx).txAntenna.TXRUMap,aNodes(txIndx).txAntenna.antSpacing,aNodes(txIndx).txAntenna.azimuthBeamAngles,aNodes(txIndx).txAntenna.zenithBeamAngles,aNodes(txIndx).txAntenna.orientationInfo,aNodes(txIndx).getCategory());

    Location_S txWrapLoc=getWrapAroundLocation(rxNodeID,txNodeID);
    
    NodeLinkInfo_S tempLinkInfo=aNodes(rxIndx).getLinkInfo(txNodeID);
    
    mat dcorrForSPConsistency = dcorrForSPConsistencyCal(); 
    Area_S coverageAreaLoc = sysTerrain.scmInfo.coverageArea; 
        
    channelCloud.scmLinks(scmLinkIndx).acquireSCMGridsForSpConsist(aNodes(txIndx).getSCMGridsForSpConsist(),dcorrForSPConsistency,coverageAreaLoc);

    bool isToInitialise = true;
    ivec rxGroupID=findGroup(aNodes(rxNodeID).getGroupID(),"SameLoc",true);//! Finding the group that having the servicenodes with the same loc(name) and getting thier IDs//
    ivec txGroupID=findGroup(aNodes(txNodeID).getGroupID(),"SameLoc",true);//! Finding the group that having the servernodes with the same loc(name) and getting thier IDs//
    
    if(txGroupID(0)!=-1)//! Transmitter grouping is done if it is the first node in the group ,set the properties as defined by linkto tx node//
    {	
        if(aNodeGroups(txGroupID(0)).nodeIDs.length())
        {
            int rxNodeIndx=find(channelCloud.nodesInfo.nodeList,rxNodeID);
            int txNodeIndx=find(channelCloud.nodesInfo.nodeList,txNodeID);
            int basetxNodeIndx = find(channelCloud.nodesInfo.nodeList,aNodeGroups(txGroupID(0)).nodeIDs(0));
            channelCloud.rndSeed(rxNodeIndx)(txNodeIndx)(0) = channelCloud.rndSeed(rxNodeIndx)(basetxNodeIndx)(0);
            
            channelCloud.scmLinks(scmLinkIndx).acquireSCMGridsForSpConsist(aNodes(aNodeGroups(txGroupID(0)).nodeIDs(0)).getSCMGridsForSpConsist(),dcorrForSPConsistency,coverageAreaLoc);
        }
    }

    if(isToInitialise) 
        channelCloud.initializeSCMLink(txNodeID,rxNodeID,aNodes(txIndx).getCarrierFreqInHz(),tempLinkInfo.scmLargeScaleParams,txWrapLoc,aNodes(rxIndx).getLoc(),aNodes(txIndx).txAntenna,aNodes(rxIndx).rxAntenna,tempLinkInfo.isLOS,maintainReciprocity,addPathSpecificSCMAntennaGain,reinitialize);

    channelCloud.scmLinks(scmLinkIndx).setDoppler(aNodes(rxIndx).getVelocityVector(),aNodes(txIndx).getVelocityVector());
   
//     if(txNodeID==0)
//     {
//         cout<<"ZoA : "<< channelCloud.scmLinks(scmLinkIndx).ntnSCMLink->clusterZOA<<endl;
//         cout<<"ZoD : "<< channelCloud.scmLinks(scmLinkIndx).ntnSCMLink->clusterZOD<<endl;
//         cout<<"AoA : "<< channelCloud.scmLinks(scmLinkIndx).ntnSCMLink->clusterAOA<<endl;
//         cout<<"AoD : "<< channelCloud.scmLinks(scmLinkIndx).ntnSCMLink->clusterAOD<<endl;
//     }
    
    if(!maintainReciprocity)
    {
        int revLinkIndx = channelCloud.getScmLinkIndx(txNodeID,rxNodeID);
        if(revLinkIndx==-1)
        {
            channelCloud.preAllocateSCMLink(txNodeID,rxNodeID,maintainReciprocity);
            revLinkIndx = channelCloud.getScmLinkIndx(txNodeID,rxNodeID);
        }
        channelCloud.scmLinks(revLinkIndx)=channelCloud.scmLinks(scmLinkIndx);
        channelCloud.scmLinks(revLinkIndx).scm5GLink->reInitChannelForFDDReciprocity(aNodes(rxIndx).getCarrierFreqInHz());
    }
}

void McellSystem::updateSCMLinkForMobility(int rxNodeID, ivec txNodeIDs, double deltaT)
{
    for(int txNode_cnt=0;txNode_cnt<txNodeIDs.length();txNode_cnt++)
    {
        updateSCMLinkForMobility(rxNodeID,txNodeIDs(txNode_cnt),deltaT);
    }
}

void McellSystem::updateSCMLinkForMobility(int rxNodeID, int txNodeID, double deltaT)
{
    // Getting scmGridsForSpConsist from node and assigning in respective SCMLink
    int scmLinkIndx = channelCloud.getScmLinkIndx(rxNodeID,txNodeID);
    if(scmLinkIndx==-1)
    {
        cout<<"SCM link uninitalized. "<<endl;abort();
    }
    
    int rxIndx = find(nodeIDs,rxNodeID);
    
    NodeLinkInfo_S tempLinkInfo=aNodes(rxIndx).getLinkInfo(txNodeID);
    double sigma_DS = pow(10.0,tempLinkInfo.scmLargeScaleParams.ds); 
    
    channelCloud.scmLinks(scmLinkIndx).scm5GLink->update5GChannelForMobility(tempLinkInfo.txLocation,tempLinkInfo.rxLocation,sigma_DS,deltaT);
    
}

ChannelOutput_S McellSystem::receiveData(int rxNodeID,int rxBeamID, int txNodeID, int ofdmSymbolIndx, double currentTimeInSeconds, double endTimeInSeconds, bool printChannel, bool computeSumPower, cmat txAntennaCorrelationMatrix, cmat rxAntennaCorrelationMatrix){

  if(channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_)
    if(!channelCloud.isSCMLinkInitialized(txNodeID,rxNodeID))
    {
        initSCMLink(rxNodeID,txNodeID,0);
    }

    return(channelCloud.receiveData(rxNodeID, rxBeamID, txNodeID,ofdmSymbolIndx, currentTimeInSeconds,endTimeInSeconds, printChannel,computeSumPower, txAntennaCorrelationMatrix, rxAntennaCorrelationMatrix));
}

ChannelOutput_S McellSystem::receiveData(int rxNodeID, int rxBeamID, AntennaCorrelationType_E antCorrelationType,int ofdmSymbolIndx, double currentTimeInSeconds, double endTimeInSeconds, bool printChannel, ivec txNodeList, ivec txNodesToSumPowerFrom){

  if(channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_)
  {
    ivec txNodes=channelCloud.getNodesInTxMode();
//     if(find(txNodes,rxNodeID)!=-1){cout<<"[both:] Given Rx node is in Tx mode in the Channel Cloud.."<<endl;abort();}
    int rxNodeIndx=find(channelCloud.nodesInfo.nodeList,rxNodeID);
    if(rxNodeIndx==-1)
    {
      cout<<"[both:]Unknown rxNodeID to receiveData()."<<endl;abort();
    }

    if(txNodeList.length()!=0)
    {
      if(find(find(txNodes,txNodeList),-1)!=-1)
	{ cout<<"[both:] Error.. Not all nodes in txNodeList is in Tx mode in the Channel Cloud.."<<endl;abort(); }

      txNodes=txNodeList;
    }
    for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
      if(!channelCloud.isSCMLinkInitialized(txNodes(tx_cnt),rxNodeID))
      {
        initSCMLink(txNodes(tx_cnt),rxNodeID,1);
      }
  }
  return(channelCloud.receiveData(rxNodeID,rxBeamID, antCorrelationType, ofdmSymbolIndx ,currentTimeInSeconds,endTimeInSeconds,printChannel, txNodeList,txNodesToSumPowerFrom));
}

ChannelOutput_S McellSystem::generateChannel(int rxNodeID, int rxBeamID, int txNodeID, double currentTimeInSeconds, int fftSize, cmat txAntennaCorrelationMatrix, cmat rxAntennaCorrelationMatrix){

  if(channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_)
  {
    if(!channelCloud.isSCMLinkInitialized(rxBeamID,rxNodeID))
    {
        initSCMLink(txNodeID,rxNodeID,1);
    }
  }
  return(channelCloud.generateChannel(rxNodeID, rxBeamID,txNodeID, currentTimeInSeconds,fftSize,txAntennaCorrelationMatrix, rxAntennaCorrelationMatrix));
}

ChannelOutput_S McellSystem::generateChannel(int rxNodeID, int rxBeamID, AntennaCorrelationType_E antCorrelationType, double currentTimeInSeconds,int fftSize, ivec txNodeList){

  if(channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_)
  {
    ivec txNodes;
//     if(find(txNodes,rxNodeID)!=-1){cout<<"[both:] Given Rx node is in Tx mode in the Channel Cloud.."<<endl;abort();}
    int rxNodeIndx=find(channelCloud.nodesInfo.nodeList,rxNodeID);
    if(rxNodeIndx==-1)
    {
      cout<<"[both:]Unknown rxNodeID to receiveData()."<<endl;abort();
    }

    txNodes=channelCloud.getNodesInTxMode();

    if(txNodeList.length()!=0)
    {
      if(find(find(txNodes,txNodeList),-1)!=-1)
      { cout<<"[both:] Error.. Not all nodes in txNodeList is in Tx mode in the Channel Cloud.."<<endl;abort(); }

      txNodes=txNodeList;
    }
    for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
      if(!channelCloud.isSCMLinkInitialized(txNodes(tx_cnt),rxNodeID))
      {
          initSCMLink(txNodes(tx_cnt),rxNodeID,1);
      }
  }
  return(channelCloud.generateChannel(rxNodeID,rxBeamID, antCorrelationType ,currentTimeInSeconds,fftSize, txNodeList));
}

ChannelMatrix_S McellSystem::generateChannel(int rxNodeID, int rxBeamID, int txNodeID, vec currentTimeInSeconds,int fftSize, cmat txAntennaCorrelationMatrix, cmat rxAntennaCorrelationMatrix){

  if(channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_)
  {
    if(!channelCloud.isSCMLinkInitialized(txNodeID,rxNodeID))
    {
        initSCMLink(txNodeID,rxNodeID,1);
    }
  }
  return(channelCloud.generateChannel(rxNodeID,rxBeamID, txNodeID, currentTimeInSeconds,fftSize,txAntennaCorrelationMatrix, rxAntennaCorrelationMatrix));
}

ChannelMatrix_S McellSystem::generateChannel(int rxNodeID, int rxBeamID, AntennaCorrelationType_E antCorrelationType, vec currentTimeInSeconds, int fftSize, ivec txNodeList){

  if(channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_)
  {
    ivec txNodes;
//     if(find(txNodes,rxNodeID)!=-1){cout<<"[both:] Given Rx node is in Tx mode in the Channel Cloud.."<<endl;abort();}
    int rxNodeIndx=find(channelCloud.nodesInfo.nodeList,rxNodeID);
    if(rxNodeIndx==-1)
    {
      cout<<"[both:]Unknown rxNodeID to receiveData()."<<endl;abort();
    }


    txNodes=channelCloud.getNodesInTxMode();
    if(txNodeList.length()!=0)
    {
      if(find(find(txNodes,txNodeList),-1)!=-1)
      { cout<<"[both:] Error.. Not all nodes in txNodeList is in Tx mode in the Channel Cloud.."<<endl;abort(); }

      txNodes=txNodeList;
    }
    for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
      if(!channelCloud.isSCMLinkInitialized(txNodes(tx_cnt),rxNodeID))
      {
          initSCMLink(txNodes(tx_cnt),rxNodeID,1);
      }
  }
  return(channelCloud.generateChannel(rxNodeID,rxBeamID, antCorrelationType ,currentTimeInSeconds,fftSize, txNodeList));
}


ChannelMatrix_S McellSystem::generateChannelForAnalogBeams(int rxNodeID, ivec txNodeList, ivec& nTxBeamsPerTxNode, int& nRxBeams, double currentTimeInSec, bool isChannelGenForRSRP){
  
  if(channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_)
  {
    ivec txNodes;
    int rxNodeIndx=find(channelCloud.nodesInfo.nodeList,rxNodeID);
    if(rxNodeIndx==-1)
    {
      cout<<"[both:]Unknown rxNodeID to receiveData()."<<endl;abort();
    }
    
    
    txNodes=channelCloud.getNodesInTxMode();
    if(txNodeList.length()!=0)
    {
      if(find(find(txNodes,txNodeList),-1)!=-1)
      { cout<<"[both:] Error.. Not all nodes in txNodeList is in Tx mode in the Channel Cloud.."<<endl;abort(); }
      
      txNodes=txNodeList;
    }
    for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
      if(!channelCloud.isSCMLinkInitialized(txNodes(tx_cnt),rxNodeID))
      {
          initSCMLink(txNodes(tx_cnt),rxNodeID,1);
      }
  }
  else
  {
    cout<<"[both:] Error.. Supported only for SCM Channel .."<<endl;abort();
  }
  return(channelCloud.generateChannelForAnalogBeams(rxNodeID,txNodeList,nTxBeamsPerTxNode,nRxBeams,currentTimeInSec,isChannelGenForRSRP));
}



// ChannelMatrix_S McellSystem::generateChannel(int rxNodeID, int txNodeID, vec currentTimeInSeconds,int fftSize, cmat txAntennaCorrelationMatrix, cmat rxAntennaCorrelationMatrix){
// 
//   if(channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_)
//   {
//     if(!channelCloud.isSCMLinkInitialized(txNodeID,rxNodeID))
//     {
//       int txIndx=find(nodeIDs,txNodeID);
//       int rxIndx=find(nodeIDs,rxNodeID);
//       NodeLinkInfo_S tempLinkInfo=aNodes(rxIndx).getLinkInfo(txNodeID);
//       channelCloud.initializeSCMLink(txNodeID,rxNodeID,aNodes(rxIndx).getCarrierFreqInHz(),tempLinkInfo.scmLargeScaleParams,aNodes(txIndx).getLoc(),aNodes(rxIndx).getLoc(),aNodes(txIndx).txAntenna,aNodes(rxIndx).rxAntenna,tempLinkInfo.isLOS,addPathSpecificSCMAntennaGain);
//     }
//   }
//     return(channelCloud.generateChannel(rxNodeID, txNodeID, currentTimeInSeconds,fftSize,txAntennaCorrelationMatrix, rxAntennaCorrelationMatrix));
// }

// ChannelMatrix_S McellSystem::generateChannel(int rxNodeID, AntennaCorrelationType_E antCorrelationType, vec currentTimeInSeconds,int fftSize, ivec txNodeList){
// 
//   if(channelCloud.getChannelGenerationMethod()==_CHANNEL_GENERATION_METHOD_SCM_)
//   {
//     ivec txNodes;
//     if(find(txNodes,rxNodeID)!=-1){cout<<"[both:] Given Rx node is in Tx mode in the Channel Cloud.."<<endl;abort();}
//     int rxNodeIndx=find(channelCloud.nodesInfo.nodeList,rxNodeID);
//     if(rxNodeIndx==-1)
//     {
//       cout<<"[both:]Unknown rxNodeID to receiveData()."<<endl;abort();
//     }
// 
// 
//     txNodes=channelCloud.getNodesInTxMode();
//      if(txNodeList.length()!=0);
//     {
//       if(find(find(txNodes,txNodeList),-1)!=-1)
//       { cout<<"[both:] Error.. Not all nodes in txNodeList is in Tx mode in the Channel Cloud.."<<endl;abort(); }
// 
//       txNodes=txNodeList;
//     }
//     for(int tx_cnt=0;tx_cnt<txNodes.length();tx_cnt++)
//       if(!channelCloud.isSCMLinkInitialized(txNodes(tx_cnt),rxNodeID))
//       {
// 	int txIndx=find(nodeIDs,txNodes(tx_cnt));
// 	int rxIndx=find(nodeIDs,rxNodeID);
// 	NodeLinkInfo_S tempLinkInfo=aNodes(rxIndx).getLinkInfo(txNodes(tx_cnt));
// 	channelCloud.initializeSCMLink(txNodes(tx_cnt),rxNodeID,aNodes(rxIndx).getCarrierFreqInHz(),tempLinkInfo.scmLargeScaleParams,aNodes(txIndx).getLoc(),aNodes(rxIndx).getLoc(),aNodes(txIndx).txAntenna,aNodes(rxIndx).rxAntenna,tempLinkInfo.isLOS,addPathSpecificSCMAntennaGain);
//       }
//   }
//   return(channelCloud.generateChannel(rxNodeID, antCorrelationType ,currentTimeInSeconds,fftSize, txNodeList));
// }

Location_S McellSystem::getWrapAroundLocation(int baseNodeID, int nodeID){

  if(cellLayout!=_CELLLAYOUT_HEXAGONAL_)
    return(aNodes(nodeID).getLoc());
  else
  {
    // Assuming cellID is same as )th sector nodeID of the hexagonal Cell
    int baseCellID=aNodes(baseNodeID).getCellID();
    int nodeCellID=aNodes(nodeID).getCellID();
    if(baseCellID!=nodeCellID)
      return(WrapMyLocation(aNodes(baseCellID*nSectorsPerSite).getNeighbourCellLoc(nodeCellID),aNodes(nodeCellID*nSectorsPerSite).getLoc(),aNodes(nodeID).getLoc()));
    else
      return(aNodes(nodeID).getLoc());
  }
}

Location_S McellSystem::getWrapAroundLocation(int baseNodeID, Location_S &location ,int cellIDOfLocation){
  
  if(cellLayout!=_CELLLAYOUT_HEXAGONAL_)
    return(location);
  else
  {
    // Assuming cellID is same as )th sector nodeID of the hexagonal Cell
    int baseCellID=aNodes(baseNodeID).getCellID();
    int nodeCellID=cellIDOfLocation;
    if(baseCellID!=nodeCellID)
      return(WrapMyLocation(aNodes(baseCellID*nSectorsPerSite).getNeighbourCellLoc(nodeCellID),aNodes(nodeCellID*nSectorsPerSite).getLoc(),location));
    else
      return(location);
  }
}

void McellSystem::printBuildingInfo(string filename) {
  
  std::ofstream fout(filename.c_str(), std::ios::out);
  fout << "ID\t\tx\t\ty\t\tBuilding Side\t\tRoom Side\t\tRoom Height\t\tNo. of floors"<<endl;
  for(int i=0; i < buildingInfo.buildingLocations.length(); i++)
    fout << i << buildingInfo.buildingLocations(i).x << "\t\t" << buildingInfo.buildingLocations(i).y << "\t\t" << buildingInfo.roomSize.length << "\t\t" << buildingInfo.roomSize.height << buildingInfo.floorPerbuilding(i) << endl;
}

int McellSystem::getBuildingCount() {
  
  return(buildingInfo.buildingLocations.length()); 
}

void McellSystem::generateCoveragePlan() {
  
  int maxHeight=buildingInfo.maximumFloors*buildingInfo.roomSize.height;
  if(max(nodeTypeHeights)>(double)maxHeight) maxHeight=ceil_i(max(nodeTypeHeights));
  maxHeight++;
  sysTerrain.coverageFootprint=zeros_i(ceil_i(coverageArea.yLen),ceil_i(coverageArea.xLen));

  if(buildingInfo.enableWallCountForPathloss)
  {
    sysTerrain.coveragePlan.set_size(maxHeight);
    for(int i=0; i<maxHeight; i++)
    {
      sysTerrain.coveragePlan(i).set_size(ceil_i(coverageArea.yLen),ceil_i(coverageArea.xLen));
      sysTerrain.coveragePlan(i).clear();
    }
  }
  
  int buildingRowStart,buildingColStart;
  for(int b_cnt=0; b_cnt<buildingInfo.buildingLocations.length(); b_cnt++)
  {
    Location_S buildingLoc=buildingInfo.buildingLocations(b_cnt);
    buildingRowStart=round_i(coverageArea.yLen/2-(buildingLoc.y+buildingInfo.buildingArea.yLen/2));
    buildingColStart=round_i(coverageArea.xLen/2+(buildingLoc.x-buildingInfo.buildingArea.xLen/2));
    sysTerrain.coverageFootprint.set_submatrix(buildingRowStart,buildingColStart,ones_i(buildingInfo.buildingArea.yLen,buildingInfo.buildingArea.xLen)*(b_cnt+1));
    
    if(buildingInfo.enableWallCountForPathloss)
      for(int cnt=0;cnt<buildingInfo.floorPerbuilding(b_cnt)*buildingInfo.roomSize.height;cnt++)
	sysTerrain.coveragePlan(cnt).set_submatrix(buildingRowStart,buildingColStart,buildingInfo.floor3DModel(cnt));
    
  }
}

ivec McellSystem :: findIndoorNodes(ivec &buildingIndx, string nodeType) {
  
//   double cov_xlen = getCoverageArea().xLen;
//   double cov_ylen = getCoverageArea().yLen;
  
  ivec indoorNodes(0);
  buildingIndx.set_length(0);
  
    if(nodeType=="")
      for(int node_cnt=0; node_cnt < sum(nNodes); node_cnt++)
      {
	int rowIndx = round_i(coverageArea.yLen/2-aNodes(node_cnt).getLoc().y);
	int colIndx = round_i(coverageArea.xLen/2+aNodes(node_cnt).getLoc().x);
	if(sysTerrain.coverageFootprint(rowIndx,colIndx) != 0)
	{
	  append(indoorNodes,node_cnt);
	  append(buildingIndx,sysTerrain.coverageFootprint(rowIndx,colIndx)-1);
	}
      }
    else
    {
      ivec nodeList=getNodes(nodeType);
      for(int node_cnt=0; node_cnt < nodeList.length(); node_cnt++)
      {
	int rowIndx = round_i(coverageArea.yLen/2-aNodes(nodeList(node_cnt)).getLoc().y);
	int colIndx = round_i(coverageArea.xLen/2+aNodes(nodeList(node_cnt)).getLoc().x);
	if(sysTerrain.coverageFootprint(rowIndx,colIndx) != 0)
	{
	  cout<<"Node Loc : "<<rowIndx<<","<<colIndx<<endl;
	  append(indoorNodes,nodeList(node_cnt));
	  append(buildingIndx,sysTerrain.coverageFootprint(rowIndx,colIndx)-1);
	}
      }
    }
    return(indoorNodes);
}

void McellSystem::initIndoorInfo(string configFile, bool createbuildings){
  
  //Building Creation
  buildingInfo.init(configFile,createbuildings);
#ifdef USING_MPI
  if(currentTaskid==0)
#endif    
  cout<<"[detl:]Loading Indoor info..."<<endl;
  sysTerrain.setRoomSize(buildingInfo.roomSize.length,buildingInfo.roomSize.width,buildingInfo.roomSize.height);
  sysTerrain.setWallSpacing(buildingInfo.indoorWallSpacing);
  sysTerrain.setMaximumFloors(buildingInfo.maximumFloors);
  sysTerrain.setWallPenetrationLoss(buildingInfo.thickWallLossInDB,buildingInfo.thinWallLossInDB);
  if(createbuildings && buildingInfo.nBuildings)
  {
    cout<<"[detl:]Generating building locations."<<endl;
    dropBuildings();
  }
  
}

void McellSystem::dropBuildings()
{
  
  Array<Location_S> buildingLocations(0);  
  
  if(cellLayout==_CELLLAYOUT_HEXAGONAL_)
  {
    int hexDropNodeType=find(nodeDropMethods,_DROPMETHOD_HEXAGONAL_);
    int hexNodeCount=getNodeCount(aNodeTypes(hexDropNodeType));
    ivec hexNodes=getNodes(hexDropNodeType);
    double buildingMinDistance=buildingInfo.floor3DModel(0).rows();
    if(buildingInfo.floor3DModel(0).cols()>buildingInfo.floor3DModel(0).rows())buildingMinDistance=buildingInfo.floor3DModel(0).cols();
    if(sysTerrain.getCellISD()/2<buildingMinDistance)
    {
      cout<<"[both:]Error. Building dimension too big to fit inside the Hexagonal cell."<<endl;abort();
    }
    if(buildingInfo.isBuildingsPlanned)
    {
      if(buildingInfo.isPlannedBuildingLocationsforASector)
      {
	if(buildingInfo.plannedLocations.length()/2!=buildingInfo.nBuildings/hexNodeCount)
	  {cout<<"[both:] Insufficient building location values."<<endl;abort();}
	
	Array<Location_S> locationsPerCell(0);
	for(int sec_cnt=0;sec_cnt<nSectorsPerSite;sec_cnt++)
	  append(locationsPerCell,getRotatedLocations(buildingInfo.plannedLocations,sec_cnt*360.0/nSectorsPerSite));
	ivec hexNodes=getNodes(hexDropNodeType);
	for(int cell_cnt=0;cell_cnt<(hexNodeCount/nSectorsPerSite);cell_cnt++)
	  append(buildingLocations,getShiftedLocations(locationsPerCell,aNodes(cell_cnt*nSectorsPerSite).getLoc()));
	
	//Checking for overlap
	Location_S temp,buildLoc;
	for(int b_cnt=0;b_cnt<buildingInfo.nBuildings;b_cnt++)
	  for(int cnt=0;cnt<b_cnt;cnt++)
	  {
	    temp=getWrapAroundLocation(hexNodes(b_cnt/buildingInfo.plannedLocations.length()),buildingLocations(cnt),aNodes(hexNodes(cnt/buildingInfo.plannedLocations.length())).getCellID());
	    if(findDistance(temp,buildLoc)<buildingMinDistance)
	      {cout<<"[both:] Building overlap for planned building locations."<<endl;abort();}
	  }
	
      }
      else
	buildingLocations=buildingInfo.plannedLocations;
    }
    else
    {
      if(buildingInfo.nBuildings<hexNodeCount)
      {
	ivec chosenHexNodes=randPick(hexNodes,buildingInfo.nBuildings);// Randomly picking cells
	Location_S temp,buildLoc;
	for(int b_cnt=0;b_cnt<buildingInfo.nBuildings;b_cnt++)
	{
	  
	  bool proximityFlag=false;
	  do
	  {
	    proximityFlag=false;
	    
	    
	    bool validLoc=false;
	    do
	    {
	      validLoc=true;
	      buildLoc=generateUniformDropLocationInHexCell(aNodes(chosenHexNodes(b_cnt)).getLoc(),cellISD/sqrt(3),minDist+buildingMinDistance/2)(0);
	      	      
	      //Checking if the corner points of the building are inside the hexagon -10010
	      if(isInsideHexCell(buildLoc.x-buildingInfo.buildingArea.xLen/2,buildLoc.y-buildingInfo.buildingArea.yLen/2,aNodes(chosenHexNodes(b_cnt)).getLoc(),cellISD,minDist)==false) validLoc=false;
	      if(isInsideHexCell(buildLoc.x-buildingInfo.buildingArea.xLen/2,buildLoc.y+buildingInfo.buildingArea.yLen/2,aNodes(chosenHexNodes(b_cnt)).getLoc(),cellISD,minDist)==false) validLoc=false;
	      if(isInsideHexCell(buildLoc.x+buildingInfo.buildingArea.xLen/2,buildLoc.y-buildingInfo.buildingArea.yLen/2,aNodes(chosenHexNodes(b_cnt)).getLoc(),cellISD,minDist)==false) validLoc=false;
	      if(isInsideHexCell(buildLoc.x+buildingInfo.buildingArea.xLen/2,buildLoc.y+buildingInfo.buildingArea.yLen/2,aNodes(chosenHexNodes(b_cnt)).getLoc(),cellISD,minDist)==false) validLoc=false;
	      
	    }while(validLoc==false);
	    
	    for(int cnt=0;cnt<b_cnt;cnt++)
	    {
	      temp=getWrapAroundLocation(chosenHexNodes(b_cnt),buildingLocations(cnt),aNodes(chosenHexNodes(cnt)).getCellID());
	      if(findDistance(temp,buildLoc)<buildingMinDistance)
	      {
		proximityFlag=true;
		break;
	      }
	    }
	  }while(proximityFlag==true);
	  
	  append(buildingLocations,buildLoc);
	}
      }
      else
      {
	Location_S temp,buildLoc;
	for(int b_cnt=0;b_cnt<buildingInfo.nBuildings;b_cnt++)
	{
	  bool proximityFlag=false;
	  int count=0;
	  do
	  {
	    proximityFlag=false;
	    bool validLoc=false;
	    int count1=0;
	    do{
	      validLoc=true;
	      buildLoc=generateUniformDropLocationInHexCell(aNodes(hexNodes(b_cnt%hexNodeCount)).getLoc(),cellISD/sqrt(3),minDist+buildingMinDistance/2)(0);
	      
	      //Checking if the corner points of the building are inside the hexagon -10010
	      if(isInsideHexCell(buildLoc.x-buildingInfo.buildingArea.xLen/2,buildLoc.y-buildingInfo.buildingArea.yLen/2,aNodes(hexNodes(b_cnt%hexNodeCount)).getLoc(),cellISD,minDist)==false) validLoc=false;
	      if(isInsideHexCell(buildLoc.x-buildingInfo.buildingArea.xLen/2,buildLoc.y+buildingInfo.buildingArea.yLen/2,aNodes(hexNodes(b_cnt%hexNodeCount)).getLoc(),cellISD,minDist)==false) validLoc=false;
	      if(isInsideHexCell(buildLoc.x+buildingInfo.buildingArea.xLen/2,buildLoc.y-buildingInfo.buildingArea.yLen/2,aNodes(hexNodes(b_cnt%hexNodeCount)).getLoc(),cellISD,minDist)==false) validLoc=false;
	      if(isInsideHexCell(buildLoc.x+buildingInfo.buildingArea.xLen/2,buildLoc.y+buildingInfo.buildingArea.yLen/2,aNodes(hexNodes(b_cnt%hexNodeCount)).getLoc(),cellISD,minDist)==false) validLoc=false;
	      count1++;
	      if(count1>100)
	      {
		cout<<"Too many attempts."<<endl;abort();
	      }
	    }while(validLoc==false);
	    for(int cnt=0;cnt<b_cnt;cnt++)
	    {
	      temp=getWrapAroundLocation(hexNodes(b_cnt%hexNodeCount),buildingLocations(cnt),aNodes(hexNodes(cnt%hexNodeCount)).getCellID());
	      if(findDistance(temp,buildLoc)<buildingMinDistance)
	      {
		proximityFlag=true;
		break;
	      }
	    }
	    count++;
	    if(count>100)
	    {
	      cout<<"Too many attempts."<<endl;abort();
	    }
	  }while(proximityFlag==true);
	  
	  append(buildingLocations,buildLoc);
	} 
      }
    
    }
  }
  else
  {
    if(buildingInfo.isBuildingsPlanned)
    {
      if(buildingInfo.isPlannedBuildingLocationsforASector)
	{ cout<<"[both:]Error.isPlannedBuildingLocationsforASector cant' be enabled in Random Layout."<<endl;abort();}
      else
	buildingLocations=buildingInfo.plannedLocations;
    }
    else
      buildingLocations=generateUniformDropLocation(coverageArea,buildingInfo.nBuildings);
    
  }
  buildingInfo.buildingLocations=buildingLocations;
  if(buildingInfo.isFixedFloors)
    buildingInfo.floorPerbuilding=ones_i(buildingLocations.length())*sysTerrain.getMaximumFloors();
  else
    buildingInfo.floorPerbuilding=randi(buildingLocations.length(),1,sysTerrain.getMaximumFloors());
  
   generateCoveragePlan();
}

void McellSystem::dropInBuildingNodes(string nodeType){
  
  // Collecting Nodes
  ivec inbuildingNodeTypes;
  if(nodeType=="")
    inbuildingNodeTypes=find(nodeDropMethods,_DROPMETHOD_INBUILDING_,true);
  else
  {
    if(findInStringArray(aNodeTypes,nodeType)!=-1)
      inbuildingNodeTypes=to_ivec(findInStringArray(aNodeTypes,nodeType));
    else
      {cout<<"[both:]Unknown nodeType to dropInBuildingNodes()."<<endl;abort();}
  }
  
  if(inbuildingNodeTypes.length())
    cout<<"[detl:]Dropping In-Building nodes.."<<endl;
  
  for(int nodeType_cnt=0;nodeType_cnt<inbuildingNodeTypes.length();nodeType_cnt++)
  {
    ivec nodes=getNodes(inbuildingNodeTypes(nodeType_cnt));
    if(aNodes(nodes(0)).getLoc().z>buildingInfo.roomSize.height)
    {
      cout<<"[both:]Error. Node height greater than floor height for Inbuilding node."<<endl;
      abort();
    }
    nodes=shuffleVec(nodes);
    Area_S buildingArea;
    buildingArea.xLen=buildingInfo.floor3DModel(0).cols();
    buildingArea.yLen=buildingInfo.floor3DModel(0).rows();
    Array<Location_S> inbuildingLocations=generateUniformDropLocation(buildingArea,nodes.length());
    
    int totalFloors=sum(buildingInfo.floorPerbuilding);
    if(nodes.length()<totalFloors) // Less nodes than total floors 
    {
      ivec rndBuildingIndx=randi(nodes.length(),0,(buildingInfo.nBuildings-1));// Randomly distributing nodes
      for(int node_cnt=0;node_cnt<nodes.length();node_cnt++)
      {
	Location_S location;
	location.x=inbuildingLocations(node_cnt).x+buildingInfo.buildingLocations(rndBuildingIndx(node_cnt)).x;
	location.y=inbuildingLocations(node_cnt).y+buildingInfo.buildingLocations(rndBuildingIndx(node_cnt)).y;
	location.floorNumber=randi(0,buildingInfo.floorPerbuilding(rndBuildingIndx(node_cnt))-1); // Randomly picking a floor
	location.z=location.floorNumber*buildingInfo.roomSize.height+aNodes(nodes(node_cnt)).getLoc().z;
	location.isIndoor=true;
	location.isInBuilding=true;
	aNodes(nodes(node_cnt)).setLoc(location,true);
	aNodes(nodes(node_cnt)).setHexagonalCellID(findHexagonalCellID(nodes(node_cnt)));
      }
    }
    else // More nodes than total floors
    {
      int nodesPerFloor=nodes.length()/totalFloors;
      int restOfNodes=nodes.length()-nodesPerFloor*totalFloors;
      int buildingIndx=0;
      int totalNode_cnt=0;
      for(int floor_cnt=0, buildingFloor_cnt=0;floor_cnt<totalFloors;floor_cnt++,buildingFloor_cnt++)
      {
	if(buildingFloor_cnt>buildingInfo.floorPerbuilding(buildingIndx))
	{
	  buildingFloor_cnt=0;
	  buildingIndx++;
	}
	for(int node_cnt=0;node_cnt<nodesPerFloor;node_cnt++,totalNode_cnt++)
	{
	  Location_S location;
	  location.x=inbuildingLocations(totalNode_cnt).x+buildingInfo.buildingLocations(buildingIndx).x;
	  location.y=inbuildingLocations(totalNode_cnt).y+buildingInfo.buildingLocations(buildingIndx).y;
	  location.floorNumber=buildingFloor_cnt;
	  location.z=buildingFloor_cnt*buildingInfo.roomSize.height+aNodes(nodes(totalNode_cnt)).getLoc().z;
	  location.isIndoor=true;
	  aNodes(nodes(totalNode_cnt)).setLoc(location,true);
	  aNodes(nodes(totalNode_cnt)).setHexagonalCellID(findHexagonalCellID(nodes(totalNode_cnt)));
	}
	
      }
      ivec rndBuildingIndx=randi(restOfNodes,0,(buildingInfo.nBuildings-1));// Randomly distributing remaining nodes
      for(int node_cnt=0;node_cnt<restOfNodes;node_cnt++,totalNode_cnt++)
      {
	Location_S location;
	location.x=inbuildingLocations(totalNode_cnt).x+buildingInfo.buildingLocations(rndBuildingIndx(node_cnt)).x;
	location.y=inbuildingLocations(totalNode_cnt).y+buildingInfo.buildingLocations(rndBuildingIndx(node_cnt)).y;
	location.floorNumber=randi(0,buildingInfo.floorPerbuilding(rndBuildingIndx(node_cnt))-1);// Randomly picking a floor
	location.z=location.floorNumber*buildingInfo.roomSize.height+aNodes(nodes(totalNode_cnt)).getLoc().z;
	location.isIndoor=true;
	aNodes(nodes(totalNode_cnt)).setLoc(location,true);
	aNodes(nodes(totalNode_cnt)).setHexagonalCellID(findHexagonalCellID(nodes(totalNode_cnt)));
      }
    }
    
  }
}

void McellSystem::setNonInBuildingIndoorNodes(){
  
  ivec nonInbuildingNodeTypes=findNot(nodeDropMethods,_DROPMETHOD_INBUILDING_);
  for(int nodeType_cnt=0;nodeType_cnt<nonInbuildingNodeTypes.length();nodeType_cnt++)
  {
    ivec buildingIndx;
    ivec indoorNodes=findIndoorNodes(buildingIndx, aNodeTypes(nonInbuildingNodeTypes(nodeType_cnt)));
    if(indoorNodes.length()!=0)
    {
      if(nodeDropMethods(nonInbuildingNodeTypes(nodeType_cnt))==_DROPMETHOD_HEXAGONAL_)
      {
	cout<<"[both:]Error. Hexagonal Nodes can't be set to Indoor. Invalid building locations. "<<endl;
	abort();
      }
      
      if(aNodes(indoorNodes(0)).getLoc().z>buildingInfo.roomSize.height)
      {
	cout<<"aNodes(indoorNodes(0)).getLoc().z:	"<<aNodes(indoorNodes(0)).getLoc().z<<endl;
	cout<<"[both:]Error. Node height greater than floor height for Inbuilding node."<<endl;
	abort();
      }
      
      if(indoorNodePercentage(nonInbuildingNodeTypes(nodeType_cnt))!=0)
      {
	setNodeTypeAsOutdoor(aNodeTypes(nonInbuildingNodeTypes(nodeType_cnt)));
	double diffPercentage=indoorNodePercentage(nonInbuildingNodeTypes(nodeType_cnt))-(indoorNodes.length()*100.0)/(double)nNodes(nonInbuildingNodeTypes(nodeType_cnt));
	if(diffPercentage>0)
	{
	  cout<<"[detl:]Adjusting indoor percentage of node type : "<<aNodeTypes(nonInbuildingNodeTypes(nodeType_cnt))<<" to "<<diffPercentage<<endl;
	  ivec allNodes=getNodes(nonInbuildingNodeTypes(nodeType_cnt));
	  ivec outBuildingNodes=remove(allNodes,indoorNodes);
	  setNodeListAsIndoor(outBuildingNodes,diffPercentage);
	}
      }
      
      
      int rndFloor;
      for(int node_cnt=0;node_cnt<indoorNodes.length();node_cnt++)
      {
	rndFloor=randi(0,buildingInfo.floorPerbuilding(buildingIndx(node_cnt))-1);
	Location_S loc=aNodes(indoorNodes(node_cnt)).getLoc();
	loc.floorNumber=rndFloor;
	loc.isIndoor=true;
	loc.isInBuilding=true;
	loc.z=loc.z+rndFloor*buildingInfo.roomSize.height;
	int hexID=aNodes(indoorNodes(node_cnt)).getCellID();
	aNodes(indoorNodes(node_cnt)).setLoc(loc,true); //This func sets hexCellID to -1
	aNodes(indoorNodes(node_cnt)).setHexagonalCellID(hexID);
      }
    }
  }
}

void McellSystem::loadCarrierAssociationInfo(ivec srvcNodes, ivec associatedServerNodes){
  
  carrierAssociationInfo.serviceNodes=srvcNodes;
  carrierAssociationInfo.associatedServerNodesForEachServiceNode=associatedServerNodes;
  for(int srvc_cnt=0;srvc_cnt<carrierAssociationInfo.serviceNodes.length();srvc_cnt++)
    aNodes(carrierAssociationInfo.serviceNodes(srvc_cnt)).setCarrierFreqInHz(aNodes(carrierAssociationInfo.associatedServerNodesForEachServiceNode(srvc_cnt)).getCarrierFreqInHz());
  
}

void McellSystem::updateCarrierAssociationInfo(int srvcNode, int associatedServerNode){
  
  int indx=find(carrierAssociationInfo.serviceNodes,srvcNode);
  if(indx!=-1)
  {
    if(find(nodeIDs,associatedServerNode)!=-1)
    {
      carrierAssociationInfo.associatedServerNodesForEachServiceNode(indx)=associatedServerNode;
      aNodes(srvcNode).setCarrierFreqInHz(aNodes(associatedServerNode).getCarrierFreqInHz());
    }
    else
    {
      cout<<"[detl:]Warning. Unknown server node to updateCarrierAssociationInfo()."<<endl;
    }
  }
  else
  {
    cout<<"[detl:]Warning. Unknown service node to updateCarrierAssociationInfo()."<<endl;
  }
  
}

bool McellSystem::isInCoverage(Location_S location)
{
    if(abs(location.x)<=coverageArea.xLen/2 && abs(location.y)<=coverageArea.yLen/2)
        return true;
    else
        return false;
}

void McellSystem::printSCMparams(ivec serviceNodes,ivec serverNodes,string filename)
{
    filename = filename +".temp"+toString(currentTaskid);
    ofstream os(filename.c_str(),std::ios::out);
    if(currentTaskid==0)
        os<<"%srvcNode,\tassociatedSrvrNode,\tDS (ns),\tASD,\tZSD,\tASA,\tZSA,\tSF,\tK_rice \t NOTE: All the angles are in degree"<<endl;
    double UE_sigma_AOD,UE_sigma_AOA,UE_sigma_DS,UE_sigma_ZOA,UE_sigma_ZOD;
    for(int srvc_cnt=0;srvc_cnt<serviceNodes.length();srvc_cnt++)
    {
        int srvcIndx = find(nodeIDs,serviceNodes(srvc_cnt));
      //  for(int srvr_cnt=0;srvr_cnt<serverNodes.length();srvr_cnt++)
        {
            NodeLinkInfo_S linkInfo = aNodes(srvcIndx).getLinkInfo(serverNodes(srvc_cnt));
            SCMLargeScaleParameters_S channel5GParameters = linkInfo.scmLargeScaleParams;
            
            UE_sigma_DS = pow(10.0,channel5GParameters.ds); 
            UE_sigma_AOD = pow(10.0,channel5GParameters.asd); 
            UE_sigma_AOA = pow(10.0,channel5GParameters.asa);  
            UE_sigma_ZOA = pow(10.0,channel5GParameters.zsa); 
            UE_sigma_ZOD = pow(10.0,channel5GParameters.zsd);
            
            if (UE_sigma_AOA >104.0)
                UE_sigma_AOA = 104.0;
            if (UE_sigma_AOD >104.0)
                UE_sigma_AOD = 104.0;
            if (UE_sigma_ZOA >52.0)
                UE_sigma_ZOA = 52.0;
            if (UE_sigma_ZOD >52.0)
                UE_sigma_ZOD = 52.0;
            
            os<<serviceNodes(srvc_cnt)<<","<<serverNodes(srvc_cnt)<<","<<UE_sigma_DS*1e9<<","<<UE_sigma_AOD<<","<<UE_sigma_ZOD<<"\t";
            os<<UE_sigma_AOA<<","<<UE_sigma_ZOA<<"\t"<<channel5GParameters.sf<<","<<inv_dB(channel5GParameters.k)<<endl;
        }
    }
    os.close();
}

vec McellSystem::getAntennaTemperature(ivec nodeIDs)
{
    vec antTemp(nodeIDs.length());
    for(int cnt=0;cnt<nodeIDs.length();cnt++)
    {
        antTemp(cnt) = aNodes(nodeIDs(cnt)).txAntenna.getTemperature();
    }
    return antTemp;
}

vec McellSystem::getNoiseFigureInDB(ivec nodeIDs)
{
    vec antTemp(nodeIDs.length());
    for(int cnt=0;cnt<nodeIDs.length();cnt++)
    {
        antTemp(cnt) = noiseFigurePerNodeType(aNodes(nodeIDs(cnt)).getType());
    }
    return antTemp;
}

vec McellSystem::getGTRatioInDB(ivec nodeIDs)
{
    vec gtTemp(nodeIDs.length());
    for(int cnt=0;cnt<nodeIDs.length();cnt++)
    {
        gtTemp(cnt) = aNodes(nodeIDs(cnt)).txAntenna.getGTRatioInDB();
    }
    return gtTemp;
}

ivec McellSystem::getNodesWithFrequencyID(int satID,int freqID,NodeCategory_E nodeCategory)
{
    ivec nodes(0);
    for(int cnt=0;cnt<aNodes.length();cnt++)
    {
        if(aNodes(cnt).getFrequencyID()==freqID && aNodes(cnt).getCategory()==nodeCategory && aNodes(cnt).getSatelliteID()==satID)
        append(nodes,cnt);
    }
    return nodes;
}


void printAntennaGain(double antennaGain,Location_S loc)
{
  
  std::ofstream logs;
  static bool isNewPrint=1;
  string antlogFilename = "./AntLog";
  if(isNewPrint)
  {
    logs.open(antlogFilename.c_str(),ios::out);
    
    if(currentTaskid==0)
      logs<<"%\t xLOc \t yLoc \t zLoc \t ANtenna Log\n";
    isNewPrint=0;
  }
  else
    logs.open(antlogFilename.c_str(),ios::app);

  logs<<loc.x<<","<<loc.y<<","<<loc.z<<","<<antennaGain<<",\n";
}







