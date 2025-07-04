#define STB_IMAGE_IMPLEMENTATION
#include "C:/Users/User/Documents/code/.cpp/stb_image.h"
#include "src/includes.h"
#include "src/render.h"
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
	GLFW_KEY_LEFT_ALT
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


	GLFWwindow* Window = render::initializeWindow(currentScreenRes.x, currentScreenRes.y, "Heightmap-Renderer/main");
	glfwSetFramebufferSizeCallback(Window, framebuffer_size_callback);
	glfwGetCursorPos(Window, &cursorXPos, &cursorYPos);
	glEnable(GL_BLEND);

	cursorXPosPrev = cursorXPos;
	cursorYPosPrev = cursorYPos;
	utils::GLErrorcheck("Window Creation", true);


	Camera camera;




	renderedFrameID = render::createTexture(display::RENDER_RESOLUTION.x, display::RENDER_RESOLUTION.y);
	cloudTextureID = render::createTexture(constants::MAP_RESOLUTION.x, constants::MAP_RESOLUTION.y, GL_R32F);
	GLuint heightMap = render::loadTexture(constants::CURRENT_MAP + "-height");
	GLuint colourMap = render::loadTexture(constants::CURRENT_MAP + "-colour");


	//Cloud shader
	GLuint cloudShader = render::createShaderProgram("clouds", false);

	//Environment shader
	GLuint envShader = render::createShaderProgram("environment", false);

	//Display Shader
	GLuint displayShader = render::createShaderProgram("display");



	glViewport(0, 0, currentScreenRes.x, currentScreenRes.y);
	glDisable(GL_DEPTH_TEST);
	GLuint VAO = render::getVAO();


	utils::GLErrorcheck("Initialisation", true);



	//Initialize keyMap for input tracking
	for (int key : monitoredKeys) {
		keyMap[key] = false;
	}
	float verticalFOV = constants::TO_DEG * 2 * atan(tan(radians(camera.FOV / 2.0f)) * (display::RENDER_RESOLUTION.x / display::RENDER_RESOLUTION.y));

	int tick = 0;
	while (!glfwWindowShouldClose(Window)) {
		double frameStart = glfwGetTime();
		glfwPollEvents();

		// Get inputs for this frame
		for (int key : monitoredKeys) {
			int keyState = glfwGetKey(Window, key);
			if (keyState == GLFW_PRESS) {
				keyMap[key] = true;

			} else if (keyState == GLFW_RELEASE) {
				keyMap[key] = false;
			}
		}


		if (keyMap[GLFW_KEY_ESCAPE]) {
			break; //Quit
		}

		if (keyMap[GLFW_KEY_1]) {
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
		camera.angle.x += cursorXDelta * (config::TURN_SPEED_CURSOR);
		camera.angle.y -= cursorYDelta * (config::TURN_SPEED_CURSOR);
		camera.angle.x = utils::angleClamp(camera.angle.x);
		camera.angle.y = glm::clamp(camera.angle.y, -90.0f+(verticalFOV), 90.0f-(verticalFOV));



		//Cloud Shader.
		glViewport(0, 0, constants::MAP_RESOLUTION.x, constants::MAP_RESOLUTION.y);
		glUseProgram(cloudShader);
		glBindImageTexture(0, cloudTextureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

		glBindTextureUnit(0, heightMap);

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

		glBindTextureUnit(0, heightMap);
		glBindTextureUnit(1, colourMap);

		GLint cameraPosLocation = glGetUniformLocation(envShader, "cameraPosition");
		GLint cameraAngleLocation = glGetUniformLocation(envShader, "cameraAngle");
		GLint cameraFocalLengthLocation = glGetUniformLocation(envShader, "cameraFocalLength");
		GLint cameraFOVLocation = glGetUniformLocation(envShader, "cameraFOV");
		GLint cameraMaxDistLocation = glGetUniformLocation(envShader, "cameraMaxDistance");
		GLint skyColourLocation = glGetUniformLocation(envShader, "skyColour");
		cloudHeightLocation = glGetUniformLocation(envShader, "cloudHeight");
		
		glUniform3f(cameraPosLocation, camera.position.x, camera.position.y, camera.position.z);
		glUniform2f(cameraAngleLocation, camera.angle.x, camera.angle.y);
		glUniform1f(cameraFocalLengthLocation, camera.focalLength);
		glUniform1f(cameraFOVLocation, camera.FOV);
		glUniform1f(cameraMaxDistLocation, camera.viewDistance);
		glUniform3f(skyColourLocation, display::SKY_COLOUR.x, display::SKY_COLOUR.y, display::SKY_COLOUR.z);
		glUniform1i(cloudHeightLocation, display::CLOUD_HEIGHT);

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
