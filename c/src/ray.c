#include "headers/ray.h"
#include "graphics.h"
#include "headers/map.h"
#include <math.h>

ray_t rays[NUM_RAYS];

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
  ray_t *ray = &rays[rayId];
  ray->rayAngle = rayAngle;
  normalizeAngle(&ray->rayAngle);
  ray->wasHitVert = false;

  ///////////////////////////////////////
  // horz
  bool foundHorzWallHit = false;
  float xstep, ystep;
  float xintercept, yintercept;
  yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
  yintercept += isRayFacingDown(ray->rayAngle) ? TILE_SIZE : 0;

  xintercept = player.x + (yintercept - player.y) / tan(ray->rayAngle);

  // calc the increment value for steps
  ystep = TILE_SIZE;
  ystep *= isRayFacingUp(ray->rayAngle) ? -1 : 1;

  xstep = TILE_SIZE / tan(ray->rayAngle);
  xstep *= (isRayFacingLeft(ray->rayAngle) && xstep > 0) || (isRayFacingRight(ray->rayAngle) && xstep < 0)
               ? -1
               : 1;

  float nextHorzTouchX = xintercept;
  float nextHorzTouchY = yintercept;

  float horzWallHitX = 0;
  float horzWallHitY = 0;
  int horxzColor = 0;
  while (isInsideMap(nextHorzTouchX, nextHorzTouchY)) {
    float peeky = nextHorzTouchY;
    if (isRayFacingUp(ray->rayAngle)) {
      peeky--;
    }
    if (mapHasWallAt(nextHorzTouchX, peeky)) {
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
  xintercept += isRayFacingRight(ray->rayAngle) ? TILE_SIZE : 0;

  yintercept = player.y + (xintercept - player.x) * tan(ray->rayAngle);

  // calc the increment value for steps
  xstep = TILE_SIZE;
  xstep *= isRayFacingLeft(ray->rayAngle) ? -1 : 1;

  ystep = TILE_SIZE * tan(ray->rayAngle);
  ystep *=
      (isRayFacingUp(ray->rayAngle) && ystep > 0) || (isRayFacingDown(ray->rayAngle) && ystep < 0) ? -1 : 1;

  float nextVertTouchX = xintercept;
  float nextVertTouchY = yintercept;

  float vertWallHitX = 0;
  float vertWallHitY = 0;
  int vertColor = 0;
  while (isInsideMap(nextVertTouchX, nextVertTouchY)) {
    float peekx = nextVertTouchX;
    if (isRayFacingLeft(ray->rayAngle)) {
      peekx -= 1;
    }

    if (mapHasWallAt(peekx, nextVertTouchY)) {
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

void renderRays(void) {
  for (int i = 0; i < NUM_RAYS; i++) {
    ray_t *ray = &rays[i];
    drawLine((player.x) * MINIMAP_SCALE_FACTOR + (player.width * 0.5),
             (player.y) * MINIMAP_SCALE_FACTOR + (player.height * 0.5),
             ray->wallhitx * MINIMAP_SCALE_FACTOR,
             ray->wallhity * MINIMAP_SCALE_FACTOR, 0xFF0000FF);
  }
}

bool isRayFacingDown(float angle) { return angle > 0 && angle < PI; }

bool isRayFacingUp(float angle) { return angle < 0 || angle >= PI; }
bool isRayFacingRight(float angle) {
  return angle < (0.5 * PI) || angle > (1.5 * PI);
}
bool isRayFacingLeft(float angle) {
  return angle > (0.5 * PI) && angle < (1.5 * PI);
}