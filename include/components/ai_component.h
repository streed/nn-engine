#ifndef __AI_COMPONENT__
#define __AI_COMPONENT__

class Engine;
class GameObject;
class World;

class AIComponent {
  public:
    virtual ~AIComponent() {}
    virtual void update(GameObject *object, Engine &engine, World &world, double frameTime) = 0;
};

#endif
