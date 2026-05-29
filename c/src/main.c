#include "SDL_pixels.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "constants.h"
#include "headers/textures.h"
#include "textures.h"
#include <SDL.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
int isRunning = 0;
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
  int wasHitVert;
  float distance;
  int isRayFacingUp;
  int isRayFacingDown;
  int isRayFacingLeft;
  int isRayFacingRight;
  int wallColor;
} rays[NUM_RAYS];

int initializeWindow(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error Initializing SDL\n");
    return FALSE;
  }
  window =
      SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS);
  if (!window) {
    fprintf(stderr, "Error Creating SDL_Window\n");
    return FALSE;
  }
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    fprintf(stderr, "Error Creating SDL_Renderer\n");
    return FALSE;
  }
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  return TRUE;
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
    isRunning = FALSE;
  }

  // set texture
  loadWallTextures();
}

void processInput(void) {
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type) {
  case SDL_QUIT:
    isRunning = FALSE;
    return;
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE) {
      isRunning = FALSE;
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
      isRunning = FALSE;
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
  if (x < 0 || y < 0 || x >= WINDOW_WIDTH || y >= WINDOW_HEIGHT) {
    return TRUE;
  }

  int localX = (int)(x / TILE_SIZE);
  int localY = (int)(y / TILE_SIZE);
  if (map[localY][localX] != 0) {
    return TRUE;
  }
  return FALSE;
}

int wallColorAt(float x, float y) {
  if (x < 0 || y < 0 || x >= WINDOW_WIDTH || y >= WINDOW_HEIGHT) {
    return 1;
  }
  return map[(int)(y / TILE_SIZE)][(int)(x / TILE_SIZE)];
}

void movePlayer(float deltaTime) {
  player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;
  float moveStep = player.walkDirection * player.walkSpeed * deltaTime;
  float newX = player.x + cos(player.rotationAngle) * moveStep;
  if (hasWallAt(newX, player.y) == FALSE) {
    player.x = newX;
  }
  float newY = player.y + sin(player.rotationAngle) * moveStep;
  if (hasWallAt(player.x, newY) == FALSE) {
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
  ray->isRayFacingDown = ray->rayAngle > 0 && ray->rayAngle < PI;
  ray->isRayFacingUp = !ray->isRayFacingDown;
  ray->isRayFacingRight =
      ray->rayAngle < (0.5 * PI) || ray->rayAngle > (1.5 * PI);
  ray->isRayFacingLeft = !ray->isRayFacingRight;
  ray->wasHitVert = FALSE;

  ///////////////////////////////////////
  // horz
  int foundHorzWallHit = FALSE;
  float xstep, ystep;
  float xintercept, yintercept;
  yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
  yintercept += ray->isRayFacingDown ? TILE_SIZE : 0;

  xintercept = player.x + (yintercept - player.y) / tan(ray->rayAngle);

  // calc the increment value for steps
  ystep = TILE_SIZE;
  ystep *= ray->isRayFacingUp ? -1 : 1;

  xstep = TILE_SIZE / tan(ray->rayAngle);
  xstep *= (ray->isRayFacingLeft && xstep > 0) ||
                   (ray->isRayFacingRight && xstep < 0)
               ? -1
               : 1;

  float nextHorzTouchX = xintercept;
  float nextHorzTouchY = yintercept;

  float horzWallHitX = 0;
  float horzWallHitY = 0;
  int horxzColor = 0;
  while (nextHorzTouchX >= 0 && nextHorzTouchX <= WINDOW_WIDTH &&
         nextHorzTouchY >= 0 && nextHorzTouchY <= WINDOW_HEIGHT) {
    float peeky = nextHorzTouchY;
    if (ray->isRayFacingUp) {
      peeky--;
    }
    if (hasWallAt(nextHorzTouchX, peeky)) {
      //
      foundHorzWallHit = TRUE;
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
  int foundVertWallHit = FALSE;
  xintercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
  xintercept += ray->isRayFacingRight ? TILE_SIZE : 0;

  yintercept = player.y + (xintercept - player.x) * tan(ray->rayAngle);

  // calc the increment value for steps
  xstep = TILE_SIZE;
  xstep *= ray->isRayFacingLeft ? -1 : 1;

  ystep = TILE_SIZE * tan(ray->rayAngle);
  ystep *=
      (ray->isRayFacingUp && ystep > 0) || (ray->isRayFacingDown && ystep < 0)
          ? -1
          : 1;

  float nextVertTouchX = xintercept;
  float nextVertTouchY = yintercept;

  float vertWallHitX = 0;
  float vertWallHitY = 0;
  int vertColor = 0;
  while (nextVertTouchX >= 0 && nextVertTouchX <= WINDOW_WIDTH &&
         nextVertTouchY >= 0 && nextVertTouchY <= WINDOW_HEIGHT) {
    float peekx = nextVertTouchX;
    if (ray->isRayFacingLeft) {
      peekx -= 1;
    }

    if (hasWallAt(peekx, nextVertTouchY)) {
      //
      foundVertWallHit = TRUE;
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
  ray->wasHitVert = TRUE;
}

void castAllRays(void) {
  float projPlane = DIST_TO_PROJ_PLANE;
  for (int strip_id = 0; strip_id < NUM_RAYS; strip_id++) {
    float rayAngle = player.rotationAngle + atan((strip_id - NUM_RAYS * 0.5) / projPlane);
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
    float projectedWallHeight =
        (TILE_SIZE / perpDistance) * DIST_TO_PROJ_PLANE;
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
      textureOffsetY =
          distanceFromTop * ((float)textHeight / wallStripHeight);
      // TEXTURE COLORING
      uint32_t texelColor =
          wallTextures[rays[i].wallColor - 1]
              .texture_buffer[(textWidth * textureOffsetY) +
                              textureOffsetX];
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
  while (isRunning != FALSE) {
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