#include "systems/render_system.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <array>

using namespace std;

#ifdef __APPLE__
#include <SDL_ttf.h>
#else
#include <SDL2/SDL_ttf.h>
#endif

#include "globals.h"

#include "engine/config.h"
#include "engine/engine.h"
#include "graphics/color_rgb.h"
#include "graphics/texture.h"
#include "graphics/camera.h"
#include "coordinator.h"
#include "components.h"
#include "raycast.h"
#include "world.h"

namespace NN::Systems::Graphics {
  RenderSystem::RenderSystem() {}

  RenderSystem::RenderSystem(NN::Config *config): config(config) {}

  bool RenderSystem::setup(NN::Config *newConfig) {
    config = newConfig;

    Uint32 flags = SDL_WINDOW_SHOWN;

    if (config->getFullscreen()) {
      flags |= SDL_WINDOW_FULLSCREEN;
    }

    this->window = SDL_CreateWindow("nn-engine",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        config->getScreenWidth(),
        config->getScreenHeight(),
        flags);

    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    this->screen = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_BGRA32,
        SDL_TEXTUREACCESS_TARGET,
        config->getScreenWidth(),
        config->getScreenHeight());

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

  void RenderSystem::update(NN::Engine &engine, double frameTime) {
    NN::Coordinator *coordinator = engine.getCoordinator();
    Components::Position player = coordinator->getComponent<Components::Position>(engine.getCurrentPlayer());
    Components::Camera camera = coordinator->getComponent<Components::Camera>(engine.getCurrentPlayer());

    drawWorld(player, camera, engine.getWorld(), engine.getConfig()->getTextures());
  }


  void RenderSystem::drawWorld(Components::Position &player,
                               Components::Camera &camera,
                               World *world,
                               std::vector<NN::Graphics::Texture> *textures) {
    // Ceiling and floor
    for(int y = 0; y < config->getScreenHeight(); y++) {
      double rayDirX0 = camera.dirX - camera.planeX;
      double rayDirY0 = camera.dirY - camera.planeY;
      double rayDirX1 = camera.dirX + camera.planeX;
      double rayDirY1 = camera.dirY + camera.planeY;

      // Center of screen
      int p = y - config->getScreenHeight() / 2;
      int posZ = config->getScreenHeight() / 2;

      double rowDistance = 1.0 * posZ / p;

      double floorStepX = rowDistance * (rayDirX1 - rayDirX0) / config->getScreenWidth();
      double floorStepY = rowDistance * (rayDirY1 - rayDirY0) / config->getScreenWidth();

      double floorX = player.posX + rowDistance * rayDirX0;
      double floorY = player.posY + rowDistance * rayDirY0;

      for(int x = 0; x < config->getScreenWidth(); x++) {
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
        buffer[config->getScreenHeight() - y - 1][x] = color;
      }
    }

    // Wall Rendering
    for(int x = 0; x < config->getScreenWidth(); x++) {
      double cameraX = camera.getCameraX(x, config->getScreenWidth());
      double rayDirX = camera.getRayDirX(cameraX);
      double rayDirY = camera.getRayDirY(cameraX);

      Utils::RayCast ray(player.posX, player.posY, rayDirX, rayDirY);

      Utils::RayCastHit hit = ray.collideWorld(world);

      zBuffer[x] = hit.perpWallDist;

      int lineHeight = (int)(config->getScreenHeight() / hit.perpWallDist);
      int drawStart = -lineHeight / 2 + config->getScreenHeight() / 2;
      if (drawStart < 0) {
        drawStart = 0;
      }

      int drawEnd = lineHeight / 2 + config->getScreenHeight() / 2;
      if (drawEnd > config->getScreenHeight()) {
        drawEnd = config->getScreenHeight() - 1;
      }

      drawTextureSlice(x, drawStart, drawEnd, hit, textures);
    }
  }

  void RenderSystem::drawTextureSlice(int x, int bottom, int end, Utils::RayCastHit hit, std::vector<NN::Graphics::Texture> *textures) {
    int lineHeight = (int)(config->getScreenHeight() / hit.perpWallDist);
    double step = 1.0 * TEXTURE_WIDTH / lineHeight;
    double texPos = (bottom - config->getScreenHeight() / 2 + lineHeight / 2) * step;
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

  void RenderSystem::drawBuffer() {
    SDL_UpdateTexture(screen, NULL, (const void *)&buffer, config->getScreenWidth() * sizeof(Uint32));
  }

  void RenderSystem::clearBuffer() {
    for(int y = 0; y < config->getScreenHeight(); y++) {
      for(int x = 0; x < config->getScreenWidth(); x++) {
        buffer[y][x] = 0;
      }
    }
  }

  void RenderSystem::present(bool debug, int fps) {
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

  void RenderSystem::clear() {
    clearBuffer();
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
  }

  void RenderSystem::cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
  }

  double *RenderSystem::getZBuffer() {
    return zBuffer;
  }

  void RenderSystem::setBufferPixel(int x, int y, Uint32 color) {
    buffer[y][x] = color;
  }

}
