#ifndef TF_DD_TRACK_TRACK_POSITION_SPATIAL_Z_DEF_H
#define TF_DD_TRACK_TRACK_POSITION_SPATIAL_Z_DEF_H

#include "../../../base.h"

DEFINE_SINGLE(Z, ISpatial) {
  float mZ;
  operator float() { return mZ; }
  void set(float Z) { mZ = Z; }
  SINGLE_BODY(Z, ISpatial);
};

#endif
