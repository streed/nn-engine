#ifndef __CONFIG__
#define __CONFIG__

#include <vector>

#include "graphics/texture.h"

namespace NN {
  class Config {
    std::vector<Graphics::Texture> textures;
    bool fullscreen;

    public:
    Config(int argc, char **args);

    std::vector<Graphics::Texture> *getTextures();
    bool getFullscreen();

    int getScreenWidth();
    int getScreenHeight();

    private:
    void loadTextures();
    void loadSprites();
  };
}
#endif
