#ifndef __COMPONENTS__
#define __COMPONENTS__

#include <bitset>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <unordered_map>

#include "entities.h"

namespace NN {
  namespace Components {
    using ComponentType = std::uint8_t;
    const ComponentType MAX_COMPONENTS = 32;

    typedef struct _Camera {
      double dirX;
      double dirY;
      double planeX;
      double planeY;

      double getCameraX(int x, int screenWidth) {
        return 2.0 * x / double(screenWidth) - 1;
      }

      double getRayDirX(double cameraX) {
        return dirX + planeX * cameraX;
      }

      double getRayDirY(double cameraX) {
        return dirY + planeY * cameraX;
      }

      double getInvDet() {
        return 1.0 / (planeX * dirY - dirX * planeY);
      }

      void rotate(double rotateSpeed) {
        double oldDirX = dirX;
        dirX = dirX * cos(rotateSpeed) - dirY * sin(rotateSpeed);
        dirY = oldDirX * sin(rotateSpeed) + dirY * cos(rotateSpeed);
        double oldPlaneX = planeX;
        planeX = planeX * cos(rotateSpeed) - planeY * sin(rotateSpeed);
        planeY = oldPlaneX * sin(rotateSpeed) + planeY * cos(rotateSpeed);
      }
    } Camera;

    typedef struct _Position {
      double posX;
      double posY;

      bool operator<(struct _Position& pos) const {
          return posX < pos.posX && posY < pos.posY;
      }

      bool operator<(const struct _Position& pos) const {
          if (posX < pos.posX) {
              return true;
          } else if (posX > pos.posX) {
              return false;
          } else {
              return posY < pos.posY;
          }
      }

      bool operator!=(const struct _Position& pos) {
          return !(*this == pos);
      }

      bool operator==(const struct _Position& pos) {
          return fabs(posX - pos.posX) < std::numeric_limits<double>::epsilon() &&
				 fabs(posY - pos.posY) < std::numeric_limits<double>::epsilon();
      }

    } Position;

    typedef struct _Velocity {
      double velocityX;
      double velocityY;
      double maxSpeed;
      double maxRotateSpeed;
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

    typedef struct _Sprite {
      int textureIndex;
      int spriteWidth;
      int spriteHeight;
    } Sprite;

    typedef struct _Animation {
        int startIndex;
        int endIndex;
        unsigned int msPerFrame;
        double timeRemainingOnFrame;
    } Animation;

    typedef struct _AnimatedSprite {
        int startTexturIndex;
        int endTextureIndex;
        int currentFrame;
        int currentAnimation;
        int spriteWidth;
        int spriteHeight;
        double spriteScaleX;
        double spriteScaleY;
        int vMove;
        bool actuallyStatic;
        Animation animations[10];

        bool isValid() {
            return spriteWidth != 0 && spriteHeight != 0;
        }
    } AnimatedSprite;

    typedef struct _Input {
      bool forward;
      bool backward;
      bool strafeLeft;
      bool strafeRight;
      bool rotateLeft;
      bool rotateRight;
      bool quit;
      bool debug;
    } Input;


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
          return componentArray[entityToIndex[entity]];
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
