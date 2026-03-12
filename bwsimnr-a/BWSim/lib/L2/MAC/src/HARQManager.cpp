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

/*! \Note All the entities for codeWords within this file , is indexed with Actual codeWord Index.
 *         Whereas in harqInfoOutput & ChannelCoder, codeWordIndices corresponds to permuted CodeWord Index
 */
#include "../include/HARQManager.h"

HARQManager::HARQManager(){
  maximumHarqProcesses=-1;
  minDelayBetweenSuccessiveTransmissions=-1;
  maximumHarqTransmissions=-1;
  nTBTransmitted=-1;
  harqIDUsedForLastTransmission=-1;
  rvSequenceForRateMatching.set_length(0);
  
}

void HARQManager::initializeHarqManager(const string& harqType, const int& maximumHarqProcesses, const int& minDelayBetweenSuccessiveTransmissions, const int& maximumHarqTransmissions)
{
  if(maximumHarqProcesses == 0 || maximumHarqTransmissions == 0)
  {
    cout<<"[both:] maximumHarqProcesses and maximumHarqTransmissions should be > 0"<<endl;
    abort();
  }
  else if(maximumHarqProcesses < minDelayBetweenSuccessiveTransmissions)
  {
    cout<<"[both:] Error: _maxLTE_HARQ_PROCESS must be >= _minDelayBetweenTransmissions...";
    abort();
  }
  if(maximumHarqTransmissions == 1)
    cout<<"[both:] Warning: Running in NO-HARQ case \n";

  this->harqType = harqType;
  this->maximumHarqProcesses = maximumHarqProcesses;
  this->minDelayBetweenSuccessiveTransmissions = minDelayBetweenSuccessiveTransmissions;
  this->maximumHarqTransmissions = maximumHarqTransmissions;
  this->harqProcessesInHARQManager.set_size(this->maximumHarqProcesses);

  //! RV-Index sequence to be used for HARQ transmissions... \ref ETSI TS 136 104 V10.6.0 (2012-03) : Section 8.2.1-1 (page:74)
  this->rvSequenceForRateMatching = ivec("0 2 3 1 0 2 3 1");
}

/*! \brief Function to update RV sequence for rate matching */
void HARQManager::updateRVSeqRateMatching(ivec rvSeqForRateMatching)
{
   this->rvSequenceForRateMatching.zeros();
   this->rvSequenceForRateMatching = rvSeqForRateMatching;

}

/*! \brief Compute the HarqID, newDataIndicator and rvIndex for the transmission in current subframe for a UE
\b Eligible \b harqProcess having minimum TBidPerCodeWord is selected  */
HarqInfo HARQManager::getCurrentHarqInfo(const int& subframeNo, int numCodeWordsRequired) {

  HarqInfo harqInfoOutput;
  vec concatedTBid;
  ivec concatedHarqID;
  harqInfoOutput.rvIndicesPerCodeword.set_size(numCodeWordsRequired);
  
  if(subframeNo == 0)
  {
    clear();
  }

  //! Select Most eligible HarqProcess
  harqInfoOutput.harqProcessID = getMostEligibleHarqProcessID(subframeNo,harqInfoOutput.codeWordIndicesToConsider,numCodeWordsRequired);
  bvec isNewTransmission = isNewtransmitting(subframeNo, numCodeWordsRequired);
  harqInfoOutput.isNewTransmission = isNewTransmission;
  harqInfoOutput.codeBlockIndicesToConsider.set_length(isNewTransmission.length());
  for(int iiCodeWord=0; iiCodeWord < harqInfoOutput.codeWordIndicesToConsider.length(); iiCodeWord++)
    if(!isNewTransmission(iiCodeWord) && harqProcessesInHARQManager(harqInfoOutput.harqProcessID).pktReceptionStatusPerCodewordPerCB(iiCodeWord).length())
    {
      harqInfoOutput.codeBlockIndicesToConsider(iiCodeWord)=find(harqProcessesInHARQManager(harqInfoOutput.harqProcessID).pktReceptionStatusPerCodewordPerCB(iiCodeWord));
    }
  //! Now toggle the newDataIndicator, if it's a new transmission...
  harqInfoOutput.newDataIndicator.set_length(isNewTransmission.length());
  for(int iiCodeWord=0; iiCodeWord < harqInfoOutput.codeWordIndicesToConsider.length(); iiCodeWord++) {
    int iCodeWord=harqInfoOutput.codeWordIndicesToConsider(iiCodeWord);
    if(length(harqProcessesInHARQManager(harqInfoOutput.harqProcessID).newDataIndicator_old) <= iCodeWord) {
      harqProcessesInHARQManager(harqInfoOutput.harqProcessID).newDataIndicator_old.set_length(iCodeWord+1,true);
      harqProcessesInHARQManager(harqInfoOutput.harqProcessID).newDataIndicator_old(iCodeWord) = 1;             //! In order to make starting point of newDataIndicator to be 0
    }
    if(length(harqProcessesInHARQManager(harqInfoOutput.harqProcessID).isAckNackReceived) <= iCodeWord) {
      harqProcessesInHARQManager(harqInfoOutput.harqProcessID).isAckNackReceived.set_length(iCodeWord+1,true);
      harqProcessesInHARQManager(harqInfoOutput.harqProcessID).isAckNackReceived(iCodeWord) = 0;
    }
    harqInfoOutput.newDataIndicator(iiCodeWord) = isNewTransmission(iiCodeWord) ^ harqProcessesInHARQManager(harqInfoOutput.harqProcessID).newDataIndicator_old(iCodeWord);
  }

  //! Now Compute rvIndicesPerCodeword looking at whether choosen harqProcessesInHARQManager is successfully transmitted last time..
  for(int iiCodeWord=0; iiCodeWord < harqInfoOutput.codeWordIndicesToConsider.length(); iiCodeWord++) {
    int iCodeWord=harqInfoOutput.codeWordIndicesToConsider(iiCodeWord);
    harqInfoOutput.rvIndicesPerCodeword(iiCodeWord) = (isNewTransmission(iiCodeWord) || harqType == "CC")? 0:rvSequenceForRateMatching(rem(harqProcessesInHARQManager(harqInfoOutput.harqProcessID).txCountPerCodeWord(iCodeWord),rvSequenceForRateMatching.length()));
  }
  harqInfoOutput.TBSizePerCodewordInBits=zeros_i(harqInfoOutput.codeWordIndicesToConsider.length())-1;
  harqInfoOutput.infoBitsperCodeWord.set_length(harqInfoOutput.codeWordIndicesToConsider.length());
  harqInfoOutput.harqBuffer.set_length(harqInfoOutput.codeWordIndicesToConsider.length());
  
  
  for(int iiCodeWord=0; iiCodeWord < harqInfoOutput.codeWordIndicesToConsider.length(); iiCodeWord++)
  {
      harqInfoOutput.infoBitsperCodeWord(iiCodeWord).set_length(0);
      harqInfoOutput.harqBuffer(iiCodeWord).set_length(0);
      
    if(!harqInfoOutput.isNewTransmission(iiCodeWord))
    {
      harqInfoOutput.TBSizePerCodewordInBits(iiCodeWord)=harqProcessesInHARQManager(harqInfoOutput.harqProcessID).tbSizePerCodeWord(harqInfoOutput.codeWordIndicesToConsider(iiCodeWord));
      harqInfoOutput.infoBitsperCodeWord(iiCodeWord)=harqProcessesInHARQManager(harqInfoOutput.harqProcessID).currentHarqInfo.infoBitsperCodeWord(harqInfoOutput.codeWordIndicesToConsider(iiCodeWord));
        
    }
  }
  harqInfoOutput.powerOffsetInDBFromOLLA = olla.powerOffsetInDB;
  return(harqInfoOutput);
}

ivec HARQManager::getCodewordsToConsider(int subframeNo, int nCodewords){
  ivec codeWordIndicesToConsider;
  getMostEligibleHarqProcessID(subframeNo,codeWordIndicesToConsider,nCodewords);
  return codeWordIndicesToConsider;
}


bvec HARQManager::isNewtransmitting(int subframeNo, int numCodeWordsRequired){
  
  HarqInfo harqInfoOutput;
  vec concatedTBid;
  ivec concatedHarqID;
  
  harqInfoOutput.harqProcessID = getMostEligibleHarqProcessID(subframeNo,harqInfoOutput.codeWordIndicesToConsider,numCodeWordsRequired);
    
  bvec isNewTransmission;
  isNewTransmission.set_length(numCodeWordsRequired);     //! Indicate whether it's new transmission Per CodeWord(Output)
  harqInfoOutput.rvIndicesPerCodeword.set_size(numCodeWordsRequired);

  
  //! Now set isNewTransmission looking at whether choosen harqProcessesInHARQManager is successfully transmitted last time or not..
  for(int iiCodeWord=0; iiCodeWord < harqInfoOutput.codeWordIndicesToConsider.length(); iiCodeWord++)
  {
    int iCodeWord=harqInfoOutput.codeWordIndicesToConsider(iiCodeWord);
    //! For dropped packets, do New-Transmission
    if(!harqProcessesInHARQManager(harqInfoOutput.harqProcessID).isAckNackReceived(iCodeWord))
      isNewTransmission(iiCodeWord) = true;
    //! If last pkt is successfull, or maximum number of transmissions are done, then do New-Transmission. Else do Re-Transmission.
    else if(harqProcessesInHARQManager(harqInfoOutput.harqProcessID).txCountPerCodeWord.length()>iCodeWord)
      isNewTransmission(iiCodeWord) = (harqProcessesInHARQManager(harqInfoOutput.harqProcessID).txCountPerCodeWord(iCodeWord) >= maximumHarqTransmissions || harqProcessesInHARQManager(harqInfoOutput.harqProcessID).pktReceptionStatusPerCodeword(iCodeWord))? true:false;
    else
      isNewTransmission(iiCodeWord) = true;
  }
  return isNewTransmission;
}

/*! \brief If any of the codeWord is re-transmission, then return true... */
bool HARQManager::isRetransmitting(int subframeNo, int nCodewords){

  bvec isNewTransmissionPerCodeword = isNewtransmitting(subframeNo, nCodewords);
  for(int iCodeword=0;iCodeword<isNewTransmissionPerCodeword.length();iCodeword++){
    if(!isNewTransmissionPerCodeword(iCodeword))
      return true;
  }
  return false;
}

/*! \details Following function is called everytime a pkt is transmitted.
 *  \warning HarqManager assumes (total # of transmission(fresh/re)) = ()# calls to this function)
 */
void HARQManager::setTransmissionStatus(const HarqInfo& harqInfo, const int& subframeNo)
{
    if(traceValuePerTraceName["printHARQManagerDebugInfo"]=="true")
      cout<<"In setTransmissionStatus harqInfo.codeWordIndicesToConsider = "<<harqInfo.codeWordIndicesToConsider<<" harqInfo.newDataIndicator = "<<harqInfo.newDataIndicator<<endl;
    int maxCodeWords = max(max(harqInfo.codeWordIndicesToConsider)+1,harqProcessesInHARQManager(harqInfo.harqProcessID).newDataIndicator_old.length());
    bvec isNewTransmission(maxCodeWords);     //! Indicate whether it's new transmission Per CodeWord

    harqIDUsedForLastTransmission = harqInfo.harqProcessID;
    harqProcessesInHARQManager(harqInfo.harqProcessID).txCountPerCodeWord.set_size(maxCodeWords,true);
    harqProcessesInHARQManager(harqInfo.harqProcessID).tbSizePerCodeWord.set_size(maxCodeWords,true);
    
    if(harqProcessesInHARQManager(harqInfo.harqProcessID).TBidPerCodeWord.length()<maxCodeWords) // To depricate the preference of new harq proces over new transmission
      append(harqProcessesInHARQManager(harqInfo.harqProcessID).TBidPerCodeWord,int(1e9)*ones_i(maxCodeWords-harqProcessesInHARQManager(harqInfo.harqProcessID).TBidPerCodeWord.length()));
    else
      harqProcessesInHARQManager(harqInfo.harqProcessID).TBidPerCodeWord.set_size(maxCodeWords,true);
    harqProcessesInHARQManager(harqInfo.harqProcessID).pktReceptionStatusPerCodeword.set_size(maxCodeWords,true);
    harqProcessesInHARQManager(harqInfo.harqProcessID).pktReceptionStatusPerCodewordPerCB.set_size(maxCodeWords,true);
    for(int cwd_cnt=0;cwd_cnt<maxCodeWords;cwd_cnt++)
        harqProcessesInHARQManager(harqInfo.harqProcessID).pktReceptionStatusPerCodewordPerCB(cwd_cnt).set_size(0);
    
    
    //! ******************************** Find out if it's new/re transmission... *********************** !//
    for(int iiCodeWord=0; iiCodeWord < harqInfo.codeWordIndicesToConsider.length(); iiCodeWord++) 
    {
      int iCodeWord=harqInfo.codeWordIndicesToConsider(iiCodeWord);
      harqProcessesInHARQManager(harqInfo.harqProcessID).isAckNackReceived(iCodeWord) = false;
      if(length(harqProcessesInHARQManager(harqInfo.harqProcessID).newDataIndicator_old) <= iCodeWord) {
        harqProcessesInHARQManager(harqInfo.harqProcessID).newDataIndicator_old.set_length(iCodeWord+1,true);
        harqProcessesInHARQManager(harqInfo.harqProcessID).newDataIndicator_old(iCodeWord) = 1;             //! In order to make starting point of newDataIndicator to be 0
      }
      isNewTransmission(iCodeWord) = harqInfo.newDataIndicator(iiCodeWord) ^ harqProcessesInHARQManager(harqInfo.harqProcessID).newDataIndicator_old(iCodeWord);
      //! Backup the newDataIndicator...
      harqProcessesInHARQManager(harqInfo.harqProcessID).newDataIndicator_old(iCodeWord) = harqInfo.newDataIndicator(iiCodeWord);
    }
    /* ************************************************** ****************************** ************* */

    //! ******************************** Assign TBidPerCodeWord & txCountPerCodeWord *********************** !//
    harqProcessesInHARQManager(harqInfo.harqProcessID).lastTxframe = subframeNo;
    for(int iiCodeWord=0; iiCodeWord < harqInfo.codeWordIndicesToConsider.length(); iiCodeWord++) {
      int iCodeWord=harqInfo.codeWordIndicesToConsider(iiCodeWord);
      if(isNewTransmission(iCodeWord)) {
	harqProcessesInHARQManager(harqInfo.harqProcessID).tbSizePerCodeWord(iCodeWord)=harqInfo.TBSizePerCodewordInBits(iiCodeWord);
	harqProcessesInHARQManager(harqInfo.harqProcessID).TBidPerCodeWord(iCodeWord) = ++ nTBTransmitted;
        harqProcessesInHARQManager(harqInfo.harqProcessID).txCountPerCodeWord(iCodeWord) = 1;
      }
      else
        harqProcessesInHARQManager(harqInfo.harqProcessID).txCountPerCodeWord(iCodeWord) += 1;
    }
    /* ************************************************** ****************************** ************* */

    //! Backup the harqInfo ...
    harqProcessesInHARQManager(harqInfo.harqProcessID).currentHarqInfo = harqInfo;

    //! Print HARQ DebugInfo ...
    if(traceValuePerTraceName["printHARQDebugInfo"]=="true")
      cout<<"HARQManager::setTransmissionStatus harqProcessID = "<<harqInfo.harqProcessID<<" codeWordIndicesToConsider = "<<harqInfo.codeWordIndicesToConsider
        <<" newDataIndicator = "<<harqInfo.newDataIndicator<<" rvIndicesPerCodeword = "<<harqInfo.rvIndicesPerCodeword<<endl;
}

/*! \brief Set the Reception-Status (ACK/NACK)
 */
void HARQManager::setReceptionStatus(bvec pktReceptionStatusPerCodeword ,Array<bvec> pktReceptionStatusPerCodewordPerCB,int harqID, Array<vec> harqBuffer)
{
    //! If harqID is not specified, then pick the stored value...
    if(harqID==-1) harqID = harqIDUsedForLastTransmission;


    //! Correctly store the pktReceptionStatusPerCodeword, & assign TBidPerCodeWord
    for(int iiCodeWord=0; iiCodeWord < harqProcessesInHARQManager(harqID).currentHarqInfo.codeWordIndicesToConsider.length(); iiCodeWord++) 
    {
      int iCodeWord=harqProcessesInHARQManager(harqID).currentHarqInfo.codeWordIndicesToConsider(iiCodeWord);
      harqProcessesInHARQManager(harqID).isAckNackReceived(iCodeWord) = true;
      if(pktReceptionStatusPerCodewordPerCB.length())
        harqProcessesInHARQManager(harqID).pktReceptionStatusPerCodewordPerCB(iCodeWord) = pktReceptionStatusPerCodewordPerCB(iiCodeWord);
      harqProcessesInHARQManager(harqID).pktReceptionStatusPerCodeword(iCodeWord) = pktReceptionStatusPerCodeword(iiCodeWord);
      
      if(harqProcessesInHARQManager(harqID).pktReceptionStatusPerCodeword(iCodeWord)) //! If last Pkt is success, then give less priority for choosing this harqProcess..
          harqProcessesInHARQManager(harqID).TBidPerCodeWord(iCodeWord) = 1e8;
      
      if(harqBuffer.length())
        if(!harqProcessesInHARQManager(harqID).pktReceptionStatusPerCodeword(iCodeWord))
          harqProcessesInHARQManager(harqID).currentHarqInfo.harqBuffer(iCodeWord) = harqBuffer(iiCodeWord);
        
        olla.addOffset(pktReceptionStatusPerCodeword(iiCodeWord)); 
    }

    //! Print HARQ DebugInfo ...
    if(traceValuePerTraceName["printHARQDebugInfo"]=="true")
      cout<<"HARQManager::setReceptionStatus harqID = "<<harqID<<" pktReceptionStatusPerCodewordPerCBG = "<<pktReceptionStatusPerCodewordPerCB<<endl;
}
/*! \brief Clear the HarqManager to it's intial state..
 */
void HARQManager::clear()
{
    nTBTransmitted = 0;
    harqIDUsedForLastTransmission = 0;
    for(int harqID=0; harqID<harqProcessesInHARQManager.length(); harqID++)	//! Pick the eligible oldest HARQProcessesInHARQManager
    {
	harqProcessesInHARQManager(harqID).lastTxframe = -1e5;
	harqProcessesInHARQManager(harqID).pktReceptionStatusPerCodeword.ones();
        harqProcessesInHARQManager(harqID).txCountPerCodeWord.zeros();
	harqProcessesInHARQManager(harqID).newDataIndicator_old.ones();
        for(int iiCodeWord=0; iiCodeWord < harqProcessesInHARQManager(harqID).currentHarqInfo.codeWordIndicesToConsider.length(); iiCodeWord++) {
          int iCodeWord=harqProcessesInHARQManager(harqID).currentHarqInfo.codeWordIndicesToConsider(iiCodeWord);
          harqProcessesInHARQManager(harqID).TBidPerCodeWord(iCodeWord) = 1e8;
        }
    }
}

/*! \brief Function to return most eligible harqID & codeWordIndices to be transmitted inside that harqProcesses...
 *  \details most eligible harqProcess is the one, which has got minimum TBidPerCodeWord
 * */
int HARQManager::getMostEligibleHarqProcessID(const int& subframeNo,ivec & codeWordIndicesToConsider,int& numCodeWordsRequired)
{
  ivec concatedHarqID;
  Array<Array<ivec> > tbIDsPerHarqProcessPerCodeWordCombination;
  Array<Array<ivec> > codeWordIndicesPerHarqProcessPerCodeWordCombination;
  Array<vec> tbIDsPerHarqProcessPerCodeWordCombination_mean;
  ivec nCodewords(harqProcessesInHARQManager.length());
  //! ******************************** First Select the most elligible harqProcess *********************** !//
  int runIndex=0; ivec runIndexPerHarqID(0);
  for(int harqID=0; harqID<harqProcessesInHARQManager.length(); harqID++)       //! Pick the eligible oldest HARQProcessesInHARQManager
    if(isHarqProcessInHARQManagerEligible(harqProcessesInHARQManager(harqID),subframeNo))
    {
      runIndexPerHarqID.set_length(harqID+1,true); runIndexPerHarqID(harqID)=runIndex;
      append(concatedHarqID,harqID);
      tbIDsPerHarqProcessPerCodeWordCombination.set_length(runIndex+1,true); tbIDsPerHarqProcessPerCodeWordCombination(runIndex).set_length(0);
      codeWordIndicesPerHarqProcessPerCodeWordCombination.set_length(runIndex+1,true); codeWordIndicesPerHarqProcessPerCodeWordCombination(runIndex).set_length(0);
      tbIDsPerHarqProcessPerCodeWordCombination_mean.set_length(runIndex+1,true); tbIDsPerHarqProcessPerCodeWordCombination_mean(runIndex).set_length(0);
      ivec TBidPerCodeWord(harqProcessesInHARQManager(harqID).isAckNackReceived.length());
      for(int iCodeWord=0; iCodeWord<harqProcessesInHARQManager(harqID).isAckNackReceived.length(); iCodeWord++)
	if(harqProcessesInHARQManager(harqID).isAckNackReceived(iCodeWord))
	  TBidPerCodeWord(iCodeWord)= harqProcessesInHARQManager(harqID).TBidPerCodeWord(iCodeWord);
	else
	  TBidPerCodeWord(iCodeWord)= 1e8;
      //! if numCodeWordsRequired is more, then take the average of all existing codeWords...
      nCodewords(harqID)=numCodeWordsRequired;
      
      if(nCodewords(harqID)==-1)
	nCodewords(harqID)=harqProcessesInHARQManager(harqID).TBidPerCodeWord.length(); // taking max codewords;
	
      if(harqProcessesInHARQManager(harqID).TBidPerCodeWord.length()<nCodewords(harqID)) {
          append(tbIDsPerHarqProcessPerCodeWordCombination(runIndex),TBidPerCodeWord);
          append(codeWordIndicesPerHarqProcessPerCodeWordCombination(runIndex),getIntegers(0,harqProcessesInHARQManager(harqID).TBidPerCodeWord.length()-1));
      }
      //! Else get All combination (TODO Feature Addition: Has to improve the algorithm to get all combinations, needed iff nCodeWords>2 implemented...)
      for (int iCodeWord=0; iCodeWord<harqProcessesInHARQManager(harqID).TBidPerCodeWord.length()-nCodewords(harqID)+1; iCodeWord++) {
          append(tbIDsPerHarqProcessPerCodeWordCombination(runIndex),TBidPerCodeWord(iCodeWord,iCodeWord+nCodewords(harqID)-1));
          append(codeWordIndicesPerHarqProcessPerCodeWordCombination(runIndex),getIntegers(iCodeWord,iCodeWord+nCodewords(harqID)-1));
      }
      tbIDsPerHarqProcessPerCodeWordCombination_mean(runIndex) = mean_lastDimension(tbIDsPerHarqProcessPerCodeWordCombination(runIndex));
      runIndex++;
    }

  if(tbIDsPerHarqProcessPerCodeWordCombination_mean.length() == 0) { cout<<"[both:] Error: No Eligible HARQ process..."; abort(); }
  int harqIDtoReturn = concatedHarqID(min_index_firstDimension(tbIDsPerHarqProcessPerCodeWordCombination_mean));
  codeWordIndicesToConsider = codeWordIndicesPerHarqProcessPerCodeWordCombination(runIndexPerHarqID(harqIDtoReturn))(min_index(tbIDsPerHarqProcessPerCodeWordCombination_mean(runIndexPerHarqID(harqIDtoReturn))));
  /* ******************************** ******************************************** ******************** */
  //! ******************************** Now Make codeWordIndicesToConsider proper size... ************* !//
  //! *** Now make harqInfoOutput.codeWordIndicesToConsider = [harqInfoOutput.codeWordIndicesToConsider] Union [0:numCodeWordsRequired-1]
  append(codeWordIndicesToConsider ,remove(to_ivec(linspace(0,nCodewords(harqIDtoReturn)-1,nCodewords(harqIDtoReturn))),codeWordIndicesToConsider));
  //! *** Now make proper size, by remove the extra elements in harqInfoOutput.codeWordIndicesToConsider
  codeWordIndicesToConsider = codeWordIndicesToConsider.left(nCodewords(harqIDtoReturn));
  numCodeWordsRequired=nCodewords(harqIDtoReturn);
  /* ******************************** ************************************************** ****************************** ************* */
  return harqIDtoReturn;
}


/*! \brief Check if harqProcessInHARQManager is elligible for transmission in current subframe
 * HarqProcess is eligible, if any of the following conditions true.
 * 1. If minDelayBetweenSuccessiveTransmissions is satisfied
 */
bool HARQManager::isHarqProcessInHARQManagerEligible(const HARQProcessInHARQManager& harqProcessInHARQManager,const int& subframeNo)
{
    //! If delay constraints in successive transmissions is met, then harqProcess is eligible for transmission...
    if(subframeNo-harqProcessInHARQManager.lastTxframe >= minDelayBetweenSuccessiveTransmissions)
      return true;
    else
      return false;
}
