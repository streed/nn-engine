#ifndef __CONFIG__
#define __CONFIG__

#include <vector>

#include "texture.h"

class Config {
  std::vector<Texture> textures;
  bool fullscreen;

  public:
    Config(int argc, char **args);

    std::vector<Texture> *getTextures();
    bool getFullscreen();

  private:
    void loadTextures();
    void loadSprites();
};

#endif
