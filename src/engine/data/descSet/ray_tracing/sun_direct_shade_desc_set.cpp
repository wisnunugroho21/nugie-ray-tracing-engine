#include "sun_direct_shade_desc_set.hpp"

namespace nugiEngine {
  EngineSunDirectShadeDescSet::EngineSunDirectShadeDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool,
		std::vector<VkDescriptorBufferInfo> uniformBufferInfo, std::vector<VkDescriptorBufferInfo> buffersInfo[4], 
		VkDescriptorBufferInfo modelsInfo[1]) 
	{
		this->createDescriptor(device, descriptorPool, uniformBufferInfo, buffersInfo, modelsInfo);
  }

  void EngineSunDirectShadeDescSet::createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool,
		std::vector<VkDescriptorBufferInfo> uniformBufferInfo, std::vector<VkDescriptorBufferInfo> buffersInfo[4], 
		VkDescriptorBufferInfo modelsInfo[1]) 
	{
    this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(device)
				.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.build();
		
	this->descriptorSets.clear();
		for (int i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorSet descSet;

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeBuffer(0, &uniformBufferInfo[i])
				.writeBuffer(1, &buffersInfo[0][i])
				.writeBuffer(2, &buffersInfo[1][i])
				.writeBuffer(3, &buffersInfo[2][i])
				.writeBuffer(4, &buffersInfo[3][i])
				.writeBuffer(5, &modelsInfo[0])
				.build(&descSet);

			this->descriptorSets.emplace_back(descSet);
		}
  }
}