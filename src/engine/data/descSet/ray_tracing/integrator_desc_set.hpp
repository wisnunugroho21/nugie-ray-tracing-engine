#pragma once

#include "../../../../vulkan/device/device.hpp"
#include "../../../../vulkan/buffer/buffer.hpp"
#include "../../../../vulkan/descriptor/descriptor.hpp"

#include <memory>

namespace nugiEngine {
	class EngineIntegratorDescSet {
		public:
			EngineIntegratorDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool,
				std::vector<VkDescriptorImageInfo> indirectImageInfos, std::vector<VkDescriptorBufferInfo> buffersInfo[5]);

			VkDescriptorSet getDescriptorSets(int frameIndex) { return this->descriptorSets[frameIndex]; }
			std::shared_ptr<EngineDescriptorSetLayout> getDescSetLayout() const { return this->descSetLayout; }

		private:
      std::shared_ptr<EngineDescriptorSetLayout> descSetLayout;
			std::vector<VkDescriptorSet> descriptorSets;

			void createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool,
				std::vector<VkDescriptorImageInfo> indirectImageInfos, std::vector<VkDescriptorBufferInfo> buffersInfo[6]);
	};
	
}