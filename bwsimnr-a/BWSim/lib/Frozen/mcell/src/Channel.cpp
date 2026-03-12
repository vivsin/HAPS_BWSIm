/*************************************************************************
*
* CEWiT CONFIDENTIAL
* __________________
*
* All Rights Reserved Â© 2012 CEWiT, India
*
* NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
* and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
* Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
* express, printed and signed license for use is strictly forbidden.
*/

#include "../include/Channel.h"
#include <cmath>
std::ofstream gain("netGain.txt");
std::ofstream ch_gain("channelgain.txt");
using std::cos;
using std::sin;

vec rndVec_gen(unsigned int state,int size);
double rnd_gen(unsigned int &state);
double gauss_rnd_gen(unsigned int &state);

ChannelFilter::ChannelFilter():inputBuffer(0),filterBuffer(0) {}

void ChannelFilter::init(double samplingFrequency,int antennaCount, bool timeDomain,int filterLength){

  sampleIntervalInSec=1/samplingFrequency;
  currentTimePerBuffer=zeros(antennaCount)-1;
  endTimePerBuffer=zeros(antennaCount);
  isTimeDomain=timeDomain;
  inputBuffer.set_length(antennaCount);
  for(int ant_cnt=0;ant_cnt<antennaCount;ant_cnt++)
    inputBuffer(ant_cnt).set_size(0);
  
  if(timeDomain)
  {
    if(filterLength>0)
    {
      filterBuffer.set_length(antennaCount);
      for(int ant_cnt=0;ant_cnt<antennaCount;ant_cnt++)
	filterBuffer(ant_cnt)=zeros_c(filterLength);
    }
    else
      {cout<<"[both:]Error. Filter length need to be specified for timeDomain channel in ChannelFilter."<<endl;abort();}
  }
}

void ChannelFilter::load(Array< cvec >& input, double currentTimeInSec, int overwriteOrAdd){

  if(inputBuffer(0).length()!=0)
  {
    if(overwriteOrAdd==1) //overwrite
    {
      if(isTimeDomain)
      {
	endTimePerBuffer=currentTimePerBuffer+inputBuffer(0).length()*sampleIntervalInSec;
	if(currentTimeInSec<endTimePerBuffer(0))
	{
	  int bufferLoadShift=round_i((currentTimeInSec-endTimePerBuffer(0))/sampleIntervalInSec);
	  for(int ant_cnt=0;ant_cnt<filterBuffer.length();ant_cnt++)
	  {
	    for(int data_cnt=0;data_cnt<bufferLoadShift;data_cnt++)
	      filterBuffer(ant_cnt).shift_right(inputBuffer(ant_cnt)(data_cnt));
	  }
	}
	else
	{
	  int bufferLoadShift=round_i((currentTimeInSec-endTimePerBuffer(0))/sampleIntervalInSec);
	  for(int ant_cnt=0;ant_cnt<filterBuffer.length();ant_cnt++)
	  {
	    // Here input buffer is yet to be updated, so belongs to N-1 subframe
	    for(int data_cnt=0;data_cnt<filterBuffer(ant_cnt).length();data_cnt++)
	      filterBuffer(ant_cnt)(data_cnt)=inputBuffer(ant_cnt)(inputBuffer(ant_cnt).length()-1-data_cnt);
	    if(bufferLoadShift)
	    {
	      if(filterBuffer(ant_cnt).length()<=bufferLoadShift)
		filterBuffer(ant_cnt).clear();
	      else
		filterBuffer(ant_cnt).shift_right(complex< double >(0,0),bufferLoadShift);
	    }
	  }
	}
      }
      inputBuffer=input;
      currentTimePerBuffer=currentTimeInSec;
    }
    else if(overwriteOrAdd==2) // add
    {
      if(isTimeDomain)
      {
	endTimePerBuffer=currentTimePerBuffer+inputBuffer(0).length()*sampleIntervalInSec;
	if((endTimePerBuffer(0)-currentTimeInSec)>1e-10)
	{
// 	  cout<<"Adding up data in channel.."<<endl;
	  
	  if(input.length()!=inputBuffer.length())
	  {
	    cout<<"Error. Antenna SizeMismatch.."<<endl;abort();
	  }
	  
	  double endTimeForLoad=currentTimeInSec+input(0).length()*sampleIntervalInSec;
	  	  
	  // This procedure won't affect filter buffer as it is expired data
	  if(endTimeForLoad>endTimePerBuffer(0)) // Input is contained in the inputBuffer; 
	  {
	    int zerosToPad=round_i((endTimeForLoad-endTimePerBuffer(0))/sampleIntervalInSec);
	    for(int ant_cnt=0;ant_cnt<inputBuffer.length();ant_cnt++) // assuming input's antenna and inputBuffer's antenna are always same
	      append(inputBuffer(ant_cnt),zeros_c(zerosToPad)); 
	  }
	  
	  int bufferLoadShift=round_i((currentTimeInSec-currentTimePerBuffer(0))/sampleIntervalInSec);
	  for(int ant_cnt=0;ant_cnt<input.length();ant_cnt++) // assuming input's antenna and inputBuffer's antenna are always same
	  {
	    for(int data_cnt=bufferLoadShift;data_cnt<bufferLoadShift+input(ant_cnt).length();data_cnt++)
            inputBuffer(ant_cnt)(data_cnt)+=input(ant_cnt)(data_cnt-bufferLoadShift);
	  }
	}
	else
	{
	  int bufferLoadShift=round_i((currentTimeInSec-endTimePerBuffer(0))/sampleIntervalInSec);
	  for(int ant_cnt=0;ant_cnt<filterBuffer.length();ant_cnt++)
	  {
	    // Here input buffer is yet to be updated, so belongs to N-1 subframe
	    for(int data_cnt=0;data_cnt<filterBuffer(ant_cnt).length();data_cnt++)
	      filterBuffer(ant_cnt)(data_cnt)=inputBuffer(ant_cnt)(inputBuffer(ant_cnt).length()-1-data_cnt);
	    if(bufferLoadShift)
	    {
	      if(filterBuffer(ant_cnt).length()<=bufferLoadShift)
		filterBuffer(ant_cnt).clear();
	      else
		filterBuffer(ant_cnt).shift_right(complex< double >(0,0),bufferLoadShift);
	    }
	  }
	  inputBuffer=input;
	  currentTimePerBuffer=currentTimeInSec;
	}
      }
    }
    else
    {
      cout<<"[detl:]Warning. Trying to overwrite data in ChannelFilter."<<endl;
      abort();
    }
  }
  else
  {
    inputBuffer=input;
    currentTimePerBuffer=currentTimeInSec;
    if(isTimeDomain)
    {
      endTimePerBuffer=currentTimePerBuffer;
      for(int ant_cnt=0;ant_cnt<filterBuffer.length();ant_cnt++)
	filterBuffer(ant_cnt)=zeros_c(filterBuffer(ant_cnt).length());
    }
  }

}

void ChannelFilter::load(Array< cvec >& input, ivec indices, double currentTimeInSec, int overwriteOrAdd){
  
  //    cout<<"input buffer length ::"<<inputBuffer.length()<<endl;
  //    cout<<"input length ::"<<input.length()<<endl;
  
  if(inputBuffer(indices(0)).length()!=0)
  {
    if(overwriteOrAdd==1) //overwrite
    {
      if(isTimeDomain)
      {
	for(int indx=0;indx<indices.length();indx++)
	{
	  int ant_cnt=indices(indx);
	  endTimePerBuffer(ant_cnt)=currentTimePerBuffer(ant_cnt)+inputBuffer(ant_cnt).length()*sampleIntervalInSec;
	  if(currentTimeInSec<endTimePerBuffer(indices(indx)))
	  {
	    int bufferLoadShift=round_i((currentTimeInSec-currentTimePerBuffer(indices(indx)))/sampleIntervalInSec);
	    {
	      for(int data_cnt=0;data_cnt<bufferLoadShift;data_cnt++)
		filterBuffer(ant_cnt).shift_right(inputBuffer(ant_cnt)(data_cnt));
	    }
	  }
	  else
	  {
	    int bufferLoadShift=round_i((currentTimeInSec-endTimePerBuffer(indices(indx)))/sampleIntervalInSec);
	    // Here input buffer is yet to be updated, so belongs to N-1 subframe
	    for(int data_cnt=0;data_cnt<filterBuffer(ant_cnt).length();data_cnt++)
	      filterBuffer(ant_cnt)(data_cnt)=inputBuffer(ant_cnt)(inputBuffer(ant_cnt).length()-1-data_cnt);
	    if(bufferLoadShift)
	    {
	      if(filterBuffer(ant_cnt).length()<=bufferLoadShift)
		filterBuffer(ant_cnt).clear();
	      else
		filterBuffer(ant_cnt).shift_right(complex< double >(0,0),bufferLoadShift);
	    }
	  }
	}
      }
      setInArray(inputBuffer,indices,input);
      setInVec(currentTimePerBuffer,indices,currentTimeInSec);
    }
    else if(overwriteOrAdd==2) // add
    {
      if(isTimeDomain)
      {
	for(int indx=0;indx<indices.length();indx++)
	{
	  int ant_cnt=indices(indx);
	  endTimePerBuffer(ant_cnt)=currentTimePerBuffer(ant_cnt)+inputBuffer(ant_cnt).length()*sampleIntervalInSec;
	  if((endTimePerBuffer(ant_cnt)-currentTimeInSec)>1e-10)
	  {
// 	    cout<<"Adding up data in channel.."<<endl;
	    
	    double endTimeForLoad=currentTimeInSec+input(indx).length()*sampleIntervalInSec;
	    
	    // This procedure won't affect filter buffer as it is expired data
	    if(endTimeForLoad>endTimePerBuffer(ant_cnt)) // Input is contained in the inputBuffer; 
	    {
	      int zerosToPad=round_i((endTimeForLoad-endTimePerBuffer(ant_cnt))/sampleIntervalInSec);
		append(inputBuffer(ant_cnt),zeros_c(zerosToPad)); 
	    }
	    
	    int bufferLoadShift=round_i((currentTimeInSec-currentTimePerBuffer(ant_cnt))/sampleIntervalInSec);
	    for(int data_cnt=bufferLoadShift;data_cnt<bufferLoadShift+input(indx).length();data_cnt++)
	      inputBuffer(ant_cnt)(data_cnt)+=input(indx)(data_cnt-bufferLoadShift);
	  
	  }
	  else
	  {
	    int bufferLoadShift=round_i((currentTimeInSec-endTimePerBuffer(ant_cnt))/sampleIntervalInSec);
	    // Here input buffer is yet to be updated, so belongs to N-1 subframe
	    for(int data_cnt=0;data_cnt<filterBuffer(ant_cnt).length();data_cnt++)
	      filterBuffer(ant_cnt)(data_cnt)=inputBuffer(ant_cnt)(inputBuffer(ant_cnt).length()-1-data_cnt);
	    if(bufferLoadShift)
	    {
	      if(filterBuffer(ant_cnt).length()<=bufferLoadShift)
		filterBuffer(ant_cnt).clear();
	      else
		filterBuffer(ant_cnt).shift_right(complex< double >(0,0),bufferLoadShift);
	    }
	    inputBuffer(ant_cnt)=input(indx);
	    currentTimePerBuffer(ant_cnt)=currentTimeInSec;
	  }
	}
	
      }
    }
    else
    {
      cout<<"[detl:]Warning. Trying to overwrite data in ChannelFilter."<<endl;
      abort();
    }
  }
  else
  {
    setInArray(inputBuffer,indices,input);
    setInVec(currentTimePerBuffer,indices,currentTimeInSec);
    if(isTimeDomain)
    {
      setInVec(endTimePerBuffer,indices,currentTimeInSec);
      for(int indx=0;indx<indices.length();indx++)
      {
	int ant_cnt=indices(indx);
	filterBuffer(ant_cnt)=zeros_c(filterBuffer(ant_cnt).length());
      }
    }
  }
  
}

bool ChannelFilter::isHavingData(double currentTimeInSec){
  
  bool output=false;
  if(inputBuffer.length()!=0)
  {
    if(isTimeDomain)
    {
      for(int ant_cnt=0;ant_cnt<inputBuffer.length();ant_cnt++)
      {
	double endTime=currentTimePerBuffer(ant_cnt)+inputBuffer(ant_cnt).length()*sampleIntervalInSec;
	if((abs(currentTimeInSec-currentTimePerBuffer(ant_cnt))<1e-10 || (currentTimeInSec-currentTimePerBuffer(ant_cnt))>1e-10) && (endTime-currentTimeInSec)>1e-10) //double comparisions may fail !!
	  output=true;
      }
    }
    else
    {
      for(int ant_cnt=0;ant_cnt<inputBuffer.length();ant_cnt++)
      {
	if(abs(currentTimeInSec-currentTimePerBuffer(ant_cnt))<1e-10) // comparing only current time => partial interference not supported
	  output=true;
      }
    }
  }
  return output;
}

cvec ChannelFilter::getInputData(int currentAntenna,double startTimeInSec, int ofdmSymbolIndx,int dataSize){

  if(isTimeDomain)
  {
    int bufferLoadShift=round_i((startTimeInSec-currentTimePerBuffer(currentAntenna))/sampleIntervalInSec);
    if(bufferLoadShift>inputBuffer(currentAntenna).length())
      {cout<<"[both:]Trying to access data for future time in getInputData()."<<endl;abort();}

    cvec output(inputBuffer(currentAntenna).length()-bufferLoadShift);
    for(int cnt=bufferLoadShift;cnt<inputBuffer(currentAntenna).length();cnt++)
      output(cnt-bufferLoadShift)=inputBuffer(currentAntenna)(cnt);
    return(output);
  }
  else
    return(inputBuffer(currentAntenna).get(ofdmSymbolIndx*dataSize,(ofdmSymbolIndx+1)*dataSize-1));
}

bool ChannelFilter::isLoaded(){

  if(inputBuffer.length()) // need to change this
    return(true);
  else
    return(false);
}

// Returns true when the buffer associated to the antenna is loaded with data. 
bool ChannelFilter::isLoaded(int Antenna){
    
    if(inputBuffer(Antenna).length()) // need to change this
        return(true);
    else
        return(false);
}

void ChannelFilter::clearBuffer(){

  for(int cnt=0;cnt<inputBuffer.length();cnt++)
    inputBuffer(cnt).set_size(0);
}

void ChannelFilter::clearBuffer(ivec indices){
    
    for(int cnt=0;cnt<indices.length();cnt++)
        inputBuffer(indices(cnt)).set_size(0);
}

cvec ChannelFilter::applyTimeDomainChannel(cvec &channel,int currentAntenna, double currentTimeInSec, double endTimeInSeconds, double samplingFreqToUse){
  
    if( samplingFreqToUse==-1 || samplingFreqToUse==1.0/sampleIntervalInSec)
    {
      if(filterBuffer(currentAntenna).length()<channel.length())
      {
// 	cout<<"Channel Length : "<<channel.length()<<endl;
// 	cout<<"Buffer Length : "<<filterBuffer(currentAntenna).length()<<endl;
 	cout<<"[both:] filterBuffer length is less than channel length...Resizing the buffer..."<<endl;//abort();
          filterBuffer(currentAntenna).set_length(channel.length(),true); //FIXME temporary fix  
      }
      int endPoint;
      if(endTimeInSeconds!=-1)
      {
	endPoint=round_i((endTimeInSeconds-currentTimePerBuffer(currentAntenna))/sampleIntervalInSec);
	if(endPoint>inputBuffer(currentAntenna).length())
	{
	  cout<<"endPoint > inputBuffer(currentAntenna).length() : "<<endPoint<<" , "<<inputBuffer(currentAntenna).length()<<endl;
	  cout<<"[both:]Error. End time exceeding loaded data timing."<<endl;abort();
	}
      }
      else
	endPoint=inputBuffer(currentAntenna).length();
      
      cvec instantFilterBuffer=filterBuffer(currentAntenna)(0,channel.length()-1);
      int bufferLoadShift=round_i((currentTimeInSec-currentTimePerBuffer(currentAntenna))/sampleIntervalInSec);
      if(bufferLoadShift>inputBuffer(currentAntenna).length())
      {cout<<"[both:]Error. Trying to receive data for future time."<<endl;abort();}
      int cnt=0;
      for(;cnt<bufferLoadShift;cnt++)
	instantFilterBuffer.shift_right(inputBuffer(currentAntenna)(cnt));
      
      cvec output(endPoint-bufferLoadShift);
      for(;cnt<endPoint;cnt++)
      {
	instantFilterBuffer.shift_right(inputBuffer(currentAntenna)(cnt));
	output(cnt-bufferLoadShift)=sum(elem_mult(instantFilterBuffer,channel));
      }
      return(output);
    }
    else
    {
      double filterBufferStartTime=currentTimePerBuffer(currentAntenna)-filterBuffer(currentAntenna).length()*sampleIntervalInSec;
      cvec tempFilterBuffer=linearRateMatch(filterBuffer(currentAntenna),filterBufferStartTime,samplingFreqToUse);
      cvec tempInputBuffer=linearRateMatch(inputBuffer(currentAntenna),currentTimePerBuffer(currentAntenna),samplingFreqToUse);
      
      
      if(tempFilterBuffer.length()<channel.length())
      {
	cout<<"[both:]Insufficient length of filterBuffer length for the channel length."<<endl;abort();
      }
      int endPoint;
      if(endTimeInSeconds!=-1)
      {
	endPoint=round_i((endTimeInSeconds-currentTimePerBuffer(currentAntenna))*samplingFreqToUse);
	if(endPoint>tempInputBuffer.length())
	{
	  cout<<"[both:]Error. End time exceeding loaded data timing."<<endl;abort();
	}
      }
      else
	endPoint=tempInputBuffer.length();
      
      cvec instantFilterBuffer=tempFilterBuffer(0,channel.length()-1);
      int bufferLoadShift=round_i((currentTimeInSec-currentTimePerBuffer(currentAntenna))*samplingFreqToUse);
      if(bufferLoadShift>tempInputBuffer.length())
      {cout<<"[both:]Error. Trying to receive data for future time."<<endl;abort();}
      int cnt=0;
      for(;cnt<bufferLoadShift;cnt++)
	instantFilterBuffer.shift_right(tempInputBuffer(cnt));
      
      cvec output(endPoint-bufferLoadShift);
      for(;cnt<endPoint;cnt++)
      {
	instantFilterBuffer.shift_right(tempInputBuffer(cnt));
	output(cnt-bufferLoadShift)=sum(elem_mult(instantFilterBuffer,channel));
      }
      return(output);
    }
}

cvec ChannelFilter::applyFrequencyDomainChannel(cvec &channel,int currentAntenna, int ofdmSymbolIndx){

     return(elem_mult(channel,inputBuffer(currentAntenna).get(ofdmSymbolIndx*channel.length(),(ofdmSymbolIndx+1)*channel.length()-1)));
}

cvec ChannelFilter::applyChannel(cvec &channel,int currentAntenna, int ofdmSymbolIndx, double currentTimeInSec, double endTimeInSeconds, double samplingFreqToUse){
  
  if(isTimeDomain)
    return(applyTimeDomainChannel(channel,currentAntenna,currentTimeInSec,endTimeInSeconds,samplingFreqToUse));
  else
    return(applyFrequencyDomainChannel(channel,currentAntenna,ofdmSymbolIndx));
  
}

double  ChannelFilter::getCurrentTime(int currentAntenna){

  return(currentTimePerBuffer(currentAntenna));
}

cvec ChannelFilter::linearRateMatch(cvec input,double currentTime, double desiredSamplingFreq){
  
  int outputLength=floor_i((double)input.length()*sampleIntervalInSec*desiredSamplingFreq);
  cvec output(outputLength);
  
  double newTime=(double)(ceil_i(currentTime*desiredSamplingFreq))/desiredSamplingFreq;
  for(int cnt=0;cnt<outputLength;cnt++)
  {
    double prevSampleTime=(double)(floor_i(currentTime/sampleIntervalInSec))*sampleIntervalInSec;
    double nextSampleTime=(double)(ceil_i(currentTime/sampleIntervalInSec))*sampleIntervalInSec;
    
    int prevSampleIndx=(prevSampleTime-currentTime)/sampleIntervalInSec;
    int nextSampleIndx=(nextSampleTime-currentTime)/sampleIntervalInSec;
    
    double alpha=(newTime-prevSampleTime)/sampleIntervalInSec;
    
    output(cnt)=alpha*input(prevSampleIndx)+(1.0-alpha)*input(nextSampleIndx);
    
    newTime+=1/desiredSamplingFreq;
  }
  return output;

}

vec getPerClusterParameters(FadingChannelModel_E channelModel)
{
    vec param; 
    switch(channelModel)
    {
        case _FADING_CHANNEL_MODEL_CDL_A_:
            param = "5 11 3 3 10";
            break;
        case _FADING_CHANNEL_MODEL_CDL_B_:
            param = "10 22 3 7 8";
            break;
        case _FADING_CHANNEL_MODEL_CDL_C_:
            param = "2 15 3 7 7";
            break;
        case _FADING_CHANNEL_MODEL_CDL_D_:
            param = "5 8 3 3 11";
            break;
        case _FADING_CHANNEL_MODEL_CDL_E_:
            param = "5 11 3 7 8";
            break;
        default:
            cout<<"Unsupported Fading Model.."<<endl;abort();
            break;
    }
    return param;
}

mat getChannelParameters(FadingChannelModel_E channelModel)
{
    mat channelParam;
    switch(channelModel)
    {
        case _FADING_CHANNEL_MODEL_CDL_A_:
            channelParam =  "0	0.3819	0.4025	0.5868	0.461	0.5375	0.6708	0.575	0.7618	1.5375	1.8978	2.2242	2.1718	2.4942	2.5119	3.0582	4.081	4.4579	4.5695	4.7966	5.0066	5.3043	9.6586;"
                            "-13.4	0	-2.2	-4	-6	-8.2	-9.9	-10.5	-7.5	-15.9	-6.6	-16.7	-12.4	-15.2	-10.8	-11.3	-12.7	-16.2	-18.3	-18.9	-16.6	-19.9	-29.7;"
                            "-178.1	-4.2	-4.2	-4.2	90.2	90.2	90.2	121.5	-81.7	158.4	-83	134.8	-153	-172	-129.9	-136	165.4	148.4	132.7	-118.6	-154.1	126.5	-56.2;"
                            "51.3	-152.7	-152.7	-152.7	76.6	76.6	76.6	-1.8	-41.9	94.2	51.9	-115.9	26.6	76.6	-7	-23	-47.2	110.4	144.5	155.3	102	-151.8	55.2;"
                            "50.2	93.2	93.2	93.2	122	122	122	150.2	55.2	26.4	126.4	171.6	151.4	157.2	47.2	40.4	43.3	161.8	10.8	16.7	171.7	22.7	144.9;"
                            "125.4	91.3	91.3	91.3	94	94	94	47.1	56	30.1	58.8	26	49.2	143.1	117.4	122.7	123.2	32.6	27.2	15.2	146	150.7	156.1";
            break;
        case _FADING_CHANNEL_MODEL_CDL_B_:
            channelParam =  "0	0.1072	0.2155	0.2095	0.287	0.2986	0.3752	0.5055	0.3681	0.3697	0.57	0.5283	1.1021	1.2756	1.5474	1.7842	2.0169	2.8294	3.0219	3.6187	4.1067	4.279	4.7834;"
                            "0	-2.2	-4	-3.2	-9.8	-1.2	-3.4	-5.2	-7.6	-3	-8.9	-9	-4.8	-5.7	-7.5	-1.9	-7.6	-12.2	-9.8	-11.4	-14.9	-9.2	-11.3;"
                            "9.3	9.3	9.3	-34.1	-65.4	-11.4	-11.4	-11.4	-67.2	52.5	-72	74.3	-52.2	-50.5	61.4	30.6	-72.5	-90.6	-77.6	-82.6	-103.6	75.6	-77.6;"
                            "-173.3	-173.3	-173.3	125.5	-88	155.1	155.1	155.1	-89.8	132.1	-83.6	95.3	103.7	-87.8	-92.5	-139.1	-90.6	58.6	-79	65.8	52.7	88.7	-60.4;"
                            "105.8	105.8	105.8	115.3	119.3	103.2	103.2	103.2	118.2	102	100.4	98.3	103.4	102.5	101.4	103	100	115.2	100.5	119.6	118.7	117.8	115.7;"
                            "78.9	78.9	78.9	63.3	59.9	67.5	67.5	67.5	82.6	66.3	61.6	58	78.2	82	62.4	78	60.9	82.9	60.8	57.3	59.9	60.1	62.3";
            break;
        case _FADING_CHANNEL_MODEL_CDL_C_:
            channelParam =  "0	0.2099	0.2219	0.2329	0.2176	0.6366	0.6448	0.656	0.6584	0.7935	0.8213	0.9336	1.2285	1.3083	2.1704	2.7105	4.2589	4.6003	5.4902	5.6077	6.3065	6.6374	7.0427	8.6523;"
                            "-4.4	-1.2	-3.5	-5.2	-2.5	0	-2.2	-3.9	-7.4	-7.1	-10.7	-11.1	-5.1	-6.8	-8.7	-13.2	-13.9	-13.9	-15.8	-17.1	-16	-15.7	-21.6	-22.8;"
                            "-46.6	-22.8	-22.8	-22.8	-40.7	0.3	0.3	0.3	73.1	-64.5	80.2	-97.1	-55.3	-64.3	-78.5	102.7	99.2	88.8	-101.9	92.2	93.3	106.6	119.5	-123.8;"
                            "-101	120	120	120	-127.5	170.4	170.4	170.4	55.4	66.5	-48.1	46.9	68.1	-68.7	81.5	30.7	-16.4	3.8	-13.7	9.7	5.6	0.7	-21.9	33.6;"
                            "97.2	98.6	98.6	98.6	100.6	99.2	99.2	99.2	105.2	95.3	106.1	93.5	103.7	104.2	93	104.2	94.9	93.1	92.2	106.7	93	92.9	105.2	107.8;"
                            "87.6	72.1	72.1	72.1	70.1	75.3	75.3	75.3	67.4	63.8	71.4	60.5	90.6	60.1	61	100.7	62.3	66.7	52.9	61.8	51.9	61.7	58	57";
            break;
        case _FADING_CHANNEL_MODEL_CDL_D_:
            channelParam =  "0	0	0.035	0.612	1.363	1.405	1.804	2.596	1.775	4.042	7.937	9.424	9.708	12.525;"
                            "-0.2	-13.5	-18.8	-21	-22.8	-17.9	-20.1	-21.9	-22.9	-27.8	-23.6	-24.8	-30	-27.7;"
                            "0	0	89.2	89.2	89.2	13	13	13	34.6	-64.5	-32.9	52.6	-132.1	77.2;"
                            "-180	-180	89.2	89.2	89.2	163	163	163	-137	74.5	127.7	-119.6	-9.1	-83.8;"
                            "98.5	98.5	85.5	85.5	85.5	97.5	97.5	97.5	98.5	88.4	91.3	103.8	80.3	86.5;"
                            "81.5	81.5	86.9	86.9	86.9	79.4	79.4	79.4	78.2	73.6	78.3	87	70.6	72.9";
            break;
        case _FADING_CHANNEL_MODEL_CDL_E_:
            channelParam =  "0	0	0.5133	0.544	0.563	0.544	0.7112	1.9092	1.9293	1.9589	2.6426	3.7136	5.4524	12.0034	20.6419;"
                            "-0.03	-22.03	-15.8	-18.1	-19.8	-22.9	-22.4	-18.6	-20.8	-22.6	-22.3	-25.6	-20.2	-29.8	-29.2;"
                            "0	0	57.5	57.5	57.5	-20.1	16.2	9.3	9.3	9.3	19	32.7	0.5	55.9	57.6;"
                            "-180	-180	18.2	18.2	18.2	101.8	112.9	-155.5	-155.5	-155.5	-143.3	-94.7	147	-36.2	-26;"
                            "99.6	99.6	104.2	104.2	104.2	99.4	100.8	98.8	98.8	98.8	100.8	96.4	98.9	95.6	104.6;"
                            "80.4	80.4	80.4	80.4	80.4	80.8	86.3	82.7	82.7	82.7	82.9	88	81	88.6	78.3";
            break;
        case _FADING_CHANNEL_MODEL_TDL_A_:
            channelParam =  "0 0.3819 0.4025 0.5868 0.461 0.5375 0.6708 0.575 0.7618 1.5375 1.8978 2.2242 2.1718 2.4942 2.5119 3.0582 4.081 4.4579 4.5695 4.7966 5.0066 5.3043 9.6586;"
                            "-13.4 0 -2.2 -4 -6 -8.2 -9.9 -10.5 -7.5 -15.9 -6.6 -16.7 -12.4 -15.2 -10.8 -11.3 -12.7 -16.2 -18.3 -18.9 -16.6 -19.9 -29.7";
            break;
        case _FADING_CHANNEL_MODEL_TDL_B_:
            channelParam =  "0 0.1072 0.2155 0.2095 0.287 0.2986 0.3752 0.5055 0.3681 0.3697 0.57 0.5283 1.1021 1.2756 1.5474 1.7842 2.0169 2.8294 3.0219 3.6187 4.1067 4.279 4.7834;"
                            "0 -2.2 -4 -3.2 -9.8 -1.2 -3.4 -5.2 -7.6 -3 -8.9 -9 -4.8 -5.7 -7.5 -1.9 -7.6 -12.2 -9.8 -11.4 -14.9 -9.2 -11.3";
            break;
        case _FADING_CHANNEL_MODEL_TDL_C_:
            channelParam =  "0 0.2099 0.2219 0.2329 0.2176 0.6366 0.6448 0.656 0.6584 0.7935 0.8213 0.9336 1.2285 1.3083 2.1704 2.7105 4.2589 4.6003 5.4902 5.6077 6.3065 6.6374 7.0427 8.6523;"
                            "-4.4 -1.2 -3.5 -5.2 -2.5 0 -2.2 -3.9 -7.4 -7.1 -10.7 -11.1 -5.1 -6.8 -8.7 -13.2 -13.9 -13.9 -15.8 -17.1 -16 -15.7 -21.6 -22.8";
            break;
        case _FADING_CHANNEL_MODEL_TDL_D_:
            channelParam =  "0 0 0.035 0.612 1.363 1.405 1.804 2.596 1.775 4.042 7.937 9.424 9.708 12.525;"
                            "-0.2 -13.5 -18.8 -21 -22.8 -17.9 -20.1 -21.9 -22.9 -27.8 -23.6 -24.8 -30 -27.7";
            break;
        case _FADING_CHANNEL_MODEL_TDL_E_:
            channelParam =  "0 0 0.5133 0.544 0.563 0.544 0.7112 1.9092 1.9293 1.9589 2.6426 3.7136 5.4524 12.0034 20.6519;"
                            "-0.03 -22.03 -15.8 -18.1 -19.8 -22.9 -22.4 -18.6 -20.8 -22.6 -22.3 -25.6 -20.2 -29.8 -29.2";
            break;
        case _FADING_CHANNEL_MODEL_FLAT_FADING_:
        case _FADING_CHANNEL_MODEL_AWGN_:
            channelParam =  "0;"
                            "0";
        break;
        default:
            cout<<"Unsupported Fading Model..."<<endl;abort();
            break;
    }
    return channelParam;
}

cmat generateRotationMatrix(double Xpr,vec phase)
{
  // Refer to 2nd Matrix of Eqn 7.5-28 in TR 38.901.
    
    cmat Rot;
    Rot.set_size(2,2);
    
    for(int i=0,p=0;i<2;i++)
        for(int j=0;j<2;j++,p++)
        {
            double phi =  phase(p);
            Rot(i,j) = expj(phi);
        }
    Rot(0,1) *= sqrt(1/Xpr);
    Rot(1,0) *= sqrt(1/Xpr);
    
    return Rot;
}

bool isCDLModel(FadingChannelModel_E fadingModel)
{
    switch(fadingModel)
    {
        case _FADING_CHANNEL_MODEL_CDL_A_:
        case _FADING_CHANNEL_MODEL_CDL_B_:
        case _FADING_CHANNEL_MODEL_CDL_C_:
        case _FADING_CHANNEL_MODEL_CDL_D_:
        case _FADING_CHANNEL_MODEL_CDL_E_:
            return true;
            break;
        default:
            return false;
            break;
    }
    return false;
}

bool isTDLModel(FadingChannelModel_E fadingModel)
{
    switch(fadingModel)
    {
        case _FADING_CHANNEL_MODEL_TDL_A_:
        case _FADING_CHANNEL_MODEL_TDL_B_:
        case _FADING_CHANNEL_MODEL_TDL_C_:
        case _FADING_CHANNEL_MODEL_TDL_D_:
        case _FADING_CHANNEL_MODEL_TDL_E_:
            return true;
            break;
        default:
            return false;
            break;
    }
    return false;
}

cvec LLSChannel::generateTDLChannelTaps(int nTaps,int nSinusoids,double velocity,double currentTimeInSeconds,double carrierFreq,unsigned int rnd_state)
{
    double doppler=velocity*carrierFreq/lightSpeed;
    double wd=2*pi*doppler*currentTimeInSeconds;
    double theta,psin,alphan,phi;
    
    cvec tapWeight = zeros_c(nTaps);
    for(int i=0;i<nTaps;i++)
    {
        theta = (rnd_gen(rnd_state)-0.5)*2*pi;
        phi = (rnd_gen(rnd_state)-0.5)*2*pi;
        for(int j=0;j<nSinusoids;j++)
        {
            psin=(rnd_gen(rnd_state)-0.5)*2*pi;
            alphan = (2*pi*j-pi+theta)/((double)(4*nSinusoids));
            tapWeight(i) += (expj(psin)*cos(wd*cos(alphan)+phi));
        }
        tapWeight(i) *= sqrt(2.0/(double)nSinusoids); 
    }
    return tapWeight;
}

void LLSChannel::initChannel(FadingChannelModel_E chanModel,double carrierFrequency,double delaySpread,mat velocity,Antenna* txAntennaPtr,Antenna* rxAntennaPtr)
{
    channelModel = chanModel;
    velocityVector = getCartesianVector(velocity(1,0),velocity(2,0),0,velocity(0,0));
    carrierFrequencyInHz = carrierFrequency;
    if(chanModel==_FADING_CHANNEL_MODEL_TDL_D_ || chanModel == _FADING_CHANNEL_MODEL_TDL_E_ || chanModel == _FADING_CHANNEL_MODEL_CDL_D_ || chanModel == _FADING_CHANNEL_MODEL_CDL_E_)
    {
        isLOS = true;
    }
    else
        isLOS = false;
    
    if(isCDLModel(chanModel))
    {
        calculateLocationVectorsAndPolAngles(txAntennaPtr,carrierFrequencyInHz,txLocationVectorPerAntennaElement,txPolarizationAngleInDegreePerAntennaElement);
        calculateLocationVectorsAndPolAngles(rxAntennaPtr,carrierFrequencyInHz,rxLocationVectorPerAntennaElement,rxPolarizationAngleInDegreePerAntennaElement);
        
        txBeamParameters = txAntennaPtr->getBeamParameters(carrierFrequency),rxBeamParameters = rxAntennaPtr->getBeamParameters(carrierFrequency);
    }
    reInitChannel(txAntennaPtr,rxAntennaPtr,delaySpread);
}

void LLSChannel::reInitChannel(Antenna* txAntennaPtr,Antenna* rxAntennaPtr,double delaySpread)
{
    if(!isCDLModel(channelModel))
    {
        mat channelParam = getChannelParameters(channelModel);
        losTapPow = 0.0;
        
        nClusters = channelParam.cols();
        clusterDelay = channelParam.get_row(0)*delaySpread;
        clusterPow = inv_dB(channelParam.get_row(1));
        clusterPow /= sum(clusterPow); 
        
        if(isLOS)
        {
            losTapPow = clusterPow(0);
            clusterPow.del(0);
            clusterDelay.del(0);
            nClusters--;
        }
        rndSeed.set_length(rxAntennaPtr->numTxRUs);
        for(int rxAntCnt=0;rxAntCnt<rxAntennaPtr->numTxRUs;rxAntCnt++)
        {
            rndSeed(rxAntCnt).set_length(txAntennaPtr->numTxRUs);
            for(int txAntCnt=0;txAntCnt<txAntennaPtr->numTxRUs;txAntCnt++)
                rndSeed(rxAntCnt)(txAntCnt) = randi(1,99999);
        }
    }
    else
    {
        mat channelParam = getChannelParameters(channelModel);
        vec clusterParam = getPerClusterParameters(channelModel);
        vec clusterAOD,clusterAOA,clusterZOD,clusterZOA;
        vec subpathOffsets = "0.0447 -0.0447 0.1413 -0.1413 0.2492 -0.2492 0.3715 -0.3715 0.5129 -0.5129 0.6797 -0.6797 0.8844 -0.8844 1.1481 -1.1481 1.5195 -1.5195 2.1551 -2.1551";
        
        nClusters = channelParam.cols(),nRaysPerCluster=subpathOffsets.length();
        
        clusterDelay = channelParam.get_row(0)*delaySpread,clusterPow = inv_dB(channelParam.get_row(1));
        
        clusterPow /= sum(clusterPow);
        
        if(isLOS)
        {
            vec losParams = channelParam.get_col(0);
            losAOD = losParams(2),losAOA = losParams(3),losZOD = losParams(4),losZOA = losParams(5);
            losTapPow = clusterPow(0);
            
            losTxAntennaGain = sqrt(inv_dB(txAntennaPtr->getAntennaGain(losParams(2),losParams(4))));
            losRxAntennaGain = sqrt(inv_dB(rxAntennaPtr->getAntennaGain(losParams(3),losParams(5))));
            
            channelParam.del_col(0);
            clusterPow.del(0);clusterDelay.del(0);nClusters--;
        }
        
        clusterAOD = channelParam.get_row(2),clusterAOA = channelParam.get_row(3);
        clusterZOD = channelParam.get_row(4),clusterZOA = channelParam.get_row(5);
        
        double c_ASD = clusterParam(0),c_ASA = clusterParam(1), c_ZSD = clusterParam(2),c_ZSA = clusterParam(3);
        double XPR = clusterParam(4);
        
        txAntennaGainPerSubpath.set_length(nClusters);
        rxAntennaGainPerSubpath.set_length(nClusters);
        subpathPhase.set_length(nClusters);
        xprPerSubpath.set_length(nClusters);
        
        subpathAOD.set_length(nClusters);
        subpathZOD.set_length(nClusters);
        subpathAOA.set_length(nClusters);
        subpathZOA.set_length(nClusters);
        
        for(int clusterCnt=0;clusterCnt<nClusters;clusterCnt++)
        {
            subpathAOD(clusterCnt).set_length(nRaysPerCluster);
            subpathZOD(clusterCnt).set_length(nRaysPerCluster);
            subpathAOA(clusterCnt).set_length(nRaysPerCluster);
            subpathZOA(clusterCnt).set_length(nRaysPerCluster);
            
            txAntennaGainPerSubpath(clusterCnt).set_length(nRaysPerCluster);
            rxAntennaGainPerSubpath(clusterCnt).set_length(nRaysPerCluster);
            
            subpathPhase(clusterCnt).set_length(0);
            xprPerSubpath(clusterCnt).set_length(nRaysPerCluster);
            
            ivec idx = getIntegers(0,nRaysPerCluster-1);
            ivec zodAssociation = randPick(idx,nRaysPerCluster); //random Coupling AOA,ZOD,ZOA with AOD within cluster or subcluster.
            ivec aoaAssociation = randPick(idx,nRaysPerCluster);
            ivec zoaAssociation = randPick(idx,nRaysPerCluster);
            
            for (int pathCnt = 0; pathCnt<nRaysPerCluster; pathCnt++)
            {
                subpathAOD(clusterCnt)(pathCnt) = clusterAOD(clusterCnt) + c_ASD*subpathOffsets(idx(pathCnt));
                subpathZOD(clusterCnt)(pathCnt) = clusterZOD(clusterCnt) + c_ZSD*subpathOffsets(zodAssociation(pathCnt));

                //mapping ZOD to [0,180]
                subpathZOD(clusterCnt)(pathCnt) = circmod(subpathZOD(clusterCnt)(pathCnt),360.0);
                if(subpathZOD(clusterCnt)(pathCnt)>180)
                    subpathZOD(clusterCnt)(pathCnt) = 360-subpathZOD(clusterCnt)(pathCnt);
                
                subpathAOA(clusterCnt)(pathCnt) = clusterAOA(clusterCnt) + c_ASA*subpathOffsets(aoaAssociation(pathCnt));
                subpathZOA(clusterCnt)(pathCnt) = clusterZOA(clusterCnt) + c_ZSA*subpathOffsets(zoaAssociation(pathCnt));
                
                //mapping ZOA to [0,180]
                subpathZOA(clusterCnt)(pathCnt) = circmod(subpathZOA(clusterCnt)(pathCnt),360.0);
                if(subpathZOA(clusterCnt)(pathCnt)>180)
                    subpathZOA(clusterCnt)(pathCnt) = 360-subpathZOA(clusterCnt)(pathCnt);
                
                txAntennaGainPerSubpath(clusterCnt)(pathCnt) = sqrt(inv_dB(txAntennaPtr->getAntennaGain(subpathAOD(clusterCnt)(pathCnt),subpathZOD(clusterCnt)(pathCnt))));
                rxAntennaGainPerSubpath(clusterCnt)(pathCnt) = sqrt(inv_dB(rxAntennaPtr->getAntennaGain(subpathAOA(clusterCnt)(pathCnt),subpathZOA(clusterCnt)(pathCnt))));
                cout<<"txAntennaGainPerSubpath(clusterCnt)(pathCnt)"<<txAntennaGainPerSubpath(clusterCnt)(pathCnt)<<endl;
                cout<<"rxAntennaGainPerSubpath(clusterCnt)(pathCnt)"<<rxAntennaGainPerSubpath(clusterCnt)(pathCnt)<<endl;

                append(subpathPhase(clusterCnt),(2*randu(4)-1)*pi);
                xprPerSubpath(clusterCnt)(pathCnt) = inv_dB(XPR);
            }
        }
    }
    fadingParams.tapDel = clusterDelay;
    fadingParams.tapGain = sqrt(clusterPow);
    fadingParams.nTaps = clusterDelay.length();
    
}

dComplex LLSChannel::getLOSPathGain(int currentTxAntennaElement,int currentRxAntennaElement,double currentTimeInSeconds)
{
    cmat Rot = to_cmat(mat("1 0;0 -1"));
    mat r_tx = getCartesianVector(losAOD,losZOD);
    mat r_rx = getCartesianVector(losAOA,losZOA);
    
    double txFieldAmplitude = losTxAntennaGain, rxFieldAmplitude = losRxAntennaGain;
    
    double lamda=lightSpeed/carrierFrequencyInHz;
    mat txLocationVector = txLocationVectorPerAntennaElement(currentTxAntennaElement),rxLocationVector = rxLocationVectorPerAntennaElement(currentRxAntennaElement);
    double txPolAngle = txPolarizationAngleInDegreePerAntennaElement(currentTxAntennaElement),rxPolAngle = rxPolarizationAngleInDegreePerAntennaElement(currentRxAntennaElement);
    double K = 2*pi/lamda,deg2rad = pi/180;
    
    double c1,c2,c3;
    cmat F_tx(2,1),F_rx(2,1);
    
    //Refer to Eqn 7.5-28 in 3GPP TR 38.901
    
    F_tx(0,0) = txFieldAmplitude*cos(deg2rad*txPolAngle);
    F_tx(1,0) = txFieldAmplitude*sin(deg2rad*txPolAngle);
    
    F_rx(0,0) = rxFieldAmplitude*cos(deg2rad*rxPolAngle);
    F_rx(1,0) = rxFieldAmplitude*sin(deg2rad*rxPolAngle);
    
    c1 = K*(r_rx.T()*rxLocationVector)(0,0);
    c2 = K*(r_tx.T()*txLocationVector)(0,0);
    c3 = K*(r_rx.T()*velocityVector)(0,0)*currentTimeInSeconds;
    
    dComplex pathGain = sqrt(losTapPow)*(F_rx.T()*Rot*F_tx)(0,0)*expj(c1)*expj(c2)*expj(c3);
    return pathGain;
}

dComplex LLSChannel::getPathGain(int clusterCnt,int pathCnt,int currentTxAntennaElement,int currentRxAntennaElement,double currentTimeInSeconds)
{
    cmat Rot = generateRotationMatrix(xprPerSubpath(clusterCnt)(pathCnt),subpathPhase(clusterCnt)(4*pathCnt,4*pathCnt+3));
    mat r_tx = getCartesianVector(subpathAOD(clusterCnt)(pathCnt),subpathZOD(clusterCnt)(pathCnt));
    mat r_rx = getCartesianVector(subpathAOA(clusterCnt)(pathCnt),subpathZOA(clusterCnt)(pathCnt));
    
    double txFieldAmplitude = txAntennaGainPerSubpath(clusterCnt)(pathCnt),rxFieldAmplitude = rxAntennaGainPerSubpath(clusterCnt)(pathCnt);
    
    double lamda=lightSpeed/carrierFrequencyInHz;
    mat txLocationVector = txLocationVectorPerAntennaElement(currentTxAntennaElement),rxLocationVector = rxLocationVectorPerAntennaElement(currentRxAntennaElement);
    double txPolAngle = txPolarizationAngleInDegreePerAntennaElement(currentTxAntennaElement),rxPolAngle = rxPolarizationAngleInDegreePerAntennaElement(currentRxAntennaElement);
    double K = 2*pi/lamda,deg2rad = pi/180;
    
    double c1,c2,c3;
    cmat F_tx(2,1),F_rx(2,1);
    
    //Refer to Eqn 7.5-28 in 3GPP TR 38.901
    
    F_tx(0,0) = txFieldAmplitude*cos(deg2rad*txPolAngle);
    F_tx(1,0) = txFieldAmplitude*sin(deg2rad*txPolAngle);
    
    F_rx(0,0) = rxFieldAmplitude*cos(deg2rad*rxPolAngle);
    F_rx(1,0) = rxFieldAmplitude*sin(deg2rad*rxPolAngle);
    
    c1 = K*(r_rx.T()*rxLocationVector)(0,0);
    c2 = K*(r_tx.T()*txLocationVector)(0,0);
    c3 = K*(r_rx.T()*velocityVector)(0,0)*currentTimeInSeconds;
    
    dComplex pathGain = (sqrt(clusterPow(clusterCnt)/nRaysPerCluster))*(F_rx.T()*Rot*F_tx)(0,0)*expj(c1)*expj(c2)*expj(c3);
    return pathGain;
}

cvec LLSChannel::generateTapWeights(int txAntennaCount, int rxAntennaCount,double currentTimeInSeconds)
{
    cvec channel;
    if(!isCDLModel(channelModel))
    {
        int nSinusoids = 50;
        channel = generateTDLChannelTaps(nClusters,nSinusoids,trace(velocityVector.T()*velocityVector),currentTimeInSeconds,carrierFrequencyInHz,rndSeed(rxAntennaCount)(txAntennaCount));
        channel = elem_mult(channel,to_cvec(sqrt(clusterPow)));
        if(isLOS)
        {
            dComplex losTap = generateTDLChannelTaps(1,1,trace(velocityVector.T()*velocityVector),currentTimeInSeconds,carrierFrequencyInHz,rndSeed(rxAntennaCount)(txAntennaCount))(0);
            channel(0) += sqrt(losTapPow)*losTap;
        }
    }
    else
    {
        channel = zeros_c(nClusters);
        cvec rxBeamWeights = rxBeamParameters.getBeamformingVector(0);
        cvec txBeamWeights = txBeamParameters.getBeamformingVector(0);
        
        ivec rxElementIndices = rxBeamParameters.TXRUparameters.getTXRUAntennaElements(rxAntennaCount);
        for(int rxElemCnt=0;rxElemCnt<rxElementIndices.length();rxElemCnt++)
        {
            int currentRxAntennaElement = rxElementIndices(rxElemCnt);
            ivec txElementIndices = txBeamParameters.TXRUparameters.getTXRUAntennaElements(txAntennaCount);
            cvec tapWeights = zeros_c(nClusters);
            for(int txElemCnt=0;txElemCnt<txElementIndices.length();txElemCnt++)
            {
                int currentTxAntennaElement = txElementIndices(txElemCnt);
                cvec channelTaps = zeros_c(nClusters);
                for(int clusterCnt=0;clusterCnt<nClusters;clusterCnt++)
                {
                    for(int pathCnt=0;pathCnt<nRaysPerCluster;pathCnt++)
                    {
                        channelTaps(clusterCnt) += getPathGain(clusterCnt,pathCnt,currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds);
                    }
                    if(isLOS && clusterCnt==0)
                    {
                        channelTaps(0) += getLOSPathGain(currentTxAntennaElement,currentRxAntennaElement,currentTimeInSeconds);
                    }
                }
                tapWeights += txBeamWeights(txElemCnt)*channelTaps; 
            }
            channel += rxBeamWeights(rxElemCnt)*tapWeights;
        }
    }
    dComplex channelGain = sum_sqr(abs(channel));
    if(isnan(channelGain.real()))
    {
        cout<<"Channel Tap Weights cannot be NAN..."<<endl;abort();
    }
    return channel;
}

ChannelCloud::ChannelCloud(){

    #ifdef USING_SECURITY 
    BWSIM_Password_function();
    #endif
    
  isChannelInitialized=false;
  channelReciprocity=true;
  parameterStatus=false;
  timeDomain=-1;
  addMultiPathFading=1;
  addNoise=1;
  dataSize=-1;
  commonSamplingFrequency=-1;
  channelGenerationMethod=_CHANNEL_GENERATION_METHOD_UNINIT_;
  channelModelType=_MODEL_UNINIT_;
  sysWideChannelInfo.channelModel=_FADING_CHANNEL_MODEL_UNINIT_;
  sysWideChannelInfo.fadingType=_FADING_TYPE_UNINIT_;
  //sysWideChannelInfo.dopplerSpread=-1;
  linkSpecificChannelInfo.set_size(0);
  channelInfoIndx.set_size(0,0);
  channelFilterPerNode.set_size(0);
  customFadingParameters.set_size(0);
  customFadingParametersName.set_size(0);
  nodesInfo.nodeList.set_size(0);
  linkGainInDB.set_size(0,0);
  pathDelayInSec.set_size(0,0);

  velocityMatrix.set_size(0,0);
  scmLinks.set_size(0);
  addPathDelay=true;
  nodeSyncList.set_length(0);
}

void ChannelCloud::loadChannel(string fileName){

  parse(fileName,"timeDomain",timeDomain);
  if(timeDomain)
    cout<<"[detl:]Configuring to timeDomain Channel."<<endl;
  parse(fileName,"addMultiPathFading",addMultiPathFading);
  parse(fileName,"addNoise",addNoise);
  parse(fileName,"samplingFrequency",commonSamplingFrequency);
  parse(fileName,"enableFlatFading",enableFlatFading);
  if(!parse(fileName,"addOxygenLoss",addOxygenLoss)){addOxygenLoss=false;}
  if(!parse(fileName,"applyFaradayRotation",applyFaradayRotation)){applyFaradayRotation = false;}
  if(!parse(fileName,"useGroundReflection",useGroundReflection)){useGroundReflection=false;}

  channelGenerationMethod = parseChannelGenerationMethod(fileName);
  channelModelType = parseChannelModelType(fileName);
  string fadType="";
  parse(fileName,"fadingType",fadType);
  if(fadType!="")
  {
    if(fadType=="_FADING_TYPE_CORRELATED_")
      sysWideChannelInfo.fadingType=_FADING_TYPE_CORRELATED_;
    else if(fadType=="_FADING_TYPE_UNCORRELATED_")
      sysWideChannelInfo.fadingType=_FADING_TYPE_UNCORRELATED_;
    else if(fadType=="_FADING_TYPE_AWG_")
      sysWideChannelInfo.fadingType=_FADING_TYPE_AWG_;
    else
    {
      cout<<"[both:]Error. Unknown Fading Type to load from config file."<<endl;
      abort();
    }
  }

  channelModelDecider.load(fileName);
  //double userVelocity,carrierFrequencyInHz;
  vec serviceNodeVelocity;
  if(!parse(fileName,"serviceNodeVelocity",serviceNodeVelocity)){

    cout<<"[both:]Error. Unable to parse serviceNodeVelocity from config file in loadChannel()."<<endl;abort();
  }
//   double carrierFreqInMHz;
//   if(!parse(fileName,"carrierFreqInMHz",carrierFreqInMHz)){
// 
//     cout<<"[both:]Error. Unable to parse carrierFreq from config file in loadChannel()."<<endl;abort();
//   }
//   carrierFrequencyInHz=carrierFreqInMHz*1e6;
//  sysWideChannelInfo.dopplerSpread=userVelocity/lightSpeed*carrierFrequencyInHz;

  if(sysWideChannelInfo.fadingType==_FADING_TYPE_CORRELATED_)
  {
    sysWideChannelInfo.channelModel=channelModelDecider.getChannelModel(serviceNodeVelocity(0)); // using first velocity to decide channelModel
    if(channelModelDecider.isTDLmodel || isCDLModel(sysWideChannelInfo.channelModel))
      sysWideChannelInfo.delaySpreadTDL=channelModelDecider.getDelaySpreadForTDL(serviceNodeVelocity(0));
  }
  if(currentTaskid==0)
    cout<<"[detl:]Loading system wide channel model as "<<sysWideChannelInfo.channelModel<<endl;

  if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
    sysWideChannelInfo.scmPropagationScenario=loadSCMScenario(fileName);
  
  if(isCDLModel(sysWideChannelInfo.channelModel) && channelGenerationMethod!=_CHANNEL_GENERATION_METHOD_CDL_)
  {
        cout<<"Only _CHANNEL_GENERATION_METHOD_CDL_ is supported for CDL Models. Resetting channelGenerationMethod to _CHANNEL_GENERATION_METHOD_CDL_.."<<endl;
        channelGenerationMethod = _CHANNEL_GENERATION_METHOD_CDL_;
  }
};

void ChannelCloud::loadCorrMatrix(AntennaCorrelationType_E antennaCorrelation){

    CorrMatrix.load(antennaCorrelation);

}

void ChannelCloud::initChannel(bool channelreciprocity,bool enableQuasiStatic,double quasiStaticDurationinSec){

  isChannelInitialized=true;
  channelReciprocity=channelreciprocity;
  loadDefault();
  if(isChannelReady()==false)
  {
	cout<<"[both:]Error. Channel not fully configured."<<endl;
	abort();
  }

  if(channelGenerationMethod!=_CHANNEL_GENERATION_METHOD_SCM_)
  {
    if(sysWideChannelInfo.channelModel!=_FADING_CHANNEL_MODEL_UNINIT_)
    {
      if(sysWideChannelInfo.channelModel!=_FADING_CHANNEL_MODEL_CUSTOM_)
	//sysWideChannelInfo.fadingParameters=generateFadingParameters(sysWideChannelInfo.channelModel,sysWideChannelInfo.dopplerSpread);
	sysWideChannelInfo.fadingParameters=generateFadingParameters(sysWideChannelInfo.channelModel,sysWideChannelInfo.delaySpreadTDL);
      else
	sysWideChannelInfo.fadingParameters=customFadingParameters(findInStringArray(customFadingParametersName,sysWideChannelInfo.customFadParametersName));
    }

    for(int chnInfo_cnt=0;chnInfo_cnt<linkSpecificChannelInfo.length();chnInfo_cnt++)
    {
      if(linkSpecificChannelInfo(chnInfo_cnt).channelModel!=_FADING_CHANNEL_MODEL_CUSTOM_)
	//linkSpecificChannelInfo(chnInfo_cnt).fadingParameters=generateFadingParameters(linkSpecificChannelInfo(chnInfo_cnt).channelModel,linkSpecificChannelInfo(chnInfo_cnt).dopplerSpread);
	linkSpecificChannelInfo(chnInfo_cnt).fadingParameters=generateFadingParameters(linkSpecificChannelInfo(chnInfo_cnt).channelModel,linkSpecificChannelInfo(chnInfo_cnt).delaySpreadTDL);
      else
	linkSpecificChannelInfo(chnInfo_cnt).fadingParameters=customFadingParameters(findInStringArray(customFadingParametersName,linkSpecificChannelInfo(chnInfo_cnt).customFadParametersName));
    }
  }

  rndSeed.set_size(nodesInfo.nodeList.length());  for(int d1=0;d1<nodesInfo.nodeList.length();d1++) rndSeed(d1).set_size(nodesInfo.nodeList.length());

  for(int row_cnt=0;row_cnt<rndSeed.length();row_cnt++)
    for(int column_cnt=0;column_cnt<rndSeed(row_cnt).length();column_cnt++)
    {
      (rndSeed(row_cnt)(column_cnt)).set_size(nodesInfo.antennaCount(row_cnt)*nodesInfo.antennaCount(column_cnt));
      (rndSeed(column_cnt)(row_cnt)).set_size(nodesInfo.antennaCount(row_cnt)*nodesInfo.antennaCount(column_cnt));
      for(int vec_cnt=0;vec_cnt<(rndSeed(row_cnt)(column_cnt)).length();vec_cnt++)
	(rndSeed(row_cnt)(column_cnt)).set(vec_cnt,rand());
      if(channelReciprocity)
	(rndSeed(column_cnt)(row_cnt))=(rndSeed(row_cnt)(column_cnt));
      else
	for(int vec_cnt=0;vec_cnt<(rndSeed(row_cnt)(column_cnt)).length();vec_cnt++)
	  (rndSeed(column_cnt)(row_cnt)).set(vec_cnt,rand());
    }

    
    
  if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
  {
    if(currentTaskid==0)
      cout<<"[detl:]Activating SCM Channel."<<endl;

    scmLinkIndx=zeros_i(nodesInfo.nodeList.length(),nodesInfo.nodeList.length())-1;
    scmReciprocityFlag=zeros_b(nodesInfo.nodeList.length(),nodesInfo.nodeList.length());
//     if(!channelReciprocity)
//     {cout<<"[detl:]Warning. Channel Reciprocity correction not done in SCM."<<endl;abort();}
  /*  
    if(timeDomain)
    {
      cout<<"[detl:]Warning. Channel Filter length hardcoded to 200 assuming the channel length will always be less than 200.."<<endl;
      abort();
    }*/
    
  }
  channelFilterPerNode.set_size(nodesInfo.nodeList.length());

  for(int node_cnt=0;node_cnt<channelFilterPerNode.length();node_cnt++)
  {
    if(nodesInfo.samplingFrequency.length())
    {
      if(timeDomain)
	channelFilterPerNode(node_cnt).init(nodesInfo.samplingFrequency(node_cnt),nodesInfo.antennaCount(node_cnt),true,200);
      else
	channelFilterPerNode(node_cnt).init(nodesInfo.samplingFrequency(node_cnt),nodesInfo.antennaCount(node_cnt));
    }
    else
    {
      if(timeDomain)
	channelFilterPerNode(node_cnt).init(commonSamplingFrequency,nodesInfo.antennaCount(node_cnt),true,200);
      else
	channelFilterPerNode(node_cnt).init(commonSamplingFrequency,nodesInfo.antennaCount(node_cnt));
    }
  }
  if(enableQuasiStatic)
  {
    quasiStaticChannelInfo.isEnabled=true;
    quasiStaticChannelInfo.isTimeDomain=timeDomain;
    quasiStaticChannelInfo.quasiStaticDurationPerRxNode.set_length(nodesInfo.nodeList.length());
    quasiStaticChannelInfo.quasiStaticDurationPerRxNode=quasiStaticDurationinSec; // Assuming same value for all nodes.. Need to change for different velocities...
    quasiStaticChannelInfo.channelGenTimePerLink.set_length(nodesInfo.nodeList.length());
    quasiStaticChannelInfo.oldChannelPerRxNode.set_length(nodesInfo.nodeList.length());
    quasiStaticChannelInfo.channelTapDelays.set_length(nodesInfo.nodeList.length());
    quasiStaticChannelInfo.samplingFreqToUse = commonSamplingFrequency;
    for(int node_cnt=0;node_cnt<nodesInfo.nodeList.length();node_cnt++)
    {
      quasiStaticChannelInfo.channelGenTimePerLink(node_cnt).set_length(nodesInfo.nodeList.length());
      quasiStaticChannelInfo.channelGenTimePerLink(node_cnt)=-1.0;
      quasiStaticChannelInfo.oldChannelPerRxNode(node_cnt).set_length(nodesInfo.nodeList.length());
      for(int tx_cnt=0;tx_cnt<nodesInfo.nodeList.length();tx_cnt++)
	quasiStaticChannelInfo.oldChannelPerRxNode(node_cnt)(tx_cnt).set_length(0);
      quasiStaticChannelInfo.channelTapDelays(node_cnt).set_length(nodesInfo.nodeList.length());
      for(int tx_cnt=0;tx_cnt<nodesInfo.nodeList.length();tx_cnt++)
        quasiStaticChannelInfo.channelTapDelays(node_cnt)(tx_cnt).set_length(0);
    }
  }
  
}

bool ChannelCloud::isChannelReady(){

  bool channelStatus=true;
  if(timeDomain==-1)
  {
	cout<<"[both:]Error. Channel Domain uninitialized. Use setChannelToTimeDomain() "
		  "for setting to Time Domain or call init() to load default value."<<endl;
	channelStatus=false;
  }
  if(dataSize==0)
  {
	cout<<"[both:]Error. Please transmit data from all Tx before initializing channel for receiving."<<endl;
	exit(0);
  }
  if(commonSamplingFrequency==-1)
  {
	cout<<"[both:]Error. Channel Sampling Frequency uninitialized. Use setSamplingFrequencyInHz()."<<endl;
	channelStatus=false;
  }
  if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_UNINIT_)
  {
	cout<<"[both:]Error. Channel Generation Method Uninitialized. "
		  "use setChannelGenerationMethod() or call init() to load default value."<<endl;
	channelStatus=false;
  }
  if(nodesInfo.nodeList.length()!=0)
  {
    if(linkSpecificChannelInfo.length()<(nodesInfo.nodeList.length()*nodesInfo.nodeList.length()) && sysWideChannelInfo.fadingType==_FADING_TYPE_UNINIT_)
	{
	  cout<<"[both:]Error. System  WideFading Uninitialized. "
			"use setSysWideFadingType() or call init() to load default value."<<endl;
	  channelStatus=false;
	}
	if(linkSpecificChannelInfo.length()<(nodesInfo.nodeList.length()*nodesInfo.nodeList.length()) && sysWideChannelInfo.channelModel==_FADING_CHANNEL_MODEL_UNINIT_)
	{
	  cout<<"[both:]Error. System Wide Channel Model Uninitialized. use setSysWideChannelModel() "
			"or call init() to load default value."<<endl;
	  channelStatus=false;
	}
	if(linkSpecificChannelInfo.length()<(nodesInfo.nodeList.length()*nodesInfo.nodeList.length()) && sysWideChannelInfo.channelModel==_FADING_CHANNEL_MODEL_UNINIT_)
	{
	  cout<<"[both:]Error. System Wide Channel Model Uninitialized. use setSysWideDopplerSpread() "
	  "or call init() to load default value."<<endl;
	  channelStatus=false;
	}
	if(find(nodesInfo.antennaCount,-1)!=-1)
	{
	  cout<<"[both:]Error. Antenna Count for all nodes not available in the channel."<<endl;
	  channelStatus=false;
	}
  }
  if(linkGainInDB.size()==0)
  {
	cout<<"[both:]Error. Link Budget Info not yet set in the channel."<<endl;
	channelStatus=false;
  }
  return(channelStatus);
}


bool ChannelCloud::isTimeDomain(){

  return(timeDomain);
}

void ChannelCloud::disableMultiPathChannel(){

  addMultiPathFading=false;
}

void ChannelCloud::enableMultiPathChannel(){

  addMultiPathFading=true;
}

void ChannelCloud::disableNoise(){

  addNoise=false;
}

void ChannelCloud::enableNoise(){

  addNoise=true;
}

void ChannelCloud::disablePathDelay(){

  addPathDelay=false;
}

void ChannelCloud::enablePathDelay(){

  addPathDelay=true;
}

void ChannelCloud::copyChannelSeeds(int fromNodeID, int toNodeID){

  if(!isChannelInitialized)
    cout<<"[detl:]Warning. channel is not yet initialized. Please Initialize channel before calling copyChannelSeeds()."<<endl;
  else
  {
   if(channelGenerationMethod== _CHANNEL_GENERATION_METHOD_JAKES_)
   {
      //bool isServerNode=true;
      int fromNodeIndx, toNodeIndx;
      fromNodeIndx=find(nodesInfo.nodeList,fromNodeID);
      toNodeIndx=find(nodesInfo.nodeList,toNodeID);
      if(fromNodeIndx==-1 || toNodeIndx==-1)
      {
	  cout<<"[detl:]Warning. Unknown nodeIDs or unmatched nodeIDs to copyChannelSeeds()."<<endl;
	  return;
      }

      if(nodesInfo.antennaCount(fromNodeIndx)!=nodesInfo.antennaCount(toNodeIndx))
      {
	cout<<"[detl:]Warning. Cannot copy channel seeds if the antenna counts are not matching."<<endl;
	return;
      }

       for(int cnt=0;cnt<rndSeed.length();cnt++) rndSeed(cnt)(toNodeIndx)=rndSeed(cnt)(fromNodeIndx);
       rndSeed(toNodeIndx)=rndSeed(fromNodeIndx);

   }
   else if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_MOD_JAKES_)
   {
     cout<<"[detl:]Warning. copyChannelSeeds() not ready for the selected channel generation method."<<endl;
     return;
   }
   else if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SMITH_)
   {
     cout<<"[detl:]Warning. copyChannelSeeds() not ready for the selected channel generation method."<<endl;
     return;
   }
   else //	if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
   {
     cout<<"[detl:]Warning. copyChannelSeeds() not ready for the selected channel generation method."<<endl;
     return;
   }
  }
}

double ChannelCloud::getNoiseVariancePerDimension(int rxNodeID){

  if(addNoise)
  {
    int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
    if(rxNodeIndx==-1)
    {
      cout<<"[both:]Error. Unknown rxNodeID to getNoiseVariancePerDimension()."<<endl;
      abort();
    }
    else
      return(nodesInfo.noiseVariancePerDimension(rxNodeIndx));

  }
  else
	return(0);
}

double ChannelCloud::getNodeCarrierFrequency(int rxNodeID){
  
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[both:]Error. Unknown rxNodeID to getNodeCarrierFrequency()."<<endl;
    abort();
  }
  else
    return(nodesInfo.carrierFrequency(rxNodeIndx));
    
  
}

double ChannelCloud::getNodeVelocity(int txNodeID,int rxNodeID){
  
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  int txNodeIndx=find(nodesInfo.nodeList,txNodeID);
  if(rxNodeIndx==-1 || txNodeIndx==-1)
  {
    cout<<"[both:]Error. Unknown nodeID to getNodeVelocity()."<<endl;
    abort();
  }
  else
    return(velocityMatrix(rxNodeIndx,txNodeIndx));
  
  
}

int ChannelCloud::getDataSize(){

  return(dataSize);
}

int ChannelCloud::getScmLinkIndx(int rxNodeID, int txNodeID)
{
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  int txNodeIndx=find(nodesInfo.nodeList,txNodeID);
  if(rxNodeIndx==-1 || txNodeIndx==-1)
  {
      cout<<"[both:]Error. Unknown nodeID to getScmLinkIndx()."<<endl;
    abort();
  }
  else
    return(scmLinkIndx(rxNodeIndx,txNodeIndx));
}
void ChannelCloud::setSCMLinkIndx(int rxNodeID, int txNodeID,int index)
{
    int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
    int txNodeIndx=find(nodesInfo.nodeList,txNodeID);
    if(rxNodeIndx==-1 || txNodeIndx==-1)
    {
        cout<<"[both:]Error. Unknown nodeID to getScmLinkIndx()."<<endl;
        abort();
    }
    scmLinkIndx(rxNodeIndx,txNodeIndx) = index;
}
double ChannelCloud::getCommonSamplingFrequencyInHz(){

  return(commonSamplingFrequency);
}

double ChannelCloud::getSimulationDurationInSeconds(){

  return(simulationDuration);
}

ChannelGenerationMethod_E ChannelCloud::getChannelGenerationMethod(){
  return(channelGenerationMethod);
}

FadingChannelModel_E ChannelCloud::getSysWideChannelModel(){

  return(sysWideChannelInfo.channelModel);
}

FadingType_E ChannelCloud::getSysWideFadingType(){

  return(sysWideChannelInfo.fadingType);
}

SCMPropagationScenario ChannelCloud::getSysWideSCMPropagationScenario(){

  return(sysWideChannelInfo.scmPropagationScenario);
}

// double ChannelCloud::getSysWideDopplerSpread(){
// 
//   return(sysWideChannelInfo.dopplerSpread);
// }

ivec ChannelCloud::getNodes(){

  return(nodesInfo.nodeList);
}

ivec ChannelCloud::getNodesInTxMode(){

  return(currentTxNodes);
}

ChannelInfo_S ChannelCloud::getLinkSpecificChannelInfo(int txNode, int rxNode){

  int txNodeIndx=find(nodesInfo.nodeList,txNode);
  int rxNodeIndx=find(nodesInfo.nodeList,rxNode);
  if(txNodeIndx==-1)
  {
	cout<<"[both:]Error. Given tx node not available in the list for getLinkSpecificChannelInfo()."<<endl;
	abort();
  }
  if(rxNodeIndx==-1)
  {
	cout<<"[both:]Error. Given rx node not available in the list for getLinkSpecificChannelInfo()."<<endl;
	abort();
  }
  int infoIndx=channelInfoIndx(rxNodeIndx,txNodeIndx);
  if(infoIndx==-1)
	return(sysWideChannelInfo);
  else
	return(linkSpecificChannelInfo(infoIndx));
}

SCMLink ChannelCloud::getSCMLink(int rxNode)
{
  return(scmLinks(rxNode));
}

double ChannelCloud::getLinkSpecificChannelGainInDB(int txNode, int rxNode,int txPanelID,int rxPanelID, bool isWithSCMSpecificAntGain,bool isLOS){
  //cout<<"check LOS"<<isLOS<<endl;
  int txNodeIndx=find(nodesInfo.nodeList,txNode);
  int rxNodeIndx=find(nodesInfo.nodeList,rxNode);
  if(txNodeIndx==-1 || rxNodeIndx==-1)
  {
	cout<<"[both:]Undefined Node ID to getLinkSpecificAttenuationFactor()."<<endl;
	exit(0);
  }
  if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
  {
    if(!isWithSCMSpecificAntGain)
        return(linkGainInDB(rxNodeIndx,txNodeIndx));
    else
    {
      if(scmLinkIndx(rxNodeIndx,txNodeIndx)!=-1)
      {
        int txOrientationID = nodesInfo.getOrientationID(txNode,txPanelID);
        int rxOrientationID = nodesInfo.getOrientationID(rxNode,rxPanelID);
        SCMLink* scmLink = &scmLinks(scmLinkIndx(rxNodeIndx,txNodeIndx));
        double SCMgain = (!isLOS) ? scmLink->getAvgAntennaGainInDB(txOrientationID,rxOrientationID) : scmLink->getLOSAntennaGainInDB(txOrientationID,rxOrientationID);
        return (getChannelGain(rxNode,txNode)+ SCMgain);
      }
    }
  }
  else
    return(linkGainInDB(rxNodeIndx,txNodeIndx));
  
  return(-500);
}

vec ChannelCloud::getLinkSpecificChannelGainInDB(ivec txNodes, int rxNode,ivec txPanelIDs,int rxPanelID, bool isWithSCMSpecificAntGain){


  vec channelGainInDB(txNodes.length());
  for(int txNode_cnt=0;txNode_cnt<txNodes.length();txNode_cnt++)
    channelGainInDB(txNode_cnt)=getLinkSpecificChannelGainInDB(txNodes(txNode_cnt),rxNode,txPanelIDs(txNode_cnt),rxPanelID,isWithSCMSpecificAntGain);
  return(channelGainInDB);

}

vec ChannelCloud::getLinkSpecificChannelGainInDB(int txNode, ivec rxNodes,int txPanelID,ivec rxPanelIDs, bool isWithSCMSpecificAntGain){

  vec channelGainInDB(rxNodes.length());
  if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
  {
    if(rxPanelIDs.length())
    {
        for(int rxNode_cnt=0;rxNode_cnt<rxNodes.length();rxNode_cnt++)
            channelGainInDB(rxNode_cnt)=getLinkSpecificChannelGainInDB(txNode,rxNodes(rxNode_cnt),txPanelID,rxPanelIDs(rxNode_cnt),isWithSCMSpecificAntGain);
    }
    else //FIXME Taking the panel with maximum gain, Actual txpanelID of unknown.
    {
        for(int rxNode_cnt=0;rxNode_cnt<rxNodes.length();rxNode_cnt++)
        {
            int nPanels = nodesInfo.getNumberOfPanels(rxNodes(rxNode_cnt));
            vec Gains(nPanels);
            for(int panel_cnt=0;panel_cnt<nPanels;panel_cnt++)
            {
                Gains(panel_cnt) = getLinkSpecificChannelGainInDB(txNode,rxNodes(rxNode_cnt),txPanelID,panel_cnt,isWithSCMSpecificAntGain);
            }
        }
    }
  }
  else
  {
      for(int rxNode_cnt=0;rxNode_cnt<rxNodes.length();rxNode_cnt++)
            channelGainInDB(rxNode_cnt)=getLinkSpecificChannelGainInDB(txNode,rxNodes(rxNode_cnt),0,0,isWithSCMSpecificAntGain);
  }
  return(channelGainInDB);
}

mat ChannelCloud::getLinkSpecificChannelGainInDB(ivec txNodes, ivec rxNodes, bool isWithSCMSpecificAntGain,ivec txPanelIDs,ivec rxPanelIDs){

  mat channelGainInDB;
  channelGainInDB.set_size(rxNodes.length(),txNodes.length());
  if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
  {
    if((txPanelIDs.length() && rxPanelIDs.length()) || channelGenerationMethod!=_CHANNEL_GENERATION_METHOD_SCM_)
    {
        for(int rxNode_cnt=0;rxNode_cnt<rxNodes.length();rxNode_cnt++)
            channelGainInDB.set_row(rxNode_cnt,getLinkSpecificChannelGainInDB(txNodes,rxNodes(rxNode_cnt),txPanelIDs,rxPanelIDs(rxNode_cnt),isWithSCMSpecificAntGain));
    }
    else //FIXME Taking the combination with maximum channel gain of all panelIDs for each node combination.
    {
        for(int rxNode_cnt=0;rxNode_cnt<rxNodes.length();rxNode_cnt++)
        {
            for(int txNode_cnt=0;txNode_cnt<txNodes.length();txNode_cnt++)
            {
                int nRxPanels = nodesInfo.getNumberOfPanels(rxNodes(rxNode_cnt));
                int nTxPanels = nodesInfo.getNumberOfOrientations(txNodes(txNode_cnt)); //BS follows orientation selection
                vec gainsPerCombination(nRxPanels*nTxPanels);
                for(int rxPanel_cnt=0,panelCnt=0;rxPanel_cnt<nRxPanels;rxPanel_cnt++)
                { 
                    for(int txPanel_cnt=0;txPanel_cnt<nTxPanels;txPanel_cnt++,panelCnt++)
                    {
                        gainsPerCombination(panelCnt) = getLinkSpecificChannelGainInDB(txNodes(txNode_cnt),rxNodes(rxNode_cnt),txPanel_cnt,rxPanel_cnt,isWithSCMSpecificAntGain);
                    }
                }
                channelGainInDB(rxNode_cnt,txNode_cnt) = max(gainsPerCombination);
            }
        }
    }  
  }
  else
  {
      for(int rxNode_cnt=0;rxNode_cnt<rxNodes.length();rxNode_cnt++)
            channelGainInDB.set_row(rxNode_cnt,getLinkSpecificChannelGainInDB(txNodes,rxNodes(rxNode_cnt),zeros_i(txNodes.length()),0,isWithSCMSpecificAntGain));
  }
  return(channelGainInDB);
}

FadingParameters_S ChannelCloud::getLinkSpecificFadingParameters(int txNode, int rxNode){

  return(getLinkSpecificChannelInfo(txNode,rxNode).fadingParameters);
}

void ChannelCloud::clearChannelBuffer(){

    for(int node_cnt=0;node_cnt<channelFilterPerNode.length();node_cnt++)
      channelFilterPerNode(node_cnt).clearBuffer();
}

void ChannelCloud::clearChannelBuffer(ivec txNodes, ivec txBeamIDs){
  
  ivec channelFilterIndices=find(nodesInfo.nodeList,txNodes);
  for(int indx_cnt=0;indx_cnt<channelFilterIndices.length();indx_cnt++)
  {
    if(txBeamIDs.length())
    {   
        ivec txRUs=nodesInfo.getTXRUs(txNodes(indx_cnt),txBeamIDs(indx_cnt));
        channelFilterPerNode(channelFilterIndices(indx_cnt)).clearBuffer(txRUs);
    }
    else
        channelFilterPerNode(channelFilterIndices(indx_cnt)).clearBuffer();
  }
}

void ChannelCloud::clearQuasistaticChannelInfo(){
  
  quasiStaticChannelInfo.clear();
}

ivec ChannelCloud::getTxNodesWithData(double currentTimeInSec){
  
  ivec txNodes=getNodesInTxMode();
  ivec txNodeIndx=find(nodesInfo.nodeList,txNodes);
  int length=txNodes.length();
  for(int cnt=length-1;cnt>=0;cnt--)
    if(!channelFilterPerNode(txNodeIndx(cnt)).isHavingData(currentTimeInSec))
      txNodes.del(cnt);
    
  return txNodes;  
}

void ChannelCloud::setDataSize(int size, bool overwrite){

  if(overwrite==true || dataSize==-1)
	dataSize=size;
  else
	cout<<"[detl:]Warning. Trying to overwrite dataSize in the channel."<<endl;
}

void ChannelCloud::setNoiseVariancePerDimension(int rxNodeID, double noiseVar){

  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[detl:]Unknown rxNodeID to setNoiseVariancePerDimension(). Skipping Updation."<<endl;
    return;
  }
  else
    nodesInfo.noiseVariancePerDimension(rxNodeIndx)=noiseVar;
}

void ChannelCloud::setNoiseVariancePerDimension(ivec rxNodeIDs, double noiseVar){

  ivec rxNodeIndx=find(nodesInfo.nodeList,rxNodeIDs);
  for(int cnt=0;cnt<rxNodeIndx.length();cnt++)
  {
    if(rxNodeIndx(cnt)==-1)
    {
      cout<<"[detl:]Unknown rxNodeID to setNoiseVariancePerDimension(). Skipping Updation."<<endl;
      return;
    }
    else
      nodesInfo.noiseVariancePerDimension(rxNodeIndx(cnt))=noiseVar;
  }
}

void ChannelCloud::setSamplingFrequencyInHz(double samplingFreq){

  commonSamplingFrequency=samplingFreq;
}

void ChannelCloud::setSamplingFrequencyInHz(ivec nodeIDs, double samplingFreq){
  
  if(nodesInfo.nodeList.length()!=0)
  {
    ivec nodeIndices=find(nodesInfo.nodeList,nodeIDs);
    if(find(nodeIndices,-1,true).length()==0)
    {
      if(commonSamplingFrequency!=-1)
      {
	if(nodesInfo.samplingFrequency.length()==0)
	  nodesInfo.samplingFrequency=ones(nodesInfo.nodeList.length())*commonSamplingFrequency;
	
	nodesInfo.samplingFrequency(nodeIndices)=ones(nodeIDs.length())*samplingFreq;
      }
      else
      {
	cout<<"Can't initialize samplingFreq as commonSamplingFrequency is not yet initialized."<<endl;
	abort();
      }
    }
    else
    {
      cout<<"Unknown nodeIDs to setSamplingFrequencyInHz()."<<endl;
      abort();
    }
      
  }
  else
  {
    cout<<"Can't initialize samplingFreq as NodesInfo is not yet initialized."<<endl;
    abort();
  }
}

void ChannelCloud::setSimulationDurationInSeconds(double simulationDur){

  simulationDuration=simulationDur;
}

void ChannelCloud::setChannelToFrequencyDomain(){

  timeDomain=false;
}

void ChannelCloud::setChannelToTimeDomain(){

  timeDomain=true;
}

void ChannelCloud::setCustomFadingParameters(string fadingParmsName, FadingParameters_S customFadingParams){

  if(findInStringArray(customFadingParametersName,fadingParmsName)!=-1)
  {
	cout<<"[both:]Error. 'fadingParmsName' already exists. Please specify different name in setCustomFadingParameters."<<endl;
	abort();
  }

  append(customFadingParametersName,fadingParmsName);
  append(customFadingParameters,customFadingParams);
}

void ChannelCloud::setChannelGenerationMethod(ChannelGenerationMethod_E method){

  channelGenerationMethod=method;
}

void ChannelCloud::setSysWideChannelInfo(FadingChannelModel_E sysChannelModel,FadingType_E fadingType,  string customFadParamsName){

  sysWideChannelInfo.channelModel=sysChannelModel;
  sysWideChannelInfo.fadingType=fadingType;
  if(customFadParamsName!="")
  if(findInStringArray(customFadingParametersName,customFadParamsName)==-1)
  {
	cout<<"[both:]Error. Undefined 'customFadingParametersName' to setSysWideChannelInfo()."<<endl;
	abort();
  }
  sysWideChannelInfo.customFadParametersName=customFadParamsName;
}

void ChannelCloud::setSysWideChannelInfo(SCMPropagationScenario scmScenario){

  sysWideChannelInfo.scmPropagationScenario=scmScenario;
}

void ChannelCloud::setSysLinkGainInfo(string filename){

  if(parse(filename,"nodeList",nodesInfo.nodeList)==false)
  {
    cout<<"[both:]Error. Unable to load nodeList from the file in setSysLinkGainInfo()."<<endl;
    abort();
  }

  vec nodeVelocity;
  if(parse(filename,"nodeVelocity",nodeVelocity)==false)
  {
    cout<<"[both:]Error. Unable to load nodeVelocity from the file in setSysLinkGainInfo()."<<endl;
    abort();
  }
  
  if(parse(filename,"linkGainInDB",linkGainInDB)==false)
  {
    cout<<"[both:]Error. Unable to load linkGainInDB from the file in setSysLinkGainInfo()."<<endl;
    abort();
  }

  if(linkGainInDB.rows()!=nodesInfo.nodeList.length())
  {cout<<"[both:]Error. Mismatch between number of nodes and linkBudget rows().";abort();}
  if(linkGainInDB.cols()!=nodesInfo.nodeList.length())
  {cout<<"[both:]Error. Mismatch between number of nodes and linkBudget cols().";abort();}

  currentTxNodes=nodesInfo.nodeList;

  if(parse(filename,"propagationDelayInSec",pathDelayInSec)==false)
  {
    cout<<"[detl:]Warning. Unable to load propagationDelayInSec from the file in setSysLinkGainInfo(). Using no delay."<<endl;
    pathDelayInSec=zeros(nodesInfo.nodeList.length(),nodesInfo.nodeList.length());
  }
  instantPathDelayInSec=pathDelayInSec;
  nodesInfo.antennaCount=zeros_i(nodesInfo.nodeList.length())-1;
  nodesInfo.beamParametersPerNode.set_length(nodesInfo.nodeList.length());
  nodesInfo.nodeType.set_length(nodesInfo.nodeList.length());
  nodesInfo.beamIDPerTxRU.set_length(nodesInfo.nodeList.length());
  nodesInfo.noiseVariancePerDimension=zeros(nodesInfo.nodeList.length())-1;
  nodesInfo.carrierFrequency=zeros(nodesInfo.nodeList.length())-1;

  nodeSyncList=zeros_i(nodesInfo.nodeList.length())-1;
  if(!isTimeDomain())
    clearChannelBuffer();
  channelInfoIndx=zeros_i(nodesInfo.nodeList.length(),nodesInfo.nodeList.length())-1;
  
  velocityMatrix=zeros(nodesInfo.nodeList.length(),nodesInfo.nodeList.length())-1;
  for(int node_cnt=0;node_cnt<nodeVelocity.length();node_cnt++)
  {
    vec relativeVelocity=abs(nodeVelocity-nodeVelocity(node_cnt));
    velocityMatrix.set_row(node_cnt,relativeVelocity);
  }
  
}

void ChannelCloud::setSysLinkGainInfo(ivec nodeList,mat pathDelay, mat &linkGainTableInDB){

  if(linkGainTableInDB.rows()!=nodeList.length())
  {cout<<"[both:]Error. Mismatch between number of nodes and linkBudget rows().";abort();}
  if(linkGainTableInDB.cols()!=nodeList.length())
  {cout<<"[both:]Error. Mismatch between number of nodes and linkBudget cols().";abort();}

  nodesInfo.nodeList=nodeList;
  nodesInfo.antennaCount=zeros_i(nodeList.length())-1;
  nodesInfo.noiseVariancePerDimension=zeros(nodeList.length())-1;
  nodesInfo.carrierFrequency=zeros(nodeList.length())-1;
  nodesInfo.beamParametersPerNode.set_length(nodeList.length());
  nodesInfo.beamIDPerTxRU.set_length(nodeList.length());
  
  linkGainInDB=linkGainTableInDB;
  pathDelayInSec=pathDelay;
  instantPathDelayInSec=pathDelayInSec;

  currentTxNodes=nodeList;
  
  nodeSyncList=zeros_i(nodeList.length())-1;
  if(!isTimeDomain())
    clearChannelBuffer();
  channelInfoIndx=zeros_i(nodeList.length(),nodeList.length())-1;
  velocityMatrix=zeros(nodeList.length(),nodeList.length())-1;  
}

void ChannelCloud::updateLinkGainInfo(int refNode, ivec nodeList,Array<vec> &linkGainTableInDB){

  int indx=find(nodesInfo.nodeList,refNode);
  if(indx!=-1)
  {
    vec oldValues=linkGainInDB.get_row(indx);
    ivec updateIndices=find(nodesInfo.nodeList,nodeList);
    setInVec(oldValues,updateIndices,linkGainTableInDB(0));
    linkGainInDB.set_row(indx,oldValues);

    oldValues=linkGainInDB.get_col(indx);
    setInVec(oldValues,updateIndices,linkGainTableInDB(1));
    linkGainInDB.set_col(indx,oldValues);
  }
  else
  {
    cout<<"[detl:]Unknown refNode to updateLinkGainInfo()."<<endl;
  }
}

void ChannelCloud::setLinkSpecificChannelInfo(ivec txNodeList, ivec rxNodeList,double linkVelocity,   FadingType_E linkFadingType, bool overwrite){

  if(nodesInfo.nodeList.length()==0)
  {cout<<"[both:]Error. Please load the channel with link budget info before setting Link specific channel info."<<endl;abort();}

  ivec txNodeIndx(txNodeList.length()),rxNodeIndx(rxNodeList.length());
  for(int txNode_cnt=0;txNode_cnt<txNodeList.length();txNode_cnt++)
  {
    txNodeIndx(txNode_cnt)=find(nodesInfo.nodeList,txNodeList(txNode_cnt));
    if(txNodeIndx(txNode_cnt)==-1)
      cout<<"[detl:]Warning. Link Budget Info for tx node "<<txNodeList(txNode_cnt)<<" not available in the channel. Skipping the node."<<endl;
  }
  for(int rxNode_cnt=0;rxNode_cnt<rxNodeList.length();rxNode_cnt++)
  {
    rxNodeIndx(rxNode_cnt)=find(nodesInfo.nodeList,rxNodeList(rxNode_cnt));
    if(rxNodeIndx(rxNode_cnt)==-1)
      cout<<"[detl:]Warning. Link Budget Info for rx node "<<rxNodeList(rxNode_cnt)<<" not available in the channel. Skipping the node."<<endl;
  }

  int infoIndx;
  Array<ivec> newInfoNodeIDS;
  newInfoNodeIDS.set_size(0);
  bool newEntry=true;
  for(int txNode_cnt=0;txNode_cnt<txNodeList.length();txNode_cnt++)
    if(txNodeIndx(txNode_cnt)!=-1)
    {
      for(int rxNode_cnt=0;rxNode_cnt<rxNodeList.length();rxNode_cnt++)
	if(rxNodeIndx(rxNode_cnt)!=-1)
	{
	  //Reciprocity maintained internally ...
	  infoIndx=channelInfoIndx(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt));
	  if(infoIndx!=-1)
	  {
	    if(overwrite==true)
	    {
	      linkSpecificChannelInfo(infoIndx).fadingType=linkFadingType;
	      velocityMatrix(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkVelocity;
	      if(linkFadingType==_FADING_TYPE_CORRELATED_)
	      {
		linkSpecificChannelInfo(infoIndx).channelModel=channelModelDecider.getChannelModel(linkVelocity);
		if(channelModelDecider.isTDLmodel || isCDLModel(linkSpecificChannelInfo(infoIndx).channelModel))
		  linkSpecificChannelInfo(infoIndx).delaySpreadTDL=channelModelDecider.getDelaySpreadForTDL(linkVelocity);
	      }
	    }
	  }
	  else
	  {
	    if(newEntry)
	    {
	      ChannelInfo_S myChannelInfo;
	      velocityMatrix(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkVelocity;
	      myChannelInfo.fadingType=linkFadingType;
	      if(linkFadingType==_FADING_TYPE_CORRELATED_)
	      {
		myChannelInfo.channelModel=channelModelDecider.getChannelModel(linkVelocity);
		if(channelModelDecider.isTDLmodel || isCDLModel(myChannelInfo.channelModel))
		  myChannelInfo.delaySpreadTDL=channelModelDecider.getDelaySpreadForTDL(linkVelocity);
	      }
	      append(linkSpecificChannelInfo,myChannelInfo);
	      newEntry=false;
	    }
	    //To maintain Reciprocity
	    channelInfoIndx(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkSpecificChannelInfo.length()-1;
	    channelInfoIndx(txNodeIndx(txNode_cnt),rxNodeIndx(rxNode_cnt))=linkSpecificChannelInfo.length()-1;
	    
	  }
	}
    }
    
}

void ChannelCloud::setLinkSpecificChannelInfo(ivec txNodeList, ivec rxNodeList,FadingChannelModel_E linkChannelModel,double linkVelocity,   FadingType_E linkFadingType, bool overwrite, string customFadParamsName){

  if(nodesInfo.nodeList.length()==0)
  {cout<<"[both:]Please load the channel with link budget info before setting Link specific channel info."<<endl;abort();}

  ivec txNodeIndx(txNodeList.length()),rxNodeIndx(rxNodeList.length());
  for(int txNode_cnt=0;txNode_cnt<txNodeList.length();txNode_cnt++)
  {
    txNodeIndx(txNode_cnt)=find(nodesInfo.nodeList,txNodeList(txNode_cnt));
    if(txNodeIndx(txNode_cnt)==-1)
      cout<<"[detl:]Warning. Link Budget Info for tx node "<<txNodeList(txNode_cnt)<<" not available in the channel. Skipping the node."<<endl;
  }
  for(int rxNode_cnt=0;rxNode_cnt<rxNodeList.length();rxNode_cnt++)
  {
    rxNodeIndx(rxNode_cnt)=find(nodesInfo.nodeList,rxNodeList(rxNode_cnt));
    if(rxNodeIndx(rxNode_cnt)==-1)
      cout<<"[detl:]Warning. Link Budget Info for rx node "<<rxNodeList(rxNode_cnt)<<" not available in the channel. Skipping the node."<<endl;
  }

  int infoIndx;
  Array<ivec> newInfoNodeIDS;
  newInfoNodeIDS.set_size(0);
  bool newEntry=true;
  for(int txNode_cnt=0;txNode_cnt<txNodeList.length();txNode_cnt++)
  if(txNodeIndx(txNode_cnt)!=-1)
  {
	for(int rxNode_cnt=0;rxNode_cnt<rxNodeList.length();rxNode_cnt++)
	if(rxNodeIndx(rxNode_cnt)!=-1)
	{
	  //Reciprocity maintained internally ...
	  infoIndx=channelInfoIndx(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt));
	  if(infoIndx!=-1)
	  {
		if(overwrite==true)
		{
		  linkSpecificChannelInfo(infoIndx).channelModel=linkChannelModel;
		  linkSpecificChannelInfo(infoIndx).fadingType=linkFadingType;
		  if(linkChannelModel==_FADING_CHANNEL_MODEL_CUSTOM_)
		  {
		    if(findInStringArray(customFadingParametersName,customFadParamsName)==-1)
		    {
			  cout<<"[both:]Error. Undefined 'customFadingParametersName' to setLinkSpecificChannelInfo()."<<endl;
			  abort();
		    }
		    linkSpecificChannelInfo(infoIndx).customFadParametersName=customFadParamsName;
		  }
		  velocityMatrix(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkVelocity;
		}
	  }
	  else
	  {
	    if(newEntry)
	    {
	      ChannelInfo_S myChannelInfo;
	      velocityMatrix(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkVelocity;
	      myChannelInfo.fadingType=linkFadingType;
	      myChannelInfo.channelModel=linkChannelModel;
	      if(linkChannelModel==_FADING_CHANNEL_MODEL_CUSTOM_)
	      {
		if(findInStringArray(customFadingParametersName,customFadParamsName)==-1)
		{
		  cout<<"[both:]Error. Undefined 'customFadingParametersName' to setLinkSpecificChannelInfo()."<<endl;
		  abort();
		}
		myChannelInfo.customFadParametersName=customFadParamsName;
	      }
	      append(linkSpecificChannelInfo,myChannelInfo);
	      newEntry=false;
	    }
	    //To maintain Reciprocity
	    channelInfoIndx(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkSpecificChannelInfo.length()-1;
	    channelInfoIndx(txNodeIndx(txNode_cnt),rxNodeIndx(rxNode_cnt))=linkSpecificChannelInfo.length()-1;
	    
	  }
	}
  }
  
}

void ChannelCloud::setLinkSpecificChannelInfo(ivec txNodeList, ivec rxNodeList, SCMPropagationScenario scmScenario,double linkVelocity,  bool overwrite){

  if(nodesInfo.nodeList.length()==0)
  {cout<<"[both:]Please load the channel with link budget info before setting Link specific channel info."<<endl;abort();}

  ivec txNodeIndx(txNodeList.length()),rxNodeIndx(rxNodeList.length());
  for(int txNode_cnt=0;txNode_cnt<txNodeList.length();txNode_cnt++)
  {
    txNodeIndx(txNode_cnt)=find(nodesInfo.nodeList,txNodeList(txNode_cnt));
    if(txNodeIndx(txNode_cnt)==-1)
      cout<<"[detl:]Warning. Link Budget Info for tx node "<<txNodeList(txNode_cnt)<<" not available in the channel. Skipping the node."<<endl;
  }
  for(int rxNode_cnt=0;rxNode_cnt<rxNodeList.length();rxNode_cnt++)
  {
    rxNodeIndx(rxNode_cnt)=find(nodesInfo.nodeList,rxNodeList(rxNode_cnt));
    if(rxNodeIndx(rxNode_cnt)==-1)
      cout<<"[detl:]Warning. Link Budget Info for rx node "<<rxNodeList(rxNode_cnt)<<" not available in the channel. Skipping the node."<<endl;
  }

  int infoIndx;
  Array<ivec> newInfoNodeIDS;
  newInfoNodeIDS.set_size(0);
  bool newEntry=true;
  
  for(int txNode_cnt=0;txNode_cnt<txNodeList.length();txNode_cnt++)
  {
    for(int rxNode_cnt=0;rxNode_cnt<rxNodeList.length();rxNode_cnt++)
    {
      //Reciprocity maintained internally ...
      infoIndx=channelInfoIndx(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt));
      if(infoIndx!=-1)
      {
	if(overwrite==true)
	{
	  linkSpecificChannelInfo(infoIndx).scmPropagationScenario=scmScenario;
	  velocityMatrix(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkVelocity;
	}
      }
      else
      {
	if(newEntry)
	{
	  ChannelInfo_S myChannelInfo;
	  velocityMatrix(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkVelocity;
	  myChannelInfo.scmPropagationScenario=scmScenario;
	  append(linkSpecificChannelInfo,myChannelInfo);
	  newEntry=false;
	}
	//To maintain Reciprocity
	channelInfoIndx(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkSpecificChannelInfo.length()-1;
	channelInfoIndx(txNodeIndx(txNode_cnt),rxNodeIndx(rxNode_cnt))=linkSpecificChannelInfo.length()-1;
      }
    }
  }
}



/*
void ChannelCloud::setLinkSpecificChannelInfo(ivec txNodeList, ivec rxNodeList,double linkVelocity,   FadingType_E linkFadingType, bool overwrite){


  if(nodesInfo.nodeList.length()==0)
  {cout<<"[both:]Error. Please load the channel with link budget info before setting Link specific channel info."<<endl;abort();}

  ivec txNodeIndx(txNodeList.length()),rxNodeIndx(rxNodeList.length());
  for(int txNode_cnt=0;txNode_cnt<txNodeList.length();txNode_cnt++)
  {
    txNodeIndx(txNode_cnt)=find(nodesInfo.nodeList,txNodeList(txNode_cnt));
    if(txNodeIndx(txNode_cnt)==-1)
      cout<<"[detl:]Warning. Link Budget Info for tx node "<<txNodeList(txNode_cnt)<<" not available in the channel. Skipping the node."<<endl;
  }
  for(int rxNode_cnt=0;rxNode_cnt<rxNodeList.length();rxNode_cnt++)
  {
    rxNodeIndx(rxNode_cnt)=find(nodesInfo.nodeList,rxNodeList(rxNode_cnt));
    if(rxNodeIndx(rxNode_cnt)==-1)
      cout<<"[detl:]Warning. Link Budget Info for rx node "<<rxNodeList(rxNode_cnt)<<" not available in the channel. Skipping the node."<<endl;
  }

  int infoIndx;
  Array<ivec> newInfoNodeIDS;
  newInfoNodeIDS.set_size(0);
  bool newEntry=true;
  for(int txNode_cnt=0;txNode_cnt<txNodeList.length();txNode_cnt++)
    if(txNodeIndx(txNode_cnt)!=-1)
    {
      for(int rxNode_cnt=0;rxNode_cnt<rxNodeList.length();rxNode_cnt++)
	if(rxNodeIndx(rxNode_cnt)!=-1)
	{
	  infoIndx=channelInfoIndx(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt));
	  if(infoIndx!=-1)
	  {
	    if(overwrite==true)
	    {
	      linkSpecificChannelInfo(infoIndx).fadingType=linkFadingType;
// 	      linkSpecificChannelInfo(infoIndx).dopplerSpread=linkDopplerSpread;
// 	      if(linkFadingType==_FADING_TYPE_CORRELATED_)
// 		linkSpecificChannelInfo(infoIndx).channelModel=channelModelDecider.getChannelModel(linkDopplerSpread);
	      velocityMatrix(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkVelocity;
	      if(linkFadingType==_FADING_TYPE_CORRELATED_)
		linkSpecificChannelInfo(infoIndx).channelModel=channelModelDecider.getChannelModel(linkVelocity);
	
	    }
	  }
	  else
	  {
	    if(newEntry)
	    {
	      ChannelInfo_S myChannelInfo;
// 	      myChannelInfo.dopplerSpread=linkDopplerSpread;
	       velocityMatrix(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkVelocity;
	      myChannelInfo.fadingType=linkFadingType;
	      if(linkFadingType==_FADING_TYPE_CORRELATED_)
		//myChannelInfo.channelModel=channelModelDecider.getChannelModel(linkDopplerSpread);
	      myChannelInfo.channelModel=channelModelDecider.getChannelModel(linkVelocity);	     
	      append(linkSpecificChannelInfo,myChannelInfo);
	      newEntry=false;
	    }
	    //To maintain Reciprocity
	    channelInfoIndx(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkSpecificChannelInfo.length()-1;
	    channelInfoIndx(txNodeIndx(txNode_cnt),rxNodeIndx(rxNode_cnt))=linkSpecificChannelInfo.length()-1;
	    
	  }
	}
    }
    
}

void ChannelCloud::setLinkSpecificChannelInfo(ivec txNodeList, ivec rxNodeList,FadingChannelModel_E linkChannelModel,double linkVelocity,   FadingType_E linkFadingType, bool overwrite, string customFadParamsName){


  if(nodesInfo.nodeList.length()==0)
  {cout<<"[both:]Please load the channel with link budget info before setting Link specific channel info."<<endl;abort();}

  ivec txNodeIndx(txNodeList.length()),rxNodeIndx(rxNodeList.length());
  for(int txNode_cnt=0;txNode_cnt<txNodeList.length();txNode_cnt++)
  {
    txNodeIndx(txNode_cnt)=find(nodesInfo.nodeList,txNodeList(txNode_cnt));
    if(txNodeIndx(txNode_cnt)==-1)
      cout<<"[detl:]Warning. Link Budget Info for tx node "<<txNodeList(txNode_cnt)<<" not available in the channel. Skipping the node."<<endl;
  }
  for(int rxNode_cnt=0;rxNode_cnt<rxNodeList.length();rxNode_cnt++)
  {
    rxNodeIndx(rxNode_cnt)=find(nodesInfo.nodeList,rxNodeList(rxNode_cnt));
    if(rxNodeIndx(rxNode_cnt)==-1)
      cout<<"[detl:]Warning. Link Budget Info for rx node "<<rxNodeList(rxNode_cnt)<<" not available in the channel. Skipping the node."<<endl;
  }

  int infoIndx;
  Array<ivec> newInfoNodeIDS;
  newInfoNodeIDS.set_size(0);
  bool newEntry=true;
  for(int txNode_cnt=0;txNode_cnt<txNodeList.length();txNode_cnt++)
  if(txNodeIndx(txNode_cnt)!=-1)
  {
	for(int rxNode_cnt=0;rxNode_cnt<rxNodeList.length();rxNode_cnt++)
	if(rxNodeIndx(rxNode_cnt)!=-1)
	{
	  infoIndx=channelInfoIndx(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt));
	  if(infoIndx!=-1)
	  {
		if(overwrite==true)
		{
		  linkSpecificChannelInfo(infoIndx).channelModel=linkChannelModel;
		  linkSpecificChannelInfo(infoIndx).fadingType=linkFadingType;
		  if(linkChannelModel==_FADING_CHANNEL_MODEL_CUSTOM_)
		  {
		    if(findInStringArray(customFadingParametersName,customFadParamsName)==-1)
		    {
			  cout<<"[both:]Error. Undefined 'customFadingParametersName' to setLinkSpecificChannelInfo()."<<endl;
			  abort();
		    }
		    linkSpecificChannelInfo(infoIndx).customFadParametersName=customFadParamsName;
		  }
		 // linkSpecificChannelInfo(infoIndx).dopplerSpread=linkDopplerSpread;
		   velocityMatrix(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkVelocity;		
		}
	  }
	  else
	  {
	    if(newEntry)
	    {
	      ChannelInfo_S myChannelInfo;
	      //myChannelInfo.dopplerSpread=linkDopplerSpread;
	      velocityMatrix(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkVelocity;	      
	      myChannelInfo.fadingType=linkFadingType;
	      myChannelInfo.channelModel=linkChannelModel;
	      if(linkChannelModel==_FADING_CHANNEL_MODEL_CUSTOM_)
	      {
		if(findInStringArray(customFadingParametersName,customFadParamsName)==-1)
		{
		  cout<<"[both:]Error. Undefined 'customFadingParametersName' to setLinkSpecificChannelInfo()."<<endl;
		  abort();
		}
		myChannelInfo.customFadParametersName=customFadParamsName;
	      }
	      append(linkSpecificChannelInfo,myChannelInfo);
	      newEntry=false;
	    }
	    //To maintain Reciprocity
	    channelInfoIndx(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkSpecificChannelInfo.length()-1;
	    channelInfoIndx(txNodeIndx(txNode_cnt),rxNodeIndx(rxNode_cnt))=linkSpecificChannelInfo.length()-1;
	    
	  }
	}
  }
  
}

void ChannelCloud::setLinkSpecificChannelInfo(ivec txNodeList, ivec rxNodeList, SCMPropagationScenario scmScenario,double linkVelocity,  bool overwrite){


  if(nodesInfo.nodeList.length()==0)
  {cout<<"[both:]Please load the channel with link budget info before setting Link specific channel info."<<endl;abort();}

  ivec txNodeIndx(txNodeList.length()),rxNodeIndx(rxNodeList.length());
  for(int txNode_cnt=0;txNode_cnt<txNodeList.length();txNode_cnt++)
  {
    txNodeIndx(txNode_cnt)=find(nodesInfo.nodeList,txNodeList(txNode_cnt));
    if(txNodeIndx(txNode_cnt)==-1)
      cout<<"[detl:]Warning. Link Budget Info for tx node "<<txNodeList(txNode_cnt)<<" not available in the channel. Skipping the node."<<endl;
  }
  for(int rxNode_cnt=0;rxNode_cnt<rxNodeList.length();rxNode_cnt++)
  {
    rxNodeIndx(rxNode_cnt)=find(nodesInfo.nodeList,rxNodeList(rxNode_cnt));
    if(rxNodeIndx(rxNode_cnt)==-1)
      cout<<"[detl:]Warning. Link Budget Info for rx node "<<rxNodeList(rxNode_cnt)<<" not available in the channel. Skipping the node."<<endl;
  }

  int infoIndx;
  Array<ivec> newInfoNodeIDS;
  newInfoNodeIDS.set_size(0);
  bool newEntry=true;
  for(int txNode_cnt=0;txNode_cnt<txNodeList.length();txNode_cnt++)
  {
    for(int rxNode_cnt=0;rxNode_cnt<rxNodeList.length();rxNode_cnt++)
    {
      infoIndx=channelInfoIndx(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt));
      if(infoIndx!=-1)
      {
	if(overwrite==true)
	{
	  linkSpecificChannelInfo(infoIndx).scmPropagationScenario=scmScenario;
	  //linkSpecificChannelInfo(infoIndx).dopplerSpread=linkDopplerSpread;
	  velocityMatrix(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkVelocity;	
	}
      }
      else
      {
	if(newEntry)
	{
	  ChannelInfo_S myChannelInfo;
	  //myChannelInfo.dopplerSpread=linkDopplerSpread;
	  velocityMatrix(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkVelocity;	 
	  myChannelInfo.scmPropagationScenario=scmScenario;
	  append(linkSpecificChannelInfo,myChannelInfo);
	  newEntry=false;
	}
	//To maintain Reciprocity
	channelInfoIndx(rxNodeIndx(rxNode_cnt),txNodeIndx(txNode_cnt))=linkSpecificChannelInfo.length()-1;
	channelInfoIndx(txNodeIndx(txNode_cnt),rxNodeIndx(rxNode_cnt))=linkSpecificChannelInfo.length()-1;
      }
    }
  }
}
*/
void ChannelCloud::setLinkGain(int nodeX, int nodeY, double xToyLinkGain,double yToxLinkGain, bool overwtrite){

  int xIndx=find(nodesInfo.nodeList,nodeX);
  int yIndx=find(nodesInfo.nodeList,nodeY);
  if(yIndx!=-1 && xIndx!=-1)
  {
    if(overwtrite)
    {
      linkGainInDB(yIndx,xIndx)=xToyLinkGain;
      linkGainInDB(xIndx,yIndx)=yToxLinkGain;
    }
    else
      if(std::isnan(linkGainInDB(yIndx,xIndx)) && std::isnan(linkGainInDB(xIndx,yIndx)) )
      {
	linkGainInDB(yIndx,xIndx)=xToyLinkGain;
	linkGainInDB(xIndx,yIndx)=yToxLinkGain;
      }
	  else
	    cout<<"[detl:]Warning. Trying to overwrite Link budget for node : "<<nodeX<<" and node : "<<nodeY<<endl;
  }
  else
    cout<<"[detl:]Warning. Unknown Nodes to setLinkGain."<<endl;

}

void ChannelCloud::setNodesAntennaCount(ivec nodeList,int count){

  if(nodesInfo.nodeList.length()==0)
  {
	cout<<"[both:]Error. Load nodes before setting node Antenna count."<<endl;
	abort();
  }

  int nodeIndx;
  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
  {
	nodeIndx=find(nodesInfo.nodeList,nodeList(node_cnt));
	if(nodeIndx!=-1)
	  nodesInfo.antennaCount(nodeIndx)=count;
	else
	  cout<<"[detl:]Warning. Node "<<nodeList(node_cnt)<<" specified in setNodesAntennaCount() is not available in the channel. Skipping entry."<<endl;
  }
}

void ChannelCloud::setNodesAntennaCount(ivec nodeList,ivec countList){

  if(nodesInfo.nodeList.length()==0)
  {
	cout<<"[both:]Error. Load nodes before setting Antenna count."<<endl;
	abort();
  }

  if(nodeList.length()!=countList.length())
  {
    cout<<"[both:]Error. Length mismatch between nodeList and countList passed to setNodesAntennaCount()."<<endl;
	abort();
  }

  int nodeIndx;
  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
  {
	nodeIndx=find(nodesInfo.nodeList,nodeList(node_cnt));
	if(nodeIndx!=-1)
	  nodesInfo.antennaCount(nodeIndx)=countList(node_cnt);
	else
	{
	  cout<<"[detl:]Warning. Node "<<nodeList(node_cnt)<<" specified for setNodesAntennaCount() is not available in the channel. Skipping entry."<<endl;
	}
  }
}

void ChannelCloud::setNodesCarrierFrequency(ivec nodeList,double carrierFreq){
  
  if(nodesInfo.nodeList.length()==0)
  {
    cout<<"[both:]Error. Load nodes before setting node carrierFreq."<<endl;
    abort();
  }
  
  int nodeIndx;
  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
  {
    nodeIndx=find(nodesInfo.nodeList,nodeList(node_cnt));
    if(nodeIndx!=-1)
      nodesInfo.carrierFrequency(nodeIndx)=carrierFreq;
    else
      cout<<"[detl:]Warning. Node "<<nodeList(node_cnt)<<" specified in setNodesCarrierFrequency() is not available in the channel. Skipping entry."<<endl;
  }
}

void ChannelCloud::setNodesCarrierFrequency(ivec nodeList,vec carrierFreq){
  
  if(nodesInfo.nodeList.length()==0)
  {
    cout<<"[both:]Error. Load nodes before setting Antenna count."<<endl;
    abort();
  }
  
  if(nodeList.length()!=carrierFreq.length())
  {
    cout<<"[both:]Error. Length mismatch between nodeList and countList passed to setNodesCarrierFrequency()."<<endl;
    abort();
  }
  
  int nodeIndx;
  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
  {
    nodeIndx=find(nodesInfo.nodeList,nodeList(node_cnt));
    if(nodeIndx!=-1)
      nodesInfo.carrierFrequency(nodeIndx)=carrierFreq(node_cnt);
    else
    {
      cout<<"[detl:]Warning. Node "<<nodeList(node_cnt)<<" specified for setNodesCarrierFrequency() is not available in the channel. Skipping entry."<<endl;
    }
  }
}


void ChannelCloud::setNodesNoiseVariancePerDimension(ivec nodeList,double noiseVar){

  if(nodesInfo.nodeList.length()==0)
  {
    cout<<"[both:]Error. Load nodes before setting server node Noise Variance."<<endl;
    abort();
  }

  int nodeIndx;
  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
  {
    nodeIndx=find(nodesInfo.nodeList,nodeList(node_cnt));
    if(nodeIndx!=-1)
      nodesInfo.noiseVariancePerDimension(nodeIndx)=noiseVar;
    else
      cout<<"[detl:]Warning. Node "<<nodeList(node_cnt)<<" specified in setNodesNoiseVariancePerDimension() is not available in the channel. Skipping entry."<<endl;
  }
}

void ChannelCloud::setNodesNoiseVariancePerDimension(ivec nodeList,vec noiseVars){

  if(nodesInfo.nodeList.length()==0)
  {
    cout<<"[both:]Error. Load nodes before setting Noise Variance."<<endl;
    abort();
  }

  if(nodeList.length()!=noiseVars.length())
  {
    cout<<"[both:]Error. Length mismatch between nodeList and noiseVars passed to setServerNodesNoiseVariance()."<<endl;
    abort();
  }

  int nodeIndx;
  for(int node_cnt=0;node_cnt<nodeList.length();node_cnt++)
  {
    nodeIndx=find(nodesInfo.nodeList,nodeList(node_cnt));
    if(nodeIndx!=-1)
      nodesInfo.noiseVariancePerDimension(nodeIndx)=noiseVars(node_cnt);
    else
    {
      cout<<"[detl:]Warning. Node "<<nodeList(node_cnt)<<" specified for setNodesNoiseVariancePerDimension() is not available in the channel. Skipping entry."<<endl;
    }
  }
}

void ChannelCloud::transmitData(int txNodeID, int txBeamID, int datasize, Array< cvec >& dataFromAllAntennas, double currentTimeInSeconds, int overwriteOrAdd){

  if(currentTxNodes.length()==0)
  {
    cout<<"[both:] Please call setNodesToTxMode() before transmitData().. "<<endl;
    abort();
  }
  if(find(currentTxNodes,txNodeID)==-1)
  {
    cout<<"[both:] Error .. txNodeID : "<<txNodeID<<" not in Tx mode in the Channel Cloud.. Please call setNodesToTxMode() before transmitData().. "<<endl;
    abort();
  }

  if(nodesInfo.nodeList.length()==0)
  {
	cout<<"[detl:]Please set nodes and their linkBudget before transmitting data."<<endl;
  }
  int txNodeIndx=find(nodesInfo.nodeList,txNodeID);
  if(txNodeIndx==-1)
  {
	cout<<"[both:]Error. No Links to the given TX node available in the channel."<<endl;
	abort();
  }
  ivec txRUs=nodesInfo.getTXRUs(txNodeID,txBeamID);
  int antennaCount=txRUs.length();

  if(antennaCount!=dataFromAllAntennas.length())
  {
	cout<<"[both:]Error. Mismatch between antenna count set for Tx node "<<txNodeID<<" and the transmitted data size."<<endl;
	abort();
  }
  if(dataSize==-1)
	dataSize=datasize;
  
  
  channelFilterPerNode(txNodeIndx).load(dataFromAllAntennas,txRUs,currentTimeInSeconds,overwriteOrAdd);
}

cvec ChannelCloud::applyGaussianNoiseAndReturnNoise(cvec &signal, double noiseVariancePerDimension){

  cvec noise=randn_c(signal.length())*sqrt(noiseVariancePerDimension*2);
  signal+=noise;
  return(noise);
}

void ChannelCloud::setNodesToTxMode(ivec txNodes){

  currentTxNodes=txNodes;
}

RestOfTransmittersInfo_S ChannelCloud::getRestOfTransmittersInfo(int rxNodeID, ivec txNodesToOmit){

  RestOfTransmittersInfo_S roiInfo;
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  append(txNodesToOmit,nodesInfo.nodeList(findNot(nodesInfo.nodeList,currentTxNodes)));
  ivec txIndices=findNot(nodesInfo.nodeList,txNodesToOmit);
  roiInfo.txNodeID=nodesInfo.nodeList.get(txIndices);
  roiInfo.channelGainInDB.set_size(txIndices.length());
  for(int txNode_cnt=0;txNode_cnt<txIndices.length();txNode_cnt++)
      roiInfo.channelGainInDB(txNode_cnt)=getLinkGainInDB(rxNodeID,roiInfo.txNodeID(txNode_cnt));
//      roiInfo.channelGainInDB(txNode_cnt)=getLinkGainInDB(rxNodeIndx,txIndices(txNode_cnt));
  return(roiInfo);
}

ChannelOutput_S ChannelCloud::receiveData(int rxNodeID, int rxBeamID, AntennaCorrelationType_E antCorrelationType, int ofdmSymbolIndx, double currentTimeInSeconds, double endTimeInSeconds, bool printChannel, ivec txNodeList, ivec txNodesToSumPowerFrom){

  ChannelOutput_S channelOutput,tempChannelOutput;
  
  
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
	cout<<"[both:]Error. No linkBudget info for Rx node "<<rxNodeID<<" available in the channel."<<endl;
	abort();
  }

  if(txNodeList.length()==0)
    txNodeList=currentTxNodes;
  else
    if(find(find(currentTxNodes,txNodeList),-1)!=-1)
    {
      cout<<"[both:] Error .. TxNodeList to receiveData() contains nodes which are not in Tx Mode .. "<<endl;
      abort();
    }

  int rxAntennaCount,txAntennaCount;
  rxAntennaCount=nodesInfo.getTXRUCount(rxNodeID,rxBeamID);
  if(rxAntennaCount==-1)
  {
	cout<<"[both:]Error. No antenna count for Rx node "<<rxNodeID<<" available in the channel."<<endl;
	abort();
  }
  
  int outputSize;
  double samplingFreqToUse;
  if(nodesInfo.samplingFrequency.length()!=0)
    samplingFreqToUse=nodesInfo.samplingFrequency(rxNodeIndx); //using rxNodeID sampling freqneucy to decide output size
  else
    samplingFreqToUse=commonSamplingFrequency;
    
  if(timeDomain)
    outputSize=round_i((endTimeInSeconds-currentTimeInSeconds)*samplingFreqToUse);
  else
    outputSize=dataSize;
  channelOutput.rxNodeID=rxNodeID;
  channelOutput.txNodeIDs=txNodeList;
  channelOutput.channels.set_size(txNodeList.length());
  channelOutput.data.set_size(rxAntennaCount);
  channelOutput.summedUpSignalPerAntennaPerTone.set_length(rxAntennaCount);
  for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
  {
    channelOutput.summedUpSignalPerAntennaPerTone(rxAntenna_cnt)=zeros_c(outputSize);
    channelOutput.data(rxAntenna_cnt)=zeros_c(outputSize);
  }
  
  double channelGainInDB;
  ivec myTxNodeIndices=find(nodesInfo.nodeList,txNodeList);
  ivec sumPowerNodeIndices=find(txNodesToSumPowerFrom,txNodeList);
  bool sumTonePower;
  
  
  
  for(int txNode_cnt=0;txNode_cnt<myTxNodeIndices.length();txNode_cnt++)
  {
    
    
        if(sumPowerNodeIndices(txNode_cnt)==-1)
	  sumTonePower=false;
	else
	  sumTonePower=true;
 	channelGainInDB=linkGainInDB.get(rxNodeIndx,myTxNodeIndices(txNode_cnt));
//    channelGainInDB = getLinkGainInDB(rxNodeIndx,myTxNodeIndices(txNode_cnt));
	if(!std::isnan(channelGainInDB))
	  if(channelFilterPerNode(myTxNodeIndices(txNode_cnt)).isLoaded())
	{
	  txAntennaCount=nodesInfo.antennaCount(myTxNodeIndices(txNode_cnt));

	  if(antCorrelationType!=_ANTENNA_CORRELATION_UNCORRELATED_)
	  {              
                 	    
	    if(CorrMatrix.isInitialized)
	    {
		Array<cmat> antennaCorrelationMatrices=getAntennaCorrelationMatrices(txAntennaCount,rxAntennaCount);                                
		tempChannelOutput=receiveData(rxNodeID,rxBeamID,nodesInfo.nodeList(myTxNodeIndices(txNode_cnt)),ofdmSymbolIndx,currentTimeInSeconds,endTimeInSeconds,printChannel,sumTonePower,antennaCorrelationMatrices(0),antennaCorrelationMatrices(1));
	    }
	    else
	      tempChannelOutput=receiveData(rxNodeID,rxBeamID,nodesInfo.nodeList(myTxNodeIndices(txNode_cnt)),ofdmSymbolIndx,currentTimeInSeconds,endTimeInSeconds,printChannel,sumTonePower);
	  }
	  else
	    tempChannelOutput=receiveData(rxNodeID,rxBeamID,nodesInfo.nodeList(myTxNodeIndices(txNode_cnt)),ofdmSymbolIndx,currentTimeInSeconds,endTimeInSeconds,printChannel,sumTonePower);
	  for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
	  {
		channelOutput.data(rxAntenna_cnt)+=tempChannelOutput.data(rxAntenna_cnt);
		if(sumTonePower)	channelOutput.summedUpSignalPerAntennaPerTone(rxAntenna_cnt)+=tempChannelOutput.summedUpSignalPerAntennaPerTone(rxAntenna_cnt);
	  }
	  channelOutput.channels(txNode_cnt)=tempChannelOutput.channels(0);
   
	}
	
	/*
       if (rxNodeID==10)
       {  if (txNode_cnt==0)
	 {  if (ofdmSymbolIndx==0)
	   
	     { 
	     cout<<"txNode_count="<<txNode_cnt<<endl;
	     cout<<"rxNode_count="<<rxNodeID<<endl;
	     cout<<"ofdm_symbol_index="<<ofdmSymbolIndx<<endl;
	  cmat ch(4,2), ch_avg(4,2), U_ch, V_ch;
	  vec S_ch;
	  
      ch(0,0)=sum(channelOutput.channels(0)(0)(0));    // ch_avg(txAntenna_cnt)(rxAntenna_cnt)
      ch(1,0)=sum(channelOutput.channels(0)(1)(0));
      ch(2,0)=sum(channelOutput.channels(0)(2)(0));
      ch(3,0)=sum(channelOutput.channels(0)(3)(0));
      ch(0,1)=sum(channelOutput.channels(0)(0)(1));
      ch(1,1)=sum(channelOutput.channels(0)(1)(1));
      ch(2,1)=sum(channelOutput.channels(0)(2)(1));
      ch(3,1)=sum(channelOutput.channels(0)(3)(1));
      ch_avg=ch/(channelOutput.channels(0)(0)(0)).length();
      svd(ch_avg,U_ch, S_ch ,  V_ch); 
      cout<<"channel matrix"<<ch_avg<<endl;
      cout<<"precoder matrix"<<V_ch<<endl;
	  }
	 }
	 
      }*/
  }
  
  if(addNoise)
  {
    double noiseVar=nodesInfo.noiseVariancePerDimension(rxNodeIndx);
  
//     Check for carrierFreq and add PN
   if (nodesInfo.carrierFrequency(rxNodeIndx) >= 6*1e9) {
     
   // cout<<"Adding phase noise : "<<  nodesInfo.carrierFrequency(rxNodeIndx)<<endl;
      
    int pnPSDID=6;
    cvec pnSamples = generatePNSamples(rxNodeID,pnPSDID,channelOutput.data(0).length());
    
  

  // Calculating Constant Phase Error (CPE) due to Phase Noise

    cvec pnConstantPhaseErr(1);
   
    pnConstantPhaseErr(0) = 0;
    for (int idx = 0; idx < channelOutput.data(0).length(); idx++)
      pnConstantPhaseErr(0) = pnConstantPhaseErr(0) + pnSamples(idx);
  
   
     pnConstantPhaseErr = pnConstantPhaseErr/channelOutput.data(0).length();
  
    for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
      channelOutput.data(rxAntenna_cnt) = elem_mult(channelOutput.data(rxAntenna_cnt),pnSamples);

    for(int txNode_cnt = 0; txNode_cnt <txNodeList.length(); txNode_cnt++)
      for(int txAntenna_cnt = 0; txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
	  channelOutput.channels(txNode_cnt)(txAntenna_cnt)(rxAntenna_cnt) = channelOutput.channels(txNode_cnt)(txAntenna_cnt)(rxAntenna_cnt) * pnConstantPhaseErr(0) ;
 }    
  
// abort(); 

    if(noiseVar<0) {cout<<"[detl:]Noise Variance not set for node " <<rxNodeID<<"  skipping noise addition."<<endl; noiseVar=0;}
    for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
      channelOutput.summedUpSignalPerAntennaPerTone(rxAntenna_cnt)+=applyGaussianNoiseAndReturnNoise(channelOutput.data(rxAntenna_cnt),noiseVar);
  }
  
  return(channelOutput);
}

ChannelOutput_S ChannelCloud::receiveData(int rxNodeID, int rxBeamID, int txNodeID, int ofdmSymbolIndx, double currentTimeInSeconds, double endTimeInSeconds, bool printChannel ,bool computeSumPower, cmat txAntennaCorrelationMatrix, cmat rxAntennaCorrelationMatrix){

  ChannelOutput_S channelOutput;
  if(find(currentTxNodes,txNodeID)==-1){cout<<"[both:] Error.. TxNode : "<<txNodeID<<" not in Tx mode .. "<<endl;abort();}

  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[both:]Error. No linkBudget info for Rx node "<<rxNodeID<<" available in the channel."<<endl;
    abort();
  }

  int txNodeIndx=find(nodesInfo.nodeList,txNodeID);
  if(txNodeIndx==-1)
  {
    cout<<"[both:]Error. No linkBudget info for Tx node "<<txNodeID<<" available in the channel."<<endl;
    abort();
  }
  
  if(nodesInfo.carrierFrequency(txNodeIndx)!=nodesInfo.carrierFrequency(rxNodeIndx))
  {
    cout<<"[both:]Error. Receiving data across carriers not possible currently in receiveData()."<<endl;
    abort();
  }

  if(nodesInfo.carrierFrequency(txNodeIndx)!=nodesInfo.carrierFrequency(rxNodeIndx))
  {
    cout<<"[both:]Error. Receiving data across carriers not possible currently in receiveData()."<<endl;
    abort();
  }
  
  int txAntennaCount;
  int rxAntennaCount;
  ivec rxAntenna=nodesInfo.getTXRUs(rxNodeID, rxBeamID);
  rxAntennaCount=rxAntenna.length();
  if(rxAntennaCount==-1)
  {
    cout<<"[both:]Error. No antenna count for Rx node "<<rxNodeID<<" available in the channel."<<endl;
    abort();
  }
  txAntennaCount=nodesInfo.antennaCount(txNodeIndx); // txAntennaCount not change as we need to receive from all antenna irrespective of beam
  if(txAntennaCount==-1)
  {
	cout<<"[both:]Error. No antenna count for Tx node "<<txNodeID<<" available in the channel."<<endl;
	abort();
  }
  
  if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_MOD_JAKES_)
    if(!modifiedJakesInfo.isLinked(rxNodeID,txNodeID))
      generateTapWeightsForModifiedJakes(rxNodeID,to_ivec(txNodeID),currentTimeInSeconds);
    
    

  int outputSize;
  double samplingFreqToUse;
  if(nodesInfo.samplingFrequency.length()!=0)
    samplingFreqToUse=nodesInfo.samplingFrequency(rxNodeIndx); //using rxNodeID sampling freqneucy to decide output size
  else
    samplingFreqToUse=commonSamplingFrequency;
  
  if(timeDomain)
    outputSize=round_i((endTimeInSeconds-currentTimeInSeconds)*samplingFreqToUse);
  else
    outputSize=dataSize;
  
  cvec tempData;
  channelOutput.data.set_size(rxAntennaCount);
  channelOutput.summedUpSignalPerAntennaPerTone.set_size(rxAntennaCount);
  for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
  {
    channelOutput.data(rxAntenna_cnt)=zeros_c(outputSize);
    channelOutput.summedUpSignalPerAntennaPerTone(rxAntenna_cnt)=zeros_c(outputSize);
  }

  channelOutput.channels.set_size(1);
  channelOutput.channels(0).set_size(txAntennaCount);
  for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	channelOutput.channels(0)(txAntenna_cnt).set_size(rxAntennaCount);
  Vec<unsigned int> randVec;
  double channelGainInDB=getChannelGain(rxNodeID,txNodeID);
//  double channelGainInDB=getLinkGainInDB(rxNodeIndx,txNodeIndx);

  if(!std::isnan(channelGainInDB))
  if(channelFilterPerNode(txNodeIndx).isLoaded())
  {
    if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_JAKES_)
      randVec=rndSeed(rxNodeIndx)(txNodeIndx);
    bool isChannelAvailable=false;
    if(quasiStaticChannelInfo.isEnabled)
      isChannelAvailable=quasiStaticChannelInfo.isChannelAvailable(txNodeIndx,rxNodeIndx,currentTimeInSeconds);

	if(txAntennaCorrelationMatrix.size()==0  && rxAntennaCorrelationMatrix.size()==0)
	{
	  for(int txAntenna_cnt=0, seed_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	  {
	    for(int rxAntennaIndx=0;rxAntennaIndx<rxAntennaCount;rxAntennaIndx++,seed_cnt++)
	    {
	      int rxAntenna_cnt=rxAntenna(rxAntennaIndx);
	      if(addMultiPathFading)
	      {
		if(quasiStaticChannelInfo.isEnabled)
		{
		  if(isChannelAvailable)
		  {
		    quasiStaticChannelInfo.getOldChannel(txNodeIndx,rxNodeIndx,txAntenna_cnt,rxAntenna_cnt,currentTimeInSeconds,dataSize, channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx));
		  }
		  else
		  {
                    if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_ || channelGenerationMethod==_CHANNEL_GENERATION_METHOD_MOD_JAKES_ || channelGenerationMethod==_CHANNEL_GENERATION_METHOD_CDL_)
		    {
		      channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=generateFadingChannel(txNodeID,rxNodeID,currentTimeInSeconds,printChannel,txAntenna_cnt,rxAntenna_cnt);
		    }
		    else
		    {
		      channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=generateFadingChannel(txNodeID,rxNodeID,currentTimeInSeconds,printChannel,txAntenna_cnt,rxAntenna_cnt,randVec.get(seed_cnt));
		    }
		  }
		  channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)*=sqrt(inv_dB(channelGainInDB));
		}
		else
		{
		  
                  if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_ || channelGenerationMethod==_CHANNEL_GENERATION_METHOD_MOD_JAKES_ || channelGenerationMethod==_CHANNEL_GENERATION_METHOD_CDL_)
		  {
		    channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=generateFadingChannel(txNodeID,rxNodeID,currentTimeInSeconds,printChannel,txAntenna_cnt,rxAntenna_cnt);
		  }
		  else
		  {
		    channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=generateFadingChannel(txNodeID,rxNodeID,currentTimeInSeconds,printChannel,txAntenna_cnt,rxAntenna_cnt,randVec.get(seed_cnt));
		  }
		  channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)*=sqrt(inv_dB(channelGainInDB));
		}
		// Data will be Retrieved only from the Loaded TXRUs i.e. which are associated to any User ELSE ignored if not associated to any user.
		// Needed when one or more beams are not associated to any user - Added by Goraknath.
            if(channelFilterPerNode(txNodeIndx).isLoaded(txAntenna_cnt))  
            {
                tempData=channelFilterPerNode(txNodeIndx).applyChannel(channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx),txAntenna_cnt,ofdmSymbolIndx,currentTimeInSeconds,endTimeInSeconds);
                channelOutput.data(rxAntennaIndx)+=tempData;
                if(computeSumPower)	channelOutput.summedUpSignalPerAntennaPerTone(rxAntennaIndx)+=tempData;
            }
        }
	      else
	      {
// 		if(txAntenna_cnt == rxAntenna_cnt)
		{
		  if(addPathDelay)
		  {
		    cvec delaySamples=zeros_c(floor_i(instantPathDelayInSec(rxNodeIndx,txNodeIndx)*samplingFreqToUse));
		    channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=concat(delaySamples,to_cvec(1,0));
		    if(timeDomain==0)
		    {
		      channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=fft(channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx),dataSize);
		      channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=fftShift(channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx));
		    }
		  }
		  else
		  {
		 
		    if(timeDomain)
		      channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=to_cvec(1,0);
		    else
		      channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=ones_c(dataSize);
		  }
		}
// 		else
// 		{
// 		  if(timeDomain)
// 		    channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=to_cvec(0,0);
// 		  else
// 		    channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=zeros_c(dataSize);
// 		}
        channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)*=sqrt(inv_dB(channelGainInDB));
        // Data will be Retrieved only from the Loaded TXRUs i.e. which are associated to any User ELSE ignored if not associated to any user.
        // Needed when one or more beams are not associated to any user - Added by Goraknath.
        if(channelFilterPerNode(txNodeIndx).isLoaded(txAntenna_cnt)){
		cvec rxData=channelFilterPerNode(txNodeIndx).applyChannel(channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx),txAntenna_cnt,ofdmSymbolIndx,currentTimeInSeconds,endTimeInSeconds);
		channelOutput.data(rxAntennaIndx)+=rxData;
		if(computeSumPower)	channelOutput.summedUpSignalPerAntennaPerTone(rxAntennaIndx)+=rxData;
	      }}
	    }
	  }
	}
	else
	{
	  if(txAntennaCorrelationMatrix.size()==0)
		txAntennaCorrelationMatrix=eye_c(txAntennaCount);
	  else if(rxAntennaCorrelationMatrix.size()==0)
		rxAntennaCorrelationMatrix=eye_c(rxAntennaCount);

	  Array< Array< cvec > > channel;
	  cmat instantChannel;
	  instantChannel.set_size(rxAntennaCount,txAntennaCount);

	  if(quasiStaticChannelInfo.isEnabled)
	  {
	    if(isChannelAvailable)
	      channel=quasiStaticChannelInfo.getOldChannel(txNodeIndx,rxNodeIndx,currentTimeInSeconds);
	    else
	    {
	      channel.set_size(txAntennaCount);for(int d1=0;d1<txAntennaCount;d1++)channel(d1).set_size(rxAntennaCount);
	      for(int txAntenna_cnt=0, seed_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
		for(int rxAntennaIndx=0;rxAntennaIndx<rxAntennaCount;rxAntennaIndx++,seed_cnt++)
		{
		  int rxAntenna_cnt=rxAntenna(rxAntennaIndx);
		  
                  if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
		  {
		    channel(txAntenna_cnt)(rxAntennaIndx)=generateFadingChannel(nodesInfo.nodeList(txNodeIndx),rxNodeID,currentTimeInSeconds,printChannel,txAntenna_cnt,rxAntenna_cnt);
		  }
		  else
		  {
		    channel(txAntenna_cnt)(rxAntennaIndx)=generateFadingChannel(nodesInfo.nodeList(txNodeIndx),rxNodeID,currentTimeInSeconds,printChannel,txAntenna_cnt,rxAntenna_cnt,randVec.get(seed_cnt));
		  }
		}
	    }
	  }
	  else
	  {
	    channel.set_size(txAntennaCount);for(int d1=0;d1<txAntennaCount;d1++)channel(d1).set_size(rxAntennaCount);
	    for(int txAntenna_cnt=0, seed_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	      for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++,seed_cnt++)
                if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
		{
		  channel(txAntenna_cnt)(rxAntenna_cnt)=generateFadingChannel(nodesInfo.nodeList(txNodeIndx),rxNodeID,currentTimeInSeconds,printChannel,txAntenna_cnt,rxAntenna_cnt);
		}
		else
		{
		  channel(txAntenna_cnt)(rxAntenna_cnt)=generateFadingChannel(nodesInfo.nodeList(txNodeIndx),rxNodeID,currentTimeInSeconds,printChannel,txAntenna_cnt,rxAntenna_cnt,randVec.get(seed_cnt));
		}

	  }


	  for(int channelSample_cnt=0;channelSample_cnt<channel(0)(0).length();channelSample_cnt++)
	  {
		for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
		  for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
			instantChannel(rxAntenna_cnt,txAntenna_cnt)=channel(txAntenna_cnt)(rxAntenna_cnt).get(channelSample_cnt);

                instantChannel = sqrtm(rxAntennaCorrelationMatrix)*instantChannel*transpose(sqrtm(txAntennaCorrelationMatrix));


		for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
		  for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
			channel(txAntenna_cnt)(rxAntenna_cnt).set(channelSample_cnt,instantChannel(rxAntenna_cnt,txAntenna_cnt));
	  }

	  for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	  {
		for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
		{
		  channelOutput.channels(0)(txAntenna_cnt)(rxAntenna_cnt)=channel(txAntenna_cnt)(rxAntenna_cnt);
		  channelOutput.channels(0)(txAntenna_cnt)(rxAntenna_cnt)*=sqrt(inv_dB(channelGainInDB));
          // Data will be Retrieved only from the Loaded TXRUs i.e. which are associated to any User ELSE ignored if not associated to any user.
          // Needed when one or more beams are not associated to any user - Added by Goraknath.
          if(channelFilterPerNode(txNodeIndx).isLoaded(txAntenna_cnt)){
		  tempData=channelFilterPerNode(txNodeIndx).applyChannel(channelOutput.channels(0)(txAntenna_cnt)(rxAntenna_cnt),txAntenna_cnt,ofdmSymbolIndx,currentTimeInSeconds,endTimeInSeconds);
		  channelOutput.data(rxAntenna_cnt)+=tempData;
		  if(computeSumPower)	channelOutput.summedUpSignalPerAntennaPerTone(rxAntenna_cnt)+=tempData;
		  }
        }
      }
	}
  }

  return(channelOutput);
}

ChannelOutput_S ChannelCloud::generateChannel(int rxNodeID, int rxBeamID, AntennaCorrelationType_E antCorrelationType, double currentTimeInSeconds, int fftSize, ivec txNodeList){

  ChannelOutput_S channelOutput,tempChannelOutput;
  
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[both:]Error. No linkBudget info for Rx node "<<rxNodeID<<" available in the channel."<<endl;
    abort();
  }

  if(txNodeList.length()==0)
    txNodeList=currentTxNodes;
  else
    if(find(find(currentTxNodes,txNodeList),-1)!=-1)
    {
      cout<<"[both:] Error .. TxNodeList to receiveData() contains nodes which are not in Tx Mode .. "<<endl;
      abort();
    }

  int rxAntennaCount,txAntennaCount;
  rxAntennaCount=nodesInfo.getTXRUCount(rxNodeID,rxBeamID);

  channelOutput.rxNodeID=rxNodeID;
  channelOutput.txNodeIDs=txNodeList;
  channelOutput.channels.set_size(txNodeList.length());

  double channelGainInDB;
  ivec myTxNodeIndices=find(nodesInfo.nodeList,txNodeList);
  for(int txNode_cnt=0;txNode_cnt<myTxNodeIndices.length();txNode_cnt++)
  {
//    channelGainInDB=getLinkGainInDB(rxNodeIndx,myTxNodeIndices(txNode_cnt));
    channelGainInDB=linkGainInDB(rxNodeIndx,myTxNodeIndices(txNode_cnt));
    if(!std::isnan(channelGainInDB))
    {
      txAntennaCount=nodesInfo.antennaCount(myTxNodeIndices(txNode_cnt));
      if(antCorrelationType!=_ANTENNA_CORRELATION_UNCORRELATED_)
      {
	if(CorrMatrix.isInitialized)
	{
	  Array<cmat> antennaCorrelationMatrices=getAntennaCorrelationMatrices(txAntennaCount,rxAntennaCount);        
	  tempChannelOutput=generateChannel(rxNodeID,rxBeamID,nodesInfo.nodeList(myTxNodeIndices(txNode_cnt)),currentTimeInSeconds,fftSize,antennaCorrelationMatrices(0),antennaCorrelationMatrices(1));
	}
	else
	  tempChannelOutput=generateChannel(rxNodeID,rxBeamID,nodesInfo.nodeList(myTxNodeIndices(txNode_cnt)),currentTimeInSeconds,fftSize);

      }
      else
	tempChannelOutput=generateChannel(rxNodeID,rxBeamID,nodesInfo.nodeList(myTxNodeIndices(txNode_cnt)),currentTimeInSeconds,fftSize);
      channelOutput.channels(txNode_cnt)=tempChannelOutput.channels(0);
    }
  }
  return(channelOutput);
}


ChannelOutput_S ChannelCloud::generateChannel(int rxNodeID, int rxBeamID, int txNodeID, double currentTimeInSeconds, int fftSize, cmat txAntennaCorrelationMatrix, cmat rxAntennaCorrelationMatrix){

  ChannelOutput_S channelOutput;
 
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[both:]Error. No linkBudget info for Rx node "<<rxNodeID<<" available in the channel."<<endl;
    abort();
  }

  int txNodeIndx=find(nodesInfo.nodeList,txNodeID);
  int txAntennaCount;
  int rxAntennaCount;

  ivec rxAntenna=nodesInfo.getTXRUs(rxNodeID,rxBeamID);
  rxAntennaCount=rxAntenna.length();
  if(rxAntennaCount==-1)
  {
    cout<<"[both:]Error. No antenna count for Rx node "<<rxNodeID<<" available in the channel."<<endl;
    abort();
  }


  if(txNodeIndx==-1)
  {
    cout<<"[both:]Error. No linkBudget info for Tx node "<<txNodeID<<" available in the channel."<<endl;
    abort();
  }
  txAntennaCount=nodesInfo.antennaCount(txNodeIndx);
  if(txAntennaCount==-1)
  {
    cout<<"[both:]Error. No antenna count for Tx node "<<txNodeID<<" available in the channel."<<endl;
    abort();
  }

  if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_MOD_JAKES_)
    if(!modifiedJakesInfo.isLinked(rxNodeID,txNodeID))
      generateTapWeightsForModifiedJakes(rxNodeID,to_ivec(txNodeID),currentTimeInSeconds);
    
  cvec tempData;
  channelOutput.channels.set_size(1);
  channelOutput.channels(0).set_size(txAntennaCount);
  for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
    channelOutput.channels(0)(txAntenna_cnt).set_size(rxAntennaCount);
 
  Vec<unsigned int> randVec;
  double channelGainInDB=getChannelGain(rxNodeID,txNodeID);
  if(!std::isnan(channelGainInDB))
    {
      //cout<<"scmLinks(scmLinkIndx(rxNodeIndx,txNodeIndx)).getLOSAntennaGainInDB() : "<<scmLinks(scmLinkIndx(rxNodeIndx,txNodeIndx)).getLOSAntennaGainInDB()<<endl;abort();
  
      randVec=rndSeed(rxNodeIndx)(txNodeIndx);

      bool isChannelAvailable=false;
      if(quasiStaticChannelInfo.isEnabled)
	isChannelAvailable=quasiStaticChannelInfo.isChannelAvailable(txNodeIndx,rxNodeIndx,currentTimeInSeconds);

      if(txAntennaCorrelationMatrix.size()==0  && rxAntennaCorrelationMatrix.size()==0)
      {
	for(int txAntenna_cnt=0, seed_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	{
	  for(int rxAntennaIndx=0;rxAntennaIndx<rxAntennaCount;rxAntennaIndx++,seed_cnt++)
	  {
	    int rxAntenna_cnt=rxAntenna(rxAntennaIndx);
	    if(addMultiPathFading)
	    {
	      if(quasiStaticChannelInfo.isEnabled)
	      {
		if(isChannelAvailable)
		{
		  quasiStaticChannelInfo.getOldChannel(txNodeIndx,rxNodeIndx,txAntenna_cnt,rxAntenna_cnt,currentTimeInSeconds,fftSize, channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx));
		}
		else
		{
		  if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
		  {
		    channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=generateFadingChannel(txNodeID,rxNodeID,currentTimeInSeconds,false,txAntenna_cnt,rxAntenna_cnt,0,fftSize);
		  }
		  else
		  {
		    channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=generateFadingChannel(txNodeID,rxNodeID,currentTimeInSeconds,false,txAntenna_cnt,rxAntenna_cnt,randVec.get(seed_cnt),fftSize);
		   
		  }

		}
		channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)*=sqrt(inv_dB(channelGainInDB));
// 		channelOutput.channels_forRSRP(0)(txAntenna_cnt)(rxAntennaIndx)*=sqrt(inv_dB(channelGainInDB));
	      }
	      else
	      {
		if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
		  channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=generateFadingChannel(txNodeID,rxNodeID,currentTimeInSeconds,false,txAntenna_cnt,rxAntenna_cnt,0,fftSize);
		else
		  channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=generateFadingChannel(txNodeID,rxNodeID,currentTimeInSeconds,false,txAntenna_cnt,rxAntenna_cnt,randVec.get(seed_cnt),fftSize);

		channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)*=sqrt(inv_dB(channelGainInDB));
// 		channelOutput.channels_forRSRP(0)(txAntenna_cnt)(rxAntennaIndx)*=sqrt(inv_dB(channelGainInDB));
	      }
	    }
	    else
	    {
	      if(txAntenna_cnt == rxAntenna_cnt)
	      {
		if(timeDomain==1)
		  channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=to_cvec(1,0);
		else
		  channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=ones_c(fftSize);
	      }
	      else
	      {
		if(timeDomain==1)
		  channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=to_cvec(0,0);
		else
		  channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=zeros_c(fftSize);
	      }
	      channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)*=sqrt(inv_dB(channelGainInDB));
	    }
	  }
	}
      }
      else
      {
	if(txAntennaCorrelationMatrix.size()==0)
	  txAntennaCorrelationMatrix=eye_c(txAntennaCount);
	else if(rxAntennaCorrelationMatrix.size()==0)
	  rxAntennaCorrelationMatrix=eye_c(rxAntennaCount);

	Array< Array< cvec > > channel;
	
	cmat instantChannel;
	instantChannel.set_size(rxAntennaCount,txAntennaCount);

	if(quasiStaticChannelInfo.isEnabled)
	{
	  if(isChannelAvailable)
	    channel=quasiStaticChannelInfo.getOldChannel(txNodeIndx,rxNodeIndx,currentTimeInSeconds);
	  else
	  {
	    channel.set_size(txAntennaCount);for(int d1=0;d1<txAntennaCount;d1++)channel(d1).set_size(rxAntennaCount);
	    for(int txAntenna_cnt=0, seed_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	      for(int rxAntennaIndx=0;rxAntennaIndx<rxAntennaCount;rxAntennaIndx++,seed_cnt++)
	      {
		int rxAntenna_cnt=rxAntenna(rxAntennaIndx);
		if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
		  channel(txAntenna_cnt)(rxAntennaIndx)=generateFadingChannel(nodesInfo.nodeList(txNodeIndx),rxNodeID,currentTimeInSeconds,false,txAntenna_cnt,rxAntenna_cnt,0,fftSize);
		else
		  channel(txAntenna_cnt)(rxAntennaIndx)=generateFadingChannel(nodesInfo.nodeList(txNodeIndx),rxNodeID,currentTimeInSeconds,false,txAntenna_cnt,rxAntenna_cnt,randVec.get(seed_cnt),fftSize);
	      }
	  }
	}
	else
	{
	  channel.set_size(txAntennaCount);for(int d1=0;d1<txAntennaCount;d1++)channel(d1).set_size(rxAntennaCount);
	  for(int txAntenna_cnt=0, seed_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	    for(int rxAntennaIndx=0;rxAntennaIndx<rxAntennaCount;rxAntennaIndx++,seed_cnt++)
	    {
	      int rxAntenna_cnt=rxAntenna(rxAntennaIndx);
	      if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
		channel(txAntenna_cnt)(rxAntennaIndx)=generateFadingChannel(nodesInfo.nodeList(txNodeIndx),rxNodeID,currentTimeInSeconds,false,txAntenna_cnt,rxAntenna_cnt,0,fftSize);
	      else
		channel(txAntenna_cnt)(rxAntennaIndx)=generateFadingChannel(nodesInfo.nodeList(txNodeIndx),rxNodeID,currentTimeInSeconds,false,txAntenna_cnt,rxAntenna_cnt,randVec.get(seed_cnt),fftSize);
	    }
	}
	for(int channelSample_cnt=0;channelSample_cnt<channel(0)(0).length();channelSample_cnt++)
	{
	  for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	    for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
	      instantChannel(rxAntenna_cnt,txAntenna_cnt)=channel(txAntenna_cnt)(rxAntenna_cnt).get(channelSample_cnt);

	  instantChannel = sqrtm(rxAntennaCorrelationMatrix)*instantChannel*transpose(sqrtm(txAntennaCorrelationMatrix));

	  for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	    for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
	      channel(txAntenna_cnt)(rxAntenna_cnt).set(channelSample_cnt,instantChannel(rxAntenna_cnt,txAntenna_cnt));
	}

	for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	  for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
	  {
	    channelOutput.channels(0)(txAntenna_cnt)(rxAntenna_cnt)=channel(rxAntenna_cnt)(txAntenna_cnt);
	    channelOutput.channels(0)(txAntenna_cnt)(rxAntenna_cnt)*=sqrt(inv_dB(channelGainInDB));
	    channelOutput.channels_forRSRP(0)(txAntenna_cnt)(rxAntenna_cnt)*=sqrt(inv_dB(channelGainInDB));

	  }
	
      }
    }

    return(channelOutput);
}

ChannelMatrix_S ChannelCloud::generateChannel(int rxNodeID, int rxBeamID, AntennaCorrelationType_E antCorrelationType, vec currentTimeInSeconds, int fftSize, ivec txNodeList){
  
  ChannelMatrix_S channelMatrix,tempChannelOutput;

  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[both:]Error. No linkBudget info for Rx node "<<rxNodeID<<" available in the channel."<<endl;
    abort();
  }
  

  if(txNodeList.length()==0)
    txNodeList=currentTxNodes;
  else
    if(find(find(currentTxNodes,txNodeList),-1)!=-1)
    {
      cout<<"[both:] Error .. TxNodeList to receiveData() contains nodes which are not in Tx Mode .. "<<endl;
      abort();
    }
    
    int rxAntennaCount,txAntennaCount;
  rxAntennaCount=nodesInfo.getTXRUCount(rxNodeID,rxBeamID);
  
  channelMatrix.rxNodeID=rxNodeID;
  channelMatrix.txNodeIDs=txNodeList;
  channelMatrix.timeStamp=currentTimeInSeconds;
  channelMatrix.channels.set_size(txNodeList.length());
  
  //   if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_MOD_JAKES_)
  //     generateTapWeightsForModifiedJakes(rxNodeID,txNodeList,currentTimeInSeconds);
  
  double channelGainInDB;
  ivec myTxNodeIndices=find(nodesInfo.nodeList,txNodeList);
  for(int txNode_cnt=0;txNode_cnt<myTxNodeIndices.length();txNode_cnt++)
  {
//    channelGainInDB=getLinkGainInDB(rxNodeIndx,myTxNodeIndices(txNode_cnt));
      channelGainInDB=linkGainInDB(rxNodeIndx,myTxNodeIndices(txNode_cnt));
    if(!std::isnan(channelGainInDB))
    {
      txAntennaCount=nodesInfo.antennaCount(myTxNodeIndices(txNode_cnt));
      if(antCorrelationType!=_ANTENNA_CORRELATION_UNCORRELATED_)
      {
	if(CorrMatrix.isInitialized)
	{
	  Array<cmat> antennaCorrelationMatrices=getAntennaCorrelationMatrices(txAntennaCount,rxAntennaCount);        
	  tempChannelOutput=generateChannel(rxNodeID,rxBeamID,nodesInfo.nodeList(myTxNodeIndices(txNode_cnt)),currentTimeInSeconds,fftSize,antennaCorrelationMatrices(0),antennaCorrelationMatrices(1));
	}
	else
	  tempChannelOutput=generateChannel(rxNodeID,rxBeamID,nodesInfo.nodeList(myTxNodeIndices(txNode_cnt)),currentTimeInSeconds,fftSize);
	
      }
      else
	tempChannelOutput=generateChannel(rxNodeID,rxBeamID,nodesInfo.nodeList(myTxNodeIndices(txNode_cnt)),currentTimeInSeconds,fftSize);
      channelMatrix.channels(txNode_cnt)=tempChannelOutput.channels(0);
    }
    
  }
  return(channelMatrix);
}

ChannelMatrix_S ChannelCloud::generateChannelForAnalogBeams(int rxNodeID, ivec txNodeList, ivec& nTxBeamsPerTxNode, int& nRxBeams, double currentTimeInSec, bool isChannelGenForRSRP){
  
  ChannelMatrix_S channelMatrix,tempChannelOutput;


  if(txNodeList.length()==0)
    txNodeList=currentTxNodes;
  else
    if(find(find(currentTxNodes,txNodeList),-1)!=-1)
    {
      cout<<"[both:] Error.. TxNodeList to generateChannelForAnalogBeams() contains nodes which are not in Tx Mode .. "<<endl;
      abort();
    }
    
  channelMatrix.rxNodeID=rxNodeID;

  ivec myTxNodeIndices=find(nodesInfo.nodeList,txNodeList);
  //cout << "myTxNodeIndices : "<< myTxNodeIndices <<endl;
  //abort();
  int nTotalAnalogBeams=0;
  
  nTxBeamsPerTxNode = -1*ones_i(txNodeList.length());
  nRxBeams = -1;
    
  for(int txNode_cnt=0;txNode_cnt<myTxNodeIndices.length();txNode_cnt++)
  {
    int txNodeIndx=find(nodesInfo.nodeList,nodesInfo.nodeList(myTxNodeIndices(txNode_cnt)));
    if(txNodeIndx==-1)
    {
      cout<<"[both:]Error. No linkBudget info for Tx node "<<nodesInfo.nodeList(myTxNodeIndices(txNode_cnt))<<" available in the channel."<<endl;
      abort();
    }
    nTxBeamsPerTxNode(txNode_cnt) = nodesInfo.getNumberOfBeams(txNodeList(txNode_cnt));
    //cout << "nTxBeamsPerTxNode(txNode_cnt) : " << nTxBeamsPerTxNode(txNode_cnt) <<endl;
    
  }
  nTotalAnalogBeams = sum(nTxBeamsPerTxNode);
  //cout << "nTotalAnalogBeams : " << nTotalAnalogBeams <<endl;
  //abort();
  
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[both:]Error. No linkBudget info for Rx node "<<rxNodeID<<" available in the channel."<<endl;
    abort();
  }
  nRxBeams = nodesInfo.getNumberOfBeams(rxNodeID);
  //cout << "nRxBeams : " << nRxBeams <<endl;
  nTotalAnalogBeams *= nRxBeams; 
  //cout << "nTotalAnalogBeams : " << nTotalAnalogBeams <<endl;
  //abort();
  channelMatrix.txNodeIDs.set_size(nTotalAnalogBeams);
  channelMatrix.channels_forRSRP.set_size(nTotalAnalogBeams);
  
  
  for(int txNode_cnt=0, totalCnt=0;txNode_cnt<myTxNodeIndices.length();txNode_cnt++)
  {
      tempChannelOutput=generateChannelForAnalogBeams(rxNodeID,nodesInfo.nodeList(myTxNodeIndices(txNode_cnt)),currentTimeInSec,isChannelGenForRSRP);
    //cout << "tempChannelOutput : " << tempChannelOutput <<endl;
    for(int beam_cnt=0;beam_cnt<tempChannelOutput.txNodeIDs.length();beam_cnt++,totalCnt++)
    {
      channelMatrix.txNodeIDs(totalCnt) = tempChannelOutput.txNodeIDs(beam_cnt);
      channelMatrix.channels_forRSRP(totalCnt) = tempChannelOutput.channels_forRSRP(beam_cnt);
    }
  }
  //cout << "channelMatrix : " << channelMatrix <<endl;
  //abort();
  return(channelMatrix);
}

ChannelMatrix_S ChannelCloud::generateChannelForAnalogBeams(int rxNodeID, int txNodeID, double currentTimeInSec, bool isChannelGenForRSRP){
  
  ChannelMatrix_S channelMatrix;
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[both:]Error. No linkBudget info for Rx node "<<rxNodeID<<" available in the channel."<<endl;
    abort();
  }
  
  int txNodeIndx=find(nodesInfo.nodeList,txNodeID);
  if(txNodeIndx==-1)
  {
    cout<<"[both:]Error. No linkBudget info for Tx node "<<txNodeID<<" available in the channel."<<endl;
    abort();
  }
  
  sBeamParameters rxBeamParameters = nodesInfo.getBeamParameters(rxNodeID);
  //cout << "rxBeamParameters : "<< rxBeamParameters<<endl;
  int rxAnalogBeams = rxBeamParameters.nTotalBeams;
  //cout << "rxAnalogBeams : "<< rxAnalogBeams <<endl;
  //abort();
  sBeamParameters txBeamParameters = nodesInfo.getBeamParameters(txNodeID);
  int txAnalogBeams = txBeamParameters.nTotalBeams;
  //cout << "txAnalogBeams : "<< txAnalogBeams <<endl;
  //abort();
  ivec defaultTxRUsPerTxBeam = txBeamParameters.txRUPerBeamForAnalogBeamSweeping;
  Array<ivec> txRUsPerRxBeam = rxBeamParameters.txRUsPerBeam;
  
  int nAnalogBeamCombinations = txAnalogBeams*rxAnalogBeams;
  //cout<<"nAnalogBeamCombinations"<<nAnalogBeamCombinations<<endl;
  if( nAnalogBeamCombinations <= 0)
  {
    cout<<"[both:]Error. No analogBeamCount for Tx node "<<txNodeID<<" available in the channel."<<endl;
    abort();
  }
  
  channelMatrix.txNodeIDs.set_length( nAnalogBeamCombinations );
  channelMatrix.rxNodeID = rxNodeID;
  channelMatrix.channels_forRSRP.set_size( nAnalogBeamCombinations );
  
  int antennaCountForTxBeam = 1;//NOTE: using single port for initial beam sweeping
  
  double samplingFreqToUse;
  if(nodesInfo.samplingFrequency.length()!=0)
      samplingFreqToUse=nodesInfo.samplingFrequency(rxNodeIndx); //using rxNodeID sampling freqneucy to decide output size
  else
      samplingFreqToUse=commonSamplingFrequency;
  
  //cout << "samplingFreqToUse : " << samplingFreqToUse <<endl;
  //abort();
      ChannelInfo_S channelInfo;
  
  int chInfoIndx=channelInfoIndx(rxNodeIndx,txNodeIndx);
  //cout << "chInfoIndx : " << chInfoIndx <<endl;
  //abort();
  if(chInfoIndx!=-1)
      channelInfo=linkSpecificChannelInfo(chInfoIndx);

  else
      channelInfo=sysWideChannelInfo;

  
  channelInfo.fadingParameters.tapDel=scmLinks(scmLinkIndx(rxNodeIndx,txNodeIndx)).getTapDelays();
  channelInfo.fadingParameters.nTaps=channelInfo.fadingParameters.tapDel.length();
  //cout << "channelInfo.fadingParameters.tapDel : " << channelInfo.fadingParameters.tapDel <<endl;
  //cout<<"channelInfo.fadingParameters.nTaps :"<<channelInfo.fadingParameters.nTaps<<endl;
  //abort();
  double channelGainInDB=getChannelGain(rxNodeID,txNodeID);
  //cout << "channelGainInDB : " << channelGainInDB <<endl;
  //cout << "txAnalogBeams : " << txAnalogBeams <<endl;
  for(int txBeam_cnt=0, beam_cnt = 0;txBeam_cnt<txAnalogBeams;txBeam_cnt++)
  {
      for(int rxBeam_cnt=0;rxBeam_cnt<rxAnalogBeams;rxBeam_cnt++,beam_cnt++)
      {
        channelMatrix.channels_forRSRP(beam_cnt).set_size(antennaCountForTxBeam);
        int antennaCountPerRxBeam = txRUsPerRxBeam(rxBeam_cnt).length();
        channelMatrix.txNodeIDs(beam_cnt) = getVectorWithIntegers(3,txNodeID,txBeam_cnt,rxBeam_cnt);
        
        for(int txAntenna_cnt=0;txAntenna_cnt<antennaCountForTxBeam;txAntenna_cnt++)
        { //cout<<"antennaCountForTxBeam: "<<antennaCountForTxBeam<<endl;
            channelMatrix.channels_forRSRP(beam_cnt)(txAntenna_cnt).set_size(antennaCountPerRxBeam);
            for(int rxAntenna_cnt=0;rxAntenna_cnt<antennaCountPerRxBeam;rxAntenna_cnt++)
            {
                cvec tapWeights = generate5GAASChannelTapGains(txNodeID,rxNodeID,defaultTxRUsPerTxBeam(txAntenna_cnt),txRUsPerRxBeam(rxBeam_cnt)(rxAntenna_cnt),currentTimeInSec,isChannelGenForRSRP,txBeam_cnt,rxBeam_cnt);
                //cout<<"txNodeID : "<<txNodeID <<endl;
                //cout<<"rxNodeID : "<<rxNodeID <<endl;
                //cout<<"defaultTxRUsPerTxBeam(txAntenna_cnt) : "<<defaultTxRUsPerTxBeam(txAntenna_cnt) <<endl;
                //cout<<"txRUsPerRxBeam(rxBeam_cnt)(rxAntenna_cnt) : "<<txRUsPerRxBeam(rxBeam_cnt)(rxAntenna_cnt) <<endl;
                //cout<<"txBeam_cnt : "<<txBeam_cnt <<endl;
                //cout<<"rxBeam_cnt : "<<rxBeam_cnt <<endl;
                //cout<<"rxBeam_cnt : "<<rxBeam_cnt <<endl;
                //cout<<"rxAntenna_cnt : "<< rxAntenna_cnt <<endl;

                //cvec tapWeights = generate5GAASChannelTapGains(1,62,defaultTxRUsPerTxBeam(txAntenna_cnt),txRUsPerRxBeam(rxBeam_cnt)(rxAntenna_cnt),0.0,true,txBeam_cnt,rxBeam_cnt);
                //cout << "tapWeights : " << tapWeights <<endl;
                //abort(); //remove abort santhosh
                cvec sampleSpacedChannel=zeros_c(floor_i(channelInfo.fadingParameters.tapDel(channelInfo.fadingParameters.tapDel.length()-1)*samplingFreqToUse)+1);
                //cout << "sampleSpacedChannelinitial : " << sampleSpacedChannel <<endl;
                //sampleSpacedChannel.clear();
                int channelIndx=0;
                //cout<<"channelInfo.fadingParameters.nTaps :"<<channelInfo.fadingParameters.nTaps<<endl;
                for(int tap_cnt=0;tap_cnt< channelInfo.fadingParameters.nTaps;tap_cnt++)

                {
                    channelIndx=floor_i(channelInfo.fadingParameters.tapDel(tap_cnt)*samplingFreqToUse);
                    //cout << "channelIndx : " << channelIndx <<endl;
                    if(channelIndx>=sampleSpacedChannel.length())
                    {
                        cout<<"[both:] Insufficient Channel length to represent the Channel Model at the given sampling Frequency."<<endl;
                        abort();
                    }

                    sampleSpacedChannel(channelIndx) = sampleSpacedChannel(channelIndx) +tapWeights(tap_cnt);


                }
                //cout << "sampleSpacedChannel angle value : " << angle(sampleSpacedChannel) <<endl;
                //abort();
                channelMatrix.channels_forRSRP(beam_cnt)(txAntenna_cnt)(rxAntenna_cnt)= sampleSpacedChannel*sqrt(inv_dB(channelGainInDB));
                //cout<<"sampleSpacedChannel*sqrt"<<":"<<sampleSpacedChannel*sqrt(inv_dB(channelGainInDB))<<endl;
                gain<<abs(sampleSpacedChannel*sqrt(inv_dB(channelGainInDB)))<<endl;
            }
        }
      }
  }
  //cout << "Check abort :"<<endl;
  //abort(); // remove abort santhosh
  return(channelMatrix);
}

ChannelMatrix_S ChannelCloud::generateChannel(int rxNodeID, int rxBeamID, int txNodeID, vec currentTimeInSeconds, int fftSize, cmat txAntennaCorrelationMatrix, cmat rxAntennaCorrelationMatrix){
  
  ChannelMatrix_S channelMatrix;
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[both:]Error. No linkBudget info for Rx node "<<rxNodeID<<" available in the channel."<<endl;
    abort();
  }
  
  int txNodeIndx=find(nodesInfo.nodeList,txNodeID);
  int txAntennaCount;
  int rxAntennaCount;
  
  ivec rxAntenna=nodesInfo.getTXRUs(rxNodeID,rxBeamID);
  rxAntennaCount=rxAntenna.length();
  if(rxAntennaCount==-1)
  {
    cout<<"[both:]Error. No antenna count for Rx node "<<rxNodeID<<" available in the channel."<<endl;
    abort();
  }
  
  
  if(txNodeIndx==-1)
  {
    cout<<"[both:]Error. No linkBudget info for Tx node "<<txNodeID<<" available in the channel."<<endl;
    abort();
  }
  txAntennaCount=nodesInfo.antennaCount(txNodeIndx);
  if(txAntennaCount==-1)
  {
    cout<<"[both:]Error. No antenna count for Tx node "<<txNodeID<<" available in the channel."<<endl;
    abort();
  }
  
  channelMatrix.txNodeIDs=to_ivec(txNodeID);
  channelMatrix.rxNodeID=rxNodeID;
  channelMatrix.timeStamp=currentTimeInSeconds;
  
  cvec tempData;
  channelMatrix.channels.set_size(1);
  channelMatrix.channels(0).set_size(txAntennaCount);
  for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
  {
    channelMatrix.channels(0)(txAntenna_cnt).set_size(rxAntennaCount);
    if(timeDomain==0)
    {
      for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
	channelMatrix.channels(0)(txAntenna_cnt)(rxAntenna_cnt)=zeros_c(fftSize,currentTimeInSeconds.length());
    }
    else
    {
      cout<<"Error. Time-domain support not available in generateChannel()."<<endl;
      abort();
    }
  }
  
  Vec<unsigned int> randVec;
  double channelGainInDB=getChannelGain(rxNodeID,txNodeID);
  if(!std::isnan(channelGainInDB))
  {
    randVec=rndSeed(rxNodeIndx)(txNodeIndx);
    
    if(txAntennaCorrelationMatrix.size()==0  && rxAntennaCorrelationMatrix.size()==0)
    {
      for(int time_cnt=0;time_cnt<currentTimeInSeconds.length();time_cnt++)
      {
	if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_MOD_JAKES_)
	  if(!modifiedJakesInfo.isLinked(rxNodeID,txNodeID))
	    generateTapWeightsForModifiedJakes(rxNodeID,to_ivec(txNodeID),currentTimeInSeconds(time_cnt));
	
	bool isChannelAvailable=false;
	if(quasiStaticChannelInfo.isEnabled)
	  isChannelAvailable=quasiStaticChannelInfo.isChannelAvailable(txNodeIndx,rxNodeIndx,currentTimeInSeconds(time_cnt));  
	  
	for(int txAntenna_cnt=0, seed_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	{
	  for(int rxAntennaIndx=0;rxAntennaIndx<rxAntennaCount;rxAntennaIndx++,seed_cnt++)
	  {
	    int rxAntenna_cnt=rxAntenna(rxAntennaIndx);
	    
	    if(addMultiPathFading)
	    {
	      if(quasiStaticChannelInfo.isEnabled)
	      {
		if(isChannelAvailable)
		  {
		    channelMatrix.channels(0)(txAntenna_cnt)(rxAntennaIndx).set_col(time_cnt,quasiStaticChannelInfo.getOldChannel(txNodeIndx,rxNodeIndx,txAntenna_cnt,rxAntenna_cnt,currentTimeInSeconds(time_cnt),fftSize));
		  }
		  else
		  {
		    if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
                      channelMatrix.channels(0)(txAntenna_cnt)(rxAntennaIndx).set_col(time_cnt,generateFadingChannel(txNodeID,rxNodeID,currentTimeInSeconds(time_cnt),false,txAntenna_cnt,rxAntenna_cnt,0,fftSize));
		    else
		       channelMatrix.channels(0)(txAntenna_cnt)(rxAntennaIndx).set_col(time_cnt,generateFadingChannel(txNodeID,rxNodeID,currentTimeInSeconds(time_cnt),false,txAntenna_cnt,rxAntenna_cnt,randVec.get(seed_cnt),fftSize));
		  }
	      }
	      else
	      {
		if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
                    channelMatrix.channels(0)(txAntenna_cnt)(rxAntennaIndx).set_col(time_cnt,generateFadingChannel(txNodeID,rxNodeID,currentTimeInSeconds(time_cnt),false,txAntenna_cnt,rxAntenna_cnt,0,fftSize));
		else
                    channelMatrix.channels(0)(txAntenna_cnt)(rxAntennaIndx).set_col(time_cnt,generateFadingChannel(txNodeID,rxNodeID,currentTimeInSeconds(time_cnt),false,txAntenna_cnt,rxAntenna_cnt,randVec.get(seed_cnt),fftSize));
		
// 		channelMatrix.channels(0)(txAntenna_cnt)(rxAntennaIndx)*=sqrt(inv_dB(channelGainInDB));
	      }
	    }
	    else
	    {
	      if(txAntenna_cnt == rxAntenna_cnt)
	      {
		// 	    if(timeDomain==1)
		// 	      channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=to_cvec(1,0);
		// 	    else
		channelMatrix.channels(0)(txAntenna_cnt)(rxAntennaIndx).set_col(time_cnt,ones_c(fftSize));
	      }
	      else
	      {
		// 	    if(timeDomain==1)
		// 	      channelOutput.channels(0)(txAntenna_cnt)(rxAntennaIndx)=to_cvec(0,0);
		// 	    else
		channelMatrix.channels(0)(txAntenna_cnt)(rxAntennaIndx).set_col(time_cnt,zeros_c(fftSize));
	      }
	    }
	  }
	}
      
      }
      for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
      for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
	channelMatrix.channels(0)(txAntenna_cnt)(rxAntenna_cnt)*=sqrt(inv_dB(channelGainInDB));
    }
    else
    {
      if(txAntennaCorrelationMatrix.size()==0)
	txAntennaCorrelationMatrix=eye_c(txAntennaCount);
      else if(rxAntennaCorrelationMatrix.size()==0)
	rxAntennaCorrelationMatrix=eye_c(rxAntennaCount);
      
      Array< Array< cmat > > channel;
      channel.set_size(txAntennaCount);
      for(int d1=0;d1<txAntennaCount;d1++)
      {
	channel(d1).set_size(rxAntennaCount);
	for(int d2=0;d2<rxAntennaCount;d2++)
	  channel(d1)(d2).set_size(fftSize,currentTimeInSeconds.length());
      }
      
      cmat instantChannel;
      instantChannel.set_size(rxAntennaCount,txAntennaCount);
      for(int time_cnt=0;time_cnt<currentTimeInSeconds.length();time_cnt++)
      {
	if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_MOD_JAKES_)
	  if(!modifiedJakesInfo.isLinked(rxNodeID,txNodeID))
	    generateTapWeightsForModifiedJakes(rxNodeID,to_ivec(txNodeID),currentTimeInSeconds(time_cnt));
	  
	if(quasiStaticChannelInfo.isEnabled)
	{
	  if(quasiStaticChannelInfo.isChannelAvailable(txNodeIndx,rxNodeIndx,currentTimeInSeconds(time_cnt)))
	  {
	    for(int txAntenna_cnt=0, seed_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	      for(int rxAntennaIndx=0;rxAntennaIndx<rxAntennaCount;rxAntennaIndx++,seed_cnt++)
	      {
		int rxAntenna_cnt=rxAntenna(rxAntennaIndx);
		channel(txAntenna_cnt)(rxAntennaIndx).set_col(time_cnt,quasiStaticChannelInfo.getOldChannel(txNodeIndx,rxNodeIndx,txAntenna_cnt,rxAntenna_cnt,currentTimeInSeconds(time_cnt),fftSize));
	      }
	  }
	  else
	  {
	    for(int txAntenna_cnt=0, seed_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	      for(int rxAntennaIndx=0;rxAntennaIndx<rxAntennaCount;rxAntennaIndx++,seed_cnt++)
	      {
		int rxAntenna_cnt=rxAntenna(rxAntennaIndx);
		if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
		  channel(txAntenna_cnt)(rxAntennaIndx).set_col(time_cnt,generateFadingChannel(nodesInfo.nodeList(txNodeIndx),rxNodeID,currentTimeInSeconds(time_cnt),false,txAntenna_cnt,rxAntenna_cnt,0,fftSize));
		else
		  channel(txAntenna_cnt)(rxAntennaIndx).set_col(time_cnt,generateFadingChannel(nodesInfo.nodeList(txNodeIndx),rxNodeID,currentTimeInSeconds(time_cnt),false,txAntenna_cnt,rxAntenna_cnt,randVec.get(seed_cnt),fftSize));
	      }
	  }
	}
	else
	{
	  for(int txAntenna_cnt=0, seed_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	    for(int rxAntennaIndx=0;rxAntennaIndx<rxAntennaCount;rxAntennaIndx++,seed_cnt++)
	    {
	      int rxAntenna_cnt=rxAntenna(rxAntennaIndx);
	      if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
		channel(txAntenna_cnt)(rxAntennaIndx).set_col(time_cnt,generateFadingChannel(nodesInfo.nodeList(txNodeIndx),rxNodeID,currentTimeInSeconds(time_cnt),false,txAntenna_cnt,rxAntenna_cnt,0,fftSize));
	      else
		channel(txAntenna_cnt)(rxAntennaIndx).set_col(time_cnt,generateFadingChannel(nodesInfo.nodeList(txNodeIndx),rxNodeID,currentTimeInSeconds(time_cnt),false,txAntenna_cnt,rxAntenna_cnt,randVec.get(seed_cnt),fftSize));
	    }
	}
      }
      for(int row_cnt=0;row_cnt<channel(0)(0).rows();row_cnt++)
	for(int col_cnt=0;col_cnt<channel(0)(0).cols();col_cnt++)
	{
	  for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	    for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
	      instantChannel(rxAntenna_cnt,txAntenna_cnt)=channel(txAntenna_cnt)(rxAntenna_cnt).get(row_cnt,col_cnt);
	    
	    instantChannel=rxAntennaCorrelationMatrix*instantChannel*txAntennaCorrelationMatrix;
	  
	  for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	    for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
	      channel(txAntenna_cnt)(rxAntenna_cnt).set(row_cnt,col_cnt,instantChannel(rxAntenna_cnt,txAntenna_cnt));
	}
      
      for(int txAntenna_cnt=0;txAntenna_cnt<txAntennaCount;txAntenna_cnt++)
	for(int rxAntenna_cnt=0;rxAntenna_cnt<rxAntennaCount;rxAntenna_cnt++)
	{
	  channelMatrix.channels(0)(txAntenna_cnt)(rxAntenna_cnt)=channel(rxAntenna_cnt)(txAntenna_cnt);
	  channelMatrix.channels(0)(txAntenna_cnt)(rxAntenna_cnt)*=sqrt(inv_dB(channelGainInDB));
	}
	
	
    }
  }
  
  return(channelMatrix);
}

double ChannelCloud::addSignalsModelledAsNoise(int rxNodeID, ChannelOutput_S &channelOutput,double currentTimeInSeconds ,ivec txNodeListToModelAsNoise, vec noiseModellingTxSubcarrierPowerInDBm){

  if(find(find(currentTxNodes,txNodeListToModelAsNoise),-1)!=-1)
  {
    cout<<"[both:] Error .. Some of the nodes in txNodeListToModelAsNoise are not in Tx Mode .. "<<endl;
    abort();
  }

  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[both:]Error. Given Rx node "<<rxNodeID<<" available in the channel."<<endl;
    abort();
  }

  double txVariancePerDimension=getInstantNoiseModellingPower(rxNodeID,currentTimeInSeconds,txNodeListToModelAsNoise,noiseModellingTxSubcarrierPowerInDBm)/2.0;
  cvec modelledNoise;
  for(int rxAntenna_cnt=0;rxAntenna_cnt<nodesInfo.antennaCount(rxNodeIndx);rxAntenna_cnt++)
  {
    modelledNoise=randn_c(channelOutput.data(rxAntenna_cnt).length())*sqrt(txVariancePerDimension*2);
    channelOutput.data(rxAntenna_cnt)+=modelledNoise;
    channelOutput.summedUpSignalPerAntennaPerTone(rxAntenna_cnt)+=modelledNoise;
  }

  return txVariancePerDimension*2.0;
}

double ChannelCloud::getInstantNoiseModellingPower(int rxNodeID, double currentTimeInSeconds ,ivec txNodeListToModelAsNoise, vec noiseModellingTxSubcarrierPowerInDBm){
  
  if(find(find(currentTxNodes,txNodeListToModelAsNoise),-1)!=-1)
  {
    cout<<"[both:] Error .. Some of the nodes in txNodeListToModelAsNoise are not in Tx Mode .. "<<endl;
    abort();
  }
  
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[both:]Error. Given Rx node "<<rxNodeID<<" available in the channel."<<endl;
    abort();
  }
  
  if(noiseModellingTxSubcarrierPowerInDBm.length()!=txNodeListToModelAsNoise.length())
  {
    cout<<"[both:]Error. Length Mismatch between txNodeListToModelAsNoise and noiseModellingTxSubcarrierPowerInDBm in addSignalsModelledAsNoise()."<<endl;
    abort();
  }
  //double wd;
  double rxPowerInDBm;
  cvec tapWeight;
  double txVariancePerDimension=0;
  int txNodeIndx;
  ChannelInfo_S channelInfo;
  int chInfoIndx;
  Vec<unsigned int> randVec;
  bool isGenMethodChanged=false;
  ChannelGenerationMethod_E originalChnGenMethod=channelGenerationMethod;
  if(channelGenerationMethod!=_CHANNEL_GENERATION_METHOD_JAKES_)
  {
    channelGenerationMethod=_CHANNEL_GENERATION_METHOD_JAKES_;
    isGenMethodChanged=true;
  }
  
  for(int tx_cnt=0;tx_cnt<txNodeListToModelAsNoise.length();tx_cnt++)
  {
    txNodeIndx=find(nodesInfo.nodeList,txNodeListToModelAsNoise(tx_cnt));
    chInfoIndx=channelInfoIndx(rxNodeIndx,txNodeIndx);
    randVec=rndSeed(rxNodeIndx)(txNodeIndx);
    
    if(chInfoIndx!=-1)
      channelInfo=linkSpecificChannelInfo(chInfoIndx);
    else
      channelInfo=sysWideChannelInfo;
    rxPowerInDBm=noiseModellingTxSubcarrierPowerInDBm(tx_cnt)+linkGainInDB(rxNodeIndx,txNodeIndx);
//    rxPowerInDBm=noiseModellingTxSubcarrierPowerInDBm(tx_cnt)+getLinkGainInDB(rxNodeIndx,txNodeIndx);
    if(channelInfo.fadingType==_FADING_TYPE_CORRELATED_)
    {
      tapWeight=generateChannelTaps(channelInfo,velocityMatrix(rxNodeIndx,txNodeIndx),currentTimeInSeconds,generateFadingParameters(_FADING_CHANNEL_MODEL_FLAT_FADING_),nodesInfo.carrierFrequency(rxNodeIndx),randVec(0));
      txVariancePerDimension+=abs(tapWeight(0))*inv_dB(rxPowerInDBm-30)/2;
    }
    else
      txVariancePerDimension+=inv_dB(rxPowerInDBm-30)/2;
  }
  
  if(isGenMethodChanged)
    channelGenerationMethod=originalChnGenMethod;
  
  return txVariancePerDimension*2.0;
}

void ChannelCloud::loadDefault(){

  if(commonSamplingFrequency==-1)
  {
	cout<<"[both:]Error. Sampling Frequency not set. Default value not possible."<<endl;
	abort();
  }
  if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_UNINIT_)
  {
	cout<<"[detl:]Loading default value for channel Generation Method as Jakes."<<endl;
	channelGenerationMethod=_CHANNEL_GENERATION_METHOD_JAKES_;
  }
  if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_MOD_JAKES_ && modifiedJakesInfo.maximumNo==-1)
  {
    cout<<"[detl:]Loading default value for maximumNo as 256 for modified Jakes."<<endl;
    cout<<"[detl:]Default value for No as 64 for modified Jakes declared in initChannel."<<endl;
    modifiedJakesInfo.maximumNo=256;
  }
  if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SMITH_ && simulationDuration==-1)
  {
	cout<<"[both:]Error. Simulation Duration required for Smith channel generation not set. Default value not possible."<<endl;
	abort();
  }
  if(nodesInfo.nodeList.length()!=0 && linkSpecificChannelInfo.length()<(nodesInfo.nodeList.length()*nodesInfo.nodeList.length()))
  {
    if(sysWideChannelInfo.fadingType==_FADING_TYPE_UNINIT_)
    {
	  cout<<"[detl:]Loading default value for system Wide channel Model as Correlated Fading."<<endl;
	  sysWideChannelInfo.fadingType=_FADING_TYPE_CORRELATED_;
    }
    if(sysWideChannelInfo.channelModel==_FADING_CHANNEL_MODEL_UNINIT_)
    {
	  cout<<"[detl:]Loading default value for system Wide channel Model as Pedestrian B."<<endl;
	  sysWideChannelInfo.channelModel=_FADING_CHANNEL_MODEL_PED_B_;
    }
//     if(sysWideChannelInfo.dopplerSpread==_FADING_CHANNEL_MODEL_UNINIT_)
//     {
//       cout<<"[detl:]Loading default value for system Wide dopplerSpread as 7Hz."<<endl;
//       sysWideChannelInfo.dopplerSpread=7;
//     }

  }
}

void ChannelCloud::syncLink(int nodeX, int nodeY, bool overwrite){

  int nodeXIndx=find(nodesInfo.nodeList,nodeX);
  int nodeYIndx=find(nodesInfo.nodeList,nodeY);
  vec xRxDelay,yRxDelay;
  if(nodeXIndx!=-1 && nodeYIndx!=-1)
  {
    xRxDelay=pathDelayInSec.get_row(nodeXIndx);
    yRxDelay=pathDelayInSec.get_row(nodeYIndx);
    if(overwrite)
    {
      instantPathDelayInSec.set_row(nodeXIndx,xRxDelay-xRxDelay(nodeYIndx));
      instantPathDelayInSec.set_row(nodeYIndx,yRxDelay-yRxDelay(nodeXIndx));
      nodeSyncList(nodeXIndx)=nodeY;
      nodeSyncList(nodeYIndx)=nodeX;
    }
    else
    {
      if(nodeSyncList(nodeXIndx)!=-1)
	cout<<"[detl:]Given node : "<<nodeXIndx<<" already synched with node : "<<nodeSyncList(nodeXIndx)<<endl;
      else
      {
	instantPathDelayInSec.set_row(nodeXIndx,xRxDelay-xRxDelay(nodeYIndx));
	instantPathDelayInSec.set_row(nodeYIndx,yRxDelay-yRxDelay(nodeXIndx));
	nodeSyncList(nodeXIndx)=nodeY;
	nodeSyncList(nodeYIndx)=nodeX;
      }
    }
  }

}

FadingParameters_S ChannelCloud::generateFadingParameters(FadingChannelModel_E channelModel,double delaySpreadTDL, double dopplerSpread){


  FadingParameters_S tempChannelParms;
  switch(channelModel)
  {
#ifdef USING_CALIBRATION
if (traceValuePerTraceName["PUSCH_3_0caseA"]=="true" || traceValuePerTraceName["PUSCH_3_0caseBsubband"]=="true" || traceValuePerTraceName["PUSCH_3_0caseBwideband"]=="true" || traceValuePerTraceName["PUSCH_2_0subband"]=="true" || traceValuePerTraceName["PUSCH_2_0wideband"]=="true")
{
    case _FADING_CHANNEL_MODEL_2_TAP_:
	{
	  tempChannelParms.nTaps=2;
	  tempChannelParms.tapDel="0 0.45e-6";
	  tempChannelParms.tapGain.set_size(2);
	  tempChannelParms.tapGain(0)=sqrt(1.0/2.0);	  
	  tempChannelParms.tapGain(1)=sqrt(1.0/2.0);
	  break;
	}
}
#endif
	case _FADING_CHANNEL_MODEL_SUI_3_:
	{
	  tempChannelParms.nTaps=3;
	  tempChannelParms.tapDel="0 0.4e-6 0.9e-6";
	  tempChannelParms.tapGain.set_size(3);
	  tempChannelParms.tapGain(0)=sqrt(1./2.);	  tempChannelParms.tapGain(1)=sqrt(pow(10,(-5./10.))/2);	  tempChannelParms.tapGain(2)=sqrt(pow(10,(-10./10.))/2);
	  break;
	}
	case _FADING_CHANNEL_MODEL_PED_A_:
	{
	  tempChannelParms.nTaps=4;
	  tempChannelParms.tapDel="0 0.11e-6 0.19e-6 0.41e-6";
	  tempChannelParms.tapGain.set_size(4);
	  tempChannelParms.tapGain(0)=sqrt(1./2.);tempChannelParms.tapGain(1)= sqrt(pow(10,(-9.7/10.))/2);
	  tempChannelParms.tapGain(2)=sqrt(pow(10,(-19.2/10.))/2); tempChannelParms.tapGain(3)=sqrt(pow(10,(-22.8/10.))/2);
	  break;
	}
	case _FADING_CHANNEL_MODEL_PED_B_:
	{
	  tempChannelParms.nTaps=6;
	  tempChannelParms.tapDel="0 0.2e-6 0.8e-6 1.2e-6 2.3e-6 3.7e-6";
	  tempChannelParms.tapGain.set_size(6);
	  tempChannelParms.tapGain(0)=sqrt(1./2.);tempChannelParms.tapGain(1)=sqrt(pow(10,(-1./10.))/2);tempChannelParms.tapGain(2)=sqrt(pow(10,(-5./10.))/2);
	  tempChannelParms.tapGain(3)=sqrt(pow(10,(-8./10.))/2);tempChannelParms.tapGain(4)=sqrt(pow(10,(-17.8/10.))/2);tempChannelParms.tapGain(5)=sqrt(pow(10,(-24./10.))/2);
	  break;
	}
	case _FADING_CHANNEL_MODEL_VEH_A_:
	{
	  tempChannelParms.nTaps=6;
	  tempChannelParms.tapDel="0 0.3e-6 0.7e-6 1.1e-6 1.7e-6 2.5e-6";
	  tempChannelParms.tapGain.set_size(6);
	  tempChannelParms.tapGain(0)=sqrt(1./2.);tempChannelParms.tapGain(1)=sqrt(pow(10,(-1./10.))/2);tempChannelParms.tapGain(2)=sqrt(pow(10,(-9./10.))/2);
	  tempChannelParms.tapGain(3)=sqrt(pow(10,(-10./10.))/2);tempChannelParms.tapGain(4)=sqrt(pow(10,(-15./10.))/2);tempChannelParms.tapGain(5)=sqrt(pow(10,(-20./10.))/2);
	  break;
	}
	case _FADING_CHANNEL_MODEL_EPA_:
	{
	  tempChannelParms.nTaps=7;
	  tempChannelParms.tapDel="0 0.03e-6 0.07e-6 0.09e-6  0.11e-6 0.19e-6 0.41e-6 ";
	  tempChannelParms.tapGain.set_size(7);
	  tempChannelParms.tapGain(0)=sqrt(1./2.);tempChannelParms.tapGain(1)=sqrt(pow(10,(-1./10.))/2);tempChannelParms.tapGain(2)=sqrt(pow(10,(-2./10.))/2);
	  tempChannelParms.tapGain(3)=sqrt(pow(10,(-3./10.))/2);tempChannelParms.tapGain(4)=sqrt(pow(10,(-8./10.))/2);tempChannelParms.tapGain(5)=sqrt(pow(10,(-17.2/10.))/2);
	  tempChannelParms.tapGain(6)=sqrt(pow(10,(-20.8/10.))/2);
	  break;
	}
	case _FADING_CHANNEL_MODEL_EVA_:
	{
	  tempChannelParms.nTaps=9;
	  tempChannelParms.tapDel="0 0.03e-6  0.15e-6  0.31e-6  0.37e-6  0.71e-6  1.09e-6  1.73e-6  2.51e-6  ";
	  tempChannelParms.tapGain.set_size(9);
	  tempChannelParms.tapGain(0)=sqrt(1./2.);tempChannelParms.tapGain(1)=sqrt(pow(10,(-1.5/10.))/2);tempChannelParms.tapGain(2)=sqrt(pow(10,(-1.4/10.))/2);
	  tempChannelParms.tapGain(3)=sqrt(pow(10,(-3.6/10.))/2);tempChannelParms.tapGain(4)=sqrt(pow(10,(-0.6/10.))/2);tempChannelParms.tapGain(5)=sqrt(pow(10,(-9.1/10.))/2);
	  tempChannelParms.tapGain(6)=sqrt(pow(10,(-7.0/10.))/2);tempChannelParms.tapGain(7)=sqrt(pow(10,(-12.0/10.))/2);tempChannelParms.tapGain(8)=sqrt(pow(10,(-16.9/10.))/2);
	  break;
	}
	case _FADING_CHANNEL_MODEL_ETU_:
	{
	  tempChannelParms.nTaps=9;
	  tempChannelParms.tapDel="0 0.05e-6  0.12e-6  0.2e-6 0.23e-6  0.5e-6 1.6e-6  2.3e-6  5e-6 ";
	  tempChannelParms.tapGain.set_size(9);
	  tempChannelParms.tapGain(0)=sqrt(pow(10,(-1./10.))/2);tempChannelParms.tapGain(1)=sqrt(pow(10,(-1./10.))/2);tempChannelParms.tapGain(2)=sqrt(pow(10,(-1./10.))/2);
	  tempChannelParms.tapGain(3)=sqrt(pow(10,(-0./10.))/2);tempChannelParms.tapGain(4)=sqrt(pow(10,(-0./10.))/2);tempChannelParms.tapGain(5)=sqrt(pow(10,(-0./10.))/2);
	  tempChannelParms.tapGain(6)=sqrt(pow(10,(-3./10.))/2);tempChannelParms.tapGain(7)=sqrt(pow(10,(-5./10.))/2);tempChannelParms.tapGain(8)=sqrt(pow(10,(-7./10.))/2);
	  break;
	}
	case _FADING_CHANNEL_MODEL_MODIFIED_PED_B_:
	{
	  tempChannelParms.nTaps=24;
	  tempChannelParms.tapDel="0 0.04e-6 0.07e-6 0.120e-6 0.210e-6 0.250e-6 0.290e-6 0.350e-6 0.780e-6 0.830e-6 0.880e-6 "
		"0.920e-6 1.200e-6 1.250e-6 1.310e-6 1.350e-6 2.290e-6 2.350e-6 2.380e-6 2.400e-6 3.700e-6 3.730e-6 3.730e-6 3.870e-6";
		tempChannelParms.tapGain.set_size(24);
		tempChannelParms.tapGain(0)=sqrt(pow(10,(-1.175/10.))/2.); tempChannelParms.tapGain(1)=sqrt(pow(10,(-0./10.))/2); tempChannelParms.tapGain(2)=sqrt(pow(10,(-0.1729/10.))/2);
		tempChannelParms.tapGain(3)=sqrt(pow(10,(-0.2113/10.))/2); tempChannelParms.tapGain(4)=sqrt(pow(10,(-0.2661/10.))/2); tempChannelParms.tapGain(5)=sqrt(pow(10,(-0.3963/10.))/2); tempChannelParms.tapGain(6)=sqrt(pow(10,(-4.32/10.))/2);
		tempChannelParms.tapGain(7)=sqrt(pow(10,(-1.1608/10.))/2); tempChannelParms.tapGain(8)=sqrt(pow(10,(-10.4232/10.))/2); tempChannelParms.tapGain(9)=sqrt(pow(10,(-5.7198/10.))/2); tempChannelParms.tapGain(10)=sqrt(pow(10,(-3.4798/10.))/2);
		tempChannelParms.tapGain(11)=sqrt(pow(10,(-4.1745/10.))/2); tempChannelParms.tapGain(12)=sqrt(pow(10,(-10.1101/10.))/2); tempChannelParms.tapGain(13)=sqrt(pow(10,(-5.646/10.))/2); tempChannelParms.tapGain(14)=sqrt(pow(10,(-10.0817/10.))/2);
		tempChannelParms.tapGain(15)=sqrt(pow(10,(-9.4109/10.))/2); tempChannelParms.tapGain(16)=sqrt(pow(10,(-13.9434/10.))/2); tempChannelParms.tapGain(17)=sqrt(pow(10,(-9.1845/10.))/2); tempChannelParms.tapGain(18)=sqrt(pow(10,(-5.5766/10.))/2);
		tempChannelParms.tapGain(19)=sqrt(pow(10,(-7.6455/10.))/2); tempChannelParms.tapGain(20)=sqrt(pow(10,(-38.1923/10.))/2); tempChannelParms.tapGain(21)=sqrt(pow(10,(-22.3097/10.))/2); tempChannelParms.tapGain(22)=sqrt(pow(10,(-26.0472/10.))/2);
		tempChannelParms.tapGain(23)=sqrt(pow(10,(-21.6155/10.))/2);
	  break;
	}
	case _FADING_CHANNEL_MODEL_MODIFIED_VEH_A_:
	{
	  tempChannelParms.nTaps=24;
	  tempChannelParms.tapDel="0 0.05e-6 0.09e-6 0.130e-6 0.270e-6 0.300e-6 0.390e-6 0.420e-6 0.670e-6 0.750e-6 0.770e-6 0.800e-6 "
	  "1.040e-6 1.060e-6 1.070e-6 1.190e-6 1.670e-6 1.710e-6 1.820e-6 1.840e-6 2.480e-6 2.500e-6 2.540e-6 2.620e-6";
	  tempChannelParms.tapGain.set_size(24);
	  tempChannelParms.tapGain(0)=sqrt(pow(10,(-3.1031/10.))/2.); tempChannelParms.tapGain(1)= sqrt(pow(10,(-0.4166/10.))/2); tempChannelParms.tapGain(2)= sqrt(pow(10,(-0/10.))/2); tempChannelParms.tapGain(3)= sqrt(pow(10,(-1.0065/10.))/2);
	  tempChannelParms.tapGain(4)=sqrt(pow(10,(-1.4083/10.))/2); tempChannelParms.tapGain(5)= sqrt(pow(10,(-1.4436/10.))/2); tempChannelParms.tapGain(6)= sqrt(pow(10,(-1.5443/10.))/2); tempChannelParms.tapGain(7)= sqrt(pow(10,(-4.0437/10.))/2);
	  tempChannelParms.tapGain(8)=sqrt(pow(10,(-16.6369/10.))/2); tempChannelParms.tapGain(9)= sqrt(pow(10,(-14.3955/10.))/2); tempChannelParms.tapGain(10)= sqrt(pow(10,(-4.9259/10.))/2); tempChannelParms.tapGain(11)= sqrt(pow(10,(-16.516/10.))/2);
	  tempChannelParms.tapGain(12)=sqrt(pow(10,(-9.222/10.))/2); tempChannelParms.tapGain(13)= sqrt(pow(10,(-11.9058/10.))/2); tempChannelParms.tapGain(14)= sqrt(pow(10,(-10.1378/10.))/2); tempChannelParms.tapGain(15)= sqrt(pow(10,(-14.1861/10.))/2);
	  tempChannelParms.tapGain(16)=sqrt(pow(10,(-16.9901/10.))/2); tempChannelParms.tapGain(17)= sqrt(pow(10,(-13.2515/10.))/2); tempChannelParms.tapGain(18)= sqrt(pow(10,(-14.8881/10.))/2); tempChannelParms.tapGain(19)= sqrt(pow(10,(-30.348/10.))/2);
	  tempChannelParms.tapGain(20)=sqrt(pow(10,(-19.5257/10.))/2); tempChannelParms.tapGain(21)= sqrt(pow(10,(-19.0286/10.))/2); tempChannelParms.tapGain(22)= sqrt(pow(10,(-38.1504/10.))/2); tempChannelParms.tapGain(23)= sqrt(pow(10,(-20.7436/10.))/2);

	  break;
	}
	case _FADING_CHANNEL_MODEL_TDL_A_:
	{
	  tempChannelParms.nTaps=23;
	  tempChannelParms.tapDel=vec("0 0.3819 0.4025 0.5868 0.461 0.5375 0.6708 0.575 0.7618 1.5375 1.8978 2.2242 2.1718 2.4942 2.5119 3.0582 4.081 4.4579 4.5695 4.7966 5.0066 5.3043 9.6586");
	  tempChannelParms.tapDel = delaySpreadTDL*tempChannelParms.tapDel;
	  tempChannelParms.tapGain=vec("-13.4 0 -2.2 -4 -6 -8.2 -9.9 -10.5 -7.5 -15.9 -6.6 -16.7 -12.4 -15.2 -10.8 -11.3 -12.7 -16.2 -18.3 -18.9 -16.6 -19.9 -29.7");
	  tempChannelParms.tapGain=sqrt(inv_dB(tempChannelParms.tapGain));  
	  break;
	}
	case _FADING_CHANNEL_MODEL_TDL_B_:
	{
	  tempChannelParms.nTaps=23;
	  tempChannelParms.tapDel=vec("0 0.1072 0.2155 0.2095 0.287 0.2986 0.3752 0.5055 0.3681 0.3697 0.57 0.5283 1.1021 1.2756 1.5474 1.7842 2.0169 2.8294 3.0219 3.6187 4.1067 4.279 4.7834");
	  tempChannelParms.tapDel = delaySpreadTDL*tempChannelParms.tapDel;
	  tempChannelParms.tapGain=vec("0 -2.2 -4 -3.2 -9.8 -1.2 -3.4 -5.2 -7.6 -3 -8.9 -9 -4.8 -5.7 -7.5 -1.9 -7.6 -12.2 -9.8 -11.4 -14.9 -9.2 -11.3");
	  tempChannelParms.tapGain=sqrt(inv_dB(tempChannelParms.tapGain));  
	  break;
	}
	case _FADING_CHANNEL_MODEL_TDL_C_:
	{
	  tempChannelParms.nTaps=24;
	  tempChannelParms.tapDel=vec("0 0.2099 0.2219 0.2329 0.2176 0.6366 0.6448 0.656 0.6584 0.7935 0.8213 0.9336 1.2285 1.3083 2.1704 2.7105 4.2589 4.6003 5.4902 5.6077 6.3065 6.6374 7.0427 8.6523");
	  tempChannelParms.tapDel = delaySpreadTDL*tempChannelParms.tapDel;
	  tempChannelParms.tapGain=vec("-4.4 -1.2 -3.5 -5.2 -2.5 0 -2.2 -3.9 -7.4 -7.1 -10.7 -11.1 -5.1 -6.8 -8.7 -13.2 -13.9 -13.9 -15.8 -17.1 -16 -15.7 -21.6 -22.8");
	  tempChannelParms.tapGain=sqrt(inv_dB(tempChannelParms.tapGain));  
	  break;
	}
	case _FADING_CHANNEL_MODEL_TDL_D_:
	{
	  tempChannelParms.nTaps=14;
	  tempChannelParms.tapDel=vec("0 0 0.035 0.612 1.363 1.405 1.804 2.596 1.775 4.042 7.937 9.424 9.708 12.525");
	  tempChannelParms.tapDel = delaySpreadTDL*tempChannelParms.tapDel;
	  tempChannelParms.tapGain=vec("-0.2 -13.5 -18.8 -21 -22.8 -17.9 -20.1 -21.9 -22.9 -27.8 -23.6 -24.8 -30 -27.7");
	  tempChannelParms.tapGain=sqrt(inv_dB(tempChannelParms.tapGain));  
	  break;
	}
	case _FADING_CHANNEL_MODEL_TDL_E_:
	{
	  tempChannelParms.nTaps=15;
	  tempChannelParms.tapDel=vec("0 0 0.5133 0.544 0.563 0.544 0.7112 1.9092 1.9293 1.9589 2.6426 3.7136 5.4524 12.0034 20.6519");
	  tempChannelParms.tapDel = delaySpreadTDL*tempChannelParms.tapDel;
	  tempChannelParms.tapGain=vec("-0.03 -22.03 -15.8 -18.1 -19.8 -22.9 -22.4 -18.6 -20.8 -22.6 -22.3 -25.6 -20.2 -29.8 -29.2");
	  tempChannelParms.tapGain=sqrt(inv_dB(tempChannelParms.tapGain));  
	  break;
	}
	case _FADING_CHANNEL_MODEL_FLAT_FADING_: {

	  tempChannelParms.nTaps=1;
	  tempChannelParms.tapGain.set_size(1);
	  tempChannelParms.tapGain=sqrt(pow(10,(0/10.))/2.);
	  tempChannelParms.tapDel=zeros(1);
	  break;
	}
	case _FADING_CHANNEL_MODEL_CDL_A_:
    case _FADING_CHANNEL_MODEL_CDL_B_:
    case _FADING_CHANNEL_MODEL_CDL_C_:
    {
        mat channelParam = getChannelParameters(channelModel);
        tempChannelParms.tapDel = channelParam.get_row(0)*delaySpreadTDL;
        tempChannelParms.tapGain = sqrt(inv_dB(channelParam.get_row(1)));
        tempChannelParms.nTaps = channelParam.cols();
        tempChannelParms.nSinusoids=50;
        double energy=sum(sqr(tempChannelParms.tapGain));
        tempChannelParms.tapGain/=sqrt(energy);
        break;
    }
    case _FADING_CHANNEL_MODEL_CDL_D_:
    case _FADING_CHANNEL_MODEL_CDL_E_:
    {
        mat channelParam = getChannelParameters(channelModel);
        tempChannelParms.tapDel = channelParam.get_row(0)*delaySpreadTDL;
        tempChannelParms.tapGain = sqrt(inv_dB(channelParam.get_row(1)));
        tempChannelParms.nTaps = channelParam.cols()-1;
        tempChannelParms.tapDel.del(0);
        tempChannelParms.nSinusoids=50;
        double energy=sum(sqr(tempChannelParms.tapGain));
        tempChannelParms.tapGain/=sqrt(energy);
        break;
    }
	default:
	{
	  cout<<"[both:]Error. Unsupported Channel Model to generateFadingParameters().";
	  abort();
	}
  }

  switch(channelGenerationMethod)
  {
	case _CHANNEL_GENERATION_METHOD_SMITH_:
	{
	  if(dopplerSpread==-1)
	  {
	    cout<<"[both:]Error. dopplerSpread need to be passed for _CHANNEL_GENERATION_METHOD_SMITH_ in generateFadingParameters()."<<endl;
	    abort();
	  }
	  tempChannelParms.nSpecSamples=ceil_i(dopplerSpread*simulationDuration);
	  break;
	}
	case _CHANNEL_GENERATION_METHOD_JAKES_:
	{
	  tempChannelParms.nSinusoids=50;
	  break;
	}
	case _CHANNEL_GENERATION_METHOD_SCM_:
    case _CHANNEL_GENERATION_METHOD_MOD_JAKES_:
    case _CHANNEL_GENERATION_METHOD_CDL_:
	{
	  break;
	}
	default:
	{
	  cout<<"[both:]Undefined Channel Generation Method for generateFadingParameters()."<<endl;
	  abort();
	}
  }

  double energy=sum(sqr(tempChannelParms.tapGain));
  tempChannelParms.tapGain/=sqrt(energy);

  return(tempChannelParms);

}

cvec ChannelCloud::generateFadingChannel(int txNodeID, int rxNodeID, double currentTimeInSeconds, bool printChannel, int currentTxAntenna, int currentRxAntenna, unsigned int rndSeed, int fftSize){

  ChannelInfo_S channelInfo;
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[both:]Error. Given Rx node "<<rxNodeID<<" available in the channel."<<endl;
    abort();
  }
  int txNodeIndx=find(nodesInfo.nodeList,txNodeID);
  if(txNodeIndx==-1)
  {
	cout<<"[both:]Error. Tx node "<<txNodeID<<" not linked to the channel."<<endl;
	abort();
  }
  int chInfoIndx;
  double pathDelay=0;
  chInfoIndx=channelInfoIndx(rxNodeIndx,txNodeIndx);
  if(instantPathDelayInSec.size()!=0)
    pathDelay=instantPathDelayInSec(rxNodeIndx,txNodeIndx);

  if(channelGenerationMethod!=_CHANNEL_GENERATION_METHOD_SCM_)
  {
    if(chInfoIndx!=-1)
	channelInfo=linkSpecificChannelInfo(chInfoIndx);
    else
	channelInfo=sysWideChannelInfo;
  }
  else
  {
    if(scmLinkIndx(rxNodeIndx,txNodeIndx)==-1)
    {
      cout<<"[both:]SCM link between nodes "<<txNodeID<<" and "<<rxNodeID<<" not initialized."<<endl;
      abort();
    }
    channelInfo.fadingType=_FADING_TYPE_CORRELATED_;
    channelInfo.fadingParameters.tapDel=scmLinks(scmLinkIndx(rxNodeIndx,txNodeIndx)).getTapDelays();
    channelInfo.fadingParameters.nTaps=channelInfo.fadingParameters.tapDel.length();
  }

  if(addPathDelay)
    channelInfo.fadingParameters.tapDel+=pathDelay;

  double samplingFreqToUse;
  if(nodesInfo.samplingFrequency.length()!=0)
    samplingFreqToUse=nodesInfo.samplingFrequency(rxNodeIndx); //using rxNodeID sampling freqneucy to decide output size
  else
    samplingFreqToUse=commonSamplingFrequency;
    
  cvec myChannel(0);
  if(timeDomain==0)
  {
    if(fftSize!=-1)
      myChannel.set_size(fftSize);
    else if(dataSize!=-1)
    {
      fftSize=dataSize;
      myChannel.set_size(dataSize);
    }
    else
    {
      cout<<"[both:]FFT size not available in generateFadingChannel. Either transmit data for the transmitter or pass fftSize ti the function."<<endl;
      abort();
    }
    myChannel.clear();
  }

  if(channelInfo.fadingType==_FADING_TYPE_UNCORRELATED_)
  {
    if(timeDomain==0)
      for(int data_cnt=0;data_cnt<myChannel.length();data_cnt++)
      {
	myChannel(data_cnt)=complex< double >(gaussian(0,0.5),gaussian(0,0.5));
      }
    else
    {
	  cout<<"[both:]Error. Uncorrelated Fading not available for time Domain CHannel."<<endl;
	  return(cvec(0));
    }
  }
  else if(channelInfo.fadingType==_FADING_TYPE_CORRELATED_)
  {
	cvec tapWeights;
	if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
	{
	  if(currentRxAntenna==-1 || currentTxAntenna==-1)
	  {
	    cout<<"[both:]Pass current antenna to generateFadingChannel() in case of SCM."<<endl;
	    abort();
	  }
	  
	  // AAS Channel
	  
	  //If not configured, this will be default beam (lowest beamID available to the txRU) - Dhiv
	  int txBeamID = nodesInfo.beamIDPerTxRU(txNodeIndx)(currentTxAntenna);
      int rxBeamID = nodesInfo.beamIDPerTxRU(rxNodeIndx)(currentRxAntenna);
      
	  SCMPropagationScenario propagationScenario5GChannel = scmLinks(scmLinkIndx(rxNodeIndx,txNodeIndx)).propagationScenario;
	  if(is5GSCMScenario(propagationScenario5GChannel))
	  {
	    if(scmReciprocityFlag(rxNodeIndx,txNodeIndx)) //using reverse link for channel generation
	    {
            tapWeights=generate5GAASChannelTapGains(rxNodeID, txNodeID, currentRxAntenna, currentTxAntenna, currentTimeInSeconds,false,rxBeamID,txBeamID);
	    }
	    else
	    {
            tapWeights=generate5GAASChannelTapGains(txNodeID, rxNodeID, currentTxAntenna, currentRxAntenna, currentTimeInSeconds,false,txBeamID,rxBeamID);
	    }
	  }
	  else
	  {
	    if(scmReciprocityFlag(rxNodeIndx,txNodeIndx)) //using reverse link for channel generation
	    {
	      tapWeights=scmLinks(scmLinkIndx(rxNodeIndx,txNodeIndx)).generateTapGains(currentRxAntenna,currentTxAntenna,currentTimeInSeconds);
	    }
	    else
	    {
	      tapWeights=scmLinks(scmLinkIndx(rxNodeIndx,txNodeIndx)).generateTapGains(currentTxAntenna,currentRxAntenna,currentTimeInSeconds);
	    }
	  }
	  

	}
	else if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_MOD_JAKES_)
	{
	  tapWeights=modifiedJakesInfo.getTapWeights(txNodeID,rxNodeID,channelInfo.fadingParameters.nTaps);
	  for(int tap_cnt=0;tap_cnt<channelInfo.fadingParameters.nTaps;tap_cnt++)
	    tapWeights(tap_cnt)*=channelInfo.fadingParameters.tapGain(tap_cnt);
	}
	else if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_JAKES_)
	{
 	  tapWeights=generateChannelTaps(channelInfo,velocityMatrix(rxNodeIndx,txNodeIndx),currentTimeInSeconds,channelInfo.fadingParameters,nodesInfo.carrierFrequency(rxNodeIndx),rndSeed,samplingFreqToUse);
#ifdef USING_CALIBRATION
	  if(traceValuePerTraceName["PUSCH_3_0caseA"]=="true" || traceValuePerTraceName["PUSCH_3_0caseBsubband"]=="true" || traceValuePerTraceName["PUSCH_3_0caseBwideband"]=="true"|| traceValuePerTraceName["PUSCH_2_0subband"]=="true" || traceValuePerTraceName["PUSCH_2_0wideband"]=="true")
        {
	  tapWeights.set_length(2);
	  double vel=velocityMatrix(rxNodeIndx,txNodeIndx);
	  double ope_freq=nodesInfo.carrierFrequency(rxNodeIndx);
	  double dopplerSpread=(vel*ope_freq)/(3*pow(10,8));
	  tapWeights(0)=complex< double >(channelInfo.fadingParameters.tapGain(0),0);
	  tapWeights(1)=complex< double >(channelInfo.fadingParameters.tapGain(1),0)*complex< double >(cos(2*pi*dopplerSpread*currentTimeInSeconds),-sin(2*pi*dopplerSpread*currentTimeInSeconds));
	  }
#endif
 	}
 	else if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_CDL_)
    {
        if(isCDLModel(sysWideChannelInfo.channelModel))
        {
            tapWeights = llsChannel.generateTapWeights(currentTxAntenna,currentRxAntenna,currentTimeInSeconds);
        }
    }
	//cout <<"channel :"<<tapWeights<< endl;abort();
	static bool printHeading=true;
	ofstream fout;
	if(printChannel && currentRxAntenna==0 && currentTxAntenna==0)
	{
	  fout.open("./Results/ChannelLog.txt",ios::app);
	  if(printHeading)
	  {
	    fout<<"%///////////////////////////////////////////////////////////////////////////%"<<endl;
	    fout<<"%Time-domain channel for 1st antenna pair with sampling frequency "<<samplingFreqToUse<<"%"<<endl;
	    fout<<"%PDP TapPow = "<<channelInfo.fadingParameters.tapGain<<endl;
	    fout<<"%PDP TapDelay = "<<channelInfo.fadingParameters.tapDel<<endl;
	    fout<<"%///////////////////////////////////////////////////////////////////////////%"<<endl;
	    
	    printHeading=false;
	  }
	}

  // Store the tap weights instead of the channel to reduce memmory usage.
  if (quasiStaticChannelInfo.isEnabled) {
    if (quasiStaticChannelInfo.oldChannelPerRxNode(rxNodeIndx)(txNodeIndx).length() == 0) {
      quasiStaticChannelInfo.oldChannelPerRxNode(rxNodeIndx)(txNodeIndx)
          .set_length(nodesInfo.antennaCount(txNodeIndx));
      for (int txAnt_cnt = 0; txAnt_cnt < nodesInfo.antennaCount(txNodeIndx); txAnt_cnt++)
        quasiStaticChannelInfo.oldChannelPerRxNode(rxNodeIndx)(txNodeIndx)(txAnt_cnt)
            .set_length(nodesInfo.antennaCount(rxNodeIndx));
    }
    quasiStaticChannelInfo.updatesamplingFreqToUse(samplingFreqToUse);
    quasiStaticChannelInfo.updateChannelTapDelay(txNodeIndx, rxNodeIndx, channelInfo.fadingParameters.tapDel);
    quasiStaticChannelInfo.updateChannel(txNodeIndx, rxNodeIndx, currentTxAntenna,
                                         currentRxAntenna, currentTimeInSeconds, tapWeights);
  }

	if(timeDomain==1)
	{
	  myChannel.set_size(floor_i(channelInfo.fadingParameters.tapDel(channelInfo.fadingParameters.tapDel.length()-1)*samplingFreqToUse)+1);
	  
	  myChannel.clear();
	  int channelIndx=0;
	  for(int tap_cnt=0;tap_cnt< channelInfo.fadingParameters.nTaps;tap_cnt++)
	  {
		channelIndx=floor_i(channelInfo.fadingParameters.tapDel(tap_cnt)*samplingFreqToUse);
		if(channelIndx>=myChannel.length())
		{
		  cout<<"[both:] Insufficient Channel length to represent the Channel Model at the given sampling Frequency."<<endl;
		  abort();
		}
		myChannel(channelIndx) = myChannel(channelIndx) +tapWeights(tap_cnt);
	  }
	  if(printChannel && currentRxAntenna==0 && currentTxAntenna==0)
	  {
	    for(int tap_cnt=0;tap_cnt<myChannel.length();tap_cnt++)
	      fout<<myChannel(tap_cnt)<<" ";
	    fout<<endl;
	  }
	}
	else
	{
	  cvec sampleSpacedChannel=zeros_c(floor_i(channelInfo.fadingParameters.tapDel(channelInfo.fadingParameters.tapDel.length()-1)*samplingFreqToUse)+1);
	  //sampleSpacedChannel.clear();
	  int channelIndx=0;
	  for(int tap_cnt=0;tap_cnt< channelInfo.fadingParameters.nTaps;tap_cnt++)
	  {
	    channelIndx=floor_i(channelInfo.fadingParameters.tapDel(tap_cnt)*samplingFreqToUse);
	    if(channelIndx>=sampleSpacedChannel.length())
	    {
	      cout<<"[both:] Insufficient Channel length to represent the Channel Model at the given sampling Frequency."<<endl;
	      abort();
	    }
	    
	    sampleSpacedChannel(channelIndx) = sampleSpacedChannel(channelIndx) +tapWeights(tap_cnt);
	  }
	  if(printChannel && currentRxAntenna==0 && currentTxAntenna==0)
	  {
	    for(int tap_cnt=0;tap_cnt<sampleSpacedChannel.length();tap_cnt++)
	      fout<<sampleSpacedChannel(tap_cnt)<<" ";
	    fout<<endl;
	  }
	  
	  if(sampleSpacedChannel.length()<fftSize)
	    append(sampleSpacedChannel,zeros_c(fftSize-sampleSpacedChannel.length()));
	  else
	    sampleSpacedChannel=sampleSpacedChannel(0,fftSize-1); //Truncating Channel
	  
	  myChannel=fft(sampleSpacedChannel); 
	  myChannel=fftShift(myChannel);
	}
  }
  
  return(myChannel);
}

cvec ChannelCloud::generateFadingChannelForInitialAssociation(int txNodeID, int rxNodeID, int currentTxAntenna, int currentRxAntenna,int txBeamID, int rxBeamID){
    
    int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
    if(rxNodeIndx==-1)
    {
        cout<<"[both:]Error. Given Rx node "<<rxNodeID<<" available in the channel."<<endl;
        abort();
    }
    int txNodeIndx=find(nodesInfo.nodeList,txNodeID);
    if(txNodeIndx==-1)
    {
        cout<<"[both:]Error. Tx node "<<txNodeID<<" not linked to the channel."<<endl;
        abort();
    }
    
    if(channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
    {
        if(scmLinkIndx(rxNodeIndx,txNodeIndx)==-1)
        {
            cout<<"[both:]SCM link between nodes "<<txNodeID<<" and "<<rxNodeID<<" not initialized."<<endl;
            abort();
        }
    }
    else
    {
        cout<<"[both:]This function is supported only for _CHANNEL_GENERATION_METHOD_SCM_"<<endl;
        abort();
    }
    
    
    if(currentRxAntenna==-1 || currentTxAntenna==-1)
    {
        cout<<"[both:]Pass current antenna to generateFadingChannel() in case of SCM."<<endl;
        abort();
    }
    return(generate5GAASChannelTapGains(txNodeID,rxNodeID,currentTxAntenna,currentRxAntenna,0.0,true,txBeamID,rxBeamID));
}

cvec ChannelCloud::generate5GAASChannelTapGains(int txNodeID, int rxNodeID, int currentTxAntenna, int currentRxAntenna, double currentTimeInSeconds, bool isChannelGenForRSRP, int txBeamID,int rxBeamID) 
{
  int scmLinkIndx = getScmLinkIndx(rxNodeID,txNodeID);
  if(scmLinkIndx==-1)
  {
        cout<<"SCM Link is not initialised for nodePair: ("<<txNodeID<<","<<rxNodeID<<")"<<endl;abort();
  }
  
  sBeamParameters txBeamParameters = nodesInfo.getBeamParameters(txNodeID);
  sBeamParameters rxBeamParameters = nodesInfo.getBeamParameters(rxNodeID);
  
  ivec physicalTxAntennaElemNums = txBeamParameters.TXRUparameters.getTXRUAntennaElements(currentTxAntenna);
  ivec physicalRxAntennaElemNums = rxBeamParameters.TXRUparameters.getTXRUAntennaElements(currentRxAntenna);
  //cout << "physicalTxAntennaElemNums : "<< physicalTxAntennaElemNums <<endl;
  //cout << "physicalRxAntennaElemNums : "<< physicalRxAntennaElemNums <<endl;
  //abort();
  int txOrientationID = txBeamParameters.TXRUparameters.orientationIDperTXRU(currentTxAntenna);
  int rxOrientationID = rxBeamParameters.TXRUparameters.orientationIDperTXRU(currentRxAntenna);
  //cout << "txOrientationID : "<< txOrientationID <<endl;
  //cout << "rxOrientationID : "<< rxOrientationID <<endl;
  //abort();
  if(txBeamID==-1)
  {
      cout<<"TxBeamID uninitialized in generate5GAASChannelTapGains().."<<endl;abort();
  }
  if(rxBeamID==-1)
  {
      cout<<"RxBeamID uninitialized in generate5GAASChannelTapGains().."<<endl;abort();
  }
  
  cvec txAnalogWeights = txBeamParameters.getBeamformingVector(txBeamID);
  cvec rxAnalogWeights = rxBeamParameters.getBeamformingVector(rxBeamID);
  //cout << "txAnalogWeights : "<< txAnalogWeights <<endl;
  //cout << "rxAnalogWeights : "<< rxAnalogWeights <<endl;
  //abort();
  int channelLength = scmLinks(scmLinkIndx).getTapDelays().length();
  cvec effectiveTapWeights(0);
  for(int j=0;j<physicalRxAntennaElemNums.length();j++)
  {
    cvec tempTapWeights(0);
    for(int i=0;i<physicalTxAntennaElemNums.length();i++)
    {
        cvec tapGains;
        //cout << "isChannelGenForRSRP : " << isChannelGenForRSRP <<endl;
        //abort();
        if(isChannelGenForRSRP)
        {
            //abort();
            tapGains = scmLinks(scmLinkIndx).fixedTapGainsForInitialBeam(currentRxAntenna)(txOrientationID)(j)(i);
//             tapGains = scmLinks(scmLinkIndx).scm5GLink->generate5GChannelTapGainsTxEtoRxE(physicalTxAntennaElemNums(i),physicalRxAntennaElemNums(j),currentTimeInSeconds,false,txPanelID,rxPanelID,txOrientationID,rxOrientationID); 
        }
        else
        {
            //abort();
            if(scmLinks(scmLinkIndx).ntnSCMLink->isInitialized())
                tapGains = scmLinks(scmLinkIndx).ntnSCMLink->generateTapGainsTxEtoRxE(physicalTxAntennaElemNums(i),physicalRxAntennaElemNums(j),currentTimeInSeconds,false,txOrientationID,rxOrientationID); 
            else
                tapGains = scmLinks(scmLinkIndx).scm5GLink->generate5GChannelTapGainsTxEtoRxE(physicalTxAntennaElemNums(i),physicalRxAntennaElemNums(j),currentTimeInSeconds,false,txOrientationID,rxOrientationID); 
        }
        //cout << "tapGains : "<< tapGains <<endl;
        //abort();
        if(i==0)
            tempTapWeights = txAnalogWeights (i)*tapGains;
        else
            tempTapWeights += txAnalogWeights (i)*tapGains;
    }
    
    //cout<<"tempTapWeights"<<tempTapWeights<<endl;
    if(j==0)
        effectiveTapWeights = rxAnalogWeights (j)*tempTapWeights;
    else
        effectiveTapWeights += rxAnalogWeights (j)*tempTapWeights;
  }
  //cout<<"effectiveTapWeights_Summ"<<abs(sum(effectiveTapWeights))<<endl;
  return effectiveTapWeights;
}

cvec ChannelCloud::generateChannelTaps(ChannelInfo_S channelInfo, double velocity,double currentTimeInSeconds, FadingParameters_S channelParams,double carrierFreq,  unsigned int rndSeed, double samplingFrequency){

  cvec tapWeight(channelParams.nTaps);
  tapWeight.clear();
  unsigned int rnd_state=rndSeed;
  switch(channelGenerationMethod)
  {

	case _CHANNEL_GENERATION_METHOD_SMITH_:
	{
	  cmat igv;	// Space for in-phase gaussian samples to implement fading
	  cmat qgv;	// space for quadrature gaussian samples to implement fading
	  igv.set_size(channelParams.nTaps,channelParams.nSpecSamples);
	  qgv.set_size(channelParams.nTaps,channelParams.nSpecSamples);

	  for(int tap_cnt=0;tap_cnt<channelParams.nTaps;tap_cnt++)
		for(int specSamples_cnt =0;specSamples_cnt<channelParams.nSpecSamples;specSamples_cnt++)
		{
		  igv(tap_cnt,specSamples_cnt)=complex< double >(gauss_rnd_gen(rnd_state)/sqrt(2),gauss_rnd_gen(rnd_state)/sqrt(2));
		  qgv(tap_cnt,specSamples_cnt)=complex< double >(gauss_rnd_gen(rnd_state)/sqrt(2),gauss_rnd_gen(rnd_state)/sqrt(2));
		}

	  int n,k,tap;
	  int m=2*channelParams.nSpecSamples+1,f0;  //total number of samples we get is nospecsampless/2
	  double samplingDuratrion=dataSize/samplingFrequency;
	  int nosamples = (int) (simulationDuration/samplingDuratrion);
	  double real1,real2;

	  vec psd(m),psd1(m),psd2(m),psd3(m),psd4(m);
	  vec a1(channelParams.nSpecSamples),b1(channelParams.nSpecSamples),a2(channelParams.nSpecSamples),b2(channelParams.nSpecSamples);
	  vec c1(m),d1(m),c2(m),d2(m);
	  double eng;

	  for(tap=0;tap<channelParams.nTaps;tap++)
	  {
		for(int i=0;i<m;i++)
		{
		  if(i==0)
		  {
			c1(i)=0; d1(i)=0; c2(i)=0; d2(i)=0;
		  }
		  else if(i<=channelParams.nSpecSamples)
		  {
			c1(i)=igv(tap,i-1).real();
			d1(i)=igv(tap,i-1).imag();
			c2(i)=qgv(tap,i-1).real();
			d2(i)=qgv(tap,i-1).imag();
		  }
		  else
		  {
			c1(i)= igv(tap,2*channelParams.nSpecSamples-i).real();
			d1(i)= -igv(tap,2*channelParams.nSpecSamples-i).imag();
			c2(i)= qgv(tap,2*channelParams.nSpecSamples-i).real();
			d2(i)= -qgv(tap,2*channelParams.nSpecSamples-i).imag();
		  }
		}
		eng=0;
		//generating the Doppler spectrum
		switch(channelInfo.channelModel)
		{
		  case _FADING_CHANNEL_MODEL_SUI_3_:
		  {
			for (f0=0;f0<m;f0++)
			{
			  if(f0<=channelParams.nSpecSamples)
				psd(f0)=sqrt(0.785*pow((double)f0/m,4)-1.72*sqr((double)f0/m)+1);
			  else
				psd(f0)=psd(m-f0);
			  eng+=sqr(psd(f0));
			}
			break;
		  }
		  case _FADING_CHANNEL_MODEL_PED_B_:
		  {
			for (f0=0;f0<m;f0++)
			{
			  if(f0<=channelParams.nSpecSamples)
				psd(f0)=sqrt(1.5/(pi*m*sqrt(1-sqr((double)f0/m))));
			  else
				psd(f0)=psd(m-f0);
			  eng+=sqr(psd(f0));
			}
			break;
		  }
		  default:
		  {
			cout<<"[both:]Smith method of Channel generation supports only SUI3 and PedB channel models."<<endl;
			abort();
		  }
		}
		// normalising the filter energy
		for (f0=0;f0<m;f0++)
		  psd(f0)/=sqrt(eng);

		//multiplying the random sequence with the Doppler spectrum
		for(int i=0;i<m;i++)
		{

		  psd1(i)=c1(i)*psd(i);
		  psd2(i)=d1(i)*psd(i);
		  psd3(i)=c2(i)*psd(i);
		  psd4(i)=d2(i)*psd(i);
		}

		//computing DFT for the required tap sample
		n = (int) (currentTimeInSeconds/samplingDuratrion);
		real1=0;real2=0;
		for(k=0;k<m;k++)
		{
		  if(k<channelParams.nSpecSamples+1)
		  {
			real1+=psd1(k)*cos(2*pi*n*k/nosamples)-psd2(k)*sin(2*pi*n*k/nosamples);
			real2+=psd3(k)*cos(2*pi*n*k/nosamples)-psd4(k)*sin(2*pi*n*k/nosamples);
		  }
		  else //if(k>=nospecsamples+1)
		  {
			real1 += psd1(k)*cos(2*pi*n*(nosamples-2*channelParams.nSpecSamples+k-1)/nosamples)-psd2(k)*sin(2*pi*n*(nosamples-2*channelParams.nSpecSamples+k-1)/nosamples);
			real2 += psd3(k)*cos(2*pi*n*(nosamples-2*channelParams.nSpecSamples+k-1)/nosamples)-psd4(k)*sin(2*pi*n*(nosamples-2*channelParams.nSpecSamples+k-1)/nosamples);
		  }
		}
		tapWeight(tap)=complex< double >(real1/sqrt(2),real2/sqrt(2));
	  }
	  break;
	}
	case _CHANNEL_GENERATION_METHOD_JAKES_:
	{
	  //double wd=2*pi*channelInfo.dopplerSpread*currentTimeInSeconds;
	  double doppler=velocity*carrierFreq/lightSpeed;
	  double wd=2*pi*doppler*currentTimeInSeconds;
	  double theta,psin,alphan,phi;
	  tapWeight.clear();
	  for(int i=0;i<channelParams.nTaps;i++)
	  {

		theta = (rnd_gen(rnd_state)-0.5)*2*pi;
		phi = (rnd_gen(rnd_state)-0.5)*2*pi;
		for(int j=0;j<channelParams.nSinusoids;j++)
		{
		  psin=(rnd_gen(rnd_state)-0.5)*2*pi;
		  alphan = (2*pi*j-pi+theta)/((double)(4*channelParams.nSinusoids));
		  tapWeight(i) += complex< double >(cos(psin)*cos(wd*cos(alphan)+phi), sin(psin)*cos(wd*cos(alphan)+phi));
		}
		tapWeight(i) = complex< double >(tapWeight(i).real()*(sqrt(2./(double)channelParams.nSinusoids)),tapWeight(i).imag()*(sqrt(2./(double)channelParams.nSinusoids)));
	  }
	  break;
	}
	default:
	{
	  cout<<"[both:]Error. Unsupported Channel Generation Method for generateChannelTaps()."<<endl;
	  abort();
	}
  }

  //applying the power profiles
  for(int tap_cnt=0;tap_cnt<channelParams.nTaps;tap_cnt++)
	tapWeight(tap_cnt)*=channelParams.tapGain(tap_cnt);

  return(tapWeight);

}

void ChannelCloud::generateTapWeightsForModifiedJakes(int rxNodeID, ivec txNodeIDs, double currentTimeInSeconds){
  
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[both:]Error. Given Rx node "<<rxNodeID<<" available in the channel."<<endl;
    abort();
  }
  
  int chInfoIndx;
  ivec txNodeIndices=find(nodesInfo.nodeList,txNodeIDs);
  vec randVec(0);
  Array<vec> randVecPerLink(txNodeIDs.length());
  
  double wd;
//   chInfoIndx=channelInfoIndx(rxNodeIndx,txNodeIndices(0));
//   if(chInfoIndx==-1)
//     wd=2*pi*sysWideChannelInfo.dopplerSpread*currentTimeInSeconds;
//   else
//     wd=2*pi*linkSpecificChannelInfo(chInfoIndx).dopplerSpread*currentTimeInSeconds;
  
  wd=2*pi*velocityMatrix(rxNodeIndx,txNodeIndices(0))*nodesInfo.carrierFrequency(rxNodeIndx)/lightSpeed*currentTimeInSeconds;
  
  for(int cnt=1;cnt<txNodeIDs.length();cnt++)
     if(wd!=2*pi*velocityMatrix(rxNodeIndx,txNodeIndices(0))*nodesInfo.carrierFrequency(rxNodeIndx)/lightSpeed*currentTimeInSeconds)
	 return;
//   {
//     if(chInfoIndx==-1)
//     {
//       if(wd!=2*pi*sysWideChannelInfo.dopplerSpread*currentTimeInSeconds)
// 	return;
//     }
//     else
//     {
//       if(wd!=2*pi*linkSpecificChannelInfo(chInfoIndx).dopplerSpread*currentTimeInSeconds)
// 	return;
//     }
//   }
  
  for(int cnt=0;cnt<txNodeIDs.length();cnt++)
  {
    chInfoIndx=channelInfoIndx(rxNodeIndx,txNodeIndices(cnt));
    int totalTaps,No;
    if(chInfoIndx==-1)
      totalTaps=sysWideChannelInfo.fadingParameters.nTaps*nodesInfo.antennaCount(rxNodeIndx)*nodesInfo.antennaCount(txNodeIndices(cnt));
    else
      totalTaps=linkSpecificChannelInfo(chInfoIndx).fadingParameters.nTaps*nodesInfo.antennaCount(rxNodeIndx)*nodesInfo.antennaCount(txNodeIndices(cnt));

    No=(int)pow(2.0,ceil_i(log2(totalTaps)));
    if(No<128)
      No=64; 

    randVecPerLink(cnt)=2*pi*rndVec_gen(rndSeed(rxNodeIndx)(txNodeIndices(cnt))(0),No);
    append(randVec,randVecPerLink(cnt));
    
  }
  
  int totalTapWeightsRequired=randVec.length();
  int No=(int)pow(2.0,ceil_i(log2(totalTapWeightsRequired)));
  
  cvec channelWeights(0);
  Array<cvec> channelWeightsPerLink(txNodeIDs.length());
  if(No<=modifiedJakesInfo.maximumNo)
  {
    modifiedJakesInfo.refreshWalshMatrix(No);
    vec alphan=modifiedJakesInfo.alphan(0,totalTapWeightsRequired-1);
    cvec temp(0);
    int start=0;
    for(int cnt=0;cnt<txNodeIDs.length();cnt++)
    {
      append(temp,to_cvec(cos(wd*cos(alphan(start,start+randVecPerLink(cnt).length()-1))+randVecPerLink(cnt))));
      start+=randVecPerLink(cnt).length();
    }
    
    channelWeights=modifiedJakesInfo.walshMatrix.get(0,totalTapWeightsRequired-1,0,No-1) * elem_mult(modifiedJakesInfo.ebetan.get(0,totalTapWeightsRequired-1),temp)*(sqrt(2.0/(double)No));
    start=0;
    for(int cnt=0;cnt<txNodeIDs.length();cnt++)
    {
      channelWeightsPerLink(cnt)=channelWeights(start,start+randVecPerLink(cnt).length()-1);
      start+=randVecPerLink(cnt).length();
    }
    modifiedJakesInfo.loadTapweights(txNodeIDs,rxNodeID,channelWeightsPerLink);
  }
  else
  {
    modifiedJakesInfo.refreshWalshMatrix();
    vec alphan=modifiedJakesInfo.alphan(0,totalTapWeightsRequired-1);
    cvec temp(0);
    int start=0;
    for(int cnt=0;cnt<txNodeIDs.length();cnt++)
    {
      append(temp,to_cvec(cos(wd*cos(alphan(start,start+randVecPerLink(cnt).length()-1))+randVecPerLink(cnt))));
      start+=randVecPerLink(cnt).length();
    }
    
    int cnt;
    for(cnt=0;cnt<(No/modifiedJakesInfo.maximumNo)-1;cnt++) // split the generation of walsh matrix if size is greater than maximum No
      append(channelWeights,modifiedJakesInfo.walshMatrix * elem_mult(modifiedJakesInfo.ebetan,temp(cnt*modifiedJakesInfo.maximumNo,(cnt+1)*modifiedJakesInfo.maximumNo-1))*(sqrt(2.0/(double)modifiedJakesInfo.maximumNo)));
    append(channelWeights,modifiedJakesInfo.walshMatrix * elem_mult(modifiedJakesInfo.ebetan,temp(cnt*modifiedJakesInfo.maximumNo,No-1))*(sqrt(2.0/(double)modifiedJakesInfo.maximumNo)));
    
    start=0;
    for(int cnt=0;cnt<txNodeIDs.length();cnt++)
    {
      channelWeightsPerLink(cnt)=channelWeights(start,start+randVecPerLink(cnt).length()-1);
      start+=randVecPerLink(cnt).length();
    }
    modifiedJakesInfo.loadTapweights(txNodeIDs,rxNodeID,channelWeightsPerLink);
  }
}

vec rndVec_gen(unsigned int state,int size)
{
  vec a(size);
  unsigned long long int updatestate;
  for(int cnt=0;cnt<size;cnt++)
  {
    updatestate=state*16807;
    state=updatestate%RAND_MAX;
    a(cnt)=((double)(state)/(double)RAND_MAX);
  }
  
  return a;
}

double rnd_gen(unsigned int &state)
{
  double a;
  unsigned long long int updatestate;
  updatestate=state*16807;
  state=updatestate%RAND_MAX;
  a=((double)(state)/(double)RAND_MAX);

  return a;
}

double gauss_rnd_gen(unsigned int &state)
{
  double a,b,r,g;
  double mu=0,var=0.5;
  unsigned long long int updatestate;

  updatestate=state*16807;
  state=updatestate%RAND_MAX;
  a=((double)(state)/(double)RAND_MAX);
  updatestate=state*16807;
  state=updatestate%RAND_MAX;
  b=(((double)(state)*2*3.14)/(double)RAND_MAX);
  if(a>=1) a=0.999999;
  r=sqrt(2*var*log(1/(1-a)));
  g=(r*cos(2*3.14*b))+mu;

  return g;
}

double ChannelCloud::getDopplerSpreadInHz(int txNodeID, int rxNodeID)
{
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[both:]Error. Given Rx node "<<rxNodeID<<" not available in the channel."<<endl;
    abort();
  }

  int txNodeIndx=find(nodesInfo.nodeList,txNodeID);
  if(txNodeIndx==-1)
  {
    cout<<"[both:]Error. Given Tx node "<<txNodeID<<" not available in the channel."<<endl;
    abort();
  }
  double dopplerSpreadInHz;
  dopplerSpreadInHz=velocityMatrix(rxNodeIndx,txNodeIndx)*nodesInfo.carrierFrequency(rxNodeIndx)/lightSpeed;

  return dopplerSpreadInHz;
}

double ChannelCloud::initializeSCMLink(int txNodeID, int rxNodeID, double carrierFrequencyInHz, SCMLargeScaleParameters_S scmLargeScaleParameters, Location_S txNodeLocation, Location_S rxNodeLocation,Antenna &txAntenna, Antenna &rxAntenna, bool isLOS, bool maintainReciprocity,bool addPathSpecificAntennaGain, bool reInitialize){
  
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  if(rxNodeIndx==-1)
  {
    cout<<"[both:]Error. Given Rx node "<<rxNodeID<<" not available in the channel."<<endl;
    abort();
  }

  int txNodeIndx=find(nodesInfo.nodeList,txNodeID);
  if(txNodeIndx==-1)
  {
    cout<<"[both:]Error. Given Tx node "<<txNodeID<<" not available in the channel."<<endl;
    abort();
  }
  
  // rndseed used for SCM initialization to manage SCM links same across cores in MPI simulation
  setSeed(rndSeed(rxNodeIndx)(txNodeIndx)(0));
  
  int chInfoIndx;
  SCMLink *mySCMLink;
 
  chInfoIndx=channelInfoIndx(rxNodeIndx,txNodeIndx);
  int mySCMLinkIndx=scmLinkIndx(rxNodeIndx,txNodeIndx);
  if(mySCMLinkIndx==-1)
  {
    append(scmLinks,SCMLink());
    mySCMLink=&scmLinks(scmLinks.length()-1);
    scmLinkIndx(rxNodeIndx,txNodeIndx)=scmLinks.length()-1;
    scmReciprocityFlag(rxNodeIndx,txNodeIndx)=0;
    if(maintainReciprocity)
    {
      scmLinkIndx(txNodeIndx,rxNodeIndx)=scmLinks.length()-1;
      scmReciprocityFlag(txNodeIndx,rxNodeIndx)=1;
    }
  }
  else
    mySCMLink=&scmLinks(mySCMLinkIndx);
  
  SCMPropagationScenario scmScenario;
  if(chInfoIndx==-1)
    scmScenario=sysWideChannelInfo.scmPropagationScenario;
  else
    scmScenario=linkSpecificChannelInfo(chInfoIndx).scmPropagationScenario;

  double dopplerSpreadInHz;
  dopplerSpreadInHz=velocityMatrix(rxNodeIndx,txNodeIndx)*nodesInfo.carrierFrequency(rxNodeIndx)/lightSpeed;

//   if(chInfoIndx==-1)
//     dopplerSpreadInHz=sysWideChannelInfo.dopplerSpread;
//   else
//     dopplerSpreadInHz=linkSpecificChannelInfo(chInfoIndx).dopplerSpread;

  bool isOutdoorToindoor;
  if((txNodeLocation.isIndoor && !rxNodeLocation.isIndoor) || (rxNodeLocation.isIndoor && !txNodeLocation.isIndoor))
    isOutdoorToindoor=true;
  else
    isOutdoorToindoor=false;
  double txAntSpacing=txAntenna.getAntennaWavelengthSpacing();
  double rxAntSpacing=rxAntenna.getAntennaWavelengthSpacing();
  if(txAntSpacing==-1 || rxAntSpacing==-1)
  {
    cout<<"[both:]Antenna spacing not initialized in the antenna object to initializeSCMLink()."<<endl;
    abort();
  }

  if(reInitialize || !mySCMLink->isSCMChannelInitialized())
  {
    mySCMLink->addPathSpecificAntennaGain = addPathSpecificAntennaGain;
    mySCMLink->propagationScenario = scmScenario;
    double samplingFrquencyInHz;
    if(nodesInfo.samplingFrequency.length()!=0)
      samplingFrquencyInHz=nodesInfo.samplingFrequency(rxNodeIndx);
    else
      samplingFrquencyInHz=commonSamplingFrequency;


    // Below code written to restrict the PDP generated by SCM not exceeding 200 samples - Dhiv
    
    double maxDelay=0;
    /*int maxAllowedTaps=200; // Assuming max cp wont be more than 200 samples
    int nTaps=0;
    
    do{
    */
    mySCMLink->enableFlatFading = enableFlatFading;
    mySCMLink->addOxygenLoss = addOxygenLoss;
    mySCMLink->useGroundReflection = useGroundReflection;
    maxDelay=mySCMLink->initializeFadingParams(isOutdoorToindoor,isLOS,dopplerSpreadInHz,carrierFrequencyInHz,scmScenario,scmLargeScaleParameters,txNodeLocation,rxNodeLocation,&txAntenna,&rxAntenna,txAntSpacing,rxAntSpacing,channelModelType);
//       nTaps=ceil_i(maxDelay*samplingFrquencyInHz);
//     }
//     while(nTaps>maxAllowedTaps);
//     
  
    return(maxDelay);
  }
  else
    cout<<"[detl:]Warning SCM link between "<<txNodeID<<" , "<<rxNodeID<<" is initialized already."<<endl;
  
  return(max(mySCMLink->getTapDelays()));
}

void ChannelCloud::preAllocateSCMLink(ivec rxNodeIDs, Array<ivec> txNodeIDsPerRxNode, bool maintainReciprocity){
//   scmLinkIndx
//   cout<<"rxNodeIDsIn Allocation:"<<rxNodeIDs.length() <<endl;
//   cout<<"txNodeIDsPerRxNode:"<<txNodeIDsPerRxNode.length() <<endl;abort();
  
  if(rxNodeIDs.length()!=txNodeIDsPerRxNode.length())
  {
    cout<<"[both:]Error. Length mismatch in preAllocateSCMLink()."<<endl;
    abort();
  }
  
  ivec rxNodeIndx=find(nodesInfo.nodeList,rxNodeIDs);
  if(find(rxNodeIndx,-1)!=-1)
  {
    cout<<"[both:]Error. Some of the given Rx nodes not available in the channel in preAllocateSCMLink()."<<endl;
    abort();
  }
  
  int linkIndx=scmLinks.length();
  Array<ivec> txNodeIndx(rxNodeIDs.length());
  for(int rx_cnt=0;rx_cnt<txNodeIDsPerRxNode.length();rx_cnt++)
  if(txNodeIDsPerRxNode(rx_cnt).length()!=0)  
  {
    txNodeIndx(rx_cnt)=find(nodesInfo.nodeList,txNodeIDsPerRxNode(rx_cnt));
    if(find(txNodeIndx(rx_cnt),-1)!=-1)
    {
      cout<<"[both:]Error. Some of the given Tx nodes not available in the channel in preAllocateSCMLink()."<<endl;
      abort();
    }
    for(int link_cnt=0;link_cnt<txNodeIndx(rx_cnt).length();link_cnt++)
    {
      if(!maintainReciprocity)
      {	
	if(scmLinkIndx(rxNodeIndx(rx_cnt),txNodeIndx(rx_cnt)(link_cnt))==-1)
	{
	  scmLinkIndx(rxNodeIndx(rx_cnt),txNodeIndx(rx_cnt)(link_cnt))=linkIndx;
	  scmReciprocityFlag(rxNodeIndx(rx_cnt),txNodeIndx(rx_cnt)(link_cnt))=0;
	  linkIndx++;
	}
      }
      else
      {
	if(scmLinkIndx(rxNodeIndx(rx_cnt),txNodeIndx(rx_cnt)(link_cnt))==-1 && scmLinkIndx(txNodeIndx(rx_cnt)(link_cnt),rxNodeIndx(rx_cnt))==-1)
	{
	  scmLinkIndx(rxNodeIndx(rx_cnt),txNodeIndx(rx_cnt)(link_cnt))=linkIndx;
	  scmReciprocityFlag(rxNodeIndx(rx_cnt),txNodeIndx(rx_cnt)(link_cnt))=0;
	  scmLinkIndx(txNodeIndx(rx_cnt)(link_cnt),rxNodeIndx(rx_cnt))=linkIndx;
	  scmReciprocityFlag(txNodeIndx(rx_cnt)(link_cnt),rxNodeIndx(rx_cnt))=1;
	  linkIndx++;
	}
	else
	{
	  if(scmLinkIndx(rxNodeIndx(rx_cnt),txNodeIndx(rx_cnt)(link_cnt))==-1)
	  {
	    scmLinkIndx(rxNodeIndx(rx_cnt),txNodeIndx(rx_cnt)(link_cnt))=scmLinkIndx(txNodeIndx(rx_cnt)(link_cnt),rxNodeIndx(rx_cnt));
	    scmReciprocityFlag(rxNodeIndx(rx_cnt),txNodeIndx(rx_cnt)(link_cnt))=1;
	  }
	  else
	  {
	    scmLinkIndx(txNodeIndx(rx_cnt)(link_cnt),rxNodeIndx(rx_cnt))=scmLinkIndx(rxNodeIndx(rx_cnt),txNodeIndx(rx_cnt)(link_cnt));
	    scmReciprocityFlag(txNodeIndx(rx_cnt)(link_cnt),rxNodeIndx(rx_cnt))=1;
	  }
	}
      }
	
	
    }
  }
  scmLinks.set_length(linkIndx,true);
}

void ChannelCloud::preAllocateSCMLink(int rxNodeID,int txNodeID,bool maintainReciprocity)
{
    int txNodeIndx=find(nodesInfo.nodeList,txNodeID);
    if(txNodeIndx==-1)
    {
        cout<<"[both:]Error. Some of the given Rx nodes not available in the channel in preAllocateSCMLink()."<<endl;
        abort();
    }
    int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
    if(rxNodeIndx==-1)
    {
        cout<<"[both:]Error. Some of the given Rx nodes not available in the channel in preAllocateSCMLink()."<<endl;
        abort();
    }
    int linkIndx=scmLinks.length();
    
    if(!maintainReciprocity)
    {
        if(scmLinkIndx(rxNodeIndx,txNodeIndx) == -1)
        {
            scmLinkIndx(rxNodeIndx,txNodeIndx) = linkIndx;
            scmReciprocityFlag(rxNodeIndx,txNodeIndx) = 0;
            linkIndx++;
        }
    }
    else
    {
        if(scmLinkIndx(rxNodeIndx,txNodeIndx) == -1 && scmLinkIndx(txNodeIndx,rxNodeIndx) == -1)
        {
            scmLinkIndx(rxNodeIndx,txNodeIndx) = linkIndx;
            scmReciprocityFlag(rxNodeIndx,txNodeIndx) = 1;
            scmLinkIndx(txNodeIndx,rxNodeIndx) = linkIndx;
            scmReciprocityFlag(txNodeIndx,rxNodeIndx) = 1;
            linkIndx++;
        }
        else 
        {
            if(scmLinkIndx(rxNodeIndx,txNodeIndx)==-1)
            {
                scmLinkIndx(rxNodeIndx,txNodeIndx) = scmLinkIndx(txNodeIndx,rxNodeIndx);
                scmReciprocityFlag(rxNodeIndx,txNodeIndx) = 1;
            }
            else if(scmLinkIndx(txNodeIndx,rxNodeIndx) == -1)
            {
                scmLinkIndx(txNodeIndx,rxNodeIndx) = scmLinkIndx(rxNodeIndx,txNodeIndx);
                scmReciprocityFlag(txNodeIndx,rxNodeIndx) = 1;
            }
        }
    }
    scmLinks.set_length(linkIndx,true);
}

void ChannelCloud::preAllocateSCMLink(Array<ivec> rxNodeIDsPerTxNode, ivec txNodeIDs, bool maintainReciprocity){
  
  if(rxNodeIDsPerTxNode.length()!=txNodeIDs.length())
  {
    cout<<"[both:]Error. Length mismatch in preAllocateSCMLink()."<<endl;
    abort();
  }
  
  ivec txNodeIndx=find(nodesInfo.nodeList,txNodeIDs);
  if(find(txNodeIndx,-1)!=-1)
  {
    cout<<"[both:]Error. Some of the given Rx nodes not available in the channel in preAllocateSCMLink()."<<endl;
    abort();
  }
  
  int linkIndx=scmLinks.length();
  Array<ivec> rxNodeIndxPerTx(txNodeIDs.length());
  for(int tx_cnt=0;tx_cnt<rxNodeIDsPerTxNode.length();tx_cnt++)
  {
    rxNodeIndxPerTx(tx_cnt)=find(nodesInfo.nodeList,rxNodeIDsPerTxNode(tx_cnt));
    if(find(rxNodeIndxPerTx(tx_cnt),-1)!=-1)
    {
      cout<<"[both:]Error. Some of the given Rx nodes not available in the channel in preAllocateSCMLink()."<<endl;
      abort();
    }
    for(int link_cnt=0;link_cnt<rxNodeIndxPerTx(tx_cnt).length();link_cnt++)
    {
      if(!maintainReciprocity)
      {
	if(scmLinkIndx(rxNodeIndxPerTx(tx_cnt)(link_cnt),txNodeIndx(tx_cnt))==-1)
	{
	  scmLinkIndx(rxNodeIndxPerTx(tx_cnt)(link_cnt),txNodeIndx(tx_cnt))=linkIndx;
	  scmReciprocityFlag(rxNodeIndxPerTx(tx_cnt)(link_cnt),txNodeIndx(tx_cnt))=0;
	  linkIndx++;
	}
      }
      else
      {
	if(scmLinkIndx(rxNodeIndxPerTx(tx_cnt)(link_cnt),txNodeIndx(tx_cnt))==-1 && scmLinkIndx(txNodeIndx(tx_cnt),rxNodeIndxPerTx(tx_cnt)(link_cnt))==-1)
	{
	  scmLinkIndx(rxNodeIndxPerTx(tx_cnt)(link_cnt),txNodeIndx(tx_cnt))=linkIndx;
	  scmReciprocityFlag(rxNodeIndxPerTx(tx_cnt)(link_cnt),txNodeIndx(tx_cnt))=0;
	  scmLinkIndx(txNodeIndx(tx_cnt),rxNodeIndxPerTx(tx_cnt)(link_cnt))=linkIndx;
	  scmReciprocityFlag(txNodeIndx(tx_cnt),rxNodeIndxPerTx(tx_cnt)(link_cnt))=1;
	  linkIndx++;
	}
	else
	{
	  if(scmLinkIndx(rxNodeIndxPerTx(tx_cnt)(link_cnt),txNodeIndx(tx_cnt))==-1)
	  {
	    scmLinkIndx(rxNodeIndxPerTx(tx_cnt)(link_cnt),txNodeIndx(tx_cnt))=scmLinkIndx(txNodeIndx(tx_cnt),rxNodeIndxPerTx(tx_cnt)(link_cnt));
	    scmReciprocityFlag(rxNodeIndxPerTx(tx_cnt)(link_cnt),txNodeIndx(tx_cnt))=1;
	  }
	  else
	  {
	    scmLinkIndx(txNodeIndx(tx_cnt),rxNodeIndxPerTx(tx_cnt)(link_cnt))=scmLinkIndx(rxNodeIndxPerTx(tx_cnt)(link_cnt),txNodeIndx(tx_cnt));
	    scmReciprocityFlag(txNodeIndx(tx_cnt),rxNodeIndxPerTx(tx_cnt)(link_cnt))=1;
	  }
	}
      }	
    }
  }
  scmLinks.set_length(linkIndx,true);
}

bool ChannelCloud::isSCMLinkInitialized(int txNodeID, int rxNodeID){

  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  int txNodeIndx=find(nodesInfo.nodeList,txNodeID);
  if(txNodeIndx==-1 || rxNodeIndx==-1)
  {
    cout<<rxNodeID<<","<<txNodeID<<endl;
    cout<<"[both:]Unknown or un-paired nodeIDs to isSCMLinkInitialized().";abort();
  }

  if(scmLinkIndx(rxNodeIndx,txNodeIndx)==-1)
    return(false);
  else
    return(true);
  
}

void ChannelCloud::clearSCMFixedTapGains()
{
    for(int i=0;i<scmLinks.length();i++)
        scmLinks(i).clearTapGains();
}

Array<cmat> ChannelCloud::getAntennaCorrelationMatrices(int txAntennaCount, int rxAntennaCount){
  
  Array<cmat> correlationMatrices;
  correlationMatrices.set_size(2);
  
  correlationMatrices(0)=eye_c(txAntennaCount);
  correlationMatrices(1)=eye_c(rxAntennaCount);
  
  if(CorrMatrix.antennaCorrelation!=_ANTENNA_CORRELATION_UNCORRELATED_)
  {
    if(txAntennaCount==1)
      correlationMatrices(0) = to_cmat(CorrMatrix.Tx1Ant);
    else if(txAntennaCount==2)
      correlationMatrices(0) = to_cmat(CorrMatrix.Tx2Ant);
    else if(txAntennaCount==4)
      correlationMatrices(0) = to_cmat(CorrMatrix.Tx4Ant);
    else
    {cout<<"[both:] Error ... Correlation Matrix For Antenna defined only for 1,2,4 Tx antennnas at BS.. ...";abort();}
    
    
    if(rxAntennaCount==1)
      correlationMatrices(1) = to_cmat(CorrMatrix.Rx1Ant);
    else if(rxAntennaCount==2)
      correlationMatrices(1) =to_cmat(CorrMatrix.Rx2Ant);
    else if(rxAntennaCount==4)
      correlationMatrices(1) = to_cmat(CorrMatrix.Rx4Ant);
    else
    {cout<<"[both:] Error ... Correlation Matrix For Antenna defined only for 1,2,4 Tx antennnas at BS.. ...";abort();}
  }
  return(correlationMatrices);
}

// Code included for PN

PhaseNoiseParams_S generatePNPSD(int pnPSDID, double carrierFreq){
  
 PhaseNoiseParams_S tempPNParams;

 /****  
  *  Procure the PSD corresponding to the given PN ID
  *  Convert the PSD to the required carrier frequency and return the structure
  ****/
 
  switch(pnPSDID){
   
   case 1:
   
     tempPNParams.freqVal = "1e3 1e4 1e5 1e6 1e7 1e8";
     tempPNParams.PSDValues = "-79.4 -79.45 -83.35 -111.67 -128.8 -138.6";
     tempPNParams.fcBase = 30e9;    
     break;  
     
     case 2:
   
     tempPNParams.freqVal = "1e3 1e4 1e5 1e6 1e7 1e8";
     tempPNParams.PSDValues = "-70.16 -76.02 -82.26 -96.25 -125.67 -129.94" ;
     tempPNParams.fcBase = 60e9;
     break;
   
 
   case 3:
     
     tempPNParams.freqVal = "1e3 1e4 1e5 1e6 1e7 1e8";
     tempPNParams.PSDValues = "-71.08 -80.3 -83.05 -98.33 -118.28 -131.47 ";
     tempPNParams.fcBase = 30e9;
     break;
   
   case 4:
   
     tempPNParams.freqVal = "1e3 1e4 1e5 1e6 1e7 1e8";
     tempPNParams.PSDValues = "-70.67 -78.09 -80.69 -105.27 -122.91 -125.12" ;
     tempPNParams.fcBase = 70e9;
     break;
   
   case 5:
   
     tempPNParams.freqVal = "1e3 1e4 1e5 1e6 1e7 1e8";
     tempPNParams.PSDValues = "-60 -70 -79 -85 -110 -125 -126" ;
     tempPNParams.fcBase = 50e9;
     break;
   
   case 6:
   
     tempPNParams.freqVal = "1e3 1e4 1e5 1e6 1e7 1e8";
     tempPNParams.PSDValues = "-70 -84 -92 -100 -128 -136" ;
     tempPNParams.fcBase = 50e9;
     break;
   
 } 
 
 
 for(int freqIdx = 0; freqIdx < 6; freqIdx++) 
   tempPNParams.PSDValues(freqIdx)  +=  20*log10(carrierFreq/tempPNParams.fcBase);
    
 return(tempPNParams); 
  
}

cvec ChannelCloud::generatePNSamples(int rxNodeID,int pnPSDID,int numPNSamples){
  
  # define eps 0.0001
  
  int rxNodeIndx=find(nodesInfo.nodeList,rxNodeID);
  
  double samplingFrquencyInHz;
  if(nodesInfo.samplingFrequency.length()!=0)
    samplingFrquencyInHz=nodesInfo.samplingFrequency(rxNodeIndx);
  else
    samplingFrquencyInHz=commonSamplingFrequency;
  
  double carrierFrequencyInHz=nodesInfo.carrierFrequency(rxNodeIndx);
  PhaseNoiseParams_S tempPNParams;
  int psdFreqLength = 6;
  int idx, idx2, numFinePoints = numPNSamples * 1;
  double psdFreqPoints[psdFreqLength],psdValueCarrierFreq[psdFreqLength];
  double fineFreqPoints[numFinePoints/2], finePSDPointsSSB[numFinePoints/2];
  double psdDb[numFinePoints],psdLinear[numFinePoints];
  cvec randVal(numFinePoints);
  cvec phaseNoiseFreqVal(numFinePoints);
  cvec phaseNoiseTimeAll(numFinePoints); 
  cvec phasorValue(numPNSamples);
  double slope[psdFreqLength];
  
  
  // Call function to fetch PSD values and assign to local variables
  
  tempPNParams = generatePNPSD(pnPSDID,carrierFrequencyInHz);
  
  for (idx=0;idx < psdFreqLength; idx++){
    psdFreqPoints[idx] = tempPNParams.freqVal(idx);
    psdValueCarrierFreq[idx] = tempPNParams.PSDValues(idx);
  }
  // Generate vector of fine frequency points
  
  for(idx = 0;idx <= (numFinePoints/2)-1;idx++) {
    fineFreqPoints[idx] = (float(idx)/numFinePoints) * samplingFrquencyInHz;
  }
  double deltaFreq = fineFreqPoints[2] - fineFreqPoints[1];

  // Calculate slope of PSD and create a vector of PSD values with fine frequency granularity
  int startIdx =0;
  
  while(fineFreqPoints[startIdx] <=  psdFreqPoints[0])
  {
    finePSDPointsSSB[startIdx] = psdValueCarrierFreq[0];
    startIdx += 1;
  }

  for(idx = 0; idx < psdFreqLength-1; idx++)
  {
    double denSlope = log10(psdFreqPoints[idx+1])-log10(psdFreqPoints[idx]);
    slope[idx] = (psdValueCarrierFreq[idx+1] - psdValueCarrierFreq[idx])/denSlope;
  
    for(idx2 = startIdx;idx2 < numFinePoints/2;idx2++)
    {
      if (fineFreqPoints[idx2] <= psdFreqPoints[idx+1])
      {
	
	finePSDPointsSSB[idx2] = slope[idx] * log10(fineFreqPoints[idx2]+eps) + psdValueCarrierFreq[idx]
	- slope[idx] * log10(psdFreqPoints[idx]+eps);
      }
      else 
	break;
      
    }
    startIdx = idx2;
  
  }
  
  // Create two sided PSD from single sided PSD and create PSD in linear scale
  
  for(idx=0;idx<=numFinePoints/2-1;idx++)
  {
    psdDb[idx] = finePSDPointsSSB[numFinePoints/2-1-idx]; 
    psdDb[idx+(numFinePoints/2)] = finePSDPointsSSB[idx];
    psdLinear[idx] =  numFinePoints * sqrt(deltaFreq) * pow(10,(psdDb[idx]/20));
    psdLinear[idx+(numFinePoints/2)] =  numFinePoints * sqrt(deltaFreq) * pow(10,(psdDb[idx+(numFinePoints/2)]/20));
  }

  
  // Generate PN values using PSD linear values
  
  randVal = randn_c(numFinePoints)  ;  

  for (idx = 0; idx < numFinePoints; idx++){
    phaseNoiseFreqVal(idx) =  randVal(idx) * psdLinear[idx];
  }
  
  phaseNoiseTimeAll = ifft(fftShift(phaseNoiseFreqVal)) ;   // Insert the existing IFFT function
  
  // check if ifftshift needs to be included ****
  
    
  for(idx = 0;idx<numPNSamples;idx++)
    phasorValue(idx) = complex< double >(cos(phaseNoiseTimeAll(idx).real()),sin(phaseNoiseTimeAll(idx).real()));
  return(phasorValue);
}

void ChannelCloud::printAntennaGains(ivec serverNodes,ivec serviceNodes,string fileName)
{
    ofstream op;
    string tempFileName = fileName+".temp"+toString(currentTaskid);
    op.open(tempFileName.c_str(),ios::out);
    
    if(currentTaskid==0)
        op<<"%rxNodeID,\t txNodeID,\t txElementGain ,\t rxElementGain,\t txAntennaLOSGain,\t txAntennaAvgGain,\t rxAntennaLOSGain,\t rxAntennaAvgGain ,\t linkCondition ,\t NOTE: all are power gains and are in dB"<<endl;
    
    for(int srvc_cnt=0;srvc_cnt<serviceNodes.length();srvc_cnt++)
    {
        int rxNodeIndx = find(nodesInfo.nodeList,serviceNodes(srvc_cnt));
        int txNodeIndx = find(nodesInfo.nodeList,serverNodes(srvc_cnt));
        int scmIndx = scmLinkIndx(rxNodeIndx,txNodeIndx);
        if(scmIndx!=-1)
        {
            op<<serviceNodes(srvc_cnt)<<",\t "<<serverNodes(srvc_cnt)<<",\t ";
            scmLinks(scmIndx).printAntennaGains(op);
                
            SCMLinkCondition linkCondition = scmLinks(scmIndx).scm5GLink->scmLinkCondition;
            op<<",\t"<<getString(linkCondition)<<endl;
        }
    }
}

double ChannelCloud::getChannelGain(int rxNodeID,int txNodeID)
{
    int rxNodeIndx = find(nodesInfo.nodeList,rxNodeID);
    int txNodeIndx = find(nodesInfo.nodeList,txNodeID);
    
    double channelGain = linkGainInDB(rxNodeIndx,txNodeIndx);
    //cout << "channelGain01 :"<< channelGain<< endl;
    ch_gain<<channelGain<<endl;
    if(addMultiPathFading && channelGenerationMethod==_CHANNEL_GENERATION_METHOD_SCM_)
    {
        //abort();
        int scmIndx = scmLinkIndx(rxNodeIndx,txNodeIndx);
        channelGain -=  scmLinks(scmIndx).getLOSAntennaGainInDB(0,0);
        ch_gain<<channelGain<<endl;
    }
    //abort();
    return channelGain;
}
double ChannelCloud::getLinkGainInDB(int rxNodeID,int txNodeID)
{
    int rxNodeIndx = find(nodesInfo.nodeList,rxNodeID);
    int txNodeIndx = find(nodesInfo.nodeList,txNodeID);
    
    return linkGainInDB(rxNodeIndx,txNodeIndx);
}

