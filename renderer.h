#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <SDL2/SDL.h>

#include "camera.h"
#include "world.h"
#include "player.h"
#include "globals.h"
#include "texture.h"
#include "raycast.h"

class Renderer {
  Camera *camera;
  World world;
  SDL_Renderer *renderer = NULL;
  SDL_Window *window = NULL;
  SDL_Texture *screen = NULL;
  std::vector<Texture> *textures;

  Uint32 buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

  public:
    Renderer(Camera *camera, World world): camera(camera), world(world) {};
    bool setup(int width, int height, std::vector<Texture> *textures);
    void drawTextureSlice(int x, int bottom, int end, RayCastHit hit);
    void drawWorld(Player &player);
    void drawBuffer();
    void clearBuffer();
    void present();
    void clear();
    void cleanup();
};

#endif
