#pragma once
#include "cerealise/cerealise.hpp"
#include <optional>
#include <vector>

template <typename T>
void check_parse_unparse(T v1, std::optional<size_t> expected_len = {}) {
  std::vector<uint8_t> buf(expected_len.value_or(100));

  size_t len;
  REQUIRE(cerealise::unparse(v1, buf.data(), buf.size(), len));
  if (expected_len)
    REQUIRE(len == *expected_len);

  T v2;
  size_t bytes_read;
  REQUIRE(cerealise::parse(v2, buf.data(), len, bytes_read));
  REQUIRE(bytes_read == len);

  REQUIRE(v1 == v2);
}
