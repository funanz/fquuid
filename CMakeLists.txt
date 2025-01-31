# Copyright 2024 granz.fisherman@gmail.com
# https://opensource.org/license/mit

cmake_minimum_required (VERSION 3.16)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebinfo" CACHE STRING "Default build type")
endif ()

project(fquuid)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_REQUIRED_INCLUDES}
)

# fquuid-perf-test
add_executable(fquuid-perf-test perf/fquuid_perf_test.cpp)
target_compile_features(fquuid-perf-test PUBLIC cxx_std_20)
if (UNIX)
  target_compile_options(fquuid-perf-test PRIVATE -Wall)
elseif (MSVC)
  target_link_libraries(fquuid-perf-test PRIVATE bcrypt.lib)
endif()

# boost-perf-test
include(CheckIncludeFile)
check_include_file("boost/version.hpp" HAVE_BOOST)
if (HAVE_BOOST)
  add_executable(boost-perf-test perf/boost_perf_test.cpp)
  target_compile_features(boost-perf-test PUBLIC cxx_std_20)
  if (UNIX)
    target_compile_options(boost-perf-test PRIVATE -Wall)
  endif()
endif()

# fquuid-test
add_executable(fquuid-test test/fquuid_test.cpp)
target_compile_features(fquuid-test PUBLIC cxx_std_20)
if (UNIX)
  target_compile_options(fquuid-test PRIVATE -Wall -D_GLIBCXX_DEBUG)
elseif (MSVC)
  target_link_libraries(fquuid-test PRIVATE bcrypt.lib)
endif()

enable_testing()
add_test(NAME all COMMAND fquuid-test)

# fquuidgen
add_executable(fquuidgen tool/fquuidgen.cpp)
target_compile_features(fquuidgen PUBLIC cxx_std_20)
if (UNIX)
  target_compile_options(fquuidgen PRIVATE -Wall)
elseif (MSVC)
  target_link_libraries(fquuidgen PRIVATE bcrypt.lib)
endif()
