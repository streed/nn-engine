#ifndef __SPRITE__
#define __SPRITE__

class Sprite {
  public:
    Sprite(int textureIndex): textureIndex(textureIndex) {}
    int textureIndex;
};

class DrawableSprite {
  public:
    DrawableSprite(Sprite *sprite, double x, double y): sprite(sprite), x(x), y(y) {}

    Sprite *sprite;
    double x;
    double y;
};

#endif
