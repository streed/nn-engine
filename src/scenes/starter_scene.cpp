#include <iostream>
using namespace std;

#include "scenes/starter_scene.h"

#include "components/basic_moving_object_phyiscs_component.h"
#include "components/imp_ai_component.h"
#include "components/player_input_component.h"
#include "components/sprite_graphics_component.h"

#include "globals.h"
#include "engine.h"
#include "world.h"

#include "graphics/camera.h"
#include "graphics/sprite.h"

#include "game_objects/imp.h"
#include "game_objects/player.h"
#include "game_objects/static_sprite.h"

void StarterScene::onCreate() {
  cout << "StarterScene onCreated called" << endl;
  world = new World(MAP_WIDTH,  MAP_HEIGHT, (int *)&worldMap);
  camera = new Camera(-1, 0, 0, 0.66);
  player = new Player(camera,
                  4,
                  10,
                  5,
                  3,
                  new PlayerInputComponent(),
                  new BasicMovingObjectPhysicsComponent());

  engine.addGameObject(player);
  engine.addGameObject(new Imp(3, 21, new ImpAIComponent(),
                                      new SpriteGraphicsComponent(new Sprite(11)),
                                      new BasicMovingObjectPhysicsComponent()));
  engine.addGameObject(new Imp(4, 20, new ImpAIComponent(),
                                      new SpriteGraphicsComponent(new Sprite(11)),
                                      new BasicMovingObjectPhysicsComponent()));
  
  engine.addGameObject(new StaticSprite(3.5, 10.5, new SpriteGraphicsComponent(new Sprite(8))));
  engine.addGameObject(new StaticSprite(4.5, 15.5, new SpriteGraphicsComponent(new Sprite(8))));
  engine.addGameObject(new StaticSprite(1.5, 15.5, new SpriteGraphicsComponent(new Sprite(8))));
  engine.addGameObject(new StaticSprite(4.5, 17.5, new SpriteGraphicsComponent(new Sprite(10))));
  engine.addGameObject(new StaticSprite(1.5, 17.5, new SpriteGraphicsComponent(new Sprite(10))));
}

void StarterScene::onDestroy() {
  gameObjects->clear();
  delete gameObjects;
}

void StarterScene::onActivate() {
  cout << "StarterScene activated" << endl;
}

void StarterScene::onDeactivate() {
  cout << "StarterScene deactivated" << endl;
}

void StarterScene::update(double frameTime) {
}
