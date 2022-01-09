#ifndef __GAME_OF_LIFE_WORLD_H__
#define __GAME_OF_LIFE_WORLD_H__

#include "time_control.h"

/// Stores the game state.
struct World {

    unsigned char *cells;
    unsigned int rows;
    unsigned int cols;

    // Position of the top left cell in world coords
    int tl_cell_pos_x;
    int tl_cell_pos_y;

    struct TimeControl update_rate;
};

struct World *worldCreate();
void worldDestroy(struct World *self);
void worldUpdate(struct World *self);
void worldToggleCell(struct World *self, int cell_pos_x, int cell_pos_y);
void worldLoadFromFile(struct World *self, const char *file_name);
void worldSaveToFile(struct World *self, const char *file_name);
void worldPrint(struct World *self);

#endif // __GAME_OF_LIFE_WORLD_H__
