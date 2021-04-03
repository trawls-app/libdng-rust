//
// Created by chris on 31.03.21.
//

#ifndef DNGBINDINGS_DNG_H
#define DNGBINDINGS_DNG_H

#include "DNGHandler.h"
#include <dng_host.h>
#include <dng_xmp_sdk.h>

extern "C" DNGHandler *createHandler(const char *app_name, const char *app_version);
extern "C" void destroyHandler(DNGHandler *handler);
extern "C" void callHandlerDummy(DNGHandler *handler);

#endif //DNGBINDINGS_DNG_H
