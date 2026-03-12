// /*************************************************************************
//  * 
//  * CEWiT CONFIDENTIAL
//  * __________________
//  *
//  * All Rights Reserved © 2014 CEWiT, India
//  *
//  * NOTICE: All information contained herein is, and remains the property of Centre of Excellence in Wireless Technology (CEWiT)
//  * and its suppliers, if any. The intellectual and technical concepts contained herein may be proprietary to CEWiT.
//  * Unauthorized use, duplication, reverse engineering, any form of redistribution, or use in part or in whole other than by prior,
//  * express, printed and signed license for use is strictly forbidden.
//  */
// 
// #include "../include/DLSyncDetector.h"
// 
// void DownlinkSyncDetector(cvec input_samps)
// {
//   int N_cp_l_else = 144;
//   
//   /// Find symbol start locations
//   IvecAndInt CT= find_coarse_time_and_freq_offset(input_samps, N_cp_l_else);
//   
//   cout << "start_loc_vec = "<< CT.start_loc_vec << endl;
//   cout << "freq_offset = "<< CT.freq_offset << endl;
//   
//   /// Remove frequency error
//   cvec freq_offset_vec=zeros_c(input_samps.length());
//   vec freq_offset_vec_real=zeros(input_samps.length()),freq_offset_vec_imag=zeros(input_samps.length());
//   
//   for(int i=1;i<input_samps.length();i++) {
//     freq_offset_vec_real(i-1) = cos(-i*CT.freq_offset*2*pi*(0.0005/15360));
//     freq_offset_vec_imag(i-1) = sin(-i*CT.freq_offset*2*pi*(0.0005/15360));
//   }
//   
//   freq_offset_vec = to_cvec(freq_offset_vec_real,freq_offset_vec_imag);
//   input_samps     = elem_div(input_samps,freq_offset_vec);
//   
//   /// Search for PSS and find fine timing offset
//   
//   PSS_struct PSSDemod = find_pss_and_fine_timing(input_samps,CT.start_loc_vec);
//   
//   if(PSSDemod.pss_symb != 0)
//     cout << "Found PSS: N_id_2 = " << PSSDemod.N_id_2 << " in " << PSSDemod.pss_symb << " ("<< PSSDemod.pss_thresh << ") "<<" Start : "<<PSSDemod.fine_start<< endl;
//   else {
//     cout << "ERROR: Didnt find PSS" << endl; abort();
//   }
//   
//   /// Find SSS
//   
//   SSS_struct SSSDemod = find_sss(input_samps,PSSDemod.N_id_2,PSSDemod.fine_start,PSSDemod.pss_thresh);
//   
//   if(SSSDemod.f_start_idx != 0) {
//     if(SSSDemod.f_start_idx < 0)
//       SSSDemod.f_start_idx = SSSDemod.f_start_idx + 307200;
//     
//     cout << "Found SSS: N_id_1 = " << SSSDemod.N_id_1 << ", 0 index is " <<  SSSDemod.f_start_idx << ", cell_id is " << 3*SSSDemod.N_id_1 + PSSDemod.N_id_2 <<endl;
//   }
//   else {
//     cout << "ERROR: Didnt find SSS" << endl;
//     abort();
//   }
//   
// }
// 
// 
// IvecAndInt find_coarse_time_and_freq_offset(cvec in, int N_cp_l_else)
// {
//   
//   IvecAndInt CT;
//   CT.start_loc_vec = zeros_i(7);
//   CT.freq_offset=0;
//   int slot,n,z,index,m,idx;
//   double corr_re,corr_im,abs_corr_max;
//   
//   /// Decompose input
//   vec in_re = real(in);
//   vec in_im = imag(in);
//   
//   /// Can only rely on symbols 0 and 4 because of CRS
//   
//   /// Rough correlation
//   vec abs_corr = zeros(15360);
//   
//   for(slot=0;slot<=10;slot++) {
//     for(n=0;n<15360;n=n+40) {
//       corr_re = 0;
//       corr_im = 0;
//       for(z=0;z<N_cp_l_else;z++) {
// 	index   = (slot*15360) + n + z;
// 	corr_re = corr_re + in_re(index)*in_re(index+2048) + in_im(index)*in_im(index+2048);
// 	corr_im = corr_im + in_re(index)*in_im(index+2048) - in_im(index)*in_re(index+2048);
//       }
//       abs_corr(n) = abs_corr(n) + corr_re*corr_re + corr_im*corr_im;
//     }
//   }
//   
//   /// Find first and second max
//   
//   ivec abs_corr_idx = zeros_i(2);
//   
//   for(m=0;m<=1;m++) {
//     abs_corr_max = 0;
//     for(n=0;n<7680;n++) {
//       if(abs_corr((m*7680)+n) > abs_corr_max) {
// 	abs_corr_max      = abs_corr((m*7680)+n);
// 	abs_corr_idx(m) = (m*7680)+n;
//       }
//     }
//   }
//   
//   /// Fine correlation and fraction frequency offset
//   abs_corr      = zeros(15360);
//   vec corr_freq_err = zeros(15360);
//   
//   for(slot=1;slot<=10;slot++) {
//     for(idx=0;idx<2;idx++) {
//       if((abs_corr_idx(idx) - 40) < 1)
// 	abs_corr_idx(idx) = 40;
//       if((abs_corr_idx(idx) + 40) > 15360)
// 	abs_corr_idx(idx) = 15360 - 40;
//       
//       for(n=abs_corr_idx(idx)-40;n<=abs_corr_idx(idx)+40;n++) {
// 	corr_re = 0;
// 	corr_im = 0;
// 	for(z=0;z<N_cp_l_else;z++) {
// 	  index = (slot*15360) + n + z;
// 	  corr_re = corr_re + in_re(index)*in_re(index+2048) + in_im(index)*in_im(index+2048);
// 	  corr_im = corr_im + in_re(index)*in_im(index+2048) - in_im(index)*in_re(index+2048);
// 	}
// 	abs_corr(n)      = abs_corr(n) + corr_re*corr_re + corr_im*corr_im;
// 	corr_freq_err(n) = corr_freq_err(n) + atan2(corr_im, corr_re)/(2048*2*pi*(0.0005/15360));
//       }
//     }
//   }
//   
//   
//   /// Find first and second max
//   abs_corr_idx = zeros_i(2);
//   
//   for(m=0;m<2;m++) {
//     abs_corr_max = 0;
//     for(n=0;n<7680;n++) {
//       if(abs_corr((m*7680)+n) > abs_corr_max) {
// 	abs_corr_max      = abs_corr((m*7680)+n);
// 	abs_corr_idx(m) = (m*7680)+n;
//       }
//     }
//   }
//   
//   /// Determine frequency offset FIXME No integer offset is calculated here
//   CT.freq_offset = (corr_freq_err(abs_corr_idx(0))/10 + corr_freq_err(abs_corr_idx(1))/10)/2;
//   
//   /// Determine the symbol start locations from the correlation peaks
//   /// FIXME Needs some work
//   
//   int tmp = abs_corr_idx(0);
//   
//   if(tmp > 0)
//     tmp = tmp - 2192;
//   
//   for(n=0;n<7;n++) {
//     CT.start_loc_vec(n) = tmp + ((n+1)*2192);
//   }
//   
//   return CT;
// }
// 
// 
// PSS_struct find_pss_and_fine_timing(cvec input,ivec coarse_start)
// {
//   PSS_struct PSSDemod;
//   PSSDemod.fine_start=zeros_i(7); PSSDemod.N_id_2=0;PSSDemod.pss_symb=0;PSSDemod.pss_thresh=0;
//   cvec pss,symb,pss_corr=zeros_c(1);
//   //complex< double > pss_corr;
//   
//   /// DEFINES Assuming 20MHz
//   int N_rb_dl      = 100;
//   int N_sc_rb      = 12; // Only dealing with normal cp at this time
//   int N_symb_dl    = 7;  // Only dealing with normal cp at this time
//   int FFT_pad_size = 424;
//   
//   /// Generate primary synchronization signals
//   cmat pss_mod_vec = zeros_c(3, N_rb_dl*N_sc_rb);
//   
//   for(int loc_N_id_2=0;loc_N_id_2<=2;loc_N_id_2++) {
//     pss = generate_pss(loc_N_id_2);
//     for(int n=0;n<=61;n++) {
//       int k = n - 31 + (N_rb_dl*N_sc_rb)/2;
//       pss_mod_vec(loc_N_id_2, k) = pss(n);
//     }
//   }
//   
//   mat pss_corr_mat=zeros(84,3);
//   
//   /// Demod symbols and correlate with primary synchronization signals
//   int num_slots_to_demod = floor_i(200000/15360.0)-1;
//   for(int n=0;n<num_slots_to_demod;n++) {
//     for(int m=0;m<N_symb_dl;m++) {
//       symb = (samps_to_symbs(input,coarse_start(m)+(15360*n),0,FFT_pad_size,0));
//       symb = conj(symb);
//       for(int loc_N_id_2=0;loc_N_id_2<=2;loc_N_id_2++) {
// 	pss_corr(0) = 0;
// 	for(int z=0;z<N_rb_dl*N_sc_rb;z++) {
// 	  pss_corr(0) = pss_corr(0) + symb(z)*pss_mod_vec(loc_N_id_2, z);
// 	}
// 	pss_corr_mat((n*N_symb_dl)+m, loc_N_id_2) = abs(pss_corr(0));
//       }
//     }
//   }
// //   cout << "pss_corr_mat = " << pss_corr_mat << endl;
//   ivec abs_slot_num=zeros_i(3);
//   vec val(3);
//   for(int ii=0;ii<3;ii++)
//   {
//     val(ii) = max(pss_corr_mat.get_col(ii));
//     abs_slot_num(ii) = max_index(pss_corr_mat.get_col(ii));
//   }
//   
//   /// Find maximum
//   
// //   cout<<"val : "<<val<<endl;
//   
//   PSSDemod.N_id_2 = max_index(val);
//   PSSDemod.pss_symb = abs_slot_num(PSSDemod.N_id_2);
//   
//   /// Find optimal timing
//   int  N_s = floor_i((abs_slot_num(PSSDemod.N_id_2))/7.0);
//   int N_symb = rem(abs_slot_num(PSSDemod.N_id_2), 7);
//   vec timing_vec=zeros(81);
//   for(int timing=-40;timing<=40;timing++) {
//     
//     symb = (samps_to_symbs(input, coarse_start(N_symb)+timing+(15360*N_s), 0, FFT_pad_size, 0));
//     symb = conj(symb);
//     pss_corr(0) = 0;
//     
//     for(int z=0;z<N_rb_dl*N_sc_rb;z++)
//       pss_corr(0) = pss_corr(0) + symb(z)*pss_mod_vec(PSSDemod.N_id_2, z);
//     
//     timing_vec(timing+40) = abs(pss_corr(0));
//   }
//   
//   PSSDemod.pss_thresh = max(timing_vec);
//   int timing_plus_40 = max_index(timing_vec);
//   
//   /// Construct fine symbol start locations
//   int pss_timing_idx = coarse_start(N_symb)+(15360*N_s)+timing_plus_40-40;
//   PSSDemod.fine_start(0)  = pss_timing_idx + (2048+144)*1 - 15360;
//   PSSDemod.fine_start(1)  = pss_timing_idx + (2048+144)*1 + 2048+160 - 15360;
//   PSSDemod.fine_start(2)  = pss_timing_idx + (2048+144)*2 + 2048+160 - 15360;
//   PSSDemod.fine_start(3)  = pss_timing_idx + (2048+144)*3 + 2048+160 - 15360;
//   PSSDemod.fine_start(4)  = pss_timing_idx + (2048+144)*4 + 2048+160 - 15360;
//   PSSDemod.fine_start(5)  = pss_timing_idx + (2048+144)*5 + 2048+160 - 15360;
//   PSSDemod.fine_start(6)  = pss_timing_idx + (2048+144)*6 + 2048+160 - 15360;
//   
//   if(PSSDemod.fine_start(0) < 1)
//     PSSDemod.fine_start = PSSDemod.fine_start + 307200;
//   
//   
//   //cout << "PSSDemod.pss_thresh = " << PSSDemod.pss_thresh << endl;
//     //cout << "timing_plus_40 = " << timing_plus_40 << endl;
//     
//     
//     return PSSDemod;
// }
// 
// SSS_struct find_sss(cvec input ,int N_id_2, ivec start ,double pss_thresh)
// {
//   
//   SSS_struct SSSDemod;
//   SSSDemod.N_id_1=0,SSSDemod.f_start_idx=0;
//   
//   int k;
//   /// DEFINES Assuming 20MHz
//   int N_rb_dl      = 100;
//   int N_sc_rb      = 12; /// Only dealing with normal cp at this time
//   int N_cp_l_0     = 160;
//   int N_cp_l_else  = 144;
//   int FFT_pad_size = 424;
//   
//   /// Generate secondary synchronization signals
//   SSS S;
//   cmat sss_mod_vec_0 = zeros_c(168, N_rb_dl*N_sc_rb);
//   cmat sss_mod_vec_5 = zeros_c(168, N_rb_dl*N_sc_rb);
//   
//   for(int loc_N_id_1=0;loc_N_id_1<=167;loc_N_id_1++) {
//     S = generate_sss(loc_N_id_1, N_id_2);
//     for(int m=0;m<=61;m++) {
//       k = m - 31 + (N_rb_dl*N_sc_rb)/2;
//       sss_mod_vec_0(loc_N_id_1, k) = S.sss_d_u_0(m);
//       sss_mod_vec_5(loc_N_id_1, k) = S.sss_d_u_5(m);
//     }
//   }
//   
//   double sss_thresh = pss_thresh * .9;
//   
//   cout<<"Search SSS "<<endl;
//   /// Demod symbol and search for secondary synchronization signals
//   cvec symb =samps_to_symbs(input,start(5), 0, FFT_pad_size, 0);
//   
//   cvec sss_corr=zeros_c(1);
//   
//   for(int loc_N_id_1=0;loc_N_id_1<=167;loc_N_id_1++) {
//     sss_corr=zeros_c(1);
//     for(int m=0;m<N_rb_dl*N_sc_rb;m++) {
//       sss_corr(0) = sss_corr(0) + symb(m)*sss_mod_vec_0(loc_N_id_1, m);
//     }
//     
//     if(abs(sss_corr(0)) > sss_thresh) {
//       SSSDemod.N_id_1      = loc_N_id_1;
//       SSSDemod.f_start_idx = start(5) - ((2048+N_cp_l_else)*4 + 2048+N_cp_l_0);
//       break;
//     }
//     
//     sss_corr=zeros_c(1);
//     for(int m=0;m<N_rb_dl*N_sc_rb;m++) {
//       sss_corr(0) = sss_corr(0) + symb(m)*sss_mod_vec_5(loc_N_id_1, m);
//     }
//     
//     if(abs(sss_corr(0)) > sss_thresh) {
//       SSSDemod.N_id_1      = loc_N_id_1;
//       SSSDemod.f_start_idx = start(5) - ((2048+N_cp_l_else)*4 + 2048+N_cp_l_0);
//       SSSDemod.f_start_idx = SSSDemod.f_start_idx - (15360*10);
//       break;
//     }
//   }
//   return SSSDemod;
// }
// 
// 
// 
// cvec samps_to_symbs(cvec samps, int slot_start_idx, int symb_offset, int FFT_pad_size, int scale)
// {
//   int CP_len;
//   cvec symbs;
//   
//   /// Calculate index and CP length
//   if(mod(symb_offset,7)==0) CP_len = 160;
//   else CP_len = 144;
//   
//   int index = slot_start_idx + (2048+144)*symb_offset;
//   if(symb_offset > 0) index = index + 16;
//   
//   /// Take FFT
//   cvec temp = fft(samps(index+CP_len,index+CP_len+2047));
//   cvec tmp = fftShift(temp);
//   // Remove DC subcarrier
//   cvec tmp_symbs = concat(tmp(FFT_pad_size,1023), tmp(1025,2047-(FFT_pad_size-1)));
//   
//   vec symbs_angle = angle(tmp_symbs),symbs_real,symbs_imag;
//   
//   if(scale == 0)
//     symbs = tmp_symbs;
//   else {
//     for(int n=0;n<tmp_symbs.length();n++) {
//       symbs_real(n) = cos(symbs_angle(n));
//       symbs_imag(n) = sin(symbs_angle(n));
//       symbs = to_cvec(symbs_real,symbs_imag);
//     }
//   }
//   return symbs;
// }
