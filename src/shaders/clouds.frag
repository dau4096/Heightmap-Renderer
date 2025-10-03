/* clouds.frag */
#version 460 core

layout(binding=0) uniform sampler2DArray heightMaps;
uniform int cloudHeight;
uniform int time;

layout(r32f, binding=0) uniform image2D cloudMap;


vec2 fragPosition;
ivec2 mapSize, mapPosition;
float cloudDensity;


float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 34.45);
    return fract(p.x * p.y);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    
    vec2 u = f * f * (3.0 - 2.0 * f);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float animatedNoise(vec2 pos, float time) {
    return noise(pos + vec2(time * 0.1, time * 0.05));
}


void main() {
	fragPosition = gl_FragCoord.xy;
	mapSize = imageSize(cloudMap);
	mapPosition = ivec2(fragPosition);


	vec2 UV = fragPosition / vec2(mapSize);
	cloudDensity = 1.0f;//((mapPosition.x & 1) == 0) ? 1.0f : 0.0f;//animatedNoise(UV, time);


	imageStore(cloudMap, mapPosition, vec4(cloudDensity, 0.0f, 0.0f, 0.0f));
}