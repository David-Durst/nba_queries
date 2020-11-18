cmake_minimum_required (VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

project (nba_queries VERSION "0.1")

include_directories(${PROJECT_BINARY_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB NBA_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
list(FILTER NBA_SOURCES EXCLUDE REGEX "\.#.*")


# from OpenCV via https://medium.com/@onur.dundar1/cmake-tutorial-585dd180109b
# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(STATUS "CMAKE_SOURCE_DIR is ${CMAKE_SOURCE_DIR}")
    message(STATUS "CMAKE_BINARY_DIR is ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()


add_executable(nba_queries ${NBA_SOURCES})

# add the install targets
install (TARGETS nba_queries DESTINATION bin)

#include(Tests.cmake)<Paste>
