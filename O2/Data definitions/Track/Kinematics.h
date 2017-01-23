#ifndef TF_DD_TRACKS_KINEMATICS_H
#define TF_DD_TRACKS_KINEMATICS_H

#include "../base.h"
#include "Kinematics/Orientation.h"
#include "Kinematics/Pt.h"

template <typename... T> struct Kinematics : IMulti, ITrack {
  static_assert(
      all_true<std::is_base_of<ITrack::IKinematics, T>::value...>::value,
      "Kinematics instanced with non-IKinematics type in parameter pack");
  static const char *Name() { return "Kinematics"; }
  std::tuple<T...> mTuple;
  template <typename G>
  typename std::enable_if<is_one_of<G, T...>::value, G>::type Get() {
    return get_element_by_type<G>(mTuple);
  }
};

typedef Kinematics<Pt, Orientation> KinematicsAll;
#endif
