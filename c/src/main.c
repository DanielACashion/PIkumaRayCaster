#include "headers/defs.h"
#include "headers/graphics.h"
#include "headers/map.h"
#include "headers/textures.h"
#include "headers/ray.h"
#include "headers/player.h"
#include "textures.h"
#include <SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

bool isRunning = false;
int lastFrameTicks = 1;

void setup(void) {
  isRunning = initializeWindow();

  // set texture
  loadWallTextures();
}

void processInput(void) {
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type) {
  case SDL_QUIT:
    isRunning = false;
    return;
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE) {
      isRunning = false;
      return;
    }
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
      player.walkDirection = 1;
    }
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
      player.walkDirection = -1;
    }
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
      player.turnDirection = 1;
    }
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) {
      player.turnDirection = -1;
    }
    break;
  case SDL_KEYUP:
    if (event.key.keysym.sym == SDLK_ESCAPE) {
      isRunning = false;
      return;
    }
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
      player.walkDirection = 0;
    }
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
      player.walkDirection = 0;
    }
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
      player.turnDirection = 0;
    }
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) {
      player.turnDirection = 0;
    }
    break;
  }
}

void update(void) {
  int currentTicks = SDL_GetTicks();
  int timeToWait = FRAME_TIME_LENGTH - (currentTicks - lastFrameTicks);
  if (timeToWait > 0 && timeToWait <= FRAME_TIME_LENGTH) {
    SDL_Delay(timeToWait);
    currentTicks += (timeToWait);
  }
  float deltatime = (currentTicks - lastFrameTicks) / 1000.0f;
  movePlayer(deltatime);
  castAllRays();
  lastFrameTicks = currentTicks;
}

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

void render(void) {
  // TODO:normal player pov
  clearColorBuffer(0x00EE30FF);
  renderWallProjection();

  renderMap();
  renderRays();
  renderPlayer();
  renderColorBuffer();
}

void releaseResources(void) {
  freeWallTextures();
  destroyWindow();
}

int main(int argc, char *argv[]) {
  isRunning = initializeWindow();
  setup();
  puts("Program is running");
  while (isRunning != false) {
    processInput();
    update();
    render();
  }
  releaseResources();
  return 0;
}