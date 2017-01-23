#ifndef TF_DD_GENERIC_POSITION_SPATIAL_H
#define TF_DD_GENERIC_POSITION_SPATIAL_H
#include "../../base.h"

struct Spatial : ISingle, IGeneric::IPosition {
  float x, y, z;
  static const char *Name() { return "Spatial"; }
};
#endif
