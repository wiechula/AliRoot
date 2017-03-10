#ifndef ALI_O2_COMPONENTS_VERTEX_SIGNAL_TO_NOISE_Y_H
#define ALI_O2_COMPONENTS_VERTEX_SIGNAL_TO_NOISE_Y_H
#include "Ids.h"
namespace ecs {
namespace vertex {
class SignalToNoiseY {
  float mSignalToNoiseY;

public:
  SignalToNoiseY(float SignalToNoiseY) : mSignalToNoiseY(SignalToNoiseY) {}
  SignalToNoiseY() : mSignalToNoiseY(0) {}
  operator float() const { return mSignalToNoiseY; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float SignalToNoiseY) { mSignalToNoiseY = SignalToNoiseY; }
  static const char *Id() { return "SignalToNoiseY"; }
};
}
}
#endif
