#ADD_SUBDIRECTORY(libjpeg)
ADD_SUBDIRECTORY(xmp_sdk)
ADD_SUBDIRECTORY(dng_sdk)

FIND_PACKAGE(Threads)

# =======================================================
# libdng source code

ADD_LIBRARY( dng STATIC ${CMAKE_CURRENT_SOURCE_DIR}/dng.cpp )

TARGET_INCLUDE_DIRECTORIES( dng INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
# TARGET_COMPILE_DEFINITIONS( dng PRIVATE -DkLocalUseThreads=1 )
TARGET_COMPILE_OPTIONS( dng PRIVATE -fexceptions -std=c++11 )

TARGET_LINK_LIBRARIES( dng dng_sdk xmp-sdk)
