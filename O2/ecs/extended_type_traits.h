#ifndef ECS_EXTENDED_TYPE_TRAITS_H
#define ECS_EXTENDED_TYPE_TRAITS_H
#include <tuple>
#include <type_traits>

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

// method to find the index of the first true in a parametec pack.
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

// method to check if and are true in a parameter pack
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
