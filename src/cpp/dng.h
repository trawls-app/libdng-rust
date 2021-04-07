//
// Created by chris on 31.03.21.
//

#ifndef DNGBINDINGS_DNG_H
#define DNGBINDINGS_DNG_H

#include "rawConverter.h"
#include <dng_host.h>
#include <dng_xmp_sdk.h>

extern "C" RawConverter * createConverter(const char *app_name, const char *app_version);
extern "C" void destroyConverter(RawConverter *handler);
extern "C" void callDummy(RawConverter *handler);

#endif //DNGBINDINGS_DNG_H
