#ifndef ALI_O2_COMPONENTS_TRACK_CLUSTER_MAPPING_H
#define ALI_O2_COMPONENTS_TRACK_CLUSTER_MAPPING_H
#include "../../ecs/VariableComponent.h"
#include "Ids.h"
namespace ecs {
namespace track {
class ClusterMapping : IVariableComponent {
  uint64_t mIndex;

public:
  ClusterMapping(uint64_t index) : mIndex(index) {}
  ClusterMapping() : mIndex(0) {}
  operator uint64_t() const { return mIndex; }
  uint64_t operator=(uint64_t other) {
    set(other);
    return other;
  }
  void set(uint64_t index) { mIndex = index; }
  static const char *Id() { return "ClusterMapping"; }
};
}
}
#endif
