cmake_minimum_required(VERSION 3.0.0)
project(JSON_Parser VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(JSON_Parser main.c)

target_include_directories(JSON_Parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
