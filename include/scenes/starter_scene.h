#ifndef __STARTER_SCENE__
#define __STARTER_SCENE__

#include <vector>

#include "scene/scene.h"

class Camera;
class Engine;
class GameObject;
class Player;
class World;

class StarterScene: public Scene {
  public:
    StarterScene(Engine &engine, int nextScene): Scene(),
                                                 engine(engine),
                                                 nextScene(nextScene) {}

    void onCreate() override;
    void onDestroy() override;
    void onActivate() override;
    void onDeactivate() override;

    void update(double frameTime) override;

  private:
    int nextScene;
    Engine &engine;
    Camera *camera;
};

#endif
