#ifndef ALI_O2_COMPONENTS_TRACK_TAN_LAMBDA_H
#define ALI_O2_COMPONENTS_TRACK_TAN_LAMBDA_H
#include "Ids.h"
namespace ecs {
namespace track {
class TanLambda {
  float mTanLambda;

public:
  TanLambda(float TanLambda) : mTanLambda(TanLambda) {}
  TanLambda() : mTanLambda(0) {}
  operator float() const { return mTanLambda; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float TanLambda) { mTanLambda = TanLambda; }
  static uint16_t Id() { return ids::Components::TanLambda; }
};
}
}
#endif
