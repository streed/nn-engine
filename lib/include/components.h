#ifndef NN_COMPONENTS_H
#define NN_COMPONENTS_H

#include <bitset>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <unordered_map>

#include "entities.h"

namespace NN {
  namespace Components {
    using ComponentType = std::uint8_t;
    const ComponentType MAX_COMPONENTS = 32;

    struct Camera {
      double dirX;
      double dirY;
      double planeX;
      double planeY;

      double getCameraX(int x, int screenWidth) const {
        return 2.0 * x / double(screenWidth) - 1;
      }

      double getRayDirX(double cameraX) const {
        return dirX + planeX * cameraX;
      }

      double getRayDirY(double cameraX) const {
        return dirY + planeY * cameraX;
      }

      double getInvDet() const {
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
    };

    struct Position {
      double posX;
      double posY;

      bool operator<(const Position& pos) const {
          if (posX < pos.posX) {
              return true;
          } else if (posX > pos.posX) {
              return false;
          } else {
              return posY < pos.posY;
          }
      }

      bool operator!=(const Position& pos) const {
          return !(*this == pos);
      }

      bool operator==(const Position& pos) const {
          return fabs(posX - pos.posX) < std::numeric_limits<double>::epsilon() &&
				 fabs(posY - pos.posY) < std::numeric_limits<double>::epsilon();
      }

    };

    struct Velocity {
      double velocityX;
      double velocityY;
      double maxSpeed;
      double maxRotateSpeed;
    };

    struct Rotation {
      double maxRotateSpeed;
    };

    struct ImpAiComponent {
      bool seeking;
      double searchDistance;
      double timeUntilNextShot;
      double shootingCoolDown;
    };

    struct Sprite {
      int textureIndex;
      int spriteWidth;
      int spriteHeight;
    };

    struct Animation {
        int startIndex;
        int endIndex;
        unsigned int msPerFrame;
        double timeRemainingOnFrame;
    };

    struct AnimatedSprite {
        int startTextureIndex;
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

        bool isValid() const {
            return spriteWidth != 0 && spriteHeight != 0;
        }
    };

    struct Input {
      bool forward;
      bool backward;
      bool strafeLeft;
      bool strafeRight;
      bool rotateLeft;
      bool rotateRight;
      bool quit;
      bool debug;
      bool shoot;
      bool weapon1;
      bool weapon2;
      bool weapon3;
      bool weapon4;
      bool interact;
    };


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
          if (entityToIndex.find(entity) != entityToIndex.end()) {
            remove(entity);
          }
        }

      private:
        std::array<T, Entities::MAX_ENTITIES> componentArray;
        std::unordered_map<Entities::Entity, size_t> entityToIndex;
        std::unordered_map<size_t, Entities::Entity> indexToEntity;
        size_t size = 0;
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
