/***********************************************************************************************
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
 * *********************************************************************************************
 */

#ifndef _SOS_H_
#define _SOS_H_

#include "../../simSupport/include/simSupport.h"
#include "StructsAndEnums.h"

//using namespace itpp;

class SoS{
public:
    // Name of the preset
    ACF_type name;
    
    // Distribution of random variables (Normal or Uniform)
    Distribution distribution;
      
    //double pi = 3.141592653589793238;
    bool isInitialised;
  //private :
    // Decorrelation distance in [m]
    vec dist_decorr;
    // The number of dimensions (1, 2 or 3)    
    vec Pdist_decorr;
    // The number of dimensions (1, 2 or 3)
    int dimensions;
    // Number of sampling frequencies
    int no_coefficients;
    // Vector of sample points for the ACF in [m]
    vec dist;
    
    // Desired aotocorrelation function
    vec acf;
    
    // Sinusoid coefficients (frequencies)
    mat sos_freq;
    
    // Phase offsets
    mat sos_phase;
    
    // Amplitude of the sinusoid coefficients
    double sos_amp;

  public:
    SoS(ACF_type &acf_type, Distribution distribution, vec &dist_decorr) ;
    SoS();
    void set_distribution(Distribution &value);
    void set_dist_decorr(vec &value);
    int  get_no_coefficients();
    int  get_dimensions();
    void printVariables();
    void set_acf( ACF_type &acf_type, Distribution &distribution, vec &dist_decorr );
    //(double ase, vec test_dir)  generate( vec R, vec D, int N, int dim, bool uniform_smp, vec T, bool random_init, bool show_progress );
    void load(std::string  &filename );
    vec val(mat &Rx_positions, mat &Tx_positions );
    vec val(mat &Rx_positions);
    void init(bool use_same );
    mat quantize_ang(mat angles);
    void generate_coefficient(ACF_type &acf_type, Distribution &distribution, vec &dist_decorr);
    void packSphere(const int &N, vec &theta, vec &phi);
    void sosFunctionEstimate(const vec &rminusrr, const vec &dnorm,const double amp, const vec &frRange, const double &frRes, double &fri, vec &rDxn);
    void generateCo(vec &ACF, vec &d, int &nSinusoid,int &dim,bool &unifSamp,int &T,bool &randomInit,vec &frRange,double &frRes);


};
mat randn_sc(mat &dist_decorr, mat &ca, mat &cb, ACF_type acf_type);
mat rand_sc( mat &dist_decorr, mat &ca, mat &cb, ACF_type &acf_type );
mat randu_sc( mat &dist_decorr, mat &ca, mat &cb, ACF_type &acf_type );
imat randi_sc(mat &dist_decorr, mat &ca, mat &cb, int &imax, ACF_type &acf_type);
mat randexp_sc(mat &dist_decorr, mat &ca, mat &cb, double &lambda, ACF_type &acf_type);

mat randn_sc(mat &dist_decorr, mat &ca, ACF_type acf_type);
mat rand_sc( mat &dist_decorr, mat &ca, ACF_type &acf_type );
mat randu_sc( mat &dist_decorr, mat &ca, ACF_type &acf_type );
imat randi_sc(mat &dist_decorr, mat &ca, int &imax, ACF_type &acf_type);
mat randexp_sc(mat &dist_decorr, mat &ca, double &lambda, ACF_type &acf_type);

mat rand_sc(mat& dist_decorr,mat& ca,Distribution distribution,ACF_type acf_type);
mat rand_sc(mat& dist_decorr,mat& ca,mat& cb,Distribution distribution,ACF_type acf_type,int currentID,int totalNum);
mat rand_sc(mat& dist_decorr,mat& ca,Distribution distribution,ACF_type acf_type,int currentID,int totalNum);

#endif
