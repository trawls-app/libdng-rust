#include "dng.h"
#include <iostream>


RawConverter *
createConverter(ImageInfoContainer image_info, unsigned short *image_buffer, ExifBindings exif_bindings, void *exif_context, const char *make,
                const char *model) {
    auto converter = new RawConverter(image_info, exif_bindings, exif_context, make, model);
    converter->registerPublisher(exif_bindings.log_debug);

    try {
        converter->buildNegative("", image_buffer);
        converter->renderImage();
        converter->renderPreviews();
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

    return converter;
}

void destroyConverter(RawConverter *handler) {
    delete handler;
}

void callDummy(RawConverter *handler) {
    handler->dummy();
}

void setAppName(RawConverter *handler, const char *app_name, const char *app_version) {
    handler->setAppName(app_name, app_version);
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


void writeDNG(RawConverter *handler, const char *path) {
    handler->writeDng(path);
}

void writeTIFF(RawConverter *handler, const char *path) {
    handler->writeTiff(path);
}

void writeJPEG(RawConverter *handler, const char *path) {
    handler->writeJpeg(path);
}




