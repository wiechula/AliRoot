#ifndef TF_DD_VERTEX_POSITION_SPATIAL_H
#define TF_DD_VERTEX_POSITION_SPATIAL_H

#include "../Position.h"
namespace vertex {
struct Spatial : ISingle, Parent<IPosition> {
  float x, y, z;
  const static std::string Name() { return ParentName() + "/Spatial"; }
};
}
#endif
