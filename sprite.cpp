#include "sprite.h"

double Sprite::distanceFromSquared(Player &player) {
  return (player.posX - posX) * (player.posX - posX) +
         (player.posY - posY) * (player.posY - posY);
}

int Sprite::getTextureIndex() {
  return textureIndex;
}
