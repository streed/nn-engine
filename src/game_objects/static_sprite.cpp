#include "game_objects/static_sprite.h"

void StaticSprite::update(Engine &engine, World &world, double frameTime) {
  graphicsComponent->update(*this, engine);
}
