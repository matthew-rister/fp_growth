cmake_minimum_required (VERSION 3.8)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories("${CMAKE_SOURCE_DIR}/extern" "${CMAKE_SOURCE_DIR}/src", "${CMAKE_SOURCE_DIR}/test")

add_executable(fp_growth "main.cpp" "test/frequent_pattern_tree_test.cpp")

set_property(TARGET fp_growth PROPERTY CXX_STANDARD 17)
