#ifndef TF_DD_TRACK_KINEMATICS_TEMPORAL_DEF_H
#define TF_DD_TRACK_KINEMATICS_TEMPORAL_DEF_H

#include "../Kinematics.h"
namespace track {
struct Pt : ISingle, Parent<IKinematics> {
  using flat_type = Pt;
  float mPt;
  operator float() { return mPt; }
  void set(float pt) { mPt = pt; }
  const static std::string Name() { return ParentName() + "/Pt"; }
};
}
#endif
