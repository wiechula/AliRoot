#ifndef TF_DD_VERTEX_COVARIANCE_H
#define TF_DD_VERTEX_COVARIANCE_H

#include "../Vertex.h"
namespace vertex {
struct Covariance : ISingle, Parent<IVertex> {
  float cov[15];
  using flat_type = Covariance;
  const static std::string Name() { return ParentName() + "/Covariance"; }
};
}

#endif
