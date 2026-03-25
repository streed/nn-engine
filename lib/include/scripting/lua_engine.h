#ifndef NN_LUA_ENGINE_H
#define NN_LUA_ENGINE_H

#include <string>
#include <functional>
#include <memory>

struct lua_State;

namespace NN {
  class Engine;

  namespace Audio {
    class SoundSystem;
  }

  namespace Scripting {

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

      private:
        void registerAPI();
        bool callFunction(const std::string &name);

        lua_State *L = nullptr;
        Engine *engine = nullptr;
        Audio::SoundSystem *soundSystem = nullptr;
        bool scriptLoaded = false;
    };

  }
}

#endif
