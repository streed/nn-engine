#ifndef NN_LUA_ENGINE_H
#define NN_LUA_ENGINE_H

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <cstdint>

#include "entities.h"

struct lua_State;

namespace NN {
  class Engine;

  namespace Audio {
    class SoundSystem;
  }

  namespace UI {
    class UICanvas;
  }

  namespace Scripting {

    // Timer managed by Lua engine
    struct ScriptTimer {
      double remaining;
      double interval;   // 0 = one-shot, >0 = repeating
      std::string callback;
      int id;
    };

    class LuaEngine {
      public:
        LuaEngine();
        ~LuaEngine();

        // Initialize Lua state and register the game API
        bool setup(Engine *engine, Audio::SoundSystem *soundSystem);

        // Load and execute a script file
        bool loadScript(const std::string &filepath);

        // Lifecycle callbacks (called by the engine)
        void callOnStart();
        void callOnUpdate(double frameTime);
        void callOnZoneEnter(const std::string &zoneName);
        void callOnZoneExit(const std::string &zoneName);
        void callOnEntityKilled(uint32_t entity);
        void callOnDoorOpened(int doorX, int doorY);
        void callOnInteract();

        bool isLoaded() const;

        // Entity name registry
        void registerEntity(const std::string &name, Entities::Entity entity);
        void unregisterEntity(const std::string &name);
        Entities::Entity getEntityByName(const std::string &name) const;
        bool hasEntity(const std::string &name) const;

        // Timer management (called from game loop)
        void updateTimers(double dt);

        // Script message display (for UI integration)
        const std::string &getCurrentMessage() const;
        bool hasMessage() const;
        double getMessageTimer() const;

      private:
        void registerAPI();
        bool callFunction(const std::string &name);

        lua_State *L = nullptr;
        Engine *engine = nullptr;
        Audio::SoundSystem *soundSystem = nullptr;
        bool scriptLoaded = false;

        // Entity name -> ECS entity ID mapping
        std::unordered_map<std::string, Entities::Entity> entityNames;

        // Timer system
        std::vector<ScriptTimer> timers;
        int nextTimerId = 1;

        // Script message
        std::string currentMessage;
        double messageTimer = 0.0; // seconds remaining, 0 = permanent until cleared
    };

  }
}

#endif
