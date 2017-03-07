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
  const AliESDVertex *vertex = event->GetVertex();
  mVertexX.push_back(vertex->GetX());
  mVertexY.push_back(vertex->GetY());
  mVertexZ.push_back(vertex->GetZ());
  mVertexT.push_back(timestampNs +
                     rng.Gaus(timestampNs, sqrt(10000 * numberOfTracks)));
  Double_t cov[6];
  vertex->GetCovarianceMatrix(cov);
  mVertexCovariance.push_back(cov);
  mVertexESDEventIndex.push_back(mEventCounter);
  mVertexSignalToNoiseX.push_back(vertex->GetXSNR());
  mVertexSignalToNoiseY.push_back(vertex->GetYSNR());
  mVertexSignalToNoiseZ.push_back(vertex->GetZSNR());
  mVertexChiSquared.push_back(vertex->GetChi2());
  mVertexSigma.push_back(vertex->GetDispersion());
  int nIndices = vertex->GetNIndices();
  // How many tracks were already added before this event.
  size_t trackIndexOffset = mTrackX.size();
  UShort_t *indices = vertex->GetIndices();
  for (int i = 0; i < nIndices; i++) {
    mVertexUsedTracksIndices.push_back(indices[i] + trackIndexOffset);
  }
  mVertexUsedTracksIndicesMapping.push_back(mVertexUsedTracksIndicesOffset);
  mVertexUsedTracksIndicesMapping.push_back(nIndices);
  mVertexUsedTracksIndicesOffset += nIndices;

  for (int i = 0; i < numberOfTracks; i++) {
    AliESDtrack *esdTrack = event->GetTrack(i);
    AliExternalTrackParam param;
    param.CopyFromVTrack(esdTrack);

    mTrackEventIndex.push_back(mEventCounter);
    mTrackX.push_back(param.GetX());
    mTrackY.push_back(param.GetY());
    mTrackZ.push_back(param.GetZ());
    mTrackT.push_back(timestampNs + rng.Gaus(timestampNs, 100));
    Double_t cov[15];
    esdTrack->GetExternalCovariance(cov);
    mTrackCovariance.push_back(cov);
    mTrackPt.push_back(param.Pt());
    mTrackSinAlpha.push_back(param.GetSnp());
    mTrackTanLambda.push_back(param.GetTgl());
    mTrackMass.push_back(param.M());
    mTrackMonteCarloIndex.push_back(esdTrack->GetLabel());
    mTrackChargeSign.push_back(param.Charge() > 0);
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

  size_t vertexCount = mVertexX.size();
  h.forceInsertEntity<Vertex_t>(vertexCount);
  h.forceInsertComponentData<Vertex_t, vertex::X>(mVertexX.data());
  h.forceInsertComponentData<Vertex_t, vertex::Y>(mVertexY.data());
  h.forceInsertComponentData<Vertex_t, vertex::Z>(mVertexZ.data());
  h.forceInsertComponentData<Vertex_t, vertex::T>(mVertexT.data());
  h.forceInsertComponentData<Vertex_t, vertex::Covariance>(
      mVertexCovariance.data());
  h.forceInsertComponentData<Vertex_t, vertex::ESDEventIndex>(
      mVertexESDEventIndex.data());
  h.forceInsertComponentData<Vertex_t, vertex::SignalToNoiseX>(
      mVertexSignalToNoiseX.data());
  h.forceInsertComponentData<Vertex_t, vertex::SignalToNoiseY>(
      mVertexSignalToNoiseY.data());
  h.forceInsertComponentData<Vertex_t, vertex::SignalToNoiseZ>(
      mVertexSignalToNoiseZ.data());
  h.forceInsertComponentData<Vertex_t, vertex::ChiSquared>(
      mVertexChiSquared.data());
  h.forceInsertComponentData<Vertex_t, vertex::Sigma>(mVertexSigma.data());
  // TODO: insert this N->M data
  h.forceInsertComponentData<Vertex_t, vertex::UsedTracksIndices>(
      mVertexUsedTracksIndicesMapping.data(), mVertexUsedTracksIndices.size(),
      mVertexUsedTracksIndices.data());

  size_t trackCount = mTrackX.size();
  h.forceInsertEntity<Track_t>(trackCount);
  h.forceInsertComponentData<Track_t, track::Covariance>(
      mTrackCovariance.data());
  h.forceInsertComponentData<Track_t, track::Pt>(mTrackPt.data());
  h.forceInsertComponentData<Track_t, track::SinAlpha>(mTrackSinAlpha.data());
  h.forceInsertComponentData<Track_t, track::TanLambda>(mTrackTanLambda.data());
  h.forceInsertComponentData<Track_t, track::Mass>(mTrackMass.data());
  h.forceInsertComponentData<Track_t, track::MonteCarloIndex>(
      mTrackMonteCarloIndex.data());
  h.forceInsertComponentData<Track_t, track::ChargeSign>(
      mTrackChargeSign.data());
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
