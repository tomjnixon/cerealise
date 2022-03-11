#include <compare>
#include <cstdint>
#include <string>

#include "catch.hpp"
#include "userial/userial.hpp"

struct Test1 {
  uint8_t x;
  uint32_t y;

  auto operator<=>(const Test1&) const = default;
};

namespace userial {
template <> struct Adapter<Test1> {
  template <typename T, typename F> static bool adapt(T v, F f) {
    return f(v.x) && f(v.y);
  }
};
} // namespace userial

TEST_CASE("custom class external") {
  Test1 v1{1, 999};

  uint8_t buf[100];
  size_t len;
  REQUIRE(userial::unparse(v1, buf, 100, len));
  REQUIRE(len == 5);

  Test1 v2;
  REQUIRE(userial::parse(v2, buf, len));

  REQUIRE(v1 == v2);
}
