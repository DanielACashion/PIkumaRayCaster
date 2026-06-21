#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>
#define PI 3.14159265
#define TWO_PI 6.28318530

#define TILE_SIZE 64

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 800

#define FOV_ANGLE (60 * PI / 180)
#define NUM_RAYS (WINDOW_WIDTH)

#define FPS 120
#define FRAME_TIME_LENGTH (1000 / FPS)

#define MINIMAP_SCALE_FACTOR 0.125

#define NUM_TEXTURES 14

#define DIST_TO_PROJ_PLANE  (WINDOW_WIDTH * 0.5) / tan(FOV_ANGLE * 0.5)

typedef uint32_t color_t;

#endif