#ifndef NN_SCENE_H
#define NN_SCENE_H

#include <vector>
#include <cstdlib>

class World;

namespace NN {
  class Engine;

  namespace Entities {
      using Entity = std::uint32_t;
  }

  namespace Scenes {
    class Scene {
      public:
        Scene(Engine *engine): engine(engine), player(-1), world(nullptr) {}
        virtual void onCreate() = 0;
        virtual void onDestroy() = 0;
        virtual void update(double frameTime) {};
        virtual void onActivate() {};
        virtual void onDeactivate() {};

      protected:
        Engine *engine;
        Entities::Entity player;
        World *world;

        friend class SceneStateMachine;
    };
  }
}
#endif
