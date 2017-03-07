#ifndef ALI_O2_COMPONENTS_VERTEX_SIGNAL_TO_NOISE_X_H
#define ALI_O2_COMPONENTS_VERTEX_SIGNAL_TO_NOISE_X_H
#include "Ids.h"
namespace ecs {
namespace vertex {
class SignalToNoiseX {
  float mSignalToNoiseX;

public:
  SignalToNoiseX(float SignalToNoiseX) : mSignalToNoiseX(SignalToNoiseX) {}
  SignalToNoiseX() : mSignalToNoiseX(0) {}
  operator float() const { return mSignalToNoiseX; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float SignalToNoiseX) { mSignalToNoiseX = SignalToNoiseX; }
  static uint16_t Id() { return ids::Components::SignalToNoiseX; }
};
}
}
#endif
