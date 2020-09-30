#ifndef __AI_COMPONENT__
#define __AI_COMPONENT__

class GameObject;
class Player;
class World;

class AIComponent {
  public:
    virtual ~AIComponent() {}
    virtual void update(GameObject &object, Player &player, World &world, double frameTime) = 0;
};

#endif
