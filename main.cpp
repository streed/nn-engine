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

using namespace std;

World world = World(MAP_WIDTH,  MAP_HEIGHT, (int *)&worldMap);

int main(int argc, char **args) {
  Camera camera = Camera(-1, 0, 0, 0.66);
  Player player = Player(&camera, 2, 2, 3, MOVE_SPEED, ROTATE_SPEED);
  Game game(SCREEN_WIDTH, SCREEN_HEIGHT, &camera, world);
  game.addPlayer(&player);
  game.run();

  return 0;
}
