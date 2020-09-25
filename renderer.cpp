#include "renderer.h"

#include <iostream>

using namespace std;

#include "globals.h"
#include "color_rgb.h"
#include "raycast.h"

ColorRGB getColor(int colorIndex, int side) {
  ColorRGB color;
  switch(colorIndex) {
    case 1: color = RGB_Red; break;
    case 2: color = RGB_Green; break;
    case 3: color = RGB_Blue; break;
    case 4: color = RGB_White; break;
    default: color = RGB_Yellow; break;
  }

  if (side == 1) {
    color = color / 2;
  }

  return color;
}

bool Renderer::setup(int width, int height) {
    this->window = SDL_CreateWindow("nn-engine",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH,
                              SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_GL_SetSwapInterval(0);
    
    if (window == NULL) {
      cout << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
      return false;
    } else {
      return true;
    }
}

void Renderer::drawWallSlice(int x, int bottom, int end, int colorIndex, int side) {
  ColorRGB color = getColor(colorIndex, side);
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(renderer, x, bottom, x, end);
}

void Renderer::present() {
  SDL_RenderPresent(renderer);
}

void Renderer::clear() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
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


        drawWallSlice(x, drawStart, drawEnd, world.getMapPoint(hit.mapX, hit.mapY), hit.side);
      }
}
