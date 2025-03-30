#version 450

#include "diffraction.include"

layout (binding = 2) uniform UBO
{
	float attenuation;
} ubo;

void main() {
	float x = baseUBO.xrange * inUV.x - baseUBO.xrange / 2.0;
	float y = baseUBO.yrange * inUV.y - baseUBO.yrange / 2.0;

	float fx = exp(-x*x/(2.0*ubo.attenuation*ubo.attenuation));
	float fy = exp(-y*y/(2.0*ubo.attenuation*ubo.attenuation));

	vec4 lightRGB = texture(samplerTexture, vec2((baseUBO.waveLength - baseUBO.minVisibleWaveLength) / (baseUBO.maxVisibleWaveLength - baseUBO.minVisibleWaveLength), 1.0));
	outColor = adaptLuminosity(lightRGB, fx * fy * baseUBO.luminosity);
}