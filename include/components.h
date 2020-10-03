#ifndef __COMPONENTS__
#define __COMPONENTS__

#include <memory>
#include <bitset>
#include <cstdlib>
#include <unordered_map>

#include "entities.h"

namespace NN {
  namespace Components {
    using ComponentType = std::uint8_t;
    const ComponentType MAX_COMPONENTS = 32;

    typedef struct _Position {
      double posX;
      double posY;
    } Position;

    typedef struct _Velocity {
      double velocityX;
      double velocityY;
      double maxSpeed;
    } Velocity;

    typedef struct _Rotation {
      double maxRotateSpeed;
    } Rotation;

    typedef struct _ImpAIComponent {
      bool seeking;
      double searchDistance;
      double timeUntilNextShot;
      double shootingCoolDown;
    } ImpAiComponent;


    class ComponentArrayInterface {
      public:
        virtual ~ComponentArrayInterface() = default;
        virtual void destroyedEntity(Entities::Entity entity) = 0;
    };

    template <typename T> class ComponentArray: public ComponentArrayInterface {
      public:
        void insert(Entities::Entity entity, T component) {
          size_t newIndex = size;
          entityToIndex[entity] = newIndex;
          indexToEntity[newIndex] = entity;
          componentArray[newIndex] = component;
          size++;
        }

        void remove(Entities::Entity entity) {
          size_t indexOfRemoved = entityToIndex[entity];
          size_t indexOfLast = size - 1;
          Entities::Entity lastEntity = indexToEntity[indexOfLast];
          entityToIndex[lastEntity] = indexOfRemoved;
          indexToEntity[indexOfRemoved] = lastEntity;

          entityToIndex.erase(entity);
          indexToEntity.erase(indexOfLast);

          size--;
        }

        T &get(Entities::Entity entity) {
          componentArray[entityToIndex[entity]];
        }

        void destroyedEntity(Entities::Entity entity) override {
          remove(entity);
        }

      private:
        std::array<T, Entities::MAX_ENTITIES> componentArray;
        std::unordered_map<Entities::Entity, size_t> entityToIndex;
        std::unordered_map<size_t, Entities::Entity> indexToEntity;
        size_t size;
    };

    class ComponentManager {
      public:
        template<typename T> void registerComponent() {
          const char *typeName = typeid(T).name();
          componentTypes.insert({typeName, nextComponentType});
          componentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});
          nextComponentType++;
        }

        template<typename T>ComponentType getComponentType() {
          const char *typeName = typeid(T).name();
          return componentTypes[typeName];
        }

        template<typename T>void addComponent(Entities::Entity entity, T component) {
          getComponentArray<T>()->insert(entity, component);
        }

        template<typename T>void removeComponent(Entities::Entity entity) {
          getComponentArray<T>()->remove(entity);
        }

        template<typename T>T& getComponent(Entities::Entity entity) {
          return getComponentArray<T>()->get(entity);
        }

        void destroyedEntity(Entities::Entity entity) {
          for (auto const &pair: componentArrays) {
            auto const &component = pair.second;
            component->destroyedEntity(entity);
          }
        }

      private:
        std::unordered_map<const char *, ComponentType> componentTypes{};
        std::unordered_map<const char *, std::shared_ptr<ComponentArrayInterface>> componentArrays{};
        ComponentType nextComponentType{};

        template<typename T> std::shared_ptr<ComponentArray<T>> getComponentArray() {
          const char *typeName = typeid(T).name();
          return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
        }
    };
  };
};

#endif
