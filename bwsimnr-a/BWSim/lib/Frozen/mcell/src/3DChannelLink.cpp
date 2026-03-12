// /*************************************************************************
// *
// * CEWiT CONFIDENTIAL
// * __________________
// *
// * All Rights Reserved © 2012 CEWiT, India
// *
// * NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
// * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
// * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
// * express, printed and signed license for use is strictly forbidden.
// */
// 
// #include "../include/SCMLink.h"
// 
// SCM3DLink::SCM3DLink(){
//   
//   isReadyFor3DChannel=false;
//   isLOS3DChannel=-1;
//   isOutdoorToIndoor3DChannel=-1;
//   nClusters3DChannel=-1;
//   tapPow3DChannel.set_length(0);
//   tapDelays3DChannel.set_length(0);
//   pathAoD3DChannel.set_length(0);
//   pathAoA3DChannel.set_length(0);
//   subpath3DAoA_association_Index.set_length(0);
//   subpathPhase_3Dfixedpt.set_length(0);
//   subpath_Boresight_3DAoD_angle.set_length(0);
//   subpath_Boresight_3DAoA_angle.set_length(0);
//   SubPathtypeinCluster3D.set_length(0);
//   aMap3D.set_length(0);
//   XPR_lin_3D=-1;
//   theta_Tx_3D=-1;
//   theta_Rx_3D=-1;
//   phi_Tx_3D=-1;
//   phi_Rx_3D=-1;
//   K_rice_3D=-1;
//   v_phase_3D=-1;
//   h_phase_3D=-1;
//   propagationScenario3DChannel=_SCM_UNINIT_;
//   addPathSpecificAntennaGain3D=true;
//   txLos3DAntGain=0;
//   rxLos3DAntGain=0;
//   txCrossPolarizationFlag3D=false;
//   txPolarizationAngleInDegree3D="90";
//   rxCrossPolarizationFlag3D=false;
//   rxPolarizationAngleInDegree3D="90";
//   txHeight=-1;
//   rxHeight=-1;
//   muOffSetZOD=-1;
//   muZSD=-1;
//   sectorTilt = 0;
// //   antElementsPerCol=-1;
//   txVerAntElementSpacing=-1.0;
//   txHorzAntElementSpacing=-1.0; 
//   rxVerAntElementSpacing=-1.0;
//   rxHorzAntElementSpacing=-1.0;
//   txVerAntPanalSpacing=-1.0;
//   txHorzAntPanalSpacing=-1.0;
//   rxVerAntPanalSpacing=-1.0;
//   rxHorzAntPanalSpacing=-1.0;
// }
// 
// // SCM3DLink::SCM3DLink(int dummy){
// //   
// //   isReadyFor3DChannel=false;
// //   isLOS3DChannel=-1;
// //   isOutdoorToIndoor3DChannel=-1;
// //   nClusters3DChannel=-1;
// //   tapPow3DChannel.set_length(0);
// //   tapDelays3DChannel.set_length(0);
// //   pathAoD3DChannel.set_length(0);
// //   pathAoA3DChannel.set_length(0);
// //   subpath3DAoA_association_Index.set_length(0);
// //   subpathPhase_3Dfixedpt.set_length(0);
// //   subpath_Boresight_3DAoD_angle.set_length(0);
// //   subpath_Boresight_3DAoA_angle.set_length(0);
// //   SubPathtypeinCluster3D.set_length(0);
// //   aMap3D.set_length(0);
// //   XPR_lin_3D=-1;
// //   theta_Tx_3D=-1;
// //   theta_Rx_3D=-1;
// //   phi_Tx_3D=-1;
// //   phi_Rx_3D=-1;
// //   K_rice_3D=-1;
// //   v_phase_3D=-1;
// //   h_phase_3D=-1;
// //   propagationScenario3DChannel=_SCM_UNINIT_;
// //   addPathSpecificAntennaGain3D=true;
// //   txCrossPolarizationFlag3D=false;
// //   txPolarizationAngleInDegree3D="90";
// //   rxCrossPolarizationFlag3D=false;
// //   rxPolarizationAngleInDegree3D="90";
// //   txHeight=-1;
// //   rxHeight=-1;
// //   muOffSetZOD=-1;
// //   muZSD=-1;
// //   antElementsPerCol=-1;
// //   verAntElementSpacing=-1.0;
// //   horzAntElementSpacing=-1.0;
// // }
// 
// bool SCM3DLink::is3DChannelInitialized(){
//   
//   return(isReadyFor3DChannel);
// }
// 
// void SCM3DLink::diable3DChannelPathSpecificAntennaGain(){
//   
//   addPathSpecificAntennaGain3D=false;
// }
// 
// void SCM3DLink::enable3DChannelPathSpecificAntennaGain(){
//   
//   addPathSpecificAntennaGain3D=true;
// }
// 
// double SCM3DLink::initialize3DChannelFadingParams(int  isOutdoortoIndoor,bool isLineOfSight,double dopplerSpread,double carrierFrequency,SCMPropagationScenario propScenario,SCMLargeScaleParameters_S &channel3DParameters,Location_S nodeLocationTx, Location_S nodeLocationRx,Antenna &txAntenna, Antenna &rxAntenna ,double txAntSpacing,double rxAntSpacing)
// {
//   
//   isLOS3DChannel = isLineOfSight;
//   isOutdoorToIndoor3DChannel=isOutdoortoIndoor;
//    
//   propagationScenario3DChannel=propScenario;
//   dopplerSpreadInHz3DChannel=dopplerSpread;
//   carrierFrequencyInHz3DChannel=carrierFrequency;
//   mobileVelocity3DChannel=dopplerSpreadInHz3DChannel*lightSpeed/carrierFrequencyInHz3DChannel;
//   vec v2_LSP;
//   K_rice_3D = channel3DParameters.k;
//   muOffSetZOD=channel3DParameters.muOffSetZOD;
//   muZSD=channel3DParameters.muZSD;
//   txHeight=nodeLocationTx.z;
//   rxHeight=nodeLocationRx.z;
//   txNumberOfTxRUs=txAntenna.numTxRUs;
//   rxNumberOfTxRUs=rxAntenna.numTxRUs;
//   totalTxAntElem = txAntenna.antElementCount;
//   totalRxAntElem = rxAntenna.antElementCount;
//   txAntStructure = txAntenna.antStructure;
//   (txAntenna.antStructure(4) == 2 ? txCrossPolarizationFlag3D=1:txCrossPolarizationFlag3D=0);
//   rxAntStructure= rxAntenna.antStructure;
//   (rxAntenna.antStructure(4) == 2 ? rxCrossPolarizationFlag3D= 1:rxCrossPolarizationFlag3D= 0);
//   
//   if(isLOS3DChannel == true )
//   {
//     
//     v2_LSP.set_size(7);
//     v2_LSP(0) = channel3DParameters.ds;
//     v2_LSP(1) = channel3DParameters.asd;
//     v2_LSP(2) = channel3DParameters.asa;
//     v2_LSP(3) = channel3DParameters.sf;    
//     v2_LSP(4) = channel3DParameters.zsa;
//     v2_LSP(5) = channel3DParameters.zsd;
//     v2_LSP(6) = channel3DParameters.k;
//     K_rice_3D = pow(10.0,v2_LSP(6)/10.0);
//       
//   }
//   else
//   {
//     v2_LSP.set_size(6);
//     v2_LSP(0) = channel3DParameters.ds;
//     v2_LSP(1) = channel3DParameters.asd;
//     v2_LSP(2) = channel3DParameters.asa;
//     v2_LSP(3) = channel3DParameters.sf;
//     v2_LSP(4) = channel3DParameters.zsa;
//     v2_LSP(5) = channel3DParameters.zsd;
//   }
//   
//   
//   initialize3DChannelModel(v2_LSP,nodeLocationTx,nodeLocationRx,txAntenna,rxAntenna,txAntSpacing,rxAntSpacing);
//   return(max(tapDelays3DChannel));
// }
// 
// void SCM3DLink::initialize3DChannelModel(vec v2_LSP,Location_S nodeLocationTx, Location_S nodeLocationRx,Antenna &txAntenna, Antenna &rxAntenna,double txAntSpacing, double rxAntSpacing)
// {
//   
//   int NosOfClusters , NosOfRays;
//   NosOfRays = 20;
//   double UE_sigma_AOD,UE_sigma_AOA,UE_sigma_DS,UE_sigma_SF,UE_sigma_ZOA,UE_sigma_ZOD;
//   
//   UE_sigma_DS = pow(10.0,v2_LSP(0)); 
//   UE_sigma_AOD = pow(10.0,v2_LSP(1)); 
//   UE_sigma_AOA = pow(10.0,v2_LSP(2)); 
//   UE_sigma_SF = v2_LSP(3); 
//   UE_sigma_ZOA = pow(10.0,v2_LSP(4)); 
//   UE_sigma_ZOD = pow(10.0,v2_LSP(5)); 
// 
//     
//   NosOfClusters = get3DChannelClusterCount();
//   nClusters3DChannel = NosOfClusters;
//   
//   v_phase_3D = (2*randu()-1)*180;
//   h_phase_3D=(2*randu()-1)*180;
//   
//   // Limiting the AOA and AOD to 104 degress & Limiting the ZOA and ZOD to 52 degress as per 3GPP TR 36.873 V2.0.0 (2014-03)
//   if (UE_sigma_AOA >104.0)
//     UE_sigma_AOA = 104.0;
//   if (UE_sigma_AOD >104.0)
//     UE_sigma_AOD = 104.0;
//   if (UE_sigma_ZOA >52.0)
//     UE_sigma_ZOA = 52.0;
//   if (UE_sigma_ZOD >52.0)
//     UE_sigma_ZOD = 52.0;
//   
//   
//   double elevationTilt=-1.0; //It could be between 0 and 180, as per Table 7.1-1 TR 36.873
// //   vec TxRUtiltAngle;
// //   TxRUtiltAngle.set_length(0);
// //AAS
//   int Mg, Ng, M,N,P, d_gv,d_gh, d_v, d_h,TXRUsPerPanal, elemPerTXRU, rxElemPerTXRU;
//   double tiltAngle_V, tiltAngle_H;
// 
//   int nElemPerTxRU, rxRows;  
//   int nTxRUs;
//   double deg2rad = pi/180; 
//   double lamda=lightSpeed/carrierFrequencyInHz3DChannel;
//   
//   
// /*
//   if(txAntenna.antElementsPerCol!=-1 && txAntenna.tiltAngle!=-1)//Downlink
//   {
//     if(txAntenna.isAASEnabled)
//       TxRUtiltAngle=txAntenna.TxRUtiltAngle;
//     else
//       elevationTilt=txAntenna.tiltAngle;
//     //txRows=txAntenna.antElementsPerCol;
//     nElemPerTxRU=txAntenna.TxRUToAntMapping.cols();
//     antElementsPerCol=txAntenna.antElementsPerCol;
//     verAntElementSpacing=txAntenna.verAntSpacing*lamda;
//     horzAntElementSpacing=txAntenna.horzAntSpacing*lamda;
//     rxRows=1;
//   }
//   else if(rxAntenna.antElementsPerCol!=-1 && rxAntenna.tiltAngle!=-1)//uplink
//   { 
//     elevationTilt=rxAntenna.tiltAngle;
//     rxRows=rxAntenna.antElementsPerCol; 
//     antElementsPerCol=rxAntenna.antElementsPerCol; 
//     verAntElementSpacing=rxAntenna.verAntSpacing*lamda;
//     horzAntElementSpacing=rxAntenna.horzAntSpacing*lamda;
//     nElemPerTxRU=1;
//   }
//   else{ cout<<"Error...elevationTilt & txRows can not be -ve, please check configFiles."<<endl;abort();}  
//   */
//     if(txVerAntElementSpacing<0 && txHorzAntElementSpacing<0)
//     { cout<<"Error...verAntElementSpacing & horzAntElementSpacing can not be -ve, please check configFiles."<<endl;abort();}   
//   
//     
//   nTxRUs=txAntenna.numTxRUs;
//   double temp_Tilt1 = 0.0, temp_Tilt2 = 0.0;
//   
//   txVerAntPanalSpacing = txAntenna.antSpacing(0)*lamda; // d_v value of Antenna panal spacing
//   txHorzAntPanalSpacing = txAntenna.antSpacing(1)*lamda;// d_h value of Antenna panal spacing
//   
//   rxVerAntPanalSpacing = rxAntenna.antSpacing(0)*lamda; // d_v value of Antenna spacing
//   rxHorzAntPanalSpacing = rxAntenna.antSpacing(1)*lamda;// d_h value of Antenna spacing
//   
//   txVerAntElementSpacing = txAntenna.antSpacing(2)*lamda; // d_v value of Antenna spacing
//   txHorzAntElementSpacing = txAntenna.antSpacing(3)*lamda;// d_h value of Antenna spacing
//   
//   rxVerAntElementSpacing = rxAntenna.antSpacing(2)*lamda; // d_v value of Antenna spacing
//   rxHorzAntElementSpacing = rxAntenna.antSpacing(3)*lamda;// d_h value of Antenna spacing
//   
//   tiltAngle_V = txAntenna.zenithBeamAngles(0);
//   tiltAngle_H = txAntenna.azimuthBeamAngles(0);
//   
//   sectorTilt = txAntenna.getHorizontalTilt();
//   
//   
//   
//   
// //   if(txAntenna.isAASEnabled)
// //   {
// 
//       temp_Tilt1 =2*pi*(txVerAntElementSpacing*sin(deg2rad*tiltAngle_V))/lamda;
//       temp_Tilt2 =-2*pi*(txHorzAntElementSpacing*cos(deg2rad*tiltAngle_V)*sin(deg2rad*tiltAngle_H))/lamda;
// //   }
// //   else
// //   {
// //     for(int i=0;i<nTxRUs;i++)
// //     {
// //       temp_Tilt1(i)=2*pi*txVerAntElementSpacing*cos(deg2rad*elevationTilt)/lamda;//!FIXME need to modify for AAS off case
// // //       temp_elevationTilt(i)=2*pi*verAntElementSpacing*cos(deg2rad*elevationTilt)/lamda;//needs to change for different vertical tilt
// //     }
// //   }
//   
//   generate3DChannelPowerDelayProfiles(NosOfClusters,UE_sigma_DS,tapPow3DChannel,tapDelays3DChannel);
//   pathAoD3DChannel=generate3DChannelPathAODs(NosOfClusters,tapPow3DChannel,UE_sigma_AOD);
//   pathAoA3DChannel=generate3DChannelPathAOAs(NosOfClusters,tapPow3DChannel,UE_sigma_AOA);  
//   pathZoD3DChannel=generate3DChannelPathZODs(NosOfClusters,tapPow3DChannel,UE_sigma_ZOD);
//   pathZoA3DChannel=generate3DChannelPathZOAs(NosOfClusters,tapPow3DChannel,UE_sigma_ZOA);  
//   subpath3DAoA_association_Index=associate3DChannelBSMSpaths(tapPow3DChannel.length(),NosOfRays);
//   subpath3DZoA_association_Index=associate3DChannelBSMSpaths(tapPow3DChannel.length(),NosOfRays);
//   subpath3DZoD_association_Index=associate3DChannelBSMSpaths(tapPow3DChannel.length(),NosOfRays);  
//   getBoresight_LOS_3Dangle(theta_Tx_3D,theta_Rx_3D,phi_Tx_3D,phi_Rx_3D,nodeLocationTx,nodeLocationRx,sectorTilt);   
//   generate3DChannelSubClusters(NosOfRays);
//   subpathPhase_3Dfixedpt = generate3DSubpathPhases(tapPow3DChannel.length());  
//   XPR_lin_3D = get3DCrossPolarizationRatio();  
//   computeBoresight2subpath_3Dangles(tapPow3DChannel.length(),NosOfRays);  
//   compute3DAntennaGains(txAntenna,rxAntenna);
//   
//     
//   
// //   txCrossPolarizationFlag3D=txAntenna.isAntennaCrossPolarized();
// //   int txAntennaCount=txAntenna.getNumberOfPhysicalAntenna();     
// //   txPolarizationAngleInDegree3D.set_size(txAntennaCount);
//   
//   //Complex weight for antenna element m in elevation,Table 7.1-1, TR 36.873
//   // AAS
//   Mg = txAntenna.antStructure(0);
//   Ng = txAntenna.antStructure(1);
//   M = txAntenna.antStructure(2);
//   N = txAntenna.antStructure(3);
//   P = txAntenna.antStructure(4);
//  
//   int txAntennaCount=txAntenna.getNumberOfPhysicalAntenna(); // Equal to TXRUs   
//   txPolarizationAngleInDegree3D.set_size(txAntennaCount);  
// 
//   
//   elemPerTXRU = txAntenna.antElementCount/txAntenna.numTxRUs;
//   
//   int nRowsPerTXRU = M;
//   int nColsPerTXRU = floor_i(elemPerTXRU/M);
//   
//   antWeights.set_length(elemPerTXRU);
//   int antCnt = 0;
//   for(int rowIndx=0;rowIndx<nRowsPerTXRU;rowIndx++)
//     for(int colIndx=0;colIndx<nColsPerTXRU;colIndx++)
//     {
//       antWeights(antCnt) = (1/sqrt(elemPerTXRU))*dComplex(cos((temp_Tilt1*rowIndx)+(temp_Tilt2*colIndx)),sin((temp_Tilt1*rowIndx)+(temp_Tilt2*colIndx)));
//     }
//   
//   
//   if(txCrossPolarizationFlag3D==false)
//   {
//       /* For 2D matrix pattern, the antenna elements are stored in the following form
//     * Example given for 16 elements, 4X4 matrix
//     * 
//     * 0  4  8   12
//     * 1  5  9   13
//     * 2  6  10  14
//     * 3  7  11  15
//     * 
//     */
//     
//     //Forming 2D Tx-Antenna Pattern
//     // txRows=1;//if K=1,txRows=1 or else txRows=M as per Table 7.1-1 TR 36.873
//      
//      
// //     if(txAntennaCount < antElementsPerCol || txAntennaCount%antElementsPerCol!=0)
// //     {
// //       cout<<"txAntennaCount : "<<txAntennaCount<<endl;
// //       cout<<"antElementsPerCol : "<<antElementsPerCol<<endl;
// //       cout<<"Error....For 2D Antenna pattern Tx Antenna shall be greater than & integer multiple of rows="<<nElemPerTxRU<<endl;abort();
// //     }
//     //txColumns=txAntennaCount/txRows;
// //     txAntElementIndxFor3D.set_length(txAntennaCount);
// //     elevationAntWeight.set_size(antElementsPerCol,txAntennaCount/antElementsPerCol);
// //     elevationWeights.set_size(nTxRUs);
// 
//     txAntElementIndxFor3D.set_length(txAntennaCount);
// 
// 
// 
//     
//     for(int tx_ant=0;tx_ant<txAntennaCount;tx_ant++)
//     {
// // 	txAntElementIndxFor3D(tx_ant).set_length(2);	
// // 	txAntElementIndxFor3D(tx_ant)(0)=(tx_ant%antElementsPerCol); //row Index 
// // 	txAntElementIndxFor3D(tx_ant)(1)=floor_i(double(tx_ant)/antElementsPerCol);//column index 
// // 	txPolarizationAngleInDegree3D(tx_ant)=90;
//       txAntElementIndxFor3D(tx_ant).set_length(2);	
//       txAntElementIndxFor3D(tx_ant)(0)=floor((tx_ant*nColsPerTXRU)/(Ng*N))*nRowsPerTXRU; //(Row index) Assumed that all element in column belong to one TXRU
//       txAntElementIndxFor3D(tx_ant)(1)=(tx_ant*nColsPerTXRU) % (Ng*N);//column index 
//       txPolarizationAngleInDegree3D(tx_ant)=90;  
//       
//     }
// 
// //       elevationWeights(txruIndx).set_length(nElemPerTxRU);
// //       cvec antennaWeight=zeros_c(nElemPerTxRU);
// //       for(int rowIndx=0;rowIndx<nElemPerTxRU;rowIndx++)
// //       {
// //          antennaWeight(rowIndx)=(1/sqrt(nElemPerTxRU))*dComplex(cos(temp_elevationTilt(txruIndx)*rowIndx),sin(-1*temp_elevationTilt(txruIndx)*rowIndx));
// // 	 elevationWeights(txruIndx)(rowIndx)=(1/sqrt(nElemPerTxRU))*dComplex(cos(temp_elevationTilt(txruIndx)*rowIndx),sin(-1*temp_elevationTilt(txruIndx)*rowIndx));
// //       }
// //       setInMatrix(elevationAntWeight,txAntenna.TxRUToAntMapping.get_row(txruIndx),antennaWeight);
//       
// 
//       
//     
//   }
//   else // if txCrossPolarization is True
//   {
//     
// 	/* For 2D matrix pattern for cross-polarized array, the antenna elements are stored in the following form
//       * Example given for 16 elements, 4X2 matrix
//       * 
//       * (0,1)   (8,9)  
//       * (2,3)  (10,11)
//       * (4,5)  (12,13)
//       * (6,7)  (14,15)  
//       * 
//       */
// 
//       if(txAntennaCount%2==1)
// 	{
// 	  cout<<"[detl:] Error. Cross Polarization can't be enabled with odd number of tx antennas."<<endl;
// 	  abort();
// 	}
// 	
// 
//       //Forming 2D Tx-Antenna Pattern
//       int xPOl_txAntennaCount = txAntennaCount/2;
// //       if(xPOl_txAntennaCount < nElemPerTxRU || xPOl_txAntennaCount%nElemPerTxRU!=0)
// //       {
// // 	cout<<"Error....For 2D Antenna pattern Tx Antenna shall be greater than & integer multiple of rows="<<nElemPerTxRU<<endl;abort();
// // 	
// //       }
//       //txColumns=xPOl_txAntennaCount/txRows;
// //       nTxRUs=txAntennaCount/nElemPerTxRU;
// //       txAntElementIndxFor3D.set_length(txAntennaCount);
// //       elevationAntWeight.set_size(antElementsPerCol,txAntennaCount/antElementsPerCol);
// //       elevationWeights.set_size(nTxRUs);
//       txAntElementIndxFor3D.set_length(txAntennaCount);
//       antWeights.set_size(txAntennaCount);
//       int nRowsPerTXRU = M;
//       int nColsPerTXRU = floor_i(elemPerTXRU/M);
//       int antCnt = 0;
//       
//       for(int tx_ant=0;tx_ant < xPOl_txAntennaCount;tx_ant++)
//       {
// // 	  txAntElementIndxFor3D(tx_ant).set_length(2);
// // 	  txAntElementIndxFor3D(xPOl_txAntennaCount+tx_ant).set_length(2);
// // 	  
// // 	  txAntElementIndxFor3D(tx_ant)(0)=(tx_ant%antElementsPerCol); //row Index     
// // 	  txAntElementIndxFor3D(tx_ant)(1)=floor_i(tx_ant/antElementsPerCol);//column index     
// // 	  
// // 	  txAntElementIndxFor3D(xPOl_txAntennaCount+tx_ant)(0)=(tx_ant%antElementsPerCol); //row Index     
// // 	  txAntElementIndxFor3D(xPOl_txAntennaCount+tx_ant)(1)=floor_i(tx_ant/antElementsPerCol);//column index   
// // 	  
// // 	  
// // 	  txPolarizationAngleInDegree3D(tx_ant)=45;
// // 	  txPolarizationAngleInDegree3D(xPOl_txAntennaCount+tx_ant)=-45;
//      
// 	antCnt = tx_ant*2; // Choosing alternate locations
// 	txAntElementIndxFor3D(antCnt).set_length(2);
// 	txAntElementIndxFor3D(antCnt+1).set_length(2);
// 	
// 	txAntElementIndxFor3D(antCnt)(0)=floor((tx_ant*nColsPerTXRU)/(Ng*N))*M; //(Row index) Assumed that all element in column belong to one TXRU 
// 	txAntElementIndxFor3D(antCnt+1)(0)=floor((tx_ant*nColsPerTXRU)/(Ng*N))*M;;//row index   
// 	
// 	txAntElementIndxFor3D(antCnt)(1)=(tx_ant*nColsPerTXRU) % (Ng*N);//column index 
// 	txAntElementIndxFor3D(antCnt+1)(1)=(tx_ant*nColsPerTXRU) % (Ng*N); //col Index     
// 	
// 	txPolarizationAngleInDegree3D(antCnt)=45;
// 	txPolarizationAngleInDegree3D(antCnt+1)=-45;
// 	
//       }  
//   }
//   
//   //Forming 2D Rx-Antenna Pattern
//   //AAS
//   Mg = rxAntenna.antStructure(0);
//   Ng = rxAntenna.antStructure(1);
//   M = rxAntenna.antStructure(2);
//   N = rxAntenna.antStructure(3);
//   P = rxAntenna.antStructure(4);
//    
//   int rxAntennaCount=rxAntenna.getNumberOfPhysicalAntenna();  
//   rxPolarizationAngleInDegree3D.set_size(rxAntennaCount);
//   rxElemPerTXRU = rxAntenna.antElementCount/rxAntenna.numTxRUs;      
//   
//   if(rxCrossPolarizationFlag3D==false)
//   {
// //     rxRows=1;
// //     if(rxAntennaCount < rxRows || rxAntennaCount%rxRows!=0)
// //     {
// //       cout<<"Error....For 2D Antenna pattern Rx Antenna shall be greater than & integer multiple of rows="<<rxRows<<endl;abort();
// //       
// //     }
// //     rxColumns=rxAntennaCount/rxRows;
//     antWeights.set_size(rxAntennaCount);
//     int nRowsPerTXRU = M;
//     int nColsPerTXRU = floor_i(rxElemPerTXRU/M);
//     rxAntElementIndxFor3D.set_length(rxAntennaCount);
//     
//     for(int rx_ant=0;rx_ant<rxAntennaCount;rx_ant++)
//     {
// // 	rxAntElementIndxFor3D(rx_ant).set_length(2);
// // 	
// // 	rxAntElementIndxFor3D(rx_ant)(0)=(rx_ant%rxRows); //row Index         
// // 	rxAntElementIndxFor3D(rx_ant)(1)=floor_i(rx_ant/rxRows);//column index
// // 	
// // 	rxPolarizationAngleInDegree3D(rx_ant)=90;
//       rxAntElementIndxFor3D(rx_ant).set_length(2);	
//       rxAntElementIndxFor3D(rx_ant)(0)=floor((rx_ant*nColsPerTXRU)/(Ng*N))*M; //(Row index) Assumed that all element in column belong to one TXRU
//       rxAntElementIndxFor3D(rx_ant)(1)=(rx_ant*nColsPerTXRU) % (Ng*N);//column index 
//       rxPolarizationAngleInDegree3D(rx_ant)=90;  
//       
//     }
//     
//   }
//   else //rxCrossPolarizationFlag3D is True
//   {
//     /* For 2D matrix pattern for cross-polarized array, the antenna elements are stored in the following form
//       * Example given for 16 elements, 4X2 matrix
//       * 
//       * (0,1)   (8,9)  
//       * (2,3)  (10,11)
//       * (4,5)  (12,13)
//       * (6,7)  (14,15)  
//       * 
//       */
// 
//       if(rxAntennaCount%2==1)
// 	{
// 	  cout<<"[detl:] Error. Cross Polarization can't be enabled with odd number of Rx antennas."<<endl;
// 	  abort();
// 	}
// 
//       //Forming 2D Rx-Antenna Pattern
//       int xPOl_rxAntennaCount = rxAntennaCount/2;
// //       rxRows=1; 
// //       if(xPOl_rxAntennaCount < rxRows || xPOl_rxAntennaCount%rxRows!=0)
// //       {
// // 	cout<<"Error....For 2D Antenna pattern Rx Antenna shall be greater than & integer multiple of rows="<<rxRows<<endl;abort();
// // 	
// //       }
// //       rxColumns=xPOl_rxAntennaCount/rxRows;
//       
//       antWeights.set_size(rxAntennaCount);
//       int nRowsPerTXRU = M;
//       int nColsPerTXRU = floor_i(rxElemPerTXRU/M);
//       int antCnt = 0;
//       rxAntElementIndxFor3D.set_length(rxAntennaCount);
//       
//       for(int rx_ant=0;rx_ant < xPOl_rxAntennaCount;rx_ant++)
//       {
// // 	  rxAntElementIndxFor3D(rx_ant).set_length(2);
// // 	  
// // 	  rxAntElementIndxFor3D(rx_ant)(0)=(rx_ant%rxRows); //row Index     
// // 	  rxAntElementIndxFor3D(rx_ant)(1)=floor_i(rx_ant/rxRows);//column index     
// // 	  
// // 	  rxAntElementIndxFor3D(xPOl_rxAntennaCount+rx_ant)(0)=(rx_ant%rxRows); //row Index     
// // 	  rxAntElementIndxFor3D(xPOl_rxAntennaCount+rx_ant)(1)=floor_i(rx_ant/rxRows);//column index  
// // 	  
// // 	  rxPolarizationAngleInDegree3D(rx_ant)=45;
// // 	  rxPolarizationAngleInDegree3D(xPOl_rxAntennaCount+rx_ant)=-45;
// 	
// 	antCnt = rx_ant*2; // Choosing alternate locations
// 	rxAntElementIndxFor3D(antCnt).set_length(2);
// 	rxAntElementIndxFor3D(antCnt+1).set_length(2);
// 	
// 	rxAntElementIndxFor3D(antCnt)(0)=floor((rx_ant*nColsPerTXRU)/(Ng*N))*M; //(Row index) Assumed that all element in column belong to one TXRU 
// 	rxAntElementIndxFor3D(antCnt+1)(0)=floor((rx_ant*nColsPerTXRU)/(Ng*N))*M;;//row index   
// 	
// 	rxAntElementIndxFor3D(antCnt)(1)=(rx_ant*nColsPerTXRU) % (Ng*N);//column index 
// 	rxAntElementIndxFor3D(antCnt+1)(1)=(rx_ant*nColsPerTXRU) % (Ng*N); //col Index     
// 	
// 	rxPolarizationAngleInDegree3D(antCnt)=45;
// 	rxPolarizationAngleInDegree3D(antCnt+1)=-45;
// 	
//       }
//     
//   }
//   
//   isReadyFor3DChannel=true;
// }
// 
// 
// int SCM3DLink::get3DChannelClusterCount()
// {
// 
//   int NosOfClusters;
//   switch (propagationScenario3DChannel) // 3GPP TR 36.873 V2.0.0 (2014-03), Table 7.3-6
//   {
//        
//     case _SCM3D_URBAN_MICRO_:      
//       if (isOutdoorToIndoor3DChannel == 1) NosOfClusters = 12;
//       else//Out Door 
//       {
// 	if (isLOS3DChannel==1)    NosOfClusters = 12;
//         else		NosOfClusters = 19;	
//       }
//       break;      
//       
//     case _SCM3D_URBAN_MACRO_:      
//       if (isOutdoorToIndoor3DChannel == 1) NosOfClusters = 12;
//       else//Out Door
//       {
// 	if (isLOS3DChannel==1)    NosOfClusters = 12;
// 	else		NosOfClusters = 20;
//       }        
//      break; 
//      
//     default :
//       cout << "\n\n\ngetClusterCount: unknown Propagation scenario -- exiting program" << endl;
//       abort();
//       break;
//   }
//   
//   return NosOfClusters;
// }
// 
// void SCM3DLink::generate3DChannelPowerDelayProfiles(int NosOfClusters,double sigma_DS,vec& tapPow3DChannel,vec& tapdelays)
// {
//     vec z,tau,tau_Los,tauc,Pn_dash,Pnc,Pn,Pn_sort,zeta;
//     double tmp,cutoff;
//     double sigma_RND,D,K1,r_DS;
//     ivec Pn_sort_index,skip_index;
//     int M;
//     
//     r_DS = get3DChannelDelayScaling();
//     sigma_RND = get3DChannelClusterShadow();
//     
//    // sigma_RND=pow(10.0,sigma_RND/10.0);// added for testing
//     
//     z = randu(NosOfClusters); 
//     tau = -1*r_DS*sigma_DS*log(z);
//     sort(tau);
//     tmp = tau(0);
//     tau = tau - tmp;
//     tau(0)=0;    
//     
//     if (isLOS3DChannel == 1)
//     {
//       zeta = sigma_RND*randn(NosOfClusters); 
//       Pn_dash = elem_mult( exp( (1-r_DS)*tau/(r_DS*sigma_DS) ) , pow10(-1*zeta/10) );
//       Pn = Pn_dash/sum(Pn_dash);
//       
//       Pn_sort = -1*Pn;
//       sort(Pn_sort);
//       Pn_sort = Pn_sort*-1; 
//       Pn_sort_index = sort_index(-1*Pn);
//       
//       skip_index.set_length(0);
//       cutoff = 0.0032*max(Pn); // -25dB
//       for (int i = 0; i < NosOfClusters; i++)
//       {
// 	if (Pn_sort(i) < cutoff)
// 	{
// 	  M = skip_index.length();
// 	  skip_index.set_length(M+1,true);
// 	  skip_index(M) = Pn_sort_index(i);
// 	}
//       }
//       
//       sort(skip_index);
//       for (int i = skip_index.length()-1; i>=0; i--)
//       {
// 	tau.del(skip_index(i));
// 	Pn.del(skip_index(i));
// 	
//       } 
//       
//       tauc = tau;
//       K1 = 10*log10(K_rice_3D); 
//       D = 0.7705 - 0.0433*K1 + 0.0002*pow(K1,2) + 0.000017*pow(K1,3);
//       tau_Los = tauc/D;
//       Pnc = Pn;
//       tapdelays = tau_Los;
//       
//     }
//     else //NLOS
//     {
//       zeta = sigma_RND*randn(NosOfClusters); 
//       Pn_dash = elem_mult( exp( (1-r_DS)*tau/(r_DS*sigma_DS) ) , pow10(-1*zeta/10) );
//       Pn = Pn_dash/sum(Pn_dash);
//       
//       Pn_sort = -1*Pn; 
//       sort(Pn_sort);
//       Pn_sort = Pn_sort*-1; 
//       Pn_sort_index = sort_index(-1*Pn);
//       
//       skip_index.set_length(0);
//       cutoff = 0.0032*max(Pn); // -25dB
//       for (int i = 0; i < NosOfClusters; i++)
//       {
// 	if (Pn_sort(i) < cutoff)
// 	{
// 	  M = skip_index.length();
// 	  skip_index.set_length(M+1,true);
// 	  skip_index(M) = Pn_sort_index(i);
// 	}
//       }
//       
//       sort(skip_index);
//       for (int i = skip_index.length()-1; i>=0; i--)
//       {
// 	tau.del(skip_index(i));
// 	Pn.del(skip_index(i));
// 	
//       } 
//       
//       tauc = tau;
//       Pnc = Pn;
//       tapdelays = tauc;
//     }
//     Pnc = Pnc/sum(Pnc); 
//     tapPow3DChannel = Pnc;  
// }
// 
// 
// double SCM3DLink::get3DChannelDelayScaling()
// {
//   double rtau;
//   switch (propagationScenario3DChannel) // see 3GPP 3GPP TR 36.873 V2.0.0 (2014-03) Table 7.3-6
//   {
//     case _SCM3D_URBAN_MICRO_:      
//       if (isOutdoorToIndoor3DChannel == 1)rtau = 2.2;
//       else
//       {
// 	if (isLOS3DChannel==1)	rtau = 3.2;
// 	else	  	rtau = 3.0;
//       }
//       break;
//       
//     case _SCM3D_URBAN_MACRO_:
//       if (isOutdoorToIndoor3DChannel == 1)rtau = 2.2;
//       else
//       {
// 	 if (isLOS3DChannel==1)	rtau = 2.5;
// 	 else	  	rtau = 2.3;
//       }
//       break;
//       
//     default :
//       cout << "\n\n\ngetDelayScaling: unknown Propagation scenario -- exiting program" << endl;
//       exit(0);
//       break;
//   }
//   
//   return rtau;
//   
// }
// 
// double SCM3DLink::get3DChannelClusterShadow()
// {
//   double zeta;
//   
//   switch (propagationScenario3DChannel) // see 3GPP 3GPP TR 36.873 V2.0.0 (2014-03), Table 7.3-6
//   {
//     case _SCM3D_URBAN_MICRO_:
//       if (isOutdoorToIndoor3DChannel == 1) zeta = 4.0;
//       else//Out Door
//       {
// 	if (isLOS3DChannel==1)      zeta = 3.0;
// 	else		 zeta = 3.0;
//       }
//      break; 
//      
//     case _SCM3D_URBAN_MACRO_:
//       if (isOutdoorToIndoor3DChannel == 1) zeta = 4.0;
//       else//Out Door
//       {
// 	if (isLOS3DChannel==1)      zeta = 3.0;
// 	else		 zeta = 3.0;
//       }
//      break; 
//      
//     default :
//       cout << "\n\n\ngetClusterShadow: unknown Propagation scenario -- exiting program" << endl;
//       exit(0);
//       break;
//   }
//   return zeta;
//   
// }
// 
// 
// vec SCM3DLink::generate3DChannelPathAODs(int NosOfClusters,vec tpow,double sigma_AOD)
// {
//   vec pathAoD3DChannel;
//   
//   switch (propagationScenario3DChannel)
//   {
//     case _SCM3D_URBAN_MICRO_:
//       pathAoD3DChannel = generateAzimuth3DChannelClusterAngles(tpow,sigma_AOD);
//       break;
//     case _SCM3D_URBAN_MACRO_:
//       pathAoD3DChannel = generateAzimuth3DChannelClusterAngles(tpow,sigma_AOD);
//       break;    
//     default :
//       cout << "\n\n\ngeneratePathAODs: unknown Propagation scenario -- exiting program" << endl;
//       exit(0);
//       break;
//   }
//   return(pathAoD3DChannel);
// }
// 
// vec SCM3DLink::generate3DChannelPathAOAs(int NosOfClusters,vec tpow,double sigma_AOA)
// {
//   vec pathAoA3DChannel;
//  
//   pathAoA3DChannel.set_length(tpow.length());
//   
//   
//   switch (propagationScenario3DChannel)
//   {
//     case _SCM3D_URBAN_MICRO_:
//       pathAoA3DChannel = generateAzimuth3DChannelClusterAngles(tpow,sigma_AOA);
//       break;
//     case _SCM3D_URBAN_MACRO_:
//       pathAoA3DChannel = generateAzimuth3DChannelClusterAngles(tpow,sigma_AOA);
//       break;    
//     default :
//       cout << "\n\n\ngeneratePathAOAs: unknown Propagation scenario -- exiting program" << endl;
//       exit(0);
//       break;
//   }
//   return(pathAoA3DChannel);
// }
// 
// vec SCM3DLink::generateAzimuth3DChannelClusterAngles(vec tpow, double sigma_A)
// {
//     vec pathAngles,tmp,SignScale;
//     double C;
//     double Pmax;
//     int NosOfClusters;
//     
//     pathAngles.set_length(tpow.length());
//     NosOfClusters = get3DChannelClusterCount();
//     
//     switch (NosOfClusters) // see 3GPP 3GPP TR 36.873 V2.0.0 (2014-03), Table 7.3-2
//     {
//       case 4:      C = 0.779;break;
//       case 5:      C = 0.860;break;
//       case 8:      C = 1.018;break;
//       case 10:     C = 1.090;break;
//       case 11:     C = 1.123;break;
//       case 12:     C = 1.146;break;
//       case 14:     C = 1.190;break;
//       case 15:     C = 1.211;break;
//       case 16:     C = 1.226;break;
//       case 19:     C = 1.273;break;
//       case 20:     C = 1.289;break;
//       default :
// 	cout << "\n\n\ngenerateClusterAngles: wrong number of clusters -- exiting program" << endl;
// 	abort();
// 	break;
//     }
//     
//     double K = 10*log10(K_rice_3D);
//     if (isLOS3DChannel == 1)    
//       C = C*(1.1035 - 0.028*K - 0.002*pow(K,2) + 0.0001*pow(K,3));
//     
//     
//     K = pow(10.0,K/10);
//     if (isLOS3DChannel == 1) 
//     {
//       tpow = (1/(K+1))*tpow;
//       tpow(0) = tpow(0) + K/(K+1);
//     }
//     
//     Pmax = max(tpow);
//     int PmaxIndx=max_index(tpow);
//     for (int i=0; i < pathAngles.length(); i++)
//       if(i==PmaxIndx)
// 	pathAngles(i) = 0;
//       else
// 	pathAngles(i) = (2.0*sigma_A/1.4)*sqrt( -1*log( tpow(i)/Pmax ) ) / C; 
//       
// 
//     tmp = randu(pathAngles.length()) - 0.5;
//     SignScale = sign(tmp);   
//     tmp = (sigma_A/7)*randn(pathAngles.length()); 
//     pathAngles = elem_mult(pathAngles,SignScale) + tmp; 
//     if (isLOS3DChannel == 1)
//       pathAngles = pathAngles - pathAngles(0);
//     
//     return pathAngles;
// }
// 
// vec SCM3DLink::generate3DChannelPathZODs(int NosOfClusters,vec tpow,double sigma_ZOD)
// {
//   vec pathZoD;
//   
//   switch (propagationScenario3DChannel)
//   {
//     case _SCM3D_URBAN_MICRO_:
//       pathZoD = generateZenith3DChannelClusterAngles(tpow,sigma_ZOD,1);
//       break;
//     case _SCM3D_URBAN_MACRO_:
//       pathZoD = generateZenith3DChannelClusterAngles(tpow,sigma_ZOD,1);
//       break;    
//     default :
//       cout << "\n\n\ngeneratePathAODs: unknown Propagation scenario -- exiting program" << endl;
//       exit(0);
//       break;
//   }
//   return(pathZoD);
// }
// 
// vec SCM3DLink::generate3DChannelPathZOAs(int NosOfClusters,vec tpow,double sigma_ZOA)
// {
//   vec pathZAoA;
//   pathZAoA.set_length(tpow.length());
//   
//   
//   switch (propagationScenario3DChannel)
//   {
//     case _SCM3D_URBAN_MICRO_:
//         pathZAoA = generateZenith3DChannelClusterAngles(tpow,sigma_ZOA,0);
//       break;
//     case _SCM3D_URBAN_MACRO_:
//         pathZAoA = generateZenith3DChannelClusterAngles(tpow,sigma_ZOA,0);
//       break;    
//     default :
//       cout << "\n\n\ngeneratePathAOAs: unknown Propagation scenario -- exiting program" << endl;
//       exit(0);
//       break;
//   }
//   return(pathZAoA);
// }
// 
// vec SCM3DLink::generateZenith3DChannelClusterAngles(vec tpow, double sigma_Z,bool isZOD)
// {
//     vec pathAngles,tmp,SignScale;
//     double C;
//     double Pmax;
//     int NosOfClusters;   
//     
//     pathAngles.set_length(tpow.length());
//     NosOfClusters = get3DChannelClusterCount();
//     
//     switch (NosOfClusters) // see 3GPP 3GPP TR 36.873 V2.0.0 (2014-03), Table 7.3-4
//     {
//       case 12:     C = 1.104;break;
//       case 19:     C = 1.184;break;
//       case 20:     C = 1.178;break;
//       default :
// 	cout << "\n\n\ngenerateClusterAngles: wrong number of clusters -- exiting program" << endl;
// 	abort();
// 	break;
//     }
//     
//     double K = 10*log10(K_rice_3D);
//     if (isLOS3DChannel == 1)    
//       C = C*(1.3086 + 0.0339*K - 0.0077*pow(K,2) + 0.0002*pow(K,3));
//     
//     
//     K = pow(10.0,K/10);
//     if (isLOS3DChannel == 1) 
//     {
//       tpow = (1/(K+1))*tpow;
//       tpow(0) = tpow(0) + K/(K+1);
//     }
//     
//     Pmax = max(tpow);
//     int PmaxIndx=max_index(tpow);
//     for (int i=0; i < pathAngles.length(); i++)
//       if(i==PmaxIndx)
// 	pathAngles(i) = 0;
//       else
// 	pathAngles(i) = (-1*sigma_Z)*log(tpow(i)/Pmax)/C; //Eq (13)
//       
// 
//     tmp = randu(pathAngles.length()) - 0.5;
//     SignScale = sign(tmp);
//     tmp = (sigma_Z/7)*randn(pathAngles.length()); 
//     pathAngles = elem_mult(pathAngles,SignScale) + tmp; 
//     if (isLOS3DChannel == 1)
//       pathAngles = pathAngles - pathAngles(0);
//     
//     if(isZOD)
//       pathAngles = pathAngles + muOffSetZOD;   
//     
//     
//     return pathAngles;
// }
// 
// Array<ivec> SCM3DLink::associate3DChannelBSMSpaths(int nTaps,int NosOfRays) //AOD to AOA association
// {
//   Array<ivec> associated_Index;
//   associated_Index.set_size(nTaps);
//   int m,n,tmp1;
//   ivec idx;
//   
//   if (NosOfRays != 20)
//   {
//     cout << "\n\n\nassociate3DChannelBSMSpaths: Size mismatch  should be same as subpath angle offsets size, presently assumed to be 20 -- exiting program" << endl;
//     exit(0);
//   }
//   
//   idx.set_length(NosOfRays);
//   for (int i = 0; i<NosOfRays; i++)
//     idx(i) = i;
//   
//   for (int i = 0; i<nTaps; i++)
//   {
//     associated_Index(i) = idx;
//     
//     for (int j = 0; j<NosOfRays*10; j++)
//     {
//       m = randi(0,NosOfRays-1);
//       n = randi(0,NosOfRays-1);
//       
//       tmp1 = associated_Index(i)(m);
//       associated_Index(i)(m) = associated_Index(i)(n);
//       associated_Index(i)(n) = tmp1;
//     }
//   }
//   return(associated_Index);
// }
// 
// void SCM3DLink::getBoresight_LOS_3Dangle(double& thetaTx, double& thetaRx, double& phiTx, double& phiRx, Location_S nodeLocationTx, Location_S nodeLocationRx, double sectorTilt)
// {
//   double lamda = lightSpeed/carrierFrequencyInHz3DChannel;
//   
// 
//   //Mg*Ng*M*N*P
//   int totalElmAtTx =  (txAntStructure(0)*txAntStructure(1)*txAntStructure(2)*txAntStructure(3)*txAntStructure(4));
//   int elemPerTXRU = totalElmAtTx/txNumOfTxRUs;
//   int elemPerColPerTXRU = txAntStructure(2);// M value of AAS
//   int elemPerRowPerTXRU = floor_i(elemPerTXRU/elemPerColPerTXRU); 
//   vec angleInDegree=find3DAngle(nodeLocationTx,nodeLocationRx,txVerAntElementSpacing,txHorzAntElementSpacing,elemPerColPerTXRU,elemPerColPerTXRU,sectorTilt);
//   thetaTx = angleInDegree(0);
//   phiTx = 90+angleInDegree(1); 
//   
//   // At Receiver
//   totalElmAtTx =  (rxAntStructure(0)*rxAntStructure(1)*rxAntStructure(2)*rxAntStructure(3)*rxAntStructure(4));
//   elemPerTXRU = totalElmAtTx/rxNumOfTxRUs;
//   elemPerColPerTXRU = txAntStructure(2);// M value of AAS
//   elemPerRowPerTXRU = floor_i(elemPerTXRU/elemPerColPerTXRU); 
//   angleInDegree=find3DAngle(nodeLocationRx,nodeLocationTx,rxVerAntElementSpacing,rxHorzAntElementSpacing,elemPerColPerTXRU,elemPerColPerTXRU,sectorTilt);
//   thetaRx = angleInDegree(0);
//   phiRx = 90+angleInDegree(1); 
// }
// 
// void SCM3DLink::generate3DChannelSubClusters(int M)
// {
//     double subdelay1,subdelay2;
//     vec newDelays,tmp1,allDelays,newtapPow3DChannel,Pn_sort;
//     ivec newIndex,SnewIndex,newClusterIndex,clusterSubPathtype,subPathIndices,angleMap,Pn_sort_index,strong2,unsplitindx;
//     int Q, x=-1,tt,n0,n1,s0,s1,cnt;
//     
//     Pn_sort = tapPow3DChannel;
//     Pn_sort_index = sort_index(tapPow3DChannel);
//     strong2 = Pn_sort_index.mid(Pn_sort_index.length()-2,2);
//     if (tapDelays3DChannel(strong2(0))>tapDelays3DChannel(strong2(1)))
//     {
//       tt = strong2(0);
//       strong2(0) = strong2(1);
//       strong2(1) = tt;
//     }
//     n0 = strong2(0); 
//     n1 = strong2(1);
//     
//     unsplitindx.set_length(tapPow3DChannel.length());
//     for (int i = 0; i<unsplitindx.length(); i++)
//     {
//       unsplitindx(i) = i;
//     }
//     unsplitindx.del(n1);
//     unsplitindx.del(n0);
//     
//     Q = 4;
//     newDelays.set_length(Q);
//     subdelay1 = 5e-9;
//     subdelay2 = 10e-9;
//     newDelays(0) = tapDelays3DChannel(n0)+subdelay1;
//     newDelays(1) = tapDelays3DChannel(n0)+subdelay2;
//     newDelays(2) = tapDelays3DChannel(n1)+subdelay1;
//     newDelays(3) = tapDelays3DChannel(n1)+subdelay2;
//     
//     /// Find the locations into which new delays falls in a sorted order
//     newIndex.set_length(newDelays.length());
//     tmp1 = tapDelays3DChannel;
//     for (int m = 0; m<newDelays.length(); m++)
//     {
//       for (int i = 1; i < tmp1.length(); i++) 
//       {
// 	if (newDelays(m) < tmp1(i))
// 	{
// 	  x = i;
// 	  break;
// 	}
// 	else
// 	  x = i+1;
//       }
//       tmp1.ins(x,newDelays(m));
//       newIndex(m) = x;
//       for (int n = 0; n<m; n++)
// 	if(newIndex(m)<=newIndex(n))
// 	  newIndex(n) = newIndex(n)+1;
//     }
//     ///find the index of each of the originial clusters in the new sorted order (of delays)
//       newClusterIndex.set_length(tapPow3DChannel.length());
//       for (int i =0; i<newClusterIndex.length(); i++) 
//       newClusterIndex(i) = i;
//       
//       SnewIndex = newIndex;
//       sort(SnewIndex);
//       for (int i = 0; i<SnewIndex.length(); i++)
//       {
// 	for (int j = 0; j<newClusterIndex.length(); j++)
// 	{
// 	  if (newClusterIndex(j) >= SnewIndex(i)) 
// 	    newClusterIndex(j) = newClusterIndex(j)+1;
// 	}
//       }
//       
//       allDelays = tmp1; 
//       clusterSubPathtype.set_length(tapPow3DChannel.length()+Q);
//       newtapPow3DChannel.set_length(tapPow3DChannel.length()+Q);
//       angleMap.set_length(tapPow3DChannel.length()+Q);
//       
//       
//       subPathIndices = get3DSubPathIndicesofClusters(0);
//       if (subPathIndices.length() != M)
// 	cout << "generate3DChannelSubClusters: num of subpaths in original cluster should be same as set0 indicies" << endl;
//       
//       s0 = n0;
//       s1 = n1;
//       for (int i =0; i<newDelays.length();i++)
// 	if (tapDelays3DChannel(n1)>newDelays(i))
// 	  s1 = s1+1;
// 
//       cnt = 0;
//       
//       for (int i = 0; i < newClusterIndex.length(); i++)
//       {
// 	if (!(newClusterIndex(i) == s0 || newClusterIndex(i) == s1))
// 	{
// 	  clusterSubPathtype(newClusterIndex(i)) = 0; 
// 	  newtapPow3DChannel(newClusterIndex(i)) = tapPow3DChannel(unsplitindx(cnt));
// 	  angleMap(newClusterIndex((i))) = unsplitindx(cnt);
// 	  cnt++;
// 	}
//       }
//       
//       subPathIndices = get3DSubPathIndicesofClusters(1);
//       clusterSubPathtype((s0)) = 1; 
//       clusterSubPathtype((s1)) = 1; 
//       newtapPow3DChannel((s0)) = tapPow3DChannel(n0)*subPathIndices.length()/M;
//       newtapPow3DChannel((s1)) = tapPow3DChannel(n1)*subPathIndices.length()/M;
//       angleMap((s0)) = n0;
//       angleMap((s1)) = n1;
//       
//       subPathIndices = get3DSubPathIndicesofClusters(2); 
//       clusterSubPathtype(newIndex(0)) = 2; 
//       newtapPow3DChannel(newIndex(0)) = tapPow3DChannel(n0)*subPathIndices.length()/M;
//       angleMap(newIndex(0)) = n0;
//       
//       subPathIndices = get3DSubPathIndicesofClusters(3); 
//       clusterSubPathtype(newIndex(3)) = 3;
//       newtapPow3DChannel(newIndex(3)) = tapPow3DChannel(n1)*subPathIndices.length()/M;
//       angleMap(newIndex(3)) = n1;
//       if ( abs(allDelays(newIndex(1)) - (allDelays(s0) + subdelay2)) < abs(allDelays(newIndex(1)) - (allDelays(s1) + subdelay1))) // sub delay2 is 10 ns
//       {
// 	subPathIndices = get3DSubPathIndicesofClusters(3);
// 	clusterSubPathtype(newIndex(1)) = 3;
// 	newtapPow3DChannel(newIndex(1)) = tapPow3DChannel(n0)*subPathIndices.length()/M;
// 	angleMap(newIndex(1)) = n0;
// 	subPathIndices = get3DSubPathIndicesofClusters(2);
// 	clusterSubPathtype(newIndex(2)) = 2;
// 	newtapPow3DChannel(newIndex(2)) = tapPow3DChannel(n1)*subPathIndices.length()/M;
// 	angleMap(newIndex(2)) = n1;
//       }
//       else
//       {
// 	subPathIndices = get3DSubPathIndicesofClusters(2);
// 	clusterSubPathtype(newIndex(1)) = 2;
// 	newtapPow3DChannel(newIndex(1)) = tapPow3DChannel(n1)*subPathIndices.length()/M;
// 	angleMap(newIndex(1)) = n1;
// 	subPathIndices = get3DSubPathIndicesofClusters(3);
// 	clusterSubPathtype(newIndex(2)) = 3;
// 	newtapPow3DChannel(newIndex(2)) = tapPow3DChannel(n0)*subPathIndices.length()/M;
// 	angleMap(newIndex(2)) = n0;
//       }
//       
//       if (abs(sum(tapPow3DChannel) - sum(newtapPow3DChannel)) > 0.0000001 )
//       {
// 	cout << "\n\n\ngenerate3DChannelSubClusters: Error in power calculation while adjusting for subclusters --- exiting program" << endl;
// 	exit(0);
//       }
//       
//       tapPow3DChannel = newtapPow3DChannel;
//       tapDelays3DChannel = allDelays;
//       SubPathtypeinCluster3D = clusterSubPathtype;
//       aMap3D = angleMap;
// }
// 
// ivec SCM3DLink::get3DSubPathIndicesofClusters(int setid)
// {
//   ivec indices;
//   switch (setid)
//   {
//     case 0:
//       indices = "0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19"; 
//       break;
//     case 1:
//       indices = "0 1 2 3 4 5 6 7 18 19";
//       break;
//     case 2:
//       indices = "8 9 10 11 16 17";
//       break;
//     case 3:
//       indices = "12 13 14 15";
//       break;
//     default :
//       cout << "\n\n\nget3DSubPathIndicesofClusters: unknown cluster subpath set id --- exiting program" << endl;
//       exit(0);
//       break;
//   }
//   
//   return indices;
// }
// 
// Array<ivec> SCM3DLink::generate3DSubpathPhases(int N)
// {
//     int cnt;
//     ivec idx;
//     Array<ivec> phases;
//     phases.set_size(N);
//     
//     for (int i = 0; i< N; i++)
//     {
//       idx = get3DSubPathIndicesofClusters(SubPathtypeinCluster3D(i));
//       cnt = 4*idx.length();
//       if (isLOS3DChannel ==1 && i == 0)
// 	cnt = 4*idx.length()+2;
//       phases(i) = floor_i(10*(randu(cnt)*360-180.0)+0.5); 
//     }
//     
//     return phases;
// }
// 
// double SCM3DLink::get3DCrossPolarizationRatio() //! need to confirm since to be generated for each ray m of each cluster n
// {
//   double mu, sigma;
//   
//   switch (propagationScenario3DChannel) // see 3GPP TR 36.814 v 9.0.0, Table B.1.2.2.1-4
//   {   
//     
//     case _SCM3D_URBAN_MICRO_:
//       if (isOutdoorToIndoor3DChannel == 1) { mu = 9.0; sigma = 11.0;}
//       else//Out Door
//       {
// 	if (isLOS3DChannel==1)  { mu = 9.0; sigma = 3.0;}
// 	else		{ mu = 8.0; sigma = 3.0;}
//       }
//       break;
//       
//     case _SCM3D_URBAN_MACRO_:
//       if (isOutdoorToIndoor3DChannel == 1) { mu = 9.0; sigma = 11.0;}
//       else
//       {
// 	if (isLOS3DChannel==1) { mu = 8.0; sigma = 4.0;}
// 	else		{ mu = 7.0; sigma = 3.0;}
//       }
//       break;
//     
//     default :
//       cout << "\n\n\nget3DCrossPolarizationRatio: unknown Propagation scenario -- exiting program" << endl;
//       exit(0);
//       break;
//   }  
//   
//   double sigma_linear = pow(10.0,sigma/10);
//   double mu_linear = pow(10.0,mu/10);   
//   double tmp = sigma_linear*randn() + mu_linear;
//   
//    return pow(10.0,tmp/10); 
// }
// 
// void SCM3DLink::computeBoresight2subpath_3Dangles(int nTaps, int nRays)
// {
//   vec subpathAoDoffsets, subpathAoAoffsets,subpathZoAoffsets,subpathZoDoffsets,tmpAoD,tmpAoA,tmpZoD,tmpZoA;
//   ivec idx;
//   
//   subpath_Boresight_3DAoD_angle.set_size(nTaps);
//   subpath_Boresight_3DAoA_angle.set_size(nTaps);
//   subpath_Boresight_3DZoD_angle.set_size(nTaps);
//   subpath_Boresight_3DZoA_angle.set_size(nTaps);
//   
//   subpathAoDoffsets = generate3DSubpathAngleOffsets(nRays,0); 
//   subpathAoAoffsets = generate3DSubpathAngleOffsets(nRays,1);
//   subpathZoAoffsets = generate3DSubpathAngleOffsets(nRays,2);
//   subpathZoDoffsets = generate3DSubpathAngleOffsets(nRays,3);  
//   
//   
//   
//   
//   for (int i=0; i<nTaps; i++)
//   {
//     idx = get3DSubPathIndicesofClusters(SubPathtypeinCluster3D(i));
//     
//     tmpAoD.set_length(idx.length());
//     tmpAoA.set_length(idx.length());
//     tmpZoD.set_length(idx.length());
//     tmpZoA.set_length(idx.length());
//     
//     for (int j = 0; j<idx.length(); j++)
//     {
//       //for calibration
//       tmpAoD(j) = (double) theta_Tx_3D + pathAoD3DChannel(aMap3D(i)) + subpathAoDoffsets(idx(j));
//       tmpAoA(j) = (double) theta_Rx_3D + pathAoA3DChannel(aMap3D(i)) + subpathAoAoffsets(subpath3DAoA_association_Index(aMap3D(i))(idx(j)));
//       
//       if(isOutdoorToIndoor3DChannel)
//       {
//           tmpZoD(j) = 90 + pathZoD3DChannel(aMap3D(i)) + subpathZoDoffsets(subpath3DZoD_association_Index(aMap3D(i))(idx(j))); //Need to confirm      
//           tmpZoA(j) = 90 + pathZoA3DChannel(aMap3D(i)) + subpathZoAoffsets(subpath3DZoA_association_Index(aMap3D(i))(idx(j)));//Need to confirm
//       }
//       else//OutDoor
//       {
//           tmpZoD(j) = (double) phi_Tx_3D + pathZoD3DChannel(aMap3D(i)) + subpathZoDoffsets(subpath3DZoD_association_Index(aMap3D(i))(idx(j))); //Need to confirm      
//           tmpZoA(j) = (double) phi_Rx_3D + pathZoA3DChannel(aMap3D(i)) + subpathZoAoffsets(subpath3DZoA_association_Index(aMap3D(i))(idx(j)));//Need to confirm
//       }
//       
//     }
//     subpath_Boresight_3DAoD_angle(i) = tmpAoD;
//     subpath_Boresight_3DAoA_angle(i) = tmpAoA;
//     subpath_Boresight_3DZoD_angle(i) = tmpZoD;
//     subpath_Boresight_3DZoA_angle(i) = tmpZoA;  
//     
//     
//     /* Wrapping each of ZoA, ZoD parameters in (0,360) */
//     for(int indx1=0;indx1<subpath_Boresight_3DZoA_angle.length();indx1++)
//       for(int indx2=0;indx2<subpath_Boresight_3DZoA_angle(indx1).length();indx2++)
//       {
// 	if(subpath_Boresight_3DZoA_angle(indx1)(indx2)<0)
// 	    subpath_Boresight_3DZoA_angle(indx1)(indx2) = 360+subpath_Boresight_3DZoA_angle(indx1)(indx2);
// 	else if(subpath_Boresight_3DZoA_angle(indx1)(indx2)>360)
// 	    subpath_Boresight_3DZoA_angle(indx1)(indx2) = subpath_Boresight_3DZoA_angle(indx1)(indx2)-360;
// 		
// 	if(subpath_Boresight_3DZoA_angle(indx1)(indx2)>180)
// 	    subpath_Boresight_3DZoA_angle(indx1)(indx2) = 360-subpath_Boresight_3DZoA_angle(indx1)(indx2);
//       }
//     
//     for(int indx1=0;indx1<subpath_Boresight_3DZoD_angle.length();indx1++)
//       for(int indx2=0;indx2<subpath_Boresight_3DZoD_angle(indx1).length();indx2++)
//       {
// 	if(subpath_Boresight_3DZoD_angle(indx1)(indx2)<0)
// 	    subpath_Boresight_3DZoD_angle(indx1)(indx2) = 360+subpath_Boresight_3DZoD_angle(indx1)(indx2);
// 	else if(subpath_Boresight_3DZoD_angle(indx1)(indx2)>360)
// 	    subpath_Boresight_3DZoD_angle(indx1)(indx2) = subpath_Boresight_3DZoD_angle(indx1)(indx2)-360;
// 		
// 	if(subpath_Boresight_3DZoD_angle(indx1)(indx2)>180)
// 	    subpath_Boresight_3DZoD_angle(indx1)(indx2) = 360-subpath_Boresight_3DZoD_angle(indx1)(indx2);
//       }
// 
//   }
//   
// }
// 
// vec SCM3DLink::generate3DSubpathAngleOffsets(int M,int  isAzimuth_Zenith) // isAzimuth_Zenith: 0=>AoD, 1=>AoA, 2=>ZoA, 3=>ZoD
// {
//     vec offsets;
//     double rmsclusterAzimuthSpread;
//     
//     if (M != 20)
//     {
//       cout << "\n\n\ngenerate3DSubpathAngleOffsets: unsupported number of subpaths M -- exiting program" << endl;
//       cout << "generate3DSubpathAngleOffsets: only M=20 is currently supported -- exiting program" << endl;
//       exit(0);
//     }
//     
//     offsets.set_length(M);
//     
//     offsets(0) = 0.0447; //see 3GPP TR 36.873 V2.0.0 (2014-03), Table 7.3-3
//     offsets(1) = -0.0447;
//     offsets(2) = 0.1413;
//     offsets(3) = -0.1413;
//     offsets(4) = 0.2492;
//     offsets(5) = -0.2492;
//     offsets(6) = 0.3715;
//     offsets(7) = -0.3715;
//     offsets(8) = 0.5129;
//     offsets(9) = -0.5129;
//     offsets(10) = 0.6797;
//     offsets(11) = -0.6797;
//     offsets(12) = 0.8844;
//     offsets(13) = -0.8844;
//     offsets(14) = 1.1481;
//     offsets(15) = -1.1481;
//     offsets(16) = 1.5195;
//     offsets(17) = -1.5195;
//     offsets(18) = 2.1551;
//     offsets(19) = -2.1551;
//     
//     if(isAzimuth_Zenith != 3)
//     {
//       rmsclusterAzimuthSpread = getCluster3DRMSAzimuthSpread(isAzimuth_Zenith);
//       offsets = rmsclusterAzimuthSpread*offsets;
//     }
//     else
//     {
//       double muZSD_linear = pow(10.0,muZSD/10.0);
//       offsets = (3.0/8.0)*pow(10.0,muZSD_linear)*offsets; // Eq(19),3GPP TR 36.873 V2.0.0 (2014-03), Table 7.3-6  
//       
//     }
//     
//     return offsets;
// }
// 
// 
// double SCM3DLink::getCluster3DRMSAzimuthSpread(int isAzimuth_Zenith) // isAzimuth_Zenith: 0=>AoD, 1=>AoA, 2=>ZoA, 3=>ZoD
// {
//   double AS=-1;
//   
//   if (isAzimuth_Zenith == 0)//0=>AoD
//   {
//     switch (propagationScenario3DChannel) //cluster ASD see 3GPP TR 36.873 V2.0.0 (2014-03), Table 7.3-6
//     {
//       case _SCM3D_URBAN_MICRO_:
// 	if (isOutdoorToIndoor3DChannel == 1)AS = 5.0;
// 	else//Out Door
// 	{
// 	  if (isLOS3DChannel==1)	  AS = 3.0;
// 	  else	          AS = 10.0;
// 	}
// 	break;
// 	
//      case _SCM3D_URBAN_MACRO_:
//        if (isOutdoorToIndoor3DChannel == 1)AS = 5.0;
//        else//Out Door
//        {
// 	 if (isLOS3DChannel==1)	  AS = 5.0;
// 	 else	          AS = 2.0;
//        }
//       break;
//       
//       default :
// 	cout << "\n\n\ngetCluster3DRMSAzimuthSpread: unknown Propagation scenario -- exiting program" << endl;
// 	abort();
// 	break;
//     }
//   }
//   else if(isAzimuth_Zenith == 1)//1=>AoA
//   {
//     switch (propagationScenario3DChannel) //cluster ASA see 3GPP TR 36.873 V2.0.0 (2014-03), Table 7.3-6
//     {
//       case _SCM3D_URBAN_MICRO_:
// 	if (isOutdoorToIndoor3DChannel == 1)AS = 8.0;
// 	else//Out Door
// 	{
// 	  if (isLOS3DChannel==1)	  AS = 17.0;
// 	  else	          AS = 22.0;
// 	}
// 	break;
//      case _SCM3D_URBAN_MACRO_:
//        if (isOutdoorToIndoor3DChannel == 1)AS = 8.0;
//        else//Out Door
//        {
// 	 if (isLOS3DChannel==1)	  AS = 11.0;
// 	 else	          AS = 15.0;
//        }
//        break;
//       default :
// 	cout << "\n\n\ngetCluster3DRMSAzimuthSpread: unknown Propagation scenario -- exiting program" << endl;
// 	abort();
// 	break;
//     }
//   }
//  else if(isAzimuth_Zenith == 2)//2=>ZoA
//   {
//     switch (propagationScenario3DChannel) //cluster ZSA see 3GPP TR 36.873 V2.0.0 (2014-03), Table 7.3-6
//     {
//       case _SCM3D_URBAN_MICRO_:
// 	if (isOutdoorToIndoor3DChannel == 1)AS = 3.0;
// 	else//Out Door
// 	{
// 	  if (isLOS3DChannel==1)  AS = 7.0;
// 	  else	          AS = 7.0;
// 	}
// 	break;
//      case _SCM3D_URBAN_MACRO_:
//        if (isOutdoorToIndoor3DChannel == 1)AS = 3.0;
//        else//Out Door
//        {
// 	 if (isLOS3DChannel==1)  AS = 7.0;
// 	 else	          AS = 7.0;
//        }
//       break;
//       default :
// 	cout << "\n\n\ngetCluster3DRMSAzimuthSpread: unknown Propagation scenario -- exiting program" << endl;
// 	abort();
// 	break;
//     }
//   }
//   else
//   {
//     cout<<"[both:]Error.. Invalid isAzimuth_Zenith = "<<isAzimuth_Zenith<<endl;abort();
//   }
//   return AS;  
// }
// 
// void SCM3DLink::compute3DAntennaGains(Antenna &txAntenna, Antenna &rxAntenna){
// 
//   txLos3DAntGain =sqrt(inv_dB(txAntenna.getAntennaGain(theta_Tx_3D, phi_Tx_3D)));
//   rxLos3DAntGain =sqrt(inv_dB(rxAntenna.getAntennaGain(theta_Rx_3D, phi_Rx_3D)));
//   //txLos3DAntGain=1;
// //   cout << "txLos3DAntGain" << txLos3DAntGain << endl;
// //   cout << "rxLos3DAntGain" << rxLos3DAntGain << endl;abort();
//   
//   if(addPathSpecificAntennaGain3D)
//   {  
//     subpathAoDZoDAntennaGain.set_length(subpath_Boresight_3DAoD_angle.length());
//     for(int cnt1=0;cnt1<subpath_Boresight_3DAoD_angle.length();cnt1++)
//     {
//       subpathAoDZoDAntennaGain(cnt1).set_length(subpath_Boresight_3DAoD_angle(cnt1).length());
//       for(int cnt2=0;cnt2<subpath_Boresight_3DAoD_angle(cnt1).length();cnt2++)
// 	subpathAoDZoDAntennaGain(cnt1)(cnt2)=sqrt(inv_dB(txAntenna.getAntennaGain(subpath_Boresight_3DAoD_angle(cnt1)(cnt2),subpath_Boresight_3DZoD_angle(cnt1)(cnt2))));
//     }   
//     
//     subpathAoAZoAAntennaGain.set_length(subpath_Boresight_3DAoA_angle.length());
//     for(int cnt1=0;cnt1<subpath_Boresight_3DAoA_angle.length();cnt1++)
//     {
//       subpathAoAZoAAntennaGain(cnt1).set_length(subpath_Boresight_3DAoA_angle(cnt1).length());
//       for(int cnt2=0;cnt2<subpath_Boresight_3DAoA_angle(cnt1).length();cnt2++)
// 	subpathAoAZoAAntennaGain(cnt1)(cnt2)=sqrt(inv_dB(rxAntenna.getAntennaGain(subpath_Boresight_3DAoA_angle(cnt1)(cnt2),subpath_Boresight_3DZoA_angle(cnt1)(cnt2))));
//     }
//     
//     vec avgGains = getAvgAntennaGains(subpathAoDZoDAntennaGain,subpathAoAZoAAntennaGain,tapPow3DChannel,isLOS3DChannel,K_rice_3D);
//     txAvg3DAntGain = avgGains(0),rxAvg3DAntGain = avgGains(1);
//   }
//  
// }
// 
// cvec SCM3DLink::generate3DChannelTapGains(int currentTxAntenna,int currentTxAntennaElement, /*int physicalAntNum*/ int currentRxAntenna,int currentRxAntennaElement,double currentTimeInSeconds,cvec elevationWeights_temp)
// {
//   
//   if(!isReadyFor3DChannel)
//   {
//     cout<<"[both:] 3DChannelLink not initialized ... Please call initializeFadingParams() before calling generateTapGains()..."<<endl;
//     abort();
//   }
//   
//   double GTx_hpol, GTx_vpol, GRx_hpol,GRx_vpol,K,deg2rad;
//   int isVpolar,M;
//   cmat F_rx,F_tx,Rot,tmp;
//   mat r_tx,r_rx;
//   cvec tapGain;
//   ivec idx;
//   double c1,c2,c3,pvv,phv,pvh,phh;
//   double lamda=lightSpeed/carrierFrequencyInHz3DChannel;
//   double mobileVelocity3DChannel=dopplerSpreadInHz3DChannel*lamda;
//  
//   
//   F_rx.set_size(1,2);
//   F_tx.set_size(2,1);
//   Rot.set_size(2,2);  
//   
//   r_tx.set_size(3,1);
//   r_rx.set_size(3,1);
//    
//   
//   K = 2*pi/lamda;
//   deg2rad = pi/180; 
//   tapGain.set_length(tapPow3DChannel.length());
//   tapGain.zeros();
//   for (int i=0; i<tapPow3DChannel.length(); i++)
//   {
//     idx = get3DSubPathIndicesofClusters(SubPathtypeinCluster3D(i));
//     M = idx.length();
//     for (int j = 0; j<M; j++)
//     {
//       if(addPathSpecificAntennaGain3D)
//       {
//           GTx_vpol = subpathAoDZoDAntennaGain(i)(j)*sin(deg2rad*txPolarizationAngleInDegree3D(currentTxAntenna));
//           GTx_hpol = subpathAoDZoDAntennaGain(i)(j)*cos(deg2rad*txPolarizationAngleInDegree3D(currentTxAntenna));
// 
//           GRx_vpol = subpathAoAZoAAntennaGain(i)(j)*sin(deg2rad*rxPolarizationAngleInDegree3D(currentRxAntenna));
//           GRx_hpol = subpathAoAZoAAntennaGain(i)(j)*cos(deg2rad*rxPolarizationAngleInDegree3D(currentRxAntenna));
//       }
//       else
//       {
//           GTx_vpol = txLos3DAntGain*sin(deg2rad*txPolarizationAngleInDegree3D(currentTxAntenna));
//           GTx_hpol = txLos3DAntGain*cos(deg2rad*txPolarizationAngleInDegree3D(currentTxAntenna));
// 
//           GRx_vpol = rxLos3DAntGain*sin(deg2rad*rxPolarizationAngleInDegree3D(currentRxAntenna));
//           GRx_hpol = rxLos3DAntGain*cos(deg2rad*rxPolarizationAngleInDegree3D(currentRxAntenna));
//       }
//       
//       r_rx = getCartesianVector(subpath_Boresight_3DAoA_angle(i)(j),subpath_Boresight_3DZoA_angle(i)(j));
//       r_tx = getCartesianVector(subpath_Boresight_3DAoD_angle(i)(j),subpath_Boresight_3DZoD_angle(i)(j));
//       
//       mat velocityUnitVector = getCartesianVector(h_phase_3D,v_phase_3D);
//       
//       mat temp1;
//       temp1.set_size(1,1);
//       temp1=transpose(r_rx)*velocityUnitVector;      
//      // v_phase_3D = mobileVelocity3DChannel*temp1(0,0);//Doppler frequency component Eq(24) 3GPP TR 36.873 V2.0.0 (2014-03)   
//       
// 
//       mat txLocationVector;
//       mat rxLocationVector;
//       txLocationVector.set_size(3,1); 
//       rxLocationVector.set_size(3,1); 
//       txLocationVector(0,0)=0.0;
// //       txLocationVector(1,0)=txAntElementIndxFor3D(physicalAntNum)(1)*horzAntElementSpacing;
// //       txLocationVector(2,0)=txHeight+txAntElementIndxFor3D(physicalAntNum)(0)*verAntElementSpacing;
// //       rxLocationVector(0,0)=0.0;
// //       rxLocationVector(1,0)=rxAntElementIndxFor3D(currentRxAntenna)(1)*horzAntElementSpacing;
// //       rxLocationVector(2,0)=rxHeight+rxAntElementIndxFor3D(currentRxAntenna)(0)*verAntElementSpacing;
//      
//       int elemRowIndxInTXRU = currentTxAntennaElement%txAntStructure(2); // M of antenna panal for Tx
//       int elemColIndxInTXRU = floor(currentTxAntennaElement/txAntStructure(2));
//       
//       int tempIndexCol = txAntElementIndxFor3D(currentTxAntenna)(1); //physicalAntNum
//       int tempIndexRow = txAntElementIndxFor3D(currentTxAntenna)(0);
//       int panalCntHorz = floor( tempIndexCol/txAntStructure(3));
//       int  panalCntVer = floor( tempIndexRow/txAntStructure(2));
//       txLocationVector(1,0)=(tempIndexCol+elemColIndxInTXRU-panalCntHorz)*txHorzAntElementSpacing + panalCntHorz*txHorzAntPanalSpacing;
//       txLocationVector(2,0)=txHeight+(tempIndexRow+elemRowIndxInTXRU-panalCntVer)*txVerAntElementSpacing + panalCntVer*txVerAntElementSpacing;
//       
//       elemRowIndxInTXRU = currentRxAntennaElement%rxAntStructure(2); // M of antenna panal for Rx
//       elemColIndxInTXRU = floor(currentRxAntennaElement/rxAntStructure(2));
//       tempIndexCol = rxAntElementIndxFor3D(currentRxAntenna)(1);
//       tempIndexRow = rxAntElementIndxFor3D(currentRxAntenna)(0);
//       panalCntHorz = floor( tempIndexCol/rxAntStructure(3));
//       panalCntVer = floor( tempIndexRow/rxAntStructure(2));
//       
//       rxLocationVector(0,0)=0.0;
//       rxLocationVector(1,0)=(tempIndexCol+elemColIndxInTXRU-panalCntHorz)*rxHorzAntElementSpacing + panalCntHorz*rxHorzAntPanalSpacing;
//       rxLocationVector(2,0)=rxHeight+(tempIndexRow+elemRowIndxInTXRU-panalCntVer)*rxVerAntElementSpacing + panalCntVer*rxVerAntElementSpacing;
//       
//       temp1=transpose(r_rx)*rxLocationVector;            
//       
//       c1 = K*temp1(0,0);
//       temp1=transpose(r_tx)*txLocationVector;
//       c2= K*temp1(0,0);
//       c3 = K*mobileVelocity3DChannel*temp1(0,0)*currentTimeInSeconds;
//       
//       pvv = deg2rad*subpathPhase_3Dfixedpt(i)(4*j)/10; 
//       pvh = deg2rad*subpathPhase_3Dfixedpt(i)(4*j+1)/10;
//       phv = deg2rad*subpathPhase_3Dfixedpt(i)(4*j+2)/10;
//       phh = deg2rad*subpathPhase_3Dfixedpt(i)(4*j+3)/10;
//       
//       F_rx(0,0) = GRx_vpol;
//       F_rx(0,1) = GRx_hpol;
//       F_tx(0,0) = GTx_vpol;
//       F_tx(1,0) = GTx_hpol;      
//       
//       Rot(0,0) = dComplex(cos(pvv),sin(pvv));
//       Rot(0,1) = sqrt(1/XPR_lin_3D)*dComplex(cos(pvh),sin(pvh));      
//       Rot(1,0) = sqrt(1/XPR_lin_3D)*dComplex(cos(phv),sin(phv));
//       Rot(1,1) = dComplex(cos(phh),sin(phh));     
//       
//       tmp = F_rx*Rot*F_tx;  
//       tapGain(i) +=elevationWeights_temp(0)*sqrt(tapPow3DChannel(i)/M)*tmp(0,0)*dComplex(cos(c1),sin(c1))*dComplex(cos(c2),sin(c2))*dComplex(cos(c3),sin(c3));
//     }
//   }
//   
//   
//   if (isLOS3DChannel == 1)
//   {
//     GTx_vpol = txLos3DAntGain*sin(deg2rad*txPolarizationAngleInDegree3D(currentTxAntenna));
//     GTx_hpol = txLos3DAntGain*cos(deg2rad*txPolarizationAngleInDegree3D(currentTxAntenna));
// 
//     GRx_vpol = rxLos3DAntGain*sin(deg2rad*rxPolarizationAngleInDegree3D(currentRxAntenna));
//     GRx_hpol = rxLos3DAntGain*cos(deg2rad*rxPolarizationAngleInDegree3D(currentRxAntenna));
//     
//     r_rx = getCartesianVector(subpath_Boresight_3DAoA_angle(0)(0),subpath_Boresight_3DZoA_angle(0)(0));
//     r_tx = getCartesianVector(subpath_Boresight_3DAoD_angle(0)(0),subpath_Boresight_3DZoD_angle(0)(0));
//     
//     mat velocityUnitVector = getCartesianVector(h_phase_3D,v_phase_3D);
//     
//     mat temp1;
//     temp1.set_size(1,1);
//     temp1=transpose(r_rx)*velocityUnitVector;      
//     //v_phase_3D = mobileVelocity3DChannel*temp1(0,0);//Doppler frequency component Eq(24) 3GPP TR 36.873 V2.0.0 (2014-03)
//     
//     
//     mat txLocationVector;
//     mat rxLocationVector;
//     txLocationVector.set_size(3,1); 
//     rxLocationVector.set_size(3,1); 
//     txLocationVector(0,0)=0.0;
//     int elemRowIndxInTXRU = currentTxAntennaElement%txAntStructure(2); // M of antenna panal for Tx
//     int elemColIndxInTXRU = floor(currentTxAntennaElement/txAntStructure(2));
//     
//     int tempIndexCol = txAntElementIndxFor3D(currentTxAntenna)(1); //physicalAntNum
//     int tempIndexRow = txAntElementIndxFor3D(currentTxAntenna)(0);
//     int panalCntHorz = floor( tempIndexCol/txAntStructure(3));
//     int  panalCntVer = floor( tempIndexRow/txAntStructure(2));
//     txLocationVector(1,0)=(tempIndexCol+elemColIndxInTXRU-panalCntHorz)*txHorzAntElementSpacing + panalCntHorz*txHorzAntPanalSpacing;
//     txLocationVector(2,0)=txHeight+(tempIndexRow+elemRowIndxInTXRU-panalCntVer)*txVerAntElementSpacing + panalCntVer*txVerAntElementSpacing;
//     
//     elemRowIndxInTXRU = currentRxAntennaElement%rxAntStructure(2); // M of antenna panal for Rx
//     elemColIndxInTXRU = floor(currentRxAntennaElement/rxAntStructure(2));
//     tempIndexCol = rxAntElementIndxFor3D(currentRxAntenna)(1);
//     tempIndexRow = rxAntElementIndxFor3D(currentRxAntenna)(0);
//     panalCntHorz = floor( tempIndexCol/rxAntStructure(3));
//     panalCntVer = floor( tempIndexRow/rxAntStructure(2));
//     
//     temp1=transpose(r_rx)*rxLocationVector;  
//     c1 = K*temp1(0,0);
//     temp1=transpose(r_tx)*txLocationVector;
//     c2= K*temp1(0,0);
//     c3 = K*mobileVelocity3DChannel*temp1(0,0)*currentTimeInSeconds;    
//     
//     
//     M = subpathPhase_3Dfixedpt.length();
//     pvv = deg2rad*subpathPhase_3Dfixedpt(0)(M-2)/10;
//     phh = deg2rad*subpathPhase_3Dfixedpt(0)(M-1)/10;
//     
//     F_rx(0,0) = GRx_vpol;
//     F_rx(0,1) = GRx_hpol;
//     F_tx(0,0) = GTx_vpol;
//     F_tx(1,0) = GTx_hpol;
//     
//     Rot.zeros();
//     Rot(0,0) = dComplex(cos(pvv),sin(pvv));
//     Rot(1,1) = dComplex(cos(phh),sin(phh));
//     
//     tmp = F_rx*Rot*F_tx;     
//     tapGain = sqrt(1/(K_rice_3D+1.0))*tapGain;
//     tapGain(0) += sqrt(K_rice_3D/(K_rice_3D+1.0))*tmp(0,0)*dComplex(cos(c1),sin(c1))*dComplex(cos(c2),sin(c2))*dComplex(cos(c3),sin(c3));
//     
//   }    
//   return tapGain;
// }
// 
// 
// vec SCM3DLink::get3DChannelTapDelays(){
//   
//   if(!isReadyFor3DChannel)
//   {
//     cout<<"Please initialize 3D-Channel Link before calling getTapDelays()..."<<endl;
//     return(vec(""));
//   }
//   return(tapDelays3DChannel);
// }
// 
// double SCM3DLink::get3DChannelLOSAntennaGainInDB(){
//   if(!isReadyFor3DChannel)
//   {
//     cout<<"Please initialize 3D-Channel Link before calling getTapDelays()..."<<endl;
//     abort();
//   }
//    return(dB(sqr(txLos3DAntGain*rxLos3DAntGain))); 
// }
// 
// // double SCM3DLinkAAS::initializeAASChannelFadingParams(int  isOutdoortoIndoor,bool isLineOfSight,double dopplerSpread,double carrierFrequency,SCMPropagationScenario propScenario,SCMLargeScaleParameters_S &channel3DParameters,Location_S nodeLocationTx, Location_S nodeLocationRx,Antenna &txAntenna, Antenna &rxAntenna ,double txAntSpacing,double rxAntSpacing)
// // {
// //   txNumberOfTxRUs=txAntenna.numTxRUs;
// //   rxNumberOfTxRUs=rxAntenna.numTxRUs;
// //   txAntStructure =txAntenna.antStructure;
// //   rxAntStructure = rxAntenna.antStructure;
// //   totalTxAntElem = txAntenna.antElementCount;
// //   totalRxAntElem = rxAntenna.antElementCount;
// // 
// //   //   /*TxRUToAntMapping=txAntenna.TxRUToAntMapping*/;
// //   
// //    return( initialize3DChannelFadingParams(isOutdoortoIndoor,isLineOfSight,dopplerSpread,carrierFrequency,propScenario,channel3DParameters,nodeLocationTx,nodeLocationRx,txAntenna,rxAntenna,txAntSpacing,rxAntSpacing));
// // 
// // }
// 
// 
// 
// cvec SCM3DLink::generateAASChannelTapGains(int currentTxAntenna, int currentRxAntenna,double currentTimeInSeconds) 
// {
//   cvec effectiveTapWeights;
// 
//   //AAS
//   int elementsPerTXRU = totalTxAntElem/txNumberOfTxRUs; 
//   ivec physicalTxAntennaElemNums=getIntegers(0,elementsPerTXRU-1,1);
//   cvec elevationWeights_temp;
//   int elementsPerRxTXRU = totalRxAntElem/rxNumberOfTxRUs; 
//   ivec physicalRxAntennaElemNums = getIntegers(0,elementsPerRxTXRU-1,1);
//   elevationWeights_temp.set_length(1);
//   elevationWeights_temp(0) = antWeights(0);
//   
//   effectiveTapWeights = generate3DChannelTapGains(currentTxAntenna,physicalTxAntennaElemNums(0)/*,physicalAntennaNums(0)*/,currentRxAntenna,physicalRxAntennaElemNums(0),currentTimeInSeconds,elevationWeights_temp);
//   for(int i=1;i<physicalTxAntennaElemNums.length();i++)
//     for(int j=1;j<physicalRxAntennaElemNums.length();j++)
//     {
//       elevationWeights_temp(0)=antWeights(i);
//       //FIXME did not add Rx beamforming gain weight need to be added later
//       effectiveTapWeights += generate3DChannelTapGains(currentTxAntenna,physicalTxAntennaElemNums(i)/*,physicalAntennaNums(0)*/,currentRxAntenna,physicalRxAntennaElemNums(j),currentTimeInSeconds,elevationWeights_temp);
//     }
//     effectiveTapWeights/=(dComplex(sqrt(physicalTxAntennaElemNums.length()),0)*dComplex(sqrt(physicalRxAntennaElemNums.length()),0));
//   //cout << "hn(" << floor_i((currentTimeInSeconds/1e-3)+1) << ",:) =" << effectiveTapWeights << endl;
//   return effectiveTapWeights;
// }
// 
// 
