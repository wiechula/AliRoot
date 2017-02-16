#ifndef TF_DD_TRACK_KINEMATICS_PT_DEF_H
#define TF_DD_TRACK_KINEMATICS_PT_DEF_H

#include "../../base.h"

DEFINE_SINGLE(Pt, IKinematics) {
  float mPt;
  operator float() { return mPt; }
  void set(float pt) { mPt = pt; }
  SINGLE_BODY(Pt, IKinematics);
};

#endif
