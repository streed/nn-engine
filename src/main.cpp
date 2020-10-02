#include <iostream>
#include <stdexcept>
#include <vector>
#include <math.h>

#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "globals.h"
#include "world.h"
#include "engine.h"

#include "scene/scene_state_machine.h"

#include "scenes/changelevel_scene.h"
#include "scenes/movement_test_scene.h"
#include "scenes/starter_scene.h"

#include "managers/components_manager.h"

using namespace std;

int main(int argc, char **args) {
  Config config(argc, args);
  auto names = ComponentsManager::get().getComponentNames();

  cout << "Registered Components:" << endl;
  for (auto &it: names) {
    cout << "\t" << it << endl;
  }
  cout << "Done listing registered Components." << endl;

  Engine engine(SCREEN_WIDTH, SCREEN_HEIGHT, config);

  SceneStateMachine sceneMachine;
  engine.setSceneStateMachine(&sceneMachine);

  boost::shared_ptr<MovementTestScene> movementTestScene(new MovementTestScene(engine));
  boost::shared_ptr<StarterScene> starterScene(new StarterScene(engine, 0));
  boost::shared_ptr<ChangeLevelScene> changeLevelTestScene(new ChangeLevelScene(engine));

  if (argc == 2) {
    int id = sceneMachine.add(movementTestScene);
    sceneMachine.switchTo(id);
  } else if(argc == 3) {
    int firstId = sceneMachine.add(changeLevelTestScene);
    int secondId = sceneMachine.add(movementTestScene);
    sceneMachine.switchTo(firstId);
  } else {
    int id = sceneMachine.add(starterScene);
    sceneMachine.switchTo(id);
  }

  try {
    engine.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << endl;
    throw;
  }

  return 0;
}
