#include <iostream>
#include <algorithm>
#include <utility>
#include <functional>

using namespace std;

#include <boost/scoped_ptr.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "input/keyboard.h"
#include "input/input_packet.h"

#include "camera.h"
#include "config.h"
#include "game_objects/player.h"
#include "world.h"
#include "renderer.h"
#include "sprite.h"
#include "engine.h"


static int gameObjectsProcessingThread(void *ptr) {
  Engine *engine = (Engine *)ptr;
  engine->processGameObjects();
}

Engine::Engine(int width, int height, Camera *camera, World world, Config config): width(width),
                                                                               height(height),
                                                                               camera(camera),
                                                                               world(world),
                                                                               renderer(Renderer(camera, world)),
                                                                               config(config),
                                                                               quit(false) {
  gameObjectsLock = SDL_CreateMutex();
  spritesLock = SDL_CreateMutex();
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

      //cout << "Starting Entity processing thread!" << endl;
      //SDL_Thread *gameObjectsThread = SDL_CreateThread(gameObjectsProcessingThread, "RenderThread", (void *)this);

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

        processGameObjects();

        renderer.drawWorld(*player);
        renderer.drawSprites(*player, sprites);
        renderer.present(debug, (int)(1 / frameTime));
        renderer.clear();
        clearSprites();


        int frameTicks = SDL_GetTicks() - fpsCapTime;
        if (frameTicks < fpsTicksPerFrame) {
          SDL_Delay(fpsTicksPerFrame - frameTicks);
        }
      }

      //SDL_WaitThread(gameObjectsThread, NULL);
    }

    renderer.cleanup();
  }

  SDL_Quit();
  TTF_Quit();
  SDL_DestroyMutex(gameObjectsLock);
  SDL_DestroyMutex(spritesLock);
}

void Engine::processGameObjects() {
  //while(!quit) {
    oldCapTime = capTime;
    capTime = SDL_GetTicks();
    oldProcessingTIme = processingTime;
    processingTime = SDL_GetTicks();
    double diff = processingTime - oldProcessingTIme;
    processingFrameTime = (processingTime - oldProcessingTIme) / 1000.0;

    /*
     * Handle Entities
     */
    for(const auto &object: gameObjects) {
      object->update(*this, world, processingFrameTime);
    }

    /*int frameTicks = SDL_GetTicks() - capTime;
    if (frameTicks < processingFpsTicksPerFrame) {
      SDL_Delay(processingFpsTicksPerFrame - frameTicks);
    }*/
  //}
}

void Engine::addPlayer(Player *player) {
  this->player = player;
}

Player *Engine::getPlayer() {
  return player;
}

void Engine::drawSprite(Sprite *sprite, double x, double y) {
  sprites.push_back(new DrawableSprite(sprite, x, y));
}

void Engine::addGameObject(GameObject *object) {
  gameObjects.push_back(object);
}

void Engine::removeGameObject(GameObject *object) {
    std::vector<GameObject *>::iterator where = std::find(gameObjects.begin(), gameObjects.end(), object);
    if (where != gameObjects.end()) {
      gameObjects.erase(where);
      delete *where;
    }
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
