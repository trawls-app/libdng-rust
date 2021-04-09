//
// Created by chris on 09.04.21.
//

#ifndef DNGBINDINGS_WRAPPER_H
#define DNGBINDINGS_WRAPPER_H

typedef struct ImageInfoContainer {
    const char *make;
    const char *model;
    unsigned short width, height;
    const double black_levels[4], white_levels[4];
    const double camera_neutral[3];
    const float xyz_to_cam[3][3];
} ImageInfoContainer;

#endif //DNGBINDINGS_WRAPPER_H
