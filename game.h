#ifndef GAME_H
#define GAME_H

#include "camera.h"
#include "renderer.h"
#include "player.h"
#include "world.h"

static bool KEY_PRESSES[322];

class Game {
  bool quit;
  bool debug;
  int width;
  int height;
  double time;
  double oldTime;
  Renderer renderer;
  Camera *camera;
  Player *player;
  World world;

  public:
    Game(int width, int height, Camera *camera, World world);
    void run();
    void addPlayer(Player *player);

  private:
    void clearKeys();
    void handleKeyboard();
    InputPacket handleInput();
};

#endif
