#include "indirect_shade_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <string>

namespace NugieApp {
	IndirectShadeRenderSystem::IndirectShadeRenderSystem(NugieVulkan::Device* device, NugieVulkan::DescriptorSetLayout* descriptorSetLayout, uint32_t width, uint32_t height, uint32_t nSample) 
		: device{device}, width{width}, height{height}, nSample{nSample}
	{
		this->createPipelineLayout(descriptorSetLayout);
		this->createPipeline();
	}

	IndirectShadeRenderSystem::~IndirectShadeRenderSystem() {
		vkDestroyPipelineLayout(this->device->getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void IndirectShadeRenderSystem::createPipelineLayout(NugieVulkan::DescriptorSetLayout* descriptorSetLayout) {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(RayTracePushConstant);

		VkDescriptorSetLayout setLayout = descriptorSetLayout->getDescriptorSetLayout();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &setLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(this->device->getLogicalDevice(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void IndirectShadeRenderSystem::createPipeline() {
		assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		this->pipeline = NugieVulkan::ComputePipeline::Builder(this->device, this->pipelineLayout)
			.setDefault("shader/indirect_shade.comp.spv")
			.build();
	}

	void IndirectShadeRenderSystem::render(NugieVulkan::CommandBuffer* commandBuffer, VkDescriptorSet descriptorSets, uint32_t randomSeed) {
		this->pipeline->bind(commandBuffer->getCommandBuffer());

		vkCmdBindDescriptorSets(
			commandBuffer->getCommandBuffer(),
			VK_PIPELINE_BIND_POINT_COMPUTE,
			this->pipelineLayout,
			0,
			1,
			&descriptorSets,
			0,
			nullptr
		);

		RayTracePushConstant pushConstant{};
		pushConstant.randomSeed = randomSeed;

		vkCmdPushConstants(
			commandBuffer->getCommandBuffer(), 
			this->pipelineLayout, 
			VK_SHADER_STAGE_COMPUTE_BIT,
			0,
			sizeof(RayTracePushConstant),
			&pushConstant
		);

		this->pipeline->dispatch(commandBuffer->getCommandBuffer(), (this->width * this->height * this->nSample) / 32u, 1u, 1u);
	}
}