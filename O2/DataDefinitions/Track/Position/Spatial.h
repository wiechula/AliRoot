#ifndef TF_DD_TRACK_POSITION_SPATIAL_H
#define TF_DD_TRACK_POSITION_SPATIAL_H

#include "../Position.h"
namespace track {
IMPLEMENT_MULTIPLE(Spatial, IPosition);
}

#include "Spatial/X.h"
#include "Spatial/Y.h"
#include "Spatial/Z.h"
namespace track {
using SpatialAll = Spatial<X, Y, Z>;
}
#endif
