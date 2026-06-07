#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "defs.h"
#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>


bool initializeWindow(void);
void destroyWindow(void);
void clearColorBuffer(color_t color);
void renderColorBuffer(void);
void drawPixel(int x, int y, color_t color);
void drawRect(int x, int y, int width, int height, color_t color);
void drawLine(int x1, int y1, int x2, int y2, color_t color);
void changeColorIntensity(color_t* color, float factor);
#endif