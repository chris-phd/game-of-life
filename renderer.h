#ifndef __GAME_OF_LIFE_RENDERER_H__
#define __GAME_OF_LIFE_RENDERER_H__

#include "world.h"

#include <glad/glad.h>  // OpenGL loading library. Must be included before glfw
#include <GLFW/glfw3.h> // Multiplatform library for OpenGL

struct Renderer {
   
    GLuint program_id;

    unsigned int vao;
    unsigned int vbo;
    unsigned int ibo;
    unsigned int cbo;
    unsigned int num_vertices;

    GLuint model_matrix_id;
    GLuint view_matrix_id;
    GLuint perspective_matrix_id;
};

struct Renderer *rendererCreate();
void rendererDestroy(struct Renderer *self);
void renderWorld(struct Renderer *self, struct World *world);
void renderClear(struct Renderer *self);

#endif // __GAME_OF_LIFE_RENDERER_H__
