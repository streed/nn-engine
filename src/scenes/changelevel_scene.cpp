#include "scenes/changelevel_scene.h"

#include "components/input_component.h"
#include "components/physics_component.h"
#include "managers/components_manager.h"
#include "game_objects/player.h"


static int worldMap[5][3] = {
  {4,4,4,4,4},
  {4,0,0,0,4},
  {4,4,4,4,4}
};

void ChangeLevelScene::onCreate() {
  world = new World(5, 3, (int *)&worldMap);
  camera = new Camera(-1, 0, 0, 0.66);
  player = new Player(camera,
                      2,
                      2,
                      5,
                      3,
                      static_cast<InputComponent *>(ComponentsManager::get().createComponent("PlayerInputComponent")),
                      static_cast<PhysicsComponent *>(ComponentsManager::get().createComponent("BasicMovingObjectPhysicsComponent")));
  engine.addGameObject(player);
}

void ChangeLevelScene::onDestroy() {
  gameObjects->clear();
}
