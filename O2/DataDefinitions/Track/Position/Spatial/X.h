#ifndef TF_DD_TRACK_TRACK_POSITION_SPATIAL_X_DEF_H
#define TF_DD_TRACK_TRACK_POSITION_SPATIAL_X_DEF_H

#include "../../../base.h"

DEFINE_SINGLE(X, ISpatial) {
  float mX;
  operator float() { return mX; }
  void set(float X) { mX = X; }
  SINGLE_BODY(X, ISpatial);
};

#endif
