#ifndef __SCENE__
#define __SCENE__

#include <iostream>
using namespace std;

#include <vector>

class GameObject;
class World;
class Player;
class Camera;

class Scene {
  public:
    Scene() {
      gameObjects = new std::vector<GameObject *>(1024);
    }
    virtual void onCreate() = 0;
    virtual void onDestroy() = 0;
    virtual void onActivate() {};
    virtual void onDeactivate() {};
    virtual void update(double frameTime) {};

  protected:
    World *world;
    Player *player;
    Camera *camera;
    std::vector<GameObject *> *gameObjects;

    friend class SceneStateMachine;
};

#endif
