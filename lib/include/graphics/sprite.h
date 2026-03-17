#ifndef NN_SPRITE_H
#define NN_SPRITE_H

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
