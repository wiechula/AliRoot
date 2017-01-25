#include "Track.h"
#include <iostream>

int main(int argc, char **argv) {
  TrackAll track;
  // TODO: Make a template specializtion which allows us to write
  // track.Get<Kinematics>() and autoresolve the type based on Track
  // std::cout << "position at " << track.Get<IGeneric::IPosition>.Get() <<
  // std::endl;
  // std::cout << "covariance at " << track.Get<Covariance>() << std::endl;
  float f = *track.Get<ITrack::IKinematics>().Get<Pt>();
  std::cout << " Pt is " << f << std::endl;
  return 0;
  // return 0.0f;
  // return kin.Get<Pt>();
}
