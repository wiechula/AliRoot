#ifndef ALI_O2_COMPONENTS_TRACK_Y_H
#define ALI_O2_COMPONENTS_TRACK_Y_H
#include "Ids.h"
namespace ecs {
namespace track {
class Y {
  float mY;

public:
  Y(float Y) : mY(Y) {}
  Y() : mY(0) {}
  operator float() const { return mY; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float Y) { mY = Y; }
  static const char *Id() { return "Y"; }
};
}
}
#endif
