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
	class RayTraceVertexModel {
		public:
			RayTraceVertexModel(NugieVulkan::Device* device, std::vector<RayTraceVertex> vertices);

			VkDescriptorBufferInfo getVertexnfo() { return this->vertexBuffer->descriptorInfo(); }
			
		private:
			NugieVulkan::Device* device;
			
			std::unique_ptr<NugieVulkan::Buffer> vertexBuffer;
			void createVertexBuffers(std::vector<RayTraceVertex> vertices);
	};
} // namespace NugieApp
