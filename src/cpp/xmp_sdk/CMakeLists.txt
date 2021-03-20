find_package(expat CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIR} )
ADD_DEFINITIONS(${EXPAT_DEFINITIONS})

# =======================================================
# XMP SDK source code.

IF (WIN32)
    SET(HOST_IO ${CMAKE_CURRENT_SOURCE_DIR}/source/Host_IO-Win.cpp)
ELSE()
    SET(HOST_IO ${CMAKE_CURRENT_SOURCE_DIR}/source/Host_IO-POSIX.cpp)
ENDIF()

ADD_LIBRARY( xmp-sdk STATIC
        ${HOST_IO}
        ${CMAKE_CURRENT_SOURCE_DIR}/source/IOUtils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/PerfUtils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/SafeStringAPIs.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/UnicodeConversions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/XIO.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/XML_Node.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/XMP_LibUtils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/XMP_ProgressTracker.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/XMPFiles_IO.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XMPCore/source/ExpatAdapter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XMPCore/source/ParseRDF.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XMPCore/source/WXMPIterator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XMPCore/source/WXMPMeta.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XMPCore/source/WXMPUtils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XMPCore/source/XMPCore_Impl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XMPCore/source/XMPIterator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XMPCore/source/XMPMeta-GetSet.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XMPCore/source/XMPMeta-Parse.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XMPCore/source/XMPMeta-Serialize.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XMPCore/source/XMPMeta.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XMPCore/source/XMPUtils-FileInfo.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XMPCore/source/XMPUtils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/zuid/interfaces/MD5.cpp )


TARGET_INCLUDE_DIRECTORIES( xmp-sdk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public/include
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )



IF (WIN32)
    TARGET_COMPILE_DEFINITIONS( xmp-sdk PUBLIC -DWIN_ENV=1 )
ELSE()
    TARGET_COMPILE_DEFINITIONS( xmp-sdk PUBLIC -DUNIX_ENV=1 )
ENDIF()

# -XMP_64=1                # manually set 64-bit


TARGET_COMPILE_OPTIONS( xmp-sdk PRIVATE -w )

TARGET_LINK_LIBRARIES( xmp-sdk STATIC expat::expat ZLIB::ZLIB )
