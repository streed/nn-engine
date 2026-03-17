#ifndef NN_KEYBOARD_H
#define NN_KEYBOARD_H

#include <string>
#include <unordered_map>

#define MAX_NUM_KEYS 322

class InputPacket;
union SDL_Event;

class Keyboard {
  public:
    static Keyboard &get();
    InputPacket *getInput();
    void update(SDL_Event &event);
    void clear();

    // Load key bindings from a JSON config file.
    // Expected format: { "bindings": { "forward": "W", "shoot": "SPACE", ... } }
    // Falls back to defaults for any missing binding.
    void loadBindings(const std::string &configPath);

    // Get the scancode bound to a named action
    int getBinding(const std::string &action) const;

  private:
    Keyboard();
    Keyboard(const Keyboard &);
    ~Keyboard();

    void setupDefaultBindings();
    static int scancodeFromName(const std::string &name);

    bool keyPresses[MAX_NUM_KEYS];

    // Maps action name -> SDL scancode
    std::unordered_map<std::string, int> bindings;
};

#endif
