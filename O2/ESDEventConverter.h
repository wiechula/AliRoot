#ifndef ALI_O2_WRAPPER_ESD_EVENT_CONVERTER_H
#define ALI_O2_WRAPPER_ESD_EVENT_CONVERTER_H
#include "Entities/Track.h"

class AliESDEvent;
class ESDEventConverter {
  unsigned mEventCounter = 0;
  using Track_t = ecs::Track<ecs::track::X, ecs::track::Y, ecs::track::Z,
                             ecs::track::T, ecs::track::ESDEventIndex>;
  std::vector<ecs::track::ESDEventIndex> mTrackEventIndex;
  std::vector<ecs::track::X> mTrackX;
  std::vector<ecs::track::Y> mTrackY;
  std::vector<ecs::track::Z> mTrackZ;
  std::vector<ecs::track::T> mTrackT;

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
