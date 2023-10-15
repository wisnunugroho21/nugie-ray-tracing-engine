#pragma once

#include "../../../vulkan/command/command_buffer.hpp"
#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/pipeline/compute_pipeline.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/descriptor/descriptor_set_layout.hpp"
#include "../../../vulkan/swap_chain/swap_chain.hpp"
#include "../../utils/camera/camera.hpp"
#include "../../ray_ubo.hpp"

#include <memory>
#include <vector>

namespace NugieApp {
	class LightShadeRenderSystem {
		public:
			LightShadeRenderSystem(NugieVulkan::Device* device, NugieVulkan::DescriptorSetLayout* descriptorSetLayout, uint32_t width, uint32_t height, uint32_t nSample);
			~LightShadeRenderSystem();

			void render(NugieVulkan::CommandBuffer* commandBuffer, VkDescriptorSet descriptorSets);

		private:
			void createPipelineLayout(NugieVulkan::DescriptorSetLayout* descriptorSetLayout);
			void createPipeline();

			NugieVulkan::Device* device;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<NugieVulkan::ComputePipeline> pipeline;

			uint32_t width, height, nSample;
	};
}