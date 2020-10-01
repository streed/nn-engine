#ifndef __GAME_OBJECT_MANAGER__
#define __GAME_OBJECT_MANAGER__

#include <string>
#include <vector>
#include <unordered_map>

class GameObject;

typedef GameObject *(*configureGameObject)();

class GameObjectManager {
  public:
    static GameObjectManager &get();
    const std::vector<std::string> getGameObjectNames();
    const GameObject *createGameObject(std::string gameObjectName);
    bool registerGameObject(const std::string gameObjectName, const configureGameObject);

  private:
    GameObjectManager();
    GameObjectManager(const GameObjectManager &);
    ~GameObjectManager();

    std::unordered_map<std::string, configureGameObject> generators;
};

#endif
