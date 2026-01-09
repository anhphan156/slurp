#include "sticker.h"
#include <cairo/cairo.h>

void get_transformation(struct sticker *sticker, double sticker_w, double sticker_h, struct slurp_box *box, double *x, double *y, double *scaled_w, double *scaled_h) {
    *scaled_w = sticker->scale * box->width / sticker_w;
    *scaled_h = *scaled_w;

    if (sticker->anchor == 0) {
        *x = box->x;
        *y = box->y;
    } else if (sticker->anchor == 1) {
        *x = (box->x + box->width) - *scaled_w * sticker_w;
        *y = box->y;
    } else if (sticker->anchor == 2) {
        *x = (box->x + box->width) - *scaled_w * sticker_w;
        *y = (box->y + box->height) - *scaled_h * sticker_h;
    } else if (sticker->anchor == 3) {
        *x = box->x;
        *y = (box->y + box->height) - *scaled_h * sticker_h;
    }
}
