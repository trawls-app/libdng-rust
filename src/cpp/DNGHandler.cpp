//
// Created by chris on 31.03.21.
//

#include "DNGHandler.h"

#include <iostream>


DNGHandler::DNGHandler() {
    _app_name = "test";
}

void DNGHandler::dummy() {
    std::cout << "DNGHandler::dummy()" << std::endl;
    std::cout << "App Name: " << _app_name << std::endl;
}

DNGHandler::~DNGHandler() {
    std::cout << "DNGHandler::~DNGHandler()" << std::endl;
}


