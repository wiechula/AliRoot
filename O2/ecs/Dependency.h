#ifndef ECS_DEPENDENCY_H
#define ECS_DEPENDENCY_H
#include <tuple>
namespace ecs {
/// A flag for signaling that a component is a virtual package of components
class IDependency {};
template <typename... Dependencies>
class Dependency : public std::tuple<Dependencies...> {};
}

#endif
