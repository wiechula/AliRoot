#ifndef TF_DD_TRACKS_KINEMATICS_ORIENTATION_DEF_H
#define TF_DD_TRACKS_KINEMATICS_ORIENTATION_DEF_H

#include "../../base.h"
struct Orientation : ISingle<ITrack::IKinematics> {
  float mSine;
  float mDip;
  static const char *Name() { return "Orientation"; }
};
#endif
