#ifndef __INPUT_COMPONENT__
#define __INPUT_COMPONENT__

#include "components/component.h"

class GameObject;

class InputComponent: public Component {
  public:
    virtual ~InputComponent() {}
    virtual void update(GameObject *object, double frameTime) = 0;
};

#endif
