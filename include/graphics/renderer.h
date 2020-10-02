#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#ifdef __APPLE__
#include <SDL.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif

#include "graphics/texture.h"
#include "world.h"

class Camera;
class DrawableSprite;
class Player;
class RayCastHit;
class Sprite;

#include "globals.h"

class Renderer {
  SDL_Renderer *renderer = NULL;
  SDL_Window *window = NULL;
  SDL_Texture *screen = NULL;
  TTF_Font *font = NULL;
  std::vector<Texture> *textures;

  Uint32 buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
  double zBuffer[SCREEN_WIDTH];

  public:
    bool setup(int width, int height, std::vector<Texture> *textures, bool fullscreen);
    void drawTextureSlice(int x, int bottom, int end, RayCastHit hit);
    void drawWorld(Player &player, World &world);
    void drawSprites(Player &player, std::vector<DrawableSprite *> &sprites);
    void drawBuffer();
    void clearBuffer();
    void present(bool debug, int fps);
    void clear();
    void cleanup();
};

#endif