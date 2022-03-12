#pragma once
#include "cerealise/cerealise.hpp"
#include <optional>

template <typename T>
void check_parse_unparse(T v1, std::optional<size_t> expected_len = {}) {
  uint8_t buf[100];
  size_t len;
  REQUIRE(cerealise::unparse(v1, buf, 100, len));
  if (expected_len)
    REQUIRE(len == *expected_len);

  T v2;
  REQUIRE(cerealise::parse(v2, buf, len));

  REQUIRE(v1 == v2);
}
