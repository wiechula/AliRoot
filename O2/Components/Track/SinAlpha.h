#ifndef ALI_O2_COMPONENTS_TRACK_SIN_ALPHA_H
#define ALI_O2_COMPONENTS_TRACK_SIN_ALPHA_H
#include "Ids.h"
namespace ecs {
namespace track {
class SinAlpha {
  float mSinAlpha;

public:
  SinAlpha(float SinAlpha) : mSinAlpha(SinAlpha) {}
  SinAlpha() : mSinAlpha(0) {}
  operator float() const { return mSinAlpha; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float SinAlpha) { mSinAlpha = SinAlpha; }
  static uint16_t Id() { return ids::Components::SinAlpha; }
};
}
}
#endif
