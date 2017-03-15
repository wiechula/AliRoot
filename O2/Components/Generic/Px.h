#ifndef ALI_O2_COMPONENTS_GENERIC_PX_H
#define ALI_O2_COMPONENTS_GENERIC_PX_H

namespace ecs {
namespace generic {
class Px {
  float mPx;

public:
  Px(float Px) : mPx(Px) {}
  Px() : mPx(0) {}
  operator float() const { return mPx; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float Px) { mPx = Px; }
  static const char *Id() { return "Px"; }
};
}
}
#endif
