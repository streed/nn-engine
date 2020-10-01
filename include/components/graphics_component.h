#ifndef __GRAPHICS_COMPONENT__
#define __GRAPHICS_COMPONENT__

#include "components/component.h"

class Engine;
class GameObject;

class GraphicsComponent: public Component {
  public:
    virtual ~GraphicsComponent() {}
    virtual void update(GameObject *object, Engine &engine) = 0;
};

#endif
