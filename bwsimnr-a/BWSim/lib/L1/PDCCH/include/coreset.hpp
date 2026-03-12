#pragma once
#ifndef CORESET_CORESET_H
#define CORESET_CORESET_H

#include "../../../Frozen/simSupport/include/simSupport.h"
#include "../../../Frozen/channelCoder/Polar/include/polarCodec.hpp"
#include "controlResourceSetIE.hpp"

// higher layer paras ControlResourceSet
template <class T>
class Coreset {
  // Higher layer parameters
  // ControlResourceSetIE
  ControlResourceSetIE controlResourceSetIE;

  // Alises for easy access.
  int &controlResourceSetId =
      controlResourceSetIE.controlResourceSetId.ControlResourceSetId;
  int &duration        = controlResourceSetIE.duration;
  int &regBundleSize   = controlResourceSetIE.interleaved.regBundleSize;
  int &interleaverSize = controlResourceSetIE.interleaved.interleaverSize;
  int &shiftIndex      = controlResourceSetIE.interleaved.shiftIndex;
  PrecoderGranularity &precoderGranularity =
      controlResourceSetIE.precoderGranularity;

  // Variables set using the higher layer parameters
  // Used in ts 38 211 doc.
  int N_rb_coreset   = -1;
  int N_symb_coreset = -1;
  int N_reg_coreset  = -1;
  int N_shift        = -1;
  int n_RNTI         = -1;
  int N_id           = -1;
  int N_id_cell      = -1;

  // Parameters required for DM-RS for PDCCH
  int N_sf    = -1;
  int dmrslen = 500; // TODO: Set appropriate dmrs lenght.

  // Internal variables configured based on the higher layer parameters.
  // Used for implementation purpose only, is not present in any TS docs.
  bool isInterleaved = false;

  // Number of symbols & bits in a CCE
  static constexpr int n_symbol_cce = 9 * 6;
  static constexpr int n_bits_cce   = n_symbol_cce * 2;

  // Internal functions
  int
  getSeed() {
    return ((n_RNTI << 16) + N_id);
  }

  PolarCodec<double, bool> codec;
  QAM qpskModem;

public:
  explicit Coreset(const ControlResourceSetIE &IE);

  void
  setSlotNumber(const int N_sf) {
    this->N_sf = N_sf;
  }

  void
  setC_RNTI(const int rnti) {
    this->n_RNTI = rnti;
  }

  void
  setCellID(int id) {
    this->N_id_cell = id;
    this->N_shift   = id;
    this->N_id      = id;
  }

  int
  getRNTI() const {
    return this->n_RNTI;
  }

  void
  printValues(void);

  std::map<int, int>
  genInterleaverMap(int aggregationLevel);

  // Tx side functions
  //
  void
  scrambler(std::vector<T> &in_bits);

  std::vector<std::complex<double>>
  modulater(const std::vector<T> &in_bits);

  std::map<int, std::vector<std::complex<double>>>
  interleaver(std::vector<std::complex<double>> &in_symbs);
  // interleaver(std::vector < std::complex< double >>&);

  std::map<int, std::vector<std::complex<double>>>
  mapToPhyRB(std::map<int, std::vector<std::complex<double>>> &regMap);

  // Rx
  std::vector<std::complex<double>>
  extractCCE(std::map<int, std::vector<std::complex<double>>> &phyMap,
             int aggregationLevel);

  // std::vector < T >
  std::vector<double>
  demodulater(std::vector<std::complex<double>> &modSymbs,
              std::vector<std::complex<double>> &channel, double N0);

  std::vector<T>
  decode(std::vector<double> &soft_bits, int n_DCI, int aggregationLevel);

  void
  descrambler(std::vector<double> &llrs);

  std::map<int, std::vector<std::complex<double>>>
  getCoresetSymbols(std::vector<T> &dciBits, int aggregationLevel);

  std::vector<T>
  getDciBits(std::map<int, std::vector<std::complex<double>>> &phyMap,
             int dciSize, int aggregationLevel,
             std::vector<std::complex<double>> &channel, double N0);

  bvec
  decodeCoreset(cvec coresetSymbols, cvec coresetChannel, int dciSize,
                int aggregationLevel, double N0);

  bool
  decode(std::vector<double> &soft_bits, int n_DCI, int aggregationLevel,
         std::vector<T> &decodedDCI);

  bool
  decodeCoreset(cvec coresetSymbols, cvec coresetChannel, int dciSize,
                int aggregationLevel, double N0, bvec &decodeDCI);
};

#endif // _CORESET_H
