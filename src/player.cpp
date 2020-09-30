#include <iostream>
using namespace std;

#include <math.h>
#include <boost/smart_ptr.hpp>

#include "player.h"
#include "input/keyboard.h"
#include "input/input_packet.h"
#include "components/physics_component.h"
#include "components/input_component.h"
#include "camera.h"
#include "world.h"

void Player::handleInputs(World world, double frameTime) {
  inputComponent.update(*this, frameTime);
  physicsComponent.update(*this, world, frameTime);
}
