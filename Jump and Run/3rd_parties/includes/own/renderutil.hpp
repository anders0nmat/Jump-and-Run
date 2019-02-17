#pragma once
#ifndef STB_IMAGE_IMPLEMENTATION
	#define STB_IMAGE_IMPLEMENTATION
	#include "../stb_image.h"
#endif

#include "Helper.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace renderUtil {

	namespace init {

		void initGLFW(int version) {
			glfwInit();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_SAMPLES, 4);
		}

		void createWindow(GLFWwindow*& window, int x = 800, int y = 600, std::string title = "Title", GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL) {
			window = glfwCreateWindow(x, y, title.c_str(), monitor, share);
			if (window == NULL)
			{
				glfwTerminate();
				console::printError("Init: Window creation failed", true);
			}
			glfwMakeContextCurrent(window);
		}

		void initGLcommands() {
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				console::printError("Init: Initializing OpenGL Commands failed", true);
			}
		}
	}

	// A class containing all necessary features to control a shader program
	class ShaderEngine {
	public:
		GLuint ID = 0;

		ShaderEngine(std::string vertexPath, std::string fragmentPath) {
			std::string vertexCode, fragmentCode;
			std::ifstream vertexFile, fragmentFile;
			try {
				vertexFile.open(vertexPath);
				fragmentFile.open(fragmentPath);
				std::stringstream vertexString, fragmentString;
				vertexString << vertexFile.rdbuf();
				fragmentString << fragmentFile.rdbuf();
				vertexCode = vertexString.str();
				fragmentCode = fragmentString.str();
			}
			catch (std::ifstream::failure e) {
				console::printError("ShaderEngine: Opening Files failed | Vertex: [" + vertexPath + "] Fragment: [" + fragmentPath + "]");
			}
			const char* c_src_vertex = vertexCode.c_str();
			const char* c_src_fragment = fragmentCode.c_str();

			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

			glShaderSource(vertexShader, 1, &c_src_vertex, NULL);
			glCompileShader(vertexShader);

			int success;
			char infoLog[512];

			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success) {

				glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
				console::printError("ShaderEngine: Compiling Vertex Shader failed | Error Code:" + std::string(infoLog) + " | [" + vertexPath + "]");
			}

			glShaderSource(fragmentShader, 1, &c_src_fragment, NULL);
			glCompileShader(fragmentShader);

			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
				console::printError("ShaderEngine: Compiling Fragment Shader failed | Error Code:" + std::string(infoLog) + " | [" + fragmentPath + "]");
			}


			ID = glCreateProgram();
			glAttachShader(ID, vertexShader);
			glAttachShader(ID, fragmentShader);
			glLinkProgram(ID);
			glGetProgramiv(ID, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(ID, 512, NULL, infoLog);
				console::printError("ShaderEngine: Linking Shader Program failed | Error Code: " + std::string(infoLog));
			}
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
		}

		~ShaderEngine() {
			glDeleteProgram(ID);
		}

		void use() {
			glUseProgram(ID);
		}

		void setMat4(std::string uniform, glm::mat4 matrix) {
			glUniformMatrix4fv(glGetUniformLocation(ID, uniform.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
		}

		void setInt(std::string uniform, int value) {
			glUniform1i(glGetUniformLocation(ID, uniform.c_str()), value);
		}

		void setIVec2(std::string uniform, glm::ivec2 value) {
			glUniform2i(glGetUniformLocation(ID, uniform.c_str()), value.x, value.y);
		}

		void setIVec3(std::string uniform, glm::ivec3 value) {
			glUniform3i(glGetUniformLocation(ID, uniform.c_str()), value.x, value.y, value.z);
		}

		void setIVec4(std::string uniform, glm::ivec4 value) {
			glUniform4i(glGetUniformLocation(ID, uniform.c_str()), value.x, value.y, value.z, value.w);
		}

		void setUInt(std::string uniform, GLuint value) {
			glUniform1ui(glGetUniformLocation(ID, uniform.c_str()), value);
		}

		void setFloat(std::string uniform, float value) {
			glUniform1f(glGetUniformLocation(ID, uniform.c_str()), value);
		}

		void setVec2(std::string uniform, glm::vec2 value) {
			glUniform2f(glGetUniformLocation(ID, uniform.c_str()), value.x, value.y);
		}

		void setVec3(std::string uniform, glm::vec3 value) {
			glUniform3f(glGetUniformLocation(ID, uniform.c_str()), value.x, value.y, value.z);
		}

		void setVec4(std::string uniform, glm::vec4 value) {
			glUniform4f(glGetUniformLocation(ID, uniform.c_str()), value.x, value.y, value.z, value.w);
		}
	};

	// A class to load and use Textures
	class TextureEngine {
	private:
		// Loaded textures <Path, ID> | Used to prevent double loading
		std::unordered_map<std::string, GLuint> texture_map;
		// TexID matcher | Index refers to TexID
		std::vector<GLuint> textures;
	public:

		TextureEngine()
		{
			stbi_set_flip_vertically_on_load(true);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glActiveTexture(GL_TEXTURE0);
			textures.push_back(0);
			texture_map.emplace("NULL", 0);
		}

		~TextureEngine()
		{
			glDeleteTextures(textures.size(), textures.data());
		}

		GLuint addFromFile(std::string AFile)
		{
			auto found = texture_map.find(AFile);
			if (found != texture_map.end()) {
				return found->second;
			}

			GLuint texID;
			int width, height, nrChannels;
			unsigned char* texData = stbi_load(AFile.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
			if (texData) {
				glGenTextures(1, &texID);
				glBindTexture(GL_TEXTURE_2D, texID);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else {
				stbi_image_free(texData);
				console::printWarn("TextureManager: Failed to load File [" + AFile + "]");
				return 0;
			}
			stbi_image_free(texData);
			textures.push_back(texID);
			texture_map.emplace(AFile, textures.size() - 1);
			return textures.size() - 1;
		}

		void use(GLuint AID)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[AID]);
		}
	};

	namespace camValues {
		// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
		enum Camera_Movement {
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT,
			UP,
			DOWN
		};

		// Default camera values
		const float YAW = -90.0f;
		const float PITCH = 0.0f;
		const float SPEED = 2.5f;
		const float SENSITIVITY = 0.1f;
		const float ZOOM = 45.0f;
		const float ZOOM_MIN = 1.0f;
		const float ZOOM_MAX = 90.0f;
	}

	// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
	class Camera
	{
	public:
		// Camera Attributes
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;
		// Euler Angles
		float Yaw;
		float Pitch;
		// Camera options
		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;
		float ZoomMin;
		float ZoomMax;

		// Constructor with vectors
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = camValues::YAW, float pitch = camValues::PITCH, float zoomMin = camValues::ZOOM_MIN, float zoomMax = camValues::ZOOM_MAX) :
			Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(camValues::SPEED), MouseSensitivity(camValues::SENSITIVITY), Zoom(camValues::ZOOM), ZoomMin(zoomMin), ZoomMax(zoomMax)
		{
			Position = position;
			WorldUp = up;
			Yaw = yaw;
			Pitch = pitch;
			updateCameraVectors();
		}
		// Constructor with scalar values
		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(camValues::SPEED), MouseSensitivity(camValues::SENSITIVITY), Zoom(camValues::ZOOM)
		{
			Position = glm::vec3(posX, posY, posZ);
			WorldUp = glm::vec3(upX, upY, upZ);
			Yaw = yaw;
			Pitch = pitch;
			updateCameraVectors();
		}

		// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
		glm::mat4 getViewMatrix()
		{
			return glm::lookAt(Position, Position + Front, Up);
		}

		// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
		void processKeyboard(camValues::Camera_Movement direction, float deltaTime)
		{
			float velocity = MovementSpeed * deltaTime;
			if (direction == camValues::FORWARD)
				Position += Front * glm::vec3(1.0f, 0.0f, 1.0f) * velocity;
			if (direction == camValues::BACKWARD)
				Position -= Front * glm::vec3(1.0f, 0.0f, 1.0f) * velocity;
			if (direction == camValues::LEFT)
				Position -= Right * velocity;
			if (direction == camValues::RIGHT)
				Position += Right * velocity;
			if (direction == camValues::UP)
				Position += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
			if (direction == camValues::DOWN)
				Position -= glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
		}

		// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
		void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
		{
			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			Yaw += xoffset;
			Pitch += yoffset;

			// Make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch)
			{
				if (Pitch > 89.0f)
					Pitch = 89.0f;
				if (Pitch < -89.0f)
					Pitch = -89.0f;
			}

			// Update Front, Right and Up Vectors using the updated Euler angles
			updateCameraVectors();
		}

		// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
		void processMouseScroll(float yoffset)
		{
			if (Zoom >= ZoomMin && Zoom <= ZoomMax)
				Zoom -= yoffset;
			if (Zoom <= ZoomMin)
				Zoom = ZoomMin;
			if (Zoom >= ZoomMax)
				Zoom = ZoomMax;
		}

	private:
		// Calculates the front vector from the Camera's (updated) Euler Angles
		void updateCameraVectors()
		{
			// Calculate the new Front vector
			glm::vec3 front;
			front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			front.y = sin(glm::radians(Pitch));
			front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			Front = glm::normalize(front);
			// Also re-calculate the Right and Up vector
			Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));
		}
	};
}
