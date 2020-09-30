#include "graphics/color_rgb.h"

ColorRGB::ColorRGB(int r, int g, int b) {
  this->r = r;
  this->g = g;
  this->b = b;
}

ColorRGB::ColorRGB() {
  this->r = 0;
  this->g = 0;
  this->b = 0;
}

ColorRGB ColorRGB::operator/(const int div) {
  return ColorRGB(r / div, g / div, b / div);
}
