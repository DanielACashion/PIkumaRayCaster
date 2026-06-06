#include "headers/defs.h"
#include "headers/graphics.h"
#include "headers/map.h"
#include "headers/player.h"
#include "headers/ray.h"
#include "headers/textures.h"
#include "headers/wall.h"
#include <SDL.h>
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