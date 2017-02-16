#include "../../DataDefinitions/Track.h"
#include "../../DataDefinitions/Vertex.h"
#include <iostream>

template <typename... T> class test : public T... { int x; };

using namespace track;
int main(int argc, char **argv) {

  using myTrack =
      Track<Kinematics<Pt>, Position<Temporal, SpatialAll>, MuonScore>;
  test<myTrack> x;
  myTrack track;
  track.Get<IKinematics>();
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
