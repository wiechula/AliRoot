#ifndef ALI_O2_COMPONENENTS_TRACK_PDG_CODE_H
#define ALI_O2_COMPONENENTS_TRACK_PDG_CODE_H
#include "Ids.h"
namespace ecs {
namespace track {
class PdgCode {
  int32_t mIndex;

public:
  PdgCode(int32_t index) : mIndex(index) {}
  PdgCode() : mIndex(0) {}
  operator int32_t() const { return mIndex; }
  int32_t operator=(int32_t other) {
    set(other);
    return other;
  }
  void set(int32_t index) { mIndex = index; }
  static const char *Id() { return "PdgCode"; }
};
}
}
#endif
