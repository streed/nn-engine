#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>

#include "camera.h"
#include "world.h"
#include "player.h"

class Renderer {
  Camera *camera;
  World world;
  SDL_Renderer *renderer;
  SDL_Window *window;
  public:
    Renderer(Camera *camera, World world): camera(camera), world(world) {};
    bool setup(int width, int height);
    void drawWallSlice(int x, int bottom, int end, int colorIndex, int side);
    void drawWorld(Player &player);
    void present();
    void clear();
    void cleanup();
};

#endif
