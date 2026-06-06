#include "headers/wall.h"
void renderWallProjection(void) {
  for (int i = 0; i < NUM_RAYS; i++) {
    float perpDistance =
        rays[i].distance * cos(rays[i].rayAngle - player.rotationAngle);
    if (perpDistance < 1) {
      perpDistance = 1;
    }
    float projectedWallHeight = (TILE_SIZE / perpDistance) * DIST_TO_PROJ_PLANE;
    int wallStripHeight = projectedWallHeight;
    int wallTopPixel = (WINDOW_HEIGHT * 0.5) - wallStripHeight * 0.5;
    if (wallTopPixel < 0) {
      wallTopPixel = 0;
    }
    int wallBottomPixel = (WINDOW_HEIGHT * 0.5) + wallStripHeight * 0.5;
    if (wallBottomPixel > WINDOW_HEIGHT) {
      wallBottomPixel = WINDOW_HEIGHT;
    }
    for (int y = 0; y < wallTopPixel; y++) {
      drawPixel(i, y, 0xFFF2C883);
    }
    int textureOffsetX, textureOffsetY = 0;
    if (rays[i].wasHitVert) {
      textureOffsetX = (int)rays[i].wallhity % TILE_SIZE;
    } else {
      textureOffsetX = (int)rays[i].wallhitx % TILE_SIZE;
    }
    int textHeight = wallTextures[rays[i].wallColor - 1].height;
    int textWidth = wallTextures[rays[i].wallColor - 1].width;
    for (int y = wallTopPixel; y < wallBottomPixel; y++) {
      int distanceFromTop = y + (wallStripHeight * 0.5) - (WINDOW_HEIGHT * 0.5);
      textureOffsetY = distanceFromTop * ((float)textHeight / wallStripHeight);
      // TEXTURE COLORING
      uint32_t texelColor =
          wallTextures[rays[i].wallColor - 1]
              .texture_buffer[(textWidth * textureOffsetY) + textureOffsetX];

      drawPixel(i, y, (rays[i].wasHitVert) ? (texelColor * 1) : texelColor);
    }

    for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++) {
      drawPixel(i, y, 0xFF6183AE);
    }
  }
}