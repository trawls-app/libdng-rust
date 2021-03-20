#include "library.h"
#include <dng_xmp_sdk.h>
#include <dng_host.h>
#include <iostream>

extern "C" void hello() {
    dng_xmp_sdk::InitializeSDK();
    dng_host host;
    //host.Reset(dynamic_cast<dng_host*>(new DngHost()));
    std::cout << "Hello, World!" << std::endl;
}
