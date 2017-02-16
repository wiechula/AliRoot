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
struct None {};
// void means it has no parents, in that case the ParentName is empty.
template <> class Parent<None> {
protected:
  const static std::string ParentName() { return ""; }
};

template <typename Base, typename Head, typename... Tail,
          typename std::enable_if<sizeof...(Tail) != 0>::type * = nullptr,
          typename std::enable_if<
              !Base::template directlyContains<Head>()>::type * = nullptr>
constexpr bool contains() {
  return false;
} /*no tail present*/
template <typename Base, typename Head> constexpr bool contains() {
  return Base::template directlyContains<Head>();
}
template <typename Base, typename Head, typename... Tail,
          typename std::enable_if<sizeof...(Tail) != 0>::type * = nullptr,
          typename std::enable_if<Base::template directlyContains<Head>()>::type
              * = nullptr>
constexpr bool contains() {
  return contains<
      typename std::result_of<decltype (&Base::template Get<Head>)(Base)>::type,
      Tail...>();
}

// Implements the body for a IMulti struct
#define MULTI_BODY(BASE)                                                                                   \
  using IThis = I##BASE; /*First we check that all template arguments (T...)                               \
                         * have this type as one of                                                        \
                         * their possible parents */                                                       \
  static_assert(all_true<std::is_base_of<Parent<IThis>, T>::value...>::value,                              \
                #BASE " instanced with non Parent<" #BASE                                                  \
                      "> deriving type in parameter pack");                                                \
  /*Create a tuple called mTuple which stores pointers to any type in T which                              \
   * is an ISingle and and normal types for any IMulti (which in turn contain                              \
   * the same type of tuple) */                                                                            \
  std::tuple<typename std::conditional<std::is_base_of<ISingle, T>::value,                                 \
                                       T *, T>::type...>                                                   \
      mTuple;                                                                                              \
  size_t mSize;                                                                                            \
  template <int N, typename std::enable_if<(N > 0)>::type * = nullptr,                                     \
            typename tupletype =                                                                           \
                typename std::tuple_element<N - 1, decltype(mTuple)>::type,                                \
            typename std::enable_if<std::is_pointer<tupletype>::value>::type                               \
                * = nullptr>                                                                               \
  void internal_allocate(size_t size) {                                                                    \
    std::get<N - 1>(mTuple) =                                                                              \
        (tupletype)malloc(sizeof(*std::get<N - 1>(mTuple)) * size);                                        \
    std::cout << "Allocated " << std::get<N - 1>(mTuple)->Name() << " : "                                  \
              << size << " * " << sizeof(*std::get<N - 1>(mTuple)) << "bytes"                              \
              << std::endl;                                                                                \
    internal_allocate<N - 1>(size);                                                                        \
  }                                                                                                        \
  template <int N, typename std::enable_if<(N > 0)>::type * = nullptr,                                     \
            typename tupletype =                                                                           \
                typename std::tuple_element<N - 1, decltype(mTuple)>::type,                                \
            typename std::enable_if<!std::is_pointer<tupletype>::value>::type                              \
                * = nullptr>                                                                               \
  void internal_allocate(size_t size) {                                                                    \
    internal_allocate<N - 1>(size);                                                                        \
  }                                                                                                        \
  template <int N, typename std::enable_if<N == 0>::type * = nullptr>                                      \
  void internal_allocate(size_t size) {}                                                                   \
  BASE(const size_t size = 1) : mSize(size) {                                                              \
    internal_allocate<sizeof...(T)>(size);                                                                 \
    std::cout << std::endl;                                                                                \
  }                                                                                                        \
  /*returns a const copy of a child of this struct                                                         \
     * type*/                                                                                              \
  template <typename G /*The type we requests*/,                                                           \
            typename std::enable_if<is_one_of<G, T...>::value>::type * =                                   \
                nullptr /*G has to be one of the types in T*/,                                             \
            unsigned index = first_true<is_one_of<G, T>::value...>::                                       \
                value /*Cache the tuple index of G*/>                                                      \
  constexpr const typename std::conditional<                                                               \
      std::is_base_of<ISingle, G>::value, G *,                                                             \
      G>::type /*the return type depends on                                                                \
                  where G is an ISingle                                                                    \
                  (pointer) or IMulti(value)*/                                                             \
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
  constexpr const typename std::tuple_element<index, std::tuple<T...>>::                                   \
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
  }                                                                                                        \
  /*no tail present*/                                                                                      \
  template <typename Head> constexpr static bool directlyContains() {                                      \
    return any_true<std::is_base_of<Head, T>::value...>::value;                                            \
  }

////allows us to write get<IPosition, ISpatial, X> instead of
/// Get<IPosition>().Get<ISpatial>().Get<X>()
//// requires c++14 or some trickery to get the return type I haven't figured
/// out yet.
// template <typename Current> decltype(auto) GetRef2() {                                                   \
  //   return GetRef<Current>();                                                                              \
  // }                                                                                                        \
  // template <typename Current, typename... Next,                                                            \
  //           typename std::enable_if<sizeof...(Next) != 0>::type * = nullptr>                               \
  // decltype(auto) GetRef2() {                                                                               \
  //   auto current = GetRef<Current>();                                                                      \
  //   return current.GetRef2<Next...>();                                                                     \
  // }

// Creates an interface type Ix for any IMulti type so that they can be easilly
// accesed (e.g. IKinematics to access Kinematics<Pt, Orientation>)
#define IMPLEMENT_ITYPE(x, PARENT)                                             \
  struct PARENT;                                                               \
  struct I##x : Parent<PARENT> {                                               \
    const static std::string Name() {                                          \
      return Parent<PARENT>::ParentName() + "/" + #x;                          \
    }                                                                          \
  };

#define DEFINE_SINGLE(BASE, PARENT)                                            \
  struct PARENT;                                                               \
  struct BASE : ISingle, Parent<PARENT>

#define SINGLE_BODY(BASE, PARENT)                                              \
  const static std::string Name() {                                            \
    return Parent<PARENT>::ParentName() + "/" + #BASE;                         \
  }

// Implements an IMulti type 'BASE' with the given parent
#define DEFINE_MULTI(BASE, PARENT)                                             \
  IMPLEMENT_ITYPE(BASE, PARENT);                                               \
  template <typename... T> struct BASE : IMulti, I##BASE

#endif
