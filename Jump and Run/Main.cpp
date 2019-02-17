
#include <own/modelloader.hpp>
#include "Map.hpp"
#include "Player.hpp"
#include "Physics.hpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, physics::movementX &movX) {
	movX = static_cast<physics::movementX>(0);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		movX = static_cast<physics::movementX>(movX - 1);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		movX = static_cast<physics::movementX>(movX + 1);
}

int main(int argc, char* argv[]) {

	console::printInfo("Running from " + std::string(argv[0]));
	GLFWwindow* window;
	renderUtil::init::initGLFW(3);
	renderUtil::init::createWindow(window, 800, 600, "Jump and Run");
	renderUtil::init::initGLcommands();

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_callback);
	glfwSetKeyCallback(window, key_callback);

	renderUtil::ShaderEngine shader("shader.vert", "shader.frag");
	shader.use();

	modelLoader::ModelContainer models;
	renderUtil::TextureEngine textures;

	glm::mat4 projection = glm::ortho(0.0, 16.0, 0.0, 9.0, 0.1, 100.0);
	shader.setMat4("projection", projection);

	glm::mat4 view;
	shader.setMat4("view", view);

	gameMap::Block::model = models.addFromFile("block.obj");
	gameMap::Block::texture = textures.addFromFile("block.png");
	gamePlayer::Player player(glm::vec2(3, 6));

	int playerModel = models.addFromFile("player.obj");
	int playerTexture =textures.addFromFile("player.png");

	gameMap::Block solidBlock;

	gameMap::Map map(&models, &textures, &shader);
	physics::PhysicsHandler physics(&player,&map);

	for (int i = 0; i < 16; ++i) {
		map.addBlock(glm::uvec2(i, 2), &solidBlock);
	}

	map.addBlock(glm::uvec2(0, 0), &solidBlock);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	physics::movementX movX;

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		processInput(window, movX);
		physics.updatePhysics(movX);

		shader.use();
		map.renderMap();

		textures.use(playerTexture);
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(player.pos.x - player.width, player.pos.y - player.height, -2));
		shader.setMat4("model", model);
		models.draw(playerModel);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	
	//system("pause");
	return 0;
}