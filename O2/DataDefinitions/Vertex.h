#ifndef TF_DD_VERTEX_H
#define TF_DD_VERTEX_H
#include "base.h"

IMPLEMENT_MULTIPLE(Vertex, void);

#include "Vertex/Covariance.h"
#include "Vertex/Position.h"
using VertexAll = Vertex<vertex::PositionAll, vertex::Covariance>;
#endif
