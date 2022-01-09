#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>

struct Renderer *rendererCreate() {
    fprintf(stderr, "renderer::rendererCreate: \n");

    struct Renderer *self = malloc(sizeof(struct Renderer));
    if (!self) {
        fprintf(stderr, "Failed to allocate memory for the Renderer.\n");
        return NULL;
    }


    // Create the shader programs
    

    glClearColor(0.2, 0.3, 0.3, 1.0);

    return self;
}

void rendererDestroy(struct Renderer *self) {
    fprintf(stderr, "renderer::rendererDestroy: \n");
    if (!self)
        return;

    free(self);
}

void renderWorld(struct Renderer *self, struct World *world) {
    fprintf(stderr, "renderer::renderWorld: \n");

}

void renderClear(struct Renderer *self) {
    fprintf(stderr, "renderer::renderClear: \n");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
