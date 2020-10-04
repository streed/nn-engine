#ifndef __ENGINE__
#define __ENGINE__

#include <memory>

#include "entities.h"

class World;

namespace NN {
  namespace Systems::Graphics {
    class RenderSystem;
  }
  class SceneStateMachine;
  class Coordinator;
  class Config;

  class Engine {
    public:
      Engine(Config *config);
      ~Engine();
      void setup();
      void run();

      Coordinator *getCoordinator();
      Entities::Entity getCurrentPlayer();
      Config *getConfig();
      std::shared_ptr<Systems::Graphics::RenderSystem> getRenderSystem();
      World *getWorld();

    private:
      void render();
      void processEvents();

      Config *config;
      Coordinator *coordinator;
      World *world;

      static const int DESIRED_FPS = 35;
      static const int GAME_LOOP_TICKS = 1000 / DESIRED_FPS;

      double currentFrameTime;
      double oldFrameTime;

      Entities::Entity currentPlayer;

      std::shared_ptr<Systems::Graphics::RenderSystem> renderSystem;
  };
}

#endif
