#include "headers/player.h"

player_t player = {
    .x = WINDOW_WIDTH * 0.5,
    .y = WINDOW_HEIGHT * 0.5,
    .width = 5,
    .height = 5,
    .turnDirection = 0,
    .walkDirection = 0,
    .rotationAngle = PI * 0.5,
    .walkSpeed = 200,
    .turnSpeed = 65 * (PI / 180),
};

void movePlayer(float deltaTime) {
  player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;
  float moveStep = player.walkDirection * player.walkSpeed * deltaTime;
  float newX = player.x + cos(player.rotationAngle) * moveStep;
  if (mapHasWallAt(newX, player.y) == false) {
    player.x = newX;
  }
  float newY = player.y + sin(player.rotationAngle) * moveStep;
  if (mapHasWallAt(player.x, newY) == false) {
    player.y = newY;
  }
}

void renderMapPlayer(void) {
  drawRect(player.x * MINIMAP_SCALE_FACTOR, player.y * MINIMAP_SCALE_FACTOR,
           player.width, player.height, 0xFF0000FF);
  // SDL_RenderFillRect(renderer, &rect);
  // SDL_RenderDrawLine(
  //     renderer, (player.x) * MINIMAP_SCALE_FACTOR,
  //     (player.y) * MINIMAP_SCALE_FACTOR,
  //     (player.x + cos(player.rotationAngle) * 40) * MINIMAP_SCALE_FACTOR,
  //     (player.y + sin(player.rotationAngle) * 40) * MINIMAP_SCALE_FACTOR);
}
