#include "textures.h"
#include "SDL_stdinc.h"
#include "headers/textures.h"
#include "headers/upng.h"
#include <stdint.h>

static char *textureFileNames[NUM_TEXTURES] = {
    "./images/redbrick.png",   "./images/purplestone.png",
    "./images/mossystone.png", "./images/graystone.png",
    "./images/colorstone.png", "./images/bluestone.png",
    "./images/wood.png",       "./images/eagle.png"};

void loadWallTextures(void) {
  for (int i = 0; i < NUM_TEXTURES; i++) {
    upng_t *upng;
    upng = upng_new_from_file(textureFileNames[i]);
    if (upng != NULL) {
      upng_decode(upng);
      if (upng_get_error(upng) == UPNG_EOK) {
        wallTextures[i].upng = upng;
        wallTextures[i].height = upng_get_width(upng);
        wallTextures[i].width = upng_get_height(upng);
        wallTextures[i].texture_buffer = (uint32_t *)upng_get_buffer(upng);
        continue;
      }
      wallTextures[i].width = 0;
      wallTextures[i].height = 0;
      wallTextures[i].upng = NULL;
      wallTextures[i].texture_buffer = NULL;
      upng_free(upng);
    }
  }
}

void freeWallTextures(void) {
  for (int i = 0; i < NUM_TEXTURES; i++) {
    if (wallTextures[i].texture_buffer) {
      wallTextures[i].texture_buffer = NULL;
      wallTextures[i].width = 0;
      wallTextures[i].height = 0;
      upng_free(wallTextures[i].upng);
    }
  }
}