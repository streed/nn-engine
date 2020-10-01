#ifndef __SCENE_STATE_MACHINE__
#define __SCENE_STATE_MACHINE__

#include <vector>
#include <memory>
#include <unordered_map>
#include <boost/shared_ptr.hpp>

class Camera;
class GameObject;
class Player;
class Scene;
class World;

class SceneStateMachine {
  public:
    SceneStateMachine();

    void update(double frameTime);

    int add(boost::shared_ptr<Scene> scene);
    void switchTo(int id);
    void remove(int id);


  private:
    std::vector<GameObject *> *getGameObjects();
    World *getWorld();
    Player *getPlayer();
    Camera *getCamera();

    std::unordered_map<int, boost::shared_ptr<Scene>> scenes;
    boost::shared_ptr<Scene> currentScene;
    int insertedSceneId;

    friend class Engine;
};

#endif
