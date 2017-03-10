#ifndef ALI_O2_COMPONENENTS_TRACK_MONTE_CARLO_INDEX_H
#define ALI_O2_COMPONENENTS_TRACK_MONTE_CARLO_INDEX_H
#include "Ids.h"
namespace ecs {
namespace track {
class MonteCarloIndex {
  int32_t mIndex;

public:
  MonteCarloIndex(int32_t index) : mIndex(index) {}
  MonteCarloIndex() : mIndex(0) {}
  operator int32_t() const { return mIndex; }
  int32_t operator=(int32_t other) {
    set(other);
    return other;
  }
  void set(int32_t index) { mIndex = index; }
  static const char *Id() { return "MonteCarloIndex"; }
};
}
}
#endif
