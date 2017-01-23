#ifndef TF_DD_TRACKS_H
#define TF_DD_TRACKS_H
#include "base.h"

#include "Generic/Position.h"
#include "Track/Covariance.h"
#include "Track/Kinematics.h"

template <typename... T> struct Track : IMulti, ITrack {
  static_assert(all_true<std::is_base_of<ITrack, T>::value...>::value,
                "Track instanced with non-ITrack type in parameter pack");
  static const char *Name() { return "Track"; }
  std::tuple<T...> mTuple;
  template <typename G>
  typename std::enable_if<is_one_of<G, T...>::value, G>::type Get() {
    return get_element_by_type<G>(mTuple);
  }
  template <template <typename...> class G, typename... Pack>
  typename std::enable_if<is_one_of<G<Pack...>, T...>::value, G<Pack...>>::type
  Get() {
    return get_element_by_type<G<Pack...>>(mTuple);
  }
};

typedef Track<PositionAll, KinematicsAll, Covariance> TrackAll;
#endif
