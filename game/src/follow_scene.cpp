#include <cstdlib>
#include <ctime>

#include "follow_scene.h"

#include "entities.h"
#include "components.h"
#include "coordinator.h"
#include "world.h"
#include "PathFinderSystem.h"

static int worldMap[18*10] = {
  1,1,4,4,4,4,4,4,2,2,
  1,0,0,0,0,0,0,0,0,2,
  4,0,0,0,0,0,0,0,0,4,
  4,0,0,0,0,0,0,0,0,4,
  4,0,0,0,0,0,0,0,0,4,
  4,0,0,0,0,0,0,0,0,4,
  4,0,0,0,0,0,0,0,0,4,
  4,0,0,0,0,0,0,0,0,4,
  3,0,0,0,0,0,0,0,0,5,
  3,3,4,4,0,0,4,4,5,5,
  1,0,0,0,0,0,0,0,0,2,
  4,0,0,0,0,0,0,0,0,4,
  4,0,0,0,0,0,0,0,0,4,
  4,0,0,0,0,0,0,0,0,4,
  4,0,0,0,0,0,0,0,0,4,
  4,0,0,0,0,0,0,0,0,4,
  4,0,0,0,0,0,0,0,0,4,
  3,4,4,4,4,4,4,4,4,5,
};

void FollowScene::onCreate() {
	NN::Coordinator* coordinator = engine->getCoordinator();

    // Register the new component and the path finding system below
    coordinator->registerComponent<TargetEntity>();
    pathFinderSystem = coordinator->registerSystem<PathFinderSystem>();

    NN::Entities::Signature signature;
    signature.set(coordinator->getComponentType<NN::Components::Position>());
    signature.set(coordinator->getComponentType<TargetEntity>());
    coordinator->setSystemSignature<PathFinderSystem>(signature);

	world = new World(10, 18, (int*)&worldMap);

	NN::Entities::Entity currentPlayer = coordinator->createEntity();
	
    // We're creating a few things here...first a controllable player and then two penguins. The first
    // penguin follows the player and the second penguin follows the first penguin.
    engine->setCurrentPlayer(currentPlayer);
    coordinator->addComponent<NN::Components::Position>(currentPlayer, NN::Components::Position{ 5.5, 2 });
    coordinator->addComponent<NN::Components::Camera>(currentPlayer, NN::Components::Camera{ -1, 0, 0, 0.66 });
    coordinator->addComponent<NN::Components::Input>(currentPlayer, NN::Components::Input{ false,
                                                                                           false,
                                                                                           false,
                                                                                           false,
                                                                                           false,
                                                                                           false,
                                                                                           false,
                                                                                           false });
    coordinator->addComponent<NN::Components::Velocity>(currentPlayer, NN::Components::Velocity{ 0, 0, 5, 3 });

    NN::Entities::Entity penguin = coordinator->createEntity();
    coordinator->addComponent<NN::Components::Position>(penguin, NN::Components::Position{ 5.5, 16.5 });
	coordinator->addComponent<NN::Components::AnimatedSprite>(penguin, NN::Components::AnimatedSprite{ 11,
																									   11,
																									   11,
																									   0,
																									   64,
																									   64,
																									   1,
																									   1,
																									   0,
																									   true,
																									   {} });
    coordinator->addComponent<NN::Components::Velocity>(penguin, NN::Components::Velocity{ 0.0, 0.0, 3, 3 });
    coordinator->addComponent<TargetEntity>(penguin, TargetEntity{ currentPlayer, 1.0 });

    NN::Entities::Entity penguinFollower = coordinator->createEntity();
    coordinator->addComponent<NN::Components::Position>(penguinFollower, NN::Components::Position{ 8, 8 });
	coordinator->addComponent<NN::Components::AnimatedSprite>(penguinFollower, NN::Components::AnimatedSprite{ 11,
																									   11,
																									   11,
																									   0,
																									   64,
																									   64,
																									   2,
																									   2,
																									   64,
																									   true,
																									   {} });
    coordinator->addComponent<NN::Components::Velocity>(penguinFollower, NN::Components::Velocity{ 0.0, 0.0, 2, 3 });
    coordinator->addComponent<TargetEntity>(penguinFollower, TargetEntity{penguin, 0.5 });

    NN::Entities::Entity penguinFollower2 = coordinator->createEntity();
    coordinator->addComponent<NN::Components::Position>(penguinFollower2, NN::Components::Position{ 5.5, 16 });
	coordinator->addComponent<NN::Components::AnimatedSprite>(penguinFollower2, NN::Components::AnimatedSprite{ 11,
																									   11,
																									   11,
																									   0,
																									   64,
																									   64,
																									   3,
																									   3,
																									   64,
																									   true,
																									   {} });
    coordinator->addComponent<NN::Components::Velocity>(penguinFollower2, NN::Components::Velocity{ 0.0, 0.0, 2, 3 });
    coordinator->addComponent<TargetEntity>(penguinFollower2, TargetEntity{penguinFollower, 0.5 });

    std::srand(std::time(nullptr));
    // for (int i = 0; i < 10; i++) {
    //     for (int j = 0; j < 18; j++) {
    //         NN::Entities::Entity fire = coordinator->createEntity();
    //         coordinator->addComponent<NN::Components::Position>(fire, NN::Components::Position{ double(i) + 0.5, double(j) + 0.5 });
    //         coordinator->addComponent<NN::Components::AnimatedSprite>(fire, NN::Components::AnimatedSprite{ 13,
    //                                                                                                         13,
    //                                                                                                         13,
    //                                                                                                         0,
    //                                                                                                         64,
    //                                                                                                         64,
    //                                                                                                         1,
    //                                                                                                         1,
    //                                                                                                         64,
    //                                                                                                         false,
    //                                                                                                         {NN::Components::Animation{13, 13, (unsigned int)(std::rand() % 1000 + 1), 0}} });
    //     }
    // }
}

void FollowScene::update(double frameTime) {
    pathFinderSystem->update(engine, frameTime);
}
