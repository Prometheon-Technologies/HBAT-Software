cmake_minimum_required(VERSION 3.0.0)
project(HMS VERSION 0.1.0)

include(CTest)
enable_testing()

add_library( HMS.cpp)
add_library(<name> []
             [EXCLUDE_FROM_ALL]
             [<source>...])

add_library(<name> [STATIC | SHARED | MODULE]
[EXCLUDE_FROM_ALL]
[<source>...])

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
