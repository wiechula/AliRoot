#ifndef ALI_O2_COMPONENENTS_VERTEX_ESD_EVENT_MAPPING_H
#define ALI_O2_COMPONENENTS_VERTEX_ESD_EVENT_MAPPING_H
#include "Ids.h"
namespace ecs {
namespace vertex {
class ESDEventMapping {
  uint32_t mTrackStart;
  uint32_t mTrackCount;
  uint32_t mMcTrackStart;
  uint32_t mMcTrackCount;

public:
  ESDEventMapping(uint32_t TrackStart = 0, uint32_t TrackCount = 0,
                  uint32_t McTrackStart = 0, uint32_t McTrackCount = 0)
      : mTrackStart(TrackStart), mTrackCount(TrackCount),
        mMcTrackStart(McTrackStart), mMcTrackCount(McTrackCount) {}
  static const char *Id() { return "ESDEventMapping"; }
  uint32_t trackStart() const { return mTrackStart; }
  uint32_t trackCount() const { return mTrackCount; }
  uint32_t mcTrackStart() const { return mMcTrackStart; }
  uint32_t mcTrackCount() const { return mMcTrackCount; }
};
}
}
#endif
