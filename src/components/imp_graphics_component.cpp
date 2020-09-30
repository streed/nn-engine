#include <iostream>
using namespace std;

#include "components/imp_graphics_component.h"
#include "game_objects/basic_moving_object.h"

#include "engine.h"

void ImpGraphicsComponent::update(GameObject &object, Engine &engine) {
  BasicMovingObject &imp = dynamic_cast<BasicMovingObject &>(object);
  engine.drawSprite(sprite, imp.posX, imp.posY);
}
