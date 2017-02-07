#ifndef TF_DD_VERTEX_POSITION_H
#define TF_DD_VERTEX_POSITION_H

#include "../Vertex.h"
namespace vertex {
IMPLEMENT_MULTIPLE(Position, IVertex);
}

#include "Position/Spatial.h"
#include "Position/Temporal.h"
namespace vertex {
using PositionAll = Position<Temporal, Spatial>;
}
#endif
