#include <vector>

#include "catch.hpp"
#include "cerealise/cerealise.hpp"
#include "cerealise/vector.hpp"
#include "utils.hpp"

TEST_CASE("vector") { check_parse_unparse(std::vector<uint32_t>{1, 2, 3}, 13); }
