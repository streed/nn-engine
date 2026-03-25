#include "scripting/lua_engine.h"

#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#include "engine/engine.h"
#include "coordinator.h"
#include "components.h"
#include "weapons/weapon_components.h"
#include "world.h"
#include "audio/sound_system.h"

namespace NN::Scripting {

  // We store the sol::state as an opaque pointer to avoid sol2 in the header
  static sol::state *getSol(lua_State *L) {
    // Not used - we keep our own sol::state
    (void)L;
    return nullptr;
  }

  // The actual sol::state, stored here to keep header clean
  struct LuaEngineImpl {
    sol::state lua;
  };
  static std::unique_ptr<LuaEngineImpl> impl;

  LuaEngine::LuaEngine() {}

  LuaEngine::~LuaEngine() {
    impl.reset();
    L = nullptr;
  }

  bool LuaEngine::setup(Engine *eng, Audio::SoundSystem *snd) {
    engine = eng;
    soundSystem = snd;

    impl = std::make_unique<LuaEngineImpl>();
    impl->lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table);
    L = impl->lua.lua_state();

    registerAPI();
    return true;
  }

  void LuaEngine::registerAPI() {
    auto &lua = impl->lua;

    // ── Map / Door control ──────────────────────────────────────────
    lua.set_function("open_door", [this](int x, int y) {
      World *w = engine->getWorld();
      if (!w) return;
      DoorState *door = w->getDoor(x, y);
      if (door) {
        door->opening = true;
        door->closing = false;
        door->autoCloseTimer = -1.0;
      }
    });

    lua.set_function("close_door", [this](int x, int y) {
      World *w = engine->getWorld();
      if (!w) return;
      DoorState *door = w->getDoor(x, y);
      if (door) {
        door->closing = true;
        door->opening = false;
        door->autoCloseTimer = -1.0;
      }
    });

    lua.set_function("toggle_door", [this](int x, int y) {
      World *w = engine->getWorld();
      if (!w) return;
      DoorState *door = w->getDoor(x, y);
      if (door) {
        if (door->openProgress < 0.5) {
          door->opening = true;
          door->closing = false;
        } else {
          door->closing = true;
          door->opening = false;
        }
        door->autoCloseTimer = -1.0;
      }
    });

    lua.set_function("set_door_auto_close", [this](int x, int y, double delay) {
      World *w = engine->getWorld();
      if (!w) return;
      DoorState *door = w->getDoor(x, y);
      if (door) {
        door->autoCloseDelay = delay;
      }
    });

    lua.set_function("is_door_open", [this](int x, int y) -> bool {
      World *w = engine->getWorld();
      if (!w) return false;
      DoorState *door = w->getDoor(x, y);
      return door && door->openProgress >= 0.9;
    });

    // ── Player info ─────────────────────────────────────────────────
    lua.set_function("get_player_pos", [this]() -> std::tuple<double, double> {
      auto &pos = engine->getCoordinator()->getComponent<Components::Position>(
        engine->getCurrentPlayer());
      return {pos.posX, pos.posY};
    });

    lua.set_function("set_player_pos", [this](double x, double y) {
      auto &pos = engine->getCoordinator()->getComponent<Components::Position>(
        engine->getCurrentPlayer());
      pos.posX = x;
      pos.posY = y;
    });

    lua.set_function("get_player_health", [this]() -> double {
      auto &health = engine->getCoordinator()->getComponent<Components::Health>(
        engine->getCurrentPlayer());
      return health.current;
    });

    lua.set_function("set_player_health", [this](double h) {
      auto &health = engine->getCoordinator()->getComponent<Components::Health>(
        engine->getCurrentPlayer());
      health.current = h;
    });

    // ── Map queries ─────────────────────────────────────────────────
    lua.set_function("get_tile", [this](int x, int y) -> int {
      World *w = engine->getWorld();
      return w ? w->getMapPoint(x, y) : -1;
    });

    lua.set_function("is_traversable", [this](int x, int y) -> bool {
      World *w = engine->getWorld();
      return w ? w->isTraversable(x, y) : false;
    });

    // ── Zone queries ────────────────────────────────────────────────
    lua.set_function("is_player_in_zone", [this](const std::string &name) -> bool {
      World *w = engine->getWorld();
      if (!w) return false;
      auto &pos = engine->getCoordinator()->getComponent<Components::Position>(
        engine->getCurrentPlayer());
      return w->isInZone(name, pos.posX, pos.posY);
    });

    // ── Sound ───────────────────────────────────────────────────────
    lua.set_function("play_sound", [this](const std::string &name) {
      if (soundSystem) soundSystem->playSound(name);
    });

    lua.set_function("play_sound_at", [this](const std::string &name, double x, double y) {
      if (!soundSystem) return;
      auto &pos = engine->getCoordinator()->getComponent<Components::Position>(
        engine->getCurrentPlayer());
      soundSystem->playSoundAt(name, pos.posX, pos.posY, x, y);
    });

    lua.set_function("play_music", [this](const std::string &name) {
      if (soundSystem) soundSystem->playMusic(name);
    });

    lua.set_function("stop_music", [this]() {
      if (soundSystem) soundSystem->stopMusic();
    });

    lua.set_function("set_music_volume", [this](int vol) {
      if (soundSystem) soundSystem->setMusicVolume(vol);
    });

    // ── UI Messages ─────────────────────────────────────────────────
    lua.set_function("show_message", [this](const std::string &text) {
      // Store message for the scene to display via UI system
      std::cout << "[SCRIPT] " << text << std::endl;
      // The scene can poll this - we store it in a global for now
      impl->lua["_engine_message"] = text;
    });

    lua.set_function("get_message", [this]() -> std::string {
      sol::optional<std::string> msg = impl->lua["_engine_message"];
      return msg.value_or("");
    });

    lua.set_function("clear_message", [this]() {
      impl->lua["_engine_message"] = sol::nil;
    });

    // ── Utility ─────────────────────────────────────────────────────
    lua.set_function("print", [](const std::string &msg) {
      std::cout << "[LUA] " << msg << std::endl;
    });

    lua.set_function("distance", [](double x1, double y1, double x2, double y2) -> double {
      double dx = x2 - x1;
      double dy = y2 - y1;
      return std::sqrt(dx * dx + dy * dy);
    });
  }

  bool LuaEngine::loadScript(const std::string &filepath) {
    if (!impl) return false;

    try {
      auto result = impl->lua.safe_script_file(filepath);
      if (!result.valid()) {
        sol::error err = result;
        std::cerr << "Lua script error in " << filepath << ": " << err.what() << std::endl;
        return false;
      }
      scriptLoaded = true;
      return true;
    } catch (const sol::error &e) {
      std::cerr << "Lua load error: " << e.what() << std::endl;
      return false;
    }
  }

  bool LuaEngine::callFunction(const std::string &name) {
    if (!impl || !scriptLoaded) return false;

    sol::protected_function fn = impl->lua[name];
    if (!fn.valid()) return false;

    auto result = fn();
    if (!result.valid()) {
      sol::error err = result;
      std::cerr << "Lua error in " << name << "(): " << err.what() << std::endl;
      return false;
    }
    return true;
  }

  void LuaEngine::callOnStart() { callFunction("on_start"); }

  void LuaEngine::callOnUpdate(double frameTime) {
    if (!impl || !scriptLoaded) return;
    sol::protected_function fn = impl->lua["on_update"];
    if (fn.valid()) {
      auto result = fn(frameTime);
      if (!result.valid()) {
        sol::error err = result;
        std::cerr << "Lua error in on_update(): " << err.what() << std::endl;
      }
    }
  }

  void LuaEngine::callOnZoneEnter(const std::string &zoneName) {
    if (!impl || !scriptLoaded) return;
    sol::protected_function fn = impl->lua["on_zone_enter"];
    if (fn.valid()) {
      auto result = fn(zoneName);
      if (!result.valid()) {
        sol::error err = result;
        std::cerr << "Lua error in on_zone_enter(): " << err.what() << std::endl;
      }
    }
  }

  void LuaEngine::callOnZoneExit(const std::string &zoneName) {
    if (!impl || !scriptLoaded) return;
    sol::protected_function fn = impl->lua["on_zone_exit"];
    if (fn.valid()) {
      auto result = fn(zoneName);
      if (!result.valid()) {
        sol::error err = result;
        std::cerr << "Lua error in on_zone_exit(): " << err.what() << std::endl;
      }
    }
  }

  void LuaEngine::callOnEntityKilled(uint32_t entity) {
    if (!impl || !scriptLoaded) return;
    sol::protected_function fn = impl->lua["on_entity_killed"];
    if (fn.valid()) {
      auto result = fn(entity);
      if (!result.valid()) {
        sol::error err = result;
        std::cerr << "Lua error in on_entity_killed(): " << err.what() << std::endl;
      }
    }
  }

  void LuaEngine::callOnDoorOpened(int doorX, int doorY) {
    if (!impl || !scriptLoaded) return;
    sol::protected_function fn = impl->lua["on_door_opened"];
    if (fn.valid()) {
      auto result = fn(doorX, doorY);
      if (!result.valid()) {
        sol::error err = result;
        std::cerr << "Lua error in on_door_opened(): " << err.what() << std::endl;
      }
    }
  }

  void LuaEngine::callOnInteract() { callFunction("on_interact"); }

  bool LuaEngine::isLoaded() const { return scriptLoaded; }

}
