#ifndef __KEYBOARD__
#define __KEYBOARD__

#define MAX_NUM_KEYS 322

class InputPacket;
union SDL_Event;

class Keyboard {
  public:
    static Keyboard &get();
    InputPacket *getInput();
    void update(SDL_Event &event);
    void clear();

  private:
    Keyboard();
    Keyboard(const Keyboard &);
    ~Keyboard();

    bool keyPresses[MAX_NUM_KEYS];
};

#endif
