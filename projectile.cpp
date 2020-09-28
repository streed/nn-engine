#include <iostream>

using namespace std;

#include <vector>

#include "projectile.h"
#include "world.h"
#include "game.h"
#include "player.h"
#include "entity.h"

Entities::Registration<Projectile> Projectile::registered = Entities::Registration<Projectile>("Projectile");

void Projectile::update(Game *game,
                        World &world,
                        Player *player,
                        std::vector<Entity *> *entities,
                        double timeDiff) {
  lifeSpanSeconds -= timeDiff;

  int mapX = int(posX);
  int mapY = int(posY);

  if (lifeSpanSeconds > 0 && world.getMapPoint(mapX, mapY) == 0) {
    double speed = maxSpeedClip * timeDiff;
    posX = posX + dirX * speed;
    posY = posY + dirY * speed;
  } else {
    alive = false;
    game->removeEntity(this);
  }
}

bool Projectile::isAlive() {
  return alive;
}
