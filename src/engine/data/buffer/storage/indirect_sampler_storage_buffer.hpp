#pragma once

#include "../../../../vulkan/device/device.hpp"
#include "../../../../vulkan/buffer/buffer.hpp"
#include "../../../../vulkan/command/command_buffer.hpp"
#include "../../../ray_ubo.hpp"
#include "../../../utils/transform/transform.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace NugieApp {
	class IndirectSamplerStorageBuffer {
		public:
			IndirectSamplerStorageBuffer(NugieVulkan::Device* device, std::shared_ptr<std::vector<IndirectSamplerData>> datas);

			std::vector<VkDescriptorBufferInfo> getBuffersInfo();

			void transferToRead(NugieVulkan::CommandBuffer* commandBuffer, uint32_t frameIndex);
			void transferToWrite(NugieVulkan::CommandBuffer* commandBuffer, uint32_t frameIndex);
			void transferFromReadToWriteRead(NugieVulkan::CommandBuffer* commandBuffer, uint32_t frameIndex);
			void transferFromWriteReadToRead(NugieVulkan::CommandBuffer* commandBuffer, uint32_t frameIndex);
			
		private:
			NugieVulkan::Device* device;
			std::vector<std::unique_ptr<NugieVulkan::Buffer>> buffers;

			void createBuffers(std::shared_ptr<std::vector<IndirectSamplerData>> datas);
	};
} // namespace NugieApp
