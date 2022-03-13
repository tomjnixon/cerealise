#include <compare>

#include "catch.hpp"
#include "cerealise/cerealise.hpp"
#include "utils.hpp"

struct Test1 {
  uint8_t x;
  uint32_t y;

  auto operator<=>(const Test1 &) const = default;
};

template <> struct cerealise::Adapter<Test1> {
  template <typename T, typename F> static bool adapt(T &v, F &f) {
    return f(v.x) && f(v.y);
  }
};

TEST_CASE("custom class external") { check_parse_unparse<Test1>({1, 999}, 5); }

struct Test2 {
  uint8_t x;
  uint32_t y;

  auto operator<=>(const Test2 &) const = default;

  template <typename T, typename F> static bool cerealise(T &v, F &f) {
    return f(v.x) && f(v.y);
  }
};

TEST_CASE("custom class internal") { check_parse_unparse<Test2>({1, 999}, 5); }
