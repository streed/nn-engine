#ifndef __INPUT_COMPONENT__
#define __INPUT_COMPONENT__

class GameObject;

class InputComponent {
  public:
    virtual ~InputComponent() {}
    virtual void update(GameObject *object, double frameTime) = 0;
};

#endif
