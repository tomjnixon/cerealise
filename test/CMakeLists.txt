add_executable(
  tests
  main.cpp
  array.cpp
  builtins.cpp
  custom.cpp
  string.cpp
  optional.cpp
  variant.cpp)
target_link_libraries(tests PRIVATE cerealise)
add_test(NAME tests COMMAND tests)
