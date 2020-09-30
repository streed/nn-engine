#ifndef __PLAYER_INPUT_COMPONENT__
#define __PLAYER_INPUT_COMPONENT__

#include "components/input_component.h"

class GameObject;

class PlayerInputComponent: public InputComponent {
  public:
    virtual void update(GameObject *object, double frameTime);
};

#endif
