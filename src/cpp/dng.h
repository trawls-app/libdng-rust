//
// Created by chris on 31.03.21.
//

#ifndef DNGBINDINGS_DNG_H
#define DNGBINDINGS_DNG_H

#include "rawConverter.h"
#include <dng_host.h>
#include <dng_xmp_sdk.h>

extern "C" RawConverter *
createConverter(ImageInfoContainer image_info, unsigned short *image_buffer, ExifBindings exif_bindings, void *exif_context, const char *make,
                const char *model);
extern "C" void destroyConverter(RawConverter *handler);
extern "C" void setAppName(RawConverter *handler, const char *app_name, const char *app_version);
extern "C" void callDummy(RawConverter *handler);
extern "C" void buildNegative(RawConverter *handler, unsigned short *image_buffer);

extern "C" void writeDNG(RawConverter *handler, const char *path);
extern "C" void writeTIFF(RawConverter *handler, const char *path);
extern "C" void writeJPEG(RawConverter *handler, const char *path);

void _write_output(const char *str);

#endif //DNGBINDINGS_DNG_H
