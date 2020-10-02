#ifndef __COMPONENTS__
#define __COMPONENTS__

#include <bitset>
#include <cstdlib>

#include "entities.h"

namespace NN {
  namespace Components {
    using ComponentType = std::uint8_t;
    const ComponentType = MAX_COMPONENTS = 32;
    using Signature = std::bitset<MAX_COMPONENTS>;

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
          size_t = newIndex = size;
          entityToIndexMap[entity] = newIndex;
          indexToEntity[newIndex] = entity;
          componentArray[newIndex] = component;
          size++;
        }

        void remove(Entities::Entity entity) {
          size_t indexOfRemoved = entityToIndex[entity];
          size_t indexOfLast = size - 1;
          Entities::Entity lastEntity = indexToEntity[indexOfLast];
          entityToIndexMap[lastEntity] = indexOfRemoved;
          indexToEntity[indexOfRemoved] = lastEntity;

          entityToIndexMap.erase(entity);
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
        std::unordered_map<size_t, Entity> indexToEntity;
        size_t size;
    };
  };
};

#endif
