#ifndef __PLAYER_INPUT_COMPONENT__
#define __PLAYER_INPUT_COMPONENT__

#include "components/input_component.h"
#include "managers/components_registration.h"

class GameObject;

class PlayerInputComponent: public InputComponent {
  static Components::Registration<PlayerInputComponent> registered;
  public:
    virtual void update(GameObject *object, double frameTime);
};

#endif
