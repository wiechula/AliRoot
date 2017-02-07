#ifndef TF_BASE_H
#define TF_BASE_H
#include "../extended_type_traits.h"
#include <iostream>
#include <string>
#include <vector>

// TODO: see if it is possible to refactor in such a way that we can get the
// parent from any type as well as the children.

// Empty base classes to use as tags for the template type safety.
// When (If...) the C++ commision standardizes concepts this should be replaced
// with concepts.
class IData {};           // Any part of the 'data tree'
class ISingle : IData {}; // An endpoint, i.e. not containing any other data
class IMulti : IData {};  // A node of the tree, containing several endpoints

// Used for generating identifiers and checking valid placement.
// An IData should derive from this to specify where it can be attached.
template <typename T = void> class Parent {
protected:
  // Returns the name of the parent. This should be called and prepended from
  // T::Name() too.
  const static std::string ParentName() { return T::Name(); }
};
// void means it has no parents, in that case the ParentName is empty.
template <> class Parent<void> {
protected:
  const static std::string ParentName() { return ""; }
};

// Implements the body for a IMulti struct
#define MULTI_BODY(IThis)                                                                                  \
  /*First we check that all template arguments (T...) have this type as one of                             \
   * their possible parents */                                                                             \
  static_assert(all_true<std::is_base_of<Parent<IThis>, T>::value...>::value,                              \
                #IThis " instanced with non Parent<" #IThis                                                \
                       "> deriving type in parameter pack");                                               \
  /*Create a tuple called mTuple which stores pointers to any type in T which                              \
   * is an ISingle and and normal types for any IMulti (which in turn contain                              \
   * the same type of tuple) */                                                                            \
  std::tuple<typename std::conditional<std::is_base_of<ISingle, T>::value,                                 \
                                       T *, T>::type...>                                                   \
      mTuple;                                                                                              \
  /*Used for testing SoA vs AoS, is the same as the outer type only its tuple                              \
   * is a struct containing all endpoints*/                                                                \
  struct flat_type {                                                                                       \
    std::tuple<typename T::flat_type...> mTuple;                                                           \
    /*See outer definition*/                                                                               \
    template <typename G, typename std::enable_if<                                                         \
                              is_one_of<G, T...>::value>::type * = nullptr,                                \
              unsigned index = first_true<is_one_of<G, T>::value...>::value,                               \
              typename basetype = typename G::flat_type>                                                   \
    basetype &GetRef() {                                                                                   \
      return std::get<index>(mTuple);                                                                      \
    }                                                                                                      \
    template <                                                                                             \
        typename G,                                                                                        \
        typename std::enable_if<any_true<                                                                  \
            (std::is_base_of<G, T>::value &&                                                               \
             !std::is_same<G, T>::value)...>::value>::type * = nullptr,                                    \
        unsigned index = first_true<std::is_base_of<G, T>::value...>::value>                               \
    typename std::tuple_element<index,                                                                     \
                                std::tuple<typename T::flat_type...>>::type &                              \
    GetRef() {                                                                                             \
      return std::get<index>(mTuple);                                                                      \
    }                                                                                                      \
  };                                                                                                       \
  /*returns a const copy of a child of this struct                                                         \
   * type*/                                                                                                \
  template <typename G /*The type we requests*/,                                                           \
            typename std::enable_if<is_one_of<G, T...>::value>::type * =                                   \
                nullptr /*G has to be one of the types in T*/,                                             \
            unsigned index = first_true<is_one_of<G, T>::value...>::                                       \
                value /*Cache the tuple index of G*/>                                                      \
  constexpr typename std::conditional<std::is_base_of<ISingle, G>::value, G *,                             \
                                      G>::type /*the return type depends on                                \
                                                  where G is an ISingle                                    \
                                                  (pointer) or IMulti(value)*/                             \
      Get() const {                                                                                        \
    return std::get<index>(mTuple);                                                                        \
  }                                                                                                        \
  /*Same Get function but now for IMulti interface types (e.g. IKinematics)*/ template <                   \
      typename G /*Type we request*/,                                                                      \
      typename std::enable_if<                                                                             \
          any_true<(std::is_base_of<G, T>::value &&                                                        \
                    !std::is_same<G, T>::value)...>::value>::type * =                                      \
          nullptr /*Enabled iff one of the values of T derives from it,
           but is not same type (is_base_of<G,G> == true)
           to prevent conflicts with the above template*/,                                   \
      unsigned index = first_true<std::is_base_of<G, T>::value...>::value>                                 \
  constexpr typename std::tuple_element<index, std::tuple<T...>>::                                         \
      type /*Fetch the return type from the tuple because it can not be infered from the interface*/ Get() \
          const {                                                                                          \
    return std::get<index>(mTuple);                                                                        \
  }                                                                                                        \
  /*same as above only return by reference*/ template <                                                    \
      typename G,                                                                                          \
      typename std::enable_if<is_one_of<G, T...>::value>::type * = nullptr,                                \
      unsigned index = first_true<is_one_of<G, T>::value...>::value>                                       \
  typename std::conditional<std::is_base_of<ISingle, G>::value, G *, G>::type                              \
      &                                                                                                    \
      GetRef() {                                                                                           \
    return std::get<index>(mTuple);                                                                        \
  }                                                                                                        \
  template <                                                                                               \
      typename G,                                                                                          \
      typename std::enable_if<                                                                             \
          any_true<(std::is_base_of<G, T>::value &&                                                        \
                    !std::is_same<G, T>::value)...>::value>::type * = nullptr,                             \
      unsigned index = first_true<std::is_base_of<G, T>::value...>::value>                                 \
  typename std::tuple_element<index, std::tuple<T...>>::type &GetRef() {                                   \
    return std::get<index>(mTuple);                                                                        \
  }

// Creates an interface type Ix for any IMulti type so that they can be easilly
// accesed (e.g. IKinematics to access Kinematics<Pt, Orientation>)
#define IMPLEMENT_ITYPE(x, PARENT)                                             \
  struct I##x : Parent<PARENT> {                                               \
    const static std::string Name() {                                          \
      return Parent<PARENT>::ParentName() + "/" + #x;                          \
    }                                                                          \
  };

// Unused
#define IMPLEMENT_GENERIC_ITYPE(x, P)                                          \
  template <typename P> struct IT##x : Parent<P> {                             \
    const static std::string Name() {                                          \
      return Parent<P>::ParentName() + "/" + #x;                               \
    }                                                                          \
  };                                                                           \
  struct I##x {};

// Implements an IMulti type 'BASE' with the given parent
#define IMPLEMENT_MULTIPLE(BASE, PARENT)                                       \
  IMPLEMENT_ITYPE(BASE, PARENT);                                               \
  template <typename... T> struct BASE : IMulti, I##BASE {                     \
    MULTI_BODY(I##BASE);                                                       \
  };

/// Currently unused
#define IMPLEMENT_GENERIC_MULTIPLE(BASE)                                       \
  IMPLEMENT_ITYPE(BASE, P);                                                    \
  template <typename P, typename... T>                                         \
  struct BASE : IMulti, IT##BASE<P>, I##BASE {                                 \
    MULTI_BODY(I##BASE<P>);                                                    \
  };

#endif
