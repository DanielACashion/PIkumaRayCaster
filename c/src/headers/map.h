#ifndef MAP_H
#define MAP_H
#include "defs.h"
#include "graphics.h"


bool mapHasWallAt(float x, float y);
int wallColorAt(float x, float y);
void renderMap(void);

#endif