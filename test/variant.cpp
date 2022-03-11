#include <compare>
#include <variant>

#include "catch.hpp"
#include "cerealise/cerealise.hpp"
#include "cerealise/variant.hpp"

template <typename T> void check(T v1, size_t expected_len) {
  uint8_t buf[100];
  size_t len;
  REQUIRE(cerealise::unparse(v1, buf, 100, len));
  REQUIRE(len == expected_len);

  T v2;
  REQUIRE(cerealise::parse(v2, buf, len));

  REQUIRE(v1 == v2);
}

using T = std::variant<uint8_t, uint32_t>;

TEST_CASE("variant") {
  check<T>((uint8_t)5, 5);
  check<T>((uint32_t)5, 8);
}
