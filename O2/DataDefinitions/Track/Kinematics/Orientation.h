#ifndef TF_DD_TRACK_KINEMATICS_ORIENTATION_DEF_H
#define TF_DD_TRACK_KINEMATICS_ORIENTATION_DEF_H

#include "../Kinematics.h"
namespace track {
struct Orientation : ISingle, Parent<IKinematics> {
  using flat_type = Orientation;
  float mSine;
  float mDip;
  const static std::string Name() { return ParentName() + "/Orientation"; }
};
}
#endif
