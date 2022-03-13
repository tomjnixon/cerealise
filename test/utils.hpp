#pragma once
#include "cerealise/cerealise.hpp"
#include <optional>
#include <vector>

template <typename T>
void check_parse_unparse(T v1, std::optional<size_t> expected_len = {}) {
  size_t len = cerealise::measure(v1);
  REQUIRE(len > 0);

  if (expected_len)
    REQUIRE(len == *expected_len);

  std::vector<uint8_t> buf(len);

  size_t real_len;
  REQUIRE(cerealise::unparse(v1, buf.data(), buf.size(), real_len));
  REQUIRE(real_len == len);

  T v2;
  size_t bytes_read;
  REQUIRE(cerealise::parse(v2, buf.data(), len, bytes_read));
  REQUIRE(bytes_read == len);

  REQUIRE(v1 == v2);
}
