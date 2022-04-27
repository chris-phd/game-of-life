#ifndef __GAME_OF_LIFE_FILEIO_H__
#define __GAME_OF_LIFE_FILEIO_H__

unsigned int readFile(const char *path, unsigned int max_bytes, char *contents);
int saveFile(const char *path, unsigned int size_in_bytes, char *contents);

#endif // __GAME_OF_LIFE_FILEIO_H__
