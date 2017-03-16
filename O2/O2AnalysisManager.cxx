#include "O2AnalysisManager.h"

void O2AnalysisManager::startAnalysis() {
#pragma omp
  {
    // This is where we create our histograms, seems to occasionaly crash if
    // done in parallel...
    // #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < mTasks.size(); i++) {
      auto &task = mTasks[i];
      task->UserInit();
    }
    // TODO: We can start running in parallel over files once a merging scheme
    // is defined. When we do this, the tasks need to become copies per file
    // too.
    // #pragma omp for schedule(dynamic) collapse(2)
    for (auto &handler : mHandlers) {
#pragma omp parallel for schedule(dynamic)
      for (int i = 0; i < mTasks.size(); i++) {
        auto &task = mTasks[i];
        task->setHandler(handler);
        task->init();
        for (size_t event = 0; event < task->numberOfEvents(); event++) {
          task->execute(event);
        }
      }
    }
    // Here we tend to write output, which can also crash root when done in
    // parallel...
    // #pragma omp for schedule(dynamic)
    for (int i = 0; i < mTasks.size(); i++) {
      auto &task = mTasks[i];
      task->finish();
    }
  }
}
