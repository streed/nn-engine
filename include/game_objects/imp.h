#ifndef __IMP__
#define __IMP__

#include "game_objects/basic_moving_object.h"
#include "components/ai_component.h"
#include "components/graphics_component.h"
#include "components/physics_component.h"

class Imp: public BasicMovingObject {
  public:
    Imp(double posX,
        double posY,
        AIComponent *aiComponent,
        GraphicsComponent *graphicsComponent,
        PhysicsComponent *physicsComponent): BasicMovingObject(posX, posY, 3, 3),
                                             impAIComponent(aiComponent),
                                             graphicsComponent(graphicsComponent),
                                             physicsComponent(physicsComponent) {}
    void update(Engine &engine, World &world, double frameTime);

  private:
    AIComponent *impAIComponent;
    GraphicsComponent *graphicsComponent;
    PhysicsComponent *physicsComponent;
};

#endif
