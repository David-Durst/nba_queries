cmake_minimum_required (VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_BUILD_TYPE Release)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopt-info-vec-all")
#set(CMAKE_VERBOSE_MAKEFILE ON)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

project (nba_queries VERSION "0.1")
include(GNUInstallDirs)
find_package(OpenMP REQUIRED)
find_package(CapnProto REQUIRED)

include_directories(${PROJECT_BINARY_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src/proto)
# following line is a hack for clion to recognize the directory
include_directories(./build/src/proto)
file(GLOB_RECURSE NBA_SOURCES ${PROJECT_SOURCE_DIR}/src/cpp/lib/*.cpp)
list(FILTER NBA_SOURCES EXCLUDE REGEX "\.#.*")
file(GLOB NBA_TESTS "${PROJECT_SOURCE_DIR}/test/*.cpp")
list(FILTER NBA_TESTS EXCLUDE REGEX "\.#.*")
file(GLOB_RECURSE NBA_TESTS_SOURCES ${PROJECT_SOURCE_DIR}/src/cpp/lib/*.cpp)
list(FILTER NBA_TESTS_SOURCES EXCLUDE REGEX "\.#.*")
file(GLOB_RECURSE NBA_HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE PROTO_FILES ${PROJECT_SOURCE_DIR}/src/proto/*.capnp)


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

# Generate header and source files for all the CapnProto files
foreach(proto ${PROTO_FILES})
    # Get filename without extension
    get_filename_component(PROTO_NAME_WE ${proto} NAME_WE)
    get_filename_component(PROTO_NAME ${proto} NAME)
    # message(STATUS "Generating Protobuf Code for ${PROTO_NAME}")
    capnp_generate_cpp(PROTO_SRCS PROTO_HDRS ${proto})
    add_library(${PROTO_NAME_WE} STATIC ${PROTO_SRCS} ${PROTO_HDRS})
    target_link_libraries(${PROTO_NAME_WE} CapnProto::capnp)
    message("current binary dir ${CMAKE_CURRENT_BINARY_DIR}")
    message("capnpc_output_dir ${CAPNPC_OUTPUT_DIR}")
endforeach()

add_library(nba_queries_lib STATIC ${NBA_SOURCES} ${NBA_HEADERS})
add_executable(nba_queries ${PROJECT_SOURCE_DIR}/src/cpp/bin/query_main.cpp)
add_executable(cleaned_nba_queries ${PROJECT_SOURCE_DIR}/src/cpp/bin/cleaned_query_main.cpp)
add_executable(preprocess_moments ${PROJECT_SOURCE_DIR}/src/cpp/bin/preprocess_main.cpp)
add_executable(generate_nba_proto ${PROJECT_SOURCE_DIR}/src/cpp/bin/generate_nba_proto_main.cpp)
target_link_libraries(nba_queries OpenMP::OpenMP_CXX)
target_link_libraries(cleaned_nba_queries OpenMP::OpenMP_CXX)
target_link_libraries(preprocess_moments OpenMP::OpenMP_CXX)
target_link_libraries(nba_queries_lib OpenMP::OpenMP_CXX)
target_link_libraries(nba_queries nba_queries_lib)
target_link_libraries(preprocess_moments nba_queries_lib)
target_link_libraries(cleaned_nba_queries nba_queries_lib)
target_link_libraries(generate_nba_proto nba_queries_lib)
# add this to enable debug printing statements
#target_compile_definitions(nba_queries PRIVATE DEBUG)
#target_compile_definitions(nba_queries PRIVATE CALLGRIND)
OPTION(ENABLE_CALLGRIND "enable CALLGRIND preprocessor" OFF) # Enabled by default
IF(ENABLE_CALLGRIND)
    target_compile_definitions(cleaned_nba_queries PRIVATE CALLGRIND)
ENDIF(ENABLE_CALLGRIND)

set_target_properties(nba_queries_lib PROPERTIES VERSION ${PROJECT_VERSION})

target_link_libraries(nba_queries_lib moment ${Protobuf_LIBRARIES})
#target_link_libraries(generate_nba_proto PUBLIC moment ${Protobuf_LIBRARIES})
#target_include_directories(nba_queries_lib PUBLIC ${Protobuf_INCLUDE_DIRS} ${PROJECT_BINARY_DIR})
#message("cmake_binary_dir ${PROJECT_BINARY_DIR}")

# add the install targets
install (TARGETS nba_queries DESTINATION bin)
install (TARGETS cleaned_nba_queries DESTINATION bin)
install (TARGETS preprocess_moments DESTINATION bin)
install (TARGETS nba_queries_lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDERDIR})

# ADDING TESTS
# add executable
add_executable(tests ${NBA_TESTS_SOURCES} ${NBA_TESTS})
target_link_libraries(tests OpenMP::OpenMP_CXX)
target_link_libraries(tests nba_queries_lib)
target_include_directories(tests PUBLIC ${PROJECT_SOURCE_DIR}/catch)
target_include_directories(tests PUBLIC ${PROJECT_BINARY_DIR}/catch)

add_subdirectory(src/cpp/cars)
