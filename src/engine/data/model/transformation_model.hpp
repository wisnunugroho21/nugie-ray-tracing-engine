#pragma once

#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/command/command_buffer.hpp"
#include "../../ray_ubo.hpp"
#include "../../utils/transform/transform.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace NugieApp {
	class TransformationModel {
		public:
			TransformationModel(NugieVulkan::Device* device, NugieVulkan::CommandBuffer *commandBuffer, std::vector<Transformation> transformations);
			TransformationModel(NugieVulkan::Device* device, NugieVulkan::CommandBuffer *commandBuffer, std::vector<TransformComponent> transformationComponents);

			VkDescriptorBufferInfo getTransformationInfo() { return this->transformationBuffer->descriptorInfo();  }
			
		private:
			NugieVulkan::Device* device;
			std::shared_ptr<NugieVulkan::Buffer> transformationBuffer;

			std::vector<Transformation> convertToMatrix(std::vector<TransformComponent> transformations);
			void createBuffers(NugieVulkan::CommandBuffer *commandBuffer, std::vector<Transformation> transformations);
	};
} // namespace NugieApp
