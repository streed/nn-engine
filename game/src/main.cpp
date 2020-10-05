#include <iostream>
#include <stdexcept>
#include <vector>
#include <math.h>
#include <memory>

#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <engine/engine.h>
#include "engine/config.h"
#include "entities.h"
#include "systems/system.h"
#include "systems/physics_system.h"
#include "scene/scene_state_machine.h"
#include "scenes/basic_example_scene.h"
#include "coordinator.h"
#include "world.h"

using namespace std;

int main(int argc, char **args) {
  NN::Config config(argc, args);
  NN::Engine engine(&config);

  NN::Scenes::SceneStateMachine sceneStateMachine;
  sceneStateMachine
    .add(std::shared_ptr<NN::Scenes::Examples::BasicExampleScene>(
          new NN::Scenes::Examples::BasicExampleScene(engine)));

  engine.setSceneStateMachine(&sceneStateMachine);

  engine.setup();
  engine.run();

  return 0;
}
