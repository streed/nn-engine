#ifndef __PROJECTILE_OBJECT__
#define __PROJECTILE_OBJECT__

#include "game_objects/positional_object.h"

class GraphicsComponent;
class PhysicsComponent;

class ProjectileObject: public PositionalObject {
  public:
    ProjectileObject(double posX,
                     double posY,
                     double dirX,
                     double dirY,
                     double lifeSpan,
                     double maxSpeed,
                     PhysicsComponent *physicsComponent,
                     GraphicsComponent *graphicsComponent): PositionalObject(posX, posY),
                                                            dirX(dirX),
                                                            dirY(dirY),
                                                            lifeSpan(lifeSpan),
                                                            maxSpeed(maxSpeed),
                                                            physicsComponent(physicsComponent),
                                                            graphicsComponent(graphicsComponent) {}
    ~ProjectileObject();

    void update(Engine &engine, World &world, double frameTime);

    double dirX;
    double dirY;
    double maxSpeed;
    double lifeSpan;

  private:
    PhysicsComponent *physicsComponent;
    GraphicsComponent *graphicsComponent;
};

#endif
