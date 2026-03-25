#include "scripting/lua_engine.h"

#include <iostream>
#include <cmath>
#include <algorithm>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#include "engine/engine.h"
#include "coordinator.h"
#include "components.h"
#include "weapons/weapon_components.h"
#include "world.h"
#include "audio/sound_system.h"

namespace NN::Scripting {

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
    impl->lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string,
                             sol::lib::table, sol::lib::coroutine);
    L = impl->lua.lua_state();

    registerAPI();
    return true;
  }

  void LuaEngine::registerAPI() {
    auto &lua = impl->lua;

    // ── Door Control ────────────────────────────────────────────────
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
      if (door) door->autoCloseDelay = delay;
    });

    lua.set_function("is_door_open", [this](int x, int y) -> bool {
      World *w = engine->getWorld();
      if (!w) return false;
      DoorState *door = w->getDoor(x, y);
      return door && door->openProgress >= 0.9;
    });

    lua.set_function("get_door_progress", [this](int x, int y) -> double {
      World *w = engine->getWorld();
      if (!w) return 0.0;
      DoorState *door = w->getDoor(x, y);
      return door ? door->openProgress : 0.0;
    });

    // ── Player Control ──────────────────────────────────────────────
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

    lua.set_function("get_player_dir", [this]() -> std::tuple<double, double> {
      auto &cam = engine->getCoordinator()->getComponent<Components::Camera>(
        engine->getCurrentPlayer());
      return {cam.dirX, cam.dirY};
    });

    lua.set_function("set_player_dir", [this](double dirX, double dirY) {
      auto &cam = engine->getCoordinator()->getComponent<Components::Camera>(
        engine->getCurrentPlayer());
      // Normalize direction
      double len = std::sqrt(dirX * dirX + dirY * dirY);
      if (len < 0.001) return;
      cam.dirX = dirX / len;
      cam.dirY = dirY / len;
      // Recalculate plane perpendicular to direction, maintaining FOV
      double fov = 0.66; // standard FOV factor
      cam.planeX = -cam.dirY * fov;
      cam.planeY = cam.dirX * fov;
    });

    lua.set_function("rotate_player", [this](double angle) {
      auto &cam = engine->getCoordinator()->getComponent<Components::Camera>(
        engine->getCurrentPlayer());
      cam.rotate(angle);
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

    lua.set_function("get_player_max_health", [this]() -> double {
      auto &health = engine->getCoordinator()->getComponent<Components::Health>(
        engine->getCurrentPlayer());
      return health.max;
    });

    lua.set_function("set_player_speed", [this](double speed) {
      auto &vel = engine->getCoordinator()->getComponent<Components::Velocity>(
        engine->getCurrentPlayer());
      vel.maxSpeed = speed;
    });

    lua.set_function("get_player_speed", [this]() -> double {
      auto &vel = engine->getCoordinator()->getComponent<Components::Velocity>(
        engine->getCurrentPlayer());
      return vel.maxSpeed;
    });

    // ── Entity Control ──────────────────────────────────────────────
    lua.set_function("get_entity", [this](const std::string &name) -> int {
      if (!hasEntity(name)) return -1;
      return static_cast<int>(getEntityByName(name));
    });

    lua.set_function("entity_exists", [this](const std::string &name) -> bool {
      return hasEntity(name);
    });

    lua.set_function("get_entity_pos", [this](const std::string &name) -> std::tuple<double, double> {
      if (!hasEntity(name)) return {0.0, 0.0};
      auto ent = getEntityByName(name);
      auto &pos = engine->getCoordinator()->getComponent<Components::Position>(ent);
      return {pos.posX, pos.posY};
    });

    lua.set_function("set_entity_pos", [this](const std::string &name, double x, double y) {
      if (!hasEntity(name)) return;
      auto ent = getEntityByName(name);
      auto &pos = engine->getCoordinator()->getComponent<Components::Position>(ent);
      pos.posX = x;
      pos.posY = y;
    });

    lua.set_function("get_entity_health", [this](const std::string &name) -> double {
      if (!hasEntity(name)) return 0.0;
      auto ent = getEntityByName(name);
      auto &health = engine->getCoordinator()->getComponent<Components::Health>(ent);
      return health.current;
    });

    lua.set_function("set_entity_health", [this](const std::string &name, double h) {
      if (!hasEntity(name)) return;
      auto ent = getEntityByName(name);
      auto &health = engine->getCoordinator()->getComponent<Components::Health>(ent);
      health.current = h;
    });

    lua.set_function("damage_entity", [this](const std::string &name, double amount) {
      if (!hasEntity(name)) return;
      auto ent = getEntityByName(name);
      auto &health = engine->getCoordinator()->getComponent<Components::Health>(ent);
      health.takeDamage(amount);
    });

    lua.set_function("heal_entity", [this](const std::string &name, double amount) {
      if (!hasEntity(name)) return;
      auto ent = getEntityByName(name);
      auto &health = engine->getCoordinator()->getComponent<Components::Health>(ent);
      health.heal(amount);
    });

    lua.set_function("set_entity_sprite", [this](const std::string &name, int textureIndex) {
      if (!hasEntity(name)) return;
      auto ent = getEntityByName(name);
      auto &sprite = engine->getCoordinator()->getComponent<Components::AnimatedSprite>(ent);
      sprite.startTextureIndex = textureIndex;
      sprite.currentFrame = 0;
    });

    lua.set_function("spawn_entity", [this](const std::string &name, double x, double y,
                                             int textureIndex, int spriteW, int spriteH) -> int {
      auto *coord = engine->getCoordinator();
      auto ent = coord->createEntity();

      Components::Position pos{};
      pos.posX = x;
      pos.posY = y;
      coord->addComponent(ent, pos);

      Components::AnimatedSprite sprite{};
      sprite.startTextureIndex = textureIndex;
      sprite.endTextureIndex = textureIndex;
      sprite.currentFrame = 0;
      sprite.currentAnimation = 0;
      sprite.spriteWidth = spriteW;
      sprite.spriteHeight = spriteH;
      sprite.spriteScaleX = 1.0;
      sprite.spriteScaleY = 1.0;
      sprite.vMove = 0;
      sprite.actuallyStatic = true;
      coord->addComponent(ent, sprite);

      Components::Health health{};
      health.current = 100.0;
      health.max = 100.0;
      coord->addComponent(ent, health);

      registerEntity(name, ent);
      return static_cast<int>(ent);
    });

    lua.set_function("destroy_entity", [this](const std::string &name) {
      if (!hasEntity(name)) return;
      auto ent = getEntityByName(name);
      engine->getCoordinator()->destroyEntity(ent);
      unregisterEntity(name);
    });

    lua.set_function("move_entity_toward", [this](const std::string &name,
                                                    double targetX, double targetY, double speed) {
      if (!hasEntity(name)) return;
      auto ent = getEntityByName(name);
      auto &pos = engine->getCoordinator()->getComponent<Components::Position>(ent);
      double dx = targetX - pos.posX;
      double dy = targetY - pos.posY;
      double dist = std::sqrt(dx * dx + dy * dy);
      if (dist < 0.01) return;
      pos.posX += (dx / dist) * speed;
      pos.posY += (dy / dist) * speed;
    });

    // ── Map Queries & Mutation ──────────────────────────────────────
    lua.set_function("get_tile", [this](int x, int y) -> int {
      World *w = engine->getWorld();
      return w ? w->getMapPoint(x, y) : -1;
    });

    lua.set_function("set_tile", [this](int x, int y, int value) {
      World *w = engine->getWorld();
      if (w) w->setMapPoint(x, y, value);
    });

    lua.set_function("is_traversable", [this](int x, int y) -> bool {
      World *w = engine->getWorld();
      return w ? w->isTraversable(x, y) : false;
    });

    lua.set_function("get_map_size", [this]() -> std::tuple<int, int> {
      World *w = engine->getWorld();
      return w ? std::make_tuple(w->width, w->height) : std::make_tuple(0, 0);
    });

    // ── Zone Queries ────────────────────────────────────────────────
    lua.set_function("is_player_in_zone", [this](const std::string &name) -> bool {
      World *w = engine->getWorld();
      if (!w) return false;
      auto &pos = engine->getCoordinator()->getComponent<Components::Position>(
        engine->getCurrentPlayer());
      return w->isInZone(name, pos.posX, pos.posY);
    });

    lua.set_function("is_point_in_zone", [this](const std::string &name, double x, double y) -> bool {
      World *w = engine->getWorld();
      return w ? w->isInZone(name, x, y) : false;
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

    lua.set_function("set_sound_volume", [this](int vol) {
      if (soundSystem) soundSystem->setMasterSoundVolume(vol);
    });

    // ── UI Messages ─────────────────────────────────────────────────
    lua.set_function("show_message", [this](const std::string &text, sol::optional<double> duration) {
      currentMessage = text;
      messageTimer = duration.value_or(0.0); // 0 = permanent until cleared
    });

    lua.set_function("get_message", [this]() -> std::string {
      return currentMessage;
    });

    lua.set_function("clear_message", [this]() {
      currentMessage.clear();
      messageTimer = 0.0;
    });

    // ── Timer System ────────────────────────────────────────────────
    lua.set_function("set_timeout", [this](double seconds, const std::string &callback) -> int {
      int id = nextTimerId++;
      timers.push_back({seconds, 0.0, callback, id});
      return id;
    });

    lua.set_function("set_interval", [this](double seconds, const std::string &callback) -> int {
      int id = nextTimerId++;
      timers.push_back({seconds, seconds, callback, id});
      return id;
    });

    lua.set_function("cancel_timer", [this](int id) {
      timers.erase(
        std::remove_if(timers.begin(), timers.end(),
                        [id](const ScriptTimer &t) { return t.id == id; }),
        timers.end());
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

    lua.set_function("random", [](int min, int max) -> int {
      if (min >= max) return min;
      return min + (std::rand() % (max - min + 1));
    });

    lua.set_function("random_float", [](double min, double max) -> double {
      return min + (static_cast<double>(std::rand()) / RAND_MAX) * (max - min);
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
      // Automatically call on_start when script is loaded
      callOnStart();
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

    // Update timers
    updateTimers(frameTime);

    // Update message timer
    if (messageTimer > 0.0) {
      messageTimer -= frameTime;
      if (messageTimer <= 0.0) {
        currentMessage.clear();
        messageTimer = 0.0;
      }
    }

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
    // Find entity name if registered
    std::string entName;
    for (const auto &[name, ent] : entityNames) {
      if (ent == entity) { entName = name; break; }
    }
    sol::protected_function fn = impl->lua["on_entity_killed"];
    if (fn.valid()) {
      auto result = fn(entity, entName);
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

  // Entity registry
  void LuaEngine::registerEntity(const std::string &name, Entities::Entity entity) {
    entityNames[name] = entity;
  }

  void LuaEngine::unregisterEntity(const std::string &name) {
    entityNames.erase(name);
  }

  Entities::Entity LuaEngine::getEntityByName(const std::string &name) const {
    auto it = entityNames.find(name);
    if (it != entityNames.end()) return it->second;
    return 0;
  }

  bool LuaEngine::hasEntity(const std::string &name) const {
    return entityNames.find(name) != entityNames.end();
  }

  // Timer system
  void LuaEngine::updateTimers(double dt) {
    if (!impl || !scriptLoaded) return;

    // Process timers - collect expired ones first to avoid mutation during iteration
    std::vector<ScriptTimer> expired;
    for (auto &timer : timers) {
      timer.remaining -= dt;
      if (timer.remaining <= 0.0) {
        expired.push_back(timer);
      }
    }

    // Remove one-shots and reset repeating timers
    for (const auto &t : expired) {
      if (t.interval > 0.0) {
        // Reset repeating timer
        for (auto &timer : timers) {
          if (timer.id == t.id) {
            timer.remaining = timer.interval;
            break;
          }
        }
      } else {
        // Remove one-shot
        timers.erase(
          std::remove_if(timers.begin(), timers.end(),
                          [&t](const ScriptTimer &timer) { return timer.id == t.id; }),
          timers.end());
      }
    }

    // Fire callbacks
    for (const auto &t : expired) {
      sol::protected_function fn = impl->lua[t.callback];
      if (fn.valid()) {
        auto result = fn();
        if (!result.valid()) {
          sol::error err = result;
          std::cerr << "Lua timer error (" << t.callback << "): " << err.what() << std::endl;
        }
      }
    }
  }

  // Script message
  const std::string &LuaEngine::getCurrentMessage() const {
    return currentMessage;
  }

  bool LuaEngine::hasMessage() const {
    return !currentMessage.empty();
  }

  double LuaEngine::getMessageTimer() const {
    return messageTimer;
  }

}
