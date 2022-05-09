#include "renderer.h"
#include "fileio.h"
#include "matrix.h"
#include "window.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_SHADER_LEN 4096
#define CELL_SPACING 1.0f

// Global GLFW window
extern struct Window window;

/// Returns the handle to the created shader. Returns 0 if failed to create and compile.
/// GLenum = GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
static GLuint createShader(const char *shader_source, GLenum shader_type) {
    unsigned int shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &shader_source, NULL);
    glCompileShader(shader_id);

    int success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
        fprintf(stderr, "Failed to compile shader: \n%s\n", infoLog);
        return 0;
    }

    return shader_id;
}

/// Creates the transform matrices used for rendering objects in the correct location
/// and perspective. Returns 1 if successful, 0 on failure.
static int createTransforms(struct Renderer *self) {

    self->model_matrix_id = glGetUniformLocation(self->program_id, "m");
    self->view_matrix_id = glGetUniformLocation(self->program_id, "v");
    self->projection_matrix_id = glGetUniformLocation(self->program_id, "p");

    float model_matrix[16];
    identityMatrix(model_matrix);

    float view_matrix[16];
    self->eye[0] = 0.0;//10.0f;
    self->eye[1] = 0.0;//-10.0f;
    self->eye[2] = 1.0f;
    float target[] = {self->eye[0], self->eye[1], 0.0f};
    float up[] = {0.0f, 1.0f, 0.0f};
    lookDir(self->eye, target, up, view_matrix);

    float orthographic_matrix[16];
    float aspect = (float) window.size_x / window.size_y;
    orthographicProjection(aspect*-1.0f, aspect*1.0f, -1.0f, 1.0f, 0.0f, 100.0f, orthographic_matrix);

    glUniformMatrix4fv(self->model_matrix_id, 1, GL_FALSE, model_matrix);
    glUniformMatrix4fv(self->view_matrix_id, 1, GL_FALSE, view_matrix);
    glUniformMatrix4fv(self->projection_matrix_id, 1, GL_FALSE, orthographic_matrix);

    return 1;
}

// Creates a shader program given a path to a vertex shader and fragment shader file.
// TODO: add attributes as input parameters
GLuint createShaderProgram(const char *vs_path, const char *fs_path) { 

    // Create the vertex shader
    char vs_shader_source[MAX_SHADER_LEN];
    readFile(vs_path, MAX_SHADER_LEN, vs_shader_source);
    GLuint vs_id = createShader(vs_shader_source, GL_VERTEX_SHADER);
    if (!vs_id) {
        fprintf(stderr, "Failed to create vertex shader: %s\n", vs_path);
        return 0;
    }

    // Create the fragment shader
    char fs_shader_source[MAX_SHADER_LEN];
    readFile(fs_path, MAX_SHADER_LEN, fs_shader_source);
    GLuint fs_id = createShader(fs_shader_source, GL_FRAGMENT_SHADER);
    if (!fs_id) {
        fprintf(stderr, "Failed to create fragment shader: %s\n", fs_path);
        return 0;
    }

    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vs_id);
    glAttachShader(program_id, fs_id);

    // Bind the attributes here when you need to

    glLinkProgram(program_id);
    GLuint success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        // Should really find a way to check the length of the info log
        // glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLogLength);
        char infoLog[512];
        glGetProgramInfoLog(program_id, 512, NULL, infoLog);
        fprintf(stderr, "Failed to link shader: vs = %s, fs = %s)\n", vs_path, fs_path);
        fprintf(stderr, "    %s\n", infoLog);
        return 0;
    }

    glDetachShader(program_id, vs_id);
    glDetachShader(program_id, fs_id);
    glDeleteShader(vs_id);
    glDeleteShader(fs_id);

    return program_id;
}

static void createVertices(struct Renderer *self, enum ColorScheme cs) {

    // Cell vertices in local coords
    float cell_size = 0.49;
    float cell_vertices[] = {
         cell_size,  cell_size,  0.0,
         cell_size, -cell_size,  0.0,
        -cell_size,  cell_size,  0.0,
        -cell_size, -cell_size,  0.0,
    };

    float alive_r[] = {0.278f, 0.753f, 0.7};
    float alive_g[] = {0.510f, 0.004f, 0.7};
    float alive_b[] = {0.000f, 0.882f, 0.7};


    // One color per vertex.
    float alive_cell_color[] = {
        alive_r[cs], alive_g[cs], alive_b[cs],
        alive_r[cs], alive_g[cs], alive_b[cs],
        alive_r[cs], alive_g[cs], alive_b[cs],
        alive_r[cs], alive_g[cs], alive_b[cs]
    };

    float dead_r[] = {0.200f, 0.941f, 0.180f};
    float dead_g[] = {0.200f, 0.914f, 0.180f};
    float dead_b[] = {0.200f, 0.867f, 0.180f};

    float dead_cell_color[] = {
        dead_r[cs], dead_g[cs], dead_b[cs],
        dead_r[cs], dead_g[cs], dead_b[cs],
        dead_r[cs], dead_g[cs], dead_b[cs],
        dead_r[cs], dead_g[cs], dead_b[cs]
    };

    // A little unecessary for a 2D cell since no vertices are shared.
    unsigned int cell_indices[] = {
        0, 1, 2, // Face 1
        1, 3, 2,
    };

    self->num_vertices = sizeof(cell_vertices) / sizeof(float);

    glGenVertexArrays(1, &self->vao);
    glBindVertexArray(self->vao);

    glGenBuffers(1, &self->vbo); 
    glBindBuffer(GL_ARRAY_BUFFER, self->vbo); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(cell_vertices), cell_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &self->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cell_indices), cell_indices, GL_STATIC_DRAW); 

    glGenBuffers(1, &self->cbo_alive);
    glBindBuffer(GL_ARRAY_BUFFER, self->cbo_alive);
    glBufferData(GL_ARRAY_BUFFER, sizeof(alive_cell_color), alive_cell_color, GL_STATIC_DRAW); // could make this a dynamic draw if I want to change the color of the cells??
    
    glGenBuffers(1, &self->cbo_dead);
    glBindBuffer(GL_ARRAY_BUFFER, self->cbo_dead);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dead_cell_color), dead_cell_color, GL_STATIC_DRAW);
}

struct Renderer *rendererCreate(enum ColorScheme cs) {

    struct Renderer *self = malloc(sizeof(struct Renderer));
    if (!self) {
        fprintf(stderr, "Failed to allocate memory for the Renderer.\n");
        return NULL;
    }


    // Create the shader programs
    self->program_id = createShaderProgram("../basic_color.vs", "../basic_color.fs");
    if (!self->program_id) {
        fprintf(stderr, "Failed to create the shader program.\n");
        return NULL;
    }
    glUseProgram(self->program_id);

    float clear_r[] = {0.157f, 0.902f, 0.450f};
    float clear_g[] = {0.290f, 0.282f, 0.450f};
    float clear_b[] = {0.000f, 0.623f, 0.450f};

    glClearColor(clear_r[cs], clear_g[cs], clear_b[cs], 1.0f);


    if (!createTransforms(self)) {
        fprintf(stderr, "Failed to create renderer transforms.\n");
        return NULL;
    }

    createVertices(self, cs);

    return self;
}

void rendererDestroy(struct Renderer *self) {
    if (!self)
        return;

    free(self);
}

static float windowZoom() {
    return window.mouse.scroll * 0.1f;
}

static void setWindowZoom(float zoom) {
    float scroll = zoom * 10.0f;
    if (scroll > SCROLL_MAX)
        scroll = SCROLL_MAX;
    else if (scroll < SCROLL_MIN)
        scroll = SCROLL_MIN;
    window.mouse.scroll = scroll;
}

static float max(float a, float b) {
    if (a > b)
        return a;

    return b;
}

/// Places the center of the screen at the center of the live cells.
/// Sets zoom so that all occupied squares are visible.
void rendererRecenter(struct Renderer *self, struct World *world) {
    if (!self || !world)
        return;

    // Init min and max to their opposites.
    float cell_spacing = CELL_SPACING;
    float max_x = (world->tl_cell_pos_x - 0.5f) * cell_spacing;
    float max_y = (-world->tl_cell_pos_y - 0.5f) * cell_spacing;
    float min_x = (world->cols + world->tl_cell_pos_x + 0.5f) * cell_spacing;
    float min_y = (-world->rows - world->tl_cell_pos_y + 0.5f) * cell_spacing;

    int no_cells_are_live = 1;
    for (int r = 0; r < world->rows; ++r) {
        for (int c = 0; c < world->cols; ++c) {
            unsigned char *cell = worldCell(world, c, r);
            if (!cell) {
                fprintf(stderr, "renderer::rendererRecenter: Cell c=%d, r=%d invalid.\n", c, r);
                continue;
            }

            if (*cell) {
                no_cells_are_live = 0;

                float x = ((int) c + world->tl_cell_pos_x) * cell_spacing;
                float y = -((int) r + world->tl_cell_pos_y) * cell_spacing;
                if (x - 0.5f*cell_spacing < min_x)
                    min_x = x - 0.5f*cell_spacing;
                if (x + 0.5f*cell_spacing > max_x)
                    max_x = x + 0.5f*cell_spacing;
                if (y - 0.5f*cell_spacing < min_y)
                    min_y = y - 0.5f*cell_spacing;
                if (y + 0.5f*cell_spacing > max_y)
                    max_y = y + 0.5f*cell_spacing;
            }
        }
    }

    if (no_cells_are_live)
        return;

    // Set the zoom and eye position so that all occupied squares are visible
    self->eye[0] = (max_x + min_x) * 0.5f;
    self->eye[1] = (max_y + min_y) * 0.5f;

    float top = max_y - self->eye[1];
    float right = max_x - self->eye[0];
    float aspect_inv = ((float) window.size_y) / window.size_x;
    float zoom = max(top, right*aspect_inv);

    setWindowZoom(0.5f*zoom);
}

int rendererGrowWorldToFillView(struct Renderer *self, struct World *world) {

    float aspect = ((float) window.size_x) / window.size_y;
    float top = 2.0f * windowZoom();
    float right = aspect * 2.0f * windowZoom();

    float max_x = self->eye[0] + right;
    float min_x = self->eye[0] - right;
    float max_y = self->eye[1] + top;
    float min_y = self->eye[1] - top;

    float cell_spacing_inv = 1.0f / CELL_SPACING;
    int visible_tl_cell_pos_x = min_x * cell_spacing_inv;
    int visible_tl_cell_pos_y = -max_y * cell_spacing_inv;
    int visible_br_cell_pos_x = (int) max_x * cell_spacing_inv + 1;
    int visible_br_cell_pos_y = (int) -min_y * cell_spacing_inv + 1;

//    fprintf(stderr, "    tl cell pos x= %d\n", world->tl_cell_pos_x);
//    fprintf(stderr, "    tl cell pos y= %d\n", world->tl_cell_pos_y);
//    fprintf(stderr, "    visible tl x = %d\n", visible_tl_cell_pos_x);
//    fprintf(stderr, "    visible tl y = %d\n", visible_tl_cell_pos_y);
//    fprintf(stderr, "    visible br x = %d\n", visible_br_cell_pos_x);
//    fprintf(stderr, "    visible br y = %d\n", visible_br_cell_pos_y);

    float block_cols_inv = 1.0f / world->block_cols;
    float block_rows_inv = 1.0f / world->block_rows;
    float grow_left = ((float) world->tl_cell_pos_x - visible_tl_cell_pos_x) * block_cols_inv; // so if the difference is less than one block size, then no new space is allocated..
    float grow_right = ((float) visible_br_cell_pos_x - world->tl_cell_pos_x - world->cols) * block_cols_inv; // Need to increase by 1 if grow left is at all positive, cant truncate.
    float grow_top = ((float) world->tl_cell_pos_y - visible_tl_cell_pos_y) * block_rows_inv;
    float grow_bottom = ((float) visible_br_cell_pos_y - world->tl_cell_pos_y - world->rows) * block_rows_inv;


    grow_left = ceil(grow_left);
    grow_right = ceil(grow_right);
    grow_top = ceil(grow_top);
    grow_bottom = ceil(grow_bottom);
    
    grow_left = max(grow_left, 0.0f);
    grow_right = max(grow_right, 0.0f);
    grow_top = max(grow_top, 0.0f);
    grow_bottom = max(grow_bottom, 0.0f);

//    fprintf(stderr, "    grow_left   = %d\n", (int) grow_left);
//    fprintf(stderr, "    grow_right  = %d\n", (int) grow_right);
//    fprintf(stderr, "    grow_top    = %d\n", (int) grow_top);
//    fprintf(stderr, "    grow_bottom = %d\n", (int) grow_bottom);
    if (grow_left > 0 || grow_right > 0 || grow_top > 0 || grow_bottom > 0) {

        int prev_tl_cell_pos_x = world->tl_cell_pos_x;
        int prev_tl_cell_pos_y = world->tl_cell_pos_y;
        
        // Backup the existing state of the cells
        for (int r = 0; r < world->cn_rows; ++r) {
            for (int c = 0; c < world->cn_cols; ++c) {
                unsigned char *cell = worldCell(world, c, r);
                unsigned char *cell_next = worldCellNext(world, c, r);
                *cell_next = *cell;
                *cell = 0;
            }
        }

        if (!worldIncreaseCells(world, grow_top, grow_bottom, grow_left, grow_right))
            return 0;

        // TODO: reduce code repeition with world update
        // Copy the next cells to the current cells
        int col_offset = prev_tl_cell_pos_x - world->tl_cell_pos_x;
        int row_offset = prev_tl_cell_pos_y - world->tl_cell_pos_y;

        for (int r = 0; r < world->cn_rows; ++r) {
            for (int c = 0; c < world->cn_cols; ++c) {
                unsigned char *cell = worldCell(world, c + col_offset, r + row_offset);
                unsigned char *cell_next = worldCellNext(world, c, r);
                *cell = *cell_next;
                *cell_next = 0;
            }
        }

        if (!worldIncreaseCellsNext(world))
            return 0;
    }

    return 1;
}

// is_alive = when true, cells are rendered solid.
static void renderCell(struct Renderer *self, float pos[3], int is_alive) {
    float model_matrix[16];
    translation(pos[0], pos[1], pos[2], model_matrix);
    transpose(model_matrix);

    glUniformMatrix4fv(self->model_matrix_id, 1, GL_FALSE, model_matrix);

    GLuint vs_layout_specifier = 0;
    glEnableVertexAttribArray(vs_layout_specifier);
    glBindBuffer(GL_ARRAY_BUFFER, self->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ibo);
    glVertexAttribPointer(vs_layout_specifier, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    vs_layout_specifier = 1;
    glEnableVertexAttribArray(vs_layout_specifier);
    if (is_alive)
        glBindBuffer(GL_ARRAY_BUFFER, self->cbo_alive);
    else
        glBindBuffer(GL_ARRAY_BUFFER, self->cbo_dead);
    glVertexAttribPointer(vs_layout_specifier, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDrawElements(GL_TRIANGLES, self->num_vertices, GL_UNSIGNED_INT, (void*)0);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

}

static void handleMoveCommands(struct Renderer *self) {
    
    float speed = 0.05f;
    float dx = 0.0f;
    float dy = 0.0f;

    if ((window.keyboard.keys[GLFW_KEY_LEFT_SHIFT].pressed || window.keyboard.keys[GLFW_KEY_RIGHT_SHIFT].pressed))
        return;

    if (window.keyboard.keys[GLFW_KEY_W].pressed || window.keyboard.keys[GLFW_KEY_UP].pressed)
        dy += speed;

    if (window.keyboard.keys[GLFW_KEY_S].pressed || window.keyboard.keys[GLFW_KEY_DOWN].pressed)
        dy -= speed;

    if (window.keyboard.keys[GLFW_KEY_A].pressed || window.keyboard.keys[GLFW_KEY_LEFT].pressed)
        dx += speed;

    if (window.keyboard.keys[GLFW_KEY_D].pressed || window.keyboard.keys[GLFW_KEY_RIGHT].pressed)
        dx -= speed;

    if (fabs(dx) > speed*0.1f || fabs(dy) > speed*0.1f) {
        float zoom = windowZoom();
        self->eye[0] = self->eye[0] - (dx * zoom);
        self->eye[1] = self->eye[1] + (dy * zoom);
    }
}

static void handleWorldCommands(struct World *world) {

    // Speedup/slowdown world updates
    if ((window.keyboard.keys[GLFW_KEY_LEFT_SHIFT].pressed || window.keyboard.keys[GLFW_KEY_RIGHT_SHIFT].pressed)) {
        if (window.keyboard.keys[GLFW_KEY_W].pressed || window.keyboard.keys[GLFW_KEY_UP].pressed) {
            world->update_rate.ticks_per_sec += 0.2f;
            if (world->update_rate.ticks_per_sec > 20.0f)
                world->update_rate.ticks_per_sec = 20.0f;
        }

        if (window.keyboard.keys[GLFW_KEY_S].pressed || window.keyboard.keys[GLFW_KEY_DOWN].pressed) {
            world->update_rate.ticks_per_sec -= 0.2f;
            if (world->update_rate.ticks_per_sec < 0.6f)
                world->update_rate.ticks_per_sec = 0.6f;
        }
    }

    // Enter/leave edit mode
    if ((window.keyboard.keys[GLFW_KEY_E].pressed && !window.keyboard.keys[GLFW_KEY_E].held) ||
        (window.keyboard.keys[GLFW_KEY_SPACE].pressed && !window.keyboard.keys[GLFW_KEY_SPACE].held)) {
        if (world->updates_paused)
            world->updates_paused = 0;
        else
            world->updates_paused = 1;

        if (world->edit_mode)
            world->edit_mode = 0;
        else
            world->edit_mode = 1;

        window.keyboard.keys[GLFW_KEY_E].held = 1;
    }


}

static void handleEditCommands(struct World *world, float left, float right, 
                               float bottom, float top, const float eye[3],
                               float cell_spacing, const float top_left_cell[3]) {
    
    // Edit world commands
    if (world->edit_mode && window.mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed &&
        !window.mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].held) {
        
        window.mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].held = 1;

        float x = window.mouse.pos_x;
        float y = window.mouse.pos_y;
        float world_x = eye[0] + 2*(right - left)*(x/window.size_x - 0.5f);
        float world_y = eye[1] + 2*(bottom - top)*(y/window.size_y - 0.5f);

        int c = round(((float) round(world_x) -     top_left_cell[0]) / cell_spacing);
        int r = round((top_left_cell[1] - (float) round(world_y)) / cell_spacing);

        worldToggleCell(world, c, r);        
    }
}

void renderWorld(struct Renderer *self, struct World *world) {
    handleMoveCommands(self);
    handleWorldCommands(world);

    // Is is possible to avoid doing this every loop, and do only when the
    // aspect ratio changes?
    float orthographic_matrix[16];
    float aspect = (float) window.size_x / window.size_y;
    float aspect_inv = 1.0f / aspect;
    float zoom = windowZoom();
    float right = aspect*zoom;
    float left = -right;
    float top = zoom;
    float bottom = -top;
    orthographicProjection(left, right, bottom, top, 0.0f, 1000.0f, orthographic_matrix);
    glUniformMatrix4fv(self->projection_matrix_id, 1, GL_FALSE, orthographic_matrix);

    
    float view_matrix[16];
    float target[] = {self->eye[0], self->eye[1], 0.0f};
    float up[] = {0.0f, 1.0f, 0.0f};
    lookDir(self->eye, target, up, view_matrix);
    glUniformMatrix4fv(self->view_matrix_id, 1, GL_FALSE, view_matrix);

    float cell_spacing = CELL_SPACING;
    float top_left[] = {cell_spacing * (float)world->tl_cell_pos_x, 
                        -cell_spacing * (float)world->tl_cell_pos_y, 
                        0.0f};
    float cell_pos[] = {0.0f, 0.0f, 0.0f};
    handleEditCommands(world, left, right, bottom, top, self->eye, cell_spacing, top_left);

    for (int r = 0; r < world->rows; ++r) {
        for (int c = 0; c < world->cols; ++c) {
            unsigned char *cell = worldCell(world, c, r);
            if (!cell) {
                fprintf(stderr, "renderer::renderWorld: No value at cell (col = %d, row = %d)\n", c, r);
                fprintf(stderr, "    tl_cell_pos_x = %d\n", world->tl_cell_pos_x);
                fprintf(stderr, "    tl_cell_pos_y = %d\n", world->tl_cell_pos_y);
                fprintf(stderr, "    total rows    = %d\n", world->rows);
                fprintf(stderr, "    total cols    = %d\n", world->cols);

                return;
            }
            cell_pos[0] = top_left[0] + cell_spacing * c;
            cell_pos[1] = top_left[1] - cell_spacing * r;
            renderCell(self, cell_pos, *cell);
        }
    }
}

void renderClear(struct Renderer *self) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
