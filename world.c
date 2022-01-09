#include "world.h"

#include <stdio.h>
#include <stdlib.h>

struct World *worldCreate() {
    fprintf(stderr, "world::worldCreate: \n");

    struct World *self = malloc(sizeof(struct World));
    if (!self) {
        fprintf(stderr, "Failed to allocate memory for the world.\n");
        return NULL;
    }
    
    self->update_rate.ticks_per_sec = 2.0f;
    self->update_rate.last_tick = timeNow();
    self->tl_cell_pos_x = 0;
    self->tl_cell_pos_y = 0;
    self->rows = 100;
    self->cols = 100;
    self->cells = calloc(self->rows * self->cols, sizeof(unsigned char));

    if (!self->cells) {
        fprintf(stderr, "Failed to allocate memory for the world cells.\n");
        free(self);
        return NULL;
    }

    return self;
}

void worldDestroy(struct World *self) {
    fprintf(stderr, "world::worldDestroy: \n");
    if (!self)
        return;

    free(self->cells);
    free(self);
}

void worldUpdate(struct World *self) {
    fprintf(stderr, "world::update_world: \n");


}

void worldToggleCell(struct World *self, int cell_pos_x, int cell_pos_y) {

}

void worldLoadFromFile(struct World *self, const char *file_name) {

}

void worldSaveToFile(struct World *self, const char *file_name) {

}

void worldPrint(struct World *self) {

}
