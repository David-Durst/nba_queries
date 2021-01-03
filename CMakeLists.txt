cmake_minimum_required (VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

project (nba_queries VERSION "0.1")

include_directories(${PROJECT_BINARY_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB_RECURSE NBA_SOURCES ${PROJECT_SOURCE_DIR}/src/cpp/lib/*.cpp)
list(FILTER NBA_SOURCES EXCLUDE REGEX "\.#.*")
file(GLOB NBA_TESTS "${PROJECT_SOURCE_DIR}/test/*.cpp")
list(FILTER NBA_TESTS EXCLUDE REGEX "\.#.*")
file(GLOB_RECURSE NBA_TESTS_SOURCES ${PROJECT_SOURCE_DIR}/src/cpp/lib/*.cpp)
list(FILTER NBA_TESTS_SOURCES EXCLUDE REGEX "\.#.*")


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

add_executable(nba_queries ${NBA_SOURCES} ${PROJECT_SOURCE_DIR}/src/cpp/bin/query_main.cpp)
add_executable(preprocess_moments ${NBA_SOURCES} ${PROJECT_SOURCE_DIR}/src/cpp/bin/preprocess_main.cpp)
# add this to enable debug printing statements
#target_compile_definitions(nba_queries PRIVATE DEBUG)
#target_compile_definitions(nba_queries PRIVATE CALLGRIND)

# add the install targets
install (TARGETS nba_queries DESTINATION bin)

# ADDING TESTS
# add executable
add_executable(tests ${NBA_TESTS_SOURCES} ${NBA_TESTS})
target_include_directories(tests PUBLIC ${PROJECT_SOURCE_DIR}/catch)
target_include_directories(tests PUBLIC ${PROJECT_BINARY_DIR}/catch)

