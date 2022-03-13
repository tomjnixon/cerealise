#pragma once
#include "cerealise.hpp"
#include <array>

namespace cerealise {

template <typename T, size_t N> struct Adapter<std::array<T, N>> {
  template <typename TT, typename F> static bool adapt(TT &v, F &f) {
    for (auto &element : v)
      if (!f(element))
        return false;

    return true;
  }
};

} // namespace cerealise
