#include "sampling_ray_raster_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <string>

#include "../data/model/vertex_model.hpp"

namespace NugieApp {
	SamplingRayRasterRenderSystem::SamplingRayRasterRenderSystem(NugieVulkan::Device* device, NugieVulkan::DescriptorSetLayout* descriptorSetLayout, NugieVulkan::RenderPass* renderPass)
		: device{device}
	{
		this->createPipelineLayout(descriptorSetLayout);
		this->createPipeline(renderPass);
	}

	SamplingRayRasterRenderSystem::~SamplingRayRasterRenderSystem() {
		vkDestroyPipelineLayout(this->device->getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void SamplingRayRasterRenderSystem::createPipelineLayout(NugieVulkan::DescriptorSetLayout* descriptorSetLayout) {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(RayTracePushConstant);

		VkDescriptorSetLayout setLayout = descriptorSetLayout->getDescriptorSetLayout();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1u;
		pipelineLayoutInfo.pSetLayouts = &setLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 1u;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(this->device->getLogicalDevice(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void SamplingRayRasterRenderSystem::createPipeline(NugieVulkan::RenderPass* renderPass) {
		assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		this->pipeline = NugieVulkan::GraphicPipeline::Builder(this->device, renderPass, this->pipelineLayout)
			.setDefault("shader/sampling.vert.spv", "shader/sampling.frag.spv", Vertex::getVertexBindingDescriptions(), Vertex::getVertexAttributeDescriptions())
			.build();
	}

	void SamplingRayRasterRenderSystem::render(NugieVulkan::CommandBuffer* commandBuffer, VkDescriptorSet descriptorSets, VertexModel* model, uint32_t randomSeed) {
		this->pipeline->bind(commandBuffer->getCommandBuffer());

		vkCmdBindDescriptorSets(
			commandBuffer->getCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
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
			VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(RayTracePushConstant),
			&pushConstant
		);

		model->bind(commandBuffer);
		model->draw(commandBuffer);
	}
}