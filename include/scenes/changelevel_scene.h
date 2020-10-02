#ifndef __CHANGELEVEL_SCENE__
#define __CHANGELEVEL_SCENE__

#include "scene/scene.h"

class ChangeLevelScene: public Scene {
  public:
    ChangeLevelScene(Engine &engine): Scene(engine) {};

    void onCreate() override;
    void onDestroy() override;
    void onActivate() {};
    void onDeactivate() {};

    void update(double frameTime) {};
};

#endif
