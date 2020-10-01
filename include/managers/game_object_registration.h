#ifndef __GAME_OBJECT_REGISTRATION__
#define __GAME_OBJECT_REGISTRATION__

namespace GameObjects {
  template<typename T> class Registration {
    public:
      Registration(const std::string name) {
        GameObjectManager::get().registerGameObject(name, []() {
            static_assert(std::is_base_of<GameObject, T>::value,
                          "You can only register objects that are derived from GameObject")
            return static_cast<GameObject *>(new T());
        });
      }
  };
};

#endif
