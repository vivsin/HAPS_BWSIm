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

#ifndef HARQMANAGER_H
#define HARQMANAGER_H

#include "../../../L1/PHY/include/StructsAndEnums.h"



/*! \brief Structure for HARQ Process
      */
struct HARQProcessInHARQManager
{
    bvec isAckNackReceived;
    ivec TBidPerCodeWord;
    int lastTxframe;
    ivec txCountPerCodeWord;
    ivec tbSizePerCodeWord;
    Array<bvec> pktReceptionStatusPerCodewordPerCB;
    bvec pktReceptionStatusPerCodeword;
    bvec newDataIndicator_old; 
    HarqInfo currentHarqInfo;  
    
    HARQProcessInHARQManager() {
      isAckNackReceived=zeros_b(2);
      lastTxframe=1e6;
      TBidPerCodeWord="88888888";
      txCountPerCodeWord.set_length(0); 
      tbSizePerCodeWord.set_length(0); 
      pktReceptionStatusPerCodewordPerCB.set_length(0);
      pktReceptionStatusPerCodeword.set_length(0);
      newDataIndicator_old.set_length(0);
    }
};


struct OuterloopLinkAdaptation_S
{
    double nackOffsetInDB; 
    double ackOffsetInDB; 
    double powerOffsetInDB;
    
    OuterloopLinkAdaptation_S()
    {
        nackOffsetInDB = -0.45;
        ackOffsetInDB = 0.05;
        powerOffsetInDB = 0;
    }
    void addOffset(bool isSuccess)
    {
        if(isSuccess)
            powerOffsetInDB += ackOffsetInDB;
        else
            powerOffsetInDB += nackOffsetInDB;
    }
};

class HARQManager
{
  private :
    string harqType;
    int maximumHarqProcesses;
    int minDelayBetweenSuccessiveTransmissions;	
    int maximumHarqTransmissions;
    int nTBTransmitted;	
    int harqIDUsedForLastTransmission;	
    ivec rvSequenceForRateMatching;
    Array<HARQProcessInHARQManager> harqProcessesInHARQManager; 
    bool isHarqProcessInHARQManagerEligible(const HARQProcessInHARQManager &harqProcessInHARQManager,const int& subframeNo);
    int getMostEligibleHarqProcessID(const int& subframeNo, ivec& codeWordIndicesToConsider, int& numCodeWordsRequired);
    OuterloopLinkAdaptation_S olla;

  public :
    HARQManager();
    void initializeHarqManager(const string& harqType="IR", const int& maximumHarqProcesses=8, const int& minDelayBetweenSuccessiveTransmissions=8, const int& maximumHarqTransmissions=4);
    HarqInfo getCurrentHarqInfo(const int& subframeNo, int numCodeWordsRequired=1);
    ivec getCodewordsToConsider(int subframeNo, int nCodewords);
    bvec isNewtransmitting(int subframeNo, int numCodeWordsRequired = 1);
    bool isRetransmitting(int subframeNo, int nCodewords=1);
    void setTransmissionStatus(const HarqInfo& harqInfo,const int& subframeNo);
    void setReceptionStatus(bvec pktReceptionStatusPerCodeword, Array< bvec > pktReceptionStatusPerCodewordPerCB="", int harqID = 1, Array<vec> harqBuffer="");
    void updateRVSeqRateMatching(ivec rvSeqForRateMatching);
    void clear();
};


template<class T> int min_index_firstDimension(Array<Vec<T> > input) {
    Vec<T> temp; temp.set_length(input.length());
    for (int i=0;i<input.length();++i) {
        temp(i) = min(input(i));
    }
    return min_index(temp);
}

template<class T> vec mean_lastDimension(Array<Vec<T> > input) {
    vec out; out.set_length(input.length());
    for (int i=0;i<input.length();++i) {
        out(i) = mean(to_vec(input(i)));
    }
    return out;
}


#endif
