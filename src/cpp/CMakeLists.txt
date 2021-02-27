cmake_minimum_required(VERSION 3.16)
project(dng)

IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/EHsc")
ENDIF(MSVC)

ADD_SUBDIRECTORY(libjpeg)
ADD_SUBDIRECTORY(xmp_sdk)
ADD_SUBDIRECTORY(dng_sdk)
ADD_SUBDIRECTORY(xmp_sdk/third-party/zlib)


FIND_PACKAGE(Threads)

# =======================================================
# libdng source code

ADD_LIBRARY( dng STATIC ${CMAKE_CURRENT_SOURCE_DIR}/dng.cpp )

TARGET_INCLUDE_DIRECTORIES( dng PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
# TARGET_COMPILE_DEFINITIONS( dng PRIVATE -DkLocalUseThreads=1 )
TARGET_COMPILE_OPTIONS( dng PUBLIC -fexceptions -std=c++11 )

TARGET_LINK_LIBRARIES( dng dng_sdk xmp-sdk libjpeg zlibstatic)
install(TARGETS dng dng_sdk xmp-sdk libjpeg zlibstatic DESTINATION .)
