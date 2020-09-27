#ifndef GAME_H
#define GAME_H

#include <vector>

#include "camera.h"
#include "config.h"
#include "player.h"
#include "renderer.h"
#include "world.h"
#include "sprite.h"

static bool KEY_PRESSES[322];

class Game {
  Config config;
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
  std::vector<Sprite *> sprites;

  public:
    Game(int width, int height, Camera *camera, World world);
    void run();
    void addPlayer(Player *player);
    void addSprite(Sprite *sprite);

  private:
    void clearKeys();
    void handleKeyboard();
    InputPacket handleInput();
};

#endif
