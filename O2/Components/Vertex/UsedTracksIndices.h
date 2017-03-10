#ifndef ALI_O2_COMPONENENTS_VERTEX_USED_TRACKS_INDICES_H
#define ALI_O2_COMPONENENTS_VERTEX_USED_TRACKS_INDICES_H
#include "../../ecs/VariableComponent.h"
#include "Ids.h"
namespace ecs {
namespace vertex {
class UsedTracksIndices : IVariableComponent {
  uint32_t mIndex;

public:
  UsedTracksIndices(uint32_t index) : mIndex(index) {}
  UsedTracksIndices() : mIndex(0) {}
  operator uint32_t() const { return mIndex; }
  uint32_t operator=(uint32_t other) {
    set(other);
    return other;
  }
  void set(uint32_t index) { mIndex = index; }
  static const char *Id() { return "UsedTracksIndices"; }
};
}
}
#endif
