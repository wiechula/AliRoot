#include "../../DataDefinitions/Track.h"
#include "../../DataDefinitions/Vertex.h"
#include <iostream>

using namespace track;
int main(int argc, char **argv) {
  using myTrack =
      Track<Kinematics<Pt>, Position<Temporal, SpatialAll>, MuonScore>;
  std::cout << "Pt for tracks is located at: " << track::Pt::Name()
            << std::endl;
  std::cout << "TrackAll is located at: " << TrackAll::Name() << std::endl;
  std::cout << "myTrack is located at: " << myTrack::Name() << std::endl;

  std::cout << "Track position are located at: " << IPosition::Name()
            << std::endl;
  std::cout << "Vertex position are located at: " << vertex::IPosition::Name()
            << std::endl;
  return 0;
  // return 0.0f;
  // return kin.Get<Pt>();
}
