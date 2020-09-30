#include <iostream>
using namespace std;

#include "components/basic_moving_object_phyiscs_component.h"

#include "game_objects/basic_moving_object.h"
#include "world.h"

void BasicMovingObjectPhysicsComponent::update(BasicMovingObject *object, World &world, double frameTime) {
  double newX = object->posX + object->velocityX;
  double newY = object->posY + object->velocityY;

  cout << object->velocityX << ", " << object->velocityY << endl;

  if (world.getMapPoint(int(newX), int(object->posY)) == 0) {
    object->posX = newX;
  }

  if (world.getMapPoint(int(object->posX), int(newY)) == 0) {
    object->posY = newY;
  }
}
