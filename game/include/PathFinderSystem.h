#pragma once
#include "systems\system.h"
#include "entities.h"

namespace NN {
	class Engine;
}

typedef struct _TargetEntity {
	enum TargetState {
		SEEKING,
		STOPPED
	};

	NN::Entities::Entity target;
	double stopDistanace;
	enum TargetState state;
} TargetEntity;

class PathFinderSystem : public NN::Systems::System {
	public:
		void update(NN::Engine* engine, double frameTime) override;
};

