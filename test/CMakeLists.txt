
add_executable(tests main.cpp custom.cpp)
target_link_libraries(tests PRIVATE userial)
add_test(NAME tests COMMAND tests)
