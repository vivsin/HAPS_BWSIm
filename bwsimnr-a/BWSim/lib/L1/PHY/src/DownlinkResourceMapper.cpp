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


DownlinkResourceMapper::DownlinkResourceMapper(){
  
  fftSizePerBandwidth = ivec("128 256 512 1024 1536 2048 4096");
  rbgSizePerBandwidth = ivec("1 2 4 4 8 8 16"); 

  nDLRB = -1;
  fftSize = -1;
  isLocalisedRB.set_length(0);
  RBG_Size = -1;
  usedSubcarriers = -1;
  guardSubcarriersInOneSide = -1;
  dcSubcarrier = -1;
  nPRB.set_length(0);
}

void DownlinkResourceMapper::init(int fft_size, int nRB, Size_S resourceBlockSize,bool isLocalised , bool gap1){
  
  rbSize=resourceBlockSize;

  ivec nVRB;
  nVRB = getIntegers(0,nRB-1);
  
  int indx = find(fftSizePerBandwidth,fft_size);
  
  if(indx != -1)
  {
    nDLRB = nRB;
    RBG_Size = rbgSizePerBandwidth(indx);
    usedSubcarriers = rbSize.numberOfSubcarriers * nRB;
    fftSize = fftSizePerBandwidth(indx);
    guardSubcarriersInOneSide = (fftSizePerBandwidth(indx) - usedSubcarriers)/2;
    dcSubcarrier = fftSizePerBandwidth(indx)/2;
  
    nPRB.set_length(nDLRB);
    for(int j = 0; j < nDLRB; j++)
      nPRB(j)=ones_i(2)*nVRB(j);
    
    isLocalisedRB = ones_i(nDLRB);
  }
  else
  {
    cout<<"[both:]Unsupported fftSize "+toString(fft_size)+".. Aborting in DownlinkResourceMapper::init()"<<endl;abort();
  }
    
    
//     else
//     {
//       if(gap1)
//       {
// 	if(DL_N_RB>=6 && DL_N_RB <=10)      	Ngap = ceil(DL_N_RB/2);
// 	else if(DL_N_RB==11)      			Ngap = 4;
// 	else if(DL_N_RB>=12 && DL_N_RB <=19)      	Ngap = 8;
// 	else if(DL_N_RB>=20 && DL_N_RB <=26)      	Ngap = 12;
// 	else if(DL_N_RB>=27 && DL_N_RB <=44)      	Ngap = 18;
// 	else if(DL_N_RB>=45 && DL_N_RB <=49)     	Ngap = 27;
// 	else if(DL_N_RB>=50 && DL_N_RB <=63)      	Ngap = 27;         
// 	else if(DL_N_RB>=64 && DL_N_RB <=79)      	Ngap = 32;         
// 	else if(DL_N_RB>=80 && DL_N_RB <=110)      	Ngap = 48;
// 	else
// 	{
// 	  cout<<"[both:] Error... Unsupported N_RB to DownlinkResourceMapper..."<<endl;
// 	  abort();
// 	}
// 	
// 	DL_N_VRB = 2*min(Ngap,(DL_N_RB-Ngap));
// 	DL_N_VRB_Tilde = DL_N_VRB;
//       } 
//       else
//       {   
// 	  if(DL_N_RB<50){
// 	    cout<<" [both:] Error ... Gap2 is defined for 50 to 110 resource blocks only..."<<endl;
// 	    abort();
// 	  }
// 	else if(DL_N_RB>=50 && DL_N_RB <=63)	Ngap = 9;
// 	else if(DL_N_RB>=64 && DL_N_RB <=79)    Ngap = 16;
// 	else if(DL_N_RB>=80 && DL_N_RB <=110)   Ngap = 16;
// 	else
// 	{
// 	  cout<<"[both:] Error... Unsupported N_RB to DownlinkResourceMapper..."<<endl;
// 	  abort();
// 	}
// 	
// 	DL_N_VRB = floor(DL_N_RB/(2*Ngap))*2*Ngap;	// Not used anywhere
// 	DL_N_VRB_Tilde = 2*Ngap;
//       }
// 
//     //-------------Number of rows and nulls for Interleaving------------------
//       
// 	
//       Nrow = ceil_i(double(DL_N_VRB_Tilde)/double((4*RBG_Size)))*RBG_Size;
//       
//       
//       Nnull = 4*Nrow - DL_N_VRB_Tilde; 
//       
//   
//       
//       
//       int n_VRB_Tilde,n_PRB_Tilde1,n_PRB_Tilde2, n_PRB_Tilde_Even, n_PRB_Tilde_Odd, n_PRB_Even, n_PRB_Odd;
//       
//       
//       
//       for(int vrb_cnt=0;vrb_cnt<DL_N_VRB;vrb_cnt++)
//       {
// 	n_VRB_Tilde = mod(nVRB(vrb_cnt),DL_N_VRB_Tilde);
// 	n_PRB_Tilde1 = 2*Nrow*mod(n_VRB_Tilde,2) + floor_i(double(n_VRB_Tilde)/double(2)) + DL_N_VRB_Tilde*floor_i(double(nVRB(vrb_cnt))/double(DL_N_VRB_Tilde));
// 	n_PRB_Tilde2 = Nrow*mod(n_VRB_Tilde,4) + floor_i(double(n_VRB_Tilde)/double(4)) + DL_N_VRB_Tilde*floor_i(double(nVRB(vrb_cnt))/double(DL_N_VRB_Tilde));
// 	
//       
//       
//       
//     //------------------For Even Slot Number--------------------------
//       
//     
//       
// 	    if(Nnull != 0 && n_VRB_Tilde >= (DL_N_VRB_Tilde-Nnull) && mod(n_VRB_Tilde,2) == 1)
// 		    n_PRB_Tilde_Even = n_PRB_Tilde1-Nrow;
// 	    else if(Nnull != 0 && n_VRB_Tilde >= (DL_N_VRB_Tilde-Nnull) && mod(n_VRB_Tilde,2) == 0)
// 		    n_PRB_Tilde_Even = n_PRB_Tilde1-Nrow+Nnull/2;
// 	    else if(Nnull != 0 && n_VRB_Tilde < (DL_N_VRB_Tilde-Nnull) && mod(n_VRB_Tilde,4) >= 2)
// 		    n_PRB_Tilde_Even = n_PRB_Tilde2-Nnull/2;
// 	    else
// 		    n_PRB_Tilde_Even = n_PRB_Tilde2;
// 	    
//     
// 	
// 	
//       //------------------For Odd Slot Number--------------------------
//       n_PRB_Tilde_Odd = mod(n_PRB_Tilde_Even + DL_N_VRB_Tilde/2,DL_N_VRB_Tilde) + DL_N_VRB_Tilde*floor_i(nVRB(vrb_cnt)/DL_N_VRB_Tilde);
// 	
// 	
//       //--------------The VRB Numbers Mapped to PRB Numbers are---------------------
// 	
// 	if(n_PRB_Tilde_Even < DL_N_VRB_Tilde/2)
// 	  n_PRB_Even = n_PRB_Tilde_Even;
// 	else
// 	  n_PRB_Even = n_PRB_Tilde_Even + Ngap - DL_N_VRB_Tilde/2;
// 	
// 	if(n_PRB_Tilde_Odd < DL_N_VRB_Tilde/2)
// 	  n_PRB_Odd = n_PRB_Tilde_Odd;
// 	else
// 	  n_PRB_Odd = n_PRB_Tilde_Odd + Ngap - DL_N_VRB_Tilde/2;
// 	
// 	
// 	nPRBPerBandwidth(i)(vrb_cnt)(0)=n_PRB_Even;
// 	nPRBPerBandwidth(i)(vrb_cnt)(1)=n_PRB_Odd;
// 	
// 	
//       }
//       ivec temp1;temp1.set_length(DL_N_VRB);
//       for(int vrb_cnt=0;vrb_cnt<DL_N_VRB;vrb_cnt++)
// 	temp1(vrb_cnt)=nPRBPerBandwidth(i)(vrb_cnt)(0);
//       
//       ivec temp2=getIntegers(0,DL_N_RB-1);
//       
//       
//       temp2=find(temp1,temp2);
//       
//       ivec temp;temp.set_length(DL_N_RB-DL_N_VRB);
//       temp=find(temp2,-1,1);
//       
//       for(int rb_cnt=0;rb_cnt<temp.length();rb_cnt++)
//       {
// 	nPRBPerBandwidth(i)(DL_N_VRB+rb_cnt)(0)=temp(rb_cnt);
// 	nPRBPerBandwidth(i)(DL_N_VRB+rb_cnt)(1)=temp(rb_cnt);
//       }
// 
//     }

  
}

Size_S DownlinkResourceMapper::getRBSize(){
  
  return(rbSize);
}

int DownlinkResourceMapper::getResourceBlockGroupingSize(){
  
  return(RBG_Size);
}

int DownlinkResourceMapper::getResourceBlocksPerSubframe(){
  
  return(nDLRB);
}

int DownlinkResourceMapper::getFFTSize(){
  
  return(fftSize);
}

int DownlinkResourceMapper::getUsedSubcarriers(){
  
  return(usedSubcarriers);
}

ivec DownlinkResourceMapper::getPRBIndex(int nVRB){
  if(nVRB>=nDLRB)
  {
    cout<<"[both:] Error...Invalid nVRB to getPRBIndex()..."<<endl;
    abort();
  }
  return(nPRB(nVRB));
}

Array<ivec> DownlinkResourceMapper::getPRBMapping(){
 
  return(nPRB);
}

bool DownlinkResourceMapper::isLocalised(int VRBNumber){
  return(isLocalisedRB(VRBNumber));
}

Array<ivec> DownlinkResourceMapper::getPRBSubcarrierStartLocationinEachSlot(int nVRB){
  
  ivec prbIndex=getPRBIndex(nVRB);
  
  Array<ivec> prbSubcarrier(2);
  for(int cnt=0;cnt<2;cnt++)
  {
    int temp=prbIndex(cnt)*rbSize.numberOfSubcarriers+guardSubcarriersInOneSide;
    prbSubcarrier(cnt)=getIntegers(temp,temp+rbSize.numberOfSubcarriers-1);
    prbSubcarrier(cnt)+=to_ivec(prbSubcarrier(cnt)>=dcSubcarrier);
  }
  return(prbSubcarrier);
}

Array<ivec> DownlinkResourceMapper::getLocalisedSubcarrierStartLocationinEachSlot(int PRBNumber){
  
  Array<ivec> prbSubcarrier(2);
  for(int cnt=0;cnt<2;cnt++)
  {
    int temp=PRBNumber*rbSize.numberOfSubcarriers+guardSubcarriersInOneSide;
    prbSubcarrier(cnt)=getIntegers(temp,temp+rbSize.numberOfSubcarriers-1);
    prbSubcarrier(cnt)+=to_ivec(prbSubcarrier(cnt)>=dcSubcarrier);
  }
  return(prbSubcarrier);
}

int DownlinkResourceMapper::getGuardSubcarriersInOneSide(){
 
  return (guardSubcarriersInOneSide);
}
