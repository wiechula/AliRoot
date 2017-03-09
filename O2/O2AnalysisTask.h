#ifndef ALI_O2_ANALYSIS_TASK_H
#define ALI_O2_ANALYSIS_TASK_H
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

template <typename Vertex, typename Track>
class O2ESDAnalysisTask : public O2AnalysisTask {
  class Event {
    const Vertex mVertex;
    const ecs::EntityCollection<Track> mTracks;

  public:
    Event(const Vertex vertex, const ecs::EntityCollection<Track> tracks)
        : mVertex(vertex), mTracks(tracks) {}
    const Vertex &getVertex() const { return mVertex; }
    const ecs::EntityCollection<Track> &getTracks() const { return mTracks; }
    size_t getNumberOfTracks() const { return mTracks.size(); }
    const Track getTrack(size_t id) const { return mTracks[id]; }
  };
  std::vector<Event> mEvents;

protected:
  virtual size_t numberOfEvents() {
    return getHandler()->template getCount<Vertex>();
  }
  virtual void init() {
    std::cout << "init ESD called \n";
    ecs::EntityCollection<Vertex> vertices(*getHandler());
    auto eventIndices =
        getHandler()
            ->template getTypedComponentData<ecs::Track<>,
                                             ecs::track::ESDEventIndex>();
    unsigned start = 0;
    unsigned value = eventIndices[0];
    unsigned u;
    for (u = 1; u < eventIndices.size(); u++) {
      if (eventIndices[u] != value) {
        mEvents.push_back(Event(
            vertices[mEvents.size()],
            ecs::EntityCollection<Track>(*getHandler(), start, u - start)));
        start = u;
        value = eventIndices[u];
      }
    }
    mEvents.push_back(
        Event(vertices[start],
              ecs::EntityCollection<Track>(*getHandler(), start, u - start)));
    std::cout << "created " << mEvents.size() << " Events\n";
  }
  Event getEvent() const { return mEvents[getCurrentEvent()]; }
};

#endif
