#include "renderer.h"
#include "world.h"
#include "window.h"

#include <stdio.h>

int init(struct Renderer **renderer, struct World **world);
void cleanup(struct Renderer *renderer, struct World *world);

// Global GLFW window
extern struct Window window;

int main(int argc, char *argv[]) {

    fprintf(stderr, "main: \n");

    struct Renderer *renderer;
    struct World *world;
    if (!init(&renderer, &world)) {
        cleanup(renderer, world);
        return 1;
    }

    // Main Loop
    windowLoop(renderer, world);

    cleanup(renderer, world);

    return 0;
}

int init(struct Renderer **renderer, struct World **world) {

    if (!windowInit()) {
        fprintf(stderr, "Failed to setup window.\n");
        return 0;
    }

    *renderer = rendererCreate();
    if (!(*renderer)) {
        fprintf(stderr, "Failed to create the renderer.\n");
        return 0;
    }

    *world = worldCreate();
    if (!(*world)) {
        fprintf(stderr, "Failed to create world.\n");
        return 0;
    }

    return 1;
}

void cleanup(struct Renderer *renderer, struct World *world) {
    worldDestroy(world);
    rendererDestroy(renderer);
    windowCleanup();
}
