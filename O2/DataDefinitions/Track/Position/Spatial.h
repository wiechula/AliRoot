#ifndef TF_DD_TRACK_POSITION_SPATIAL_H
#define TF_DD_TRACK_POSITION_SPATIAL_H

#include "Spatial/X.h"
#include "Spatial/Y.h"
#include "Spatial/Z.h"
#include <cmath>

DEFINE_MULTI(Spatial, IPosition) {
  MULTI_BODY(Spatial);

public:
  template <typename U = Spatial<T...>,
            typename std::enable_if<contains<U, X>() && contains<U, Y>() &&
                                    contains<U, Z>()>::type * = nullptr>
  float length_3d(int index) const {
    return sqrtf(*Get<X>() * *Get<X>() * +*Get<Y>() * *Get<Y>() +
                 *Get<Z>() * *Get<Z>());
    ;
  }
  // Proof-of-concept, get the 3d distance between two ISpatial types which all
  // contain X,Y,Z:
  template <typename Other, typename U = Spatial<T...>,
            typename std::enable_if<contains<U, X>() && contains<U, Y>() &&
                                    contains<U, Z>() && contains<Other, X>() &&
                                    contains<Other, Y>() &&
                                    contains<Other, Z>()>::type * = nullptr>
  float distance_from(const Other &other) const {
    float dx = *Get<X>() - *(other.template Get<X>());
    float dy = *Get<Y>() - *(other.template Get<Y>());
    float dz = *Get<Z>() - *(other.template Get<Z>());
    return sqrtf(dx * dx + dy * dy + dz * dz);
  }
};
using SpatialAll = Spatial<X, Y, Z>;

#endif
