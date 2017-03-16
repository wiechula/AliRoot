#ifndef ALI_O2_ESD_ANALYSIS_TASK_H
#define ALI_O2_ESD_ANALYSIS_TASK_H

#include "Entities/Particle.h"
#include "Entities/Track.h"
#include "Entities/Vertex.h"
#include "O2AnalysisTask.h"
#include "ecs/EntityCollection.h"

template <typename Vertex, typename Track, typename Particle = ecs::Particle<>>
class O2ESDAnalysisTask : public O2AnalysisTask {
  class Event {
    unsigned mId;
    const ecs::EntityCollection<Vertex> mVertices;
    const ecs::EntityCollection<Track> mTracks;
    const ecs::EntityCollection<Particle> &mParticles;

  public:
    Event(const ecs::EntityCollection<Vertex> vertices,
          const ecs::EntityCollection<Track> tracks,
          const ecs::EntityCollection<Particle> &particles)
        : mVertices(vertices), mTracks(tracks), mParticles(particles) {}
    const Vertex &getVertex() const { return mVertices[0]; }
    const ecs::EntityCollection<Track> &getTracks() const { return mTracks; }
    size_t getNumberOfTracks() const { return mTracks.size(); }
    const ecs::EntityCollection<Track> &getTracks() { return mTracks; }
    const ecs::EntityCollection<Particle> &getParticles() { return mParticles; }
    const Track getTrack(size_t id) const { return mTracks[id]; }
    const Particle getParticle(size_t label) const { return mParticles[label]; }
  };
  ecs::EntityCollection<Particle> mParticles;
  std::vector<Event> mEvents;

public:
  virtual ~O2ESDAnalysisTask(){};
  virtual size_t numberOfEvents() { return mEvents.size(); }
  virtual void init() {
    ecs::EntityCollection<ecs::Vertex<ecs::vertex::ESDEventMapping>>
        verticesMapping(*getHandler());
    mParticles.fill(*getHandler());
    for (int i = 0; i < verticesMapping.size(); i++) {
      auto map =
          verticesMapping[i].template get<ecs::vertex::ESDEventMapping>();
      mEvents.push_back(
          Event(ecs::EntityCollection<Vertex>(*getHandler(), i, 1),
                ecs::EntityCollection<Track>(*getHandler(), map.trackStart(),
                                             map.trackCount()),
                mParticles));
    }
  }
  Event getEvent() const { return mEvents[getCurrentEvent()]; }
};

#endif
