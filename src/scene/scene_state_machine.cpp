#include <iostream>
using namespace std;

#include "scene/scene_state_machine.h"
#include "scene/scene.h"

#include "graphics/camera.h"
#include "world.h"

namespace NN::Scenes {
  SceneStateMachine::SceneStateMachine(): scenes(0), currentScene(0) {}

  void SceneStateMachine::update(double frameTime) {
    if (currentScene) {
      currentScene->update(frameTime);
    }
  }

  int SceneStateMachine::add(boost::shared_ptr<Scene> scene) {
    auto inserted = scenes.insert(std::make_pair(insertedSceneId, scene));
    insertedSceneId++;

    if (!currentScene) {
      currentScene = scene;
    }

    inserted.first->second->onCreate();

    return insertedSceneId - 1;
  }

  void SceneStateMachine::remove(int id) {
    auto it = scenes.find(id);
    if (it != scenes.end()) {
      if (currentScene == it->second) {
        currentScene = NULL;
      }

      it->second->onDestroy();

      scenes.erase(it);
    }
  }

  void SceneStateMachine::switchTo(int id) {
    auto it = scenes.find(id);
    if (it != scenes.end()) {
      if (currentScene) {
        currentScene->onDeactivate();
      }

      currentScene = it->second;
      currentScene->onActivate();
    }
  }

  Entities::Entity SceneStateMachine::getPlayer() {
    return currentScene->player;
  }
}
