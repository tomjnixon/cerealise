#pragma once
#include "cerealise.hpp"
#include <optional>

namespace cerealise {

template <typename T> struct Adapter<std::optional<T>> {
  template <typename TT, typename F> static bool adapt(TT &v, F &f) {
    if constexpr (F::parsing) {
      bool has_value;
      if (!f.boolean(has_value))
        return false;

      if (has_value)
        v.emplace();
      else
        v.reset();
    } else {
      if (!f.boolean(v.has_value()))
        return false;
    }

    if (v.has_value())
      if (!f(v.value()))
        return false;

    return true;
  }
};

} // namespace cerealise
