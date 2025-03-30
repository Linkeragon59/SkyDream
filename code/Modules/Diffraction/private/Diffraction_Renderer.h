#pragma once

#include "Diffraction_Data.h"
#include "Render_Renderer.h"

namespace Diffraction
{
	using namespace Render;

	class DiffractionRenderer : public Renderer
	{
	public:
		DiffractionRenderer(RenderTarget& aRenderTarget, const char* aPixelShader, uint aUBOSize)
			: Renderer(aRenderTarget)
			, myPixelShader(aPixelShader)
			, myUBOSize(aUBOSize)
		{}

		void Draw(void* someBaseUBOData, void* someUBOData, const VkDescriptorImageInfo* aLightSpectrumTexture);
	
	protected:
		void SetupInternal() override;
		void CleanupInternal() override;
	
		void StartFrameInternal() override;
		void EndFrameInternal() override;
	
	protected:
		// Descriptor Sets
		void SetupDescriptorSets() override;
		void DestroyDescriptorSetLayouts() override;
		VkDescriptorSetLayout myDescriptorSetLayout = VK_NULL_HANDLE;
	
		void SetupRenderPass() override;

		void SetupPipelines() override;
		void DestroyPipelines() override;
		VkPipeline myPipeline = VK_NULL_HANDLE;
		VkPipelineLayout myPipelineLayout = VK_NULL_HANDLE;
	
	private:
		std::string myPixelShader;
		uint myUBOSize;

		void SetupRenderData();
		void DestroyRenderData();
		BufferPtr myBaseDiffractionData;
		BufferPtr myDiffractionData;
	};
}
