#include <iostream>
#include <vector>
#include <math.h>
#include <SDL2/SDL.h>

#include "globals.h"
#include "world.h"
#include "camera.h"
#include "player.h"
#include "engine.h"
#include "texture.h"
#include "sprite.h"
#include "monster.h"
#include "entity_factory.h"

using namespace std;

World world = World(MAP_WIDTH,  MAP_HEIGHT, (int *)&worldMap);

int main(int argc, char **args) {
  Config config(argc, args);
  Camera camera = Camera(-1, 0, 0, 0.66);
  Player player = Player(&camera, 4, 10, 3, MOVE_SPEED, ROTATE_SPEED);
  Engine engine(SCREEN_WIDTH, SCREEN_HEIGHT, &camera, world, config);
  engine.addPlayer(&player);
  engine.addSprite(new Sprite(4.5, 15.5, 8));
  engine.addSprite(new Sprite(1.5, 15.5, 8));
  engine.addSprite(new Sprite(4.5, 17.5, 10));
  engine.addSprite(new Sprite(1.5, 17.5, 10));
  Monster monster(100, 3, 20, 11);
  engine.addEntity(&monster);
  cout << "Registered entities:" << endl;
  for (auto const &name: EntityFactory::get().getEntityNames()) {
    cout << "\t" << name << endl;
  }
  engine.run();

  return 0;
}
