#include <iostream>
using namespace std;
#include <map>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <queue>

#include "PathFinderSystem.h"

#include "engine/engine.h"
#include "world.h"
#include "components.h"
#include "coordinator.h"
#include "utils.h"
#include "entities.h"

using namespace NN;
using namespace NN::Components;

#define EPSILON 0.5

double distance(const Position &a, const Position &b);
double h(const Position &a, const Position &b);
Position getNextFromOpenSet(std::map<const Position, Position> &openSet, std::map<const Position, unsigned int> &fScore);
Position findNextPositionToMoveTo(const Position &current, const Position &target, World* world);
std::vector<Position> getNeighbors(World* world, const Position &poisition);
std::vector<Position> reconstructPath(std::map<const Position, Position>& cameFrom, const Position &start, Position end);

void PathFinderSystem::update(NN::Engine *engine, double frameTime) {
	Coordinator* coordinator = engine->getCoordinator();
	World* world = engine->getWorld();

	for (auto const& entity : entities) {
		auto& position = coordinator->getComponent<Position>(entity);
		auto& velocity = coordinator->getComponent<Velocity>(entity);
		auto& target = coordinator->getComponent<TargetEntity>(entity);
		auto& targetPosition = coordinator->getComponent<Position>(target.target);

		double targetDistance = distance(position, targetPosition);
		if (targetDistance < target.stopDistanace) {
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

			float diffX = (nextCellToMoveTo.posX + 0.5) - position.posX;
			float diffY = (nextCellToMoveTo.posY + 0.5) - position.posY;

			float length = (diffX * diffX) + (diffY * diffY);
			length = length * inverseSqrt(length);
			double moveSpeed = velocity.maxSpeed * frameTime;
			double dirX = diffX / double(length);
			double dirY = diffY / double(length);

			velocity.velocityX = dirX * moveSpeed;
			velocity.velocityY = dirY * moveSpeed;
		} else {
			//cout << entity << " found their target: " << target.target << endl;
		}
	}
}

double distance(const Position& a, const Position& b) {
	float diffX = float(a.posX - b.posX);
	float diffY = float(a.posY - b.posY);
	float length = (diffX * diffX) + (diffY * diffY);
	return double(length) * inverseSqrt(length);
}

double h(const Position& a, const Position& b) {
	return distance(a, b);
}

Position getNextFromOpenSet(std::map<const Position, Position> &openSet, std::map<const Position, unsigned int> &fScore) {
	std::vector<std::pair<unsigned int, Position> > fScorePositionTuples;

	for (auto& position : openSet) {
		unsigned int f = fScore[position.first];
		fScorePositionTuples.push_back(std::make_pair(f, position.first));
	}

	std::sort(fScorePositionTuples.begin(), fScorePositionTuples.end());

	return fScorePositionTuples.front().second;
}

std::vector<Position> getNeighbors(World* world, const Position &position) {
	double x = position.posX;
	double y = position.posY;

	std::vector<Position> potentialNeighbors;

	potentialNeighbors.push_back({ x, y - 1 });
	potentialNeighbors.push_back({ x, y + 1 });

	potentialNeighbors.push_back({ x - 1, y });
	potentialNeighbors.push_back({ x + 1, y });

	std::vector<Position> neighbors;

	for (auto const& potentialPoint : potentialNeighbors) {
		double px = potentialPoint.posX;
		double py = potentialPoint.posY;

		bool traversable = world->isTraversable(int(px), int(py));
		if (!((px < 0 || py < 0 || px >= world->width || py >= world->height) && !traversable)) {
			neighbors.push_back(potentialPoint);
		}
	}

	return neighbors;
}

std::vector<Position> reconstructPath(std::map<const Position, Position>& cameFrom, const Position& source, Position target) {
	std::vector<Position> path;
	path.push_back(target);

	while (target != source) {
		target = cameFrom[target];
		path.insert(path.begin(), target);
	}

	return path;
}

Position findNextPositionToMoveTo(const Position &source, const Position &target, World* world) {
	int cellX = int(target.posX);
	int cellY = int(target.posY);

	std::map<const Position, Position> openSet;
	std::map<const Position, Position> cameFrom;

	std::map<const Position, unsigned int> gScore;
	std::map<const Position, unsigned int> fScore;

	for (int x = 0; x < world->width; x++) {
		for (int y = 0; y < world->height; y++) {
			if (world->isTraversable(int(x), int(y))) {
				Position p = { x, y };
				gScore[p] = std::numeric_limits<unsigned int>::max();
				fScore[p] = std::numeric_limits<unsigned int>::max();
			}
		}
	}

	gScore[source] = 0;
	fScore[source] = double(h(source, target));

	bool foundPath = false;
	openSet[source] = source;

	Position current;
	while (!openSet.empty()) {
		current = getNextFromOpenSet(openSet, fScore);

		if (fabs(current.posX - target.posX) < EPSILON && fabs(current.posY - target.posY) < EPSILON) {
			foundPath = true;
			break;
		}

		openSet.erase(current);

		for (auto const& neighbor : getNeighbors(world, current)) {
			unsigned int tentativeGScore = gScore[current] + 1;

			if (tentativeGScore < gScore[neighbor]) {
				cameFrom[neighbor] = current;
				gScore[neighbor] = tentativeGScore;
				fScore[neighbor] = gScore[neighbor] + h(neighbor, target);

				if (openSet.count(neighbor) == 0) {
					openSet[neighbor] = neighbor;
				}
			}
		}
	}

	if (foundPath) {
		std::vector<Position> path = reconstructPath(cameFrom, source, current);

		if (path.size() >= 2) {
			return path.at(1);
		} else {
			return path.at(0);
		}
	} else {
		return source;
	}
}
