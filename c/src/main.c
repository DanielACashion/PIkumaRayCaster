#include "SDL_pixels.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "SDL_video.h"
#include "constants.h"
#include "headers/textures.h"
#include "textures.h"
#include <SDL.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
bool isRunning = false;
int lastFrameTicks = 1;

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

uint32_t *colorBuffer = NULL;
SDL_Texture *colorBufferTexture = NULL;

struct Player {
  float x;
  float y;
  float width;
  float height;
  int turnDirection; //-1 left, 1 right
  int walkDirection; //-1 back, 1 forward
  float rotationAngle;
  float walkSpeed;
  float turnSpeed;
} player;

struct Ray {
  float rayAngle;
  float wallhitx;
  float wallhity;
  bool wasHitVert;
  float distance;
  int wallColor;
} rays[NUM_RAYS];

bool initializeWindow(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error Initializing SDL\n");
    return false;
  }
  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);
  int fullScreenWidth = display_mode.w;
  int fullScreenHeight = display_mode.h;
  window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, fullScreenWidth,
                            fullScreenHeight, SDL_WINDOW_BORDERLESS);
  if (!window) {
    fprintf(stderr, "Error Creating SDL_Window\n");
    return false;
  }
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    fprintf(stderr, "Error Creating SDL_Renderer\n");
    return false;
  }
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  return true;
}

void destroyWindow(void) {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  freeWallTextures();
}

void setup(void) {
  player.x = WINDOW_WIDTH * 0.5;
  player.y = WINDOW_HEIGHT * 0.5;
  player.width = 5;
  player.height = 5;
  player.turnDirection = 0;
  player.walkDirection = 0;
  player.rotationAngle = PI * 0.5;
  player.walkSpeed = 200;
  player.turnSpeed = 65 * (PI / 180);
  colorBuffer = (uint32_t *)malloc(sizeof(uint32_t) * (uint32_t)WINDOW_WIDTH *
                                   (uint32_t)WINDOW_HEIGHT);
  colorBufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                         SDL_TEXTUREACCESS_STREAMING,
                                         WINDOW_WIDTH, WINDOW_HEIGHT);

  if (!colorBuffer || !colorBufferTexture) {
    isRunning = false;
  }

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

int hasWallAt(float x, float y) {
  if (x < 0 || y < 0 || x >= MAP_NUM_COLS * TILE_SIZE ||
      y >= MAP_NUM_ROWS * TILE_SIZE) {
    return true;
  }

  int localX = (int)(x / TILE_SIZE);
  int localY = (int)(y / TILE_SIZE);
  if (map[localY][localX] != 0) {
    return true;
  }
  return false;
}

int wallColorAt(float x, float y) {
  if (x < 0 || y < 0 || x >= MAP_NUM_COLS * TILE_SIZE ||
      y >= MAP_NUM_ROWS * TILE_SIZE) {
    return 1;
  }
  return map[(int)(y / TILE_SIZE)][(int)(x / TILE_SIZE)];
}

void movePlayer(float deltaTime) {
  player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;
  float moveStep = player.walkDirection * player.walkSpeed * deltaTime;
  float newX = player.x + cos(player.rotationAngle) * moveStep;
  if (hasWallAt(newX, player.y) == false) {
    player.x = newX;
  }
  float newY = player.y + sin(player.rotationAngle) * moveStep;
  if (hasWallAt(player.x, newY) == false) {
    player.y = newY;
  }
}

void normalizeAngle(float *rayAngle) {
  *rayAngle = remainder(*rayAngle, TWO_PI);
  if (*rayAngle < 0) {
    *rayAngle += TWO_PI;
  }
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2) {
  return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void castRay(float rayAngle, int rayId) {
  struct Ray *ray = &rays[rayId];
  ray->rayAngle = rayAngle;
  normalizeAngle(&ray->rayAngle);
  bool isRayFacingDown = ray->rayAngle > 0 && ray->rayAngle < PI;
  bool isRayFacingUp = !isRayFacingDown;
  bool isRayFacingRight =
      ray->rayAngle < (0.5 * PI) || ray->rayAngle > (1.5 * PI);
  bool isRayFacingLeft = !isRayFacingRight;
  ray->wasHitVert = false;

  ///////////////////////////////////////
  // horz
  bool foundHorzWallHit = false;
  float xstep, ystep;
  float xintercept, yintercept;
  yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
  yintercept += isRayFacingDown ? TILE_SIZE : 0;

  xintercept = player.x + (yintercept - player.y) / tan(ray->rayAngle);

  // calc the increment value for steps
  ystep = TILE_SIZE;
  ystep *= isRayFacingUp ? -1 : 1;

  xstep = TILE_SIZE / tan(ray->rayAngle);
  xstep *= (isRayFacingLeft && xstep > 0) || (isRayFacingRight && xstep < 0)
               ? -1
               : 1;

  float nextHorzTouchX = xintercept;
  float nextHorzTouchY = yintercept;

  float horzWallHitX = 0;
  float horzWallHitY = 0;
  int horxzColor = 0;
  while (nextHorzTouchX >= 0 && nextHorzTouchX <= (MAP_NUM_COLS * TILE_SIZE) &&
         nextHorzTouchY >= 0 && nextHorzTouchY <= (MAP_NUM_ROWS * TILE_SIZE)) {
    float peeky = nextHorzTouchY;
    if (isRayFacingUp) {
      peeky--;
    }
    if (hasWallAt(nextHorzTouchX, peeky)) {
      //
      foundHorzWallHit = true;
      horzWallHitX = nextHorzTouchX;
      horzWallHitY = nextHorzTouchY;
      horxzColor = wallColorAt(nextHorzTouchX, peeky);
      break;
    }
    nextHorzTouchX += xstep;
    nextHorzTouchY += ystep;
  }
  ////////////////////////////////////////////
  // vertical ray grid intersection
  bool foundVertWallHit = false;
  xintercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
  xintercept += isRayFacingRight ? TILE_SIZE : 0;

  yintercept = player.y + (xintercept - player.x) * tan(ray->rayAngle);

  // calc the increment value for steps
  xstep = TILE_SIZE;
  xstep *= isRayFacingLeft ? -1 : 1;

  ystep = TILE_SIZE * tan(ray->rayAngle);
  ystep *=
      (isRayFacingUp && ystep > 0) || (isRayFacingDown && ystep < 0) ? -1 : 1;

  float nextVertTouchX = xintercept;
  float nextVertTouchY = yintercept;

  float vertWallHitX = 0;
  float vertWallHitY = 0;
  int vertColor = 0;
  while (nextVertTouchX >= 0 && nextVertTouchX <= (MAP_NUM_COLS * TILE_SIZE) &&
         nextVertTouchY >= 0 && nextVertTouchY <= (MAP_NUM_ROWS * TILE_SIZE)) {
    float peekx = nextVertTouchX;
    if (isRayFacingLeft) {
      peekx -= 1;
    }

    if (hasWallAt(peekx, nextVertTouchY)) {
      //
      foundVertWallHit = true;
      vertWallHitX = nextVertTouchX;
      vertWallHitY = nextVertTouchY;
      vertColor = wallColorAt(peekx, nextVertTouchY);
      break;
    }
    nextVertTouchX += xstep;
    nextVertTouchY += ystep;
  }
  // calc both distances
  // choose smaller val
  float horzhitdistance =
      (foundHorzWallHit) ? distanceBetweenPoints(player.x, player.y,
                                                 horzWallHitX, horzWallHitY)
                         : INT_MAX;
  float verthitdistance =
      (foundVertWallHit) ? distanceBetweenPoints(player.x, player.y,
                                                 vertWallHitX, vertWallHitY)
                         : INT_MAX;

  if (horzhitdistance < verthitdistance) {
    ray->distance = horzhitdistance;
    ray->wallhitx = horzWallHitX;
    ray->wallhity = horzWallHitY;
    ray->wallColor = horxzColor;
    return;
  }
  ray->distance = verthitdistance;
  ray->wallhitx = vertWallHitX;
  ray->wallhity = vertWallHitY;
  ray->wallColor = vertColor;
  ray->wasHitVert = true;
}

void castAllRays(void) {
  float projPlane = DIST_TO_PROJ_PLANE;
  for (int strip_id = 0; strip_id < NUM_RAYS; strip_id++) {
    float rayAngle =
        player.rotationAngle + atan((strip_id - NUM_RAYS * 0.5) / projPlane);
    castRay(rayAngle, strip_id);
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

void renderMap(void) {
  for (int i = 0; i < MAP_NUM_ROWS; i++) {
    for (int j = 0; j < MAP_NUM_COLS; j++) {
      int tileX = j * TILE_SIZE;
      int tileY = i * TILE_SIZE;
      int tileColor = map[i][j] != 0 ? 255 : 0;

      SDL_SetRenderDrawColor(renderer, tileColor, tileColor, tileColor, 255);
      SDL_Rect mapTileRect = {
          tileX * MINIMAP_SCALE_FACTOR, tileY * MINIMAP_SCALE_FACTOR,
          TILE_SIZE * MINIMAP_SCALE_FACTOR, TILE_SIZE * MINIMAP_SCALE_FACTOR};
      SDL_RenderFillRect(renderer, &mapTileRect);
    }
  }
}

void renderRays(void) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  for (int i = 0; i < NUM_RAYS; i++) {
    struct Ray *ray = &rays[i];
    SDL_RenderDrawLine(
        renderer, (player.x + (player.width * 0.5)) * MINIMAP_SCALE_FACTOR,
        (player.y + (player.height * 0.5)) * MINIMAP_SCALE_FACTOR,
        ray->wallhitx * MINIMAP_SCALE_FACTOR,
        ray->wallhity * MINIMAP_SCALE_FACTOR);
  }
}

void renderPlayer(void) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
  SDL_Rect rect = {player.x * MINIMAP_SCALE_FACTOR,
                   player.y * MINIMAP_SCALE_FACTOR, player.width,
                   player.height};
  SDL_RenderFillRect(renderer, &rect);
  SDL_RenderDrawLine(
      renderer, (player.x) * MINIMAP_SCALE_FACTOR,
      (player.y) * MINIMAP_SCALE_FACTOR,
      (player.x + cos(player.rotationAngle) * 40) * MINIMAP_SCALE_FACTOR,
      (player.y + sin(player.rotationAngle) * 40) * MINIMAP_SCALE_FACTOR);
}
void clearColorBuffer(uint32_t color) {
  for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
    colorBuffer[i] = color;
  }
}
void renderColorBuffer(void) {
  SDL_UpdateTexture(colorBufferTexture, NULL, colorBuffer,
                    (int)((uint32_t)WINDOW_WIDTH * sizeof(uint32_t)));
  SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
}
void generate3DProject(void) {
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
      colorBuffer[WINDOW_WIDTH * y + i] = 0xFFF2C883;
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
      //
      colorBuffer[WINDOW_WIDTH * y + i] =
          (rays[i].wasHitVert) ? (texelColor * 1) : texelColor;
    }

    for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++) {
      colorBuffer[WINDOW_WIDTH * y + i] = 0xFF6183AE;
    }
  }
}
void render(void) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

  // TODO:normal player pov
  clearColorBuffer(0x00EE30FF);
  generate3DProject();
  renderColorBuffer();

  renderMap();
  renderRays();
  renderPlayer();

  SDL_RenderPresent(renderer);
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
  destroyWindow();
  if (colorBuffer) {
    free(colorBuffer);
  }
  if (colorBufferTexture) {
    SDL_DestroyTexture(colorBufferTexture);
  }
  for (int i = 0; i < NUM_TEXTURES; i++) {
    //  if (textures[0])
  }
  return 0;
}