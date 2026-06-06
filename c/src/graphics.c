
#include "graphics.h"
#include "textures.h"
#include <stdint.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *colorBufferTexture = NULL;

static uint32_t *colorBuffer = NULL;

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
  colorBuffer = (uint32_t *)malloc(sizeof(uint32_t) * (uint32_t)WINDOW_WIDTH *
                                   (uint32_t)WINDOW_HEIGHT);
  colorBufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                         SDL_TEXTUREACCESS_STREAMING,
                                         WINDOW_WIDTH, WINDOW_HEIGHT);

  if (!colorBuffer || !colorBufferTexture) {
    return false;
  }
  return true;
}

void clearColorBuffer(uint32_t color) {
  for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
    colorBuffer[i] = color;
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
}

void renderColorBuffer(void) {
  SDL_UpdateTexture(colorBufferTexture, NULL, colorBuffer,
                    (int)((uint32_t)WINDOW_WIDTH * sizeof(uint32_t)));
  SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void destroyWindow(void) {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  if (colorBuffer) {
    free(colorBuffer);
  }
  if (colorBufferTexture) {
    SDL_DestroyTexture(colorBufferTexture);
  }
}
void drawPixel(int x, int y, uint32_t color) {
  colorBuffer[(y * WINDOW_WIDTH) + x] = color;
}

void drawRect(int x, int y, int width, int height, uint32_t color) {
  int minx = (x < 0) ? 0 : x;
  int miny = (y < 0) ? 0 : y;
  for (int i = miny; i < y + height; i++) {
    if (i > WINDOW_HEIGHT) {
      continue;
    }
    for (int j = minx; j < x + width; j++) {
      if (j > WINDOW_WIDTH) {
        continue;
      }
      drawPixel(j, i, color);
    }
  }
}

void drawLine(int x0, int y0, int x1, int y1, uint32_t color) {
  // draw left to right
  int delta_x = (x1 - x0);
  int delta_y = (y1 - y0);

  int side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);
  if (side_length == 0) {
    side_length = 1;
  }

  float x_inc = delta_x / (float)side_length;
  float y_inc = delta_y / (float)side_length;
  int max_x = x1, min_x = x0, max_y = y1, min_y = y0;
  if (x1 < x0) {
    max_x = x0;
    min_x = x1;
  }
  if (y1 < y0) {
    max_y = y0;
    min_y = y1;
  }
  float x = (float)x0;
  float y = (float)y0;

  for (int i = 0; i <= side_length; i++) {
    drawPixel((int)round(x), (int)round(y), color);
    x += x_inc;
    y += y_inc;
  }
}