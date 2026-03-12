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

#include "../include/Equalizer.h"

EqualizerOutput_S equlizeOfdmaUplink(Array<cvec> &rxDataPerRB, Array< cmat > &precodedChannelPerRB, Array< Array< cmat >  > &intrPrecodedChannelPerRB,ivec intrSFBCLayers, double txScPowInWatts, vec &intrTxPowInWatts, Array<cmat> &estIntrCovPerRB, double restOfInterferenceWithNoiseVariance, DemodulationScheme_E demodulationScheme, bool isEstimatedIntrcov)
{
  int nLayers=precodedChannelPerRB(0).cols();
  int nRxAntenna=precodedChannelPerRB(0).rows();
  int dataTonesPerRB=rxDataPerRB(0).length();
  cmat noiseWithROI(nRxAntenna,nRxAntenna);
  cmat IntrCov(nRxAntenna,nRxAntenna);
  cmat estsym(nLayers,1);
  vec ppSINR(nLayers);
  cmat precodedChannel;
  cmat rxsym(nRxAntenna,1);
  EqualizerOutput_S equalizerOutput;
  equalizerOutput.equalizedChannel.set_size(nLayers);
  equalizerOutput.equalizedSymbols.set_size(nLayers);
  equalizerOutput.equalizedNoiseVariance.set_size(nLayers);
  equalizerOutput.postProcessingSinr.set_size(nLayers);
  // set the size of equalizedSymbols for all the layers 
  for(int iLayer=0;iLayer <nLayers; iLayer++)
  {
    equalizerOutput.equalizedSymbols(iLayer).set_size(dataTonesPerRB);
    equalizerOutput.equalizedChannel(iLayer).set_size(dataTonesPerRB);
    equalizerOutput.equalizedNoiseVariance(iLayer).set_size(dataTonesPerRB);
    equalizerOutput.postProcessingSinr(iLayer).set_size(dataTonesPerRB);
  }
  IntrCov.clear();
  int nInterferers=intrPrecodedChannelPerRB.length();
  
  Array<cmat> IntrChan(nInterferers);
  
  cmat EquNoise(nLayers ,nLayers);
  cmat eqChn(nLayers,nLayers);
  // this loop executes (total tones in an RB-crs tones) times for crs port-0
  for(int j = 0; j < dataTonesPerRB; j++)
  {
    precodedChannel=sqrt((double)txScPowInWatts)*precodedChannelPerRB(j);
    
    for (int rxant=0;rxant <nRxAntenna; rxant++)
      rxsym(rxant) = rxDataPerRB(rxant)(j);
    
    noiseWithROI = (restOfInterferenceWithNoiseVariance)*eye_c(nRxAntenna);
    
    if(!isEstimatedIntrcov)
    {
      // If the number of interferers are greater than one calculate the interference covariance matrix 
      if(nInterferers)
      {
	IntrCov.clear();
	for (int intr = 0;intr <nInterferers; intr++)
	{
	  if(intrSFBCLayers(intr))
	  {
	    int intrLayers=intrSFBCLayers(intr);
	    int scindx=j/intrLayers;
	    int scLocalIndx=j%intrLayers;
	    cmat intrChannel=sqrt(intrTxPowInWatts(intr))*intrPrecodedChannelPerRB(intr)(scindx);
	    cmat interferenceCov=intrChannel*intrChannel.H();
	    
	    interferenceCov=interferenceCov.get_cols(scLocalIndx*nRxAntenna,(scLocalIndx+1)*nRxAntenna-1);
	    interferenceCov = interferenceCov.get_rows(scLocalIndx*nRxAntenna,(scLocalIndx+1)*nRxAntenna-1);
	    
	    if(scLocalIndx==1 || scLocalIndx==3) 
	      interferenceCov=conj(interferenceCov);
	    
	    IntrCov = IntrCov + interferenceCov;
	  }
	  else
	  {
	    IntrChan(intr)=sqrt(intrTxPowInWatts(intr))*intrPrecodedChannelPerRB(intr)(j);
	    if(IntrChan(intr).size()==0)
	      cout<<"error "<<endl;
	    IntrCov = IntrCov + IntrChan(intr)*IntrChan(intr).hermitian_transpose();
	  }
	}
	IntrCov = IntrCov + noiseWithROI;
      }
      else
	IntrCov = noiseWithROI;
    }
    else
      IntrCov=estIntrCovPerRB(j);
    
    
    cmat equFilter;
    // switch statement selects the MMSE (or) MRC based on the demodulation scheme
    switch(demodulationScheme)
    {
      case _DEMODULATION_SCHEME_MMSE_:
	equFilter  =  sqrt(txScPowInWatts)*precodedChannel.hermitian_transpose()*inv( (precodedChannel*precodedChannel.hermitian_transpose())+IntrCov);
	break;
      case _DEMODULATION_SCHEME_MRC_:
	equFilter  = hermitian_transpose(precodedChannel)/sqrt((double)txScPowInWatts);
	break;
      case _DEMODULATION_SCHEME_WHITE_MRC_:
      {
	complex< double > noiseVariance= (trace(IntrCov)/complex< double >(nRxAntenna,0));// Taking avg interference power across antennas
	equFilter  =  sqrt((double)txScPowInWatts)*hermitian_transpose(precodedChannel)/((hermitian_transpose(precodedChannel)*precodedChannel)(0,0)+noiseVariance);
	break;
      }
      default:
      {cout<<"[both:] Demodulation scheme not valid . . . in equlizeOfdma();";abort();}
      
    }
    
    estsym  =  equFilter*rxsym;
    eqChn   =  equFilter*precodedChannel;
    EquNoise = equFilter*IntrCov*hermitian_transpose(equFilter);
    ppSINR.set_size(eqChn.rows());
    
    // Included non-diagonal terms in the noise variance 
    cvec temp=diag(eqChn);
    cmat desiredEquChn=diag(temp);
    cmat residualISI=eqChn-desiredEquChn;
    EquNoise+=residualISI*residualISI.hermitian_transpose();
    eqChn=desiredEquChn;
    
    //PPSINR will be computed later in time-domain
    for (int iLayer =0;iLayer <nLayers;iLayer++) 
      ppSINR(iLayer)=-1;
    
    
    for(int iLayer=0;iLayer <nLayers; iLayer++)
      equalizerOutput.equalizedSymbols(iLayer).set(j,estsym(iLayer));
    for(int iLayer=0;iLayer <nLayers; iLayer++)
      equalizerOutput.equalizedChannel(iLayer).set(j,eqChn(iLayer,iLayer));
    for(int iLayer=0;iLayer <nLayers; iLayer++)
      equalizerOutput.equalizedNoiseVariance(iLayer).set(j,abs(EquNoise(iLayer,iLayer)));
    for(int iLayer=0;iLayer <nLayers; iLayer++)
      equalizerOutput.postProcessingSinr(iLayer).set(j,ppSINR(iLayer));
  }
  
  return(equalizerOutput);
}



/*! \fn equalizeMyDataInRB(...)
* \brief returns the equalized symbol and channel for input transmission mode
* \param [in,out] dataBlkInfo ---> gives the transmission scheme 
* \param [in,out] rxDataPerRB ---> contains the receiver data on each antenna and the length of rxDataPerRB is equal to the number of receiver antennas
* \param [in,out] precodedChannelPerRB ---> precoded channel per resource block which depends on the number of receiver antennas and number of layers(Nr x Nl X 152)
* \param [in] intrPrecodedChannelPerRB ---> The strongest 8 interference channels for all tones and layers for an RB, Interference channel per RB which depends on the number of interferences, receiver antennas and number of layers(Ni x Nr x Nl X 152) 
* \param [in] txScPowInWatts ---> per subcarrier transmitted power
* \param [in] intrTxPowInWatts ---> Interference transmitted power per subcarrier
* \param [in,out] estIntrCovPerRB ---> Interference covariance is estimated using the equation (y-Hp)
* \param [in] restOfInterferenceWithNoiseVariance ---> This term is the combination of noise and weak interference variances
* \param [in] demodulationScheme ---> MMSE or MRC 
* \param [in] isEstimatedIntrcov ---> It is a bool variable which tells wheather the interference covariance is calculated with or without interference channel kmowledge 
* \return EqualizerOutput_S*/
EqualizerOutput_S equalizeMyDataInRB(Array<cvec> &rxDataPerRB, Array< cmat > &precodedChannelPerRB, Array< Array< cmat >  > intrPrecodedChannelPerRB,ivec intrSFBCLayers, double txScPowInWatts, vec intrTxPowInWatts, Array<cmat> &estIntrCovPerRB, double restOfInterferenceWithNoiseVariance, DemodulationScheme_E demodulationScheme, bool isEstimatedIntrcov, Waveform_E waveform){
  
  if(waveform == _Waveform_OFDMA_)
  {
    return(equlizeOfdma(rxDataPerRB, precodedChannelPerRB, intrPrecodedChannelPerRB,intrSFBCLayers, txScPowInWatts,intrTxPowInWatts, estIntrCovPerRB, restOfInterferenceWithNoiseVariance, demodulationScheme, isEstimatedIntrcov));
  }
  else if(waveform == _Waveform_SCFDMA_)
  {
    return(equlizeOfdmaUplink(rxDataPerRB, precodedChannelPerRB, intrPrecodedChannelPerRB,intrSFBCLayers, txScPowInWatts,intrTxPowInWatts, estIntrCovPerRB, restOfInterferenceWithNoiseVariance, demodulationScheme, isEstimatedIntrcov));
  }
  else
  {
    cout<<"[both:] Unsupported waveform in equalizeMyDataInRB()"<<endl;abort();
  }
}


/*! \fn equlizeOfdma(...)
* \brief returns the equalized symbols if the transmission scheme is otherthan transmit diversity and Multiuser MIMO 
* \param [in,out] rxDataPerRB ---> contains the receiver data on each antenna and the length of rxDataPerRB is equal to the number of receiver antennas
* \param [in,out] precodedChannelPerRB ---> precoded channel per resource block which depends on the number of receiver antennas and number of layers(Nr x Nl X 152)
* \param [in,out] intrPrecodedChannelPerRB ---> Interference channel per RB which depends on the number of interferences, receiver antennas and number of layers(Ni x Nr x Nl X 152)
* \param [in] txScPowInWatts ---> per subcarrier transmitted power
* \param [in,out] intrTxPowInWatts ---> Interference transmitted power per subcarrier
* \param [in,out] estIntrCovPerRB ---> Interference covariance is estimated using the equation (y-Hp)
* \param restOfInterferenceWithNoiseVariance ---> contains the rest of interference with the Noise variance 
* \param [in] demodulationScheme ---> MMSE or MRC 
* \param [in] isEstimatedIntrcov ---> It is a bool variable which tells wheather the interference covariance is calculated with or without interference channel kmowledge 
* \return equalizerOutput*/

EqualizerOutput_S equlizeOfdma(Array<cvec> &rxDataPerRB, Array< cmat > &precodedChannelPerRB, Array< Array< cmat >  > &intrPrecodedChannelPerRB,ivec intrSFBCLayers, double txScPowInWatts, vec &intrTxPowInWatts, Array<cmat> &estIntrCovPerRB, double restOfInterferenceWithNoiseVariance, DemodulationScheme_E demodulationScheme, bool isEstimatedIntrcov)
{
  int nLayers=precodedChannelPerRB(0).cols();
  int nRxAntenna=precodedChannelPerRB(0).rows();
  int dataTonesPerRB=rxDataPerRB(0).length();
  cmat noiseWithROI(nRxAntenna,nRxAntenna);
  cmat IntrCov(nRxAntenna,nRxAntenna);
  cmat estsym(nLayers,1);
  vec ppSINR(nLayers);
  cmat precodedChannel;
  cmat rxsym(nRxAntenna,1);
  EqualizerOutput_S equalizerOutput;
  equalizerOutput.equalizedChannel.set_size(nLayers);
  equalizerOutput.equalizedSymbols.set_size(nLayers);
  equalizerOutput.equalizedNoiseVariance.set_size(nLayers);
  equalizerOutput.postProcessingSinr.set_size(nLayers);
 // set the size of equalizedSymbols for all the layers 
  for(int iLayer=0;iLayer <nLayers; iLayer++)
  {
	equalizerOutput.equalizedSymbols(iLayer).set_size(dataTonesPerRB);
	equalizerOutput.equalizedChannel(iLayer).set_size(dataTonesPerRB);
	equalizerOutput.equalizedNoiseVariance(iLayer).set_size(dataTonesPerRB);
	equalizerOutput.postProcessingSinr(iLayer).set_size(dataTonesPerRB);
  }
  IntrCov.clear();
  int nInterferers=intrPrecodedChannelPerRB.length();

  Array<cmat> IntrChan(nInterferers);
  
  cmat EquNoise(nLayers ,nLayers);
  cmat eqChn(nLayers,nLayers);
// this loop executes (total tones in an RB-crs tones) times for crs port-0
  for(int j = 0; j < dataTonesPerRB; j++)
  {
	precodedChannel=sqrt((double)txScPowInWatts)*precodedChannelPerRB(j);
	
	for (int rxant=0;rxant <nRxAntenna; rxant++)
	  rxsym(rxant) = rxDataPerRB(rxant)(j);
	
	noiseWithROI = (restOfInterferenceWithNoiseVariance)*eye_c(nRxAntenna);
      
	if(!isEstimatedIntrcov)
	{
	  // If the number of interferers are greater than one calculate the interference covariance matrix 
	  if(nInterferers)
	  {
		IntrCov.clear();
		for (int intr = 0;intr <nInterferers; intr++)
		{
		  if(intrSFBCLayers(intr))
		  {
		    int intrLayers=intrSFBCLayers(intr);
		    int scindx=j/intrLayers;
		    int scLocalIndx=j%intrLayers;
		    cmat intrChannel=sqrt(intrTxPowInWatts(intr))*intrPrecodedChannelPerRB(intr)(scindx);
		    cmat interferenceCov=intrChannel*intrChannel.H();
		    
		    interferenceCov=interferenceCov.get_cols(scLocalIndx*nRxAntenna,(scLocalIndx+1)*nRxAntenna-1);
		    interferenceCov = interferenceCov.get_rows(scLocalIndx*nRxAntenna,(scLocalIndx+1)*nRxAntenna-1);
    
		    if(scLocalIndx==1 || scLocalIndx==3) 
		      interferenceCov=conj(interferenceCov);
		    
		    IntrCov = IntrCov + interferenceCov;
		  }
		  else
		  {
		    IntrChan(intr)=sqrt(intrTxPowInWatts(intr))*intrPrecodedChannelPerRB(intr)(j);
		    if(IntrChan(intr).size()==0)
		      cout<<"error "<<endl;
		    IntrCov = IntrCov + IntrChan(intr)*IntrChan(intr).hermitian_transpose();
		  }
		}
		IntrCov = IntrCov + noiseWithROI;
	  }
	  else
	    IntrCov = noiseWithROI;
	}
	else
	  IntrCov=estIntrCovPerRB(j);

	
	cmat MMSEfilter;
	// switch statement selects the MMSE (or) MRC based on the demodulation scheme
	switch(demodulationScheme)
	{
	  case _DEMODULATION_SCHEME_MMSE_:
	  {
	    MMSErx(precodedChannel, rxsym, EquNoise, estsym, eqChn,ppSINR, IntrCov, nLayers, txScPowInWatts);
	    break;
	  }
	  case _DEMODULATION_SCHEME_MRC_:
	  {
// 	    IntrCov = (trace(IntrCov)/(double)nRxAntenna)*eye_c(nRxAntenna);
	    MRCrx(precodedChannel,rxsym,EquNoise,estsym,eqChn,ppSINR,IntrCov,nLayers, txScPowInWatts);
	    break;
	  }
	  case _DEMODULATION_SCHEME_WHITE_MRC_:
	  {
	    whiteMRCrx(precodedChannel,rxsym,EquNoise,estsym,eqChn,ppSINR,IntrCov,nLayers, txScPowInWatts);
	    break;
	  }
	  // If the demodulation scheme is other than MMSE and MRC equalizer display the error
	  default:
	  {cout<<"[both:] Demodulation scheme not valid . . . in equlizeOfdma();";abort();}

	}
	for(int iLayer=0;iLayer <nLayers; iLayer++)
  {
	  equalizerOutput.equalizedSymbols(iLayer).set(j,estsym(iLayer));
	  equalizerOutput.equalizedChannel(iLayer).set(j,eqChn(iLayer,iLayer));
	  equalizerOutput.equalizedNoiseVariance(iLayer).set(j,1.0);
	  equalizerOutput.postProcessingSinr(iLayer).set(j,ppSINR(iLayer)); 
  }
  }

  return(equalizerOutput);
}
/*! \fn equalize2LayerTxDiv(...)
* \brief returns the equalized symbols if the transmission scheme is transmit diversity 
* \param [in,out] estsym ---> Received signal is multiplied with MRC receiver
* \param [in,out] equchn ---> hermitian transpose of precoded channel is multiplied with precoded channel(H^H*H)
* \param [in,out] ppSINR ---> Post-processing SINR
* \param [in,out] rxDataPerRB ---> Received data at all the receiving antennas
* \param [in,out] precodedChannelPerRB ---> precoder multiplied channel from the desired base station
* \param [in,out] intrPrecodedChannelPerRB ---> consists of precoder multiplied channel of strongest interferers
* \param [in] txScPowInWatts ---> Transmitted subcarrier power 
* \param [in] intrTxPowInWatts ---> Strong interference tansmitted power
* \param [in] restOfInterferenceWithNoiseVariance ---> Weak interference power plus noise variance
* \param [in] loopIndx ---> gives the tone locations for which equalization performs
* \param [in] isEstimatedIntrcov ---> It is a bool variable which tells wheather the interference covariance is calculated with or without interference channel kmowledge
* \param [in,out] estIntrCovPerRB ---> Interference covariance vector
* \param [in] usedLayers ---> Number of layers(number of transmitting antennas)
*/
void equalize2LayerTxDiv(cmat &estsym, cmat &equchn, vec &ppSINR,DemodulationScheme_E desc,Array<cvec> &rxDataPerRB, Array< cmat > &precodedChannelPerRB, Array< Array< cmat >  > &intrPrecodedChannelPerRB,ivec intrSFBCLayers, double txScPowInWatts, vec intrTxPowInWatts,double restOfInterferenceWithNoiseVariance, int loopIndx, bool isEstimatedIntrcov, Array<cmat> &estIntrCovPerRB, ivec usedLayers=""){
  
  int nLayers=2;
  int nRxAntenna=precodedChannelPerRB(0).rows()/precodedChannelPerRB(0).cols();
  if(usedLayers.length()==0)
    usedLayers=getIntegers(0,nLayers-1);
    // initializing the desired and interference channel matrix
  cmat precodedChannel=zeros_c(nLayers*nRxAntenna,nLayers);
  cmat intrprecodedChannel;
  cmat equnoise=zeros_c(nLayers,nLayers);
  cmat IntrCov=zeros_c(nLayers*nRxAntenna,nLayers*nRxAntenna);
  cmat rxsym=zeros_c(nLayers*nRxAntenna,1);
  int noInterferers=intrPrecodedChannelPerRB.length();
  
  estsym=zeros_c(nLayers,1);
  equchn=zeros_c(nLayers,nLayers);

  for(int rxant=0;rxant<nRxAntenna;rxant++)
  {
    // arranging the receiving data matrix in SFBC matrix form
    rxsym(rxant)   = rxDataPerRB(rxant)(nLayers*loopIndx);
    rxsym(rxant+nRxAntenna) = conj(rxDataPerRB(rxant)(nLayers*loopIndx +1));
  }
  
  if(precodedChannelPerRB(0).cols()==2) //2 Layers SFBC
     precodedChannel = sqrt((double)txScPowInWatts)*precodedChannelPerRB(loopIndx);
  else //4 Layer SFBC
  {
    cmat currentChannel=sqrt((double)txScPowInWatts)*precodedChannelPerRB(loopIndx/2);
    if(loopIndx%2==0)  //Handling first 2 layers
    {
      currentChannel=currentChannel.get_cols(0,1);
      precodedChannel = currentChannel.get_rows(0,currentChannel.rows()/2-1);
    }
    else
    {
      currentChannel=currentChannel.get_cols(2,3);
      precodedChannel = currentChannel.get_rows(currentChannel.rows()/2,currentChannel.rows()-1);
    }
  }  

  // If the interference covariance matrix is not estimating
  if(!isEstimatedIntrcov)
  {	
    if(noInterferers)
    {
      IntrCov.clear();
      //calculating and adding interference covariance matrix from all strong interferers 
      for(int intr=0;intr<noInterferers;intr++)
      {
	cmat interferenceCov;
	if(intrSFBCLayers(intr))
	{
	  int intrLayers=intrSFBCLayers(intr);
	  if(intrLayers==2)
	  {
	    intrprecodedChannel=sqrt(intrTxPowInWatts(intr))*intrPrecodedChannelPerRB(intr)(loopIndx);
	    interferenceCov=intrprecodedChannel*intrprecodedChannel.H();
	  }
	  else
	  {
	    intrprecodedChannel=sqrt(intrTxPowInWatts(intr))*intrPrecodedChannelPerRB(intr)(loopIndx/2);
	    interferenceCov=intrprecodedChannel*intrprecodedChannel.H();
	    if(loopIndx%2==0)
	    {
	      interferenceCov=interferenceCov.get_cols(0,interferenceCov.cols()/2-1);
	      interferenceCov = interferenceCov.get_rows(0,interferenceCov.rows()/2-1);
	    }
	    else
	    {
	      interferenceCov=interferenceCov.get_cols(interferenceCov.cols()/2,interferenceCov.cols()-1);
	      interferenceCov = interferenceCov.get_rows(interferenceCov.rows()/2,interferenceCov.rows()-1);
	    }
	  }
	}
	else
	{
	  cmat tempChannel1=sqrt(intrTxPowInWatts(intr))*intrPrecodedChannelPerRB(intr)(2*loopIndx);
	  cmat tempChannel2=sqrt(intrTxPowInWatts(intr))*intrPrecodedChannelPerRB(intr)(2*loopIndx+1);
	  cmat interferenceCov1=tempChannel1*tempChannel1.H();
	  cmat interferenceCov2=tempChannel2*tempChannel2.H();
	  interferenceCov2=conj(interferenceCov2);
	  interferenceCov=zeros_c(nRxAntenna*2,nRxAntenna*2);
	  interferenceCov.set_submatrix(0,0,interferenceCov1);
	  interferenceCov.set_submatrix(nRxAntenna,nRxAntenna,interferenceCov2);
	}
	  
	IntrCov += interferenceCov;
      }
      IntrCov = IntrCov+(restOfInterferenceWithNoiseVariance)*eye_c(nLayers*nRxAntenna);//cout<<"[both:] Intrcov "<<IntrCov<<endl;abort();
    }
    // if the number of strong interferer channels are zero
    else
      IntrCov = (restOfInterferenceWithNoiseVariance)*eye_c(nLayers*nRxAntenna);
  }
  else
    IntrCov = estIntrCovPerRB(nLayers*loopIndx);

  
    if(desc==_DEMODULATION_SCHEME_MMSE_)
      MMSErx(precodedChannel,rxsym,equnoise,estsym,equchn,ppSINR,IntrCov,nLayers, txScPowInWatts);
    else if(desc==_DEMODULATION_SCHEME_MRC_)
      MRCrx(precodedChannel,rxsym,equnoise,estsym,equchn,ppSINR,IntrCov,nLayers, txScPowInWatts);
    else
      {
	cout<<"[detl:]Unknown demodulation scheme to equalize2LayerTxDiv()."<<endl;
	abort();
      }
      
    //taking conjugate for the symbol x2 in the received vector [x1 x2^*]
    for(int iLayer=0; iLayer<nLayers; iLayer++)
      if(mod(iLayer,2)==1)
      {
	estsym(iLayer) = conj(estsym(iLayer));
	equchn(iLayer) = conj(equchn(iLayer,iLayer));
      }
    
}
/*! \fn txDivDecoder(...)
* \brief returns the equalized symbols if the transmission scheme is otherthan transmit diversity and Multiuser MIMO
* \param [in,out] rxDataPerRB ---> Received data at all the receiving antennas
* \param [in,out] precodedChannelPerRB ---> precoder multiplied channel from the desired base station
* \param [in,out] intrPrecodedChannelPerRB ---> consists of precoder multiplied channel of strongest interferers
* \param [in] txScPowInWatts ---> Transmitted subcarrier power
* \param [in] intrTxPowInWatts ---> Strong interference tansmitted power
* \param [in,out] estIntrCovPerRB ---> Interference covariance vector
* \param [in] restOfInterferenceWithNoiseVariance ---> Weak interference power plus noise variance
* \param [in] isEstimatedIntrcov ---> It is a bool variable which tells wheather the interference covariance is calculated with or without interference channel kmowledge 
* \return returns the equalized symbols */
EqualizerOutput_S txDivDecoder(Array<cvec> &rxDataPerRB, Array< cmat > &precodedChannelPerRB, Array< Array< cmat >  > &intrPrecodedChannelPerRB,ivec intrSFBCLayers, double txScPowInWatts, vec &intrTxPowInWatts, Array<cmat> &estIntrCovPerRB, double restOfInterferenceWithNoiseVariance, DemodulationScheme_E demodulationScheme,bool isEstimatedIntrcov)
{
  int nRxAntenna=rxDataPerRB.length();
  int nLayers=precodedChannelPerRB(0).rows()/nRxAntenna;
  int dataTonesPerRB=rxDataPerRB(0).length();
  EqualizerOutput_S equalizerOutput;
  equalizerOutput.equalizedChannel.set_size(nLayers);
  equalizerOutput.equalizedSymbols.set_size(nLayers);
  equalizerOutput.postProcessingSinr.set_size(1);
  equalizerOutput.postProcessingSinr(0).set_size(dataTonesPerRB);

  for(int iLayer=0;iLayer <nLayers; iLayer++)
  {
    equalizerOutput.equalizedSymbols(iLayer).set_size(dataTonesPerRB/nLayers);
    equalizerOutput.equalizedChannel(iLayer).set_size(dataTonesPerRB/nLayers);
  }
  
  cmat estsym;
  cmat equchn;
  vec ppSINR;
  switch(nLayers)
  {
    case 2:  
      for(int j = 0; j < dataTonesPerRB/nLayers; j++)
      {
	equalize2LayerTxDiv(estsym,equchn,ppSINR,demodulationScheme,rxDataPerRB,precodedChannelPerRB,intrPrecodedChannelPerRB,intrSFBCLayers,txScPowInWatts,intrTxPowInWatts,restOfInterferenceWithNoiseVariance,j,isEstimatedIntrcov,estIntrCovPerRB);
	equalizerOutput.postProcessingSinr(0)[nLayers*j]=ppSINR(0);
	equalizerOutput.postProcessingSinr(0)[nLayers*j+1]=ppSINR(1); 
	
	// collects the equalized symbols from the function equalize2LayerTxDiv
	for(int iLayer=0;iLayer <nLayers; iLayer++)
	{
	  equalizerOutput.equalizedSymbols(iLayer).set(j,estsym(iLayer));
	  equalizerOutput.equalizedChannel(iLayer).set(j,equchn(iLayer,iLayer));
	}
      } 
      break;
    case 4:
      for(int j = 0; j < dataTonesPerRB/2; j++) 
      {
	//This model doesn't assume all layers have same channel 
	// ALso the model decodes 2 layers at a time 
	if(j%2==0)//Even Loops where layers 0&1 are decoded
	{
	  ivec usedLayers(2);usedLayers(0)=0;usedLayers(1)=2;
	  equalize2LayerTxDiv(estsym,equchn,ppSINR,demodulationScheme,rxDataPerRB,precodedChannelPerRB,intrPrecodedChannelPerRB,intrSFBCLayers,txScPowInWatts,intrTxPowInWatts,restOfInterferenceWithNoiseVariance,j,isEstimatedIntrcov,estIntrCovPerRB,usedLayers);
	  equalizerOutput.equalizedSymbols(0).set(j/2,estsym(0));
	  equalizerOutput.equalizedChannel(0).set(j/2,equchn(0,0));
	  equalizerOutput.equalizedSymbols(1).set(j/2,estsym(1));
	  equalizerOutput.equalizedChannel(1).set(j/2,equchn(1,1));
	}
	else//Odd Loops where layers 2&3 are decoded
	{
	  ivec usedLayers(2);usedLayers(0)=1;usedLayers(1)=3;
	  equalize2LayerTxDiv(estsym,equchn,ppSINR,demodulationScheme,rxDataPerRB,precodedChannelPerRB,intrPrecodedChannelPerRB,intrSFBCLayers,txScPowInWatts,intrTxPowInWatts,restOfInterferenceWithNoiseVariance,j,isEstimatedIntrcov,estIntrCovPerRB,usedLayers);
	  equalizerOutput.equalizedSymbols(2).set((j-1)/2,estsym(0));
	  equalizerOutput.equalizedChannel(2).set((j-1)/2,equchn(0,0));
	  equalizerOutput.equalizedSymbols(3).set((j-1)/2,estsym(1));
	  equalizerOutput.equalizedChannel(3).set((j-1)/2,equchn(1,1));
	  
	}
	equalizerOutput.postProcessingSinr(0)[2*j]=ppSINR(0);
	equalizerOutput.postProcessingSinr(0)[2*j+1] = ppSINR(1);
	
      }
      break;  
      
    default:
    {cout<<"[both:] Undefined nLayers .... exiting in  txDivDecoder() "<<nLayers<<endl;abort();}
  }
  return(equalizerOutput);
}

/*! \fn CLMUMIMOdecoder(...)
* \brief returns the equalized symbols if the transmission scheme is MultiUserMIMO 
* \param [in,out] dataBlkInfo ---> gives the transmission scheme 
* \param [in] rbNumber ---> used to finout the resource of a particular user in Multiuser MIMO
* \param [in,out] rxDataPerRB ---> contains the receiver data on each antenna and the length of rxDataPerRB is equal to the number of receiver antennas
* \param [in,out] precodedChannelPerRB ---> precoded channel per resource block which depends on the number of receiver antennas and number of layers(Nr x Nl X 152)
* \param [in,out] intrPrecodedChannelPerRB ---> Interference channel per RB which depends on the number of interferences (8), receiver antennas and number of layers(Ni x Nr x Nl X 152)
* \param [in] txScPowInWatts ---> per subcarrier transmitted power
* \param [in,out] intrTxPowInWatts ---> Interference transmitted power per subcarrier
* \param [in,out] estIntrCovPerRB ---> Interference covariance is estimated using the equation (y-Hp)
* \param restOfInterferenceWithNoiseVariance ---> contains the rest of interference with the Noise variance 
* \param [in] demodulationScheme ---> MMSE or MRC 
* \param [in] isEstimatedIntrcov ---> It is a bool variable which tells wheather the interference covariance is calculated with or without interference channel kmowledge 
* \return equalizerOutput*/

// EqualizerOutput_S CLMUMIMOdecoder(DataBlkInfo_S dataBlkInfo, int rbNumber, Array<cvec> &rxDataPerRB, Array< cmat > &precodedChannelPerRB, Array< Array< cmat >  > &intrPrecodedChannelPerRB,ivec intrSFBCLayers, double txScPowInWatts, vec &intrTxPowInWatts, Array<cmat> &estIntrCovPerRB, double restOfInterferenceWithNoiseVariance, DemodulationScheme_E demodulationScheme, bool isEstimatedIntrcov)
// {
//   int nLayers=precodedChannelPerRB(0).cols();
//   ivec Layers=getIntegers(0,nLayers-1);
//   int nRxAntenna=precodedChannelPerRB(0).rows();
//   int nLayersPerUE = dataBlkInfo.dataGenInfo.nLayers;
//   int dataTonesPerRB=rxDataPerRB(0).length();
//   ivec scheduledLayers=1;
//   ivec intrLayers=findNot(Layers, scheduledLayers);
//   int idealIntrCov=1;
//   int nInterferers=intrPrecodedChannelPerRB.length();
//   cmat noiseWithROI(nRxAntenna,nRxAntenna);
//   cmat IntrCov(nRxAntenna,nRxAntenna);
//   cmat estsym(nLayersPerUE,1);
//   cmat precodedChannel(nRxAntenna,nLayersPerUE);
//   cmat precodedIUIchannel(nRxAntenna,nLayersPerUE);
//   cmat rxsym(nRxAntenna,1);
//   cmat EquNoise(nLayersPerUE ,nLayersPerUE);
//   cmat eqChn(nLayersPerUE,nLayersPerUE);
//   vec ppSINR(nLayersPerUE);
// 
//   noiseWithROI = (restOfInterferenceWithNoiseVariance)*eye_c(nRxAntenna);
// 
//   EqualizerOutput_S equalizerOutput;
//   equalizerOutput.equalizedChannel.set_size(nLayersPerUE);
//   equalizerOutput.equalizedSymbols.set_size(nLayersPerUE);
//   equalizerOutput.postProcessingSinr.set_size(nLayersPerUE);
//   for(int iLayer=0;iLayer <nLayersPerUE; iLayer++)
//   {
// 	equalizerOutput.equalizedSymbols(iLayer).set_size(dataTonesPerRB);
// 	equalizerOutput.equalizedChannel(iLayer).set_size(dataTonesPerRB);
// 	equalizerOutput.postProcessingSinr(iLayer).set_size(dataTonesPerRB);
//   }
// 
//   Array<cmat> IntrChan(nInterferers);
//   
//   for(int j = 0; j < dataTonesPerRB; j++)
//   {
// 	precodedChannel=sqrt((double)txScPowInWatts)*precodedChannelPerRB(j).get_cols(scheduledLayers);
// 	
// 	for (int rxant=0;rxant <nRxAntenna; rxant++)
// 	  rxsym(rxant) = rxDataPerRB(rxant)(j);
// 	
//        // calculating the Inter User interference (or) Interstream interference
// 	precodedIUIchannel=sqrt((double)txScPowInWatts)*precodedChannelPerRB(j).get_cols(intrLayers);/// Inter user interference
// 	
// 	if(!isEstimatedIntrcov)
// 	{
// 	  // If the number of interferers are greater than one calculate the interference covariance matrix
// 	  if(nInterferers)
// 	  {
// 		IntrCov.clear();
// 		for (int intr = 0;intr <nInterferers; intr++)
// 		{
// 		  if(intrSFBCLayers(intr))
// 		  {
// 		    int intrLayers=intrPrecodedChannelPerRB(intr)(0).rows()/nRxAntenna;
// 		    int scindx=j/intrLayers;
// 		    int scLocalIndx=j%intrLayers;
// 		    cmat intrChannel=sqrt(intrTxPowInWatts(intr))*intrPrecodedChannelPerRB(intr)(scindx);
// 		    cmat interferenceCov=intrChannel*intrChannel.H();
// 		    if(intrLayers==4)
// 		    {
// 		      if(scLocalIndx>1) // Handling 2nd and 3rd layers
// 		      {
// 			interferenceCov=interferenceCov.get_cols(interferenceCov.cols()/2,interferenceCov.cols()-1);
// 			interferenceCov = interferenceCov.get_rows(interferenceCov.rows()/2,interferenceCov.rows()-1);
// 		      }
// 		      else
// 		      {
// 			interferenceCov=interferenceCov.get_cols(0,interferenceCov.cols()/2-1);
// 			interferenceCov = interferenceCov.get_rows(0,interferenceCov.rows()/2-1);
// 		      }
// 		      
// 		    }
// 		    
// 		    if(scLocalIndx==1 || scLocalIndx==3) 
// 		      interferenceCov=interferenceCov.T();
// 		    
// 		    IntrCov = IntrCov + interferenceCov;
// 		  }
// 		  else
// 		  {
// 		    IntrChan(intr)=sqrt(intrTxPowInWatts(intr))*intrPrecodedChannelPerRB(intr)(j);
// 		    IntrCov = IntrCov + IntrChan(intr)*IntrChan(intr).hermitian_transpose();
// 		  }
// 		}
// 	  }
// 	  // Interference covariance in the absence of interference
// 	  else
// 		IntrCov = noiseWithROI;
// 	}
// 	else
// 	  IntrCov=estIntrCovPerRB(j);
// 
// 	IntrCov = IntrCov + precodedIUIchannel*precodedIUIchannel.hermitian_transpose()+noiseWithROI;
// 
// 	switch(demodulationScheme)
// 	{
// 	  case _DEMODULATION_SCHEME_MMSE_:
// 	  {
// 	    MMSErx(precodedChannel, rxsym, EquNoise, estsym, eqChn,ppSINR, IntrCov, nLayersPerUE, txScPowInWatts);
// 	    break;
// 	  }
// 	  case _DEMODULATION_SCHEME_MRC_:
// 	  {
// 	    IntrCov = (trace(IntrCov)/nRxAntenna)*eye_c(nRxAntenna);
// 	    MRCrx(precodedChannel,rxsym,EquNoise,estsym,eqChn,ppSINR,IntrCov,nLayersPerUE, txScPowInWatts);
// 	    break;
// 	  }
// 	  default:
// 	  {cout<<"[both:] Demodulation scheme not valid . . . in CLMUMIMOdecoder();";abort();}
// 
// 	}
// 	for(int iLayer=0;iLayer <nLayersPerUE; iLayer++)
// 	  equalizerOutput.equalizedSymbols(iLayer).set(j,estsym(iLayer));
// 	for(int iLayer=0;iLayer <nLayersPerUE; iLayer++)
// 	  equalizerOutput.equalizedChannel(iLayer).set(j,eqChn(iLayer,iLayer));
// 	for(int iLayer=0;iLayer <nLayersPerUE; iLayer++)
// 	  equalizerOutput.postProcessingSinr(iLayer).set(j,ppSINR(iLayer));
//   }
//   return(equalizerOutput);
// }

/*! \fn MMSErx(...)
* \brief does the MMSE equalization 
* \param [in] precodedChannel ---> channel multiplied with appropriate precoder (Nr x Nl)
* \param [in] rxsym ---> received vector at the receiver for an UE
* \param [in,out] EquNoise ---> channel is multiplied with MMSE filter at receiver 
* \param [in,out] EqChn ---> channel is multiplied with MMSE filter at receiver 
* \param [in,out] estsym ---> received signal is multiplied with MMSE equalized channel at receiver
* \param [in,out] ppSINR ---> post processing SINR 
* \param [in] IntrCov ---> Interference covariance 
* \param [in] nLayers ---> number of layers
* \param [in] txScPowInWatts ---> per subcarrier transmitted power
*/
void MMSErx(cmat precodedChannel,cmat rxsym,cmat &EquNoise,cmat &estsym,cmat &EqChn,vec &ppSINR,cmat IntrCov,int nLayers, double txScPowInWatts)
{
  cmat MMSEfilter  =  sqrt(txScPowInWatts)*precodedChannel.hermitian_transpose()*inv( (precodedChannel*precodedChannel.hermitian_transpose())+IntrCov);
  estsym  =  MMSEfilter*rxsym;
  EqChn   =  MMSEfilter*precodedChannel;
  EquNoise = MMSEfilter*IntrCov*hermitian_transpose(MMSEfilter);
  ppSINR.set_size(EqChn.rows());
  
  // Included non-diagonal terms in the noise variance 
  cvec temp=diag(EqChn);
  cmat desiredEquChn=diag(temp);
  cmat residualISI=EqChn-desiredEquChn;
  EquNoise+=residualISI*residualISI.hermitian_transpose();
  EqChn=desiredEquChn;
  for (int iLayer =0;iLayer <nLayers;iLayer++) // Assuming independent detection
  {
      estsym(iLayer) = estsym(iLayer)/sqrt(abs(EquNoise(iLayer,iLayer)));
      #ifdef USING_CALIBRATION
      if (traceValuePerTraceName["PUSCH_3_0caseA"]=="true" || traceValuePerTraceName["PUSCH_3_0caseBsubband"]=="true" || traceValuePerTraceName["PUSCH_3_0caseBwideband"]=="true" || traceValuePerTraceName["PUSCH_2_0subband"]=="true" || traceValuePerTraceName["PUSCH_2_0wideband"]=="true")
      if(EquNoise(iLayer,iLayer) == dComplex(0,0))
	EquNoise(iLayer,iLayer)=dComplex(1,0);
      #endif
      EqChn(iLayer,iLayer) = EqChn(iLayer,iLayer)/sqrt(abs(EquNoise(iLayer,iLayer)));
      ppSINR(iLayer)=sqr(abs(EqChn(iLayer,iLayer)));
      
  }
  
}
/*! \fn MRCrx(...)
* \brief does the MRC equalization 
* \param [in] precodedChannel ---> channel multiplied with appropriate precoder (Nr x Nl)
* \param [in] rxsym ---> received vector at the receiver for an UE
* \param [in,out] EquNoise ---> channel is multiplied with MMSE filter at receiver 
* \param [in,out] EqChn ---> channel is multiplied with MMSE filter at receiver 
* \param [in,out] estsym ---> received signal is multiplied with MMSE equalized channel at receiver
* \param [in,out] ppSINR ---> post processing SINR 
* \param [in] IntrCov ---> Interference covariance 
* \param [in] nLayers ---> number of layers
* \param [in] txScPowInWatts ---> per subcarrier transmitted power
*/
void MRCrx(cmat precodedChannel,cmat rxsym,cmat &EquNoise,cmat &estsym,cmat &EqChn,vec &ppSINR,cmat IntrCov,int nLayers ,  double txScPowInWatts)
{
  cmat MRCfilter  =  hermitian_transpose(precodedChannel)/sqrt((double)txScPowInWatts);
  estsym =  MRCfilter*rxsym;
  EquNoise =MRCfilter*IntrCov*hermitian_transpose(MRCfilter);/// replace with 1e-50*eye_c(MRCfilter.rows()) when noise and interference is disabled.
  EqChn = MRCfilter*precodedChannel;
  ppSINR.set_size(EqChn.rows());

  // Included non-diagonal terms in the noise variance 
  cvec temp=diag(EqChn);
  cmat desiredEquChn=diag(temp);
  cmat residualISI=EqChn-desiredEquChn;
  EquNoise+=residualISI*residualISI.H();
  EqChn=desiredEquChn;
  for (int iLayer =0;iLayer <nLayers;iLayer++) // Assuming independent detection 
  {
    estsym(iLayer) = estsym(iLayer)/sqrt(abs(EquNoise(iLayer,iLayer)));
    
    #ifdef USING_CALIBRATION
    if (traceValuePerTraceName["PUSCH_3_0caseA"]=="true" || traceValuePerTraceName["PUSCH_3_0caseBsubband"]=="true" || traceValuePerTraceName["PUSCH_3_0caseBwideband"]=="true" || traceValuePerTraceName["PUSCH_2_0subband"]=="true" || traceValuePerTraceName["PUSCH_2_0wideband"]=="true")
      if(EquNoise(iLayer,iLayer) == dComplex(0,0))
	EquNoise(iLayer,iLayer)=dComplex(1,0);
      #endif
    
    EqChn(iLayer,iLayer) = EqChn(iLayer,iLayer)/sqrt(abs(EquNoise(iLayer,iLayer)));
    ppSINR(iLayer)=sqr(abs(EqChn(iLayer,iLayer)));
  }
}

void whiteMRCrx(cmat precodedChannel,cmat rxsym,cmat &EquNoise,cmat &estsym,cmat &EqChn,vec &ppSINR,cmat IntrCov,int nLayers ,  double txScPowInWatts)
{
  int nRxAntenna=precodedChannel.rows();
  if(precodedChannel.cols()!=1)
  {
    cout<<"[both:] Error. whiteMRCrx() supports only single layer transmission."<<endl;
    abort();
  }
  complex< double > noiseVariance= (trace(IntrCov)/complex< double >(nRxAntenna,0));// Taking avg interference power across antennas
  cmat MRCfilter  =  sqrt((double)txScPowInWatts)*hermitian_transpose(precodedChannel)/((hermitian_transpose(precodedChannel)*precodedChannel)(0,0)+noiseVariance);
  estsym =  MRCfilter*rxsym;
  EquNoise =MRCfilter*IntrCov*hermitian_transpose(MRCfilter);/// replace with 1e-50*eye_c(MRCfilter.rows()) when noise and interference is disabled.
  EqChn = MRCfilter*precodedChannel;
  ppSINR.set_size(EqChn.rows());

  // Included non-diagonal terms in the noise variance
  cvec temp=diag(EqChn);
  cmat desiredEquChn=diag(temp);
  cmat residualISI=EqChn-desiredEquChn;
  EquNoise+=residualISI*residualISI.H();
  EqChn=desiredEquChn;
  for (int iLayer =0;iLayer <nLayers;iLayer++) // Assuming independent detection
  {
    estsym(iLayer) = estsym(iLayer)/sqrt(abs(EquNoise(iLayer,iLayer)));
    EqChn(iLayer,iLayer) = EqChn(iLayer,iLayer)/sqrt(abs(EquNoise(iLayer,iLayer)));
    ppSINR(iLayer)=sqr(abs(EqChn(iLayer,iLayer)));
  }
}

EqualizerOutput_S equalizeControlChannel(int nLayers,Array<cvec> &rxData, Array< cmat > &myChannel, Array< Array< cmat >  > intrChannelPerRB, double txScPowInWatts, vec intrTxPowInWatts, Array<cmat> &estIntrCovPerRB, double restOfInterferenceWithNoiseVariance, DemodulationScheme_E demodulationScheme, bool isEstimatedIntrcov)
{
  ivec interfererSFBCLayers;
  if(nLayers>1)
    interfererSFBCLayers=ones_i(intrTxPowInWatts.length())*nLayers;
  else
    interfererSFBCLayers=zeros_i(intrTxPowInWatts.length());
  
  switch(nLayers)
  {
    case 1:
      return(equlizeOfdma(rxData, myChannel, intrChannelPerRB,interfererSFBCLayers, txScPowInWatts,intrTxPowInWatts, estIntrCovPerRB, restOfInterferenceWithNoiseVariance, demodulationScheme, isEstimatedIntrcov));
    default:
      return(txDivDecoder(rxData, myChannel,intrChannelPerRB,interfererSFBCLayers,txScPowInWatts,intrTxPowInWatts,estIntrCovPerRB,restOfInterferenceWithNoiseVariance, demodulationScheme,isEstimatedIntrcov));
  }
}

vec getSINRperLayer(DemodulationScheme_E desc, cmat precodedChannel,cmat Intrcov, int nLayers, double sigpowInWatts)
{
  vec sinr;
  switch(desc)
  {
    case _DEMODULATION_SCHEME_MRC_:
      sinr = getSINRwithMRCperLayer(precodedChannel, Intrcov, nLayers, sigpowInWatts);
      break;
    case _DEMODULATION_SCHEME_MMSE_:
      sinr = getSINRwithMMSEperLayer(precodedChannel, Intrcov, nLayers, sigpowInWatts);
      break;
    case _DEMODULATION_SCHEME_WHITE_MRC_:
      sinr = getSINRwithWhiteMRCperLayer(precodedChannel, Intrcov, nLayers, sigpowInWatts);
      break;
    default:
    {cout<<"both:] Undefined DemodulationScheme .... exiting in getSINRperLayer()"<<endl;abort();}
  }
  return(sinr);
}


