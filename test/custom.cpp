#include <compare>

#include "catch.hpp"
#include "cerealise/cerealise.hpp"

struct Test1 {
  uint8_t x;
  uint32_t y;

  auto operator<=>(const Test1 &) const = default;
};

namespace cerealise {
template <> struct Adapter<Test1> {
  template <typename T, typename F> static bool adapt(T &v, F &f) {
    return f(v.x) && f(v.y);
  }
};
} // namespace cerealise

TEST_CASE("custom class external") {
  Test1 v1{1, 999};

  uint8_t buf[100];
  size_t len;
  REQUIRE(cerealise::unparse(v1, buf, 100, len));
  REQUIRE(len == 5);

  Test1 v2;
  REQUIRE(cerealise::parse(v2, buf, len));

  REQUIRE(v1 == v2);
}

struct Test2 {
  uint8_t x;
  uint32_t y;

  auto operator<=>(const Test2 &) const = default;

  template <typename T, typename F> static bool cerealise(T &v, F &f) {
    return f(v.x) && f(v.y);
  }
};

TEST_CASE("custom class internal") {
  Test2 v1{1, 999};

  uint8_t buf[100];
  size_t len;
  REQUIRE(cerealise::unparse(v1, buf, 100, len));
  REQUIRE(len == 5);

  Test2 v2;
  REQUIRE(cerealise::parse(v2, buf, len));

  REQUIRE(v1 == v2);
}
