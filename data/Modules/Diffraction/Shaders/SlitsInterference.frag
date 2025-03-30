#version 450

#include "diffraction.include"
#include "interference.include"

layout (binding = 2) uniform UBO
{
	float slitsCount;
	float slitsWidth;
	float slitsSpacing;
	float attenuation;
} ubo;

void main() {
	float x = baseUBO.xrange * inUV.x - baseUBO.xrange / 2.0;
	float y = baseUBO.yrange * inUV.y - baseUBO.yrange / 2.0;

	float fx = diffraction(x, ubo.slitsWidth, baseUBO.waveLength, baseUBO.screenDistance) * interference(x, ubo.slitsSpacing, ubo.slitsCount, baseUBO.waveLength, baseUBO.screenDistance);
	float fy = exp(-y*y/(2.0*ubo.attenuation*ubo.attenuation));

	vec4 lightRGB = texture(samplerTexture, vec2((baseUBO.waveLength - baseUBO.minVisibleWaveLength) / (baseUBO.maxVisibleWaveLength - baseUBO.minVisibleWaveLength), 1.0));
	outColor = adaptLuminosity(lightRGB, fx * fy * baseUBO.luminosity);
}