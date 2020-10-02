#include <iostream>
#include <algorithm>
#include <utility>
#include <functional>

using namespace std;

#include <boost/scoped_ptr.hpp>

#ifdef __APPLE__
#include <SDL.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif

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

      double lag = 0.0;
      while(!quit) {
        currentFrameTime = SDL_GetTicks();
        double elapsed = currentFrameTime - oldFrameTime;
        double frameTime = elapsed / 1000.0;
        lag += elapsed;

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


        while (lag >= ticksPerFrame) {
          sceneStateMachine->update(ticksPerFrame / 1000.0);
          processGameObjects();
          sceneStateMachine->getWorld()->resetNavMesh();
          lag -= ticksPerFrame;
        }

        renderGameObjects();
        renderer.drawWorld(*sceneStateMachine->getPlayer(), *sceneStateMachine->getWorld());
        renderer.drawSprites(*sceneStateMachine->getPlayer(), sprites);
        renderer.present(debug, (int)(1 / frameTime));
        renderer.clear();
        clearSprites();
        oldFrameTime = currentFrameTime;
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
  /*
   * Handle GameObjects
   */
  for (auto *object: *getGameObjects()) {
    PositionalObject *positionalObject = dynamic_cast<PositionalObject *>(object);

    if (positionalObject) {
      sceneStateMachine->getWorld()->markNotTraversable(int(positionalObject->posX), int(positionalObject->posY));
    }
  }

  for (auto *object: *getGameObjects()) {
    if (object != NULL) {
      object->update(*this, *sceneStateMachine->getWorld(), ticksPerFrame / 1000.0);
    }
  }
}

void Engine::renderGameObjects() {
  for (auto *object: *getGameObjects()) {
    if (object != NULL) {
      object->draw(*this);
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

bool Engine::getDebug() {
  return debug;
}
