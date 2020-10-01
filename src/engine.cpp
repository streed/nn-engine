#include <iostream>
#include <algorithm>
#include <utility>
#include <functional>

using namespace std;

#include <boost/scoped_ptr.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "graphics/camera.h"
#include "graphics/renderer.h"
#include "graphics/sprite.h"
#include "game_objects/player.h"
#include "input/keyboard.h"
#include "input/input_packet.h"

#include "config.h"
#include "world.h"
#include "engine.h"


Engine::Engine(int width,
               int height,
               Config config): width(width),
                               height(height),
                               renderer(Renderer()),
                               config(config),
                               sceneStateMachine(sceneStateMachine),
                               quit(false) {
  sprites.reserve(1024);
}

void Engine::setSceneStateMachine(SceneStateMachine *sceneStateMachine) {
  this->sceneStateMachine = sceneStateMachine;
}

void Engine::run() {
  if (SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_JOYSTICK) < 0 || TTF_Init() < 0) {
    cout << "SDL Could not initialize! SDL_Error: " << SDL_GetError() << "\n";
    cout << "TTF Could not inituialize! TTF_Error: " << TTF_GetError() << "\n";
  } else {
    if (renderer.setup(width, height, config.getTextures(), config.getFullscreen())) {
      if (debug) {
        cout << "Window created, starting game." << endl;
      }

      while(!quit) {
        oldFpsCapTime = fpsCapTime;
        fpsCapTime = SDL_GetTicks();
        oldFrameTime = currentFrameTime;
        currentFrameTime = SDL_GetTicks();
        double frameTime = (currentFrameTime - oldFrameTime) / 1000.0;

        /*
         * Input logic
         */
        processEvents();

        boost::scoped_ptr<InputPacket> inputPacket(Keyboard::get().getInput());

        if (inputPacket.get()->quit) {
          if (debug) {
            cout << "Quitting the game" << endl;
          }
          quit = true;
        }

        if (inputPacket.get()->debug) {
          debug = !debug;
        }

        sceneStateMachine->update(frameTime);
        processGameObjects();

        renderer.drawWorld(*sceneStateMachine->getPlayer(), *sceneStateMachine->getWorld());
        renderer.drawSprites(*sceneStateMachine->getPlayer(), sprites);
        renderer.present(debug, (int)(1 / frameTime));
        renderer.clear();
        clearSprites();

        int frameTicks = SDL_GetTicks() - fpsCapTime;
        if (frameTicks < fpsTicksPerFrame) {
          SDL_Delay(fpsTicksPerFrame - frameTicks);
        }
      }
    }

    renderer.cleanup();
  }

  SDL_Quit();
  TTF_Quit();
  SDL_DestroyMutex(gameObjectsLock);
  SDL_DestroyMutex(spritesLock);
}

void Engine::processGameObjects() {
  oldCapTime = capTime;
  capTime = SDL_GetTicks();
  oldProcessingTIme = processingTime;
  processingTime = SDL_GetTicks();
  double diff = processingTime - oldProcessingTIme;
  processingFrameTime = (processingTime - oldProcessingTIme) / 1000.0;

  /*
   * Handle GameObjects
   */
  for(auto *object: *getGameObjects()) {
    if (object != NULL) {
      object->update(*this, *sceneStateMachine->getWorld(), processingFrameTime);
    }
  }
}

Player *Engine::getPlayer() {
  return sceneStateMachine->getPlayer();
}

void Engine::drawSprite(Sprite *sprite, double x, double y) {
  sprites.push_back(new DrawableSprite(sprite, x, y));
}

void Engine::addGameObject(GameObject *object) {
  getGameObjects()->push_back(object);
}

void Engine::removeGameObject(GameObject *object) {
  std::vector<GameObject *> *gameObjects = getGameObjects();
  std::vector<GameObject *>::iterator where = std::find(gameObjects->begin(), gameObjects->end(), object);
  if (where != gameObjects->end()) {
    gameObjects->erase(where);
  }
}

std::vector<GameObject *> *Engine::getGameObjects() {
  return sceneStateMachine->getGameObjects();
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

void Engine::clearSprites() {
  sprites.clear();
}

