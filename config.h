#ifndef __CONFIG__
#define __CONFIG__

#include <vector>

#include "texture.h"

class Config {
  std::vector<Texture> textures;

  public:
    Config();

    std::vector<Texture> *getTextures();

  private:
    void loadTextures();
};

#endif
