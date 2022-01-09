#ifndef __GAME_OF_LIFE_RENDERER_H__
#define __GAME_OF_LIFE_RENDERER_H__

#include "world.h"

struct Renderer {

};

struct Renderer *rendererCreate();
void rendererDestroy(struct Renderer *self);
void renderWorld(struct Renderer *self, struct World *world);
void renderClear(struct Renderer *self);

#endif // __GAME_OF_LIFE_RENDERER_H__
