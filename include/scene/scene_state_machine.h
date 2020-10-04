#ifndef __SCENE_STATE_MACHINE__
#define __SCENE_STATE_MACHINE__

#include <vector>
#include <memory>
#include <unordered_map>

#include "entities.h"

class World;

namespace NN {
  class Engine;
  namespace Scenes {

    class Scene;

    class SceneStateMachine {
      public:
        SceneStateMachine();

        void update(double frameTime);

        int add(std::shared_ptr<Scene> scene);
        void switchTo(int id);
        void remove(int id);


      private:
        std::unordered_map<int, std::shared_ptr<Scene>> scenes;
        std::shared_ptr<Scene> currentScene;
        int insertedSceneId;

        World *getWorld();

        friend class NN::Engine;
    };
  }
}

#endif
