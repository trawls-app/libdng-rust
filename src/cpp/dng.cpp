#include "dng.h"

#include <iostream>


DNGHandler *createHandler(const char *app_name, const char *app_version) {
    return new DNGHandler(app_name, app_version);
}

void destroyHandler(DNGHandler *handler) {
    std::cout << "Destroying handler" << std::endl;
    delete handler;
}


void callHandlerDummy(DNGHandler *handler) {
    handler->dummy();
}


