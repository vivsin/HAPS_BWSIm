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

#include "../include/ReferenceSignalGenerator.h"
/*! \fn getCRSPortIndices(...)
* \brief returns CRS port indices in portsUsed vector 
* \param [in] portsUsed ---> all ports used for a particular transmission mode.
* \param [out] crsPorts ---> contains the CRS port indices in portsUsed
* \return crsPorts*/
ivec getCRSPortIndices(ivec portsUsed){

  ivec crsPorts(0);
  for(int portIndex=0; portIndex<portsUsed.length(); portIndex++)
    if((portsUsed(portIndex)>=0) && (portsUsed(portIndex)<4))
      append(crsPorts, portIndex);
    return(crsPorts);
}
/*! \fn getDMRSPortIndices(...)
* \brief returns index of DMRS ports in the given portsUsed vector 
* \param [in] portsUsed ---> all ports used for a particular transmission mode.
* \param [out] dmrsPorts ---> contains the DMRS port indices in portsUsed vector
* \return dmrsPorts*/
ivec getDMRSPortIndices(ivec portsUsed){

  ivec dmrsPorts(0);
  for(int portIndex=0; portIndex<portsUsed.length(); portIndex++)
    if(portsUsed(portIndex)==5 || ((portsUsed(portIndex)>6) && (portsUsed(portIndex)<15)))
      append(dmrsPorts, portIndex);
  return(dmrsPorts);
}

/*! \fn getCSIRSPortIndices(...)
* \brief returns index of CSIRS ports in the given portsUsed vector 
* \param [in] portsUsed ---> all ports used for a particular transmission mode.
* \param [out] csirsPorts ---> contains the CSIRS port indices in portsUsed vector
* \return csirsPorts*/
ivec getCSIRSPortIndices(ivec portsUsed){

  ivec csirsPorts(0);
  for(int portIndex=0; portIndex<portsUsed.length(); portIndex++)
    if((portsUsed(portIndex)>14) && (portsUsed(portIndex)<23))
      append(csirsPorts, portIndex);
    return(csirsPorts);
}
/*! \fn getNRCSIRSPortIndices(...)
 * \brief returns index of NRCSIRS ports in the given portsUsed vector 
 * \param [in] portsUsed ---> all ports used for a particular transmission mode.
 * \param [out] NRcsirsPorts ---> contains the CSIRS port indices in portsUsed vector
 * \return csirsPorts*/
ivec getNRCSIRSPortIndices(ivec portsUsed){
  
  ivec NRcsirsPorts(0);
  for(int portIndex=0; portIndex<portsUsed.length(); portIndex++)
    if(((portsUsed(portIndex)>=3000) && (portsUsed(portIndex)<=3032))|| (portsUsed(portIndex)==3100))
      append(NRcsirsPorts, portIndex);
    return(NRcsirsPorts);
}
/*! \fn getCRSPortIndices(...)
* \brief returns the vector of CRS ports used for transmission in a given transmission mode 
* \param [in] portsUsed ---> all ports used for a particular transmission mode.
* \param [out] crsPorts ---> contains the CRS ports in portsUsed vector
* \return crsPorts*/ 
ivec getCRSPortNumbers(ivec portsUsed){

  ivec crsPorts(0);
  for(int portIndex=0; portIndex<portsUsed.length(); portIndex++)
    if((portsUsed(portIndex)>=0) && (portsUsed(portIndex)<4))
      append(crsPorts, portsUsed(portIndex));
    return(crsPorts);
}
/*! \fn getDMRSPortIndices(...)
* \brief returns the vector of DMRS ports used for transmission in a given transmission mode 
* \param [in] portsUsed ---> all ports used for a particular transmission mode.
* \param [out] dmrsPorts ---> contains the DMRS ports in portsUsed vector
* \return dmsPorts*/ 
ivec getDMRSPortNumbers(ivec portsUsed){

  ivec dmrsPorts(0);
  for(int portIndex=0; portIndex<portsUsed.length(); portIndex++)
    if(portsUsed(portIndex)==5 || ((portsUsed(portIndex)>6) && (portsUsed(portIndex)<15)) || ((portsUsed(portIndex)>=1000) && (portsUsed(portIndex)<=1011)))
      append(dmrsPorts, portsUsed(portIndex));
    return(dmrsPorts);
}
/*! \fn getCSIRSPortIndices(...)
* \brief returns the vector of CSIRS ports used for transmission in a given transmission mode 
* \param [in] portsUsed ---> all ports used for a particular transmission mode.
* \param [out] csirsPorts ---> contains the CSIRS ports in portsUsed vector
* \return csirsPorts*/ 

ivec getCSIRSPortNumbers(ivec portsUsed){
  
  ivec csirsPorts(0);
  for(int portIndex=0; portIndex<portsUsed.length(); portIndex++)
    if((portsUsed(portIndex)>14) && (portsUsed(portIndex)<23))
      append(csirsPorts, portsUsed(portIndex));
    //Change if multiple CSI-RS configured
    else if((int(portsUsed(portIndex)/100)>14) && (int(portsUsed(portIndex)/100)<23))
      append(csirsPorts, portsUsed(portIndex));
    
    else if((portsUsed(portIndex)>=3100) && (portsUsed(portIndex)<=3432))
      append(csirsPorts, portsUsed(portIndex));
    
    return(csirsPorts);
}
/*! \fn getNRCSIRSPortIndices(...)
 * \brief returns the vector of NRCSIRS ports used for transmission in a given transmission mode 
 * \param [in] portsUsed ---> all ports used for a particular transmission mode.
 * \param [out] NRcsirsPorts ---> contains the NRCSIRS ports in portsUsed vector
 * \return NRcsirsPorts*/ 

ivec getNRCSIRSPortNumbers(ivec portsUsed){
  
  ivec NRcsirsPorts(0);
  for(int portIndex=0; portIndex<portsUsed.length(); portIndex++)
    if(((portsUsed(portIndex)>=3000) && (portsUsed(portIndex)<=3032))|| (portsUsed(portIndex)==3100))
      append(NRcsirsPorts, portsUsed(portIndex));
    //Change if multiple CSI-RS configured
    else if(((int(portsUsed(portIndex)/100)>3000) && (int(portsUsed(portIndex)/100)<3032)) || (int(portsUsed(portIndex)/100)==3100))
      append(NRcsirsPorts, portsUsed(portIndex));
    return(NRcsirsPorts);
}

/*! \fn getCRSsequence(...)
* \brief Generates CRS sequence of length equals to #CRS tones per port for given symbol in a slot and frame.
* \param [in] cellID ---> cell Identity number
* \param [in] symbolIndex --> Indicates the RS symbol location in a slot ("0:6")
* \param [in] slotIndex --> Indicates the slot index in a frame ("0:19")
* \param [in] nPhysicalRBs --> #RBs in a DL-Frame across freq.
* \return RSseq ---> CRS sequence of length equals to #RS tones per port for given symbol in a slot and frame.
*/
cvec getCRSsequence(int cellID, int symbolIndex, int slotIndex, int nPhysicalRBs)
{
  ivec goldSeq;
  cvec RSseq;
  double Cinit;
  int nMaxPhysicalResources = 110;
  int seqLength = nMaxPhysicalResources * 2;
  // gold sequence is generated based on cellID, slot number and OFDM symbol number
  Cinit = pow(2.0,10.0) * (7.0 * (slotIndex + 1) + symbolIndex + 1) * (2.0 * cellID + 1.0) + 2.0 * cellID + 1.0;
  goldSeq = generateGoldSequence(Cinit, 2*seqLength);

  RSseq.set_size(nMaxPhysicalResources*2);
  // complex reference Signal sequence is generated using the gold sequnce 
  for (int iSeq = 0;iSeq < (nMaxPhysicalResources*2);iSeq ++)
  {
    RSseq(iSeq) = dComplex((1.0 - 2.0 * goldSeq(2 * iSeq)),(1.0 - 2.0 * goldSeq(2 * iSeq + 1))) / sqrt(2.0);
  }
  // taking the middle 2*nPhysicalRBs reference symbols from the total symbols
  RSseq = RSseq.mid(nMaxPhysicalResources-nPhysicalRBs,2*nPhysicalRBs);
 
  return RSseq;
}

/*! \fn getDMRSsequence(...)
* \brief Generates DMRS sequence of length equals to #DMRS tones per port for given Subframe in a frame.
* \param [in] cellID ---> cell Identity number
* \param [in] subFrameIndex --> Indicates the Subframe index in a frame ("0:9")
* \param [in] nPhysicalRBs --> #RBs in a DL-Frame across freq.
* \param [in] portIndex ---> DMRS Port index
* \param [in] resourceBlockIDs---> gives the Resource Block number in which the DMRS has to sent
* \return [out] rsseqForMyRBs ---> DMRS sequence of length equals to #RS tones per port for given Subframe in a frame.
*/

cvec getDMRSsequence(int cellID, int nRNTI, int nSCID, int subFrameIndex, int portIndex, int nPhysicalRBs, ivec resourceBlockIDs)
{
  ivec goldSeq;
  cvec RSseq, RSseq1=zeros_c(12*resourceBlockIDs.length());/// assuming normal CP
    double Cinit;
  int seqLength;
  int nMaxPhysicalResources = 110;
  
 // If the port Index is 5 then the pseudo-random sequence is initiated with the following Cinit
  if(portIndex == 5)
  {
    Cinit = (subFrameIndex + 1.0)*(2.0 * (cellID + 1) + 1) *pow(2.0,16.0)+nRNTI;
    seqLength = nPhysicalRBs * 12;
  }
  // If the port index is between 7-14 then the pseudo-random sequence is initiated with the following Cinit
  else
  {
    Cinit = (subFrameIndex + 1.0)*(2.0 * (cellID + 1) + 1) *pow(2.0,16.0)+nSCID;
    seqLength = nMaxPhysicalResources * 12;/// assuming normal CP
  }
  
  // genereates the gold sequence of length 1320(110x12) for all 110 RBs
  goldSeq = generateGoldSequence(Cinit,2*seqLength);
  RSseq.set_size(seqLength);
  for (int iSeq = 0;iSeq < (seqLength);iSeq ++)
  {
    RSseq(iSeq) = dComplex((1.0 - 2.0 * goldSeq(2 * iSeq)),(1.0 - 2.0 * goldSeq(2 * iSeq + 1))) / sqrt(2.0);
  }
  int seq=0, cover;
  ;
  // this if statement returns the DMRS reference symbols after multiplying with the cover to assinged RBs for a particular UE
  if(portIndex != 5)
  {
    ivec orthogonalCover=getOrthoCoverCode(portIndex);
    for(int iRB=0; iRB<resourceBlockIDs.length(); iRB++)
      for(int lp=0; lp<4; lp++)
	for(int mp=0; mp<3; mp++, seq++)
	{
	  int id=3*lp*nMaxPhysicalResources+3*resourceBlockIDs(iRB)+mp;
	  if(mod(mp+resourceBlockIDs(iRB),2)==0)
	    cover=orthogonalCover(lp);
	  else //if(mod(mp+resourceBlockIDs(iRB),2)==1)
	    cover=orthogonalCover(3-lp);
	  // multiplies the Reference symbols with particular cover 
	  RSseq1(seq)=(cover/ sqrt(2.0))*dComplex((1.0 - 2.0 * goldSeq(2 * id)),(1.0 - 2.0 * goldSeq(2 * id + 1)));
	}
	
    return RSseq1;
  }
  
  cmat rssqePerRB=reshape(RSseq,nPhysicalRBs,12);
  cmat rsseqForMyRBs=rssqePerRB.get_rows(resourceBlockIDs);
  return rvectorize(rsseqForMyRBs);
  
}

int getCDMGroupIndx(int portNumber)
{
    if(portNumber>=1000 && portNumber<=1007)
    {
        switch(portNumber)
        {
            case 1000:
            case 1001:
            case 1004:
            case 1005:
                return 0;
                break;
            case 1002:
            case 1003:
            case 1006:
            case 1007:
                return 1;
                break;
        }
    }
    else if(portNumber>=1100 && portNumber<=1111)
    {
        switch(portNumber)
        {
            case 1100:
            case 1101:
            case 1106:
            case 1107:
                return 0;
                break;
            case 1102:
            case 1103:
            case 1108:
            case 1109:
                return 1;
                break;
            case 1104:
            case 1105:
            case 1110:
            case 1111:
                return 2;
                break;
        }
    }
    
    return -1;
}

cvec getDMRSSequenceNR(int cellID, int RNTI, int nSCID, int subFrameIndex, int portIndex, int nPhysicalRBs, ivec resourceBlockIDs, int nDMRSPerRB,int dmrsSymbolNumber,int symno,int dmrstype, int nCDM)
{
    // Refer 3GPP TR 38.211 v16.0.0, Section 7.4.1.1 
    ivec goldSeq;
    cvec RSseq=zeros_c(nDMRSPerRB*resourceBlockIDs.length());/// assuming normal CP
    double Cinit;
    int seqLength;
    int nMaxPhysicalResources = 275;
    int lamda = getCDMGroupIndx(portIndex);
    int N0 = 4*RNTI+2*cellID , N1 = 4*RNTI+2*cellID+1; //To make scramblingIDs to be unique and different for every BS and UE pair.
    
    int n_SCID = (lamda==1) ? 1-nSCID : nSCID;
    int scID = (lamda==-1) ? cellID : (n_SCID==0) ? N0 : N1;
    
    if((portIndex >= 1000 && portIndex <= 1007) || (portIndex >= 1100 && portIndex <= 1111))
    {
        int nSymbolsPerSlot = 14;
        Cinit = pow(2,17)*((nSymbolsPerSlot*subFrameIndex + dmrsSymbolNumber+1)*(2*scID+1)+floor_i(lamda/2)) + 2*scID + n_SCID;
        Cinit=(int)Cinit% (int)pow(2,31);
        seqLength = nPhysicalRBs * nDMRSPerRB;/// assuming normal CP
    }
    else
    {
        cout<<"Invalid DMRS Port "<<portIndex<<endl;abort();
    }
    
    int nREsPerRB = 12;

    int cover;
    double betaDMRS=sqrt((double)nCDM); //Refer Section 6.2.2 in 3GPP TR 38.211
    int delta = (dmrstype==1) ? lamda : 2*lamda;

    Array<ivec> orthogonalCover=getNROrthoCoverCode(portIndex);
    
    int dmrsDensityPerRB = (dmrstype==1) ? 3 : 2;   // No.of DMRS sets present in a RB.
    
    // generates the gold sequence of length 1320(110x12) for all 110 RBs
    goldSeq = generateGoldSequence(Cinit,2*seqLength);
    
    cvec sequence = zeros_c(nPhysicalRBs*nREsPerRB);    // generating the sequence for all REs in the whole band
    
    for(int rb_cnt=0;rb_cnt<nPhysicalRBs;rb_cnt++)
    {
        for(int dmrs_cnt=0;dmrs_cnt<dmrsDensityPerRB;dmrs_cnt++)
        {
            int n = dmrsDensityPerRB*rb_cnt + dmrs_cnt;
            for(int k_=0;k_<2;k_++)
            {
                int reId = (dmrstype==1) ? 4*n+2*k_+delta : 6*n+k_+delta;
                int t = 2*n+k_;
                dComplex r = dComplex((1.0 - 2.0 * goldSeq(2*t)),(1.0 - 2.0 * goldSeq(2*t + 1)))/sqrt(2);
                cover = orthogonalCover(k_)(symno);
                sequence(reId) = betaDMRS*cover*r;
            }
        }
    }
    
    //Copying the sequence of only required REs 
    for(int rb_cnt=0,re_cnt=0;rb_cnt<resourceBlockIDs.length();rb_cnt++)
    {
        for(int dmrs_cnt=0;dmrs_cnt<dmrsDensityPerRB;dmrs_cnt++)
        {
            for(int k_=0;k_<2;k_++,re_cnt++)
            {
                int reIndxInRB = (dmrstype==1) ? 4*dmrs_cnt+2*k_+delta : 6*dmrs_cnt+k_+delta;
                int reID = resourceBlockIDs(rb_cnt)*nREsPerRB +reIndxInRB; 
                RSseq(re_cnt) = sequence(reID);
            }
        }
    }
    
//     goldSeq = generateGoldSequence(Cinit,2*seqLength);
//     
//     for(int rb_cnt=0,re_cnt=0;rb_cnt<resourceBlockIDs.length();rb_cnt++)
//     {
//         for(int dmrs_cnt=0;dmrs_cnt<dmrsDensityPerRB;dmrs_cnt++)
//         {
//             int n = dmrsDensityPerRB*resourceBlockIDs(rb_cnt) + dmrs_cnt;
//             for(int k_=0;k_<2;k_++,re_cnt++)
//             {
//                 int reId = 2*n+k_;
//                 cover=orthogonalCover(k_)(symno);
//                 dComplex r = dComplex((1.0 - 2.0 * goldSeq(2*reId)),(1.0 - 2.0 * goldSeq(2*reId + 1)))/sqrt(2);
//                 RSseq(re_cnt) = betaDMRS*cover*r;
//             }
//         }
//     }
    
    return RSseq;
}

cvec getNRDMRSsequence(int cellID, int nRNTI, int nSCID, int subFrameIndex, int portIndex, int nPhysicalRBs, ivec resourceBlockIDs, int nDMRSPerRB,int dmrssymbolnumber,int symno, int dmrstype, int nCDM)
{
    return getDMRSSequenceNR(cellID,nRNTI,nSCID,subFrameIndex,portIndex,nPhysicalRBs,resourceBlockIDs,nDMRSPerRB,dmrssymbolnumber,symno,dmrstype,nCDM);
    
  
}
/*! \fn getCSIRSsequence(...)
* \brief Generates CSIRS sequence of length equals to maximun possible #CSIRS tones per port for given symbol in a slot and frame.
* \param [in] cellID ---> cell Identity number
* \param [in] symbolIndex --> Indicates the RS symbol location in a slot ("0:6")
* \param [in] slotIndex --> Indicates the slot index in a frame ("0:19")
* \param [in] nPhysicalRBs --> #RBs in a DL-Frame across freq.
* \return [out] RSseq --> CSIRS sequence of length equals to maximun possible #RS tones per port for given symbol in a slot and frame.
*/
cvec getCSIRSsequence(int cellID, int symbolIndex, int slotIndex, int nPhysicalRBs)
{
  ivec goldSeq;
  cvec RSseq;
  double Cinit;
  int nMaxPhysicalResources = 110;
  int seqLength = nMaxPhysicalResources;
  // gold sequence is generated based on cellID, slot Index and symbol index
  Cinit = pow(2.0,10.0) * (7.0 * (slotIndex + 1) + symbolIndex + 1) * (2.0 * cellID + 1.0) + 2.0 * cellID + 1.0;
  goldSeq = generateGoldSequence(Cinit,2*seqLength);
  // complex reference Signal sequence is generated using the gold sequnce 
  RSseq.set_size(nMaxPhysicalResources);
  for (int iSeq = 0;iSeq < (nMaxPhysicalResources);iSeq ++)
  {
    RSseq(iSeq) = dComplex((1.0 - 2.0 * goldSeq(2 * iSeq)),(1.0 - 2.0 * goldSeq(2 * iSeq + 1))) / sqrt(2.0);
  }

  RSseq = RSseq.mid(floor_i((nMaxPhysicalResources-nPhysicalRBs)/2),nPhysicalRBs);
  return RSseq;
}

cvec getNRCSIRSsequence(int cellID, int symbolIndex, int slotIndex, int nPhysicalRBs, int rsCountPerSym, int symno, int FDCDM, int TDCDM, int portIndex)
{
  ivec goldSeq;
  cvec RSseq;
  double Cinit;
  int nMaxPhysicalResources = 275;
  //int seqLength = nMaxPhysicalResources*2;
  int seqLength = nMaxPhysicalResources*rsCountPerSym;
  // gold sequence is generated based on cellID, slot Index and symbol index
  Cinit = pow(2.0,10.0) * ((14.0 * slotIndex) + symbolIndex + 1) * (2.0 * cellID + 1.0) + cellID; // cellID is assumed to be equal to nID
  Cinit=(int)Cinit% (int)pow(2,31);
  goldSeq = generateGoldSequence(Cinit,2*seqLength);
  // complex reference Signal sequence is generated using the gold sequnce 
  
  RSseq.set_size(seqLength);
  Array<ivec> orthogonalCover=getNROrthoCoverCodeforCSIRS(portIndex,FDCDM,TDCDM);
  int cover;
  for (int iSeq = 0;iSeq < seqLength;iSeq ++)
  { if(orthogonalCover.length()!=1)
    cover=orthogonalCover(iSeq%2)(symno);
    else
     cover=orthogonalCover(0)(0);
    RSseq(iSeq) =1.0 / sqrt(2.0/**FDCDM*TDCDM*/)*(complex< double >)cover*dComplex((1.0 - 2.0 * goldSeq(2 * iSeq)),(1.0 - 2.0 * goldSeq(2 * iSeq + 1)));
  
}
  //RSseq = RSseq.mid(0,nPhysicalRBs*2);
  RSseq = RSseq.mid(0,nPhysicalRBs*rsCountPerSym);
  return RSseq;
}

/*! \fn getMBSFNRSsequence(...)
* \brief Generates MBSFNRS sequence of length equals to #MBSFNRS tones per port for given symbol in a slot and frame.
* \param [in] MBSFNid ---> MBSFN-dedicated cell identity
* \param [in] symbolIndex --> Indicates the RS symbol location in a slot ("0:6")
* \param [in] slotIndex --> Indicates the slot index in a frame ("0:19")
* \param [in] PhysicalRBs --> #RBs in a DL-Frame across freq.
* \return [out] RSseq --> MBSFNRS sequence of length equals to #RS tones per port for given symbol in a slot and frame.
*/

cvec getMBSFNsequence(int MBSFNid, int symbolIndex, int slotIndex)
{
  ivec goldSeq;
  cvec RSseq;
  double Cinit;
  int nMaxPhysicalResources = 110;
  int seqLength = nMaxPhysicalResources * 6;
  // gold sequence is generated based on MBSFNid, slot Index and symbol index
  Cinit = pow(2.0,9.0) * (7.0 * (slotIndex + 1) + symbolIndex + 1) * (2.0 * MBSFNid + 1.0) +  MBSFNid;
  goldSeq = generateGoldSequence(Cinit,2*seqLength);

  RSseq.set_size(nMaxPhysicalResources*6);
  // complex reference Signal sequence is generated using the gold sequnce 
  for (int iSeq = 0;iSeq < (nMaxPhysicalResources*6);iSeq ++)
  {
    RSseq(iSeq) = dComplex((1.0 - 2.0 * goldSeq(2 * iSeq)),(1.0 - 2.0 * goldSeq(2 * iSeq + 1))) / sqrt(2.0);
  }

  return RSseq;
}



/*! \fn getPRSsequence(...)
* \brief Generates PRS sequence of length equals to maximun possible #PRS tones per port for given symbol in a slot and frame.
* \param [in] cellID ---> cell Identity number
* \param [in] symbolIndex ---> Indicates the RS symbol location in a slot ("0:6")
* \param [in] slotIndex ---> Indicates the slot index in a frame ("0:19")
* \param [in] nPhysicalRBs ---> max #RBs in a DL-Frame across freq.
* \return [out] RSseq ---> PRS sequence of length equals to maximun possible #RS tones per port for given symbol in a slot and frame.
*/

cvec getPRSsequence(int cellID, int symbolIndex, int slotIndex, int nPhysicalRBs)
{
  ivec goldSeq;
  cvec RSseq;
  double Cinit;
  int nMaxPhysicalResources = 110;
  int seqLength = nMaxPhysicalResources * 2;
  // gold sequence is generated based on cellID, slot Index and symbol index
  Cinit = pow(2.0,10.0) * (7.0 * (slotIndex + 1) + symbolIndex + 1) * (2.0 * cellID + 1.0) + 2.0 * cellID + 1.0;
  goldSeq = generateGoldSequence(Cinit,2*seqLength);

  RSseq.set_size(nMaxPhysicalResources*2);
  // complex reference Signal sequence is generated using the gold sequnce 
  for (int iSeq = 0;iSeq < (nMaxPhysicalResources*2);iSeq ++)
  {
    RSseq(iSeq) = dComplex((1.0 - 2.0 * goldSeq(2 * iSeq)),(1.0 - 2.0 * goldSeq(2 * iSeq + 1))) / sqrt(2.0);
  }

  return RSseq.mid(nMaxPhysicalResources-nPhysicalRBs,2*nPhysicalRBs);
}

cvec getPTRSsequenceTPenabled(int cellID, int symbolIndex, int slotIndex, int nSymbolPerSlot, int nRNTI, int puschSC, int scheduledBW, ModulationScheme_E MCS)
{
  ivec goldSeq, mindices;
  cvec RSseq;
  ivec coverW;
  double Cinit, BhetaPTRS;
  int mindex, cover;
  int nSamplesPerGroup, nPTRSGroups ; // see where & whether to store these value(either in any structs etc)
  bool enableTransformPrecoding = 1, dftSofdm = 1; // fill this value in config file or somewhere
  
  getNoOfSamplesAndPTRSGroupsTPenabled(scheduledBW, nSamplesPerGroup, nPTRSGroups);
  int seqLength = nSamplesPerGroup*nPTRSGroups;
  // gold sequence is generated based on cellID, slot number and OFDM symbol number
  Cinit = mod((pow(2.0,17.0) * (nSymbolPerSlot * slotIndex + symbolIndex + 1) * (2.0 * cellID + 1.0) + 2.0 * cellID), (pow2(31.0)));
  goldSeq = generateGoldSequence(Cinit, 2*seqLength);
  coverW=getOrthoCoverCodeForPTRSTPenabled(nRNTI,nSamplesPerGroup);
  RSseq.set_size(seqLength);
  
  mindices = getPTRSindexmTPenabled(nSamplesPerGroup, nPTRSGroups, puschSC);
  
  
  if ((enableTransformPrecoding == 1) || (dftSofdm == 1)) 
  {
    if (MCS == _MODULATION_SCHEME_QPSK_ || MCS == _MODULATION_SCHEME_BPSK_) BhetaPTRS = 1;
    if (MCS == _MODULATION_SCHEME_16_QAM_) BhetaPTRS = 3/sqrt(5);
    if (MCS == _MODULATION_SCHEME_64_QAM_) BhetaPTRS = 7/sqrt(21);
    if (MCS == _MODULATION_SCHEME_256_QAM_) BhetaPTRS = 15/sqrt(85);
  }
  
  // complex reference Signal sequence is generated using the gold sequnce '
  for (int iSeq = 0;iSeq < (seqLength);iSeq ++)
  {
    //cout<<"iSeq/nPTRSGroups\t"<<iSeq/nPTRSGroups<<"\tPTRSGroups\t"<<nPTRSGroups<<endl;
    if(iSeq/nSamplesPerGroup>= 1)
    {
      //cout<<"iSeq-nSamplesPerGroup\t"<<iSeq-nSamplesPerGroup<<endl;
      cover = coverW(mod(iSeq,nSamplesPerGroup));
    }
    else
    {
      //cout<<"coverW\t"<<coverW<<"\tiSeq\t"<<iSeq<<"\tseqLength\t"<<seqLength<<endl;
      cover = coverW(iSeq);
      
    }
    
    RSseq(iSeq) = (cover/sqrt(2.0))*exp(dComplex(0,1)*pi*double(mod(mindices(iSeq),2)))*dComplex((1.0 - 2.0 * goldSeq(iSeq)),(1.0 - 2.0 * goldSeq(iSeq)));
    //     cout << "deep RSseq(iSeq)\t" << RSseq(iSeq) << "\tiSeq " << iSeq << endl;
    
  }
  
  return RSseq;
}

/*! \fn generateRS(...)
* \brief Maps the CRS sequence to the CRS locations for given port in Subframe.
* \param [in] rsPositionMatrix ---> contains the reference signal positions of all ports in a RB pair
* \param [in,out] referenceSignal ---> it is a class variable
* \param [in] nRB ---> number of resource blocks
* \param [in] cellID ---> cell Identity number
* \param [in] subFrameIndexInFrame ---> subframe number in a frame
* \param [in] portsUsedInSubframe ---> it is a vector of port numbers used for transmission to a given mode 
* \param [in] portReferenceNumbers ---> these are used to represent the ports in an RB(0 represents data)
* \param [in] subFrameNumber ---> subframe number in a frame
* \param [in] nRNTI ---> it is a sequence initialization value which depends on C-RNTI
* \param portIndex ---> Index of the CRS port need to be filled.
* \param subframeStructure ---> Matrix of Subframe dimensions with data indicated as "0" and ports indicated as "1, 2, 3, 4"
* \return void
* \note Need to fix the variables rsInfo, nSymbolsperSlot, nRBs(per SF), nSlotsPerRB, nTxAntenna, portToPhysicalAntennaMapping
*/
void generateRS(ReferenceSignal_S &referenceSignal, int nRB, int cellID , int subFrameIndexInFrame, ivec portsUsedInSubframe , RBInfo_S &crsRBInfo, Array <DataBlkInfo_S > &dataBlkInfo, ivec nRNTI, bool isDownlink)
{   
  /// Same IDCELL=> same symbols for the given port even for a different interlace .. So no need to generate RS signal again for nodes with same IDCELL
  int rsCountPerSym;
  int nSlotsPerRB=crsRBInfo.rsPositionMatrix.cols()/7;
  cvec temp1;
  cmat temp2;
  int sym1, sym2;
  sort(portsUsedInSubframe);
  referenceSignal.rsSequencePerRB.set_length(portsUsedInSubframe.length());
  referenceSignal.portIndices=portsUsedInSubframe;
  ivec crsPorts=getCRSPortNumbers(portsUsedInSubframe), CSIRSports=getCSIRSPortNumbers(portsUsedInSubframe);
  int crsPortsLength=crsPorts.length(), CSIRSportsLength=CSIRSports.length();
  
  referenceSignal.CRSsymbolsPerPort.set_length(crsPortsLength);
  referenceSignal.CRSsequencePerPortPerSymbol.set_length(crsPortsLength);
  referenceSignal.CSIRSsymbolsPerPort.set_length(CSIRSportsLength);
  referenceSignal.CSIRSsequencePerPortPerSymbol.set_length(CSIRSportsLength);
  
  for(int crs_cnt=0;crs_cnt<crsPortsLength;crs_cnt++)
  {
    referenceSignal.CRSsymbolsPerPort(crs_cnt).set_length(0);
    if(crsPorts(crs_cnt)<2)
      referenceSignal.CRSsequencePerPortPerSymbol(crs_cnt).set_length(2*nSlotsPerRB);
    else
      referenceSignal.CRSsequencePerPortPerSymbol(crs_cnt).set_length(nSlotsPerRB);
  }
  for(int rs_cnt=0;rs_cnt<CSIRSportsLength;rs_cnt++)
  {
    referenceSignal.CSIRSsymbolsPerPort(rs_cnt).set_length(0);
    referenceSignal.CSIRSsequencePerPortPerSymbol(rs_cnt).set_length(nSlotsPerRB);
  }
  
 
  
  for(int portIndex=0; portIndex<portsUsedInSubframe.length(); portIndex++)
  {
    //referenceSignal.rsSequencePerRB(portIndex).set_size(0,0);
    referenceSignal.rsSequencePerRB(portIndex).set_length(nRB);
    for(int rb_cnt=0;rb_cnt<nRB;rb_cnt++)
      referenceSignal.rsSequencePerRB(portIndex)(rb_cnt).set_length(0);
    
    // If the given port number is 5 0r 7-14 then the DMRS reference signal will be filled
    if(portsUsedInSubframe(portIndex)==5 || (portsUsedInSubframe(portIndex)>=7 && portsUsedInSubframe(portIndex)<=14))
    {
        for(int usr=0; usr<dataBlkInfo.length(); usr++)
        {	
            if(find(dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.DeModPorts, portsUsedInSubframe(portIndex))!=-1 && dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.DeModrsType==_RSTYPE_DMRS_)
            {
                imat rsPositionMatrix=dataBlkInfo(usr).dataGenInfo.rbInfo.rsPositionMatrix;
                ivec portsUsedInScheduledRBs=dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.rsPorts;
                int portReferenceNumber=dataBlkInfo(usr).dataGenInfo.rbInfo.getPortReferenceNumber(portsUsedInSubframe(portIndex));
                rsCountPerSym=find(rsPositionMatrix,portReferenceNumber,true).length();
                // if(referenceSignal.rsSequencePerRB(portIndex).size()==0)
                //  referenceSignal.rsSequencePerRB(portIndex)=zeros_c(nRB,rsCountPerSym);
                temp1=getDMRSsequence(cellID, nRNTI(usr), 0 ,subFrameIndexInFrame, portsUsedInSubframe(portIndex), nRB, dataBlkInfo(usr).resourceBlocks);
                temp2=reshape(temp1,rsCountPerSym,temp1.length()/rsCountPerSym).transpose();
                for(int r=0; r<temp2.rows(); r++)	    {
                    append(referenceSignal.rsSequencePerRB(portIndex)(dataBlkInfo(usr).resourceBlocks(r)),temp2.get_row(r));
                    //.set_row(dataBlkInfo(usr).resourceBlocks(r),temp2.get_row(r));
                }
            } 
        }
    }
    // If the given port number is 1000-1011 then the DMRS reference signal for NR will be filled
  else if((portsUsedInSubframe(portIndex)>=1000 && portsUsedInSubframe(portIndex)<=1007) || (portsUsedInSubframe(portIndex)>=1100 && portsUsedInSubframe(portIndex)<=1111))
  {	//referenceSignal.rsSequencePerRB(portIndex).set_length(nRB);
        
      for(int usr=0; usr<dataBlkInfo.length(); usr++)
      {	
	if(find(dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.DeModPorts, portsUsedInSubframe(portIndex))!=-1 && dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.DeModrsType==_RSTYPE_DMRS_)
	{ 
        int symno, FDCDM=1, TDCDM=1;
        int dmrstype=dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.dmrsType;
        int nCDM=dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.nCDM;
        
      ivec cuVec, ruVec;
	  imat rsPositionMatrix=dataBlkInfo(usr).dataGenInfo.rbInfo.rsPositionMatrix;
	  ivec portsUsedInScheduledRBs=dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.rsPorts;
	  int portReferenceNumber=dataBlkInfo(usr).dataGenInfo.rbInfo.getPortReferenceNumber(portsUsedInSubframe(portIndex));
	  
	  cuVec=cvectorize(rsPositionMatrix);
	  int i=find(cuVec,portReferenceNumber);
	  i++;
	  while(1)
	  {if(cuVec(i)==portReferenceNumber)
	    {
	      FDCDM++;
	      i++;
	    }
	    else
	      break;
	  }
	  ruVec=rvectorize(rsPositionMatrix);
	  int j = find(ruVec,portReferenceNumber);
	  j++;
	  while(1)
	  {if(ruVec(j)==portReferenceNumber)
	    {
	      TDCDM++;
	      j++;
	    }
	    else
	      break;
	  }
	  // 	  cout<<"portReferenceNumber : "<<portReferenceNumber<<endl;
	  int rsCountPerRB=find(rsPositionMatrix,portReferenceNumber,true).length();
// 	  cout<<"find(rsPositionMatrix,portReferenceNumber,true) : "<<find(rsPositionMatrix,portReferenceNumber,true)<<endl;
// 	  cout<<"find(rsPositionMatrix,portReferenceNumber,true).length() : "<<find(rsPositionMatrix,portReferenceNumber,true).length()<<endl;
	 // if(referenceSignal.rsSequencePerRB(portIndex).cols()<rsCountPerRB)
	   // appendHorizontal(referenceSignal.rsSequencePerRB(portIndex),zeros_c(nRB,rsCountPerRB-referenceSignal.rsSequencePerRB(portIndex).cols()));
	  ivec rslocationsinRB=find(rsPositionMatrix,portReferenceNumber,true);
	  //int rsCountPerRBPerUsr=find(rsPositionMatrix.get_cols(dataBlkInfo(usr).symPos),portReferenceNumber,true).length();
	  // 	  
	   ivec dmrsSym(0);
// 	   int rsCountPerSymbol;
// 	   if((rslocationsinRB(0)+1)!=rslocationsinRB(1))
// 	     rsCountPerSymbol=6;
// 	   else
// 	     rsCountPerSymbol=4;
 	   ivec rslocationinsymbol(0);
// 	   ivec rslocationsinRBperUsr=find(rsPositionMatrix.get_cols(dataBlkInfo(usr).symPos),portReferenceNumber,true);
 	   int rsCountPerSymbolPerUsr;
// 	   if((rslocationsinRBperUsr(0)+1)!=rslocationsinRBperUsr(1))
// 	     rsCountPerSymbolPerUsr=6;
// 	   else
// 	     rsCountPerSymbolPerUsr=4;
	   
	   for(int col=0;col<rsPositionMatrix.cols();col++)
	    if(find(rsPositionMatrix.get_col(col),portReferenceNumber)!=-1)
	      append(dmrsSym,col);
	   //  cout<<" sym len :"<<dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.rsSymPosition.length()<<endl;
	   ivec dmrsSymIndx=find(dmrsSym,dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.rsSymPosition(find(portsUsedInScheduledRBs,portsUsedInSubframe(portIndex))));
	  ivec dmrsSymbolspositions=dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.rsSymPosition(find(portsUsedInScheduledRBs,portsUsedInSubframe(portIndex)));
	  int dmrsSeqStart=dmrsSymIndx(0)*6;
// 	  cout<<"dataBlkInfo(usr).resourceBlocksrsCountPerRBPerUsr : "<<dataBlkInfo(usr).resourceBlocks<<endl;
	
	  for(int symbolno = 0;symbolno<dmrsSym.length();symbolno++)
	  { 
          rslocationinsymbol=find(rsPositionMatrix.get_col(dmrsSym(symbolno)),portReferenceNumber,true);
          rsCountPerSymbolPerUsr=rslocationinsymbol.length();
	    
        if(TDCDM!=1)
            symno=symbolno%TDCDM;
	    else
            symno=0;
	    
	    temp1=getNRDMRSsequence(cellID, nRNTI(usr), 0 ,subFrameIndexInFrame, portsUsedInSubframe(portIndex), nRB, dataBlkInfo(usr).resourceBlocks, rsCountPerSymbolPerUsr, dmrsSym(symbolno), symno, dmrstype, nCDM);
        temp2=(reshape(temp1,rsCountPerSymbolPerUsr,temp1.length()/rsCountPerSymbolPerUsr)).T();
	    for(int r=0; r<temp2.rows(); r++)	    
	  { 
	    append(referenceSignal.rsSequencePerRB(portIndex)(dataBlkInfo(usr).resourceBlocks(r)),temp2.get_row(r));
	    //referenceSignal.rsSequencePerRB(portIndex).set_submatrix(dataBlkInfo(usr).resourceBlocks(r),dmrsSeqStart,temp2.get_rows(r,r));
	  }
	    
	  }
	} 
      }
    }
    
    else if(portsUsedInSubframe(portIndex)>=1200 && portsUsedInSubframe(portIndex)<=1201)
    {
      for(int usr=0; usr<dataBlkInfo.length(); usr++)
      { 
	if(find(dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.PtrsPorts, portsUsedInSubframe(portIndex))!=-1)
	{  int ptrsrb; 	
	  imat rsPositionMatrix=dataBlkInfo(usr).dataGenInfo.rbInfo.rsPositionMatrix;
	  ivec portsUsedInScheduledRBs=dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.rsPorts;
	  int portReferenceNumber=dataBlkInfo(usr).dataGenInfo.rbInfo.getPortReferenceNumber(portsUsedInSubframe(portIndex));
	  for(int loop =0; loop<portsUsedInScheduledRBs.length();loop++)
	  {
	    if(portsUsedInScheduledRBs(loop)==portReferenceNumber)
	    { ptrsrb=loop;
	      break;	      
	    }
	  }  
	  int rsCountPerRB=find(rsPositionMatrix,portReferenceNumber,true).length();
	  ivec rslocationsinRB=find(rsPositionMatrix,portReferenceNumber,true);
	  ivec PTRSsym(0);
	  int rsCountPerSymbol=1;
	  ivec rslocationsinRBperUsr=find(rsPositionMatrix.get_cols(dataBlkInfo(usr).symPos),portReferenceNumber,true);
	  int rsCountPerSymbolPerUsr=1;
	  bool TPenabled=1;
	  
	  for(int col=0;col<rsPositionMatrix.cols();col++)
	    if(find(rsPositionMatrix.get_col(col),portReferenceNumber)!=-1)
	      append(PTRSsym,col);
	    ivec PTRSSymbolspositions=findUniqueNumbers(rslocationsinRB/12) ;
	  
	  if(TPenabled)
	  {
	    for(int symbolno = 0;symbolno<PTRSSymbolspositions.length();symbolno++)
	    {
	      temp1=getPTRSsequenceTPenabled(cellID, PTRSSymbolspositions(symbolno), subFrameIndexInFrame, rsCountPerSymbolPerUsr, nRNTI(usr), dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.rsrbpos(ptrsrb).length(), nRB, _MODULATION_SCHEME_QPSK_);
	      temp2=reshape(temp1,temp1.length()/rsCountPerSymbolPerUsr,rsCountPerSymbolPerUsr);
	      
	      for(int r=0; r<temp2.rows(); r++)
	      { 
		//cout<<"resourceBlocks rows()"<<"\t"<<dataBlkInfo(usr).resourceBlocks.length()<<"\tr\t"<<r<<endl;
		append(referenceSignal.rsSequencePerRB(portIndex)(dataBlkInfo(usr).resourceBlocks(r)),temp2.get_row(r));
		//referenceSignal.rsSequencePerRB(portIndex).set_submatrix(dataBlkInfo(usr).resourceBlocks(r),dmrsSeqStart,temp2.get_rows(r,r));
	      }	    
	    }
	  }
	  else
	  {
	    ivec PTRSSymIndx=find(PTRSsym,dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.rsSymPosition(find(portsUsedInScheduledRBs,portsUsedInSubframe(portIndex))));
	    // ivec PTRSSymbolspositions=dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.rsSymPosition(find(portsUsedInScheduledRBs,portsUsedInSubframe(portIndex)));
	    int PTRSSeqStart=PTRSSymIndx(0)*1;
	    for(int symbolno = 0;symbolno<PTRSSymbolspositions.length();symbolno++)
	    {
	      temp1=getNRDMRSsequence(cellID, nRNTI(usr), 0, subFrameIndexInFrame, dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.PTRSassocDMRSPorts(portsUsedInSubframe(portIndex)%1200), nRB, dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.rsrbpos(ptrsrb), rsCountPerSymbolPerUsr, PTRSSymbolspositions(symbolno),0, dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.dmrsType, dataBlkInfo(usr).dataGenInfo.rbInfo.rsInfo.nCDM);
	      temp2=reshape(temp1,temp1.length()/rsCountPerSymbolPerUsr,rsCountPerSymbolPerUsr);
	      for(int r=0; r<temp2.rows(); r++)
	      { 
		cout<<"usr\t"<<usr<<"dataBlkInfo(usr).resourceBlocks length:\t"<<dataBlkInfo(usr).resourceBlocks.length()<<"\t(dataBlkInfo(usr).resourceBlocks(r)):\t"<<(dataBlkInfo(usr).resourceBlocks(r))<<"\ttemp2.get_row(r):\t"<<temp2.get_row(r)<<"\ttemp2.get_row(r) length\t"<<temp2.rows()<<endl;
		append(referenceSignal.rsSequencePerRB(portIndex)(dataBlkInfo(usr).resourceBlocks(r)),temp2.get_row(r));
		//referenceSignal.rsSequencePerRB(portIndex).set_submatrix(dataBlkInfo(usr).resourceBlocks(r),dmrsSeqStart,temp2.get_rows(r,r));
	      }
	    }
	  }
	}
      }
    }
    
    // If the given port is between 0-3 then CRS reference signal will be arranged wrt to the symbol number
    else if(isDownlink && portsUsedInSubframe(portIndex)>=0 && portsUsedInSubframe(portIndex)<=3)/// CRS
    {
      imat rsPositionMatrix=crsRBInfo.rsPositionMatrix;
      int portReferenceNumber=crsRBInfo.getPortReferenceNumber(portsUsedInSubframe(portIndex));
      sym1=0;
      int crsIndex=find(crsPorts, portsUsedInSubframe(portIndex));
      for(int sym_cnt=0;sym_cnt<rsPositionMatrix.cols();sym_cnt++)
      {
	rsCountPerSym=find(rsPositionMatrix.get_col(sym_cnt),portReferenceNumber,true).length();
	// if the reference symbol position exist in the particular colomn the if statement executes
	if(rsCountPerSym!=0)
	{
	  temp1 = getCRSsequence(cellID, (sym_cnt%(rsPositionMatrix.cols()/nSlotsPerRB)), floor_i(sym_cnt/(rsPositionMatrix.cols()/nSlotsPerRB)), nRB);
	  append(referenceSignal.CRSsymbolsPerPort(crsIndex), sym_cnt);
	  referenceSignal.CRSsequencePerPortPerSymbol(crsIndex)(sym1) = temp1;sym1++;
	  temp2=reshape(temp1,rsCountPerSym,temp1.length()/rsCountPerSym).transpose();
	  // appends the reference symbols of [0,4,7,11] colomns for total bandwidth 
	  //appendHorizontal(referenceSignal.rsSequencePerRB(portIndex), temp2);
	  for(int r=0; r<temp2.rows(); r++)	    
	  append(referenceSignal.rsSequencePerRB(portIndex)(r),temp2.get_row(r));
	  
	  	  
	}
      }
     // cout << referenceSignal.CRSsymbolsPerPort << endl;abort();
    }
//     // If theappend given port is between 15-22 then CSI-RS reference signal will be arranged wrt to the symbol number
    else if(isDownlink && portsUsedInSubframe(portIndex)>=15 && portsUsedInSubframe(portIndex)<=22)/// CSI-RS
    {
      imat rsPositionMatrix=crsRBInfo.rsPositionMatrix;
      
      int portReferenceNumber=crsRBInfo.getPortReferenceNumber(portsUsedInSubframe(portIndex));
     sym2=0;
      int csirsIndex=find(CSIRSports, portsUsedInSubframe(portIndex));
      for(int sym_cnt=0;sym_cnt<rsPositionMatrix.cols();sym_cnt++)
      {
	rsCountPerSym=find(rsPositionMatrix.get_col(sym_cnt),portReferenceNumber,true).length();
	// if the CSI-RS reference symbol position exist in the particular colomn the if statement executes
	if(rsCountPerSym!=0)
	{
	  temp1 = getCSIRSsequence(cellID, (sym_cnt%(rsPositionMatrix.cols()/nSlotsPerRB)), floor_i(sym_cnt/(rsPositionMatrix.cols()/nSlotsPerRB)), nRB);
	  if((portsUsedInSubframe(portIndex)==16 || portsUsedInSubframe(portIndex)==18 ||portsUsedInSubframe(portIndex)==20 ||portsUsedInSubframe(portIndex)==22) && (sym2==1))
	    temp1 = -temp1;
	  append(referenceSignal.CSIRSsymbolsPerPort(csirsIndex), sym_cnt);
	  referenceSignal.CSIRSsequencePerPortPerSymbol(csirsIndex)(sym2) = temp1;sym2++;
	  temp2=reshape(temp1,rsCountPerSym,temp1.length()/rsCountPerSym).transpose();
	  for(int r=0; r<temp2.rows(); r++)	    
	    append(referenceSignal.rsSequencePerRB(portIndex)(r),temp2.get_row(r));
	}
      }
    }
    else if(isDownlink && portsUsedInSubframe(portIndex)>=3100 && portsUsedInSubframe(portIndex)<=3432)/// CSI-RS-NR
    { int symno;
      int FDCDM=1;
      int TDCDM=1;
      ivec cuVec;
      ivec ruVec;
      int symswithcsirs=0;
      imat rsPositionMatrix=crsRBInfo.rsPositionMatrix;
      int portReferenceNumber=crsRBInfo.getPortReferenceNumber(portsUsedInSubframe(portIndex));
      //for(FDCDM=0;;FDCDM++)
     cuVec=cvectorize(rsPositionMatrix);
      int i=find(cuVec,portReferenceNumber);
     i++;
     while(1)
     {if(cuVec(i)==portReferenceNumber)
       {
	 FDCDM++;
	 i++;
       }
     else
       break;
     }
	ruVec=rvectorize(rsPositionMatrix);
	int j = find(ruVec,portReferenceNumber);
	j++;
	while(1)
	{if(ruVec(j)==portReferenceNumber)
	  {
	    TDCDM++;
	    j++;
	  }
	  else
	    break;
	}
	sym2=0;
	for(int sym_cnt=0;sym_cnt<rsPositionMatrix.cols();sym_cnt++)
	{
	  rsCountPerSym=find(rsPositionMatrix.get_col(sym_cnt),portReferenceNumber,true).length();
	  if(rsCountPerSym!=0)
	  symswithcsirs++;  
	}
	 int csirsIndex=find(CSIRSports, portsUsedInSubframe(portIndex));
	referenceSignal.CSIRSsequencePerPortPerSymbol(csirsIndex).set_length(symswithcsirs);
     
      /*if(TDCDM==symswithcsirs)
      referenceSignal.CSIRSsequencePerPortPerSymbol(csirsIndex).set_length(TDCDM);
      else
	referenceSignal.CSIRSsequencePerPortPerSymbol(csirsIndex).set_length(TDCDM*symswithcsirs);
      */
      for(int sym_cnt=0;sym_cnt<rsPositionMatrix.cols();sym_cnt++)
      {rsCountPerSym=find(rsPositionMatrix.get_col(sym_cnt),portReferenceNumber,true).length();
	// if the CSI-RS reference symbol position exist in the particular colomn the if statement executes
	if(rsCountPerSym!=0)
	{  if(TDCDM!=1)
	    symno=sym2%TDCDM;
	  else
	    symno=0;
	  temp1 = getNRCSIRSsequence(cellID, (sym_cnt%(rsPositionMatrix.cols()/nSlotsPerRB)), floor_i(sym_cnt/(rsPositionMatrix.cols()/nSlotsPerRB)), nRB, rsCountPerSym, symno, FDCDM, TDCDM, portsUsedInSubframe(portIndex));
// 	  if((portsUsedInSubframe(portIndex)%2==1) && (sym2==1))
// 	    temp1 = -temp1;
	  append(referenceSignal.CSIRSsymbolsPerPort(csirsIndex), sym_cnt);
	 referenceSignal.CSIRSsequencePerPortPerSymbol(csirsIndex)(sym2).set_length(temp1.length());
	  referenceSignal.CSIRSsequencePerPortPerSymbol(csirsIndex)(sym2) = temp1;
	  sym2++;
	  //temp2=reshape(temp1,nRB,temp1.length()/nRB);
	  temp2=(reshape(temp1,temp1.length()/nRB,nRB)).T();
	  //appendHorizontal(referenceSignal.rsSequencePerRB(portIndex), temp2);
	  for(int r=0; r<temp2.rows(); r++)	    
	  {append(referenceSignal.rsSequencePerRB(portIndex)(r),temp2.get_row(r));
	   }


	  }
      }
    }
    
   //Abhichanges
    // If the given port is between 15-22 and multiple CSI-RSs are configured then CSI-RS reference signal will be arranged wrt to the symbol number
    else if(isDownlink && int(portsUsedInSubframe(portIndex)/100)>=15 && int(portsUsedInSubframe(portIndex)/100)<=22)/// CSI-RS multiple configurations
    {
      imat rsPositionMatrix=crsRBInfo.rsPositionMatrix;
      int portReferenceNumber=crsRBInfo.getPortReferenceNumber(portsUsedInSubframe(portIndex));
      
      sym2=0;
      int csirsIndex=find(CSIRSports, portsUsedInSubframe(portIndex));
      int csirsPort = (portsUsedInSubframe(portIndex)/100);
      for(int sym_cnt=0;sym_cnt<rsPositionMatrix.cols();sym_cnt++)
      {
	rsCountPerSym=find(rsPositionMatrix.get_col(sym_cnt),portReferenceNumber,true).length();
	// if the CSI-RS reference symbol position exist in the particular colomn the if statement executes
	if(rsCountPerSym!=0)
	{
	  temp1 = getCSIRSsequence(cellID, (sym_cnt%(rsPositionMatrix.cols()/nSlotsPerRB)), floor_i(sym_cnt/(rsPositionMatrix.cols()/nSlotsPerRB)), nRB);
	  
	  if((csirsPort==16 || csirsPort ==18 || csirsPort==20 || csirsPort==22) && (sym2==1))
	    temp1 = -temp1;
	  append(referenceSignal.CSIRSsymbolsPerPort(csirsIndex), sym_cnt);
	  referenceSignal.CSIRSsequencePerPortPerSymbol(csirsIndex)(sym2) = temp1;sym2++;
	  temp2=reshape(temp1,rsCountPerSym,temp1.length()/rsCountPerSym).transpose();
	  //appendHorizontal(referenceSignal.rsSequencePerRB(portIndex), temp2);
	  for(int r=0; r<temp2.rows(); r++)	    
	    append(referenceSignal.rsSequencePerRB(portIndex)(r),temp2.get_row(r));
	  
	}
      }
    }
    else if(isDownlink && portsUsedInSubframe(portIndex)==61 || portsUsedInSubframe(portIndex)==62)/// PRS
    {
      imat rsPositionMatrix=crsRBInfo.rsPositionMatrix;
      int portReferenceNumber=crsRBInfo.getPortReferenceNumber(portsUsedInSubframe(portIndex));
      for(int sym_cnt=0;sym_cnt<rsPositionMatrix.cols();sym_cnt++)
      {
	rsCountPerSym=find(rsPositionMatrix.get_col(sym_cnt),portReferenceNumber,true).length();
	// if the reference symbol position exist in the particular colomn the if statement executes
	if(rsCountPerSym!=0)
	{
	  temp1 = getPRSsequence(cellID, (sym_cnt%(rsPositionMatrix.cols()/nSlotsPerRB)), floor_i(sym_cnt/(rsPositionMatrix.cols()/nSlotsPerRB)), nRB);
	  temp2=reshape(temp1,rsCountPerSym,temp1.length()/rsCountPerSym).transpose();
	  // appends the reference symbols of [0,4,7,11] colomns for total bandwidth 
	  //appendHorizontal(referenceSignal.rsSequencePerRB(portIndex), temp2);
	  for(int r=0; r<temp2.rows(); r++)	    
	    append(referenceSignal.rsSequencePerRB(portIndex)(r),temp2.get_row(r));
	  
	}
      }
    }
  }
}
/*! \fn getOrthoCoverCode(...)
* \brief Generates Orhogonal Cover Codes (OCC) for the RS ports which share same locations in RB.
* \param portIndex ---> DMRS ports {7,8,11,13} and {9,10.12.14}, CSIRS ports {15,16}, {17,18}, {19,20}, and {21,22} use same locations in RB.
* \return Orthogonal cover code vector
*/
Array<ivec> getNROrthoCoverCode(int portIndex)
{
 Array<ivec> orthoCoverCode;
  if((portIndex>=1100) && (portIndex<=1111))
  {
    orthoCoverCode.set_length(2);
    orthoCoverCode(0).set_length(2);
    orthoCoverCode(1).set_length(2);
    switch(portIndex)
    {
	case 1100:
	case 1102:
	case 1104:
	  orthoCoverCode(0)="1 1";
	  orthoCoverCode(1)="1 1";
	  break;
	case 1101:
	case 1103:
	case 1105:
	  orthoCoverCode(0)="1 1";
	  orthoCoverCode(1)="-1 -1";
	  break;
	case 1106:
	case 1108:
	case 1110:
	  orthoCoverCode(0)="+1 -1";
	  orthoCoverCode(1)="+1 -1";
	  break;
	case 1107:
	case 1109:
	case 1111:
	  orthoCoverCode(0)="+1 -1";
	  orthoCoverCode(1)="-1 +1";
	  break;
	default:
	  cout<<"[both:] Undefined portIndex, exiting in ....getNROrthoCoverCode()"<<endl;abort();
    } 
   }
 
 
 // generates orthogonal Cover Code for UE-specific reference signals from port-1000 to port-1007
  if((portIndex>=1000) && (portIndex<=1007))
  {
    orthoCoverCode.set_length(2);
    orthoCoverCode(0).set_length(2);
    orthoCoverCode(1).set_length(2);
    switch(portIndex)
    {
	case 1000:
	case 1002:
	  orthoCoverCode(0)="1 1";
	  orthoCoverCode(1)="1 1";
	  break;
	case 1001:
	case 1003:
	  orthoCoverCode(0)="1 1";
	  orthoCoverCode(1)="-1 -1";
	  break;
	case 1004:
	case 1006:
	  orthoCoverCode(0)="+1 -1";
	  orthoCoverCode(1)="+1 -1";
	  break;
	case 1005:
	case 1007:
	  orthoCoverCode(0)="+1 -1";
	  orthoCoverCode(1)="-1 +1";
	  break;
	default:
	  cout<<"[both:] Undefined portIndex, exiting in ....getNROrthoCoverCode()"<<endl;abort();
    }
  }
  
  
   return orthoCoverCode;
}
Array<ivec> getNROrthoCoverCodeforCSIRS(int portIndex,int FDCDM, int TDCDM)
{
  Array<ivec> orthoCoverCode;
  orthoCoverCode.set_length(FDCDM);
  
  if(FDCDM == 1 && TDCDM == 1)
  {orthoCoverCode(0).set_length(TDCDM);
   orthoCoverCode(0)="1";
  }
  if(FDCDM == 2 && TDCDM == 1)
  {orthoCoverCode(0).set_length(TDCDM);
    orthoCoverCode(1).set_length(TDCDM);
    if(portIndex%2==0)
    {
      orthoCoverCode(0)="1";
      orthoCoverCode(1)="1";
    }
   else
   {
     orthoCoverCode(0)="1";
     orthoCoverCode(1)="-1";
   }
  }
  if(FDCDM == 2 && TDCDM == 2)
  {orthoCoverCode(0).set_length(TDCDM);
    orthoCoverCode(1).set_length(TDCDM);
  if(portIndex == 3400 || portIndex == 3404 || portIndex == 3408 || portIndex == 3412 || portIndex == 3416 || portIndex == 3420 || portIndex == 3424 || portIndex == 3428)
  {orthoCoverCode(0)="1 1";
    orthoCoverCode(1)="1 1";
  }
  if(portIndex == 3401 || portIndex == 3405 || portIndex == 3409 || portIndex == 3413 || portIndex == 3417 || portIndex == 3421 || portIndex == 3425 || portIndex == 3429)
  {orthoCoverCode(0)="1 1";
    orthoCoverCode(1)="-1 -1";
  }
  if(portIndex == 3402 || portIndex == 3406 || portIndex == 3410 || portIndex == 3414 || portIndex == 3418 || portIndex == 3422 || portIndex == 3426 || portIndex == 3430)
  {orthoCoverCode(0)="1 -1";
    orthoCoverCode(1)="1 -1";
  }
  if(portIndex == 3403 || portIndex == 3407 || portIndex == 3411 || portIndex == 3415 || portIndex == 3419 || portIndex == 3423 || portIndex == 3427 || portIndex == 3431)
  {orthoCoverCode(0)="1 -1";
    orthoCoverCode(1)="-1 1";
  }
  }
  return orthoCoverCode;
}

/*! \fn getOrthoCoverCode(...)
 * \brief Generates Orhogonal Cover Codes (OCC) for the RS ports which share same locations in RB.
 * \param portIndex ---> DMRS ports {7,8,11,13} and {9,10.12.14}, CSIRS ports {15,16}, {17,18}, {19,20}, and {21,22} use same locations in RB.
 * \return Orthogonal cover code vector
 */
ivec getOrthoCoverCode(int portIndex)
{
  ivec orthoCoverCode;
  // generates orthogonal Cover Code for UE-specific reference signals from port-7 to port-15
  if((portIndex>=7) && (portIndex<15))
  {
    orthoCoverCode.set_length(4);
    switch(portIndex)
    {
      case 7:
      case 9:
	orthoCoverCode = "1 1 1 1";
	break;
      case 8:
      case 10:
	orthoCoverCode = "1 -1 1 -1";
	break;
      case 11:
	orthoCoverCode = "1 1 -1 -1";
	break;
      case 12:
	orthoCoverCode = "-1 -1 1 1";
	break;
      case 13:
	orthoCoverCode = "1 -1 -1 1";
	break;
      case 14:
	orthoCoverCode = "-1 1 1 -1";
	break;
      default:
	cout<<"[both:] Undefined portIndex, exiting in ....getOrthoCoverCode()"<<endl;abort();
    }
  }
   // generates orthogonal cover code for CSI-RS reference signals
  else if(((portIndex==15) || (portIndex>=17) || (portIndex==19) || (portIndex>=21)) && portIndex<1000)
  {
    orthoCoverCode.set_length(2);
    orthoCoverCode = "1 1";
  }
  else if(((portIndex==16) || (portIndex>=18) || (portIndex==20) || (portIndex>=22)) && portIndex<1000)
  {
    orthoCoverCode.set_length(2);
    orthoCoverCode = "1 -1";
    
  }
  else
  {
    cout<<"[both:] Orhogonal Cover Codes are not used for the given portIndex"<<endl;abort();
  }
  return orthoCoverCode;
}


/*! \fn fillRS(...)
* \brief returns reference signals for all reference signal types (after multiplying with port to antenna mapper) for particular transmission mode in one subframe .
* \param [in,out] referenceSignal ---> it is a class variable
* \param [in] subFrameNumber ---> subframe number in a frame
* \param [in] logicalRUIndex --->
* \param [in] RBinfo--->
* \param [in] rbSize --->
* \param [in] precoder ---> precoding matrix for UE-specific reference signals
* \return void
* \note Need to fix the variables rsInfo, nSymbolsperSlot, nRBs(per SF), nSlotsPerRB, nTxAntenna, portToPhysicalAntennaMapping
*/
/*! \fn fillRS(...)
* \brief returns reference signals for all reference signal types (after multiplying with port to antenna mapper) for particular transmission mode in one subframe .
* \return void
*/
// void fillRS(ReferenceSignal_S &referenceSignal, int logicalRUIndex, RBInfo_S RBinfo, Size_S rbSize ,Framehandler& framehandler, ResourceMapper* resourceMapper, PortToAntennaMapper_S *portToPhysicalAntennaMapper, bool isDownlink, cmat precoder)
// {
//   RSInfo_S rsInfo=RBinfo.rsInfo;
//   cmat portToPhyAntennaMapperForCQIports, precodedMapper;
//   cvec portToAntennaMapperForMyPort, rsSeq1;
//   Array<cvec> rsSequence(rsInfo.rsPorts.length());
//   int portNumber;
//   string toneType;
//   ivec portReferenceNumbers=RBinfo.getPortReferenceNumbers(rsInfo.rsPorts), portReferenceNumbers2, portReferenceNumbers1=findUniqueNumbers(portReferenceNumbers);
//   bvec isFilled=zeros_b(rsInfo.rsPorts.length());
//   portToPhyAntennaMapperForCQIports=portToPhysicalAntennaMapper->getPortToAntennaMapper(rsInfo.nCQIports,framehandler.frame.length());
//  // this loop executes for all possible ports for given transmission mode
//   for(int port_cnt=0;port_cnt<rsInfo.rsPorts.length();port_cnt++)
//   {
//     portNumber=rsInfo.rsPorts(port_cnt);
//     int portIndex=find(referenceSignal.portIndices,portNumber);
//     toneType="port"+toString(portNumber);
//     
//     // gives the index for reference symbol positions(Index starts from top to bottom) 
//     ivec row1stIndices=framehandler.getRow1stToneIndicesInRU(logicalRUIndex,toneType);
//     int reCountInSlot0=sum(row1stIndices<((rbSize.numberOfSymbols/2)*rbSize.numberOfSubcarriers));
//     int reCountInSlot1=row1stIndices.length()-reCountInSlot0;
//     ivec phyRB;
//     if(isDownlink)
//       phyRB=resourceMapper->dlResourceMapper.getPRBIndex(logicalRUIndex % resourceMapper->dlResourceMapper.getResourceBlocksPerSubframe());
//     else
//       phyRB=resourceMapper->ulResourceMapper.getPRBIndex(logicalRUIndex % resourceMapper->ulResourceMapper.getResourceBlocksPerSubframe());
//     
//     // gives the reference symbols for all the ports for one subframe
// 
//    // cout<<"\n REcount:"<< reCountInSlot0<<'\t'<<reCountInSlot1<<'\t'<<portIndex;
//     //cout<<'\n'<<referenceSignal.rsSequencePerRB(portIndex)(phyRB(0));
//     
//     if(referenceSignal.rsSequencePerRB(portIndex)(phyRB(0)).length())
//     {
//       isFilled(port_cnt)=true;
//       rsSequence(port_cnt)=concat((referenceSignal.rsSequencePerRB(portIndex)(phyRB(0))).left(reCountInSlot0), (referenceSignal.rsSequencePerRB(portIndex)(phyRB(1))).right(reCountInSlot1));
//     }
//   }
//   // If the antenna ports are DMRS ports  
//   if(rsInfo.DeModrsType==_RSTYPE_DMRS_)
//   {
//       if(precoder.size()==0)
// 	precodedMapper=portToPhyAntennaMapperForCQIports;
//       else
// 	precodedMapper=portToPhyAntennaMapperForCQIports*precoder;
//   }
//     
//   for(int port_cnt=0;port_cnt<rsInfo.rsPorts.length();port_cnt++)
//     if(isFilled(port_cnt) )
//   {
//     Array<cvec> allPortSequence;  
//     Array<cvec> physicalAntennaRS(framehandler.frame.length());
//     
//     
//     ivec reps;
//     int nPorts;
//     portNumber=rsInfo.rsPorts(port_cnt);
//     toneType="port"+toString(portNumber);
//     /* for mode-1 to mode-6 this if statement returns the CRS referenceSignal for one RB after multiplying with portToPhyAntennaMapperForCQIports and for
//      mode-7 to mode-9 it returns the CSI-RS reference symbols for one RB after multiplying with portToPhyAntennaMapper */
//     if(isDownlink && find(rsInfo.CQIports, rsInfo.rsPorts(port_cnt))!=-1)/// CSI-RS/CRS
//     {
//       nPorts=rsInfo.nCQIports; // number of CQI ports for particular mode
//       allPortSequence.set_size(nPorts);
//       portReferenceNumbers2=RBinfo.getPortReferenceNumbers(rsInfo.CQIports); // gives the port reference number for CQI ports
//       rsSeq1.set_size(nPorts);
//       reps=find(portReferenceNumbers2, portReferenceNumbers(port_cnt), true);
//       // gives the Reference signals for desired port and fills zeros for other ports
//       for(int p=0; p<nPorts; p++)
// 	if(find(reps, p)!=-1)
// 	  allPortSequence(p)=rsSequence(find(rsInfo.rsPorts, rsInfo.CQIports(p)));
// 	else
// 	  allPortSequence(p)=zeros_c(rsSequence(port_cnt).length());
//       // this loop has the port to antenna mapper multipled CRS reference signal at one port in one RB (Including zeros for filling other port CRS locations)
//       for(int ant_cnt=0;ant_cnt<physicalAntennaRS.length();ant_cnt++)
//       {
// 	physicalAntennaRS(ant_cnt).set_length(rsSequence(port_cnt).length());
// 	for(int sym3=0; sym3<rsSequence(port_cnt).length(); sym3++)
// 	{
// 	  for(int p=0; p<nPorts; p++)
// 	    rsSeq1(p)=allPortSequence(p)(sym3);
// 	  //multiplying the CQI port reference sequence with the port to antenna mapper
// 	    physicalAntennaRS(ant_cnt)(sym3)=sum(elem_mult(rsSeq1, portToPhyAntennaMapperForCQIports.get_row(ant_cnt)))*sqrt(inv_dB(rsInfo.rsBoostInDB));	    
// 	}
//       }
//     }
//     // for mode-7 to mode-9 this statement returns the DMRS referenceSignal for one RB after multiplying with portToPhyAntennaMapperForCQIports
//     else if(rsInfo.DeModrsType==_RSTYPE_DMRS_ && (rsInfo.rsPorts(port_cnt)==5 || ( rsInfo.rsPorts(port_cnt)>=7 && rsInfo.rsPorts(port_cnt)<=14) || ( rsInfo.rsPorts(port_cnt)>=1000 && rsInfo.rsPorts(port_cnt)<=1011)))// DMRS
//     {
//       nPorts=rsInfo.DeModPorts.length();// number of CQI ports for particular mode
//       allPortSequence.set_size(nPorts);
//       portReferenceNumbers2=RBinfo.getPortReferenceNumbers(rsInfo.DeModPorts);// gives the port reference number for DMRS ports
//       rsSeq1.set_size(nPorts);
//       reps=find(portReferenceNumbers2, portReferenceNumbers(port_cnt), true);
//     
//       // gives the Reference signals for desired port and fills zeros for other ports
//       for(int p=0; p<nPorts; p++)
// 	if(find(reps, p)!=-1)
// 	  allPortSequence(p)=rsSequence(find(rsInfo.rsPorts, rsInfo.DeModPorts(p)));
// 	else
// 	  allPortSequence(p)=zeros_c(rsSequence(port_cnt).length());
// 	// this loop has the precoding multipled DMRS reference signal at one port in one RB (Including zeros for filling other port CRS locations)
// 	for(int ant_cnt=0;ant_cnt<physicalAntennaRS.length();ant_cnt++)
// 	{
// 	  physicalAntennaRS(ant_cnt).set_length(rsSequence(port_cnt).length());
// 	  for(int sym3=0; sym3<rsSequence(port_cnt).length(); sym3++)
// 	  {
// 	    for(int p=0; p<nPorts; p++)
// 	      rsSeq1(p)=allPortSequence(p)(sym3);
// 	    //multiplying the DMRS port reference sequence with the precoder and port to antenna mapper
// 	    physicalAntennaRS(ant_cnt)(sym3)=sum(elem_mult(rsSeq1, precodedMapper.get_row(ant_cnt)))*sqrt(inv_dB(rsInfo.rsBoostInDB));
// 	  }
// 	}
//     }
//     // for mode-7 to mode-9 this statement returns the CRS referenceSignal for one RB after multiplying with portToPhyAntennaMapperForCQIports
//     else if(isDownlink && rsInfo.CQIrsType!=_RSTYPE_CRS_ && rsInfo.rsPorts(port_cnt)>=0 && rsInfo.rsPorts(port_cnt)<=3)/// CRS
//     {
//       nPorts=rsInfo.crsPorts.length(); // number of CRS ports for particular mode
//       allPortSequence.set_size(nPorts);
//       portReferenceNumbers2=RBinfo.getPortReferenceNumbers(rsInfo.crsPorts);// gives the port reference number for CRS ports
//       rsSeq1.set_size(nPorts);
//       reps=find(portReferenceNumbers2, portReferenceNumbers(port_cnt), true);
//       // gives the Reference signals for desired port and fills zeros for other ports
//       for(int p=0; p<nPorts; p++)
// 	if(find(reps, p)!=-1)
// 	  allPortSequence(p)=rsSequence(find(rsInfo.rsPorts, rsInfo.crsPorts(p)));
// 	else
// 	  allPortSequence(p)=zeros_c(rsSequence(port_cnt).length());
//       // this loop has the port to antenna mapper multipled CRS reference signal at one port in one RB (Including zeros for filling other port CRS locations)
// 	for(int ant_cnt=0;ant_cnt<physicalAntennaRS.length();ant_cnt++)
// 	{
// 	  physicalAntennaRS(ant_cnt).set_length(rsSequence(port_cnt).length());// sets the length of physicalAntennaRS(0) length to 8
// 	  for(int sym3=0; sym3<rsSequence(port_cnt).length(); sym3++)
// 	  {
// 	    
// 	    for(int p=0; p<nPorts; p++)
// 	      rsSeq1(p)=allPortSequence(p)(sym3);
// 	    // if the number of CRS ports and CQI ports are equal multiply the CRS symbols with port to antenna mapper
// 	    if(rsInfo.crsPorts.length()==rsInfo.nCQIports)
// 	      physicalAntennaRS(ant_cnt)(sym3)=sum(elem_mult(rsSeq1, portToPhyAntennaMapperForCQIports.get_row(ant_cnt)))*sqrt(inv_dB(rsInfo.rsBoostInDB));
// 	    // if the number of CRS ports and CQI ports are not equal append zeros to the CRS symbols and multiply the CRS symbols with port to antenna mapper
// 	    else
// 	    {
// 	      cvec rsSeq2(rsInfo.nCQIports);
// 	      rsSeq2 = concat(rsSeq1, zeros_c(rsInfo.nCQIports-rsInfo.crsPorts.length()));
// 	      physicalAntennaRS(ant_cnt)(sym3)=sum(elem_mult(rsSeq2, portToPhyAntennaMapperForCQIports.get_row(ant_cnt)))*sqrt(inv_dB(rsInfo.rsBoostInDB));
// 	    }
// 	  }
// 	}
//     }
//     //====================================================================================================
//     // Code to apply Downlink Power allocation (3gpp 36.213 - Section 5.2) difference between
//     // OFDM symbols with and without CRS
//     // Multiply the power mask on the CRS
//     //====================================================================================================
//     else if(isDownlink && (portNumber==61 || portNumber==62))
//     {
//       int rsIndx=find(referenceSignal.portIndices,rsInfo.rsPorts(port_cnt));
//       int prsReferenceNumber=RBinfo.getPortReferenceNumbers(to_ivec(portNumber))(0);
//       
//       // If the port-6 is not available for the given mode display the error
//       if(rsIndx==-1)
//       {
// 	cout<<"Reference signal for port "<<portNumber<<" not available in ReferenceSignal_S..."<<endl;
// 	return;
//       }
//       
//       string toneType="port"+toString(portNumber);
//       imat prsRBstructure=framehandler.getResourceUnitMatrix(toneType);
//       ivec port6Indices=find(prsRBstructure, prsReferenceNumber , true);
//       int reCountInSlot0=sum(port6Indices<((rbSize.numberOfSymbols/2)*rbSize.numberOfSubcarriers));
//       int reCountInSlot1=port6Indices.length()-reCountInSlot0;
//       ivec phyRB=resourceMapper->dlResourceMapper.getPRBIndex(logicalRUIndex);
//       cvec rsSequence(0);
//       append(rsSequence,(referenceSignal.rsSequencePerRB(rsIndx)(phyRB(0))).left(reCountInSlot0));
//       append(rsSequence,(referenceSignal.rsSequencePerRB(rsIndx)(phyRB(1))).right(reCountInSlot1));
//       cmat precodedMapper;
//       precodedMapper=portToPhyAntennaMapperForCQIports;
//       if(portNumber==61)
// 	portToAntennaMapperForMyPort=precodedMapper.get_col(0);
//       else
// 	portToAntennaMapperForMyPort=precodedMapper.get_col(1);
//       
//       // CQI ports are not equal append zeros to the CRS
//       for(int ant_cnt=0;ant_cnt<physicalAntennaRS.length();ant_cnt++)
// 	physicalAntennaRS(ant_cnt)=rsSequence*portToAntennaMapperForMyPort(ant_cnt)*sqrt(inv_dB(rsInfo.rsBoostInDB));
//     }  
//     
//     
//     if(isDownlink && RBinfo.dlPowMask.size()!=0)
//     {
//         ivec row1stIndices=framehandler.getRow1stToneIndicesInRU(logicalRUIndex,toneType);
//         for(int ant_cnt=0;ant_cnt<physicalAntennaRS.length();ant_cnt++)
//             for(int sym=0; sym<row1stIndices.length(); sym++) 
//                  physicalAntennaRS(ant_cnt)(sym) = physicalAntennaRS(ant_cnt)(sym) * sqrt(inv_dB(RBinfo.crsPowBoostInDB));
//     }
//     if(physicalAntennaRS(0).length())
//     {
//       if(find(rsInfo.CQIports, rsInfo.rsPorts(port_cnt))!=-1 && logicalRUIndex==0)
//       {
// 	cout<<"Fill Seq for "<<rsInfo.rsPorts(port_cnt)<<" in p1 : "<<  getAcrossArray(physicalAntennaRS,0)<<endl;
// 	cout<<"Fill Seq for "<<rsInfo.rsPorts(port_cnt)<<" in p2 : "<<  getAcrossArray(physicalAntennaRS,1)<<endl;
//       }
//       framehandler.setTonesInRU(physicalAntennaRS,logicalRUIndex,toneType,true);
//     }
//   }
//  
//   if(traceValuePerTraceName["referenceSignalGenerator_01_traceFlag"]=="true")
//   {
//     ivec cellSpecificSubcarrierUpShift(rsInfo.nCQIports), cellSpecificSymbolRightShift(rsInfo.nCQIports), RStones, portLocations;
//     int CSIRSconfig=0;
//     imat configPort;
//     Array<cmat> txRB;
//     string rbpath="../configFiles/LTERbPattern/LTERbPatternDL/";
//     getRSportShifts(cellSpecificSubcarrierUpShift, cellSpecificSymbolRightShift, rsInfo.CQIports, rsInfo.cellid, CSIRSconfig);
//     for(int port_cnt=0;port_cnt<rsInfo.nCQIports;port_cnt++)
//     {
//       string rbFileName=rbpath+"Port"+toString(rsInfo.CQIports(port_cnt))+".txt";
//       parse(rbFileName, "rbStructure", configPort);
//       if(rsInfo.CQIports(port_cnt)>=0 && rsInfo.CQIports(port_cnt)<4)
//       {
// 	txRB = framehandler.getTonesInRU(logicalRUIndex);
// 	portLocations = find(configPort, rsInfo.portReferenceNumbers(find(rsInfo.rsPorts, rsInfo.CQIports(port_cnt))), true);
// 	for(int i=0; i<portLocations.length()/2; i++)
// 	{
// 	  int symLoc = (portLocations(2*i)+cellSpecificSymbolRightShift(port_cnt))%(configPort.cols());
// 	  int SCloc = (portLocations(2*i+1)+cellSpecificSubcarrierUpShift(port_cnt))%(configPort.rows());
// 	  if(txRB(port_cnt)(SCloc, symLoc).real()==0 && txRB(port_cnt)(SCloc, symLoc).imag()==0)
// 	  {
// 	    cout<<"[both:] Improper CRS port filling exiting in fillRS()..."<<endl; abort();
// 	    exit(-1);
// 	  }
// 	}
// 	cout<<"[both:] Reference Signal Filling Level 1 Test 1 for CRS port is succesful in fillRS()... for Cell ID "<<rsInfo.cellid<<" and RB "<<logicalRUIndex<<endl;
// 	exit(0);
//       }
//       else if(rsInfo.CQIports(port_cnt)>=15 && rsInfo.CQIports(port_cnt)<23)
//       {
// 	txRB = framehandler.getTonesInRU(logicalRUIndex);
// 	portLocations = find(configPort, rsInfo.portReferenceNumbers(find(rsInfo.rsPorts, rsInfo.CQIports(port_cnt))), true);
// 	for(int i=0; i<portLocations.length()/2; i++)
// 	{
// 	  int symLoc = (portLocations(2*i)+cellSpecificSymbolRightShift(port_cnt))%(configPort.cols());
// 	  int SCloc = (portLocations(2*i+1)+cellSpecificSubcarrierUpShift(port_cnt))%(configPort.rows());
// 	  if(txRB(port_cnt)(SCloc, symLoc).real()==0 && txRB(port_cnt)(SCloc, symLoc).imag()==0)
// 	  {
// 	    cout<<"[both:] Improper CSIRS port filling exiting in fillRS()..."<<endl; abort();
// 	    exit(-1);
// 	  }
// 	}
// 	    cout<<"[both:] Reference Signal Filling Level 1 Test 1 for CSIRS port is succesful in fillRS()... for Cell ID "<<rsInfo.cellid<<" and RB "<<logicalRUIndex<<endl;
// 	    exit(0);
//       }
//       else
//       {
// 	cout<<"[both:] Invalid port exiting in fillRS()..."<<endl; abort();
//       }
//     }
//     cellSpecificSubcarrierUpShift.set_length(rsInfo.DeModPorts.length());
//     cellSpecificSymbolRightShift.set_length(rsInfo.DeModPorts.length());
//     getRSportShifts(cellSpecificSubcarrierUpShift, cellSpecificSymbolRightShift, rsInfo.DeModPorts, rsInfo.cellid, CSIRSconfig);
//     for(int port_cnt=0;port_cnt<rsInfo.DeModPorts.length();port_cnt++)
//     {
//       string rbFileName=rbpath+"Port"+toString(rsInfo.DeModPorts(port_cnt))+".txt";
//       parse(rbFileName, "rbStructure", configPort);
//       if(rsInfo.DeModPorts(port_cnt)==5 || (rsInfo.DeModPorts(port_cnt)>=7 && rsInfo.DeModPorts(port_cnt)<15))
//       {
// 	txRB = framehandler.getTonesInRU(logicalRUIndex);
// 	portLocations = find(configPort, rsInfo.portReferenceNumbers(find(rsInfo.rsPorts, rsInfo.DeModPorts(port_cnt))), true);
// 	for(int i=0; i<portLocations.length()/2; i++)
// 	{
// 	  int symLoc = (portLocations(2*i)+cellSpecificSymbolRightShift(port_cnt))%(configPort.cols());
// 	  int SCloc = (portLocations(2*i+1)+cellSpecificSubcarrierUpShift(port_cnt))%(configPort.rows());
// 	  for(int j=0; j<rsInfo.nCQIports; j++)
// 	  if(txRB(j)(SCloc, symLoc).real()==0 && txRB(port_cnt)(SCloc, symLoc).imag()==0)
// 	  {
// 	    cout<<"[both:] Improper DMRS port filling exiting in fillRS()..."<<endl; abort();
// 	    exit(-1);
// 	  }
// 	}
// 	    cout<<"[both:] Reference Signal Filling Level 1 Test 1 for DMRS port is succesful in fillRS()... for Cell ID "<<rsInfo.cellid<<" and RB "<<logicalRUIndex<<endl;
// 	    exit(0);
//       }
//     }
//   }
// }

void fillRS(ReferenceSignal_S &referenceSignal, int logicalRUIndex, RBInfo_S RBinfo, Size_S rbSize ,Framehandler& framehandler, ResourceMapper* resourceMapper, PortToAntennaMapper_S *portToPhysicalAntennaMapper, bool isDownlink, cmat precoder)
{
  RSInfo_S rsInfo=RBinfo.rsInfo;
  cmat portToPhyAntennaMapperForCQIports, precodedMapper;
  cvec portToAntennaMapperForMyPort, rsSeq1;
  Array<cvec> rsSequence(rsInfo.rsPorts.length());
  int portNumber;
  string toneType;
  ivec portReferenceNumbers=RBinfo.getPortReferenceNumbers(rsInfo.rsPorts), portReferenceNumbers2, portReferenceNumbers1=findUniqueNumbers(portReferenceNumbers);
  bvec isFilled=zeros_b(rsInfo.rsPorts.length());
  portToPhyAntennaMapperForCQIports=portToPhysicalAntennaMapper->getPortToAntennaMapper(rsInfo.nCQIports,framehandler.frame.length());
  // this loop executes for all possible ports for given transmission mode
  for(int port_cnt=0;port_cnt<rsInfo.rsPorts.length();port_cnt++)
  {
    portNumber=rsInfo.rsPorts(port_cnt);
    int portIndex=find(referenceSignal.portIndices,portNumber);
    toneType="port"+toString(portNumber);
    
    // gives the index for reference symbol positions(Index starts from top to bottom) 
    ivec row1stIndices=framehandler.getRow1stToneIndicesInRU(logicalRUIndex,toneType);
    int reCountInSlot0=sum(row1stIndices<((rbSize.numberOfSymbols/2)*rbSize.numberOfSubcarriers));
    int reCountInSlot1=row1stIndices.length()-reCountInSlot0;
    ivec phyRB;
    if(isDownlink)
      phyRB=resourceMapper->dlResourceMapper.getPRBIndex(logicalRUIndex % resourceMapper->dlResourceMapper.getResourceBlocksPerSubframe());
    else
      phyRB=resourceMapper->ulResourceMapper.getPRBIndex(logicalRUIndex % resourceMapper->ulResourceMapper.getResourceBlocksPerSubframe());
    
    // gives the reference symbols for all the ports for one subframe
    
    // cout<<"\n REcount:"<< reCountInSlot0<<'\t'<<reCountInSlot1<<'\t'<<portIndex;
    //cout<<'\n'<<referenceSignal.rsSequencePerRB(portIndex)(phyRB(0));
    if(find(rsInfo.otherDemodPorts,portNumber)==-1)
    if(referenceSignal.rsSequencePerRB(portIndex)(phyRB(0)).length())
    {
      isFilled(port_cnt)=true;
      rsSequence(port_cnt)=concat((referenceSignal.rsSequencePerRB(portIndex)(phyRB(0))).left(reCountInSlot0), (referenceSignal.rsSequencePerRB(portIndex)(phyRB(1))).right(reCountInSlot1));
    }
  }
  
  // If the antenna ports are DMRS ports  
  if(rsInfo.DeModrsType==_RSTYPE_DMRS_)
  {
    if(precoder.size()==0)
      precodedMapper=portToPhyAntennaMapperForCQIports;
    else
      precodedMapper=portToPhyAntennaMapperForCQIports*precoder;
  }
  //clear CQI port positions as addOver is not allowed. To handle MU-MIMO filling - Dhiv
  for(int port_cnt=0;port_cnt<rsInfo.CQIports.length();port_cnt++)
  {
      portNumber=rsInfo.CQIports(port_cnt);
      toneType="port"+toString(portNumber);
      Array<cvec> physicalAntennaRS=framehandler.getTonesInRU(logicalRUIndex,toneType);
      for(int ant_cnt=0;ant_cnt<physicalAntennaRS.length();ant_cnt++)
          physicalAntennaRS(ant_cnt).zeros();
      framehandler.setTonesInRU(physicalAntennaRS,logicalRUIndex,toneType);
  }
//   
  for(int port_cnt=0;port_cnt<rsInfo.rsPorts.length();port_cnt++)
    if(isFilled(port_cnt))
    {
      Array<cvec> allPortSequence;  
      Array<cvec> physicalAntennaRS(framehandler.frame.length());
      
      ivec reps;
      int nPorts;
      portNumber=rsInfo.rsPorts(port_cnt);
      toneType="port"+toString(portNumber);
      /* for mode-1 to mode-6 this if statement returns the CRS referenceSignal for one RB after multiplying with portToPhyAntennaMapperForCQIports and for
       *    mode-7 to mode-9 it returns the CSI-RS reference symbols for one RB after multiplying with portToPhyAntennaMapper */
     
      if(isDownlink && find(rsInfo.CQIports, rsInfo.rsPorts(port_cnt))!=-1)/// CSI-RS/CRS
      {
	  for(int ant_cnt=0;ant_cnt<physicalAntennaRS.length();ant_cnt++)
	   physicalAntennaRS(ant_cnt).set_length(rsSequence(port_cnt).length());
	  
	  //multiplying the CQI port reference sequence with the port to antenna mapper
	  for(int re_cnt=0; re_cnt<rsSequence(port_cnt).length(); re_cnt++)
	    setAcrossArray(physicalAntennaRS,re_cnt,portToPhyAntennaMapperForCQIports.get_col(find(rsInfo.CQIports, rsInfo.rsPorts(port_cnt)))*rsSequence(port_cnt)(re_cnt)*sqrt(inv_dB(rsInfo.rsBoostInDB)));	    
      }
      // for mode-7 to mode-9 this statement returns the DMRS referenceSignal for one RB after multiplying with portToPhyAntennaMapperForCQIports
      else if(rsInfo.DeModrsType==_RSTYPE_DMRS_ && find(rsInfo.DeModPorts, rsInfo.rsPorts(port_cnt))!=-1)// DMRS
      {
          
	for(int ant_cnt=0;ant_cnt<physicalAntennaRS.length();ant_cnt++)
	  physicalAntennaRS(ant_cnt).set_length(rsSequence(port_cnt).length());
	
	//multiplying the CQI port reference sequence with the port to antenna mapper
	for(int re_cnt=0; re_cnt<rsSequence(port_cnt).length(); re_cnt++)
	  setAcrossArray(physicalAntennaRS,re_cnt,precodedMapper.get_col(find(rsInfo.DeModPorts, rsInfo.rsPorts(port_cnt)))*rsSequence(port_cnt)(re_cnt)*sqrt(inv_dB(rsInfo.rsBoostInDB)));	    
	
      }
      // for mode-7 to mode-9 this statement returns the CRS referenceSignal for one RB after multiplying with portToPhyAntennaMapperForCQIports
      else if(isDownlink && rsInfo.CQIrsType!=_RSTYPE_CRS_ && rsInfo.rsPorts(port_cnt)>=0 && rsInfo.rsPorts(port_cnt)<=3)/// CRS
      {
	nPorts=rsInfo.crsPorts.length(); // number of CRS ports for particular mode
	allPortSequence.set_size(nPorts);
	portReferenceNumbers2=RBinfo.getPortReferenceNumbers(rsInfo.crsPorts);// gives the port reference number for CRS ports
	rsSeq1.set_size(nPorts);
	reps=find(portReferenceNumbers2, portReferenceNumbers(port_cnt), true);
	// gives the Reference signals for desired port and fills zeros for other ports
	for(int p=0; p<nPorts; p++)
	  if(find(reps, p)!=-1)
	    allPortSequence(p)=rsSequence(find(rsInfo.rsPorts, rsInfo.crsPorts(p)));
	  else
	    allPortSequence(p)=zeros_c(rsSequence(port_cnt).length());
	  // this loop has the port to antenna mapper multipled CRS reference signal at one port in one RB (Including zeros for filling other port CRS locations)
	  for(int ant_cnt=0;ant_cnt<physicalAntennaRS.length();ant_cnt++)
	  {
	    physicalAntennaRS(ant_cnt).set_length(rsSequence(port_cnt).length());// sets the length of physicalAntennaRS(0) length to 8
	    for(int sym3=0; sym3<rsSequence(port_cnt).length(); sym3++)
	    {
	      
	      for(int p=0; p<nPorts; p++)
		rsSeq1(p)=allPortSequence(p)(sym3);
	      // if the number of CRS ports and CQI ports are equal multiply the CRS symbols with port to antenna mapper
	      if(rsInfo.crsPorts.length()==rsInfo.nCQIports)
		physicalAntennaRS(ant_cnt)(sym3)=sum(elem_mult(rsSeq1, portToPhyAntennaMapperForCQIports.get_row(ant_cnt)))*sqrt(inv_dB(rsInfo.rsBoostInDB));
	      // if the number of CRS ports and CQI ports are not equal append zeros to the CRS symbols and multiply the CRS symbols with port to antenna mapper
	      else
	      {
		cvec rsSeq2(rsInfo.nCQIports);
		rsSeq2 = concat(rsSeq1, zeros_c(rsInfo.nCQIports-rsInfo.crsPorts.length()));
		physicalAntennaRS(ant_cnt)(sym3)=sum(elem_mult(rsSeq2, portToPhyAntennaMapperForCQIports.get_row(ant_cnt)))*sqrt(inv_dB(rsInfo.rsBoostInDB));
	      }
	    }
	  }
      }
      //====================================================================================================
      // Code to apply Downlink Power allocation (3gpp 36.213 - Section 5.2) difference between
      // OFDM symbols with and without CRS
      // Multiply the power mask on the CRS
      //====================================================================================================
      else if(isDownlink && (portNumber==61 || portNumber==62))
      {
	int rsIndx=find(referenceSignal.portIndices,rsInfo.rsPorts(port_cnt));
	int prsReferenceNumber=RBinfo.getPortReferenceNumbers(to_ivec(portNumber))(0);
	
	// If the port-6 is not available for the given mode display the error
	if(rsIndx==-1)
	{
	  cout<<"Reference signal for port "<<portNumber<<" not available in ReferenceSignal_S..."<<endl;
	  return;
	}
	
	string toneType="port"+toString(portNumber);
	imat prsRBstructure=framehandler.getResourceUnitMatrix(toneType);
	ivec port6Indices=find(prsRBstructure, prsReferenceNumber , true);
	int reCountInSlot0=sum(port6Indices<((rbSize.numberOfSymbols/2)*rbSize.numberOfSubcarriers));
	int reCountInSlot1=port6Indices.length()-reCountInSlot0;
	ivec phyRB=resourceMapper->dlResourceMapper.getPRBIndex(logicalRUIndex);
	cvec rsSequence(0);
	append(rsSequence,(referenceSignal.rsSequencePerRB(rsIndx)(phyRB(0))).left(reCountInSlot0));
	append(rsSequence,(referenceSignal.rsSequencePerRB(rsIndx)(phyRB(1))).right(reCountInSlot1));
	cmat precodedMapper;
	precodedMapper=portToPhyAntennaMapperForCQIports;
	if(portNumber==61)
	  portToAntennaMapperForMyPort=precodedMapper.get_col(0);
	else
	  portToAntennaMapperForMyPort=precodedMapper.get_col(1);
	
	// CQI ports are not equal append zeros to the CRS
	for(int ant_cnt=0;ant_cnt<physicalAntennaRS.length();ant_cnt++)
	  physicalAntennaRS(ant_cnt)=rsSequence*portToAntennaMapperForMyPort(ant_cnt)*sqrt(inv_dB(rsInfo.rsBoostInDB));
      }  
    else if(rsInfo.PtrsPorts.length()!=0 && (rsInfo.rsPorts(port_cnt)>=1200 && rsInfo.rsPorts(port_cnt)<=1201) && find(rsInfo.rsrbpos(port_cnt),logicalRUIndex)!=-1)// DMRS
    { nPorts=rsInfo.PtrsPorts.length();// number of CQI ports for particular mode
      allPortSequence.set_size(nPorts);
      portReferenceNumbers2=RBinfo.getPortReferenceNumbers(rsInfo.PtrsPorts);// gives the port reference number for DMRS ports
      rsSeq1.set_size(nPorts);
      reps=find(portReferenceNumbers2, portReferenceNumbers(port_cnt), true);
      // gives the Reference signals for desired port and fills zeros for other ports
      for(int p=0; p<nPorts; p++)
	if(find(reps, p)!=-1)
	  allPortSequence(p)=rsSequence(find(rsInfo.rsPorts, rsInfo.PtrsPorts(p)));
	else
	  allPortSequence(p)=zeros_c(rsSequence(port_cnt).length());
	// this loop has the precoding multipled DMRS reference signal at one port in one RB (Including zeros for filling other port CRS locations)
	/*cout<<"rsInfo.DeModPorts and rsInfo.PTRSassocDMRSPorts\t"<< rsInfo.DeModPorts <<"\t"<< rsInfo.PTRSassocDMRSPorts<<endl;
	cout<<"find(rsInfo.DeModPorts,rsInfo.PTRSassocDMRSPorts)\t"<<find(rsInfo.DeModPorts,rsInfo.PTRSassocDMRSPorts)<<endl;
	*/
	cmat ptrsPrecoder=precodedMapper.get_cols(find(rsInfo.DeModPorts,rsInfo.PTRSassocDMRSPorts));
	for(int ant_cnt=0;ant_cnt<physicalAntennaRS.length();ant_cnt++)
	{
	  physicalAntennaRS(ant_cnt).set_length(rsSequence(port_cnt).length());
	  for(int sym3=0; sym3<rsSequence(port_cnt).length(); sym3++)
	  {
	    for(int p=0; p<nPorts; p++)
	      rsSeq1(p)=allPortSequence(p)(sym3);
	    //multiplying the DMRS port reference sequence with the precoder and port to antenna mapper
	    physicalAntennaRS(ant_cnt)(sym3)=sum(elem_mult(rsSeq1, ptrsPrecoder.get_row(ant_cnt)))*sqrt(inv_dB(rsInfo.rsBoostInDB));
	  }
	}
     }
      
      if(isDownlink && RBinfo.dlPowMask.size()!=0)
      {
	ivec row1stIndices=framehandler.getRow1stToneIndicesInRU(logicalRUIndex,toneType);
	for(int ant_cnt=0;ant_cnt<physicalAntennaRS.length();ant_cnt++)
	  for(int sym=0; sym<row1stIndices.length(); sym++) 
	    physicalAntennaRS(ant_cnt)(sym) = physicalAntennaRS(ant_cnt)(sym) * sqrt(inv_dB(RBinfo.crsPowBoostInDB));
      }
      if(physicalAntennaRS(0).length())
     	framehandler.setTonesInRU(physicalAntennaRS,logicalRUIndex,toneType,true);
     
    }
    
    if(traceValuePerTraceName["referenceSignalGenerator_01_traceFlag"]=="true")
    {
      ivec cellSpecificSubcarrierUpShift(rsInfo.nCQIports), cellSpecificSymbolRightShift(rsInfo.nCQIports), RStones, portLocations;
      int CSIRSconfig=0;
      imat configPort;
      Array<cmat> txRB;
      string rbpath="../configFiles/LTERbPattern/LTERbPatternDL/";
      getRSportShifts(cellSpecificSubcarrierUpShift, cellSpecificSymbolRightShift, rsInfo.CQIports, rsInfo.cellid, CSIRSconfig);
      for(int port_cnt=0;port_cnt<rsInfo.nCQIports;port_cnt++)
      {
	string rbFileName=rbpath+"Port"+toString(rsInfo.CQIports(port_cnt))+".txt";
	parse(rbFileName, "rbStructure", configPort);
	if(rsInfo.CQIports(port_cnt)>=0 && rsInfo.CQIports(port_cnt)<4)
	{
	  txRB = framehandler.getTonesInRU(logicalRUIndex);
	  portLocations = find(configPort, rsInfo.portReferenceNumbers(find(rsInfo.rsPorts, rsInfo.CQIports(port_cnt))), true);
	  for(int i=0; i<portLocations.length()/2; i++)
	  {
	    int symLoc = (portLocations(2*i)+cellSpecificSymbolRightShift(port_cnt))%(configPort.cols());
	    int SCloc = (portLocations(2*i+1)+cellSpecificSubcarrierUpShift(port_cnt))%(configPort.rows());
	    if(txRB(port_cnt)(SCloc, symLoc).real()==0 && txRB(port_cnt)(SCloc, symLoc).imag()==0)
	    {
	      cout<<"[both:] Improper CRS port filling exiting in fillRS()..."<<endl; abort();
	      exit(-1);
	    }
	  }
	  cout<<"[both:] Reference Signal Filling Level 1 Test 1 for CRS port is succesful in fillRS()... for Cell ID "<<rsInfo.cellid<<" and RB "<<logicalRUIndex<<endl;
	  exit(0);
	}
	else if(rsInfo.CQIports(port_cnt)>=15 && rsInfo.CQIports(port_cnt)<23)
	{
	  txRB = framehandler.getTonesInRU(logicalRUIndex);
	  portLocations = find(configPort, rsInfo.portReferenceNumbers(find(rsInfo.rsPorts, rsInfo.CQIports(port_cnt))), true);
	  for(int i=0; i<portLocations.length()/2; i++)
	  {
	    int symLoc = (portLocations(2*i)+cellSpecificSymbolRightShift(port_cnt))%(configPort.cols());
	    int SCloc = (portLocations(2*i+1)+cellSpecificSubcarrierUpShift(port_cnt))%(configPort.rows());
	    if(txRB(port_cnt)(SCloc, symLoc).real()==0 && txRB(port_cnt)(SCloc, symLoc).imag()==0)
	    {
	      cout<<"[both:] Improper CSIRS port filling exiting in fillRS()..."<<endl; abort();
	      exit(-1);
	    }
	  }
	  cout<<"[both:] Reference Signal Filling Level 1 Test 1 for CSIRS port is succesful in fillRS()... for Cell ID "<<rsInfo.cellid<<" and RB "<<logicalRUIndex<<endl;
	  exit(0);
	}
	else
	{
	  cout<<"[both:] Invalid port exiting in fillRS()..."<<endl; abort();
	}
      }
      cellSpecificSubcarrierUpShift.set_length(rsInfo.DeModPorts.length());
      cellSpecificSymbolRightShift.set_length(rsInfo.DeModPorts.length());
      getRSportShifts(cellSpecificSubcarrierUpShift, cellSpecificSymbolRightShift, rsInfo.DeModPorts, rsInfo.cellid, CSIRSconfig);
      for(int port_cnt=0;port_cnt<rsInfo.DeModPorts.length();port_cnt++)
      {
	string rbFileName=rbpath+"Port"+toString(rsInfo.DeModPorts(port_cnt))+".txt";
	parse(rbFileName, "rbStructure", configPort);
	if(rsInfo.DeModPorts(port_cnt)==5 || (rsInfo.DeModPorts(port_cnt)>=7 && rsInfo.DeModPorts(port_cnt)<15))
	{
	  txRB = framehandler.getTonesInRU(logicalRUIndex);
	  portLocations = find(configPort, rsInfo.portReferenceNumbers(find(rsInfo.rsPorts, rsInfo.DeModPorts(port_cnt))), true);
	  for(int i=0; i<portLocations.length()/2; i++)
	  {
	    int symLoc = (portLocations(2*i)+cellSpecificSymbolRightShift(port_cnt))%(configPort.cols());
	    int SCloc = (portLocations(2*i+1)+cellSpecificSubcarrierUpShift(port_cnt))%(configPort.rows());
	    for(int j=0; j<rsInfo.nCQIports; j++)
	      if(txRB(j)(SCloc, symLoc).real()==0 && txRB(port_cnt)(SCloc, symLoc).imag()==0)
	      {
		cout<<"[both:] Improper DMRS port filling exiting in fillRS()..."<<endl; abort();
		exit(-1);
	      }
	  }
	  cout<<"[both:] Reference Signal Filling Level 1 Test 1 for DMRS port is succesful in fillRS()... for Cell ID "<<rsInfo.cellid<<" and RB "<<logicalRUIndex<<endl;
	  exit(0);
	}
      }
    }

    
}


/*! \brief Function to get the Uplink-referencesignals for a UE attached to Cell with given citeID & allocated RB-pairs indicated by 'rbIndices'
*  \param[in] cellID ---> cellID of the attached BS for the UE.
*  \param [in] scheduledRBs ---> scheduled resource blocks in a subframe
*  \param [in] portIndices ---> port numbers
*  \param [in] rsCountPerRBPerPort ---> Number of reference signals in an RB for the given port
*  \param[in] referenceSignalScheme ---> method used to generate the reference signals
*  \return Pilot sequnce to be filled for one symbol in an RB   */
Array<cvec> getUplinkDMRS(int cellID, ivec scheduledRBs, ivec portIndices, ivec rsCountPerRBPerPort, string referenceSignalScheme)
{
  ivec Primes = "2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97 101 103 107 109 113 127 131 137 139 149 151 157 163 167 173 179 181 191 193 197 199 211 223 227 229 233 239 241 251 257 263 269 271 277 281 283 293 307 311 313 317 331 337 347 349 353 359 367 373 379 383 389 397 401 409 419 421 431 433 439 443 449 457 461 463 467 479 487 491 499 503 509 521 523 541 547 557 563 569 571 577 587 593 599 601 607 613 617 619 631 641 643 647 653 659 661 673 677 683 691 701 709 719 727 733 739 743 751 757 761 769 773 787 797 809 811 821 823 827 829 839 853 857 859 863 877 881 883 887 907 911 919 929 937 941 947 953 967 971 977 983 991 997 1009 1013 1019 1021 1031 1033 1039 1049 1051 1061 1063 1069 1087 1091 1093 1097 1103 1109 1117 1123 1129 1151 1153 1163 1171 1181 1187 1193 1201 1213 1217 1223 1229 1231 1237 1249 1259 1277 1279 1283 1289 1291 1297 1301 1303 1307 1319 ";

  //! Following map contains Zadoff-Chu sequences with the format zadoffChuPilotSequence[length][baseIndex][derivedIndex]
  std::map< int,std::map< int,std::map<int,cvec> > > zadoffChuPilotSequence; 
  Array<cvec> referenceSignal(portIndices.length());
  for(int port_cnt=0;port_cnt<portIndices.length();port_cnt++)
  {
    if( referenceSignalScheme =="" || referenceSignalScheme == "ZadoffChu")  //! As per the LTE-Spec
    {
      //! Pilot occupy entire 3rd & 10th symbol in an uplink RB-pair...
      int seqLength = rsCountPerRBPerPort(port_cnt)*scheduledRBs.length();//72
      int baseIndex=1;                        //! \todo for the time being baseIndex is kept 1 for all transmitters...
      int derivedIndex=cellID%12;
      //! Check If Zadoff-Chu pilot sequence is already there, then pick the sequence & return
      if(length(zadoffChuPilotSequence[seqLength][baseIndex][derivedIndex]) >0)
	referenceSignal(port_cnt)=zadoffChuPilotSequence[seqLength][baseIndex][derivedIndex];
      //! else generate the sequence.....
      else
      {
	cvec primeSequence;
	int Mzc = Primes(find(Primes > seqLength/2)(0)-1);	//! Get the Prime-length which is <= seqLength/2 ...
	vec l=linspace(0,Mzc-1,Mzc);				//! Generate the Zadoff-Chu sequence with prime-length...
	primeSequence = exp(-dComplex(0,1)*pi*(double)baseIndex*elem_mult(l,(l+1))/(double)Mzc);
	//! Cyclicaly extend the prime-length sequence to required 'seqLength/2'...
	cvec singleSlotSequence = elem_mult(cyclicallySuffix(primeSequence,seqLength/2),exp(dComplex(0,1)*pi*(double)derivedIndex*linspace(0,seqLength/2-1,seqLength/2)/6));
	//! Since first & second slot contains same sequence, concatenate the sequence together...
	zadoffChuPilotSequence[seqLength][baseIndex][derivedIndex] = concat(singleSlotSequence,singleSlotSequence);
	referenceSignal(port_cnt)=zadoffChuPilotSequence[seqLength][baseIndex][derivedIndex];
      }
    }
    else if(referenceSignalScheme == "reuse_6")   //! As per CEWIT Proposal. (Hadamard sequence is used for each RB-Pairs)
    {
      cmat basePilot = "1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0;  \
      1,0,0,0,0,0,-1,0,0,0,0,0,1,0,0,0,0,0,-1,0,0,0,0,0;  \
      1,0,0,0,0,0,1,0,0,0,0,0,-1,0,0,0,0,0,-1,0,0,0,0,0;  \
      1,0,0,0,0,0,-1,0,0,0,0,0,-1,0,0,0,0,0,1,0,0,0,0,0;";
      //! \note Pilots are boosted to same total power.
      basePilot = sqrt(6)*basePilot;
      referenceSignal(port_cnt)=cyclicallyShift(basePilot.get_row(cellID/6),cellID%6);
    }
    else
    {
      cout<<"[both:] Error: pilotScheme ="<<referenceSignalScheme<<" is not defined..."<<endl;abort();
    }
  }
  return(referenceSignal);

}


void getCSIRSoffset(int CSIRSconfig, int nCSIports, int &symOffset, int &SCoffset, int frameType)
{
  switch(nCSIports)
  {
    case 1:
    case 2:
    {
      switch(frameType)
      {
	case 1:
	{
	  switch(CSIRSconfig)
	  {
	    case 0: SCoffset=0; symOffset=0; break;
	    case 1: SCoffset=2; symOffset=4; break;
	    case 2: SCoffset=0; symOffset=4; break;
	    case 3: SCoffset=-2; symOffset=4; break;
	    case 4: SCoffset=0; symOffset=7; break;
	    case 5: SCoffset=-1; symOffset=0; break;
	    case 6: SCoffset=1; symOffset=4; break;
	    case 7: SCoffset=-1; symOffset=4; break;
	    case 8: SCoffset=-3; symOffset=4; break;
	    case 9: SCoffset=-1; symOffset=7; break;
	    case 10: SCoffset=-6; symOffset=0; break;
	    case 11: SCoffset=-7; symOffset=0; break;
	    case 12: SCoffset=-4; symOffset=4; break;
	    case 13: SCoffset=-5; symOffset=4; break;
	    case 14: SCoffset=-6; symOffset=4; break;
	    case 15: SCoffset=-7; symOffset=4; break;
	    case 16: SCoffset=-8; symOffset=4; break;
	    case 17: SCoffset=-9; symOffset=4; break;
	    case 18: SCoffset=-6; symOffset=7; break;
	    case 19: SCoffset=-7; symOffset=7; break;
	    default:{cout<<"[both:] Invalind CSIRSconfig exiting in getCSIRSloacation()... "<<endl;abort();}
	    break;
	  }
	}
	break;
      case 2:
      {
	switch(CSIRSconfig)
	{
	  case 0: SCoffset=0; symOffset=0; break;
	  case 1: SCoffset=2; symOffset=4; break;
	  case 2: SCoffset=0; symOffset=4; break;
	  case 3: SCoffset=-2; symOffset=4; break;
	  case 4: SCoffset=0; symOffset=7; break;
	  case 5: SCoffset=-1; symOffset=0; break;
	  case 6: SCoffset=1; symOffset=4; break;
	  case 7: SCoffset=-1; symOffset=4; break;
	  case 8: SCoffset=-3; symOffset=4; break;
	  case 9: SCoffset=-1; symOffset=7; break;
	  case 10: SCoffset=-6; symOffset=0; break;
	  case 11: SCoffset=-7; symOffset=0; break;
	  case 12: SCoffset=-4; symOffset=4; break;
	  case 13: SCoffset=-5; symOffset=4; break;
	  case 14: SCoffset=-6; symOffset=4; break;
	  case 15: SCoffset=-7; symOffset=4; break;
	  case 16: SCoffset=-8; symOffset=4; break;
	  case 17: SCoffset=-9; symOffset=4; break;
	  case 18: SCoffset=-6; symOffset=7; break;
	  case 19: SCoffset=-7; symOffset=7; break;
	  case 20: SCoffset=2; symOffset=3; break;
	  case 21: SCoffset=0; symOffset=3; break;
	  case 22: SCoffset=-2; symOffset=3; break;
	  case 23: SCoffset=1; symOffset=3; break;
	  case 24: SCoffset=-1; symOffset=3; break;
	  case 25: SCoffset=-3; symOffset=3; break;
	  case 26: SCoffset=-4; symOffset=3; break;
	  case 27: SCoffset=-5; symOffset=3; break;
	  case 28: SCoffset=-6; symOffset=3; break;
	  case 29: SCoffset=-7; symOffset=3; break;
	  case 30: SCoffset=-8; symOffset=3; break;
	  case 31: SCoffset=-9; symOffset=3; break;
	  default:{cout<<"[both:] Invalind CSIRSconfig exiting in getCSIRSloacation()... "<<endl;abort();}
	  break;
	}
      }
      break;
      default:
      {cout<<"[both:] Invalind frameType exiting in getCSIRSloacation()... "<<endl;abort();}
      break;
      }
    }
    break;
  case 4:
  {
    switch(frameType)
    {
      case 1:
      {
	switch(CSIRSconfig)
	{
	  case 0: SCoffset=0; symOffset=0; break;
	  case 1: SCoffset=2; symOffset=4; break;
	  case 2: SCoffset=0; symOffset=4; break;
	  case 3: SCoffset=-2; symOffset=4; break;
	  case 4: SCoffset=0; symOffset=7; break;
	  case 5: SCoffset=-1; symOffset=0; break;
	  case 6: SCoffset=1; symOffset=4; break;
	  case 7: SCoffset=-1; symOffset=4; break;
	  case 8: SCoffset=-3; symOffset=4; break;
	  case 9: SCoffset=-1; symOffset=7; break;
	  default:{cout<<"[both:] Invalind CSIRSconfig exiting in getCSIRSloacation()... "<<endl;abort();}
	  break;
	}
      }
      break;
      case 2:
      {
	switch(CSIRSconfig)
	{
	  case 0: SCoffset=0; symOffset=0; break;
	  case 1: SCoffset=2; symOffset=4; break;
	  case 2: SCoffset=0; symOffset=4; break;
	  case 3: SCoffset=-2; symOffset=4; break;
	  case 4: SCoffset=0; symOffset=7; break;
	  case 5: SCoffset=-1; symOffset=0; break;
	  case 6: SCoffset=1; symOffset=4; break;
	  case 7: SCoffset=-1; symOffset=4; break;
	  case 8: SCoffset=-3; symOffset=4; break;
	  case 9: SCoffset=-1; symOffset=7; break;
	  case 20: SCoffset=2; symOffset=3; break;
	  case 21: SCoffset=0; symOffset=3; break;
	  case 22: SCoffset=-2; symOffset=3; break;
	  case 23: SCoffset=1; symOffset=3; break;
	  case 24: SCoffset=-1; symOffset=3; break;
	  case 25: SCoffset=-3; symOffset=3; break;
	  default:{cout<<"[both:] Invalind CSIRSconfig exiting in getCSIRSloacation()... "<<endl;abort();}
	  break;
	}
      }
      break;
      default:
      {cout<<"[both:] Invalind frameType exiting in getCSIRSloacation()... "<<endl;abort();}
      break;
    }
  }
  break;
  case 8:
  {
    switch(frameType)
    {
      case 1:
      {
	switch(CSIRSconfig)
	{
	  case 0: SCoffset=0; symOffset=0; break;
	  case 1: SCoffset=2; symOffset=4; break;
	  case 2: SCoffset=0; symOffset=4; break;
	  case 3: SCoffset=-2; symOffset=4; break;
	  case 4: SCoffset=0; symOffset=7; break;
	  default:{cout<<"[both:] Invalind CSIRSconfig exiting in getCSIRSloacation()... "<<endl;abort();}
	  break;
	}
      }
      break;
      case 2:
      {
	switch(CSIRSconfig)
	{
	  case 0: SCoffset=0; symOffset=0; break;
	  case 1: SCoffset=2; symOffset=4; break;
	  case 2: SCoffset=0; symOffset=4; break;
	  case 3: SCoffset=-2; symOffset=4; break;
	  case 4: SCoffset=0; symOffset=7; break;
	  case 20: SCoffset=2; symOffset=3; break;
	  case 21: SCoffset=0; symOffset=3; break;
	  case 22: SCoffset=-2; symOffset=3; break;
	  default:{cout<<"[both:] Invalind CSIRSconfig exiting in getCSIRSloacation()... "<<endl;abort();}
	  break;
	}
      }
      break;
      default:
      {cout<<"[both:] Invalind frameType exiting in getCSIRSloacation()... "<<endl;abort();}
      break;
  }
  }
  break;
  default:
  {cout<<"[both:] Invalind nCSIports exiting in getCSIRSloacation()... "<<endl;abort();}
  break;
  }
}

void getNRCSIRSoffset(int CSIRSconfig, int nNRCSIports, int &symOffset, int &SCoffset, int frameType)
{  switch(nNRCSIports)
  {
    case 1:
    {
      switch(frameType)
      {
	case 1:
	{
	  switch(CSIRSconfig)
	  {
	    case 0: SCoffset=0; symOffset=0; break;
	    case 1: SCoffset=-1; symOffset=0; break;
	    case 2: SCoffset=-2; symOffset=0; break;
	    case 3: SCoffset=-3; symOffset=0; break;
	    case 4: SCoffset=-4; symOffset=0; break;
	    case 5: SCoffset=-5; symOffset=0; break;
	    case 6: SCoffset=-6; symOffset=0; break;
	    case 7: SCoffset=-7; symOffset=0; break;
	    case 8: SCoffset=-8; symOffset=0; break;
	    case 9: SCoffset=-9; symOffset=0; break;
	    case 10: SCoffset=-10; symOffset=0; break;
	    case 11: SCoffset=-11; symOffset=0; break;
	    case 12: SCoffset=0; symOffset=0; break;
	    case 13: SCoffset=-1; symOffset=0; break;
	    case 14: SCoffset=-2; symOffset=0; break;
	    case 15: SCoffset=-3; symOffset=0; break;
	    default:{cout<<"[both:] Invalind CSIRSconfig exiting in getCSIRSloacation()... "<<endl;abort();}
	    break;
	  }
	}
	break;
	    case 2:
	    {
	      switch(CSIRSconfig)
	      {
		case 0: SCoffset=0; symOffset=0; break;
		case 1: SCoffset=-1; symOffset=0; break;
		case 2: SCoffset=-2; symOffset=0; break;
		case 3: SCoffset=-3; symOffset=0; break;
		case 4: SCoffset=-4; symOffset=0; break;
		case 5: SCoffset=-5; symOffset=0; break;
		case 6: SCoffset=-6; symOffset=0; break;
		case 7: SCoffset=-7; symOffset=0; break;
		case 8: SCoffset=-8; symOffset=0; break;
		case 9: SCoffset=-9; symOffset=0; break;
		case 10: SCoffset=-10; symOffset=0; break;
		case 11: SCoffset=-11; symOffset=0; break;
		case 12: SCoffset=0; symOffset=0; break;
		case 13: SCoffset=-1; symOffset=0; break;
		case 14: SCoffset=-2; symOffset=0; break;
		case 15: SCoffset=-3; symOffset=0; break;
		default:{cout<<"[both:] Invalind CSIRSconfig exiting in getCSIRSloacation()... "<<endl;abort();}
		break;
	      }
	    }
	    break;
		default:
		{cout<<"[both:] Invalind frameType exiting in getCSIRSloacation()... "<<endl;abort();}
		break;
      }
    }
      break;
      default:
      {
      cout<<"[both:] Invalind nCSIports exiting in getCSIRSloacation()... "<<endl;abort();}
      break;
  }
}


/*! \brief the function fillMyUplinkReferenceSignal recives the reference signals for one ofdm symbol and returns reference signal for one RB pair to frameHandler 
*  \param[in] cellID ---> cellID of the attached BS for the UE.
*  \param [in] rsPowerInWatts ---> reference signal power in watts
*  \param [in] scheduledRBs --->number of RBs given for an UE for uplink transmission
*  \param [in] totalRBs ---> total number of resource blocks depends on the bandwidth
*  \param [in] portNumbers ---> ports used in Uplink transmission
*  \param [in] rbInfo ---> gives the number of rows and colomns in an RB
*  \param[in] uplinkReferenceSignalScheme ---> method used to generate the reference signals 
*  \return Pilot sequnce to be filled for all the RB-pairs allocated for the UE    */
void fillMyUplinkReferenceSignal(int cellID,Framehandler *frameHandler,PortToAntennaMapper_S *portToPhysicalAntennaMapper,ivec scheduledRBs,int totalRBs , ivec portNumbers,ReferenceSignal_S *referenceSignal, RBInfo_S *rbInfo, string uplinkReferenceSignalScheme){
    
  
  sort(portNumbers);
  portNumbers=remove(portNumbers,-2);
 portNumbers=remove(portNumbers,1200);
  ivec rsCountPerRBPerPort(portNumbers.length());
  referenceSignal->rsSequencePerRB.set_length(portNumbers.length());
  
  referenceSignal->portIndices=portNumbers ;
 
  //calculates the number of DMRS reference Signal to be transmitted in all the ports per subframe
  for(int port_cnt=0;port_cnt<portNumbers.length();port_cnt++)
  { rsCountPerRBPerPort(port_cnt)=count(rbInfo->rsPositionMatrix,rbInfo->rsInfo.portReferenceNumbers(find(rbInfo->rsInfo.rsPorts,portNumbers(port_cnt)))); 
    referenceSignal->rsSequencePerRB(port_cnt).set_length(totalRBs);
    for(int rb_cnt=0;rb_cnt<totalRBs;rb_cnt++)
      referenceSignal->rsSequencePerRB(port_cnt)(rb_cnt).set_length(0);
  }
  
  
  
  
  Size_S rbSize;
  rbSize.numberOfSubcarriers=rbInfo->rsPositionMatrix.rows();
  rbSize.numberOfSymbols=rbInfo->rsPositionMatrix.cols();
  
  // function getUplinkDMRS returns one port DMRS reference signal 
  Array<cvec> refSignal=getUplinkDMRS(cellID, scheduledRBs, portNumbers, rsCountPerRBPerPort, uplinkReferenceSignalScheme);
  cmat portToAntMapper= portToPhysicalAntennaMapper->getPortToAntennaMapper(portNumbers.length(),frameHandler->frame.length());
  Array<cvec> referenceSignalPerAntenna(frameHandler->frame.length());
  cvec portToAntennaMapperForMyPort;
  string toneType;
  int portIndx;
  cvec rsPerRB;
  Array<ivec> rsPorsPerRB(totalRBs);
  for(int port_cnt=0;port_cnt<portNumbers.length();port_cnt++)
  {
    portIndx=port_cnt;
    // Initializing the variable rsSequencePerRB 
//     if(portIndx==-1)
//     {
//       portIndx=referenceSignal->portIndices.length();//0
//       append(referenceSignal->portIndices,portNumbers(port_cnt));
//       append(referenceSignal->rsSequencePerRB,zeros_c(totalRBs*rbSize.numberOfSubcarriers,rsCountPerRBPerPort(port_cnt)));
//     }
//    referenceSignal->rsSequencePerRB(portIndx)=zeros_c(totalRBs,rsCountPerRBPerPort(port_cnt));
    referenceSignal->rsSequencePerRB(portIndx).set_length(totalRBs);
    portToAntennaMapperForMyPort=portToAntMapper.get_col(port_cnt);
    toneType="port"+toString(portNumbers(port_cnt));
    for(int rb_cnt=0;rb_cnt<scheduledRBs.length();rb_cnt++)
    {       if(portNumbers(port_cnt)<1000) // LTE RS
	// at both the symbols in an RB the same DMRS reference sybols are transmitting 
	rsPerRB=concat(refSignal(port_cnt).get((rb_cnt*rsCountPerRBPerPort(port_cnt))/2,((rb_cnt+1)*rsCountPerRBPerPort(port_cnt)-1)/2),refSignal(port_cnt).get((rb_cnt*rsCountPerRBPerPort(port_cnt))/2,((rb_cnt+1)*rsCountPerRBPerPort(port_cnt)-1)/2));
      else if(portNumbers(port_cnt)>=1000 && portNumbers(port_cnt)<=1007) // NR DMRS
	rsPerRB=refSignal(port_cnt).get(rb_cnt*rsCountPerRBPerPort(port_cnt),(rb_cnt+1)*rsCountPerRBPerPort(port_cnt)-1);
     else
     {
       cout<<"Unknown RS .."<<endl;abort();
     }
             /// Below line works assuming rbIndex=rbNumber ....
           cout<<"size of RSseq"<<referenceSignal->rsSequencePerRB.length()<<"ports ::"<<portIndx<<endl;
	   cout<<"size of RSseq sc"<<referenceSignal->rsSequencePerRB(portIndx).length()<<endl;
	   cout<<"scheduled RB ::"<<scheduledRBs(rb_cnt)<<endl;
       referenceSignal->rsSequencePerRB(portIndx)(scheduledRBs(rb_cnt))=rsPerRB;
      
       for(int ant_cnt=0;ant_cnt<portToAntennaMapperForMyPort.length();ant_cnt++)
      	referenceSignalPerAntenna(ant_cnt)=rsPerRB*portToAntennaMapperForMyPort(ant_cnt);
      
      frameHandler->setTonesInRU(referenceSignalPerAntenna,scheduledRBs(rb_cnt),toneType,true);
    }
  }
}

void generateUplinkReferenceSignal(int cellID,ivec scheduledRBs,int totalRBs , ivec portNumbers,ReferenceSignal_S *referenceSignal, RBInfo_S *rbInfo, string uplinkReferenceSignalScheme){
  cout<<"hello deepak generateUplinkReferenceSignal"<<endl;
  sort(portNumbers);
  portNumbers=remove(portNumbers,-2);
  portNumbers=remove(portNumbers,1200);
  ivec rsCountPerRBPerPort(portNumbers.length());
  //calculates the number of DMRS reference Signal to be transmitted in all the ports per subframe
  referenceSignal->rsSequencePerRB.set_length(portNumbers.length());
  referenceSignal->portIndices=portNumbers ;
  for(int port_cnt=0;port_cnt<portNumbers.length();port_cnt++)
  { rsCountPerRBPerPort(port_cnt)=count(rbInfo->rsPositionMatrix,rbInfo->rsInfo.portReferenceNumbers(find(rbInfo->rsInfo.rsPorts,portNumbers(port_cnt))));
    referenceSignal->rsSequencePerRB(port_cnt).set_length(totalRBs);
    for(int rb_cnt=0;rb_cnt<totalRBs;rb_cnt++)
      referenceSignal->rsSequencePerRB(port_cnt)(rb_cnt).set_length(0);
    
  }
  
  ivec dmrsPorts=getDMRSPortNumbers(portNumbers);
  int  dmrsLength=dmrsPorts.length();
  
  
  referenceSignal->DMRSsymbolsPerPort.set_length(dmrsLength);
  referenceSignal->DMRSsequencePerPortPerSymbol.set_length(dmrsLength);
  
  for(int rs_cnt=0;rs_cnt<dmrsLength;rs_cnt++)
  {
      //Need to check if using rsSymPosition is appropriate - Dhiv
    referenceSignal->DMRSsymbolsPerPort(rs_cnt)=rbInfo->rsInfo.rsSymPosition(find(rbInfo->rsInfo.rsPorts,dmrsPorts(rs_cnt)));
    referenceSignal->DMRSsequencePerPortPerSymbol(rs_cnt).set_length(1); //Assuming only 1 DMRS Symbol 
  }
  
  Size_S rbSize;
  rbSize.numberOfSubcarriers=rbInfo->rsPositionMatrix.rows();
  rbSize.numberOfSymbols=rbInfo->rsPositionMatrix.cols();
  
  // function getUplinkDMRS returns one port DMRS reference signal 
  Array<cvec> refSignal=getUplinkDMRS(cellID, scheduledRBs, portNumbers, rsCountPerRBPerPort, uplinkReferenceSignalScheme);
  for(int rs_cnt=0;rs_cnt<dmrsLength;rs_cnt++)
    referenceSignal->DMRSsequencePerPortPerSymbol(rs_cnt)(0)=refSignal(rs_cnt);
  
  cvec portToAntennaMapperForMyPort;
  string toneType;
  int portIndx;
  cvec rsPerRB;
  Array<ivec> rsPorsPerRB(totalRBs);
  for(int port_cnt=0;port_cnt<portNumbers.length();port_cnt++)
  {
    portIndx=port_cnt;
    referenceSignal->rsSequencePerRB(portIndx).set_length(totalRBs);
    
    toneType="port"+toString(portNumbers(port_cnt));
    for(int rb_cnt=0;rb_cnt<scheduledRBs.length();rb_cnt++)
    {
      if(portNumbers(port_cnt)<1000) // LTE RS
	// at both the symbols in an RB the same DMRS reference sybols are transmitting 
	rsPerRB=concat(refSignal(port_cnt).get((rb_cnt*rsCountPerRBPerPort(port_cnt))/2,((rb_cnt+1)*rsCountPerRBPerPort(port_cnt)-1)/2),refSignal(port_cnt).get((rb_cnt*rsCountPerRBPerPort(port_cnt))/2,((rb_cnt+1)*rsCountPerRBPerPort(port_cnt)-1)/2));
      else if(portNumbers(port_cnt)>=1000 && portNumbers(port_cnt)<=1007) // NR DMRS
	rsPerRB=refSignal(port_cnt).get(rb_cnt*rsCountPerRBPerPort(port_cnt),(rb_cnt+1)*rsCountPerRBPerPort(port_cnt)-1);
      else
      {
	cout<<"Unknown RS .."<<endl;abort();
      }
      /// Below line works assuming rbIndex=rbNumber ....
      referenceSignal->rsSequencePerRB(portIndx)(scheduledRBs(rb_cnt))=rsPerRB;
    }
  }
}

void getRSportShifts(ivec &cellSpecificSubcarrierUpShift, ivec &cellSpecificSymbolRightShift, ivec rsPorts, int cellID, int CSIRSconfig)
{
  int nCSIports = getCSIRSPortNumbers(rsPorts).length();
//   int nNRCSIports = getNRCSIRSPortNumbers(rsPorts).length();
  
   for (int i=0; i<rsPorts.length(); i++)
  {
    if(((rsPorts(i)>=0)&&(rsPorts(i)<4))||(rsPorts(i) == 61 || rsPorts(i) == 62))
    {
      cellSpecificSubcarrierUpShift(i) = mod(cellID , 6);
      cellSpecificSymbolRightShift(i) = 0;
    }
    else if(rsPorts(i) == 5)
    {
      cellSpecificSubcarrierUpShift(i) = mod(cellID , 3);
      cellSpecificSymbolRightShift(i) = 0;
    }
    else if((rsPorts(i) == 4)||((rsPorts(i)>6)&&(rsPorts(i)<15))||((rsPorts(i)>=1000)&&(rsPorts(i)<=1007))||((rsPorts(i)>=1100)&&(rsPorts(i)<=1111))||((rsPorts(i)>=1200)&&(rsPorts(i)<=1201)))
    {
      cellSpecificSubcarrierUpShift(i) = 0;
      cellSpecificSymbolRightShift(i) = 0;
    }
     else if((rsPorts(i) == 3500)||(rsPorts(i) == 3501))
    {
      cellSpecificSubcarrierUpShift(i) = 0;
      cellSpecificSymbolRightShift(i) = 0;
    }
    else if((rsPorts(i)>14)&&(rsPorts(i)<23))
      getCSIRSoffset(CSIRSconfig, nCSIports, cellSpecificSymbolRightShift(i), cellSpecificSubcarrierUpShift(i));
//     else if(((rsPorts(i)>=3000)&&(rsPorts(i)<=3032)) || (rsPorts(i)==3100))
//       getNRCSIRSoffset(CSIRSconfig, nNRCSIports, cellSpecificSymbolRightShift(i), cellSpecificSubcarrierUpShift(i));
    else
    {cout<<rsPorts(i)<<"[both:] Invalid portNumber in getRSportShifts()... "<<endl;abort();}
  }

}

void getCSIRSportShifts(int &CSIRSConfigSpecificSubcarrierUpShift, int &CSIRSConfigSpecificSymbolRightShift, int nCSIRSPorts, int CSIRSconfig)
{
  
  getCSIRSoffset(CSIRSconfig, nCSIRSPorts, CSIRSConfigSpecificSymbolRightShift, CSIRSConfigSpecificSubcarrierUpShift);	
  
}
void getNRCSIRSportShifts(int &CSIRSConfigSpecificSubcarrierUpShift, int &CSIRSConfigSpecificSymbolRightShift, int nNRCSIRSPorts, int CSIRSconfig)
{
  
  getNRCSIRSoffset(CSIRSconfig, nNRCSIRSPorts, CSIRSConfigSpecificSymbolRightShift, CSIRSConfigSpecificSubcarrierUpShift);	
  
}

Array<cvec> getTxRSPerPort(ReferenceSignal_S *refSignal,ResourceMapper *resourceMapper,int vrbNumber,ivec portIndices,imat rbRSPattern, bool isDownlink)
{
  ivec prbNumber;
  if(isDownlink)
    prbNumber=resourceMapper->dlResourceMapper.getPRBIndex(vrbNumber% resourceMapper->dlResourceMapper.getResourceBlocksPerSubframe());
  else
    prbNumber=resourceMapper->ulResourceMapper.getPRBIndex(vrbNumber);
  
  
  if(prbNumber(0)!=prbNumber(1)) // Distributed allocation
  {
    Array<cvec> vrbRSPerPort(portIndices.length());
    
    //Slot 0 RS
    int rsCountPerSlot;
    Array<cvec> prbRSPerPort=refSignal->getTxRSPerPort(prbNumber(0), portIndices);
    
    imat slotMat=rbRSPattern.get_cols(0,rbRSPattern.cols()/2-1);
    for(int port_cnt=0;port_cnt<portIndices.length();port_cnt++)
    {
      rsCountPerSlot=count(slotMat,portIndices(port_cnt)+1);
      vrbRSPerPort(port_cnt)=prbRSPerPort(port_cnt).left(rsCountPerSlot);
    } 
    //Slot 1 RS
    slotMat=rbRSPattern.get_cols(rbRSPattern.cols()/2, rbRSPattern.cols()-1);
    prbRSPerPort=refSignal->getTxRSPerPort(prbNumber(1), portIndices);
    for(int port_cnt=0;port_cnt<portIndices.length();port_cnt++)
    {
      rsCountPerSlot=count(slotMat,portIndices(port_cnt)+1);
      append(vrbRSPerPort(port_cnt),prbRSPerPort(port_cnt).right(rsCountPerSlot));
    } 
    
    return(vrbRSPerPort);
  }
  else //Localised allocation
    return(refSignal->getTxRSPerPort(prbNumber(0), portIndices));  
}

