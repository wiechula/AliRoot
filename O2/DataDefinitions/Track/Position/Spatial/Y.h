#ifndef TF_DD_TRACK_POSITION_SPATIAL_Y_H
#define TF_DD_TRACK_POSITION_SPATIAL_Y_H

#include "../Spatial.h"
namespace track {
struct Y : ISingle, Parent<ISpatial> {
  using flat_type = Y;
  float y;
  operator float() { return y; }
  void set(float y) { this->y = y; }
  const static std::string Name() { return ParentName() + "/Y"; }
};
}

#endif
