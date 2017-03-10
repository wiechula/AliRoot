#ifndef ALI_O2_COMPONENTS_VERTEX_IDS_H
#define ALI_O2_COMPONENTS_VERTEX_IDS_H
#include <cstdint>
namespace ecs {
namespace vertex {
namespace ids {
// NOTE: ONLY EVER APPEND TO THIS LIST! do not: prepends, shuffle or delete
// entries!
enum Components : uint16_t {
  X,
  Y,
  Z,
  T,
  Covariance,
  ESDEventIndex,
  SignalToNoiseX,
  SignalToNoiseY,
  SignalToNoiseZ,
  ChiSquared,
  Sigma,
  UsedTracksIndices,
  ESDEventMapping
};
}
}
}

#endif
