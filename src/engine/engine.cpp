#include <iostream>
using namespace std;

#include "engine/engine.h"
#include "engine/config.h"

#include "systems/render_system.h"
#include "entities.h"
#include "coordinator.h"
#include "world.h"

static int worldMap[10][10] = {
  {1,1,4,4,4,4,4,4,2,2},
  {1,0,0,0,0,0,0,0,0,2},
  {4,0,0,0,0,0,2,2,0,4},
  {4,0,5,5,5,0,2,0,0,4},
  {4,0,0,0,1,1,2,0,2,4},
  {4,3,0,3,1,1,0,0,0,4},
  {4,0,0,0,0,0,0,1,0,4},
  {4,4,0,3,3,3,3,1,0,4},
  {3,0,0,0,0,0,0,1,0,5},
  {3,3,4,4,4,4,4,4,5,5}
};


namespace NN {
  Engine::Engine(Config *config): config(config) {
    coordinator = new Coordinator();
    coordinator->init();
    renderSystem = coordinator->registerSystem<Systems::Graphics::RenderSystem>();

    coordinator->registerComponent<Components::Position>();
    coordinator->registerComponent<Components::Camera>();

    Entities::Signature signature;
    signature.set(coordinator->getComponentType<Components::Position>());
    signature.set(coordinator->getComponentType<Components::Camera>());
    coordinator->setSystemSignature<Systems::Graphics::RenderSystem>(signature);

    world = new World(10, 10, (int *)&worldMap);
  }

  Engine::~Engine() {}

  void Engine::setup() {
    if (SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_JOYSTICK) < 0 || TTF_Init() < 0) {
      cout << "SDL Could not initialize! SDL_Error: " << SDL_GetError() << "\n";
      cout << "TTF Could not inituialize! TTF_Error: " << TTF_GetError() << "\n";
    } else {
      if (renderSystem->setup(config)) {
        cout << "Window created, starting game." << endl;
      }
    }

    // Temp setup to get a camera inplace.
    currentPlayer = coordinator->createEntity();
    coordinator->addComponent<Components::Position>(currentPlayer, Components::Position{2, 2});
    coordinator->addComponent<Components::Camera>(currentPlayer, Components::Camera{-1, 0, 0, 0.66});
  }

  void Engine::run() {
    double lag = 0.0;
    while(true) {
      currentFrameTime = SDL_GetTicks();
      double elapsed = currentFrameTime - oldFrameTime;
      double frameTime = elapsed / 1000.0;

      renderSystem->update(*this, frameTime);
      renderSystem->present(false, frameTime);
      renderSystem->clear();

      oldFrameTime = currentFrameTime;
    }
  }

  void Engine::render() {
  }

  void Engine::processEvents() {
  }

  NN::Coordinator *Engine::getCoordinator() {
    return coordinator;
  }

  Entities::Entity Engine::getCurrentPlayer() {
    return currentPlayer;
  }

  NN::Config *Engine::getConfig() {
    return config;
  }

  std::shared_ptr<Systems::Graphics::RenderSystem> Engine::getRenderSystem() {
    return renderSystem;
  }

  World *Engine::getWorld() {
    return world;
  }
}
