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
	enum TargetState state;
};

class PathFinderSystem : public NN::Systems::System {
	public:
		void update(NN::Engine* engine, double frameTime) override;
};
