#include "intersect_object_render_system.hpp"

#include <stdexcept>
#include <array>
#include <string>

namespace nugiEngine {
	EngineIntersectObjectRenderSystem::EngineIntersectObjectRenderSystem(EngineDevice& device, VkDescriptorSetLayout descriptorSetLayouts, uint32_t width, uint32_t height, uint32_t nSample) 
		: appDevice{device}, width{width}, height{height}, nSample{nSample}
	{
		this->createPipelineLayout(descriptorSetLayouts);
		this->createPipeline();
	}

	EngineIntersectObjectRenderSystem::~EngineIntersectObjectRenderSystem() {
		vkDestroyPipelineLayout(this->appDevice.getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void EngineIntersectObjectRenderSystem::createPipelineLayout(VkDescriptorSetLayout descriptorSetLayouts) {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayouts;

		if (vkCreatePipelineLayout(this->appDevice.getLogicalDevice(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void EngineIntersectObjectRenderSystem::createPipeline() {
		assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		this->pipeline = EngineComputePipeline::Builder(this->appDevice, this->pipelineLayout)
			.setDefault("shader/intersect_object.comp.spv")
			.build();
	}

	void EngineIntersectObjectRenderSystem::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, VkDescriptorSet descriptorSets) {
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

		this->pipeline->dispatch(commandBuffer->getCommandBuffer(), (this->width * this->height * this->nSample) / 32u, 1u, 1u);
	}
}