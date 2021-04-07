#include "dng.h"

#include <iostream>


RawConverter * createConverter(const char *app_name, const char *app_version) {
    return new RawConverter(app_name, app_version, 100, 100);
}

void destroyConverter(RawConverter *handler) {
    std::cout << "Destroying RawConverter" << std::endl;
    delete handler;
}


void callDummy(RawConverter *handler) {
    handler->dummy();
}


