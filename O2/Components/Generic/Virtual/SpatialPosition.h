#ifndef ALI_O2_COMPONENTS_GENERIC_SPATIAL_POSITION_H
#define ALI_O2_COMPONENTS_GENERIC_SPATIAL_POSITION_H
#include "../../../ecs/Dependency.h"
#include "../X.h"
#include "../Y.h"
#include "../Z.h"

namespace ecs {
namespace generic {
class SpatialPosition : public Dependency<X, Y, Z> {};
}
}
#endif
