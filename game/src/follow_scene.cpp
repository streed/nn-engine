#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

#include "follow_scene.h"

#include "entities.h"
#include "components.h"
#include "coordinator.h"
#include "world.h"
#include "scene/map_loader.h"
#include "PathFinderSystem.h"
#include "weapons/weapon_components.h"
#include "weapons/weapon_system.h"
#include "ui/ui_system.h"

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

FollowScene::FollowScene(NN::Engine *engine, const std::string &mapFile)
  : Scene(engine), mapFile(mapFile) {}

void FollowScene::onCreate() {
	NN::Coordinator* coordinator = engine->getCoordinator();

	coordinator->registerComponent<TargetEntity>();
	pathFinderSystem = coordinator->registerSystem<PathFinderSystem>();

	NN::Entities::Signature signature;
	signature.set(coordinator->getComponentType<NN::Components::Position>());
	signature.set(coordinator->getComponentType<TargetEntity>());
	coordinator->setSystemSignature<PathFinderSystem>(signature);

	if (!mapFile.empty()) {
		createFromMapFile();
	} else {
		createHardcoded();
	}

	setupWeapons();
	setupHUD();
}

void FollowScene::createFromMapFile() {
	NN::Coordinator* coordinator = engine->getCoordinator();
	NN::Scenes::MapData mapData = NN::Scenes::MapLoader::loadFromFile(mapFile);

	if (mapData.width == 0 || mapData.tiles.empty()) {
		std::cerr << "Failed to load map, falling back to hardcoded" << std::endl;
		createHardcoded();
		return;
	}

	// Create world from loaded map data
	int *tiles = new int[mapData.tiles.size()];
	std::copy(mapData.tiles.begin(), mapData.tiles.end(), tiles);
	world = new World(mapData.width, mapData.height, tiles);

	// Pass 1: Create all entities and register their names
	namedEntities.clear();
	std::vector<std::pair<NN::Entities::Entity, const NN::Scenes::EntityDef*>> createdEntities;

	for (const auto &entityDef : mapData.entities) {
		NN::Entities::Entity entity = coordinator->createEntity();

		if (!entityDef.name.empty()) {
			namedEntities[entityDef.name] = entity;
		}

		coordinator->addComponent<NN::Components::Position>(entity,
			NN::Components::Position{ entityDef.posX, entityDef.posY });

		if (entityDef.type == "player") {
			engine->setCurrentPlayer(entity);
			coordinator->addComponent<NN::Components::Camera>(entity,
				NN::Components::Camera{ entityDef.dirX, entityDef.dirY, entityDef.planeX, entityDef.planeY });
			coordinator->addComponent<NN::Components::Input>(entity,
				NN::Components::Input{ false, false, false, false, false, false, false, false });
			coordinator->addComponent<NN::Components::Velocity>(entity,
				NN::Components::Velocity{ 0, 0, entityDef.maxSpeed, entityDef.maxRotateSpeed });
		} else {
			if (entityDef.textureIndex >= 0) {
				coordinator->addComponent<NN::Components::AnimatedSprite>(entity,
					NN::Components::AnimatedSprite{
						entityDef.textureIndex,
						entityDef.textureIndex,
						entityDef.textureIndex,
						0,
						entityDef.spriteWidth,
						entityDef.spriteHeight,
						entityDef.spriteScaleX,
						entityDef.spriteScaleY,
						entityDef.vMove,
						true,
						{}
					});
			}

			if (entityDef.type == "npc") {
				coordinator->addComponent<NN::Components::Velocity>(entity,
					NN::Components::Velocity{ 0.0, 0.0, entityDef.maxSpeed, entityDef.maxRotateSpeed });
			}
		}

		createdEntities.push_back({ entity, &entityDef });
	}

	// Pass 2: Resolve follow targets by name and add TargetEntity components
	for (const auto &[entity, entityDef] : createdEntities) {
		if (!entityDef->followTarget.empty()) {
			auto targetIt = namedEntities.find(entityDef->followTarget);
			if (targetIt != namedEntities.end()) {
				coordinator->addComponent<TargetEntity>(entity,
					TargetEntity{ targetIt->second, entityDef->stopDistance });
			} else {
				std::cerr << "Warning: Follow target '" << entityDef->followTarget
						  << "' not found for entity '" << entityDef->name << "'" << std::endl;
			}
		}
	}
}

void FollowScene::createHardcoded() {
	NN::Coordinator* coordinator = engine->getCoordinator();

	world = new World(10, 18, (int*)&worldMap);

	NN::Entities::Entity currentPlayer = coordinator->createEntity();
	engine->setCurrentPlayer(currentPlayer);
	coordinator->addComponent<NN::Components::Position>(currentPlayer, NN::Components::Position{ 5.5, 2 });
	coordinator->addComponent<NN::Components::Camera>(currentPlayer, NN::Components::Camera{ -1, 0, 0, 0.66 });
	coordinator->addComponent<NN::Components::Input>(currentPlayer, NN::Components::Input{ false, false, false, false, false, false, false, false });
	coordinator->addComponent<NN::Components::Velocity>(currentPlayer, NN::Components::Velocity{ 0, 0, 5, 3 });

	NN::Entities::Entity penguin = coordinator->createEntity();
	coordinator->addComponent<NN::Components::Position>(penguin, NN::Components::Position{ 5.5, 16.5 });
	coordinator->addComponent<NN::Components::AnimatedSprite>(penguin, NN::Components::AnimatedSprite{ 11, 11, 11, 0, 64, 64, 1, 1, 0, true, {} });
	coordinator->addComponent<NN::Components::Velocity>(penguin, NN::Components::Velocity{ 0.0, 0.0, 3, 3 });
	coordinator->addComponent<TargetEntity>(penguin, TargetEntity{ currentPlayer, 1.0 });

	NN::Entities::Entity penguinFollower = coordinator->createEntity();
	coordinator->addComponent<NN::Components::Position>(penguinFollower, NN::Components::Position{ 8, 8 });
	coordinator->addComponent<NN::Components::AnimatedSprite>(penguinFollower, NN::Components::AnimatedSprite{ 11, 11, 11, 0, 64, 64, 2, 2, 64, true, {} });
	coordinator->addComponent<NN::Components::Velocity>(penguinFollower, NN::Components::Velocity{ 0.0, 0.0, 2, 3 });
	coordinator->addComponent<TargetEntity>(penguinFollower, TargetEntity{ penguin, 0.5 });

	NN::Entities::Entity penguinFollower2 = coordinator->createEntity();
	coordinator->addComponent<NN::Components::Position>(penguinFollower2, NN::Components::Position{ 5.5, 16 });
	coordinator->addComponent<NN::Components::AnimatedSprite>(penguinFollower2, NN::Components::AnimatedSprite{ 11, 11, 11, 0, 64, 64, 3, 3, 64, true, {} });
	coordinator->addComponent<NN::Components::Velocity>(penguinFollower2, NN::Components::Velocity{ 0.0, 0.0, 2, 3 });
	coordinator->addComponent<TargetEntity>(penguinFollower2, TargetEntity{ penguinFollower, 0.5 });
}

void FollowScene::onDestroy() {
	engine->getUISystem()->removeCanvas(&hudCanvas);
}

void FollowScene::setupWeapons() {
	NN::Coordinator* coordinator = engine->getCoordinator();

	// Add health to the player
	coordinator->addComponent<NN::Components::Health>(engine->getCurrentPlayer(),
		NN::Components::Health{ 100.0, 100.0 });

	// Add health to all penguins (anything with AnimatedSprite + Position)
	// Register them as hittable for the weapon system
	auto weaponSys = engine->getWeaponSystem();

	// Create a hitscan weapon entity
	NN::Entities::Entity weaponEntity = coordinator->createEntity();
	coordinator->addComponent<NN::Components::Weapon>(weaponEntity,
		NN::Components::Weapon{
			NN::Components::WeaponType::HITSCAN,
			25.0,    // damage
			50.0,    // range
			0.3,     // fire rate (seconds between shots)
			0.0,     // cooldown remaining
			false,   // firing
			-1,      // ammo (infinite)
			-1,      // max ammo
			0.0,     // projectile speed (N/A for hitscan)
			-1,      // projectile texture (N/A)
			0.2,     // projectile radius (N/A)
			-1,      // weapon texture
			-1,      // muzzle flash texture
			0.0,     // muzzle flash timer
			0.05,    // muzzle flash duration
			0        // slot
		});

	// Create a projectile weapon entity
	NN::Entities::Entity rocketEntity = coordinator->createEntity();
	coordinator->addComponent<NN::Components::Weapon>(rocketEntity,
		NN::Components::Weapon{
			NN::Components::WeaponType::PROJECTILE,
			50.0,    // damage
			30.0,    // range
			1.0,     // fire rate
			0.0,     // cooldown remaining
			false,   // firing
			10,      // ammo
			10,      // max ammo
			8.0,     // projectile speed
			12,      // projectile texture (fireball)
			0.3,     // projectile radius
			-1,      // weapon texture
			-1,      // muzzle flash texture
			0.0,     // muzzle flash timer
			0.1,     // muzzle flash duration
			1        // slot
		});
}

void FollowScene::setupHUD() {
	using namespace NN::UI;

	// Crosshair at screen center
	hudCanvas.addCrosshair(Color(0, 255, 0, 200), 8, 2, 3);

	// Health bar at bottom-left
	healthBarId = hudCanvas.addBar(
		10, -30, 150, 16,
		100.0, 100.0,
		Color(200, 30, 30, 220),
		Color(60, 60, 60, 180),
		Anchor::BOTTOM_LEFT, 5);

	// Health label
	hudCanvas.addText(10, -50, "HP", Color(255, 255, 255, 200), 14,
		Anchor::BOTTOM_LEFT, 10);

	// Ammo text at bottom-right
	ammoTextId = hudCanvas.addText(
		-100, -30, "AMMO: INF",
		Color(255, 255, 100, 220), 14,
		Anchor::BOTTOM_RIGHT, 10);

	// Weapon indicator at bottom-center
	hudCanvas.addText(
		-30, -30, "[1] Hitscan  [2] Rocket",
		Color(200, 200, 200, 180), 12,
		Anchor::BOTTOM_CENTER, 10);

	// Register the canvas with the UI system
	engine->getUISystem()->addCanvas(&hudCanvas);
}

void FollowScene::update(double frameTime) {
	pathFinderSystem->update(engine, frameTime);

	// Sync weapon firing state with player input
	NN::Coordinator* coordinator = engine->getCoordinator();
	auto &input = coordinator->getComponent<NN::Components::Input>(engine->getCurrentPlayer());

	// Update weapon firing state from input
	// For simplicity, update all weapon entities' firing state
	for (auto const &entity : engine->getWeaponSystem()->entities) {
		auto &weapon = coordinator->getComponent<NN::Components::Weapon>(entity);
		weapon.firing = input.shoot;
	}

	// Update HUD health bar
	if (healthBarId >= 0) {
		auto &health = coordinator->getComponent<NN::Components::Health>(engine->getCurrentPlayer());
		auto &bar = hudCanvas.getElement(healthBarId);
		bar.value = health.current;
		bar.maxValue = health.max;
	}
}
