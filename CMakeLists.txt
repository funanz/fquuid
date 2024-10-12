cmake_minimum_required (VERSION 3.16)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebinfo" CACHE STRING "Default build type")
endif ()

project(fquuid)

add_executable(fquuid-perf-test fquuid_perf_test.cpp)
target_compile_features(fquuid-perf-test PUBLIC cxx_std_20)
if (UNIX)
  target_compile_options(fquuid-perf-test PRIVATE -Wall)
elseif (MSVC)
  target_link_libraries(fquuid-perf-test PRIVATE bcrypt.lib)
endif()

add_executable(fquuid-test fquuid_test.cpp)
target_compile_features(fquuid-test PUBLIC cxx_std_20)
if (UNIX)
  target_compile_options(fquuid-test PRIVATE -Wall -D_GLIBCXX_DEBUG)
elseif (MSVC)
  target_link_libraries(fquuid-test PRIVATE bcrypt.lib)
endif()

enable_testing()
add_test(NAME all COMMAND fquuid-test)
