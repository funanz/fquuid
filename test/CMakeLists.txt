# Copyright 2024 granz.fisherman@gmail.com
# https://opensource.org/license/mit

# Unit test settings
set(CHAR_TYPES char wchar_t char8_t char16_t char32_t)

set(CHAR_MODE_char 0)
set(CHAR_MODE_wchar_t 1632)
set(CHAR_MODE_char8_t 8)
set(CHAR_MODE_char16_t 16)
set(CHAR_MODE_char32_t 32)

# Unit test executable
foreach(TYPE IN LISTS CHAR_TYPES)
  add_executable(fquuid-test-${TYPE} fquuid_test.cpp)
  target_compile_features(fquuid-test-${TYPE} PUBLIC cxx_std_20)
  if (UNIX)
    target_compile_options(fquuid-test-${TYPE} PRIVATE -DCHAR_MODE=${CHAR_MODE_${TYPE}} -Wall -D_GLIBCXX_DEBUG PRIVATE)
  elseif (MSVC)
    target_compile_options(fquuid-test-${TYPE} PRIVATE -DCHAR_MODE=${CHAR_MODE_${TYPE}})
    target_link_libraries(fquuid-test-${TYPE} PRIVATE bcrypt.lib)
  endif()
  add_test(NAME test-${TYPE} COMMAND fquuid-test-${TYPE})
endforeach()
