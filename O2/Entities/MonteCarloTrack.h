#ifndef ALI_O2_ENITIES_MONTE_CARLO_TRACK_H
#define ALI_O2_ENITIES_MONTE_CARLO_TRACK_H

#include "Track.h"
namespace ecs {
template <typename... Components>
class MonteCarloTrack : public Track<Components...> {
public:
  static uint16_t Id() { return ecs::ids::Entities::MonteCarloTrack; }
  using ecs::Entity<Components...>::Entity;
};
}
#endif
