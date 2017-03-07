#ifndef ALI_O2_WRAPPER_ESD_EVENT_CONVERTER_H
#define ALI_O2_WRAPPER_ESD_EVENT_CONVERTER_H
#include "Entities/Track.h"
#include "Entities/Vertex.h"

class AliESDEvent;
class ESDEventConverter {
  unsigned mEventCounter = 0;
  using Track_t =
      ecs::Track<ecs::track::ESDEventIndex, ecs::track::X, ecs::track::Y,
                 ecs::track::Z, ecs::track::T, ecs::track::Covariance,
                 ecs::track::Pt, ecs::track::SinAlpha, ecs::track::TanLambda,
                 ecs::track::Mass, ecs::track::MonteCarloIndex,
                 ecs::track::ChargeSign>;
  using Vertex_t =
      ecs::Vertex<ecs::vertex::X, ecs::vertex::Y, ecs::vertex::Z,
                  ecs::vertex::T, ecs::vertex::Covariance,
                  ecs::vertex::ESDEventIndex, ecs::vertex::SignalToNoiseX,
                  ecs::vertex::SignalToNoiseY, ecs::vertex::SignalToNoiseZ,
                  ecs::vertex::ChiSquared, ecs::vertex::Sigma,
                  ecs::vertex::UsedTracksIndices>;

  std::vector<ecs::vertex::X> mVertexX;
  std::vector<ecs::vertex::Y> mVertexY;
  std::vector<ecs::vertex::Z> mVertexZ;
  std::vector<ecs::vertex::T> mVertexT;
  std::vector<ecs::vertex::Covariance> mVertexCovariance;
  std::vector<ecs::vertex::ESDEventIndex> mVertexESDEventIndex;
  std::vector<ecs::vertex::SignalToNoiseX> mVertexSignalToNoiseX;
  std::vector<ecs::vertex::SignalToNoiseY> mVertexSignalToNoiseY;
  std::vector<ecs::vertex::SignalToNoiseZ> mVertexSignalToNoiseZ;
  std::vector<ecs::vertex::ChiSquared> mVertexChiSquared;
  std::vector<ecs::vertex::Sigma> mVertexSigma;
  // TODO: wrap this in a class?
  std::vector<ecs::vertex::UsedTracksIndices> mVertexUsedTracksIndices;
  std::vector<uint64_t> mVertexUsedTracksIndicesMapping;
  size_t mVertexUsedTracksIndicesOffset = 0;

  std::vector<ecs::track::ESDEventIndex> mTrackEventIndex;
  std::vector<ecs::track::X> mTrackX;
  std::vector<ecs::track::Y> mTrackY;
  std::vector<ecs::track::Z> mTrackZ;
  std::vector<ecs::track::T> mTrackT;
  std::vector<ecs::track::Covariance> mTrackCovariance;
  std::vector<ecs::track::Pt> mTrackPt;
  std::vector<ecs::track::SinAlpha> mTrackSinAlpha;
  std::vector<ecs::track::TanLambda> mTrackTanLambda;
  std::vector<ecs::track::Mass> mTrackMass;
  std::vector<ecs::track::MonteCarloIndex> mTrackMonteCarloIndex;
  std::vector<ecs::track::ChargeSign> mTrackChargeSign;

public:
  /// Reads data from an AliESDEvent and copies it into it's own buffers. Uses
  /// the given timestamp as a 'real' event time to generate simulated detection
  /// times.
  void addESDEvent(const AliESDEvent *event, double timestampNs);
  /// Takes the stored data and sends it to a Handler which then outputs the
  /// data to a file. No copies are involved except for N->M Mappings in order
  /// to fit the data in a contigous buffer.
  void toFile(const std::string &filename);
};
#endif
