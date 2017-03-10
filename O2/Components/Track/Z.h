#ifndef ALI_O2_COMPONENTS_TRACK_Z_H
#define ALI_O2_COMPONENTS_TRACK_Z_H
#include "Ids.h"
namespace ecs {
namespace track {
class Z {
  float mZ;

public:
  Z(float Z) : mZ(Z) {}
  Z() : mZ(0) {}
  operator float() const { return mZ; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float Z) { mZ = Z; }
  static const char *Id() { return "Z"; }
};
}
}
#endif
