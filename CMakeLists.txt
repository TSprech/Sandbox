cmake_minimum_required(VERSION 3.22)
project(Sandbox)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

enable_testing()

find_program(GCOV_PATH gcov)
if(NOT GCOV_PATH)
    message(FATAL_ERROR "Code coverage analysis requires gcov!")
endif()

add_executable(SandboxTest test/SimpleTest.cpp test/FakeLibraryTest.cpp main.cpp)

set(BUILD_GMOCK OFF)
add_subdirectory(test/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_link_libraries(SandboxTest gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(SandboxTest
                     PROPERTIES
                     DISCOVERY_TIMEOUT  # How long to wait (in seconds) before crashing
                     240)

#if(CMAKE_CXX_COMPILER_ID MATCHES GNU AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
target_link_options(SandboxTest PUBLIC --coverage -fprofile-arcs -ftest-coverage)
target_compile_options(SandboxTest PUBLIC --coverage -fprofile-arcs -ftest-coverage)
#endif()