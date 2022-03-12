#include <string>

#include "catch.hpp"
#include "cerealise/cerealise.hpp"
#include "cerealise/string.hpp"
#include "utils.hpp"

TEST_CASE("string") { check_parse_unparse(std::string{"OHAI"}, 5); }

TEST_CASE("long string") {
  std::string longstr(255, 'a');
  check_parse_unparse(longstr, longstr.size() + 2);
}
