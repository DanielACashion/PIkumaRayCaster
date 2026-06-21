#ifndef TEXTURES_H
#define TEXTURES_H
#include "defs.h"
#include "upng.h"


upng_t* textures[NUM_TEXTURES];

void loadTextures(void);
void freeTextures(void);

#endif