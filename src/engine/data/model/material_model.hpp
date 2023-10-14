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

namespace NugieApp {
	class MaterialModel {
		public:
			MaterialModel(NugieVulkan::Device* device, std::vector<Material> materials);

			VkDescriptorBufferInfo getMaterialInfo() { return this->materialBuffer->descriptorInfo();  }
			
		private:
			NugieVulkan::Device* device;
			std::unique_ptr<NugieVulkan::Buffer> materialBuffer;

			void createBuffers(std::vector<Material> materials);
	};
} // namespace NugieApp
