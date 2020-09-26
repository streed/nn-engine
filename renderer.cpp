#include "renderer.h"

#include <iostream>
#include <vector>

using namespace std;

#include "globals.h"
#include "color_rgb.h"
#include "raycast.h"
#include "texture.h"

bool Renderer::setup(int width, int height, vector<Texture> *textures) {
    this->window = SDL_CreateWindow("nn-engine",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH,
                              SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    this->screen = SDL_CreateTexture(renderer,
                                     SDL_PIXELFORMAT_RGBA32,
                                     SDL_TEXTUREACCESS_TARGET,
                                     width,
                                     height);

    this->textures = textures;

    SDL_GL_SetSwapInterval(0);
    
    if (window == NULL) {
      cout << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
      return false;
    } else {
      return true;
    }
}

void Renderer::drawTextureSlice(int x, int bottom, int end, RayCastHit hit) {
  int lineHeight = (int)(SCREEN_HEIGHT / hit.perpWallDist);
  double step = 1.0 * TEXTURE_WIDTH / lineHeight;
  double texPos = (bottom - SCREEN_HEIGHT / 2 + lineHeight / 2) * step;
  for (int y = bottom; y < end; y++) {
    int texY = (int)texPos & (TEXTURE_HEIGHT - 1);
    texPos += step;

    unsigned int color = textures->at(hit.textureIndex)
                                 .getPixels()
                                 ->at(TEXTURE_HEIGHT * texY + hit.texX);

    if (hit.side == 1) {
      color = (color >> 1) & 8355711;
    }

    buffer[y][x] = color;
  }
}

void Renderer::drawBuffer() {
  SDL_UpdateTexture(screen, NULL, (const void *)&buffer, SCREEN_WIDTH * sizeof(Uint32));
}

void Renderer::clearBuffer() {
  for(int y = 0; y < SCREEN_HEIGHT; y++) {
    for(int x = 0; x < SCREEN_WIDTH; x++) {
      buffer[y][x] = 0;
    }
  }
}

void Renderer::present() {
  drawBuffer();
  SDL_RenderCopy(renderer, screen, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void Renderer::clear() {
  clearBuffer();
  SDL_RenderClear(renderer);
}

void Renderer::cleanup() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

void Renderer::drawWorld(Player &player) {
  for(int x = 0; x < SCREEN_WIDTH; x++) {
    double cameraX = player.camera->getCameraX(x);
    double rayDirX = player.camera->getRayDirX(cameraX);
    double rayDirY = player.camera->getRayDirY(cameraX);

    RayCast ray(player.posX, player.posY, rayDirX, rayDirY);

    RayCastHit hit = ray.collideWorld(world);

    int lineHeight = (int)(SCREEN_HEIGHT / hit.perpWallDist);
    int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
    if (drawStart < 0) {
      drawStart = 0;
    }

    int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
    if (drawEnd > SCREEN_HEIGHT) {
      drawEnd = SCREEN_HEIGHT - 1;
    }

    drawTextureSlice(x, drawStart, drawEnd, hit);
  }
}
