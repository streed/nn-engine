#include <iostream>

using namespace std;

#include <vector>

#include "projectile.h"
#include "world.h"
#include "game.h"
#include "player.h"
#include "entity.h"

void Projectile::update(Game *game,
                        World &world,
                        Player *player,
                        std::vector<Entity *> *entities,
                        double timeDiff) {
  lifeSpanSeconds -= timeDiff;

  if (lifeSpanSeconds > 0) {
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
