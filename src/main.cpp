#include <iostream>
#include <vector>
#include <math.h>
#include <SDL2/SDL.h>

#include "globals.h"
#include "world.h"
#include "engine.h"
#include "graphics/camera.h"
#include "graphics/sprite.h"
#include "components/player_input_component.h"
#include "components/basic_moving_object_phyiscs_component.h"
#include "components/imp_ai_component.h"
#include "components/sprite_graphics_component.h"
#include "game_objects/player.h"
#include "game_objects/imp.h"
#include "game_objects/static_sprite.h"

using namespace std;

World world = World(MAP_WIDTH,  MAP_HEIGHT, (int *)&worldMap);

int main(int argc, char **args) {
  Config config(argc, args);
  Camera camera = Camera(-1, 0, 0, 0.66);
  Player player = Player(&camera,
                         4,
                         10,
                         5,
                         3,
                         new PlayerInputComponent(),
                         new BasicMovingObjectPhysicsComponent());
  Engine engine(SCREEN_WIDTH, SCREEN_HEIGHT, &camera, world, config);
  engine.addPlayer(&player);
  engine.addGameObject(&player);
  engine.addGameObject(new Imp(3, 21, new ImpAIComponent(),
                                      new SpriteGraphicsComponent(new Sprite(11)),
                                      new BasicMovingObjectPhysicsComponent()));
  /*engine.addGameObject(new Imp(4, 20, new ImpAIComponent(),
                                      new SpriteGraphicsComponent(new Sprite(11)),
                                      new BasicMovingObjectPhysicsComponent()));
  */
  engine.addGameObject(new StaticSprite(4.5, 15.5, new SpriteGraphicsComponent(new Sprite(8))));
  engine.addGameObject(new StaticSprite(1.5, 15.5, new SpriteGraphicsComponent(new Sprite(8))));
  engine.addGameObject(new StaticSprite(4.5, 17.5, new SpriteGraphicsComponent(new Sprite(10))));
  engine.addGameObject(new StaticSprite(1.5, 17.5, new SpriteGraphicsComponent(new Sprite(10))));

  engine.run();

  return 0;
}
