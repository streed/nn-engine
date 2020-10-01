#include "game_objects/static_sprite.h"

void StaticSprite::draw(Engine &engine) {
  graphicsComponent->update(this, engine);
}
