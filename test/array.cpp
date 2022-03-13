#include <array>

#include "catch.hpp"
#include "cerealise/array.hpp"
#include "cerealise/cerealise.hpp"
#include "utils.hpp"

TEST_CASE("array") {
  check_parse_unparse(std::array<uint32_t, 3>{1, 2, 3}, 12);
}
