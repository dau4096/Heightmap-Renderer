/* environment.frag */
#version 460 core


layout(binding=0) uniform sampler2DArray heightMaps;
layout(binding=1) uniform sampler2DArray colourMaps;
uniform vec3 cameraPosition;
uniform vec2 cameraAngle;
uniform float cameraFocalLength;
uniform float cameraFOV;
uniform float cameraMaxDistance;
uniform vec3 skyColour;
uniform int cloudHeight;
uniform int mapIndex;


layout(rgba32f, binding=0) uniform image2D renderedFrame;
layout(r32f, binding=1) uniform image2D cloudMap;



const float EPSILON = 1e-4f;
const vec2 INVALIDv2 = vec2(1e30f, 1e30f);


float determinant(vec2 vecA, vec2 vecB) {
	return (vecA.x * vecB.y) - (vecA.y * vecB.x);
}


void main() {
	vec2 fragPosition = gl_FragCoord.xy;
	ivec2 renderResolution = imageSize(renderedFrame);
	ivec2 framePosition = ivec2(fragPosition);
	ivec2 mapLimits = textureSize(heightMaps, 0).xy;


	float aspectRatio = renderResolution.y / renderResolution.x;
	float rayOffsetXY = ((fragPosition.x / renderResolution.x)-0.5f) * cameraFOV;
	float angleXY = radians(cameraAngle.x + rayOffsetXY);
	vec2 rayDirectionXY = vec2(sin(angleXY), cos(angleXY));


	float ndcY = (fragPosition.y / renderResolution.y) * 2.0f - 1.0f; //[-1.0f - 1.0f]
	float angleZ = ndcY * radians(cameraFOV / 2.0f);
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
		float height = texture(heightMaps, vec3(mapUV.xy, mapIndex)).r * 255.0f;

		float rayZ = cameraPosition.z + (t * deltaZ);
		if (rayZ < 0.0f) {
			minTValue = t;
			break;
		} else if ((height + EPSILON) >= rayZ){
			minTValue = t;
			break;
		}
	}



	float compensatedCloudHeight = (1.0f - (minTValue / 2.0f)) * cloudHeight;

	vec4 cloudContribution = vec4(1.0f, 1.0f, 1.0f, 0.0f);
	if (((cameraPosition.z + deltaZ > compensatedCloudHeight) && (cameraPosition.z < cloudHeight))
		 || ((cameraPosition.z + deltaZ < compensatedCloudHeight) && (cameraPosition.z > cloudHeight))){
		ivec2 cloudUV = ivec2(cameraPosition.xy + deltaXY*minTValue);
		float cloudOpacity = imageLoad(cloudMap, cloudUV).r;
		cloudContribution = vec4(1.0f, 1.0f, 1.0f, cloudOpacity);
	}

	if (minTValue < 1.0f) {
		//Must hit terrain
		vec2 colourUV = (cameraPosition.xy + deltaXY*minTValue) / mapLimits;
		vec3 colour = texture(colourMaps, vec3(colourUV.xy, mapIndex)).rgb;

		if ((cloudContribution.a > EPSILON) && (cameraPosition.z > cloudHeight)) {
			vec4 fragColour = vec4(mix(colour.rgb, cloudContribution.rgb, cloudContribution.a), 1.0f);
			imageStore(renderedFrame, framePosition, fragColour);
		} else {
			imageStore(renderedFrame, framePosition, vec4(colour.rgb, 1.0f));
		}
	} else {
		//Must hit sky.
		if (cloudContribution.a > EPSILON) {
			vec4 fragColour = vec4(mix(skyColour.rgb, cloudContribution.rgb, cloudContribution.a), 1.0f);
			imageStore(renderedFrame, framePosition, fragColour);
		} else {
			imageStore(renderedFrame, framePosition, vec4(skyColour.rgb, 1.0f));
		}
	}
}