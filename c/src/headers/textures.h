#ifndef TEXTURES_H
#define TEXTURES_H
#include "constants.h"
#include "upng.h"
#include <stdint.h>

typedef struct {
  int width;
  int height;
  upng_t*  upng;
  uint32_t *texture_buffer;
} texture_t;

texture_t wallTextures[NUM_TEXTURES];

void loadWallTextures(void);
void freeWallTextures(void);

#endif