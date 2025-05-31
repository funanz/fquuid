# Copyright 2024 granz.fisherman@gmail.com
# https://opensource.org/license/mit

# Unit test settings
set(CHAR_TYPES char wchar_t char8_t char16_t char32_t)

# Unit test executable
foreach(TYPE IN LISTS CHAR_TYPES)
  set(TARGET fquuid-test-${TYPE})
  string(TOUPPER CT_${TYPE} TYPE_NAME)

  add_executable(${TARGET} fquuid_test.cpp)
  target_compile_features(${TARGET} PUBLIC cxx_std_20)
  if (UNIX)
    target_compile_options(${TARGET} PRIVATE -DCHAR_TYPE=${TYPE_NAME} -Wall -D_GLIBCXX_DEBUG)
  elseif (MSVC)
    target_compile_options(${TARGET} PRIVATE -DCHAR_TYPE=${TYPE_NAME})
    target_link_libraries(${TARGET} PRIVATE bcrypt.lib)
  endif()
  add_test(NAME test-${TYPE} COMMAND ${TARGET})
endforeach()
