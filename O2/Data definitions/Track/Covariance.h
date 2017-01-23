#ifndef TF_DD_TRACKS_COVARIANCE_H
#define TF_DD_TRACKS_COVARIANCE_H

#include "../base.h"
struct Covariance : ISingle, ITrack {
  float cov[15];
  static const char *Name() { return "Covariance"; }
};

#endif
