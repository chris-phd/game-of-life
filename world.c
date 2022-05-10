#include "world.h"
#include "fileio.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_WORLD_FILE_BYTES 16384

struct World *worldCreate() {

    struct World *self = malloc(sizeof(struct World));
    if (!self) {
        fprintf(stderr, "Failed to allocate memory for the world.\n");
        return NULL;
    }
    
    self->block_rows = 16;
    self->block_cols = 16;
    self->update_rate.ticks_per_sec = 12.0f;
    self->update_rate.last_tick = timeNow();
    self->updates_paused = 0;
    self->edit_mode = 0;
    self->tl_cell_pos_x = 0;
    self->tl_cell_pos_y = 0;
    self->rows = self->block_rows;
    self->cols = self->block_cols;
    self->cells = calloc(self->rows * self->cols, sizeof(unsigned char));
    if (!self->cells) {
        fprintf(stderr, "Failed to allocate memory for the world cells.\n");
        free(self);
        return NULL;
    }

    self->cn_rows = self->rows;
    self->cn_cols = self->cols;
    self->cells_next = calloc(self->rows * self->cols, sizeof(unsigned char));
    if (!self->cells_next) {
        fprintf(stderr, "Failed to allocate memory for the world cells_next.\n");
        free(self->cells);
        free(self);
        return NULL;
    }

    return self;
}

void worldDestroy(struct World *self) {
    if (!self)
        return;
    
    free(self->cells);
    free(self->cells_next);
    free(self);
}

static int isWithinDomain(struct World *self, int c, int r) {
    return r >= 0 && c >= 0 && r < (int) self->rows && c < (int) self->cols;
}

/// Grows the size of the cells in the specified directions.
int worldIncreaseCells(struct World *self, int grow_top, int grow_bottom, int grow_left, int grow_right) {
    self->rows = self->rows + (unsigned int) (grow_top + grow_bottom) * self->block_rows;
    self->cols = self->cols + (unsigned int) (grow_left + grow_right) * self->block_cols;
    self->cells = realloc(self->cells, sizeof(unsigned char) * self->rows * self->cols);
    
    if (!self->cells) {
        fprintf(stderr, "world::worldIncreaseCells: Error! Failed to allocate memory to increase cells.\n");
        return 0;            
    }

    for (int i = 0; i < self->rows * self->cols; ++i)
        self->cells[i] = 0;

    if (grow_left)
        self->tl_cell_pos_x = self->tl_cell_pos_x - self->block_cols;
    if (grow_top)
        self->tl_cell_pos_y = self->tl_cell_pos_y - self->block_rows;
}

/// Grows cellsNext to match the size of cells. Call after copying cells next into cells.
int worldIncreaseCellsNext(struct World *self) {
    self->cn_rows = self->rows;
    self->cn_cols = self->cols;
    self->cells_next = realloc(self->cells_next, sizeof(unsigned char) * self->rows * self->cols);
    if (!self->cells_next) {
        fprintf(stderr, "world::worldIncreaseCellsNext: Error! Failed to allocate memory to increase cells_next.\n");
        return 0;            
    }
}

int worldUpdate(struct World *self) {

    if (self->updates_paused)
        return 1;

    // 1. Any live cell with two or three live neighbours survives.
    // 2. Any dead cell with three live neighbours becomes a live cell.
    // 3. All other live cells die in the next generation.

    int grow_left = 0;
    int grow_right = 0;
    int grow_top = 0;
    int grow_bottom = 0;

    for (int r = 0; r < (int) self->rows; ++r) {
        for (int c = 0; c < (int) self->cols; ++c) {

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
                
                if (!grow_top && r == 0)
                    grow_top = 1;
                if (!grow_bottom && r == self->rows-1)
                    grow_bottom = 1;
                if (!grow_left && c == 0)
                    grow_left = 1;
                if (!grow_right && c == self->cols-1)
                    grow_right = 1;

            } else {
                unsigned char *cell_next = worldCellNext(self, c, r);
                *cell_next = 0;
            }
        }
    }

    int prev_tl_cell_pos_x = self->tl_cell_pos_x;
    int prev_tl_cell_pos_y = self->tl_cell_pos_y;

    // Increase the size of the domain if necessary
    int increase_size = grow_left || grow_right || grow_top || grow_bottom;
    if (increase_size) {
        if (!worldIncreaseCells(self, grow_top, grow_bottom, grow_left, grow_right))
            return 0;
    }

    // Copy the next cells to the current cells
    int col_offset = prev_tl_cell_pos_x - self->tl_cell_pos_x;
    int row_offset = prev_tl_cell_pos_y - self->tl_cell_pos_y;

    for (int r = 0; r < self->cn_rows; ++r) {
        for (int c = 0; c < self->cn_cols; ++c) {
            unsigned char *cell = worldCell(self, c + col_offset, r + row_offset);
            unsigned char *cell_next = worldCellNext(self, c, r);
            *cell = *cell_next;
            *cell_next = 0;
        }
    }


    if (increase_size) {
        if (!worldIncreaseCellsNext(self))
            return 0;
    }

    return 1;
}

void worldToggleCell(struct World *self, int c, int r) {
    
    unsigned char *cell = worldCell(self, c, r);
    if (!cell)
        return;

    if (*cell)
        *cell = 0;
    else
        *cell = 1;

    return;

}

unsigned char *worldCell(struct World *self, int c, int r) {
    if (!isWithinDomain(self, c, r)) {
        fprintf(stderr, "world::worldCell: Error! Tried to access a cell that does not exist.\n");
        fprintf(stderr, "    (c = %d, r = %d), (total cols = %d, total rows = %d)\n", c, r, self->cols, self->rows);
        return NULL;
    }

    int i = ((int) self->cols) * r + c;
    return &self->cells[i];
}

unsigned char *worldCellNext(struct World *self, int c, int r) {
    if (!isWithinDomain(self, c, r)) {
        fprintf(stderr, "world::worldCellNext: Error! Tried to access a cell that does not exist.\n");
        return NULL;
    }

    int i = ((int) self->cn_cols) * r + c;
    return &self->cells_next[i];
}

int worldLoadFromFile(struct World *self, const char *file_name) {

    char contents[MAX_WORLD_FILE_BYTES];
    unsigned int bytes = readFile(file_name, MAX_WORLD_FILE_BYTES, contents);
    if (!bytes) {
        fprintf(stderr, "world::worldLoadFromFile: Failed to load %s.\n", file_name);
        return 0;
    }

    // Determine the size of the world in the file.
    unsigned int cell_index = 0;
    int is_first_row = 1;
    int num_rows = 1; // The last row may not have a new line character
    int num_cols = 0;
    while (cell_index != bytes -1) {
        char cell_char = contents[cell_index];
        ++cell_index;

        if (cell_char == '\n') {
            if (is_first_row)
                is_first_row = 0;
            
            ++num_rows;
        }
        if (is_first_row)
            ++num_cols;
    }

    // Increase the size of the world if necessary. Deliberate truncate.
    int grow_right = ceil((num_cols - (float) self->cols) / (float) self->block_cols);
    int grow_bottom = ceil((num_rows - (float) self->rows) / (float) self->block_rows);
    if (grow_right > 0 || grow_bottom > 0) {
        if (grow_right < 0)
            grow_right = 0;
        if (grow_bottom < 0)
            grow_bottom = 0;

        if (!worldIncreaseCells(self, 0, grow_bottom, 0, grow_right))
            return 0;

        if (!worldIncreaseCellsNext(self))
            return 0;
    }

    // Copy the state of the world into memory.
    int cols_per_row = 0;
    int r = 0;
    int c = 0;
    is_first_row = 1;
    cell_index = 0;
    while (cell_index != bytes -1) {
        char cell_char = contents[cell_index];
        ++cell_index;

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

        } else {

            unsigned char *cell = worldCell(self, c, r);
            if (!cell) {
                fprintf(stderr, "world::worldLoadFromFile: Error! World file exceeds maximum rows or cols\n");
                return 0;
            }

            if (cell_char == '1') {
                *cell = 1;
            } else if (cell_char == '0') {
                *cell = 0;
            } else {
                fprintf(stderr, "world::worldLoadFromFile: Error! Invalid char in world file\n");
                return 0;
            }

        } 
        ++c;
    }

    return 1;
}

int worldSaveToFile(struct World *self, const char *file_name) {

    // +1 for the new line characters at the end of each row
    unsigned int size_in_bytes = (self->rows + 1) * self->cols;
    if (size_in_bytes > 10e6) {
        fprintf(stderr, "world::worldSaveToFile: World size exceeds file limit.");
        return 0;
    }

    char *file_contents = calloc(size_in_bytes, sizeof(char));
    if (!file_contents) {
        fprintf(stderr, "world::worldSaveToFile: Failed to create file_contents");
        return 0;
    }

    int fc_inx = 0;
    for (int r = 0; r < self->rows; ++r) {
        for (int c = 0; c < self->cols; ++c) {
            unsigned char *cell = worldCell(self, c, r);
            if (!cell) {
                fprintf(stderr, "world::worldSaveToFile: Error reading cells at r = %d, c = %d\n", r, c);
                ++fc_inx;
                continue;
            }
            if (*cell)
                file_contents[fc_inx] = '1';
            else
                file_contents[fc_inx] = '0';
            
            ++fc_inx;
        }

        file_contents[fc_inx] = '\n';
        ++fc_inx;
    }

    int success = saveFile(file_name, size_in_bytes, file_contents);    
    free(file_contents);

    return success;
}

void worldPrint(struct World *self) {
    for (int r = 0; r < self->rows; ++r) {
        for (int c = 0; c < self->cols; ++c) {
            unsigned char *cell = worldCell(self, c, r);
            if (!cell) {
                fprintf(stderr, "world::worldPrint: Error reading cells at r = %d, c = %d\n", r, c);
                continue;
            }
            printf("%d ", (int) *cell);
        }
        printf("\n");
    }
}
