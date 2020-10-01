#ifndef __GAME_OBJECT_REGISTRATION__
#define __GAME_OBJECT_REGISTRATION__

#include "components/component.h"
#include "managers/components_manager.h"

namespace Components {
  template<typename T> class Registration {
    public:
      Registration(const std::string name) {
        ComponentsManager::get().registerComponent(name, []() {
            static_assert(std::is_base_of<Component, T>::value,
                          "You can only register objects that are derived from GameObject");
            return static_cast<Component *>(new T());
        });
      }
  };
};

#endif
