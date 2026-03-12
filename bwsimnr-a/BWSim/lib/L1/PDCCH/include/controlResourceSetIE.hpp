#pragma once

// Higher layer parameters
// Refer 3GPP TS 38.311 sec 6.3.2 p81
enum PrecoderGranularity { sameAsREGBundle, allContiguousRBs };

struct ControlResourceSetId {
  int ControlResourceSetId = -1;
};

struct Interleaved {
  int regBundleSize   = -1;
  int interleaverSize = -1;
  int shiftIndex      = -1;
};

enum Cce_REG_MappingType { interleaved, nonInterleaved };

// ControlResourceSetIE
struct ControlResourceSetIE {
  ControlResourceSetId controlResourceSetId;
  bool frequencyDomainResources[45] = {false};
  int duration                      = -1;
  Cce_REG_MappingType cce_REG_MappingType;
  Interleaved interleaved;
  PrecoderGranularity precoderGranularity;
  int pdcch_DMRS_ScramblingID = -1;
};
