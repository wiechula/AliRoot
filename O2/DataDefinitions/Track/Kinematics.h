#ifndef TF_DD_TRACK_KINEMATICS_H
#define TF_DD_TRACK_KINEMATICS_H

#include "Kinematics/Orientation.h"
#include "Kinematics/Pt.h"

DEFINE_MULTI(Kinematics, ITrack) {
  MULTI_BODY(Kinematics);
  template <typename U = Kinematics<T...>,
            typename std::enable_if<contains<U, Pt>()>::type * = nullptr>
  Pt pt(int index) const {
    return Get<Pt>()[index];
  }
};
using KinematicsAll = Kinematics<Pt, Orientation>;

#endif
