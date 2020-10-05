#ifndef __SYSTEM__
#define __SYSTEM__

#include <unordered_map>
#include <memory>
#include <set>

#include "entities.h"


namespace NN {
  class Engine;
  namespace Systems {
    class System {
      public:
        virtual void update(NN::Engine &engine, double frameTime) = 0;
        std::set<Entities::Entity> entities;
    };

    class SystemManager {
      public:
        void destroyedEntity(Entities::Entity entity);
        void entitySignatureChanged(Entities::Entity entity, Entities::Signature signature);
        template<typename T> std::shared_ptr<T> registerSystem() {
          const char *typeName = typeid(T).name();
          auto system = std::make_shared<T>();
          systems.insert({typeName, system});
          return system;
        }

        template<typename T> void setSignature(Entities::Signature signature) {
          const char *typeName = typeid(T).name();
          signatures.insert({typeName, signature});
        }

      private:
        std::unordered_map<const char *, Entities::Signature> signatures{};
        std::unordered_map<const char *, std::shared_ptr<System>> systems{};
    };
  };
};

#endif
