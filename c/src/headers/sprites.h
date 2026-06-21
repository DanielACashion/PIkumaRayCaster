#ifndef SPRITES_H
#define SPRITES_H
#include "graphics.h"
#include "defs.h"
#include "player.h"
#include "textures.h"
typedef struct {
  float x;
  float y;
  float distance;
  float angle;
  int texture;
  bool visible;
} sprite_t;

void detectVisibleSprites(void);
void renderSpritesProjection(void);
void renderMapSprites(void);

#endif