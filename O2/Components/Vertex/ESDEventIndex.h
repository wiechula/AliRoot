#ifndef ALI_O2_COMPONENENTS_VERTEX_ESD_EVENT_INDEX_H
#define ALI_O2_COMPONENENTS_VERTEX_ESD_EVENT_INDEX_H
#include "Ids.h"
namespace ecs {
namespace vertex {
class ESDEventIndex {
  uint32_t mIndex;

public:
  ESDEventIndex(uint32_t index) : mIndex(index) {}
  ESDEventIndex() : mIndex(0) {}
  operator uint32_t() const { return mIndex; }
  uint32_t operator=(uint32_t other) {
    set(other);
    return other;
  }
  void set(uint32_t index) { mIndex = index; }
  static uint16_t Id() { return ids::Components::ESDEventIndex; }
};
}
}
#endif
