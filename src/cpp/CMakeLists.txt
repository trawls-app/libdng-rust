cmake_minimum_required(VERSION 3.16.3)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(dngbindings)

set(CMAKE_CXX_STANDARD 14)

find_package(expat CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

find_package(JPEG REQUIRED)

ADD_SUBDIRECTORY(xmp_sdk)
ADD_SUBDIRECTORY(dng_sdk)

add_library(dngbindings dng.cpp)
target_link_libraries(dngbindings PRIVATE dng-sdk)
install(TARGETS dngbindings dng-sdk xmp-sdk)
