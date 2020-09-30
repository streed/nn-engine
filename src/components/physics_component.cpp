#include "components/physics_component.h"

#include "player.h"
#include "world.h"

void PhysicsComponent::update(Player &player, World &world, double frameTime) {
  double newX = player.posX + player.velocityX;
  double newY = player.posY + player.velocityY;

  if (world.getMapPoint(int(newX), int(player.posY)) == 0) {
    player.posX = newX;
  }

  if (world.getMapPoint(int(player.posX), int(newY)) == 0) {
    player.posY = newY;
  }
}
