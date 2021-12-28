#include "fileio.h"

#include <stdio.h>

unsigned int readFile(const char *path, unsigned int max_bytes, char *contents) {
    if (max_bytes == 0)
        return 0;

    FILE *fp;
    fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "Failed to open %s\n", path);
        return 0;
    }

    int bytes = 0;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        *(contents+bytes) = (char) c;
        ++bytes;

        if (bytes == max_bytes-1) {
            fprintf(stderr, "Reached max size when reading %s\n", path);
            break;
        }
    }
    *(contents+bytes) = '\0'; // null terminator
    fclose(fp);

    return bytes;
}
