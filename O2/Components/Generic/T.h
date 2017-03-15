#ifndef ALI_O2_COMPONENTS_GENERIC_T_H
#define ALI_O2_COMPONENTS_GENERIC_T_H

namespace ecs {
namespace generic {
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
  static const char *Id() { return "T"; }
};
}
}
#endif
