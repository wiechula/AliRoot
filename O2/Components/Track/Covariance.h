#ifndef ALI_O2_COMPONENTS_TRACK_COVARIANCE_H
#define ALI_O2_COMPONENTS_TRACK_COVARIANCE_H
#include "Ids.h"
namespace ecs {
namespace track {
class Covariance {
  double mCovariance[5 + 4 + 3 + 2 + 1];

public:
  Covariance(double Covariance[15]) {
    for (int i = 0; i < 15; i++) {
      mCovariance[i] = Covariance[i];
    }
  }
  Covariance() {
    for (int i = 0; i < 15; i++) {
      mCovariance[i] = 0;
    }
  }
  static const char *Id() { return "Covariance"; }
};
}
}
#endif
