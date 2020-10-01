#ifndef __MOVEMENT_TEST_SCENE__
#define __MOVEMENT_TEST_SCENE__

#include "scene/scene.h"

class MovementTestScene: public Scene {
  public:
    MovementTestScene(Engine &engine): Scene(engine) {};

    void onCreate() override;
    void onDestroy() override;
    void onActivate() {};
    void onDeactivate() {};

    void update(double frameTime) {};
};

#endif
