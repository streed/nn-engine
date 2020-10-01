#include "scenes/movement_test_scene.h"

#include "components/basic_moving_object_phyiscs_component.h"
#include "components/player_input_component.h"
#include "game_objects/player.h"
#include "graphics/camera.h"
#include "managers/components_manager.h"

#include "engine.h"

static int worldMap[10][10] = {
  {4,4,4,4,4,4,4,4,4,4},
  {4,0,0,0,0,0,0,0,0,4},
  {4,0,0,0,0,0,0,0,0,4},
  {4,0,0,0,0,0,0,0,0,4},
  {4,0,0,0,1,1,0,0,0,4},
  {4,0,0,0,1,1,0,0,0,4},
  {4,0,0,0,0,0,0,0,0,4},
  {4,0,0,0,0,0,0,0,0,4},
  {4,0,0,0,0,0,0,0,0,4},
  {4,4,4,4,4,4,4,4,4,4}
};

void MovementTestScene::onCreate() {
  world = new World(10, 10, (int *)&worldMap);
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

void MovementTestScene::onDestroy() {
  gameObjects->clear();
  delete gameObjects;
}
