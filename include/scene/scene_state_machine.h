#ifndef __SCENE_STATE_MACHINE__
#define __SCENE_STATE_MACHINE__

#include <vector>
#include <memory>
#include <unordered_map>
#include <boost/shared_ptr.hpp>

#include "entities.h"


namespace NN::Scenes {
  class Scene;

  class SceneStateMachine {
    public:
      SceneStateMachine();

      void update(double frameTime);

      int add(boost::shared_ptr<Scene> scene);
      void switchTo(int id);
      void remove(int id);


    private:
      Entities::Entity getPlayer();

      std::unordered_map<int, boost::shared_ptr<Scene>> scenes;
      boost::shared_ptr<Scene> currentScene;
      int insertedSceneId;

      friend class Engine;
  };
}

#endif
