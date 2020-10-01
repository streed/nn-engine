#include <boost/assert.hpp>

#include "managers/game_object_manager.h"
#include "game_objects/game_object.h"

GameObjectManager::GameObjectManager() {}

GameObjectManager::GameObjectManager(const GameObjectManager &gameObjectManager) {
  generators = gameObjectManager.generators;
}

GameObjectManager::~GameObjectManager() {}

GameObjectManager &GameObjectManager::get() {
  static GameObjectManager instance;
  return instance;
}

const std::vector<std::string> GameObjectManager::getGameObjectNames() {
  std::vector<std::string> names;
  for (auto const &pair: generators) {
    names.push_back(pair.first);
  }

  return names;
}

const GameObject *GameObjectManager::createGameObject(std::string gameObjectName) {
  auto it = generators.find(gameObjectName);
  BOOST_ASSERT_MSG(it != generators.end(), "GameObjectManager: Could not find that GameObject by name");

  return it->second();
}
