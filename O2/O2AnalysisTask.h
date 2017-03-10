#ifndef ALI_O2_ANALYSIS_TASK_H
#define ALI_O2_ANALYSIS_TASK_H
#include "Entities/McTrack.h"
#include "Entities/Track.h"
#include "Entities/Vertex.h"
#include "ecs/EntityCollection.h"
#include "ecs/Handler.h"

class O2AnalysisTask {
  const ecs::Handler *mHandler = nullptr;
  size_t mCurrentEvent = 0;

protected:
  size_t getCurrentEvent() const { return mCurrentEvent; }
  const ecs::Handler *getHandler() const { return mHandler; }
  // We need to create an event using the specification provided by the task.
  // This is the method that generates one, by default using the old esd index.
  // virtual std::unique_ptr<void> CreateEvent(size_t index) = 0;
  void setCurrentEvent(size_t event) { mCurrentEvent = event; }

public:
  // O2AnalysisTask(const O2AnalysisTask &) = delete;
  // O2AnalysisTask(){};
  virtual ~O2AnalysisTask() {}
  virtual size_t numberOfEvents() = 0;
  void setHandler(const ecs::Handler &handler) { mHandler = &handler; }
  virtual void init() {}
  virtual void UserInit() {}
  void execute(size_t event) {
    setCurrentEvent(event);
    UserExec();
  }
  virtual void UserExec() = 0;
  virtual void finish() {}
};

template <typename Vertex, typename Track, typename McTrack = ecs::McTrack<>>
class O2ESDAnalysisTask : public O2AnalysisTask {
  class Event {
    unsigned mId;
    const ecs::EntityCollection<Vertex> mVertices;
    const ecs::EntityCollection<Track> mTracks;
    const ecs::EntityCollection<McTrack> mMcTracks;

  public:
    Event(const ecs::EntityCollection<Vertex> vertices,
          const ecs::EntityCollection<Track> tracks,
          const ecs::EntityCollection<McTrack> mcTracks)
        : mVertices(vertices), mTracks(tracks), mMcTracks(mcTracks) {}
    const Vertex &getVertex() const { return mVertices[0]; }
    const ecs::EntityCollection<Track> &getTracks() const { return mTracks; }
    size_t getNumberOfTracks() const { return mTracks.size(); }
    const ecs::EntityCollection<Track> &getTracks() { return mTracks; }
    const ecs::EntityCollection<McTrack> &getMcTracks() { return mMcTracks; }
    const Track getTrack(size_t id) const { return mTracks[id]; }
    const McTrack getMcTrack(size_t label) const { return mMcTracks[label]; }
  };
  std::vector<Event> mEvents;

protected:
  virtual size_t numberOfEvents() { return mEvents.size(); }
  virtual void init() {
    std::cout << "init ESD called \n";
    ecs::EntityCollection<ecs::Vertex<ecs::vertex::ESDEventMapping>>
        verticesMapping(*getHandler());
    for (int i = 0; i < verticesMapping.size(); i++) {
      auto map =
          verticesMapping[i].template get<ecs::vertex::ESDEventMapping>();
      mEvents.push_back(
          Event(ecs::EntityCollection<Vertex>(*getHandler(), i, 1),
                ecs::EntityCollection<Track>(*getHandler(), map.trackStart(),
                                             map.trackCount()),
                ecs::EntityCollection<McTrack>(
                    *getHandler(), map.mcTrackStart(), map.mcTrackCount())));
    }
    std::cout << "created " << mEvents.size() << " Events\n";
  }
  Event getEvent() const { return mEvents[getCurrentEvent()]; }
};

#endif
