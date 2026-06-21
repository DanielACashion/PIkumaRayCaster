#include "textures.h"
#include "SDL_stdinc.h"
#include "headers/textures.h"
#include "headers/upng.h"
#include <stdio.h>

static char *textureFileNames[NUM_TEXTURES] = {
    "./images/redbrick.png",   "./images/purplestone.png",
    "./images/mossystone.png", "./images/graystone.png",
    "./images/colorstone.png", "./images/bluestone.png",
    "./images/wood.png",       "./images/eagle.png",
    "./images/pikuma.png",     "./images/barrel.png",
    "./images/light.png",      "./images/table.png",
    "./images/guard.png",      "./images/armor.png"};

void loadTextures(void) {
  for (int i = 0; i < NUM_TEXTURES; i++) {
    upng_t *upng = upng_new_from_file(textureFileNames[i]);
    if (upng != NULL) {
      upng_decode(upng);
      if (upng_get_error(upng) == UPNG_EOK) {
        textures[i] = upng;
        continue;
      } else {
        printf("Error decoding texture file %s\n", textureFileNames[i]);
        upng_free(upng);
      }

    } else {
      printf("Error loading texture %s\n", textureFileNames[i]);
    }
  }
}

void freeTextures(void) {
  for (int i = 0; i < NUM_TEXTURES; i++) {
    if (textures[i]) {
      upng_free(textures[i]);
    }
  }
}