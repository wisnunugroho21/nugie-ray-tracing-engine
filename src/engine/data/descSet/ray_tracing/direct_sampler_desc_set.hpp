#pragma once

#include "../../../../vulkan/device/device.hpp"
#include "../../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/descriptor/descriptor_pool.hpp"
#include "../../../vulkan/descriptor/descriptor_set_layout.hpp"

#include <memory>

namespace NugieApp {
	class DirectSamplerDescSet {
		public:
			DirectSamplerDescSet(NugieVulkan::Device* device, NugieVulkan::DescriptorPool* descriptorPool, std::vector<VkDescriptorBufferInfo> uniformBufferInfo, 
				std::vector<VkDescriptorBufferInfo> buffersInfo[5], VkDescriptorBufferInfo modelsInfo[2]);

			VkDescriptorSet getDescriptorSets(int frameIndex) { return this->descriptorSets[frameIndex]; }
			NugieVulkan::DescriptorSetLayout* getDescSetLayout() const { return this->descSetLayout.get(); }

		private:
      std::unique_ptr<NugieVulkan::DescriptorSetLayout> descSetLayout;
			std::vector<VkDescriptorSet> descriptorSets;

			void createDescriptor(NugieVulkan::Device* device, NugieVulkan::DescriptorPool* descriptorPool, 
				std::vector<VkDescriptorBufferInfo> uniformBufferInfo, std::vector<VkDescriptorBufferInfo> buffersInfo[5],
				VkDescriptorBufferInfo modelsInfo[2]);
	};
	
}