#ifndef ALI_O2_COMPONENTS_TRACK_CHARGE_SIGN_H
#define ALI_O2_COMPONENTS_TRACK_CHARGE_SIGN_H
#include "Ids.h"
namespace ecs {
namespace track {
class ChargeSign {
  bool mChargeSign;

public:
  ChargeSign(bool ChargeSign) : mChargeSign(ChargeSign) {}
  ChargeSign() : mChargeSign(0) {}
  operator bool() const { return mChargeSign; }
  bool operator=(bool other) {
    set(other);
    return other;
  }
  void set(bool ChargeSign) { mChargeSign = ChargeSign; }
  static const char *Id() { return "ChargeSign"; }
};
}
}
#endif
