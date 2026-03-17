#pragma once

#include <string>
#include <unordered_map>

#include "engine/engine.h"
#include "scene/scene.h"
#include "ui/ui_components.h"

class PathFinderSystem;

class FollowScene : public NN::Scenes::Scene {
	public:
		FollowScene(NN::Engine *engine, const std::string &mapFile = "");

		void onCreate() override;
		void onDestroy() override;
		void update(double frameTime) override;

	private:
		void createFromMapFile();
		void createHardcoded();
		void setupHUD();
		void setupWeapons();

		std::string mapFile;
		std::shared_ptr<PathFinderSystem> pathFinderSystem;
		std::unordered_map<std::string, NN::Entities::Entity> namedEntities;

		// HUD
		NN::UI::UICanvas hudCanvas;
		int healthBarId = -1;
		int ammoTextId = -1;
};
