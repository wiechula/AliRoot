#ifndef ECS_ENTITY_H
#define ECS_ENTITY_H
#include "Dependency.h"
#include "EntityCollection.h"
#include "Slice.h"
#include "VariableComponent.h"
#include "extended_type_traits.h"
#include <tuple>

namespace ecs {

/// An Entity is in essence a reference to an EntityCollection and an id.
/// Entities can bee created cheaply from EntityCollections and behave just as a
/// normal struct used to behave.
template <typename... Components> class Entity {
  // First we define some public meta-template features.
public:
  using Self = Entity<Components...>;
  // defines a single tuple containing all the components that were given to
  // Enity, unrolling any dependencies.
  // can contain duplicates if duplicates are given.
  using ComponentsTuple = decltype(std::tuple_cat(
      std::declval<typename GetDependencyTuple<Components>::type>()...));
  constexpr static size_t Size = std::tuple_size<ComponentsTuple>::value;

  // IndexOf returns the index of a component in the component list
  template <typename G, typename std::enable_if<tuple_contains<
                            G, ComponentsTuple>::value>::type * = nullptr>
  constexpr static int IndexOf() {
    return tuple_index_of<G, ComponentsTuple>::value;
  }

  // define type<N> as the type of the N'th component
  template <unsigned N,
            typename std::enable_if<N<Size>::type * = nullptr> struct TypeOf {
    using type = typename std::tuple_element<N, ComponentsTuple>::type;
  };

  // Check if this entity contains all off the components <head, Rest...>
  template <typename Head, typename... Rest,
            typename std::enable_if<0 != sizeof...(Rest)>::type * = nullptr>
  constexpr static bool Contains() {
    return tuple_contains<Head, ComponentsTuple>::value && Contains<Rest...>();
  }
  // Specialization to stop recursion
  template <typename Head> constexpr static bool Contains() {
    return tuple_contains<Head, ComponentsTuple>::value;
  }

  // Checks if none of the types in <Head, Rest...> are included.
  template <typename Head, typename... Rest,
            typename std::enable_if<0 != sizeof...(Rest)>::type * = nullptr>
  constexpr static bool DoesntContain() {
    return !tuple_contains<Head, ComponentsTuple>::value &&
           DoesntContain<Rest...>();
  }
  // Specialization to stop recursion
  template <typename Head> constexpr static bool DoesntContain() {
    return !tuple_contains<Head, ComponentsTuple>::value;
  }

private:
  /// index into the component arrays of the entityCollection for this specific
  /// entity.
  size_t mId;
  /// A reference to the the entitycollection that stores all the component
  /// arrays.
  const EntityCollectionInner<Entity> *mCollection;

public:
  /// construct from a parent collection and id.
  /// TODO: remove this and make only entityCollection produce entities?
  Entity<Components...>(const EntityCollectionInner<Entity> *collection,
                        size_t id)
      : mId(id), mCollection(collection) {}

  /// Get function to get one of the components/sub-entities from the
  /// collection.
  template <typename G /*The type we requests*/,
            typename std::enable_if<
                !std::is_base_of<IVariableComponent, G>::value &&
                tuple_contains<G, ComponentsTuple>::value>::type * = nullptr>
  constexpr G get() const {
    return mCollection->template get<G>()[mId];
  }

  /// Specialization for the case where teh requested component has an N->M
  /// mapping. Returns a slice (sized array) to the M mapped values.
  template <typename G /*The type we requests*/,
            typename std::enable_if<
                std::is_base_of<IVariableComponent, G>::value &&
                tuple_contains<G, ComponentsTuple>::value>::type * = nullptr>
  Slice<G> get() const {
    std::pair<uint64_t *, G *> data = mCollection->template get<G>();
    uint64_t index = data.first[2 * mId + 0];
    uint64_t size = data.first[2 * mId + 1];
    return Slice<G>((G *)(data.second + index), size);
  }
};
}
#endif
