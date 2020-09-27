#include <iostream>
#include <vector>
#include <math.h>
#include <SDL2/SDL.h>

#include "globals.h"
#include "world.h"
#include "camera.h"
#include "player.h"
#include "game.h"
#include "texture.h"
#include "sprite.h"
#include "monster.h"

using namespace std;

World world = World(MAP_WIDTH,  MAP_HEIGHT, (int *)&worldMap);

int main(int argc, char **args) {
  Config config(argc, args);
  Camera camera = Camera(-1, 0, 0, 0.66);
  Player player = Player(&camera, 4, 10, 3, MOVE_SPEED, ROTATE_SPEED);
  Game game(SCREEN_WIDTH, SCREEN_HEIGHT, &camera, world, config);
  game.addPlayer(&player);
  game.addSprite(new Sprite(4.5, 15.5, 8));
  game.addSprite(new Sprite(1.5, 15.5, 8));
  game.addSprite(new Sprite(4.5, 17.5, 10));
  game.addSprite(new Sprite(1.5, 17.5, 10));
  Monster monster(3, 20, 11);
  game.addSprite(&monster);
  game.addEntity(&monster);
  game.run();

  return 0;
}
