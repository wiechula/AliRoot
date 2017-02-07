#ifndef TF_DD_TRACK_POSITION_SPATIAL_Z_H
#define TF_DD_TRACK_POSITION_SPATIAL_Z_H

#include "../Spatial.h"
namespace track {
struct Z : ISingle, Parent<ISpatial> {
  using flat_type = Z;
  float z;
  operator float() { return z; }
  void set(float z) { this->z = z; }
  const static std::string Name() { return ParentName() + "/Z"; }
};
}

#endif
