#ifndef ECS_DEPENDENCY_H
#define ECS_DEPENDENCY_H
#include <tuple>
namespace ecs {
/// A flag for signaling that a component is a virtual package of components
class IDependency {};
template <typename... Dependencies> class Dependency : public IDependency {
public:
  using tuple_type = std::tuple<Dependencies...>;
};

template <typename Dependency>
typename Dependency::tuple_type f();

template <typename Dependency,
          typename std::enable_if<!std::is_base_of<
              IDependency, Dependency>::value>::type * = nullptr>
std::tuple<Dependency> f();

template <typename D> struct GetDependencyTuple {
  using type = decltype(f<D>());
};
}
#endif
