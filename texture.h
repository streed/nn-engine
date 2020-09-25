#ifndef __TEXTURE__
#define __TEXTURE__

#include <fstream>
#include <string>
#include <vector>

#include "color_rgb.h"

class Texture {
  unsigned long width;
  unsigned long height;

  std::string filename;
  std::vector<ColorRGB> pixels;

  void loadFile(std::vector<unsigned char> &buffer);
  void loadImage();

  public:
    Texture(int width, int height, std::string filename);

    std::vector<unsigned char> getStripe(int x);
};

#endif
