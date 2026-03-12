/*************************************************************************
 * 
 * CEWiT CONFIDENTIAL
 * __________________
 *
 * All Rights Reserved © 2014 CEWiT, India
 *
 * \ NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
 * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
 * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
 * express, printed and signed license for use is strictly forbidden.
 */

#include "../include/srsTx.h"

mat phi_6 = ("-3    -1     3     3    -1    -3;"
    "-3     3    -1    -1     3    -3;"
    "-3    -3    -3     3     1    -3;"
     "1     1     1     3    -1    -3;"
     "1     1     1    -3    -1     3;"
    "-3     1    -1    -3    -3    -3;"
    "-3     1     3    -3    -3    -3;"
    "-3    -1     1    -3     1    -1;"
    "-3    -1    -3     1    -3    -3;"
    "-3    -3     1    -3     3    -3;"
    "-3     1     3     1    -3    -3;"
    "-3    -1    -3     1     1    -3;"
     "1     1     3    -1    -3     3;"
     "1     1     3     3    -1     3;"
     "1     1     1    -3     3    -1;"
     "1     1     1    -1     3    -3;"
    "-3    -1    -1    -1     3    -1;"
    "-3    -3    -1     1    -1    -3;"
    "-3    -3    -3     1    -3    -1;"
    "-3     1     1    -3    -1    -3;"
    "-3     3    -3     1     1    -3;"
    "-3     1    -3    -3    -3    -1;"
     "1     1    -3     3     1     3;"
     "1     1    -3    -3     1    -3;"
     "1     1     3    -1     3     3;"
     "1     1    -3     1     3     3;"
     "1     1    -1    -1     3    -1;"
     "1     1    -1     3    -1    -1;"
     "1     1    -1     3    -3    -1;"
     "1     1    -3     1    -1    -1"
);

mat phi_12 = ("-3     1    -3    -3    -3     3    -3    -1     1     1     1    -3;"
    "-3     3     1    -3     1     3    -1    -1     1     3     3     3;"
    "-3     3     3     1    -3     3    -1     1     3    -3     3    -3;"
    "-3    -3    -1     3     3     3    -3     3    -3     1    -1    -3;"
    "-3    -1    -1     1     3     1     1    -1     1    -1    -3     1;"
    "-3    -3     3     1    -3    -3    -3    -1     3    -1     1     3;"
     "1    -1     3    -1    -1    -1    -3    -1     1     1     1    -3;"
    "-1    -3     3    -1    -3    -3    -3    -1     1    -1     1    -3;"
    "-3    -1     3     1    -3    -1    -3     3     1     3     3     1;"
    "-3    -1    -1    -3    -3    -1    -3     3     1     3    -1    -3;"
    "-3     3    -3     3     3    -3    -1    -1     3     3     1    -3;"
    "-3    -1    -3    -1    -1    -3     3     3    -1    -1     1    -3;"
    "-3    -1     3    -3    -3    -1    -3     1    -1    -3     3     3;"
    "-3     1    -1    -1     3     3    -3    -1    -1    -3    -1    -3;"
     "1     3    -3     1     3     3     3     1    -1     1    -1     3;"
    "-3     1     3    -1    -1    -3    -3    -1    -1     3     1    -3;"
    "-1    -1    -1    -1     1    -3    -1     3     3    -1    -3     1;"
    "-1     1     1    -1     1     3     3    -1    -1    -3     1    -3;"
    "-3     1     3     3    -1    -1    -3     3     3    -3     3    -3;"
    "-3    -3     3    -3    -1     3     3     3    -1    -3     1    -3;"
     "3     1     3     1     3    -3    -1     1     3     1    -1    -3;"
    "-3     3     1     3    -3     1     1     1     1     3    -3     3;"
    "-3     3     3     3    -1    -3    -3    -1    -3     1     3    -3;"
     "3    -1    -3     3    -3    -1     3     3     3    -3    -1    -3;"
    "-3    -1     1    -3     1     3     3     3    -1    -3     3     3;"
    "-3     3     1    -1     3     3    -3     1    -1     1    -1     1;"
    "-1     1     3    -3     1    -1     1    -1    -1    -3     1    -1;"
    "-3    -3     3     3     3    -3    -1     1    -3     3     1    -3;"
     "1    -1     3     1     1    -1    -1    -1     1     3    -3     1;"
    "-3     3    -3     3    -3    -3     3    -1    -1     1     3    -3"
);

mat phi_18 = ("-1     3    -1    -3     3     1    -3    -1     3    -3    -1    -1     1     1     1    -1    -1    -1;"
     "3    -3     3    -1     1     3    -3    -1    -3    -3    -1    -3     3     1    -1     3    -3     3;"
    "-3     3     1    -1    -1     3    -3    -1     1     1     1     1     1    -1     3    -1    -3    -1;"
    "-3    -3     3     3     3     1    -3     1     3     3     1    -3    -3     3    -1    -3    -1     1;"
     "1     1    -1    -1    -3    -1     1    -3    -3    -3     1    -3    -1    -1     1    -1     3     1;"
     "3    -3     1     1     3    -1     1    -1    -1    -3     1     1    -1     3     3    -3     3    -1;"
    "-3     3    -1     1     3     1    -3    -1     1     1    -3     1     3     3    -1    -3    -3    -3;"
     "1     1    -3     3     3     1     3    -3     3    -1     1     1    -1     1    -3    -3    -1     3;"
    "-3     1    -3    -3     1    -3    -3     3     1    -3    -1    -3    -3    -3    -1     1     1     3;"
     "3    -1     3     1    -3    -3    -1     1    -3    -3     3     3     3     1     3    -3     3    -3;"
    "-3    -3    -3     1    -3     3     1     1     3    -3    -3     1     3    -1     3    -3    -3     3;"
    "-3    -3     3     3     3    -1    -1    -3    -1    -1    -1     3     1    -3    -3    -1     3    -1;"
    "-3    -1    -3    -3     1     1    -1    -3    -1    -3    -1    -1     3     3    -1     3     1     3;"
     "1     1    -3    -3    -3    -3     1     3    -3     3     3     1    -3    -1     3    -1    -3     1;"
    "-3     3    -1    -3    -1    -3     1     1    -3    -3    -1    -1     3    -3     1     3     1     1;"
     "3     1    -3     1    -3     3     3    -1    -3    -3    -1    -3    -3     3    -3    -1     1     3;"
    "-3    -1    -3    -1    -3     1     3    -3    -1     3     3     3     1    -1    -3     3    -1    -3;"
    "-3    -1     3     3    -1     3    -1    -3    -1     1    -1    -3    -1    -1    -1     3     3     1;"
    "-3     1    -3    -1    -1     3     1    -3    -3    -3    -1    -3    -3     1     1     1    -1    -1;"
     "3     3     3    -3    -1    -3    -1     3    -1     1    -1    -3     1    -3    -3    -1     3     3;"
    "-3     1     1    -3     1     1     3    -3    -1    -3    -1     3    -3     3    -1    -1    -1    -3;"
     "1    -3    -1    -3     3     3    -1    -3     1    -3    -3    -1    -3    -1     1     3     3     3;"
    "-3    -3     1    -1    -1     1     1    -3    -1     3     3     3     3    -1     3     1     3     1;"
     "3    -1    -3     1    -3    -3    -3     3     3    -1     1    -3    -1     3     1     1     3     3;"
     "3    -1    -1     1    -3    -1    -3    -1    -3    -3    -1    -3     1     1     1    -3    -3     3;"
    "-3    -3     1    -3     3     3     3    -1     3     1     1    -3    -3    -3     3    -3    -1    -1;"
    "-3    -1    -1    -3     1    -3     3    -1    -1    -3     3     3    -3    -1     3    -1    -1    -1;"
    "-3    -3     3     3    -3     1     3    -1    -3     1    -1    -3     3    -3    -1    -1    -1     3;"
    "-1    -3     1    -3    -3    -3     1     1     3     3    -3     3     3    -3    -1     3    -3     1;"
    "-3     3     1    -1    -1    -1    -1     1    -1     3     3    -3    -1     1     3    -1     3    -1"
);

mat phi_24 = ("-1    -3     3    -1     3     1     3    -1     1    -3    -1    -3    -1     1     3    -3    -1    -3     3     3     3    -3    -3    -3;"
    "-1    -3     3     1     1    -3     1    -3    -3     1    -3    -1    -1     3    -3     3     3     3    -3     1     3     3    -3    -3;"
    "-1    -3    -3     1    -1    -1    -3     1     3    -1    -3    -1    -1    -3     1     1     3     1    -3    -1    -1     3    -3    -3;"
     "1    -3     3    -1    -3    -1     3     3     1    -1     1     1     3    -3    -1    -3    -3    -3    -1     3    -3    -1    -3    -3;"
    "-1     3    -3    -3    -1     3    -1    -1     1     3     1     3    -1    -1    -3     1     3     1    -1    -3     1    -1    -3    -3;"
    "-3    -1     1    -3    -3     1     1    -3     3    -1    -1    -3     1     3     1    -1    -3    -1    -3     1    -3    -3    -3    -3;"
    "-3     3     1     3    -1     1    -3     1    -3     1    -1    -3    -1    -3    -3    -3    -3    -1    -1    -1     1     1    -3    -3;"
    "-3     1     3    -1     1    -1     3    -3     3    -1    -3    -1    -3     3    -1    -1    -1    -3    -1    -1    -3     3     3    -3;"
    "-3     1    -3     3    -1    -1    -1    -3     3     1    -1    -3    -1     1     3    -1     1    -1     1    -3    -3    -3    -3    -3;"
     "1     1    -1    -3    -1     1     1    -3     1    -1     1    -3     3    -3    -3     3    -1    -3     1     3    -3     1    -3    -3;"
    "-3    -3    -3    -1     3    -3     3     1     3     1    -3    -1    -1    -3     1     1     3     1    -1    -3     3     1     3    -3;"
    "-3     3    -1     3     1    -1    -1    -1     3     3     1     1     1     3     3     1    -3    -3    -1     1    -3     1     3    -3;"
     "3    -3     3    -1    -3     1     3     1    -1    -1    -3    -1     3    -3     3    -1    -1     3     3    -3    -3     3    -3    -3;"
    "-3     3    -1     3    -1     3     3     1     1    -3     1     3    -3     3    -3    -3    -1     1     3    -3    -1    -1    -3    -3;"
    "-3     1    -3    -1    -1     3     1     3    -3     1    -1     3     3    -1    -3     3    -3    -1    -1    -3    -3    -3     3    -3;"
    "-3    -1    -1    -3     1    -3    -3    -1    -1     3    -1     1    -1     3     1    -3    -1     3     1     1    -1    -1    -3    -3;"
    "-3    -3     1    -1     3     3    -3    -1     1    -1    -1     1     1    -1    -1     3    -3     1    -3     1    -1    -1    -1    -3;"
     "3    -1     3    -1     1    -3     1     1    -3    -3     3    -3    -1    -1    -1    -1    -1    -3    -3    -1     1     1    -3    -3;"
    "-3     1    -3     1    -3    -3     1    -3     1    -3    -3    -3    -3    -3     1    -3    -3     1     1    -3     1     1    -3    -3;"
    "-3    -3     3     3     1    -1    -1    -1     1    -3    -1     1    -1     3    -3    -1    -3    -1    -1     1    -3     3    -1    -3;"
    "-3    -3    -1    -1    -1    -3     1    -1    -3    -1     3    -3     1    -3     3    -3     3     3     1    -1    -1     1    -3    -3;"
     "3    -1     1    -1     3    -3     1     1     3    -1    -3     3     1    -3     3    -1    -1    -1    -1     1    -3    -3    -3    -3;"
    "-3     1    -3     3    -3     1    -3     3     1    -1    -3    -1    -3    -3    -3    -3     1     3    -1     1     3     3     3    -3;"
    "-3    -1     1    -3    -1    -1     1     1     1     3     3    -1     1    -1     1    -1    -1    -3    -3    -3     3     1    -1    -3;"
    "-3     3    -1    -3    -1    -1    -1     3    -1    -1     3    -3    -1     3    -3     3    -3    -1     3     1     1    -1    -3    -3;"
    "-3     1    -1    -3    -3    -1     1    -3    -1    -3     1     1    -1     1     1     3     3     3    -1     1    -1     1    -1    -3;"
    "-1     3    -1    -1     3     3    -1    -1    -1     3    -1    -3     1     3     1     1    -3    -3    -3    -1    -3    -1    -3    -3;"
     "3    -3    -3    -1     3     3    -3    -1     3     1     1     1     3    -1     3    -3    -1     3    -1     3     1    -1    -3    -3;"
    "-3     1    -3     1    -3     1     1     3     1    -3    -3    -1     1     3    -1    -3     3     1    -1    -3    -3    -3    -3    -3;"
     "3    -3    -1     1     3    -1    -1    -3    -1     3    -1    -3    -1    -3     3    -1     3     1     1    -3     3    -3    -3    -3"
);


double getSRSUplinkPower(double PCmax, float alpha, double pathloss, srsConfig srsconfig){
 
  double pTemp;
  pTemp = srsconfig.p0dBm + 10*log10(srsconfig.nRBs) + alpha*pathloss;
  pTemp = min(pTemp,PCmax);
  pTemp = pow(10,(pTemp-30)/10);
  
  return pTemp;
}


Array<srsConfig> generateSRSconfig(AssociationInfo_S associationInfo, int nSRSports)    //FDM scheme
{
  ivec UE = associationInfo.serviceNodes;
  ivec gNB = associationInfo.serverNodes;
  Array<srsConfig> srsconfig(UE.size());
  int groupIndx = 0;
  int seqIndx = 0;
  ivec symbols;
  symbols = getIntegers(10,13);
  int UEindx = 0;
  for(int gNBindx = 0;gNBindx < gNB.size();gNBindx++)
  {
    ivec assoc_UEs;
    assoc_UEs = associationInfo.associatedServiceNodesPerServerNode(gNB(gNBindx));
    ivec assoc_UEIndices = find(associationInfo.serviceNodes,assoc_UEs);
    for(int aUEindx = 0;aUEindx < assoc_UEs.size();aUEindx++)
    {
      srsconfig(UEindx).UEid = assoc_UEs(aUEindx);
      srsconfig(UEindx).nSRSports = nSRSports;
      srsconfig(UEindx).nRBs = 12;
      srsconfig(UEindx).K_TC = 4;
      srsconfig(UEindx).symbols = symbols;
      srsconfig(UEindx).rbOffset = ((aUEindx%8)/2)*srsconfig(aUEindx).nRBs;
      srsconfig(UEindx).combOffset = ((aUEindx%8)%2);
      srsconfig(UEindx).cyclicShift = aUEindx/8;
      if(srsconfig(UEindx).cyclicShift > 2){cout<<endl<<"Error: cyclic shift offset of SRS config exceeding limit";abort();}
      srsconfig(UEindx).u = gNBindx%30;
      srsconfig(UEindx).v = 0;
      srsconfig(UEindx).seqLength = srsconfig(UEindx).nRBs*12/srsconfig(UEindx).K_TC;
      srsconfig(UEindx).periodicity = 1;
      srsconfig(UEindx).offset = 0;
      srsconfig(UEindx).p0dBm = -76;
      
      // Calculating SRS Tx power
      sBeamPairLink beamPairLink = associationInfo.associatedBeamPairLinkforEachServiceNode(assoc_UEIndices(aUEindx));
      double pathloss = beamPairLink.rspIndBm - beamPairLink.rsrpIndBm;
      srsconfig(UEindx).srsTXpowerInWatts = getSRSUplinkPower(23,0.8,pathloss,srsconfig(UEindx));
          
      UEindx++;
    }
  }
  
  return srsconfig;
}

srsConfig getUEsrsConfig(Array<srsConfig> srsconfig, int UE){
  srsConfig srsCon;
  int flag = 1;
  if(srsconfig.size()!=0)
  {
    for(int indx = 0;indx < srsconfig.size();indx++){
      if(srsconfig(indx).UEid == UE){return srsconfig(indx);}
    }
    cout<<"Error in  getUEsrsConfig()"<<endl;
    abort();
  }
  else
  {
          cout<<"Error in  getUEsrsConfig()"<<endl;
          abort();
      } 
  
}

cmat generateZCseq(srsConfig srs){
  int M_ZC = 36;
  int n_SRS_cs_max;
  int p_i = 1000;
  if(srs.K_TC == 2){n_SRS_cs_max = 8;}
  else if(srs.K_TC == 4){n_SRS_cs_max = 12;}
  else{cout<<"Abort: Invalid K_TC Value"<<endl;abort();}
  cmat zcSeq(srs.nSRSports, srs.seqLength);
  if(srs.seqLength == 6){
    for(int antIndx = 0;antIndx < srs.nSRSports; antIndx++){
      int n_SRS_CS_i = (srs.cyclicShift+(n_SRS_cs_max*(p_i-1000))/srs.nSRSports)%n_SRS_cs_max;
      double alpha_i = 2*pi*n_SRS_CS_i/n_SRS_cs_max;
      for(int i = 0;i < srs.seqLength;i++){
	zcSeq(antIndx,i) = expj(alpha_i*i,1)*expj(phi_6(srs.u,i),1);
      }
      p_i = p_i+1;
    }
  }
  else if(srs.seqLength == 12){
    for(int antIndx = 0;antIndx < srs.nSRSports; antIndx++){
      int n_SRS_CS_i = (srs.cyclicShift+(n_SRS_cs_max*(p_i-1000))/srs.nSRSports)%n_SRS_cs_max;
      double alpha_i = 2*pi*n_SRS_CS_i/n_SRS_cs_max;
      for(int i = 0;i < srs.seqLength;i++){
	zcSeq(antIndx,i) = expj(alpha_i*i,1)*expj(phi_12(srs.u,i),1);
      }
      p_i = p_i+1;
    }
  }
  else if(srs.seqLength == 18){
    for(int antIndx = 0;antIndx < srs.nSRSports; antIndx++){
      int n_SRS_CS_i = (srs.cyclicShift+(n_SRS_cs_max*(p_i-1000))/srs.nSRSports)%n_SRS_cs_max;
      double alpha_i = 2*pi*n_SRS_CS_i/n_SRS_cs_max;
      for(int i = 0;i < srs.seqLength;i++){
	zcSeq(antIndx,i) = expj(alpha_i*i,1)*expj(phi_18(srs.u,i),1);
      }
      p_i = p_i+1;
    }
  }
  else if(srs.seqLength == 24){
    for(int antIndx = 0;antIndx < srs.nSRSports; antIndx++){
      int n_SRS_CS_i = (srs.cyclicShift+(n_SRS_cs_max*(p_i-1000))/srs.nSRSports)%n_SRS_cs_max;
      double alpha_i = 2*pi*n_SRS_CS_i/n_SRS_cs_max;
      for(int i = 0;i < srs.seqLength;i++){
	zcSeq(antIndx,i) = expj(alpha_i*i,1)*expj(phi_24(srs.u,i),1);
      }
      p_i = p_i+1;
    }
  }
  else if(srs.seqLength == 30){
    for(int antIndx = 0;antIndx < srs.nSRSports; antIndx++){
      int n_SRS_CS_i = (srs.cyclicShift+(n_SRS_cs_max*(p_i-1000))/srs.nSRSports)%n_SRS_cs_max;
      double alpha_i = 2*pi*n_SRS_CS_i/n_SRS_cs_max;
      for(int i = 0;i < srs.seqLength;i++){
	zcSeq(antIndx,i) = expj(alpha_i*i,1)*expj(-1*pi*(srs.u+1)*(i+1)*(i+2)/31);
      }
      p_i = p_i+1;
    }
  }
  else{
    int N_ZC = nearestPrime(srs.seqLength);
    double q_bar = (double)N_ZC*(srs.u+1)/31;
    int q = floor_i(q_bar+0.5)+srs.v*pow(-1,floor_i(2*q_bar));
    for(int antIndx = 0;antIndx < srs.nSRSports; antIndx++){
      int n_SRS_CS_i = (srs.cyclicShift+(n_SRS_cs_max*(p_i-1000))/srs.nSRSports)%n_SRS_cs_max;
      double alpha_i = 2*pi*n_SRS_CS_i/n_SRS_cs_max;
      for(int i = 0;i < srs.seqLength;i++){
	int m = mod(i,N_ZC);
	zcSeq(antIndx,i) = expj(alpha_i*i,1)*expj(-1*pi*q*m*(m+1)/N_ZC,1);
      }
      p_i = p_i+1;
    }
  }
  
  return zcSeq;
}






















