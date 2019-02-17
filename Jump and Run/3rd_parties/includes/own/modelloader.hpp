#pragma once
#include "renderutil.hpp"
#include <unordered_map>
#include <unordered_set>
#include <glm/gtx/hash.hpp>

namespace util {

	inline glm::vec3 stovec3(const char* str) {
		char* lastElement;
		glm::vec3 result;
		result.x = strtof(str, &lastElement);
		result.y = strtof(lastElement + 1, &lastElement);
		result.z = strtof(lastElement + 1, NULL);
		return result;
	}

	inline glm::vec2 stovec2(const char* str) {
		char* lastElement;
		glm::vec2 result;
		result.x = strtof(str,&lastElement);
		result.y = strtof(lastElement + 1, NULL);
		return result;
	}

	inline glm::ivec3 stoivec3(const char* str) {
		char* lastElement;
		glm::ivec3 result;

		result.x = strtol(str, &lastElement, 10);
		result.y = strtol(lastElement + 1, &lastElement, 10);
		result.z = strtol(lastElement + 1, NULL, 10);

		return result;
	}

	inline bool getline(std::streambuf*& sb, std::string& str) {
		str.clear();

		while (true) {
			int c = sb->sbumpc();
			switch (c) {
			case '\n': {
				if (str.empty())
					return false;
				return true;
			}
			case EOF: {
				if (str.empty())
					return false;
				return true;
			}
			default: {
				str += c;
			} break;
			}
		}
	}

	inline std::string strToken(std::string &str, std::string delim, size_t &pos) {
		size_t i = str.find_first_of(delim, pos) - pos;
		std::string result = str.substr(pos,i);
		pos += i;
		return result;
	}
	
	void getPointFromString(std::string &str, std::vector<glm::ivec3> &vec) {
		size_t lastElement = 0;
		vec.push_back(stoivec3(strToken(str, " f", lastElement).c_str()));
		vec.push_back(stoivec3(strToken(str, " f", ++lastElement).c_str()));
		vec.push_back(stoivec3(strToken(str, " f", ++lastElement).c_str()));
	}
}

namespace modelLoader {

	struct ModelData {
		std::vector<glm::vec3> verticies;
		std::vector<glm::vec2> textures;
		std::vector<glm::vec3> normals;
		std::vector<glm::uvec3> EBOs;

		ModelData(std::vector<glm::vec3> verticies, std::vector<glm::vec2> textures, std::vector<glm::vec3> normals, std::vector<glm::uvec3> EBOs)
			: verticies(verticies), textures(textures), normals(normals), EBOs(EBOs) {}
		ModelData(std::string AFile) {
			loadFromFile(AFile);
		}
		ModelData(){}

		void loadFromFile(std::string AFile) {

			std::ifstream inputFile;
			inputFile.open(AFile);
			if (!inputFile) {
				console::printError("Error reading model data", true);
			}

			std::vector<glm::vec3> inVectors;
			inVectors.push_back(glm::vec3(0, 0, 0));

			std::vector<glm::vec2> inTextures;
			inTextures.push_back(glm::vec2(0, 0));

			std::vector<glm::vec3> inNormals;
			inNormals.push_back(glm::vec3(0, 0, 0));

			std::vector<glm::ivec3> points;

			std::string currLine, cmd;
			char* tmp;
			std::streambuf* sb = inputFile.rdbuf();

			while (util::getline(sb, currLine)) {
				cmd = strtok_s(const_cast<char*>(currLine.c_str()), " ", &tmp);
				currLine = tmp;

				if (cmd == "v") {
					inVectors.push_back(util::stovec3(currLine.c_str()));
					continue;
				}

				if (cmd == "vt") {
					inTextures.push_back(util::stovec2(currLine.c_str()));
					continue;
				}

				if (cmd == "vn") {
					inNormals.push_back(util::stovec3(currLine.c_str()));
					continue;
				}

				if (cmd == "f") {
					util::getPointFromString(currLine, points);
					continue;
				}
			}

			inputFile.close();

			std::vector<glm::vec3> &outVectors = verticies;
			outVectors.reserve(inVectors.size() * 3);

			std::vector<glm::vec2> &outTextures = textures;
			outTextures.reserve(inTextures.size() * 3);

			std::vector<glm::vec3> &outNormals = normals;
			outNormals.reserve(normals.size() * 3);

			std::vector<glm::uvec3> &outEBO = EBOs;
			outEBO.reserve(static_cast<size_t>(points.size() / 3));
			GLuint currEBO = 0;

			std::unordered_map<glm::uvec3, GLuint> map_point(points.size());
			//std::unordered_map<glm::ivec3, GLuint>::iterator mit;
			std::pair<std::unordered_map<glm::uvec3, GLuint>::iterator, bool> mit2;
			int x = 0;
#ifdef DEBUG_MSG
			console::printInfo("Total points count: " + std::to_string(points.size()));
			int j = 0;
#endif
			for (auto &i : points) {
#ifdef DEBUG_MSG
				++j;
				if (!(j % 10000))
				{
					console::print("\rCurrent Point Nr: " + std::to_string(j), false);
				}
#endif

				/*mit = map_point.find(i);
				if (mit != map_point.end()) {
					outEBO.push_back(mit->second);
					continue;
				}*/

				GLuint *e = NULL;

				switch (x) {
				case 0: {
					++x;
					outEBO.push_back(glm::uvec3(0, 0, 0));
					e = &(outEBO.back().x);
				} break;
				case 1: {
					++x;
					e = &(outEBO.back().y);
				} break;
				case 2: {
					x = 0;
					e = &(outEBO.back().z);
				} break;
				default: {
					x = 0;
				} break;
				}


				mit2 = map_point.emplace(i, currEBO);
				if (!mit2.second) {
					*e = mit2.first->second;
					continue;
				}
				*e = currEBO;
				++currEBO;

				// Vector Coords
				if (i.x != 0) {
					outVectors.push_back(inVectors[i.x]);
				}

				// Texture Coords
				if (i.y != 0) {
					outTextures.push_back(inTextures[i.y]);
				}

				// Normal Coords
				if (i.z != 0) {
					outNormals.push_back(inNormals[i.z]);
				}
			}

#ifdef DEBUG_MSG
			console::linebreak();
#endif
		}
	};

	class ModelContainer {
	private:
		// Model Container <VAO id, EBO count> | EBO Count is needed for drawing
		std::unordered_map<GLuint, GLuint> models;
		// Loaded Filenames <Path, VAO id> | To prevent double-loading
		std::unordered_map<std::string, GLuint> loadedFiles;

		GLuint initializeData(ModelData &data, int shaderLayer, GLenum type = GL_STATIC_DRAW) {
			GLuint vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

				GLuint vbo[3];
				glGenBuffers(3,vbo);

				if (data.verticies.size() > 0) {
					glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
					glBufferData(GL_ARRAY_BUFFER, data.verticies.size() * sizeof(glm::vec3), data.verticies.data(), type);
					glVertexAttribPointer(shaderLayer, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
					glEnableVertexAttribArray(shaderLayer);
				}

				if (data.textures.size() > 0) {
					glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
					glBufferData(GL_ARRAY_BUFFER, data.textures.size() * sizeof(glm::vec2), data.textures.data(), type);
					glEnableVertexAttribArray(shaderLayer + 1);
					glVertexAttribPointer(shaderLayer + 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
				}

				if (data.normals.size() > 0) {
					glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
					glBufferData(GL_ARRAY_BUFFER, data.normals.size() * sizeof(glm::vec3), data.normals.data(), type);
					glEnableVertexAttribArray(shaderLayer + 2);
					glVertexAttribPointer(shaderLayer + 2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
				}

				GLuint ebo;
				glGenBuffers(1, &ebo);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.EBOs.size() * sizeof(glm::uvec3), data.EBOs.data(), type);
			
			glBindVertexArray(0);

			return vao;
		}
	public:
		~ModelContainer() {
			for (auto &i : models ) {
				glDeleteVertexArrays(1, &(i.first));
			}
		}

		// Adds Model from File | Prevents double loading based on path
		int addFromFile(std::string AFile, GLenum type = GL_STATIC_DRAW) {
			auto fc = loadedFiles.find(AFile);
			if (fc != loadedFiles.end()) {
				return fc->second;
			}

			if (util::fileExt(AFile) == ".obj") {
				ModelData data(AFile);
				GLuint id = initializeData(data, 0, type);
				loadedFiles.emplace(AFile, id);
				return models.emplace(id, data.EBOs.size() * 3).first->first;
			}
			else {
				console::printWarn("Dont support File Extension");
				return -1;
			}
		}

		// Adds Model from modelLoader::ModelData | WARNING: does not prevent double-loading
		int addFromData(ModelData &data, GLenum type = GL_STATIC_DRAW) {
			GLuint id = initializeData(data, 0, type);
			return models.emplace(id, data.EBOs.size()).first->first;
		}

		// Draws by id
		void draw(size_t id, GLenum mode = GL_TRIANGLES) {
			if (models.find(id) == models.end()) {
				return;
			}
			glBindVertexArray(id);
			glDrawElements(mode, models.at(id), GL_UNSIGNED_INT, 0);
		}
	};
}