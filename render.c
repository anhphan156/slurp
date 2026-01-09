#include <cairo/cairo.h>
#include <stdio.h>

#include "pool-buffer.h"
#include "render.h"
#include "slurp.h"
#include "sticker.h"

static void set_source_u32(cairo_t *cairo, uint32_t color) {
    cairo_set_source_rgba(cairo, (color >> (3 * 8) & 0xFF) / 255.0, (color >> (2 * 8) & 0xFF) / 255.0, (color >> (1 * 8) & 0xFF) / 255.0, (color >> (0 * 8) & 0xFF) / 255.0);
}

static void draw_rect(cairo_t *cairo, struct slurp_box *box, uint32_t color) {
    set_source_u32(cairo, color);
    cairo_rectangle(cairo, box->x, box->y, box->width, box->height);
}

static void draw_rect_with_image(cairo_t *cairo, struct slurp_box *box, uint32_t color, struct sticker *sticker) {
    cairo_save(cairo);

    cairo_rectangle(cairo, box->x, box->y, box->width, box->height);

    double sticker_w = cairo_image_surface_get_width(sticker->surface);
    double sticker_h = cairo_image_surface_get_height(sticker->surface);
    double x         = 0;
    double y         = 0;
    double scaled_w  = 0;
    double scaled_h  = 0;

    get_transformation(sticker, sticker_w, sticker_h, box, &x, &y, &scaled_w, &scaled_h);
    cairo_translate(cairo, x, y);
    cairo_scale(cairo, scaled_w, scaled_h);

    cairo_set_source_surface(cairo, sticker->surface, 0, 0);
    cairo_fill(cairo);

    cairo_restore(cairo);
}

void render(struct slurp_output *output) {
    struct slurp_state *state  = output->state;
    struct pool_buffer *buffer = output->current_buffer;
    cairo_t            *cairo  = buffer->cairo;

    // Clear
    cairo_set_operator(cairo, CAIRO_OPERATOR_SOURCE);
    set_source_u32(cairo, state->colors.background);
    cairo_paint(cairo);

    // Draw option boxes from input
    struct slurp_box *choice_box;
    wl_list_for_each(choice_box, &state->boxes, link) {
        if (box_intersect(&output->logical_geometry, choice_box)) {
            draw_rect(cairo, choice_box, state->colors.choice);
            cairo_fill(cairo);
        }
    }

    struct slurp_seat *seat;
    wl_list_for_each(seat, &state->seats, link) {
        struct slurp_selection *current_selection = slurp_seat_current_selection(seat);

        if (!current_selection->has_selection && state->crosshairs) {
            struct slurp_box *output_box = &output->logical_geometry;
            if (in_box(output_box, current_selection->x, current_selection->y)) {

                set_source_u32(cairo, state->colors.border);
                cairo_rectangle(cairo, output_box->x, current_selection->y, output->logical_geometry.width, 1);
                cairo_fill(cairo);
                cairo_rectangle(cairo, current_selection->x, output->logical_geometry.y, 1, output->logical_geometry.height);
                cairo_fill(cairo);
            }
        }

        if (!current_selection->has_selection) {
            continue;
        }

        if (!box_intersect(&output->logical_geometry, &current_selection->selection)) {
            continue;
        }
        struct slurp_box *sel_box = &current_selection->selection;

        draw_rect(cairo, sel_box, state->colors.selection);
        cairo_fill(cairo);

        if (state->stickers_enabled) {
            cairo_set_operator(cairo, CAIRO_OPERATOR_OVER);
            for (size_t i = 0; i < state->stickers_size; i += 1)
                draw_rect_with_image(cairo, sel_box, state->colors.selection, state->stickers + i);
        }

        // Draw border
        cairo_set_line_width(cairo, state->border_weight);
        draw_rect(cairo, sel_box, state->colors.border);
        cairo_stroke(cairo);

        if (state->display_dimensions) {
            cairo_select_font_face(cairo, state->font_family, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size(cairo, 14);
            set_source_u32(cairo, state->colors.border);
            // buffer of 12 can hold selections up to 99999x99999
            char dimensions[12];
            snprintf(dimensions, sizeof(dimensions), "%ix%i", sel_box->width, sel_box->height);
            cairo_move_to(cairo, sel_box->x + sel_box->width + 10, sel_box->y + sel_box->height + 20);
            cairo_show_text(cairo, dimensions);
        }
    }
}
