cmake_minimum_required(VERSION 3.0.0)
project(fudpa-lab VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(CPack)

include(CTest)
enable_testing()

include_directories(lib)
include_directories(third_party/gtest/googletest/include)

add_subdirectory(third_party/gtest)
add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(test)
