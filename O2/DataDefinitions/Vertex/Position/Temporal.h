#ifndef TF_DD_VERTEX_POSITION_TEMPORAL_H
#define TF_DD_VERTEX_POSITION_TEMPORAL_H

#include "../Position.h"
namespace vertex {
struct Temporal : ISingle, Parent<IPosition> {
  float t;
  using flat_type = Temporal;
  const static std::string Name() { return ParentName() + "/Temporal"; }
};
}

#endif
