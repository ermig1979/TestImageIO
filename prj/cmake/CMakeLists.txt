cmake_minimum_required(VERSION 2.8)
project(TestImageIO)

set(ROOT_DIR ${CMAKE_SOURCE_DIR}/../..)
set(3RD_DIR ${ROOT_DIR}/3rd)

include_directories(${3RD_DIR}/Cpl/src)

set(REQUIRE_SIMD ON CACHE BOOL "" FORCE)
include_directories(${3RD_DIR}/libjpeg-turbo)
add_subdirectory(${3RD_DIR}/libjpeg-turbo libjpeg-turbo)

include_directories(${3RD_DIR}/stb)


set(SIMD_AVX512VNNI OFF CACHE BOOL "" FORCE)
set(SIMD_TEST OFF CACHE BOOL "" FORCE)
set(SIMD_SHARED OFF CACHE BOOL "" FORCE)
set(SIMD_GET_VERSION OFF CACHE BOOL "" FORCE)
set(SIMD_TOOLCHAIN ${CMAKE_CXX_COMPILER})
set(SIMD_INFO ${ADX_INFO} CACHE BOOL "" FORCE)
set(SIMD_SYNET OFF CACHE BOOL "" FORCE)
set(SIMD_INT8_DEBUG OFF CACHE BOOL "" FORCE)
set(SIMD_PERF OFF CACHE BOOL "" FORCE)
set(SIMD_INSTALL OFF CACHE BOOL "" FORCE)
set(SIMD_UNINSTALL OFF CACHE BOOL "" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
include_directories(${3RD_DIR}/Simd/src)
add_subdirectory(${3RD_DIR}/Simd/prj/cmake Simd)

include_directories(${ROOT_DIR}/src)

file(GLOB_RECURSE TEST_SRC ${ROOT_DIR}/src/TestImageIO/*.cpp)
add_executable(TestImageIO ${TEST_SRC})
target_link_libraries(TestImageIO Simd turbojpeg -lpthread)

