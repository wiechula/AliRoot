#ifndef ALI_O2_ANALYSIS_TASK_H
#define ALI_O2_ANALYSIS_TASK_H

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
  // this task can be used as-is, in which case no events are generated for the
  // user and the user can directly interface with the handler instead.
  virtual size_t numberOfEvents() { return 1; };
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

#endif
