#include "light_shade_desc_set.hpp"
#include "../../../../vulkan/descriptor/descriptor_writer.hpp"

namespace NugieApp {
  LightShadeDescSet::LightShadeDescSet(NugieVulkan::Device* device, NugieVulkan::DescriptorPool* descriptorPool,
		std::vector<VkDescriptorBufferInfo> buffersInfo[3], VkDescriptorBufferInfo modelsInfo[2]) 
	{
		this->createDescriptor(device, descriptorPool, buffersInfo, modelsInfo);
  }

  void LightShadeDescSet::createDescriptor(NugieVulkan::Device* device, NugieVulkan::DescriptorPool* descriptorPool, 
		std::vector<VkDescriptorBufferInfo> buffersInfo[3], VkDescriptorBufferInfo modelsInfo[2]) 
	{
    this->descSetLayout = 
			NugieVulkan::DescriptorSetLayout::Builder(device)
				.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.build();
		
	this->descriptorSets.clear();
		for (int i = 0; i < NugieVulkan::Device::MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorSet descSet;

			NugieVulkan::DescriptorWriter(device, this->descSetLayout.get(), descriptorPool)
				.writeBuffer(0, buffersInfo[0][i])
				.writeBuffer(1, buffersInfo[1][i])
				.writeBuffer(2, buffersInfo[2][i])
				.writeBuffer(3, modelsInfo[0])
				.writeBuffer(4, modelsInfo[1])
				.build(&descSet);

			this->descriptorSets.emplace_back(descSet);
		}
  }
}