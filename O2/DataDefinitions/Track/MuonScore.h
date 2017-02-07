#ifndef TF_DD_TRACK_MUONSCORE_H
#define TF_DD_TRACK_MUONSCORE_H

#include "../Track.h"
namespace track {
struct MuonScore : ISingle, Parent<ITrack> {
  using flat_type = MuonScore;
  unsigned mScore;
  operator unsigned() { return mScore; }
  void set(unsigned score) { mScore = score; }
  const static std::string Name() { return ParentName() + "/MuonScore"; }
};
}

#endif
