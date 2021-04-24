//
// Created by chris on 09.04.21.
//

#ifndef DNGBINDINGS_WRAPPER_H
#define DNGBINDINGS_WRAPPER_H

typedef struct Area {
    unsigned short top;
    unsigned short left;
    unsigned short bottom;
    unsigned short right;
} Area;

typedef struct ImageInfoContainer {
    unsigned short width, height;
    const double black_levels[4], white_levels[4];
    const double camera_neutral[3];
    const float xyz_to_cam[3][3];
    Area active_area;
} ImageInfoContainer;

enum ExifTag {
    ExposureTime,
    ApertureValue,
    CameraMake,
    CameraModel,
    ISOSpeedValue
};


typedef struct ExifRational { int numerator; int denominator; } ExifRational;
typedef struct ExifURational { unsigned int numerator; unsigned int denominator; } ExifURational;

typedef struct ExifBindings {
    unsigned int (*get_uint)(void*, unsigned int, unsigned short, unsigned int*);
    unsigned int (*get_urational)(void*, unsigned int, unsigned short, ExifURational*);
    unsigned int (*get_rational)(void*, unsigned int, unsigned short, ExifRational*);
    char *  (*get_string)(void*, unsigned int);
    void (*free_rs_string)(char*);
} ExifBindings;

#endif //DNGBINDINGS_WRAPPER_H
