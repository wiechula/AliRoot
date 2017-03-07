#ifndef ALI_O2_COMPONENTS_VERTEX_COVARIANCE_H
#define ALI_O2_COMPONENTS_VERTEX_COVARIANCE_H
#include "Ids.h"
namespace ecs {
namespace vertex {
class Covariance {
  double mCovariance[3 + 2 + 1]; // TODO: check size

public:
  Covariance(double Covariance[6]) {
    for (int i = 0; i < 6; i++) {
      mCovariance[i] = Covariance[i];
    }
  }
  Covariance() {
    for (int i = 0; i < 6; i++) {
      mCovariance[i] = 0;
    }
  }
  static uint16_t Id() { return ids::Components::Covariance; }
};
}
}
#endif
