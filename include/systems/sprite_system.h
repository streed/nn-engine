#ifndef __SPRITE_SYSTEM__
#define __SPRITE_SYSTEM__

#ifdef __APPLE__
#include <SDL.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif

#include "systems/system.h"

namespace NN::Systems::BuiltIns {
  class SpriteSystem: public NN::Systems::System {
    public:
      void update(Engine &engine, double frameTime) override;
  };
}

#endif
