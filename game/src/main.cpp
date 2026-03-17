#include <iostream>
#include <string>
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

int main(int argc, char *args[]) {
  NN::Config config(argc, args);
  NN::Engine engine(&config);
  engine.setup();

  // Check if a map file was provided as the last argument
  std::string mapFile;
  if (argc > 1) {
    std::string lastArg = args[argc - 1];
    if (lastArg.size() > 5 && lastArg.substr(lastArg.size() - 5) == ".json"
        && lastArg.find("--") != 0) {
      mapFile = lastArg;
    }
  }

  auto sceneStateMachine = std::make_shared<NN::Scenes::SceneStateMachine>();
  sceneStateMachine->add(std::make_shared<FollowScene>(&engine, mapFile));
  engine.setSceneStateMachine(sceneStateMachine);

  std::cout << "nnEngine - Running!" << std::endl;
  engine.run();

  return 0;
}
