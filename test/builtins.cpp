#include <compare>

#include "catch.hpp"
#include "cerealise/cerealise.hpp"
#include "utils.hpp"

template <typename T, size_t size = sizeof(T)> struct FixedIntTest {
  T x;

  template <typename TT, typename F> static bool cerealise(TT &v, F &f) {
    return f.template fixedint<size>(v.x);
  }

  auto operator<=>(const FixedIntTest<T, size> &) const = default;
};

TEST_CASE("unsigned integers") {
  check_parse_unparse<FixedIntTest<uint8_t>>({0x12}, 1);
  check_parse_unparse<FixedIntTest<uint16_t>>({0x1234}, 2);
  check_parse_unparse<FixedIntTest<uint32_t, 3>>({0x123456}, 3);
  check_parse_unparse<FixedIntTest<uint32_t>>({0x12345678}, 4);
  check_parse_unparse<FixedIntTest<uint64_t, 5>>({0x1234567690}, 5);
  check_parse_unparse<FixedIntTest<uint64_t>>({0x1234567890abcdef}, 8);
}

TEST_CASE("signed integers") {
  check_parse_unparse<FixedIntTest<int8_t>>({0x12}, 1);
  check_parse_unparse<FixedIntTest<int8_t>>({-0x12}, 1);
  check_parse_unparse<FixedIntTest<int16_t>>({0x1234}, 2);
  check_parse_unparse<FixedIntTest<int16_t>>({-0x1234}, 2);
  check_parse_unparse<FixedIntTest<int32_t, 3>>({0x123456}, 3);
  check_parse_unparse<FixedIntTest<int32_t, 3>>({-0x123456}, 3);
  check_parse_unparse<FixedIntTest<int32_t>>({0x12345678}, 4);
  check_parse_unparse<FixedIntTest<int32_t>>({-0x12345678}, 4);
  check_parse_unparse<FixedIntTest<int64_t, 5>>({0x1234567690}, 5);
  check_parse_unparse<FixedIntTest<int64_t, 5>>({-0x1234567690}, 5);
  check_parse_unparse<FixedIntTest<int64_t>>({0x1234567890abcdef}, 8);
  check_parse_unparse<FixedIntTest<int64_t>>({-0x1234567890abcdef}, 8);
}
