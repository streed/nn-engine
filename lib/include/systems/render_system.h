#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#if defined __APPLE__ || _MSC_VER
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
    struct RayCastHit;
  }

  namespace Systems::Graphics {

    class RenderSystem: public Systems::System {

      public:
        RenderSystem();
        RenderSystem(NN::Config *config);
        bool setup(NN::Config *config);
        void update(NN::Engine *engine, double frameTime) override;

        void present(bool debug, int fps);
        // Split present for UI overlay: first draws buffer + debug, then UI renders, then final present
        void presentPreUI(bool debug, int fps);
        void presentFinal();
        void clear();

        // Draw a muzzle flash effect on screen (intensity 0.0 to 1.0)
        void drawMuzzleFlash(double intensity);

        double *getZBuffer();
        void setBufferPixel(int x, int y, Uint32 color);

        SDL_Renderer *getRenderer();
        TTF_Font *getFont();

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

        std::vector<Uint32> buffer;
        std::vector<double> zBuffer;
    };
  }
}
#endif
