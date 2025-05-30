# Copyright 2024 granz.fisherman@gmail.com
# https://opensource.org/license/mit

add_executable(fquuid-test fquuid_test.cpp)
target_compile_features(fquuid-test PUBLIC cxx_std_20)
if (UNIX)
  target_compile_options(fquuid-test PRIVATE -Wall -D_GLIBCXX_DEBUG)
elseif (MSVC)
  target_link_libraries(fquuid-test PRIVATE bcrypt.lib)
endif()

add_test(NAME all COMMAND fquuid-test)
