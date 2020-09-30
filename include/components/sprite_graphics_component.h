#ifndef __SPRITE_GRAPHICS_COMPONENT__
#define __SPRITE_GRAPHICS_COMPONENT__

#include "components/graphics_component.h"

#include "sprite.h"

class SpriteGraphicsComponent: public GraphicsComponent {
  public:
    SpriteGraphicsComponent(Sprite *sprite): sprite(sprite) {}
    void update(GameObject &object, Engine &engine);

    Sprite *sprite;
};

#endif
