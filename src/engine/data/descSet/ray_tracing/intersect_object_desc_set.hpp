#pragma once

#include "../../../../vulkan/device/device.hpp"
#include "../../../../vulkan/buffer/buffer.hpp"
#include "../../../../vulkan/descriptor/descriptor.hpp"

#include <memory>

namespace nugiEngine {
	class EngineIntersectObjectDescSet {
		public:
			EngineIntersectObjectDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool,
				std::vector<VkDescriptorBufferInfo> buffersInfo[2], VkDescriptorBufferInfo modelsInfo[7]);

			VkDescriptorSet getDescriptorSets(int frameIndex) { return this->descriptorSets[frameIndex]; }
			std::shared_ptr<EngineDescriptorSetLayout> getDescSetLayout() const { return this->descSetLayout; }

		private:
      std::shared_ptr<EngineDescriptorSetLayout> descSetLayout;
			std::vector<VkDescriptorSet> descriptorSets;

			void createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool,
				std::vector<VkDescriptorBufferInfo> buffersInfo[2], VkDescriptorBufferInfo modelsInfo[7]);
	};
	
}