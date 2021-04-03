//
// Created by chris on 31.03.21.
//

#ifndef DNGBINDINGS_DNGHANDLER_H
#define DNGBINDINGS_DNGHANDLER_H

#include "rawConverter.h"

#include <string>

class DNGHandler {
public:
    DNGHandler(std::string app_name, std::string app_version);
    ~DNGHandler();
    void dummy();

protected:
    RawConverter _raw_converter;
    std::string _app_name, _app_version;
};


#endif //DNGBINDINGS_DNGHANDLER_H
