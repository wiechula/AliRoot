#ifndef TF_DD_TRACK_MUONSCORE_H
#define TF_DD_TRACK_MUONSCORE_H

#include "../base.h"

DEFINE_SINGLE(MuonScore, ITrack) {
  unsigned mScore;
  operator unsigned() { return mScore; }
  void set(unsigned score) { mScore = score; }
  SINGLE_BODY(MuonScore, ITrack);
};

#endif
