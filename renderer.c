#include "renderer.h"
#include "fileio.h"
#include "matrix.h"
#include "window.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_SHADER_LEN 4096

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
    fprintf(stderr, "renderer::createTransforms: \n");

    self->model_matrix_id = glGetUniformLocation(self->program_id, "m");
    self->view_matrix_id = glGetUniformLocation(self->program_id, "v");
    self->projection_matrix_id = glGetUniformLocation(self->program_id, "p");

    float model_matrix[16];
    identityMatrix(model_matrix);

    float view_matrix[16];
    self->eye[0] = 10.0f;
    self->eye[1] = -10.0f;
    self->eye[2] = 1.0f;
    float target[] = {0.0f, 0.0f, 0.0f};
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

static void createVertices(struct Renderer *self) {

    // Cell vertices in local coords
    float cell_size = 0.49;
    float cell_vertices[] = {
         cell_size,  cell_size,  0.0,
         cell_size, -cell_size,  0.0,
        -cell_size,  cell_size,  0.0,
        -cell_size, -cell_size,  0.0,
    };

    // One color per vertex. (generated randomly for now)
    float alive_cell_color[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f
    };

    float dead_cell_color[] = {
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f
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

struct Renderer *rendererCreate() {
    fprintf(stderr, "renderer::rendererCreate: \n");

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

    glClearColor(0.2, 0.3, 0.3, 1.0);


    if (!createTransforms(self)) {
        fprintf(stderr, "Failed to create renderer transforms.\n");
        return NULL;
    }

    createVertices(self);

    return self;
}

void rendererDestroy(struct Renderer *self) {
    fprintf(stderr, "renderer::rendererDestroy: \n");
    if (!self)
        return;

    free(self);
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

static float windowZoom() {
    return window.mouse.scroll * 0.1f;
}

static void handleMoveCommands(struct Renderer *self) {
    
    float speed = 0.05f;
    float dx = 0.0f;
    float dy = 0.0f;

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

void renderWorld(struct Renderer *self, struct World *world) {
    handleMoveCommands(self);


    // Is is possible to avoid doing this every loop, and do only when the
    // aspect ratio changes?
    float orthographic_matrix[16];
    float aspect = (float) window.size_x / window.size_y;
    float aspect_inv = 1.0f / aspect;
    float zoom = windowZoom();
    orthographicProjection(aspect*zoom*-1.0f, aspect*zoom*1.0f, -zoom*1.0f, 
                           zoom*1.0f, 0.0f, 1000.0f, orthographic_matrix);
    glUniformMatrix4fv(self->projection_matrix_id, 1, GL_FALSE, orthographic_matrix);

    
    float view_matrix[16];
    float target[] = {self->eye[0], self->eye[1], 0.0f};
    float up[] = {0.0f, 1.0f, 0.0f};
    lookDir(self->eye, target, up, view_matrix);
    glUniformMatrix4fv(self->view_matrix_id, 1, GL_FALSE, view_matrix);

    float cell_spacing = 1.0f;
    float top_left[] = {cell_spacing * (float)world->tl_cell_pos_x, 
                        cell_spacing * (float)world->tl_cell_pos_y, 
                        0.0f};
    float cell_pos[] = {0.0f, 0.0f, 0.0f};
    for (int r = 0; r < world->rows; ++r) {
        for (int c = 0; c < world->cols; ++c) {
            unsigned char *cell = worldCell(world, c, r);
            if (!cell) {
                fprintf(stderr, "renderer::renderWorld: No value at cell (%d %d)\n", c, r);
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
