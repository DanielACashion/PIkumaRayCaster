#ifndef RAY_H
#define RAY_H
#include <stdbool.h>
#include "defs.h"
#include "player.h"

typedef struct {
  float rayAngle;
  float wallhitx;
  float wallhity;
  bool wasHitVert;
  float distance;
  int wallColor;
} ray_t;

extern ray_t rays[NUM_RAYS];

void castAllRays(void);
void normalizeAngle(float *rayAngle);
float distanceBetweenPoints(float x1, float y1, float x2, float y2);

bool isRayFacingUp(float angle);
bool isRayFacingDown(float angle);
bool isRayFacingRight(float angle);
bool isRayFacingLeft(float angle);

void renderRays(void);

#endif