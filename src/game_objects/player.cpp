#include "game_objects/player.h"

#include <iostream>
using namespace std;

void Player::update(Engine &engine, World &world, double frameTime) {
  inputComponent->update(this, frameTime);
  physicsComponent->update(this, engine, world, frameTime);
}
