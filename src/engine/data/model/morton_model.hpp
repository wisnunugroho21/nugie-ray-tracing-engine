#pragma once

#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/command/command_buffer.hpp"
#include "../../ray_ubo.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
	class EngineMortonModel {
		public:
			EngineMortonModel(EngineDevice &device, std::shared_ptr<std::vector<Pixel>> mortonPixels);

			EngineMortonModel(const EngineMortonModel&) = delete;
			EngineMortonModel& operator = (const EngineMortonModel&) = delete;

			VkDescriptorBufferInfo getMortonPixelsInfo() { return this->mortonPixelBuffer->descriptorInfo();  }
			
		private:
			EngineDevice &engineDevice;
			std::shared_ptr<EngineBuffer> mortonPixelBuffer;

			void createBuffers(std::shared_ptr<std::vector<Pixel>> mortonPixels);
	};
} // namespace nugiEngine
