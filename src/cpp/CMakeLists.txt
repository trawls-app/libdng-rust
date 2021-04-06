cmake_minimum_required(VERSION 3.16.3)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(dngbindings)

set(CMAKE_CXX_STANDARD 14)

IF(MSVC)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
ENDIF(MSVC)

find_package(expat CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

find_package(JPEG REQUIRED)

ADD_SUBDIRECTORY(xmp_sdk)
ADD_SUBDIRECTORY(dng_sdk)

add_library(dngbindings
        ${CMAKE_CURRENT_SOURCE_DIR}/dng.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DNGHandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dnghost.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/negativeProcessor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rawConverter.cpp
       # ${CMAKE_CURRENT_SOURCE_DIR}/vendorProcessors/DNGprocessor.cpp
       # ${CMAKE_CURRENT_SOURCE_DIR}/vendorProcessors/ILCE7processor.cpp
       # ${CMAKE_CURRENT_SOURCE_DIR}/vendorProcessors/FujiProcessor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/vendorProcessors/variousVendorProcessor.cpp
        )

target_link_libraries(dngbindings PRIVATE dng-sdk ZLIB::ZLIB)
install(TARGETS dngbindings dng-sdk xmp-sdk)
