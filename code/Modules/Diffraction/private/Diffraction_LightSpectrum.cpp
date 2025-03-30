#include "Diffraction_LightSpectrum.h"

#include <stb_image.h>

namespace Diffraction
{
	LightSpectrum::LightSpectrum()
	{
		const char* texture = "Modules/Diffraction/Textures/LightSpectrum.png";

		myLightSpectrumPixelData = stbi_load(texture, &myLightSpectrumWidth, &myLightSpectrumHeight, &myLightSpectrumChannels, STBI_rgb_alpha);
		Assert(myLightSpectrumPixelData, "Failed to load an image!");

		VkDeviceSize textureSize = static_cast<VkDeviceSize>(myLightSpectrumWidth) * static_cast<VkDeviceSize>(myLightSpectrumHeight) * 4;
		Buffer textureStaging;

		textureStaging.Create(textureSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		textureStaging.Map();
		memcpy(textureStaging.myMappedData, myLightSpectrumPixelData, static_cast<size_t>(textureSize));
		textureStaging.Unmap();

		myLightSpectrumTexture = new Image(myLightSpectrumWidth, myLightSpectrumHeight,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		myLightSpectrumTexture->TransitionLayout(VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			RenderCore::GetInstance()->GetGraphicsQueue());

		VkCommandBuffer commandBuffer = Helpers::BeginOneTimeCommand();
		{
			VkBufferImageCopy imageCopyRegion{};
			imageCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageCopyRegion.imageSubresource.mipLevel = 0;
			imageCopyRegion.imageSubresource.baseArrayLayer = 0;
			imageCopyRegion.imageSubresource.layerCount = 1;
			imageCopyRegion.imageExtent = { static_cast<uint>(myLightSpectrumWidth), static_cast<uint>(myLightSpectrumHeight), 1 };
			vkCmdCopyBufferToImage(commandBuffer, textureStaging.myBuffer, myLightSpectrumTexture->myImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);
		}
		Helpers::EndOneTimeCommand(commandBuffer, RenderCore::GetInstance()->GetGraphicsQueue());

		textureStaging.Destroy();

		myLightSpectrumTexture->TransitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			RenderCore::GetInstance()->GetGraphicsQueue());

		myLightSpectrumTexture->CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
		myLightSpectrumTexture->CreateImageSampler(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
		myLightSpectrumTexture->SetupDescriptor();
	}

	LightSpectrum::~LightSpectrum()
	{
		stbi_image_free(myLightSpectrumPixelData);

		myLightSpectrumTexture = nullptr;
	}

	const VkDescriptorImageInfo* LightSpectrum::GetLightSpectrumDescriptor() const
	{
		Assert(myLightSpectrumTexture);
		return &myLightSpectrumTexture->myDescriptor;
	}

	void LightSpectrum::GetLightSpectrumColor(float aWaveLength, float aMinWaveLength, float aMaxWaveLength, glm::u8vec4& anOutColor) const
	{
		Assert(myLightSpectrumPixelData);
		int u = static_cast<int>(myLightSpectrumWidth * (aWaveLength - aMinWaveLength) / (aMaxWaveLength - aMinWaveLength));
		u = std::clamp(u, 0, myLightSpectrumWidth - 1);
		uint8* pixelOffset = myLightSpectrumPixelData + u * myLightSpectrumChannels;
		anOutColor.r = pixelOffset[0];
		anOutColor.g = pixelOffset[1];
		anOutColor.b = pixelOffset[2];
		anOutColor.a = pixelOffset[3];
	}
}
