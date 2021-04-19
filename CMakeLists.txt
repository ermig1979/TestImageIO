cmake_minimum_required(VERSION 2.8)
project(TestImageIO)

set(REQUIRE_SIMD ON CACHE BOOL "" FORCE)
include_directories(./3rd/libjpeg-turbo)
add_subdirectory(./3rd/libjpeg-turbo libjpeg-turbo)

set(SIMD_AVX512 ON CACHE BOOL "" FORCE)
set(SIMD_AVX512VNNI OFF CACHE BOOL "" FORCE)
set(SIMD_TEST OFF CACHE BOOL "" FORCE)
set(SIMD_SHARED ON CACHE BOOL "" FORCE)
set(SIMD_GET_VERSION OFF CACHE BOOL "" FORCE)
set(SIMD_TOOLCHAIN ${CMAKE_CXX_COMPILER})
set(SIMD_INFO OFF FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
include_directories(./3rd/Simd/src)
add_subdirectory(./3rd/Simd/prj/cmake Simd)

file(GLOB_RECURSE SRC_CPP ./src/*.cpp)
add_executable(TestImageIO ${SRC_CPP})
target_link_libraries(TestImageIO Simd turbojpeg -lpthread)

