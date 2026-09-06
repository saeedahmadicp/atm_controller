cmake_minimum_required(VERSION 3.16)
project(atm_controller)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(atmController src/atmController.cpp)
target_include_directories(atmController PUBLIC include)

enable_testing()

add_executable(atm_tests tests/atmControllerTest.cpp)
target_include_directories(atm_tests PRIVATE tests)
target_link_libraries(atm_tests PRIVATE atmController)

add_test(NAME atm_tests COMMAND atm_tests)
