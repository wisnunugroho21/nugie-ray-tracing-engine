#pragma once

#include "../../../vulkan/command/command_buffer.hpp"
#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/pipeline/compute_pipeline.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../ray_ubo.hpp"

#include <memory>
#include <vector>

namespace NugieApp {
	class GlobalUniform {
		public:
			GlobalUniform(NugieVulkan::Device* device);

			std::vector<VkDescriptorBufferInfo> getBuffersInfo() const;

			void writeGlobalData(uint32_t frameIndex, RayTraceUbo ubo);

		private:
      NugieVulkan::Device* device;
			std::vector<std::shared_ptr<NugieVulkan::Buffer>> uniformBuffers;

			void createUniformBuffer();
	};
}