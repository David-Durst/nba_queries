cmake_minimum_required (VERSION 3.14)

project (car_queries VERSION "0.1")
find_package(OpenMP REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE CAR_SOURCES ${PROJECT_SOURCE_DIR}/lib/*.cpp)
list(FILTER CAR_SOURCES EXCLUDE REGEX "\.#.*")
add_library(car_queries_lib STATIC ${CAR_SOURCES} ${CAR_HEADERS})
add_executable(preprocess_cars ${PROJECT_SOURCE_DIR}/bin/preprocess_main.cpp)
target_link_libraries(car_queries_lib OpenMP::OpenMP_CXX car_moment)
target_link_libraries(preprocess_cars car_queries_lib)

install (TARGETS preprocess_cars DESTINATION bin)
