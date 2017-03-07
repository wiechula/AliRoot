#ifndef ALI_O2_COMPONENTS_TRACK_PT_H
#define ALI_O2_COMPONENTS_TRACK_PT_H
#include "Ids.h"
namespace ecs {
namespace track {
class Pt {
  float mPt;

public:
  Pt(float Pt) : mPt(Pt) {}
  Pt() : mPt(0) {}
  operator float() const { return mPt; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float Pt) { mPt = Pt; }
  static uint16_t Id() { return ids::Components::Pt; }
};
}
}
#endif
