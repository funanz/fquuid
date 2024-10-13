cmake_minimum_required (VERSION 3.16)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebinfo" CACHE STRING "Default build type")
endif ()

project(fquuid)

include_directories(${CMAKE_INCLUDE_PATH})

# fquuid-perf-test
add_executable(fquuid-perf-test fquuid_perf_test.cpp)
target_compile_features(fquuid-perf-test PUBLIC cxx_std_20)
if (UNIX)
  target_compile_options(fquuid-perf-test PRIVATE -Wall)
elseif (MSVC)
  target_link_libraries(fquuid-perf-test PRIVATE bcrypt.lib)
endif()

# boost-perf-test
include(CheckIncludeFile)
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_INCLUDE_PATH})
check_include_file("boost/version.hpp" HAVE_BOOST)
if (HAVE_BOOST)
  add_executable(boost-perf-test boost_perf_test.cpp)
  target_compile_features(boost-perf-test PUBLIC cxx_std_20)
  if (UNIX)
    target_compile_options(boost-perf-test PRIVATE -Wall)
  endif()
endif()

# fquuid-test
add_executable(fquuid-test fquuid_test.cpp)
target_compile_features(fquuid-test PUBLIC cxx_std_20)
if (UNIX)
  target_compile_options(fquuid-test PRIVATE -Wall -D_GLIBCXX_DEBUG)
elseif (MSVC)
  target_link_libraries(fquuid-test PRIVATE bcrypt.lib)
endif()

enable_testing()
add_test(NAME all COMMAND fquuid-test)

# fquuidgen
add_executable(fquuidgen fquuidgen.cpp)
target_compile_features(fquuidgen PUBLIC cxx_std_20)
if (UNIX)
  target_compile_options(fquuidgen PRIVATE -Wall)
elseif (MSVC)
  target_link_libraries(fquuidgen PRIVATE bcrypt.lib)
endif()
