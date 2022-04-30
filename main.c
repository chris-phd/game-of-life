#include "renderer.h"
#include "world.h"
#include "window.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int init(struct Renderer **renderer, struct World **world);
void cleanup(struct Renderer *renderer, struct World *world);
void printUsage();

// Global GLFW window
extern struct Window window;

int main(int argc, char *argv[]) {

    fprintf(stderr, "main: \n");

    int load_file = 0;
    char load_file_path[256];
    int save_file = 0;
    char save_file_path[256];
    int colour_scheme = 0; // 0 = terminal, 1 = vapourwave, 2 = technicolor

    int opt;
    while ((opt = getopt(argc, argv, "l:s:c:")) != -1) {
        switch (opt) {
            case 'l':
                load_file = 1;
                strcpy(load_file_path, optarg);
                break;
            case 's':
                save_file = 1;
                strcpy(save_file_path, optarg);
                break;
            case 'c':
                if (strcmp(optarg, "terminal") == 0)
                    colour_scheme = 0;
                else if (strcmp(optarg, "vaporwave") == 0)
                    colour_scheme = 1;
                else if (strcmp(optarg, "technicolor") == 0)
                    colour_scheme = 2;

                break;
            case ':':
                fprintf(stderr, "Option needs a value\n");
                printUsage();
                return 1;
                break;
            default:
                fprintf(stderr, "Unrecognised command line option.\n");
                printUsage();
                return 1;
        }
    }

    struct Renderer *renderer;
    struct World *world;
    if (!init(&renderer, &world)) {
        cleanup(renderer, world);
        return 1;
    }

    if (load_file && !worldLoadFromFile(world, load_file_path)) {
        fprintf(stderr, "Failed to load world file %s\n", load_file_path);
        cleanup(renderer, world);
        return 1;
    }

    // Main Loop
    windowLoop(renderer, world);

    if (save_file)
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
    fprintf(stderr, "./game_of_life -l load_file_path -s save_file_path -c [terminal, vaporwave, technicolor]");
}
