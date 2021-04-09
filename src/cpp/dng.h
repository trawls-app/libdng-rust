//
// Created by chris on 31.03.21.
//

#ifndef DNGBINDINGS_DNG_H
#define DNGBINDINGS_DNG_H

#include "rawConverter.h"
#include <dng_host.h>
#include <dng_xmp_sdk.h>

extern "C" RawConverter *
createConverter(const char *app_name, const char *app_version, ImageInfoContainer *image_info);
extern "C" void destroyConverter(RawConverter *handler);
extern "C" void callDummy(RawConverter *handler);
extern "C" void buildNegative(RawConverter *handler, unsigned short *image_buffer);

extern "C" void writeDNG(RawConverter *handler, const char *path);
extern "C" void writeTIFF(RawConverter *handler, const char *path);
extern "C" void writeJPEG(RawConverter *handler, const char *path);

void _write_output(const char *str);

#endif //DNGBINDINGS_DNG_H
