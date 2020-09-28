#ifndef __ENTITY_FACTORY_REGISTRATION__
#define __ENTITY_FACTORY_REGISTRATION__

#include "entity_factory.h"

namespace Entities {
  template <typename T> class Registration {
    public:
      Registration(const std::string name) {
        EntityFactory::get().registerEntity(name, [](){
          return static_cast<Entity *>(new T());
        });
      }
  };
}

#endif
