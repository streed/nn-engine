#ifndef __SCENE__
#define __SCENE__

#include <iostream>
using namespace std;

#include <vector>

namespace NN {
  class Engine;

  namespace Scenes {
    class Scene {
      public:
        Scene(Engine &engine): engine(engine) {}
        virtual void onCreate() = 0;
        virtual void onDestroy() = 0;
        virtual void onActivate() {};
        virtual void onDeactivate() {};
        virtual void update(double frameTime) {};

      protected:
        NN::Engine &engine;
        Entities::Entity player;

        friend class SceneStateMachine;
    };
  }
}
#endif
