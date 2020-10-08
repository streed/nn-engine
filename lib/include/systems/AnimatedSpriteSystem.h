#pragma once
#include "systems/system.h"

namespace NN {
	class Engine;
	namespace Systems::BuiltIns {
		class AnimatedSpriteSystem : public System {
			public:
				void update(NN::Engine* engine, double frameTime);
		};
	}
}

