#ifndef __STATIC_SPRITE__
#define __STATIC_SPRITE__

#include "components/graphics_component.h"
#include "game_objects/positional_object.h"

class StaticSprite: public PositionalObject {
  public:
    StaticSprite(double posX,
                 double posY,
                 GraphicsComponent *graphicsComponent): PositionalObject(posX, posY),
                                                        graphicsComponent(graphicsComponent) {}
    void update(Engine &engine, World &world, double frameTime);

  private:
    GraphicsComponent *graphicsComponent;
};

#endif
