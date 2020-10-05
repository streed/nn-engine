#ifndef COLOR_RGB_H
#define COLOR_RGB_H

class ColorRGB {
  public:
    int r;
    int g;
    int b;

    ColorRGB(int r, int g, int b);
    ColorRGB();
    ColorRGB operator/(const int div);
};

static const ColorRGB RGB_Red(255, 0, 0);
static const ColorRGB RGB_Green(0, 255, 0);
static const ColorRGB RGB_Blue(0, 0, 255);
static const ColorRGB RGB_White(255, 255, 255);
static const ColorRGB RGB_Yellow(255, 255, 0);

#endif
