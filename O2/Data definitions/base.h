#ifndef TF_BASE_H
#define TF_BASE_H
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

// Method to get the element of a tuple by type.
namespace detail {

template <class T, std::size_t N, class... Args>
struct get_number_of_element_from_tuple_by_type_impl {
  static constexpr auto value = N;
};

template <class T, std::size_t N, class... Args>
struct get_number_of_element_from_tuple_by_type_impl<T, N, T, Args...> {
  static constexpr auto value = N;
};

template <class T, std::size_t N, class U, class... Args>
struct get_number_of_element_from_tuple_by_type_impl<T, N, U, Args...> {
  static constexpr auto value =
      get_number_of_element_from_tuple_by_type_impl<T, N + 1, Args...>::value;
};

} // namespace detail

template <class T, class... Args>
T get_element_by_type(const std::tuple<Args...> &t) {
  return std::get<detail::get_number_of_element_from_tuple_by_type_impl<
      T, 0, Args...>::value>(t);
}

// Template to check if a type is one of the members of a parameter pack
template <typename...> struct is_one_of {
  static constexpr bool value = false;
};

template <typename F, typename S, typename... T> struct is_one_of<F, S, T...> {
  static constexpr bool value =
      std::is_same<F, S>::value || is_one_of<F, T...>::value;
};

// Template to check if condition holds true for all members of a parameter
// pack.
template <bool... b> struct BoolArray {};
template <bool... b>
using all_true = std::is_same<BoolArray<b...>, BoolArray<(b, true)...>>;

// TODO: see if it is possible to refactor in such a way that we can get the
// parent from any type as well as the children.

// Empty base classes to use as tags for the template type safety.
// When (If...) the C++ commision standardizes concepts this should be replaced
// with
// concepts.
class IData {};
class ISingle : IData {};
class IMulti : IData {};
// TODO: implement this
template <typename T> class IOptional : IData {};

// Predefine the dependency graph.
// Include any Multi type here.
struct IGeneric {
  struct IPosition {};
};
struct ITrack {
  struct IKinematics {};
};
struct IVertex {};

namespace internal {
// ISingle (depthwise end-point)
template <typename T, size_t I = 0>
typename std::enable_if<std::is_base_of<ISingle, T>::value, void>::type
walkSubstructur(std::string &parent, std::vector<std::string> &contents) {
  contents.push_back(parent + T::Name());
}

// IMulti tuple get out-of-bounds specialization (widthwise end-point)
template <typename T, size_t I = 0>
typename std::enable_if<std::is_base_of<IMulti, T>::value &&
                            (I == std::tuple_size<typename T::Data_t>()),
                        void>::type
walkSubstructur(std::string &parent, std::vector<std::string> &contents) {}

// IMulti handler.
template <typename T, size_t I = 0>
typename std::enable_if<std::is_base_of<IMulti, T>::value &&
                            (I < std::tuple_size<typename T::Data_t>()),
                        void>::type
walkSubstructur(std::string &parent, std::vector<std::string> &contents) {

  // descend into the child, adding, adding our name to the path
  walkSubstructur<typename std::tuple_element<I, typename T::Data_t>::type>(
      (parent + T::Name()) + "/");
  // then move on to the next element of the tuple.
  walkSubstructur<T, I + 1>(parent);
}
}

// TODO: this isn't really a vector, but a tree, but implementing that now would
// be premature optimization.
template <typename T>
std::vector<std::string> getDataMembers(std::string base = "") {
  std::vector<std::string> vec;
  internal::walkSubstructur<T>(base, vec);
  return vec;
}

// Use as get<TrackAll> or get<Track<Kinematics, Position<Temporal>>,
// Vertex<PositionAll> for example.
// Should return a multicontainer then.
//
// In order to fetch e.g. a track use
// container.get<Track<Position<Temporal>>>(i)
// which will return a type of Data_t.

#endif
