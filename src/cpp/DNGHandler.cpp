//
// Created by chris on 31.03.21.
//

#include "DNGHandler.h"

#include <iostream>


DNGHandler::DNGHandler(std::string app_name, std::string app_version) : _raw_converter(app_name.c_str(),
                                                                                       app_version.c_str(), 0, 0) {
    _app_name = app_name;
    _app_version = app_version;

    std::cout << "DNGHandler::DNGHandler(): " << _app_name << " " << _app_version << std::endl;
}

void DNGHandler::dummy() {
    std::cout << "DNGHandler::dummy()" << std::endl;
}

DNGHandler::~DNGHandler() {
    std::cout << "DNGHandler::~DNGHandler(): " << _app_name << " " << _app_version << std::endl;
}


