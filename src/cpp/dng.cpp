#include "dng.h"

#include <iostream>

extern "C" void dummy() {
  std::cout << "Called dummy from static C++ code" << std::endl;
  callHandlerDummy(getHandler());
}


DNGHandler * getHandler() {
    return new DNGHandler();
}

void destroyHandler(DNGHandler *handler) {
    std::cout << "Destroying handler" << std::endl;
    delete handler;
}


void callHandlerDummy(DNGHandler *handler) {
    handler->dummy();
}


