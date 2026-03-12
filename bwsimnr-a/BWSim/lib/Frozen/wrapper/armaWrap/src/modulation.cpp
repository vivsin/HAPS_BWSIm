/*************************************************************************
*
* CEWiT CONFIDENTIAL
* __________________
*
* All Rights Reserved © 2014 CEWiT, India
*
\ NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
* and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
* Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
* express, printed and signed license for use is strictly forbidden.
*/

#include "../include/modulation.h"
#include<list>

// TO store the closeset point of all constellation points.
std::list<std::vector< std::complex<double> > > nearestNeighbourVectorList;

// BPSK modulation
static const double l1_2[2] = {1.0, -1.0}; 	// (1 - 2b(n))
static const double l2_1[2] = {-1.0, 1.0}; 	// (1 - 2(~b(n)))

double distSqr_::operator()(std::complex<double> x ,std::complex<double>  y)
{
    return wrapper::sqr(x-y);
}

void likelyHood_::set_NoiseVariance(double n0) {
    N0=n0;
}
double likelyHood_::operator()(std::complex<double> x ,std::complex<double>  y)
{
    return exp(-(wrapper::sqr(x-y)/N0));
}


//local functions for 256QAM as per NR formula
std::vector<bool> split_bits(int value,int nbits)// function to split 'nbits' bit  from given integer MSB goes to first bit and LSB goes to last
{
    std::vector<bool> bools(nbits);
    for(auto x=bools.rbegin();x!=bools.rend();++x)
    {
        *x=value&1;
        value>>=1;
    }
    return bools;

}

// Convert a bit vector to and interger
int bitToNum(const std::vector<bool> &b) {
	int m = 0;
	int base = 0;
	for(auto x=b.rbegin(); x!=b.rend(); ++x) {
		m |= (*x) << base++;
	}
	return m;
}

std::vector< std::complex<double> > getNRConstellation(int M)
{
    if(M==256 ) //38.211 5.1.6 as of now only 256 QAM
    {
        const int k=8;//log2(M=256)
        const double  onebysqrt170=1.0/sqrt(170);// tocheck the constellation points make this value 1
        std::vector< std::complex<double> > cp; cp.reserve(M);//place to store final constellation points, reserving size as push_back is used to fill
        for(int a=0;a<M;++a)
        {
//38.211 gives the formula as given below . Here assumption  include lowest index bit stored in  MSB
           std::vector<bool> b=split_bits(a,k);//splitting bits

               double xr=onebysqrt170*l1_2[b[0]]*(  8.0-(l1_2[b[2]]*(  4.0 -(l1_2[b[4]]*( 2.0-l1_2[b[6]] ))   ))     );//even bits go to form real axis
               double xi=onebysqrt170*l1_2[b[1]]*(  8.0-(l1_2[b[3]]*(  4.0 -(l1_2[b[5]]*( 2.0-l1_2[b[7]] ))   ))     );//odd bits go to imag axis

               std::complex<double> x(xr,xi);
               cp.push_back(x);

        }
        return cp;//returning final constellation point for 256 QAM
    }
    else
    {
        std::cerr<<"Error due to wrong parameter.. as of now call this only for 256 QAM(M=256) but  M="<<M<<" aborting..."<<std::endl;
        abort();
    }



}

namespace wrapper {
Modulation::Modulation()
{
    {
         std::complex<double>  a[]= {  std::complex<double> (1./std::sqrt(2.0),1.0/std::sqrt(2.0)),- std::complex<double> (1./std::sqrt(2.0),1.0/std::sqrt(2.0))};
        std::vector< std::complex<double> > modSymbols(a,a+2);

        modMtoAlph[2]= modSymbols;
    }

    {
         std::complex<double>  a[]= {  std::complex<double> (1./std::sqrt(2.0),1.0/std::sqrt(2.0)), std::complex<double> (1./std::sqrt(2.0),-1.0/std::sqrt(2.0)), std::complex<double> (-1./std::sqrt(2.0),1.0/std::sqrt(2.0)), std::complex<double> (-1./std::sqrt(2.0),-1.0/std::sqrt(2.0))};
        std::vector< std::complex<double> > modSymbols(a,a+4);
        modMtoAlph[4]= modSymbols;
    }


    {
         std::complex<double>  a[]= {
             std::complex<double> (1./std::sqrt(10.0),1.0/std::sqrt(10.0)),
             std::complex<double> (1./std::sqrt(10.0),3.0/std::sqrt(10.0)),
             std::complex<double> (3./std::sqrt(10.0),1.0/std::sqrt(10.0)),
             std::complex<double> (3./std::sqrt(10.0),3.0/std::sqrt(10.0)),

             std::complex<double> (1./std::sqrt(10.0),-1.0/std::sqrt(10.0)),
             std::complex<double> (1./std::sqrt(10.0),-3.0/std::sqrt(10.0)),
             std::complex<double> (3./std::sqrt(10.0),-1.0/std::sqrt(10.0)),
             std::complex<double> (3./std::sqrt(10.0),-3.0/std::sqrt(10.0)),

             std::complex<double> (-1./std::sqrt(10.0),1.0/std::sqrt(10.0)),
             std::complex<double> (-1./std::sqrt(10.0),3.0/std::sqrt(10.0)),
             std::complex<double> (-3./std::sqrt(10.0),1.0/std::sqrt(10.0)),
             std::complex<double> (-3./std::sqrt(10.0),3.0/std::sqrt(10.0)),

             std::complex<double> (-1./std::sqrt(10.0),-1.0/std::sqrt(10.0)),
             std::complex<double> (-1./std::sqrt(10.0),-3.0/std::sqrt(10.0)),
             std::complex<double> (-3./std::sqrt(10.0),-1.0/std::sqrt(10.0)),
             std::complex<double> (-3./std::sqrt(10.0),-3.0/std::sqrt(10.0)),
        };

        std::vector< std::complex<double> > modSymbols(a,a+16);
        modMtoAlph[16]= modSymbols;
    }


    {
         std::complex<double>  a[]= {

             std::complex<double> (3./std::sqrt(42.0),3.0/std::sqrt(42.0)),
             std::complex<double> (3./std::sqrt(42.0),1.0/std::sqrt(42.0)),
             std::complex<double> (1./std::sqrt(42.0),3.0/std::sqrt(42.0)),
             std::complex<double> (1./std::sqrt(42.0),1.0/std::sqrt(42.0)),

             std::complex<double> (3./std::sqrt(42.0),5.0/std::sqrt(42.0)),
             std::complex<double> (3./std::sqrt(42.0),7.0/std::sqrt(42.0)),
             std::complex<double> (1./std::sqrt(42.0),5.0/std::sqrt(42.0)),
             std::complex<double> (1./std::sqrt(42.0),7.0/std::sqrt(42.0)),

             std::complex<double> (5./std::sqrt(42.0),3.0/std::sqrt(42.0)),
             std::complex<double> (5./std::sqrt(42.0),1.0/std::sqrt(42.0)),
             std::complex<double> (7./std::sqrt(42.0),3.0/std::sqrt(42.0)),
             std::complex<double> (7./std::sqrt(42.0),1.0/std::sqrt(42.0)),

             std::complex<double> (5./std::sqrt(42.0),5.0/std::sqrt(42.0)),
             std::complex<double> (5./std::sqrt(42.0),7.0/std::sqrt(42.0)),
             std::complex<double> (7./std::sqrt(42.0),5.0/std::sqrt(42.0)),
             std::complex<double> (7./std::sqrt(42.0),7.0/std::sqrt(42.0)),




             std::complex<double> (3./std::sqrt(42.0),-3.0/std::sqrt(42.0)),
             std::complex<double> (3./std::sqrt(42.0),-1.0/std::sqrt(42.0)),
             std::complex<double> (1./std::sqrt(42.0),-3.0/std::sqrt(42.0)),
             std::complex<double> (1./std::sqrt(42.0),-1.0/std::sqrt(42.0)),

             std::complex<double> (3./std::sqrt(42.0),-5.0/std::sqrt(42.0)),
             std::complex<double> (3./std::sqrt(42.0),-7.0/std::sqrt(42.0)),
             std::complex<double> (1./std::sqrt(42.0),-5.0/std::sqrt(42.0)),
             std::complex<double> (1./std::sqrt(42.0),-7.0/std::sqrt(42.0)),

             std::complex<double> (5./std::sqrt(42.0),-3.0/std::sqrt(42.0)),
             std::complex<double> (5./std::sqrt(42.0),-1.0/std::sqrt(42.0)),
             std::complex<double> (7./std::sqrt(42.0),-3.0/std::sqrt(42.0)),
             std::complex<double> (7./std::sqrt(42.0),-1.0/std::sqrt(42.0)),

             std::complex<double> (5./std::sqrt(42.0),-5.0/std::sqrt(42.0)),
             std::complex<double> (5./std::sqrt(42.0),-7.0/std::sqrt(42.0)),
             std::complex<double> (7./std::sqrt(42.0),-5.0/std::sqrt(42.0)),
             std::complex<double> (7./std::sqrt(42.0),-7.0/std::sqrt(42.0)),





             std::complex<double> (-3./std::sqrt(42.0),3.0/std::sqrt(42.0)),
             std::complex<double> (-3./std::sqrt(42.0),1.0/std::sqrt(42.0)),
             std::complex<double> (-1./std::sqrt(42.0),3.0/std::sqrt(42.0)),
             std::complex<double> (-1./std::sqrt(42.0),1.0/std::sqrt(42.0)),

             std::complex<double> (-3./std::sqrt(42.0),5.0/std::sqrt(42.0)),
             std::complex<double> (-3./std::sqrt(42.0),7.0/std::sqrt(42.0)),
             std::complex<double> (-1./std::sqrt(42.0),5.0/std::sqrt(42.0)),
             std::complex<double> (-1./std::sqrt(42.0),7.0/std::sqrt(42.0)),

             std::complex<double> (-5./std::sqrt(42.0),3.0/std::sqrt(42.0)),
             std::complex<double> (-5./std::sqrt(42.0),1.0/std::sqrt(42.0)),
             std::complex<double> (-7./std::sqrt(42.0),3.0/std::sqrt(42.0)),
             std::complex<double> (-7./std::sqrt(42.0),1.0/std::sqrt(42.0)),

             std::complex<double> (-5./std::sqrt(42.0),5.0/std::sqrt(42.0)),
             std::complex<double> (-5./std::sqrt(42.0),7.0/std::sqrt(42.0)),
             std::complex<double> (-7./std::sqrt(42.0),5.0/std::sqrt(42.0)),
             std::complex<double> (-7./std::sqrt(42.0),7.0/std::sqrt(42.0)),




             std::complex<double> (-3./std::sqrt(42.0),-3.0/std::sqrt(42.0)),
             std::complex<double> (-3./std::sqrt(42.0),-1.0/std::sqrt(42.0)),
             std::complex<double> (-1./std::sqrt(42.0),-3.0/std::sqrt(42.0)),
             std::complex<double> (-1./std::sqrt(42.0),-1.0/std::sqrt(42.0)),

             std::complex<double> (-3./std::sqrt(42.0),-5.0/std::sqrt(42.0)),
             std::complex<double> (-3./std::sqrt(42.0),-7.0/std::sqrt(42.0)),
             std::complex<double> (-1./std::sqrt(42.0),-5.0/std::sqrt(42.0)),
             std::complex<double> (-1./std::sqrt(42.0),-7.0/std::sqrt(42.0)),

             std::complex<double> (-5./std::sqrt(42.0),-3.0/std::sqrt(42.0)),
             std::complex<double> (-5./std::sqrt(42.0),-1.0/std::sqrt(42.0)),
             std::complex<double> (-7./std::sqrt(42.0),-3.0/std::sqrt(42.0)),
             std::complex<double> (-7./std::sqrt(42.0),-1.0/std::sqrt(42.0)),

             std::complex<double> (-5./std::sqrt(42.0),-5.0/std::sqrt(42.0)),
             std::complex<double> (-5./std::sqrt(42.0),-7.0/std::sqrt(42.0)),
             std::complex<double> (-7./std::sqrt(42.0),-5.0/std::sqrt(42.0)),
             std::complex<double> (-7./std::sqrt(42.0),-7.0/std::sqrt(42.0)),
        };

        std::vector< std::complex<double> > modSymbols(a,a+64);
        modMtoAlph[64]= modSymbols;
    }
    {
           modMtoAlph[256]= getNRConstellation(256);
    }
}

cVec Modulation::modulate_bits(const bVec& x)
{
    int symlength=x.size()/k;
    int xsize=x.size();

    //first ensure x.size() is a multiple of k
    if(xsize%k != 0)
    {
        std::cout<<"[both:] Warning in modulate_bits input size"<<x.size()<<" is not a multiple of k "<<k<<" adding zeros to end"<<endl;
        symlength++;
    }
    iVec symbolBits(symlength);
    for(int sidx=0,bit=0; sidx<symlength; sidx++)
    {
        symbolBits(sidx)=0;
        for(int bidx=k-1; (bidx>=0)&&(bit<xsize); bidx--,bit++)
        {
            symbolBits(sidx)|=((x[bit]&1)<<bidx);
        }
    }

    cVec op(symlength);
    for(int i=0; i<symlength; i++)
    {
        op[i]=currentMod[symbolBits[i]];
    }
    return op;
}

void Modulation::getConstellation_equalized(const std::complex<double>  channel, std::vector< std::complex<double> > & chConstellation)
{
    binary_Function_scalar(currentMod.begin(),currentMod.end(),channel,chConstellation.begin(),multC);
}


std::vector<double> Modulation::getLikelyHood(const std::complex<double>  rxValue, std::vector<  std::complex<double>  >& chConstellation,double n0)
{
    std::vector<double>likelymetric(M);
    likelyHood_ likelyhood(n0);
    scalar_binary_Function(rxValue,chConstellation.begin(),chConstellation.end(),likelymetric.begin(),likelyhood);
    return likelymetric;
}

std::vector<double> Modulation::getdistanceSquare(const std::complex<double>  rxValue, std::vector<  std::complex<double>  >& chConstellation)
{
    std::vector<double>distSqrmetric(M);
    distSqr_ distSqr;
    scalar_binary_Function(rxValue,chConstellation.begin(),chConstellation.end(),distSqrmetric.begin(),distSqr);
    return distSqrmetric;
}


dVec Modulation::demodulate_soft_bits(cVec & rx_vec,const cVec & channel,double N0,DEMOD_METHOD dm)
{
	int rsize=rx_vec.size();
	if(rsize != channel.size())
	{
		std::cout<<"[both:] Error In demodulate_soft_bits, size of input and channel is not matching...exiting"<<endl;
		exit(1);
	}
	dVec softbits(rsize*k);

	if (dm == SECTOR)
		demodulate_sector(rx_vec, channel, N0, softbits);
	else if (dm == APPROX)
		demodulate_approx(rx_vec, channel, N0, softbits);
	else if (dm == LOGMAP)
		demodulate_logmap(rx_vec, channel, N0, softbits);

	return softbits;
}

void
Modulation::demodulate_logmap(cVec & rx_vec,
const cVec & channel, double N0,
dVec &softbits)
{
	int rsize = rx_vec.size();
	for(int symIdx=0; symIdx<rsize; symIdx++)
	{
		std::vector< std::complex<double> > chConstellation(M);

		getConstellation_equalized(channel(symIdx),chConstellation);

		std::vector<double> lmetric=getLikelyHood(rx_vec(symIdx),chConstellation,N0);

		for(int bit=0; bit<k; bit++)
		{
			double likelyhood0=0;
			double likelyhood1=0;
			for(int alph=0; alph<M; alph++)
			{
				(((alph>>bit)&1)==0)?likelyhood0+=lmetric[alph]:likelyhood1+=lmetric[alph];
			}
			
			if(likelyhood0<=0)
			{
				std::cout<<"[both:] Error value of likelyhood0 is zero, negative making it non-Zero 1e-10"<<endl;
				likelyhood0=1e-10;
			}
			
			if(likelyhood1<=0)
			{
				std::cout<<"[both:] Error value of likelyhood1 is zero, negative making it non-Zero 1e-10"<<endl;
				likelyhood1=1e-10;
			}
			
			softbits[(symIdx+1)*k-bit-1]=std::log(likelyhood0)-std::log(likelyhood1);
		}
	}
}

void
Modulation::demodulate_approx(cVec & rx_vec,
const cVec & channel,double N0,
dVec &softbits)
{
	int rsize = rx_vec.size();
	for(int symIdx=0; symIdx<rsize; symIdx++)
	{
		std::vector< std::complex<double> > chConstellation(M);
		getConstellation_equalized(channel(symIdx),chConstellation);

		std::vector<double> lmetric=getdistanceSquare(rx_vec(symIdx),chConstellation);

		for(int bit=0; bit<k; bit++)
		{
			double dist0=std::numeric_limits<double>::max();
			double dist1=std::numeric_limits<double>::max();
			for(int alph=0; alph<M; alph++)
			{
				if(((alph>>bit)&1)==0){ if(dist0>lmetric[alph])dist0=lmetric[alph];}
				else{if(dist1>lmetric[alph])dist1=lmetric[alph];}
			}

			softbits[(symIdx+1)*k-bit-1]=(dist1-dist0)/N0;
		}
	}
}

void
Modulation::demodulate_sector(cVec & rx_vec,
const cVec & channel,double N0,
dVec &softbits) {
	int rsize = rx_vec.size();

	// Use APPROX method for QPSK and BPSK
	if (M <= 4) {
		demodulate_approx(rx_vec, channel, N0, softbits);
		return;
	}

	// Which method to use for finding the closest points.
	SECTOR_METHOD sm = LOOKUP;

	std::vector<bool> demodBits(k);
	std::vector< std::complex<double> > * closestPts;
	if(sm==EQUATION)
	{
		closestPts=new std::vector< std::complex<double> >(k);
	}

	std::vector< std::complex<double> > closestPtsEq(k);
	std::vector<double> lmetric1(k);

	for(int symIdx=0; symIdx<rsize; symIdx++) {
		// Divide the rx value by channel coeff
		std::complex<double> yByh = divC(rx_vec[symIdx], channel[symIdx]);

		// Find the bits of the nearest sector
		// Bits corrsponding to real part goes to even bit location
		// and bits of imaginary part goes to odd location.
		// Decode the real and imaginary part by a PAM decoder.
		decodePAMSymbol(std::real(yByh), demodBits.begin());
		decodePAMSymbol(std::imag(yByh), demodBits.begin() + 1);
		int sectorPtIdx = bitToNum(demodBits); 	// Find its index number
		// Get the constellation point.
		std::complex<double> sectorPt = currentMod[sectorPtIdx];

		// Find the closest point to the given sector.
		if(sm == EQUATION)
			getClosestPts(demodBits, *closestPts);
		else if(sm == LOOKUP) 
			closestPts = closestPtTbl[sectorPtIdx];

		// Multiply the constellation by the channel coeffeicient
		wrapper::binary_Function_scalar(closestPts->begin(), closestPts->end(),
										channel[symIdx], closestPtsEq.begin(), multC);
		std::complex<double>  sectorPtEq = multC(channel[symIdx], sectorPt);

		// Find the distance between the received vector and the sector point.
		distSqr_ distSqr;
		double lmetric0 = distSqr(rx_vec[symIdx], sectorPtEq);

		// Find the distance between the received vector and the closest points.
		wrapper::scalar_binary_Function(rx_vec[symIdx], closestPtsEq.begin(),
										closestPtsEq.end(), lmetric1.begin(), distSqr);

		// The final LLRs for the symbol.
		int start = symIdx * k; 
		for(int i=0; i<k; i++) {
			// softbits[start + i] = (1 - 2*demodBits[i]) * (lmetric1[i] - lmetric0) / N0;
			if (demodBits[i])
				softbits[start + i] = (lmetric0 - lmetric1[i]) / N0;
			else
				softbits[start + i] = (lmetric1[i] - lmetric0) / N0;
		}
	}
	if(sm==EQUATION)
        delete closestPts;
}

void
Modulation::decodePAMSymbol(double y, std::vector<bool>::iterator it) {
	// double sign = 0;

	// First check if the point is +ve or -ve
	// If y is +ve then (1 - 2b(0)) is +ve,i.e , b(0) = 0;
	// So +ve sign means 0 and -ve corresponds to 1.
	// sign = std::copysign(1.0, y);
	*it = (y < 0.0);
	it += 2;

	// Once we determine where the point is located, i.e, +ve or -ve,
	// then we can ignore the sign 
	y = std::abs(y);
	// y *= sign;
	// if (y < 0.0)
	// 	y = -y;

	// For other bits we are finding the sign of -(1 - 2b(n))
	// So -ve means 0 and positive means 1;
	for(double shift:shifts) {
		y -= shift;

		// sign = std::copysign(1.0, y);
		*it = (y > 0.0);
		it += 2;

		y = std::abs(y);
		// y *= sign;
		// if (y < 0.0)
		// 	y = -y;
	}
}

void
Modulation::getClosestPts(const std::vector<bool> &b,
std::vector< std::complex<double> > &cp) {
	int i = 0;

	if (M == 256) {

		// The real and imaginary part of the point.
		double ptReal = onebysqrt170 * l1_2[b[0]] * (8.0 - (l1_2[b[2]] *
							(4.0 - (l1_2[b[4]] * (2.0 - l1_2[b[6]])))));
		double ptImag = onebysqrt170 * l1_2[b[1]] * (8.0 - (l1_2[b[3]] *
							(4.0 - (l1_2[b[5]] * (2.0 - l1_2[b[7]])))));

		// Point differing at pos=0
		double ptReal0 = onebysqrt170 * l2_1[b[0]];
		// Point differing at pos=1
		double ptImag1 = onebysqrt170 * l2_1[b[1]];
		// Point differing at pos=2
		double ptReal2 = onebysqrt170 * l1_2[b[0]] * (8.0 - l2_1[b[2]]);
		// Point differing at pos=3
		double ptImag3 = onebysqrt170 * l1_2[b[1]] * (8.0 - l2_1[b[3]]);
		// Point differing at pos=4
		double ptReal4 = onebysqrt170 * l1_2[b[0]] * (8.0 - (l1_2[b[2]] *
											(4.0 - l2_1[b[4]])));
		// Point differing at pos=5
		double ptImag5 = onebysqrt170 * l1_2[b[1]] * (8.0 - (l1_2[b[3]] *
											(4.0 - l2_1[b[5]])));
		// Point differing at pos=6
		double ptReal6 = onebysqrt170 * l1_2[b[0]] * (8.0 - (l1_2[b[2]] *
								(4.0 - (l1_2[b[4]] * (2.0 - l2_1[b[6]])))));
		// Point differing at pos=7
		double ptImag7 = onebysqrt170 * l1_2[b[1]] * (8.0 - (l1_2[b[3]] *
								(4.0 - (l1_2[b[5]] * (2.0 - l2_1[b[7]])))));

		// Pos=0
		cp[i++] = (std::complex<double>(ptReal0, ptImag));
		// Pos=1
		cp[i++] = (std::complex<double>(ptReal, ptImag1));
		// Pos=2
		cp[i++] = (std::complex<double>(ptReal2, ptImag));
		// Pos=3
		cp[i++] = (std::complex<double>(ptReal, ptImag3));
		// Pos=4
		cp[i++] = (std::complex<double>(ptReal4, ptImag));
		// Pos=5
		cp[i++] = (std::complex<double>(ptReal, ptImag5));
		// Pos=6
		cp[i++] = (std::complex<double>(ptReal6, ptImag));
		// Pos=7
		cp[i++] = (std::complex<double>(ptReal, ptImag7));
	} else if (M == 64) {

		// The real and imaginary part of the point.
		double ptReal = onebysqrt42 * l1_2[b[0]] * (4.0 - (l1_2[b[2]] *
											(2.0 - (l1_2[b[4]]))));
		double ptImag = onebysqrt42 * l1_2[b[1]] * (4.0 - (l1_2[b[3]] *
											(2.0 - (l1_2[b[5]]))));

		// Point differing at pos=0
		double ptReal0 = onebysqrt42 * l2_1[b[0]];
		// Point differing at pos=1
		double ptImag1 = onebysqrt42 * l2_1[b[1]];
		// Point differing at pos=2
		double ptReal2 = onebysqrt42 * l1_2[b[0]] * (4.0 - l2_1[b[2]]);
		// Point differing at pos=3
		double ptImag3 = onebysqrt42 * l1_2[b[1]] * (4.0 - l2_1[b[3]]);
		// Point differing at pos=4
		double ptReal4 = onebysqrt42 * l1_2[b[0]] * (4.0 - (l1_2[b[2]] *
												(2.0 - l2_1[b[4]])));
		// Point differing at pos=5
		double ptImag5 = onebysqrt42 * l1_2[b[1]] * (4.0 - (l1_2[b[3]] *
												(2.0 - l2_1[b[5]])));

		// Pos=0
		cp[i++] = (std::complex<double>(ptReal0, ptImag));
		// Pos=1
		cp[i++] = (std::complex<double>(ptReal, ptImag1));
		// Pos=2
		cp[i++] = (std::complex<double>(ptReal2, ptImag));
		// Pos=3
		cp[i++] = (std::complex<double>(ptReal, ptImag3));
		// Pos=4
		cp[i++] = (std::complex<double>(ptReal4, ptImag));
		// Pos=5
		cp[i++] = (std::complex<double>(ptReal, ptImag5));
	} else if (M == 16) {

		// The real and imaginary part of the point.
		double ptReal = onebysqrt10 * l1_2[b[0]] * (2.0 - l1_2[b[2]]);
		double ptImag = onebysqrt10 * l1_2[b[1]] * (2.0 - l1_2[b[3]]);

		// Point differing at pos=0
		double ptReal0 = onebysqrt10 * l2_1[b[0]];
		// Point differing at pos=1
		double ptImag1 = onebysqrt10 * l2_1[b[1]];
		// Point differing at pos=2
		double ptReal2 = onebysqrt10 * l1_2[b[0]] * (2.0 - l2_1[b[2]]);
		// Point differing at pos=3
		double ptImag3 = onebysqrt10 * l1_2[b[1]] * (2.0 - l2_1[b[3]]);

		// Pos=0
		cp[i++] = (std::complex<double>(ptReal0, ptImag));
		// Pos=1
		cp[i++] = (std::complex<double>(ptReal, ptImag1));
		// Pos=2
		cp[i++] = (std::complex<double>(ptReal2, ptImag));
		// Pos=3
		cp[i++] = (std::complex<double>(ptReal, ptImag3));
	} else {
		std::cerr<<"Implemneted only for {256, 64, 16} QAM. M = "<<M<<std::endl;
		abort();
	}
}

void Modulation::genClosestPtsTbl(void) {
	for(int i=0; i<M; i++)  {
		std::vector< std::complex< double > > pts(k);
		
		getClosestPts(split_bits(i, k), pts);
		nearestNeighbourVectorList.push_front(pts);
		closestPtTbl[i] = &(nearestNeighbourVectorList.front());
	}
}

}
