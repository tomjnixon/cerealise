#!/bin/bash

find test/ include/ -name '*.cpp' -or -name '*.hpp' -and \! -name catch.hpp | xargs clang-format -i

find -name CMakeLists.txt | xargs cmake-format -i
