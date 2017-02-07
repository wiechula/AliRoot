#ifndef O2_EXTENDED_TYPE_TRAITS_H
#define O2_EXTENDED_TYPE_TRAITS_H
#include <tuple>
#include <type_traits>
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

template <bool... b> struct first_true {
  template <
      unsigned index = 0,
      typename std::enable_if<index<sizeof...(b)-1>::type * =
                                  nullptr> static constexpr unsigned check() {
    static_assert(index < sizeof...(b), "none are true");
    return std::get<index>(std::make_tuple(b...)) ? index : check<index + 1>();
  }
  template <unsigned index = 0,
            typename std::enable_if<index >= sizeof...(b)-1>::type * = nullptr>
  static constexpr unsigned check() {
    static_assert(index < sizeof...(b), "none are true");
    return std::get<index>(std::make_tuple(b...)) ? index : 0;
  }
  static constexpr unsigned value = first_true<b...>::check();
};

template <bool... b> struct any_true {
  template <unsigned index = 0,
            typename std::enable_if<index<sizeof...(b)-1>::type * =
                                        nullptr> static constexpr bool check() {
    return std::get<index>(std::make_tuple(b...)) ? true : check<index + 1>();
  }
  template <unsigned index = 0,
            typename std::enable_if<index == sizeof...(b)-1>::type * = nullptr>
  static constexpr bool check() {
    return std::get<index>(std::make_tuple(b...));
  }
  static constexpr bool value = any_true<b...>::check();
};

#endif
