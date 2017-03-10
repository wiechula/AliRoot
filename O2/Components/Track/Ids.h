#ifndef ALI_O2_COMPONENTS_TRACK_IDS_H
#define ALI_O2_COMPONENTS_TRACK_IDS_H
#include <cstdint>
namespace ecs {
namespace track {
namespace ids {
// NOTE: ONLY EVER APPEND TO THIS LIST! do not: prepends, shuffle or delete
// entries!
enum Components : uint16_t {
  X,
  Y,
  Z,
  T,
  Pt,
  SinAlpha,
  TanLambda,
  ClusterMapping,
  ESDEventIndex,
  Covariance,
  ChargeSign,
  Mass,
  MonteCarloIndex,
  PdgCode
};
}
}
}

#endif
