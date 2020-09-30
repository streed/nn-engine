#include <iostream>
using namespace std;

#include "components/sprite_graphics_component.h"
#include "game_objects/positional_object.h"

#include "engine.h"

void SpriteGraphicsComponent::update(GameObject &object, Engine &engine) {
  PositionalObject &imp = dynamic_cast<PositionalObject &>(object);
  engine.drawSprite(sprite, imp.posX, imp.posY);
}
