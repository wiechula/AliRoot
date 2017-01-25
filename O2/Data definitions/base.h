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
// with
// concepts.
// template = parent
template <typename T = void> class IData {};
template <typename T = void> class ISingle : IData<T> {};
template <typename T = void> class IMulti : IData<T> {};
// TODO: implement this
// template <typename T> class IOptional : IData {};

// Predefine the dependency graph.
// Include any Multi type here.
struct IGeneric {
  struct IPosition {};
};
struct ITrack {
  struct IKinematics {};
};
struct IVertex {};
//
// namespace internal {
// // ISingle (depthwise end-point)
// template <typename T, size_t I = 0, typename G>
// typename std::enable_if<std::is_base_of<ISingle<G>, T>::value, void>::type
// walkSubstructur(std::string &parent, std::vector<std::string> &contents) {
//   contents.push_back(parent + T::Name());
// }
//
// // IMulti tuple get out-of-bounds specialization (widthwise end-point)
// template <typename T, size_t I = 0, typename G>
// typename std::enable_if<std::is_base_of<IMulti<G>, T>::value &&
//                             (I == std::tuple_size<typename T::Data_t>()),
//                         void>::type
// walkSubstructur(std::string &parent, std::vector<std::string> &contents) {}
//
// // IMulti handler.
// template <typename T, size_t I = 0, typename G>
// typename std::enable_if<std::is_base_of<IMulti<G>, T>::value &&
//                             (I < std::tuple_size<typename T::Data_t>()),
//                         void>::type
// walkSubstructur(std::string &parent, std::vector<std::string> &contents) {
//
//   // descend into the child, adding, adding our name to the path
//   walkSubstructur<typename std::tuple_element<I, typename T::Data_t>::type>(
//       (parent + T::Name()) + "/");
//   // then move on to the next element of the tuple.
//   walkSubstructur<T, I + 1>(parent);
// }
// }

// TODO: this isn't really a vector, but a tree, but implementing that now would
// be premature optimization.
// template <typename T>
// std::vector<std::string> getDataMembers(std::string base = "") {
//   std::vector<std::string> vec;
//   internal::walkSubstructur<T>(base, vec);
//   return vec;
// }

// Use as get<TrackAll> or get<Track<Kinematics, Position<Temporal>>,
// Vertex<PositionAll> for example.
// Should return a multicontainer then.
//
// In order to fetch e.g. a track use
// container.get<Track<Position<Temporal>>>(i)
// which will return a type of Data_t.

#endif
