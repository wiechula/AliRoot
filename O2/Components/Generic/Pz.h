#ifndef ALI_O2_COMPONENTS_GENERIC_PZ_H
#define ALI_O2_COMPONENTS_GENERIC_PZ_H

namespace ecs {
namespace generic {
class Pz {
  float mPz;

public:
  Pz(float Pz) : mPz(Pz) {}
  Pz() : mPz(0) {}
  operator float() const { return mPz; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float Pz) { mPz = Pz; }
  static const char *Id() { return "Pz"; }
};
}
}
#endif
