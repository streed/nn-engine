#ifndef __CONFIG__
#define __CONFIG__

#include <vector>

#include "graphics/texture.h"

namespace NN {
  class Config {
    std::vector<Graphics::Texture> textures;
    bool fullscreen;
    std::string baseGameDir;

    public:
    Config(int argc, char **args);

    std::vector<Graphics::Texture> *getTextures();
    bool getFullscreen();

    int getScreenWidth();
    int getScreenHeight();

    private:
    void loadJsonConfig();
    void loadTextures();
    void loadSprites();
  };
}
#endif
