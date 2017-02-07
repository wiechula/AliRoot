#ifndef TF_DD_TRACK_POSITION_TEMPORAL_H
#define TF_DD_TRACK_POSITION_TEMPORAL_H

#include "../Position.h"
namespace track {
struct Temporal : ISingle, Parent<IPosition> {
  using flat_type = Temporal;
  float t;
  const static std::string Name() { return ParentName() + "/Temporal"; }
};
}

#endif
