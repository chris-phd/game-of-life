#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "world.h"

int main(void) {

    fprintf(stderr, "test_world: \n");

    struct World *world = worldCreate();


    char world_file_2[] = "../resources/tests/test_world_2.txt";

    if (!worldLoadFromFile(world, world_file_2)) {
        fprintf(stderr, "test_world: worldLoadFromFile  FAILED\n");
        worldDestroy(world);
        return -1;
    }

    worldPrint(world);
    char world_file_1[] = "../resources/tests/test_world_1.txt";
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

    // Test the world resizing in both x and y dir
    char world_file_3[] = "../resources/tests/test_world_3.txt";
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
    if (world->rows != rows_initial + world->block_rows && world->cols != cols_initial + world->block_cols) {
        fprintf(stderr, "test_world: worldUpdate ran but failed to increase world size\n");
        fprintf(stderr, "test_world: worldUpdate test_world_3    FAILED\n");
        worldDestroy(world);
        return -1;
    }

    worldDestroy(world);

#if 0 
    struct World *world2 = worldCreate();
    // Test the world resizing only in the y dir
    char world_file_4[] = "../resources/tests/test_world_4.txt";
    if (!worldLoadFromFile(world2, world_file_4)) {
        fprintf(stderr, "test_world: worldLoadFromFile  FAILED\n");
        worldDestroy(world);
        return -1;
    }
    worldPrint(world2);
    rows_initial = world->rows;
    cols_initial = world->cols;
    
    if (!worldUpdate(world2)) {
        fprintf(stderr, "test_world: worldUpdate    FAILED\n");
        worldDestroy(world2);
        return -1;
    }
    worldPrint(world);
    if (world->rows != rows_initial + world2->block_rows && world->cols != cols_initial) {
        fprintf(stderr, "test_world: worldUpdate ran but failed to increase world size\n");
        fprintf(stderr, "test_world: worldUpdate test_world_4    FAILED\n");
        worldDestroy(world);
        return -1;
    }


    worldDestroy(world2);
#endif

    fprintf(stderr, "test_world: All tests PASSED\n");
    return 0;
}
