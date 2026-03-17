#ifndef NN_BASIC_EXAMPLE_SCENE_H
#define NN_BASIC_EXAMPLE_SCENE_H

#include "scene/scene.h"

namespace NN {
  class Engine;
  namespace Scenes::Examples {
    class BasicExampleScene: public Scenes::Scene {
      public:
        BasicExampleScene(Engine *engine): Scene(engine) {}

        void onCreate() override;
        void onDestroy() override;
        void update(double frameTime) override;
    };
  }
}

#endif
