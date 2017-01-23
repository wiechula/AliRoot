#ifndef TF_DD_TRACKS_KINEMATICS_TEMPORAL_DEF_H
#define TF_DD_TRACKS_KINEMATICS_TEMPORAL_DEF_H

#include "../../base.h"
struct Pt : ISingle, ITrack::IKinematics {
  float mPt;
  operator float() { return mPt; }
  static const char *Name() { return "Pt"; }
};
#endif
