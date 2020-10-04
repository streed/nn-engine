#include <iostream>
using namespace std;

#include <boost/scoped_ptr.hpp>

#include "coordinator.h"
#include "engine/config.h"
#include "engine/engine.h"
#include "entities.h"
#include "input/input_packet.h"
#include "input/keyboard.h"
#include "systems/render_system.h"
#include "systems/input_system.h"
#include "systems/player_movement_system.h"
#include "systems/physics_system.h"
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
    quit = false;
    debug = false;
    coordinator = new Coordinator();
    coordinator->init();

    /*
     * TODO: Refactor this out to the Scene class.
     *
     * All of the below should be moved into the Scene
     * class and setup when onCreated is called.
     */

    renderSystem = coordinator->registerSystem<Systems::Graphics::RenderSystem>();
    inputSystem = coordinator->registerSystem<Systems::BuiltIns::InputSystem>();
    playerMovementSystem = coordinator->registerSystem<Systems::BuiltIns::PlayerMovementSystem>();
    physicsSystem = coordinator->registerSystem<Systems::BuiltIns::PhysicsSystem>();

    coordinator->registerComponent<Components::Position>();
    coordinator->registerComponent<Components::Velocity>();
    coordinator->registerComponent<Components::Camera>();
    coordinator->registerComponent<Components::Input>();

    Entities::Signature renderSignature;
    renderSignature.set(coordinator->getComponentType<Components::Position>());
    renderSignature.set(coordinator->getComponentType<Components::Camera>());
    coordinator->setSystemSignature<Systems::Graphics::RenderSystem>(renderSignature);

    Entities::Signature inputSignature;
    inputSignature.set(coordinator->getComponentType<Components::Input>());
    coordinator->setSystemSignature<Systems::BuiltIns::InputSystem>(inputSignature);

    Entities::Signature playerSignature;
    playerSignature.set(coordinator->getComponentType<Components::Camera>());
    playerSignature.set(coordinator->getComponentType<Components::Input>());
    playerSignature.set(coordinator->getComponentType<Components::Velocity>());
    coordinator->setSystemSignature<Systems::BuiltIns::PlayerMovementSystem>(playerSignature);

    Entities::Signature physicsSignature;
    physicsSignature.set(coordinator->getComponentType<Components::Position>());
    physicsSignature.set(coordinator->getComponentType<Components::Velocity>());
    coordinator->setSystemSignature<Systems::BuiltIns::PhysicsSystem>(physicsSignature);

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
    coordinator->addComponent<Components::Input>(currentPlayer, Components::Input{false, false, false, false, false, false, false, false});
    coordinator->addComponent<Components::Velocity>(currentPlayer, Components::Velocity{0, 0, 5, 3});
  }

  void Engine::run() {
    double lag = 0.0;
    while(!quit) {
      currentFrameTime = SDL_GetTicks();
      double elapsed = currentFrameTime - oldFrameTime;
      lag += elapsed;

      double frameTime = elapsed / 1000.0;

      processEvents();

      boost::scoped_ptr<InputPacket> inputPacket(Keyboard::get().getInput());

      if (inputPacket.get()->quit) {
        quit = true;
      }

      if (inputPacket.get()->debug) {
        debug != debug;
      }

      inputSystem->update(*this, frameTime);

      while (lag >= GAME_LOOP_TICKS) {
        playerMovementSystem->update(*this, GAME_LOOP_TICKS / 1000.0);
        physicsSystem->update(*this, GAME_LOOP_TICKS / 1000.0);
        lag -= GAME_LOOP_TICKS;
      }

      renderSystem->update(*this, frameTime);
      renderSystem->present(debug, (int)(1.0 / frameTime));
      renderSystem->clear();

      oldFrameTime = currentFrameTime;
    }
  }

  void Engine::render() {
  }

  void Engine::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          quit = true;
          break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
          Keyboard::get().update(event);
          break;
      }
    }
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