#pragma once
#include "cerealise.hpp"
#include <string>

namespace cerealise {

template <> struct Adapter<std::string> {
  template <typename TT, typename F> static bool adapt(TT &v, F &f) {
    size_t size = v.size();
    if (!f.varint(size))
      return false;

    if constexpr (F::parsing)
      v.resize(size);

    for (size_t i = 0; i < size; i++)
      if (!f(v[i]))
        return false;

    return true;
  }
};

} // namespace cerealise
