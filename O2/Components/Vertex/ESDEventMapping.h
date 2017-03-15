#ifndef ALI_O2_COMPONENENTS_VERTEX_ESD_EVENT_MAPPING_H
#define ALI_O2_COMPONENENTS_VERTEX_ESD_EVENT_MAPPING_H
#include <cstdint>
namespace ecs {
namespace vertex {
class ESDEventMapping {
  uint32_t mTrackStart;
  uint32_t mTrackCount;

public:
  ESDEventMapping(uint32_t TrackStart = 0, uint32_t TrackCount = 0)
      : mTrackStart(TrackStart), mTrackCount(TrackCount) {}
  static const char *Id() { return "ESDEventMapping"; }
  uint32_t trackStart() const { return mTrackStart; }
  uint32_t trackCount() const { return mTrackCount; }
};
}
}
#endif
