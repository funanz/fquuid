# Copyright 2024 granz.fisherman@gmail.com
# https://opensource.org/license/mit

add_executable(fquuidgen fquuidgen.cpp)
target_compile_features(fquuidgen PUBLIC cxx_std_20)
if (UNIX)
  target_compile_options(fquuidgen PRIVATE -Wall)
elseif (MSVC)
  target_link_libraries(fquuidgen PRIVATE bcrypt.lib)
endif()
