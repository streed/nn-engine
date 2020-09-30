#include <iostream>
#include <vector>
#include <math.h>
#include <SDL2/SDL.h>

#include "globals.h"
#include "world.h"
#include "camera.h"
#include "engine.h"
#include "texture.h"
#include "sprite.h"
#include "entity_factory.h"
#include "components/player_input_component.h"
#include "components/basic_moving_object_phyiscs_component.h"
#include "game_objects/player.h"

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
  //engine.addSprite(new Sprite(8));
  //engine.addSprite(new Sprite(8));
  //engine.addSprite(new Sprite(10));
  //engine.addSprite(new Sprite(10));

  cout << "Registered entities:" << endl;
  for (auto const &name: EntityFactory::get().getEntityNames()) {
    cout << "\t" << name << endl;
  }

  engine.run();

  return 0;
}
