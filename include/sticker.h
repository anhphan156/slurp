#ifndef _STICKER_H
#define _STICKER_H

#include "box.h"
#include <cairo/cairo.h>

struct sticker {
    char            *path;
    float            scale;
    int              pivot;
    int              anchor;
    cairo_surface_t *surface;
};

void get_transformation(struct sticker *, double, double, struct slurp_box *, double *, double *, double *, double *);

#endif
