#ifndef TF_DD_TRACK_COVARIANCE_H
#define TF_DD_TRACK_COVARIANCE_H

#include "../base.h"

DEFINE_SINGLE(Covariance, ITrack) {
  float cov[15];
  SINGLE_BODY(Covariance, ITrack);
};

#endif
