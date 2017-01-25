#ifndef TF_DD_VERTEX_COVARIANCE_H
#define TF_DD_VERTEX_COVARIANCE_H

#include "../base.h"
struct Covariance : ISingle<IVertex> {
  float cov[10];
};

#endif
