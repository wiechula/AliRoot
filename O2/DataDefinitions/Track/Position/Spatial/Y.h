#ifndef TF_DD_TRACK_TRACK_POSITION_SPATIAL_Y_DEF_H
#define TF_DD_TRACK_TRACK_POSITION_SPATIAL_Y_DEF_H

#include "../../../base.h"

DEFINE_SINGLE(Y, ISpatial) {
  float mY;
  operator float() { return mY; }
  void set(float Y) { mY = Y; }
  SINGLE_BODY(Y, ISpatial);
};

#endif
