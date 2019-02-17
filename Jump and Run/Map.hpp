#pragma once
#include <unordered_map>
#include <glm/glm.hpp>
#include <own/modelloader.hpp>

namespace gameMap {
	class Block {
	private:

	public:
		static int model, texture;

		bool isCollision(glm::vec2 pos) {
			return true;
		}
	};

	int Block::model = 0, Block::texture = 0;

	class Map {
	private:
		std::unordered_map<glm::uvec2, Block*> mapGrid;
		modelLoader::ModelContainer* modelContainer;
		renderUtil::TextureEngine* textureContainer;
		renderUtil::ShaderEngine* shader;
	public:
		Map(modelLoader::ModelContainer* container, renderUtil::TextureEngine* textureContainer, renderUtil::ShaderEngine* shader)
			: modelContainer(container), textureContainer(textureContainer), shader(shader) {}

		bool addBlock(glm::uvec2 pos, Block* block) {
			if (mapGrid.find(pos) != mapGrid.end())
				return false;
			mapGrid.emplace(pos, block);
			return true;
		}

		bool replaceBlock(glm::uvec2 pos, Block* block) {
			if (mapGrid.find(pos) == mapGrid.end())
				return false;
			mapGrid.at(pos) = block;
		}

		bool getCollision(glm::uvec2 pos) {
			if (mapGrid.find(pos) == mapGrid.end())
				return false;
			return mapGrid.at(pos)->isCollision(pos);
		}

		void renderMap() {
			for (auto &i : mapGrid) {
				textureContainer->use(i.second->texture);
				glm::mat4 model;
				model = glm::translate(model, glm::vec3(i.first.x, i.first.y, -2));
				shader->setMat4("model", model);
				modelContainer->draw(i.second->model);
			}
		}
	};
}