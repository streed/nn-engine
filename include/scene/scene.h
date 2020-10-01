#ifndef __SCENE__
#define __SCENE__

#include <iostream>
using namespace std;

#include <vector>

class Camera;
class Engine;
class GameObject;
class Player;
class World;

class Scene {
  public:
    Scene(Engine &engine): engine(engine) {
      gameObjects = new std::vector<GameObject *>(1024);
    }
    virtual void onCreate() = 0;
    virtual void onDestroy() = 0;
    virtual void onActivate() {};
    virtual void onDeactivate() {};
    virtual void update(double frameTime) {};

  protected:
    Engine &engine;
    World *world;
    Player *player;
    Camera *camera;
    std::vector<GameObject *> *gameObjects;

    friend class SceneStateMachine;
};

#endif
