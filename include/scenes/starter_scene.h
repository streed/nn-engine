#ifndef __STARTER_SCENE__
#define __STARTER_SCENE__

#include "scene/scene.h"

class StarterScene: public Scene {
  public:
    StarterScene(int nextScene): nextScene(nextScene) {}

    void onCreate() override;
    void onDestroy() override;
    void onActivate() override;
    void onDeactivate() override;

    void update(double frameTime) override;
    void lastUpdate(double frameTime) override;
    void draw() override;

  private:
    int nextScene;
};

#endif
