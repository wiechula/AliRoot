#include "Track.h"

std::vector<std::string> getTrackMembers() {
  return getDataMembers<TrackAll>();
}

float test() {
  TrackAll track;
  // TODO: Make a template specializtion which allows us to write
  // track.Get<Kinematics>() and autoresolve the type based on Track
  Kinematics<Pt, Orientation> kin = track.Get<Kinematics<Pt, Orientation>>();
  return kin.Get<Pt>();
}
