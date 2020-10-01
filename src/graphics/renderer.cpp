#include "graphics/renderer.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <array>

using namespace std;

#ifdef __APPLE__
#include <SDL_ttf.h>
#elif
#include <SDL2/SDL_ttf.h>
#endif

#include "globals.h"

#include "graphics/color_rgb.h"
#include "graphics/texture.h"
#include "graphics/sprite.h"
#include "graphics/camera.h"
#include "game_objects/player.h"
#include "raycast.h"
#include "world.h"

bool Renderer::setup(int width, int height, vector<Texture> *textures, bool fullscreen) {
  Uint32 flags = SDL_WINDOW_SHOWN;

  if (fullscreen) {
    flags |= SDL_WINDOW_FULLSCREEN;
  }

  this->window = SDL_CreateWindow("nn-engine",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      flags);

  this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  this->screen = SDL_CreateTexture(renderer,
      SDL_PIXELFORMAT_BGRA32,
      SDL_TEXTUREACCESS_TARGET,
      width,
      height);

  this->textures = textures;

  SDL_GL_SetSwapInterval(0);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  font = TTF_OpenFont("./font/font.TTF", 32);

  if (font == NULL) {
    cout << "TTF Font could not be loaded: " << TTF_GetError() << "\n";
    return false;
  }

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

    Uint32 color = textures->at(hit.textureIndex)
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

void Renderer::present(bool debug, int fps) {
  drawBuffer();
  SDL_RenderCopy(renderer, screen, NULL, NULL);

  if (debug) {
    ostringstream fpsString;
    fpsString << "FPS: " << fps;
    SDL_Color color = {50, 205, 50};
    SDL_Surface *fpsText = TTF_RenderText_Blended(font, fpsString.str().c_str(), color);
    SDL_Rect dest;
    dest.x = 0;
    dest.y = 0;
    dest.w = fpsText->w;
    dest.h = fpsText->h;

    SDL_Texture *fpsTexture = SDL_CreateTextureFromSurface(renderer, fpsText);
    SDL_RenderCopy(renderer, fpsTexture, NULL, &dest);
    SDL_DestroyTexture(fpsTexture);
    SDL_FreeSurface(fpsText);
  }

  SDL_RenderPresent(renderer);
}

void Renderer::clear() {
  clearBuffer();
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
}

void Renderer::cleanup() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

void Renderer::drawWorld(Player &player, World &world) {
  // Ceiling and floor
  for(int y = 0; y < SCREEN_HEIGHT; y++) {
    double rayDirX0 = player.camera->dirX - player.camera->planeX;
    double rayDirY0 = player.camera->dirY - player.camera->planeY;
    double rayDirX1 = player.camera->dirX + player.camera->planeX;
    double rayDirY1 = player.camera->dirY + player.camera->planeY;

    // Center of screen
    int p = y - SCREEN_HEIGHT / 2;
    int posZ = SCREEN_HEIGHT / 2;

    double rowDistance = 1.0 * posZ / p;

    double floorStepX = rowDistance * (rayDirX1 - rayDirX0) / SCREEN_WIDTH;
    double floorStepY = rowDistance * (rayDirY1 - rayDirY0) / SCREEN_WIDTH;

    double floorX = player.posX + rowDistance * rayDirX0;
    double floorY = player.posY + rowDistance * rayDirY0;

    for(int x = 0; x < SCREEN_WIDTH; x++) {
      int cellX = (int)floorX;
      int cellY = (int)floorY;

      int tx = (int)(TEXTURE_WIDTH * (floorX - cellX)) & (TEXTURE_WIDTH - 1);
      int ty = (int)(TEXTURE_HEIGHT * (floorY - cellY)) & (TEXTURE_HEIGHT - 1);

      floorX += floorStepX;
      floorY += floorStepY;

      int floorTexture = 3;
      int ceilingTexture = 7;

      Uint32 color = textures->at(floorTexture)
        .getPixels()
        ->at(TEXTURE_HEIGHT * ty + tx);
      color = (color >> 1) & 8355711;
      buffer[y][x] = color;

      color = textures->at(ceilingTexture)
        .getPixels()
        ->at(TEXTURE_HEIGHT * ty + tx);
      color = (color >> 1) & 8355711;
      buffer[SCREEN_HEIGHT - y - 1][x] = color;
    }
  }

  // Wall Rendering
  for(int x = 0; x < SCREEN_WIDTH; x++) {
    double cameraX = player.camera->getCameraX(x);
    double rayDirX = player.camera->getRayDirX(cameraX);
    double rayDirY = player.camera->getRayDirY(cameraX);

    RayCast ray(player.posX, player.posY, rayDirX, rayDirY);

    RayCastHit hit = ray.collideWorld(world);

    zBuffer[x] = hit.perpWallDist;

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

void sortSprites(int *order, double *dist, int amount) {
  std::vector<std::pair<double, int> > sprites(amount);
  for(int i = 0; i < amount; i++) {
    sprites[i].first = dist[i];
    sprites[i].second = order[i];
  }

  std::sort(sprites.begin(), sprites.end());

  for(int i = 0; i < amount; i++) {
    dist[i] = sprites[amount - i - 1].first;
    order[i] = sprites[amount - i - 1].second;
  }
}

double distanceFromPlayer(Player &player, DrawableSprite *sprite) {
  return (player.posX - sprite->x) * (player.posX - sprite->x) +
         (player.posY - sprite->y) * (player.posY - sprite->y);
}

void Renderer::drawSprites(Player &player, std::vector<DrawableSprite *> &sprites) {
  if (sprites.size() <= 0) {
    return;
  }

  int  *spriteOrder = new int[sprites.size()];
  double *spriteDistance = new double[sprites.size()];

  for(size_t i = 0; i < sprites.size(); i++) {
    spriteOrder[i] = i;
    spriteDistance[i] = distanceFromPlayer(player, sprites.at(i));
  }

  sortSprites(spriteOrder, spriteDistance, sprites.size());

  for(int i = 0; i < sprites.size(); i++) {
    DrawableSprite *sprite = sprites.at(spriteOrder[i]);

    double spriteX = sprite->x - player.posX;
    double spriteY = sprite->y - player.posY;

    double invDet = player.camera->getInvDet();

    double transformX = invDet * (player.camera->dirY * spriteX - player.camera->dirX * spriteY);
    double transformY = invDet * (-player.camera->planeY * spriteX + player.camera->planeX * spriteY);

    int spriteScreenX = int((SCREEN_WIDTH / 2) * (1 + transformX / transformY));

    int spriteHeight = abs(int(SCREEN_HEIGHT / transformY));

    int drawStartY = -spriteHeight / 2 + SCREEN_HEIGHT / 2;
    if (drawStartY < 0) {
      drawStartY = 0;
    }

    int drawEndY = spriteHeight / 2 + SCREEN_HEIGHT / 2;

    if (drawEndY >= SCREEN_HEIGHT) {
      drawEndY = SCREEN_HEIGHT - 1;
    }

    int spriteWidth = abs(int(SCREEN_HEIGHT / transformY));

    int drawStartX = -spriteWidth/ 2 + spriteScreenX;
    if (drawStartX < 0) {
      drawStartX = 0;
    }

    int drawEndX = spriteWidth / 2 + spriteScreenX;

    if (drawEndX >= SCREEN_WIDTH) {
      drawEndX = SCREEN_WIDTH - 1;
    }

    for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
      int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * TEXTURE_WIDTH / spriteWidth) / 256;

      if (transformY > 0 && stripe > 0 && stripe < SCREEN_WIDTH && transformY < zBuffer[stripe]) {
        for(int y = drawStartY; y < drawEndY; y++) {
          int d = (y) * 256 - SCREEN_HEIGHT * 128 + spriteHeight * 128;
          int texY = ((d * TEXTURE_HEIGHT) / spriteHeight) / 256;
          Uint32 color = textures->at(sprite->sprite->textureIndex)
            .getPixels()
            ->at(TEXTURE_WIDTH * texY + texX);

          if ((color & 0x00FFFFFF) != 0) {
            buffer[y][stripe] = color;
          }
        }
      }
    }
  }

  delete spriteOrder;
  delete spriteDistance;
}
