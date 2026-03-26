#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "penguin_shooter_scene.h"

#include "engine/config.h"
#include "entities.h"
#include "components.h"
#include "coordinator.h"
#include "world.h"
#include "PathFinderSystem.h"
#include "weapons/weapon_components.h"
#include "weapons/weapon_system.h"
#include "ui/ui_system.h"

// A bigger arena map for the shooter
// Note: tiles marked 3 at door positions are wall placeholders that become doors
static int shooterMap[20 * 20] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,2,2,0,0,0,0,0,0,0,0,0,0,3,3,0,0,1,
	1,0,0,2,2,0,0,0,0,0,0,0,0,0,0,3,3,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,4,4,3,3,4,4,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,4,0,0,0,0,4,0,0,0,0,0,0,1,
	1,0,0,0,0,0,3,0,0,0,0,0,0,3,0,0,0,0,0,1,
	1,0,0,0,0,0,3,0,0,0,0,0,0,3,0,0,0,0,0,1,
	1,0,0,0,0,0,0,4,0,0,0,0,4,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,4,4,3,3,4,4,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,5,5,0,0,0,0,0,0,0,0,0,0,6,6,0,0,1,
	1,0,0,5,5,0,0,0,0,0,0,0,0,0,0,6,6,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

PenguinShooterScene::PenguinShooterScene(NN::Engine *engine)
	: Scene(engine) {}

void PenguinShooterScene::onCreate() {
	NN::Coordinator* coordinator = engine->getCoordinator();

	coordinator->registerComponent<TargetEntity>();
	pathFinderSystem = coordinator->registerSystem<PathFinderSystem>();

	NN::Entities::Signature signature;
	signature.set(coordinator->getComponentType<NN::Components::Position>());
	signature.set(coordinator->getComponentType<TargetEntity>());
	coordinator->setSystemSignature<PathFinderSystem>(signature);

	setupMap();
	setupPlayer();
	setupPenguins();
	setupWeapon();
	setupHUD();

	std::srand(std::time(nullptr));
}

void PenguinShooterScene::onDestroy() {
	engine->getUISystem()->removeCanvas(&hudCanvas);
	delete world;
	world = nullptr;
}

void PenguinShooterScene::setupMap() {
	world = new World(20, 20, (int*)&shooterMap);

	// Add doors at the arena entrances (eagle texture = index 2)
	// North entrance (top wall, row 7, cols 9-10) - opens up
	world->addDoor(9, 7, true, 2, 0.5, 5.0);
	world->addDoor(10, 7, true, 2, 0.5, 5.0);

	// South entrance (bottom wall, row 12, cols 9-10) - opens down
	world->addDoor(9, 12, false, 2, 0.5, 5.0);
	world->addDoor(10, 12, false, 2, 0.5, 5.0);

	// West entrance (left wall, col 6, rows 9-10) - opens up
	world->addDoor(6, 9, true, 2, 0.5, 5.0);
	world->addDoor(6, 10, true, 2, 0.5, 5.0);

	// East entrance (right wall, col 13, rows 9-10) - opens down
	world->addDoor(13, 9, false, 2, 0.5, 5.0);
	world->addDoor(13, 10, false, 2, 0.5, 5.0);
}

void PenguinShooterScene::setupPlayer() {
	NN::Coordinator* coordinator = engine->getCoordinator();

	NN::Entities::Entity currentPlayer = coordinator->createEntity();
	engine->setCurrentPlayer(currentPlayer);

	// Start in the center of the arena
	coordinator->addComponent<NN::Components::Position>(currentPlayer,
		NN::Components::Position{ 10.5, 10.5 });
	coordinator->addComponent<NN::Components::Camera>(currentPlayer,
		NN::Components::Camera{ -1, 0, 0, 0.66 });
	coordinator->addComponent<NN::Components::Input>(currentPlayer,
		NN::Components::Input{ false, false, false, false, false, false, false, false, false, false, false, false, false, false });
	coordinator->addComponent<NN::Components::Velocity>(currentPlayer,
		NN::Components::Velocity{ 0, 0, 5, 3 });
	coordinator->addComponent<NN::Components::Health>(currentPlayer,
		NN::Components::Health{ 100.0, 100.0 });
}

void PenguinShooterScene::setupPenguins() {
	NN::Coordinator* coordinator = engine->getCoordinator();
	NN::Entities::Entity player = engine->getCurrentPlayer();

	// Penguin spawn positions around the map edges
	struct SpawnPoint { double x, y; };
	SpawnPoint spawns[] = {
		{ 2.5,  2.5 },
		{ 17.5, 2.5 },
		{ 2.5,  17.5 },
		{ 17.5, 17.5 },
		{ 10.5, 2.5 },
		{ 2.5,  10.5 },
		{ 17.5, 10.5 },
		{ 10.5, 17.5 },
	};

	totalPenguins = 8;
	penguinsAlive = totalPenguins;

	for (int i = 0; i < totalPenguins; i++) {
		NN::Entities::Entity penguin = coordinator->createEntity();
		penguins.push_back(penguin);

		coordinator->addComponent<NN::Components::Position>(penguin,
			NN::Components::Position{ spawns[i].x, spawns[i].y });

		coordinator->addComponent<NN::Components::AnimatedSprite>(penguin,
			NN::Components::AnimatedSprite{
				11, 11, 11, 0,
				64, 64,
				1.0, 1.0,
				0,
				true,
				{}
			});

		coordinator->addComponent<NN::Components::Velocity>(penguin,
			NN::Components::Velocity{ 0.0, 0.0, 2.0 + (i % 3) * 0.5, 3 });

		// All penguins chase the player
		coordinator->addComponent<TargetEntity>(penguin,
			TargetEntity{ player, 0.8 });

		coordinator->addComponent<NN::Components::Health>(penguin,
			NN::Components::Health{ 50.0, 50.0 });

		// Register as hittable for hitscan
		engine->getWeaponSystem()->hittableEntities.insert(penguin);
	}
}

void PenguinShooterScene::setupWeapon() {
	NN::Coordinator* coordinator = engine->getCoordinator();

	weaponEntity = coordinator->createEntity();
	coordinator->addComponent<NN::Components::Weapon>(weaponEntity,
		NN::Components::Weapon{
			NN::Components::WeaponType::HITSCAN,
			25.0,    // damage per shot
			50.0,    // range
			1.0,     // fire rate: once per second
			0.0,     // cooldown
			false,   // firing
			-1,      // infinite ammo
			-1,
			0.0, -1, 0.2,
			-1, -1,
			0.0, 0.08,
			0
		});
}

void PenguinShooterScene::setupHUD() {
	using namespace NN::UI;

	// Crosshair - Doom style dot
	crosshairId = hudCanvas.addCrosshair(Color(255, 255, 255, 255), 6, 2, 2);

	// Health bar - bottom left, Doom style
	hudCanvas.addRect(0, -60, 200, 60, Color(40, 40, 40, 200), Anchor::BOTTOM_LEFT, 1);
	hudCanvas.addText(10, -55, "HEALTH", Color(180, 180, 180, 255), 24, Anchor::BOTTOM_LEFT, 10);
	healthBarId = hudCanvas.addBar(10, -35, 180, 20,
		100.0, 100.0,
		Color(200, 0, 0, 255),
		Color(80, 0, 0, 200),
		Anchor::BOTTOM_LEFT, 5);

	// Kill counter - bottom right
	hudCanvas.addRect(-200, -60, 200, 60, Color(40, 40, 40, 200), Anchor::BOTTOM_RIGHT, 1);
	killCountId = hudCanvas.addText(-190, -45, "KILLS: 0 / 8",
		Color(255, 200, 50, 255), 28, Anchor::BOTTOM_RIGHT, 10);

	// Weapon name - bottom center
	hudCanvas.addText(-80, -25, "PENGUIN BLASTER",
		Color(200, 200, 200, 180), 22, Anchor::BOTTOM_CENTER, 10);

	// Game message (for win/lose) - center, initially invisible
	messageId = hudCanvas.addText(-100, -20, "",
		Color(255, 50, 50, 255), 32, Anchor::CENTER, 50);
	hudCanvas.getElement(messageId).visible = false;

	// Muzzle flash elements (initially invisible)
	// Screen tint
	int sw = engine->getConfig()->getScreenWidth();
	int sh = engine->getConfig()->getScreenHeight();
	muzzleFlashTintId = hudCanvas.addRect(0, 0, sw, sh,
		Color(255, 200, 100, 40), Anchor::TOP_LEFT, 90);
	hudCanvas.getElement(muzzleFlashTintId).visible = false;

	// Outer flash glow at bottom-center
	muzzleFlashOuterId = hudCanvas.addRect(-20, -70, 40, 60,
		Color(255, 240, 180, 220), Anchor::BOTTOM_CENTER, 91);
	hudCanvas.getElement(muzzleFlashOuterId).visible = false;

	// Inner bright core
	muzzleFlashInnerId = hudCanvas.addRect(-10, -65, 20, 45,
		Color(255, 255, 240, 240), Anchor::BOTTOM_CENTER, 92);
	hudCanvas.getElement(muzzleFlashInnerId).visible = false;

	engine->getUISystem()->addCanvas(&hudCanvas);
}

void PenguinShooterScene::update(double frameTime) {
	if (gameOver) {
		gameOverTimer += frameTime;
		// Allow quit after game over
		return;
	}

	// Run penguin pathfinding AI
	pathFinderSystem->update(engine, frameTime);

	// Handle weapon
	updateWeaponFiring(frameTime);

	// Check penguin attacks on player
	checkPenguinAttacks(frameTime);

	// Update HUD
	updateHUD();

	// Check win/lose
	checkWinCondition();

	// Update muzzle flash visibility based on weapon state
	NN::Coordinator* coordinator = engine->getCoordinator();
	auto &weapon = coordinator->getComponent<NN::Components::Weapon>(weaponEntity);
	bool flashActive = weapon.muzzleFlashTimer > 0.0;
	if (muzzleFlashOuterId >= 0) hudCanvas.getElement(muzzleFlashOuterId).visible = flashActive;
	if (muzzleFlashInnerId >= 0) hudCanvas.getElement(muzzleFlashInnerId).visible = flashActive;
	if (muzzleFlashTintId >= 0) hudCanvas.getElement(muzzleFlashTintId).visible = flashActive;

	if (flashActive) {
		double intensity = weapon.muzzleFlashTimer / weapon.muzzleFlashDuration;
		uint8_t outerAlpha = static_cast<uint8_t>(220 * intensity);
		uint8_t innerAlpha = static_cast<uint8_t>(240 * intensity);
		uint8_t tintAlpha = static_cast<uint8_t>(40 * intensity);
		hudCanvas.getElement(muzzleFlashOuterId).fillColor = NN::UI::Color(255, 240, 180, outerAlpha);
		hudCanvas.getElement(muzzleFlashInnerId).fillColor = NN::UI::Color(255, 255, 240, innerAlpha);
		hudCanvas.getElement(muzzleFlashTintId).fillColor = NN::UI::Color(255, 200, 100, tintAlpha);
	}

	// Update crosshair color: red on hit, yellow on fire, white default
	if (crosshairHitTimer > 0.0) {
		crosshairHitTimer -= frameTime;
		hudCanvas.getElement(crosshairId).crosshairColor = NN::UI::Color(255, 50, 50, 255);
	} else if (flashActive) {
		hudCanvas.getElement(crosshairId).crosshairColor = NN::UI::Color(255, 255, 50, 255);
	} else {
		hudCanvas.getElement(crosshairId).crosshairColor = NN::UI::Color(255, 255, 255, 255);
	}
}

void PenguinShooterScene::updateWeaponFiring(double frameTime) {
	NN::Coordinator* coordinator = engine->getCoordinator();
	auto &input = coordinator->getComponent<NN::Components::Input>(engine->getCurrentPlayer());
	auto &weapon = coordinator->getComponent<NN::Components::Weapon>(weaponEntity);

	weapon.firing = input.shoot;

	// Process damage events from weapon system
	auto &damageEvents = engine->getWeaponSystem()->getDamageEvents();
	for (const auto &event : damageEvents) {
		// Find if target is one of our penguins and has health
		auto it = std::find(penguins.begin(), penguins.end(), event.target);
		if (it != penguins.end()) {
			auto &health = coordinator->getComponent<NN::Components::Health>(event.target);
			health.takeDamage(event.damage);
			crosshairHitTimer = 0.15; // Flash crosshair red on hit

			if (health.isDead()) {
				// Remove penguin from tracking
				engine->getWeaponSystem()->hittableEntities.erase(event.target);

				// Make the sprite invisible by moving it far away (cheap despawn)
				auto &pos = coordinator->getComponent<NN::Components::Position>(event.target);
				pos.posX = -100;
				pos.posY = -100;

				// Zero out velocity so pathfinding doesn't move it
				auto &vel = coordinator->getComponent<NN::Components::Velocity>(event.target);
				vel.velocityX = 0;
				vel.velocityY = 0;
				vel.maxSpeed = 0;

				penguinsAlive--;
			}
		}
	}
}

void PenguinShooterScene::checkPenguinAttacks(double frameTime) {
	NN::Coordinator* coordinator = engine->getCoordinator();
	auto &playerPos = coordinator->getComponent<NN::Components::Position>(engine->getCurrentPlayer());
	auto &playerHealth = coordinator->getComponent<NN::Components::Health>(engine->getCurrentPlayer());

	for (auto const &penguin : penguins) {
		auto &health = coordinator->getComponent<NN::Components::Health>(penguin);
		if (health.isDead()) continue;

		auto &penguinPos = coordinator->getComponent<NN::Components::Position>(penguin);
		double dx = playerPos.posX - penguinPos.posX;
		double dy = playerPos.posY - penguinPos.posY;
		double dist = std::sqrt(dx * dx + dy * dy);

		// Penguins deal damage when close
		if (dist < 1.0) {
			playerHealth.takeDamage(15.0 * frameTime); // 15 DPS per penguin in contact
		}
	}
}

void PenguinShooterScene::updateHUD() {
	NN::Coordinator* coordinator = engine->getCoordinator();
	auto &playerHealth = coordinator->getComponent<NN::Components::Health>(engine->getCurrentPlayer());

	// Health bar
	if (healthBarId >= 0) {
		auto &bar = hudCanvas.getElement(healthBarId);
		bar.value = playerHealth.current;
		bar.maxValue = playerHealth.max;

		// Change color when low
		if (playerHealth.current < 30.0) {
			bar.barColor = NN::UI::Color(255, 50, 50, 255);
		} else if (playerHealth.current < 60.0) {
			bar.barColor = NN::UI::Color(255, 150, 0, 255);
		} else {
			bar.barColor = NN::UI::Color(200, 0, 0, 255);
		}
	}

	// Kill count
	if (killCountId >= 0) {
		int killed = totalPenguins - penguinsAlive;
		std::ostringstream ss;
		ss << "KILLS: " << killed << " / " << totalPenguins;
		hudCanvas.getElement(killCountId).text = ss.str();
	}
}

void PenguinShooterScene::checkWinCondition() {
	NN::Coordinator* coordinator = engine->getCoordinator();
	auto &playerHealth = coordinator->getComponent<NN::Components::Health>(engine->getCurrentPlayer());

	if (penguinsAlive <= 0 && !gameOver) {
		// Player wins!
		gameOver = true;
		playerWon = true;
		if (messageId >= 0) {
			auto &msg = hudCanvas.getElement(messageId);
			msg.text = "YOU WIN! ALL PENGUINS ELIMINATED!";
			msg.textColor = NN::UI::Color(50, 255, 50, 255);
			msg.visible = true;
		}
	} else if (playerHealth.isDead() && !gameOver) {
		// Player loses
		gameOver = true;
		playerWon = false;
		if (messageId >= 0) {
			auto &msg = hudCanvas.getElement(messageId);
			msg.text = "GAME OVER - THE PENGUINS WIN";
			msg.textColor = NN::UI::Color(255, 50, 50, 255);
			msg.visible = true;
		}

		// Stop player movement
		auto &vel = coordinator->getComponent<NN::Components::Velocity>(engine->getCurrentPlayer());
		vel.maxSpeed = 0;
		vel.maxRotateSpeed = 0;
	}
}
