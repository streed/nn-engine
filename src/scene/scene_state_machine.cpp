#include <iostream>
using namespace std;

#include "scene/scene_state_machine.h"
#include "scene/scene.h"

#include "game_objects/player.h"
#include "graphics/camera.h"
#include "world.h"

SceneStateMachine::SceneStateMachine(): scenes(0), currentScene(0) {}

void SceneStateMachine::update(double frameTime) {
  if (currentScene) {
    currentScene->update(frameTime);
  }
}

int SceneStateMachine::add(boost::shared_ptr<Scene> scene) {
  auto inserted = scenes.insert(std::make_pair(insertedSceneId, scene));
  scene->onCreate();
  insertedSceneId++;
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

std::vector<GameObject *> *SceneStateMachine::getGameObjects() {
  if(currentScene) {
    return currentScene->gameObjects;
  }

  return NULL;
}

World *SceneStateMachine::getWorld() {
  if (currentScene) {
    return currentScene->world;
  }

  return NULL;
}

Player *SceneStateMachine::getPlayer() {
  if (currentScene) {
    return currentScene->player;
  }

  return NULL;
}

Camera *SceneStateMachine::getCamera() {
  if (currentScene) {
    return currentScene->camera;
  }

  return NULL;
}
