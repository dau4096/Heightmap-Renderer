/* environment.frag */
#version 460 core


uniform sampler2DArray textureArray;
uniform sampler2D heightMap;
uniform sampler2D colourMap;
uniform vec3 cameraPosition;
uniform vec2 cameraAngle;
uniform float cameraFocalLength;
uniform float cameraFOV;
uniform float cameraMaxDistance;


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


	float verticalFOV = 2 * atan(tan(radians(cameraFOV / 2.0f)) * (renderResolution.x / renderResolution.y));;
	float angleZ = ((fragPosition.y / renderResolution.y) - 0.5f) * -verticalFOV;
	float deltaZ = cameraMaxDistance * tan(angleZ + radians(cameraAngle.y));
	float actualMaxDistance = cameraMaxDistance;
	float t;
	if ((abs(deltaZ) > EPSILON) && (cameraPosition.z - deltaZ) < 0) {
		t = cameraPosition.z / deltaZ; //0-1
		deltaZ = cameraPosition.z;
		actualMaxDistance *= t;
		//Integer values from heightMap, if ray goes below 0 limit raycast to shorter range.
	}
	if (cameraMaxDistance < EPSILON) {
		imageStore(renderedFrame, framePosition, vec4(1.0f, 0.0f, 1.0f, 1.0f));
		return;
	}
	vec2 furthestPoint = rayDirectionXY * actualMaxDistance;

	if (furthestPoint.x < 0.0f) {
		t = cameraPosition.x / furthestPoint.x;
		furthestPoint *= t;
	}
	if (furthestPoint.y < 0.0f) {
		t = cameraPosition.y / furthestPoint.y;
		furthestPoint *= t;
	}
	if (furthestPoint.x > mapLimits.x) {
		//t = cameraPosition.x / furthestPoint.x; //Fix.
		furthestPoint *= t;
	}
	if (furthestPoint.y > mapLimits.y) {
		//t = cameraPosition.y / furthestPoint.y; //Fix.
		furthestPoint *= t;
	}



	imageStore(renderedFrame, framePosition, vec4(furthestPoint.xy / mapLimits.xy, 0.125f, 1.0f));
	return;
	vec2 deltaXY = furthestPoint - cameraPosition.xy;

	float gradient = 0.0f;
	if (deltaXY.x > EPSILON) {
		gradient = deltaXY.y / deltaXY.x;
	}

	vec2 closestUV = INVALIDv2;
	float minTValue = 1.0f;
	float yOffset = 0;
	int maxX = int(round(abs(deltaXY.x)));
	int signX = int(sign(deltaXY.x));
	for (int xOffset = 0; xOffset <= maxX; xOffset++) {
		yOffset += gradient;
		ivec2 mapPosition = ivec2(
			round(cameraPosition.x) + xOffset * signX,
			round(cameraPosition.y + yOffset)
		);
		if ((mapPosition.x < 0) || (mapPosition.x > mapLimits.x) || (mapPosition.y < 0) || mapPosition.y > mapLimits.y) {
			continue;
		}

		vec2 UV = mapPosition / mapLimits;
		float height = texture(heightMap, UV).r;

		float t = length(ivec2(xOffset, yOffset)) / actualMaxDistance;
		float rayZ = mix(cameraPosition.z, deltaZ, t);
		if (height > rayZ) {
			if (t < minTValue) {
				minTValue = t;
				closestUV = UV;
			}
		}
	}




	if (closestUV != INVALIDv2) {
		vec3 colour = texture(colourMap, closestUV).rgb;
		vec4 fragColour = vec4(colour.rgb, 1.0f);

		imageStore(renderedFrame, framePosition, fragColour);
	} else {
		imageStore(renderedFrame, framePosition, vec4(1.0f, 0.0f, 1.0f, 1.0f));
	}
}