#include "cerealise/cerealise.hpp"
#include <cassert>

struct Test {
  uint8_t x;
  uint32_t y;

  template <typename T, typename F> static bool cerealise(T &value, F &f) {
    return f(value.x) && f(value.y);
  }
};

int main(int argc, char **argv) {
  // the value to serialise
  Test value{1, 999};

  // how many bytes do we need
  size_t len = cerealise::measure(value);
  assert(len);

  std::vector<uint8_t> buf(len);

  // unparse to buf
  size_t unparse_len;
  bool result = cerealise::unparse(value, buf.data(), len, unparse_len);
  assert(result);
  assert(unparse_len == len);

  // parse from buf
  Test parsed_value;
  size_t parse_len;
  result = cerealise::parse(parsed_value, buf.data(), len, parse_len);
  assert(result);
  assert(parse_len == len);

  // now, value == parsed_value
  assert(value.x == parsed_value.x);
  assert(value.y == parsed_value.y);
  return 0;
}
