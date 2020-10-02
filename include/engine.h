#ifndef __ENGINE__
#define __ENGINE__

#include <boost/lockfree/queue.hpp>

#include <vector>

#include "scene/scene_state_machine.h"
#include "graphics/renderer.h"
#include "world.h"
#include "config.h"

class Camera;
class DrawableSprite;
class Entity;
class GameObject;
class SceneStateMachine;
class Sprite;
struct SDL_mutex;

class Engine {
  private: 
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

    static const int desiredFps = 30;
    static const int ticksPerFrame = 1000 / desiredFps;

    SDL_mutex *gameObjectsLock;
    SDL_mutex *spritesLock;

    Renderer renderer;
    Player *player;

    SceneStateMachine *sceneStateMachine;

    std::vector<DrawableSprite *> sprites;

  public:
    Engine(int width, int height, Config config);
    void setSceneStateMachine(SceneStateMachine *sceneStateMachine);
    void run();
    Player *getPlayer();
    void drawSprite(Sprite *sprite, double x, double y);

    void addGameObject(GameObject *object);
    void removeGameObject(GameObject *object);
    std::vector<GameObject *> *getGameObjects();

    void processGameObjects();
    void renderGameObjects();
    bool getDebug();

    static Engine *get();

  private:
    void processEvents();
    void clearSprites();
};

#endif
