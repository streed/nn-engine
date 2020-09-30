#ifndef __PHYSICS_COMPONENT__
#define __PHYSICS_COMPONENT__

class Player;
class World;

class PhysicsComponent {
  public:
    void update(Player &player, World &world, double frameTime);

  private:
    double collisionDistance = 1;
};

#endif
