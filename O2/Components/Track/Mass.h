#ifndef ALI_O2_COMPONENTS_TRACK_MASS_H
#define ALI_O2_COMPONENTS_TRACK_MASS_H
#include "Ids.h"
namespace ecs {
namespace track {
class Mass {
  float mMass;

public:
  Mass(float Mass) : mMass(Mass) {}
  Mass() : mMass(0) {}
  operator float() const { return mMass; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float Mass) { mMass = Mass; }
  static const char *Id() { return "Mass"; }
};
}
}
#endif
