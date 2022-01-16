#include "renderer.h"
#include "world.h"
#include "window.h"

#include <stdio.h>

int init(struct Renderer **renderer, struct World **world);
void cleanup(struct Renderer *renderer, struct World *world);
void printUsage();

// Global GLFW window
extern struct Window window;

int main(int argc, char *argv[]) {

    fprintf(stderr, "main: \n");

    char *world_file_path = NULL;
    int load_world_file = 0;
    if (argc == 2) {
        load_world_file = 1;
        world_file_path = argv[1];
        fprintf(stderr, "world file path = %s\n", world_file_path);
    } else if (argc > 2) {
        fprintf(stderr, "To many command line arguments. \n");
        printUsage();
        return 1;
    }

    struct Renderer *renderer;
    struct World *world;
    if (!init(&renderer, &world)) {
        cleanup(renderer, world);
        return 1;
    }

    if (load_world_file && !worldLoadFromFile(world, world_file_path)) {
        fprintf(stderr, "Failed to load world file %s\n", world_file_path);
        cleanup(renderer, world);
        return 1;
    }

    // Main Looo
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

void printUsage() {
    fprintf(stderr, "Call with one argument, the relative path to a world file.\n");
    fprintf(stderr, "Or call with no arguments for an empty world file.\n");
}
