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
static int mouseXRel = 0;
static int mouseYRel = 0;

class Game {
  Config config;
  bool quit;
  bool debug;
  int width;
  int height;

  double processingFrameTime;
  double processingTime;
  double oldProcessingTIme;
  double capTime;
  double oldCapTime;

  static const int desiredProcessingFps = 25;
  static const int processingFpsTicksPerFrame = 1000 / desiredProcessingFps;

  double fpsCapTime;
  double oldFpsCapTime;
  double currentFrameTime;
  double oldFrameTime;

  static const int desiredFps = 60;
  static const int fpsTicksPerFrame = 1000 / desiredFps;

  SDL_mutex *entityLock;

  Renderer renderer;
  Camera *camera;
  Player *player;
  World world;
  std::vector<Sprite *> sprites;
  std::vector<Entity *> entities;

  public:
    Game(int width, int height, Camera *camera, World world, Config config);
    void run();
    void addPlayer(Player *player);
    void addSprite(Sprite *sprite);
    void addEntity(Entity *entity);
    void removeEntity(Entity *entity);

    void processEntities();

  private:
    void clearKeys();
    void handleKeyboard();
    InputPacket handleInput();
};

#endif
