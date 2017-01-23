#ifndef TF_DD_GENERIC_POSITION_DEF_H
#define TF_DD_GENERIC_POSITION_DEF_H
#include "../base.h"
#include "Position/Spatial.h"
#include "Position/Temporal.h"

template <typename... T> struct Position : IMulti, ITrack, IVertex {
  static_assert(
      all_true<std::is_base_of<IGeneric::IPosition, T>::value...>::value,
      "Position instanced with non-IPosition type in parameter pack");
  static const char *Name() { return "Position"; }
  typedef struct {
    std::tuple<T...> mTuple;
    template <typename G>
    std::enable_if<is_one_of<G, T...>::value, typename G::Data_t> Get() {
      return get_element_by_type<G>(mTuple);
    }
  } Data_t;
};

typedef Position<Temporal, Spatial> PositionAll;
#endif
