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

#include "../include/LayerMapper.h"

Array<ivec> getLayerToCWMapping(int nLayers,int nCodeWords)
{
    Array<ivec> map(nCodeWords);
    if(nCodeWords==1)
    {
        if(nLayers<=4)
        {
            map(0) = getIntegers(1,nLayers);
            return map;
        }
        else
        {
            cout<<"Single codeword is not supported for nLayers>4."<<endl;
            abort();
        }
    }
    else if(nCodeWords==2)
    {
        if(nLayers==1)
        {
            cout<<"nCodeWords must be 1 for one layer. "<<endl;abort();
        }
        else if(nLayers>1 && nLayers<=8)
        {
            map(0) = getIntegers(1,floor_i(nLayers/2.0));
            map(1) = getIntegers(floor_i(nLayers/2.0)+1,nLayers);
            return map; 
        }
        else
        {
            cout<<"Invalid nLayers.."<<endl;abort();
        }
    }
    cout<<"Invalid nCodeWords.."<<endl;abort();
}

/*! \fn performLayerMapping(...)
 * \brief Does LayerMapping
 * \param [in,out] modulatedSymbolsPerCodeWord ---> contains the modulated symbols in a codeword
 * \param [in,out] dataGenInfo ---> it is a class variable gives the information about the number of layers
 * \return layeredData ---> which contains the modulated symbols of each layer for any given transmission mode
 * 
 * \ref ts36.211 relese 10 sec 6.3.3 table:6.3.3.2-1,6.3.3.3-1 */

Array<cvec> performLayerMapping(Array<cvec> &modulatedSymbolsPerCodeWord, int nCodeWords, int nLayers)
{
  Array<cvec> layeredData;
  Array<cvec> singleCodeword(2);
  int mLayerSymbols;
  int mLayerSymbols1;			
  int rem,rem1;
  {
    layeredData.set_length(nLayers);
    // switch statement for number of layers
    switch(nLayers)
    {
      // if number of layers is 1 and and the codeword is one then direct mapping 
      case 1: if(nCodeWords == 1)
	layeredData = modulatedSymbolsPerCodeWord;
	// if number of layers is 1 and and the codewords are more than one then display the error
	else
	{cout<<"[both:] The number of code words cannot be more than 1."<<endl;abort();}
	break;
	// if number of layers are 2 and and the codeword is one 
      case 2: if(nCodeWords == 1)
      {
	mLayerSymbols = modulatedSymbolsPerCodeWord(0).length()/2;
	layeredData(0).set_length(mLayerSymbols);
	layeredData(1).set_length(mLayerSymbols);
	// map odd symbols of codeword to layer-1 and even symbols to layer-2
	for(int ithsymbol=0;ithsymbol<mLayerSymbols;ithsymbol++)
	{
	  layeredData(0)(ithsymbol) = modulatedSymbolsPerCodeWord(0)(2*ithsymbol);
	  layeredData(1)(ithsymbol) = modulatedSymbolsPerCodeWord(0)(2*ithsymbol+1);

	}

      }
      // if number of layers are 2 and and the codewords are 2 then do the direct mapping
      else if(nCodeWords == 2)
      {
	layeredData(0) = modulatedSymbolsPerCodeWord(0);
	layeredData(1) = modulatedSymbolsPerCodeWord(1);

      }
      // if the number of codewords are more than two display the error
      else
      {
	cout<<"[both:] The number of code words cannot be more than 2."<<endl;abort();

      };
      break;
      // if number of layers are 3 and and the codeword is one 
      case 3: if(nCodeWords == 1)
      {
     // map the symbols of codeword to three layers equally if the number of symbols are not mutiple of 3 then add parity bits
	rem = modulatedSymbolsPerCodeWord(0).length() % 3;
	if(rem == 0)
	  rem = 3;
	mLayerSymbols = ((modulatedSymbolsPerCodeWord(0).length() + 3 - rem) /3) ; // to make it multiple of 3
	layeredData(0).set_length(mLayerSymbols);
	layeredData(1).set_length(mLayerSymbols);
	layeredData(2).set_length(mLayerSymbols);
	singleCodeword(0).zeros();
	singleCodeword(0) = concat(modulatedSymbolsPerCodeWord(0),zeros_c(3 - rem));
	for(int ithsymbol=0;ithsymbol<mLayerSymbols;ithsymbol++)
	{
	  layeredData(0)(ithsymbol) = singleCodeword(0)(3*ithsymbol);
	  layeredData(1)(ithsymbol) = singleCodeword(0)(3*ithsymbol+1);
	  layeredData(2)(ithsymbol) = singleCodeword(0)(3*ithsymbol+2);
	}
     
      }
      // if number of layers are 3 and and the codewords are two 
      else if(nCodeWords == 2)
      {
	// direct symbol mapping from codeword-0 to layer-0
	layeredData(0) = modulatedSymbolsPerCodeWord(0);
	// map odd position symbols to layer-2 and even position symbols to layer-3 
	mLayerSymbols = modulatedSymbolsPerCodeWord(1).length()/2;
	layeredData(1).set_length(mLayerSymbols);
	layeredData(2).set_length(mLayerSymbols);
	for(int ithsymbol=0;ithsymbol<mLayerSymbols;ithsymbol++)
	{
	  layeredData(1)(ithsymbol) = modulatedSymbolsPerCodeWord(1)(2*ithsymbol);
	  layeredData(2)(ithsymbol) = modulatedSymbolsPerCodeWord(1)(2*ithsymbol+1);

	}

      }
      // if codewords are more than two display the error
      else
      {
	cout<<"[both:] The number of code words cannot be more than 2."<<endl;abort();

      };
      break;
      // if number of layers are 4 and and the codeword is one
      case 4: if(nCodeWords == 1)
      {
	// map the symbols of codeword to four layers equally if the number of symbols are not mutiple of 4 then add parity bits
	rem = modulatedSymbolsPerCodeWord(0).length() % 4;
	if(rem == 0)
	  rem = 4;
	mLayerSymbols = ((modulatedSymbolsPerCodeWord(0).length() + 4 -rem) /4) ; // to make it multiple of 4
	layeredData(0).set_length(mLayerSymbols);
	layeredData(1).set_length(mLayerSymbols);
	layeredData(2).set_length(mLayerSymbols);
	layeredData(3).set_length(mLayerSymbols);
	singleCodeword(0).zeros();
	singleCodeword(0) = concat(modulatedSymbolsPerCodeWord(0),zeros_c(4 - rem));
	for(int ithsymbol=0;ithsymbol<mLayerSymbols;ithsymbol++)
	{
	  layeredData(0)(ithsymbol) = singleCodeword(0)(4*ithsymbol);
	  layeredData(1)(ithsymbol) = singleCodeword(0)(4*ithsymbol + 1);
	  layeredData(2)(ithsymbol) = singleCodeword(0)(4*ithsymbol + 2);
	  layeredData(3)(ithsymbol) = singleCodeword(0)(4*ithsymbol + 3);

	};

      }
      // if number of layers are 4 and and the codewords are two 
      else if(nCodeWords == 2)
      {
	mLayerSymbols = modulatedSymbolsPerCodeWord(0).length()/2;
	layeredData(0).set_length(mLayerSymbols);
	layeredData(1).set_length(mLayerSymbols);
	mLayerSymbols1 = modulatedSymbolsPerCodeWord(1).length()/2;
	layeredData(2).set_length(mLayerSymbols1);
	layeredData(3).set_length(mLayerSymbols1);
	// map the symbols of codeword-0 to the layer-0 and layer-1 equally
	for(int ithsymbol=0;ithsymbol<mLayerSymbols;ithsymbol++)
	{
	  layeredData(0)(ithsymbol) = modulatedSymbolsPerCodeWord(0)(2*ithsymbol);
	  layeredData(1)(ithsymbol) = modulatedSymbolsPerCodeWord(0)(2*ithsymbol + 1);

	};
	// map the symbols of codeword-1 to the layer-2 and layer-3 equally
	for(int ithsymbol=0;ithsymbol<mLayerSymbols1;ithsymbol++)
	{
	  layeredData(2)(ithsymbol) = modulatedSymbolsPerCodeWord(1)(2*ithsymbol);
	  layeredData(3)(ithsymbol) = modulatedSymbolsPerCodeWord(1)(2*ithsymbol + 1);

	};

      }
      // if codewords are more than two display the error
      else
      {
	cout<<"[both:] The number of code words cannot be more than 2."<<endl;abort();

      };
      break;
      // if number of layers are 5 and and the codewords are two
      case 5 : if(nCodeWords == 2)
      {
	mLayerSymbols = modulatedSymbolsPerCodeWord(0).length()/2;
	layeredData(0).set_length(mLayerSymbols);
	layeredData(1).set_length(mLayerSymbols);
	// map the symbols of codeword-0 to the layer-0 and layer-1 equally
	for(int ithsymbol=0;ithsymbol<mLayerSymbols;ithsymbol++)
	{
	  layeredData(0)(ithsymbol) = modulatedSymbolsPerCodeWord(0)(2*ithsymbol);
	  layeredData(1)(ithsymbol) = modulatedSymbolsPerCodeWord(0)(2*ithsymbol + 1);

	};
	// map the symbols of codeword-1 to the layer-2, layer-3 and layer-4 equally
	rem = modulatedSymbolsPerCodeWord(1).length() % 3;
	if(rem == 0)
	  rem = 3;
	mLayerSymbols1 = ((modulatedSymbolsPerCodeWord(1).length() + 3 - rem) /3) ; // to make it multiple of 3
	layeredData(2).set_length(mLayerSymbols1);
	layeredData(3).set_length(mLayerSymbols1);
	layeredData(4).set_length(mLayerSymbols1);
	singleCodeword(0).zeros();
	singleCodeword(0) = concat(modulatedSymbolsPerCodeWord(1),zeros_c(3 - rem));
	for(int ithsymbol=0;ithsymbol<mLayerSymbols1;ithsymbol++)
	{
	  layeredData(2)(ithsymbol) = singleCodeword(0)(3*ithsymbol);
	  layeredData(3)(ithsymbol) = singleCodeword(0)(3*ithsymbol+1);
	  layeredData(4)(ithsymbol) = singleCodeword(0)(3*ithsymbol+2);

	};

      }
      // if codewords are more than two display the error
      else
      {
	cout<<"[both:] The number of code words has to be equal to 2."<< endl;abort();

      };
      break;
      // if number of layers are 6 and and the codewords are two
      case 6: if(nCodeWords == 2)
      {
	rem = modulatedSymbolsPerCodeWord(0).length() % 3;
	if(rem == 0)
	  rem = 3;
	mLayerSymbols = ((modulatedSymbolsPerCodeWord(0).length() + 3 - rem) /3) ; // to make it multiple of 3
	layeredData(0).set_length(mLayerSymbols);
	layeredData(1).set_length(mLayerSymbols);
	layeredData(2).set_length(mLayerSymbols);
	singleCodeword(0).zeros();
	singleCodeword(0) = concat(modulatedSymbolsPerCodeWord(0),zeros_c(3 - rem));
	// map the symbols of codeword-0 to the layer-0, layer-1, and layer-2 equally
	for(int ithsymbol=0;ithsymbol<mLayerSymbols;ithsymbol++)
	{
	  layeredData(0)(ithsymbol) = singleCodeword(0)(3*ithsymbol);
	  layeredData(1)(ithsymbol) = singleCodeword(0)(3*ithsymbol+1);
	  layeredData(2)(ithsymbol) = singleCodeword(0)(3*ithsymbol+2);

	};
	rem1 = modulatedSymbolsPerCodeWord(1).length() % 3;
	if(rem1 == 0)
	  rem1 = 3;
	mLayerSymbols1 = ((modulatedSymbolsPerCodeWord(1).length() + 3 - rem1) /3) ; // to make it multiple of 3
	layeredData(3).set_length(mLayerSymbols1);
	layeredData(4).set_length(mLayerSymbols1);
	layeredData(5).set_length(mLayerSymbols1);
	singleCodeword(1).zeros();
	singleCodeword(1) = concat(modulatedSymbolsPerCodeWord(1),zeros_c(3 - rem1));
	// map the symbols of codeword-1 to the layer-3, layer-4, and layer-5 equally
	for(int ithsymbol=0;ithsymbol<mLayerSymbols1;ithsymbol++)
	{
	  layeredData(3)(ithsymbol) = singleCodeword(1)(3*ithsymbol);
	  layeredData(4)(ithsymbol) = singleCodeword(1)(3*ithsymbol+1);
	  layeredData(5)(ithsymbol) = singleCodeword(1)(3*ithsymbol+2);

	};

      }
      // if codewords are more than two display the error
      else
      {
	cout<<"[both:] The number of code words has to be equal to 2."<< endl;abort();

      };
      break;
      case 7 :if(nCodeWords == 2)
      {
	rem = modulatedSymbolsPerCodeWord(0).length() % 3;
	if(rem == 0)
	  rem = 3;
	mLayerSymbols = ((modulatedSymbolsPerCodeWord(0).length() + 3 - rem) /3) ; // to make it multiple of 3
	layeredData(0).set_length(mLayerSymbols);
	layeredData(1).set_length(mLayerSymbols);
	layeredData(2).set_length(mLayerSymbols);
	singleCodeword(0).zeros();
	singleCodeword(0) = concat(modulatedSymbolsPerCodeWord(0),zeros_c(3 - rem));
	// map the symbols of codeword-0 to the layer-0, layer-1, and layer-2 equally
	for(int ithsymbol=0;ithsymbol<mLayerSymbols;ithsymbol++)
	{
	  layeredData(0)(ithsymbol) = singleCodeword(0)(3*ithsymbol);
	  layeredData(1)(ithsymbol) = singleCodeword(0)(3*ithsymbol+1);
	  layeredData(2)(ithsymbol) = singleCodeword(0)(3*ithsymbol+2);

	};
	rem1 = modulatedSymbolsPerCodeWord(1).length() % 4;
	if(rem1 == 0)
	  rem1 = 4;
	mLayerSymbols = ((modulatedSymbolsPerCodeWord(1).length() + 4 -rem1) /4) ; // to make it multiple of 4
	layeredData(3).set_length(mLayerSymbols);
	layeredData(4).set_length(mLayerSymbols);
	layeredData(5).set_length(mLayerSymbols);
	layeredData(6).set_length(mLayerSymbols);
	singleCodeword(1).zeros();
	singleCodeword(1) = concat(modulatedSymbolsPerCodeWord(1),zeros_c(4 - rem1));
	// map the symbols of codeword-1 to the layer-3, layer-4, layer-5 and layer-6 equally
	for(int ithsymbol=0;ithsymbol<mLayerSymbols;ithsymbol++)
	{
	  layeredData(3)(ithsymbol) = singleCodeword(1)(4*ithsymbol);
	  layeredData(4)(ithsymbol) = singleCodeword(1)(4*ithsymbol + 1);
	  layeredData(5)(ithsymbol) = singleCodeword(1)(4*ithsymbol + 2);
	  layeredData(6)(ithsymbol) = singleCodeword(1)(4*ithsymbol + 3);

	};

      }
      // if codewords are more than two display the error
      else
      {
	cout<<"[both:] The number of code words has to be equal to 2."<< endl;abort();

      };
      break;
      case 8 : if(nCodeWords == 2)
      {
	rem = modulatedSymbolsPerCodeWord(0).length() % 4;
	mLayerSymbols = ((modulatedSymbolsPerCodeWord(0).length() + rem) /4) ; // to make it multiple of 4
	layeredData(0).set_length(mLayerSymbols);
	layeredData(1).set_length(mLayerSymbols);
	layeredData(2).set_length(mLayerSymbols);
	layeredData(3).set_length(mLayerSymbols);
	singleCodeword(0).zeros();
	singleCodeword(0) = concat(modulatedSymbolsPerCodeWord(0),zeros_c(rem));
	// map the symbols of codeword-0 to the layer-1, layer-2, layer-3 and layer-4 equally
	for(int ithsymbol=0;ithsymbol<mLayerSymbols;ithsymbol++)
	{
	  layeredData(0)(ithsymbol) = singleCodeword(0)(4*ithsymbol);
	  layeredData(1)(ithsymbol) = singleCodeword(0)(4*ithsymbol + 1);
	  layeredData(2)(ithsymbol) = singleCodeword(0)(4*ithsymbol + 2);
	  layeredData(3)(ithsymbol) = singleCodeword(0)(4*ithsymbol + 3);

	};
	rem1 = modulatedSymbolsPerCodeWord(1).length() % 4;
	if(rem1 == 0)
	  rem1 = 4;
	mLayerSymbols = ((modulatedSymbolsPerCodeWord(1).length() + 4 -rem1) /4) ; // to make it multiple of 4
	layeredData(4).set_length(mLayerSymbols);
	layeredData(5).set_length(mLayerSymbols);
	layeredData(6).set_length(mLayerSymbols);
	layeredData(7).set_length(mLayerSymbols);
	singleCodeword(1).zeros();
	singleCodeword(1) = concat(modulatedSymbolsPerCodeWord(1),zeros_c(4 - rem1));
	// map the symbols of codeword-1 to the layer-4, layer-5, layer-6 and layer-7 equally
	for(int ithsymbol=0;ithsymbol<mLayerSymbols;ithsymbol++)
	{
	  layeredData(4)(ithsymbol) = singleCodeword(1)(4*ithsymbol);
	  layeredData(5)(ithsymbol) = singleCodeword(1)(4*ithsymbol + 1);
	  layeredData(6)(ithsymbol) = singleCodeword(1)(4*ithsymbol + 2);
	  layeredData(7)(ithsymbol) = singleCodeword(1)(4*ithsymbol + 3);

	};

      }
      // if codewords are more than two display the error
      else
      {
	cout<<"[both:] The number of code words has to be equal to 2."<< endl;abort();

      };
      break;
      // if the number of layers are more than eight display the error
      default: cout<<"[both:] Invalid number of layers. Transmission Mode: Large Delay CDD or Closed Loop SM or Closed LOop MU-MIMO or Closed Loop 2 Layer DMRS or Closed Loop Max 8 Layer DMRS ."<<endl;abort();

    }

  }

  return layeredData;

}
/*! \fn performLayerDeMapping(...)
 * \param[in,out] equalizedSymbolsPerLayer ---> contains the equalizedSymbols of all layers 
 * \param [in,out] dataGenInfo ---> it is a class variable gives the information about the number of layers and transmission scheme
 * \return equalizedSymbolsPerCodeWord ---> equalized symbols are demapped from number of layers to codeword
 * 
 * \ref ts36.211 relese 10 sec 6.3.3 table:6.3.3.2-1,6.3.3.3-1 */

Array<cvec> performLayerDeMapping(Array<cvec> &equalizedSymbolsPerLayer, int nCodeWords, int nLayers)
{
  int no_symbols,no_symbols1;
  bool singleCodeWord ;
  Array<cvec> equalizedSymbolsPerCodeWord(nCodeWords);

  singleCodeWord = (nCodeWords==1);
      switch(nLayers)
      {
	//if the number of layers and codewords are same do the direct demapping
	case 1:
	  equalizedSymbolsPerCodeWord.set_length(1);
	  equalizedSymbolsPerCodeWord(0) = equalizedSymbolsPerLayer(0);
	break;
	// In case of two layers and single codeword demapp the layer-0 symbols to even positions of codeword and layer-1 to odd positions
	case 2:
	  if(singleCodeWord)
	  {
	    equalizedSymbolsPerCodeWord.set_length(1);

	    no_symbols = equalizedSymbolsPerLayer(0).length();
	    equalizedSymbolsPerCodeWord(0).set_length(no_symbols*2);
	    for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	    {
	      equalizedSymbolsPerCodeWord(0)(2*ithsymbol) = equalizedSymbolsPerLayer(0)(ithsymbol);
	      equalizedSymbolsPerCodeWord(0)(2*ithsymbol+1) = equalizedSymbolsPerLayer(1)(ithsymbol);

	    };
	  }
	  // if the number of codewords are 2 and layers are 2 do the direct demapping
	else
	{
	  equalizedSymbolsPerCodeWord.set_length(2);
	  no_symbols = equalizedSymbolsPerLayer(0).length();
	  equalizedSymbolsPerCodeWord(0).set_length(no_symbols);
	  equalizedSymbolsPerCodeWord(1).set_length(no_symbols);
	  equalizedSymbolsPerCodeWord(0) = equalizedSymbolsPerLayer(0);
	  equalizedSymbolsPerCodeWord(1) = equalizedSymbolsPerLayer(1);
	}
	break;
	// demapping the 3-layers symbols to single codeword equally
	case 3: if(singleCodeWord)
	{
	  equalizedSymbolsPerCodeWord.set_length(1);
	  no_symbols = equalizedSymbolsPerLayer(0).length();
	  equalizedSymbolsPerCodeWord(0).set_length(no_symbols*3);
	  for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	  {
	    equalizedSymbolsPerCodeWord(0)(3*ithsymbol) = equalizedSymbolsPerLayer(0)(ithsymbol);
	    equalizedSymbolsPerCodeWord(0)(3*ithsymbol+1) = equalizedSymbolsPerLayer(1)(ithsymbol);
	    equalizedSymbolsPerCodeWord(0)(3*ithsymbol+2) = equalizedSymbolsPerLayer(2)(ithsymbol);
	  }

	}
	// if the number of codewords are two demap the layer-0 symbols to codeword-0 and layer-1 and layer-2 bits to codeword-1
	else
	{
	  equalizedSymbolsPerCodeWord.set_length(2);
	  no_symbols = equalizedSymbolsPerLayer(0).length();
	  equalizedSymbolsPerCodeWord(0).set_length(no_symbols);
	  equalizedSymbolsPerCodeWord(0) = equalizedSymbolsPerLayer(0);

	  no_symbols1 = equalizedSymbolsPerLayer(1).length();
	  equalizedSymbolsPerCodeWord(1).set_length(no_symbols1*2);
	  for(int ithsymbol=0;ithsymbol< no_symbols1;ithsymbol++)
	  {
	    equalizedSymbolsPerCodeWord(1)(2*ithsymbol) = equalizedSymbolsPerLayer(1)(ithsymbol);
	    equalizedSymbolsPerCodeWord(1)(2*ithsymbol+1) = equalizedSymbolsPerLayer(2)(ithsymbol);

	  }

	};
	break;
       // demap the four layer symbols to single codeword equally
	case 4: if(singleCodeWord)
	{

	  equalizedSymbolsPerCodeWord.set_length(1);
	  no_symbols = equalizedSymbolsPerLayer(0).length();
	  equalizedSymbolsPerCodeWord(0).set_length(no_symbols*4);

	  for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	  {
	    equalizedSymbolsPerCodeWord(0)(4*ithsymbol) = equalizedSymbolsPerLayer(0)(ithsymbol);
	    equalizedSymbolsPerCodeWord(0)(4*ithsymbol + 1) = equalizedSymbolsPerLayer(1)(ithsymbol);;
	    equalizedSymbolsPerCodeWord(0)(4*ithsymbol + 2) = equalizedSymbolsPerLayer(2)(ithsymbol);;
	    equalizedSymbolsPerCodeWord(0)(4*ithsymbol + 3) = equalizedSymbolsPerLayer(3)(ithsymbol);;
	  };

	}
	// demap layer-0 and layer-1 bits to codeword-0 and layer-2 and layer-3 bits to codeword-1 
	else
	{
	  equalizedSymbolsPerCodeWord.set_length(2);
	  no_symbols = equalizedSymbolsPerLayer(0).length();
	  no_symbols1 = equalizedSymbolsPerLayer(1).length();
	  equalizedSymbolsPerCodeWord(0).set_length(no_symbols*2);
	  equalizedSymbolsPerCodeWord(1).set_length(no_symbols1*2);
	  for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	  {
	    equalizedSymbolsPerCodeWord(0)(2*ithsymbol) = equalizedSymbolsPerLayer(0)(ithsymbol);
	    equalizedSymbolsPerCodeWord(0)(2*ithsymbol + 1) = equalizedSymbolsPerLayer(1)(ithsymbol);
	  };
	  for(int ithsymbol=0;ithsymbol<no_symbols1;ithsymbol++)
	  {
	    equalizedSymbolsPerCodeWord(1)(2*ithsymbol) = equalizedSymbolsPerLayer(2)(ithsymbol);
	    equalizedSymbolsPerCodeWord(1)(2*ithsymbol + 1) = equalizedSymbolsPerLayer(3)(ithsymbol);
	  };
	}
	break;
      // if the number of codewords are greater than one and number of layers are 5 then case-5 will be executed
	case 5 : if(! singleCodeWord)
	{
	  equalizedSymbolsPerCodeWord.set_length(2);
	  no_symbols = equalizedSymbolsPerLayer(0).length();
	  equalizedSymbolsPerCodeWord(0).set_length(no_symbols*2);
         // demap the layer-0 and layer-1 symbols to codeword-0 
	  for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	  {
	    equalizedSymbolsPerCodeWord(0)(2*ithsymbol) = equalizedSymbolsPerLayer(0)(ithsymbol);
	    equalizedSymbolsPerCodeWord(0)(2*ithsymbol + 1) = equalizedSymbolsPerLayer(1)(ithsymbol) ;
	  };

	  no_symbols1 = equalizedSymbolsPerLayer(2).length();
	  equalizedSymbolsPerCodeWord(1).set_length(no_symbols1*3);
	  // demap the layer-2,layer-3 and layer-4 symbols to codeword-1
	  for(int ithsymbol=0;ithsymbol<no_symbols1;ithsymbol++)
	  {
	    equalizedSymbolsPerCodeWord(1)(3*ithsymbol) = equalizedSymbolsPerLayer(2)(ithsymbol);
	    equalizedSymbolsPerCodeWord(1)(3*ithsymbol+1) = equalizedSymbolsPerLayer(3)(ithsymbol);
	    equalizedSymbolsPerCodeWord(1)(3*ithsymbol+2) =equalizedSymbolsPerLayer(4)(ithsymbol) ;
	  };
	}
	// if the number of codewords are not equal to 2 display the error
	else
	{
	  cout<<"[both:] The number of code words has to be equal to 2."<< endl;abort();

	}; break;
       // if the number of codewords are greater than one and number of layers are 6 then case-6 will be executed
	case 6: if(!singleCodeWord)
	{

	  equalizedSymbolsPerCodeWord.set_length(2);
	  no_symbols = equalizedSymbolsPerLayer(0).length();
	  equalizedSymbolsPerCodeWord(0).set_length(no_symbols*3);
	  no_symbols1 = equalizedSymbolsPerLayer(3).length();
	  equalizedSymbolsPerCodeWord(1).set_length(no_symbols1*3);
         // demap the layer-0, layer-1 and layer-2 symbols to codeword-0 
	  for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	  {
	    equalizedSymbolsPerCodeWord(0)(3*ithsymbol) = equalizedSymbolsPerLayer(0)(ithsymbol);
	    equalizedSymbolsPerCodeWord(0)(3*ithsymbol+1) = equalizedSymbolsPerLayer(1)(ithsymbol);
	    equalizedSymbolsPerCodeWord(0)(3*ithsymbol+2) =equalizedSymbolsPerLayer(2)(ithsymbol) ;

	  };
         // demap the layer-3,layer-4 and layer-5 symbols to codeword-1
	  for(int ithsymbol=0;ithsymbol<no_symbols1;ithsymbol++)
	  {
	    equalizedSymbolsPerCodeWord(1)(3*ithsymbol) = equalizedSymbolsPerLayer(3)(ithsymbol);
	    equalizedSymbolsPerCodeWord(1)(3*ithsymbol+1) = equalizedSymbolsPerLayer(4)(ithsymbol);
	    equalizedSymbolsPerCodeWord(1)(3*ithsymbol+2) =equalizedSymbolsPerLayer(5)(ithsymbol) ;
	  };


	}
	// if the number of codewords are not equal to 2 display the error
	else
	{
	  cout<<"[both:] The number of code words has to be equal to 2."<< endl;abort();
	}; break;
      // if the number of codewords are greater than one and number of layers are 7 then case-7 will be executed
	case 7 :if(!singleCodeWord)
	{

	  equalizedSymbolsPerCodeWord.set_length(2);
	  no_symbols = equalizedSymbolsPerLayer(0).length();
	  equalizedSymbolsPerCodeWord(0).set_length(no_symbols*3);
	  no_symbols1 = equalizedSymbolsPerLayer(3).length();
	  equalizedSymbolsPerCodeWord(1).set_length(no_symbols1*4);
	  // demap the layer-0, layer-1 and layer-2 symbols to codeword-0 
	  for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	  {
	    equalizedSymbolsPerCodeWord(0)(3*ithsymbol) = equalizedSymbolsPerLayer(0)(ithsymbol);
	    equalizedSymbolsPerCodeWord(0)(3*ithsymbol+1) = equalizedSymbolsPerLayer(1)(ithsymbol);
	    equalizedSymbolsPerCodeWord(0)(3*ithsymbol+2) =equalizedSymbolsPerLayer(2)(ithsymbol);
	  };

         // demap the layer-3, layer-4, layer-5 and layer-6 symbols to codeword-1 
	  for(int ithsymbol=0;ithsymbol<no_symbols1;ithsymbol++)
	  {
	    equalizedSymbolsPerCodeWord(1)(4*ithsymbol) =   equalizedSymbolsPerLayer(3)(ithsymbol);
	    equalizedSymbolsPerCodeWord(1)(4*ithsymbol + 1) = equalizedSymbolsPerLayer(4)(ithsymbol);
	    equalizedSymbolsPerCodeWord(1)(4*ithsymbol + 2) = equalizedSymbolsPerLayer(5)(ithsymbol);
	    equalizedSymbolsPerCodeWord(1)(4*ithsymbol + 3)= equalizedSymbolsPerLayer(6)(ithsymbol);
	  };


	}
	// if the number of codewords are not equal to 2 display the error
	else
	{
	  cout<<"[both:] The number of code words has to be equal to 2."<< endl;abort();
	}; break;
      // if the number of codewords are greater than one and number of layers are 7 then case-7 will be executed
	case 8 : if(!singleCodeWord)
	{

	  equalizedSymbolsPerCodeWord.set_length(2);
	  no_symbols = equalizedSymbolsPerLayer(0).length();
	  no_symbols1 = equalizedSymbolsPerLayer(4).length();
	  equalizedSymbolsPerCodeWord(0).set_length(no_symbols*4);
	  equalizedSymbolsPerCodeWord(1).set_length(no_symbols1*4);
	  // demap the layer-0, layer-1, layer-2 and layer-3 symbols to codeword-0
	  for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	  {
	    equalizedSymbolsPerCodeWord(0)(4*ithsymbol) =   equalizedSymbolsPerLayer(0)(ithsymbol);
	    equalizedSymbolsPerCodeWord(0)(4*ithsymbol + 1) = equalizedSymbolsPerLayer(1)(ithsymbol);
	    equalizedSymbolsPerCodeWord(0)(4*ithsymbol + 2) = equalizedSymbolsPerLayer(2)(ithsymbol);
	    equalizedSymbolsPerCodeWord(0)(4*ithsymbol + 3)= equalizedSymbolsPerLayer(3)(ithsymbol);
	  };


         // demap the layer-4, layer-5, layer-6 and layer-7 symbols to codeword-1
	  for(int ithsymbol=0;ithsymbol<no_symbols1;ithsymbol++)
	  {
	    equalizedSymbolsPerCodeWord(1)(4*ithsymbol) =   equalizedSymbolsPerLayer(4)(ithsymbol);
	    equalizedSymbolsPerCodeWord(1)(4*ithsymbol + 1) = equalizedSymbolsPerLayer(5)(ithsymbol);
	    equalizedSymbolsPerCodeWord(1)(4*ithsymbol + 2) = equalizedSymbolsPerLayer(6)(ithsymbol);
	    equalizedSymbolsPerCodeWord(1)(4*ithsymbol + 3)= equalizedSymbolsPerLayer(7)(ithsymbol);
	  };


	}
	// if the number of codewords are not equal to 2 display the error
	else
	{
	  cout<<"[both:] The number of code words has to be equal to 2."<< endl;abort();
	}; break;
      }
    

  
  return equalizedSymbolsPerCodeWord;
}


Array<vec> performLayerDeMapping(Array<vec> &ppSINRPerLayer, int nCodewords, int nLayers)
{
  int no_symbols,no_symbols1;
  bool singleCodeWord ;
  Array<vec> ppSINRPerCodeWord(nCodewords);
   singleCodeWord = (nCodewords==1);
      switch(nLayers)
      {
	//if the number of layers and codewords are same do the direct demapping
	case 1:
	  ppSINRPerCodeWord.set_length(1);
	  ppSINRPerCodeWord(0) = ppSINRPerLayer(0);
	  break;
	  // In case of two layers and single codeword demapp the layer-0 symbols to even positions of codeword and layer-1 to odd positions
	case 2:
	  if(singleCodeWord)
	  {
	    ppSINRPerCodeWord.set_length(1);
	    
	    no_symbols = ppSINRPerLayer(0).length();
	    ppSINRPerCodeWord(0).set_length(no_symbols*2);
	    for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	    {
	      ppSINRPerCodeWord(0)(2*ithsymbol) = ppSINRPerLayer(0)(ithsymbol);
	      ppSINRPerCodeWord(0)(2*ithsymbol+1) = ppSINRPerLayer(1)(ithsymbol);
	      
	    }
	  }
	  // if the number of codewords are 2 and layers are 2 do the direct demapping
	  else
	  {
	    ppSINRPerCodeWord.set_length(2);
	    no_symbols = ppSINRPerLayer(0).length();
	    ppSINRPerCodeWord(0).set_length(no_symbols);
	    ppSINRPerCodeWord(1).set_length(no_symbols);
	    ppSINRPerCodeWord(0) = ppSINRPerLayer(0);
	    ppSINRPerCodeWord(1) = ppSINRPerLayer(1);
	  }
	  break;
	  // demapping the 3-layers symbols to single codeword equally
	case 3: if(singleCodeWord)
	{
	  ppSINRPerCodeWord.set_length(1);
	  no_symbols = ppSINRPerLayer(0).length();
	  ppSINRPerCodeWord(0).set_length(no_symbols*3);
	  for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	  {
	    ppSINRPerCodeWord(0)(3*ithsymbol) = ppSINRPerLayer(0)(ithsymbol);
	    ppSINRPerCodeWord(0)(3*ithsymbol+1) = ppSINRPerLayer(1)(ithsymbol);
	    ppSINRPerCodeWord(0)(3*ithsymbol+2) = ppSINRPerLayer(2)(ithsymbol);
	  }
	  
	}
	// if the number of codewords are two demap the layer-0 symbols to codeword-0 and layer-1 and layer-2 bits to codeword-1
	else
	{
	  ppSINRPerCodeWord.set_length(2);
	  no_symbols = ppSINRPerLayer(0).length();
	  ppSINRPerCodeWord(0).set_length(no_symbols);
	  ppSINRPerCodeWord(0) = ppSINRPerLayer(0);
	  
	  no_symbols1 = ppSINRPerLayer(1).length();
	  ppSINRPerCodeWord(1).set_length(no_symbols1*2);
	  for(int ithsymbol=0;ithsymbol< no_symbols1;ithsymbol++)
	  {
	    ppSINRPerCodeWord(1)(2*ithsymbol) = ppSINRPerLayer(1)(ithsymbol);
	    ppSINRPerCodeWord(1)(2*ithsymbol+1) = ppSINRPerLayer(2)(ithsymbol);
	    
	  }
	  
	};
	break;
	// demap the four layer symbols to single codeword equally
	case 4: if(singleCodeWord)
	{
	  
	  ppSINRPerCodeWord.set_length(1);
	  no_symbols = ppSINRPerLayer(0).length();
	  ppSINRPerCodeWord(0).set_length(no_symbols*4);
	  
	  for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	  {
	    ppSINRPerCodeWord(0)(4*ithsymbol) = ppSINRPerLayer(0)(ithsymbol);
	    ppSINRPerCodeWord(0)(4*ithsymbol + 1) = ppSINRPerLayer(1)(ithsymbol);;
	    ppSINRPerCodeWord(0)(4*ithsymbol + 2) = ppSINRPerLayer(2)(ithsymbol);;
	    ppSINRPerCodeWord(0)(4*ithsymbol + 3) = ppSINRPerLayer(3)(ithsymbol);;
	  }
	  
	}
	// demap layer-0 and layer-1 bits to codeword-0 and layer-2 and layer-3 bits to codeword-1 
	else
	{
	  ppSINRPerCodeWord.set_length(2);
	  no_symbols = ppSINRPerLayer(0).length();
	  no_symbols1 = ppSINRPerLayer(1).length();
	  ppSINRPerCodeWord(0).set_length(no_symbols*2);
	  ppSINRPerCodeWord(1).set_length(no_symbols1*2);
	  for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	  {
	    ppSINRPerCodeWord(0)(2*ithsymbol) = ppSINRPerLayer(0)(ithsymbol);
	    ppSINRPerCodeWord(0)(2*ithsymbol + 1) = ppSINRPerLayer(1)(ithsymbol);
	  }
	  for(int ithsymbol=0;ithsymbol<no_symbols1;ithsymbol++)
	  {
	    ppSINRPerCodeWord(1)(2*ithsymbol) = ppSINRPerLayer(2)(ithsymbol);
	    ppSINRPerCodeWord(1)(2*ithsymbol + 1) = ppSINRPerLayer(3)(ithsymbol);
	  }
	}
	break;
	// if the number of codewords are greater than one and number of layers are 5 then case-5 will be executed
	case 5 : if(! singleCodeWord)
	{
	  ppSINRPerCodeWord.set_length(2);
	  no_symbols = ppSINRPerLayer(0).length();
	  ppSINRPerCodeWord(0).set_length(no_symbols*2);
	  // demap the layer-0 and layer-1 symbols to codeword-0 
	  for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	  {
	    ppSINRPerCodeWord(0)(2*ithsymbol) = ppSINRPerLayer(0)(ithsymbol);
	    ppSINRPerCodeWord(0)(2*ithsymbol + 1) = ppSINRPerLayer(1)(ithsymbol) ;
	  }
	  
	  no_symbols1 = ppSINRPerLayer(2).length();
	  ppSINRPerCodeWord(1).set_length(no_symbols1*3);
	  // demap the layer-2,layer-3 and layer-4 symbols to codeword-1
	  for(int ithsymbol=0;ithsymbol<no_symbols1;ithsymbol++)
	  {
	    ppSINRPerCodeWord(1)(3*ithsymbol) = ppSINRPerLayer(2)(ithsymbol);
	    ppSINRPerCodeWord(1)(3*ithsymbol+1) = ppSINRPerLayer(3)(ithsymbol);
	    ppSINRPerCodeWord(1)(3*ithsymbol+2) =ppSINRPerLayer(4)(ithsymbol) ;
	  }
	}
	// if the number of codewords are not equal to 2 display the error
	else
	{
	  cout<<"[both:] The number of code words has to be equal to 2."<< endl;abort();
	  
	}; break;
	// if the number of codewords are greater than one and number of layers are 6 then case-6 will be executed
	case 6: if(!singleCodeWord)
	{
	  
	  ppSINRPerCodeWord.set_length(2);
	  no_symbols = ppSINRPerLayer(0).length();
	  ppSINRPerCodeWord(0).set_length(no_symbols*3);
	  no_symbols1 = ppSINRPerLayer(3).length();
	  ppSINRPerCodeWord(1).set_length(no_symbols1*3);
	  // demap the layer-0, layer-1 and layer-2 symbols to codeword-0 
	  for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	  {
	    ppSINRPerCodeWord(0)(3*ithsymbol) = ppSINRPerLayer(0)(ithsymbol);
	    ppSINRPerCodeWord(0)(3*ithsymbol+1) = ppSINRPerLayer(1)(ithsymbol);
	    ppSINRPerCodeWord(0)(3*ithsymbol+2) =ppSINRPerLayer(2)(ithsymbol) ;
	    
	  }
	  // demap the layer-3,layer-4 and layer-5 symbols to codeword-1
	  for(int ithsymbol=0;ithsymbol<no_symbols1;ithsymbol++)
	  {
	    ppSINRPerCodeWord(1)(3*ithsymbol) = ppSINRPerLayer(3)(ithsymbol);
	    ppSINRPerCodeWord(1)(3*ithsymbol+1) = ppSINRPerLayer(4)(ithsymbol);
	    ppSINRPerCodeWord(1)(3*ithsymbol+2) =ppSINRPerLayer(5)(ithsymbol) ;
	  }
	  
	  
	}
	// if the number of codewords are not equal to 2 display the error
	else
	{
	  cout<<"[both:] The number of code words has to be equal to 2."<< endl;abort();
	}; break;
	// if the number of codewords are greater than one and number of layers are 7 then case-7 will be executed
	case 7 :if(!singleCodeWord)
	{
	  
	  ppSINRPerCodeWord.set_length(2);
	  no_symbols = ppSINRPerLayer(0).length();
	  ppSINRPerCodeWord(0).set_length(no_symbols*3);
	  no_symbols1 = ppSINRPerLayer(3).length();
	  ppSINRPerCodeWord(1).set_length(no_symbols1*4);
	  // demap the layer-0, layer-1 and layer-2 symbols to codeword-0 
	  for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	  {
	    ppSINRPerCodeWord(0)(3*ithsymbol) = ppSINRPerLayer(0)(ithsymbol);
	    ppSINRPerCodeWord(0)(3*ithsymbol+1) = ppSINRPerLayer(1)(ithsymbol);
	    ppSINRPerCodeWord(0)(3*ithsymbol+2) =ppSINRPerLayer(2)(ithsymbol);
	  }
	  
	  // demap the layer-3, layer-4, layer-5 and layer-6 symbols to codeword-1 
	  for(int ithsymbol=0;ithsymbol<no_symbols1;ithsymbol++)
	  {
	    ppSINRPerCodeWord(1)(4*ithsymbol) =   ppSINRPerLayer(3)(ithsymbol);
	    ppSINRPerCodeWord(1)(4*ithsymbol + 1) = ppSINRPerLayer(4)(ithsymbol);
	    ppSINRPerCodeWord(1)(4*ithsymbol + 2) = ppSINRPerLayer(5)(ithsymbol);
	    ppSINRPerCodeWord(1)(4*ithsymbol + 3)= ppSINRPerLayer(6)(ithsymbol);
	  }
	  
	  
	}
	// if the number of codewords are not equal to 2 display the error
	else
	{
	  cout<<"[both:] The number of code words has to be equal to 2."<< endl;abort();
	}; break;
	// if the number of codewords are greater than one and number of layers are 7 then case-7 will be executed
	case 8 : if(!singleCodeWord)
	{
	  
	  ppSINRPerCodeWord.set_length(2);
	  no_symbols = ppSINRPerLayer(0).length();
	  no_symbols1 = ppSINRPerLayer(4).length();
	  ppSINRPerCodeWord(0).set_length(no_symbols*4);
	  ppSINRPerCodeWord(1).set_length(no_symbols1*4);
	  // demap the layer-0, layer-1, layer-2 and layer-3 symbols to codeword-0
	  for(int ithsymbol=0;ithsymbol<no_symbols;ithsymbol++)
	  {
	    ppSINRPerCodeWord(0)(4*ithsymbol) =   ppSINRPerLayer(0)(ithsymbol);
	    ppSINRPerCodeWord(0)(4*ithsymbol + 1) = ppSINRPerLayer(1)(ithsymbol);
	    ppSINRPerCodeWord(0)(4*ithsymbol + 2) = ppSINRPerLayer(2)(ithsymbol);
	    ppSINRPerCodeWord(0)(4*ithsymbol + 3)= ppSINRPerLayer(3)(ithsymbol);
	  }
	  
	  
	  // demap the layer-4, layer-5, layer-6 and layer-7 symbols to codeword-1
	  for(int ithsymbol=0;ithsymbol<no_symbols1;ithsymbol++)
	  {
	    ppSINRPerCodeWord(1)(4*ithsymbol) =   ppSINRPerLayer(4)(ithsymbol);
	    ppSINRPerCodeWord(1)(4*ithsymbol + 1) = ppSINRPerLayer(5)(ithsymbol);
	    ppSINRPerCodeWord(1)(4*ithsymbol + 2) = ppSINRPerLayer(6)(ithsymbol);
	    ppSINRPerCodeWord(1)(4*ithsymbol + 3)= ppSINRPerLayer(7)(ithsymbol);
	  }
	  
	  
	}
	// if the number of codewords are not equal to 2 display the error
	else
	{
	  cout<<"[both:] The number of code words has to be equal to 2."<< endl;abort();
	}; break;
      }
      
  return ppSINRPerCodeWord;
}
Array<vec> performLayerDeMapping(vec& sinrPerLayer, int nCodewords, int nLayers)
{
    Array<vec> sinrPerCodeword(nCodewords);
    if(nCodewords==1)
            sinrPerCodeword(0) = sinrPerLayer;
        else 
        {
            int CW1_layers = floor_i((double)nLayers/nCodewords);
            sinrPerCodeword(0) = sinrPerLayer(0,CW1_layers-1);
            sinrPerCodeword(1) = sinrPerLayer(CW1_layers,nLayers-1);
        }
        return sinrPerCodeword;
    
}
Array<vec> performLayerMapping(Array<vec>& sinrPerCodeword,int nCodewords,int nLayers)
{
    Array<vec> sinrPerLayer(nLayers);
    ivec nLayersPerCW(nCodewords);
    nLayersPerCW(0) = floor_i((double)nLayers/(double)nCodewords);
    if(nCodewords==2)
        nLayersPerCW(1) = ceil_i((double)nLayers/(double)nCodewords);
    for(int cw=0;cw<nCodewords;cw++)
    {
        int nTones = sinrPerCodeword(cw).length() / nLayersPerCW(cw);
        for(int l=0;l<nLayersPerCW(cw);l++)
        {
            int layerNum = (cw==0) ? l : nLayersPerCW(0)+l;
            sinrPerLayer(layerNum).set_length(nTones);
            for(int tone=0;tone<nTones;tone++)
            {
                sinrPerLayer(layerNum)(tone) = sinrPerCodeword(cw)(nLayersPerCW(cw)*tone+l);
            }
        }
    }
    return sinrPerLayer;
}




