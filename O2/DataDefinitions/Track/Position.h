#ifndef TF_DD_TRACK_POSITION_H
#define TF_DD_TRACK_POSITION_H

#include "Position/Spatial.h"
#include "Position/Temporal.h"

DEFINE_MULTI(Position, ITrack) { MULTI_BODY(Position); };
using PositionAll = Position<Temporal, SpatialAll>;
#endif
