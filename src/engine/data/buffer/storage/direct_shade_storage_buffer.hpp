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
	class EngineDirectShadeStorageBuffer {
		public:
			EngineDirectShadeStorageBuffer(EngineDevice &device, uint32_t dataCount);

			EngineDirectShadeStorageBuffer(const EngineDirectShadeStorageBuffer&) = delete;
			EngineDirectShadeStorageBuffer& operator = (const EngineDirectShadeStorageBuffer&) = delete;

			std::vector<VkDescriptorBufferInfo> getBuffersInfo();
			
		private:
			EngineDevice &engineDevice;
			std::shared_ptr<std::vector<EngineBuffer>> buffers;

			void createBuffers(std::shared_ptr<std::vector<DirectShadeRecord>> datas);
	};
} // namespace nugiEngine
