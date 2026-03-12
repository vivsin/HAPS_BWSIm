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



# ifndef _CHN_EST_H_
# define _CHN_EST_H_

# include "SupportingFunctions.h"

class ChannelEstimator{
  
  private:
	double samplingPeriod;
	double samplingFrequency;
	double frequencySpacing;
	int fftSize;
	
  public:
	ChannelEstimator();
	bool isReady();
	void loadParams(double channelSamplingPeriod, double channelFrequencySpacing, int framefftSize);
	void setChannelSamplingPeriod(double period);
	void setChannelFrequencySpacing(double freqSpacing);
	void setFFTSize(int size);
	void setNoiseVariance(double variance);
	
	void estimateMyChannelInRB(Array< Array<cmat> > &estimatedChannelInRB, Array<cmat> &receivedRB, double channelGainInDB, vec interfererRxSCPowerInDB, Array<cvec> &txRSPerPort,double noiseVariance, double restOfintereferencePowerInWatts, double dopplerSpreadInHz, int nTaps, vec tapDel, vec tapGain, ivec portReferenceNumbers, imat &rbRSPattern, ivec portIndices, int rbNumber,bool isEPDCCH=false);
	void estimateMyChannelInRB(Array< Array<cmat> > &estimatedChannelInRB, Array<cmat> &receivedRB, double myAttenuationFactorIndB, cmat estimatedIntrCovInRB, Array<cvec> &txRSPerPort, double dopplerSpreadInHz, int nTaps, vec tapDel, vec tapGain, ivec portReferenceNumbers, imat &rbRSPattern, ivec portIndices, int rbNumber, bool isEPDCCH=false);
    void estimateBy2DMMSE(cmat &estimatedIntrCovInRB, cvec &txRS, cvec rxRS, imat &rbRSPattern, ivec portLocations, double noiseVariance, double myPathlossFactorInWatts, vec interferersPowerInWatts, double restOfintereferencePowerInWatts, double dopplerSpread, int nTaps, vec tapDel, vec tapGain);
	void estimateBy2DMMSE(cmat &estimatedChannel, cvec &txRS, cvec rxRS, imat &rbRSPattern, ivec portLocations, double myAttenuationFactorInWatts, double interferencePowerInWatts, double dopplerSpread, int nTaps, vec tapDel, vec tapGain);
    double estimateInterferenceCovarianceInRB(cmat& estimatedIntrCovInRB, Array< cmat >& receivedRB, Array< Array< cmat > >& myChannelForRB, Array< cvec >& txRSPerPort, TransmissionScheme_E myTransmissionScheme, imat& rbRSPattern, ivec ports, ivec portReferenceNumbers, ivec otherPorts="", ivec otherPortRefNumbers="");
	cvec estimateChannelinSymbolBy1DMMSE(cvec txRSPerPort, cvec rxRS, imat rsSymbol, ivec portLocations,double noiseVariance, double myPathlossFactorInWatts, vec interferersPowerInWatts,double restOfintereferencePowerInWatts, double dopplerSpread, int nTaps, vec tapDel, vec tapGain);
	Array< Array<cmat> > estimateWideBandChannel(Array< Array<cvec> > &txRSPerPortPerSymbol,int guardSubCarriersOnOneSide, Array<cmat> &rxSubFrame, imat &subFramePattern, Array<ivec> &RSsymbols, vec interferersRxPowerInWatts,double restOfintereferencePowerInWatts, double myChannelGainInDB,double noiseVariance, double dopplerSpreadInHz, ivec crsPortReferenceNumbers ,int nTaps, vec tapDel, vec tapGain, RSType_E CQIrsType, ivec portIndices);
	ivec generatecrosscorrpattern(ivec rsloc,int channelheight,int channelwidth,int nRStones);
	ivec generateautocorrpattern(ivec rsloc,int nRStones);
	
	mat chanErrModel(imat &rbRSPattern, ivec portLocations, double noiseVariance, double myAttenuationFactorInWatts, double interferersPowerInWatts, double restOfintereferencePowerInWatts, double dopplerSpread, int nTaps, vec tapDel, vec tapGain);
	Array<Array<cmat>>  errModelWideBandChannel(Array<Array<cmat>>  &idealChannel, imat &subFramePattern,  double interferersRxPowerInWatts,double restOfintereferencePowerInWatts, double myAttenuationFactorIndB,double noiseVariance, double dopplerSpreadInHz, ivec CQIportReferenceNumbers ,int nTaps, vec tapDel, vec tapGain, ivec portIndices);
	Array<cmat> errModelMyChannelInRB(Array<cmat>  &idealChannelInRB, Array<cmat> &receivedRB, double myAttenuationFactorIndB, double interfererRxSCPowerInDB, double noiseVariance, double restOfintereferencePowerInWatts, double dopplerSpreadInHz, int nTaps, vec tapDel, vec tapGain, ivec portReferenceNumbers, imat &rbRSPattern, ivec portIndices);
	

private:
	cmat findchannelcorr(ivec correlationPattern,int rows,int cols,double doppler, int nTaps, vec tapDel, vec tapGain, double samplingperiod,double freqspacing);
	complex<double> correlationmatx(double freqdiff,double timediff,double doppler, int nTaps, vec tapDel, vec tapGain,double samplingperiod,double freqspacing);
	ivec getRStoInterpolate(int sym, ivec RSsymbols);
	cmat constructCovMatritx(Array< Array<cvec> > Intrference, int rsloc, int rxAntenna, int nPorts);
};

Array<cmat> repeatPerToneInterferenceCovariance(cmat interferenceCovariance, int dataTonesPerRB);


#endif

