#include <compare>
#include <variant>

#include "catch.hpp"
#include "cerealise/cerealise.hpp"
#include "cerealise/variant.hpp"
#include "utils.hpp"

using T = std::variant<uint8_t, uint32_t>;

TEST_CASE("variant") {
  check_parse_unparse<T>((uint8_t)5, 5);
  check_parse_unparse<T>((uint32_t)5, 8);
}
