#ifndef __GAME_OF_LIFE_WORLD_H__
#define __GAME_OF_LIFE_WORLD_H__

#include "time_control.h"

/// Stores the game state.
struct World {

    unsigned char *cells;
    unsigned int rows;
    unsigned int cols;

    // State of cells on the next time step.
    unsigned char *cells_next;
    unsigned int cn_rows;
    unsigned int cn_cols;

    // Position of the top left cell in world coords
    int tl_cell_pos_x;
    int tl_cell_pos_y;

    // Memory for cells is allocated in blocks
    unsigned int block_rows;
    unsigned int block_cols;

    struct TimeControl update_rate;
    int updates_paused;
    int edit_mode;
};

struct World *worldCreate();
void worldDestroy(struct World *self);
int worldUpdate(struct World *self);
void worldToggleCell(struct World *self, int c, int r);
unsigned char *worldCell(struct World *self, int c, int r);
unsigned char *worldCellNext(struct World *self, int c, int r);
int worldLoadFromFile(struct World *self, const char *file_name);
int worldSaveToFile(struct World *self, const char *file_name);
void worldPrint(struct World *self);
int worldIncreaseCells(struct World *self, int grow_top, int grow_bottom, int grow_left, int grow_right);
int worldIncreaseCellsNext(struct World *self);

#endif // __GAME_OF_LIFE_WORLD_H__
