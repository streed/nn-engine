#include <map>
#include <algorithm>
#include <limits>
#include <queue>
#include <cmath>

#include "PathFinderSystem.h"

#include "engine/engine.h"
#include "world.h"
#include "components.h"
#include "coordinator.h"
#include "entities.h"

using namespace NN;
using namespace NN::Components;

constexpr double PATH_EPSILON = 0.5;

static double distance(const Position &a, const Position &b);
static Position findNextPositionToMoveTo(const Position &current, const Position &target, World* world);
static std::vector<Position> getNeighbors(World* world, const Position &position);

void PathFinderSystem::update(NN::Engine *engine, double frameTime) {
	Coordinator* coordinator = engine->getCoordinator();
	World* world = engine->getWorld();

	for (auto const& entity : entities) {
		auto& position = coordinator->getComponent<Position>(entity);
		auto& velocity = coordinator->getComponent<Velocity>(entity);
		auto& target = coordinator->getComponent<TargetEntity>(entity);
		auto& targetPosition = coordinator->getComponent<Position>(target.target);

		double targetDistance = distance(position, targetPosition);
		if (targetDistance < target.stopDistance) {
			target.state = TargetEntity::STOPPED;
		} else {
			target.state = TargetEntity::SEEKING;
		}

		velocity.velocityX = 0.0;
		velocity.velocityY = 0.0;

		if (target.state == TargetEntity::SEEKING) {
			Position nextCellToMoveTo = findNextPositionToMoveTo({ double(int(position.posX)), double(int(position.posY)) },
																 { double(int(targetPosition.posX)), double(int(targetPosition.posY)) },
																 world);

			double diffX = (nextCellToMoveTo.posX + 0.5) - position.posX;
			double diffY = (nextCellToMoveTo.posY + 0.5) - position.posY;

			double length = std::sqrt(diffX * diffX + diffY * diffY);
			if (length > 0.0) {
				double moveSpeed = velocity.maxSpeed * frameTime;
				double dirX = diffX / length;
				double dirY = diffY / length;

				velocity.velocityX = dirX * moveSpeed;
				velocity.velocityY = dirY * moveSpeed;
			}
		}
	}
}

static double distance(const Position& a, const Position& b) {
	double diffX = a.posX - b.posX;
	double diffY = a.posY - b.posY;
	return std::sqrt(diffX * diffX + diffY * diffY);
}

static std::vector<Position> getNeighbors(World* world, const Position &position) {
	int x = int(position.posX);
	int y = int(position.posY);

	std::vector<Position> neighbors;
	neighbors.reserve(4);

	const Position candidates[4] = {
		{ double(x), double(y - 1) },
		{ double(x), double(y + 1) },
		{ double(x - 1), double(y) },
		{ double(x + 1), double(y) },
	};

	for (const auto& pt : candidates) {
		int px = int(pt.posX);
		int py = int(pt.posY);

		if (px >= 0 && py >= 0 && px < world->width && py < world->height
			&& world->isTraversable(px, py)) {
			neighbors.push_back(pt);
		}
	}

	return neighbors;
}

static Position findNextPositionToMoveTo(const Position &source, const Position &target, World* world) {
	// A* pathfinding using a priority queue for efficient minimum extraction
	using PqEntry = std::pair<double, Position>;
	auto cmp = [](const PqEntry& a, const PqEntry& b) { return a.first > b.first; };
	std::priority_queue<PqEntry, std::vector<PqEntry>, decltype(cmp)> openQueue(cmp);

	std::map<const Position, Position> cameFrom;
	std::map<const Position, double> gScore;

	gScore[source] = 0;
	openQueue.push({ distance(source, target), source });

	Position current;
	bool foundPath = false;

	while (!openQueue.empty()) {
		current = openQueue.top().second;
		openQueue.pop();

		if (std::fabs(current.posX - target.posX) < PATH_EPSILON
			&& std::fabs(current.posY - target.posY) < PATH_EPSILON) {
			foundPath = true;
			break;
		}

		for (const auto& neighbor : getNeighbors(world, current)) {
			double tentativeG = gScore[current] + 1.0;
			auto it = gScore.find(neighbor);

			if (it == gScore.end() || tentativeG < it->second) {
				cameFrom[neighbor] = current;
				gScore[neighbor] = tentativeG;
				double f = tentativeG + distance(neighbor, target);
				openQueue.push({ f, neighbor });
			}
		}
	}

	if (foundPath) {
		// Walk back from current to source, return the first step
		Position step = current;
		while (cameFrom.count(step) && !(cameFrom[step] == source)) {
			step = cameFrom[step];
		}
		return step;
	}

	return source;
}
