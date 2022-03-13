#include <optional>

#include "catch.hpp"
#include "cerealise/cerealise.hpp"
#include "cerealise/optional.hpp"
#include "utils.hpp"

TEST_CASE("optional") {
  check_parse_unparse(std::optional<uint32_t>{}, 1);
  check_parse_unparse(std::optional<uint32_t>{3}, 5);
}
