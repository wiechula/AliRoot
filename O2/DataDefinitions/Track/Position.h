#ifndef TF_DD_TRACK_POSITION_H
#define TF_DD_TRACK_POSITION_H

#include "../Track.h"
namespace track {
IMPLEMENT_MULTIPLE(Position, ITrack);
}

#include "Position/Spatial.h"
#include "Position/Temporal.h"
namespace track {
using PositionAll = Position<Temporal, SpatialAll>;
}
#endif
