#ifndef NN_ENGINE_H
#define NN_ENGINE_H

#include <memory>

#include "entities.h"

class World;

namespace NN {
  namespace Systems {
    namespace Graphics {
      class RenderSystem;
    }

    namespace BuiltIns {
      class InputSystem;
      class PlayerMovementSystem;
      class PhysicsSystem;
      class SpriteSystem;
      class AnimatedSpriteSystem;
    }

    namespace Weapons {
      class WeaponSystem;
      class ProjectileSystem;
    }
  }

  namespace UI {
    class UISystem;
    class UICanvas;
  }

  namespace Scenes {
    class SceneStateMachine;
  }

  class Coordinator;
  class Config;

  class Engine {
    public:
      Engine(Config *config);
      ~Engine();
      void setup();
      void run();

      Coordinator *getCoordinator();
      void setCurrentPlayer(Entities::Entity entity);
      Entities::Entity getCurrentPlayer();
      Config *getConfig();
      std::shared_ptr<Systems::Graphics::RenderSystem> getRenderSystem();
      World *getWorld();

      void setSceneStateMachine(std::shared_ptr<Scenes::SceneStateMachine> sceneStateMachine);
      std::shared_ptr<Scenes::SceneStateMachine> getSceneStateMachine();

      // UI
      UI::UISystem *getUISystem();

      // Weapons
      std::shared_ptr<Systems::Weapons::WeaponSystem> getWeaponSystem();
      std::shared_ptr<Systems::Weapons::ProjectileSystem> getProjectileSystem();

    private:
      void render();
      void processEvents();

      bool quit;
      bool debug;

      Config *config;
      std::unique_ptr<Coordinator> coordinator;

      static const int DESIRED_FPS = 120;
      static const int GAME_LOOP_TICKS = 1000 / DESIRED_FPS;

      double currentFrameTime;
      double oldFrameTime;

      Entities::Entity currentPlayer;

      std::shared_ptr<Scenes::SceneStateMachine> sceneStateMachine;
      std::shared_ptr<Systems::Graphics::RenderSystem> renderSystem;
      std::shared_ptr<Systems::BuiltIns::InputSystem> inputSystem;
      std::shared_ptr<Systems::BuiltIns::PlayerMovementSystem> playerMovementSystem;
      std::shared_ptr<Systems::BuiltIns::PhysicsSystem> physicsSystem;
      std::shared_ptr<Systems::BuiltIns::AnimatedSpriteSystem> animatedSpriteSystem;
      std::shared_ptr<Systems::BuiltIns::SpriteSystem> spriteSystem;
      std::shared_ptr<Systems::Weapons::WeaponSystem> weaponSystem;
      std::shared_ptr<Systems::Weapons::ProjectileSystem> projectileSystem;

      std::unique_ptr<UI::UISystem> uiSystem;
  };
}

#endif
