#pragma once

#include "../../vulkan/command/command_buffer.hpp"
#include "../../vulkan/device/device.hpp"
#include "../../vulkan/pipeline/graphic_pipeline.hpp"
#include "../../vulkan/buffer/buffer.hpp"
#include "../../vulkan/descriptor/descriptor_set_layout.hpp"
#include "../../vulkan/swap_chain/swap_chain.hpp"
#include "../../vulkan/renderpass/renderpass.hpp"
#include "../data/model/vertex_model.hpp"
#include "../utils/camera/camera.hpp"
#include "../ray_ubo.hpp"

#include <memory>
#include <vector>

namespace NugieApp {
	class SamplingRayRasterRenderSystem {
		public:
			SamplingRayRasterRenderSystem(NugieVulkan::Device* device, NugieVulkan::DescriptorSetLayout descriptorSetLayout, NugieVulkan::RenderPass renderPass);
			~SamplingRayRasterRenderSystem();

			void render(NugieVulkan::CommandBuffer* commandBuffer, VkDescriptorSet descriptorSets, VertexModel* model, uint32_t randomSeed = 1);
		
		private:
			void createPipelineLayout(NugieVulkan::DescriptorSetLayout descriptorSetLayout);
			void createPipeline(NugieVulkan::RenderPass renderPass);

			NugieVulkan::Device* device;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<NugieVulkan::GraphicPipeline> pipeline;
	};
}