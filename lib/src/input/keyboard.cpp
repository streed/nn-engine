#include "input/keyboard.h"

#if defined __APPLE__ || _MSC_VER
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
      keyPresses[event.key.keysym.scancode] = true;
      break;
    case SDL_KEYUP:
      keyPresses[event.key.keysym.scancode] = false;
      break;
  }
}

InputPacket *Keyboard::getInput() {
  InputPacket *packet = new InputPacket(keyPresses[SDL_SCANCODE_W],
                                        keyPresses[SDL_SCANCODE_S],
                                        keyPresses[SDL_SCANCODE_A],
                                        keyPresses[SDL_SCANCODE_D],
                                        keyPresses[SDL_SCANCODE_K],
                                        keyPresses[SDL_SCANCODE_L],
                                        keyPresses[SDL_SCANCODE_Q],
                                        keyPresses[SDL_SCANCODE_P]);
  return packet;
}
