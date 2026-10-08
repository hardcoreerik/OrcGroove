cmake_minimum_required(VERSION 3.20)
project(orcgroove LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(groovematter_core
  src/field.cpp
)
target_include_directories(groovematter_core PUBLIC src)

add_library(groovematter_ui
  src/ui/tab5_ui.cpp
  src/ui/tab5_touch.cpp
)
target_include_directories(groovematter_ui PUBLIC src)

if (MSVC)
  target_compile_options(groovematter_core PRIVATE /W4)
  target_compile_options(groovematter_ui PRIVATE /W4)
else()
  target_compile_options(groovematter_core PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(groovematter_ui PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()

add_executable(test_field tests/test_field.cpp)
target_link_libraries(test_field PRIVATE groovematter_core)
add_test(NAME field COMMAND test_field)

add_executable(test_tab5_ui tests/test_tab5_ui.cpp)
target_link_libraries(test_tab5_ui PRIVATE groovematter_ui)
add_test(NAME tab5_ui COMMAND test_tab5_ui)

add_executable(test_tab5_touch tests/test_tab5_touch.cpp)
target_link_libraries(test_tab5_touch PRIVATE groovematter_ui)
add_test(NAME tab5_touch COMMAND test_tab5_touch)
