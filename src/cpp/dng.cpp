#include "dng.h"

#include <iostream>


RawConverter *
createConverter(const char *app_name, const char *app_version, unsigned short int width, unsigned short int height) {
    auto converter = new RawConverter(app_name, app_version, width, height);
    converter->registerPublisher(_write_output);
    return converter;
}

void destroyConverter(RawConverter *handler) {
    std::cout << "Destroying RawConverter" << std::endl;
    delete handler;
}

void callDummy(RawConverter *handler) {
    handler->dummy();
}

void buildNegative(RawConverter *handler, unsigned short *image_buffer) {
    try {
        handler->buildNegative("", image_buffer);
        handler->renderImage();
        handler->renderPreviews();
    }
    catch(const std::runtime_error& re) {
        std::cerr << "Runtime error: " << re.what() << std::endl;
    }
    catch(const std::exception& ex) {
        std::cerr << "Error occurred: " << ex.what() << std::endl;
    }
    catch(...) {
        std::cerr << "Unknown failure occurred." << std::endl;
    }
}

void _write_output(const char *str) {
    std::cout << str << std::endl;
}

void writeDNG(RawConverter *handler, const char *path) {
    handler->writeDng(path);
}

void writeTIFF(RawConverter *handler, const char *path) {
    handler->writeTiff(path);
}

void writeJPEG(RawConverter *handler, const char *path) {
    handler->writeJpeg(path);
}




