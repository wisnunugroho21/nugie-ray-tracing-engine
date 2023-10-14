#include "sampling_desc_set.hpp"
#include "../../../vulkan/descriptor/descriptor_writer.hpp"

namespace NugieApp {
  SamplingDescSet::SamplingDescSet(NugieVulkan::Device* device, NugieVulkan::DescriptorPool* descriptorPool, std::vector<VkDescriptorImageInfo> samplingResourcesInfo[2]) {
		this->createDescriptor(device, descriptorPool, samplingResourcesInfo);
  }

  void SamplingDescSet::createDescriptor(NugieVulkan::Device* device, NugieVulkan::DescriptorPool* descriptorPool, std::vector<VkDescriptorImageInfo> samplingResourcesInfo[2]) {
    this->descSetLayout = 
			NugieVulkan::DescriptorSetLayout::Builder(device)
				.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
				.build();
		
	this->descriptorSets.clear();
	for (int i = 0; i < NugieVulkan::Device::MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorSet descSet;

			NugieVulkan::DescriptorWriter(device, this->descSetLayout.get(), descriptorPool)
				.writeImage(0, samplingResourcesInfo[0][i])
				.writeImage(1, samplingResourcesInfo[1][i])
				.build(&descSet);

			this->descriptorSets.emplace_back(descSet);
		}
  }
}