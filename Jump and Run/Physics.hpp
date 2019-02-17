#pragma once
#include "Map.hpp"
#include "Player.hpp"


namespace physics {

	enum movementX {LEFT = -1, STILLX = 0, RIGHT = 1};
	enum movementY {UP = 1, STILLY = 0, DOWN = -1};

	class PhysicsHandler {
	private:
		gamePlayer::Player* player;
		gameMap::Map* map;
	public:
		PhysicsHandler(gamePlayer::Player* player, gameMap::Map* map)
			: player(player), map(map){}

		void updatePhysics(movementX movX) {
			glm::vec2 newPos = player->pos + player->velocity;

			if (map->getCollision(glm::uvec2(trunc(player->pos.x - player->width), trunc(newPos.y - player->height)))) {
				newPos = glm::vec2(newPos.x, trunc(newPos.y) + 1);
			}

			player->velocity = glm::vec2(player->runSpeed * movX, 0);
			player->velocity += player->acceleration;
			player->pos = newPos;
		}
	};
}