cmake_minimum_required(VERSION 3.20)
project(test_fluentqt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FluentQt REQUIRED)

add_executable(test_fluentqt test_fluentqt.cpp)
target_link_libraries(test_fluentqt FluentQt::FluentQt)
