#ifndef __SCENE__
#define __SCENE__

#include <iostream>
using namespace std;

#include <vector>

class World;

namespace NN {
  class Engine;

  namespace Scenes {
    class Scene {
      public:
        Scene(Engine &engine): engine(engine) {}
        virtual void onCreate() = 0;
        virtual void onDestroy() = 0;
        virtual void update(double frameTime) {};
        virtual void onActivate() {};
        virtual void onDeactivate() {};

      protected:
        Engine engine;
        Entities::Entity player;
        World *world;

        friend class SceneStateMachine;
    };
  }
}
#endif
