#ifndef ALI_O2_COMPONENTS_GENERIC_PY_H
#define ALI_O2_COMPONENTS_GENERIC_PY_H

namespace ecs {
namespace generic {
class Py {
  float mPy;

public:
  Py(float Py) : mPy(Py) {}
  Py() : mPy(0) {}
  operator float() const { return mPy; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float Py) { mPy = Py; }
  static const char *Id() { return "Py"; }
};
}
}
#endif
