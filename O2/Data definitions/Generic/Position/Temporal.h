#ifndef TF_DD_GENERIC_POSITION_TEMPORAL_H
#define TF_DD_GENERIC_POSITION_TEMPORAL_H
#include "../../base.h"
struct Temporal : ISingle, IGeneric::IPosition {
  float mTimestamp;
  operator float() { return mTimestamp; }
  static const char *Name() { return "Temporal"; }
};
#endif
