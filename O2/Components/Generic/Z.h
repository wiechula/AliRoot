#ifndef ALI_O2_COMPONENTS_GENERIC_Z_H
#define ALI_O2_COMPONENTS_GENERIC_Z_H

namespace ecs {
namespace generic {
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
