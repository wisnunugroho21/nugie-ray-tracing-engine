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

namespace nugiEngine {
	class EngineIndirectShadeStorageBuffer {
		public:
			EngineIndirectShadeStorageBuffer(EngineDevice &device, uint32_t dataCount);

			EngineIndirectShadeStorageBuffer(const EngineIndirectShadeStorageBuffer&) = delete;
			EngineIndirectShadeStorageBuffer& operator = (const EngineIndirectShadeStorageBuffer&) = delete;

			std::vector<VkDescriptorBufferInfo> getBuffersInfo();

			void transferToRead(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex);
			void transferToWrite(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex);
			
		private:
			EngineDevice &engineDevice;
			std::shared_ptr<std::vector<EngineBuffer>> buffers;

			void createBuffers(std::shared_ptr<std::vector<IndirectShadeRecord>> datas);
	};
} // namespace nugiEngine
