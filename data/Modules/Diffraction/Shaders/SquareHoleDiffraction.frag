#version 450

#include "diffraction.include"

layout (binding = 2) uniform UBO
{
	float holeWidth;
	float holeHeight;
} ubo;

void main() {
	float x = baseUBO.xrange * inUV.x - baseUBO.xrange / 2.0;
	float y = baseUBO.yrange * inUV.y - baseUBO.yrange / 2.0;

	float fx = diffraction(x, ubo.holeWidth, baseUBO.waveLength, baseUBO.screenDistance);
	float fy = diffraction(y, ubo.holeHeight, baseUBO.waveLength, baseUBO.screenDistance);

	vec4 lightRGB = texture(samplerTexture, vec2((baseUBO.waveLength - baseUBO.minVisibleWaveLength) / (baseUBO.maxVisibleWaveLength - baseUBO.minVisibleWaveLength), 1.0));
	outColor = adaptLuminosity(lightRGB, fx * fy * baseUBO.luminosity);
}