#ifndef ALI_O2_COMPONENTS_GENERIC_PT_H
#define ALI_O2_COMPONENTS_GENERIC_PT_H
#include "../../../ecs/Dependency.h"
#include "../Px.h"
#include "../Py.h"

namespace ecs {
namespace generic {
class Pt : Dependency<Px, Py> {};
}
}
#endif
