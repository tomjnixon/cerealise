#pragma once

#include "userial.hpp"
#include <variant>

namespace userial {
namespace detail {
template <typename TT, typename F, typename T> bool parse_variant(TT &v, F &f) {
  T v_value;
  if (f(v_value)) {
    v = v_value;
    return true;
  } else
    return false;
}

template <int t_idx, typename T, typename... Rest> struct VariantParseHelper {
  template <typename TT, typename F>
  static bool run(int this_idx, TT &v, F &f) {
    if (this_idx == t_idx)
      return parse_variant<TT, F, T>(v, f);
    else
      return VariantParseHelper<t_idx + 1, Rest...>::template run<TT, F>(
          this_idx, v, f);
  }
};

template <int t_idx, typename T> struct VariantParseHelper<t_idx, T> {
  template <typename TT, typename F>
  static bool run(int this_idx, TT &v, F &f) {
    if (this_idx != t_idx)
      return false;
    return parse_variant<TT, F, T>(v, f);
  }
};
} // namespace detail

template <typename... T> struct Adapter<std::variant<T...>> {
  template <typename TT, typename F> static bool adapt(TT &v, F &f) {
    if constexpr (F::parsing) {
      uint32_t idx;
      return f.u32(idx) &&
             detail::VariantParseHelper<0, T...>::template run<TT, F>(idx, v,
                                                                      f);
    } else
      return f.u32(v.index()) &&
             std::visit([&f](auto vv) -> bool { return f(vv); }, v);
  }
};
} // namespace userial
