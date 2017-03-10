#ifndef ALI_O2_ENITIES_MC_TRACK_H
#define ALI_O2_ENITIES_MC_TRACK_H
#include "Track.h"

namespace ecs {
template <typename... Components> class McTrack : public Track<Components...> {
public:
  static const char *Id() { return "McTrack"; }
  using ecs::Track<Components...>::Track;
};
}

#endif
