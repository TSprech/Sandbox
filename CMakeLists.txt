cmake_minimum_required(VERSION 3.22)
project(Sandbox)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_executable(SandboxTest test/SimpleTest.cpp main.cpp)

add_subdirectory(test/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_link_libraries(SandboxTest gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(SandboxTest
                     PROPERTIES
                     DISCOVERY_TIMEOUT  # How long to wait (in seconds) before crashing
                     240)