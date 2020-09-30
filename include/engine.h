#ifndef __ENGINE__
#define __ENGINE__

#include <boost/lockfree/queue.hpp>

#include <vector>

#include "graphics/renderer.h"
#include "world.h"
#include "config.h"

class Camera;
class Entity;
class Sprite;
class GameObject;
class DrawableSprite;
struct SDL_mutex;

class Engine {
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

  SDL_mutex *gameObjectsLock;
  SDL_mutex *spritesLock;

  Renderer renderer;
  Camera *camera;
  Player *player;
  World world;

  std::vector<DrawableSprite *> sprites;
  std::vector<GameObject *> gameObjects;

  public:
    Engine(int width, int height, Camera *camera, World world, Config config);
    void run();
    void addPlayer(Player *player);
    Player *getPlayer();
    void drawSprite(Sprite *sprite, double x, double y);

    void addGameObject(GameObject *object);
    void removeGameObject(GameObject *object);
    std::vector<GameObject *> &getGameObjects();

    void processGameObjects();

  private:
    void processEvents();
    void clearSprites();
};

#endif
