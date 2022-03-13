#pragma once
#include "cerealise.hpp"
#include <vector>

namespace cerealise {

template <typename T> struct Adapter<std::vector<T>> {
  template <typename TT, typename F> static bool adapt(TT &v, F &f) {
    size_t size;
    if constexpr (!F::parsing)
      size = v.size();

    if (!f.varint(size))
      return false;

    if constexpr (F::parsing)
      v.resize(size);

    for (auto &element : v)
      if (!f(element))
        return false;

    return true;
  }
};

} // namespace cerealise
