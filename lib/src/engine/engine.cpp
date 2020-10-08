#include <iostream>
using namespace std;

#include <boost/scoped_ptr.hpp>

#include "coordinator.h"
#include "engine/config.h"
#include "engine/engine.h"
#include "entities.h"
#include "input/input_packet.h"
#include "input/keyboard.h"
#include "scene/scene_state_machine.h"
#include "systems/render_system.h"
#include "systems/input_system.h"
#include "systems/player_movement_system.h"
#include "systems/physics_system.h"
#include "systems/sprite_system.h"
#include "systems/AnimatedSpriteSystem.h"
#include "world.h"

namespace NN {
  Engine::Engine(Config *config): config(config) {
    quit = false;
    debug = false;
    coordinator = new Coordinator();
    coordinator->init();

    renderSystem = coordinator->registerSystem<Systems::Graphics::RenderSystem>();
    inputSystem = coordinator->registerSystem<Systems::BuiltIns::InputSystem>();
    playerMovementSystem = coordinator->registerSystem<Systems::BuiltIns::PlayerMovementSystem>();
    physicsSystem = coordinator->registerSystem<Systems::BuiltIns::PhysicsSystem>();
    spriteSystem = coordinator->registerSystem<Systems::BuiltIns::SpriteSystem>();
    animatedSpriteSystem = coordinator->registerSystem<Systems::BuiltIns::AnimatedSpriteSystem>();

    coordinator->registerComponent<Components::Position>();
    coordinator->registerComponent<Components::Velocity>();
    coordinator->registerComponent<Components::Camera>();
    coordinator->registerComponent<Components::Input>();
    coordinator->registerComponent<Components::Sprite>();
    coordinator->registerComponent<Components::AnimatedSprite>();

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

    Entities::Signature spriteSignature;
    spriteSignature.set(coordinator->getComponentType<Components::Position>());
    spriteSignature.set(coordinator->getComponentType<Components::AnimatedSprite>());
    coordinator->setSystemSignature<Systems::BuiltIns::SpriteSystem>(spriteSignature);

    Entities::Signature animatedSpriteSignature;
    animatedSpriteSignature.set(coordinator->getComponentType<Components::AnimatedSprite>());
    coordinator->setSystemSignature<Systems::BuiltIns::AnimatedSpriteSystem>(animatedSpriteSignature);
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
  }

  void Engine::run() {
    oldFrameTime = SDL_GetTicks();
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
        debug = !debug;
      }

      inputSystem->update(this, frameTime);

	  while (lag >= GAME_LOOP_TICKS) {
		  unsigned int ticks = SDL_GetTicks();
		  playerMovementSystem->update(this, GAME_LOOP_TICKS / 1000.0);
		  animatedSpriteSystem->update(this, GAME_LOOP_TICKS / 1000.0);
		  sceneStateMachine->update(GAME_LOOP_TICKS / 1000.0);
		  physicsSystem->update(this, GAME_LOOP_TICKS / 1000.0);
		  lag -= GAME_LOOP_TICKS;
	  }

      renderSystem->update(this, frameTime);
      spriteSystem->update(this, frameTime);
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

  void Engine::setCurrentPlayer(Entities::Entity entity) {
    currentPlayer = entity;
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
    return sceneStateMachine->getWorld();
  }

  void Engine::setSceneStateMachine(std::shared_ptr<Scenes::SceneStateMachine> sceneStateMachine) {
    this->sceneStateMachine = sceneStateMachine;
  }
  
  std::shared_ptr<Scenes::SceneStateMachine> Engine::getSceneStateMachine() {
    return sceneStateMachine;
  }
}
