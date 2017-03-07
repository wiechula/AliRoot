#ifndef ALI_O2_COMPONENTS_TRACK_T_H
#define ALI_O2_COMPONENTS_TRACK_T_H
#include "Ids.h"
namespace ecs {
namespace track {
class T {
  float mT;

public:
  T(float T) : mT(T) {}
  T() : mT(0) {}
  operator float() const { return mT; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float T) { mT = T; }
  static uint16_t Id() { return ids::Components::T; }
};
}
}
#endif
