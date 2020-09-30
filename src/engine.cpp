#include "engine.h"

#include <iostream>
#include <algorithm>

using namespace std;

#include <boost/scoped_ptr.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "projectile.h"
#include "input/keyboard.h"
#include "input/input_packet.h"

static int entityProcessingThread(void *ptr) {
  Engine *game = (Engine *)ptr;
  game->processEntities();
}

Engine::Engine(int width, int height, Camera *camera, World world, Config config): width(width),
                                                                               height(height),
                                                                               camera(camera),
                                                                               world(world),
                                                                               renderer(Renderer(camera, world)),
                                                                               config(config),
                                                                               quit(false) {
  entityLock = SDL_CreateMutex();
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

      cout << "Starting Entity processing thread!" << endl;
      SDL_Thread *entityThread = SDL_CreateThread(entityProcessingThread, "RenderThread", (void *)this);

      while(!quit) {
        oldFpsCapTime = fpsCapTime;
        fpsCapTime = SDL_GetTicks();
        oldFrameTime = currentFrameTime;
        currentFrameTime = SDL_GetTicks();
        double frameTime = (currentFrameTime - oldFrameTime) / 1000.0;
        renderer.drawWorld(*player);

        SDL_LockMutex(entityLock);
          std::vector<Sprite *> allDrawables = sprites;

          for (auto const &entity: entities) {
            Sprite *maybeSprite = dynamic_cast<Sprite *>(entity);
            if (maybeSprite) {
              allDrawables.push_back(maybeSprite);
            }

            Projectile *maybeProjectile = dynamic_cast<Projectile *>(entity);

            if (maybeProjectile && maybeProjectile->isAlive()) {
              allDrawables.push_back(maybeProjectile);
            }
          }

          renderer.drawSprites(*player, allDrawables);
        SDL_UnlockMutex(entityLock);

        renderer.present(debug, (int)(1 / frameTime));
        renderer.clear();
        /*
         * Input logic
         */
        processEvents();

        player->handleInputs(world, frameTime);

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

        int frameTicks = SDL_GetTicks() - fpsCapTime;
        if (frameTicks < fpsTicksPerFrame) {
          SDL_Delay(fpsTicksPerFrame - frameTicks);
        }
      }

      SDL_WaitThread(entityThread, NULL);
    }

    renderer.cleanup();
  }

  SDL_Quit();
  TTF_Quit();
  SDL_DestroyMutex(entityLock);
}

void Engine::processEntities() {
  while(!quit) {
    oldCapTime = capTime;
    capTime = SDL_GetTicks();
    oldProcessingTIme = processingTime;
    processingTime = SDL_GetTicks();
    double diff = processingTime - oldProcessingTIme;
    processingFrameTime = (processingTime - oldProcessingTIme) / 1000.0;

    /*
     * Handle Entities
     */
    SDL_LockMutex(entityLock);
      for(const auto &entity: entities) {
        entity->update(this, world, player, &entities, processingFrameTime);
      }
    SDL_UnlockMutex(entityLock);

    int frameTicks = SDL_GetTicks() - capTime;
    if (frameTicks < processingFpsTicksPerFrame) {
      SDL_Delay(processingFpsTicksPerFrame - frameTicks);
    }
  }
}

void Engine::addPlayer(Player *player) {
  this->player = player;
}

void Engine::addSprite(Sprite *sprite) {
  sprites.push_back(sprite);
}

void Engine::addEntity(Entity *entity) {
  SDL_LockMutex(entityLock);
    entities.push_back(entity);
  SDL_UnlockMutex(entityLock);
}

void Engine::removeEntity(Entity *entity) {
  SDL_LockMutex(entityLock);
    std::vector<Entity *>::iterator where = std::find(entities.begin(), entities.end(), entity);
    if (where != entities.end()) {
      entities.erase(where);
      delete *where;
    }
  SDL_UnlockMutex(entityLock);
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
