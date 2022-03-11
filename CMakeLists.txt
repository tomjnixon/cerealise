project(cerealise)
cmake_minimum_required(VERSION 3.22)

add_library(cerealise INTERFACE)
target_include_directories(cerealise INTERFACE include)
target_compile_features(cerealise INTERFACE cxx_std_20)

enable_testing()
add_subdirectory(test)
