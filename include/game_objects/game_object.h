#ifndef __GAME_OBJECT__
#define __GAME_OBJECT__

class Engine;
class World;

class GameObject {
  public:
    virtual ~GameObject() {}
    virtual void update(Engine &engine, World &world, double processingTime) = 0;
    virtual void draw(Engine &engine) {};
};

#endif
