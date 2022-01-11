#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "world.h"

int main(void) {

    fprintf(stderr, "test_world: \n");

    struct World *world = worldCreate();


    char world_file_2[] = "../resources/test_world_2.txt";

    if (!worldLoadFromFile(world, world_file_2)) {
        fprintf(stderr, "test_world: worldLoadFromFile  FAILED\n");
        worldDestroy(world);
        return -1;
    }

    worldPrint(world);
    char world_file_1[] = "../resources/test_world_1.txt";
    if (!worldLoadFromFile(world, world_file_1)) {
        fprintf(stderr, "test_world: worldLoadFromFile  FAILED\n");
        worldDestroy(world);
        return -1;
    }

    worldPrint(world);
    if (!worldUpdate(world)) {
        fprintf(stderr, "test_world: worldUpdate    FAILED\n");
        worldDestroy(world);
        return -1;
    }
    
    worldPrint(world);
    if (!*worldCell(world, 1, 2) && !*worldCell(world, 2, 2) && !*worldCell(world, 3, 2)) {
        fprintf(stderr, "test_world: worldUpdate ran but updated cells incorrectly.\n");
        fprintf(stderr, "test_world: worldUpdate     FAILED\n");
        worldDestroy(world);
        return -1;
    }

    char world_file_3[] = "../resources/test_world_3.txt";
    if (!worldLoadFromFile(world, world_file_3)) {
        fprintf(stderr, "test_world: worldLoadFromFile  FAILED\n");
        worldDestroy(world);
        return -1;
    }
    worldPrint(world);
    unsigned int rows_initial = world->rows;
    unsigned int cols_initial = world->cols;
    
    if (!worldUpdate(world)) {
        fprintf(stderr, "test_world: worldUpdate    FAILED\n");
        worldDestroy(world);
        return -1;
    }
    worldPrint(world);
    if (world->rows != rows_initial + 1 && world->cols != cols_initial + 1) {
        fprintf(stderr, "test_world: worldUpdate ran but failed to increase world size\n");
        fprintf(stderr, "test_world: worldUpdate    FAILED\n");
        worldDestroy(world);
        return -1;
    }
    
    worldDestroy(world);

    // Should test a drifer before to see what happens when I need to reallocate on 
    // every step

    fprintf(stderr, "test_world: All tests PASSED\n");
    return 0;
}
