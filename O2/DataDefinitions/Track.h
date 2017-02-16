#ifndef TF_DD_TRACKS_H
#define TF_DD_TRACKS_H
#include "base.h"

#include "Track/Covariance.h"
#include "Track/Kinematics.h"
#include "Track/MuonScore.h"
#include "Track/Position.h"

DEFINE_MULTI(Track, None) {
  MULTI_BODY(Track);
  // TODO: macrofy
  template <typename U = Track<T...>,
            typename std::enable_if<contains<U, IKinematics, Pt>()>::type * =
                nullptr>
  Pt pt(int index) const {
    return Get<IKinematics>().pt(index);
  }
};
//
// #include "Generic/Position.h"

using TrackAll = Track<PositionAll, KinematicsAll, Covariance, MuonScore>;

#endif
