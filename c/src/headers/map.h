#ifndef MAP_H
#define MAP_H
#include "defs.h"
#include "graphics.h"
#include <stdbool.h>


#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20

bool mapHasWallAt(float x, float y);
bool isInsideMap(int x, int y);
int wallColorAt(float x, float y);
void renderMap(void);
#endif