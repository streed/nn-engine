#include "input/keyboard.h"

#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "input/input_packet.h"

Keyboard::Keyboard() {
  for(int i = 0; i < MAX_NUM_KEYS; i++) {
    keyPresses[i] = false;
  }
}

Keyboard::Keyboard(const Keyboard &keyboard) {
  for (int i = 0; i < MAX_NUM_KEYS; i++) {
    keyPresses[i] = keyboard.keyPresses[i];
  }
}

Keyboard::~Keyboard() {}

Keyboard &Keyboard::get() {
  static Keyboard instance;
  return instance;
}

void Keyboard::update(SDL_Event &event) {
  switch (event.type) {
    case SDL_KEYDOWN:
      keyPresses[event.key.keysym.sym] = true;
      break;
    case SDL_KEYUP:
      keyPresses[event.key.keysym.sym] = false;
      break;
  }
}

InputPacket *Keyboard::getInput() {
  InputPacket *packet = new InputPacket(keyPresses[SDLK_w],
                                        keyPresses[SDLK_s],
                                        keyPresses[SDLK_a],
                                        keyPresses[SDLK_d],
                                        keyPresses[SDLK_k],
                                        keyPresses[SDLK_l],
                                        keyPresses[SDLK_q],
                                        keyPresses[SDLK_p]);
  return packet;
}
