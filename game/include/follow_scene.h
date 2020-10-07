#pragma once

#include "engine/engine.h"
#include "scene/scene.h"

class PathFinderSystem;

class FollowScene : public NN::Scenes::Scene {
	public:
		FollowScene(NN::Engine *engine) : Scene(engine) {};

		void onCreate() override;
		void onDestroy() override {};
		void update(double frameTime) override;

	private:
		std::shared_ptr<PathFinderSystem> pathFinderSystem;
};

