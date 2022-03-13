add_executable(
  tests
  main.cpp
  array.cpp
  builtins.cpp
  custom.cpp
  string.cpp
  optional.cpp
  variant.cpp
  vector.cpp)
target_link_libraries(tests PRIVATE cerealise)
add_test(NAME tests COMMAND tests)

add_executable( example example.cpp)
target_link_libraries(example PRIVATE cerealise)
add_test(NAME example COMMAND example)
