#pragma once

#include "Diffraction_Data.h"
#include "Render_Renderer.h"

namespace Diffraction
{
	using namespace Render;

	class LightSpectrum
	{
	public:
		LightSpectrum();
		~LightSpectrum();

		const VkDescriptorImageInfo* GetLightSpectrumDescriptor() const;
		void GetLightSpectrumColor(float aWaveLength, float aMinWaveLength, float aMaxWaveLength, glm::u8vec4& anOutColor) const;

	private:
		ImagePtr myLightSpectrumTexture;
		uint8* myLightSpectrumPixelData = nullptr;
		int myLightSpectrumWidth = 0;
		int myLightSpectrumHeight = 0;
		int myLightSpectrumChannels = 0;
	};
}
