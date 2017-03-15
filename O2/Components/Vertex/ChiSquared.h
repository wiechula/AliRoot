#ifndef ALI_O2_COMPONENTS_VERTEX_CHI_SQUARED_H
#define ALI_O2_COMPONENTS_VERTEX_CHI_SQUARED_H

namespace ecs {
namespace vertex {
class ChiSquared {
  float mChiSquared;

public:
  ChiSquared(float ChiSquared) : mChiSquared(ChiSquared) {}
  ChiSquared() : mChiSquared(0) {}
  operator float() const { return mChiSquared; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float ChiSquared) { mChiSquared = ChiSquared; }
  static const char *Id() { return "ChiSquared"; }
};
}
}
#endif
