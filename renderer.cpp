#include "renderer.h"

#include <iostream>

using namespace std;

#include "globals.h"
#include "color_rgb.h"

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

        int mapX = player.getMapX();
        int mapY = player.getMapY();;

        double sideDistX;
        double sideDistY;

        double deltaDistX = abs(1 / rayDirX);
        double deltaDistY = abs(1 / rayDirY);

        int stepX;
        int stepY;
        int hit = 0;
        int side;

        if (rayDirX < 0) {
          stepX = -1;
          sideDistX = (player.posX - mapX) * deltaDistX;
        } else {
          stepX = 1;
          sideDistX = (mapX + 1.0 - player.posX) * deltaDistX;
        }

        if (rayDirY < 0) {
          stepY = -1;
          sideDistY = (player.posY - mapY) * deltaDistY;
        } else {
          stepY = 1;
          sideDistY = (mapY + 1.0 - player.posY) * deltaDistY;
        }

        while(hit == 0) {
          if (sideDistX < sideDistY) {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = 0;
          } else {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = 1;
          }

          if (world.getMapPoint(mapX, mapY) > 0) {
            hit = 1;
          }
        }

        double perpWallDist;
        if (side == 0) {
          perpWallDist = (mapX - player.posX + (1 - stepX) / 2) / rayDirX;
        } else {
          perpWallDist = (mapY - player.posY + (1 - stepY) / 2) / rayDirY;
        }

        int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStart < 0) {
          drawStart = 0;
        }

        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawEnd > SCREEN_HEIGHT) {
          drawEnd = SCREEN_HEIGHT - 1;
        }


        drawWallSlice(x, drawStart, drawEnd, world.getMapPoint(mapX, mapY), side);
      }
}
