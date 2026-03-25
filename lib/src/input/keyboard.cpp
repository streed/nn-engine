#include "input/keyboard.h"

#if defined __APPLE__ || _MSC_VER
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <fstream>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>

#include "input/input_packet.h"

Keyboard::Keyboard() {
  for(int i = 0; i < MAX_NUM_KEYS; i++) {
    keyPresses[i] = false;
  }
  setupDefaultBindings();
}

Keyboard::Keyboard(const Keyboard &keyboard) {
  for (int i = 0; i < MAX_NUM_KEYS; i++) {
    keyPresses[i] = keyboard.keyPresses[i];
  }
  bindings = keyboard.bindings;
}

Keyboard::~Keyboard() {}

Keyboard &Keyboard::get() {
  static Keyboard instance;
  return instance;
}

void Keyboard::setupDefaultBindings() {
  bindings["forward"]      = SDL_SCANCODE_W;
  bindings["backward"]     = SDL_SCANCODE_S;
  bindings["strafe_left"]  = SDL_SCANCODE_A;
  bindings["strafe_right"] = SDL_SCANCODE_D;
  bindings["rotate_left"]  = SDL_SCANCODE_K;
  bindings["rotate_right"] = SDL_SCANCODE_L;
  bindings["quit"]         = SDL_SCANCODE_Q;
  bindings["debug"]        = SDL_SCANCODE_P;
  bindings["shoot"]        = SDL_SCANCODE_SPACE;
  bindings["weapon1"]      = SDL_SCANCODE_1;
  bindings["weapon2"]      = SDL_SCANCODE_2;
  bindings["weapon3"]      = SDL_SCANCODE_3;
  bindings["weapon4"]      = SDL_SCANCODE_4;
  bindings["interact"]     = SDL_SCANCODE_E;
}

int Keyboard::scancodeFromName(const std::string &name) {
  // Convert name to uppercase for matching
  std::string upper = name;
  std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

  // Common key names
  if (upper == "SPACE" || upper == "SPACEBAR") return SDL_SCANCODE_SPACE;
  if (upper == "RETURN" || upper == "ENTER") return SDL_SCANCODE_RETURN;
  if (upper == "ESCAPE" || upper == "ESC") return SDL_SCANCODE_ESCAPE;
  if (upper == "TAB") return SDL_SCANCODE_TAB;
  if (upper == "BACKSPACE") return SDL_SCANCODE_BACKSPACE;
  if (upper == "LSHIFT" || upper == "LEFT_SHIFT") return SDL_SCANCODE_LSHIFT;
  if (upper == "RSHIFT" || upper == "RIGHT_SHIFT") return SDL_SCANCODE_RSHIFT;
  if (upper == "LCTRL" || upper == "LEFT_CTRL") return SDL_SCANCODE_LCTRL;
  if (upper == "RCTRL" || upper == "RIGHT_CTRL") return SDL_SCANCODE_RCTRL;
  if (upper == "LALT" || upper == "LEFT_ALT") return SDL_SCANCODE_LALT;
  if (upper == "RALT" || upper == "RIGHT_ALT") return SDL_SCANCODE_RALT;
  if (upper == "UP") return SDL_SCANCODE_UP;
  if (upper == "DOWN") return SDL_SCANCODE_DOWN;
  if (upper == "LEFT") return SDL_SCANCODE_LEFT;
  if (upper == "RIGHT") return SDL_SCANCODE_RIGHT;

  // Single letter keys A-Z
  if (upper.size() == 1 && upper[0] >= 'A' && upper[0] <= 'Z') {
    return SDL_SCANCODE_A + (upper[0] - 'A');
  }

  // Number keys 0-9
  if (upper.size() == 1 && upper[0] >= '0' && upper[0] <= '9') {
    if (upper[0] == '0') return SDL_SCANCODE_0;
    return SDL_SCANCODE_1 + (upper[0] - '1');
  }

  // F-keys
  if (upper.size() >= 2 && upper[0] == 'F') {
    try {
      int fnum = std::stoi(upper.substr(1));
      if (fnum >= 1 && fnum <= 12) {
        return SDL_SCANCODE_F1 + (fnum - 1);
      }
    } catch (...) {}
  }

  // Try SDL's own name lookup as fallback
  SDL_Scancode sc = SDL_GetScancodeFromName(name.c_str());
  if (sc != SDL_SCANCODE_UNKNOWN) {
    return sc;
  }

  return -1;
}

void Keyboard::loadBindings(const std::string &configPath) {
  std::ifstream file(configPath);
  if (!file.is_open()) {
    return; // No config file, use defaults
  }

  try {
    nlohmann::json config;
    file >> config;

    nlohmann::json bindingsJson;

    // Support both top-level "bindings" and nested under "settings"
    if (config.contains("bindings")) {
      bindingsJson = config["bindings"];
    } else if (config.contains("settings") && config["settings"].contains("bindings")) {
      bindingsJson = config["settings"]["bindings"];
    } else {
      return;
    }

    for (auto &[action, keyName] : bindingsJson.items()) {
      std::string keyStr = keyName.get<std::string>();
      int scancode = scancodeFromName(keyStr);
      if (scancode >= 0) {
        bindings[action] = scancode;
      } else {
        std::cerr << "Warning: Unknown key name '" << keyStr
                  << "' for action '" << action << "'" << std::endl;
      }
    }
  } catch (const nlohmann::json::exception &e) {
    std::cerr << "Error parsing key bindings: " << e.what() << std::endl;
  }
}

int Keyboard::getBinding(const std::string &action) const {
  auto it = bindings.find(action);
  if (it != bindings.end()) {
    return it->second;
  }
  return -1;
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
  InputPacket *packet = new InputPacket(
    keyPresses[bindings["forward"]],
    keyPresses[bindings["backward"]],
    keyPresses[bindings["strafe_left"]],
    keyPresses[bindings["strafe_right"]],
    keyPresses[bindings["rotate_left"]],
    keyPresses[bindings["rotate_right"]],
    keyPresses[bindings["quit"]],
    keyPresses[bindings["debug"]],
    keyPresses[bindings["shoot"]],
    keyPresses[bindings["weapon1"]],
    keyPresses[bindings["weapon2"]],
    keyPresses[bindings["weapon3"]],
    keyPresses[bindings["weapon4"]],
    keyPresses[bindings["interact"]]
  );
  return packet;
}
