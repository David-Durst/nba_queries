cmake_minimum_required (VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

project (nba_queries VERSION "0.1")

include_directories(${PROJECT_BINARY_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB_RECURSE NBA_SOURCES ${PROJECT_SOURCE_DIR}/src/cpp *.cpp)
list(FILTER NBA_SOURCES EXCLUDE REGEX "\.#.*")
file(GLOB NBA_TESTS "${PROJECT_SOURCE_DIR}/test/cpp/*.cpp")
list(FILTER NBA_TESTS EXCLUDE REGEX "\.#.*")
file(GLOB_RECURSE NBA_TESTS_SOURCES ${PROJECT_SOURCE_DIR}/src/cpp/lib *.cpp)
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


add_executable(nba_queries ${NBA_SOURCES})

# add the install targets
install (TARGETS nba_queries DESTINATION bin)

# ADDING TESTS
# add executable
add_executable(tests ${NBA_TESTS_SOURCES} ${NBA_TESTS})

# add catch2 library to tests and tell build to run all tests
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.12.4)

FetchContent_MakeAvailable(Catch2)

FetchContent_GetProperties(Catch2) #mispelled name in original post
if(NOT catch2_POPULATED)   # name is lowercased
  FetchContent_Populate(Catch2)
  message(STATUS "Catch source dir: ${catch2_SOURCE_DIR}")
  message(STATUS "Catch binary dir: ${catch2_BINARY_DIR}")
  add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR}) # name is lowercased
endif()

target_link_libraries(tests Catch2::Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)
catch_discover_tests(tests)
