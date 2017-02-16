#ifndef TF_DD_TRACK_KINEMATICS_ORIENTATION_DEF_H
#define TF_DD_TRACK_KINEMATICS_ORIENTATION_DEF_H

#include "../../base.h"

DEFINE_SINGLE(Orientation, IKinematics) {
  float mSine;
  float mDip;
  SINGLE_BODY(Orientation, IKinematics);
};

#endif
