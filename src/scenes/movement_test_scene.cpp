#include "scenes/movement_test_scene.h"

#include "components/basic_moving_object_phyiscs_component.h"
#include "components/player_input_component.h"
#include "components/sprite_graphics_component.h"
#include "game_objects/player.h"
#include "game_objects/imp.h"
#include "game_objects/static_sprite.h"
#include "graphics/camera.h"
#include "managers/components_manager.h"

#include "engine.h"

const int worldMap[10][10] = {
  {1,1,4,4,4,4,4,4,2,2},
  {1,0,0,0,0,0,0,0,0,2},
  {4,0,0,0,0,0,2,2,0,4},
  {4,0,5,5,5,0,2,0,0,4},
  {4,0,0,0,1,1,2,0,2,4},
  {4,3,0,3,1,1,0,0,0,4},
  {4,0,0,0,0,0,0,1,0,4},
  {4,4,0,3,3,3,3,1,0,4},
  {3,0,0,0,0,0,0,1,0,5},
  {3,3,4,4,4,4,4,4,5,5}
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
  addGameObject(player);
  addGameObject(new Imp(8, 8, static_cast<AIComponent *>(ComponentsManager::get().createComponent("ImpAIComponent")),
                              new SpriteGraphicsComponent(new Sprite(11)),
                              static_cast<PhysicsComponent *>(ComponentsManager::get().createComponent("BasicMovingObjectPhysicsComponent"))));
}

void MovementTestScene::onDestroy() {
  gameObjects->clear();
  delete gameObjects;
}
