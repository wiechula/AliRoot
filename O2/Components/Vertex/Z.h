#ifndef ALI_O2_COMPONENTS_VERTEX_Z_H
#define ALI_O2_COMPONENTS_VERTEX_Z_H
#include "Ids.h"
namespace ecs {
namespace vertex {
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
  static uint16_t Id() { return ids::Components::Z; }
};
}
}
#endif
