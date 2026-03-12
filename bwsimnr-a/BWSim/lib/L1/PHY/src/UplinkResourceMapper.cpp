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
#include "../include/ResourceMapper.h"

ivec generateGoldSequence(double Cinit,int seqLength)
{
  ivec goldSequence;
  ivec firstSequence,secSequence;
  int Nc = 1600,nSeq,ithSeq,nextInt;
  
  nSeq = 31;
  firstSequence = zeros_i(nSeq); 
  
  firstSequence(0) = 1;	
  goldSequence.set_size(seqLength);
  secSequence = to_ivec(reverse(dec2bin(31,(int)Cinit)));
  
  for (ithSeq = 0;ithSeq < (Nc + seqLength);ithSeq ++)
  {
    nextInt = (firstSequence(ithSeq + 3) + firstSequence(ithSeq)) % 2;
    append(firstSequence ,nextInt);
    
    nextInt = (secSequence(ithSeq + 3) + secSequence(ithSeq + 2) + secSequence(ithSeq + 1) + secSequence(ithSeq)) % 2;
    append(secSequence ,nextInt);
  }
  
  for (ithSeq = 0;ithSeq < seqLength;ithSeq ++)
  {
    goldSequence(ithSeq) = (firstSequence(ithSeq + Nc) + secSequence(ithSeq + Nc)) % 2;
  }
  
  return goldSequence;
}

UplinkResourceMapper::UplinkResourceMapper(){
  
  fftSizePerBandwidth = ivec("128 256 512 1024 1536 2048 4096");
  rbgSizePerBandwidth = ivec("1 2 4 5 8 8 16");
//    rbgSizePerBandwidth = ivec("1 1 1 1 1 1 1");

  nULRBs = -1;
  fftSize = -1;
  RBG_Size = -1;
  usedSubcarriers = -1;
  guardSubcarriersInOneSide = -1;
  dcSubcarrier = -1;
  nPRB.set_length(0);
  vrbNumbers.set_length(0);
}

void UplinkResourceMapper::init(int fft_size, int nRB, Size_S resourceBlockSize , HoppingMode_E hoppingmode, HoppingType_E hoppingtype ){
  
  rbSize=resourceBlockSize;
  hoppingMode=hoppingmode;
  hoppingType=hoppingtype;
  
  
  int indx = find(fftSizePerBandwidth,fft_size);
  if(indx != -1)
  {
    nULRBs = nRB;
    usedSubcarriers = rbSize.numberOfSubcarriers * nRB;
    RBG_Size = rbgSizePerBandwidth(indx);
    vrbNumbers =  getIntegers(0,nULRBs-1);
    nPRB.set_length(nULRBs);
    for(int j = 0; j < nULRBs; j++)
      nPRB(j)=ones_i(2)*vrbNumbers(j);
    fftSize = fftSizePerBandwidth(indx);
    guardSubcarriersInOneSide = (fftSizePerBandwidth(indx) - usedSubcarriers)/2;
    dcSubcarrier = fftSizePerBandwidth(indx)/2;
  }
  else
  {
    cout<<"[both:]Unsupported fftSize.. Aborting in UplinkResourceMapper::init()"<<endl;abort();
  }
}


Array<ivec> UplinkResourceMapper::performType2hopping(int N_SB , int N_HO_RB, int L_CRBS, int RB_START, int Subframe_number,int CURRENT_TX_NB,int NCellID, int UL_N_RB)
{
  
  int n_s;
  ivec n_s_index;
  
  ivec n_VRB,n_VRB_Tilda;
  ivec f_hop,f_m;
  int N_sb_RB;
  int i,ith;
  ivec c,my_product;
  Array<ivec> n_PRB_Tilda(2),n_PRB(2);
  ivec prbs;
  prbs.set_length(L_CRBS);
  double Cinit;
  
  n_VRB = to_ivec(linspace(1,L_CRBS,L_CRBS)) + RB_START -1;  
  
  
  f_hop.set_length(20);
  f_m.set_length(20);
  Cinit =NCellID;
  c = generateGoldSequence(Cinit,200);
  if(N_SB == 1)
  {
    n_VRB_Tilda = n_VRB;
    N_sb_RB = UL_N_RB;
  }
  else
  {
    n_VRB_Tilda = n_VRB - ceil_i(N_HO_RB / 2) ;
    N_sb_RB = floor_i((UL_N_RB - N_HO_RB -N_HO_RB % 2 )/N_SB);
    
  }
  
  n_s_index.set_length(2);
  
  Subframe_number = Subframe_number % 10;
  
  n_s_index(0) = 2*Subframe_number;
  n_s_index(1) = 2*Subframe_number +1;
  
  
  for(n_s =0; n_s<=1;n_s++)
  {
    if(hoppingMode == _HOPPING_MODE_INTER_)
      i = floor_i(n_s_index(n_s)/2);
    else
      i = n_s_index(n_s);
    
    switch(N_SB)
    {
      case 1: f_hop(i) = 0;
      f_m(i) = i % 2;
      
      if(hoppingMode == _HOPPING_MODE_INTER_)
	f_m(i) = CURRENT_TX_NB % 2;
      
      for(int prb_index =0;prb_index < L_CRBS;prb_index++)
      {
	prbs(prb_index) = (n_VRB_Tilda(prb_index) + f_hop(i) * N_sb_RB + ((N_sb_RB -1) - 2*(n_VRB_Tilda(prb_index)  % N_sb_RB)) * f_m(i)) % (N_sb_RB * N_SB);
      }
      n_PRB_Tilda(n_s) = prbs;
      n_PRB(n_s) = n_PRB_Tilda(n_s);
      break;
      
      
      case 2: 
	if(n_s == 0)
	{
	  ivec k = "1:9";	
	  my_product = elem_mult(c(k),to_ivec(pow(2.0,to_vec(k-1))));
	  f_hop(0) =  sum(my_product) % N_SB;
	  for( ith = 1;ith<=i;ith++)
	  {    
	    
	    ivec k = to_ivec(linspace(1,9,9)) + ith*10 ;		
	    my_product = elem_mult(c(k),to_ivec(pow(2.0,to_vec(k-ith*10-1))));
	    f_hop(ith) = (f_hop(ith-1) + sum(my_product)) % N_SB;
	  }
	}
	else
	{
	  ivec k = to_ivec(linspace(1,9,9)) + i*10 ;		
	  my_product = elem_mult(c(k),to_ivec(pow(2.0,to_vec(k-i*10-1))));
	  f_hop(i) = (f_hop(i-1) + sum(my_product)) % N_SB;
	  
	}
	f_m(i) = c(i*10);
	
	
	for(int prb_index =0;prb_index < L_CRBS;prb_index++)
	{
	  prbs(prb_index) = (n_VRB_Tilda(prb_index) + f_hop(i) * N_sb_RB + ((N_sb_RB -1) - 2*(n_VRB_Tilda(prb_index)  % N_sb_RB)) * f_m(i)) % (N_sb_RB * N_SB);
	}
	n_PRB_Tilda(n_s) = prbs;
	n_PRB(n_s) = n_PRB_Tilda(n_s) + ceil_i(N_HO_RB/2);
	break;
	
      default: if(N_SB > 2)
      {
	
	if(n_s == 0)
	{		
	  ivec k = "1:9";	
	  my_product = elem_mult(c(k),to_ivec(pow(2.0,to_vec(k-1))));
	  f_hop(0) = ( sum(my_product) % (N_SB -1) + 1) % N_SB;
	  for( ith = 1;ith<=i;ith++)
	  {		  	
	    ivec k = to_ivec(linspace(1,9,9)) + ith*10 ;	
	    
	    my_product = elem_mult(c(k),to_ivec(pow(2.0,to_vec(k- ith*10-1))));
	    f_hop(ith) = (f_hop(ith-1) + sum(my_product) % (N_SB -1) + 1) % N_SB;
	    
	  }
	}
	else
	{
	  ivec k = to_ivec(linspace(1,9,9)) + i*10 ;				  
	  my_product = elem_mult(c(k),to_ivec(pow(2.0,to_vec(k- i*10-1))));
	  f_hop(i) = (f_hop(i-1) + sum(my_product) % (N_SB -1) + 1) % N_SB;
	  
	}
	f_m(i) = c(i*10);
	
	for(int prb_index =0;prb_index < L_CRBS;prb_index++)
	{
	  prbs(prb_index) = (n_VRB_Tilda(prb_index) + f_hop(i) * N_sb_RB + ((N_sb_RB -1) - 2*(n_VRB_Tilda(prb_index)  % N_sb_RB)) * f_m(i)) % (N_sb_RB * N_SB);
	}
	n_PRB_Tilda(n_s) = prbs;
	n_PRB(n_s) = n_PRB_Tilda(n_s) + ceil_i(N_HO_RB/2);
      }
      else
      {
	cout<<"[both:] Freq hop is not supported"<<endl;exit(1);
	
      }
      
      break;
    }

  }
  return n_PRB;
  
  
}

void UplinkResourceMapper::perform_UL_Freq_hopping(int Subframe_No,int RB_START,int L_CRBS,int N_SB, int NCellID,bool isWithHopping, int N_HO_RB)
{
  
  
  if(isWithHopping)
  {
    int UL_N_RB = nULRBs;
    int n_S1_PRB,n_S1_PRB_Tilda,n_PRB;
    bvec Hop_info;
    int N_UL_hop;
    
    int N_HO_RB_Tilda;
    int N_PUSCH_RB=-1; 
    int n_PRB_Tilda;
    int y,type1_RBS;
    Array<ivec> PRB_Indices(2),PRB_temp_indices;
    ivec PRb_temp;
    int CURRENT_TX_NB = 1; // The tx no for the PDU in slot ns; harq status variable
    
    
    if(UL_N_RB >= 6 && UL_N_RB <= 49)
    {
      N_UL_hop = 1;
      Hop_info.set_length(N_UL_hop);
      if(hoppingType!=_HOPPING_TYPE_11_)
	Hop_info = "0"; // 0 --> Type1;
      else
	Hop_info = "1"; // 1---> Type 2 PUSCH Hoping
	
    }
    else
    {
      N_UL_hop = 2;
      Hop_info.set_length(N_UL_hop);
      if(hoppingType==_HOPPING_TYPE_00_)      Hop_info = "0 0"; // 00-->  Type 1 PUSCH Hoping
      else if(hoppingType!=_HOPPING_TYPE_01_) Hop_info = "0 1"; // 01-->  Type 1 PUSCH Hoping
      else if(hoppingType!=_HOPPING_TYPE_10_) Hop_info = "1 0"; // 10-->  Type 1 PUSCH Hoping
      else				    Hop_info = "1 1"; // 11-->  Type 2 PUSCH Hoping
      
    };
    
    N_HO_RB_Tilda = N_HO_RB + N_HO_RB % 2 ;
    n_S1_PRB = RB_START;
    n_S1_PRB_Tilda = n_S1_PRB - N_HO_RB_Tilda/2;
    PRb_temp = to_ivec(linspace(1,L_CRBS,L_CRBS)) +n_S1_PRB -1;
    PRB_Indices(0) =PRb_temp;
    switch(Hop_info.size())
    {
      case 1:       
	y = ceil_i(log2((UL_N_RB * (UL_N_RB + 1)/2))) - 1;
	type1_RBS = floor_i(pow2(y)/UL_N_RB);
	(void) type1_RBS; //WarningFix: unused
	if(Hop_info == "0")
	{
	  N_PUSCH_RB = UL_N_RB - N_HO_RB_Tilda -(UL_N_RB % 2);
	  
	  n_PRB_Tilda = (floor_i(N_PUSCH_RB / 2) + n_S1_PRB_Tilda) % N_PUSCH_RB;     
	  n_PRB  = n_PRB_Tilda + N_HO_RB_Tilda / 2;   // lowest index PRB of the 2nd slot of RA in subframe i ................
	  
	  PRb_temp = (to_ivec(linspace(1,L_CRBS,L_CRBS)) +n_PRB -1)  ;
	  PRB_Indices(1).set_length(L_CRBS);
	  for(int prbindex =0;prbindex<L_CRBS;prbindex++)
	  {
	    if(PRb_temp(prbindex) > N_PUSCH_RB)
	      PRB_Indices(1)(prbindex) = PRb_temp(prbindex) % N_PUSCH_RB + N_HO_RB_Tilda / 2 -1;
	    else
	      PRB_Indices(1)(prbindex) = PRb_temp(prbindex); 
	  }
	  
	  if(hoppingMode == _HOPPING_MODE_INTRA_INTER_)
	  {
	    if(CURRENT_TX_NB % 2 == 0)
	      PRB_Indices(1) = PRB_Indices(0);
	    else
	      PRB_Indices(1) =  PRB_Indices(1);
	  }
	}
	else
	{
	  PRB_temp_indices= performType2hopping(N_SB,N_HO_RB,L_CRBS,RB_START,Subframe_No,CURRENT_TX_NB,NCellID,UL_N_RB);
	  PRB_Indices = PRB_temp_indices;
	}

	break;
	
      case 2 : 
	y = ceil_i(log2((UL_N_RB * (UL_N_RB + 1)/2))) - 2;
	type1_RBS = floor_i(pow2(y)/UL_N_RB);
	if(Hop_info == "0 0")
	{
	  N_PUSCH_RB = UL_N_RB - N_HO_RB_Tilda -(UL_N_RB % 2);
	  n_PRB_Tilda = (floor_i(N_PUSCH_RB / 2) + n_S1_PRB_Tilda) % N_PUSCH_RB;     
	  n_PRB  = n_PRB_Tilda + N_HO_RB_Tilda / 2;   // lowest index PRB of the 2nd slot of RA in subframe i
	  PRb_temp =to_ivec(linspace(1,L_CRBS,L_CRBS)) +n_PRB -1;
	  PRB_Indices(1).set_length(L_CRBS);
	  for(int prbindex =0;prbindex<L_CRBS;prbindex++)
	  {
	    if(PRb_temp(prbindex) > N_PUSCH_RB)
	      PRB_Indices(1)(prbindex) = PRb_temp(prbindex) % N_PUSCH_RB + N_HO_RB_Tilda / 2 -1;
	    else
	      PRB_Indices(1)(prbindex) = PRb_temp(prbindex); 
	  }
	  
	  if(hoppingMode == _HOPPING_MODE_INTER_)
	  {
	    if(CURRENT_TX_NB % 2 == 0)
	      PRB_Indices(1) = PRB_Indices(0);
	    else
	      PRB_Indices(1) =  PRB_Indices(1);
	  }
	}
	else if(Hop_info == "0 1")
	{
	  N_PUSCH_RB = UL_N_RB - N_HO_RB_Tilda -(UL_N_RB % 2);
	  n_PRB_Tilda = ( - floor_i(N_PUSCH_RB / 4) + n_S1_PRB_Tilda) % N_PUSCH_RB; 
	  n_PRB = n_PRB_Tilda + N_HO_RB_Tilda / 2;
	  PRb_temp = to_ivec(linspace(1,L_CRBS,L_CRBS)) +n_PRB -1;
	  PRB_Indices(1).set_length(L_CRBS);
	  for(int prbindex =0;prbindex<L_CRBS;prbindex++)
	  {
	    if(PRb_temp(prbindex) > N_PUSCH_RB)
	      PRB_Indices(1)(prbindex) = PRb_temp(prbindex) % N_PUSCH_RB + N_HO_RB_Tilda / 2 -1;
	    else
	      PRB_Indices(1)(prbindex) = PRb_temp(prbindex); 
	  }
	  
	  if(hoppingMode == _HOPPING_MODE_INTER_)
	  {
	    if(CURRENT_TX_NB % 2 == 0)
	      PRB_Indices(1) = PRB_Indices(0);
	    else
	      PRB_Indices(1) =  PRB_Indices(1);
	  }
	}
	else if(Hop_info == "1 0")
	{
	  N_PUSCH_RB = UL_N_RB - N_HO_RB_Tilda -(UL_N_RB % 2);
	  n_PRB_Tilda = ( floor_i(N_PUSCH_RB / 2) + n_S1_PRB_Tilda) % N_PUSCH_RB; 
	  n_PRB = n_PRB_Tilda + N_HO_RB_Tilda / 2;
	  PRb_temp = to_ivec(linspace(1,L_CRBS,L_CRBS)) +n_PRB -1;
	  
	  PRB_Indices(1).set_length(L_CRBS);
	  for(int prbindex =0;prbindex<L_CRBS;prbindex++)
	  {
	    if(PRb_temp(prbindex) > N_PUSCH_RB)
	      PRB_Indices(1)(prbindex) = PRb_temp(prbindex) % N_PUSCH_RB + N_HO_RB_Tilda / 2 -1;
	    else
	      PRB_Indices(1)(prbindex) = PRb_temp(prbindex); 
	  }
	  
	  if(hoppingMode == _HOPPING_MODE_INTER_)
	  {
	    if(CURRENT_TX_NB % 2 == 0)
	      PRB_Indices(1) = PRB_Indices(0);
	    else
	      PRB_Indices(1) =  PRB_Indices(1);
	  }
	}
	else
	{
	  
	  PRB_temp_indices = performType2hopping(N_SB,N_HO_RB,L_CRBS,RB_START,Subframe_No,CURRENT_TX_NB,NCellID,UL_N_RB);
	  PRB_Indices = PRB_temp_indices;
	};

	
	break;
	
    }
    int startIndx=find(vrbNumbers,RB_START);
    for(int indx=startIndx,cnt=0;cnt<L_CRBS;indx++,cnt++)
      nPRB(indx)=getAcrossArray(PRB_Indices,cnt);
  }
  else
  {
    int startIndx=find(vrbNumbers,RB_START);
    for(int indx=startIndx,cnt=0;cnt<L_CRBS;indx++,cnt++)
    {
      nPRB(indx)=ones_i(2)*indx;
    }
  }
}


Size_S UplinkResourceMapper::getRBSize(){
  
  return(rbSize);
}

int UplinkResourceMapper::getResourceBlockGroupingSize(){
  
  return(RBG_Size);
}

int UplinkResourceMapper::getResourceBlocksPerSubframe(){
  
  return(nULRBs);
}

int UplinkResourceMapper::getFFTSize(){
  
  return(fftSize);
}

int UplinkResourceMapper::getUsedSubcarriers(){
  
  return(usedSubcarriers);
}

ivec UplinkResourceMapper::getPRBIndex(int nVRB){
  
  if(nVRB>=nULRBs)
  {
    cout<<"[both:] Error...Invalid nVRB to getPRBIndex()..."<<endl;
    abort();
  }
  return(nPRB(nVRB));
}

Array<ivec> UplinkResourceMapper::getPRBSubcarrierStartLocationinEachSlot(int rbNumber){
  
  int prbIndex=find(vrbNumbers,rbNumber);
  if(nPRB(prbIndex).length()==0)
  {
    cout<<"[both:] PRB Mapping uninitialized ..."<<endl;
    abort();
  }
    
  Array<ivec> prbSubcarrier(2);
  for(int cnt=0;cnt<2;cnt++)
  {
    int temp=nPRB(prbIndex)(cnt)*rbSize.numberOfSubcarriers+guardSubcarriersInOneSide;
    prbSubcarrier(cnt)=getIntegers(temp,temp+rbSize.numberOfSubcarriers-1);
  }
  return(prbSubcarrier);
}

Array<ivec> UplinkResourceMapper::getLocalisedSubcarrierStartLocationinEachSlot(int prbNumber){
  
  if(nPRB(prbNumber).length()==0)
  {
    cout<<"[both:] PRB Mapping uninitialized ..."<<endl;
    abort();
  }
  
  Array<ivec> prbSubcarrier(2);
  for(int cnt=0;cnt<2;cnt++)
  {
    int temp=nPRB(prbNumber)(cnt)*rbSize.numberOfSubcarriers+guardSubcarriersInOneSide;
    prbSubcarrier(cnt)=getIntegers(temp,temp+rbSize.numberOfSubcarriers-1);
  }
  return(prbSubcarrier);
}

int UplinkResourceMapper::getGuardSubcarriersInOneSide(){
  
  return (guardSubcarriersInOneSide);
}
