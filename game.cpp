#include "game.h"

#include <iostream>

using namespace std;

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

Game::Game(int width, int height, Camera *camera, World world): width(width),
                                                                height(height),
                                                                camera(camera),
                                                                world(world),
                                                                renderer(Renderer(camera, world)),
                                                                config(Config()),
                                                                quit(false) {
}

void Game::run() {
  if (SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_JOYSTICK) < 0 || TTF_Init() < 0) {
    cout << "SDL Could not initialize! SDL_Error: " << SDL_GetError() << "\n";
    cout << "TTF Could not inituialize! TTF_Error: " << TTF_GetError() << "\n";
  } else {

    if (renderer.setup(width, height, config.getTextures())) {
      if (debug) {
        cout << "Window created, starting game." << endl;
      }
      SDL_Event e;

      while(!quit) {
        oldTime = time;
        time = SDL_GetTicks();
        double frameTime = (time - oldTime) / 1000.0;
        renderer.drawWorld(*player);
        renderer.present(debug, (int)(1 / frameTime));
        renderer.clear();

        /*
         * Input logic
         */
        handleKeyboard();
        InputPacket inputPacket = handleInput();

        player->handleInputs(inputPacket, world, frameTime);

        if (inputPacket.quit) {
          if (debug) {
            cout << "Quitting the game" << endl;
          }
          quit = true;
        }

        if (inputPacket.debug) {
          debug = !debug;
        }
      }
    }

    renderer.cleanup();
  }

  SDL_Quit();
  TTF_Quit();
}

void Game::addPlayer(Player *player) {
  this->player = player;
}

void Game::clearKeys() {
  for(int i = 0; i < 322; i++) {
    KEY_PRESSES[i] = false;
  }
}

void Game::handleKeyboard() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_KEYDOWN:
        KEY_PRESSES[event.key.keysym.sym] = true;
        break;
      case SDL_KEYUP:
        KEY_PRESSES[event.key.keysym.sym] = false;
        break;
    }
  }
}

InputPacket Game::handleInput () {
  InputPacket packet = InputPacket(KEY_PRESSES[SDLK_w],
                                   KEY_PRESSES[SDLK_s],
                                   KEY_PRESSES[SDLK_a],
                                   KEY_PRESSES[SDLK_d],
                                   KEY_PRESSES[SDLK_k],
                                   KEY_PRESSES[SDLK_l],
                                   KEY_PRESSES[SDLK_q],
                                   KEY_PRESSES[SDLK_p]);
  return packet;
}
