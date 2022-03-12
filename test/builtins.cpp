#include <compare>
#include <limits>

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

template <typename T> struct VarIntTest {
  T x;

  template <typename TT, typename F> static bool cerealise(TT &v, F &f) {
    return f.varint(v.x);
  }

  auto operator<=>(const VarIntTest<T> &) const = default;
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

TEST_CASE("bools") {
  check_parse_unparse<bool>(true, 1);
  check_parse_unparse<bool>(false, 1);
}

TEST_CASE("floats") {
  check_parse_unparse<float>(123.0f, 4);
  check_parse_unparse<float>(-123.0f, 4);
  check_parse_unparse<double>(123.0, 8);
  check_parse_unparse<double>(-123.0, 8);
}

TEST_CASE("varint") {
  for (uint32_t i = 0; i < 0x10000; i += 32)
    check_parse_unparse<VarIntTest<uint16_t>>({(uint16_t)i});

  using limits = std::numeric_limits<int16_t>;
  for (int32_t i = limits::min(); i <= limits::max(); i += 32)
    check_parse_unparse<VarIntTest<int16_t>>({(int16_t)i});
}

TEST_CASE("zigzag") {
  using namespace cerealise::detail;

  using T = int32_t;
  using limits = std::numeric_limits<T>;
  for (T x : {limits::min(), -1, 0, 1, limits::max()}) {
    REQUIRE(decode_zigzag(encode_zigzag(x)) == x);
  }
}
