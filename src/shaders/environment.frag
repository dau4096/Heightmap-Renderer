/* environment.frag */
#version 460 core


layout(binding=0) uniform sampler2D heightMap;
layout(binding=1) uniform sampler2D colourMap;
uniform vec3 cameraPosition;
uniform vec2 cameraAngle;
uniform float cameraFocalLength;
uniform float cameraFOV;
uniform float cameraMaxDistance;
uniform vec3 skyColour;


layout(rgba32f, binding = 0) uniform image2D renderedFrame;



const float EPSILON = 1e-4f;
const vec2 INVALIDv2 = vec2(1e30f, 1e30f);


float determinant(vec2 vecA, vec2 vecB) {
	return (vecA.x * vecB.y) - (vecA.y * vecB.x);
}


void main() {
	vec2 fragPosition = gl_FragCoord.xy;
	ivec2 renderResolution = imageSize(renderedFrame);
	ivec2 framePosition = ivec2(fragPosition);
	ivec2 mapLimits = textureSize(heightMap, 0);


	float aspectRatio = renderResolution.y / renderResolution.x;
	float rayOffsetXY = ((fragPosition.x / renderResolution.x)-0.5f) * cameraFOV;
	float angleXY = radians(cameraAngle.x + rayOffsetXY);
	vec2 rayDirectionXY = vec2(sin(angleXY), cos(angleXY));


	float verticalFOV = 2 * atan(tan(radians(cameraFOV / 2.0f)) * (renderResolution.x / renderResolution.y));
	float angleZ = (-(fragPosition.y / renderResolution.y) + 0.5f) * -verticalFOV;
	float deltaZ = cameraMaxDistance * tan(angleZ + radians(cameraAngle.y));
	float actualMaxDistance = cameraMaxDistance;
	float t;
	if (cameraMaxDistance < EPSILON) {
		imageStore(renderedFrame, framePosition, vec4(skyColour.rgb, 1.0f));
		return;
	}
	vec2 furthestPoint = cameraPosition.xy + rayDirectionXY * actualMaxDistance;

	vec2 deltaXY = furthestPoint.xy - cameraPosition.xy;



	float minTValue = 1.0f;
	float altOffset = 0.0f;
	bool yAlt;
	int maxV, signV, signA;
	float gradient;
	if (abs(deltaXY.x) > abs(deltaXY.y)) {
		maxV = int(round(abs(deltaXY.x)));
		signV = (deltaXY.x > 0.0f) ? 1 : -1;
		signA = (deltaXY.y > 0.0f) ? 1 : -1;
		yAlt = true;
		gradient = deltaXY.y / deltaXY.x;
	} else {
		maxV = int(round(abs(deltaXY.y)));
		signV = (deltaXY.y > 0.0f) ? 1 : -1;
		signA = (deltaXY.x > 0.0f) ? 1 : -1;
		yAlt = false;
		gradient = deltaXY.x / deltaXY.y;
	}
	for (int offset=0; offset<=maxV; offset++) {
		altOffset += gradient*signA;

		float t = float(offset) / float(maxV);
		vec2 mapUV = (cameraPosition.xy + deltaXY*t) / mapLimits;
		float height = texture(heightMap, mapUV).r * 255.0f; //36.0f?

		float rayZ = cameraPosition.z + (t * deltaZ);
		if (rayZ < 0.0f) {
			minTValue = t;
			break;
		} else if ((height + EPSILON) >= rayZ){
			minTValue = t;
			break;
		}
	}



	if (minTValue != 1.0f) {
		vec2 colourUV = (cameraPosition.xy + deltaXY*minTValue) / mapLimits;
		vec3 colour = texture(colourMap, colourUV).rgb;
		vec4 fragColour = vec4(colour, 1.0f);

		imageStore(renderedFrame, framePosition, fragColour);
	} else {
		imageStore(renderedFrame, framePosition, vec4(skyColour.rgb, 1.0f));
	}
}