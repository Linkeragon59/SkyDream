#include "Diffraction_Renderer.h"

#include "Render_Device.h"
#include "Render_ShaderHelpers.h"
#include "Render_Debug.h"

#include "Diffraction_Module.h"

#include <stb_image.h>

namespace Diffraction
{
	void DiffractionRenderer::Draw(void* someBaseUBOData, void* someUBOData, const VkDescriptorImageInfo* aLightSpectrumTexture)
	{
		memcpy(myBaseDiffractionData->myMappedData, someBaseUBOData, sizeof(BaseDiffractionData));
		memcpy(myDiffractionData->myMappedData, someUBOData, myUBOSize);

		VkDescriptorSet descriptorSet = GetFreeDescriptorSet(myDescriptorSetLayout);
		std::array<VkWriteDescriptorSet, 3> writeDescriptorSets{};
		// Binding 0 : BaseUBO
		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].dstSet = descriptorSet;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].pBufferInfo = &myBaseDiffractionData->myDescriptor;
		writeDescriptorSets[0].descriptorCount = 1;
		// Binding 1 : Texture Sampler
		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].dstSet = descriptorSet;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].pImageInfo = aLightSpectrumTexture;
		writeDescriptorSets[1].descriptorCount = 1;
		// Binding 2 : UBO
		writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[2].dstSet = descriptorSet;
		writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[2].dstBinding = 2;
		writeDescriptorSets[2].pBufferInfo = &myDiffractionData->myDescriptor;
		writeDescriptorSets[2].descriptorCount = 1;
		vkUpdateDescriptorSets(myDevice, static_cast<uint>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

		vkCmdBindDescriptorSets(myCurrentSecondaryCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, myPipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

		vkCmdDraw(myCurrentSecondaryCommandBuffer, 3, 1, 0, 0);
	}

	void DiffractionRenderer::SetupInternal()
	{
		SetupRenderData();
	}
	
	void DiffractionRenderer::CleanupInternal()
	{
		DestroyRenderData();
	}
	
	void DiffractionRenderer::StartFrameInternal()
	{
		vkCmdBindPipeline(myCurrentSecondaryCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, myPipeline);
	
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)myRenderTarget.GetExtent().width;
		viewport.height = (float)myRenderTarget.GetExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(myCurrentSecondaryCommandBuffer, 0, 1, &viewport);
	
		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = myRenderTarget.GetExtent();
		vkCmdSetScissor(myCurrentSecondaryCommandBuffer, 0, 1, &scissor);
	}
	
	void DiffractionRenderer::EndFrameInternal()
	{
	}
	
	void DiffractionRenderer::SetupDescriptorSets()
	{
		std::array<VkDescriptorSetLayoutBinding, 3> bindings{};
		// Binding 0 : Base Diffraction data
		bindings[0].binding = 0;
		bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		bindings[0].descriptorCount = 1;
		bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		// Binding 1 : Light Spectrum Texture
		bindings[1].binding = 1;
		bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindings[1].descriptorCount = 1;
		bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		// Binding 2 : Diffraction data
		bindings[2].binding = 2;
		bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		bindings[2].descriptorCount = 1;
		bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{};
		descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutInfo.bindingCount = static_cast<uint>(bindings.size());
		descriptorLayoutInfo.pBindings = bindings.data();

		VK_CHECK_RESULT(
			vkCreateDescriptorSetLayout(myDevice, &descriptorLayoutInfo, nullptr, &myDescriptorSetLayout),
			"Failed to create the Diffraction DescriptorSetLayout");

		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = 2 * myRenderTarget.GetImagesCount();
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = myRenderTarget.GetImagesCount();

		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = myRenderTarget.GetImagesCount();

		VK_CHECK_RESULT(vkCreateDescriptorPool(myDevice, &descriptorPoolInfo, nullptr, &myDescriptorPool), "Failed to create the descriptor pool");
	}

	void DiffractionRenderer::DestroyDescriptorSetLayouts()
	{	
		vkDestroyDescriptorSetLayout(myDevice, myDescriptorSetLayout, nullptr);
		myDescriptorSetLayout = VK_NULL_HANDLE;
	}
	
	void DiffractionRenderer::SetupRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = myRenderTarget.GetColorFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = myRenderTarget.GetFinalLayout();
	
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
	
		// TODO : Use subpass dependencies for layout transitions
	
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
	
		VK_CHECK_RESULT(vkCreateRenderPass(myDevice, &renderPassInfo, nullptr, &myRenderPass), "Failed to create the renderpass");
	}

	void DiffractionRenderer::SetupPipelines()
	{
		VkShaderModule vertModule = ShaderHelpers::CreateShaderModule("Frameworks/Shaders/Fullscreen_vert.spv");
		VkShaderModule fragModule = ShaderHelpers::CreateShaderModule(myPixelShader.c_str());
	
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertModule;
		vertShaderStageInfo.pName = "main";
	
		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragModule;
		fragShaderStageInfo.pName = "main";
	
		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
	
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
	
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
	
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;
	
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
	
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
	
		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;
	
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();
	
		std::array<VkDescriptorSetLayout, 1> descriptorSetLayouts = {
			myDescriptorSetLayout
		};
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;
	
		VK_CHECK_RESULT(vkCreatePipelineLayout(myDevice, &pipelineLayoutInfo, nullptr, &myPipelineLayout), "Failed to create the pipeline layout");
	
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = myPipelineLayout;
		pipelineInfo.renderPass = myRenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	
		VK_CHECK_RESULT(vkCreateGraphicsPipelines(myDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &myPipeline), "Failed to create the pipeline");
	
		vkDestroyShaderModule(myDevice, fragModule, nullptr);
		vkDestroyShaderModule(myDevice, vertModule, nullptr);
	}

	void DiffractionRenderer::DestroyPipelines()
	{
		vkDestroyPipeline(myDevice, myPipeline, nullptr);
		myPipeline = VK_NULL_HANDLE;
		vkDestroyPipelineLayout(myDevice, myPipelineLayout, nullptr);
		myPipelineLayout = VK_NULL_HANDLE;
	}
	
	void DiffractionRenderer::SetupRenderData()
	{
		myBaseDiffractionData = new Buffer(sizeof(BaseDiffractionData),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		myBaseDiffractionData->SetupDescriptor();
		myBaseDiffractionData->Map();
		memset(myBaseDiffractionData->myMappedData, 0, sizeof(BaseDiffractionData));
		
		myDiffractionData = new Buffer(myUBOSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		myDiffractionData->SetupDescriptor();
		myDiffractionData->Map();
		memset(myDiffractionData->myMappedData, 0, myUBOSize);
	}
	
	void DiffractionRenderer::DestroyRenderData()
	{
		myBaseDiffractionData = nullptr;
		myDiffractionData = nullptr;
	}
}
