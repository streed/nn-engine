#include <iostream>
#include <stdexcept>
#include <vector>
#include <math.h>
#include <memory>

#if (defined __APPLE__ || defined _MSC_VER)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "engine/engine.h"
#include "engine/config.h"
#include "scene/scene_state_machine.h"
#include "follow_scene.h"

using namespace std;

int main(int argc, char *args[]) {
  NN::Config config(argc, args);
  NN::Engine engine(&config);
  engine.setup();

  NN::Scenes::SceneStateMachine *sceneStateMachine = new NN::Scenes::SceneStateMachine();
  sceneStateMachine->add(std::shared_ptr<FollowScene>(new FollowScene(&engine)));
  engine.setSceneStateMachine(std::shared_ptr<NN::Scenes::SceneStateMachine>(sceneStateMachine));

  cout << "nnEngine - Running!" << endl;
  engine.run();

  return 0;
}
