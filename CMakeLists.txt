project(userial)
cmake_minimum_required(VERSION 3.22)

add_library(userial INTERFACE)
target_include_directories(userial INTERFACE include)
target_compile_features(userial INTERFACE cxx_std_20)

enable_testing()
add_subdirectory(test)
