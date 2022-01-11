#include "world.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_WORLD_FILE_BYTES 8192

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
    self->rows = 20;
    self->cols = 20;
    self->cells = calloc(self->rows * self->cols, sizeof(unsigned char));
    self->cn_rows = self->rows + 2;
    self->cn_cols = self->cols + 2;
    self->cells_next = calloc(self->cn_rows * self->cn_cols, sizeof(unsigned char));

    if (!self->cells || !self->cells_next) {
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
    free(self->cells_next);
    free(self);
}

static int isWithinDomain(struct World *self, int c, int r) {
    return r >= 0 && c >= 0 && r < (int) self->rows && c < (int) self->cols;
}

int worldUpdate(struct World *self) {
    fprintf(stderr, "world::update_world: \n");

    // 1. Any live cell with two or three live neighbours survives.
    // 2. Any dead cell with three live neighbours becomes a live cell.
    // 3. All other live cells die in the next generation.

    int grow_left = 0;
    int grow_right = 0;
    int grow_top = 0;
    int grow_bottom = 0;

    // Start from -1 since cells can grow beyond current bounds in this time step
    for (int r = -1; r <= (int) self->rows; ++r) {
        for (int c = -1; c <= (int) self-> cols; ++c) {

            // Current state of the cell
            unsigned char cell = 0;
            if (isWithinDomain(self, c, r))
                cell = *worldCell(self, c, r);
 
            int live_neighbours = 0;
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if (!isWithinDomain(self, c+dc, r+dr) || (dc == 0 && dr == 0))
                        continue;
                    
                    if (*worldCell(self, c+dc, r+dr))
                        ++live_neighbours;
                }
            }

            if (!cell && live_neighbours == 3 ||
                (cell && (live_neighbours == 2 || live_neighbours == 3))) {
                unsigned char *cell_next = worldCellNext(self, c, r);
                *cell_next = 1;
                
                if (!grow_left && r == -1)
                    grow_left = 1;
                if (!grow_right && r == self->rows)
                    grow_right = 1;
                if (!grow_top && c == -1)
                    grow_top = 1;
                if (!grow_bottom && c == self->cols)
                    grow_bottom = 1;

            } else {
                unsigned char *cell_next = worldCellNext(self, c, r);
                *cell_next = 0;
            }
        }
    }

    // Increase the size of the domain if necessary
    int increase_size = grow_left || grow_right || grow_top || grow_bottom;
    if (increase_size) {
        self->rows = self->rows + (unsigned int) (grow_left + grow_right);
        self->cols = self->cols + (unsigned int) (grow_top + grow_bottom);
        self->cells = (unsigned char*) realloc(self->cells, sizeof(unsigned char) * self->rows * self->cols);
        if (!self->cells) {
            fprintf(stderr, "world::worldUpdate: Error! Failed to allocate memory to increase cells.\n");
            return 0;            
        }

        if (grow_left)
            --self->tl_cell_pos_x;
        if (grow_top)
            --self->tl_cell_pos_y;
    }

    // Copy the next cells to the current cells
    for (int r = 0; r < self->rows; ++r) {
        for (int c = 0; c < self->cols; ++c) {
            unsigned char *cell = worldCell(self, r, c);
            unsigned char *cell_next = worldCellNext(self, r - grow_top, c - grow_left);
            *cell = *cell_next;
            *cell_next = 0;
        }
    }


    if (increase_size) {
        self->cn_rows = self->cn_rows + (unsigned int) (grow_left + grow_right);
        self->cn_cols = self->cn_cols + (unsigned int) (grow_top + grow_bottom);
        self->cells_next = (unsigned char*) realloc(self->cells_next, sizeof(unsigned char) * self->cn_rows * self->cn_cols);
        if (!self->cells_next) {
            fprintf(stderr, "world::worldUpdate: Error! Failed to allocate memory to increase cells_next.\n");
            return 0;            
        }
    }

    return 1;
}

void worldToggleCell(struct World *self, int cell_pos_x, int cell_pos_y) {
    
    unsigned char *cell = worldCell(self, cell_pos_x, cell_pos_y);
    if (!cell)
        return;

    if (*cell)
        *cell = 0;
    else
        *cell = 1;

    return;

}

unsigned char *worldCell(struct World *self, int cell_pos_x, int cell_pos_y) {
    int c = cell_pos_x;
    int r = cell_pos_y;

    if (!isWithinDomain(self, r, c)) {
        fprintf(stderr, "world::worldCell: Error! Tried to access a cell that does not exist.\n");
        return NULL;
    }

    int i = ((int) self->cols) * r + c;
    return &self->cells[i];
}

unsigned char *worldCellNext(struct World *self, int cell_pos_x, int cell_pos_y) {
    // Cell next has an extra row and column on all sides of the cell matrix.
    // So need to add 1 from the cell pos (which is in the normal cell matrix
    // reference frame) to get the c and r index in the cell_next frame.

    int c = cell_pos_x + 1;
    int r = cell_pos_y + 1;
    
    if (c >= (int) self->cn_cols || r >= (int) self->cn_rows || c < 0 || r < 0) {
        fprintf(stderr, "world::worldCellNext: Error! Tried to access a cell_next that does not exist.\n");
        return NULL;
    }

    int i = ((int) self->cn_cols) * r + c;
    return &self->cells_next[i];

}

int worldLoadFromFile(struct World *self, const char *file_name) {
    fprintf(stderr, "world::worldLoadFromFile: \n");


    char contents[MAX_WORLD_FILE_BYTES];
    unsigned int bytes = readFile(file_name, MAX_WORLD_FILE_BYTES, contents);
    if (!bytes) {
        fprintf(stderr, "world::worldLoadFromFile: Failed to load %s.\n", file_name);
        return 0;
    }

    int is_first_row = 1;
    int cols_per_row = 0;
    int r = 0;
    int c = 0;
    unsigned int cell_index = 0;
    while (cell_index != bytes -1) {
        char cell_char = contents[cell_index];
        ++cell_index;

        unsigned char *cell = worldCell(self, c, r);
        if (!cell) {
            fprintf(stderr, "world::worldLoadFromFile: Error! World file exceeds maximum rows or cols\n");
            return 0;
        }

        if (cell_char == '\n') {
            if (is_first_row) {
                is_first_row = 0;
                cols_per_row = c;
            }

            if (c != cols_per_row) {
                fprintf(stderr, "world::worldLoadFromFile: Error! Unequal length rows\n");
                return 0;
            }

            ++r;
            c = 0;
            continue;

        } else if (cell_char == '1') {
            *cell = 1;
        } else if (cell_char == '0') {
            *cell = 0;
        } else {
            fprintf(stderr, "world::worldLoadFromFile: Error! Invalid char in world file\n");
            return 0;
        }

        ++c;
    }

    return 1;
}

int worldSaveToFile(struct World *self, const char *file_name) {

    return 1;
}

void worldPrint(struct World *self) {
    fprintf(stderr, "world::printWorld: \n");

    for (int r = 0; r < self->rows; ++r) {
        for (int c = 0; c < self->cols; ++c) {
            unsigned char *cell = worldCell(self, c, r);
            if (!cell) {
                fprintf(stderr, "world::worldPrint: Error reading cells at r = %d, c = %d\n", r, c);
            }
            printf("%d ", (int) *cell);
        }
        printf("\n");
    }
}
