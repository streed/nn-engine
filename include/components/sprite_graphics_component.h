#ifndef __SPRITE_GRAPHICS_COMPONENT__
#define __SPRITE_GRAPHICS_COMPONENT__

#include "components/graphics_component.h"
#include "managers/components_registration.h"

#include "graphics/sprite.h"

class SpriteGraphicsComponent: public GraphicsComponent {
  static Components::Registration<SpriteGraphicsComponent> registered;

  public:
    SpriteGraphicsComponent(Sprite *sprite): sprite(sprite) {}
    void update(GameObject *object, Engine &engine);

    Sprite *sprite;
};

#endif
