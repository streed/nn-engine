#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "engine/engine.h"
#include "scene/scene.h"
#include "ui/ui_components.h"

class PathFinderSystem;

class PenguinShooterScene : public NN::Scenes::Scene {
	public:
		PenguinShooterScene(NN::Engine *engine);

		void onCreate() override;
		void onDestroy() override;
		void update(double frameTime) override;

	private:
		void setupMap();
		void setupPlayer();
		void setupPenguins();
		void setupWeapon();
		void setupHUD();

		void updatePenguinAI(double frameTime);
		void updateWeaponFiring(double frameTime);
		void updateHUD();
		void checkWinCondition();
		void checkPenguinAttacks(double frameTime);

		std::shared_ptr<PathFinderSystem> pathFinderSystem;

		// Game state
		NN::Entities::Entity weaponEntity = 0;
		std::vector<NN::Entities::Entity> penguins;
		int penguinsAlive = 0;
		int totalPenguins = 0;
		bool gameOver = false;
		bool playerWon = false;
		double gameOverTimer = 0.0;

		// HUD
		NN::UI::UICanvas hudCanvas;
		int healthBarId = -1;
		int ammoTextId = -1;
		int killCountId = -1;
		int messageId = -1;
		int crosshairId = -1;
};
