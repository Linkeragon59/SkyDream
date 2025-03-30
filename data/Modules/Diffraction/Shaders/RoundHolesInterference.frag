#version 450

#include "diffraction.include"
#include "interference.include"

layout (binding = 2) uniform UBO
{
	float holesCount;
	float holesDiameter;
	float holesSpacing;
} ubo;

void main() {
	float x = baseUBO.xrange * inUV.x - baseUBO.xrange / 2.0;
	float y = baseUBO.yrange * inUV.y - baseUBO.yrange / 2.0;
	float r = sqrt(x*x + y*y);

	float fr = diffraction(r, ubo.holesDiameter, baseUBO.waveLength, baseUBO.screenDistance);
	float fx = interference(x, ubo.holesSpacing, ubo.holesCount, baseUBO.waveLength, baseUBO.screenDistance);

	vec4 lightRGB = texture(samplerTexture, vec2((baseUBO.waveLength - baseUBO.minVisibleWaveLength) / (baseUBO.maxVisibleWaveLength - baseUBO.minVisibleWaveLength), 1.0));
	outColor = adaptLuminosity(lightRGB, fr * fx * baseUBO.luminosity);
}