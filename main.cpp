#define STB_IMAGE_IMPLEMENTATION
#include "C:/Users/User/Documents/code/.cpp/stb_image.h"
#include "src/includes.h"
#include "src/graphics.h"
#include "src/utils.h"
using namespace std;
using namespace utils;
using namespace glm;



// Keyboard presses to monitor.
std::array<int, 16> monitoredKeys = { //16 should cover necessary keys.
	GLFW_KEY_W, GLFW_KEY_S,
	GLFW_KEY_A, GLFW_KEY_D,
	GLFW_KEY_E, GLFW_KEY_Q,
	GLFW_KEY_1, GLFW_KEY_ESCAPE,
	GLFW_KEY_LEFT_SHIFT,
	GLFW_KEY_LEFT_ALT,
	GLFW_KEY_2, GLFW_KEY_3
};



int mapIndex = 0;
std::array<glm::vec3, 29> positions = {
	glm::vec3(840.0f, 960.0f, 85.0f), 	// 0
	glm::vec3(14.0f, 424.0f, 39.0f), 	// 1
	glm::vec3(608.0f, 1000.0f, 54.0f), 	// 2
	glm::vec3(26.0f, 597.0f, 80.0f), 	// 3
	glm::vec3(64.0f, 64.0f, 64.0f), 	// 4
	glm::vec3(64.0f, 64.0f, 64.0f), 	// 5
	glm::vec3(64.0f, 64.0f, 64.0f), 	// 6
	glm::vec3(64.0f, 64.0f, 64.0f), 	// 7
	glm::vec3(64.0f, 64.0f, 64.0f), 	// 8
	glm::vec3(64.0f, 64.0f, 64.0f), 	// 9
	glm::vec3(64.0f, 64.0f, 64.0f), 	//10
	glm::vec3(64.0f, 64.0f, 64.0f), 	//11
	glm::vec3(64.0f, 64.0f, 64.0f), 	//12
	glm::vec3(64.0f, 64.0f, 64.0f), 	//13
	glm::vec3(64.0f, 64.0f, 64.0f), 	//14
	glm::vec3(64.0f, 64.0f, 64.0f), 	//15
	glm::vec3(64.0f, 64.0f, 64.0f), 	//16
	glm::vec3(64.0f, 64.0f, 64.0f), 	//17
	glm::vec3(64.0f, 64.0f, 64.0f), 	//18
	glm::vec3(64.0f, 64.0f, 64.0f), 	//19
	glm::vec3(64.0f, 64.0f, 64.0f), 	//20
	glm::vec3(64.0f, 64.0f, 64.0f), 	//21
	glm::vec3(64.0f, 64.0f, 64.0f), 	//22
	glm::vec3(64.0f, 64.0f, 64.0f), 	//23
	glm::vec3(64.0f, 64.0f, 64.0f), 	//24
	glm::vec3(64.0f, 64.0f, 64.0f), 	//25
	glm::vec3(64.0f, 64.0f, 64.0f), 	//26
	glm::vec3(64.0f, 64.0f, 64.0f), 	//27
	glm::vec3(64.0f, 64.0f, 64.0f), 	//28
};



GLuint renderedFrameID, cloudTextureID;
glm::ivec2 currentScreenRes;
unordered_map<int, bool> keyMap = {};


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	currentScreenRes = glm::ivec2(width, height);
}





int main() {
	try { //Catch exceptions
	double cursorXPos, cursorYPos, cursorXPosPrev, cursorYPosPrev;
	currentScreenRes = display::SCREEN_RESOLUTION;


	GLFWwindow* Window = graphics::initializeWindow(currentScreenRes.x, currentScreenRes.y, "Heightmap-Renderer/main");
	glfwSetFramebufferSizeCallback(Window, framebuffer_size_callback);
	glfwGetCursorPos(Window, &cursorXPos, &cursorYPos);
	glEnable(GL_BLEND);

	cursorXPosPrev = cursorXPos;
	cursorYPosPrev = cursorYPos;
	utils::GLErrorcheck("Window Creation", true);


	Camera camera;




	renderedFrameID = graphics::createTexture(display::RENDER_RESOLUTION.x, display::RENDER_RESOLUTION.y);
	cloudTextureID = graphics::createTexture(constants::MAP_RESOLUTION.x, constants::MAP_RESOLUTION.y, GL_R32F);
	GLuint heightMapArray = graphics::createTexture2DArray(true);
	GLuint colourMapArray = graphics::createTexture2DArray(false);


	//Cloud shader
	GLuint cloudShader = graphics::createShaderProgram("clouds", false);

	//Environment shader
	GLuint envShader = graphics::createShaderProgram("environment", false);

	//Display Shader
	GLuint displayShader = graphics::createShaderProgram("display");



	glViewport(0, 0, currentScreenRes.x, currentScreenRes.y);
	glDisable(GL_DEPTH_TEST);
	GLuint VAO = graphics::getVAO();


	utils::GLErrorcheck("Initialisation", true);



	//Initialize keyMap for input tracking
	for (int key : monitoredKeys) {
		keyMap[key] = false;
	}
	float verticalFOV = constants::TO_DEG * 2 * atan(tan(radians(camera.FOV / 2.0f)) * (display::RENDER_RESOLUTION.x / display::RENDER_RESOLUTION.y));

	bool prevpress = false;
	int tick = 0;
	int lastChange = 0;
	camera.position = positions[0];
	while (!glfwWindowShouldClose(Window)) {
		double frameStart = glfwGetTime();
		glfwPollEvents();

		// Get inputs for this frame
		for (int key : monitoredKeys) {
			int keyState = glfwGetKey(Window, key);
			if (keyState == GLFW_PRESS) {
				keyMap[key] = true;
				if ((key == GLFW_KEY_2) and (!prevpress)) { //Manually swap to next map.
					mapIndex = (mapIndex + 1) % 29;
					camera.position = positions[mapIndex];
					prevpress = true;
				}
				if (key == GLFW_KEY_3) { //Debug position.
					std::cout << (mapIndex+1) << " = (" << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << ")" << std::endl;
				}

			} else if (keyState == GLFW_RELEASE) {
				keyMap[key] = false;
			}
		}
		prevpress = keyMap[GLFW_KEY_2];


		if (keyMap[GLFW_KEY_ESCAPE]) {
			break; //Quit
		}

		if (!keyMap[GLFW_KEY_1]) {
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);			
		} else {
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwGetCursorPos(Window, &cursorXPos, &cursorYPos);
		}

		float camSpeed = config::CAMERA_MOVE_SPEED;
		if (keyMap[GLFW_KEY_LEFT_SHIFT]) {camSpeed *= config::CAMERA_MOVE_MULT_FAST;}
		if (keyMap[GLFW_KEY_LEFT_ALT]) {camSpeed *= config::CAMERA_MOVE_MULT_FASTER;}


		if (keyMap[GLFW_KEY_W]) {
			camera.position.x += camSpeed * sin(camera.angle.x * constants::TO_RAD);
			camera.position.y += camSpeed * cos(camera.angle.x * constants::TO_RAD);
		}
		if (keyMap[GLFW_KEY_S]) {
			camera.position.x -= camSpeed * sin(camera.angle.x * constants::TO_RAD);
			camera.position.y -= camSpeed * cos(camera.angle.x * constants::TO_RAD);
		}
		if (keyMap[GLFW_KEY_D]) {
			camera.position.x += camSpeed * sin((camera.angle.x + 90.0f) * constants::TO_RAD);
			camera.position.y += camSpeed * cos((camera.angle.x + 90.0f) * constants::TO_RAD);
		}
		if (keyMap[GLFW_KEY_A]) {
			camera.position.x -= camSpeed * sin((camera.angle.x + 90.0f) * constants::TO_RAD);
			camera.position.y -= camSpeed * cos((camera.angle.x + 90.0f) * constants::TO_RAD);
		}
		if (keyMap[GLFW_KEY_E]) {
			camera.position.z += camSpeed;
		}
		if (keyMap[GLFW_KEY_Q]) {
			camera.position.z -= camSpeed;
		}
		camera.position.x = fmod(camera.position.x+constants::MAP_RESOLUTION.x, constants::MAP_RESOLUTION.x);
		camera.position.y = fmod(camera.position.y+constants::MAP_RESOLUTION.y, constants::MAP_RESOLUTION.y);



		double cursorXDelta = cursorXPos - cursorXPosPrev;
		double cursorYDelta = cursorYPos - cursorYPosPrev;
		camera.angle.x += cursorXDelta * (config::TURN_SPEED_CURSOR) + config::TURN_SPEED_AUTO;
		camera.angle.y -= cursorYDelta * (config::TURN_SPEED_CURSOR);
		camera.angle.y = glm::clamp(camera.angle.y, -90.0f+(verticalFOV), 90.0f-(verticalFOV));

		if (camera.angle.x > 360.0f) {
			//Move to next map.
			mapIndex = (mapIndex + 1) % 29;
			camera.position = positions[mapIndex] + glm::vec3(0.5f, 0.5f, 0.0f);
			camera.angle.x = 0.0f;
			std::cout << "Changing to map: " << (mapIndex + 1) << std::endl << ((tick - lastChange) * constants::DT) << " seconds elapsed since last change." << std::endl << (tick * constants::DT) << " seconds total elapsed time." << std::endl;;
			lastChange = tick;
		}



		//Cloud Shader.
		glViewport(0, 0, constants::MAP_RESOLUTION.x, constants::MAP_RESOLUTION.y);
		glUseProgram(cloudShader);
		glBindImageTexture(0, cloudTextureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

		glBindTextureUnit(0, heightMapArray);

		GLint cloudHeightLocation = glGetUniformLocation(cloudShader, "cloudHeight");
		GLint timeLocation = glGetUniformLocation(cloudShader, "time");
		
		glUniform1i(cloudHeightLocation, display::CLOUD_HEIGHT);
		glUniform1i(timeLocation, tick);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		utils::GLErrorcheck("Cloud Shader", true);


		//Environment Shader.
		glViewport(0, 0, display::RENDER_RESOLUTION.x, display::RENDER_RESOLUTION.y);
		glUseProgram(envShader);
		glBindImageTexture(0, renderedFrameID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(1, cloudTextureID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

		glBindTextureUnit(0, heightMapArray);
		glBindTextureUnit(1, colourMapArray);

		GLint cameraPosLocation = glGetUniformLocation(envShader, "cameraPosition");
		GLint cameraAngleLocation = glGetUniformLocation(envShader, "cameraAngle");
		GLint cameraFocalLengthLocation = glGetUniformLocation(envShader, "cameraFocalLength");
		GLint cameraFOVLocation = glGetUniformLocation(envShader, "cameraFOV");
		GLint cameraMaxDistLocation = glGetUniformLocation(envShader, "cameraMaxDistance");
		GLint skyColourLocation = glGetUniformLocation(envShader, "skyColour");
		cloudHeightLocation = glGetUniformLocation(envShader, "cloudHeight");
		GLint mapIndexLocation = glGetUniformLocation(envShader, "mapIndex");
		
		glUniform3f(cameraPosLocation, camera.position.x, camera.position.y, camera.position.z);
		glUniform2f(cameraAngleLocation, camera.angle.x, camera.angle.y);
		glUniform1f(cameraFocalLengthLocation, camera.focalLength);
		glUniform1f(cameraFOVLocation, camera.FOV);
		glUniform1f(cameraMaxDistLocation, camera.viewDistance);
		glUniform3f(skyColourLocation, display::SKY_COLOUR.x, display::SKY_COLOUR.y, display::SKY_COLOUR.z);
		glUniform1i(cloudHeightLocation, display::CLOUD_HEIGHT);
		glUniform1i(mapIndexLocation, mapIndex);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		utils::GLErrorcheck("Environment Shader", true);



		//Display Shader and update screen.
		glViewport(0, 0, currentScreenRes.x, currentScreenRes.y);
		glUseProgram(displayShader);
		glBindTextureUnit(0, renderedFrameID);

		GLuint screenResLoc = glGetUniformLocation(displayShader, "screenResolution");
		glUniform2i(screenResLoc, currentScreenRes.x, currentScreenRes.y);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glfwSwapBuffers(Window);
		utils::GLErrorcheck("Display Shader", true);



		while (glfwGetTime() - frameStart < constants::DT) {}
		if (dev::SHOW_FREQ > 0) {
			double totalTime = (glfwGetTime() - frameStart);
			std::cout << floor(1/totalTime) << std::endl;
		}


		cursorXPosPrev = cursorXPos;
		cursorYPosPrev = cursorYPos;
		tick++;
	}

	glfwDestroyWindow(Window);
	glfwTerminate();
	return 0;


	//Catch exceptions.
	} catch (const std::exception& e) {
		std::cerr << "An exception was thrown: " << e.what() << std::endl;
		pause();
		return -1;
	} catch (...) {
		std::cerr << "An unspecified exception was thrown." << std::endl;
		pause();
		return -1;
	}
}
