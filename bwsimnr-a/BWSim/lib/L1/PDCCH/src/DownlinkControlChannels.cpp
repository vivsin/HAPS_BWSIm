#include <map>
#include <random>
#include <vector>

#include "../include/DownlinkControlChannels.hpp"
#include "../include/coreset.hpp"

void
GenerateAndFillCoreset(SchedulerToTransmitterInfo_S schedulerToTransmitterInfo,
                       Array<cmat> &txFrame, RBInfo_S &dlRbInfoForUnScheduled,
                       int subframeNumber, int pdcchCount,
                       int cellID, int fftSize,int nDLRBs) {
  // CORESET parameters.
  // Part of ControlResourceSet in TS 38.311
  int controlResourceSetId = 0; // TODO: Always coreset 0 is sent.
  // bit map frequencyDomainResources which gives N_rb_coreset.
  int duration = pdcchCount;
  // cce-REG-MappingType which gives isInterleaved, regBundleSize,
  // interleaverSize and shiftIndex.

  // Variables got by phrasing the details of ControlResourceSet IE.
  Cce_REG_MappingType cce_REG_MappingType = nonInterleaved;

  int regBundleSize   = 6;
  int interleaverSize = 2;
  int shiftIndex      = 0;

  int aggregationLevel = 8; // TODO: change of needed
  bool rbs[45]         = {0};
  // TODO: Make sure that this size and the expected size matches.
  int N_CCE            = 5;
  assert (nDLRBs >= (N_CCE * 6) && "Insuffecient nDLRBs provided");

  for (int i = 0; i < N_CCE; ++i) {
    rbs[i] = true;
  }
  int pdcch_DMRS_ScramblingID             = -1;
  PrecoderGranularity precoderGranularity = allContiguousRBs;

  ControlResourceSetId id;
  id.ControlResourceSetId = controlResourceSetId;

  Interleaved interleaved;
  interleaved.regBundleSize   = regBundleSize;
  interleaved.interleaverSize = interleaverSize;
  interleaved.shiftIndex      = shiftIndex;

  ControlResourceSetIE IE;
  IE.controlResourceSetId = id;
  for (int i = 0; i < 45; ++i)
    IE.frequencyDomainResources[i] = rbs[i];
  IE.duration                = duration;
  IE.cce_REG_MappingType     = cce_REG_MappingType;
  IE.interleaved             = interleaved;
  IE.precoderGranularity     = precoderGranularity;
  IE.pdcch_DMRS_ScramblingID = pdcch_DMRS_ScramblingID;

  if (schedulerToTransmitterInfo.dataBlkInfo.length() > 0) {
    // TODO: Only transmitting CORESET 0 no
    //       If needed, make changes for UE CORESET.

    // Create the coreset object
    Coreset<bool> coreset0(IE);
    coreset0.setSlotNumber(subframeNumber);
    coreset0.setCellID(cellID);
    coreset0.setC_RNTI(0);

    std::cout << "[detl:]Transmitting CORESET 0 for cellID = "
              << cellID <<  " subframeNumber = " << subframeNumber << std::endl;
    std::cout << "[detl:] Coreset using AL = " << aggregationLevel;
    std::cout << " duration = " << pdcchCount;
    std::cout << " nDLRBs = " << nDLRBs << " N_CCE = " << N_CCE << std::endl;

    // TODO: Generate DCI payload bits and size accordingly.
    std::vector<bool> dciPayloadBits(40, 1);
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<int> uniform(0, 1);
    std::generate(dciPayloadBits.begin(), dciPayloadBits.end(),
                  [&uniform, &engine]() { return (bool)uniform(engine); });

    // Generate the coreset symbols from the DCI payload.
    // The returned object is map whose element represent an OFDM symbol.
    // In one symbol, the entire RB's allocated for the coreset is filled.
    // The size of this map will be pdcchCount x (nDLRBs * 12).
    // TODO: Verify that that the size mentioned above is correct.
    auto coresetSymbolsMap =
        coreset0.getCoresetSymbols(dciPayloadBits, aggregationLevel);

    // We need to converted the map generated above to a single vector
    cvec coresetSymbols(coresetSymbolsMap.size() * coresetSymbolsMap[0].size());
    int cnt = 0;
    for (auto &symbol : coresetSymbolsMap) {
      for (auto &re : symbol.second) {
        coresetSymbols(cnt++) = re;
      }
    }

    // Find all the sub Frames(?) which suportt the pdcch tones.
    // The pdcch tones has been assigned values 50, 51, 52(?).
    imat logicalSubFrameStructure = dlRbInfoForUnScheduled.logicalSubFrameStructure;

    // Get the tones scheduled for pdcch.
    // This is a static allocation, always the first two symbols are reserved
    // for pdcch.
    ivec pdcchTones = find(logicalSubFrameStructure,
                           50 + pdcchCount - 1, true);

    // TODO: Not all the nDLRBs are used we use only floor(nDLRBs / 6) Rbs.
    //       So if nDLRBs is not a multiple of 6 there could be an issue.
    //       Fix may include just zero padding the additional RE's so that
    //       the final size matches.
    // TODO: Maybe include a assert below to make sure that the size matches.
    int usedSubcarriers           = N_CCE * 6 * 12 * pdcchCount;
    int guardSubcarriersInOneSide = (fftSize - usedSubcarriers) / 2;
    ivec pdcchFFTTones(pdcchTones.length());
    for (int i = 0; i < pdcchTones.length(); i++) {
      pdcchFFTTones(i) = pdcchTones(i) + guardSubcarriersInOneSide;
      if (pdcchFFTTones(i) >= fftSize / 2)
        pdcchFFTTones(i) += 1;
    }

    // Reducet the size of FFT tones, so that the sizes match
    pdcchFFTTones = pdcchFFTTones(0, coresetSymbols.length() - 1);

    // perform layer mapping. Since we have only one layer,
    // use a precoder with all ones and appropriate scaling factor.
    // Same as repeating all the same symbols in each antenna, but with scaling
    int nAntenna = txFrame.length();
    Array<cvec> coresetSymbolsPrecoded(nAntenna);
    for (int i = 0; i < nAntenna; ++i) {
      coresetSymbolsPrecoded(i) = coresetSymbols / complex<double>(sqrt(nAntenna), 0);;
    }

    for(int i = 0; i < nAntenna; ++i) {
      setInMatrix(txFrame(i),pdcchFFTTones,coresetSymbolsPrecoded(i));
    }
  }
}
