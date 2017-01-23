#ifndef TF_DD_VERTEX_H
#define TF_DD_VERTEX_H
#include "base.h"

#include "Generic/Position.h"
#include "Vertex/Covariance.h"

template <typename... T> struct Vertex : IMulti, IVertex {
  static_assert(all_true<std::is_base_of<IVertex, T>::value...>::value,
                "Vertex instanced with non-IVertex type in parameter pack");
  static const char *Name() { return "Vertex"; }
  typedef struct {
    std::tuple<T...> mTuple;
    template <typename G>
    std::enable_if<is_one_of<G, T...>::value, typename G::Data_t> Get() {
      return get_element_by_type<G>(mTuple);
    }
  } Data_t;
};

typedef Vertex<PositionAll, Covariance> VertexAll;
#endif
