#ifndef ALI_O2_COMPONENTS_TRACK_AMBIGOUS_CLUSTES_FLAG_H
#define ALI_O2_COMPONENTS_TRACK_AMBIGOUS_CLUSTES_FLAG_H

namespace ecs {
namespace track {
namespace mc {
class AmbiguousClustersFlag {
  bool mAmbiguousClustersFlag;

public:
  AmbiguousClustersFlag(bool AmbiguousClustersFlag)
      : mAmbiguousClustersFlag(AmbiguousClustersFlag) {}
  AmbiguousClustersFlag() : mAmbiguousClustersFlag(0) {}
  operator bool() const { return mAmbiguousClustersFlag; }
  bool operator=(bool other) {
    set(other);
    return other;
  }
  void set(bool AmbiguousClustersFlag) {
    mAmbiguousClustersFlag = AmbiguousClustersFlag;
  }
  static const char *Id() { return "AmbiguousClustersFlag"; }
};
}
}
}
#endif
