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
#include "penguin_shooter_scene.h"

int main(int argc, char *args[]) {
  NN::Config config(argc, args);
  NN::Engine engine(&config);
  engine.setup();

  auto sceneStateMachine = std::make_shared<NN::Scenes::SceneStateMachine>();

  // Check for command-line scene selection or map file
  std::string sceneType;
  std::string mapFile;

  for (int i = 1; i < argc; i++) {
    std::string arg = args[i];
    if (arg == "--scene" && i + 1 < argc) {
      sceneType = args[++i];
    } else if (arg.size() > 5 && arg.substr(arg.size() - 5) == ".json"
               && arg.find("--") != 0) {
      mapFile = arg;
    }
  }

  if (sceneType == "follow" || !mapFile.empty()) {
    // Original follow scene (penguins follow player/each other)
    sceneStateMachine->add(std::make_shared<FollowScene>(&engine, mapFile));
  } else {
    // Default: Penguin shooter game
    sceneStateMachine->add(std::make_shared<PenguinShooterScene>(&engine));
  }

  engine.setSceneStateMachine(sceneStateMachine);

  std::cout << "nnEngine - Running!" << std::endl;
  engine.run();

  return 0;
}
