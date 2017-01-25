#ifndef TF_DD_GENERIC_POSITION_DEF_H
#define TF_DD_GENERIC_POSITION_DEF_H
#include "../base.h"
#include "Position/Spatial.h"
#include "Position/Temporal.h"

template <typename... T>
struct Position : IMulti<ITrack>, IMulti<IVertex>, IGeneric::IPosition {
  using IThis = IGeneric::IPosition;
  static_assert(all_true<std::is_base_of<IData<IThis>, T>::value...>::value,
                "Position instanced with non-IPosition type in parameter pack");
  static const char *Name() { return "Position"; }
  std::tuple<typename std::conditional<
      std::is_base_of<ISingle<IThis>, T>::value, T *, T>::type...>
      mTuple;
  template <typename G, typename std::enable_if<is_one_of<G, T...>::value>::type
                            * = nullptr,
            unsigned index = first_true<is_one_of<G, T>::value...>::value,
            typename basetype =
                typename std::tuple_element<index, std::tuple<T...>>::type>
  constexpr typename std::conditional<
      std::is_base_of<ISingle<IThis>, basetype>::value, basetype *,
      basetype>::type
  Get() const {
    return std::get<index>(mTuple);
  }
  template <typename G,
            typename std::enable_if<any_true<
                (std::is_base_of<G, T>::value &&
                 !std::is_same<G, T>::value)...>::value>::type * = nullptr,
            unsigned index = first_true<std::is_base_of<G, T>::value...>::value>
  constexpr typename std::tuple_element<index, std::tuple<T...>>::type
  Get() const {
    return std::get<index>(mTuple);
  }
};

typedef Position<Temporal, Spatial> PositionAll;
#endif
