# Copyright 2024 granz.fisherman@gmail.com
# https://opensource.org/license/mit

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
check_include_file("boost/version.hpp" HAVE_BOOST)
if (HAVE_BOOST)
  add_executable(boost-perf-test boost_perf_test.cpp)
  target_compile_features(boost-perf-test PUBLIC cxx_std_20)
  if (UNIX)
    target_compile_options(boost-perf-test PRIVATE -Wall)
  endif()
endif()
