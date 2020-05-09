cmake_minimum_required (VERSION 3.8)
project (fp_growth LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories(src/ extern/)
add_executable (frequent_pattern_tree_test test/frequent_pattern_tree_test.cpp)