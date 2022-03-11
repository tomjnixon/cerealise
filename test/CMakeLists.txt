add_executable(tests main.cpp custom.cpp variant.cpp)
target_link_libraries(tests PRIVATE cerealise)
add_test(NAME tests COMMAND tests)
