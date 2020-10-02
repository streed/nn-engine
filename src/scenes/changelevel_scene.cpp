#include "scenes/changelevel_scene.h"

#include "components/input_component.h"
#include "components/physics_component.h"
#include "game_objects/player.h"
#include "graphics/camera.h"
#include "managers/components_manager.h"

#include "engine.h"


const int worldMap[3][5] = {
  {4,4,4,4,4},
  {4,0,0,0,4},
  {4,4,4,4,4}
};

void ChangeLevelScene::onCreate() {
  world = new World(5, 3, (int *)&worldMap);
  camera = new Camera(0, 1);
  player = new Player(camera,
                      1.5,
                      3.1,
                      5,
                      3,
                      static_cast<InputComponent *>(ComponentsManager::get().createComponent("PlayerInputComponent")),
                      static_cast<PhysicsComponent *>(ComponentsManager::get().createComponent("BasicMovingObjectPhysicsComponent")));
  addGameObject(player);
}

void ChangeLevelScene::onDestroy() {
  gameObjects->clear();
}
