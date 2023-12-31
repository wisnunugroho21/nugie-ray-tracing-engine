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
	class EngineMaterialModel {
		public:
			EngineMaterialModel(EngineDevice &device, std::shared_ptr<std::vector<Material>> materials);

			VkDescriptorBufferInfo getMaterialInfo() { return this->materialBuffer->descriptorInfo();  }
			
		private:
			EngineDevice &engineDevice;
			std::shared_ptr<EngineBuffer> materialBuffer;

			void createBuffers(std::shared_ptr<std::vector<Material>> materials);
	};
} // namespace nugiEngine
