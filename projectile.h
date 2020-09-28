#ifndef __PROJECTILE__
#define __PROJECTILE__

#include "sprite.h"

class Projectile: public Sprite {
  double dirX;
  double dirY;
  double lifeSpanSeconds;
  bool alive;

  public:
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
