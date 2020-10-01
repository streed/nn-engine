#include <iostream>
using namespace std;

#include <boost/assert.hpp>

#include "components/sprite_graphics_component.h"
#include "game_objects/positional_object.h"

#include "engine.h"

void SpriteGraphicsComponent::update(GameObject *object, Engine &engine) {
  PositionalObject *positionalObject = dynamic_cast<PositionalObject *>(object);
  BOOST_ASSERT_MSG(positionalObject, "SpriteGraphicsComponent's GameObject must be a sub-child of PositionalGameObject");
  engine.drawSprite(sprite, positionalObject->posX, positionalObject->posY);
}
