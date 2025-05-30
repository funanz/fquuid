# Copyright 2024 granz.fisherman@gmail.com
# https://opensource.org/license/mit

cmake_minimum_required (VERSION 3.16)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebinfo" CACHE STRING "Default build type")
endif ()

project(fquuid)

include_directories(include ${CMAKE_REQUIRED_INCLUDES})
enable_testing()

add_subdirectory(perf)
add_subdirectory(test)
add_subdirectory(tool)
