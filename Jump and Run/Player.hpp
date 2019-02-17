#pragma once
#include <glm/glm.hpp>


namespace gamePlayer {
	class Player {
	public:
		glm::vec2 pos;
		glm::vec2 velocity;
		glm::vec2 acceleration = glm::vec2(0.0, -0.01);
		float width = 0.5, height = 1;

		Player(glm::vec2 pos)
			: pos(pos){}

		const float runSpeed = 0.3f;
	};
}