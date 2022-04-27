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

    char *load_file_path = NULL;
    char *save_file_path = NULL;
    if (argc == 2 || argc == 3) {
        load_file_path = argv[1];
        if (argc == 3)
            save_file_path = argv[2];

    } else if (argc > 3) {
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

    if (load_file_path && !worldLoadFromFile(world, load_file_path)) {
        fprintf(stderr, "Failed to load world file %s\n", load_file_path);
        cleanup(renderer, world);
        return 1;
    }

    // Main Loop
    windowLoop(renderer, world);

    if (save_file_path)
        worldSaveToFile(world, save_file_path);

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
    fprintf(stderr, "TODO: Add use instructions...");
    fprintf(stderr, "Should add proper command line parsing, so -h --load and --save work");
}
