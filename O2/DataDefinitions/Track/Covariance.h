#ifndef TF_DD_TRACK_COVARIANCE_H
#define TF_DD_TRACK_COVARIANCE_H

#include "../Track.h"
namespace track {
struct Covariance : ISingle, Parent<ITrack> {
  using flat_type = Covariance;
  float cov[15];
  const static std::string Name() { return ParentName() + "/Covariance"; }
};
}

#endif
