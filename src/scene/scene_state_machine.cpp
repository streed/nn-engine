#include "scene/scene_state_machine.h"
#include "scene/scene.h"

SceneStateMachine::SceneStateMachine(): scenes(0), currentScene(0) {}

void SceneStateMachine::processInput() {
  if (currentScene) {
    currentScene->processInput();
  }
}

void SceneStateMachine::update(double frameTime) {
  if (currentScene) {
    currentScene->update(frameTime);
  }
}

void SceneStateMachine::lastUpdate(double frameTime) {
  if (currentScene) {
    currentScene->lastUpdate(frameTime);
  }
}

void SceneStateMachine::draw() {
  if (currentScene) {
    currentScene->draw();
  }
}

int SceneStateMachine::add(boost::shared_ptr<Scene> scene) {
  auto inserted = scenes.insert(std::make_pair(insertedSceneId, scene));
  insertedSceneId++;

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
