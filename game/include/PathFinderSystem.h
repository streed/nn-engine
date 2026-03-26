#pragma once
#include "systems/system.h"
#include "entities.h"

namespace NN {
	class Engine;
}

struct TargetEntity {
	enum TargetState {
		SEEKING,
		STOPPED
	};

	NN::Entities::Entity target;
	double stopDistance;
	enum TargetState state = SEEKING;

	// Cached pathfinding result
	double cachedNextX = 0.0;
	double cachedNextY = 0.0;
	double pathTimer = 0.0;        // time until next pathfind
	bool hasCachedPath = false;
};

class PathFinderSystem : public NN::Systems::System {
	public:
		void update(NN::Engine* engine, double frameTime) override;

	private:
		static constexpr double PATH_UPDATE_INTERVAL = 0.25; // recalculate every 250ms
};
