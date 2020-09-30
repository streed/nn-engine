#ifndef __IMP_GRAPHICS_COMPONENT__
#define __IMP_GRAPHICS_COMPONENT__

#include "components/graphics_component.h"

#include "sprite.h"

class ImpGraphicsComponent: public GraphicsComponent {
  public:
    ImpGraphicsComponent(Sprite *sprite): sprite(sprite) {}
    void update(GameObject &object, Engine &engine);

    Sprite *sprite;
};

#endif
