#ifndef TF_DD_TRACK_POSITION_SPATIAL_X_H
#define TF_DD_TRACK_POSITION_SPATIAL_X_H

#include "../Spatial.h"
namespace track {
struct X : ISingle, Parent<ISpatial> {
  using flat_type = X;
  float x;
  operator float() { return x; }
  void set(float x) { this->x = x; }
  const static std::string Name() { return ParentName() + "/X"; }
};
}

#endif
