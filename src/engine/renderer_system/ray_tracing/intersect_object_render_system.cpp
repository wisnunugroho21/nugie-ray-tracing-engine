#include "intersect_object_render_system.hpp"

#include <stdexcept>
#include <array>
#include <string>

namespace NugieApp {
	IntersectObjectRenderSystem::IntersectObjectRenderSystem(NugieVulkan::Device* device, NugieVulkan::DescriptorSetLayout descriptorSetLayout, uint32_t width, uint32_t height, uint32_t nSample) 
		: device{device}, width{width}, height{height}, nSample{nSample}
	{
		this->createPipelineLayout(descriptorSetLayout);
		this->createPipeline();
	}

	IntersectObjectRenderSystem::~IntersectObjectRenderSystem() {
		vkDestroyPipelineLayout(this->device->getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void IntersectObjectRenderSystem::createPipelineLayout(NugieVulkan::DescriptorSetLayout descriptorSetLayout) {
		VkDescriptorSetLayout setLayout = descriptorSetLayout.getDescriptorSetLayout();
		
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &setLayout;

		if (vkCreatePipelineLayout(this->device->getLogicalDevice(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void IntersectObjectRenderSystem::createPipeline() {
		assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		this->pipeline = NugieVulkan::ComputePipeline::Builder(this->device, this->pipelineLayout)
			.setDefault("shader/intersect_object.comp.spv")
			.build();
	}

	void IntersectObjectRenderSystem::render(NugieVulkan::CommandBuffer* commandBuffer, VkDescriptorSet descriptorSets) {
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