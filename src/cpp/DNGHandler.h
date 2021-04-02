//
// Created by chris on 31.03.21.
//

#ifndef DNGBINDINGS_DNGHANDLER_H
#define DNGBINDINGS_DNGHANDLER_H

#include <dng_host.h>
#include <dng_negative.h>

#include <string>

class DNGHandler {
public:
    DNGHandler();
    ~DNGHandler();
    void dummy();

protected:
    std::string _app_name;
    AutoPtr<dng_host> _host;
};


#endif //DNGBINDINGS_DNGHANDLER_H
