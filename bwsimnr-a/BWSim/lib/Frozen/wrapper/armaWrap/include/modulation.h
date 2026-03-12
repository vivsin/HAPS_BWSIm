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

#ifndef MODULATION_H
#define MODULATION_H
#include<map>
#include<vector>
#include<cmath>
#include<complex>
#include<iostream>
#include"SupportingFunctions.h"

enum DEMOD_METHOD {SECTOR,LOGMAP,APPROX};
enum SECTOR_METHOD {EQUATION,LOOKUP};


struct likelyHood_
{
    double N0;

    likelyHood_(double n0):N0(n0) {};
    void set_NoiseVariance(double n0);
    double operator()(std::complex<double> x ,std::complex<double>  y);

};

struct distSqr_
{

    double operator()(std::complex<double> x ,std::complex<double>  y);

};

namespace wrapper {
struct Modulation
{
	std::map<int ,std::vector< std::complex<double> > > modMtoAlph;
	int M;
	int k;
	std::vector< std::complex<double> > currentMod;

	static constexpr double onebysqrt170 = 1.0 / std::sqrt(170.0);
	static constexpr double onebysqrt10  = 1.0 / std::sqrt(10.0);
	static constexpr double onebysqrt42  = 1.0 / std::sqrt(42.0);
	// Added for sector decoding of QAM
	std::vector<double> shifts;
	std::map<int, std::vector< std::complex<double> > *> closestPtTbl;

	void set_M(int m) {
		M=m;
		k=round_i(log2(M));
		currentMod=modMtoAlph[M];

		// The shifts that comes the NR constellation equation for QAM.
		if (M > 4) {		// Not for QPSK
			std::vector<double> _shifts(k/2 - 1, 0);
			if (M == 256 ) {
				_shifts[0] = 8 * onebysqrt170;
				_shifts[1] = 4 * onebysqrt170;
				_shifts[2] = 2 * onebysqrt170;
			} else if (M == 64) {
				_shifts[0] = 4 * onebysqrt42;
				_shifts[1] = 2 * onebysqrt42;
			} else if (M == 16) {
				_shifts[0] = 2 * onebysqrt10;
			}
			shifts = _shifts;

			// Generate the closest point table.
			genClosestPtsTbl();
		}
	}

	mult_< std::complex<double> , std::complex<double> , std::complex<double> > multC;
	divide_< std::complex<double>, std::complex<double>, std::complex<double> > divC;

	Modulation();

	cVec modulate_bits(const bVec & x);

	dVec demodulate_soft_bits(cVec& rx_vec, const cVec& channel, double N0, DEMOD_METHOD dm);

	void getConstellation_equalized( const std::complex< double > channel, std::vector< std::complex< double > >& chConstellation);

	std::vector< double > getLikelyHood(const std::complex< double > rxValue, std::vector< std::complex< double > >& chConstellation, double n0);

	std::vector<double> getdistanceSquare( const std::complex< double > rxValue, std::vector< std::complex< double > >& chConstellation);

	// Added for sector decoding of QAM
	void
	demodulate_logmap(cVec & rx_vec,
	const cVec & channel, double N0,
	dVec &softbits);

	void
	demodulate_approx(cVec & rx_vec,
	const cVec & channel,double N0,
	dVec &softbits);

	void
	demodulate_sector(cVec & rx_vec,
	const cVec & channel,double N0,
	dVec &softbits);

	void
	decodePAMSymbol(double y, std::vector<bool>::iterator it);

	void
	getClosestPts(const std::vector<bool> &b,
	std::vector< std::complex<double> > &cp);

	void genClosestPtsTbl(void);

};


typedef Modulation QAM;


}
#endif // MODULATION_H
