#include <iostream>
#include <stdexcept>
#include <vector>
#include <math.h>

#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "engine/engine.h"
#include "engine/config.h"
#include "entities.h"
#include "systems/system.h"
#include "systems/physics_system.h"
#include "coordinator.h"
#include "world.h"

using namespace std;

int main(int argc, char **args) {
  NN::Config config(argc, args);
  NN::Engine engine(&config);
  engine.setup();
  engine.run();
  
  /*NN::Coordinator coordinator;
  coordinator.init();

  coordinator.registerComponent<NN::Components::Position>();

  auto physicsSystem = coordinator.registerSystem<NN::Systems::BuiltIns::PhysicsSystem>();

  NN::Entities::Signature signature;
  signature.set(coordinator.getComponentType<NN::Components::Position>());
  coordinator.setSystemSignature<NN::Systems::BuiltIns::PhysicsSystem>(signature);

  NN::Entities::Entity player = coordinator.createEntity();
  coordinator.addComponent(player, NN::Components::Position{1, 2});

  auto &playerPosition = coordinator.getComponent<NN::Components::Position>(player);
  cout << "x: " << playerPosition.posX << " y: " << playerPosition.posY << endl;

  for (int i = 0; i < 10; i++) {
    physicsSystem->update(coordinator, 0.1);
  }

  playerPosition = coordinator.getComponent<NN::Components::Position>(player);
  cout << "x: " << playerPosition.posX << " y: " << playerPosition.posY << endl;
  */

  return 0;
}
