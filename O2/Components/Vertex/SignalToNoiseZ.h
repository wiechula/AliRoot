#ifndef ALI_O2_COMPONENTS_VERTEX_SIGNAL_TO_NOISE_Z_H
#define ALI_O2_COMPONENTS_VERTEX_SIGNAL_TO_NOISE_Z_H
#include "Ids.h"
namespace ecs {
namespace vertex {
class SignalToNoiseZ {
  float mSignalToNoiseZ;

public:
  SignalToNoiseZ(float SignalToNoiseZ) : mSignalToNoiseZ(SignalToNoiseZ) {}
  SignalToNoiseZ() : mSignalToNoiseZ(0) {}
  operator float() const { return mSignalToNoiseZ; }
  float operator=(float other) {
    set(other);
    return other;
  }
  void set(float SignalToNoiseZ) { mSignalToNoiseZ = SignalToNoiseZ; }
  static const char *Id() { return "SignalToNoiseZ"; }
};
}
}
#endif
