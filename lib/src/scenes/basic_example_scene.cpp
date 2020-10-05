
#include "engine/engine.h"
#include "scenes/basic_example_scene.h"
#include "components.h"
#include "coordinator.h"
#include "world.h"

static int worldMap[10][10] = {
  {1,1,4,4,4,4,4,4,2,2},
  {1,0,0,0,0,0,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,4},
  {4,0,0,0,0,0,0,0,0,4},
  {4,0,0,0,0,0,0,0,0,4},
  {4,0,0,0,0,0,0,0,0,4},
  {4,0,0,0,0,0,0,0,0,4},
  {4,0,0,0,0,0,0,0,0,4},
  {3,0,0,0,0,0,0,0,0,5},
  {3,3,4,4,4,4,4,4,5,5}
};

namespace NN::Scenes::Examples {
  void BasicExampleScene::onCreate() {
    Coordinator *coordinator = engine.getCoordinator();

    world = new World(10, 10, (int *)&worldMap);

    Entities::Entity currentPlayer = coordinator->createEntity();
    engine.setCurrentPlayer(currentPlayer);
    coordinator->addComponent<Components::Position>(currentPlayer, Components::Position{2, 2});
    coordinator->addComponent<Components::Camera>(currentPlayer, Components::Camera{-1, 0, 0, 0.66});
    coordinator->addComponent<Components::Input>(currentPlayer, Components::Input{false,
                                                                                  false,
                                                                                  false,
                                                                                  false,
                                                                                  false,
                                                                                  false,
                                                                                  false,
                                                                                  false});
    coordinator->addComponent<Components::Velocity>(currentPlayer, Components::Velocity{0, 0, 5, 3});

    Entities::Entity penguin = coordinator->createEntity();
    coordinator->addComponent<Components::Position>(penguin, Components::Position{5.5, 5.5});
    coordinator->addComponent<Components::Sprite>(penguin, Components::Sprite{11, 64, 64});
  }

  void BasicExampleScene::onDestroy() {
  }

  void BasicExampleScene::update(double frameTime) {
  }
}
