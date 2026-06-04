#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <stdbool.h>
#include <stdint.h>
#include "defs.h"
#include <SDL.h>
#include <stdio.h>

bool initializeWindow(void);
void destroyWindow(void);
void clearColorBuffer(uint32_t color);
void renderColorBuffer(void);
void drawPixel(int x, int y, uint32_t color);
void drawRect(int x, int y , int width, int height, uint32_t color);
#endif