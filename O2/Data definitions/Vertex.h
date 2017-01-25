#ifndef TF_DD_VERTEX_H
#define TF_DD_VERTEX_H
#include "base.h"

#include "Generic/Position.h"
#include "Vertex/Covariance.h"

template <typename... T> struct Vertex : IMulti<void>, IVertex {
  using IThis = IVertex;
  static_assert(all_true<std::is_base_of<IThis, T>::value...>::value,
                "Vertex instanced with non-IVertex type in parameter pack");
  static const char *Name() { return "Vertex"; }
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

typedef Vertex<PositionAll, Covariance> VertexAll;
#endif
