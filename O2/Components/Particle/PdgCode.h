#ifndef ALI_O2_COMPONENENTS_PARTICLE_PDG_CODE_H
#define ALI_O2_COMPONENENTS_PARTICLE_PDG_CODE_H
#include <cstdint>
namespace ecs {
namespace particle {
class PdgCode {
  int32_t mCode;

public:
  PdgCode(int32_t code) : mCode(code) {}
  PdgCode() : mCode(0) {}
  operator int32_t() const { return mCode; }
  int32_t operator=(int32_t other) {
    set(other);
    return other;
  }
  void set(int32_t code) { mCode = code; }
  static const char *Id() { return "PdgCode"; }
};
}
}
#endif
