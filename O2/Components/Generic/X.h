#ifndef ALI_O2_COMPONENTS_GENERIC_X_H
#define ALI_O2_COMPONENTS_GENERIC_X_H

namespace ecs {
namespace generic {
class X {
  float mX;

public:
  X(float X) : mX(X) {}
  X() : mX(0) {}
  operator float() const { return mX; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float X) { mX = X; }
  static const char *Id() { return "X"; }
};
}
}
#endif
