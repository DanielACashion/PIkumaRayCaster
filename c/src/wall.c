#include "headers/wall.h"
#include "graphics.h"
#include "upng.h"
void renderWallProjection(void) {
  for (int i = 0; i < NUM_RAYS; i++) {
    float perpDistance =
        rays[i].distance * cos(rays[i].rayAngle - player.rotationAngle);
    if (perpDistance < 1) {
      perpDistance = 1;
    }
    float wallHeight = (TILE_SIZE / perpDistance) * DIST_TO_PROJ_PLANE;

    int wallTopY = (WINDOW_HEIGHT * 0.5) - wallHeight * 0.5;
    if (wallTopY < 0) {
      wallTopY = 0;
    }
    int wallBottomY = (WINDOW_HEIGHT * 0.5) + wallHeight * 0.5;
    if (wallBottomY > WINDOW_HEIGHT) {
      wallBottomY = WINDOW_HEIGHT;
    }
    for (int y = 0; y < wallTopY; y++) {
      drawPixel(i, y, 0xFFF2C883);
    }
    int textureOffsetX, textureOffsetY = 0;
    if (rays[i].wasHitVert) {
      textureOffsetX = (int)rays[i].wallhity % TILE_SIZE;
    } else {
      textureOffsetX = (int)rays[i].wallhitx % TILE_SIZE;
    }
    int textNum = rays[i].wallColor - 1;
    int textHeight = upng_get_height(textures[textNum]);
    int textWidth = upng_get_width(textures[textNum]);
    for (int y = wallTopY; y < wallBottomY; y++) {
      int distanceFromTop = y + (wallHeight * 0.5) - (WINDOW_HEIGHT * 0.5);
      textureOffsetY = distanceFromTop * ((float)textHeight / wallHeight);
      // TEXTURE COLORING
      color_t *wallTextureBuffer =
          (color_t *)upng_get_buffer(textures[textNum]);
      color_t texelColor =
          wallTextureBuffer[(textWidth * textureOffsetY) + textureOffsetX];
      changeColorIntensity(&texelColor, (rays[i].wasHitVert) ? 1 : 0.75);
      drawPixel(i, y, texelColor);
    }

    for (int y = wallBottomY; y < WINDOW_HEIGHT; y++) {
      drawPixel(i, y, 0xFF6183AE);
    }
  }
}