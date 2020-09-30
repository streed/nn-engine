#ifndef __INPUT_COMPONENT__
#define __INPUT_COMPONENT__

class Player;

class InputComponent {
  public:
    void update(Player &player, double frameTime);
};

#endif
