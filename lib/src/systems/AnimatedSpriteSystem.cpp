#include <iostream>
using namespace std;

#include "systems/AnimatedSpriteSystem.h"

#include "engine/engine.h"
#include "components.h"
#include "coordinator.h"

namespace NN::Systems::BuiltIns {
	void AnimatedSpriteSystem::update(NN::Engine* engine, double frameTime) {
		Coordinator* coordinator = engine->getCoordinator();
		for (const auto& entity : entities) {
			auto& animatedSprite = coordinator->getComponent<NN::Components::AnimatedSprite>(entity);

			if (!animatedSprite.actuallyStatic && animatedSprite.isValid()) {
				NN::Components::Animation& animation = animatedSprite.animations[animatedSprite.currentAnimation];
				double timeLeftInFrame = animation.timeRemainingOnFrame - frameTime;

				if (timeLeftInFrame <= 0.0) {
					// Reset the timer for the next frame...
					animation.timeRemainingOnFrame = animation.msPerFrame / 1000.0;
					
					// If we're past the end of the array then reset the frame
					// to the start of the animation.
					if (animatedSprite.currentFrame >= animation.endIndex) {
						animatedSprite.currentFrame = animation.startIndex;
					} else {
						animatedSprite.currentFrame++;
					}
				} else {
					animation.timeRemainingOnFrame = timeLeftInFrame;
				}
			}
		}
	}
}
