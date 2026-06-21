#include "headers/sprites.h"
#include "defs.h"
#include "graphics.h"
#include <math.h>
#include <stdlib.h>
#define NUM_SPRITES 2
static sprite_t sprites[NUM_SPRITES] = {
    {.x = 640, .y = 630, .texture = 9},
    {.x = 550, .y = 630, .texture = 9},
};
static int numOfVisibleSprites = 0;
static sprite_t visibleSprites[NUM_SPRITES];

void detectVisibleSprites(void) {
  numOfVisibleSprites = 0;
  for (int i = 0; i < NUM_SPRITES; i++) {
    float angleSpritePlayer =
        player.rotationAngle -
        atan2(sprites[i].y - player.y, sprites[i].x - player.x);
    if (angleSpritePlayer > PI) {
      angleSpritePlayer -= TWO_PI;
    } else if (angleSpritePlayer < -PI) {
      angleSpritePlayer += TWO_PI;
    }
    angleSpritePlayer = fabs(angleSpritePlayer);
    if (angleSpritePlayer < (FOV_ANGLE * 0.5)) {
      visibleSprites[numOfVisibleSprites] = sprites[i];
      numOfVisibleSprites++;
    }
  }
}

void renderSpritesProjection(void) {
   for (int i = 0; i < numOfVisibleSprites; i++) {
    //
  }
}
void renderMapSprites(void) {
  //
  for (int i = 0; i < numOfVisibleSprites; i++) {
    drawRect(visibleSprites[i].x * MINIMAP_SCALE_FACTOR,
             visibleSprites[i].y * MINIMAP_SCALE_FACTOR, 2, 2, 0xFF00FFFF);
  }
}