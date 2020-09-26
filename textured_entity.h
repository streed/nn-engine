#ifndef __TEXTURED_ENTITY__
#define __TEXTURED_ENTITY__

#include "entity.h"

class TexturedEntity: public Entity {
  public:
    int textureIndex;

    TexturedEntity(double posX, double posY, int textureIndex): Entity(posX, posY),
                                                                textureIndex(textureIndex) {};
}

#endif
