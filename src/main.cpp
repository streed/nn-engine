#include <iostream>
#include <stdexcept>
#include <vector>
#include <math.h>

#ifdef __APPLE__
#include <SDL.h>
#elif
#include <SDL2/SDL.h>
#endif

#include "globals.h"
#include "world.h"
#include "engine.h"

#include "scene/scene_state_machine.h"
#include "scenes/starter_scene.h"
#include "scenes/movement_test_scene.h"

using namespace std;

int main(int argc, char **args) {
  Config config(argc, args);
  Engine engine(SCREEN_WIDTH, SCREEN_HEIGHT, config);

  SceneStateMachine sceneMachine;
  engine.setSceneStateMachine(&sceneMachine);

  boost::shared_ptr<MovementTestScene> movementTestScene(new MovementTestScene(engine));
  boost::shared_ptr<StarterScene> starterScene(new StarterScene(engine, 0));

  if (argc >= 2) {
    sceneMachine.add(movementTestScene);
  } else {
    sceneMachine.add(starterScene);
  }

  try {
    engine.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << endl;
    throw;
  }

  return 0;
}
