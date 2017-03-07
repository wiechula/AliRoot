#include "ESDEventConverter.h"
#include <AliESDEvent.h>
#include <TRandom2.h>

using namespace ecs;

void ESDEventConverter::addESDEvent(const AliESDEvent *event,
                                    double timestampNs) {
  TRandom2 rng;
  int numberOfTracks = event->GetNumberOfTracks();
  if (0 == numberOfTracks) {
    return;
  }
  for (int i = 0; i < numberOfTracks; i++) {
    AliESDtrack *esdTrack = event->GetTrack(i);
    AliExternalTrackParam param;
    param.CopyFromVTrack(esdTrack);

    mTrackX.push_back(param.GetX());
    mTrackY.push_back(param.GetY());
    mTrackZ.push_back(param.GetZ());
    mTrackEventIndex.push_back(mEventCounter);
    mTrackT.push_back(timestampNs + rng.Gaus(timestampNs, 100));
    //     // Check if this track is global or ITS
    //     if (NULL == track->GetInnerParam()) {
    //   // ITS track, box the time in bins of 5000ns and assign the central
    //   time
    //   mITSTracks.push_back(
    //       O2ITSTrack(track, 5000.0 * floor(timestampNs / 5000.0) + 2500));
    // }
    // else {
    //   // global track, guass of width 100ns
    //   mGlobalTracks.push_back(
    //       O2GlobalTrack(track, rng.Gaus(timestampNs, 100)));
    // }
  }
  mEventCounter++;
}

/// Takes the stored data and sends it to a Handler which then outputs the
/// data to a file. No copies are involved except for N->M Mappings in order
/// to fit the data in a contigous buffer.
void ESDEventConverter::toFile(const std::string &filename) {
  Handler h;
  size_t trackCount = mTrackX.size();
  h.forceInsertEntity<Track_t>(trackCount);
  h.forceInsertComponentData<Track_t, track::X>(mTrackX.data());
  h.forceInsertComponentData<Track_t, track::Y>(mTrackY.data());
  h.forceInsertComponentData<Track_t, track::Z>(mTrackZ.data());
  h.forceInsertComponentData<Track_t, track::T>(mTrackT.data());
  h.forceInsertComponentData<Track_t, track::ESDEventIndex>(
      mTrackEventIndex.data());
  // h.forceInsertComponentData<Track_t, TrackToClusterMapping>(
  //     mapTable.data(), map.size(), map.data());
  // and we tell h to serialize this data to the the file "buffer.bin"
  h.toFile(filename);
}
