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

#include "globals.h"
#include "graphics/texture.h"
#include "components.h"
#include "systems/system.h"


class World;

namespace NN {
  class Config;

  namespace Graphics {
    class Texture;
  }
  
  namespace Utils {
    class RayCastHit;
  }

  namespace Systems::Graphics {

    class RenderSystem: public Systems::System {

      public:
        RenderSystem();
        RenderSystem(NN::Config *config);
        bool setup(NN::Config *config);
        void update(NN::Engine &engine, double frameTime) override;

        void present(bool debug, int fps);
        void clear();

        double *getZBuffer();
        void setBufferPixel(int x, int y, Uint32 color);

      private:
        void drawTextureSlice(int x,
                              int bottom,
                              int end,
                              Utils::RayCastHit hit,
                              std::vector<NN::Graphics::Texture> *textures);
        void drawWorld(NN::Components::Position &player,
                       NN::Components::Camera &camera,
                       World *world,
                       std::vector<NN::Graphics::Texture> *textures);
        void drawBuffer();
        void clearBuffer();
        void cleanup();

        NN::Config *config;

        SDL_Renderer *renderer = NULL;
        SDL_Window *window = NULL;
        SDL_Texture *screen = NULL;
        TTF_Font *font = NULL;

        Uint32 buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
        double zBuffer[SCREEN_WIDTH];
    };
  }
}
#endif
