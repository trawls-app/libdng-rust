//
// Created by chris on 31.03.21.
//

#include "DNGHandler.h"

#include "dng_xmp_sdk.h"

#include <iostream>


DNGHandler::DNGHandler() {
    dng_xmp_sdk::InitializeSDK();

    _host.Reset(dynamic_cast<dng_host*>(new dng_host()));
    _app_name = "test";
}

void DNGHandler::dummy() {
    std::cout << "DNGHandler::dummy()" << std::endl;
    std::cout << "App Name: " << _app_name << std::endl;
}

DNGHandler::~DNGHandler() {
    std::cout << "DNGHandler::~DNGHandler()" << std::endl;
}


