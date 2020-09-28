#ifndef __PROJECTILE__
#define __PROJECTILE__

#include "entity_factory_registration.h"
#include "sprite.h"

class Projectile: public Sprite {
  static Entities::Registration<Projectile> registered;

  double dirX;
  double dirY;
  double lifeSpanSeconds;
  bool alive;


  public:
  Projectile(): Sprite(0, 0, 0, 0, 0, -1),
                dirY(0),
                dirX(0),
                alive(true),
                lifeSpanSeconds(lifeSpanSeconds) {};
  Projectile(double posX,
             double posY,
             double dirX,
             double dirY,
             double lifeSpanSeconds,
             double speed,
             int textureIndex): Sprite(posX, posY, 0, speed, 0, 12),
                                dirX(dirX),
                                dirY(dirY),
                                alive(true),
                                lifeSpanSeconds(lifeSpanSeconds) {};

    virtual void update(Game *game,
                        World &world,
                        Player *player,
                        std::vector<Entity *> *entities,
                        double timeDiff);
    bool isAlive();
};

#endif
