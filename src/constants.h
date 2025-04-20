#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "includes.h"
#include <C:/Users/User/Documents/code/.cpp/glm/glm.hpp>
namespace constants {
	//Mathematical Constants
	constexpr float PI = 3.14159265358979f;
	constexpr float EXP = 2.71828182845905f;

	constexpr float TO_RAD = 0.01745329251994f;
	constexpr float TO_DEG = 57.2957795130824f;


	//Map Data
	constexpr glm::ivec2 MAP_RESOLUTION = glm::ivec2(1024, 1024);
	constexpr std::string CURRENT_MAP = "comanche5";


	//Physics/Rendering Frequency/dt
	constexpr int HZ = 45;
	constexpr double DT = 1.0d/HZ;


	//Invalid returns for vectors and floats.
	constexpr float INVALID = 1e30f;
	constexpr glm::vec2 INVALIDv2 = glm::vec2(INVALID, INVALID);
	constexpr glm::vec3 INVALIDv3 = glm::vec3(INVALID, INVALID, INVALID);
	constexpr glm::vec4 INVALIDv4 = glm::vec4(INVALID, INVALID, INVALID, INVALID);
}

namespace config {
	constexpr float TURN_SPEED_CURSOR = 0.025f;
	constexpr float CAMERA_MOVE_SPEED = 1.0f;
	constexpr float CAMERA_MOVE_MULT_FAST = 3.0f;
	constexpr float CAMERA_MOVE_MULT_FASTER = 10.0f;
}

namespace display {
	//Resolutions
	constexpr glm::ivec2 SCREEN_RESOLUTION = glm::ivec2(640, 400);
	constexpr glm::ivec2 RENDER_RESOLUTION = glm::ivec2(480, 270);


	//Camera Assorted
	constexpr float CAMERA_FOV = 70.0f;
	constexpr float CAMERA_MAX_DISTANCE = 4096.0f;
	constexpr float CAMERA_FOCAL_LENGTH = 1.0f;
	constexpr glm::vec3 CAMERA_START_POSITION = glm::vec3(64.0f, 64.0f, 16.0f);
	constexpr glm::vec2 CAMERA_START_ANGLE = glm::vec2(0.0f, 0.0f);
	constexpr glm::vec3 SKY_COLOUR = glm::vec3(135.0f, 206.0f, 235.0f) / 255.0f;
}


namespace dev {
	//Assorted DEV/DEBUG constants
	constexpr int SHOW_FREQ = 0;
}

#endif // CONSTANTS_H